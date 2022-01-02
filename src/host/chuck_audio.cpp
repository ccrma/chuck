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
// name: chuck_audio.cpp
// desc: chuck host digital audio I/O, using RtAudio
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// RtAudio by: Gary Scavone
//
// date: Spring 2004 -- initial versions
//       Fall 2017 -- REFACTOR-2017: clean up to be more modular
//-----------------------------------------------------------------------------
#include "chuck_audio.h"
#include "chuck_errmsg.h"
#include "util_thread.h"
#include <limits.h>
#include "rtmidi.h"

#if (defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)) && !defined(__WINDOWS_PTHREAD__)
  #include <windows.h>
  #include <sys/timeb.h>
#else
  #include <unistd.h>
  #include <sys/time.h>
#endif




// sample
#if defined(__CHUCK_USE_64_BIT_SAMPLE__)
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT64
#else
#define CK_RTAUDIO_FORMAT RTAUDIO_FLOAT32
#endif




// real-time watch dog
t_CKBOOL g_do_watchdog = FALSE;
// countermeasure
#ifdef __MACOSX_CORE__
t_CKUINT g_watchdog_countermeasure_priority = 10;
#elif defined(__PLATFORM_WIN32__) && !defined(__WINDOWS_PTHREAD__)
t_CKUINT g_watchdog_countermeasure_priority = THREAD_PRIORITY_LOWEST;
#else
t_CKUINT g_watchdog_countermeasure_priority = 0;
#endif
// watchdog timeout
t_CKFLOAT g_watchdog_timeout = 0.5;




// static initialization
t_CKBOOL ChuckAudio::m_init = FALSE;
t_CKBOOL ChuckAudio::m_start = FALSE;
t_CKBOOL ChuckAudio::m_go = FALSE;
t_CKBOOL ChuckAudio::m_silent = FALSE;
t_CKBOOL ChuckAudio::m_expand_in_mono2stereo= FALSE;
t_CKUINT ChuckAudio::m_num_channels_out = NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_num_channels_in = NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_num_channels_max = NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_sample_rate = SAMPLE_RATE_DEFAULT;
t_CKUINT ChuckAudio::m_buffer_size = BUFFER_SIZE_DEFAULT;
t_CKUINT ChuckAudio::m_num_buffers = NUM_BUFFERS_DEFAULT;
SAMPLE * ChuckAudio::m_buffer_out = NULL;
SAMPLE * ChuckAudio::m_buffer_in = NULL;
SAMPLE * ChuckAudio::m_extern_in = NULL;
SAMPLE * ChuckAudio::m_extern_out = NULL;
t_CKUINT ChuckAudio::m_dac_n = 0;
t_CKUINT ChuckAudio::m_adc_n = 0;
RtAudio * ChuckAudio::m_rtaudio = NULL;
f_audio_cb ChuckAudio::m_audio_cb = NULL;
void * ChuckAudio::m_cb_user_data = NULL;




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




//-----------------------------------------------------------------------------
// name: probe()
// desc: ...
//-----------------------------------------------------------------------------
void ChuckAudio::probe()
{
    // rtaduio pointer
    RtAudio * audio = NULL;
    // device info struct
    RtAudio::DeviceInfo info;
    
    // allocate RtAudio
    try { audio = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return;
    }

    // get count
    int devices = audio->getDeviceCount();
    EM_error2b( 0, "found %d device(s) ...", devices );

    // reset -- what does this do
    EM_reset_msg();
    
    // loop over devices
    for( int i = 0; i < devices; i++ )
    {
        try { info = audio->getDeviceInfo(i); }
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
        
        // reset
        EM_reset_msg();
    }

    // done
    SAFE_DELETE( audio );

    return;
}




