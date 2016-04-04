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
// name: digiio_rtaudio.cpp
// desc: digitalio over RtAudio (from Gary Scavone)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// RtAudio by: Gary Scavone
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "digiio_rtaudio.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h"
#include <limits.h>
#ifndef __DISABLE_RTAUDIO__
#include "RtAudio/RtAudio.h"
#endif // __DISABLE_RTAUDIO__
#if defined(__CHIP_MODE__)
#include "momu/mo_audio.h"
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
#if defined(__CHUCK_USE_64_BIT_SAMPLE__)
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT64
#else
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT32
#endif




#ifndef __DISABLE_RTAUDIO__
//-----------------------------------------------------------------------------
// name: print()
// desc: ...
//-----------------------------------------------------------------------------
void print( const RtAudio::DeviceInfo & info )
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
        if( info.isDefaultOutput ) EM_error2b( 0, "default output = YES" );
        else EM_error2b( 0, "default output = NO" );
        if( info.isDefaultInput ) EM_error2b( 0, "default input = YES" );
        else EM_error2b( 0, "default input = NO" );
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
    RtAudio::DeviceInfo info;
    
    // allocate RtAudio
    try { rta = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return;
    }

    // get count    
    int devices = rta->getDeviceCount();
    EM_error2b( 0, "found %d device(s) ...", devices );
    // EM_error2( 0, "--------------------------" );
    
    EM_reset_msg();
    
    // loop
    for( int i = 0; i < devices; i++ )
    {
        try { info = rta->getDeviceInfo(i); }
        catch( RtError & error )
        {
            error.printMessage();
            break;
        }
        
        // print
        EM_error2b( 0, "------( audio device: %d )---------------", i+1 );
        print( info );
        // skip
        if( i < devices ) EM_error2( 0, "" );
        
        EM_reset_msg();
    }

    delete rta;
#endif // __DISABLE_RTAUDIO__

    return;
}




