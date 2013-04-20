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
// name: util_math.h
// desc: a mini-compatibility library for math functions.
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Philip Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __UTIL_MATH_H__
#define __UTIL_MATH_H__

#include <math.h>

#if defined (__cplusplus) || defined(_cplusplus)  
extern "C" {
#endif 

#ifdef __PLATFORM_WIN32__

    double round( double a );
    double trunc( double a );
    double remainder( long a, long b );

#endif

// mtof
double mtof( double f );
// ftom
double ftom( double f );
// powtodb
double powtodb( double f );
// rmstodb
double rmstodb( double f );
// dbtopow
double dbtopow( double f );
// dbtorms
double dbtorms( double f );
// nextpow2
unsigned long nextpow2( unsigned long i );
// ensurepow2
unsigned long ensurepow2( unsigned long i );

#if defined (__cplusplus) || defined(_cplusplus)  
}
#endif



#endif 
