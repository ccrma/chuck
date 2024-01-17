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
#include "util_string.h"
#include "util_thread.h"
#include "util_platforms.h"
#include <limits.h>
#include "rtmidi.h"

#if (defined(__PLATFORM_WINDOWS__) && !defined(__WINDOWS_PTHREAD__))
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
#ifdef __PLATFORM_APPLE__
t_CKUINT g_watchdog_countermeasure_priority = 10;
#elif defined(__PLATFORM_WINDOWS__) && !defined(__WINDOWS_PTHREAD__)
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
t_CKUINT ChuckAudio::m_num_channels_out = CK_NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_num_channels_in = CK_NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_num_channels_max = CK_NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_sample_rate = CK_SAMPLE_RATE_DEFAULT;
t_CKUINT ChuckAudio::m_buffer_size = CK_BUFFER_SIZE_DEFAULT;
t_CKUINT ChuckAudio::m_num_buffers = CK_NUM_BUFFERS_DEFAULT;
SAMPLE * ChuckAudio::m_buffer_out = NULL;
SAMPLE * ChuckAudio::m_buffer_in = NULL;
SAMPLE * ChuckAudio::m_extern_in = NULL;
SAMPLE * ChuckAudio::m_extern_out = NULL;
t_CKINT ChuckAudio::m_dac_n = 0;
t_CKINT ChuckAudio::m_adc_n = 0;
std::string ChuckAudio::m_dac_name = "";
std::string ChuckAudio::m_adc_name = "";
std::string ChuckAudio::m_driver_name = "";
RtAudio * ChuckAudio::m_rtaudio = NULL;
ck_f_audio_cb ChuckAudio::m_audio_cb = NULL;
void * ChuckAudio::m_cb_user_data = NULL;




//-----------------------------------------------------------------------------
// global symbols expected from RtAudio | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
extern "C" const unsigned int rtaudio_num_compiled_apis;
extern "C" const unsigned int rtaudio_compiled_apis[];
extern "C" const char* rtaudio_api_names[][2];




//-----------------------------------------------------------------------------
// name: driverNameToApi()
// desc: look up from driver name to API/driver
//-----------------------------------------------------------------------------
static RtAudio::Api driverNameToApi( const char * driver )
{
    RtAudio::Api api = RtAudio::UNSPECIFIED;
    if( driver )
    {
        // get lowercase version
        std::string driverLower = ::tolower(::trim(driver));
        // from what is available, match by driver name
    #if defined(__WINDOWS_ASIO__)
        if( driverLower == "asio" )
            api = RtAudio::WINDOWS_ASIO;
    #endif
    #if defined(__WINDOWS_DS__)
        if( driverLower == "ds" || driverLower == "directsound" )
            api = RtAudio::WINDOWS_DS;
    #endif
    #if defined(__WINDOWS_WASAPI__)
        if( driverLower == "wasapi" )
            api = RtAudio::WINDOWS_WASAPI;
    #endif
    #if defined(__LINUX_ALSA__)
        if( driverLower == "alsa" )
            api = RtAudio::LINUX_ALSA;
    #endif
    #if defined(__LINUX_PULSE__)
        if( driverLower == "pulse" )
            api = RtAudio::LINUX_PULSE;
    #endif
    #if defined(__LINUX_OSS__)
        if( driverLower == "oss" )
            api = RtAudio::LINUX_OSS;
    #endif
    #if defined(__UNIX_JACK__)
        if( driverLower == "jack" )
            api = RtAudio::UNIX_JACK;
    #endif
    #if defined(__MACOSX_CORE__)
        if( driverLower == "coreaudio" || driverLower == "core" )
            api = RtAudio::MACOSX_CORE;
    #endif

        // check for Dummy
        if( driverLower == "(dummy)" )
            api = RtAudio::RTAUDIO_DUMMY;

        // XXX: add swap between ALSA, PULSE, OSS? and JACK
        if( api == RtAudio::UNSPECIFIED ) 
        {
            EM_error2( 0, "unsupported audio driver: %s", driver );
        }
    }

    // 1.5.0.0 (nshaheed) If the audio driver is UNSPECIFIED then
    // return a default driver. This will depend on OS and, in the
    // case of linux, which drivers chuck is being compiled with.
    if( api == RtAudio::UNSPECIFIED )
    {
    #if defined(__PLATFORM_WINDOWS__)
        // traditional chuck default behavior:
        // DirectSound is most general albeit high-latency
        api = RtAudio::WINDOWS_DS;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_ALSA__)
        api = RtAudio::LINUX_ALSA;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_PULSE__)
        api = RtAudio::LINUX_PULSE;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_OSS__)
        api = RtAudio::LINUX_OSS;
    #elif defined(__PLATFORM_LINUX__) && defined(__UNIX_JACK__)
        api = RtAudio::UNIX_JACK;
    #elif defined(__MACOSX_CORE__)
        api = RtAudio::MACOSX_CORE;
    #endif
    }

    return api;
}




