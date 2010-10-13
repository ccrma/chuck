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
// file: digiio_rtaudio.cpp
// desc: digitalio over rtaudio (from Gary Scavone)
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "digiio_rtaudio.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h"
#ifndef __DISABLE_RTAUDIO__
#include "rtaudio.h"
#endif // __DISABLE_RTAUDIO__
#if defined(__CHIP_MODE__)
#include "../small.h">
#endif // __CHIP_MODE__
#ifndef __DISABLE_MIDI__
#include "rtmidi.h"
#endif // __DISABLE_MIDI__
// #include <signal.h>
#if (defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)) && !defined(__WINDOWS_PTHREAD__)
#include <windows.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

// extern "C" void signal_int( int );

// static
BOOL__ Digitalio::m_init = FALSE;
DWORD__ Digitalio::m_start = 0;
DWORD__ Digitalio::m_tick_count = 0;
DWORD__ Digitalio::m_num_channels_out = NUM_CHANNELS_DEFAULT;
DWORD__ Digitalio::m_num_channels_in = NUM_CHANNELS_DEFAULT;
DWORD__ Digitalio::m_sampling_rate = SAMPLING_RATE_DEFAULT;
DWORD__ Digitalio::m_bps = BITS_PER_SAMPLE_DEFAULT;
DWORD__ Digitalio::m_buffer_size = BUFFER_SIZE_DEFAULT;
DWORD__ Digitalio::m_num_buffers = NUM_BUFFERS_DEFAULT;
RtAudio * Digitalio::m_rtaudio = NULL;
SAMPLE * Digitalio::m_buffer_out = NULL;
SAMPLE * Digitalio::m_buffer_in = NULL;
SAMPLE ** Digitalio::m_write_ptr = NULL;
SAMPLE ** Digitalio::m_read_ptr = NULL;
SAMPLE * Digitalio::m_extern_in = NULL;
SAMPLE * Digitalio::m_extern_out = NULL;
BOOL__ Digitalio::m_out_ready = FALSE;
BOOL__ Digitalio::m_in_ready = FALSE;
BOOL__ Digitalio::m_use_cb = USE_CB_DEFAULT;
DWORD__ Digitalio::m_go = 0;
DWORD__ Digitalio::m_dac_n = 0;
DWORD__ Digitalio::m_adc_n = 0;
DWORD__ Digitalio::m_end = 0;
DWORD__ Digitalio::m_block = TRUE;
DWORD__ Digitalio::m_xrun = 0;


// sample
#if defined(CK_S_DOUBLE)
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT64
#else
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT32
#endif




#ifndef __DISABLE_RTAUDIO__
//-----------------------------------------------------------------------------
// name: print()
// desc: ...
//-----------------------------------------------------------------------------
void print( const RtAudioDeviceInfo & info )
{
    EM_error2b( 0, "device name = \"%s\"", info.name.c_str() );
    if (info.probed == false)
        EM_error2b( 0, "probe [failed] ..." );
    else
    {
        EM_error2b( 0, "probe [success] ..." );
        EM_error2b( 0, "# output channels = %d", info.outputChannels );
        EM_error2b( 0, "# input channels  = %d", info.inputChannels );
        EM_error2b( 0, "# duplex Channels = %d", info.duplexChannels );
        if( info.isDefault ) EM_error2b( 0, "default device = YES" );
        else EM_error2b( 0, "default device = NO" );
        if( info.nativeFormats == 0 ) EM_error2b( 0, "no natively supported data formats(?)!" );
        else
        {
            EM_error2b( 0,  "natively supported data formats:" );
            if( info.nativeFormats & RTAUDIO_SINT8 )   EM_error2b( 0, "   8-bit int" );
            if( info.nativeFormats & RTAUDIO_SINT16 )  EM_error2b( 0, "  16-bit int" );
            if( info.nativeFormats & RTAUDIO_SINT24 )  EM_error2b( 0, "  24-bit int" );
            if( info.nativeFormats & RTAUDIO_SINT32 )  EM_error2b( 0, "  32-bit int" );
            if( info.nativeFormats & RTAUDIO_FLOAT32 ) EM_error2b( 0, "  32-bit float" );
            if( info.nativeFormats & RTAUDIO_FLOAT64 ) EM_error2b( 0, "  64-bit float" );
        }
        if ( info.sampleRates.size() < 1 ) EM_error2b( 0,"no supported sample rates found!" );
        else
        {
            EM_error2b( 0, "supported sample rates:" );
            for( unsigned int j = 0; j < info.sampleRates.size(); j++ )
                EM_error2b( 0, "  %d Hz", info.sampleRates[j] );
        }
    }
}
#endif // __DISABLE_RTAUDIO__




