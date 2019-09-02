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
#include "chuck_type.h"
#include "chuck_carrier.h"
#include "util_buffers.h"

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
#ifndef __DISABLE_SERIAL__
// hack: spencer?
struct Chuck_IO_Serial;
#endif

class CBufferSimple;
//XXXclass BBQ;
//XXXclass Digitalio;




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
    
    #ifndef __DISABLE_SERIAL__
    // HACK - spencer (added 1.3.2.0)
    // add/remove SerialIO devices to close on shred exit
    // REFACTOR-2017: TODO -- remove
    t_CKVOID add_serialio( Chuck_IO_Serial * serial );
    t_CKVOID remove_serialio( Chuck_IO_Serial * serial );
    #endif
    
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

public: // ge: 1.3.5.3
    // make and push new loop counter
    t_CKUINT * pushLoopCounter();
    // get top loop counter
    t_CKUINT * currentLoopCounter();
    // pop and clean up loop counter
    bool popLoopCounter();
    // loop counter pointer stack
    std::vector<t_CKUINT *> m_loopCounters;
    
#ifndef __DISABLE_SERIAL__
private:
    std::list<Chuck_IO_Serial *> * m_serials;
#endif
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
    void advance( t_CKINT N );
    void advance_v( t_CKINT & num_left, t_CKINT & offset );
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
    // added ge: 1.3.5.3
    Chuck_VM * vm_ref;

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




// Forward references for global messages
struct Chuck_Set_Global_Int_Request;
struct Chuck_Get_Global_Int_Request;
struct Chuck_Set_Global_Float_Request;
struct Chuck_Get_Global_Float_Request;
struct Chuck_Signal_Global_Event_Request;
struct Chuck_Listen_For_Global_Event_Request;
struct Chuck_Set_Global_String_Request;
struct Chuck_Get_Global_String_Request;
struct Chuck_Set_Global_Int_Array_Request;
struct Chuck_Get_Global_Int_Array_Request;
struct Chuck_Set_Global_Int_Array_Value_Request;
struct Chuck_Get_Global_Int_Array_Value_Request;
struct Chuck_Set_Global_Associative_Int_Array_Value_Request;
struct Chuck_Get_Global_Associative_Int_Array_Value_Request;
struct Chuck_Set_Global_Float_Array_Request;
struct Chuck_Get_Global_Float_Array_Request;
struct Chuck_Set_Global_Float_Array_Value_Request;
struct Chuck_Get_Global_Float_Array_Value_Request;
struct Chuck_Set_Global_Associative_Float_Array_Value_Request;
struct Chuck_Get_Global_Associative_Float_Array_Value_Request;
struct Chuck_Execute_Chuck_Msg_Request;

// Forward references for global storage
struct Chuck_Global_Int_Container;
struct Chuck_Global_Float_Container;
struct Chuck_Global_String_Container;
struct Chuck_Global_Event_Container;
struct Chuck_Global_UGen_Container;
struct Chuck_Global_Array_Container;




//-----------------------------------------------------------------------------
// name: enum Global_Request_Type
// desc: what kind of global message request is this? (REFACTOR-2017)
//-----------------------------------------------------------------------------
enum Chuck_Global_Request_Type
{
    // primitives
    set_global_int_request,
    get_global_int_request,
    set_global_float_request,
    get_global_float_request,
    set_global_string_request,
    get_global_string_request,
    // events
    signal_global_event_request,
    listen_for_global_event_request,
    // int arrays
    set_global_int_array_request,
    get_global_int_array_request,
    set_global_int_array_value_request,
    get_global_int_array_value_request,
    set_global_associative_int_array_value_request,
    get_global_associative_int_array_value_request,
    // float arrays
    set_global_float_array_request,
    get_global_float_array_request,
    set_global_float_array_value_request,
    get_global_float_array_value_request,
    set_global_associative_float_array_value_request,
    get_global_associative_float_array_value_request,
    // shreds
    spork_shred_request,
    // chuck_msg
    execute_chuck_msg_request
};




