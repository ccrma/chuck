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
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_VM_H__
#define __CHUCK_VM_H__

#include <string>
#include <map>
#include <vector>
#include <list>

#include "chuck_oo.h"
#include "chuck_ugen.h"
#include "chuck_type.h"
#include "chuck_carrier.h"

// tracking
#ifdef __CHUCK_STAT_TRACK__
#include "chuck_stats.h"
#endif




//-----------------------------------------------------------------------------
// vm defines
//-----------------------------------------------------------------------------
#define CKVM_MEM_STACK_SIZE          (0x1 << 16)
#define CKVM_REG_STACK_SIZE          (0x1 << 14)


// forward references
struct Chuck_Instr;
struct Chuck_VM;
struct Chuck_VM_Func;
struct Chuck_VM_FTable;
struct Chuck_Msg;
struct Chuck_Globals_Manager; // added 1.4.1.0 (jack)
struct Chuck_Instr_Reg_Push_Imm; // 1.5.1.5 (ge)
class CBufferSimple;
#ifndef __DISABLE_SERIAL__
// hack: spencer?
struct Chuck_IO_Serial;
#endif




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Stack
// desc: a VM stack; each shred has at least two (mem and reg)
//-----------------------------------------------------------------------------
struct Chuck_VM_Stack
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    // constructor
    Chuck_VM_Stack();
    // destructor
    ~Chuck_VM_Stack();

