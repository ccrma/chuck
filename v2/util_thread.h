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
// name: util_thread.h
// desc: ...
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
// based on STK's Thread
//          Perry R. Cook + Gary Scavone
// date: autumn 2004
//-----------------------------------------------------------------------------
#ifndef __UTIL_THREAD_H__
#define __UTIL_THREAD_H__


#include "chuck_def.h"


#if ( defined(__PLATFORM_MACOSX__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
  #include <pthread.h>
  #define THREAD_TYPE
  typedef pthread_t THREAD_HANDLE;
  typedef void * THREAD_RETURN;
  typedef void * (*THREAD_FUNCTION)(void *);
  typedef pthread_mutex_t MUTEX;
  #define CHUCK_THREAD pthread_t
#elif defined(__PLATFORM_WIN32__)
  #include <windows.h>
  #include <process.h>
  #define THREAD_TYPE __stdcall
  typedef unsigned long THREAD_HANDLE;
  typedef unsigned THREAD_RETURN;
  typedef unsigned (__stdcall *THREAD_FUNCTION)(void *);
  typedef CRITICAL_SECTION MUTEX;
  #define CHUCK_THREAD HANDLE
#endif




//-----------------------------------------------------------------------------
// name: struct XThread
// desc: ...
//-----------------------------------------------------------------------------
struct XThread
{
public:
    XThread();
    ~XThread();

public:
    // begin execution of the thread routine
    // the thread routine can be passed an argument via ptr
    bool start( THREAD_FUNCTION routine, void * ptr = NULL );

    // wait the specified number of milliseconds for the thread to terminate
    bool wait( long milliseconds = -1 );

public:
    // test for a thread cancellation request.
    static void test( );
    
    // clear
    void clear() { thread = 0; }

protected:
    THREAD_HANDLE thread;
};




//-----------------------------------------------------------------------------
// name: struct XMutex
// desc: ...
//-----------------------------------------------------------------------------
struct XMutex
{
public:
    XMutex();
    ~XMutex();

public:
    void acquire( );
    void release(void);

protected:
    MUTEX mutex;
};




#endif
