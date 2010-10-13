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
// file: chuck_frame.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
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
                                        t_CKBOOL is_ref )
{
    // alloc
    Chuck_Local * local = new Chuck_Local;
    // size
    local->size = size;
    // the offset
    local->offset = this->curr_offset;
    // ref
    local->is_ref = is_ref;
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
// name: pop_scope()
// desc: ....
//-----------------------------------------------------------------------------
void Chuck_Frame::pop_scope( vector<Chuck_Local *> & out )
{
    // sanity
    assert( this->stack.size() > 0 );

    Chuck_Local * local = NULL;

    // loop
    while( this->stack.size() && this->stack.back() )
    {
        // last thing
        local = stack.back();
        // free
        stack.pop_back();
        if( local )
        {
            // offset
            curr_offset -= local->size;
            // copy out
            out.push_back( local );
        }
    }
}
