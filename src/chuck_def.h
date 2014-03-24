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
// file: chuck_def.h
// desc: ChucK defines for the system
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_DEF_H__
#define __CHUCK_DEF_H__

#include <stdlib.h>
#include <memory.h>
#include <assert.h>


// types
#define t_CKTIME                    double
#define t_CKDUR                     double
#define t_CKFLOAT                   double
#define t_CKDOUBLE                  double
#define t_CKSINGLE                  float
#define t_CKINT                     long
#define t_CKUINT                    unsigned t_CKINT
#define t_CKBOOL                    unsigned t_CKINT
#define t_CKBYTE                    unsigned char
#define t_CKVOID                    void
#define t_CKVOIDPTR                 void *

// complex type
typedef struct { t_CKFLOAT re ; t_CKFLOAT im ; } t_CKCOMPLEX;
// polar type
typedef struct { t_CKFLOAT modulus ; t_CKFLOAT phase ; } t_CKPOLAR;

// size
#define sz_TIME                     sizeof(t_CKTIME)
#define sz_DUR                      sizeof(t_CKDUR)
#define sz_FLOAT                    sizeof(t_CKFLOAT)
#define sz_DOUBLE                   sizeof(t_CKDOUBLE)
#define sz_SINGLE                   sizeof(t_CKSINGLE)
#define sz_INT                      sizeof(t_CKINT)
#define sz_UINT                     sizeof(t_CKUINT)
#define sz_BOOL                     sizeof(t_CKBOOL)
#define sz_BYTE                     sizeof(t_CKBYTE)
#define sz_VOIDPTR                  sizeof(t_CKVOIDPTR)
#define sz_COMPLEX                  sizeof(t_CKCOMPLEX)
#define sz_POLAR                    sizeof(t_CKPOLAR)
#define sz_VOID                     0
#define sz_WORD                     4

// kinds (added 1.3.1.0 to faciliate 64-bit support)
// to differentiate in case int and float have same size
#define kindof_VOID                0
#define kindof_INT                 1
#define kindof_FLOAT               2
#define kindof_COMPLEX             3

typedef char *                      c_str;
typedef const char *                c_constr;

// use 64-bit samples in the audio engine
// #define __CHUCK_USE_64_BIT_SAMPLE__

// sample
#ifdef __CHUCK_USE_64_BIT_SAMPLE__
#define SAMPLE                      double
#define SILENCE                     0.0
#define CK_DDN                      CK_DDN_DOUBLE
#else
#define SAMPLE                      float
#define SILENCE                     0.0f
#define CK_DDN                      CK_DDN_SINGLE
#endif

// sample complex
typedef struct { SAMPLE re ; SAMPLE im ; } t_CKCOMPLEX_SAMPLE;

// bool
#ifndef TRUE
#define TRUE                        1
#define FALSE                       0
#endif

// 3.1415926535897932384626433832795028841971693993751058209749445...
#define ONE_PI (3.14159265358979323846)
#define TWO_PI (2.0 * ONE_PI)
#define SQRT2  (1.41421356237309504880)

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)              do { if(x){ delete x; x = NULL; } } while(0)
#define SAFE_DELETE_ARRAY(x)        do { if(x){ delete [] x; x = NULL; } } while(0)
#define SAFE_RELEASE(x)             do { if(x){ x->release(); x = NULL; } } while(0)
#define SAFE_ADD_REF(x)             do { if(x){ x->add_ref(); } } while(0)
#define SAFE_REF_ASSIGN(lhs,rhs)    do { SAFE_RELEASE(lhs); (lhs) = (rhs); SAFE_ADD_REF(lhs); } while(0)
#endif

// max + min
#define ck_max(x,y)                 ( (x) >= (y) ? (x) : (y) )
#define ck_min(x,y)                 ( (x) <= (y) ? (x) : (y) )

// dedenormal
#define CK_DDN_SINGLE(f)            f = ( f >= 0 ? \
        ( ( f > (t_CKSINGLE)1e-15 && f < (t_CKSINGLE)1e15 ) ? f : (t_CKSINGLE)0.0 ) : \
        ( ( f < (t_CKSINGLE)-1e-15 && f > (t_CKSINGLE)-1e15 ) ? f : (t_CKSINGLE)0.0 ) )
#define CK_DDN_DOUBLE(f)            f = ( f >= 0 ? \
        ( ( f > (t_CKDOUBLE)1e-15 && f < (t_CKDOUBLE)1e15 ) ? f : 0.0 ) : \
        ( ( f < (t_CKDOUBLE)-1e-15 && f > (t_CKDOUBLE)-1e15 ) ? f : 0.0 ) )


// tracking
#if defined(__CHUCK_STAT_TRACK__)
#define CK_TRACK( stmt ) stmt
#else
#define CK_TRACK( stmt )
#endif

#ifdef __MACOSX_CORE__
#define __PLATFORM_MACOSX__
#endif

#if defined(__LINUX_ALSA__) || defined(__LINUX_JACK__) || defined(__LINUX_OSS__) || defined(__LINUX_PULSE__) || defined(__UNIX_JACK__)
// defined by default in Linux makefiles
//#define __PLATFORM_LINUX__
#endif

#ifdef __PLATFORM_WIN32__
#ifndef usleep
#define usleep(x) Sleep( (x / 1000 <= 0 ? 1 : x / 1000) )
#endif
#pragma warning (disable : 4996)  // stdio deprecation
#pragma warning (disable : 4786)  // stl debug info
#pragma warning (disable : 4312)  // type casts from void*
#pragma warning (disable : 4311)  // type casts to void*
#pragma warning (disable : 4244)  // truncation
#pragma warning (disable : 4068)  // unknown pragma
#pragma warning (disable : 4018)  // signed/unsigned mismatch
#endif

#ifdef __CHIP_MODE__
#define __DISABLE_MIDI__
//#define __DISABLE_SNDBUF__
#define __DISABLE_WATCHDOG__
#define __DISABLE_RAW__
#define __DISABLE_KBHIT__
#define __DISABLE_PROMPTER__
#define __DISABLE_RTAUDIO__
#define __DISABLE_OTF_SERVER__
#define __ALTER_HID__
#define __ALTER_ENTRY_POINT__
#define __STK_USE_SINGLE_PRECISION__
#endif




#endif
