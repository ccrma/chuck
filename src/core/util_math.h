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
// desc: a mini-compatibility library for math functions
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Philip Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __UTIL_MATH_H__
#define __UTIL_MATH_H__

#include <math.h>
#include "chuck_def.h" // 1.4.1.0 (ge) for t_CKINT

#if defined (__cplusplus) || defined(_cplusplus)
extern "C" {
#endif

// 1.4.1.1 (ge) removed -- seems no longer needed on more modern windows
#ifdef __PLATFORM_WINDOWS__
#ifdef __CK_MATH_DEFINE_ROUND_TRUNC__
    double round( double a );
    double trunc( double a );
#endif
#endif

// 1.4.1.0 (ge) need this since remainder still not found (VC++ 2010)
double ck_remainder( double a, double b );


// chuck random wrapper, return t_CKINT in [0,CK_RANDOM_MAX]
t_CKINT ck_random();
// chuck random float wrapper, return t_CKFLOAT in [0,1]
t_CKFLOAT ck_random_f();
// chuck srandom wrapper
void ck_srandom( unsigned seed );
// randomizer wrapper (use this instead of seeding for initial)
void ck_randomize();


//-----------------------------------------------------------------------------
#ifndef __OLDSCHOOL_RANDOM__
//-----------------------------------------------------------------------------
#define CK_RANDOM_MAX 0x7fffffff
//-----------------------------------------------------------------------------
#else // __OLDSCHOOL_RANDOM__ (enable only for compatibility pre-c++11)
//-----------------------------------------------------------------------------
  #ifndef __PLATFORM_WINDOWS__
    #define CK_RANDOM_MAX 0x7fffffff
  #else // __PLATFORM_WINDOWS__
    #define CK_RANDOM_MAX RAND_MAX
  #endif
//-----------------------------------------------------------------------------
#endif // __OLDSCHOOL_RANDOM__
//-----------------------------------------------------------------------------


// mtof
double ck_mtof( double f );
// ftom
double ck_ftom( double f );
// powtodb
double ck_powtodb( double f );
// rmstodb
double ck_rmstodb( double f );
// dbtopow
double ck_dbtopow( double f );
// dbtorms
double ck_dbtorms( double f );
// nextpow2
unsigned long ck_nextpow2( unsigned long i );
// ensurepow2
unsigned long ck_ensurepow2( unsigned long i );

// magnitude of complex number
t_CKFLOAT ck_complex_magnitude( const t_CKCOMPLEX & cmp );
// phase of complex number
t_CKFLOAT ck_complex_phase( const t_CKCOMPLEX & cmp );
// magnitude of vec2
t_CKFLOAT ck_vec2_magnitude( const t_CKVEC2 & v );
// phase of vec2
t_CKFLOAT ck_vec2_phase( const t_CKVEC2 & v );
// magnitude of vec3
t_CKFLOAT ck_vec3_magnitude( const t_CKVEC3 & v );
// magnitude of vec4
t_CKFLOAT ck_vec4_magnitude( const t_CKVEC4 & v );



#if defined (__cplusplus) || defined(_cplusplus)
}
#endif




#endif