//-----------------------------------------------------------------------------
// name: strct Global_Request
// desc: a global request (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Global_Request
{
    Chuck_Global_Request_Type type;
    t_CKUINT retries;
    union {
        // primitives
        Chuck_Set_Global_Int_Request * setIntRequest;
        Chuck_Get_Global_Int_Request * getIntRequest;
        Chuck_Set_Global_Float_Request * setFloatRequest;
        Chuck_Get_Global_Float_Request * getFloatRequest;
        Chuck_Set_Global_String_Request * setStringRequest;
        Chuck_Get_Global_String_Request * getStringRequest;
        // events
        Chuck_Signal_Global_Event_Request * signalEventRequest;
        Chuck_Listen_For_Global_Event_Request * listenForEventRequest;
        // int arrays
        Chuck_Set_Global_Int_Array_Request * setIntArrayRequest;
        Chuck_Get_Global_Int_Array_Request * getIntArrayRequest;
        Chuck_Set_Global_Int_Array_Value_Request * setIntArrayValueRequest;
        Chuck_Get_Global_Int_Array_Value_Request * getIntArrayValueRequest;
        Chuck_Set_Global_Associative_Int_Array_Value_Request * setAssociativeIntArrayValueRequest;
        Chuck_Get_Global_Associative_Int_Array_Value_Request * getAssociativeIntArrayValueRequest;
        // float arrays
        Chuck_Set_Global_Float_Array_Request * setFloatArrayRequest;
        Chuck_Get_Global_Float_Array_Request * getFloatArrayRequest;
        Chuck_Set_Global_Float_Array_Value_Request * setFloatArrayValueRequest;
        Chuck_Get_Global_Float_Array_Value_Request * getFloatArrayValueRequest;
        Chuck_Set_Global_Associative_Float_Array_Value_Request * setAssociativeFloatArrayValueRequest;
        Chuck_Get_Global_Associative_Float_Array_Value_Request * getAssociativeFloatArrayValueRequest;
        // shreds
        Chuck_VM_Shred * shred;
        // chuck_msg
        Chuck_Execute_Chuck_Msg_Request * executeChuckMsgRequest;
    };

    Chuck_Global_Request()
    {
        retries = 0;
    }

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
    t_CKBOOL initialize( t_CKUINT srate, t_CKUINT dac_chan, t_CKUINT adc_chan,
                         t_CKUINT adaptive, t_CKBOOL halt );
    t_CKBOOL initialize_synthesis( );
    t_CKBOOL setCarrier( Chuck_Carrier * c ) { m_carrier = c; return TRUE; }
    t_CKBOOL shutdown();
    t_CKBOOL has_init() { return m_init; }

public: // run state; 1.3.5.3
    // run start
    t_CKBOOL start();
    // get run state
    t_CKBOOL running();
    // run stop
    t_CKBOOL stop();
    // backdoor to access state directly (should be called from inside VM only)
    t_CKBOOL & runningState() { return m_is_running; }

public: // shreds
    // spork code as shred; if not immediate, enqueue for next sample
    // REFACTOR-2017: added immediate flag
    Chuck_VM_Shred * spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent,
                            t_CKBOOL immediate = FALSE );
    // get reference to shreduler
    Chuck_VM_Shreduler * shreduler() const;
    // the next spork ID
    t_CKUINT next_id( );
    // the last used spork ID
    t_CKUINT last_id( );

public: // audio
    t_CKUINT srate() const;

public: // running the machine
    // compute next N frames
    t_CKBOOL run( t_CKINT numFrames, const SAMPLE * input, SAMPLE * output );
    // compute all shreds for current time
    t_CKBOOL compute( );
    // abort current running shred
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
    
public: // get error
    const char * last_error() const
    { return m_last_error.c_str(); }

