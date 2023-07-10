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
// file: chuck_oo.h
// desc: chuck baes objects
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Andrew Schran (aschran@princeton.edu) - fileIO implementation
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_OO_H__
#define __CHUCK_OO_H__

#include "chuck_def.h"
#include "chuck_carrier.h"
#ifndef __DISABLE_THREADS__
#include "util_thread.h" // added 1.3.0.0
#endif

#include <string>
#include <vector>
#include <map>
#include <queue>




// forward reference
struct Chuck_Type;
struct Chuck_Value;
struct Chuck_Func;
struct Chuck_Namespace;
struct Chuck_Context;
struct Chuck_Env;
struct Chuck_VM_Code;
struct Chuck_VM_Shred;
struct Chuck_VM;
struct Chuck_IO_File;
class  CBufferSimple;




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Object
// desc: base vm object
//-----------------------------------------------------------------------------
struct Chuck_VM_Object
{
public:
    Chuck_VM_Object();
    virtual ~Chuck_VM_Object();

public:
    // add reference (ge: april 2013: made these virtual)
    virtual void add_ref();
    // release reference
    virtual void release();
    // lock
    virtual void lock();
    // unlock | 1.5.0.0 (ge) added
    virtual void unlock();

    // NOTE: be careful when overriding these, should always
    // explicitly call up to ChucK_VM_Object (ge: 2013)

public:
    // get reference count
    t_CKUINT refcount() const;

public:
    // unlock_all: dis/allow deletion of locked objects
    static void lock_all();
    static void unlock_all();
    static t_CKBOOL our_locks_in_effect;

public:
    t_CKUINT m_ref_count; // reference count
    t_CKBOOL m_pooled; // if true, this allocates from a pool
    t_CKBOOL m_locked; // if true, this should never be deleted

public:
    // where
    std::vector<Chuck_VM_Object *> * m_v_ref;

private:
    void init_ref();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_VTable
// desc: virtual table
//-----------------------------------------------------------------------------
struct Chuck_VTable
{
public:
    std::vector<Chuck_Func *> funcs;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Object
// dsec: base object
//-----------------------------------------------------------------------------
struct Chuck_Object : public Chuck_VM_Object
{
public:
    Chuck_Object();
    virtual ~Chuck_Object();

public:
    // output current state (can be overridden)
    virtual void dump();

public:
    // output type info (can be overriden; but probably shouldn't be)
    virtual void help();

public:
    // virtual table
    Chuck_VTable * vtable;
    // reference to type
    Chuck_Type * type_ref;
    // data for the object
    t_CKBYTE * data;
    // the size of the data region
    t_CKUINT data_size;
};




// ISSUE: 64-bit (fixed 1.3.1.0)
#define CHUCK_ARRAY4_DATASIZE sz_INT
#define CHUCK_ARRAY8_DATASIZE sz_FLOAT
#define CHUCK_ARRAY16_DATASIZE sz_COMPLEX
#define CHUCK_ARRAY24_DATASIZE sz_VEC3 // 1.3.5.3
#define CHUCK_ARRAY32_DATASIZE sz_VEC4 // 1.3.5.3
#define CHUCK_ARRAY4_DATAKIND kindof_INT
#define CHUCK_ARRAY8_DATAKIND kindof_FLOAT
#define CHUCK_ARRAY16_DATAKIND kindof_COMPLEX
#define CHUCK_ARRAY24_DATAKIND kindof_VEC3 // 1.3.5.3
#define CHUCK_ARRAY32_DATAKIND kindof_VEC4 // 1.3.5.3
//-----------------------------------------------------------------------------
// name: struct Chuck_Array
// desc: native ChucK arrays (virtual base class)
//-----------------------------------------------------------------------------
struct Chuck_Array : public Chuck_Object
{
    // functionality that we can keep in common...

public:
    Chuck_Array() : m_array_type(NULL) { }
    virtual ~Chuck_Array();

    virtual t_CKINT size( ) = 0; // const { return m_size; } // array size
    virtual t_CKINT capacity( ) = 0; // const { return m_capacity; } // array capacity
    virtual t_CKINT set_size( t_CKINT size ) = 0; // array size
    virtual t_CKINT set_capacity( t_CKINT capacity ) = 0; // set
    virtual t_CKINT data_type_size( ) = 0; // size of stored type (from type_ref)
    virtual t_CKINT data_type_kind( ) = 0; // kind of stored type (from kindof)
    virtual t_CKINT find( const std::string & key ) = 0; // find
    virtual t_CKINT erase( const std::string & key ) = 0; // erase
    virtual void clear( ) = 0; // clear
    virtual void zero( t_CKUINT start, t_CKUINT end ) = 0; // zero
    virtual void zero() = 0; // zero (all)
    // get map keys | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys ) = 0;
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse() = 0;
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle() = 0;

