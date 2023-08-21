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
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_lang.h"
#include "chuck_type.h"
#include "chuck_dl.h"
#include "chuck_globals.h" // added 1.4.1.0 (jack)
#include "chuck_errmsg.h"
#include "chuck.h"
#include "ugen_xxx.h"
#include "util_buffers.h"
#include "util_platforms.h"
#include "util_string.h"

#ifndef __DISABLE_SERIAL__
#include "chuck_io.h"
#endif

#ifndef __DISABLE_HID__
#include "hidio_sdl.h"  // 1.4.0.0
#endif

#ifndef __DISABLE_MIDI__
#include "midiio_rtmidi.h"  // 1.4.1.0
#endif

#include <string>
#include <algorithm>
using namespace std;




//-----------------------------------------------------------------------------
// uncomment AND set to 1 to compile VM stack debug messages
//-----------------------------------------------------------------------------
#define CK_VM_STACK_DEBUG_ENABLE 0
// vm statck debug macros
#if CK_VM_STACK_DEBUG_ENABLE
#include <typeinfo>
#define CK_VM_STACK_DEBUG(x) x
#else
#define CK_VM_STACK_DEBUG(x)
#endif




//-----------------------------------------------------------------------------
// name: struct SortByID_LT()
// desc: for sorting less than
//-----------------------------------------------------------------------------
struct SortByID_LT
{
    bool operator()( const Chuck_VM_Shred * lhs, const Chuck_VM_Shred * rhs )
    { return lhs->xid < rhs->xid; }
};




//-----------------------------------------------------------------------------
// name: struct SortByID_GT()
// desc: for sorting greater than
//-----------------------------------------------------------------------------
struct SortByID_GT
{
    bool operator()( const Chuck_VM_Shred * lhs, const Chuck_VM_Shred * rhs )
    { return lhs->xid > rhs->xid; }
};




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
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM::Chuck_VM()
{
    // REFACTOR-2017: refs
    m_carrier = NULL;

    // data
    m_num_shreds = 0;
    m_shreduler = NULL;
    m_num_dumped_shreds = 0;
    m_globals_manager = NULL; // 1.4.1.0 (jack)
    m_msg_buffer = NULL;
    m_reply_buffer = NULL;
    m_event_buffer = NULL;
    m_shred_id = 0;
    m_halt = TRUE;
    m_is_running = FALSE;

    m_dac = NULL;
    m_adc = NULL;
    m_bunghole = NULL;
    m_srate = 0;
    m_num_dac_channels = 0;
    m_num_adc_channels = 0;
    m_init = FALSE;
    m_input_ref = NULL;
    m_output_ref = NULL;
    m_current_buffer_frames = 0;
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

    // 1.4.1.0 (jack): added globals manager
    EM_log( CK_LOG_SYSTEM, "allocating globals manager..." );
    m_globals_manager = new Chuck_Globals_Manager( this );

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

    if( !env()->ckt_dac || !env()->ckt_adc )
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
    EM_log( CK_LOG_SEVERE, "initializing 'dac'..." );
    // allocate dac and adc (REFACTOR-2017: g_t_dac changed to env()->ckt_dac)
    env()->ckt_dac->ugen_info->num_outs = env()->ckt_dac->ugen_info->num_ins = m_num_dac_channels;
    m_dac = (Chuck_UGen *)instantiate_and_initialize_object( env()->ckt_dac, this );
    // Chuck_DL_Api::Api::instance() added 1.3.0.0
    object_ctor( m_dac, NULL, this, NULL, Chuck_DL_Api::Api::instance() ); // TODO: this can't be the place to do this
    stereo_ctor( m_dac, NULL, this, NULL, Chuck_DL_Api::Api::instance() ); // TODO: is the NULL shred a problem?
    multi_ctor( m_dac, NULL, this, NULL, Chuck_DL_Api::Api::instance() );  // TODO: remove and let type system do this
    m_dac->add_ref();
    // lock it
    m_dac->lock();

    // log
    EM_log( CK_LOG_SEVERE, "initializing 'adc'..." );
    // (REFACTOR-2017: g_t_adc changed to env()->ckt_adc)
    env()->ckt_adc->ugen_info->num_ins = env()->ckt_adc->ugen_info->num_outs = m_num_adc_channels;
    m_adc = (Chuck_UGen *)instantiate_and_initialize_object( env()->ckt_adc, this );
    // Chuck_DL_Api::Api::instance() added 1.3.0.0
    object_ctor( m_adc, NULL, this, NULL, Chuck_DL_Api::Api::instance() ); // TODO: this can't be the place to do this
    stereo_ctor( m_adc, NULL, this, NULL, Chuck_DL_Api::Api::instance() );
    multi_ctor( m_adc, NULL, this, NULL, Chuck_DL_Api::Api::instance() ); // TODO: remove and let type system do this
    m_adc->add_ref();
    // lock it
    m_adc->lock();

    // log
    EM_log( CK_LOG_SEVERE, "initializing 'blackhole'..." );
    m_bunghole = new Chuck_UGen;
    m_bunghole->add_ref();
    m_bunghole->lock();
    initialize_object( m_bunghole, env()->ckt_ugen );
    m_bunghole->tick = NULL;
    m_bunghole->alloc_v( m_shreduler->m_max_block_size );
    m_shreduler->m_dac = m_dac;
    m_shreduler->m_adc = m_adc;
    m_shreduler->m_bunghole = m_bunghole;
    m_shreduler->m_num_dac_channels = m_num_dac_channels;
    m_shreduler->m_num_adc_channels = m_num_adc_channels;

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

    // REFACTOR-2017: clean up after my global variables
    m_globals_manager->cleanup_global_variables();
    CK_SAFE_DELETE( m_globals_manager );

    // log
    EM_log( CK_LOG_SEVERE, "removing shreds..." );
    // removal all | 1.5.0.8 (ge) updated from previously non-functioning code
    this->removeAll();

    // log
    EM_log( CK_LOG_SYSTEM, "freeing shreduler..." );
    // free the shreduler
    CK_SAFE_DELETE( m_shreduler );

    #ifndef __DISABLE_HID__
    // log
    EM_log( CK_LOG_SYSTEM, "unregistering VM from HID manager..." );
    // clean up this vm
    HidInManager::cleanup_buffer( this );
    #endif

    #ifndef __DISABLE_MIDI__
    EM_log( CK_LOG_SYSTEM, "unregistering VM from MIDI manager..." );
    MidiInManager::cleanup_buffer( this );
    #endif

    // log
    EM_log( CK_LOG_SYSTEM, "freeing msg/reply/event buffers..." );
    // free the msg buffer
    CK_SAFE_DELETE( m_msg_buffer );
    // free the reply buffer
    CK_SAFE_DELETE( m_reply_buffer );
    // free the event buffer
    CK_SAFE_DELETE( m_event_buffer );

    // log
    EM_pushlog();
    EM_log( CK_LOG_SEVERE, "freeing dumped shreds..." );
    // do it
    this->release_dump();
    EM_poplog();

    // log
    EM_log( CK_LOG_SYSTEM, "freeing special ugens..." );
    // go
    CK_SAFE_RELEASE( m_dac );
    CK_SAFE_RELEASE( m_adc );
    CK_SAFE_RELEASE( m_bunghole );

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
    if( m_is_running ) return FALSE;
    // set state
    m_is_running = TRUE;
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: running()
// desc: get run state
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::running() const
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

    // REFACTOR-2017: spork queued shreds, handle global messages
    // this is called once per chuck time / sample / "tick"
    // global manager added 1.4.1.0 (jack)
    m_globals_manager->handle_global_queue_messages();

    // iterate until no more shreds/events/messages
    while( iterate )
    {
        // get the shreds queued for 'now'
        shred = m_shreduler->get();
        // run all shreds waiting to run 'now'
        while( shred != NULL )
        {
            // set the current time of the shred
            shred->now = shred->wake_time;
            // track shred activation
            CK_TRACK( Chuck_Stats::instance()->activate_shred( shred ) );

            // run the shred
            // 1.5.0.0 (ge) add check for shred->is_done, which flags a shred to exit
            if( shred->is_done || !shred->run( this ) )
            {
                // track shred deactivation
                CK_TRACK( Chuck_Stats::instance()->deactivate_shred( shred ) );

                this->free( shred, TRUE );
                shred = NULL;
                if( !m_num_shreds && m_halt ) return FALSE;
            }

            // track shred deactivation
            CK_TRACK( if( shred ) Chuck_Stats::instance()->deactivate_shred( shred ) );
            // get next shred queued for 'now'
            shred = m_shreduler->get();
        }

        // set to false for now
        iterate = FALSE;

        // broadcast queued events
        while( m_event_buffer->get( &event, 1 ) )
        {
            event->broadcast_local();
            event->broadcast_global();
            iterate = TRUE;
        }

        // loop over thread-specific queued event buffers (added 1.3.0.0)
        for( list<CBufferSimple *>::const_iterator i = m_event_buffers.begin();
             i != m_event_buffers.end(); i++ )
        {
            while( (*i)->get( &event, 1 ) )
            {
                event->broadcast_local();
                event->broadcast_global();
                iterate = TRUE;
            }
        }

        // process messages
        while( m_msg_buffer->get( &msg, 1 ) )
        { process_msg( msg ); iterate = TRUE; }

        // clear dumped shreds
        if( m_num_dumped_shreds > 0 )
            release_dump();
    }

    // continue executing if have shreds left or if don't-halt
    // or if have shreds to add or globals to process
    // (TODO: restrict this to just shred-messages to pass, as it once was?)
    return ( m_num_shreds || !m_halt || m_globals_manager->more_requests() );
}




