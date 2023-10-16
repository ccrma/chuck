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

// chuck types
//-------------------------------------------
#ifndef __EMSCRIPTEN__
//-------------------------------------------
#define t_CKTIME                    double
#define t_CKDUR                     double
#define t_CKSINGLE                  float
#define t_CKDOUBLE                  double
#define t_CKFLOAT                   double
#define CK_FLT_MAX                  DBL_MAX
#define CK_FLT_MIN                  DBL_MIN
#ifdef _WIN64                       // REFACTOR-2017
#define t_CKINT                     long long // REFACTOR-2017
#define CK_INT_MAX                  LLONG_MAX
#else
#define t_CKINT                     long
#define CK_INT_MAX                  LONG_MAX
#endif
//-------------------------------------------
#else // below is for __EMSCRIPTEN__
//-------------------------------------------
#define t_CKTIME                    double
#define t_CKDUR                     double
#define t_CKFLOAT                   double
#define t_CKDOUBLE                  double
#define t_CKSINGLE                  float
#define t_CKINT                     int // REFACTOR-2017
#define CK_FLT_MAX                  DBL_MAX
#define CK_FLT_MIN                  DBL_MIN
#define CK_INT_MAX                  INT_MAX
//-------------------------------------------
#endif
//-------------------------------------------

// additional types
#define t_CKUINT                    unsigned t_CKINT
#define t_CKBOOL                    unsigned t_CKINT
#define t_CKBYTE                    unsigned char
#define t_CKVOID                    void
#define t_CKVOIDPTR                 void *

// complex type
typedef struct { t_CKFLOAT re ; t_CKFLOAT im ; } t_CKCOMPLEX;
// polar type
typedef struct { t_CKFLOAT modulus ; t_CKFLOAT phase ; } t_CKPOLAR;

// vector types
typedef struct { t_CKFLOAT x ; t_CKFLOAT y ; } t_CKVEC2;
typedef struct { t_CKFLOAT x ; t_CKFLOAT y ; t_CKFLOAT z ; } t_CKVEC3;
typedef struct { t_CKFLOAT x ; t_CKFLOAT y ; t_CKFLOAT z ; t_CKFLOAT w ; } t_CKVEC4;
typedef struct { t_CKUINT N ; t_CKFLOAT * values ; } t_CKVECTOR;

// size of types
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
#define sz_VEC2                     sizeof(t_CKVEC2)
#define sz_VEC3                     sizeof(t_CKVEC3)
#define sz_VEC4                     sizeof(t_CKVEC4)
#define sz_VECTOR                   sizeof(t_CKVECTOR)
#define sz_VOID                     0
#define sz_WORD                     4

// kinds (added 1.3.1.0 to faciliate 64-bit support)
// to differentiate in case int and float have same size
enum te_KindOf
{
    kindof_VOID = 0,
    kindof_INT,
    kindof_FLOAT,
    kindof_VEC2, // 1.5.1.7 (ge) was kindof_COMPLEX
    kindof_VEC3,
    kindof_VEC4
};

typedef char *                      c_str;
typedef const char *                c_constr;

// use 64-bit samples in the audio engine
// #define __CHUCK_USE_64_BIT_SAMPLE__

// type for audio sample
//-------------------------------------------
#ifdef __CHUCK_USE_64_BIT_SAMPLE__
//-------------------------------------------
#define SAMPLE                      double
#define CK_SILENCE                  0.0
#define CK_DDN                      CK_DDN_DOUBLE
//-------------------------------------------
#else
//-------------------------------------------
#define SAMPLE                      float
#define CK_SILENCE                  0.0f
#define CK_DDN                      CK_DDN_SINGLE
//-------------------------------------------
#endif
//-------------------------------------------
#define t_CKSAMPLE                  SAMPLE

// sample complex
typedef struct { SAMPLE re ; SAMPLE im ; } t_CKCOMPLEX_SAMPLE;

// bool
#ifndef TRUE
#define TRUE                        1
#define FALSE                       0
#endif