    Chuck_Type * m_array_type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array4
// desc: native ChucK arrays (for 4-byte/8-byte int types)
//-----------------------------------------------------------------------------
struct Chuck_Array4 : public Chuck_Array
{
public:
    Chuck_Array4( t_CKBOOL is_obj, t_CKINT capacity = 8 );
    virtual ~Chuck_Array4();

public:
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    t_CKINT get( t_CKINT i, t_CKUINT * val );
    t_CKINT get( const std::string & key, t_CKUINT * val );
    t_CKINT set( t_CKINT i, t_CKUINT val );
    t_CKINT set( const std::string & key, t_CKUINT val );
    t_CKINT push_back( t_CKUINT val );
    t_CKINT pop_back( );
    t_CKINT pop_out( t_CKINT pos );
    t_CKINT back( t_CKUINT * val ) const;
    void    zero( t_CKUINT start, t_CKUINT end );
    void    zero() { this->zero(0, m_vector.size()); }

    virtual void    clear( );
    virtual t_CKINT size( ) { return m_vector.size(); }
    virtual t_CKINT capacity( ) { return m_vector.capacity(); }
    virtual t_CKINT set_size( t_CKINT size );
    virtual t_CKINT set_capacity( t_CKINT capacity );
    virtual t_CKINT find( const std::string & key );
    virtual t_CKINT erase( const std::string & key );
    virtual t_CKINT data_type_size( ) { return CHUCK_ARRAY4_DATASIZE; }
    virtual t_CKINT data_type_kind( ) { return CHUCK_ARRAY4_DATAKIND; }
    virtual t_CKBOOL contains_objects( ) { return m_is_obj; } // 1.5.0.1 (ge) added
    // get map keys | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys );
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse();
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle();

public:
    std::vector<t_CKUINT> m_vector;
    std::map<std::string, t_CKUINT> m_map;
    t_CKBOOL m_is_obj;

    // TODO: may need additional information here for set_size, if this is part of a multi-dim array

    // t_CKINT m_size;
    // t_CKINT m_capacity;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array8
// desc: native ChucK arrays (for 8-byte float)
//-----------------------------------------------------------------------------
struct Chuck_Array8 : public Chuck_Array
{
public:
    Chuck_Array8( t_CKINT capacity = 8 );
    virtual ~Chuck_Array8();

public:
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    t_CKINT get( t_CKINT i, t_CKFLOAT * val );
    t_CKINT get( const std::string & key, t_CKFLOAT * val );
    t_CKINT set( t_CKINT i, t_CKFLOAT val );
    t_CKINT set( const std::string & key, t_CKFLOAT val );
    t_CKINT push_back( t_CKFLOAT val );
    t_CKINT pop_back( );
    t_CKINT pop_out( t_CKINT pos );
    t_CKINT back( t_CKFLOAT * val ) const;
    void    zero( t_CKUINT start, t_CKUINT end );
    void    zero() { this->zero(0, m_vector.size()); }

    virtual void    clear( );
    virtual t_CKINT size( ) { return m_vector.size(); }
    virtual t_CKINT capacity( ) { return m_vector.capacity(); }
    virtual t_CKINT set_size( t_CKINT size );
    virtual t_CKINT set_capacity( t_CKINT capacity );
    virtual t_CKINT find( const std::string & key );
    virtual t_CKINT erase( const std::string & key );
    virtual t_CKINT data_type_size( ) { return CHUCK_ARRAY8_DATASIZE; }
    virtual t_CKINT data_type_kind( ) { return CHUCK_ARRAY8_DATAKIND; }
    // get map keys | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys );
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse();
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle();

public:
    std::vector<t_CKFLOAT> m_vector;
    std::map<std::string, t_CKFLOAT> m_map;
    // t_CKINT m_size;
    // t_CKINT m_capacity;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array16
// desc: native ChucK arrays (for 16-byte)
//-----------------------------------------------------------------------------
struct Chuck_Array16 : public Chuck_Array
{
public:
    Chuck_Array16( t_CKINT capacity = 8 );
    virtual ~Chuck_Array16();

public:
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    t_CKINT get( t_CKINT i, t_CKCOMPLEX * val );
    t_CKINT get( const std::string & key, t_CKCOMPLEX * val );
    t_CKINT set( t_CKINT i, const t_CKCOMPLEX & val );
    t_CKINT set( const std::string & key, const t_CKCOMPLEX & val );
    t_CKINT push_back( const t_CKCOMPLEX & val );
    t_CKINT pop_back( );
    t_CKINT pop_out( t_CKINT pos );
    t_CKINT back( t_CKCOMPLEX * val ) const;
    void    zero( t_CKUINT start, t_CKUINT end );
    void    zero() { this->zero(0, m_vector.size()); }

