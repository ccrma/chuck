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
// file: chuck_temp.h
// desc: ...
//
// author: Andrew Appel (appel@cs.princeton.edu)
// modified: Ge Wang (gewang@cs.princeton.edu)
//           Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_TEMP_H__
#define __CHUCK_TEMP_H__

#include <stdio.h>
#include "chuck_symbol.h"
#include "chuck_utils.h"


#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif


// Temp_Temp
typedef struct Temp_Temp_ *Temp_Temp;
Temp_Temp Temp_new_temp( void );

// Temp_Temp_List
typedef struct Temp_Temp_List_ *Temp_Temp_List;
struct Temp_Temp_List_
{
    Temp_Temp head;
    Temp_Temp_List tail;
};
Temp_Temp_List Temp_new_temp_list( Temp_Temp h, Temp_Temp_List t );

// Temp_Label
typedef S_Symbol Temp_Label;
Temp_Label Temp_new_label( void );
Temp_Label Temp_named_label( c_str name );
c_str Temp_label_string( Temp_Label s );

// Temp_Label_List
typedef struct Temp_Label_List_ *Temp_Label_List;
struct Temp_Label_List_ { Temp_Label head; Temp_Label_List tail; };
Temp_Label_List Temp_new_label_list( Temp_Label h, Temp_Label_List t );

// Temp_Map
typedef struct Temp_Map_ *Temp_Map;
Temp_Map Temp_empty( void );
Temp_Map Temp_layer_map( Temp_Map over, Temp_Map under );
void Temp_enter( Temp_Map m, Temp_Temp t, c_str s );
c_str Temp_look( Temp_Map m, Temp_Temp t );
void Temp_dump_map( FILE *out, Temp_Map m );

Temp_Map Temp_name( void );


#if defined(_cplusplus) || defined(__cplusplus)
}
#endif


#endif