public:
    // REFACTOR-2017: externally accessible + global variables.
    // use these getters and setters from outside the audio thread
    t_CKBOOL get_global_int( std::string name, void (* callback)(t_CKINT) );
    t_CKBOOL set_global_int( std::string name, t_CKINT val );

    t_CKBOOL get_global_float( std::string name, void (* callback)(t_CKFLOAT) );
    t_CKBOOL set_global_float( std::string name, t_CKFLOAT val );
    
    t_CKBOOL get_global_string( std::string name, void (* callback)(const char *) );
    t_CKBOOL set_global_string( std::string name, std::string val );
    
    t_CKBOOL signal_global_event( std::string name );
    t_CKBOOL broadcast_global_event( std::string name );
    t_CKBOOL listen_for_global_event( std::string name, void (* callback)(void), t_CKBOOL listen_forever );
    t_CKBOOL stop_listening_for_global_event( std::string name, void (* callback)(void) );
    
    t_CKBOOL get_global_ugen_samples( std::string name, SAMPLE * buffer, int numFrames );
    
    t_CKBOOL set_global_int_array( std::string name, t_CKINT arrayValues[], t_CKUINT numValues );
    t_CKBOOL get_global_int_array( std::string name, void (* callback)(t_CKINT[], t_CKUINT));
    t_CKBOOL set_global_int_array_value( std::string name, t_CKUINT index, t_CKINT value );
    t_CKBOOL get_global_int_array_value( std::string name, t_CKUINT index, void (* callback)(t_CKINT) );
    t_CKBOOL set_global_associative_int_array_value( std::string name, std::string key, t_CKINT value );
    t_CKBOOL get_global_associative_int_array_value( std::string name, std::string key, void (* callback)(t_CKINT) );
    
    t_CKBOOL set_global_float_array( std::string name, t_CKFLOAT arrayValues[], t_CKUINT numValues );
    t_CKBOOL get_global_float_array( std::string name, void (* callback)(t_CKFLOAT[], t_CKUINT));
    t_CKBOOL set_global_float_array_value( std::string name, t_CKUINT index, t_CKFLOAT value );
    t_CKBOOL get_global_float_array_value( std::string name, t_CKUINT index, void (* callback)(t_CKFLOAT) );
    t_CKBOOL set_global_associative_float_array_value( std::string name, std::string key, t_CKFLOAT value );
    t_CKBOOL get_global_associative_float_array_value( std::string name, std::string key, void (* callback)(t_CKFLOAT) );

public:
    // run Chuck_Msg in the globals order
    t_CKBOOL execute_chuck_msg_with_globals( Chuck_Msg * msg );
    
public:
    // REFACTOR-2017: externally accessible + global variables.
    // these internal functions are to be used only by other
    // chuck code in the audio thread.
    t_CKBOOL init_global_int( std::string name );
    t_CKINT get_global_int_value( std::string name );
    t_CKINT * get_ptr_to_global_int( std::string name );
    
    t_CKBOOL init_global_float( std::string name );
    t_CKFLOAT get_global_float_value( std::string name );
    t_CKFLOAT * get_ptr_to_global_float( std::string name );
    
    t_CKBOOL init_global_string( std::string name );
    Chuck_String * get_global_string( std::string name );
    Chuck_String * * get_ptr_to_global_string( std::string name );
    
    t_CKBOOL init_global_event( std::string name, Chuck_Type * type );
    Chuck_Event * get_global_event( std::string name );
    Chuck_Event * * get_ptr_to_global_event( std::string name );

    t_CKBOOL init_global_ugen( std::string name, Chuck_Type * type );
    t_CKBOOL is_global_ugen_init( std::string name );
    t_CKBOOL is_global_ugen_valid( std::string name );
    Chuck_UGen * get_global_ugen( std::string name );
    Chuck_UGen * * get_ptr_to_global_ugen( std::string name );
    
    t_CKBOOL init_global_array( std::string name, Chuck_Type * type, te_GlobalType arr_type );
    Chuck_Object * get_global_array( std::string name );
    t_CKINT _get_global_int_array_value( std::string name, t_CKUINT index );
    t_CKINT _get_global_associative_int_array_value( std::string name, std::string key );
    t_CKFLOAT _get_global_float_array_value( std::string name, t_CKUINT index );
    t_CKFLOAT _get_global_associative_float_array_value( std::string name, std::string key );
    Chuck_Object * * get_ptr_to_global_array( std::string name );
    
    t_CKBOOL should_call_global_ctor( std::string name, te_GlobalType type );
    void global_ctor_was_called( std::string name, te_GlobalType type );
    
