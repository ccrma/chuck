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
    // output type info (can be overriden; but probably shouldn't be)
    virtual void help();
    // output current state (can be overridden)
    virtual void dump();

public:
    // virtual table
    Chuck_VTable * vtable;
    // reference to type
    Chuck_Type * type_ref;
    // data for the object
    t_CKBYTE * data;
    // the size of the data region
    t_CKUINT data_size;

public:
    // set VM on which this object was instantiated | 1.5.1.5
    void setOriginVM( Chuck_VM * vm );
    // set shred on which this object was instantiated | 1.5.1.5
    void setOriginShred( Chuck_VM_Shred * shred );
    // get VM on which this object was instantiated | 1.5.1.5
    Chuck_VM * originVM() const { return origin_vm; }
    // get shred on which this object was instantiated | 1.5.1.5
    Chuck_VM_Shred * originShred() const { return origin_shred; }

protected:
    // the shred on which this object was instantiated | 1.5.1.5
    Chuck_VM_Shred * origin_shred;
    // the VM on which this object was instantiated | 1.5.1.5
    Chuck_VM * origin_vm;

public: // static
    // vtable offset for toString()
    static t_CKUINT our_vt_toString;
};




// ISSUE: 64-bit (fixed 1.3.1.0)
#define CHUCK_ARRAYINT_DATASIZE sz_INT
#define CHUCK_ARRAYFLOAT_DATASIZE sz_FLOAT
#define CHUCK_ARRAY16_DATASIZE sz_COMPLEX
#define CHUCK_ARRAY24_DATASIZE sz_VEC3 // 1.3.5.3
#define CHUCK_ARRAY32_DATASIZE sz_VEC4 // 1.3.5.3
#define CHUCK_ARRAYINT_DATAKIND kindof_INT
#define CHUCK_ARRAYFLOAT_DATAKIND kindof_FLOAT
#define CHUCK_ARRAY16_DATAKIND kindof_VEC2 // 1.5.1.7
#define CHUCK_ARRAY24_DATAKIND kindof_VEC3 // 1.3.5.3
#define CHUCK_ARRAY32_DATAKIND kindof_VEC4 // 1.3.5.3
//-----------------------------------------------------------------------------
// name: struct Chuck_Array
// desc: native ChucK array (virtual base class)
//       NOTE due to certain specialized operations, e.g., ArrayInt is also
//       used to hold and manage Object references, this class is currently
//       not templated (and probably cannot be fully templated). For this
//       reason, some functionalities that requires type-specific arguments
//       (like get/set and insert) are not in the base class and are
//       implemented in array subclasses
//-----------------------------------------------------------------------------
struct Chuck_Array : public Chuck_Object
{
public:
    // constructor
    Chuck_Array();
    // destructor
    virtual ~Chuck_Array();

public: // interface common to all Chuck_Arary*
    // get array size
    virtual t_CKINT size() = 0;
    // get array capacity
    virtual t_CKINT capacity() = 0;
    // set array size
    virtual t_CKINT set_size( t_CKINT size ) = 0;
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity ) = 0;
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() = 0;
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() = 0;
    // clear
    virtual void clear() = 0;
    // zero out the array by region [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end ) = 0;
    // zero out all elements
    virtual void zero() = 0;

    // pop the back of the array | 1.5.0.8 (ge) moved into common
    virtual t_CKINT pop_back() = 0;
    // pop the front of array | 1.5.0.8 (ge) added
    virtual t_CKINT pop_front() = 0;
    // erase | 1.5.0.8 (ge) was pop_out()
    virtual t_CKINT erase( t_CKINT pos ) = 0;
    // erase by range [begin,end) | 1.5.0.8 (ge) added
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end ) = 0;

    // reverse array order | added (1.5.0.0) azaday
    virtual void reverse() = 0;
    // reverse array order | added (1.5.0.0) kunwoo, nshaheed, azaday, ge
    virtual void shuffle() = 0;
    // sort contents of array in ascending order | added (1.5.1.0) ge
    virtual void sort() = 0;

    // typed as Object array? | 1.5.0.8 (ge) moved to common
    virtual t_CKBOOL contains_objects() { return FALSE; }

public: // map-only operations
    // get all keys in map | added (1.4.2.0) nshaheed
    virtual void get_keys( std::vector<std::string> & keys ) = 0;
    // (map only) test is key is in map | 1.5.0.8 (ge) was find()
    virtual t_CKINT map_find( const std::string & key ) = 0;
    // (map only) erase entry by key | 1.5.0.8 (ge) was erase()
    virtual t_CKINT map_erase( const std::string & key ) = 0;

public:
    // Chuck_Type * m_array_type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_ArrayInt
