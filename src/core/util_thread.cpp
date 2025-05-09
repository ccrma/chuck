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
// name: util_thread.cpp
// desc: thread utilities
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on STK's Thread
//          Perry R. Cook + Gary Scavone
// date: autumn 2004
//-----------------------------------------------------------------------------
#include "util_thread.h"
#include "util_buffers.h"
#include "util_platforms.h"
#include "chuck_errmsg.h"




// static instantiation
const size_t XWriteThread::PRODUCER_BUFFER_SIZE = 1024;
XWriteThread * XWriteThread::o_defaultWriteThread = NULL;





//-----------------------------------------------------------------------------
// name: XThread()
// desc: ...
//-----------------------------------------------------------------------------
XThread::XThread( )
{
    thread = 0;
}




//-----------------------------------------------------------------------------
// name: ~XThread()
// desc: ...
//-----------------------------------------------------------------------------
XThread::~XThread( )
{
#if defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__)
    // can't self-terminate (much like the Terminator)
    bool is_self_thread = (thread == pthread_self());
#elif defined(__PLATFORM_WINDOWS__)
    // ignore for now
    bool is_self_thread = false;
#endif

    // if our thread not NULL and this destructor is NOT being called on it
    if( thread != 0 && !is_self_thread )
    {
        // TODO: find an alternative for Android?
#if defined(__PLATFORM_APPLE__) || ( defined(__PLATFORM_LINUX__) && !defined(__ANDROID__) ) || defined(__WINDOWS_PTHREAD__)
        // log
        EM_log( CK_LOG_FINER, "cancelling thread [0x%x] from [0x%x]...", getID(thread), getID(pthread_self()) );
        pthread_cancel(thread);
        pthread_join(thread, NULL);
        // log
        EM_log( CK_LOG_FINER, "joined with thread [0x%x] from [0x%x]", getID(thread), getID(pthread_self()) );
#elif defined(__PLATFORM_WINDOWS__)
        // log
        EM_log( CK_LOG_FINER, "terminating thread [0x%x] from [0x%x]...", getID(thread), (t_CKUINT)GetCurrentThreadId() );
        TerminateThread( (HANDLE)thread, 0 );
#endif
    }
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
bool XThread::start( THREAD_FUNCTION routine, void * ptr )
{
    bool result = false;

#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    if( pthread_create( &thread, NULL, *routine, ptr ) == 0 )
    {
        EM_log( CK_LOG_FINER, "starting thread [0x%x] from [0x%x]...", thread, getID(pthread_self()) );
        result = true;
    }
#elif defined(__PLATFORM_WINDOWS__)
    unsigned thread_id;
    thread = _beginthreadex( NULL, 0, routine, ptr, 0, &thread_id );
    if( thread )
    {
        EM_log( CK_LOG_FINER, "starting thread [0x%x] from [0x%x]...", thread_id, (t_CKUINT)GetCurrentThreadId() );
        result = true;
    }
#endif
    return result;
}




//-----------------------------------------------------------------------------
// name: wait()
// desc: ...
//-----------------------------------------------------------------------------
bool XThread::wait( long milliseconds, bool cancel )
{
    bool result = false;

    // TODO: find an alternative for Android?
#if( defined(__PLATFORM_APPLE__) || ( defined(__PLATFORM_LINUX__) && !defined(__ANDROID__) ) || defined(__WINDOWS_PTHREAD__) )
    // cancel the thread?
    if( cancel ) pthread_cancel( thread );
    // wait for the thread to terminate
    pthread_join( thread, NULL );
#elif defined(__PLATFORM_WINDOWS__)
    DWORD timeout, retval;
    if( milliseconds < 0 ) timeout = INFINITE;
    else timeout = milliseconds;
    retval = WaitForSingleObject( (HANDLE)thread, timeout );
    if( retval == WAIT_OBJECT_0 ) {
        CloseHandle( (HANDLE)thread );
        thread = 0;
        result = true;
    }
#endif

  return result;
}




//-----------------------------------------------------------------------------
// name: test()
// desc: ...
//-----------------------------------------------------------------------------
void XThread :: test( )
{
    // TODO: find an alternative for Android?
#if ( defined(__PLATFORM_APPLE__) || ( defined(__PLATFORM_LINUX__) && !defined(__ANDROID__) ) || defined(__WINDOWS_PTHREAD__) )
    pthread_testcancel();
#endif
}




//-----------------------------------------------------------------------------
// name: getID() | 1.5.0.4 (ge) added
// desc: get thread id from a thread handle
//-----------------------------------------------------------------------------
t_CKUINT XThread::getID( THREAD_HANDLE t )
{
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    return (t_CKUINT)t;
#elif defined(__PLATFORM_WINDOWS__)
    return (t_CKUINT)GetThreadId( (HANDLE)t );
#endif
}




//-----------------------------------------------------------------------------
// name: XMutex()
// desc: ...
//-----------------------------------------------------------------------------
XMutex::XMutex( )
{
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    pthread_mutex_init(&mutex, NULL);
#elif defined(__PLATFORM_WINDOWS__)
    InitializeCriticalSection(&mutex);
#endif
}




//-----------------------------------------------------------------------------
// name: XMutex()
// desc: ...
//-----------------------------------------------------------------------------
XMutex::~XMutex( )
{
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    pthread_mutex_destroy( &mutex );
#elif defined(__PLATFORM_WINDOWS__)
    DeleteCriticalSection(&mutex);
#endif
}




//-----------------------------------------------------------------------------
// name: acquire()
// desc: ...
//-----------------------------------------------------------------------------
void XMutex::acquire( )
{
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    pthread_mutex_lock(&mutex);
#elif defined(__PLATFORM_WINDOWS__)
    EnterCriticalSection(&mutex);
#endif
}




//-----------------------------------------------------------------------------
// name: unlock()
// desc: ...
//-----------------------------------------------------------------------------
void XMutex::release( )
{
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
    pthread_mutex_unlock(&mutex);
#elif defined(__PLATFORM_WINDOWS__)
    LeaveCriticalSection(&mutex);
#endif
}




//-----------------------------------------------------------------------------
// name: shared()
// desc: get XWriteThread shared instance
//-----------------------------------------------------------------------------
XWriteThread * XWriteThread::shared()
{
    // static XWriteThread s_defaultWriteThread;

    // check
    if( o_defaultWriteThread == NULL )
        o_defaultWriteThread = new XWriteThread();

    return o_defaultWriteThread;
}




//-----------------------------------------------------------------------------
// name: XWriteThread()
// desc: constructor
//-----------------------------------------------------------------------------
XWriteThread::XWriteThread(size_t data_buffer_size, size_t msg_buffer_size)
    :
    m_msg_buffer(new CircularBuffer<Message>(msg_buffer_size)),
    m_data_buffer(new FastCircularBuffer),
    m_thread_buffer(new t_CKBYTE[data_buffer_size])
{
    m_data_buffer->initialize( data_buffer_size, sizeof(char) );
    m_thread_exit = FALSE;
    m_thread.start( write_cb, this );
    m_stream = NULL;
    m_bytes_in_buffer = 0;
}




//-----------------------------------------------------------------------------
// name: ~XWriteThread()
// desc: destructor
//-----------------------------------------------------------------------------
XWriteThread::~XWriteThread()
{
    CK_SAFE_DELETE( m_msg_buffer );
    CK_SAFE_DELETE( m_data_buffer );
    CK_SAFE_DELETE_ARRAY( m_thread_buffer );
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: close down writer thread and delete in background
//-----------------------------------------------------------------------------
void XWriteThread::shutdown()
{
    // put shutdown message into message buffer
    Message msg;
    msg.operation = Message::SHUTDOWN;
    m_msg_buffer->put( msg );

    // smart log push
    SmartPushLog logPush;

    // log
    EM_log( CK_LOG_FINER, "waiting on write thread [0x%x]...", m_thread.getID() );
    // wait on the thread
    m_thread.wait( -1, false );
    // log
    EM_log( CK_LOG_FINER, "done waiting for thread [0x%x]", m_thread.getID() );
}




//-----------------------------------------------------------------------------
// name: fwrite()
// desc: ...
//-----------------------------------------------------------------------------
size_t XWriteThread::fwrite(const void * ptr, size_t size, size_t nitems, FILE * stream)
{
    if(stream != m_stream)
        flush_data_buffer();

    // TODO: overflow detection
    if( m_data_buffer->put((char*)ptr, size*nitems) == 0 )
    {
        EM_log( CK_LOG_HERALD, "XWriteThread::fwrite(): data buffer overflow!" );
    }

    m_bytes_in_buffer += size*nitems;
    m_stream = stream;

    if(m_bytes_in_buffer >= PRODUCER_BUFFER_SIZE)
        flush_data_buffer();

    return nitems;
}




//-----------------------------------------------------------------------------
// name: fseek()
// desc: ...
//-----------------------------------------------------------------------------
int XWriteThread::fseek(FILE *stream, long offset, int whence)
{
    flush_data_buffer();

    Message msg;
    msg.file = stream;
    msg.operation = Message::SEEK;
    msg.seek.offset = offset;
    msg.seek.whence = whence;
    m_msg_buffer->put(msg);

    return 0;
}




//-----------------------------------------------------------------------------
// name: fflush()
// desc: ...
//-----------------------------------------------------------------------------
int XWriteThread::fflush(FILE *stream)
{
    flush_data_buffer();

    Message msg;
    msg.file = stream;
    msg.operation = Message::FLUSH;
    m_msg_buffer->put(msg);

    return 0;
}




//-----------------------------------------------------------------------------
// name: fclose()
// desc: ...
//-----------------------------------------------------------------------------
int XWriteThread::fclose(FILE *stream)
{
    flush_data_buffer();

    Message msg;
    msg.file = stream;
    msg.operation = Message::CLOSE;
    m_msg_buffer->put(msg);

    return 0;
}




//-----------------------------------------------------------------------------
// name: flush_data_buffer()
// desc: ...
//-----------------------------------------------------------------------------
void XWriteThread::flush_data_buffer()
{
    if(m_bytes_in_buffer > 0)
    {
        Message msg;
        msg.file = m_stream;
        msg.operation = Message::WRITE;
        msg.write.data_size = m_bytes_in_buffer;
        m_msg_buffer->put(msg);

        m_bytes_in_buffer = 0;
    }
}




//-----------------------------------------------------------------------------
// name: write_cb()
// desc: thread function
//-----------------------------------------------------------------------------
#if ( defined(__PLATFORM_APPLE__) || defined(__PLATFORM_LINUX__) || defined(__WINDOWS_PTHREAD__) )
void * XWriteThread::write_cb(void * _thiss)
#elif defined(__PLATFORM_WINDOWS__)
unsigned XWriteThread::write_cb(void * _thiss)
#endif
{
    XWriteThread * _this = (XWriteThread *) _thiss;
    Message msg;

    while(!_this->m_thread_exit)
    {
        while(_this->m_msg_buffer->get(msg))
        {
            if(msg.operation == Message::WRITE)
            {
                size_t actual_size = _this->m_data_buffer->get(_this->m_thread_buffer,
                                                               msg.write.data_size);
                if(actual_size != msg.write.data_size)
                {
                    EM_log( CK_LOG_HERALD, "XWriteThread: buffered data size mismatch (%li : %li)",
                            msg.write.data_size, actual_size );
                }

                ::fwrite(_this->m_thread_buffer, 1, actual_size, msg.file);
            }
            else if(msg.operation == Message::SEEK)
            {
                ::fseek(msg.file, msg.seek.offset, msg.seek.whence);
            }
            else if(msg.operation == Message::FLUSH)
            {
                ::fflush(msg.file);
            }
            else if(msg.operation == Message::CLOSE)
            {
                ::fclose(msg.file);
            }
            else if(msg.operation == Message::SHUTDOWN)
            {
                _this->m_thread_exit = TRUE;
                break;
            }
        }

        ck_usleep(1000);
    }

    delete _this;
    _thiss = _this = NULL;

    return 0;
}




#ifdef __PLATFORM_APPLE__
t_CKINT XThreadUtil::our_priority = 85;
#else
t_CKINT XThreadUtil::our_priority = 0x7fffffff;
#endif




#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: set thread priority
//-----------------------------------------------------------------------------
t_CKBOOL XThreadUtil::set_priority( CHUCK_THREAD tid, t_CKINT priority )
{
    struct sched_param param;
    int policy;

    // log
    EM_log( CK_LOG_INFO, "setting thread [0x%x] priority to: %ld...", (t_CKUINT)tid, priority );

    // get for thread
    if( pthread_getschedparam( tid, &policy, &param) )
        return FALSE;

    // pthread_attr_t attr;
    // pthread_attr_init( &attr );
    // pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );

    // policy
    policy = SCHED_RR;
    // priority
    param.sched_priority = (int)priority;
    // set for thread, pthread style
    if( pthread_setschedparam( tid, policy, &param ) )
        return FALSE;

    // get for thread
    if( pthread_getschedparam( tid, &policy, &param) )
        return FALSE;

    // push
    EM_pushlog();
    // log
    EM_log( CK_LOG_FINE, "verifying thread priority: %ld...", param.sched_priority );
    // pop
    EM_poplog();

    return TRUE;
}
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: set thread priority for self
//-----------------------------------------------------------------------------
t_CKBOOL XThreadUtil::set_priority( t_CKINT priority )
{
    // pass it along with self thread
    set_priority( pthread_self(), priority );
    return TRUE;
}
t_CKINT XThreadUtil::get_max_priority()
{
    return sched_get_priority_max( SCHED_RR );
}
t_CKINT XThreadUtil::get_min_priority()
{
    return sched_get_priority_min( SCHED_RR );
}
#else
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: set thread priority
//-----------------------------------------------------------------------------
t_CKBOOL XThreadUtil::set_priority( CHUCK_THREAD tid, t_CKINT priority )
{
    // if priority is 0 then done
    if( !priority ) return TRUE;

    // log
    EM_log( CK_LOG_FINE, "setting thread [0x%x] priority to: %ld...", (t_CKUINT)GetThreadId(tid), priority );

    // set the priority the thread, windows style
    if( !SetThreadPriority( tid, priority ) )
        return FALSE;

    return TRUE;
}
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: set thread priority for self
//-----------------------------------------------------------------------------
t_CKBOOL XThreadUtil::set_priority( t_CKINT priority )
{
    // pass it along with self thread
    set_priority( GetCurrentThread(), priority );
    return TRUE;
}
t_CKINT XThreadUtil::get_max_priority()
{
    return THREAD_PRIORITY_HIGHEST; // THREAD_PRIORITY_TIME_CRITICAL;
}
t_CKINT XThreadUtil::get_min_priority()
{
    return THREAD_PRIORITY_LOWEST;
}
#endif

