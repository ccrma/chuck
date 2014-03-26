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
// file: errmsg.h
// desc: functions used in all phases of the compiler to give error messages
//       based on Andrew Appel's Tiger code
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on code by: Andrew Appel (appel@cs.princeton.edu)
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

// EM_extLineNum is synced with EM_lineNum in lexer/parser phase, then synced 
// with scanner/typechecker (EM_lineNum is not synced with scanner/typechecker)
extern int EM_extLineNum; 

void EM_newline( );

// levels
#define CK_LOG_CRAZY            10 // set this to log everything
#define CK_LOG_FINEST           9
#define CK_LOG_FINER            8
#define CK_LOG_FINE             7
#define CK_LOG_CONFIG           6
#define CK_LOG_INFO             5
#define CK_LOG_WARNING          4
#define CK_LOG_SEVERE           3
#define CK_LOG_SYSTEM           2
#define CK_LOG_CORE             1
#define CK_LOG_NONE             0  // set this to log nothing

void EM_log( int, c_constr, ... );
void EM_setlog( int );
void EM_pushlog();
void EM_poplog();

// actual level
extern int g_loglevel;
// macro to compare
#define DO_LOG(x) ( x <= g_loglevel )

void EM_error( int, c_constr, ... );
void EM_error2( int, c_constr, ... );
void EM_error2b( int, c_constr, ... );
void EM_error3( c_constr, ... );
void EM_impossible( c_constr, ... );
t_CKBOOL EM_reset( c_constr filename, FILE * fd );
void EM_change_file( c_constr filename );
const char * EM_lasterror();
void EM_reset_msg();

const char * mini( const char * str );
const char * mini_type( const char * str );

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif


#endif
