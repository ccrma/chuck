/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

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
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_bbq.h"
#include "chuck_errmsg.h"

#include <vector>
using namespace std;

#if defined(__WINDOWS_DS__) && !defined(__WINDOWS_PTHREAD__)
  #include <windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif



//-----------------------------------------------------------------------------
// name: class Chuck_VM_Frame
// desc: func frame
//-----------------------------------------------------------------------------
class Chuck_VM_Frame
{
public:
    t_CKUINT size;

public:
    Chuck_VM_Frame() { size = 0; }
    ~Chuck_VM_Frame() { }
};




//-----------------------------------------------------------------------------
// name: class Chuck_VM_Func
// desc: vm function
//-----------------------------------------------------------------------------
class Chuck_VM_Func
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
// name: class Chuck_VM_FTable
// desc: function table
//-----------------------------------------------------------------------------
class Chuck_VM_FTable
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
    m_shreds = NULL;
    m_num_shreds = 0;
    m_shreduler = NULL;
    m_msg_buffer = NULL;
    m_shred_id = 0;
    m_halt = TRUE;
    m_env = NULL;
    m_audio = FALSE;

    m_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM::~Chuck_VM()
{
    if( m_init ) shutdown();
}


// dac tick
UGEN_TICK __dac_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out ) 
{ *out = in; return TRUE; }
UGEN_TICK __bunghole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{ *out = 0.0f; return TRUE; }


// static
#ifdef __MACOSX_CORE__
t_CKINT Chuck_VM::our_priority = 95;
#else
t_CKINT Chuck_VM::our_priority = 0x7fffffff;
#endif


