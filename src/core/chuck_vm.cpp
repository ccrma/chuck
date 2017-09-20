/*----------------------------------------------------------------------------
  ChucK Concurrent, On-the-fly Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_vm.cpp
// desc: chuck virtual machine
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_lang.h"
#include "chuck_type.h"
#include "chuck_dl.h"
#include "chuck_io.h"
#include "chuck_errmsg.h"
#include "ugen_xxx.h"

#include <algorithm>
using namespace std;

#if defined(__PLATFORM_WIN32__)
  #include <windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif

// uncomment to compile VM debug messages
#define CK_VM_DEBUG_ENABLE (0)

#if CK_VM_DEBUG_ENABLE
#define CK_VM_DEBUG(x) x
#include <typeinfo>
#else
#define CK_VM_DEBUG(x)
#endif // CK_VM_DEBUG_ENABLE




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Frame
// desc: func frame
//-----------------------------------------------------------------------------
struct Chuck_VM_Frame
{
public:
    t_CKUINT size;

public:
    Chuck_VM_Frame() { size = 0; }
    ~Chuck_VM_Frame() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Func
// desc: vm function
//-----------------------------------------------------------------------------
struct Chuck_VM_Func
{
public:
    Chuck_VM_Code * code;
    Chuck_VM_Frame * frame;
    t_CKUINT index;

public:
    Chuck_VM_Func() { code = NULL; frame = NULL; index = 0; }
    ~Chuck_VM_Func() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_FTable
// desc: function table
//-----------------------------------------------------------------------------
struct Chuck_VM_FTable
{
public:
    Chuck_VM_FTable();
    ~Chuck_VM_FTable();

public:
    Chuck_VM_Func * get_func( t_CKUINT index );
    t_CKUINT append( Chuck_VM_Func * f );
    t_CKBOOL remove( t_CKUINT index );

public:
    vector<Chuck_VM_Func *> func_table;
};




//-----------------------------------------------------------------------------
// name: Chuck_VM()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM::Chuck_VM()
{
    // REFACTOR-2017: refs
    m_carrier = NULL;
    
    // data
    m_shreds = NULL;
    m_num_shreds = 0;
    m_shreduler = NULL;
    m_num_dumped_shreds = 0;
    m_msg_buffer = NULL;
    m_reply_buffer = NULL;
    m_event_buffer = NULL;
    m_shred_id = 0;
    m_halt = TRUE;

    m_dac = NULL;
    m_adc = NULL;
    m_bunghole = NULL;
    m_srate = 0;
    m_num_dac_channels = 0;
    m_num_adc_channels = 0;
    m_init = FALSE;
    m_input_ref = NULL;
    m_output_ref = NULL;
    
    // REFACTOR-2017: TODO might want to dynamically grow queue?
    m_set_external_int_queue.init( 1024 );
    m_get_external_int_queue.init( 1024 );
    m_set_external_float_queue.init( 1024 );
    m_get_external_float_queue.init( 1024 );
    m_signal_external_event_queue.init( 1024 );
    m_spork_external_shred_queue.init( 8192 );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM::~Chuck_VM()
{
    // check if init
    if( m_init )
    {
        // cleanup
        shutdown();
    }
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::initialize( t_CKUINT srate, t_CKUINT dac_chan,
                               t_CKUINT adc_chan, t_CKUINT adaptive, t_CKBOOL halt )
{
    if( m_init )
    {
        m_last_error = "VM already initialized!";
        return FALSE;
    }

    // log
    EM_log( CK_LOG_SYSTEM, "initializing virtual machine..." );
    EM_pushlog(); // push stack

    m_halt = halt;
    m_num_adc_channels = adc_chan;
    m_num_dac_channels = dac_chan;
    m_srate = srate;

    // lockdown
    Chuck_VM_Object::lock_all();

    // log
    EM_log( CK_LOG_SYSTEM, "allocating shreduler..." );
    // allocate shreduler
    m_shreduler = new Chuck_VM_Shreduler;
    m_shreduler->vm_ref = this;
    m_shreduler->set_adaptive( adaptive > 0 ? adaptive : 0 );

    // log
    EM_log( CK_LOG_SYSTEM, "allocating messaging buffers..." );
    // allocate msg buffer
    m_msg_buffer = new CBufferSimple;
    m_msg_buffer->initialize( 1024, sizeof(Chuck_Msg *) );
    //m_msg_buffer->join(); // this should return 0
    m_reply_buffer = new CBufferSimple;
    m_reply_buffer->initialize( 1024, sizeof(Chuck_Msg *) );
    //m_reply_buffer->join(); // this should return 0 too
    m_event_buffer = new CBufferSimple;
    m_event_buffer->initialize( 1024, sizeof(Chuck_Event *) );
    //m_event_buffer->join(); // this should also return 0

    // pop log
    EM_poplog();

    // TODO: clean up all the dynamic objects here on failure
    //       and in the shutdown function!

    return m_init = TRUE;
}




//-----------------------------------------------------------------------------
// name: initialize_synthesis()
// desc: requires type system
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::initialize_synthesis( )
{
    if( !m_init )
    {
        m_last_error = "VM initialize_synthesis() called on raw VM";
        return FALSE;
    }

    if( !env()->t_dac || !env()->t_adc )
    {
        m_last_error = "VM initialize_synthesis() called before type system initialized";
        return FALSE;
    }

    if( m_dac != NULL )
    {
        m_last_error = "VM synthesis already initialized";
        return FALSE;
    }

    // log
    EM_log( CK_LOG_SYSTEM, "initializing synthesis engine..." );
    // push indent
    EM_pushlog();

    // log
    EM_log( CK_LOG_SEVERE, "initializing 'dac'..." );
    // allocate dac and adc (REFACTOR-2017: g_t_dac changed to env()->t_dac)
    env()->t_dac->ugen_info->num_outs =
        env()->t_dac->ugen_info->num_ins = m_num_dac_channels;
    m_dac = (Chuck_UGen *)instantiate_and_initialize_object( env()->t_dac, this );
    // Chuck_DL_Api::Api::instance() added 1.3.0.0
    object_ctor( m_dac, NULL, NULL, Chuck_DL_Api::Api::instance() ); // TODO: this can't be the place to do this
    stereo_ctor( m_dac, NULL, NULL, Chuck_DL_Api::Api::instance() ); // TODO: is the NULL shred a problem?
    multi_ctor( m_dac, NULL, NULL, Chuck_DL_Api::Api::instance() );  // TODO: remove and let type system do this
    m_dac->add_ref();
    // lock it
    m_dac->lock();

    // log
    EM_log( CK_LOG_SEVERE, "initializing 'adc'..." );
    // (REFACTOR-2017: g_t_adc changed to env()->t_adc)
    env()->t_adc->ugen_info->num_ins =
        env()->t_adc->ugen_info->num_outs = m_num_adc_channels;
    m_adc = (Chuck_UGen *)instantiate_and_initialize_object( env()->t_adc, this );
    // Chuck_DL_Api::Api::instance() added 1.3.0.0
    object_ctor( m_adc, NULL, NULL, Chuck_DL_Api::Api::instance() ); // TODO: this can't be the place to do this
    stereo_ctor( m_adc, NULL, NULL, Chuck_DL_Api::Api::instance() );
    multi_ctor( m_adc, NULL, NULL, Chuck_DL_Api::Api::instance() ); // TODO: remove and let type system do this
    m_adc->add_ref();
    // lock it
    m_adc->lock();

    // log
    EM_log( CK_LOG_SEVERE, "initializing 'blackhole'..." );
    m_bunghole = new Chuck_UGen;
    m_bunghole->add_ref();
    m_bunghole->lock();
    initialize_object( m_bunghole, env()->t_ugen );
    m_bunghole->tick = NULL;
    m_bunghole->alloc_v( m_shreduler->m_max_block_size );
    m_shreduler->m_dac = m_dac;
    m_shreduler->m_adc = m_adc;
    m_shreduler->m_bunghole = m_bunghole;
    m_shreduler->m_num_dac_channels = m_num_dac_channels;
    m_shreduler->m_num_adc_channels = m_num_adc_channels;

    // pop indent
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::shutdown()
{
    // make sure we are in the initialized state
    if( !m_init ) return FALSE;

    // log
    EM_log( CK_LOG_SYSTEM, "shutting down virtual machine..." );
    // push indent
    EM_pushlog();
    
    // unlockdown
    // REFACTOR-2017: TODO: don't unlock all objects for all VMs? see relockdown below
    Chuck_VM_Object::unlock_all();
    
    // REFACTOR-2017: clean up after my external variables
    cleanup_external_variables();

    // log
    EM_log( CK_LOG_SYSTEM, "freeing shreduler..." );
    // free the shreduler
    SAFE_DELETE( m_shreduler );

    // log
    EM_log( CK_LOG_SYSTEM, "freeing msg/reply/event buffers..." );
    // free the msg buffer
    SAFE_DELETE( m_msg_buffer );
    // free the reply buffer
    SAFE_DELETE( m_reply_buffer );
    // free the event buffer
    SAFE_DELETE( m_event_buffer );

    // log
    EM_log( CK_LOG_SEVERE, "clearing shreds..." );
    // terminate shreds
    Chuck_VM_Shred * curr = m_shreds, * prev = NULL;
    while( curr )
    {
        prev = curr;
        curr = curr->next;
        // release shred
        prev->release();
    }
    m_shreds = NULL;
    m_num_shreds = 0;

    // log
    EM_pushlog();
    EM_log( CK_LOG_SEVERE, "freeing dumped shreds..." );
    // do it
    this->release_dump();
    EM_poplog();

    // log
    EM_log( CK_LOG_SYSTEM, "freeing special ugens..." );
    // go
    SAFE_RELEASE( m_dac );
    SAFE_RELEASE( m_adc );
    SAFE_RELEASE( m_bunghole );
    
    // set state
    m_init = FALSE;
    
    // relockdown (added 1.3.6.0)
    // REFACTOR-2017: TODO -- remove once made per-VM
    Chuck_VM_Object::lock_all();

    // log
    EM_log( CK_LOG_SYSTEM, "virtual machine shutdown complete." );

    // pop indent
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: run start (can be called before or after init)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::start()
{
    // already running?
    if( m_is_running) return FALSE;
    // set state
    m_is_running = TRUE;
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: running()
// desc: get run state
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::running()
{
    return m_is_running;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: run stop
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::stop()
{
    // running?
    if( !m_is_running ) return FALSE;
    // set state
    m_is_running = FALSE;
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: compute()
// desc: compute all shreds at one instance in ChucK time
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::compute()
{
    Chuck_VM_Shred *& shred = m_shreduler->m_current_shred;
    Chuck_Msg * msg = NULL;
    Chuck_Event * event = NULL;
    t_CKBOOL iterate = TRUE;
    
    // REFACTOR-2017: spork queued shreds
    // spork newly compiled files before trying set messages
    handle_external_spork_messages();
    
    // REFACTOR-2017: set externals
    handle_external_set_messages();
    // REFACTOR-2017: get externals
    handle_external_get_messages();

    // iteration until no more shreds/events/messages
    while( iterate )
    {
        // get the shreds queued for 'now'
        while(( shred = m_shreduler->get() ))
        {
            // set the current time of the shred
            shred->now = shred->wake_time;

            // track shred activation
            CK_TRACK( Chuck_Stats::instance()->activate_shred( shred ) );

            // run the shred
            if( !shred->run( this ) )
            {
                // track shred deactivation
                CK_TRACK( Chuck_Stats::instance()->deactivate_shred( shred ) );

                this->free( shred, TRUE );
                shred = NULL;
                if( !m_num_shreds && m_halt ) return FALSE;
            }

            // track shred deactivation
            CK_TRACK( if( shred ) Chuck_Stats::instance()->deactivate_shred( shred ) );

            // zero out
            shred = NULL;
        }

        // set to false for now
        iterate = FALSE;

        // broadcast queued events
        while( m_event_buffer->get( &event, 1 ) )
        { event->broadcast(); iterate = TRUE; }
        
        // loop over thread-specific queued event buffers (added 1.3.0.0)
        for( list<CBufferSimple *>::const_iterator i = m_event_buffers.begin();
             i != m_event_buffers.end(); i++ )
        {
            while( (*i)->get( &event, 1 ) )
            { event->broadcast(); iterate = TRUE; }
        }

        // process messages
        while( m_msg_buffer->get( &msg, 1 ) )
        { process_msg( msg ); iterate = TRUE; }

        // clear dumped shreds
        if( m_num_dumped_shreds > 0 )
            release_dump();
    }

    // continue executing if have shreds left or if don't-halt
    // or if have shreds to add
    return ( m_num_shreds || !m_halt || m_spork_external_shred_queue.more() );
}




//-----------------------------------------------------------------------------
// name: run()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::run( t_CKINT N, const SAMPLE * input, SAMPLE * output )
{
    // copy
    m_input_ref = input; m_output_ref = output;
    // frame count
    t_CKINT frame = 0;

    // for now, check for external variables once per sample (below)
    // TODO: once per buffer instead? (place here then)

    // loop it
    while( N )
    {
        // compute shreds
        if( !compute() ) goto vm_stop;

        // advance the shreduler
        if( !m_shreduler->m_adaptive )
        {
            m_shreduler->advance( frame++ );
            if( N > 0 ) N--;
        }
        else m_shreduler->advance_v( N, frame );
    }
    
    // clear
    m_input_ref = NULL; m_output_ref = NULL;

    return FALSE;

// vm stop here
vm_stop:
    // stop, 1.3.5.3
    this->stop();

    // log
    EM_log( CK_LOG_SYSTEM, "virtual machine stopped..." );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: gc
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM::gc( t_CKUINT amount )
{
}




//-----------------------------------------------------------------------------
// name: gc
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM::gc( )
{
}




//-----------------------------------------------------------------------------
// name: queue_msg()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::queue_msg( Chuck_Msg * msg, int count )
{
    assert( count == 1 );
    m_msg_buffer->put( &msg, count );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: queue_event()
// desc: since 1.3.0.0 a buffer is passed in associated with each thread
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::queue_event( Chuck_Event * event, int count, 
                                CBufferSimple * buffer )
{
    // sanity
    assert( count == 1 );
    // if null
    if( buffer == NULL )
    {
        // use non thread-safe buffer
        buffer = m_event_buffer;
    }
    // put into the buffer
    buffer->put( &event, count );

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: create_event_buffer()
// desc: added 1.3.0.0 to fix uber-crash
//-----------------------------------------------------------------------------
CBufferSimple * Chuck_VM::create_event_buffer()
{
    CBufferSimple * buffer = new CBufferSimple;
    buffer->initialize( 1024, sizeof(Chuck_Event *) );
    m_event_buffers.push_back(buffer);
    
    return buffer;
}




//-----------------------------------------------------------------------------
// name: destroy_event_buffer()
// desc: added 1.3.0.0 to fix uber-crash
//-----------------------------------------------------------------------------
void Chuck_VM::destroy_event_buffer( CBufferSimple * buffer )
{
    m_event_buffers.remove( buffer );
    delete buffer;
}




//-----------------------------------------------------------------------------
// name: get_reply()
// desc: ...
// TODO: make thread safe for multiple consumers
//-----------------------------------------------------------------------------
Chuck_Msg * Chuck_VM::get_reply()
{
    Chuck_Msg * msg = NULL;
    m_reply_buffer->get( &msg, 1 );
    return msg;
}




//-----------------------------------------------------------------------------
// name: process_msg()
// desc: ...
// TODO: make thread safe for multiple consumers
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::process_msg( Chuck_Msg * msg )
{
    t_CKUINT retval = 0xfffffff0;

    if( msg->type == MSG_REPLACE )
    {
        Chuck_VM_Shred * out = m_shreduler->lookup( msg->param );
        if( !out )
        {
            EM_error3( "[chuck](VM): error replacing shred: no shred with id %i...",
                       msg->param );
            retval = 0;
            goto done;
        }

        Chuck_VM_Shred * shred = msg->shred;
        if( !shred )
        {
            shred = new Chuck_VM_Shred;
            shred->initialize( msg->code );
            shred->name = msg->code->name;
            shred->base_ref = shred->mem;
            shred->add_ref();
        }
        // set the current time
        shred->start = m_shreduler->now_system;
        // set the id
        shred->xid = msg->param;
        // set the now
        shred->now = shred->wake_time = m_shreduler->now_system;
        // set the vm
        shred->vm_ref = this;
        // set args
        if( msg->args ) shred->args = *(msg->args);
        // add it to the parent
        if( shred->parent )
            shred->parent->children[shred->xid] = shred;

        // replace
        if( m_shreduler->remove( out ) && m_shreduler->shredule( shred ) )
        {
            EM_error3( "[chuck](VM): replacing shred %i (%s) with %i (%s)...",
                       out->xid, mini(out->name.c_str()), shred->xid, mini(shred->name.c_str()) );
            this->free( out, TRUE, FALSE );
            retval = shred->xid;
        
            // tracking new shred
            CK_TRACK( Chuck_Stats::instance()->add_shred( shred ) );

            goto done;
        }
        else
        {
            EM_error3( "[chuck](VM): shreduler ERROR replacing shred %i...",
                       out->xid );
            shred->release();
            retval = 0;
            goto done;
        }
    }
    else if( msg->type == MSG_REMOVE )
    {
        if( msg->param == 0xffffffff )
        {
            if( !this->m_num_shreds)
            {
                EM_error3( "[chuck](VM): no shreds to remove..." );
                retval = 0;
                goto done;
            }

            t_CKINT xid = m_shred_id;
            Chuck_VM_Shred * shred = NULL;
            while( xid >= 0 && m_shreduler->remove( shred = m_shreduler->lookup( xid ) ) == 0 )
                xid--;
            if( xid >= 0 )
            {
                EM_error3( "[chuck](VM): removing recent shred: %i (%s)...", 
                           xid, mini(shred->name.c_str()) );
                this->free( shred, TRUE );
                retval = xid;
            }
            else
            {
                EM_error3( "[chuck](VM): no shreds removed..." );
                retval = 0;
                goto done;
            }
        }
        else
        {
            Chuck_VM_Shred * shred = m_shreduler->lookup( msg->param );
            if( !shred )
            {
                EM_error3( "[chuck](VM): cannot remove: no shred with id %i...",
                           msg->param );
                retval = 0;
                goto done;
            }
            if( shred != m_shreduler->m_current_shred && !m_shreduler->remove( shred ) )  // was lookup
            {
                EM_error3( "[chuck](VM): shreduler: cannot remove shred %i...",
                           msg->param );
                retval = 0;
                goto done;
            }
            EM_error3( "[chuck](VM): removing shred: %i (%s)...",
                       msg->param, mini(shred->name.c_str()) );
            this->free( shred, TRUE );
            retval = msg->param;
        }
    }
    else if( msg->type == MSG_REMOVEALL )
    {
        t_CKUINT xid = m_shred_id;
        EM_error3( "[chuck](VM): removing all (%i) shreds...", m_num_shreds );
        Chuck_VM_Shred * shred = NULL;
        
        while( m_num_shreds && xid > 0 )
        {
            if( m_shreduler->remove( shred = m_shreduler->lookup( xid ) ) )
                this->free( shred, TRUE );
            xid--;
        }
        
        m_shred_id = 0;
        m_num_shreds = 0;
    }
    else if( msg->type == MSG_CLEARVM ) // added 1.3.2.0
    {
        // first removeall
        t_CKUINT xid = m_shred_id;
        EM_error3( "[chuck](VM): removing all shreds and resetting type system" );
        Chuck_VM_Shred * shred = NULL;
        
        while( m_num_shreds && xid > 0 )
        {
            if( m_shreduler->remove( shred = m_shreduler->lookup( xid ) ) )
                this->free( shred, TRUE );
            xid--;
        }
        
        // clear user type system
        if( env() )
        {
            env()->clear_user_namespace();
        }
        
        m_shred_id = 0;
        m_num_shreds = 0;
    }
    else if( msg->type == MSG_ADD )
    {
        t_CKUINT xid = 0;
        Chuck_VM_Shred * shred = NULL;
        if( msg->shred ) shred = this->spork( msg->shred );
        else shred = this->spork( msg->code, NULL, TRUE );
        xid = shred->xid;
        if( msg->args ) shred->args = *(msg->args);

        const char * s = ( msg->shred ? msg->shred->name.c_str() : msg->code->name.c_str() );
        EM_error3( "[chuck](VM): sporking incoming shred: %i (%s)...", xid, mini(s) );
        retval = xid;
        goto done;
    }
    else if( msg->type == MSG_KILL )
    {
        EM_error3( "[chuck](VM): KILL received...." );
        // close file handles and clean up
        // REFACTOR-2017: TODO all_detach function
        // all_detach();
        // TODO: free more memory?

        // log
        EM_log( CK_LOG_INFO, "(VM): exiting..." );
        // come again
        exit( 1 );
    }
    else if( msg->type == MSG_STATUS )
    {
        // fill in structure
        if( msg->user && msg->reply )
        {
            // cast
            Chuck_VM_Status * status = (Chuck_VM_Status *)msg->user;
            // get it
            m_shreduler->status( status );
        }
        else
        {
            m_shreduler->status();
        }
    }
    else if( msg->type == MSG_TIME )
    {
        float srate = m_srate; // 1.3.5.3; was: (float)Digitalio::sampling_rate();
        CK_FPRINTF_STDERR( "[chuck](VM): the values of now:\n" );
        CK_FPRINTF_STDERR( "  now = %.6f (samp)\n", m_shreduler->now_system );
        CK_FPRINTF_STDERR( "      = %.6f (second)\n", m_shreduler->now_system / srate );
        CK_FPRINTF_STDERR( "      = %.6f (minute)\n", m_shreduler->now_system / srate / 60.0f );
        CK_FPRINTF_STDERR( "      = %.6f (hour)\n", m_shreduler->now_system / srate / 60.0f / 60.0f );
        CK_FPRINTF_STDERR( "      = %.6f (day)\n", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f );
        CK_FPRINTF_STDERR( "      = %.6f (week)\n", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f / 7.0f );
    }
    else if( msg->type == MSG_RESET_ID )
    {
        t_CKUINT n = m_shreduler->highest();
        m_shred_id = n;
        CK_FPRINTF_STDERR( "[chuck](VM): reseting shred id to %lu...\n", m_shred_id + 1 );
    }

done:

    if( msg->reply )
    {
        msg->replyA = retval;
        m_reply_buffer->put( &msg, 1 );
    }
    else
        SAFE_DELETE(msg);

    return retval;
}




//-----------------------------------------------------------------------------
// name: next_id()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::next_id( )
{
    return ++m_shred_id;
}




//-----------------------------------------------------------------------------
// name: shreduler()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler * Chuck_VM::shreduler( ) const
{
    return m_shreduler;
}




//-----------------------------------------------------------------------------
// name: srate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::srate() const
{
    return m_srate; // 1.3.5.3; was: (t_CKUINT)Digitalio::sampling_rate();
}




//-----------------------------------------------------------------------------
// name: spork()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM::spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent,
                                  t_CKBOOL immediate )
{
    // allocate a new shred
    Chuck_VM_Shred * shred = new Chuck_VM_Shred;
    // set the vm
    shred->vm_ref = this;
    // initialize the shred (default stack size)
    shred->initialize( code );
    // set the name
    shred->name = code->name;
    // set the parent
    shred->parent = parent;
    // set the base ref for global
    if( parent ) shred->base_ref = shred->parent->base_ref;
    else shred->base_ref = shred->mem;
    if( immediate )
    {
        // spork it
        this->spork( shred );
    }
    else
    {
        // spork it later
        m_spork_external_shred_queue.put( shred );
    }

    // track new shred
    CK_TRACK( Chuck_Stats::instance()->add_shred( shred ) );

    return shred;
}




//-----------------------------------------------------------------------------
// name: spork()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM::spork( Chuck_VM_Shred * shred )
{
    // set the current time
    shred->start = m_shreduler->now_system;
    // set the now
    shred->now = shred->wake_time = m_shreduler->now_system;
    // set the id
    shred->xid = next_id();
    // add ref
    shred->add_ref();
    // add it to the parent
    if( shred->parent )
        shred->parent->children[shred->xid] = shred;
    // shredule it
    m_shreduler->shredule( shred );
    // count
    m_num_shreds++;

    return shred;
}




//-----------------------------------------------------------------------------
// name: free()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::free( Chuck_VM_Shred * shred, t_CKBOOL cascade, t_CKBOOL dec )
{
    assert( cascade );
    
    // log
    EM_log( CK_LOG_FINER, "freeing shred (id==%d | ptr==%p)", shred->xid,
            (t_CKUINT)shred );

    // abort on the double free
    // TODO: can a shred be dumped, then resporked? from code?
    if( shred->is_dumped ) return FALSE;

    // mark this done
    shred->is_done = TRUE;

    // free the children
    t_CKINT size = shred->children.size();
    if( size )
    {
        vector<Chuck_VM_Shred *> list; list.resize( size );
        map<t_CKUINT, Chuck_VM_Shred *>::iterator iter; t_CKINT i = 0;
        for( iter = shred->children.begin(); iter != shred->children.end(); iter++ )
            list[i++] = (*iter).second;
        for( i = 0; i < size; i++ )
            this->free( list[i], cascade );
    }

    // make sure it's done
    assert( shred->children.size() == 0 );

    // tell parent
    if( shred->parent )
        shred->parent->children.erase( shred->xid );

    // track remove shred
    CK_TRACK( Chuck_Stats::instance()->remove_shred( shred ) );

    // free!
    m_shreduler->remove( shred );
    // TODO: remove shred from event, with synchronization (still necessary with dump?)
    // if( shred->event ) shred->event->remove( shred );
    // OLD: shred->release();
    this->dump( shred );
    shred = NULL;
    if( dec ) m_num_shreds--;
    if( !m_num_shreds ) m_shred_id = 0;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: abort_current_shred()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::abort_current_shred( )
{
    // for threading
    Chuck_VM_Shred * shred = m_shreduler->m_current_shred;
    
    // if there
    if( shred )
    {
        // log
        EM_log( CK_LOG_SEVERE, "trying to abort current shred (id: %d)", shred->xid );
        // flag it
        shred->is_abort = TRUE;
    }
    else
    {
        // log
        EM_log( CK_LOG_SEVERE, "cannot abort shred: nothing currently running!" );
    }

    return shred != NULL;
}




//-----------------------------------------------------------------------------
// name: dump()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM::dump( Chuck_VM_Shred * shred )
{
    // log
    EM_log( CK_LOG_FINER, "dumping shred (id==%d | ptr==%p)", shred->xid,
            (t_CKUINT)shred );
    // add
    m_shred_dump.push_back( shred );
    // stop
    shred->is_running = FALSE;
    shred->is_done = TRUE;
    shred->is_dumped = TRUE;
    // TODO: cool?
    shred->xid = 0;
    // inc
    m_num_dumped_shreds++;
}




//-----------------------------------------------------------------------------
// name: release_dump()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM::release_dump( )
{
    // log
    EM_log( CK_LOG_FINER, "releasing dumped shreds..." );

    // iterate through dump
    for( t_CKUINT i = 0; i < m_shred_dump.size(); i++ )
        SAFE_RELEASE( m_shred_dump[i] );

    // clear the dump
    m_shred_dump.clear();
    // reset
    m_num_dumped_shreds = 0;
}




//-----------------------------------------------------------------------------
// name: get_external_int()
// desc: get an external int by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::get_external_int( std::string name,
                                     void (* callback)(t_CKINT) ) {
    Chuck_Get_External_Int_Request get_int_message;
    get_int_message.name = name;
    get_int_message.fp = callback;
    
    m_get_external_int_queue.put( get_int_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_external_int()
// desc: set an external int by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::set_external_int( std::string name, t_CKINT val ) {
    Chuck_Set_External_Int_Request set_int_message;
    set_int_message.name = name;
    set_int_message.val = val;
    
    m_set_external_int_queue.put( set_int_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_external_int()
// desc: tell the vm that an external int is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::init_external_int( std::string name )
{
    if( m_external_ints.count( name ) == 0 )
    {
        m_external_ints[name] = new Chuck_External_Int_Container;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_external_int_value()
// desc: get a value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT Chuck_VM::get_external_int_value( std::string name )
{
    // ensure exists
    init_external_int( name );
    
    return m_external_ints[name]->val;
}




//-----------------------------------------------------------------------------
// name: set_external_int_value()
// desc: set a value directly to the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT * Chuck_VM::get_ptr_to_external_int( std::string name )
{
    return &( m_external_ints[name]->val );
}




//-----------------------------------------------------------------------------
// name: get_external_float()
// desc: get an external float by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::get_external_float( std::string name,
                                       void (* callback)(t_CKFLOAT) ) {
    Chuck_Get_External_Float_Request get_float_message;
    get_float_message.name = name;
    get_float_message.fp = callback;
    
    m_get_external_float_queue.put( get_float_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_external_float()
// desc: set an external float by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::set_external_float( std::string name, t_CKFLOAT val ) {
    Chuck_Set_External_Float_Request set_float_message;
    set_float_message.name = name;
    set_float_message.val = val;
    
    m_set_external_float_queue.put( set_float_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_external_float()
// desc: tell the vm that an external float is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::init_external_float( std::string name )
{
    if( m_external_floats.count( name ) == 0 )
    {
        m_external_floats[name] = new Chuck_External_Float_Container;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_external_float_value()
// desc: get a value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_VM::get_external_float_value( std::string name )
{
    // ensure exists
    init_external_float( name );
    
    return m_external_floats[name]->val;
}




//-----------------------------------------------------------------------------
// name: set_external_float_value()
// desc: set a value directly to the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT * Chuck_VM::get_ptr_to_external_float( std::string name )
{
    return &( m_external_floats[name]->val );
}




//-----------------------------------------------------------------------------
// name: signal_external_event()
// desc: signal() an Event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::signal_external_event( std::string name ) {
    Chuck_Signal_External_Event_Request signal_event_message;
    signal_event_message.name = name;
    signal_event_message.is_broadcast = FALSE;
    
    m_signal_external_event_queue.put( signal_event_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: broadcast_external_event()
// desc: broadcast() an Event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::broadcast_external_event( std::string name ) {
    Chuck_Signal_External_Event_Request signal_event_message;
    signal_event_message.name = name;
    signal_event_message.is_broadcast = TRUE;
    
    m_signal_external_event_queue.put( signal_event_message );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_external_event()
// desc: tell the vm that an external float is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::init_external_event( std::string name, Chuck_Type * type ) {

    // if it hasn't been initted yet
    if( m_external_events.count( name ) == 0 ) {
        // create a new storage container
        m_external_events[name] = new Chuck_External_Event_Container;
        // create the chuck object
        m_external_events[name]->val =
            (Chuck_Event *) instantiate_and_initialize_object( type, this );
        // add a reference to it so it won't be deleted until we're done
        // cleaning up the VM
        m_external_events[name]->val->add_ref();
        // store its type in the container, too (is it a user-defined class?)
        m_external_events[name]->type = type;
    }
    // already exists. check if there's a type mismatch.
    else if( type->name != m_external_events[name]->type->name )
    {
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_external_event_pointer()
// desc: get a pointer directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Event * Chuck_VM::get_external_event( std::string name )
{
    return m_external_events[name]->val;
}




//-----------------------------------------------------------------------------
// name: set_external_event_pointer()
// desc: set a pointer directly on the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Event * * Chuck_VM::get_ptr_to_external_event( std::string name )
{
    return &( m_external_events[name]->val );
}




//-----------------------------------------------------------------------------
// name: cleanup_external_variables()
// desc: set a pointer directly on the vm (internal)
//-----------------------------------------------------------------------------
void Chuck_VM::cleanup_external_variables()
{
    // ints: delete containers and clear map
    for( std::map< std::string, Chuck_External_Int_Container * >::iterator it=
         m_external_ints.begin(); it!=m_external_ints.end(); it++ )
    {
        delete (it->second);
    }
    m_external_ints.clear();
    
    // floats: delete containers and clear map
    for( std::map< std::string, Chuck_External_Float_Container * >::iterator it=
         m_external_floats.begin(); it!=m_external_floats.end(); it++ )
    {
        delete (it->second);
    }
    m_external_floats.clear();
    
    // events: release events, delete containers, and clear map
    for( std::map< std::string, Chuck_External_Event_Container * >::iterator it=
         m_external_events.begin(); it!=m_external_events.end(); it++ )
    {
        SAFE_RELEASE( it->second->val );
        delete (it->second);
    }
    m_external_events.clear();
}




//-----------------------------------------------------------------------------
// name: handle_external_set_messages()
// desc: update values with external set messages
//-----------------------------------------------------------------------------
void Chuck_VM::handle_external_set_messages() {
    // TODO: only do one per call, to avoid taking up too much time?
    //       if so then "if" not "while"
    while( m_set_external_int_queue.more() ) {
        Chuck_Set_External_Int_Request set_int_message;
        if( m_set_external_int_queue.get( & set_int_message ) )
        {
            // ensure the container exists
            init_external_int( set_int_message.name );
            m_external_ints[set_int_message.name]->val = set_int_message.val;
        }
        else
        {
            // get failed
            break;
        }
        
    }
    
    while( m_set_external_float_queue.more() ) {
        Chuck_Set_External_Float_Request set_float_message;
        if( m_set_external_float_queue.get( & set_float_message ) )
        {
            // ensure the container exists
            init_external_float( set_float_message.name );
            m_external_floats[set_float_message.name]->val = set_float_message.val;
        }
        else
        {
            // get failed
            break;
        }
        
    }
    
    while( m_signal_external_event_queue.more() ) {
        Chuck_Signal_External_Event_Request signal_event_message;
        if( m_signal_external_event_queue.get( & signal_event_message ) )
        {
            // ensure it exists
            if( m_external_events.count( signal_event_message.name ) > 0 ) {
                Chuck_Event * event = get_external_event( signal_event_message.name );
                if( signal_event_message.is_broadcast )
                {
                    event->broadcast();
                }
                else
                {
                    event->signal();
                }
            }
        }
        else
        {
            // get failed
            break;
        }
    }
}




//-----------------------------------------------------------------------------
// name: handle_external_set_messages()
// desc: update values with external set messages
//-----------------------------------------------------------------------------
void Chuck_VM::handle_external_get_messages() {
    while( m_get_external_int_queue.more() ) {
        Chuck_Get_External_Int_Request get_int_message;
        if( m_get_external_int_queue.get( & get_int_message ) &&
            get_int_message.fp != NULL )
        {
            // ensure the value exists
            init_external_int( get_int_message.name );
            // call the callback with the value
            get_int_message.fp( m_external_ints[get_int_message.name]->val );
        }
        else
        {
            // get failed.... sadness.... this might be bad
            break;
        }
        
    }
    
    while( m_get_external_float_queue.more() ) {
        Chuck_Get_External_Float_Request get_float_message;
        if( m_get_external_float_queue.get( & get_float_message ) &&
            get_float_message.fp != NULL )
        {
            // ensure value exists
            init_external_float( get_float_message.name );
            // call callback with float
            get_float_message.fp( m_external_floats[get_float_message.name]->val );
        }
        else
        {
            // get failed.... sadness.... this might be bad
            break;
        }
        
    }
}




//-----------------------------------------------------------------------------
// name: handle_external_set_messages()
// desc: update values with external set messages
//-----------------------------------------------------------------------------
void Chuck_VM::handle_external_spork_messages() {
    while( m_spork_external_shred_queue.more() ) {
        Chuck_VM_Shred * shred;
        if( m_spork_external_shred_queue.get( & shred ) )
        {
            this->spork( shred );
        }
        else
        {
            // get failed...
            break;
        }
        
    }
    
}




//-----------------------------------------------------------------------------
// name: set_main_thread_hook()
// desc: ...
//-----------------------------------------------------------------------------
//t_CKBOOL Chuck_VM::set_main_thread_hook( f_mainthreadhook hook,
//                                         f_mainthreadquit quit,
//                                         void * bindle )
//{
//    if( m_main_thread_hook == NULL && m_main_thread_quit == NULL )
//    {
//        m_main_thread_bindle = bindle;
//        m_main_thread_hook = hook;
//        m_main_thread_quit = quit;
//        
//        return TRUE;
//    }
//    else
//    {
//        EM_log(CK_LOG_SEVERE, "[chuck](VM): attempt to register more than one main_thread_hook");
//        return FALSE;
//    }
//}




//-----------------------------------------------------------------------------
// name: set_main_thread_hook()
// desc: ...
//-----------------------------------------------------------------------------
//t_CKBOOL Chuck_VM::clear_main_thread_hook()
//{
//    m_main_thread_bindle = NULL;
//    m_main_thread_hook = NULL;
//    m_main_thread_quit = NULL;
//    
//    return TRUE;
//}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Stack()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Stack::Chuck_VM_Stack()
{
    stack = sp = sp_max = NULL;
    prev = next = NULL;
    m_is_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Stack()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Stack::~Chuck_VM_Stack()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Code()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Code::Chuck_VM_Code()
{
    instr = NULL;
    num_instr = 0;
    stack_depth = 0;
    need_this = FALSE;
    native_func = 0;
    native_func_type = NATIVE_UNKNOWN;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Code()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Code::~Chuck_VM_Code()
{
    // free instructions
    if( instr )
    {
        // loop over array
        for( t_CKUINT i = 0; i < num_instr; i++ )
            delete instr[i];

        // free the array
        SAFE_DELETE_ARRAY( instr );
    }

    num_instr = 0;
}




// offset in bytes at the beginning of a stack for initializing data
#define VM_STACK_OFFSET  16
// 1/factor of stack is left blank, to give room to detect overflow
#define VM_STACK_PADDING_FACTOR 16
//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Stack::initialize( t_CKUINT size )
{
    if( m_is_init )
        return FALSE;

    // make room for header
    size += VM_STACK_OFFSET;
    // allocate stack
    stack = new t_CKBYTE[size];
    if( !stack ) goto out_of_memory;

    // zero
    memset( stack, 0, size );

    // advance stack after the header
    stack += VM_STACK_OFFSET;
    // set the sp
    sp = stack;
    // upper limit (padding factor)
    sp_max = sp + size - (size / VM_STACK_PADDING_FACTOR);

    // set flag and return
    return m_is_init = TRUE;

out_of_memory:

    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while allocating stack\n" );

    // return FALSE
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Stack::shutdown()
{
    if( !m_is_init )
        return FALSE;

    // free the stack
    stack -= VM_STACK_OFFSET;
    SAFE_DELETE_ARRAY( stack );
    sp = sp_max = NULL;

    // set the flag to false
    m_is_init = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Shred()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred::Chuck_VM_Shred()
{
    mem = new Chuck_VM_Stack;
    reg = new Chuck_VM_Stack;
    code = code_orig = NULL;
    next = prev = NULL;
    instr = NULL;
    parent = NULL;
    // obj_array = NULL;
    // obj_array_size = 0;
    base_ref = NULL;
    vm_ref = NULL;
    event = NULL;
    xid = 0;
    m_serials = NULL;

    // set
    CK_TRACK( stat = NULL );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Shred()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred::~Chuck_VM_Shred()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::initialize( Chuck_VM_Code * c,
                                     t_CKUINT mem_stack_size, 
                                     t_CKUINT reg_stack_size )
{
    // allocate mem and reg
    if( !mem->initialize( mem_stack_size ) ) return FALSE;
    if( !reg->initialize( reg_stack_size ) ) return FALSE;

    // program counter
    pc = 0;
    next_pc = 1;
    // code pointer
    code_orig = code = c;
    // add reference
    code_orig->add_ref();
    // shred in dump (all done)
    is_dumped = FALSE;
    // shred done
    is_done = FALSE;
    // shred running
    is_running = FALSE;
    // shred abort
    is_abort = FALSE;
    // set the instr
    instr = c->instr;
    // zero out the id
    xid = 0;

    // initialize
    initialize_object( this, vm_ref->env()->t_shred );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::shutdown()
{
    // spencer - March 2012 (added 1.3.0.0)
    // can't dealloc ugens while they are still keys to a map; 
    // add reference, store them in a vector, and release them after
    // SPENCERTODO: is there a better way to do this????
    std::vector<Chuck_UGen *> release_v;
    release_v.reserve(m_ugen_map.size());
    
    // get iterator to our map
    map<Chuck_UGen *, Chuck_UGen *>::iterator iter = m_ugen_map.begin();
    while( iter != m_ugen_map.end() )
    {
        // get the ugen
        Chuck_UGen * ugen = iter->first;
        // CK_GC_LOG("Chuck_VM_Shred::shutdown() disconnect: 0x%08x", ugen);
        
        // store ref in array for now (added 1.3.0.0)
        release_v.push_back(ugen);
        // no need to bump reference since now ugen_map ref counts
        // ugen->add_ref();
        
        // disconnect
        ugen->disconnect( TRUE );

        // advance the iterator
        iter++;
    }

    // clear map
    m_ugen_map.clear();

    // loop over vector
    for( vector<Chuck_UGen *>::iterator rvi = release_v.begin(); 
         rvi != release_v.end(); rvi++ )
    {
        // release it
        (*rvi)->release();
    }
    
    // loop over parent object references (added 1.3.1.2)
    for( vector<Chuck_Object *>::iterator it = m_parent_objects.begin();
         it != m_parent_objects.end(); it++ )
    {
        // release it
        (*it)->release();
    }
    
    // clear the vectors (added 1.3.1.2)
    release_v.clear();
    m_parent_objects.clear();

    // reclaim the stacks
    SAFE_DELETE( mem );
    SAFE_DELETE( reg );
    base_ref = NULL;
    
    // delete temp pointer space
    // SAFE_DELETE_ARRAY( obj_array );
    // obj_array_size = 0;

    // TODO: is this right?
    if( code_orig )
        code_orig->release();
    // clear it
    code_orig = code = NULL;

    // HACK (added 1.3.2.0): close serial devices
    if(m_serials != NULL)
    {
        for(list<Chuck_IO_Serial *>::iterator i = m_serials->begin(); i != m_serials->end(); i++)
        {
            (*i)->release();
            (*i)->close();
        }
        
        m_serials->clear();
        SAFE_DELETE(m_serials);
    }
    
    // 1.3.5.3 pop all loop counters
    while( this->popLoopCounter() );
    
    // TODO: what to do with next and prev?
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::add( Chuck_UGen * ugen )
{
    if( !ugen || m_ugen_map[ugen] )
        return FALSE;

    // increment reference count (added 1.3.0.0)
    ugen->add_ref();
    
    // RUBBISH
    // cerr << "vm add ugen: 0x" << hex << (int)ugen << endl;

    m_ugen_map[ugen] = ugen;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: remove()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::remove( Chuck_UGen * ugen )
{
    if( !ugen || !m_ugen_map[ugen] )
        return FALSE;

    // decrement reference count (added 1.3.0.0)
    ugen->release();
    
    // RUBBISH
    // cerr << "vm remove ugen: 0x" << hex << (int)ugen << endl;

    // remove it
    m_ugen_map.erase( ugen );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add_parent_ref()
// desc: add parent object reference (added 1.3.1.2)
//-----------------------------------------------------------------------------
t_CKVOID Chuck_VM_Shred::add_parent_ref( Chuck_Object * obj )
{
    // sanity check
    if( !obj )
        return;
    
    // reference count
    obj->add_ref();
    
    // add it to vector
    m_parent_objects.push_back( obj );
}




//-----------------------------------------------------------------------------
// name: run()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::run( Chuck_VM * vm )
{
    // get the code
    instr = code->instr;
    is_running = TRUE;
    // pointer to running state
    t_CKBOOL * loop_running = &(vm_ref->runningState());

    // go!
    while( is_running && *loop_running && !is_abort )
    {
//-----------------------------------------------------------------------------
CK_VM_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG =--------------------------------=\n" ) );
CK_VM_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG shred %04lu code %s pc %04lu %s( %s )\n",
             this->xid, this->code->name.c_str(), this->pc, instr[pc]->name(),
             instr[pc]->params() ) );
CK_VM_DEBUG( t_CKBYTE * t_mem_sp = this->mem->sp );
CK_VM_DEBUG( t_CKBYTE * t_reg_sp = this->mem->sp );
//-----------------------------------------------------------------------------
        // execute the instruction
        instr[pc]->execute( vm, this );
//-----------------------------------------------------------------------------
CK_VM_DEBUG(CK_FPRINTF_STDERR( "CK_VM_DEBUG mem sp in: 0x%08lx out: 0x%08lx\n",
                    (unsigned long) t_mem_sp, (unsigned long) this->mem->sp ));
CK_VM_DEBUG(CK_FPRINTF_STDERR( "CK_VM_DEBUG reg sp in: 0x%08lx out: 0x%08lx\n",
                    (unsigned long) t_reg_sp, (unsigned long) this->reg->sp ));
//-----------------------------------------------------------------------------
        // set to next_pc;
        pc = next_pc;
        next_pc++;

        // track number of cycles
        CK_TRACK( this->stat->cycles++ );
    }
    
    // check abort
    if( is_abort )
    {
        // log
        EM_log( CK_LOG_SYSTEM, "aborting shred (id: %d)", this->xid );
        // done
        is_done = TRUE;
    }

    // is the shred finished
    return !is_done;
}



//-----------------------------------------------------------------------------
// name: add_serialio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKVOID Chuck_VM_Shred::add_serialio( Chuck_IO_Serial * serial )
{
    if(m_serials == NULL)
        m_serials = new list<Chuck_IO_Serial *>;
    serial->add_ref();
    m_serials->push_back( serial );
}



//-----------------------------------------------------------------------------
// name: add_serialio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKVOID Chuck_VM_Shred::remove_serialio( Chuck_IO_Serial * serial )
{
    if(m_serials == NULL)
        return;
    m_serials->remove( serial );
    serial->release();
}



//-----------------------------------------------------------------------------
// name: pushLoopCounter()
// desc: make and push new loop counter
//-----------------------------------------------------------------------------
t_CKUINT * Chuck_VM_Shred::pushLoopCounter()
{
    // new pointer
    t_CKUINT * counter = new t_CKUINT;
    // add to stack
    m_loopCounters.push_back(counter);
    // return it
    return counter;
}




//-----------------------------------------------------------------------------
// name: currentLoopCounter()
// desc: get top loop counter
//-----------------------------------------------------------------------------
t_CKUINT * Chuck_VM_Shred::currentLoopCounter()
{
    // check it
    if( m_loopCounters.size() == 0 ) return NULL;
    // return it
    return m_loopCounters[m_loopCounters.size()-1];
}




//-----------------------------------------------------------------------------
// name: popLoopCounter()
// desc: pop and clean up loop counter
//-----------------------------------------------------------------------------
bool Chuck_VM_Shred::popLoopCounter()
{
    // check it
    if( m_loopCounters.size() == 0 ) return false;
    // delete
    delete m_loopCounters[m_loopCounters.size()-1];
    // return it
    m_loopCounters.pop_back();
    // done
    return true;
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Shreduler()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler::Chuck_VM_Shreduler()
{
    now_system = 0;
    rt_audio = FALSE;
    vm_ref = NULL;
    shred_list = NULL;
    m_current_shred = NULL;
    m_dac = NULL;
    m_adc = NULL;
    m_bunghole = NULL;
    m_num_dac_channels = 0;
    m_num_adc_channels = 0;
    
    set_adaptive( 0 );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Shreduler()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler::~Chuck_VM_Shreduler()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::initialize()
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shutdown()
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_adaptive()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::set_adaptive( t_CKUINT max_block_size )
{
    m_max_block_size = max_block_size > 1 ? max_block_size : 0;
    m_adaptive = m_max_block_size > 1;
    m_samps_until_next = -1;
}




//-----------------------------------------------------------------------------
// name: add_blocked()
// desc: add shred to the shreduler's blocked list
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::add_blocked( Chuck_VM_Shred * shred )
{
    // add shred to map, using pointer
    blocked[shred] = shred;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: remove_blocked()
// desc: remove shred from the shreduler's blocked list
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::remove_blocked( Chuck_VM_Shred * shred )
{
    // remove from hash
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::iterator iter;
    iter = blocked.find( shred );
    blocked.erase( iter );

    // remove from event
    if( shred->event != NULL )
    {
        Chuck_Event * event_to_release = shred->event;
        // this call causes shred->event to become a null pointer
        shred->event->remove( shred );
        // but, we still have to release the event afterward
        // to signify that the shred is done using the event
        SAFE_RELEASE( event_to_release );
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shredule()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shredule( Chuck_VM_Shred * shred )
{
    return this->shredule( shred, now_system );
}




//-----------------------------------------------------------------------------
// name: shredule()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shredule( Chuck_VM_Shred * shred,
                                       t_CKTIME wake_time )
{
    // sanity check
    if( shred->prev || shred->next )
    {
        // something is really wrong here - no shred can be 
        // shreduled more than once
        EM_error3( "[chuck](VM): internal sanity check failed in shredule()" );
        EM_error3( "[chuck](VM): (shred shreduled while shreduled)" );

        return FALSE;
    }

    // sanity check
    if( wake_time < (this->now_system - .5) )
    {
        // trying to enqueue on a time that is less than now
        EM_error3( "[chuck](VM): internal sanity check failed in shredule()" );
        EM_error3( "[chuck](VM): (wake time is past) - %f : %f", wake_time, this->now_system );

        return FALSE;
    }

    shred->wake_time = wake_time;

    // list empty
    if( !shred_list )
        shred_list = shred;
    else
    {
        // pointers to the shred queue
        Chuck_VM_Shred * curr = shred_list;
        Chuck_VM_Shred * prev = NULL;

        while( curr )
        {
            // found the place to insert
            if( curr->wake_time > wake_time )
                break;

            prev = curr;
            curr = curr->next;
        }

        if( !prev )
        {
            shred->next = shred_list;
            if( shred_list ) shred_list->prev = shred;
            shred_list = shred;
        }
        else
        {
            // insert the shred in sorted order
            shred->next = prev->next;
            shred->prev = prev;
            if( prev->next ) prev->next->prev = shred;
            prev->next = shred;
        }
    }

    t_CKTIME diff = shred_list->wake_time - this->now_system;
    if( diff < 0 ) diff = 0;
    // if( diff < m_samps_until_next )
    m_samps_until_next = diff;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: advance_v()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::advance_v( t_CKINT & numLeft, t_CKINT & offset )
{
    t_CKINT i, j, numFrames;
    SAMPLE gain[256], sum;
    // get audio data from VM
    const SAMPLE * input = vm_ref->input_ref() + (offset*m_num_adc_channels);
    SAMPLE * output = vm_ref->output_ref() + (offset*m_num_dac_channels);
    
    // compute number of frames to compute; update
    numFrames = ck_min( m_max_block_size, numLeft );
    if( this->m_samps_until_next >= 0 )
    {
        numFrames = (t_CKINT)(ck_min( numFrames, this->m_samps_until_next ));
        if( numFrames == 0 ) numFrames = 1;
        this->m_samps_until_next -= numFrames;
    }
    numLeft -= numFrames;
    offset += numFrames;

    // advance system 'now'
    this->now_system += numFrames;

    // ???
    for( j = 0; j < m_num_adc_channels; j++ )
    {
        // update channel
        m_adc->m_multi_chan[j]->m_time = this->now_system;
        // cache gain
        gain[j] = m_adc->m_multi_chan[j]->m_gain;
    }
    
    // INPUT: adaptive block
    for( i = 0; i < numFrames; i++ )
    {
        // clear
        sum = 0.0f;
        // loop over channels
        for( j = 0; j < m_num_adc_channels; j++ )
        {
            m_adc->m_multi_chan[j]->m_current_v[i] = input[j] * gain[j] * m_adc->m_gain;
            sum += m_adc->m_multi_chan[j]->m_current_v[i];
        }
        m_adc->m_current_v[i] = sum / m_num_adc_channels;
        
        // advance pointer
        input += m_num_adc_channels;
    }
    
    // ???
    for( j = 0; j < m_num_adc_channels; j++ )
    {
        // update last
        m_adc->m_multi_chan[j]->m_last = m_adc->m_multi_chan[j]->m_current_v[numFrames-1];
    }
    // update last
    m_adc->m_last = m_adc->m_current_v[numFrames-1];
    // update time
    m_adc->m_time = this->now_system;

    // PROCESSING
    m_dac->system_tick_v( this->now_system, numFrames );

    // suck samples
    m_bunghole->system_tick_v( this->now_system, numFrames );

    // OUTPUT: adaptive block
    for( i = 0; i < numFrames; i++ )
    {
        for( j = 0; j < m_num_dac_channels; j++ )
            output[j] = m_dac->m_multi_chan[j]->m_current_v[i];
        
        // advance pointer
        output += m_num_dac_channels;
    }
}




//-----------------------------------------------------------------------------
// name: advance()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::advance( t_CKINT N )
{
    // advance system 'now'
    this->now_system += 1;

    // tick the dac; TODO: unhardcoded frame size!
    SAMPLE sum = 0.0f;
    t_CKUINT i;
    // input and output
    const SAMPLE * input = vm_ref->input_ref() + (N*m_num_adc_channels);
    SAMPLE * output = vm_ref->output_ref() + (N*m_num_dac_channels);

    // INPUT: loop over channels
    for( i = 0; i < m_num_adc_channels; i++ )
    {
        // ge: switched order of lines 1.3.5.3
        m_adc->m_multi_chan[i]->m_last = m_adc->m_multi_chan[i]->m_current;
        m_adc->m_multi_chan[i]->m_current = input[i] * m_adc->m_multi_chan[i]->m_gain * m_adc->m_gain;
        m_adc->m_multi_chan[i]->m_time = this->now_system;
        sum += m_adc->m_multi_chan[i]->m_current;
    }
    m_adc->m_last = m_adc->m_current = sum / m_num_adc_channels;
    m_adc->m_time = this->now_system;

    // PROCESSING
    m_dac->system_tick( this->now_system );
    // OUTPUT
    for( i = 0; i < m_num_dac_channels; i++ )
        output[i] = m_dac->m_multi_chan[i]->m_current; // * .5f;

    // suck samples
    m_bunghole->system_tick( this->now_system );
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM_Shreduler::get( )
{
    Chuck_VM_Shred * shred = shred_list;

    // list empty
    if( !shred )
    {
        m_samps_until_next = -1;
        return NULL;
    }

    // TODO: should this be <=?
    if( shred->wake_time <= ( this->now_system + .5 ) )
    {
        // if( shred->wake_time < this->now_system )
        //    assert( false );

        shred_list = shred->next;
        shred->next = NULL;
        shred->prev = NULL;
        
        if( shred_list )
        {
            shred_list->prev = NULL;
            m_samps_until_next = shred_list->wake_time - this->now_system;
            if( m_samps_until_next < 0 ) m_samps_until_next = 0;
        }

        return shred;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name: highest()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM_Shreduler::highest( )
{
    Chuck_VM_Shred * shred = shred_list;
    t_CKUINT n = 0;

    while( shred )
    {
        if( shred->xid > n ) n = shred->xid;
        shred = shred->next;
    }

    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::iterator iter;    
    for( iter = blocked.begin(); iter != blocked.end(); iter++ )
    {
        shred = (*iter).second;
        if( shred->xid > n ) n = shred->xid;
    }

    return n;
}




//-----------------------------------------------------------------------------
// name: replace()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::replace( Chuck_VM_Shred * out, Chuck_VM_Shred * in )
{
    assert( FALSE );

    // sanity check
    if( !out || !in )
        return FALSE;

    if( !out->prev )
        shred_list = in;
    else
        out->prev->next = in;
    
    if( out->next )
        out->next->prev = in;
    
    in->next = out->next;
    in->prev = out->prev;
    
    out->next = out->prev = NULL;

    in->wake_time = out->wake_time;
    in->start = in->wake_time;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: remove()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::remove( Chuck_VM_Shred * out )
{
    if( !out ) return FALSE;

    // if blocked
    if( out->event != NULL )
    {
        return remove_blocked( out );
    }

    // sanity check
    if( !out->prev && !out->next && out != shred_list )
        return FALSE;
    
    if( !out->prev )
        shred_list = out->next;
    else
        out->prev->next = out->next;
    
    if( out->next )
        out->next->prev = out->prev;
    
    out->next = out->prev = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM_Shreduler::lookup( t_CKUINT xid )
{
    Chuck_VM_Shred * shred = shred_list;

    // current shred?
    if( m_current_shred != NULL && m_current_shred->xid == xid )
        return m_current_shred;

    // look for in shreduled list
    while( shred )
    {
        if( shred->xid == xid )
            return shred;

        shred = shred->next;
    }

    // blocked?
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::iterator iter;
    for( iter = blocked.begin(); iter != blocked.end(); iter++ )
    {
        shred = (*iter).second;
        if( shred->xid == xid )
            return shred;
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: SortByID()
// desc: ...
//-----------------------------------------------------------------------------
struct SortByID
{
    bool operator() ( const Chuck_VM_Shred * lhs, const Chuck_VM_Shred * rhs )
    {    return lhs->xid < rhs->xid; }
};




//-----------------------------------------------------------------------------
// name: status()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::status( Chuck_VM_Status * status )
{
    Chuck_VM_Shred * shred = shred_list;
    Chuck_VM_Shred * temp = NULL;

    t_CKUINT srate = vm_ref->srate(); // 1.3.5.3; was: Digitalio::sampling_rate();
    t_CKUINT s = (t_CKUINT)now_system;
    t_CKUINT h = s/(srate*3600);
    s = s - (h*(srate*3600));
    t_CKUINT m = s / (srate*60);
    s = s - (m*(srate*60));
    t_CKUINT sec = s / srate;
    s = s - (sec*(srate));
    // float millisecond = s / (float)(srate) * 1000.0f;
    
    status->srate = srate;
    status->now_system = now_system;
    status->t_second = sec;
    status->t_minute = m;
    status->t_hour = h;
    
    // get list of shreds
    vector<Chuck_VM_Shred *> list;
    while( shred )
    {
        list.push_back( shred );
        shred = shred->next;
    }

    // get blocked
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::iterator iter;    
    for( iter = blocked.begin(); iter != blocked.end(); iter++ )
    {
        shred = (*iter).second;
        list.push_back( shred );
    }

    // get current shred
    if( ( temp = m_current_shred ) )
        list.push_back( temp );

    // sort the list
    SortByID byid;
    std::sort( list.begin(), list.end(), byid );

    // print status
    status->clear();
    for( t_CKUINT i = 0; i < list.size(); i++ )
    {
        shred = list[i];
        status->list.push_back( new Chuck_VM_Shred_Status(
            shred->xid, shred->name, shred->start, shred->event != NULL ) );
    }    
}




//-----------------------------------------------------------------------------
// name: status()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::status( )
{
    Chuck_VM_Shred_Status * shred = NULL;

    this->status( &m_status );
    t_CKUINT h = m_status.t_hour;
    t_CKUINT m = m_status.t_minute;
    t_CKUINT sec = m_status.t_second;
    CK_FPRINTF_STDOUT( "[chuck](VM): status (now == %ldh%ldm%lds, %.1f samps) ...\n",
             h, m, sec, m_status.now_system );

    // print status
    for( t_CKUINT i = 0; i < m_status.list.size(); i++ )
    {
        shred = m_status.list[i];
        CK_FPRINTF_STDOUT( 
            "    [shred id]: %ld  [source]: %s  [spork time]: %.2fs ago%s\n",
            shred->xid, mini( shred->name.c_str() ),
            (m_status.now_system - shred->start) / m_status.srate,
            shred->has_event ? " (blocked)" : "" );
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Status()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Status::Chuck_VM_Status()
{
    srate = 0;
    now_system = 0;
    t_second = t_minute = t_hour = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Status()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Status::~Chuck_VM_Status()
{
    this->clear();
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Status::clear()
{
    for( t_CKUINT i = 0; i < list.size(); i++ )
    {
        SAFE_DELETE( list[i] );
    }
    
    list.clear();
}