//-----------------------------------------------------------------------------
// name: device_named()
// desc: get device number by name; needs_dac/adc prompts further checks on
//       requested device having > 0 channels
//-----------------------------------------------------------------------------
t_CKUINT ChuckAudio::device_named( const std::string & name, t_CKBOOL needs_dac,
                                   t_CKBOOL needs_adc )
{
    // rtaudio pointer
    RtAudio * audio = NULL;
    // device info struct
    RtAudio::DeviceInfo info;
    
    // allocate RtAudio
    try { audio = new RtAudio(); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return -1;
    }
    
    // get count    
    int devices = audio->getDeviceCount();
    int device_no = -1;
    
    // loop
    for( int i = 0; i < devices; i++ )
    {
        // get info
        try { info = audio->getDeviceInfo(i); }
        catch( RtError & error )
        {
            error.printMessage();
            break;
        }

        // compare name
        if( info.name.compare(name) == 0 )
        {
            // found!
            device_no = i+1;
            break;
        }
    }
    
    // not found, yet
    if( device_no == -1 )
    {
        // no exact match found; try partial match
        for( int i = 0; i < devices; i++ )
        {
            try { info = audio->getDeviceInfo(i); }
            catch( RtError & error )
            {
                error.printMessage();
                break;
            }
            
            if( info.name.find(name) != std::string::npos )
            {
                // skip over ones with 0 channels, if needed
                if( (needs_dac && info.outputChannels == 0) ||
                    (needs_adc && info.inputChannels == 0) )
                    continue;
                // found
                device_no = i+1;
                break;
            }
        }
    }

    // clean up
    SAFE_DELETE( audio );
    
    // done
    return device_no;
}




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
}




// watch dog globals
static CHUCK_THREAD g_tid_synthesis = 0;
static XThread * g_watchdog_thread = NULL;
static t_CKBOOL g_watchdog_state = FALSE;
static t_CKFLOAT g_watchdog_time = 0;