//-----------------------------------------------------------------------------
// name: probe()
// desc: ...
//-----------------------------------------------------------------------------
void Digitalio::probe()
{
#ifndef __DISABLE_RTAUDIO__
    RtAudio * rta = NULL;
    RtAudioDeviceInfo info;
    
    // allocate RtAudio
    try { rta = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2b( 0, "%s", err.getMessageString() );
        return;
    }

    // get count    
    int devices = rta->getDeviceCount();
    EM_error2b( 0, "found %d device(s) ...", devices );
    // EM_error2( 0, "--------------------------" );
    
    // loop
    for( int i = 1; i <= devices; i++ )
    {
        try { info = rta->getDeviceInfo(i); }
        catch( RtError & error )
        {
            error.printMessage();
            break;
        }
        
        // print
        EM_error2b( 0, "------( chuck -- dac%d )---------------", i );
        print( info );
        // skip
        if( i < devices ) EM_error2( 0, "" );
    }

    delete rta;
#endif // __DISABLE_RTAUDIO__

    return;
}




#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKBOOL set_priority( CHUCK_THREAD tid, t_CKINT priority )
{
    struct sched_param param;
    int policy;

    // log
    EM_log( CK_LOG_FINE, "setting thread priority to: %ld...", priority );

    // get for thread
    if( pthread_getschedparam( tid, &policy, &param) ) 
        return FALSE;

    // priority
    param.sched_priority = priority;
    // policy
    policy = SCHED_RR;
    // set for thread
    if( pthread_setschedparam( tid, policy, &param ) )
        return FALSE;

    return TRUE;
}
#else
//-----------------------------------------------------------------------------
// name: set_priority()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKBOOL set_priority( CHUCK_THREAD tid, t_CKINT priority )
{
    // if priority is 0 then done
    if( !priority ) return TRUE;

    // log
    EM_log( CK_LOG_FINE, "setting thread priority to: %ld...", priority );

    // set the priority the thread
    if( !SetThreadPriority( tid, priority ) )
        return FALSE;

    return TRUE;
}
#endif


//-----------------------------------------------------------------------------
// name: get_current_time()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKFLOAT get_current_time( t_CKBOOL fresh = TRUE )
{
#ifdef __PLATFORM_WIN32__
    struct _timeb t;
    _ftime(&t);
    return t.time + t.millitm/1000.0;
#else
    static struct timeval t;
    if( fresh ) gettimeofday(&t,NULL);
    return t.tv_sec + (t_CKFLOAT)t.tv_usec/1000000;
#endif
        
    return 0;
}


// watch dog globals
static CHUCK_THREAD g_tid_synthesis = 0;
static XThread * g_watchdog_thread = NULL;
static t_CKBOOL g_watchdog_state = FALSE;
static t_CKFLOAT g_watchdog_time = 0;