public:
    // initialize stack of at least 'size' bytes
    t_CKBOOL initialize( t_CKUINT size );
    // shutdown and cleanup stack
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
    t_CKUINT m_size; // 1.5.1.5
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Code
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_VM_Code : public Chuck_Object
{
public:
    // constructor
    Chuck_VM_Code();
    // destructor
    virtual ~Chuck_VM_Code();

public:
    // array of Chuck_Instr *, should always end with Chuck_Instr_EOF
    Chuck_Instr ** instr;
    // size of the array
    t_CKUINT num_instr;

    // name of this code
    std::string name;
    // the depth of any function arguments (in bytes)
    t_CKUINT stack_depth;
    // whether the function needs 'this' pointer or not
    t_CKBOOL need_this;
    // whether the function is a static function inside class
    t_CKBOOL is_static; // 1.4.1.0
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
struct Chuck_VM_Shred : public Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    // constructor
    Chuck_VM_Shred( );
    // destructor
    virtual ~Chuck_VM_Shred( );

    // initialize shred
    t_CKBOOL initialize( Chuck_VM_Code * c,
                         t_CKUINT mem_st_size = 0,
                         t_CKUINT reg_st_size = 0 );
    // shutdown shred
    t_CKBOOL shutdown();
    // run the shred on vm
    t_CKBOOL run( Chuck_VM * vm );
    // yield the shred in vm (without advancing time, politely yield to run
    // all other shreds waiting to run at the current (i.e., 0::second +=> now;)
    t_CKBOOL yield(); // 1.5.0.5 (ge) made this a function from scattered code
    // associate ugen with shred
    t_CKBOOL add( Chuck_UGen * ugen );
    // unassociate ugen with shred
    t_CKBOOL remove( Chuck_UGen * ugen );
    // detach all associate ugens | 1.5.1.5 (ge) added
    void detach_ugens();

    // add parent object reference (added 1.3.1.2)
    t_CKVOID add_parent_ref( Chuck_Object * obj );
    // add get shred id | 1.5.0.8 (ge)
    t_CKUINT get_id() const { return this->xid; }

    // affects children shreds sporked from this one
    // mem is memory / call stack (for local vars)
    t_CKINT childSetMemSize( t_CKINT sizeInBytes );
    t_CKINT childGetMemSize( );
    // reg is rester / operand stack (for evaluating expressions)
    t_CKINT childSetRegSize( t_CKINT sizeInBytes );
    t_CKINT childGetRegSize( );

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
    // stacks; mem for "memory" and reg for "register"
    Chuck_VM_Stack * mem; // call stack
    Chuck_VM_Stack * reg; // operand stack

    // ref to base stack - if this is the root, then base is mem
    Chuck_VM_Stack * base_ref;

    // current vm code being run (this could change, e.g., across func calls)
    Chuck_VM_Code * code;
    // the original vm code attached to this shred (also: the one to release)
    Chuck_VM_Code * code_orig;
    // instructions
    Chuck_Instr ** instr;

    // parent shred
    Chuck_VM_Shred * parent;
    // children shreds
    std::map<t_CKUINT, Chuck_VM_Shred *> children;

    // child stack size hints | 1.5.1.5
    t_CKINT memStackSize;
    t_CKINT regStackSize;

    // program counter
    t_CKUINT pc;
    // time
    t_CKTIME now;
    // when started, in chuck time, relative to start of VM
    t_CKTIME start;
    // vm reference
    Chuck_VM * vm_ref;

public:
    // shred state
    t_CKTIME wake_time;
    t_CKUINT next_pc;
    t_CKBOOL is_done;
    t_CKBOOL is_running;
    t_CKBOOL is_abort;
    t_CKBOOL is_dumped;

    // event shred is waiting on
    Chuck_Event * event;
    // map of ugens for the shred
    std::map<Chuck_UGen *, Chuck_UGen *> m_ugen_map;
    // references kept by the shred itself (e.g., when sporking member functions)
    // to be released when shred is done -- added 1.3.1.2
    std::vector<Chuck_Object *> m_parent_objects;

public: // id
    t_CKUINT xid;
    std::string name;
    std::vector<std::string> args;

public:
    // linked list
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

public: // immediate mode temporal restriction | 1.5.1.5 (ge)
    // while in this mode, exception will be thrown on any time ops:
    // 1) if shred advances time (even by 0 duration)
    // 2) if shred waits on event
    // 3) if shred yields, or calls Machine.eval()
    //    which implicits yields to runs code on a new shred
    // this is typically set in specific cases where the programmer
    // is expected to provide a callback function that must return
    // immediately without any shreduling; for example Chugen.tick( float in )
    // or GGen.update( float dt );

    // toggle immediate mode
    void setImmediateMode( t_CKBOOL onOff ) { is_immediate_mode = onOff; }
    // check if shred is in immediate mode
    t_CKBOOL immediateMode() const { return is_immediate_mode; }
    // check if shred is in immediate mode
    t_CKBOOL immediateModeVioation() const { return is_immediate_mode_violation; }
    // test and report for immediate mode violations
    t_CKBOOL checkImmediatModeException( t_CKUINT linepos = 0 );

protected:
    t_CKBOOL is_immediate_mode;
    t_CKBOOL is_immediate_mode_violation;

#ifndef __DISABLE_SERIAL__
private:
    // serial IO list for event synchronization
    std::list<Chuck_IO_Serial *> * m_serials;
#endif
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Shreds_Watcher
// desc: ChucK virtual machine
//-----------------------------------------------------------------------------
struct Chuck_VM_Shreds_Watcher
{
    // function pointer to call
    f_shreds_watcher cb;
    // user data
    void * userdata;

    // constructor
    Chuck_VM_Shreds_Watcher( f_shreds_watcher f = NULL, void * data = NULL ) : cb(f), userdata(data) { }
    // copy constructor
    Chuck_VM_Shreds_Watcher( const Chuck_VM_Shreds_Watcher & other )
    : cb(other.cb), userdata(other.userdata) { }
    // ==
    bool operator ==( const Chuck_VM_Shreds_Watcher & other )
    { return this->cb == other.cb; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Shred_Status
// desc: status pertaining to a single shred
//-----------------------------------------------------------------------------
struct Chuck_VM_Shred_Status : public Chuck_Object
{
public:
    t_CKUINT xid;
    std::string name;
    t_CKTIME start;
    t_CKBOOL has_event;

public:
    // constructor
    Chuck_VM_Shred_Status( t_CKUINT _id, const std::string & n, t_CKTIME _start, t_CKBOOL e )
    {
        xid = _id;
        name = n;
        start = _start;
        has_event = e;
    }

    // destructor
    virtual ~Chuck_VM_Shred_Status() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Status
// desc: vm status
//-----------------------------------------------------------------------------
struct Chuck_VM_Status : public Chuck_Object
{
public:
    Chuck_VM_Status();
    virtual ~Chuck_VM_Status();
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
// desc: a ChucK shreduler shredules shreds
//-----------------------------------------------------------------------------
struct Chuck_VM_Shreduler : public Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    // constructor
    Chuck_VM_Shreduler();
    // destructor
    virtual ~Chuck_VM_Shreduler();

public: // initialization and shutdown
    // initialize shreduler
    t_CKBOOL initialize();
    // shutdown shreduler
    t_CKBOOL shutdown();

public: // shreduling (shred interface part 1)
    // shredule a shred in the shreduler (equivalent to ADD)
    t_CKBOOL shredule( Chuck_VM_Shred * shred );
    // shredule a shred in the shreduler with a specific wake time
    t_CKBOOL shredule( Chuck_VM_Shred * shred, t_CKTIME wake_time );
    // get next shred to run
    Chuck_VM_Shred * get();
    // advance shreduler
    void advance( t_CKINT N );
    // advance shreduler vectorized edition
    void advance_v( t_CKINT & num_left, t_CKINT & offset );
    // set adaptive mode and adaptive max block size
    void set_adaptive( t_CKUINT max_block_size );

public: // remove, replace, status (shred interface part 2)
    // remove a shred from the shreduler
    t_CKBOOL remove( Chuck_VM_Shred * shred );
    // replace a shred with another shred
    t_CKBOOL replace( Chuck_VM_Shred * out, Chuck_VM_Shred * in );
    // print status
    void status();
    // get status
    void status( Chuck_VM_Status * status );

public: // shred lookup and retrieval (shred interface part 3)
    // look a shred by ID
    Chuck_VM_Shred * lookup( t_CKUINT xid ) const;
    // get ID of shred currently with the highest ID
    t_CKUINT highest() const;

    // retrieve the ready list of shreds in the shreduler
    // NOTE shreds on the shreduler's ready list are sorted by
    //      wake-up time (e.g., as specifiedy by `second => now`)
    // NOTE the ready list DO NOT include shreds waiting on Events
    //      waiting shreds are on the shreduler's blocked list
    // NOTE the ready list does not include the shred currently
    //      executing in the VM
    void get_ready_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                           t_CKBOOL clearVector = TRUE ) const;
    // retrieve list of active shred IDs
    void get_ready_shred_ids( std::vector<t_CKUINT> & shredIDs,
                              t_CKBOOL clearVector = TRUE ) const;

    // retrieve the list of waiting shreds in the shreduler
    // NOTE a waiting shred is on the shreduler's blocked list as long as
    //      it is currently waiting on an Event (e.g., `event => now;`)
    // NOTE the shreduler's blocked list is unsorted; FYI a sorted list of
    //      shreds is maintained by each Event
    void get_blocked_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                             t_CKBOOL clearVector = TRUE ) const;
    // retrieve list of blocked shred IDs
    void get_blocked_shred_ids( std::vector<t_CKUINT> & shredIDs,
                                t_CKBOOL clearVector = TRUE ) const;

    // get currently executing shred
    // NOTE this can only be non-NULL during a Chuck_VM::compute() cycle
    Chuck_VM_Shred * get_current_shred() const;

    // retrieve all shreds currently in the shreduler; this includes the
    // ready list, the blocked list, and the currently executing shred
    void get_all_shreds( std::vector<Chuck_VM_Shred *> & shreds,
                         t_CKBOOL clearVector = TRUE ) const;
    // retrieve list of all shred IDs
    void get_all_shred_ids( std::vector<t_CKUINT> & shredIDs,
                            t_CKBOOL clearVector = TRUE ) const;

public: // for event related shred queue (shred interface part 4)
    // (should only be called from under the hood)
    t_CKBOOL add_blocked( Chuck_VM_Shred * shred );
    t_CKBOOL remove_blocked( Chuck_VM_Shred * shred );

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public:
    // time and audio -- this is the chuck `now`
    t_CKTIME now_system;
    // added ge: 1.3.5.3
    Chuck_VM * vm_ref;

    // shreds to be shreduled
    Chuck_VM_Shred * shred_list;
    // shreds waiting on events
    std::map<Chuck_VM_Shred *, Chuck_VM_Shred *> blocked;
    // current shred | TODO: ref count?
    Chuck_VM_Shred * m_current_shred;

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
// desc: ChucK virtual machine
//-----------------------------------------------------------------------------
struct Chuck_VM : public Chuck_Object
{
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:
    // constructor
    Chuck_VM();
    // destructor
    virtual ~Chuck_VM();

public:
    // initialize VM
    t_CKBOOL initialize( t_CKUINT srate, t_CKUINT dac_chan, t_CKUINT adc_chan,
                         t_CKUINT adaptive, t_CKBOOL halt );
    // initialize synthesis
    t_CKBOOL initialize_synthesis();
    // set carrier reference
    t_CKBOOL setCarrier( Chuck_Carrier * c ) { m_carrier = c; return TRUE; }
    // shutdown VM
    t_CKBOOL shutdown();
    // return whether VM is initialized
    t_CKBOOL has_init() { return m_init; }

public: // run state; 1.3.5.3
    // run start
    t_CKBOOL start();
    // get run state
    t_CKBOOL running() const;
    // run stop
    t_CKBOOL stop();
    // const access state directly (should be called from inside VM only)
    const t_CKBOOL & runningState() const { return m_is_running; }

public: // shredsuck
    // spork code as shred; if not immediate, enqueue for next sample
    // REFACTOR-2017: added immediate flag
    Chuck_VM_Shred * spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent,
                            t_CKBOOL immediate = FALSE );
    // get reference to shreduler
    Chuck_VM_Shreduler * shreduler() const;
    // the next spork ID
    t_CKUINT next_id( const Chuck_VM_Shred * shred = NULL );
    // the last used spork ID
    t_CKUINT last_id() const;
    // reset ID to highest current ID + 1; returns what next ID would be
    t_CKUINT reset_id();
    // the current chuck time | 1.5.0.8
    t_CKTIME now() const;

public: // audio
    t_CKUINT srate() const;

public: // running the machine
    // compute next N frames
    t_CKBOOL run( t_CKINT numFrames, const SAMPLE * input, SAMPLE * output );
    // compute all shreds for current time
    t_CKBOOL compute();
    // abort current running shred
    t_CKBOOL abort_current_shred();

public: // invoke functions
    t_CKBOOL invoke_static( Chuck_VM_Shred * shred );

public: // garbage collection
    void gc();
    void gc( t_CKUINT amount );

public: // VM message queue
    // queue message to process at next VM compute block (thread-safe but not synchronous)
    // NOTE assumes msg is dynamically allocated using `new`; will be deleted by VM
    // NOTE this defers the processing of the msg to the VM compute thread
    t_CKBOOL queue_msg( Chuck_Msg * msg, t_CKINT num_msg = 1 );
    // process a VM message immediately (synchronous but not thread-safe)
    // NOTE assumes msg is dynamically allocated using `new`; will be deleted by VM
    // NOTE this processes the msg immediately on calling thread
    t_CKUINT process_msg( Chuck_Msg * & msg );
    // get reply from reply buffer
    Chuck_Msg * get_reply();

    // CBufferSimple added 1.3.0.0 to fix uber-crash
    t_CKBOOL queue_event( Chuck_Event * event, t_CKINT num_msg = 1, CBufferSimple * buffer = NULL );
    // added 1.3.0.0 to fix uber-crash (allocates and attaches new buffer)
    CBufferSimple * create_event_buffer();
    // added 1.3.0.0 to fix uber-crash (detaches and deletes buffer)
    void destroy_event_buffer( CBufferSimple * buffer );
    // added 1.5.1.3 for granularity in handling synchronization
    void detach_event_buffer_without_delete( CBufferSimple * buffer );

public: // get error
    const char * last_error() const
    { return m_last_error.c_str(); }

public:
    // REFACTOR-2017: get associated, per-VM environment, chout, cherr
    Chuck_Carrier * carrier() const { return m_carrier; }
    Chuck_Env * env() const { return m_carrier->env; }
    Chuck_IO_Chout * chout() const { return m_carrier->chout; }
    Chuck_IO_Cherr * cherr() const { return m_carrier->cherr; }
    // 1.4.1.0 (jack): get associated globals manager
    Chuck_Globals_Manager * globals_manager() const { return m_globals_manager; }

public:
    // subscribe shreds watcher callback | 1.5.1.5
    void subscribe_watcher( f_shreds_watcher cb, t_CKUINT options, void * data = NULL );
    // notify watchers | 1.5.1.5
    void notify_watchers( ckvmShredsWatcherFlag which, Chuck_VM_Shred * shred,
                          std::list<Chuck_VM_Shreds_Watcher> & v );
    // remove shreds watcher callback | 1.5.1.5
    void remove_watcher( f_shreds_watcher cb );

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
    // for shreduler, jack: planar (non-interleaved) audio buffers
    t_CKUINT most_recent_buffer_length() { return m_current_buffer_frames; }

protected:
    // for shreduler, ge: 1.3.5.3
    const SAMPLE * m_input_ref;
    SAMPLE * m_output_ref;
    t_CKUINT m_current_buffer_frames;

public:
    // protected, but needs to be accessible from Globals Manager (1.4.1.0)
    // generally, this should not be called except by internals such as GM
    Chuck_VM_Shred * spork( Chuck_VM_Shred * shred );

protected:
    // remove all shreds from VM
    void removeAll();
    // free shred
    t_CKBOOL free_shred( Chuck_VM_Shred * shred, t_CKBOOL cascade,
                         t_CKBOOL dec = TRUE );
    void dump_shred( Chuck_VM_Shred * shred );
    void release_dump();

protected:
    t_CKBOOL m_init;
    std::string m_last_error;

    // shred
    Chuck_VM_Shred * m_shreds;
    t_CKUINT m_num_shreds;
    t_CKUINT m_shred_id;
    t_CKBOOL m_shred_check4dupes; // 1.5.1.5
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

protected:
    // 1.4.1.0 (jack): manager for global variables
    Chuck_Globals_Manager * m_globals_manager;

protected:
    // 1.5.1.5 (ge & andrew) shreds watchers
    std::list<Chuck_VM_Shreds_Watcher> m_shreds_watchers_spork;
    std::list<Chuck_VM_Shreds_Watcher> m_shreds_watchers_remove;
    std::list<Chuck_VM_Shreds_Watcher> m_shreds_watchers_suspend;
    std::list<Chuck_VM_Shreds_Watcher> m_shreds_watchers_activate;
};




//-----------------------------------------------------------------------------
// name: enum Chuck_Msg_Type
// desc: message types
//-----------------------------------------------------------------------------
enum Chuck_Msg_Type
{
    CK_MSG_ADD = 1,
    CK_MSG_REMOVE,
    CK_MSG_REMOVEALL,
    CK_MSG_REPLACE,
    CK_MSG_STATUS,
    CK_MSG_PAUSE,
    CK_MSG_EXIT,
    CK_MSG_TIME,
    CK_MSG_RESET_ID,
    CK_MSG_DONE,
    CK_MSG_ABORT,
    CK_MSG_ERROR, // added 1.3.0.0
    CK_MSG_CLEARVM,
    CK_MSG_CLEARGLOBALS,
};




// callback function prototype
typedef void (* ck_msg_func)( const Chuck_Msg * msg );
//-----------------------------------------------------------------------------
// name: struct Chuck_Msg
// desc: chuck message, used to communicate with VM
//-----------------------------------------------------------------------------
struct Chuck_Msg
{
    // type of message
    t_CKUINT type;

    // messsage parameter, as applicable
    t_CKUINT param;
    // VM code pointer, as applicable
    Chuck_VM_Code * code;
    // VM shred pointer, as applicable
    Chuck_VM_Shred * shred;
    // parent shred, as applicable
    Chuck_VM_Shred * parent;
    // time, as applicable
    t_CKTIME when;
    // pointer to status struct, as applicable
    Chuck_VM_Status * status;
    // whether to always add | 1.5.1.5
    t_CKBOOL alwaysAdd;

    // reply callback
    ck_msg_func reply_cb;
    // if true, reply on queue
    t_CKBOOL reply_queue;
    // NOTE if both reply_cb and reply_queue is non-zero, reply_cb will take precedence

    // reply arguments
    t_CKUINT replyA;
    t_CKUINT replyB;
    void * replyC;

    // argument array pointer
    std::vector<std::string> * args;

    // constructor
    Chuck_Msg() : args(NULL) { clear(); }
    // destructor
    ~Chuck_Msg() { clear(); }

    // added 1.3.0.0
    void clear()
    {
        // clean up existing args
        CK_SAFE_DELETE(args);
        // clear
        memset( this, 0, sizeof(Chuck_Msg) );
    }

    // copy in args
    void set( const std::vector<std::string> * vargs )
    {
        // clean up existing args
        CK_SAFE_DELETE(args);
        // check
        if( vargs )
        {
            // instantiate new
            args = new std::vector<std::string>;
            // copy
            (*args) = *vargs;
        }
    }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_MFunInvoker | 1.5.1.5 (ge)
// desc: construct for calling chuck-defined member functions from c++,
//       either from VM execution or outside the VM execution context
//-----------------------------------------------------------------------------
struct Chuck_VM_MFunInvoker
{
public:
    // constructor
    Chuck_VM_MFunInvoker();
    // destructor
    ~Chuck_VM_MFunInvoker();

public:
    // set up the invoker; needed before invoke()
    t_CKBOOL setup( Chuck_Func * func, t_CKUINT func_vt_offset,
                    Chuck_VM * vm, Chuck_VM_Shred * caller );
    // invoke the member function
    Chuck_DL_Return invoke( Chuck_Object * obj,
                            const std::vector<Chuck_DL_Arg> & args );
    // clean up
    void cleanup();

public:
    // dedicated shred to call the mfun on
    Chuck_VM_Shred * shred;
    // instructions for args (to be filled on invoke)
    std::vector<Chuck_Instr *> instr_args;
    // instruction to update on invoke: pushing this pointer
    Chuck_Instr_Reg_Push_Imm * instr_pushThis;
    // instruction to update on invoke: pushing the var to receive return
    Chuck_Instr_Reg_Push_Imm * instr_pushReturnVar;
};




//-----------------------------------------------------------------------------
// VM debug macros | 1.5.0.5 (ge) added
// these are governed by the presence of __CHUCK_DEBUG__ (e.g., from makefile)
//-----------------------------------------------------------------------------
#ifdef __CHUCK_DEBUG__
  // enable VM debug
  #define CK_VM_DEBUG_ENABLE 1
#else
  // disable VM debug
  #define CK_VM_DEBUG_ENABLE 0
#endif

// vm debug macros
#if CK_VM_DEBUG_ENABLE
  // calls/access 'member' on the VM_Debug singleton
  // e.g., `CK_VM_DEBUGGER( add_ref(obj) )` expands to...
  // ----> `(Chuck_VM_Debug::instance())->add_ref(obj)`
  #define CK_VM_DEBUGGER( member ) CK_VM_DEBUGGER_INSTANCE->member
  // expands to the Chuck_VM_Debug singleton
  #define CK_VM_DEBUGGER_INSTANCE (Chuck_VM_Debug::instance())
  // expands to the argument
  #define CK_VM_DEBUG_ACTION( act ) act
#else
  // empty macros; will be compiled-out
  #define CK_VM_DEBUGGER( act )
  #define CK_VM_DEBUGGER_INSTANCE
  #define CK_VM_DEBUG_ACTION( act )
#endif




// control compilation
#if CK_VM_DEBUG_ENABLE
//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Debug | 1.5.0.5 (ge) added
// desc: vm debug helper
//-----------------------------------------------------------------------------
struct Chuck_VM_Debug
{
public:
    // call this right after instantiation (tracking)
    void construct( Chuck_VM_Object * obj, const std::string & note = "" );
    // call this right before destruct (tracking)
    void destruct( Chuck_VM_Object * obj, const std::string & note = "" );
    // add this on add reference (tracking)
    void add_ref( Chuck_VM_Object * obj, const std::string & note = "" );
    // add this on release reference (tracking)
    void release( Chuck_VM_Object * obj, const std::string & note = "" );

public:
    // set log level to print to
    void set_log_evel( t_CKUINT level );
    // number of objects being tracked
    t_CKUINT num_objects();
    // print all objects being tracked
    void print_all_objects();
    // print stats
    void print_stats();
    // reset stats
    void reset_stats();

public:
    // one func to get info using runtime types
    std::string info( Chuck_VM_Object * obj );

    // info by type
    std::string info_obj( Chuck_Object * obj );
    std::string info_type( Chuck_Type * type );
    std::string info_func( Chuck_Func * func );
    std::string info_value( Chuck_Value * value );
    std::string info_namespace( Chuck_Namespace * nspc );
    std::string info_context( Chuck_Context * context );
    std::string info_env( Chuck_Env * env );
    std::string info_ugen_info( Chuck_UGen_Info * ug );
    std::string info_code( Chuck_VM_Code * code );
    std::string info_shred( Chuck_VM_Shred * shred );
    std::string info_vm( Chuck_VM * vm );

public:
    // one func to try to print them all
    void print( Chuck_VM_Object * obj, const std::string & note = "" );

public:
    // get call stack as string
    static std::string info_backtrace( );
    // print call stack at this point
     void backtrace( const std::string & note = "" );

public:
    // get shared instance
    static Chuck_VM_Debug * instance();
    // destructor
    ~Chuck_VM_Debug();

protected:
    // shared instance pointer
    static Chuck_VM_Debug * our_instance;
    // protected constructor
    Chuck_VM_Debug();

protected:
    // insert into object map
    void insert( Chuck_VM_Object * obj );
    // insert into object map
    void remove( Chuck_VM_Object * obj );
    // is present?
    t_CKBOOL contains( Chuck_VM_Object * obj );
    // get map by type name
    std::map<Chuck_VM_Object *, Chuck_VM_Object *> get_objs( const std::string & key);

protected:
    // log level
    t_CKUINT m_log_level;
    // object map by name -> map
    std::map< std::string, std::map<Chuck_VM_Object *, Chuck_VM_Object *> > m_objects_map;

protected:
    // stats
    t_CKUINT m_numConstructed;
    t_CKUINT m_numDestructed;
    t_CKUINT m_numAddRefs;
    t_CKUINT m_numReleases;
};
#endif




#endif