//-----------------------------------------------------------------------------
// name: apiToDriverName()
// desc: get driver name from API
//-----------------------------------------------------------------------------
static const char * apiToDriverName( RtAudio::Api api )
{
    static const char * const drivers[] = {
        "(Unspecified)",
        "ALSA",
        "Pulse",
        "OSS",
        "Jack",
        "CoreAudio",
        "WASAPI",
        "ASIO",
        "DirectSound", // DirectSound
        "(DUMMY)"
    };

    // cast and check
    t_CKINT index = (t_CKINT)api;
    if( index < 0 || index >= sizeof(drivers) / sizeof(const char *) ) return NULL;

    // return
    return drivers[index];
}




//-----------------------------------------------------------------------------
// name: print()
// desc: print info for an audio device
//-----------------------------------------------------------------------------
void print( const RtAudio::DeviceInfo & info )
{
    EM_error2b( 0, "device name = \"%s\"", info.name.c_str() );
    if (!info.probed)
        EM_error2b( 0, "probe [failed]..." );
    else
    {
        EM_error2b( 0, "probe [success]..." );
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
// name: rtAudioErrorHandler()
// desc: the RtAudio error handler
//-----------------------------------------------------------------------------
static void rtAudioErrorHandler( RtAudioErrorType t, const std::string & errorText )
{
    // check if empty string
    if( errorText.empty() ) return;

    // check if warning or not a warning
    if( t == RTAUDIO_WARNING )
    {
        // output to warning log level
        EM_log( CK_LOG_WARNING, "%s", errorText.c_str() );
    }
    else
    {
        // output as error
        EM_error2( 0, "%s", errorText.c_str() );
        // NOTE: problem finding audio devices, likely
    }
}




//-----------------------------------------------------------------------------
// name: probe()
// desc: probe audio devices by driver
//-----------------------------------------------------------------------------
void ChuckAudio::probe( const char * driver )
{
    // get the audio driver enum by name; handles driver == NULL case
    RtAudio::Api api = driverNameToApi( driver );
    // go back from driver enum to driver name
    const char * dnm = apiToDriverName( api );
    // rtaudio pointer
    RtAudio * audio = NULL;
    // device info struct
    RtAudio::DeviceInfo info;
    
    // allocate RtAudio
    audio = new RtAudio( api, rtAudioErrorHandler );
    if( !audio )
    {
        // error reported by our errorHandler
        return;
    }

    // get count
    int devices = audio->getDeviceCount();
    EM_error2b( 0, "[%s] driver found %d audio device(s)...", TC::green(dnm,TRUE).c_str(), devices );
    EM_error2b( 0, "" );

    // reset -- what does this do
    EM_reset_msg();
    
    // loop over devices
    for( int i = 0; i < devices; i++ )
    {
        info = audio->getDeviceInfo(i);
        if (!info.probed) { // errorHandle reports issues above
            EM_error2b(0, "");
            EM_reset_msg();
            continue;
        }
        
        // print
        EM_print2blue( "------( audio device: %d )------", i+1 );
        print( info );
        // skip
        if( i < devices ) EM_error2b( 0, "" );
        
        // reset
        EM_reset_msg();
    }

    // done
    CK_SAFE_DELETE( audio );

    return;
}




//-----------------------------------------------------------------------------
// name: device_named()
// desc: get device number by name; needs_dac/adc prompts further checks on
//       requested device having > 0 channels
//-----------------------------------------------------------------------------
t_CKINT ChuckAudio::device_named( const char * driver,
                                  const std::string & name,
                                  t_CKBOOL needs_dac,
                                  t_CKBOOL needs_adc )
{
    // rtaudio pointer
    RtAudio * audio = NULL;
    // device info struct
    RtAudio::DeviceInfo info;
    RtAudio::Api api = driverNameToApi(driver); // handles driver=NULL case
    // const char * dnm = apiToDriverName(api);
    
    // allocate RtAudio
    audio = new RtAudio(api, rtAudioErrorHandler);
    if(!audio)
        return -1; // reporting in errorHandler
    
    // get count    
    int devices = audio->getDeviceCount();
    int device_no = -1;
    
    // loop
    for( int i = 0; i < devices; i++ )
    {
        // get info
        info = audio->getDeviceInfo(i);
        if(!info.probed)
        {
            // reportError handles problem
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
            info = audio->getDeviceInfo(i);
            if(!info.probed)
            {
                // errorHandler reports issues
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
    CK_SAFE_DELETE( audio );
    
    // done
    return device_no;
}




//-----------------------------------------------------------------------------
// name: defaultDriverApi()
// desc: get default audio driver number, i.e., RtAudio::Api enum
//-----------------------------------------------------------------------------
RtAudio::Api ChuckAudio::defaultDriverApi()
{
    return driverNameToApi( NULL );
}




//-----------------------------------------------------------------------------
// name: defaultDriverName()
// desc: get default audio driver name
//-----------------------------------------------------------------------------
const char * ChuckAudio::defaultDriverName()
{
    return driverApiToName( defaultDriverApi() );
}




//-----------------------------------------------------------------------------
// name: driverNameToApi()
// desc: get API/driver enum
//-----------------------------------------------------------------------------
RtAudio::Api ChuckAudio::driverNameToApi( const char * driver )
{
    // if empty string, make the pointer null, which will retrieve default
    if( driver != NULL && driver[0] == '\0' ) driver = NULL;
    // get the driver enum
    return ::driverNameToApi( driver );
}




//-----------------------------------------------------------------------------
// name: driverApiToName()
// desc: get API/driver name
//-----------------------------------------------------------------------------
const char * ChuckAudio::driverApiToName( t_CKUINT num )
{
    // pass it on
    return apiToDriverName( (RtAudio::Api)num );
}




//-----------------------------------------------------------------------------
// name: numDrivers()
// desc: get number of compiled audio driver APIs
//-----------------------------------------------------------------------------
t_CKUINT ChuckAudio::numDrivers()
{
    return (t_CKUINT)rtaudio_num_compiled_apis;
}




//-----------------------------------------------------------------------------
// name: getDriver()
// desc: get info on a particular driver
//-----------------------------------------------------------------------------
ChuckAudioDriverInfo ChuckAudio::getDriverInfo( t_CKUINT n )
{
    // check
    if( n >= numDrivers() ) return ChuckAudioDriverInfo();

    ChuckAudioDriverInfo info;
    info.driver = rtaudio_compiled_apis[n];
    info.name = rtaudio_api_names[info.driver][0];
    info.userFriendlyName = rtaudio_api_names[info.driver][1];

    return info;
}




//-----------------------------------------------------------------------------
// name: get_current_time()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKFLOAT get_current_time( t_CKBOOL fresh = TRUE )
{
#ifdef __PLATFORM_WINDOWS__
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
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
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
    EM_log( CK_LOG_HERALD, "starting real-time watch dog process..." );
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
                EM_log( CK_LOG_HERALD, "real-time watchdog counter-measure activating..." );
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
                EM_log( CK_LOG_HERALD, "real-time watchdog resting..." );
                // raise priority
                if( g_tid_synthesis && XThreadUtil::our_priority != 0x7fffffff )
                    XThreadUtil::set_priority( g_tid_synthesis, XThreadUtil::our_priority );
                // set state
                g_watchdog_state = FALSE;
            }
        }
        
        // advance time
        ck_usleep( 40000 );
    }
    
    // log
    EM_log( CK_LOG_HERALD, "stopping real-time watch dog process..." );

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
    ck_usleep( 100000 );
    // delete watchdog thread | TODO: is this safe?
    CK_SAFE_DELETE( g_watchdog_thread );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: supportSampleRate()
// desc: does a device (which can potentially support multiple sample rates)
//       support a particular sample rate? 1.5.0.0 (ge) | added
//-----------------------------------------------------------------------------
static t_CKBOOL supportSampleRate( RtAudio::DeviceInfo & device_info, t_CKINT sample_rate )
{
    // loop over supported sample rates for a device
    for( t_CKINT i = 0; i < device_info.sampleRates.size(); i++ )
    {
        // look for a match
        if( device_info.sampleRates[i] == sample_rate )
        {
            return TRUE;
        }
    }

    // no match
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: findMatchingInputDevice()
// desc: find an input device with matching chans and sample rate
//       1.5.0.0 (ge) | added
//-----------------------------------------------------------------------------
static t_CKINT findMatchingInputDevice( RtAudio * rtaudio, t_CKINT numInputChans,
                                        t_CKINT sample_rate, t_CKBOOL allowMoreChans )
{
    // get number of devices
    t_CKINT num_devices = rtaudio->getDeviceCount();
    // struct to hold info for a device
    RtAudio::DeviceInfo device_info;

    // find first device with at least the requested channel count (and sample rate match)
    for( t_CKUINT i = 0; i < num_devices; i++ )
    {
        // get device info
        device_info = rtaudio->getDeviceInfo((unsigned int)i);
        // check sample rate
        if( supportSampleRate( device_info, sample_rate ) )
        {
            // check channels; the allowMoreChans toggles exact match or lenient match
            if( (allowMoreChans == FALSE && device_info.inputChannels == numInputChans) ||
                (allowMoreChans == TRUE && device_info.inputChannels >= numInputChans) )
            {
                // log
                EM_log( CK_LOG_INFO, "found alternate input device: %d...", i );
                // return device number
                return i;
            }
        }
    }

    // no match
    return -1;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::initialize( t_CKUINT dac_device,
                                 t_CKUINT adc_device,
                                 t_CKUINT num_dac_channels,
                                 t_CKUINT num_adc_channels,
                                 t_CKUINT sample_rate,
                                 t_CKUINT buffer_size,
                                 t_CKUINT num_buffers,
                                 ck_f_audio_cb callback,
                                 void * data,
                                 t_CKBOOL force_srate,
                                 const char * driver )
{
    // check if already initialized
    if( m_init ) return FALSE;

    m_dac_n = dac_device;
    m_adc_n = adc_device;
    m_dac_name = "[no output device]";
    m_adc_name = "[no input device]";
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

    // audio device probe results
    RtAudio::DeviceInfo dac_info;
    RtAudio::DeviceInfo adc_info;

    // variable to pass by reference into RtAudio
    unsigned int bufsize = (unsigned int)m_buffer_size;
    // check output device number; 0 means "default"
    bool useDefaultOut = ( m_dac_n == 0 );
    // check input device number; 0 means "default"
    bool useDefaultIn = ( m_adc_n == 0 );

    // log
    EM_log( CK_LOG_FINE, "initializing RtAudio..." );
    // push indent
    EM_pushlog();

    // allocate RtAudio
    RtAudio::Api api = driverNameToApi(driver);
    m_driver_name = apiToDriverName(api);
    RtAudioErrorType code = RTAUDIO_NO_ERROR;
    m_rtaudio = new RtAudio(api, rtAudioErrorHandler);
    if(!m_rtaudio)
    {
        // error reported above
        goto error; // to clean up
    }

    // convert 1-based ordinal to 0-based ordinal (added 1.3.0.0)
    // note: this is to preserve previous devices numbering after RtAudio change
    if( m_num_channels_out > 0 )
    {
        // default (refactor 1.3.1.2)
        if( useDefaultOut )
        {
            // get the default
            // FYI: getDefaultInputDevice returns 0 either as a valid index OR if no default/any devices
            m_dac_n = m_rtaudio->getDefaultOutputDevice();
        }
        else
        {
            m_dac_n -= 1;
        }

        // get device info
        RtAudio::DeviceInfo device_info = m_rtaudio->getDeviceInfo((unsigned int)m_dac_n);
        
        // ensure correct channel count if default device is requested
        if( useDefaultOut )
        {
            // check for output (channels)
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
                EM_log( CK_LOG_HERALD, "new sample rate (next highest): %d -> %d",
                        sample_rate, device_info.sampleRates[nextHighest] );
                // update sampling rate
                m_sample_rate = sample_rate = device_info.sampleRates[nextHighest];
            }
            else if( closestIndex >= 0 ) // nothing higher
            {
                // log
                EM_log( CK_LOG_HERALD, "new sample rate (closest): %d -> %d",
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
        if( useDefaultIn )
        {
            // get default input device
            // FYI getDefaultInputDevice returns 0 either as a valid index OR if no default/any devices
            m_adc_n = m_rtaudio->getDefaultInputDevice();
            // ensure correct channel count if default device is requested
            RtAudio::DeviceInfo device_info = m_rtaudio->getDeviceInfo((unsigned int)m_adc_n);

            // remember
            t_CKUINT inChannelsRequested = m_num_channels_in;

            // check if input channels > 0 || sample rate does not match
            if( device_info.inputChannels < m_num_channels_in )
            {
                // 1.4.2.0 (ge) | added: a specific but possible common case
                // for systems that have a 1-channel default input audio device -- e.g., some MacOS systems
                if( m_num_channels_in == 2 && device_info.inputChannels == 1
                    && supportSampleRate( device_info, sample_rate ) ) // try to stay on device if possible
                {
                    // nothing more for now here
                    // the code here to set the m_expand_in_mono2stereo flag has been moved below
                }
                else // need further matching
                {
                    // special case | 1.5.0.0 (ge) -- in this case of default input device,
                    // first try to match using the current default input device channels.
                    // This is to handle the situation in macOS and using bluetooth earbuds
                    // or headphones and the highest supported input sample rate is lower than
                    // the desired chuck sample rate (e.g., some input devices only support 24K);
                    // in this case, look for devices with match sample rates AND inherit the
                    // default input devices's actual channel count. This heuristic is so that
                    // on most Apple laptops since macOS 11, this will end up choosing the
                    // the onboard laptop microphone. This will hopefully serve many students
                    // working in the above setup; of course, power users can explicitly choose
                    // the input and output devices using command-line flags or other settings
                    // -
                    // special case | 1.4.0.1 (ge) -- check if the sample rate is supported
                    // this is to catch the case where the default device has
                    // insufficient channels (e.g., 1 on MacOS), finds secondary device
                    // (e.g., ZoomAudioDevice on MacOS) that has enough channels
                    // but does not support the desired sample rate
                    // amended 1.4.2.0 -- the behavior is modified above, where chuck
                    // will first check if the default input device has only 1 channel
                    // while we are requesting 2 channels...
                    // -

                    // if non-zero input channels
                    if( device_info.inputChannels > 0 )
                    {
                        // find a device with *exactly* the current device input channels
                        m_adc_n = findMatchingInputDevice( m_rtaudio, device_info.inputChannels, sample_rate, FALSE );
                    }
                    else  // if device input channel count is 0
                    {
                        // find a device with *exactly* the requested channel count
                        m_adc_n = findMatchingInputDevice( m_rtaudio, m_num_channels_in, sample_rate, FALSE);
                    }
                    
                    // check if match
                    if( m_adc_n >= 0 )
                    {
                        // get updated device info (since m_adc_n may be updated by this point)
                        device_info = m_rtaudio->getDeviceInfo((unsigned int)m_adc_n);
                        // update the request input channel request
                        m_num_channels_in = device_info.inputChannels;
                    }
                    else
                    {
                        // find a device with *at least* the requested channel count
                        m_adc_n = findMatchingInputDevice(m_rtaudio, m_num_channels_in, sample_rate, TRUE );

                        // changed 1.3.1.2 (ge): for input, if no match found by this point, we just gonna try to open half-duplex
                        if( m_adc_n < 0 )
                        {
                            // set to 0
                            m_num_channels_in = 0;
                            // log
                            EM_log( CK_LOG_HERALD, "cannot find compatible audio input device..." );
                            EM_pushlog();
                                EM_log(CK_LOG_HERALD, "defaulting to half-duplex (no audio input)...");
                            EM_poplog();

                            // problem finding audio devices, most likely
                            // EM_error2( 0, "unable to find audio input device with requested channel count (%i)...",
                            //               m_num_channels_in);
                            // clean up
                            // goto error;
                        }
                    }
                }
            }

            // if we ended up with a match
            if( m_dac_n >= 0 )
            {
                // get info for the input device we ended up using
                device_info = m_rtaudio->getDeviceInfo((unsigned int)m_adc_n);
                // check channel situation look for specific arrangement; we should have a sample rate match if we get here
                if( inChannelsRequested == 2 && device_info.inputChannels == 1 )
                {
                    // flag this for expansion in the callback
                    m_expand_in_mono2stereo = TRUE;
                    // log
                    EM_log( CK_LOG_INFO, "using 1-channel default input audio device (instead of requested 2-channel)..." );
                    EM_log( CK_LOG_INFO, "simulating stereo input from mono audio device..." );
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
    { // was: try {
        // log
        EM_log( CK_LOG_FINE, "[%s] driver trying %d input %d output...",
                m_driver_name.c_str(), m_num_channels_in, m_num_channels_out );

        RtAudio::StreamParameters output_parameters;
        output_parameters.deviceId = (unsigned int)m_dac_n;
        output_parameters.nChannels = (unsigned int)m_num_channels_out;
        output_parameters.firstChannel = 0;
        
        RtAudio::StreamParameters input_parameters;
        input_parameters.deviceId = (unsigned int)m_adc_n;
        input_parameters.nChannels = (unsigned int)(m_expand_in_mono2stereo ? 1 : m_num_channels_in);
        input_parameters.firstChannel = 0;
        
        RtAudio::StreamOptions stream_options;
        stream_options.flags = 0;
        stream_options.numberOfBuffers = (unsigned int)num_buffers;
        stream_options.streamName = "ChucK";
        stream_options.priority = 0;
            
        // open audio stream
        code = m_rtaudio->openStream(
            m_num_channels_out > 0 ? &output_parameters : NULL,
            m_num_channels_in > 0 ? &input_parameters : NULL,
            CK_RTAUDIO_FORMAT, (unsigned int)sample_rate, &bufsize,
            cb, m_cb_user_data,
            &stream_options
        );

        // check error code
        if( code > RTAUDIO_WARNING ) // was: } catch( RtAudioErrorType ) {
        {
            // RtAudio no longer throws, errors reported via errorHandler
            CK_SAFE_DELETE( m_rtaudio );
            // clean up
            goto error;
        }
    }
        
    // check bufsize
    if( bufsize != (int)m_buffer_size )
    {
        EM_log( CK_LOG_HERALD, "new buffer size: %d -> %i", m_buffer_size, bufsize );
        m_buffer_size = bufsize;
    }

    // the dac and adc devices we ended up initializing | 1.5.0.0 (ge)
    if( m_num_channels_out > 0 )
        dac_info = m_rtaudio->getDeviceInfo((unsigned int)m_dac_n);
    if( m_num_channels_in > 0 )
        adc_info = m_rtaudio->getDeviceInfo((unsigned int)m_adc_n);
    // copy names
    m_dac_name = dac_info.name;
    m_adc_name = adc_info.name;

    // pop indent
    EM_poplog();

    // greater of dac/adc channels
    m_num_channels_max = num_dac_channels > num_adc_channels ?
                         num_dac_channels : num_adc_channels;
    // log
    EM_log( CK_LOG_HERALD, "allocating buffers for %d x %d samples...",
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
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
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
    catch( RtAudioErrorType )
    {
        // RtAudio no longer throws, case shouldn't happen
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
    catch( RtAudioErrorType)
    {
        // RtAudio no longer throws, case shouldn't happen
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
void ChuckAudio::shutdown( t_CKUINT msWait )
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

        // wait for thread to wrap up | 1.5.1.3 (ge) added
        if( msWait > 0 ) ck_usleep( msWait * 1000 );
    }

    // close stream
    m_rtaudio->closeStream();
    // cleanup
    CK_SAFE_DELETE( m_rtaudio );

    // deallocate | 1.5.0.0 (ge) added
    CK_SAFE_DELETE( m_buffer_in );
    CK_SAFE_DELETE( m_buffer_out );

    // unflag
    m_init = FALSE;
    m_start = FALSE;
}
