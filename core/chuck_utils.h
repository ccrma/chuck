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
// file: chuck_utils.h
// desc: common utils, adapted from Tiger Compiler Andrew Appel
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// adapted from: Andrew Appel (appel@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_UTILS_H__
#define __CHUCK_UTILS_H__

#include "chuck_def.h"


#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif


typedef struct U_boolList_ * U_boolList;
struct U_boolList_
{
    t_CKBOOL head;
    U_boolList tail;
};


void * checked_malloc( int size );
c_str cc_str( char * );
U_boolList U_BoolList( t_CKBOOL head, U_boolList tail );


#if defined(_cplusplus) || defined(__cplusplus)
}
#endif


#endif