//-----------------------------------------------------------------------------
// name: watch_dog()
// desc: ...
//-----------------------------------------------------------------------------
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
static void * watch_dog( void * )
#else
static unsigned int __stdcall watch_dog( void * )
#endif
{
    t_CKFLOAT time;

    // boost priority?
    t_CKUINT priority = Chuck_VM::our_priority;

    // log
    EM_log( CK_LOG_SEVERE, "starting real-time watch dog processs..." );
    // push log
    EM_pushlog();
    EM_log( CK_LOG_INFO, "watchdog timeout: %f::second", g_watchdog_timeout );
    EM_log( CK_LOG_INFO, "watchdog thread priority: %d", priority );
    EM_log( CK_LOG_INFO, "watchdog countermeasure priority: %d", g_watchdog_countermeasure_priority );
    // pop log
    EM_poplog();

    // boost watchdog by same priority
    if( Chuck_VM::our_priority != 0x7fffffff )
        Chuck_VM::set_priority( priority, NULL );

    // while going
    while( g_do_watchdog )
    {
        // get
        time = get_current_time( TRUE );
        // fprintf( stderr, "last: %f now: %f\n", g_watchdog_time, time );

        // resting
        if( g_watchdog_state == FALSE )
        {
            // check xrun
            // if( Digitalio::m_xrun > 100 )
            if( time - g_watchdog_time > g_watchdog_timeout )
            {
                // log
                EM_log( CK_LOG_SEVERE, "real-time watchdog counter-measure activating..." );
                // lowering priority
                if( g_tid_synthesis && Chuck_VM::our_priority != 0x7fffffff )
                    set_priority( g_tid_synthesis, g_watchdog_countermeasure_priority );
                // set state
                g_watchdog_state = TRUE;
            }
        }
        else
        {
            // check xrun
            // if( Digitalio::m_xrun == 0 )
            if( time - g_watchdog_time < g_watchdog_timeout )
            {
                // log
                EM_log( CK_LOG_SEVERE, "real-time watchdog resting..." );
                // raise priority
                if( g_tid_synthesis && Chuck_VM::our_priority != 0x7fffffff )
                    set_priority( g_tid_synthesis, Chuck_VM::our_priority );
                // set state
                g_watchdog_state = FALSE;
            }
        }
        
        // advance time
        usleep( 40000 );
    }
    
    // log
    EM_log( CK_LOG_SEVERE, "stopping real-time watch dog process..." );

    return 0;
}




