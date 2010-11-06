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
// file: chuck_vm.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_VM_H__
#define __CHUCK_VM_H__

#include "chuck_oo.h"
#include "chuck_ugen.h"
#include "chuck_dl.h"
#include <string>
#include <map>
using namespace std;




//-----------------------------------------------------------------------------
// vm defines
//-----------------------------------------------------------------------------
#define CVM_MEM_STACK_SIZE          (0x1 << 16)
#define CVM_REG_STACK_SIZE          (0x1 << 16)


// forward references
class Chuck_Instr;
class Chuck_VM;
class Chuck_VM_Func;
class Chuck_VM_FTable;
struct Chuck_Msg;
class BBQ;
class CBuffer;
class Digitalio;




//-----------------------------------------------------------------------------
// name: class Chuck_VM_Stack
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_VM_Stack
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
// name: class Chuck_VM_Code
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_VM_Code
{
public:
    Chuck_VM_Code();
    ~Chuck_VM_Code();

public:
    Chuck_Instr ** instr;
    t_CKUINT num_instr;

    string name;
    t_CKUINT stack_depth;    
};




//-----------------------------------------------------------------------------
// name: class Chuck_VM_Shred
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_VM_Shred
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

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public: // machine components
    // stacks
    Chuck_VM_Stack * mem;
    Chuck_VM_Stack * reg;
    
    // code
    Chuck_VM_Code * code;
    Chuck_Instr ** instr;
    Chuck_VM_Shred * parent;
    map<t_CKUINT, Chuck_VM_Shred *> children;
    t_CKUINT pc;

    // time
    t_CKTIME now;
    t_CKTIME start;

public:
    t_CKTIME wake_time;
    t_CKUINT next_pc;
    t_CKBOOL is_done;
    t_CKBOOL is_running;
    map<Chuck_UGen *, Chuck_UGen *> m_ugen_map;

public: // id
    t_CKUINT id;
    string name;

public:
    Chuck_VM_Shred * prev;
    Chuck_VM_Shred * next;
};




//-----------------------------------------------------------------------------
// name: class Chuck_VM_Shreduler
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_VM_Shreduler
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

public: // high-level shred interface
    t_CKBOOL remove( Chuck_VM_Shred * shred );
    t_CKBOOL replace( Chuck_VM_Shred * out, Chuck_VM_Shred * in );
    Chuck_VM_Shred * lookup( t_CKUINT id );
    void status( );

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------
public:
    // time and audio
    t_CKTIME now_system;
    BBQ * bbq;
    
    // shreds
    Chuck_VM_Shred * shred_list;

    // ugen
    Chuck_UGen * m_dac;
    Chuck_UGen * m_adc;
    Chuck_UGen * m_bunghole;
    t_CKUINT m_num_dac_channels;
    t_CKUINT m_num_adc_channels;
};




//-----------------------------------------------------------------------------
// name: class Chuck_VM
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_VM
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
                         t_CKINT priority = 0x7fffffff );
    t_CKBOOL shutdown();

public: // shreds
    Chuck_VM_Shred * spork( Chuck_VM_Code * code, Chuck_VM_Shred * parent );
    Chuck_VM_Shred * fork( Chuck_VM_Code * code );
    Chuck_VM_Shreduler * shreduler() const;
    t_CKUINT next_id( );

public: // audio
    BBQ * bbq() const;
    t_CKUINT srate() const;

public: // running the machine
    t_CKBOOL run( );
    t_CKBOOL pause( );
    t_CKBOOL stop( );

public: // invoke functions
    t_CKBOOL invoke_static( Chuck_VM_Shred * shred );

public: // garbage collection
    void gc();
    void gc( t_CKUINT amount );

public: // msg
    t_CKBOOL queue_msg( Chuck_Msg * msg, int num_msg );
    t_CKUINT process_msg( Chuck_Msg * msg );
    Chuck_Msg * get_reply( );

public: // static/dynamic function table
    Chuck_DLL * dll_load( const char * path, const char * id = NULL );
    t_CKBOOL dll_unload( Chuck_DLL * & dll );
    void dll_clear( );
    void set_env( void * env ) { m_env = env; }
    void * get_env( ) { return m_env; }
    
public: // get error
    const char * last_error() const
    { return m_last_error.c_str(); }

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

protected:
    Chuck_VM_Shred * spork( Chuck_VM_Shred * shred );
    t_CKBOOL free( Chuck_VM_Shred * shred, t_CKBOOL cascade, 
                   t_CKBOOL dec = TRUE );

protected:
    t_CKBOOL m_init;
    t_CKBOOL m_running;
    string m_last_error;

    // shred
    Chuck_VM_Shred * m_shreds;
    t_CKUINT m_num_shreds;
    t_CKUINT m_shred_id;
    Chuck_VM_Shreduler * m_shreduler;

    // audio
    BBQ * m_bbq;

    // function table
    Chuck_VM_FTable * m_func_table;
    t_CKUINT m_num_func;

    // message queue
    CBuffer * m_msg_buffer;
    CBuffer * m_reply_buffer;

    // type information
    void * m_env;
    map<string, Chuck_DLL *> m_dlls;
    
public:
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
    MSG_DONE
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

    Chuck_Msg() { memset( this, 0, sizeof(*this) ); }
};




#endif
