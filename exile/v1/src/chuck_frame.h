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
// file: chuck_frame.h
// desc: ...
//
// author: Andrew Appel (appel@cs.princeton.edu)
// modified: Ge Wang (gewang@cs.princeton.edu)
//           Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_FRAME_H__
#define __CHUCK_FRAME_H__


#include "chuck_temp.h"




// frame
typedef struct F_Frame_ * F_Frame;
typedef struct F_Access_ * F_Access;




//-----------------------------------------------------------------------------
// name: struct F_Access_
// desc: ...
//-----------------------------------------------------------------------------
struct F_Access_
{
    unsigned int offset;
    unsigned int global;
    unsigned int size;
};




// access list
typedef struct F_Access_List_ * F_Access_List;
struct F_Access_List_
{
    F_Access head;
    F_Access_List tail;
};

// functions
F_Frame F_new_frame( Temp_Label name );
Temp_Label F_name( F_Frame f );
unsigned int F_offset( F_Access a );
F_Access_List F_formals( F_Frame f );
F_Access F_alloc_local( F_Frame f, unsigned int size, unsigned int is_global );
unsigned int F_stack_depth( F_Frame f );
void F_begin_scope( F_Frame f );
void F_end_scope( F_Frame f );




#endif