//-----------------------------------------------------------------------------
// name: watch_dog()
// desc: this sniff out possible non-time advancing infinite loops
//-----------------------------------------------------------------------------
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
static void * watch_dog( void * )
#else
static unsigned int __stdcall watch_dog( void * )
#endif
{
    // time
    t_CKFLOAT time = 0;

    // boost priority?
    t_CKUINT priority = XThreadUtil::our_priority;

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
    if( XThreadUtil::our_priority != 0x7fffffff )
        XThreadUtil::set_priority( priority );

    // while going
    while( g_do_watchdog )
    {
        // get
        time = get_current_time( TRUE );
        // CK_FPRINTF_STDERR( "last: %f now: %f\n", g_watchdog_time, time );

        // resting
        if( g_watchdog_state == FALSE )
        {
            // check time vs timeout
            if( time - g_watchdog_time > g_watchdog_timeout )
            {
                // log
                EM_log( CK_LOG_SEVERE, "real-time watchdog counter-measure activating..." );
                // lowering priority
                if( g_tid_synthesis && XThreadUtil::our_priority != 0x7fffffff )
                    XThreadUtil::set_priority( g_tid_synthesis, g_watchdog_countermeasure_priority );
                // set state
                g_watchdog_state = TRUE;
            }
        }
        else
        {
            // check time vs timeout
            if( time - g_watchdog_time < g_watchdog_timeout )
            {
                // log
                EM_log( CK_LOG_SEVERE, "real-time watchdog resting..." );
                // raise priority
                if( g_tid_synthesis && XThreadUtil::our_priority != 0x7fffffff )
                    XThreadUtil::set_priority( g_tid_synthesis, XThreadUtil::our_priority );
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
// desc: start watchdog on separate thread
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::watchdog_start()
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
// desc: stop watchdog thread
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::watchdog_stop()
{
    // already
    if( !g_watchdog_thread )
        return FALSE;

    // stop things
    g_do_watchdog = FALSE;
    // wait a bit | TODO: is this needed?
    usleep( 100000 );
    // delete watchdog thread | TODO: is this safe?
    SAFE_DELETE( g_watchdog_thread );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::initialize( t_CKUINT num_dac_channels,
                                 t_CKUINT num_adc_channels,
                                 t_CKUINT sample_rate,
                                 t_CKUINT buffer_size,
                                 t_CKUINT num_buffers,
                                 f_audio_cb callback,
                                 void * data,
                                 t_CKBOOL force_srate )
{
    if( m_init )
        return FALSE;

    m_num_channels_out = num_dac_channels;
    m_num_channels_in = num_adc_channels;
    m_sample_rate = sample_rate;
    m_buffer_size = buffer_size;
    m_num_buffers = num_buffers;
    m_start = 0;
    m_go = 0;
    m_silent = 0;
    
    // remember callback
    m_audio_cb = callback;
    // remember user data to pass to callback
    m_cb_user_data = data;

    // variable to pass by reference into RtAudio
    unsigned int bufsize = m_buffer_size;

    // log
    EM_log( CK_LOG_FINE, "initializing RtAudio..." );
    // push indent
    EM_pushlog();

    // allocate RtAudio
    try { m_rtaudio = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2( 0, "%s", err.getMessage().c_str() );
        // clean up
        goto error;
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
                    EM_error2( 0, "no audio output device with requested channel count (%i)...",
                               m_num_channels_out );
                    // clean up
                    goto error;
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
            long diff = device_info.sampleRates[i] - sample_rate;
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
                EM_error2( 0, "unsupported sample rate (%d) requested...", sample_rate );
                EM_error2( 0, "| (try --probe to enumerate available sample rates)" );
                // clean up
                goto error;
            }

            // use next highest if available
            if( nextHighest >= 0 )
            {
                // log
                EM_log( CK_LOG_SEVERE, "new sample rate (next highest): %d -> %d",
                        sample_rate, device_info.sampleRates[nextHighest] );
                // update sampling rate
                m_sample_rate = sample_rate = device_info.sampleRates[nextHighest];
            }
            else if( closestIndex >= 0 ) // nothing higher
            {
                // log
                EM_log( CK_LOG_SEVERE, "new sample rate (closest): %d -> %d",
                        sample_rate, device_info.sampleRates[closestIndex] );
                // update sampling rate
                m_sample_rate = sample_rate = device_info.sampleRates[closestIndex];
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
        // if we are requesting the default input device
        if( m_adc_n == 0 )
        {
            // get default input device
            m_adc_n = m_rtaudio->getDefaultInputDevice();
            
            // ensure correct channel count if default device is requested
            RtAudio::DeviceInfo device_info = m_rtaudio->getDeviceInfo(m_adc_n);
            
            // check if input channels > 0
            if( device_info.inputChannels < m_num_channels_in )
            {
                // find first device with at least the requested channel count
                m_adc_n = -1;
                int num_devices = m_rtaudio->getDeviceCount();
                for( int i = 0; i < num_devices; i++ )
                {
                    device_info = m_rtaudio->getDeviceInfo(i);
                    if( device_info.inputChannels >= m_num_channels_in )
                    {
                        // added 1.4.0.1 -- check if the sample rate is supported
                        // this is to catch the case where the default device has
                        // insufficient channels (e.g., 1 on MacOS), finds secondary device
                        // (e.g., ZoomAudioDevice on MacOS) that has enough channels
                        // but does not support the desired sample rate
                        bool isMatch = false;
                        for( long j = 0; j < device_info.sampleRates.size(); j++ )
                        {
                            if( device_info.sampleRates[j] == m_sample_rate )
                            {
                                // flag
                                isMatch = true;
                                // break out sample rate loop
                                break;
                            }
                        }
                        
                        // match? (does this separately to break out of outer loop)
                        if( isMatch )
                        {
                            // new input device
                            m_adc_n = i;
                            // log
                            EM_log( CK_LOG_INFO, "found alternate input device: %d...", i );
                            // break out of device loop
                            break;
                        }
                    }
                }
                
                // added 1.4.0.1 (ge) -- special case if no other matching device found...
                // for systems that have default mono audio device e.g., some MacOS systems
                if( m_adc_n == -1 )
                {
                    // get default input device
                    m_adc_n = m_rtaudio->getDefaultInputDevice();
                    // get device info
                    device_info = m_rtaudio->getDeviceInfo(m_adc_n);

                    // special case
                    if( m_num_channels_in == 2 && device_info.inputChannels == 1 )
                    {
                        // flag this for expansion in the callback
                        m_expand_in_mono2stereo = TRUE;
                        // log
                        EM_log( CK_LOG_INFO, "no matching stereo input device found..." );
                        EM_log( CK_LOG_INFO, "simulating stereo input from mono audio device..." );
                    }
                    else
                    {
                        // okay, let's not impose any further; fall through to subseqent logic
                        m_adc_n = -1;
                    }
                }

                // changed 1.3.1.2 (ge): for input, if nothing found, we just gonna try to open half-duplex
                if( m_adc_n == -1 )
                {
                    // set to 0
                    m_num_channels_in = 0;
                    // problem finding audio devices, most likely
                    // EM_error2( 0, "unable to find audio input device with requested channel count (%i)...",
                    //               m_num_channels_in);
                    // clean up
                    // goto error;
                }
            }
        }
        else
        {
            // offset to 0-index device number
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
        input_parameters.nChannels = m_expand_in_mono2stereo ? 1 : m_num_channels_in;
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
            CK_RTAUDIO_FORMAT, sample_rate, &bufsize,
            cb, m_cb_user_data,
            &stream_options );
    } catch( RtError err ) {
        // log
        EM_log( CK_LOG_INFO, "exception caught: '%s'...", err.getMessage().c_str() );
        EM_error2( 0, "%s", err.getMessage().c_str() );
        SAFE_DELETE( m_rtaudio );
        // clean up
        goto error;
    }
        
    // check bufsize
    if( bufsize != (int)m_buffer_size )
    {
        EM_log( CK_LOG_SEVERE, "new buffer size: %d -> %i", m_buffer_size, bufsize );
        m_buffer_size = bufsize;
    }

    // pop indent
    EM_poplog();

    // greater of dac/adc channels
    m_num_channels_max = num_dac_channels > num_adc_channels ?
                         num_dac_channels : num_adc_channels;
    // log
    EM_log( CK_LOG_SEVERE, "allocating buffers for %d x %d samples...",
            m_buffer_size, m_num_channels_max );

    // allocate buffers
    m_buffer_in = new SAMPLE[m_buffer_size * m_num_channels_max];
    m_buffer_out = new SAMPLE[m_buffer_size * m_num_channels_max];
    memset( m_buffer_in, 0, m_buffer_size * sizeof(SAMPLE) * m_num_channels_max );
    memset( m_buffer_out, 0, m_buffer_size * sizeof(SAMPLE) * m_num_channels_max );

    // set flag and return
    return m_init = TRUE;
    
// handle common error case (added 1.4.1.0)
error:
    // pop log indent (added 1.4.1.0)
    EM_poplog();
    // set flag and return
    return m_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: cb2()
// desc: ...
//-----------------------------------------------------------------------------
int ChuckAudio::cb( void * output_buffer, void * input_buffer, unsigned int buffer_size,
                    double streamTime, RtAudioStreamStatus status, void * user_data )
{
    // length, in bytes of output
    t_CKUINT len = buffer_size * sizeof(SAMPLE) * m_num_channels_out;
    
    // priority boost
    if( !m_go && XThreadUtil::our_priority != 0x7fffffff )
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
        XThreadUtil::set_priority( XThreadUtil::our_priority );
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
        // added 1.4.0.1 (ge) -- special case in=1; out=2
        if( m_expand_in_mono2stereo )
        {
            for( int i = 0; i < buffer_size; i++ )
            {
                // copy mono sample into stereo buffer
                m_buffer_in[i*2] = m_buffer_in[i*2+1] = ((SAMPLE *)input_buffer)[i];
            }
        }
        else
        {
            memcpy( m_buffer_in, input_buffer, len );
        }
        // copy to extern
        if( m_extern_in ) memcpy( m_extern_in, input_buffer, len );
    }

    // timestamp
    if( g_do_watchdog ) g_watchdog_time = get_current_time( TRUE );
    // call the audio cb
    m_audio_cb( m_buffer_in, m_buffer_out, buffer_size,
        m_num_channels_in, m_num_channels_out, m_cb_user_data );

    // copy local buffer to be rendered
    // REFACTOR-2017: TODO: m_end was the signal to end, what should it be now?
    if( m_start ) // REFACTOR-2017: now using m_start to gate this
    {
        memcpy( output_buffer, m_buffer_out, len );
    }
    // set all elements of local buffer to silence
    else
    {
        memset( output_buffer, 0, len );
    }

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, output_buffer, len );

    return 0;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: start host audio
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::start( )
{
    try {
        if( !m_start )
            m_rtaudio->startStream();
        m_start = TRUE;
    }
    catch( RtError err )
    {
        return FALSE;
    }

    return m_start;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: stop host audio
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::stop( )
{
    try {
        if( m_start )
            m_rtaudio->stopStream();
        m_start = FALSE;
    }
    catch( RtError err )
    {
        return FALSE;
    }
    
    // REFACTOR-2017: set end flag
    m_silent = TRUE;

    return !m_start;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void ChuckAudio::shutdown()
{
    // check flag
    if( !m_init ) return;

    // stop watchdog | REFACTOR-2017: moved this earlier to give watchdog
    // a head start in stopping, to avoid unneeded countermeasure deployment
    watchdog_stop();

    // check if started
    if( m_start )
    {
        // m_rtaudio->cancelStreamCallback();
        // m_rtaudio->stopStream();
        stop();
    }

    // close stream
    m_rtaudio->closeStream();
    // cleanup
    SAFE_DELETE( m_rtaudio );

    // unflag
    m_init = FALSE;
    m_start = FALSE;
}