// desc: native ChucK arrays
//       (for 4-byte/8-byte int, depending on 32-bit vs 64-bit & Object refs)
//       1.5.1.5 (ge) renamed from Chuck_Array4 to Chuck_ArrayInt
//-----------------------------------------------------------------------------
struct Chuck_ArrayInt : public Chuck_Array
{
public:
    Chuck_ArrayInt( t_CKBOOL is_obj, t_CKINT capacity = 8 );
    virtual ~Chuck_ArrayInt();

public: // specific to this class
    // get address
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    // get value
    t_CKINT get( t_CKINT i, t_CKUINT * val );
    t_CKINT get( const std::string & key, t_CKUINT * val );
    // set value
    t_CKINT set( t_CKINT i, t_CKUINT val );
    t_CKINT set( const std::string & key, t_CKUINT val );

    // append value
    t_CKINT push_back( t_CKUINT val );
    // get value of last element
    t_CKINT back( t_CKUINT * val ) const;
    // prepend value | 1.5.0.8 (ge) added
    // NOTE linear time O(n) operation
    t_CKINT push_front( t_CKUINT val );
    // get value of last element
    t_CKINT front( t_CKUINT * val ) const;
    // insert before position | 1.5.0.8 (ge)
    // NOTE linear time O(n) operation
    t_CKINT insert( t_CKINT pos, t_CKUINT val );

public: // array interface implementation
    // get array size
    virtual t_CKINT size() { return m_vector.size(); }
    // get array capacity
    virtual t_CKINT capacity() { return m_vector.capacity(); }
    // set array size
    virtual t_CKINT set_size( t_CKINT size );
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity );
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() { return CHUCK_ARRAYINT_DATASIZE; }
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() { return CHUCK_ARRAYINT_DATAKIND; }
    // clear
    virtual void clear();
    // zero out the array by range [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end );
    // zero out all elements
    virtual void zero() { this->zero(0, m_vector.size()); }

    // pop the back of the array
    virtual t_CKINT pop_back();
    // pop the front of array
    virtual t_CKINT pop_front();
    // pop out an element
    virtual t_CKINT erase( t_CKINT pos );
    // erase a range of elements [begin,end)
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end );

    // reverse array order
    virtual void reverse();
    // reverse array order
    virtual void shuffle();
    // sort contents of array in ascending order
    virtual void sort();

    // typed as Object array?
    virtual t_CKBOOL contains_objects() { return m_is_obj; }

public: // map only
    // get all keys in map
    virtual void get_keys( std::vector<std::string> & keys );
    // (map only) test is key is in map
    virtual t_CKINT map_find( const std::string & key );
    // (map only) erase entry by key
    virtual t_CKINT map_erase( const std::string & key );

public:
    std::vector<t_CKUINT> m_vector;
    std::map<std::string, t_CKUINT> m_map;
    t_CKBOOL m_is_obj;

    // TODO: may need additional information here for set_size, if this is part of a multi-dim array

    // t_CKINT m_size;
    // t_CKINT m_capacity;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_ArrayFloat
// desc: native ChucK arrays (for 8-byte float)
//       1.5.1.5 (ge) renamed from Chuck_Array8 to Chuck_ArrayFloat
//-----------------------------------------------------------------------------
struct Chuck_ArrayFloat : public Chuck_Array
{
public:
    Chuck_ArrayFloat( t_CKINT capacity = 8 );
    virtual ~Chuck_ArrayFloat();

public: // specific to this class
    // get address
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    // get value
    t_CKINT get( t_CKINT i, t_CKFLOAT * val );
    t_CKINT get( const std::string & key, t_CKFLOAT * val );
    // set value
    t_CKINT set( t_CKINT i, t_CKFLOAT val );
    t_CKINT set( const std::string & key, t_CKFLOAT val );

    // append value
    t_CKINT push_back( t_CKFLOAT val );
    // get value of last element
    t_CKINT back( t_CKFLOAT * val ) const;
    // prepend value | 1.5.0.8 (ge) added
    // NOTE linear time O(n) operation
    t_CKINT push_front( t_CKFLOAT val );
    // get value of last element
    t_CKINT front( t_CKFLOAT * val ) const;
    // insert before position | 1.5.0.8 (ge)
    // NOTE linear time O(n) operation
    t_CKINT insert( t_CKINT pos, t_CKFLOAT val );

public: // array interface implementation
    // get array size
    virtual t_CKINT size() { return m_vector.size(); }
    // get array capacity
    virtual t_CKINT capacity() { return m_vector.capacity(); }
    // set array size
    virtual t_CKINT set_size( t_CKINT size );
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity );
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() { return CHUCK_ARRAYFLOAT_DATASIZE; }
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() { return CHUCK_ARRAYFLOAT_DATAKIND; }
    // clear
    virtual void clear();
    // zero out the array by range [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end );
    // zero out all elements
    virtual void zero() { this->zero(0, m_vector.size()); }

    // pop the back of the array
    virtual t_CKINT pop_back();
    // pop the front of array
    virtual t_CKINT pop_front();
    // pop out an element
    virtual t_CKINT erase( t_CKINT pos );
    // erase a range of elements [begin,end)
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end );

    // reverse array order
    virtual void reverse();
    // reverse array order
    virtual void shuffle();
    // sort contents of array in ascending order
    virtual void sort();

