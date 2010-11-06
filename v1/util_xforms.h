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
// name: util_xform.h
// desc: transforms
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          FFT - from CARL distribution
// date: 11.27.2003
//-----------------------------------------------------------------------------
#ifndef __UTIL_XFORM_H__
#define __UTIL_XFORM_H__

#include <math.h>


// complex type
typedef struct { float re ; float im ; } complex;
// polar type
typedef struct { float modulus ; float phase ; } polar;

// complex absolute value
#define cmp_abs(x) ( sqrt( (x).re * (x).re + (x).im * (x).im ) )
#define __modulus(x) cmp_abs(x)
#define __phase(x) ( atan2((double)(x).re, (double)(x).im) )

#define FFT_FORWARD 1
#define FFT_INVERSE 0
#define make_window hanning

// c linkage
#if ( defined( __cplusplus ) || defined( _cplusplus ) )
  extern "C" {
#endif

// make the window
void hanning( float * window, unsigned long length );
void hamming( float * window, unsigned long length );
void blackman( float * window, unsigned long length );
// apply the window
void apply_window( float * data, float * window, unsigned long length );

// real fft, N must be power of 2
void rfft( float * x, long N, unsigned int forward );
// complex fft, NC must be power of 2
void cfft( float * x, long NC, unsigned int forward );

// c linkage
#if ( defined( __cplusplus ) || defined( _cplusplus ) )
  }
#endif

#endif
