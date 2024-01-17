/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
// desc: decrement reference; deletes objects when refcount reaches 0;
//-----------------------------------------------------------------------------
void Chuck_VM_Object::release()
{
    //-----------------------------------------------------------------------------
    // release is permitted even if ref-count is already 0 | 1.5.1.0 (ge)
    //-----------------------------------------------------------------------------
    if( m_ref_count <= 0 )
    {
        // log warning
        EM_log( CK_LOG_FINEST, "(internal) Object.release() refcount already %d", m_ref_count );

        // disabled: error out
        // EM_error3( "[chuck]: (internal error) Object.release() refcount == %d", m_ref_count );
        // make sure there is at least one reference
        // assert( m_ref_count > 0 );
    }
    else
    {
        // decrement
        m_ref_count--;
    }

    // updated 1.5.0.5 to use Chuck_VM_Debug
    CK_VM_DEBUGGER( release( this ) );

    // if no more references
    if( m_ref_count == 0 )
    {
        // this is not good | TODO: our_locks_in_effect assumes single VM
        if( our_locks_in_effect && m_locked )
        {
            EM_error2( 0, "(internal error) releasing locked VM object!" );
            // bail out
            assert( FALSE );
            // in case assert is disabled
            // *(int *)0 = 1;
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

        // cerr << "DELETE THIS: " << (void *)this << endl;
        // trigger this object's deletion / destructors
        // should be valid as long as no members are used beyond this point
        delete this; // REFACTOR-2017
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
    EM_log( CK_LOG_HERALD, "locking down special objects..." );
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
    EM_log( CK_LOG_HERALD, "unlocking special objects..." );
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




// static instantiation | 1.5.1.5
t_CKUINT Chuck_Object::our_vt_toString = 0;

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
    // zero origin shred
    origin_shred = NULL;
    // zero origin vm
    origin_vm = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Object()
// desc: chuck object internal destructor
//-----------------------------------------------------------------------------
Chuck_Object::~Chuck_Object()
{
    // get this object's type
    Chuck_Type * type = this->type_ref;

    // shred
    Chuck_VM_Shred * shred = NULL;
    // a way to check if we are in run state or post-run (shutdown) state | 1.5.1.8
    // NOTE important to look at carrier->vm directly, and not a copy of vm pointer
    Chuck_VM * vm = type && type->env() && type->env()->carrier() ? type->env()->carrier()->vm : NULL;
    // if we are in normal run state
    if( vm ) {
        // update shred ref from origin shred  | 1.5.1.8
        shred = origin_shred ? origin_shred : vm->get_current_shred();
    }

    // call destructors, from latest descended child to oldest parent | 1.3.0.0
    while( type != NULL )
    {
        // SPENCER TODO: HACK! is there a better way to call the dtor?
        // has_pre-dtor: related to info->pre_dtor, but different since info is shared with arrays
        // of this type (via new_array_type()), but this flag pertains to this type only
        if( type->info && type->has_pre_dtor ) // 1.5.0.0 (ge) added type->info check
        {
            // make sure
            assert( type->info->pre_dtor );
            // check origin of dtor
            if( type->info->pre_dtor->native_func ) // c++-defined deconstructor
            {
                // REFACTOR-2017: do we know which VM to pass in? (diff main/sub instance?)
                // pass in type-associated vm and current shred | 1.5.1.8
                ((f_dtor)(type->info->pre_dtor->native_func))( this, vm, shred, Chuck_DL_Api::instance() );
            }
            else // chuck-defined deconstructor
            {
                // this shouldn't be possible
                EM_error3( "(internal error) native_func dtor not found in class '%s'...", type->c_name() );
                // keep going for now...
            }
        }

        // chuck-defined destructor | 1.5.2.0 (ge) added
        if( type->dtor_the )
        {
            // verify
            if( !type->dtor_invoker )
            {
                EM_error3( "(internal error) dtor invoker not found in class '%s'...", type->c_name() );
            }
            else
            {
                // invoke the dtor
                // NOTE @destruct disallows accessing context-global variables/functions
                type->dtor_invoker->invoke( this, this->originShred() );
            }
        }

        // go up the inheritance
        type = type->parent;
    }

    // release class-scope member vars | 1.5.2.0 (ge) added
    type = this->type_ref; Chuck_Object * obj = NULL;
    // for each type in the inheritance chain
    while( type )
    {
        // for each mvar directly in the class
        for( t_CKUINT i = 0; i < type->obj_mvars_offsets.size(); i++ )
        {
            // get the object reference from the offsets
            obj = OBJ_MEMBER_OBJECT( this, type->obj_mvars_offsets[i] );
            // release
            CK_SAFE_RELEASE(obj);
        }

        // go up to parent type
        type = type->parent;
    }

    // release origin shred
    CK_SAFE_RELEASE( origin_shred );
    // release type reference
    CK_SAFE_RELEASE( type_ref );

    // CK_SAFE_RELEASE( origin_vm );
    // just zero out | 1.5.1.8 (ge)
    origin_vm = NULL;

    // free virtual table
    CK_SAFE_DELETE( vtable );
    // free data segment
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
// name: setOriginShred()
// desc: set origin shred
//-----------------------------------------------------------------------------
void Chuck_Object::setOriginShred( Chuck_VM_Shred * shred )
{
    // assign
    CK_SAFE_REF_ASSIGN( this->origin_shred, shred );
}




//-----------------------------------------------------------------------------
// name: setOriginVM()
// desc: set origin VM
//-----------------------------------------------------------------------------
void Chuck_Object::setOriginVM( Chuck_VM * vm )
{
    // copy only; avoid refcounting VM on a per-object basis | 1.5.1.8
    this->origin_vm = vm;
    // reference count assign
    // CK_SAFE_REF_ASSIGN( this->origin_vm, vm );
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
// name: Chuck_ArrayInt()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ArrayInt::Chuck_ArrayInt( t_CKBOOL is_obj, t_CKINT capacity )
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
// name: ~Chuck_ArrayInt()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_ArrayInt::~Chuck_ArrayInt()
{
    // 1.4.2.0 (ge) | added, which should cascade to nested array objects
    clear();
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: return address of element at position i, as an int
//-----------------------------------------------------------------------------
t_CKUINT Chuck_ArrayInt::addr( t_CKINT i )
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
t_CKUINT Chuck_ArrayInt::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get value of element at position i
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::get( t_CKINT i, t_CKUINT * val )
{
    // set to zero
    *val = 0;
    // bound check
    if( i < 0 || i >= m_vector.capacity() ) return 0;
    // get the value
    *val = m_vector[i];
    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get() -- signed edition | 1.5.2.0
// desc: get value of element at position i
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::get( t_CKINT i, t_CKINT * val )
{
    // set to zero
    *val = 0;
    // bound check
    if( i < 0 || i >= m_vector.capacity() ) return 0;
    // get the value
    *val = (t_CKINT)m_vector[i];
    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get value of element at key
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::get( const string & key, t_CKUINT * val )
{
    // set to zero
    *val = 0;
    // find
    map<string, t_CKUINT>::iterator iter = m_map.find( key );
    // check
    if( iter == m_map.end() ) return 0;
    // copy value
    *val = (*iter).second;
    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: get() -- signed edition | 1.5.2.0
// desc: get value of element at key
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::get( const string & key, t_CKINT * val )
{
    // set to zero
    *val = 0;
    // find
    map<string, t_CKUINT>::iterator iter = m_map.find( key );
    // check
    if( iter == m_map.end() ) return 0;
    // copy value
    *val = (t_CKINT)(*iter).second;
    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: set()
// desc: set element of array by position, with ref counting as applicable
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::set( t_CKINT i, t_CKUINT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() ) return 0;
    // get current value
    t_CKUINT v = m_vector[i];
    // if Object, release
    if( m_is_obj && v ) ((Chuck_Object *)v)->release();
    // set the value
    m_vector[i] = val;
    // if Object, add ref
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // return good
    return 1;
}





//-----------------------------------------------------------------------------
// name: set()
// desc: set element of map by key, with ref counting as applicable
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::set( const string & key, t_CKUINT val )
{
    // look for key
    map<string, t_CKUINT>::iterator iter = m_map.find( key );

    // if Object, release
    if( m_is_obj && iter != m_map.end() && iter->second != 0 )
        ((Chuck_Object *)iter->second)->release();

    // if 0, remove the element
    if( !val ) m_map.erase( key );
    else m_map[key] = val;

    // if Object, add ref
    if( m_is_obj && val ) ((Chuck_Object *)val)->add_ref();

    // return good
    return 1;
}




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::insert( t_CKINT i, t_CKUINT val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() ) return 0;

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
t_CKINT Chuck_ArrayInt::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}




//-----------------------------------------------------------------------------
// name: erase()
// desc: (map only) erase element by key
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayInt::map_erase( const string & key )
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
t_CKINT Chuck_ArrayInt::push_back( t_CKUINT val )
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
t_CKINT Chuck_ArrayInt::pop_back()
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
t_CKINT Chuck_ArrayInt::push_front( t_CKUINT val )
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
t_CKINT Chuck_ArrayInt::pop_front()
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // if obj
    if( m_is_obj )
    {
        // get pointer
        Chuck_Object * v = (Chuck_Object *)m_vector[0];
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
t_CKINT Chuck_ArrayInt::erase( t_CKINT pos )
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
t_CKINT Chuck_ArrayInt::erase( t_CKINT begin, t_CKINT end )
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
void Chuck_ArrayInt::get_keys( std::vector<std::string> & keys )
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
void Chuck_ArrayInt::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_ArrayInt::reverse()
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
void Chuck_ArrayInt::sort()
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
t_CKINT Chuck_ArrayInt::back( t_CKUINT * val ) const
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
void Chuck_ArrayInt::clear( )
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
t_CKINT Chuck_ArrayInt::set_capacity( t_CKINT capacity )
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
t_CKINT Chuck_ArrayInt::set_size( t_CKINT size )
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
void Chuck_ArrayInt::zero( t_CKUINT start, t_CKUINT end )
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
// name: Chuck_ArrayFloat()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ArrayFloat::Chuck_ArrayFloat( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_ArrayFloat()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_ArrayFloat::~Chuck_ArrayFloat()
{
    // do nothing
    clear();
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_ArrayFloat::addr( t_CKINT i )
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
t_CKUINT Chuck_ArrayFloat::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::get( t_CKINT i, t_CKFLOAT * val )
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
t_CKINT Chuck_ArrayFloat::get( const string & key, t_CKFLOAT * val )
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
t_CKINT Chuck_ArrayFloat::set( t_CKINT i, t_CKFLOAT val )
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
t_CKINT Chuck_ArrayFloat::set( const string & key, t_CKFLOAT val )
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
t_CKINT Chuck_ArrayFloat::insert( t_CKINT i, t_CKFLOAT val )
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
t_CKINT Chuck_ArrayFloat::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::push_back( t_CKFLOAT val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::pop_back( )
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
t_CKINT Chuck_ArrayFloat::push_front( t_CKFLOAT val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::pop_front()
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
t_CKINT Chuck_ArrayFloat::erase( t_CKINT pos )
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
t_CKINT Chuck_ArrayFloat::erase( t_CKINT begin, t_CKINT end )
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
void Chuck_ArrayFloat::get_keys( std::vector<std::string> & keys )
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
void Chuck_ArrayFloat::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_ArrayFloat::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: sort()
// desc: sort the array in ascending order
//-----------------------------------------------------------------------------
void Chuck_ArrayFloat::sort()
{
    std::sort( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayFloat::back( t_CKFLOAT * val ) const
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
void Chuck_ArrayFloat::clear( )
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
t_CKINT Chuck_ArrayFloat::set_capacity( t_CKINT capacity )
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
t_CKINT Chuck_ArrayFloat::set_size( t_CKINT size )
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
void Chuck_ArrayFloat::zero( t_CKUINT start, t_CKUINT end )
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
// name: Chuck_ArrayVec2()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec2::Chuck_ArrayVec2( t_CKINT capacity )
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
// name: ~Chuck_ArrayVec2()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec2::~Chuck_ArrayVec2()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_ArrayVec2::addr( t_CKINT i )
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
t_CKUINT Chuck_ArrayVec2::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::get( t_CKINT i, t_CKVEC2 * val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // get the value
    *val = m_vector[i];

    // return good
    return 1;
}
// redirect as vec2
t_CKINT Chuck_ArrayVec2::get( t_CKINT i, t_CKCOMPLEX * val )
{ return this->get( i, (t_CKVEC2 *)val ); }




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::get( const string & key, t_CKVEC2 * val )
{
    // set to zero
    val->x = 0;
    val->y = 0;

    // iterator
    map<string, t_CKVEC2>::iterator iter = m_map.find( key );

    // check
    if( iter != m_map.end() )
    {
        // get the value
        *val = (*iter).second;
    }

    // return good
    return 1;
}
// redirect as vec2
t_CKINT Chuck_ArrayVec2::get( const string & key, t_CKCOMPLEX * val )
{ return this->get( key, (t_CKVEC2 *)val ); }




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::set( t_CKINT i, const t_CKVEC2 & val )
{
    // bound check
    if( i < 0 || i >= m_vector.capacity() )
        return 0;

    // set the value
    m_vector[i] = val;

    // return good
    return 1;
}
// redirect as vec2
t_CKINT Chuck_ArrayVec2::set( t_CKINT i, const t_CKCOMPLEX & val )
{ return this->set( i, (t_CKVEC2 &)val ); }




//-----------------------------------------------------------------------------
// name: set()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::set( const string & key, const t_CKVEC2 & val )
{
    // 1.3.1.1: removed this
    // map<string, t_CKVEC2>::iterator iter = m_map.find( key );

    // 1.3.5.3: removed this
    // if( val.re == 0 && val.im == 0 ) m_map.erase( key ); else
    m_map[key] = val;

    // return good
    return 1;
}
// redirect as vec2
t_CKINT Chuck_ArrayVec2::set( const string & key, const t_CKCOMPLEX & val )
{ return this->set( key, (t_CKVEC2 &)val ); }




//-----------------------------------------------------------------------------
// name: insert() | 1.5.0.8 (ge) added
// desc: insert before position | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::insert( t_CKINT i, const t_CKVEC2 & val )
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
t_CKINT Chuck_ArrayVec2::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::push_back( const t_CKVEC2 & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::pop_back( )
{
    // check
    if( m_vector.size() == 0 )
        return 0;

    // zero
    m_vector[m_vector.size()-1].x = 0;
    m_vector[m_vector.size()-1].y = 0;
    // add to vector
    m_vector.pop_back();

    return 1;
}




//-----------------------------------------------------------------------------
// name: push_front() | 1.5.0.8 (ge) added
// desc: prepend element by value | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::push_front( const t_CKVEC2 & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::pop_front()
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
t_CKINT Chuck_ArrayVec2::erase( t_CKINT pos )
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
t_CKINT Chuck_ArrayVec2::erase( t_CKINT begin, t_CKINT end )
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
void Chuck_ArrayVec2::get_keys( std::vector<std::string> & keys )
{
    // clear the return array
    keys.clear();
    // iterator
    for( std::map<std::string,t_CKVEC2>::iterator iter = m_map.begin(); iter !=m_map.end(); iter++ )
    {
        // add to list
        keys.push_back((*iter).first);
    }
}




//-----------------------------------------------------------------------------
// name: reverse()
// desc: reverses array in-place
//-----------------------------------------------------------------------------
void Chuck_ArrayVec2::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_ArrayVec2::shuffle()
{
    my_random_shuffle( m_vector.begin(), m_vector.end() );
}




//-----------------------------------------------------------------------------
// name: ck_compare_polar()
// desc: compare function for sorting chuck polar values
//-----------------------------------------------------------------------------
static bool ck_compare_polar( const t_CKVEC2 & lhs, const t_CKVEC2 & rhs )
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
// name: ck_compare_vec2()
// desc: compare function for sorting chuck vec2 values
//-----------------------------------------------------------------------------
static bool ck_compare_vec2( const t_CKVEC2 & lhs, const t_CKVEC2 & rhs )
{
    // compare magnitude first
    t_CKFLOAT x = ck_vec2_magnitude(lhs);
    t_CKFLOAT y = ck_vec2_magnitude(rhs);
    // if magnitude equal
    if( x == y ) {
        // compare phase
        t_CKFLOAT xp = ck_vec2_phase(lhs);
        t_CKFLOAT yp = ck_vec2_phase(rhs);
        return xp < yp;
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
void Chuck_ArrayVec2::sort()
{
    // check betwen complex vs. polar
    if( m_isPolarType )
        std::sort( m_vector.begin(), m_vector.end(), ck_compare_polar );
    else
        std::sort( m_vector.begin(), m_vector.end(), ck_compare_vec2 );
}




//-----------------------------------------------------------------------------
// name: back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec2::back( t_CKVEC2 * val ) const
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
void Chuck_ArrayVec2::clear( )
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
t_CKINT Chuck_ArrayVec2::set_capacity( t_CKINT capacity )
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
t_CKINT Chuck_ArrayVec2::set_size( t_CKINT size )
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
void Chuck_ArrayVec2::zero( t_CKUINT start, t_CKUINT end )
{
    // sanity check
    assert( start <= m_vector.capacity() && end <= m_vector.capacity() );

    for( t_CKUINT i = start; i < end; i++ )
    {
        // zero
        m_vector[i].x = 0;
        m_vector[i].y = 0;
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_ArrayVec3()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec3::Chuck_ArrayVec3( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_ArrayVec3()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec3::~Chuck_ArrayVec3()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_ArrayVec3::addr( t_CKINT i )
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
t_CKUINT Chuck_ArrayVec3::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec3::get( t_CKINT i, t_CKVEC3 * val )
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
t_CKINT Chuck_ArrayVec3::get( const string & key, t_CKVEC3 * val )
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
t_CKINT Chuck_ArrayVec3::set( t_CKINT i, const t_CKVEC3 & val )
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
t_CKINT Chuck_ArrayVec3::set( const string & key, const t_CKVEC3 & val )
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
t_CKINT Chuck_ArrayVec3::insert( t_CKINT i, const t_CKVEC3 & val )
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
t_CKINT Chuck_ArrayVec3::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec3::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec3::push_back( const t_CKVEC3 & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec3::pop_back( )
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
t_CKINT Chuck_ArrayVec3::back( t_CKVEC3 * val ) const
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
t_CKINT Chuck_ArrayVec3::push_front( const t_CKVEC3 & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec3::pop_front()
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
t_CKINT Chuck_ArrayVec3::erase( t_CKINT pos )
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
t_CKINT Chuck_ArrayVec3::erase( t_CKINT begin, t_CKINT end )
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
void Chuck_ArrayVec3::clear( )
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
t_CKINT Chuck_ArrayVec3::set_capacity( t_CKINT capacity )
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
t_CKINT Chuck_ArrayVec3::set_size( t_CKINT size )
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
void Chuck_ArrayVec3::zero( t_CKUINT start, t_CKUINT end )
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
void Chuck_ArrayVec3::get_keys( std::vector<std::string> & keys )
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
void Chuck_ArrayVec3::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_ArrayVec3::shuffle()
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
void Chuck_ArrayVec3::sort()
{
    std::sort( m_vector.begin(), m_vector.end(), ck_compare_vec3 );
}




//-----------------------------------------------------------------------------
// name: Chuck_ArrayVec4()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec4::Chuck_ArrayVec4( t_CKINT capacity )
{
    // sanity check
    assert( capacity >= 0 );
    // set size
    m_vector.resize( capacity );
    // clear
    this->zero( 0, m_vector.capacity() );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_ArrayVec4()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_ArrayVec4::~Chuck_ArrayVec4()
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: addr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT Chuck_ArrayVec4::addr( t_CKINT i )
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
t_CKUINT Chuck_ArrayVec4::addr( const string & key )
{
    // get the addr
    return (t_CKUINT)(&m_map[key]);
}




//-----------------------------------------------------------------------------
// name: get()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec4::get( t_CKINT i, t_CKVEC4 * val )
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
t_CKINT Chuck_ArrayVec4::get( const string & key, t_CKVEC4 * val )
{
    // set to zero
    val->x = val->y = val->z = val->w = 0;

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
t_CKINT Chuck_ArrayVec4::set( t_CKINT i, const t_CKVEC4 & val )
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
t_CKINT Chuck_ArrayVec4::set( const string & key, const t_CKVEC4 & val )
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
t_CKINT Chuck_ArrayVec4::insert( t_CKINT i, const t_CKVEC4 & val )
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
t_CKINT Chuck_ArrayVec4::map_find( const string & key )
{
    return m_map.find( key ) != m_map.end();
}



//-----------------------------------------------------------------------------
// name: map_erase()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec4::map_erase( const string & key )
{
    return m_map.erase( key );
}




//-----------------------------------------------------------------------------
// name: push_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec4::push_back( const t_CKVEC4 & val )
{
    // add to vector
    m_vector.push_back( val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_back()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec4::pop_back( )
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
t_CKINT Chuck_ArrayVec4::back( t_CKVEC4 * val ) const
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
t_CKINT Chuck_ArrayVec4::push_front( const t_CKVEC4 & val )
{
    // add to vector
    m_vector.insert( m_vector.begin(), val );

    return 1;
}




//-----------------------------------------------------------------------------
// name: pop_front() | 1.5.0.8 (ge) added
// desc: pop the last element in vector | O(n) running time
//-----------------------------------------------------------------------------
t_CKINT Chuck_ArrayVec4::pop_front()
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
t_CKINT Chuck_ArrayVec4::erase( t_CKINT pos )
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
t_CKINT Chuck_ArrayVec4::erase( t_CKINT begin, t_CKINT end )
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
void Chuck_ArrayVec4::clear( )
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
t_CKINT Chuck_ArrayVec4::set_capacity( t_CKINT capacity )
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
t_CKINT Chuck_ArrayVec4::set_size( t_CKINT size )
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
void Chuck_ArrayVec4::zero( t_CKUINT start, t_CKUINT end )
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
void Chuck_ArrayVec4::get_keys( std::vector<std::string> & keys )
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
void Chuck_ArrayVec4::reverse( )
{
    std::reverse(m_vector.begin(), m_vector.end());
}




//-----------------------------------------------------------------------------
// name: shuffle() | 1.5.0.0 nshaheed, azaday, kunwoo, ge (added)
// desc: shuffle the contents of the array
//-----------------------------------------------------------------------------
void Chuck_ArrayVec4::shuffle()
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
void Chuck_ArrayVec4::sort()
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

    // invoke virtual function our_can_wait | 1.5.1.5
    Chuck_DL_Return RETURN = ck_invoke_mfun_immediate_mode( this, our_can_wait, vm, shred, NULL, 0 );
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

        // invoke virtual function our_waiting_on | 1.5.1.5 (ge & andrew)
        // this is called at the point when a shred has completed the actions
        // needed to wait on an Event, and can be notified/broadcasted from
        // a different thread
        ck_invoke_mfun_immediate_mode( this, our_waiting_on, vm, shred, NULL, 0 );
    }
    else // can't wait
    {
        // push the current time
        t_CKTIME *& sp = (t_CKTIME *&)shred->reg->sp;
        push_( sp, shred->now );
    }
}
