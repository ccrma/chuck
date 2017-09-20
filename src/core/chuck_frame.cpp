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
// file: chuck_frame.cpp
// desc: chuck scope frame
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_frame.h"
using namespace std;




//-----------------------------------------------------------------------------
// name: Chuck_Frame()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Frame::Chuck_Frame()
{
    // name
    name = "";
    // ofset
    curr_offset = 0;
    // don't know
    num_access = 0;
}




//-----------------------------------------------------------------------------
// name: alloc_local()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Local * Chuck_Frame::alloc_local( t_CKUINT size, const string & name,
    t_CKBOOL is_ref, t_CKBOOL is_obj, t_CKBOOL is_external )
{
    // alloc
    Chuck_Local * local = new Chuck_Local;
    // size
    local->size = size;
    // the offset
    local->offset = this->curr_offset;
    // ref
    local->is_ref = is_ref;
    // is object type?  (added ge: 2012 april | added 1.3.0.0)
    local->is_obj = is_obj;
    // REFACTOR-2017: is external?
    local->is_external = is_external;
    // the next offset
    this->curr_offset += local->size;
    // name
    local->name = name;
    // push the local
    this->stack.push_back( local );

    return local;
}




//-----------------------------------------------------------------------------
// name: push_scope()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Frame::push_scope( )
{
    stack.push_back( NULL );
}




//-----------------------------------------------------------------------------
// name: get_scope()
// desc: get current scope (added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Frame::get_scope( vector<Chuck_Local *> & out ) const
{
    // sanity
    assert( this->stack.size() > 0 );

    // the local
    Chuck_Local * local = NULL;
    // the index
    long index = this->stack.size() - 1;

    // loop
    while( index >= 0 && this->stack[index] != NULL )
    {
        // last thing
        local = stack[index];
        // move
        index--;
        // check if not null
        if( local )
        {
            // copy out
            out.push_back( local );
        }
    }
}




//-----------------------------------------------------------------------------
// name: pop_scope()
// desc: ....
//-----------------------------------------------------------------------------
void Chuck_Frame::pop_scope( vector<Chuck_Local *> & out )
{
    // sanity
    assert( this->stack.size() > 0 );

    // the local
    Chuck_Local * local = NULL;
    
    // loop
    while( this->stack.size() && this->stack.back() )
    {
        // last thing
        local = stack.back();
        // pop
        stack.pop_back();
        // check if not null
        if( local )
        {
            // offset
            curr_offset -= local->size;
            // copy out (NOTE: these locals should be memory managed outside)
            out.push_back( local );
        }
    }
    
    // sanity (should be at least one left)
    assert( this->stack.size() > 0 );
    // get ride of null boundary character (added 1.3.0.0)
    this->stack.pop_back();
}