public: // map only
    // get all keys in map
    virtual void get_keys( std::vector<std::string> & keys );
    // (map only) test is key is in map
    virtual t_CKINT map_find( const std::string & key );
    // (map only) erase entry by key
    virtual t_CKINT map_erase( const std::string & key );

public:
    std::vector<t_CKFLOAT> m_vector;
    std::map<std::string, t_CKFLOAT> m_map;
    // t_CKINT m_size;
    // t_CKINT m_capacity;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Array16
// desc: native ChucK arrays (for 16-byte) | vec2, complex, polar
//-----------------------------------------------------------------------------
struct Chuck_Array16 : public Chuck_Array
{
public:
    Chuck_Array16( t_CKINT capacity = 8 );
    virtual ~Chuck_Array16();

public: // specific to this class
    // get address
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    // get value
    t_CKINT get( t_CKINT i, t_CKVEC2 * val );
    t_CKINT get( t_CKINT i, t_CKCOMPLEX * val );
    t_CKINT get( const std::string & key, t_CKVEC2 * val );
    t_CKINT get( const std::string & key, t_CKCOMPLEX * val );
    // set value
    t_CKINT set( t_CKINT i, const t_CKVEC2 & val );
    t_CKINT set( t_CKINT i, const t_CKCOMPLEX & val );
    t_CKINT set( const std::string & key, const t_CKVEC2 & val );
    t_CKINT set( const std::string & key, const t_CKCOMPLEX & val );

    // append value
    t_CKINT push_back( const t_CKVEC2 & val );
    // get value of last element
    t_CKINT back( t_CKVEC2 * val ) const;
    // prepend value | 1.5.0.8 (ge) added
    // NOTE linear time O(n) operation
    t_CKINT push_front( const t_CKVEC2 & val );
    // get value of last element
    t_CKINT front( t_CKVEC2 * val ) const;
    // insert before position | 1.5.0.8 (ge)
    // NOTE linear time O(n) operation
    t_CKINT insert( t_CKINT pos, const t_CKVEC2 & val );

public: // array interface implementation
    // get array size
    virtual t_CKINT size() { return m_vector.size(); }
    // get array capacity
    virtual t_CKINT capacity() { return m_vector.capacity(); }
    // set array size
    virtual t_CKINT set_size( t_CKINT size );
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity );
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() { return CHUCK_ARRAY16_DATASIZE; }
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() { return CHUCK_ARRAY16_DATAKIND; }
    // clear
    virtual void clear();
    // zero out the array by range [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end );
    // zero out all elements
    virtual void zero() { this->zero(0, m_vector.size()); }

    // pop the back of the array
    virtual t_CKINT pop_back();
    // pop the front of array
    virtual t_CKINT pop_front();
    // pop out an element
    virtual t_CKINT erase( t_CKINT pos );
    // erase a range of elements [begin,end)
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end );

    // reverse array order
    virtual void reverse();
    // reverse array order
    virtual void shuffle();
    // sort contents of array in ascending order
    virtual void sort();

public: // map only
    // get all keys in map
    virtual void get_keys( std::vector<std::string> & keys );
    // (map only) test is key is in map
    virtual t_CKINT map_find( const std::string & key );
    // (map only) erase entry by key
    virtual t_CKINT map_erase( const std::string & key );

public:
    std::vector<t_CKVEC2> m_vector;
    std::map<std::string, t_CKVEC2> m_map;
    // semantic hint; in certain situations (like sorting)
    // need to distinguish between complex and polar | 1.5.1.0
    t_CKBOOL m_isPolarType;
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