// special non-zero value to denote "no value" | added 1.4.2.0 (ge)
#define CK_NO_VALUE                 0xffffffff

// 3.1415926535897932384626433832795028841971693993751058209749445...
#define CK_ONE_PI                   (3.14159265358979323846)
#define CK_TWO_PI                   (2.0 * CK_ONE_PI)
#define CK_SQRT2                    (1.41421356237309504880)

// macro for cleaning up: tests for NULL before and sets to NULL after
#define CK_SAFE_DELETE(x)           do { if(x){ delete (x); (x) = NULL; } } while(0)
#define CK_SAFE_DELETE_ARRAY(x)     do { if(x){ delete [] (x); (x) = NULL; } } while(0)
#define CK_SAFE_RELEASE(x)          do { if(x){ (x)->release(); (x) = NULL; } } while(0)
#define CK_SAFE_ADD_REF(x)          do { if(x){ (x)->add_ref(); } } while(0)
// NOTE the ref assign accounts for the case that lhs == rhs and the reference==1: add BEFORE release
#define CK_SAFE_REF_ASSIGN(lhs,rhs) do { Chuck_VM_Object * temp = (lhs); (lhs) = (rhs); CK_SAFE_ADD_REF(lhs); CK_SAFE_RELEASE(temp); } while(0)
#define CK_SAFE_FREE(x)             do { if(x){ free(x); (x) = NULL; } } while(0)
#define CK_SAFE_UNLOCK_DELETE(x)    do { if(x){ (x)->unlock(); delete (x); (x) = NULL; } } while(0)

// max + min
#define ck_max(x,y)                 ( (x) >= (y) ? (x) : (y) )
#define ck_min(x,y)                 ( (x) <= (y) ? (x) : (y) )

#ifndef __arm__
// dedenormal
#define CK_DDN_SINGLE(f)            f = ( f >= 0 ? \
        ( ( f > (t_CKSINGLE)1e-15 && f < (t_CKSINGLE)1e15 ) ? f : (t_CKSINGLE)0.0 ) : \
        ( ( f < (t_CKSINGLE)-1e-15 && f > (t_CKSINGLE)-1e15 ) ? f : (t_CKSINGLE)0.0 ) )
#define CK_DDN_DOUBLE(f)            f = ( f >= 0 ? \
        ( ( f > (t_CKDOUBLE)1e-15 && f < (t_CKDOUBLE)1e15 ) ? f : 0.0 ) : \
        ( ( f < (t_CKDOUBLE)-1e-15 && f > (t_CKDOUBLE)-1e15 ) ? f : 0.0 ) )
#else
#define CK_DDN_SINGLE(f) (f)
#define CK_DDN_DOUBLE(f) (f)
#endif // __arm__

// stats tracking
#if defined(__CHUCK_STAT_TRACK__)
#define CK_TRACK( stmt ) stmt
#else
#define CK_TRACK( stmt )
#endif


//-------------------------------------------
// operating system identification
// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// http://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
//-------------------------------------------

//-------------------------------------------
// platform: Apple
// related macros: __MACOSX_CORE__ __CHIP_MODE__
// NOTE: __CHIP_MODE__ is ChucK for iPhone; implies iOS
//-------------------------------------------
#if defined(__APPLE__) // && defined(__MACH__)
//-------------------------------------------
  #ifndef __PLATFORM_APPLE__
  #define __PLATFORM_APPLE__
  #endif

  // get information about the target sub-platform
  #include <TargetConditionals.h>
  #if TARGET_OS_MAC == 1
    #if TARGET_OS_IPHONE != 1
      #define __PLATFORM_MACOS__ // macOS
    #else // TARGET_OS_IPHONE not 0
      #define __PLATFORM_IOS__ // iOS, tvOS, or watchOS device
      // even more information within iOS
      #if TARGET_OS_EMBEDDED == 1
        #define __PLATFORM_IOS_DEVICE__ // device actual
      #else // TARGET_OS_EMBEDDED not 1
        #define __PLATFORM_IOS_SIMULATOR__ // simulator
      #endif
    #endif
  #else // TARGET_OS_MAC not 1
    // apparently on Apple; but cannot determine platform target
    // #error "[chuck build]: unknown Apple platform target"
  #endif
