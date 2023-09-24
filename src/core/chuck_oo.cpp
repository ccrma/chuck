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
// file: chuck_oo.cpp
// desc: chuck baes objects
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Andrew Schran (aschran@princeton.edu) - fileIO implementation
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "chuck_oo.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_instr.h"
#include "chuck_errmsg.h"
#include "chuck_dl.h"
#include "util_math.h"

#include <algorithm>
#include <iostream>
using namespace std;


// initialize
t_CKBOOL Chuck_VM_Object::our_locks_in_effect = TRUE;




//-----------------------------------------------------------------------------
// name: Chuck_VM_Object()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_VM_Object::Chuck_VM_Object()
{
    // initialize reference
    this->init_ref();

    // track this only in CK_VM_DEBUG_ENABLED mode | 1.5.0.5 (ge)
    CK_VM_DEBUGGER( construct( this ) );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_VM_Object()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_VM_Object::~Chuck_VM_Object()
{ }




//-----------------------------------------------------------------------------
// name: init_ref()
// desc: initialize vm object
//-----------------------------------------------------------------------------
void Chuck_VM_Object::init_ref()
{
    // set reference count
    m_ref_count = 0;
    // set flag
    m_pooled = FALSE;
    // set to not locked
    m_locked = FALSE;
}




//-----------------------------------------------------------------------------
// name: add_ref()
// desc: add reference
//-----------------------------------------------------------------------------
void Chuck_VM_Object::add_ref()
{
    // increment reference count
    m_ref_count++;

    // added 1.3.0.0
    // CK_VM_DEBUG( CK_FPRINTF_STDERR( "Chuck_VM_Object::add_ref() : 0x%08x, %s, %lu\n", this, mini_type(typeid(*this).name()), m_ref_count) );
    // updated 1.5.0.5 to use Chuck_VM_Debug
    CK_VM_DEBUGGER( add_ref( this ) );
}




//-----------------------------------------------------------------------------
// name: release()
// desc: remove reference
//-----------------------------------------------------------------------------
void Chuck_VM_Object::release()
{
    //-----------------------------------------------------------------------------
    // release is permitted even if ref-count is already 0 | 1.5.1.0 (ge)
    //-----------------------------------------------------------------------------
    if( m_ref_count <= 0 )
    {
        // print warning
        EM_log( CK_LOG_FINEST, "(internal) Object.release() refcount already %d", m_ref_count );

        // print error
        // EM_error3( "[chuck]: (internal error) Object.release() refcount == %d", m_ref_count );
        // make sure there is at least one reference
        // assert( m_ref_count > 0 );
    }
    else
    {
        // decrement
        m_ref_count--;
    }

    // added 1.3.0.0
    // CK_VM_DEBUG(CK_FPRINTF_STDERR( "Chuck_VM_Object::release() : 0x%08x, %s, %ulu\n", this, mini_type(typeid(*this).name()), m_ref_count));
    // updated 1.5.0.5 to use Chuck_VM_Debug
    CK_VM_DEBUGGER( release( this ) );

    // if no more references
    if( m_ref_count == 0 )
    {
        // this is not good | TODO: our_locks_in_effect assumes single VM
        if( our_locks_in_effect && m_locked )
        {
            EM_error2( 0, "(internal error) releasing locked VM object!" );
            // fail
            assert( FALSE );
            // in case assert is disabled
            *(int *)0 = 1;
        }

    #ifndef __CHUNREAL_ENGINE__
        // log | 1.5.0.0 (ge) added
        EM_log( CK_LOG_FINEST, "reclaiming %s: 0x%08x", typeid(*this).name(), this );
    #else
        // #chunreal UE seems to disallow real-time type info
        EM_log( CK_LOG_FINEST, "reclaiming object: 0x%08x", this );
    #endif // #ifndef __CHUNREAL_ENGINE__

        // track | 1.5.0.5 (ge)
        CK_VM_DEBUGGER( destruct( this ) );

        // REFACTOR-2017: doing this for now
        delete this;
    }
}




//-----------------------------------------------------------------------------
// name: lock()
// desc: lock to keep from deleted
//-----------------------------------------------------------------------------
void Chuck_VM_Object::lock()
{
    m_locked = TRUE;
}




//-----------------------------------------------------------------------------
// name: unlock()
// desc: unlock to allow deleting
//-----------------------------------------------------------------------------
void Chuck_VM_Object::unlock()
{
    m_locked = FALSE;
}




//-----------------------------------------------------------------------------
// name: lock_all()
// desc: disallow deletion of locked objects
//-----------------------------------------------------------------------------
void Chuck_VM_Object::lock_all()
{
    // log
    EM_log( CK_LOG_SEVERE, "locking down special objects..." );
    // set flag
    our_locks_in_effect = TRUE;
}




//-----------------------------------------------------------------------------
// name: unlock_all()
// desc: allow deletion of locked objects (USE WITH CAUTION!)
//-----------------------------------------------------------------------------
void Chuck_VM_Object::unlock_all()
{
    // log
    EM_log( CK_LOG_SEVERE, "unlocking special objects..." );
    // set flag
    our_locks_in_effect = FALSE;
}




//-----------------------------------------------------------------------------
// name: refcount()
// desc: get reference count
//-----------------------------------------------------------------------------
t_CKUINT Chuck_VM_Object::refcount() const
{
    return m_ref_count;
}




//-----------------------------------------------------------------------------
// name: Chuck_Object()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Object::Chuck_Object()
{
    // zero virtual table
    vtable = NULL;
    // zero type
    type_ref = NULL;
    // zero data
    data = NULL;
    // zero size
    data_size = 0;
}




//-----------------------------------------------------------------------------
// name: Chuck_Object()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Object::~Chuck_Object()
{
    // added 1.3.0.0:
    // call destructors, from latest descended child to oldest parent
    Chuck_Type * type = this->type_ref;
    while( type != NULL )
    {
        // SPENCER TODO: HACK! is there a better way to call the dtor?
        if( type->info && type->has_destructor ) // 1.5.0.0 (ge) added type->info check
        {
            // sanity check
            assert( type->info->dtor && type->info->dtor->native_func );
            // REFACTOR-2017: do we know which VM to pass in? (currently NULL)
            ((f_dtor)(type->info->dtor->native_func))( this, NULL, NULL, Chuck_DL_Api::Api::instance() );
        }

        // go up the inheritance
        type = type->parent;
    }

    // free
    CK_SAFE_DELETE( vtable );
    CK_SAFE_RELEASE( type_ref );
    CK_SAFE_DELETE_ARRAY( data );
}




//-----------------------------------------------------------------------------
// name: dump()
// desc: output current state (can be overridden)
//-----------------------------------------------------------------------------
void Chuck_Object::dump() // 1.4.1.1 (ge)
{
    // need type info
    Chuck_Type * type = this->type_ref;

    // output state with type info
    type->dump_obj( this );
}




//-----------------------------------------------------------------------------
// name: help()
// desc: output type info (can be overriden; but probably shouldn't be)
//-----------------------------------------------------------------------------
void Chuck_Object::help() // 1.4.1.0 (ge)
{
    // type to unpack
    Chuck_Type * type = this->type_ref;

    // unpack type and output its info
    type->apropos();
}




//-----------------------------------------------------------------------------
// name: Chuck_Array()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array::Chuck_Array() { }




//-----------------------------------------------------------------------------
// name: ~Chuck_Array()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array::~Chuck_Array() { }




//-----------------------------------------------------------------------------
// name: Chuck_Array4()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array4::Chuck_Array4( t_CKBOOL is_obj, t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear (as non-object, so no releases)
    m_is_obj = FALSE;
    this->zero( 0, m_vector.capacity() );
    // is object (set after clear)
    m_is_obj = is_obj;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Array4()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array4::~Chuck_Array4()
{
    // 1.4.2.0 (ge) | added, which should cascade to nested array objects
    clear();
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: return address of element at position i, as an int
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array4::addr( t_CKINT i )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the addr
    return (t_CKUINT)(&m_vector[i]);
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: return address of element at key, as an int
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array4::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get value of element at position i
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::get( t_CKINT i, t_CKUINT * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get value of element at key
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::get( const string & key, t_CKUINT * val )
{
    // set to zero
    *val = 0;
    // find
    map<string, t_CKUINT>::iterator iter = m_map.find( key );
    // check
    if( iter != m_map.end() ) *val = (*iter).second;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: set element of array by position, with ref counting as applicable
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::set( t_CKINT i, t_CKUINT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    t_CKUINT v = m_vector[i];

    // if obj
    if( m_is_obj && v ) ((Chuck_Object *)v)->release();

    // set the value
    m_vector[i] = val;

    // if obj
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // return good
    return 1;
}





//-----------------------------------------------------------------------------
// name: set()
// desc: set element of map by key, with ref counting as applicable
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::set( const string & key, t_CKUINT val )
{
    map<string, t_CKUINT>::iterator iter = m_map.find( key );

    // if obj
    if( m_is_obj && iter != m_map.end() )
        ((Chuck_Object *)(*iter).second)->release();

    if( !val ) m_map.erase( key );
    else m_map[key] = val;

    // if obj
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::insert( t_CKINT i, t_CKUINT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // insert the value
    m_vector.insert( m_vector.begin()+i, val );

    // if obj, bump reference count
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: map_find()
// desc: (map only) test if key is in map
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: (map only) erase element by key
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::map_erase( const string & key )
{
    map<string, t_CKUINT>::iterator iter = m_map.find( key );
    t_CKINT v = iter != m_map.end();

    // if obj
    if( m_is_obj && iter != m_map.end() )
        ((Chuck_Object *)(*iter).second)->release();

    // erase
    if( v ) m_map.erase( key );

    return v;
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: append element by value
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::push_back( t_CKUINT val )
{
    // if obj, reference count it (added 1.3.0.0)
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: pop the last element in vector
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::pop_back()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // if obj
    if( m_is_obj )
    {
        // get pointer
        Chuck_Object * v = (Chuck_Object *)m_vector[m_vector.size()-1];
        // if not null, release
        if( v ) v->release();
    }

    // zero
    m_vector[m_vector.size()-1] = 0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::push_front( t_CKUINT val )
{
    // if obj, reference count it (added 1.3.0.0)
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // if obj
    if( m_is_obj )
    {
        // get pointer
        Chuck_Object * v = (Chuck_Object *)m_vector[m_vector.size()-1];
        // if not null, release
        if( v ) v->release();
    }

    // add to vector
    m_vector.erase( m_vector.begin() );

    return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase element by position
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::erase( t_CKINT pos )
{
    // check
    if( m_vector.size() == 0 || pos < 0 || pos >= m_vector.size() )
        return 0;

    if( m_is_obj )
    {
        // get pointer
        Chuck_Object * v = (Chuck_Object *)m_vector[pos];
        // if not null, release
        if( v ) v->release();
    }

    // add to vector
    m_vector.erase( m_vector.begin()+pos );

    return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase a range of elements [begin,end)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::erase( t_CKINT begin, t_CKINT end )
{
    // if needed swap the two
    if( begin > end ) { t_CKINT temp = begin; begin = end; end = temp; }
    // bound beginning
    if( begin < 0 ) begin = 0;
    // bound end
    if( end > m_vector.size() ) end = m_vector.size();

    // check
    if( m_vector.size() == 0 || begin >= m_vector.size() )
        return 0;

    // if contains Object refs
    if( m_is_obj )
    {
        // loop over range
        for( t_CKINT i = begin; i < end; i++ )
        {
            // get pointer
            Chuck_Object * v = (Chuck_Object *)m_vector[i];
            // if not null, release
            if( v ) v->release();
        }
    }

    // add to vector
    m_vector.erase( m_vector.begin()+begin, m_vector.begin()+end );

    return 1;
}




//-----------------------------------------------------------------------------
// name: get_keys() | 1.4.1.1 nshaheed (added)
// desc: return vector of keys from associative array
//-----------------------------------------------------------------------------
void Chuck_Array4::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string, t_CKUINT>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}




t_CKINT my_ck_random( t_CKINT i ) { return ck_random() % i;}
//-----------------------------------------------------------------------------
// name: my_random_shuffle() | 1.5.0.0
// desc: random shuffle an array
// adapted from https://en.cppreference.com/w/cpp/algorithm/random_shuffle
//-----------------------------------------------------------------------------
template<class RandomIt>
static void my_random_shuffle( RandomIt first, RandomIt last )
{
    typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
    // iterate
    for( diff_t i = last - first - 1; i > 0; --i )
    {
        // swap
        std::swap(first[i], first[my_ck_random(i + 1)]);
    }
}
//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_Array4::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_Array4::reverse()
{
    std::reverse( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: ck_compare_int()
// desc: compare function for sorting uints as signed int
//-----------------------------------------------------------------------------
static bool ck_compare_sint( t_CKUINT lhs, t_CKUINT rhs )
{
    // sort by 2-norm / magnitude
    return (t_CKINT)lhs < (t_CKINT)rhs;
}
//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
//-----------------------------------------------------------------------------
void Chuck_Array4::sort()
{
    // if object references sort as unsigned
    if( m_is_obj ) std::sort( m_vector.begin(), m_vector.end() );
    // if not object references, sort as signed ints
    else std::sort( m_vector.begin(), m_vector.end(), ck_compare_sint );
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::back( t_CKUINT * val ) const
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // get
    *val = m_vector.back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array4::clear( )
{
    // zero
    zero( 0, m_vector.size() );

    // clear vector
    m_vector.clear();
}




//-----------------------------------------------------------------------------
// name: set_capacity()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::set_capacity( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );

    // ensure size (removed 1.4.1.0 in favor of actually setting capacity)
    // set_size( capacity );

    // if clearing size
    if( capacity < m_vector.size() )
    {
        // zero out section
        zero( capacity, m_vector.size() );
    }

    // what the size was
    t_CKINT capacity_prev = m_vector.capacity();
    // reserve vector
    m_vector.reserve( capacity );

    // if clearing size
    if( m_vector.capacity() > capacity_prev )
    {
        // zero out section
        zero( capacity_prev, m_vector.capacity() );
    }

    return m_vector.capacity();
}




//-----------------------------------------------------------------------------
// name: set_size()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array4::set_size( t_CKINT size )
{
    // sanity check
    assert( size >= 0 );

    // if clearing size
    if( size < m_vector.size() )
    {
        // zero out section
        zero( size, m_vector.size() );
    }

    // what the size was
    t_CKINT size2 = m_vector.size();
    // resize vector
    m_vector.resize( size );

    // if clearing size
    if( m_vector.size() > size2 )
    {
        // zero out section
        zero( size2, m_vector.size() );
    }

    return m_vector.size();
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array4::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    // if contains objects
    if( m_is_obj )
    {
        Chuck_Object * v = NULL;
        for( t_CKUINT i = start; i < end; i++ )
        {
            // get it
            v = (Chuck_Object *)m_vector[i];
            // release
            if( v )
            {
                v->release();
                m_vector[i] = 0;
            }
        }
    }
    else
    {
        for( t_CKUINT i = start; i < end; i++ )
        {
            // zero
            m_vector[i] = 0;
        }
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_Array8()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array8::Chuck_Array8( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Array8()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array8::~Chuck_Array8()
{
    // do nothing
    clear();
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array8::addr( t_CKINT i )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the addr
    return (t_CKUINT)(&m_vector[i]);
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array8::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::get( t_CKINT i, t_CKFLOAT * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::get( const string & key, t_CKFLOAT * val )
{
    // set to zero
    *val = 0.0;

    // iterator
    map<string, t_CKFLOAT>::iterator iter = m_map.find( key );

    // check
    if( iter != m_map.end() )
    {
        // get the value
        *val = (*iter).second;
    }

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::set( t_CKINT i, t_CKFLOAT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // set the value
    m_vector[i] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::set( const string & key, t_CKFLOAT val )
{
    // 1.3.1.1: removed this
    // map<string, t_CKFLOAT>::iterator iter = m_map.find( key );

    // 1.3.5.3: removed this
    // if( !val ) m_map.erase( key ); else

    // insert
    m_map[key] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::insert( t_CKINT i, t_CKFLOAT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // insert the value
    m_vector.insert( m_vector.begin()+i, val );

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: map_find()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::push_back( t_CKFLOAT val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::pop_back( )
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // zero
    m_vector[m_vector.size()-1] = 0.0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::push_front( t_CKFLOAT val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin() );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::erase( t_CKINT pos )
{
        // check
        if ( m_vector.size() == 0 || pos<0 || pos>=m_vector.size())
                return 0;

        // add to vector
        m_vector.erase(m_vector.begin()+pos);
        return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase a range of elements [begin,end)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::erase( t_CKINT begin, t_CKINT end )
{
    // if needed swap the two
    if( begin > end ) { t_CKINT temp = begin; begin = end; end = temp; }
    // bound beginning
    if( begin < 0 ) begin = 0;
    // bound end
    if( end > m_vector.size() ) end = m_vector.size();

    // check
    if( m_vector.size() == 0 || begin >= m_vector.size() )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin()+begin, m_vector.begin()+end );

    return 1;
}




//-----------------------------------------------------------------------------
// name: get_keys() | 1.4.1.1 nshaheed (added)
// desc: return vector of keys from associative array
//-----------------------------------------------------------------------------
void Chuck_Array8::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string,t_CKFLOAT>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_Array8::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_Array8::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
//-----------------------------------------------------------------------------
void Chuck_Array8::sort()
{
    std::sort( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::back( t_CKFLOAT * val ) const
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // get
    *val = m_vector.back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array8::clear( )
{
    // zero
    zero( 0, m_vector.size() );

    // clear vector
    m_vector.clear();
}




//-----------------------------------------------------------------------------
// name: set_capacity()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::set_capacity( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );

    // ensure size
    set_size( capacity );

    return m_vector.capacity();
}




//-----------------------------------------------------------------------------
// name: set_size()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array8::set_size( t_CKINT size )
{
    // sanity check
    assert( size >= 0 );

    // if clearing size
    if( size < m_vector.size() )
    {
        // zero out section
        zero( size, m_vector.size() );
    }

    // what the size was
    t_CKINT size2 = m_vector.size();
    // resize vector
    m_vector.resize( size );

    // if clearing size
    if( m_vector.size() > size2 )
    {
        // zero out section
        zero( size2, m_vector.size() );
    }

    return m_vector.size();
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array8::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    for( t_CKUINT i = start; i < end; i++ )
    {
        // zero
        m_vector[i] = 0.0;
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_Array16()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array16::Chuck_Array16( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
    // clear
    m_isPolarType = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Array16()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array16::~Chuck_Array16()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array16::addr( t_CKINT i )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the addr
    return (t_CKUINT)(&m_vector[i]);
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array16::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::get( t_CKINT i, t_CKCOMPLEX * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::get( const string & key, t_CKCOMPLEX * val )
{
    // set to zero
    val->re = 0.0;
    val->im = 0.0;

    // iterator
    map<string, t_CKCOMPLEX>::iterator iter = m_map.find( key );

    // check
    if( iter != m_map.end() )
    {
        // get the value
        *val = (*iter).second;
    }

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::set( t_CKINT i, const t_CKCOMPLEX & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // set the value
    m_vector[i] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::set( const string & key, const t_CKCOMPLEX & val )
{
    // 1.3.1.1: removed this
    // map<string, t_CKCOMPLEX>::iterator iter = m_map.find( key );

    // 1.3.5.3: removed this
    // if( val.re == 0 && val.im == 0 ) m_map.erase( key ); else
    m_map[key] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::insert( t_CKINT i, const t_CKCOMPLEX & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // insert the value
    m_vector.insert( m_vector.begin()+i, val );

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: map_find()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::push_back( const t_CKCOMPLEX & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::pop_back( )
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // zero
    m_vector[m_vector.size()-1].re = 0.0;
    m_vector[m_vector.size()-1].im = 0.0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::push_front( const t_CKCOMPLEX & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin() );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::erase( t_CKINT pos )
{
    // check
    if ( m_vector.size() == 0 || pos<0 || pos>=m_vector.size())
        return 0;

    // add to vector
    m_vector.erase(m_vector.begin()+pos);
    return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase a range of elements [begin,end)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::erase( t_CKINT begin, t_CKINT end )
{
    // if needed swap the two
    if( begin > end ) { t_CKINT temp = begin; begin = end; end = temp; }
    // bound beginning
    if( begin < 0 ) begin = 0;
    // bound end
    if( end > m_vector.size() ) end = m_vector.size();

    // check
    if( m_vector.size() == 0 || begin >= m_vector.size() )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin()+begin, m_vector.begin()+end );

    return 1;
}




//-----------------------------------------------------------------------------
// name: get_keys() | 1.4.1.1 nshaheed (added)
// desc: return vector of keys from associative array
//-----------------------------------------------------------------------------
void Chuck_Array16::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string,t_CKCOMPLEX>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_Array16::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_Array16::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: ck_compare_polar()
// desc: compare function for sorting chuck polar values
//-----------------------------------------------------------------------------
static bool ck_compare_polar( const t_CKCOMPLEX & lhs, const t_CKCOMPLEX & rhs )
{
    // cast to polar
    const t_CKPOLAR * pL = (const t_CKPOLAR *)&lhs;
    const t_CKPOLAR * pR = (const t_CKPOLAR *)&rhs;

    // compare magnitude first
    t_CKFLOAT x = pL->modulus;
    t_CKFLOAT y = pR->modulus;
    // if magnitude equal
    if( x == y ) {
        // compare phase
        return pL->phase < pR->phase;
    }
    // return magnitude comparison
    return x < y;
}




//-----------------------------------------------------------------------------
// name: ck_compare_complex()
// desc: compare function for sorting chuck complex values
//-----------------------------------------------------------------------------
static bool ck_compare_complex( const t_CKCOMPLEX & lhs, const t_CKCOMPLEX & rhs )
{
    // compare magnitude first
    t_CKFLOAT x = ck_complex_magnitude(lhs);
    t_CKFLOAT y = ck_complex_magnitude(rhs);
    // if magnitude equal
    if( x == y ) {
        // compare phase
        t_CKFLOAT xp = ck_complex_phase(lhs);
        t_CKFLOAT yp = ck_complex_phase(rhs);
        return xp < yp;
    }
    // return magnitude comparison
    return x < y;
}




//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
// NOTE: complex/polar type sort in chuck uses MATLAB convention:
//       https://www.mathworks.com/help/matlab/ref/sort.html
//       "By default, the sort function sorts complex values by their magnitude,
//        and breaks ties using phase angles"
//-----------------------------------------------------------------------------
void Chuck_Array16::sort()
{
    // check betwen complex vs. polar
    if( m_isPolarType )
        std::sort( m_vector.begin(), m_vector.end(), ck_compare_polar );
    else
        std::sort( m_vector.begin(), m_vector.end(), ck_compare_complex );
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::back( t_CKCOMPLEX * val ) const
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // get
    *val = m_vector.back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array16::clear( )
{
    // zero
    zero( 0, m_vector.size() );

    // clear vector
    m_vector.clear();
}




//-----------------------------------------------------------------------------
// name: set_capacity()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::set_capacity( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );

    // ensure size
    set_size( capacity );

    return m_vector.capacity();
}




//-----------------------------------------------------------------------------
// name: set_size()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array16::set_size( t_CKINT size )
{
    // sanity check
    assert( size >= 0 );

    // if clearing size
    if( size < m_vector.size() )
    {
        // zero out section
        zero( size, m_vector.size() );
    }

    // remember
    t_CKINT size2 = m_vector.size();
    // resize vector
    m_vector.resize( size );

    // if clearing size
    if( m_vector.size() > size2 )
    {
        // zero out section
        zero( size2, m_vector.size() );
    }

    return m_vector.size();
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array16::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    for( t_CKUINT i = start; i < end; i++ )
    {
        // zero
        m_vector[i].re = 0.0;
        m_vector[i].im = 0.0;
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_Array24()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array24::Chuck_Array24( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Array24()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array24::~Chuck_Array24()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array24::addr( t_CKINT i )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the addr
    return (t_CKUINT)(&m_vector[i]);
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array24::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::get( t_CKINT i, t_CKVEC3 * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::get( const string & key, t_CKVEC3 * val )
{
    // set to zero
    val->x = val->y = val->z = 0;

    // iterator
    map<string, t_CKVEC3>::iterator iter = m_map.find( key );

    // check
    if( iter != m_map.end() )
    {
        // get the value
        *val = (*iter).second;
    }

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::set( t_CKINT i, const t_CKVEC3 & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // set the value
    m_vector[i] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::set( const string & key, const t_CKVEC3 & val )
{
    // 1.3.1.1: removed this
    // map<string, t_CKVEC3>::iterator iter = m_map.find( key );

    // insert
    m_map[key] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::insert( t_CKINT i, const t_CKVEC3 & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // insert the value
    m_vector.insert( m_vector.begin()+i, val );

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: map_find()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::push_back( const t_CKVEC3 & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::pop_back( )
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // zero
    m_vector[m_vector.size()-1].x = 0.0;
    m_vector[m_vector.size()-1].y = 0.0;
    m_vector[m_vector.size()-1].z = 0.0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::back( t_CKVEC3 * val ) const
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // get
    *val = m_vector.back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::push_front( const t_CKVEC3 & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin() );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::erase( t_CKINT pos )
{
    // check
    if ( m_vector.size() == 0 || pos<0 || pos>=m_vector.size())
        return 0;

    // add to vector
    m_vector.erase(m_vector.begin()+pos);
    return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase a range of elements [begin,end)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::erase( t_CKINT begin, t_CKINT end )
{
    // if needed swap the two
    if( begin > end ) { t_CKINT temp = begin; begin = end; end = temp; }
    // bound beginning
    if( begin < 0 ) begin = 0;
    // bound end
    if( end > m_vector.size() ) end = m_vector.size();

    // check
    if( m_vector.size() == 0 || begin >= m_vector.size() )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin()+begin, m_vector.begin()+end );

    return 1;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array24::clear( )
{
    // zero
    zero( 0, m_vector.size() );

    // clear vector
    m_vector.clear();
}




//-----------------------------------------------------------------------------
// name: set_capacity()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::set_capacity( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );

    // ensure size
    set_size( capacity );

    return m_vector.capacity();
}




//-----------------------------------------------------------------------------
// name: set_size()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array24::set_size( t_CKINT size )
{
    // sanity check
    assert( size >= 0 );

    // if clearing size
    if( size < m_vector.size() )
    {
        // zero out section
        zero( size, m_vector.size() );
    }

    // remember
    t_CKINT size2 = m_vector.size();
    // resize vector
    m_vector.resize( size );

    // if clearing size
    if( m_vector.size() > size2 )
    {
        // zero out section
        zero( size2, m_vector.size() );
    }

    return m_vector.size();
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array24::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    for( t_CKUINT i = start; i < end; i++ )
    {
        // zero
        m_vector[i].x = 0;
        m_vector[i].y = 0;
        m_vector[i].z = 0;
    }
}




//-----------------------------------------------------------------------------
// name: get_keys() | 1.4.1.1 nshaheed (added)
// desc: return vector of keys from associative array
//-----------------------------------------------------------------------------
void Chuck_Array24::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string, t_CKVEC3>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_Array24::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_Array24::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: ck_compare_vec3()
// desc: compare function for sorting chuck vec3s
//-----------------------------------------------------------------------------
static bool ck_compare_vec3( const t_CKVEC3 & lhs, const t_CKVEC3 & rhs )
{
    // sort by 2-norm / magnitude
    t_CKFLOAT x = ck_vec3_magnitude(lhs);
    t_CKFLOAT y = ck_vec3_magnitude(rhs);
    // if same
    if( x == y )
    {
        // tie breakers
        if( lhs.x < rhs.x ) return true;
        if( lhs.y < rhs.y ) return true;
        if( lhs.z < rhs.z ) return true;
    }
    return x < y;
}




//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
// NOTE: sort vec3 by 2-norm (magnitude)
//-----------------------------------------------------------------------------
void Chuck_Array24::sort()
{
    std::sort( m_vector.begin(), m_vector.end(), ck_compare_vec3 );
}




//-----------------------------------------------------------------------------
// name: Chuck_Array32()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Array32::Chuck_Array32( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Array32()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Array32::~Chuck_Array32()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array32::addr( t_CKINT i )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the addr
    return (t_CKUINT)(&m_vector[i]);
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Array32::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::get( t_CKINT i, t_CKVEC4 * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::get( const string & key, t_CKVEC4 * val )
{
    // set to zero
    val->x = val->y = val->z = val->w;

    // iterator
    map<string, t_CKVEC4>::iterator iter = m_map.find( key );

    // check
    if( iter != m_map.end() )
    {
        // get the value
        *val = (*iter).second;
    }

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::set( t_CKINT i, const t_CKVEC4 & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // set the value
    m_vector[i] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::set( const string & key, const t_CKVEC4 & val )
{
    // 1.3.1.1: removed this
    // map<string, t_CKVEC4>::iterator iter = m_map.find( key );

    // 1.3.5.3: removed this
    // if( val.re == 0 && val.im == 0 ) m_map.erase( key ); else

    // insert
    m_map[key] = val;

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::insert( t_CKINT i, const t_CKVEC4 & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // insert the value
    m_vector.insert( m_vector.begin()+i, val );

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: map_find()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::push_back( const t_CKVEC4 & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::pop_back( )
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // zero
    m_vector[m_vector.size()-1].x = 0.0;
    m_vector[m_vector.size()-1].y = 0.0;
    m_vector[m_vector.size()-1].z = 0.0;
    m_vector[m_vector.size()-1].w = 0.0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::back( t_CKVEC4 * val ) const
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // get
    *val = m_vector.back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::push_front( const t_CKVEC4 & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin() );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::erase( t_CKINT pos )
{
    // check
    if ( m_vector.size() == 0 || pos<0 || pos>=m_vector.size())
        return 0;

    // add to vector
    m_vector.erase(m_vector.begin()+pos);
    return 1;
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: erase a range of elements [begin,end)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::erase( t_CKINT begin, t_CKINT end )
{
    // if needed swap the two
    if( begin > end ) { t_CKINT temp = begin; begin = end; end = temp; }
    // bound beginning
    if( begin < 0 ) begin = 0;
    // bound end
    if( end > m_vector.size() ) end = m_vector.size();

    // check
    if( m_vector.size() == 0 || begin >= m_vector.size() )
        return 0;

    // add to vector
    m_vector.erase( m_vector.begin()+begin, m_vector.begin()+end );

    return 1;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array32::clear( )
{
    // zero
    zero( 0, m_vector.size() );

    // clear vector
    m_vector.clear();
}




//-----------------------------------------------------------------------------
// name: set_capacity()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::set_capacity( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );

    // ensure size
    set_size( capacity );

    return m_vector.capacity();
}




//-----------------------------------------------------------------------------
// name: set_size()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_Array32::set_size( t_CKINT size )
{
    // sanity check
    assert( size >= 0 );

    // if clearing size
    if( size < m_vector.size() )
    {
        // zero out section
        zero( size, m_vector.size() );
    }

    // remember
    t_CKINT size2 = m_vector.size();
    // resize vector
    m_vector.resize( size );

    // if clearing size
    if( m_vector.size() > size2 )
    {
        // zero out section
        zero( size2, m_vector.size() );
    }

    return m_vector.size();
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Array32::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    for( t_CKUINT i = start; i < end; i++ )
    {
        // zero
        m_vector[i].x = 0;
        m_vector[i].y = 0;
        m_vector[i].z = 0;
        m_vector[i].w = 0;
    }
}




//-----------------------------------------------------------------------------
// name: get_keys() | 1.4.1.1 nshaheed (added)
// desc: return vector of keys from associative array
//-----------------------------------------------------------------------------
void Chuck_Array32::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string, t_CKVEC4>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}



//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_Array32::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_Array32::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: ck_compare_vec4()
// desc: compare function for sorting chuck vec4s
//-----------------------------------------------------------------------------
static bool ck_compare_vec4( const t_CKVEC4 & lhs, const t_CKVEC4 & rhs )
{
    // sort by 2-norm / magnitude
    t_CKFLOAT x = ck_vec4_magnitude(lhs);
    t_CKFLOAT y = ck_vec4_magnitude(rhs);
    // if same
    if( x == y )
    {
        // tie breakers
        if( lhs.x < rhs.x ) return true;
        if( lhs.y < rhs.y ) return true;
        if( lhs.z < rhs.z ) return true;
        if( lhs.w < rhs.w ) return true;
    }
    return x < y;
}




//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
// NOTE: sort vec3 by 2-norm (magnitude)
//-----------------------------------------------------------------------------
void Chuck_Array32::sort()
{
    std::sort( m_vector.begin(), m_vector.end(), ck_compare_vec4 );
}




// Chuck_Event static instantiation
t_CKUINT Chuck_Event::our_can_wait = 0;
t_CKUINT Chuck_Event::our_waiting_on = 0;

//-----------------------------------------------------------------------------
// name: signal_local()
// desc: signal a event/condition variable, shreduling the next waiting shred
//       (if there is one or more); local in this case means within VM
//-----------------------------------------------------------------------------
void Chuck_Event::signal_local()
{
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    if( !m_queue.empty() )
    {
        // get the shred on top of the queue
        Chuck_VM_Shred * shred = m_queue.front();
        // pop the top
        m_queue.pop();
        // release it!
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif
        // REFACTOR-2017: BUG-FIX
        // release the extra ref we added when we started waiting for this event
        CK_SAFE_RELEASE( shred->event );
        // get shreduler
        Chuck_VM_Shreduler * shreduler = shred->vm_ref->shreduler();
        // remove the blocked shred from the list
        shreduler->remove_blocked( shred );
        // shredule the signaled shred
        shreduler->shredule( shred );
        // push the current time
        t_CKTIME *& sp = (t_CKTIME *&)shred->reg->sp;
        push_( sp, shreduler->now_system );
    }
    else
    {
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif
    }
}




//-----------------------------------------------------------------------------
// name: remove()
// desc: remove a shred from the event queue
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Event::remove( Chuck_VM_Shred * shred )
{
    queue<Chuck_VM_Shred *> temp;
    t_CKBOOL removed = FALSE;

    // lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    // while something in queue
    while( !m_queue.empty() )
    {
        // check if the shred we are looking for
        if( m_queue.front() != shred )
        {
            // if not, enqueue it into temp
            temp.push( m_queue.front() );
        }
        else
        {
            // TARPIT: this might seem like the right place for
            // CK_SAFE_RELEASE(shred->event), however this might cause
            // the deletion of the object while we are still using it.
            // so, put it in the caller: Chuck_VM_Shreduler::remove_blocked()

            // zero out
            shred->event = NULL;
            // flag
            removed = TRUE;
        }

        // pop the top
        m_queue.pop();
    }

    // copy temp back to queue
    m_queue = temp;
    // release lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif

    return removed;
}




//-----------------------------------------------------------------------------
// name: global_listen()
// desc: register a callback to a global event
//-----------------------------------------------------------------------------
void Chuck_Event::global_listen( void (* cb)(void),
    t_CKBOOL listen_forever )
{
    // storage
    Chuck_Global_Event_Listener new_listener;

    // store cb and whether to listen until canceled
    new_listener.void_callback = cb;
    new_listener.listen_forever = listen_forever;
    new_listener.callback_type = ck_get_plain;

    // store storage
    m_global_queue.push( new_listener );
}




//-----------------------------------------------------------------------------
// name: global_listen()
// desc: register a callback to a global event
//-----------------------------------------------------------------------------
void Chuck_Event::global_listen( std::string name, void (* cb)(const char *),
    t_CKBOOL listen_forever )
{
    // storage
    Chuck_Global_Event_Listener new_listener;

    // store cb and whether to listen until canceled
    new_listener.named_callback = cb;
    new_listener.listen_forever = listen_forever;
    new_listener.callback_type = ck_get_name;
    new_listener.name = name;

    // store storage
    m_global_queue.push( new_listener );
}




//-----------------------------------------------------------------------------
// name: global_listen()
// desc: register a callback to a global event
//-----------------------------------------------------------------------------
void Chuck_Event::global_listen( t_CKINT id, void (* cb)(t_CKINT),
    t_CKBOOL listen_forever )
{
    // storage
    Chuck_Global_Event_Listener new_listener;

    // store cb and whether to listen until canceled
    new_listener.id_callback = cb;
    new_listener.listen_forever = listen_forever;
    new_listener.callback_type = ck_get_id;
    new_listener.id = id;

    // store storage
    m_global_queue.push( new_listener );
}




//-----------------------------------------------------------------------------
// name: remove_listen()
// desc: deregister a callback to a global event
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Event::remove_listen( void (* cb)(void) )
{
    std::queue<Chuck_Global_Event_Listener> temp;
    t_CKBOOL removed = FALSE;

    // lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    // while something in queue
    while( !m_global_queue.empty() )
    {
        // check if the callback we are looking for
        if( m_global_queue.front().callback_type != ck_get_plain || m_global_queue.front().void_callback != cb )
        {
            // if not, enqueue it into temp
            temp.push( m_global_queue.front() );
        }
        else
        {
            // flag, don't add to temp
            removed = TRUE;
        }

        // pop the top
        m_global_queue.pop();
    }

    // copy temp back to queue
    m_global_queue = temp;
    // release lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif

    return removed;
}




//-----------------------------------------------------------------------------
// name: remove_listen()
// desc: deregister a callback to a global event
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Event::remove_listen( std::string name, void (* cb)(const char *)  )
{
    std::queue<Chuck_Global_Event_Listener> temp;
    t_CKBOOL removed = FALSE;

    // lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    // while something in queue
    while( !m_global_queue.empty() )
    {
        // check if the callback we are looking for
        if( m_global_queue.front().callback_type != ck_get_name || m_global_queue.front().named_callback != cb )
        {
            // if not, enqueue it into temp
            temp.push( m_global_queue.front() );
        }
        else
        {
            // flag, don't add to temp
            removed = TRUE;
        }

        // pop the top
        m_global_queue.pop();
    }

    // copy temp back to queue
    m_global_queue = temp;
    // release lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif

    return removed;
}




//-----------------------------------------------------------------------------
// name: remove_listen()
// desc: deregister a callback to a global event
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Event::remove_listen( t_CKINT id, void (* cb)(t_CKINT)  )
{
    std::queue<Chuck_Global_Event_Listener> temp;
    t_CKBOOL removed = FALSE;

    // lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    // while something in queue
    while( !m_global_queue.empty() )
    {
        // check if the callback we are looking for
        if( m_global_queue.front().callback_type != ck_get_id || m_global_queue.front().id_callback != cb )
        {
            // if not, enqueue it into temp
            temp.push( m_global_queue.front() );
        }
        else
        {
            // flag, don't add to temp
            removed = TRUE;
        }

        // pop the top
        m_global_queue.pop();
    }

    // copy temp back to queue
    m_global_queue = temp;
    // release lock
    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif

    return removed;
}




//-----------------------------------------------------------------------------
// name: signal_global()
// desc: call a global callback listener
//-----------------------------------------------------------------------------
void Chuck_Event::signal_global()
{
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif

    if( !m_global_queue.empty() )
    {
        // get the listener on top of the queue
        Chuck_Global_Event_Listener listener = m_global_queue.front();
        // pop the top
        m_global_queue.pop();
        // call callback
        switch( listener.callback_type )
        {
        case ck_get_plain:
            if( listener.void_callback != NULL )
            {
                listener.void_callback();
            }
            break;
        case ck_get_name:
            if( listener.named_callback != NULL )
            {
                listener.named_callback( listener.name.c_str() );
            }
            break;
        case ck_get_id:
            if( listener.id_callback != NULL )
            {
                listener.id_callback( listener.id );
            }
            break;
        }
        // if call forever, add back to m_global_queue
        if( listener.listen_forever )
        {
            m_global_queue.push( listener );
        }
    }

    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif
}




//-----------------------------------------------------------------------------
// name: broadcast_global()
// desc: call a global callback listener
//-----------------------------------------------------------------------------
void Chuck_Event::broadcast_global()
{
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    std::queue< Chuck_Global_Event_Listener > call_again;

    while( !m_global_queue.empty() )
    {
        // get the listener on top of the queue
        Chuck_Global_Event_Listener listener = m_global_queue.front();
        // pop the top
        m_global_queue.pop();
        // call callback
        switch( listener.callback_type )
        {
        case ck_get_plain:
            if( listener.void_callback != NULL )
            {
                listener.void_callback();
            }
            break;
        case ck_get_name:
            if( listener.named_callback != NULL )
            {
                listener.named_callback( listener.name.c_str() );
            }
            break;
        case ck_get_id:
            if( listener.id_callback != NULL )
            {
                listener.id_callback( listener.id );
            }
            break;
        }
        // if call forever, add back to m_global_queue
        if( listener.listen_forever )
        {
            call_again.push( listener );
        }
    }

    // for those that should be called again, store them again
    m_global_queue = call_again;

    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif
}




//-----------------------------------------------------------------------------
// name: queue_broadcast()
// desc: queue the event to broadcast a event/condition variable,
//       by the owner of the queue
//       added 1.3.0.0: event_buffer to fix big-ass bug
//-----------------------------------------------------------------------------
void Chuck_Event::queue_broadcast( CBufferSimple * event_buffer )
{
    // TODO: handle multiple VM
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    if( !m_queue.empty() )
    {
        // get shred (only to get the VM ref)
        Chuck_VM_Shred * shred = m_queue.front();
        // release lock
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif
        // queue the event on the vm (added 1.3.0.0: event_buffer)
        shred->vm_ref->queue_event( this, 1, event_buffer );
    }
    else
    {
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif
    }
}




//-----------------------------------------------------------------------------
// name: broadcast_local()
// desc: broadcast a event/condition variable, shreduling all waiting shreds
//       local here means within VM
//-----------------------------------------------------------------------------
void Chuck_Event::broadcast_local()
{
    // lock queue
    #ifndef __DISABLE_THREADS__
    m_queue_lock.acquire();
    #endif
    // while not empty
    while( !m_queue.empty() )
    {
        // release first
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif
        // signal the next shred
        this->signal_local();
        // lock again
        #ifndef __DISABLE_THREADS__
        m_queue_lock.acquire();
        #endif
    }
    // release
    #ifndef __DISABLE_THREADS__
    m_queue_lock.release();
    #endif
}




//-----------------------------------------------------------------------------
// name: wait()
// desc: cause event/condition variable to block the current shred, putting it
//       on its waiting list, and suspennd the shred from the VM.
//-----------------------------------------------------------------------------
void Chuck_Event::wait( Chuck_VM_Shred * shred, Chuck_VM * vm )
{
    EM_log( CK_LOG_FINE, "shred '%d' wait on event '%x'...", shred->xid, (t_CKUINT)this );
    // make sure the shred info matches the vm
    assert( shred->vm_ref == vm );

    Chuck_DL_Return RETURN;
    // get the member function
    f_mfun canwaitplease = (f_mfun)this->vtable->funcs[our_can_wait]->code->native_func;
    // TODO: check this is right shred
    // added 1.3.0.0: the DL API instance
    canwaitplease( this, NULL, &RETURN, vm, shred, Chuck_DL_Api::Api::instance() );
    // RETURN.v_int = 1;

    // see if we can wait
    if( RETURN.v_int )
    {
        // suspend
        shred->is_running = FALSE;

        // add to waiting list
        #ifndef __DISABLE_THREADS__
        m_queue_lock.acquire();
        #endif
        m_queue.push( shred );
        #ifndef __DISABLE_THREADS__
        m_queue_lock.release();
        #endif

        // add event to shred
        assert( shred->event == NULL );
        shred->event = this;
        // the shred might need the event pointer after it's been released by the
        // vm instruction Chuck_Instr_Release_Object2, in order to tell the event
        // to forget the shred. So, add another reference so it won't be freed
        // until the shred is done with it.  REFACTOR-2017
        CK_SAFE_ADD_REF( shred->event );

        // add shred to shreduler
        vm->shreduler()->add_blocked( shred );

        // get the virtual function for waiting_on()
        f_mfun waiting_on = (f_mfun)this->vtable->funcs[our_waiting_on]->code->native_func;
        // call to nofify event that a shred has starting waiting on it
        waiting_on( this, NULL, &RETURN, vm, shred, Chuck_DL_Api::Api::instance() );
    }
    else // can't wait
    {
        // push the current time
        t_CKTIME *& sp = (t_CKTIME *&)shred->reg->sp;
        push_( sp, shred->now );
    }
}
