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
// file: chuck_errmsg.h
// desc: error msg
//
// author: Andrew Appel (appel@cs.princeton.edu)
// modified: Ge Wang (gewang.cs.princeton.edu)
//           Perry R. Cook (prc@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_ERRORMSG_H__
#define __CHUCK_ERRORMSG_H__

#include "chuck_def.h"
#include <stdio.h>


#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif


extern t_CKBOOL EM_anyErrors;
extern int EM_tokPos;
extern int EM_lineNum;

void EM_newline( );

void EM_error( int, c_str, ... );
void EM_error2( int, c_str, ... );
void EM_error3( c_str, ... );
void EM_impossible( c_str, ... );
t_CKBOOL EM_reset( c_str filename, FILE * fd );
const char * EM_lasterror();

const char * mini( const char * str );

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif


#endif