protected:
    // REFACTOR-2017: global queue
    void handle_global_queue_messages();

public:
    // REFACTOR-2017: get associated, per-VM environment, chout, cherr
    Chuck_Carrier * carrier() const { return m_carrier; }
    Chuck_Env * env() const { return m_carrier->env; }
    Chuck_IO_Chout * chout() const { return m_carrier->chout; }
    Chuck_IO_Cherr * cherr() const { return m_carrier->cherr; }


//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
protected:
    // REFACTOR-2017: added per-ChucK carrier
    Chuck_Carrier * m_carrier;

public:
    // ugen
    Chuck_UGen * m_adc;
    Chuck_UGen * m_dac;
    Chuck_UGen * m_bunghole;
    t_CKUINT m_srate;
    t_CKUINT m_num_adc_channels;
    t_CKUINT m_num_dac_channels;
    t_CKBOOL m_halt;
    t_CKBOOL m_is_running;

    // for shreduler, ge: 1.3.5.3
    const SAMPLE * input_ref() { return m_input_ref; }
    SAMPLE * output_ref() { return m_output_ref; }
    // for shreduler, jack: planar audio buffers
    t_CKUINT buffer_length() { return m_current_buffer_frames; }

protected:
    // for shreduler, ge: 1.3.5.3
    const SAMPLE * m_input_ref;
    SAMPLE * m_output_ref;
    t_CKUINT m_current_buffer_frames;

protected:
    Chuck_VM_Shred * spork( Chuck_VM_Shred * shred );
    t_CKBOOL free( Chuck_VM_Shred * shred, t_CKBOOL cascade, 
                   t_CKBOOL dec = TRUE );
    void dump( Chuck_VM_Shred * shred );
    void release_dump();

protected:
    t_CKBOOL m_init;
    std::string m_last_error;

    // shred
    Chuck_VM_Shred * m_shreds;
    t_CKUINT m_num_shreds;
    t_CKUINT m_shred_id;
    Chuck_VM_Shreduler * m_shreduler;
    // place to put dumped shreds
    std::vector<Chuck_VM_Shred *> m_shred_dump;
    t_CKUINT m_num_dumped_shreds;

    // message queue
    CBufferSimple * m_msg_buffer;
    CBufferSimple * m_reply_buffer;
    CBufferSimple * m_event_buffer;
    
    // TODO: vector? (added 1.3.0.0 to fix uber-crash)
    std::list<CBufferSimple *> m_event_buffers;

private:
    // global variables
    void cleanup_global_variables();

    std::map< std::string, Chuck_Global_Int_Container * > m_global_ints;
    std::map< std::string, Chuck_Global_Float_Container * > m_global_floats;
    std::map< std::string, Chuck_Global_String_Container * > m_global_strings;
    std::map< std::string, Chuck_Global_Event_Container * > m_global_events;
    std::map< std::string, Chuck_Global_UGen_Container * > m_global_ugens;
    std::map< std::string, Chuck_Global_Array_Container * > m_global_arrays;
    
    XCircleBuffer< Chuck_Global_Request > m_global_request_queue;
    XCircleBuffer< Chuck_Global_Request > m_global_request_retry_queue;
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
    MSG_CLEARGLOBALS,
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