//-----------------------------------------------------------------------------
// name: device_named()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ Digitalio::device_named( const std::string & name, t_CKBOOL needs_dac, t_CKBOOL needs_adc )
{
#ifndef __DISABLE_RTAUDIO__
    RtAudio * rta = NULL;
    RtAudio::DeviceInfo info;
    
    // allocate RtAudio
    try { rta = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return -1;
    }
    
    // get count    
    int devices = rta->getDeviceCount();
    int device_no = -1;
    
    // loop
    for( int i = 0; i < devices; i++ )
    {
        try { info = rta->getDeviceInfo(i); }
        catch( RtError & error )
        {
            error.printMessage();
            break;
        }
        
        if( info.name.compare(name) == 0 )
        {
            device_no = i+1;
            break;
        }
    }
    
    if( device_no == -1 )
    {
        // no exact match found; try partial match
        for( int i = 0; i < devices; i++ )
        {
            try { info = rta->getDeviceInfo(i); }
            catch( RtError & error )
            {
                error.printMessage();
                break;
            }
            
            if( info.name.find(name) != std::string::npos )
            {
                if( (needs_dac && info.outputChannels == 0) ||
                    (needs_adc && info.inputChannels == 0) )
                    continue;
                device_no = i+1;
                break;
            }
        }
    }

    // clean up
    SAFE_DELETE( rta );
    
    // done
    return device_no;
    
#endif // __DISABLE_RTAUDIO__
    
    return -1;
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
                              void * callback, void * data,
                              BOOL__ force_srate )
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
    unsigned int bufsize = m_buffer_size;

    // log
    EM_log( CK_LOG_FINE, "initializing RtAudio..." );
    // push indent
    EM_pushlog();
        
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
            EM_error2( 0, "%s", err.getMessage().c_str() );
            return m_init = FALSE;
        }
        
        // convert 1-based ordinal to 0-based ordinal (added 1.3.0.0)
        // note: this is to preserve previous devices numbering after RtAudio change
        if( m_num_channels_out > 0 )
        {
            // check output device number; 0 used to mean "default"
            bool useDefault = ( m_dac_n == 0 );

            // default (refactor 1.3.1.2)
            if( useDefault )
            {
                // get the default
                m_dac_n = m_rtaudio->getDefaultOutputDevice();
            }
            else
            {
                m_dac_n -= 1;
            }

            // get device info
            RtAudio::DeviceInfo device_info = m_rtaudio->getDeviceInfo(m_dac_n);
                
            // ensure correct channel count if default device is requested
            if( useDefault )
            {
                // check
                if( device_info.outputChannels < m_num_channels_out )
                {
                    // find first device with at least the requested channel count
                    m_dac_n = -1;
                    int num_devices = m_rtaudio->getDeviceCount();
                    for( int i = 0; i < num_devices; i++ )
                    {
                        device_info = m_rtaudio->getDeviceInfo(i);
                        if(device_info.outputChannels >= m_num_channels_out)
                        {
                            m_dac_n = i;
                            break;
                        }
                    }
                    
                    // check for error
                    if( m_dac_n == -1 )
                    {
                        EM_error2( 0, "no audio output device with requested channel count (%i)...", m_num_channels_out );
                        return m_init = FALSE;
                    }
                }
            }

            // index of closest sample rate
            long closestIndex = -1;
            // difference of closest so far
            long closestDiff = LONG_MAX;
            // the next highest
            long nextHighest = -1;
            // diff to next highest so far
            long diffToNextHighest = LONG_MAX;
            // check if request sample rate in support rates (added 1.3.1.2)
            for( long i = 0; i < device_info.sampleRates.size(); i++ )
            {
                // difference
                long diff = device_info.sampleRates[i] - sampling_rate;
                // check // ge: changed from abs to labs, 2015.11
                if( ::labs(diff) < closestDiff )
                {
                    // remember index
                    closestIndex = i;
                    // update diff
                    closestDiff = ::labs(diff);
                }

                // for next highest
                if( diff > 0 && diff < diffToNextHighest )
                {
                    // remember index
                    nextHighest = i;
                    // update diff
                    diffToNextHighest = diff;
                }
            }
            
            // see if we found exact match (added 1.3.1.2)
            if( closestDiff != 0 )
            {
                // check
                if( force_srate )
                {
                    // request sample rate not found, error out
                    EM_error2( 0, "unsupported sample rate (%d) requested...", sampling_rate );
                    EM_error2( 0, "| (try --probe to enumerate available sample rates)" );
                    return m_init = FALSE;
                }

                // use next highest if available
                if( nextHighest >= 0 )
                {
                    // log
                    EM_log( CK_LOG_SEVERE, "new sample rate (next highest): %d -> %d",
                            sampling_rate, device_info.sampleRates[nextHighest] );
                    // update sampling rate
                    m_sampling_rate = sampling_rate = device_info.sampleRates[nextHighest];
                }
                else if( closestIndex >= 0 ) // nothing higher
                {
                    // log
                    EM_log( CK_LOG_SEVERE, "new sample rate (closest): %d -> %d",
                            sampling_rate, device_info.sampleRates[closestIndex] );
                    // update sampling rate
                    m_sampling_rate = sampling_rate = device_info.sampleRates[closestIndex];
                }
                else
                {
                    // nothing to do (will fail and throw error message when opening)
                }
            }
        }
        
        // convert 1-based ordinal to 0-based ordinal
        if( m_num_channels_in > 0 )
        {
            if( m_adc_n == 0 )
            {
                m_adc_n = m_rtaudio->getDefaultInputDevice();
                
                // ensure correct channel count if default device is requested
                RtAudio::DeviceInfo device_info = m_rtaudio->getDeviceInfo(m_adc_n);
                
                // check if input channels > 0
                if( device_info.inputChannels < m_num_channels_in )
                {
                    // find first device with at least the requested channel count
                    m_adc_n = -1;
                    int num_devices = m_rtaudio->getDeviceCount();
                    for(int i = 0; i < num_devices; i++)
                    {
                        device_info = m_rtaudio->getDeviceInfo(i);
                        if(device_info.inputChannels >= m_num_channels_in)
                        {
                            m_adc_n = i;
                            break;
                        }
                    }

                    // changed 1.3.1.2 (ge): for input, if nothing found, we just gonna try to open half-duplex
                    if( m_adc_n == -1 )
                    {
                        // set to 0
                        m_num_channels_in = 0;
                        // problem finding audio devices, most likely
                        // EM_error2( 0, "unable to find audio input device with requested channel count (%i)...", m_num_channels_in);
                        // return m_init = FALSE;
                    }
                }
            }
            else
            {
                m_adc_n -= 1;
            }
        }

        // open device
        try {
            // log
            EM_log( CK_LOG_FINE, "trying %d input %d output...", 
                    m_num_channels_in, m_num_channels_out );
            
            RtAudio::StreamParameters output_parameters;
            output_parameters.deviceId = m_dac_n;
            output_parameters.nChannels = m_num_channels_out;
            output_parameters.firstChannel = 0;
            
            RtAudio::StreamParameters input_parameters;
            input_parameters.deviceId = m_adc_n;
            input_parameters.nChannels = m_num_channels_in;
            input_parameters.firstChannel = 0;
            
            RtAudio::StreamOptions stream_options;
            stream_options.flags = 0;
            stream_options.numberOfBuffers = num_buffers;
            stream_options.streamName = "ChucK";
            stream_options.priority = 0;
            
            // open RtAudio
            m_rtaudio->openStream(
                m_num_channels_out > 0 ? &output_parameters : NULL, 
                m_num_channels_in > 0 ? &input_parameters : NULL,
                CK_RTAUDIO_FORMAT, sampling_rate, &bufsize, 
                m_use_cb ? ( block ? &cb : &cb2 ) : NULL, vm_ref, 
                &stream_options );
        } catch( RtError err ) {
            // log
            EM_log( CK_LOG_INFO, "exception caught: '%s'...", err.getMessage().c_str() );
            EM_error2( 0, "%s", err.getMessage().c_str() );
            SAFE_DELETE( m_rtaudio );
            return m_init = FALSE;
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
    if( !MoAudio::init( sampling_rate, buffer_size, 2 ) )
    {
        EM_error2( 0, "%s", "(chuck)error: unable to initialize MoAudio..." );
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
//int Digitalio::cb( char * buffer, int buffer_size, void * user_data )
int Digitalio::cb( void *output_buffer, void *input_buffer,
                   unsigned int buffer_size,
                   double streamTime,
                   RtAudioStreamStatus status,
                   void *user_data )
{
    DWORD__ len = buffer_size * sizeof(SAMPLE) * m_num_channels_out;
    DWORD__ n = 20;
    DWORD__ start = 50;

    // copy input to local buffer
    if( m_num_channels_in )
    {
        memcpy( m_buffer_in, input_buffer, len );
        // copy to extern
        if( m_extern_in ) memcpy( m_extern_in, input_buffer, len );
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
        if( m_out_ready && !m_end ) memcpy( output_buffer, m_buffer_out, len );
        // set all elements of local buffer to silence
        else memset( output_buffer, 0, len );
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

        memset( output_buffer, 0, len );
        m_go++;
        return 0;
    }

    // 2nd buffer
    // RtAudio4 TODO
//    if( m_go == start )
//    {
//        n = 8; while( !m_out_ready && n-- ) usleep( 250 );
//        len /= sizeof(SAMPLE); DWORD__ i = 0;
//        SAMPLE * s = (SAMPLE *) buffer;
//        while( i < len ) *s++ *= (SAMPLE)i++/len;
//        m_go++;
//    }

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, output_buffer, len );

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
//int Digitalio::cb2( char * buffer, int buffer_size, void * user_data )
int Digitalio::cb2( void *output_buffer, void *input_buffer,
                    unsigned int buffer_size,
                    double streamTime,
                    RtAudioStreamStatus status,
                    void *user_data )
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
        memset( output_buffer, 0, len );
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
        memcpy( m_buffer_in, input_buffer, len );
        // copy to extern
        if( m_extern_in ) memcpy( m_extern_in, input_buffer, len );
    }

    // check xrun
    if( m_xrun < 6 )
    {
        // timestamp
        if( g_do_watchdog ) g_watchdog_time = get_current_time( TRUE );
        // get samples from output
        vm_ref->run( buffer_size, m_buffer_in, m_buffer_out );
        // ...
        if( m_xrun ) m_xrun--;
    }
    else
    {
        // reset
        m_xrun /= 2;
    }

    // copy local buffer to be rendered
    if( !m_end ) memcpy( output_buffer, m_buffer_out, len );
    // set all elements of local buffer to silence
    else memset( output_buffer, 0, len );

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, output_buffer, len );


    return 0;
}




