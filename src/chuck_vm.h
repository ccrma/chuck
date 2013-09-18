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
// file: chuck_vm.h
// desc: chuck virtual machine
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_VM_H__
#define __CHUCK_VM_H__

#include "chuck_oo.h"
#include "chuck_ugen.h"

// tracking
#ifdef __CHUCK_STAT_TRACK__
#include "chuck_stats.h"
#endif

#include <string>
#include <map>
#include <vector>
#include <list>



#define CK_DEBUG_MEMORY_MGMT (0)

#if CK_DEBUG_MEMORY_MGMT
#define CK_MEMMGMT_TRACK(x) do{ x; } while(0)
#else
#define CK_MEMMGMT_TRACK(x)
#endif



//-----------------------------------------------------------------------------
// vm defines
//-----------------------------------------------------------------------------
#define CVM_MEM_STACK_SIZE          (0x1 << 16)
#define CVM_REG_STACK_SIZE          (0x1 << 14)


// forward references
struct Chuck_Instr;
struct Chuck_VM;
struct Chuck_VM_Func;
struct Chuck_VM_FTable;
struct Chuck_Msg;

class BBQ;
class CBufferSimple;
class Digitalio;




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Stack
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Stack
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    Chuck_VM_Stack();
    ~Chuck_VM_Stack();

public:
    t_CKBOOL initialize( t_CKUINT size );
    t_CKBOOL shutdown();

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public: // machine
    t_CKBYTE * stack;
    t_CKBYTE * sp;
    t_CKBYTE * sp_max;

public: // linked list
    Chuck_VM_Stack * prev;
    Chuck_VM_Stack * next;

public: // state
    t_CKBOOL m_is_init;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Code
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Code : Chuck_Object
{
public:
    Chuck_VM_Code();
    ~Chuck_VM_Code();

public:
    // array of Chuck_Instr *, should always end with Chuck_Instr_EOF
    Chuck_Instr ** instr;
    // size of the array
    t_CKUINT num_instr;

    // name of this code
    std::string name;
    // the depth of any function arguments
    t_CKUINT stack_depth;
    // whether the function needs 'this' pointer or not
    t_CKBOOL need_this;
    // native
    t_CKUINT native_func;
    // is ctor?
    t_CKUINT native_func_type;
    
    // filename this code came from (added 1.3.0.0)
    std::string filename;

    // native func types
    enum { NATIVE_UNKNOWN, NATIVE_CTOR, NATIVE_DTOR, NATIVE_MFUN, NATIVE_SFUN };
};


struct Chuck_IO_Serial;


//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Shred
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Shred : Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    Chuck_VM_Shred( );
    ~Chuck_VM_Shred( );

    t_CKBOOL initialize( Chuck_VM_Code * c, 
                         t_CKUINT mem_st_size = CVM_MEM_STACK_SIZE, 
                         t_CKUINT reg_st_size = CVM_REG_STACK_SIZE );
    t_CKBOOL shutdown();
    t_CKBOOL run( Chuck_VM * vm );
    t_CKBOOL add( Chuck_UGen * ugen );
    t_CKBOOL remove( Chuck_UGen * ugen );
    
    // add parent object reference (added 1.3.1.2)
    t_CKVOID add_parent_ref( Chuck_Object * obj );
    
    // HACK - spencer (added 1.3.2.0)
    // add/remove SerialIO devices to close on shred exit
    t_CKVOID add_serialio( Chuck_IO_Serial * serial );
    t_CKVOID remove_serialio( Chuck_IO_Serial * serial );
    
//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public: // machine components
    // stacks
    Chuck_VM_Stack * mem;
    Chuck_VM_Stack * reg;

    // ref to base stack - if this is the root, then base is mem
    Chuck_VM_Stack * base_ref;

    // code
    Chuck_VM_Code * code;
    Chuck_VM_Code * code_orig; // the one to release
    Chuck_Instr ** instr;
    Chuck_VM_Shred * parent;
    std::map<t_CKUINT, Chuck_VM_Shred *> children;
    t_CKUINT pc;

    // vm
    Chuck_VM * vm_ref;

    // time
    t_CKTIME now;
    t_CKTIME start;

    // state (no longer needed, see array_alloc)
    // t_CKUINT * obj_array;
    // t_CKUINT obj_array_size;

public:
    t_CKTIME wake_time;
    t_CKUINT next_pc;
    t_CKBOOL is_done;
    t_CKBOOL is_running;
    t_CKBOOL is_abort;
    t_CKBOOL is_dumped;
    Chuck_Event * event;  // event shred is waiting on
    std::map<Chuck_UGen *, Chuck_UGen *> m_ugen_map;
    // references kept by the shred itself (e.g., when sporking member functions)
    // to be released when shred is done -- added 1.3.1.2
    std::vector<Chuck_Object *> m_parent_objects;