//-------------------------------------------
#endif // defined(__APPLE__) && defined(__MACH__)
//-------------------------------------------


//-------------------------------------------
// platform: linux
// related macros: __LINUX_ALSA__ __LINUX_PULSE__ __LINUX_OSS__ __LINUX_JACK__ __UNIX_JACK__
//-------------------------------------------
#if defined(__linux__)
//-------------------------------------------
  #ifndef __PLATFORM_LINUX__
  #define __PLATFORM_LINUX__
  #endif
//-------------------------------------------
#endif // defined(__linux__)
//-------------------------------------------


//-------------------------------------------
// platform: windows
// related macros: __WINDOWS_DS__ __WINDOWS_WASAPI__ __WINDOWS_ASIO__
// NOTE: Windows defines _WIN32 for both 32-bit and 64-bit
//-------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
//-------------------------------------------
  #ifndef __PLATFORM_WINDOWS__
  #define __PLATFORM_WINDOWS__
  #endif

  // legacy; new code should use __PLATFORM_WINDOWS__
  #ifndef __PLATFORM_WIN32__
  #define __PLATFORM_WIN32__
  #endif
//-------------------------------------------
#endif // defined(_WIN32) || defined(_WIN64)
//-------------------------------------------


//-------------------------------------------
// platform: Cygwin (on Windows)
//-------------------------------------------
#if defined(__CYGWIN__)
//-------------------------------------------
  #ifndef __PLATFORM_CYGWIN__
  #define __PLATFORM_CYGWIN__
  #endif
  #ifndef __WINDOWS_PTHREAD__
  #define __WINDOWS_PTHREAD__
  #endif
//-------------------------------------------
#endif // defined(__CYGWIN__)
//-------------------------------------------


//-------------------------------------------
// platform: web assembly
//-------------------------------------------
#if defined(__EMSCRIPTEN__)
//-------------------------------------------
  #ifndef __PLATFORM_EMSCRIPTEN__
  #define __PLATFORM_EMSCRIPTEN__
  #endif
//-------------------------------------------
#endif // defined(__EMSCRIPTEN__)
//-------------------------------------------


//-------------------------------------------
// platform: android
// NOTE: android is based on linux; linux macros are also defined
//-------------------------------------------
#if defined(__ANDROID__)
//-------------------------------------------
  #ifndef __PLATFORM_ANDROID__
  #define __PLATFORM_ANDROID__
  #endif
//-------------------------------------------
#endif // defined(__ANDROID__)
//-------------------------------------------


#ifdef __PLATFORM_WINDOWS__
// 1.4.2.0 (ge and spencer) added for legacy windows, as part of switch to snprintf
// https://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
#if defined(_MSC_VER) && _MSC_VER < 1900
  #define snprintf(buf,len, format,...) _snprintf_s(buf, len,len, format, __VA_ARGS__)
#endif
#pragma warning (disable : 4996)  // stdio deprecation
#pragma warning (disable : 4786)  // stl debug info
#pragma warning (disable : 4312)  // type casts from void*
#pragma warning (disable : 4311)  // type casts to void*
#pragma warning (disable : 4244)  // truncation
#pragma warning (disable : 4068)  // unknown pragma
#pragma warning (disable : 4018)  // signed/unsigned mismatch
#endif // #ifdef __PLATFORM_WINDOWS__

#ifdef __CHIP_MODE__
#define __DISABLE_MIDI__
//#define __DISABLE_SNDBUF__
#define __DISABLE_WATCHDOG__
#define __DISABLE_RAW__
#define __DISABLE_KBHIT__
#define __DISABLE_PROMPTER__
#define __DISABLE_OTF_SERVER__
#define __ALTER_HID__
#define __ALTER_ENTRY_POINT__
#define __STK_USE_SINGLE_PRECISION__
#endif

#ifdef __arm__
// enable additional optimization
#define __STK_USE_SINGLE_PRECISION__
#endif // __arm__

#endif
