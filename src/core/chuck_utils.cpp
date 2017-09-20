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
// file: chuck_utils.cpp
// desc: common utils, adapted from Tiger Compiler Andrew Appel
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// adapted from: Andrew Appel (appel@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chuck_utils.h"
#include "chuck_errmsg.h"




void * checked_malloc( int len )
{
    if( !len ) return NULL;

    void *p = calloc( len, 1 );
    if( !p )
    {
        EM_error2( 0, "out of memory!\n" );
        exit( 1 );
    }

    return p;
}




c_str cc_str( char * s )
{
    c_str p = (c_str)checked_malloc( strlen(s)+1 );
    strcpy( p, s );

    return p;
}




U_boolList U_BoolList( t_CKBOOL head, U_boolList tail )
{
    U_boolList list = (U_boolList)checked_malloc( sizeof(*list) );
    list->head = head;
    list->tail = tail;

    return list;
}