public: // id
    t_CKUINT xid;
    std::string name;
    std::vector<std::string> args;

public:
    Chuck_VM_Shred * prev;
    Chuck_VM_Shred * next;

    // tracking
    CK_TRACK( Shred_Stat * stat );
    
private:
    std::list<Chuck_IO_Serial *> * m_serials;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Shred_Status
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Shred_Status : Chuck_Object
{
public:
    t_CKUINT xid;
    std::string name;
    t_CKTIME start;
    t_CKBOOL has_event;    

public:
    Chuck_VM_Shred_Status( t_CKUINT _id, const std::string & n, t_CKTIME _start, t_CKBOOL e )
    {
        xid = _id;
        name = n;
        start = _start;
        has_event = e;
    }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Status
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Status : Chuck_Object
{
public:
    Chuck_VM_Status();
    ~Chuck_VM_Status();
    void clear();
    
public:
    // sample rate
    t_CKUINT srate;
    // now
    t_CKTIME now_system;
    // for display
    t_CKUINT t_second;
    t_CKUINT t_minute;
    t_CKUINT t_hour;
    // list of shred status
	std::vector<Chuck_VM_Shred_Status *> list;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Shreduler
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Shreduler : Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    Chuck_VM_Shreduler();
    ~Chuck_VM_Shreduler();

public:
    t_CKBOOL initialize();
    t_CKBOOL shutdown();

public: // shreduling
    t_CKBOOL shredule( Chuck_VM_Shred * shred );
    t_CKBOOL shredule( Chuck_VM_Shred * shred, t_CKTIME wake_time );
    Chuck_VM_Shred * get( );
    void advance( );
    void advance2( );
    void advance_v( t_CKINT & num_left );
    void set_adaptive( t_CKUINT max_block_size );

public: // high-level shred interface
    t_CKBOOL remove( Chuck_VM_Shred * shred );
    t_CKBOOL replace( Chuck_VM_Shred * out, Chuck_VM_Shred * in );
    Chuck_VM_Shred * lookup( t_CKUINT xid );
    void status( );
    void status( Chuck_VM_Status * status );
    t_CKUINT highest();

public: // for event related shred queue
    t_CKBOOL add_blocked( Chuck_VM_Shred * shred );
    t_CKBOOL remove_blocked( Chuck_VM_Shred * shred );

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public:
    // time and audio
    t_CKTIME now_system;
    t_CKBOOL rt_audio;
    BBQ * bbq;

    // shreds to be shreduled
    Chuck_VM_Shred * shred_list;
    // shreds waiting on events
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *> blocked;
    // current shred
    Chuck_VM_Shred * m_current_shred; // TODO: ref count?

    // ugen
    Chuck_UGen * m_dac;
    Chuck_UGen * m_adc;
    Chuck_UGen * m_bunghole;
    t_CKUINT m_num_dac_channels;
    t_CKUINT m_num_adc_channels;
    
    // status cache
    Chuck_VM_Status m_status;
    
    // max block size for adaptive block processing
    t_CKUINT m_max_block_size;
    t_CKBOOL m_adaptive;
    t_CKDUR m_samps_until_next;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM : Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    Chuck_VM();
    ~Chuck_VM();

public: // init
    t_CKBOOL initialize( t_CKBOOL enable_audio = TRUE, t_CKBOOL halt = TRUE,
                         t_CKUINT srate = 44100,
                         t_CKUINT buffer_size = 512, t_CKUINT num_buffers = 4,
                         t_CKUINT dac = 0, t_CKUINT adc = 0,
                         t_CKUINT dac_chan = 2, t_CKUINT adc_chan = 2,
                         t_CKBOOL block = TRUE, t_CKUINT adaptive = 0,
                         // force_srate added 1.3.1.2
                         t_CKBOOL force_srate = FALSE );
    t_CKBOOL initialize_synthesis( );
    t_CKBOOL shutdown();

public: // shreds
    Chuck_VM_Shred * spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent );
    Chuck_VM_Shred * fork( Chuck_VM_Code * code );
    Chuck_VM_Shreduler * shreduler() const;
    t_CKUINT next_id( );

public: // audio
    BBQ * bbq() const;
    t_CKUINT srate() const;
    void compensate_bbq();

public: // running the machine
    t_CKBOOL run( );
    t_CKBOOL run( t_CKINT num_samps );
    t_CKBOOL compute( );
    t_CKBOOL pause( );
    t_CKBOOL stop( );
    t_CKBOOL start_audio( );
    t_CKBOOL abort_current_shred( );

public: // invoke functions
    t_CKBOOL invoke_static( Chuck_VM_Shred * shred );

public: // garbage collection
    void gc();
    void gc( t_CKUINT amount );

public: // msg
    t_CKBOOL queue_msg( Chuck_Msg * msg, int num_msg );
    // CBufferSimple added 1.3.0.0 to fix uber-crash
    t_CKBOOL queue_event( Chuck_Event * event, int num_msg, CBufferSimple * buffer = NULL );
    t_CKUINT process_msg( Chuck_Msg * msg );
    Chuck_Msg * get_reply( );

    // added 1.3.0.0 to fix uber-crash
    CBufferSimple * create_event_buffer();
    void destroy_event_buffer( CBufferSimple * buffer );

public: // static/dynamic function table
    //void set_env( void * env ) { m_env = env; }
    //void * get_env( ) { return m_env; }
    t_CKBOOL has_init() { return m_init; }
    t_CKBOOL is_running() { return m_running; }
    
public: // get error
    const char * last_error() const
    { return m_last_error.c_str(); }

    t_CKBOOL set_main_thread_hook( f_mainthreadhook hook, f_mainthreadquit quit,
                                   void * bindle );
    t_CKBOOL clear_main_thread_hook();
    
//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public:
    // ugen
    Chuck_UGen * m_adc;
    Chuck_UGen * m_dac;
    Chuck_UGen * m_bunghole;
    t_CKUINT m_num_adc_channels;
    t_CKUINT m_num_dac_channels;
    
    t_CKBOOL m_halt;
    t_CKBOOL m_audio;
    t_CKBOOL m_block;

protected:
    Chuck_VM_Shred * spork( Chuck_VM_Shred * shred );
    t_CKBOOL free( Chuck_VM_Shred * shred, t_CKBOOL cascade, 
                   t_CKBOOL dec = TRUE );
    void dump( Chuck_VM_Shred * shred );
    void release_dump();

protected:
    t_CKBOOL m_init;
    // t_CKBOOL m_running;  -> moved to public
    t_CKBOOL m_audio_started;
    std::string m_last_error;

    // shred
    Chuck_VM_Shred * m_shreds;
    t_CKUINT m_num_shreds;
    t_CKUINT m_shred_id;
    Chuck_VM_Shreduler * m_shreduler;
    // place to put dumped shreds
    std::vector<Chuck_VM_Shred *> m_shred_dump;
    t_CKUINT m_num_dumped_shreds;

    // audio
    BBQ * m_bbq;

    // function table
    // Chuck_VM_FTable * m_func_table;
    // t_CKUINT m_num_func;

    // message queue
    CBufferSimple * m_msg_buffer;
    CBufferSimple * m_reply_buffer;
    CBufferSimple * m_event_buffer;
    
    // TODO: vector? (added 1.3.0.0 to fix uber-crash)
    std::list<CBufferSimple *> m_event_buffers;
    
    // added 1.3.2.0
    f_mainthreadhook m_main_thread_hook;
    f_mainthreadquit m_main_thread_quit;
    void *m_main_thread_bindle;

public:
    // running
    t_CKBOOL m_running;

    // priority
    static t_CKBOOL set_priority( t_CKINT priority, Chuck_VM * vm );
    static t_CKINT our_priority;
};




