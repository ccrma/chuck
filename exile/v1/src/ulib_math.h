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
// file: ulib_math.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __ULIB_MATH_H__
#define __ULIB_MATH_H__

#include "chuck_dl.h"

// query
DLL_QUERY libmath_query( Chuck_DL_Query * QUERY );

// impl
CK_DLL_FUNC( sin_impl );
CK_DLL_FUNC( cos_impl );
CK_DLL_FUNC( tan_impl );
CK_DLL_FUNC( cot_impl );
CK_DLL_FUNC( asin_impl );
CK_DLL_FUNC( acos_impl );
CK_DLL_FUNC( atan_impl );
CK_DLL_FUNC( atan2_impl );
CK_DLL_FUNC( sinh_impl );
CK_DLL_FUNC( cosh_impl );
CK_DLL_FUNC( tanh_impl );
CK_DLL_FUNC( hypot_impl );

CK_DLL_FUNC( pow_impl );
CK_DLL_FUNC( sqrt_impl );
CK_DLL_FUNC( exp_impl );
CK_DLL_FUNC( log_impl );
CK_DLL_FUNC( log2_impl );
CK_DLL_FUNC( log10_impl );
CK_DLL_FUNC( floor_impl );
CK_DLL_FUNC( ceil_impl );
CK_DLL_FUNC( round_impl );
CK_DLL_FUNC( trunc_impl );
CK_DLL_FUNC( fmod_impl );
CK_DLL_FUNC( remainder_impl );
CK_DLL_FUNC( min_impl );
CK_DLL_FUNC( max_impl );

CK_DLL_FUNC( nextpow2_impl );

#endif
