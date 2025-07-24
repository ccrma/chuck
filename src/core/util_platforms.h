/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
  Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
// name: util_platforms.h
// desc: platform-specific utilities, e.g., for Android and various
//
// author: Andriy Kunitsyn (kunitzin@gmail.com) | ChuckAndroid
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Summer 2021
//-----------------------------------------------------------------------------
#ifndef __UTIL_PLATFORMS_H__
#define __UTIL_PLATFORMS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "chuck_def.h"




// try to open a file by filname, adding extension if needed
// filename argument is changed to name of actual opened file
FILE * ck_openFileAutoExt( std::string & filenameMutable,
                           const std::string & extension );
// platform-abstracted temp file creation
FILE * ck_tmpfile();

// check if is dir
t_CKBOOL ck_isdir( const std::string & path );
// check if file exists
t_CKBOOL ck_fileexists( const std::string & path );

// do any platform-specific setup to enable ANSI escape codes
t_CKBOOL ck_configANSI_ESCcodes();

// are we output to a TTY (teletype, character by character)
// (vs say a file stream; helpful for determining if we should
// suppress printing ANSI escapes codes, e.g., color codes
// to the output stream
t_CKBOOL ck_isatty( int fd );
// get a general idea (typically stderr is used here)
t_CKBOOL ck_isatty();

// get TTY terminal width, or a default width (if can't get from TTY)
t_CKUINT ck_ttywidth();
// set default TTY width
void ck_ttywidth_setdefault( t_CKUINT width );
// get default TTY width
t_CKUINT ck_ttywidth_default();

// abstraction for getline, c edition
size_t ck_getline( char ** lineptr, size_t * n, FILE * stream );

// sleep
void ck_usleep( t_CKUINT microseconds );

// gettimeofday
int ck_gettimeofday( long * tv_sec, long * tv_usec, void * tzp );




//-----------------------------------------------------------------------------
// Chuck platform string | 1.5.5.2 (azaday)
//-----------------------------------------------------------------------------
// note: this must be placed *after* the platform defines above
//-----------------------------------------------------------------------------
#if defined( __PLATFORM_MACOS__)
  #define CHUCK_PLATFORM_STRING  "mac"
#elif defined(__PLATFORM_LINUX__)
  #define CHUCK_PLATFORM_STRING  "linux"
#elif defined(__PLATFORM_WINDOWS__)
  #define CHUCK_PLATFORM_STRING  "windows"
#elif defined(__PLATFORM_EMSCRIPTEN__)
  #define CHUCK_PLATFORM_STRING  "web"
#elif defined(__PLATFORM_IOS__)
  #define CHUCK_PLATFORM_STRING  "ios"
#elif defined(__PLATFORM_ANDROID__)
  #define CHUCK_PLATFORM_STRING  "android"
#elif defined(__PLATFORM_CYGWIN__)
  #define CHUCK_PLATFORM_STRING  "cygwin" // not sure if needed, as cygwin implies windows
#elif
  #define CHUCK_PLATFORM_STRING "unknown"
#endif





//-----------------------------------------------------------------------------
#if defined(__PLATFORM_APPLE__) && !defined(__CHIP_MODE__)
//-----------------------------------------------------------------------------
#include <CoreFoundation/CoreFoundation.h>

// struct to hold version number
struct ck_OSVersion
{
    short major;
    short minor;
    short patch;

    ck_OSVersion() { reset(); }
    void reset() { major = minor = patch = 0; }
    std::string str();
};

// get darwin kernel release version
ck_OSVersion ck_darwin_version();
// get macOS version
ck_OSVersion ck_macOS_version();
//---------------------------------------------------------
// Darwin operating system version to macOS release version
// https://en.wikipedia.org/wiki/Darwin_(operating_system)
// 25.x.x. macOS 16.x.x Tahoe
// 24.x.x. macOS 15.x.x Sequoia
// 23.x.x. macOS 14.x.x Sonoma
// 22.x.x. macOS 13.x.x Ventura
// 21.x.x. macOS 12.x.x Monterey
// 20.x.x. macOS 11.x.x Big Sur
// 19.x.x. macOS 10.15.x Catalina
// 18.x.x. macOS 10.14.x Mojave
// 17.x.x. macOS 10.13.x High Sierra
// 16.x.x  macOS 10.12.x Sierra
// 15.x.x  OS X  10.11.x El Capitan
// 14.x.x  OS X  10.10.x Yosemite
// 13.x.x  OS X  10.9.x  Mavericks
// 12.x.x  OS X  10.8.x  Mountain Lion
// 11.x.x  OS X  10.7.x  Lion
// 10.x.x  OS X  10.6.x  Snow Leopard
//  9.x.x  OS X  10.5.x  Leopard
//  8.x.x  OS X  10.4.x  Tiger
//  7.x.x  OS X  10.3.x  Panther
//  6.x.x  OS X  10.2.x  Jaguar
//  5.x    OS X  10.1.x  Puma
//  4.x    OS X  10.0.x  Cheetah
//---------------------------------------------------------

// get up time | replacement for UpTime()
AbsoluteTime ck_macOS_uptime();

#endif // #if defined(__PLATFORM_APPLE__) && !defined(__CHIP_MODE__)




//-----------------------------------------------------------------------------
#ifdef __ANDROID__
//-----------------------------------------------------------------------------
#include <jni.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// name: namespace ChuckAndroid
// desc: android specific ChucK utilities
//-----------------------------------------------------------------------------
namespace ChuckAndroid
{
    JavaVM * getJVM();
    FILE * getTemporaryFile() noexcept;
    bool copyJARURLFileToTemporary(const char * jar_url, int * fd) noexcept;
};

#endif // __ANDROID__




#endif // __UTIL_PLATFORMS_H__