//-----------------------------------------------------------------------------
// name: enum Chuck_Msg_Type
// desc: ...
//-----------------------------------------------------------------------------
enum Chuck_Msg_Type
{
    MSG_ADD = 1,
    MSG_REMOVE,
    MSG_REMOVEALL,
    MSG_REPLACE,
    MSG_STATUS,
    MSG_PAUSE,
    MSG_KILL,
    MSG_TIME,
    MSG_RESET_ID,
    MSG_DONE,
    MSG_ABORT,
    MSG_ERROR, // added 1.3.0.0
    MSG_CLEARVM,
};




// callback function prototype
typedef void (* ck_msg_func)( const Chuck_Msg * msg );
//-----------------------------------------------------------------------------
// name: struct Chuck_Msg
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Msg
{
    t_CKUINT type;
    t_CKUINT param;
    Chuck_VM_Code * code;
    Chuck_VM_Shred * shred;
    t_CKTIME when;

    void * user;
    ck_msg_func reply;
    t_CKUINT replyA;
    t_CKUINT replyB;
    void * replyC;

    std::vector<std::string> * args;

    Chuck_Msg() : args(NULL) { clear(); }
    ~Chuck_Msg() { SAFE_DELETE( args ); }
    
    // added 1.3.0.0
    void clear() { SAFE_DELETE( args ); memset( this, 0, sizeof(*this) ); }
    
    void set( const std::vector<std::string> & vargs )
    {
        SAFE_DELETE(args);
        args = new std::vector<std::string>;
        (*args) = vargs;
    }
};




#endif
