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
// file: ulib_math.h
// desc: class library for 'Math'
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __ULIB_MATH_H__
#define __ULIB_MATH_H__

#include "chuck_dl.h"

// query
DLL_QUERY libmath_query( Chuck_DL_Query * QUERY );

// impl
CK_DLL_SFUN( sin_impl );
CK_DLL_SFUN( cos_impl );
CK_DLL_SFUN( tan_impl );
CK_DLL_SFUN( cot_impl );
CK_DLL_SFUN( asin_impl );
CK_DLL_SFUN( acos_impl );
CK_DLL_SFUN( atan_impl );
CK_DLL_SFUN( atan2_impl );
CK_DLL_SFUN( sinh_impl );
CK_DLL_SFUN( cosh_impl );
CK_DLL_SFUN( tanh_impl );
CK_DLL_SFUN( hypot_impl );

CK_DLL_SFUN( pow_impl );
CK_DLL_SFUN( sqrt_impl );
CK_DLL_SFUN( exp_impl );
CK_DLL_SFUN( log_impl );
CK_DLL_SFUN( log2_impl );
CK_DLL_SFUN( log10_impl );
CK_DLL_SFUN( floor_impl );
CK_DLL_SFUN( ceil_impl );
CK_DLL_SFUN( round_impl );
CK_DLL_SFUN( trunc_impl );
CK_DLL_SFUN( fmod_impl );
CK_DLL_SFUN( remainder_impl );
CK_DLL_SFUN( min_impl );
CK_DLL_SFUN( max_impl );
CK_DLL_SFUN( isinf_impl );
CK_DLL_SFUN( isnan_impl );
CK_DLL_SFUN( floatMax_impl );
CK_DLL_SFUN( intMax_impl );

CK_DLL_SFUN( nextpow2_impl );
CK_DLL_SFUN( ensurepow2_impl );

CK_DLL_SFUN( re_impl );
CK_DLL_SFUN( im_impl );
CK_DLL_SFUN( modulus_impl );
CK_DLL_SFUN( phase_impl );
CK_DLL_SFUN( rtop_impl );
CK_DLL_SFUN( ptor_impl );

CK_DLL_SFUN( random_impl );
CK_DLL_SFUN( randomf_impl );
CK_DLL_SFUN( random2f_impl );
CK_DLL_SFUN( random2_impl );
CK_DLL_SFUN( srandom_impl );

// max for random functions
#ifdef __WINDOWS_DS__
#define CK_RANDOM_MAX RAND_MAX
#else
#define CK_RANDOM_MAX 0x7fffffff
#endif



#endif