#ifndef __WINDOWS_DS__
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::set_priority( t_CKINT priority, Chuck_VM * vm )
{
    struct sched_param param;
    pthread_t tid = pthread_self();
    int policy;
    // get for thread
    if( pthread_getschedparam( tid, &policy, &param) ) 
    {
        if( vm )
            vm->m_last_error = "could not get current scheduling parameters";
        return FALSE;
    }

    // priority
    param.sched_priority = priority;
    // policy
    policy = SCHED_RR;
    // set for thread
    if( pthread_setschedparam( tid, policy, &param ) )
    {
        if( vm )
            vm->m_last_error = "could not set new scheduling parameters";
        return FALSE;
    }
    
    return TRUE;
}
#else
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::set_priority( t_CKINT priority, Chuck_VM * vm )
{
    // if priority is 0, then do nothing
    if( !priority )
        return TRUE;

    // set the priority class of the process
    // if( !SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS ) )
    //     return FALSE;

    // set the priority the thread
    if( !SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL ) )
    {
        if( vm )
            vm->m_last_error = "could not set new scheduling parameters";
        return FALSE;
    }

    return TRUE;
}
#endif



        
//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::initialize( t_CKBOOL enable_audio, t_CKBOOL halt, t_CKUINT srate,
                               t_CKUINT buffer_size, t_CKUINT num_buffers,
                               t_CKUINT dac, t_CKUINT adc, t_CKINT priority )
{
    if( m_init )
    {
        m_last_error = "VM already initialized!";
        return FALSE;
    }

//#ifndef __WINDOWS_DS__
    // boost thread priority
    if( priority != 0x7fffffff && !set_priority( priority, this ) )
        return FALSE;
//#endif

    // allocate bbq
    m_bbq = new BBQ;
    m_halt = halt;
    m_audio = enable_audio;
    
    // allocate shreduler
    m_shreduler = new Chuck_VM_Shreduler;
    m_shreduler->bbq = m_audio ? m_bbq : NULL;

    // allocate msg buffer
    m_msg_buffer = new CBuffer;
    m_msg_buffer->initialize( 1024, sizeof(Chuck_Msg *) );
    m_reply_buffer = new CBuffer;
    m_reply_buffer->initialize( 1024, sizeof(Chuck_Msg *) );

    // allocate dac and adc
    m_num_dac_channels = 2;
    m_dac = new Chuck_UGen[2];
    m_dac[0].tick = __dac_tick;
    m_dac[1].tick = __dac_tick;
    m_num_adc_channels = 2;
    m_adc = new Chuck_UGen[2];
    m_bunghole = new Chuck_UGen;
    m_dac[0].add_ref(); m_dac[1].add_ref();
    m_adc[0].add_ref(); m_adc[1].add_ref();
    m_bunghole->add_ref();
    m_shreduler->m_dac = m_dac;
    m_shreduler->m_adc = m_adc;
    m_shreduler->m_bunghole = m_bunghole;
    m_shreduler->m_num_dac_channels = m_num_dac_channels;
    m_shreduler->m_num_adc_channels = m_num_adc_channels;

    if( m_audio )
    {
        // init bbq
        if( !m_bbq->initialize( 2, srate, 16, buffer_size, num_buffers, dac, adc ) )
        {
            m_last_error = "cannot initialize audio device (try using --silent/-s)";
            return FALSE;
        }
    }
    else
    {
        // at least set the sample rate and buffer size
        m_bbq->set_srate( srate );
        m_bbq->set_bufsize( buffer_size );
    }
    
    // m_bbq->midi_out()->open( 0 );
    // m_bbq->midi_in()->open( 0 );

    return m_init = TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::shutdown()
{
    // make sure we are in the initialized state
    if( !m_init ) return FALSE;

    // stop
    this->stop();
    usleep( 50000 );
    
    // dll
    this->dll_clear();

    // free the shreduler
    SAFE_DELETE( m_shreduler );

    // terminate shreds
    Chuck_VM_Shred * curr = m_shreds, * prev = NULL;
    while( curr )
    {
        prev = curr;
        curr = curr->next;
        // delete prev;
    }
    m_shreds = NULL;
    m_num_shreds = 0;

    // shutdown audio
    if( m_audio )
    {
        m_bbq->digi_out()->cleanup();
        m_bbq->digi_in()->cleanup();
        m_bbq->shutdown();
        SAFE_DELETE( m_bbq );
        m_audio = FALSE;
    }

    m_init = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: run()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::run( )
{
    m_running = TRUE;
    Chuck_VM_Shred * shred = NULL;
    Chuck_Msg * msg = NULL;
    
    // audio
    if( m_audio )
    {
        if( !m_bbq->digi_out()->initialize( ) )
        {
            m_last_error = "cannot open audio output (option: use --silent/-s)";
            return FALSE;
        }

        m_bbq->digi_in()->initialize( );
        m_bbq->digi_out()->start();
        m_bbq->digi_in()->start();
    }

    while( m_running )
    {
        // get the shreds queued for 'now'
        while( shred = m_shreduler->get() )
        {
            // set the current time of the shred
            shred->now = shred->wake_time;

            // run the shred
            if( !shred->run( this ) )
            {
                this->free( shred, TRUE );
                shred = NULL;
                if( !m_num_shreds && m_halt ) goto vm_stop;
            }
        }

        // advance the shreduler
        m_shreduler->advance();

        // process messages
        while( m_msg_buffer->get( &msg, 1 ) )
            process_msg( msg );
    }

// vm stop here
vm_stop:
    m_running = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: pause()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::pause( )
{
    m_running = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::stop( )
{
    m_running = FALSE;
    Digitalio::m_end = TRUE;

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
    m_msg_buffer->put( &msg, count );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_reply()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Msg * Chuck_VM::get_reply( )
{
    Chuck_Msg * msg = NULL;
    m_reply_buffer->get( &msg, 1 );
    return msg;
}




//-----------------------------------------------------------------------------
// name: process_msg()
// desc: ...
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
        }
        // set the current time
        shred->start = m_shreduler->now_system;
        // set the id
        shred->id = msg->param;

        // replace
        if( m_shreduler->replace( out, shred ) )
        {
            EM_error3( "[chuck](VM): replacing shred %i (%s) with %i (%s)...",
                       out->id, mini(out->name.c_str()), shred->id, mini(shred->name.c_str()) );
            this->free( out, TRUE, FALSE );
            retval = shred->id;
            goto done;
        }
        else
        {
            EM_error3( "[chuck](VM): shreduler ERROR replacing shred %i...",
                       out->id );
            delete shred;
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

            t_CKUINT id = m_shred_id;
            Chuck_VM_Shred * shred = NULL;
            while( id >= 0 && m_shreduler->remove( shred = m_shreduler->lookup( id ) ) == 0 )
                id--;
            if( id >= 0 )
            {
                EM_error3( "[chuck](VM): removing recent shred: %i (%s)...", 
                           id, mini(shred->name.c_str()) );
                this->free( shred, TRUE );
                retval = id;
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
            if( !m_shreduler->remove( m_shreduler->lookup( msg->param ) ) )
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
        t_CKUINT id = m_shred_id;
        EM_error3( "[chuck](VM): removing all (%i) shreds...", m_num_shreds );
        Chuck_VM_Shred * shred = NULL;

        while( m_num_shreds && id >= 0 )
        {
            if( m_shreduler->remove( shred = m_shreduler->lookup( id ) ) )
                delete shred;
            id--;
        }
        
        m_shred_id = 0;
        m_num_shreds = 0;
    }
    else if( msg->type == MSG_ADD )
    {
        t_CKUINT id = 0;
        if( msg->shred ) id = this->spork( msg->shred )->id;
        else id = this->spork( msg->code, NULL )->id;
        
        const char * s = ( msg->shred ? msg->shred->name.c_str() : msg->code->name.c_str() );
        EM_error3( "[chuck](VM): sporking incoming shred: %i (%s)...", id, mini(s) );
        retval = id;
        goto done;
    }
    else if( msg->type == MSG_KILL )
    {
        EM_error3( "[chuck](VM): KILL received...." );
        exit( 1 );
    }
    else if( msg->type == MSG_STATUS )
    {
        m_shreduler->status();
    }
    else if( msg->type == MSG_TIME )
    {
        float srate = (float)Digitalio::sampling_rate();
        fprintf( stderr, "[chuck](VM): the values of now:\n" );
        fprintf( stderr, "  now = %i (samp)\n", m_shreduler->now_system );
        fprintf( stderr, "      = %.6f (second)\n", m_shreduler->now_system / srate );
        fprintf( stderr, "      = %.6f (minute)\n", m_shreduler->now_system / srate / 60.0f );
        fprintf( stderr, "      = %.6f (hour)\n", m_shreduler->now_system / srate / 60.0f / 60.0f );
        fprintf( stderr, "      = %.6f (day)\n", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f );
        fprintf( stderr, "      = %.6f (week)\n", m_shreduler->now_system / srate / 60.0f / 60.0f / 24.0f / 7.0f );
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
// name: bbq()
// desc: ...
//-----------------------------------------------------------------------------
BBQ * Chuck_VM::bbq( ) const
{
    return m_bbq;
}




//-----------------------------------------------------------------------------
// name: srate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM::srate() const
{
    return (t_CKUINT)Digitalio::sampling_rate();
}




//-----------------------------------------------------------------------------
// name: fork()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM::fork( Chuck_VM_Code * code )
{
    return NULL;
}




//-----------------------------------------------------------------------------
// name: spork()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shred * Chuck_VM::spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent )
{
    // allocate a new shred
    Chuck_VM_Shred * shred = new Chuck_VM_Shred;
    // initialize the shred (default stack size)
    shred->initialize( code );
    // set the name
    shred->name = code->name;
    // set the parent
    shred->parent = parent;
    // spork it
    this->spork( shred );

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
    shred->id = next_id();
    // add it to the parent
    if( shred->parent )
        shred->parent->children[shred->id] = shred;
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
        shred->parent->children.erase( shred->id );

    // free!
    m_shreduler->remove( shred );
    SAFE_DELETE( shred );
    if( dec ) m_num_shreds--;
    if( !m_num_shreds ) m_shred_id = 0;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: dll_load()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DLL * Chuck_VM::dll_load( const char * path, const char * id )
{
    Chuck_DLL * dll = m_dlls[path];
    if( !dll )
    {
        dll = new Chuck_DLL(id);
        if( !dll ) return NULL;

        // load the dll
        if( !dll->load( path ) )
        {
            m_last_error = dll->last_error();
            delete dll;
            return NULL;
        }

        m_dlls[path] = dll;
    }

    return dll;
}




//-----------------------------------------------------------------------------
// name: dll_unload()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM::dll_unload( Chuck_DLL *& dll )
{
    if( !dll ) return TRUE;

    // deallocate
    dll->unload();
    delete dll;
    dll = NULL;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: dll_clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM::dll_clear()
{
    map<string, Chuck_DLL *>::iterator iter;
    for( iter = m_dlls.begin(); iter != m_dlls.end(); iter++ )
    {
        (*iter).second->unload();
        delete (*iter).second;
    }
    
    m_dlls.clear();
}




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
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Code()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Code::~Chuck_VM_Code()
{
    if( instr )
        delete [] instr;

    num_instr = 0;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Stack::initialize( t_CKUINT size )
{
    if( m_is_init )
        return FALSE;

    // allocate stack
    stack = new t_CKBYTE[size];
    if( !stack )
        return FALSE;
        
    // zero
    memset( stack, 0, size );

    // set the sp
    sp = stack;
    sp_max = sp + size;

    // set flag and return
    return m_is_init = TRUE;
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
    code = NULL;
    next = prev = NULL;
    instr = NULL;
    parent = NULL;
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
    code = c;
    // shred done
    is_done = FALSE;
    // shred running
    is_running = FALSE;
    // set the instr
    instr = c->instr;
    // zero out the id
    id = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::shutdown()
{
    // get iterator to our map
    map<Chuck_UGen *, Chuck_UGen *>::iterator iter = m_ugen_map.begin();
    while( iter != m_ugen_map.end() )
    {
        (*iter).first->disconnect( TRUE );
        iter++;
    }
    m_ugen_map.clear();

    SAFE_DELETE( mem );
    SAFE_DELETE( reg );

    // TODO: is this right?
    code = NULL;
    // what to do with next and prev?

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::add( Chuck_UGen * ugen )
{
    if( m_ugen_map[ugen] )
        return FALSE;

    m_ugen_map[ugen] = ugen;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: remove()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shred::remove( Chuck_UGen * ugen )
{
    if( !m_ugen_map[ugen] )
        return FALSE;
    
    // remove it
    m_ugen_map.erase( ugen );
    return TRUE;
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

    // go!
    while( is_running )
    {
        // execute the instruction
        instr[pc]->execute( vm, this );

        // set to next_pc;
        pc = next_pc;
        next_pc++;
    }

    // is the shred finished
    return !is_done;
}




//-----------------------------------------------------------------------------
// name: Chuck_VM_Shreduler()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Shreduler::Chuck_VM_Shreduler()
{
    shred_list = NULL;
    now_system = 0;
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
        return FALSE;
    }

    // sanity check
    if( wake_time < this->now_system )
    {
        // trying to enqueue on a time that is less than now
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

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: advance()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::advance( )
{
    // advance system 'now'
    this->now_system += 1;

    // tick the dac
    SAMPLE l, r;
    BBQ * audio = this->bbq;

    // tick in
    if( audio )
    {
        audio->digi_in()->tick_in( &l, &r );
        m_adc[0].m_current = .5f * ( l + r ) * m_adc[0].m_gain;
        // m_adc[1].m_current = r * m_adc[1].m_gain;
    }

    // dac
    m_dac[0].system_tick( this->now_system );
    //m_dac[1].system_tick( this->now_system );
    l = m_dac[0].m_current;
    //r = m_dac[1].m_current;
    l *= .5f;

    // suck samples
    m_bunghole->system_tick( this->now_system );

    // tick
    if( audio )
        audio->digi_out()->tick_out( l, l );
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
        return NULL;

    // TODO: should this be <=?
    if( shred->wake_time <= ( this->now_system + .5 ) )
    {
        // if( shred->wake_time < this->now_system )
        //    assert( false );

        shred_list = shred->next;
        shred->next = NULL;
        shred->prev = NULL;
        
        if( shred_list )
            shred_list->prev = NULL;

        return shred;
    }

    return NULL;
}



//-----------------------------------------------------------------------------
// name: replace()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_VM_Shreduler::replace( Chuck_VM_Shred * out, Chuck_VM_Shred * in )
{
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
    // sanity check
    if( !out || ( !out->prev && !out->prev && out != shred_list ) )
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
Chuck_VM_Shred * Chuck_VM_Shreduler::lookup( t_CKUINT id )
{
    Chuck_VM_Shred * shred = shred_list;
    
    // list empty
    if( !shred )
        return NULL;
    
    while( shred )
    {
        if( shred->id == id )
            return shred;

        shred = shred->next;
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: status()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_VM_Shreduler::status( )
{
    Chuck_VM_Shred * shred = shred_list;

    t_CKUINT srate = Digitalio::sampling_rate();
    t_CKUINT s = (t_CKUINT)now_system;
    t_CKUINT h = s/(srate*3600);
    s = s - (h*(srate*3600));
    t_CKUINT m = s / (srate*60);
    s = s - (m*(srate*60));
    t_CKUINT sec = s / srate;
    s = s - (sec*(srate));
    float millisecond = s / (float)(srate) * 1000.0f;
    fprintf( stdout, "[chuck](VM): status (now == %ih%im%is, %.1f samps) ...\n",
             h, m, sec, now_system );
    
    char buffer[1024];
    while( shred )
    {
        char * s = buffer, * ss = buffer;
        strcpy( buffer, shred->name.c_str() );
        while( *s++ ) if( *s == '/' ) { ss = s+1; }
        
        fprintf( stdout, 
            "    [shred id]: %i  [source]: %s  [spork time]: %.2fs ago\n",
            shred->id, mini( shred->name.c_str() ),
            (now_system-shred->start)/(float)Digitalio::sampling_rate() );
        shred = shred->next;
    }
}
