/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
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
CK_DLL_SFUN( exp2_impl );
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
CK_DLL_SFUN( min_int_impl );
CK_DLL_SFUN( max_int_impl );
CK_DLL_SFUN( isinf_impl );
CK_DLL_SFUN( isnan_impl );
CK_DLL_SFUN( equal_impl );
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
CK_DLL_SFUN( randomize_impl );

CK_DLL_SFUN( gauss_impl );

CK_DLL_SFUN( cossim_impl );
CK_DLL_SFUN( cossim3d_impl );
CK_DLL_SFUN( cossim4d_impl );

CK_DLL_SFUN( euclidean_impl );
CK_DLL_SFUN( euclidean2d_impl );
CK_DLL_SFUN( euclidean3d_impl );
CK_DLL_SFUN( euclidean4d_impl );

CK_DLL_SFUN( map_impl );
CK_DLL_SFUN( map2_impl );




#endif
