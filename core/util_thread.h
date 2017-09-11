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
// name: util_thread.h
// desc: thread utilities
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on STK's Thread
//          Perry R. Cook + Gary Scavone
// date: autumn 2004
//-----------------------------------------------------------------------------
#ifndef __UTIL_THREAD_H__
#define __UTIL_THREAD_H__

#include "chuck_def.h"
#include <stdio.h>


// forward declaration to break circular dependencies
class FastCircularBuffer;
template<typename T> class CircularBuffer;


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
    bool wait( long milliseconds = -1, bool cancel = true );

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




//-----------------------------------------------------------------------------
// name: XWriteThread()
// desc: utility class for scheduling writes to be executed on a separate
//       thread. 
//-----------------------------------------------------------------------------
class XWriteThread
{
public:
	// get the shared instance
    static XWriteThread * shared();
    // static buffer size
    const static size_t PRODUCER_BUFFER_SIZE;

public:
	// constructor
    XWriteThread( size_t data_buffer_size = 4096, size_t msg_buffer_size = 32 );
    
public:
    size_t fwrite( const void * ptr, size_t size, size_t nitems, FILE * stream );
    int fseek( FILE * stream, long offset, int whence );
    int fflush( FILE * stream );
    int fclose( FILE * stream );
    
    // DO NOT DELETE INSTANCES OF XWriteThread
    // instead call shutdown and they will be cleaned up in the background
    void shutdown();

private:    
    // DO NOT DELETE INSTANCES OF XWriteThread
    // instead call shutdown and they will be cleaned up in the background
    ~XWriteThread();

	// flush
    void flush_data_buffer();

	// callback
#if ( defined(__PLATFORM_MACOSX__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    static void * write_cb( void * _thiss );
#elif defined(__PLATFORM_WIN32__)
    static unsigned THREAD_TYPE write_cb( void * _thiss );
#endif

	// shared instance
	static XWriteThread * o_defaultWriteThread;

private:
    t_CKBOOL m_thread_exit;
    XThread m_thread;
    FastCircularBuffer * m_data_buffer;
    size_t m_bytes_in_buffer;
    t_CKBYTE * m_thread_buffer;
    FILE * m_stream;

	// internal message format
    struct Message
    {
		// operation type
        enum
        {
            WRITE,
            SEEK,
            FLUSH,
            CLOSE,
            SHUTDOWN
        } operation;
        
		// file pointer
        FILE * file;
        
		// size / offset
        union
        {
            struct
            {
                size_t data_size;
            } write;
            
            struct
            {
                long offset;
                int whence;
            } seek;
        };
    };

	// circular buffer
    CircularBuffer<Message> * m_msg_buffer;
};




#endif
