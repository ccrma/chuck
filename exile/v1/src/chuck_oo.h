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
// file: chuck_oo.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_OO_H__
#define __CHUCK_OO_H__

#include "chuck_def.h"
#include <assert.h>
#include <vector>




//-----------------------------------------------------------------------------
// name: struct Chuck_VM_Object
// dsec: base vm object
//-----------------------------------------------------------------------------
struct Chuck_VM_Object
{
public:
    Chuck_VM_Object( t_CKBOOL pooled = FALSE )
    { this->init_ref(); m_pooled = pooled; }
    virtual ~Chuck_VM_Object() { }
    virtual void init() { this->init_ref(); }
    virtual void done() { }

public:
    void init_ref() { m_ref_count = 0; }
    void add_ref()  { m_ref_count++; }
    void release()  { assert( m_ref_count > 0 ); m_ref_count--;
                      if( !m_ref_count )
                          if( !m_pooled ) delete this;
                          else assert( FALSE ); }
public:
    t_CKUINT m_ref_count;
    t_CKBOOL m_pooled;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Object
// dsec: base object
//-----------------------------------------------------------------------------
struct Chuck_Object : Chuck_VM_Object
{ };




//-----------------------------------------------------------------------------
// name: struct Chuck_Array
// desc: native ChucK arrays
//-----------------------------------------------------------------------------
template<class T>
struct Chuck_Array : Chuck_Object
{
public:
    Chuck_Array( t_CKINT size = 1 );
    ~Chuck_Array();

public:
    T operator[]( t_CKINT i );

public:
    t_CKINT push_back( T val );
    t_CKINT pop_back( );
    T back( );

public:
    std::vector<T> m_vector;
};




//-----------------------------------------------------------------------------
// name: Chuck_Event
// desc: base Chuck Event class
//-----------------------------------------------------------------------------
struct Chuck_Event : Chuck_Object
{
public:
    Chuck_Event();
    ~Chuck_Event();

public:
    t_CKUINT signal();
    t_CKUINT broadcast();
    t_CKUINT wait();
};




#endif
