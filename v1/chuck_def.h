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
// file: chuck_def.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_DEF_H__
#define __CHUCK_DEF_H__

#include <stdlib.h>
#include <memory.h>


// types
#define t_CKTIME                    double
#define t_CKDUR                     double
#define t_CKFLOAT                   double
#define t_CKDOUBLE                  double
#define t_CKSINGLE                  float
#define t_CKINT                     long
#define t_CKDWORD                   unsigned long
#define t_CKUINT                    t_CKDWORD
#define t_CKBOOL                    t_CKDWORD
#define t_CKBYTE                    unsigned char

typedef char *                      c_str;
typedef const char *                c_constr;

// sample
#define SAMPLE_FLOAT                float
#define SAMPLE                      SAMPLE_FLOAT

// bool
#ifndef TRUE
#define TRUE                        1
#define FALSE                       0
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)              { if(x){ delete x; x = NULL; } }
#define SAFE_DELETE_ARRAY(x)        { if(x){ delete [] x; x = NULL; } }
#endif

#ifdef __MACOSX_CORE__
#define __PLATFORM_MACOSX__
#endif

#if defined(__LINUX_ALSA__) || defined(__LINUX_JACK__) || defined(__LINUX_OSS__) 
#define __PLATFORM_LINUX__
#endif

#ifdef __PLATFORM_WIN32__
#define usleep(x) Sleep( (x / 1000 <= 0 ? 1 : x / 1000) )
#pragma warning (disable : 4996)  //stdio deprecation
#pragma warning (disable : 4312)  //type casts from void*
#pragma warning (disable : 4311)  //type casts to void*
#pragma warning (disable : 4244)  //truncation
#endif


#endif