//-----------------------------------------------------------------------------
// name: run()
// desc: run VM and compute the next N frames of audio
//       `N` : the number of audio frames to run
//       `input`: the incoming input array of audio samples
//       `output`: the outgoing output array of audio samples
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::run( t_CKINT N, const SAMPLE * input, SAMPLE * output )
{
    // copy
    m_input_ref = input; m_output_ref = output; m_current_buffer_frames = N;
    // frame count
    t_CKINT frame = 0;

    // zero output buffer
    memset( output, 0, N*m_num_dac_channels*sizeof(SAMPLE) );

    // for now, check for global variables once per sample (below)
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

    // log, check so we only log once, in case run() still gets called
    // after the initial stoppage; calling stop after resets m_is_running
    if( m_is_running ) EM_log( CK_LOG_SYSTEM, "stopping virtual machine..." );

    // stop | 1.3.5.3
    this->stop();

    // clear | 1.5.0.8
    m_input_ref = NULL; m_output_ref = NULL;

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
t_CKBOOL Chuck_VM::queue_msg( Chuck_Msg * msg, t_CKINT count )
{
    assert( count == 1 );
    m_msg_buffer->put( &msg, count );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: queue_event()
// desc: since 1.3.0.0 a buffer is passed in associated with each thread
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::queue_event( Chuck_Event * event, t_CKINT count,
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
// desc: get reply from reply buffer
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
// desc: process a VM message; NOTE: assumes msg is dynamically allocated...
//       and msg will be cleaned up by the VM
// TODO: make thread safe for multiple consumers
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::process_msg( Chuck_Msg * msg )
{
    t_CKUINT retval = 0xfffffff0;

    // check message type
    if( msg->type == CK_MSG_REPLACE )
    {
        Chuck_VM_Shred * out = m_shreduler->lookup( msg->param );
        if( !out )
        {
            EM_print2orange( "(VM) replacing shred: no shred with id %i...", msg->param );
            retval = 0;
            goto done;
        }

        Chuck_VM_Shred * shred = msg->shred;
        if( !shred )
        {
            shred = new Chuck_VM_Shred;
            shred->vm_ref = this;
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
            EM_print2blue( "(VM) replacing shred %i (%s) with %i (%s)...",
                            out->xid, mini(out->name.c_str()), shred->xid, mini(shred->name.c_str()) );
            this->free( out, TRUE, FALSE );
            retval = shred->xid;

            // tracking new shred
            CK_TRACK( Chuck_Stats::instance()->add_shred( shred ) );

            goto done;
        }
        else
        {
            EM_print2blue( "(VM) shreduler replacing shred %i...", out->xid );
            shred->release();
            retval = 0;
            goto done;
        }
    }
    else if( msg->type == CK_MSG_REMOVE )
    {
        if( msg->param == CK_NO_VALUE )
        {
            if( !this->m_num_shreds)
            {
                EM_print2orange( "(VM) no shreds to remove..." );
                retval = 0;
                goto done;
            }

            t_CKINT xid = m_shred_id;
            Chuck_VM_Shred * shred = NULL;
            while( xid >= 0 && m_shreduler->remove( shred = m_shreduler->lookup( xid ) ) == 0 )
                xid--;
            if( xid >= 0 )
            {
                EM_print2orange( "(VM) removing recent shred: %i (%s)...",
                                xid, mini(shred->name.c_str()) );
                this->free( shred, TRUE );
                retval = xid;
            }
            else
            {
                EM_print2orange( "(VM) no shreds removed..." );
                retval = 0;
                goto done;
            }
        }
        else
        {
            Chuck_VM_Shred * shred = m_shreduler->lookup( msg->param );
            if( !shred )
            {
                EM_print2orange( "(VM) cannot remove: no shred with id %i...", msg->param );
                retval = 0;
                goto done;
            }
            if( shred != m_shreduler->m_current_shred && !m_shreduler->remove( shred ) )  // was lookup
            {
                EM_print2orange( "(VM) shreduler: cannot remove shred %i...", msg->param );
                retval = 0;
                goto done;
            }
            EM_print2orange( "(VM) removing shred: %i (%s)...", msg->param, mini(shred->name.c_str()) );
            this->free( shred, TRUE );
            retval = msg->param;
        }
    }
    else if( msg->type == CK_MSG_REMOVEALL )
    {
        // print
        EM_print2magenta( "(VM) removing all (%i) shreds...", m_num_shreds );
        // remove all shreds
        this->removeAll();
    }
    else if( msg->type == CK_MSG_CLEARVM ) // added 1.3.2.0
    {
        // print
        EM_print2magenta( "(VM) removing all shreds and resetting type system" );
        // first, remove all shreds
        this->removeAll();
        // next, clear user type system
        if( env() ) env()->clear_user_namespace();
        // 1.4.1.0 (jack): also clear any global variables
        m_globals_manager->cleanup_global_variables();
    }
    else if( msg->type == CK_MSG_CLEARGLOBALS ) // added chunity
    {
        // clean up global variables without clearing the whole VM
        m_globals_manager->cleanup_global_variables();
    }
    else if( msg->type == CK_MSG_ADD )
    {
        t_CKUINT xid = 0;
        Chuck_VM_Shred * shred = NULL;
        if( msg->shred ) shred = this->spork( msg->shred );
        else shred = this->spork( msg->code, NULL, TRUE );
        xid = shred->xid;
        if( msg->args ) shred->args = *(msg->args);

        const char * s = ( msg->shred ? msg->shred->name.c_str() : msg->code->name.c_str() );
        EM_print2green( "(VM) sporking incoming shred: %i (%s)...", xid, mini(s) );
        retval = xid;
        goto done;
    }
    else if( msg->type == CK_MSG_EXIT )
    {
        EM_print2magenta( "(VM) EXIT received...." );
        // close file handles and clean up
        // REFACTOR-2017: TODO all_detach function
        // all_detach();
        // TODO: free more memory?

        // log
        EM_log( CK_LOG_INFO, "(VM): exiting..." );
        // come again
        exit( 1 );
    }
    else if( msg->type == CK_MSG_STATUS )
    {
        // fill in structure
        if( msg->status )
        {
            // get it
            m_shreduler->status( msg->status );
        }
        else
        {
            m_shreduler->status();
        }
    }
    else if( msg->type == CK_MSG_TIME )
    {
        float srate = m_srate; // 1.3.5.3; was: (float)Digitalio::sampling_rate();
        EM_print2magenta( "(VM) earth time: %s", timestamp_formatted().c_str() );
        EM_print2magenta( "(VM) chuck time: %.0f::samp (now)", m_shreduler->now_system );
        EM_print2vanilla( "%22.6f::second since VM start", m_shreduler->now_system / srate );
        EM_print2vanilla( "%22.6f::minute", m_shreduler->now_system / srate / 60.0f );
        EM_print2vanilla( "%22.6f::hour", m_shreduler->now_system / srate / 60.0f / 60.0f );
        EM_print2vanilla( "%22.6f::day", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f );
        EM_print2vanilla( "%22.6f::week", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f / 7.0f );
    }
    else if( msg->type == CK_MSG_RESET_ID )
    {
        // reset ID to currently active shred ID + 1
        // could be helpful to keep shred ID #'s low, especially after a lot of sporks
        EM_print2magenta( "(VM) resetting shred id to %lu...", this->reset_id() );
    }

done:

    // set return value
    msg->replyA = retval;

    // check reponse method
    if( msg->reply_cb ) // 1.5.0.8 (ge) added
    {
        // call the callback
        msg->reply_cb( msg );
    }
    else if( msg->reply_queue )
    {
        // put on reply queue
        m_reply_buffer->put( &msg, 1 );
    }
    else
    {
        // nothing further to be done; delete msg
        CK_SAFE_DELETE(msg);
    }

    return retval;
}




//-----------------------------------------------------------------------------
// name: next_id()
// desc: first increments the shred id, then returns it
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::next_id( )
{
    return ++m_shred_id;
}




//-----------------------------------------------------------------------------
// name: last_id()
// desc: returns the last used shred id
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::last_id( ) const
{
    return m_shred_id;
}




//-----------------------------------------------------------------------------
// name: reset_id() | 1.5.0.8 (ge) re-factored from code in process_msg
// desc: reset ID to lowest current ID + 1; returns what next ID would be
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::reset_id()
{
    // set shred_id to the current highest; will be incremented on next_id()
    m_shred_id = m_shreduler->highest();

    return m_shred_id + 1;
}




//-----------------------------------------------------------------------------
// name: shreduler()
// desc: get the VM's shreduler
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler * Chuck_VM::shreduler() const
{
    return m_shreduler;
}




//-----------------------------------------------------------------------------
// name: srate()
// desc: get VM sample rate
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::srate() const
{
    return m_srate; // 1.3.5.3; was: (t_CKUINT)Digitalio::sampling_rate();
}




//-----------------------------------------------------------------------------
// name: now() | 1.5.0.8 (ge) added
// desc: get the current chuck time
//-----------------------------------------------------------------------------
t_CKTIME Chuck_VM::now() const
{
    return m_shreduler ? m_shreduler->now_system : 0;
}




//-----------------------------------------------------------------------------
// name: spork()
// desc: spork shred from compiled VM code
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

    // get shred ID | 1.5.0.8 (ge) moved here so all shreds have IDs earlier
    // previously: only immediate-mode shreds were assigned IDs in this method
    // (via the other spork() function) and non-immediate shreds were returned
    // with ID 0 (their ID assignment would have been deferred)
    // presently: the shred created and returned by this function will be
    // assigned an ID regardless of the immediate flag
    shred->xid = next_id();

    // check whether to start running immediately at current time step
    // (this is more timely, but should be only used if called on same
    // thread as VM compute() / the audio thread)
    if( immediate )
    {
        // spork it
        this->spork( shred );
    }
    else // or be sporked later (by compute() on the next time step)
    {
        // spork it later
        Chuck_Global_Request spork_request;
        spork_request.type = spork_shred_request;
        spork_request.shred = shred;
        // added 1.4.1.0 (jack)
        m_globals_manager->add_request( spork_request );
    }

    // track new shred
    CK_TRACK( Chuck_Stats::instance()->add_shred( shred ) );

    return shred;
}




//-----------------------------------------------------------------------------
// name: spork()
// desc: spork a shred, shreduling it to run in the VM
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM::spork( Chuck_VM_Shred * shred )
{
    // set the current time
    shred->start = m_shreduler->now_system;
    // set the now
    shred->now = shred->wake_time = m_shreduler->now_system;
    // set the id, if one hasn't been assigned yet | 1.5.0.8 (ge) add check
    if( !shred->xid ) shred->xid = next_id();
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
// name: removeAll()
// desc: remove all shreds from VM
//-----------------------------------------------------------------------------
void Chuck_VM::removeAll()
{
    // get list of active shreds
    std::vector<Chuck_VM_Shred *> shreds;

    // get list from shreduler
    m_shreduler->get_all_shreds( shreds );

    // before 1.5.0.8, was:
    // m_shreduler->get_ready_shreds( shreds );

    // sort in descending ID order
    SortByID_GT byid;
    std::sort( shreds.begin(), shreds.end(), byid );

    // itereate over sorted list
    for( vector<Chuck_VM_Shred *>::iterator s = shreds.begin(); s != shreds.end(); s++ )
    {
        // remove and free
        if( m_shreduler->remove( *s ) ) this->free( *s, TRUE );
    }

    // reset
    m_shred_id = 0;
    m_num_shreds = 0;
}




//-----------------------------------------------------------------------------
// name: free()
// desc: deallocate a shred
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
    this->dump_shred( shred );
    shred = NULL;
    if( dec ) m_num_shreds--;
    if( !m_num_shreds ) m_shred_id = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: abort_current_shred()
// desc: abort the current shred
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
// name: dump_shred()
// desc: place a shred into the "dump" to be later released
//-----------------------------------------------------------------------------
void Chuck_VM::dump_shred( Chuck_VM_Shred * shred )
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
// desc: release the contents of the dump
//-----------------------------------------------------------------------------
void Chuck_VM::release_dump( )
{
    // log
    EM_log( CK_LOG_FINER, "releasing dumped shreds..." );

    // iterate through dump
    for( t_CKUINT i = 0; i < m_shred_dump.size(); i++ )
        CK_SAFE_RELEASE( m_shred_dump[i] );

    // clear the dump
    m_shred_dump.clear();
    // reset
    m_num_dumped_shreds = 0;
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Stack()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Stack::Chuck_VM_Stack()
{
    stack = sp = sp_max = NULL;
    prev = next = NULL;
    m_is_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Stack()
// desc: desctructor
//-----------------------------------------------------------------------------
Chuck_VM_Stack::~Chuck_VM_Stack()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Code()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Code::Chuck_VM_Code()
{
    instr = NULL;
    num_instr = 0;
    stack_depth = 0;
    need_this = FALSE;
    is_static = FALSE;
    native_func = 0;
    native_func_type = NATIVE_UNKNOWN;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Code()
// desc: destructor
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
        CK_SAFE_DELETE_ARRAY( instr );
    }

    num_instr = 0;
}




// offset in bytes at the beginning of a stack for initializing data
#define VM_STACK_OFFSET  16
// 1/factor of stack is left blank, to give room to detect overflow
#define VM_STACK_PADDING_FACTOR 16
//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize VM stack
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
    EM_error2( 0, "(VM) OutOfMemory: while allocating stack" );

    // return FALSE
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown and cleanup VM stack
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Stack::shutdown()
{
    if( !m_is_init )
        return FALSE;

    // free the stack
    stack -= VM_STACK_OFFSET;
    CK_SAFE_DELETE_ARRAY( stack );
    sp = sp_max = NULL;

    // set the flag to false
    m_is_init = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Shred()
// desc: constructor
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

    // initialize
    is_abort = FALSE;
    is_done = FALSE;
    is_dumped = FALSE;
    is_running = FALSE;
    pc = next_pc = 0;
    now = 0;
    start = 0;
    wake_time = 0;

#ifndef __DISABLE_SERIAL__
    m_serials = NULL;
#endif

    // set
    CK_TRACK( stat = NULL );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Shred()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_VM_Shred::~Chuck_VM_Shred()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize a shred from VM code
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
    initialize_object( this, vm_ref->env()->ckt_shred );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown a shred
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
    CK_SAFE_DELETE( mem );
    CK_SAFE_DELETE( reg );
    base_ref = NULL;

    // delete temp pointer space
    // CK_SAFE_DELETE_ARRAY( obj_array );
    // obj_array_size = 0;

    // TODO: is this right?
    if( code_orig )
        code_orig->release();
    // clear it
    code_orig = code = NULL;

    #ifndef __DISABLE_SERIAL__
    // HACK (added 1.3.2.0): close serial devices
    if(m_serials != NULL)
    {
        for(list<Chuck_IO_Serial *>::iterator i = m_serials->begin(); i != m_serials->end(); i++)
        {
            (*i)->release();
            (*i)->close();
        }

        m_serials->clear();
        CK_SAFE_DELETE(m_serials);
    }
    #endif

    // 1.3.5.3 pop all loop counters
    while( this->popLoopCounter() );

    // TODO: what to do with next and prev?

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: add a ugen to this shred's ugen map
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
// desc: remove a ugen from this shred's ugen map
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
// desc: run this shred's VM code
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::run( Chuck_VM * vm )
{
    // get the code
    instr = code->instr;
    is_running = TRUE;
    // pointer to running state
    const t_CKBOOL * loop_running = &(vm_ref->runningState());

    // go!
    while( is_running && *loop_running && !is_abort )
    {
//-----------------------------------------------------------------------------
CK_VM_STACK_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG =--------------------------------=\n" ) );
CK_VM_STACK_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG shred %04lu code %s pc %04lu %s( %s )\n",
                   this->xid, this->code->name.c_str(), this->pc, instr[pc]->name(),
                   instr[pc]->params() ) );
CK_VM_STACK_DEBUG( t_CKBYTE * t_mem_sp = this->mem->sp );
CK_VM_STACK_DEBUG( t_CKBYTE * t_reg_sp = this->reg->sp );
//-----------------------------------------------------------------------------
        // execute the instruction
        instr[pc]->execute( vm, this );
//-----------------------------------------------------------------------------
CK_VM_STACK_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG mem sp in: 0x%08lx out: 0x%08lx\n",
                   (unsigned long)t_mem_sp, (unsigned long)this->mem->sp ) );
CK_VM_STACK_DEBUG( CK_FPRINTF_STDERR( "CK_VM_DEBUG reg sp in: 0x%08lx out: 0x%08lx\n",
                   (unsigned long)t_reg_sp, (unsigned long)this->reg->sp ) );
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
// name: yield() | 1.5.0.5 (ge) made this a function from scattered code
// desc: yield the shred in vm (without advancing time, politely yield to run
//       all other shreds waiting to run at the current
//       same in effect as 0::second +=> now;
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::yield()
{
    // need a VM to yield on
    if( !this->vm_ref ) return FALSE;

    // suspend this shred
    this->is_running = FALSE;
    // reshredule this at the current time
    vm_ref->shreduler()->shredule( this, this->now );
    // done
    return TRUE;
}




#ifndef __DISABLE_SERIAL__
//-----------------------------------------------------------------------------
// name: add_serialio()
// desc: add serial IO (for event synchronization)
//-----------------------------------------------------------------------------
t_CKVOID Chuck_VM_Shred::add_serialio( Chuck_IO_Serial * serial )
{
    // instantiate list if needed
    if( m_serials == NULL )
        m_serials = new list<Chuck_IO_Serial *>;
    // add reference
    serial->add_ref();
    // append to list
    m_serials->push_back( serial );
}




//-----------------------------------------------------------------------------
// name: remove_serialio()
// desc: remove serial IO
//-----------------------------------------------------------------------------
t_CKVOID Chuck_VM_Shred::remove_serialio( Chuck_IO_Serial * serial )
{
    // check
    if( m_serials == NULL ) return;
    // remove from list
    m_serials->remove( serial );
    // release
    serial->release();
}
#endif




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
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler::Chuck_VM_Shreduler()
{
    now_system = 0;
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
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler::~Chuck_VM_Shreduler()
{
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize shreduler
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::initialize()
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown shreduler
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shutdown()
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_adaptive()
// desc: set adaptive mode (and adapative mode max block size)
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
        CK_SAFE_RELEASE( event_to_release );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shredule()
// desc: shredule a shred in the shreduler
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shredule( Chuck_VM_Shred * shred )
{
    return this->shredule( shred, now_system );
}




//-----------------------------------------------------------------------------
// name: shredule()
// desc: shredule a shred in the shreduler by wake time
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::shredule( Chuck_VM_Shred * shred,
                                       t_CKTIME wake_time )
{
    // sanity check
    if( shred->prev || shred->next )
    {
        // something is really wrong here - no shred can be
        // shreduled more than once
        EM_error2( 0, "(VM) internal consistency check failed in shredule()" );
        EM_error2( 0, "(VM)  |- (shred shreduled while shreduled)" );

        return FALSE;
    }

    // sanity check
    if( wake_time < (this->now_system - .5) )
    {
        // trying to enqueue on a time that is less than now
        EM_error2( 0, "(VM) internal consistency check failed in shredule()" );
        EM_error2( 0, "(VM)  |- (wake time is past) - %f : %f", wake_time, this->now_system );

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
// desc: advance the shreduler, v is for vectorize
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::advance_v( t_CKINT & numLeft, t_CKINT & offset )
{
    t_CKINT i, j, numFrames;
    SAMPLE gain[256], sum;

    #ifdef __CHUCK_USE_PLANAR_BUFFERS__
    // planar buffers means non-interleaved. this is required for some platforms,
    // e.g. WebAudio
    const SAMPLE * input = vm_ref->input_ref() + offset;
    SAMPLE * output = vm_ref->output_ref() + offset;
    t_CKUINT buffer_length = vm_ref->most_recent_buffer_length();
    #else
    // get audio data from VM
    const SAMPLE * input = vm_ref->input_ref() + (offset*m_num_adc_channels);
    SAMPLE * output = vm_ref->output_ref() + (offset*m_num_dac_channels);
    #endif

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
            #ifdef __CHUCK_USE_PLANAR_BUFFERS__
            // current frame = offset + i
            // current channel = j
            // sample = buffer size * j + offset + i
            // the "+offset" is taken care of in initialization
            // the "+i" is taken care of by incrementing input below
            m_adc->m_multi_chan[j]->m_current_v[i] = input[buffer_length * j] * gain[j] * m_adc->m_gain;
            #else
            m_adc->m_multi_chan[j]->m_current_v[i] = input[j] * gain[j] * m_adc->m_gain;
            #endif
            sum += m_adc->m_multi_chan[j]->m_current_v[i];
        }
        m_adc->m_current_v[i] = sum / m_num_adc_channels;

        // advance pointer
        #ifdef __CHUCK_USE_PLANAR_BUFFERS__
        input++;
        #else
        input += m_num_adc_channels;
        #endif
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
        {
            #ifdef __CHUCK_USE_PLANAR_BUFFERS__
            // current frame = offset + i
            // current channel = j
            // sample = buffer size * j + offset + i
            // "+offset" taken care of in initialization
            // "+i" taken care of by incrementing below
            output[buffer_length * j] = m_dac->m_multi_chan[j]->m_current_v[i];
            #else
            output[j] = m_dac->m_multi_chan[j]->m_current_v[i];
            #endif
        }

        // advance pointer
        #ifdef __CHUCK_USE_PLANAR_BUFFERS__
        output++;
        #else
        output += m_num_dac_channels;
        #endif
    }
}




//-----------------------------------------------------------------------------
// name: advance()
// desc: advance the shreduler
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::advance( t_CKINT N )
{
    // advance system 'now'
    this->now_system += 1;

    // tick the dac; TODO: unhardcoded frame size!
    SAMPLE sum = 0.0f;
    t_CKUINT i;
    // input and output
    #ifdef __CHUCK_USE_PLANAR_BUFFERS__
    const SAMPLE * input = vm_ref->input_ref() + N;
    SAMPLE * output = vm_ref->output_ref() + N;
    t_CKUINT buffer_length = vm_ref->most_recent_buffer_length();
    #else
    const SAMPLE * input = vm_ref->input_ref() + (N*m_num_adc_channels);
    SAMPLE * output = vm_ref->output_ref() + (N*m_num_dac_channels);
    #endif

    // INPUT: loop over channels
    for( i = 0; i < m_num_adc_channels; i++ )
    {
        // ge: switched order of lines 1.3.5.3
        m_adc->m_multi_chan[i]->m_last = m_adc->m_multi_chan[i]->m_current;
        #ifdef __CHUCK_USE_PLANAR_BUFFERS__
        // current frame = N
        // current channel = i
        // sample = buffer size * i + N
        // the +N is taken care of by initialization
        m_adc->m_multi_chan[i]->m_current = input[buffer_length * i] * m_adc->m_multi_chan[i]->m_gain * m_adc->m_gain;
        #else
        m_adc->m_multi_chan[i]->m_current = input[i] * m_adc->m_multi_chan[i]->m_gain * m_adc->m_gain;
        #endif
        m_adc->m_multi_chan[i]->m_time = this->now_system;
        sum += m_adc->m_multi_chan[i]->m_current;
    }
    m_adc->m_last = m_adc->m_current = sum / m_num_adc_channels;
    m_adc->m_time = this->now_system;

    // PROCESSING
    m_dac->system_tick( this->now_system );
    // OUTPUT
    for( i = 0; i < m_num_dac_channels; i++ )
    {
        #ifdef __CHUCK_USE_PLANAR_BUFFERS__
        // current frame = N
        // current channel = i
        // sample = buffer size * i + N
        // the +N is taken care of by initialization
        output[buffer_length * i] = m_dac->m_multi_chan[i]->m_current; // * .5f;
        #else
        output[i] = m_dac->m_multi_chan[i]->m_current; // * .5f;
        #endif
    }

    // suck samples
    m_bunghole->system_tick( this->now_system );
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get the next shred shreduled to run 'now'
//       NOTE if a shred is ready to run, it is taken off the ready-to-run list
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM_Shreduler::get( )
{
    // shreduler's wait to run list
    Chuck_VM_Shred * shred = shred_list;

    // check if list empty
    if( !shred )
    {
        // if empty we are done
        m_samps_until_next = -1;
        return NULL;
    }

    // check the front of the shred wait-to-run list; ready to run?
    if( shred->wake_time <= ( this->now_system + .5 ) )
    {
        // set beginning of list to next
        shred_list = shred->next;

        // sever links of ready-to-run shred
        shred->next = NULL;
        shred->prev = NULL;

        // if shred list is non-empty
        if( shred_list )
        {
            // set list head's prev pointer to NULL
            shred_list->prev = NULL;
            // compute new samps until next
            m_samps_until_next = shred_list->wake_time - this->now_system;
            // clamp to 0
            if( m_samps_until_next < 0 ) m_samps_until_next = 0;
        }

        // return
        return shred;
    }

    // nothing is ready to run 'now'
    return NULL;
}




//-----------------------------------------------------------------------------
// name: highest()
// desc: get ID of shred currently with the highest ID
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM_Shreduler::highest() const
{
    // vessel for shred pointers
    std::vector<Chuck_VM_Shred *> shreds;
    // highest so far
    t_CKUINT n = 0;

    // get all shreds
    get_all_shreds( shreds );

    // loop
    for( t_CKUINT i = 0; i < shreds.size(); i++ )
    {
        // compare
        if( shreds[i]->get_id() > n ) n = shreds[i]->get_id();
    }

    return n;
}




//-----------------------------------------------------------------------------
// name: replace()
// desc: replace a shred in the shreduler with a new shred
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
// desc: remove a shred from the shreduler
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
// name: lookup()
// desc: look up a shred by ID
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM_Shreduler::lookup( t_CKUINT xid ) const
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
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::const_iterator iter;
    for( iter = blocked.begin(); iter != blocked.end(); iter++ )
    {
        shred = (*iter).second;
        if( shred->xid == xid )
            return shred;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name: get_ready_shreds()
// desc: retrieve the ready list of shreds in the shreduler
//
// NOTE shreds on the shreduler's ready list are sorted by
//      wake-up time (e.g., as specifiedy by `second => now`)
// NOTE the ready list DO NOT include shreds waiting on Events
//      waiting shreds are on the shreduler's blocked list
// NOTE the ready list does not include the shred currently
//      executing in the VM
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_ready_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                                           t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shreds.clear();

    // shred pointer
    Chuck_VM_Shred * shred = shred_list;

    // traverse shred list
    while( shred )
    {
        // add
        shreds.push_back( shred );
        // next
        shred = shred->next;
    }
}




//-----------------------------------------------------------------------------
// name: get_ready_shred_ids() | 1.5.0.8 (ge) added
// desc: retrieve list of ready shred IDs
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_ready_shred_ids( std::vector<t_CKUINT> & shredIDs,
                                              t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shredIDs.clear();

    // list for shreds
    std::vector<Chuck_VM_Shred *> shreds;
    // get shreds
    get_ready_shreds( shreds );

    // traverse shred list
    for( t_CKUINT i = 0; i < shreds.size(); i++ )
    {
        // append id
        shredIDs.push_back( shreds[i]->xid );
    }
}




//-----------------------------------------------------------------------------
// name: get_blocked_shreds() | 1.5.0.8 (ge) added
// desc: retrieve the list of active shreds in the shreduler
//
// NOTE active shreds are on the shreduler's ready list, sorted by
//      wake-up time (e.g., as specifiedy by `second => now`)
// NOTE active shreds list DO NOT include shreds waiting on Events
//      such shreds are on the shreduler's blocked list
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_blocked_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                                             t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shreds.clear();

    // iterate through blocked list
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *>::const_iterator iter;
    for( iter = blocked.begin(); iter != blocked.end(); iter++ )
    {
        // append the shred pointer
        shreds.push_back( (*iter).second );
    }
}




//-----------------------------------------------------------------------------
// name: get_blocked_shred_ids() | 1.5.0.8 (ge) added
// desc: retrieve list of active shred IDs
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_blocked_shred_ids( std::vector<t_CKUINT> & shredIDs,
                                                t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shredIDs.clear();

    // list for shreds
    std::vector<Chuck_VM_Shred *> shreds;
    // get shreds
    get_blocked_shreds( shreds );

    // traverse shred list
    for( t_CKUINT i = 0; i < shreds.size(); i++ )
    {
        // append id
        shredIDs.push_back( shreds[i]->xid );
    }
}




//-----------------------------------------------------------------------------
// name: get_current_shred()
// desc: get currently executing shred
//       NOTE this can only be non-NULL during a Chuck_VM::compute() cycle
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM_Shreduler::get_current_shred() const
{
    return m_current_shred;
}




//-----------------------------------------------------------------------------
// name: get_active_get_all_shredsshreds() | 1.5.0.8 (ge) added
// desc: retrieve the list of active shreds in the shreduler
//
// NOTE active shreds are on the shreduler's ready list, sorted by
//      wake-up time (e.g., as specifiedy by `second => now`)
// NOTE active shreds list DO NOT include shreds waiting on Events
//      such shreds are on the shreduler's blocked list
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_all_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                                         t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shreds.clear();

    // get ready list (FALSE == don't clear vector)
    get_ready_shreds( shreds, FALSE );

    // get blocked list (FALSE == don't clear vector)
    get_blocked_shreds( shreds, FALSE );

    // add currently executing shred, if not NULL
    // (if not NULL, implies this is called from within VM.compute())
    Chuck_VM_Shred * shred = get_current_shred();
    if( shred ) shreds.push_back( shred );
}




//-----------------------------------------------------------------------------
// name: get_all_shred_ids() | 1.5.0.8 (ge) added
// desc: retrieve list of active shred IDs
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::get_all_shred_ids( std::vector<t_CKUINT> & shredIDs,
                                            t_CKBOOL clearVector ) const
{
    // clear; if not clear, then will append to existing contents
    if( clearVector ) shredIDs.clear();

    // list for shreds
    std::vector<Chuck_VM_Shred *> shreds;
    // get shreds
    get_all_shreds( shreds );

    // traverse shred list
    for( t_CKUINT i = 0; i < shreds.size(); i++ )
    {
        // append id
        shredIDs.push_back( shreds[i]->xid );
    }
}




//-----------------------------------------------------------------------------
// name: status()
// desc: get VM shreduler status
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::status( Chuck_VM_Status * status )
{
    Chuck_VM_Shred * shred = shred_list;

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

    // a vessel for shred pointers
    vector<Chuck_VM_Shred *> list;
    // get all shreds
    get_all_shreds( list );

    // sort the list
    SortByID_LT byid;
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
// desc: output/print status
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::status()
{
    Chuck_VM_Shred_Status * shred = NULL;

    this->status( &m_status );
    t_CKUINT h = m_status.t_hour;
    t_CKUINT m = m_status.t_minute;
    t_CKUINT sec = m_status.t_second;
    EM_print2magenta( "(VM) status | # of shreds in VM: %ld", m_status.list.size() );
    EM_print2vanilla( "chuck time: %.0f::samp (%ldh%ldm%lds)", m_status.now_system, h, m, sec );
    EM_print2vanilla( "earth time: %s", timestamp_formatted().c_str() );

    // print status
    if( m_status.list.size() ) EM_print2vanilla( "--------" );
    for( t_CKUINT i = 0; i < m_status.list.size(); i++ )
    {
        shred = m_status.list[i];
        EM_print2vanilla(
            "[shred id]: %ld [source]: %s [spork time]: %.2fs ago%s",
            shred->xid, mini( shred->name.c_str() ),
            (m_status.now_system - shred->start) / m_status.srate,
            shred->has_event ? " (blocked)" : "" );
    }
    if( m_status.list.size() ) EM_print2vanilla( "--------" );
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Status()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Status::Chuck_VM_Status()
{
    srate = 0;
    now_system = 0;
    t_second = t_minute = t_hour = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Status()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_VM_Status::~Chuck_VM_Status()
{
    this->clear();
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: clear VM status
//-----------------------------------------------------------------------------
void Chuck_VM_Status::clear()
{
    for( t_CKUINT i = 0; i < list.size(); i++ )
    {
        CK_SAFE_DELETE( list[i] );
    }

    list.clear();
}




// begin CK_VM_DEBUG implementation
//-----------------------------------------------------------------------------
#if CK_VM_DEBUG_ENABLE
//-----------------------------------------------------------------------------
// Chuck_VM_Debug implementation | 1.5.0.5 (ge) added
//-----------------------------------------------------------------------------
// static member
Chuck_VM_Debug * Chuck_VM_Debug::our_instance = NULL;
// internal macro | automatically use our log level
#define DEBUG_LOG( ... ) do{ CK_LOG( m_log_level, __VA_ARGS__ ); } while(0)




//-----------------------------------------------------------------------------
// name: info_backtrace()
// desc: get call stack as string
//-----------------------------------------------------------------------------
std::string Chuck_VM_Debug::info_backtrace()
{
    // return ck_backtrace();
    return "[not available]";
}


//-----------------------------------------------------------------------------
// name: backtrace()
// desc: print call stack as string
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::backtrace( const std::string & note )
{
    CK_FPRINTF_STDERR( "---- stack trace (%s) ----\n", note.c_str() );
    CK_FPRINTF_STDERR( "%s", info_backtrace().c_str() );
    CK_FPRINTF_STDERR( "--------------------------\n" );
}


//-----------------------------------------------------------------------------
// name: Chuck_VM_Debug()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Debug::Chuck_VM_Debug()
{
    // default log level
    m_log_level = CK_LOG_DEBUG;

    // stats
    reset_stats();
}


//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Debug()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_VM_Debug::~Chuck_VM_Debug()
{
    // TODO clean up map
}


//-----------------------------------------------------------------------------
// name: instance()
// desc: return static instance
//-----------------------------------------------------------------------------
Chuck_VM_Debug * Chuck_VM_Debug::instance()
{
    // if no instance
    if( !our_instance )
    {
        // allocate
        our_instance = new Chuck_VM_Debug();
    }

    return our_instance;
}


//-----------------------------------------------------------------------------
// call this right after instantiation (tracking)
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::construct( Chuck_VM_Object * obj, const std::string & note )
{
    // check
    if( !obj )
    {
        DEBUG_LOG( "CONSTRUCT(%s): *** NULL OBJECT ***" );
        return;
    }

    // log it
    DEBUG_LOG( "%s(%s): %s", TC::color(TC::FG_MAGENTA,"CONSTRUCT",true).c_str(), note.c_str(), this->info(obj).c_str() );
    // count
    m_numConstructed++;
    // FYI not adding to map here, constructor won't have type info beyond Chuck_VM_Object
}


//-----------------------------------------------------------------------------
// call this right before destruct (tracking)
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::destruct( Chuck_VM_Object * obj, const std::string & note )
{
    // check
    if( !obj )
    {
        DEBUG_LOG( "DESTRUCT(%s): *** NULL OBJECT ***" );
        return;
    }

    // log it
    DEBUG_LOG( "%s(%s): %s", TC::color(TC::FG_YELLOW,"DESTRUCT",true).c_str(), note.c_str(), this->info(obj).c_str() );
    // count
    m_numDestructed++;
    // remove it from map
    remove( obj );
}


//-----------------------------------------------------------------------------
// name: add_ref()
// desc: add reference
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::add_ref( Chuck_VM_Object * obj, const std::string & note )
{
    // check
    if( !obj )
    {
        DEBUG_LOG( "ADD_REF(%s): *** NULL OBJECT ***" );
        return;
    }

    // log it
    DEBUG_LOG( "%s(%s): %s", TC::green("ADD_REF",true).c_str(), note.c_str(), this->info(obj).c_str() );
    // count
    m_numAddRefs++;
    // add it to map (constructor won't have type info beyond Chuck_VM_Object)
    insert( obj );
}


//-----------------------------------------------------------------------------
// name: release()
// desc: release reference
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::release( Chuck_VM_Object * obj, const std::string & note )
{
    // check
    if( !obj )
    {
        DEBUG_LOG( "RELEASE(%s): *** NULL OBJECT ***" );
        return;
    }

    DEBUG_LOG( "%s(%s): %s", TC::orange("RELEASE",true).c_str(), note.c_str(), this->info(obj).c_str() );
    // count
    m_numReleases++;
}


//-----------------------------------------------------------------------------
// print stats
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::print_stats()
{
    DEBUG_LOG( TC::blue("VM DEBUG: total constructed: %ld",true).c_str(), m_numConstructed );
    DEBUG_LOG( TC::blue("VM DEBUG: total destructed: %ld",true).c_str(), m_numDestructed );
    DEBUG_LOG( TC::blue("VM DEBUG: total add refs: %ld",true).c_str(), m_numAddRefs );
    DEBUG_LOG( TC::blue("VM DEBUG: total release refs: %ld",true).c_str(), m_numReleases );
}


//-----------------------------------------------------------------------------
// reset stats
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::reset_stats()
{
    // stats
    m_numConstructed = 0;
    m_numDestructed = 0;
    m_numAddRefs = 0;
    m_numReleases = 0;
}


//-----------------------------------------------------------------------------
// set log level to print to
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::set_log_evel( t_CKUINT level )
{
    m_log_level = level;
}


//-----------------------------------------------------------------------------
// one func to try to print them all
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::print( Chuck_VM_Object * obj, const std::string & note )
{
    DEBUG_LOG( "%s%s", (note != "" ? note +" " : " ").c_str(), info(obj).c_str() );
}


//-----------------------------------------------------------------------------
// one func to get info using runtime types
//-----------------------------------------------------------------------------
std::string Chuck_VM_Debug::info( Chuck_VM_Object * obj )
{
    // check
    if( !obj ) return "[NULL info]";

    // get the type
    string type = mini_type(typeid(*obj).name());
    // info str [POINTER:REFCOUNT](TYPE)
    string ret = string("[") + ptoa(obj) + ":" + TC::blue(itoa(obj->refcount()),true) + "](" + TC::green(type,true) + "): ";

    // match on type name
    if( type == "Chuck_Object" ) ret += info_obj( (Chuck_Object *)obj );
    else if( type == "Chuck_Type" ) ret += info_type( (Chuck_Type *)obj );
    else if( type == "Chuck_Func") ret += info_func( (Chuck_Func *)obj );
    else if( type == "Chuck_Value") ret += info_value( (Chuck_Value *)obj );
    else if( type == "Chuck_Namespace") ret += info_namespace( (Chuck_Namespace *)obj );
    else if( type == "Chuck_Context") ret += info_context( (Chuck_Context *)obj );
    else if( type == "Chuck_Env") ret += info_env( (Chuck_Env *)obj );
    else if( type == "Chuck_UGen_Info") ret += info_ugen_info( (Chuck_UGen_Info *)obj );
    else if( type == "Chuck_VM_Code") ret += info_code( (Chuck_VM_Code *)obj );
    else if( type == "Chuck_VM_Shred") ret += info_shred( (Chuck_VM_Shred *)obj );
    else if( type == "Chuck_VM") ret += info_vm( (Chuck_VM *)obj );

    // no match
    return ret;
}


//-----------------------------------------------------------------------------
// info by type
//-----------------------------------------------------------------------------
std::string Chuck_VM_Debug::info_obj( Chuck_Object * obj )
{
    if( !obj ) return "[NULL obj]";
    return string("Object of type:") + info_type(obj->type_ref);
}

std::string Chuck_VM_Debug::info_type( Chuck_Type * type )
{
    if( !type ) return "[NULL type]";

    // construct value string
    string s = string("'") + type->name() + "'";

    // namescpace
    // if( type->owner ) s += info_namespace( type->owner ) + " ";
    // function?
    if( type->func ) s += info_func( type->func );

    // backtrace();
    return s;
}

std::string Chuck_VM_Debug::info_func( Chuck_Func * func )
{
    if( !func ) return "[NULL func]";
    return func->base_name + "() " + "symbol: '" + func->name + "'";
}

std::string Chuck_VM_Debug::info_value( Chuck_Value * v )
{
    // check
    if( !v ) return "[NULL value]";
    // construct value string
    string s;

    // type
    s += info_type(v->type) + " ";
    // name space
    if( v->owner ) s += info_namespace(v->owner) + ".";
    // value name
    s += v->name + "; ";
    // part of a class?
    // if( v->owner_class ) s += info_type(v->owner_class);

    return s;
}

std::string Chuck_VM_Debug::info_namespace( Chuck_Namespace * nspc )
{
    if( !nspc ) return "[NULL namespace]";
    return nspc->name;
}

std::string Chuck_VM_Debug::info_context( Chuck_Context * context )
{
    if( !context ) return "[NULL context]";

    // return string
    string s = string("'") + context->filename + "' ";
    // public class def set?
    s += string("public-class-def: ") + (context->public_class_def ? "SET" : "EMPTY");

    return s;

}

std::string Chuck_VM_Debug::info_env( Chuck_Env * env )
{
    if( !env ) return "[NULL env]";
    return "";
}

std::string Chuck_VM_Debug::info_ugen_info( Chuck_UGen_Info * ug )
{
    if( !ug ) return "[NULL ugen_info]";
    return "";
}

std::string Chuck_VM_Debug::info_code( Chuck_VM_Code * code )
{
    if( !code ) return "[NULL code]";
    return code->name;

}

std::string Chuck_VM_Debug::info_shred( Chuck_VM_Shred * shred )
{
    if( !shred ) return "[NULL shred]";
    return shred->name;
}

std::string Chuck_VM_Debug::info_vm( Chuck_VM * vm )
{
    if( !vm ) return "[NULL vm]";
    return "";
}

// insert into object map
void Chuck_VM_Debug::insert( Chuck_VM_Object * obj )
{
    // map key
    string key = mini_type(typeid(*obj).name());

    // insert
    m_objects_map[key][obj] = obj;
}

// remove from object map
void Chuck_VM_Debug::remove( Chuck_VM_Object * obj )
{
    // map key
    string key = mini_type(typeid(*obj).name());
    // check
    if( !contains(obj) ) return;
    // remove
    m_objects_map[key].erase( obj );
}

// is present?
t_CKBOOL Chuck_VM_Debug::contains( Chuck_VM_Object * obj )
{
    // map key
    string key = mini_type(typeid(*obj).name());
    // find it
    return m_objects_map.find( key ) != m_objects_map.end() &&
           m_objects_map[key].find(obj) != m_objects_map[key].end();
}

//-----------------------------------------------------------------------------
// number of objects being tracked
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM_Debug::num_objects()
{
    // outer iter
    std::map< std::string, std::map<Chuck_VM_Object *, Chuck_VM_Object *> >::iterator maps = m_objects_map.begin();
    // count
    t_CKUINT count = 0;

    // iterate
    for( ; maps != m_objects_map.end(); maps++ )
    {
        // count
        count += maps->second.size();
    }

    return count;
}

//-----------------------------------------------------------------------------
// compare functin for Chuck_VM_Object *
//-----------------------------------------------------------------------------
bool vm_obj_cmp( Chuck_VM_Object * lhs, Chuck_VM_Object * rhs )
{
    if( !lhs || !rhs ) return false;
    return lhs->refcount() > rhs->refcount();
}

//-----------------------------------------------------------------------------
// print all objects being tracked
//-----------------------------------------------------------------------------
void Chuck_VM_Debug::print_all_objects()
{
    // outer iter
    std::map< std::string, std::map<Chuck_VM_Object *, Chuck_VM_Object *> >::iterator maps = m_objects_map.begin();
    // vector
    vector<Chuck_VM_Object *> vec;

    // iterate
    for( ; maps != m_objects_map.end(); maps++ )
    {
        // print
        DEBUG_LOG( "%s (%d)", TC::blue(maps->first.c_str(),true).c_str(), maps->second.size() );
        // push
        EM_pushlog();
        // inner
        std::map<Chuck_VM_Object *, Chuck_VM_Object *>::iterator objs = maps->second.begin();
        // clear
        vec.clear();
        // iterate
        for( ; objs != maps->second.end(); objs++ )
        {
            // push vector
            vec.push_back( objs->second );
        }
        // sort
        sort( vec.begin(), vec.end(), vm_obj_cmp );
        // print
        for( t_CKINT i = 0; i < vec.size(); i++ )
        {
            print( vec[i] );
        }
        // pop
        EM_poplog();
    }
}

#endif // #if CK_VM_DEBUG_ENABLE