public: // specific to this class
    // get address
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    // get value
    t_CKINT get( t_CKINT i, t_CKVEC3 * val );
    t_CKINT get( const std::string & key, t_CKVEC3 * val );
    // set value
    t_CKINT set( t_CKINT i, const t_CKVEC3 & val );
    t_CKINT set( const std::string & key, const t_CKVEC3 & val );

    // append value
    t_CKINT push_back( const t_CKVEC3 & val );
    // get value of last element
    t_CKINT back( t_CKVEC3 * val ) const;
    // prepend value | 1.5.0.8 (ge) added
    // NOTE linear time O(n) operation
    t_CKINT push_front( const t_CKVEC3 & val );
    // get value of last element
    t_CKINT front( t_CKVEC3 * val ) const;
    // insert before position | 1.5.0.8 (ge)
    // NOTE linear time O(n) operation
    t_CKINT insert( t_CKINT pos, const t_CKVEC3 & val );

public: // array interface implementation
    // get array size
    virtual t_CKINT size() { return m_vector.size(); }
    // get array capacity
    virtual t_CKINT capacity() { return m_vector.capacity(); }
    // set array size
    virtual t_CKINT set_size( t_CKINT size );
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity );
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() { return CHUCK_ARRAY24_DATASIZE; }
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() { return CHUCK_ARRAY24_DATAKIND; }
    // clear
    virtual void clear();
    // zero out the array by range [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end );
    // zero out all elements
    virtual void zero() { this->zero(0, m_vector.size()); }

    // pop the back of the array
    virtual t_CKINT pop_back();
    // pop the front of array
    virtual t_CKINT pop_front();
    // pop out an element
    virtual t_CKINT erase( t_CKINT pos );
    // erase a range of elements [begin,end)
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end );

    // reverse array order
    virtual void reverse();
    // reverse array order
    virtual void shuffle();
    // sort contents of array in ascending order
    virtual void sort();

public: // map only
    // get all keys in map
    virtual void get_keys( std::vector<std::string> & keys );
    // (map only) test is key is in map
    virtual t_CKINT map_find( const std::string & key );
    // (map only) erase entry by key
    virtual t_CKINT map_erase( const std::string & key );

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

public: // specific to this class
    // get address
    t_CKUINT addr( t_CKINT i );
    t_CKUINT addr( const std::string & key );
    // get value
    t_CKINT get( t_CKINT i, t_CKVEC4 * val );
    t_CKINT get( const std::string & key, t_CKVEC4 * val );
    // set value
    t_CKINT set( t_CKINT i, const t_CKVEC4 & val );
    t_CKINT set( const std::string & key, const t_CKVEC4 & val );

    // append value
    t_CKINT push_back( const t_CKVEC4 & val );
    // get value of last element
    t_CKINT back( t_CKVEC4 * val ) const;
    // prepend value | 1.5.0.8 (ge) added
    // NOTE linear time O(n) operation
    t_CKINT push_front( const t_CKVEC4 & val );
    // get value of last element
    t_CKINT front( t_CKVEC4 * val ) const;
    // insert before position | 1.5.0.8 (ge)
    // NOTE linear time O(n) operation
    t_CKINT insert( t_CKINT pos, const t_CKVEC4 & val );

public: // array interface implementation
    // get array size
    virtual t_CKINT size() { return m_vector.size(); }
    // get array capacity
    virtual t_CKINT capacity() { return m_vector.capacity(); }
    // set array size
    virtual t_CKINT set_size( t_CKINT size );
    // set array capacity
    virtual t_CKINT set_capacity( t_CKINT capacity );
    // size of stored type (from type_ref)
    virtual t_CKINT data_type_size() { return CHUCK_ARRAY32_DATASIZE; }
    // kind of stored type (from kindof)
    virtual t_CKINT data_type_kind() { return CHUCK_ARRAY32_DATAKIND; }
    // clear
    virtual void clear();
    // zero out the array by range [start,end)
    virtual void zero( t_CKUINT start, t_CKUINT end );
    // zero out all elements
    virtual void zero() { this->zero(0, m_vector.size()); }

    // pop the back of the array
    virtual t_CKINT pop_back();
    // pop the front of array
    virtual t_CKINT pop_front();
    // pop out an element
    virtual t_CKINT erase( t_CKINT pos );
    // erase a range of elements [begin,end)
    virtual t_CKINT erase( t_CKINT begin, t_CKINT end );

    // reverse array order
    virtual void reverse();
    // reverse array order
    virtual void shuffle();
    // sort contents of array in ascending order
    virtual void sort();

public: // map only
    // get all keys in map
    virtual void get_keys( std::vector<std::string> & keys );
    // (map only) test is key is in map
    virtual t_CKINT map_find( const std::string & key );
    // (map only) erase entry by key
    virtual t_CKINT map_erase( const std::string & key );

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
    // virtual table offset for can_wait()
    static t_CKUINT our_can_wait;
    // virtual table offset for waiting_on() | 1.5.1.5 (ge/andrew) added
    static t_CKUINT our_waiting_on;

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