    virtual void    clear( );
    virtual t_CKINT size( ) { return m_vector.size(); }
    virtual t_CKINT capacity( ) { return m_vector.capacity(); }
    virtual t_CKINT set_size( t_CKINT size );
    virtual t_CKINT set_capacity( t_CKINT capacity );
    virtual t_CKINT find( const std::string & key );
    virtual t_CKINT erase( const std::string & key );
    virtual t_CKINT data_type_size( ) { return CHUCK_ARRAY16_DATASIZE; }
    virtual t_CKINT data_type_kind( ) { return CHUCK_ARRAY16_DATAKIND; }
    // get map keys | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys );
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse();
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle();

public:
    std::vector<t_CKCOMPLEX> m_vector;
    std::map<std::string, t_CKCOMPLEX> m_map;
    // t_CKINT m_size;
    // t_CKINT m_capacity;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array24
// desc: native ChucK arrays (for vec3)
//-----------------------------------------------------------------------------
struct Chuck_Array24 : public Chuck_Array
{
public:
    Chuck_Array24( t_CKINT capacity = 8 );
    virtual ~Chuck_Array24();

public:
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    t_CKINT get( t_CKINT i, t_CKVEC3 * val );
    t_CKINT get( const std::string & key, t_CKVEC3 * val );
    t_CKINT set( t_CKINT i, const t_CKVEC3 & val );
    t_CKINT set( const std::string & key, const t_CKVEC3 & val );
    t_CKINT push_back( const t_CKVEC3 & val );
    t_CKINT pop_back( );
    t_CKINT pop_out( t_CKUINT pos );
    t_CKINT back( t_CKVEC3 * val ) const;
    void    zero( t_CKUINT start, t_CKUINT end );
    void    zero() { this->zero(0, m_vector.size()); }

    virtual void    clear( );
    virtual t_CKINT size( ) { return m_vector.size(); }
    virtual t_CKINT capacity( ) { return m_vector.capacity(); }
    virtual t_CKINT set_size( t_CKINT size );
    virtual t_CKINT set_capacity( t_CKINT capacity );
    virtual t_CKINT find( const std::string & key );
    virtual t_CKINT erase( const std::string & key );
    virtual t_CKINT data_type_size( ) { return CHUCK_ARRAY24_DATASIZE; }
    virtual t_CKINT data_type_kind( ) { return CHUCK_ARRAY24_DATAKIND; }
    // get map keys | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys );
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse();
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle();

public:
    std::vector<t_CKVEC3> m_vector;
    std::map<std::string, t_CKVEC3> m_map;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array32
// desc: native ChucK arrays (for vec4)
//-----------------------------------------------------------------------------
struct Chuck_Array32 : public Chuck_Array
{
public:
    Chuck_Array32( t_CKINT capacity = 8 );
    virtual ~Chuck_Array32();

public:
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    t_CKINT get( t_CKINT i, t_CKVEC4 * val );
    t_CKINT get( const std::string & key, t_CKVEC4 * val );
    t_CKINT set( t_CKINT i, const t_CKVEC4 & val );
    t_CKINT set( const std::string & key, const t_CKVEC4 & val );
    t_CKINT push_back( const t_CKVEC4 & val );
    t_CKINT pop_back( );
    t_CKINT pop_out( t_CKUINT pos );
    t_CKINT back( t_CKVEC4 * val ) const;
    void    zero( t_CKUINT start, t_CKUINT end );
    void    zero() { this->zero(0, m_vector.size()); }