//-----------------------------------------------------------------------------
// name: watchdog_start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::watchdog_start()
{
    // already
    if( g_watchdog_thread )
        return FALSE;

    // flag
    g_do_watchdog = TRUE;
    // allocate it
    g_watchdog_thread = new XThread;
    // start it
    g_watchdog_thread->start( watch_dog, NULL );
    
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: watchdog_stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::watchdog_stop()
{
    // already
    if( !g_watchdog_thread )
        return FALSE;

    // stop things
    g_do_watchdog = FALSE;
    // wait a bit
    // usleep( 100000 )
    SAFE_DELETE( g_watchdog_thread );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::initialize( DWORD__ num_dac_channels,
                              DWORD__ num_adc_channels,
                              DWORD__ sampling_rate,
                              DWORD__ bps, DWORD__ buffer_size, 
                              DWORD__ num_buffers, DWORD__ block,
                              Chuck_VM * vm_ref, BOOL__ rt_audio,
                              void * callback, void * data )
{
    if( m_init )
        return FALSE;

    m_num_channels_out = num_dac_channels;
    m_num_channels_in = num_adc_channels;
    m_sampling_rate = sampling_rate;
    m_bps = bps;
    m_buffer_size = buffer_size;
    m_num_buffers = num_buffers;
    m_start = 0;
    m_tick_count = 0;
    m_go = 0;
    m_end = 0;
    m_block = block;

    DWORD__ num_channels;
    int bufsize = m_buffer_size;

    // if rt_audio is false, then set block to FALSE to avoid deadlock
    if( !rt_audio ) m_block = FALSE;

#ifndef __DISABLE_RTAUDIO__    
    // if real-time audio
    if( rt_audio )
    {
        // allocate RtAudio
        try { m_rtaudio = new RtAudio( ); }
        catch( RtError err )
        {
            // problem finding audio devices, most likely
            EM_error2( 0, "%s", err.getMessageString() );
            return m_init = FALSE;
        }

        // log
        EM_log( CK_LOG_FINE, "initializing RtAudio..." );
        // push indent
        EM_pushlog();

        // open device
        try {
            // log
            EM_log( CK_LOG_FINE, "trying %d input %d output...", 
                    m_num_channels_in, m_num_channels_out );
            // open RtAudio
            m_rtaudio->openStream(
                m_dac_n, m_num_channels_out, m_adc_n, m_num_channels_in,
                CK_RTAUDIO_FORMAT, sampling_rate,
                &bufsize, num_buffers );
            // set callback
            if( m_use_cb )
            {
                // log
                EM_log( CK_LOG_INFO, "initializing callback..." );
                if( !callback )
                {
                    if( block ) m_rtaudio->setStreamCallback( &cb, vm_ref );
                    else m_rtaudio->setStreamCallback( &cb2, vm_ref );
                }
                else
                {
                    m_rtaudio->setStreamCallback( (RtAudioCallback)callback, data );
                }
            }
        } catch( RtError err ) {
            // log
            EM_log( CK_LOG_INFO, "exception caught: '%s'...", err.getMessageString() );
            EM_log( CK_LOG_INFO, "trying %d input %d output...", 0, m_num_channels_out );
            try {
                bufsize = buffer_size;
                // try output only
                m_rtaudio->openStream(
                    m_dac_n, m_num_channels_out, 0, 0,
                    RTAUDIO_FLOAT32, sampling_rate,
                    &bufsize, num_buffers );
                // set callback
                if( m_use_cb )
                {
                    // log
                    EM_log( CK_LOG_INFO, "initializing callback (again)..." );
                    if( !callback )
                    {
                        if( block ) m_rtaudio->setStreamCallback( &cb, vm_ref );
                        else m_rtaudio->setStreamCallback( &cb2, vm_ref );
                    }
                    else
                    {
                        m_rtaudio->setStreamCallback( (RtAudioCallback)callback, data );
                    }
                }
            } catch( RtError err ) {
                EM_error2( 0, "%s", err.getMessageString() );
                SAFE_DELETE( m_rtaudio );
                return m_init = FALSE;
            }
        }
        
        // check bufsize
        if( bufsize != (int)m_buffer_size )
        {
            EM_log( CK_LOG_SEVERE, "new buffer size: %d -> %i", m_buffer_size, bufsize );
            m_buffer_size = bufsize;
        }

        // pop indent
        EM_poplog();
    }
#endif // __DISABLE_RTAUDIO__

#if defined(__CHIP_MODE__)
    if( !SMALL::init( sampling_rate, buffer_size, 2 ) )
    {
        EM_error2( 0, "%s", "(chuck)error: unable to initialize SMALL..." );
        return m_init = FALSE;
    }
#endif // __CHIP_MODE__

    if( m_use_cb )
    {
        num_channels = num_dac_channels > num_adc_channels ? 
                       num_dac_channels : num_adc_channels;
        // log
        EM_log( CK_LOG_SEVERE, "allocating buffers for %d x %d samples...",
                m_buffer_size, num_channels );
        // allocate buffers
        m_buffer_in = new SAMPLE[m_buffer_size * num_channels];
        m_buffer_out = new SAMPLE[m_buffer_size * num_channels];
        memset( m_buffer_in, 0, m_buffer_size * sizeof(SAMPLE) * num_channels );
        memset( m_buffer_out, 0, m_buffer_size * sizeof(SAMPLE) * num_channels );
        m_read_ptr = NULL;
        m_write_ptr = NULL;
    }
    
    m_in_ready = FALSE;
    m_out_ready = FALSE;

    return m_init = TRUE;
}




//-----------------------------------------------------------------------------
// name: cb()
// desc: ...
//-----------------------------------------------------------------------------
int Digitalio::cb( char * buffer, int buffer_size, void * user_data )
{
    DWORD__ len = buffer_size * sizeof(SAMPLE) * m_num_channels_out;
    DWORD__ n = 20;
    DWORD__ start = 50;

    // copy input to local buffer
    if( m_num_channels_in )
    {
        memcpy( m_buffer_in, buffer, len );
        // copy to extern
        if( m_extern_in ) memcpy( m_extern_in, buffer, len );
    }
    // flag ready
    m_in_ready = TRUE;
    // out is ready early
    if( m_go < start && m_go > 1 && m_out_ready ) m_go = start;
    // copy output into local buffer
    if( m_go >= start )
    {
        while( !m_out_ready && n-- ) usleep( 250 );
        if( m_out_ready && g_do_watchdog )
            g_watchdog_time = get_current_time( TRUE );
        // copy local buffer to be rendered
        if( m_out_ready && !m_end ) memcpy( buffer, m_buffer_out, len );
        // set all elements of local buffer to silence
        else memset( buffer, 0, len );
    }
    else  // initial condition
    {
        // priority boost
        if( !m_go && Chuck_VM::our_priority != 0x7fffffff )
            Chuck_VM::set_priority( Chuck_VM::our_priority, NULL );
        // catch SIGINT
        // signal( SIGINT, signal_int );

        // timestamp
        if( g_do_watchdog ) g_watchdog_time = get_current_time( TRUE );

        memset( buffer, 0, len );
        m_go++;
        return 0;
    }

    // 2nd buffer
    if( m_go == start )
    {
        n = 8; while( !m_out_ready && n-- ) usleep( 250 );
        len /= sizeof(SAMPLE); DWORD__ i = 0;
        SAMPLE * s = (SAMPLE *)buffer;
        while( i < len ) *s++ *= (SAMPLE)i++/len;
        m_go++;
    }

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, buffer, len );

    // set pointer to the beginning - if not ready, then too late anyway
    //*m_write_ptr = (SAMPLE *)m_buffer_out;
    //*m_read_ptr = (SAMPLE *)m_buffer_in;
    m_out_ready = FALSE;

    return 0;
}




//-----------------------------------------------------------------------------
// name: cb2()
// desc: ...
//-----------------------------------------------------------------------------
int Digitalio::cb2( char * buffer, int buffer_size, void * user_data )
{
    DWORD__ len = buffer_size * sizeof(SAMPLE) * m_num_channels_out;
    Chuck_VM * vm_ref = (Chuck_VM *)user_data;
    
    // priority boost
    if( !m_go && Chuck_VM::our_priority != 0x7fffffff )
    {
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
        g_tid_synthesis = pthread_self();
#else
        // must duplicate for handle to be usable by other threads
        g_tid_synthesis = NULL;
        DuplicateHandle(
            GetCurrentProcess(),
            GetCurrentThread(),
            GetCurrentProcess(),
            &g_tid_synthesis,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        );

        // TODO: close the duplicate handle?
#endif
        Chuck_VM::set_priority( Chuck_VM::our_priority, NULL );
        memset( buffer, 0, len );
        m_go = TRUE;

        // start watchdog
        if( g_do_watchdog )
        {
            // timestamp
            g_watchdog_time = get_current_time( TRUE );
            // start watchdog
            watchdog_start();
        }

        // let it go the first time
        return 0;
    }

    // copy input to local buffer
    if( m_num_channels_in )
    {
        memcpy( m_buffer_in, buffer, len );
        // copy to extern
        if( m_extern_in ) memcpy( m_extern_in, buffer, len );
    }

    // check xrun
    if( m_xrun < 6 )
    {
        // timestamp
        if( g_do_watchdog ) g_watchdog_time = get_current_time( TRUE );
        // get samples from output
        vm_ref->run( buffer_size );
        // ...
        if( m_xrun ) m_xrun--;
    }
    else
    {
        // reset
        m_xrun /= 2;
    }

    // copy local buffer to be rendered
    if( !m_end ) memcpy( buffer, m_buffer_out, len );
    // set all elements of local buffer to silence
    else memset( buffer, 0, len );

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, buffer, len );


    return 0;
}