#ifdef __CHIP_MODE__
//-----------------------------------------------------------------------------
// name: MoAudio_cb()
// desc: ...
//-----------------------------------------------------------------------------
void MoAudio_cb( Float32 * buffer, UInt32 numFrames, void * userData )
{
    Digitalio::cb2( (char *)buffer, (char *)buffer, numFrames, 0, 0, userData );
}
#endif // __CHIP_MODE__




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
        m_start = MoAudio::start( MoAudio_cb, g_vm );
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
        MoAudio::stop();
    m_start = FALSE;
#endif

    return !m_start;
}




//-----------------------------------------------------------------------------
// name: tick()
// desc: ...
//-----------------------------------------------------------------------------
//BOOL__ Digitalio::tick( )
//{
//#ifndef __DISABLE_RTAUDIO__
//    try
//    {
//        if( ++m_tick_count >= m_start )
//        {
//            m_rtaudio->tickStream();
//            m_tick_count = 0;
//            m_out_ready = TRUE;
//            m_in_ready = TRUE;
//        }
//        
//        return TRUE;
//    } catch( RtError err ){ return FALSE; }
//#endif // __DISABLE_RTAUDIO__
//    
//    return FALSE;
//}




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
        //if( m_use_cb ) m_rtaudio->cancelStreamCallback();
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
    // RtAudio4 TODO
    m_data_ptr_out = Digitalio::m_use_cb ? Digitalio::m_buffer_out : NULL;
    // : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
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
    // RtAudio4 TODO
    m_data_ptr_in = Digitalio::m_use_cb ? m_data : NULL;
    // : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
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