    virtual void    clear( );
    virtual t_CKINT size( ) { return m_vector.size(); }
    virtual t_CKINT capacity( ) { return m_vector.capacity(); }
    virtual t_CKINT set_size( t_CKINT size );
    virtual t_CKINT set_capacity( t_CKINT capacity );
    virtual t_CKINT find( const std::string & key );
    virtual t_CKINT erase( const std::string & key );
    virtual t_CKINT data_type_size( ) { return CHUCK_ARRAY32_DATASIZE; }
    virtual t_CKINT data_type_kind( ) { return CHUCK_ARRAY32_DATAKIND; }
    // get map keys | added (1.4.1.2) nshaheed
    virtual void get_keys( std::vector<std::string> & keys );
    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse();
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle();

public:
    std::vector<t_CKVEC4> m_vector;
    std::map<std::string, t_CKVEC4> m_map;
};




//-----------------------------------------------------------------------------
// name: enum Chuck_Global_Get_Callback_Type
// desc: enumeration for type of callback, used in globals
//-----------------------------------------------------------------------------
enum Chuck_Global_Get_Callback_Type
{
    ck_get_plain,
    ck_get_name,
    ck_get_id
};




//-----------------------------------------------------------------------------
// name: Chuck_Global_Event_Listener
// desc: base Listener struct
//-----------------------------------------------------------------------------
struct Chuck_Global_Event_Listener
{
    union {
        void (* void_callback)(void);
        void (* named_callback)(const char *);
        void (* id_callback)(t_CKINT);
    };
    t_CKBOOL listen_forever;
    Chuck_Global_Get_Callback_Type callback_type;
    std::string name;
    t_CKINT id;
    Chuck_Global_Event_Listener() : void_callback(NULL), listen_forever(FALSE),
        callback_type(ck_get_plain), name(""), id(0) {};
};




//-----------------------------------------------------------------------------
// name: Chuck_Event
// desc: base Chuck Event class
//-----------------------------------------------------------------------------
struct Chuck_Event : public Chuck_Object
{
public:
    // signal/broadcast "local" -- signal ChucK Events
    void signal_local();
    void broadcast_local();
    void wait( Chuck_VM_Shred * shred, Chuck_VM * vm );
    t_CKBOOL remove( Chuck_VM_Shred * shred );

public:
    // 1.4.0.0 (jack): global events -- signal/broadcast code running
    // externally, elsewhere in the host
    void signal_global();
    void broadcast_global();

    // 1.4.1.0 (jack): global events multiple callbacks.
    // can listen with a callback(), callback( event name ), or callback( id )
    void global_listen( void (* cb)(void), t_CKBOOL listen_forever );
    void global_listen( std::string name, void (* cb)(const char *), t_CKBOOL listen_forever );
    void global_listen( t_CKINT id, void (* cb)(t_CKINT), t_CKBOOL listen_forever );
    t_CKBOOL remove_listen( void (* cb)(void) );
    t_CKBOOL remove_listen( std::string name, void (* cb)(const char *) );
    t_CKBOOL remove_listen( t_CKINT id, void (* cb)(t_CKINT) );

public: // internal
    // added 1.3.0.0: queue_broadcast now takes event_buffer
    void queue_broadcast( CBufferSimple * event_buffer = NULL );

public:
    static t_CKUINT our_can_wait;

protected:
    std::queue<Chuck_VM_Shred *> m_queue;
    std::queue<Chuck_Global_Event_Listener> m_global_queue;

    #ifndef __DISABLE_THREADS__
    // 1.4.1.0 (ge/jack) TODO: rewrite queue_broadcast to use a lock-free queue
    // and avoid the use of a lock in events
    XMutex m_queue_lock;
    #endif
};




//-----------------------------------------------------------------------------
// name: Chuck_String
// desc: base Chuck string class
//-----------------------------------------------------------------------------
struct Chuck_String : public Chuck_Object
{
public:
    // constructor
    Chuck_String( const std::string & s = "" ) { set( s ); }
    // destructor | 1.5.0.1 (ge) made destructor virtual
    virtual ~Chuck_String() { }

    // set string (makes copy)
    void set( const std::string & s ) { m_str = s; m_charptr = m_str.c_str(); }
    // get as standard c++ string
    const std::string & str() { return m_str; }
    // get as C string (NOTE: use this in dynamical modules like chugins!)
    const char * c_str() { return m_charptr; }

private:
    // c pointer | HACK: needed for ensure string passing in dynamic modules
    const char * m_charptr; // REFACTOR-2017
    // c++ string
    std::string m_str;
};




#endif