#ifdef __SMALL_MODE__
//-----------------------------------------------------------------------------
// name: small_cb()
// desc: ...
//-----------------------------------------------------------------------------
void small_cb( Float32 * buffer, UInt32 numFrames, void * userData )
{
    Digitalio::cb2( (char *)buffer, numFrames, userData );
}
#endif // __SMALL_MODE__




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::start( )
{
#ifndef __DISABLE_RTAUDIO__
    try{ if( !m_start )
              m_rtaudio->startStream();
         m_start = TRUE;
    } catch( RtError err ){ return FALSE; }
#endif // __DISABLE_RTAUDIO__

#if defined(__CHIP_MODE__)
    if( !m_start )
        m_start = SMALL::start( small_cb, g_vm );
#endif // __CHIP_MODE__

    return m_start;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::stop( )
{
#ifndef __DISABLE_RTAUDIO__
    try{ if( m_start )
             m_rtaudio->stopStream();
         m_start = FALSE;
    } catch( RtError err ){ return FALSE; }
#endif // __DISABLE_RTAUDIO__

#if defined(__CHIP_MODE__)
    if( m_start )
        SMALL::stop();
    m_start = FALSE;
#endif

    return !m_start;
}




//-----------------------------------------------------------------------------
// name: tick()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::tick( )
{
#ifndef __DISABLE_RTAUDIO__
    try
    {
        if( ++m_tick_count >= m_start )
        {
            m_rtaudio->tickStream();
            m_tick_count = 0;
            m_out_ready = TRUE;
            m_in_ready = TRUE;
        }
        
        return TRUE;
    } catch( RtError err ){ return FALSE; }
#endif // __DISABLE_RTAUDIO__
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void Digitalio::shutdown()
{
    if( !m_init ) return;

#ifndef __DISABLE_RTAUDIO__
    if( m_start )
    {
        if( m_use_cb ) m_rtaudio->cancelStreamCallback();
        m_rtaudio->stopStream();
    }

    m_rtaudio->closeStream();
    SAFE_DELETE( m_rtaudio );
#endif // __DISABLE_RTAUDIO__

    m_init = FALSE;
    m_start = FALSE;
    
    // stop watchdog
    watchdog_stop();
}




//-----------------------------------------------------------------------------
// name: DigitalOut()
// desc: ...
//-----------------------------------------------------------------------------
DigitalOut::DigitalOut()
{
    m_data_ptr_out = NULL;
    m_data_max_out = NULL;
}




//-----------------------------------------------------------------------------
// name: ~DigitalOut()
// desc: ...
//-----------------------------------------------------------------------------
DigitalOut::~DigitalOut()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize audio out
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::initialize( )
{
#ifndef __DISABLE_RTAUDIO__
    // set pointer to beginning of local buffer
    m_data_ptr_out = Digitalio::m_use_cb ? Digitalio::m_buffer_out
        : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
#else
    // set pointer
    assert( Digitalio::m_use_cb );
    m_data_ptr_out = Digitalio::m_buffer_out;
#endif // __DISABLE_RTAUDIO__
    // calculate the end of the buffer
    m_data_max_out = m_data_ptr_out + 
        Digitalio::buffer_size() * Digitalio::num_channels_out();
    // set the writer pointer to our write pointer
    Digitalio::m_write_ptr = &m_data_ptr_out;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::start()
{
    // start stream
    return ( Digitalio::start() != 0 );
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::stop()
{
    // well
    Digitalio::stop();
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalOut::cleanup()
{
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: 1 channel
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::tick_out( SAMPLE sample )
{    
    if( !prepare_tick_out() )
        return FALSE;

    *m_data_ptr_out++ = sample;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: 2 channel
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::tick_out( SAMPLE sample_l, SAMPLE sample_r )
{
    if( !prepare_tick_out() )
        return FALSE;

    *m_data_ptr_out++ = sample_l;
    *m_data_ptr_out++ = sample_r;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: all channels
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::tick_out( const SAMPLE * samples, DWORD__ n )
{
    if( !prepare_tick_out() )
        return FALSE;

    if( !n ) n = Digitalio::m_num_channels_out;
    while( n-- )
        *m_data_ptr_out++ = *samples++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: prepare_tick_out()
// desc: data ptr ok
//-----------------------------------------------------------------------------
inline BOOL__ DigitalOut::prepare_tick_out()
{
    if( m_data_ptr_out >= m_data_max_out )
    {
        this->render();
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render()
// desc: the render
//-----------------------------------------------------------------------------
DWORD__ DigitalOut::render()
{
    //if( !Digitalio::m_use_cb && !Digitalio::tick() ) return FALSE;

    if( Digitalio::m_block )
    {
        // synchronize
        Digitalio::m_out_ready = TRUE;
        // synchronize
        while( Digitalio::m_out_ready )
            usleep( 250 );
    }

    // set pointer to the beginning - if not ready, then too late anyway
    *Digitalio::m_write_ptr = (SAMPLE *)Digitalio::m_buffer_out;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: DigitalIn()
// desc: ...
//-----------------------------------------------------------------------------
DigitalIn::DigitalIn()
{
    m_data_ptr_in = NULL;
    m_data_max_in = NULL;
}




//-----------------------------------------------------------------------------
// name: ~DigitalIn()
// desc: ...
//-----------------------------------------------------------------------------
DigitalIn::~DigitalIn()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize audio in
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::initialize( )
{
    m_data = new SAMPLE[Digitalio::buffer_size() * Digitalio::num_channels_in()];
    memset( m_data, 0, Digitalio::buffer_size() * Digitalio::num_channels_in() * sizeof(SAMPLE) );
#ifndef __DISABLE_RTAUDIO__
    // set the buffer to the beginning of the local buffer
    m_data_ptr_in = Digitalio::m_use_cb ? m_data
        : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
#else
    // set the buffer pointer
    assert( Digitalio::m_use_cb );
    m_data_ptr_in = m_data;
#endif // __DISABLE_RTAUDIO__
    // calculate past buffer
    m_data_max_in = m_data_ptr_in + 
        Digitalio::buffer_size() * Digitalio::num_channels_in();
    // set the read pointer to the local pointer
    Digitalio::m_read_ptr = &m_data_ptr_in;
    // invalidate
    m_data_ptr_in = m_data_max_in;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::start()
{
    return ( Digitalio::start() != 0 );
}




//-----------------------------------------------------------------------------
// name: capture_stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::stop()
{
    Digitalio::stop();
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalIn::cleanup()
{
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::tick_in( SAMPLE * s )
{
    if( !prepare_tick_in() )
        return 0;
    *s = *m_data_ptr_in++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::tick_in( SAMPLE * l, SAMPLE * r )
{
    if( !prepare_tick_in() )
        return FALSE;

    *l = *m_data_ptr_in++;
    *r = *m_data_ptr_in++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::tick_in( SAMPLE * sample, DWORD__ n )
{
    if( !prepare_tick_in() )
        return FALSE;

    if( !n ) n = Digitalio::m_num_channels_in;
    while( n-- )
        *sample++ = *m_data_ptr_in++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: prepare_tick_in()
// desc: data ptr ok
//-----------------------------------------------------------------------------
inline BOOL__ DigitalIn::prepare_tick_in()
{
    if( m_data_ptr_in >= m_data_max_in )
    {
        this->capture();
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cb_capture()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ DigitalIn::capture( )
{
    // if( !Digitalio::m_use_cb && !Digitalio::tick() ) return FALSE;

    if( Digitalio::m_block )
    {
        t_CKUINT n = 20;
        while( !Digitalio::m_in_ready && n-- )
            usleep( 250 );
    }

    // copy data
    memcpy( m_data, Digitalio::m_buffer_in, Digitalio::buffer_size() *
            Digitalio::num_channels_in() * sizeof(SAMPLE) );
    Digitalio::m_in_ready = FALSE;
    // set pointer to the beginning - if not ready, then too late anyway
    *Digitalio::m_read_ptr = (SAMPLE *)m_data;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudioBufferX()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferX::AudioBufferX( DWORD__ size )
{
    if( size )
        this->initialize( size );
    else
    {
        m_size = size;
        m_data = m_ptr_curr = m_ptr_end = NULL;
    }
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferX::initialize( DWORD__ size )
{
    m_size = size;
    m_data = (SAMPLE *)malloc( size * sizeof(SAMPLE) );

    if( !m_data )
        return FALSE;

    // clear the memory
    memset( m_data, 0, size * sizeof(SAMPLE) );

    // set the pointers
    m_ptr_curr = m_data;
    m_ptr_end = m_data + size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void AudioBufferX::cleanup()
{
    if( m_data )
    {
        free( m_data );
        m_data = NULL;
    }

    m_size = 0;
    m_ptr_curr = m_ptr_end = NULL;
}




//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ AudioBufferX::size()
{
    return m_size;
}




//-----------------------------------------------------------------------------
// name: data()
// desc: ...
//-----------------------------------------------------------------------------
SAMPLE * AudioBufferX::data()
{
    return m_data;
}




//-----------------------------------------------------------------------------
// name: AudioBufferIn()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferIn::AudioBufferIn( DWORD__ size )
    : AudioBufferX( size )
{ }




//-----------------------------------------------------------------------------
// name: ~AudioBufferIn()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferIn::~AudioBufferIn()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferIn::reset()
{
    m_ptr_curr = m_data;

    return TickIn::reset();
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferIn::tick_in( SAMPLE * s )
{
    if( !m_data )
        return FALSE;

    *s = *m_ptr_curr++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferIn::tick_in( SAMPLE * l, SAMPLE * r )
{
    if( !m_data || m_ptr_curr + 2 >= m_ptr_end )
        return FALSE;

    *l = *m_ptr_curr++;
    *r = *m_ptr_curr++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferIn::tick_in( SAMPLE * in, DWORD__ n )
{
    if( !m_data || m_ptr_curr + n >= m_ptr_end )
        return FALSE;

    while( n-- )
        *in++ = *m_ptr_curr++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudioBufferOut()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferOut::AudioBufferOut( DWORD__ size )
    : AudioBufferX( size )
{ }




//-----------------------------------------------------------------------------
// name: ~AudioBufferOut()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferOut::~AudioBufferOut()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferOut::reset()
{
    m_ptr_curr = m_data;

    return TickOut::reset();
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferOut::tick_out( SAMPLE s )
{
    if( !m_data )
        return FALSE;

    *m_ptr_curr++ = s;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferOut::tick_out( SAMPLE l, SAMPLE r )
{
    if( !m_data || m_ptr_curr + 2 >= m_ptr_end )
        return FALSE;

    *m_ptr_curr++ = l;
    *m_ptr_curr++ = r;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBufferOut::tick_out( const SAMPLE * out, DWORD__ n )
{
    if( !m_data || m_ptr_curr + n >= m_ptr_end )
        return FALSE;

    while( n-- )
        *m_ptr_curr++ = *out++;

    return TRUE;
}
