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

#include <string>
using namespace std;

#if (defined(__PLATFORM_WINDOWS__) && !defined(__WINDOWS_PTHREAD__))
  #include <windows.h>
  #include <sys/timeb.h>
#else
  #include <unistd.h>
  #include <sys/time.h>
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
// t_CKUINT ChuckAudio::m_num_channels_max = CK_NUM_CHANNELS_DEFAULT;
t_CKUINT ChuckAudio::m_sample_rate = CK_SAMPLE_RATE_DEFAULT;
t_CKUINT ChuckAudio::m_frame_size = CK_BUFFER_SIZE_DEFAULT;
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
ck_f_audio_cb ChuckAudio::m_audio_cb = NULL;
// void * ChuckAudio::m_cb_user_data = NULL;
ma_context ChuckAudio::m_context;
ma_device ChuckAudio::m_device;




//-----------------------------------------------------------------------------
// global symbols expected from RtAudio | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
//extern "C" const unsigned int audio_num_compiled_apis;
//extern "C" const unsigned int audio_compiled_apis[];
//extern "C" const char * audio_api_names[][2];

// function prototype
static bool open_context( const char * driver, ma_context & pContext );




//-----------------------------------------------------------------------------
// name: driverNameToApi()
// desc: look up from driver name to API/driver
//-----------------------------------------------------------------------------
static t_CKUINT driverNameToApi( const char * driver )
{
    t_CKUINT api = ma_backend_null;
    if( driver )
    {
        // TODO: add more supported backends?

        // get lowercase version
        std::string driverLower = ::tolower(::trim(driver));
        // from what is available, match by driver name
    #if defined(__WINDOWS_ASIO__)
        if( driverLower == "asio" )
            api = ma_backend_custom; // TODO: backend
    #endif
    #if defined(__WINDOWS_DS__)
        if( driverLower == "ds" || driverLower == "directsound" || driverLower == "dsound" )
            api = ma_backend_dsound;
    #endif
    #if defined(__WINDOWS_WASAPI__)
        if( driverLower == "wasapi" )
            api = ma_backend_wasapi;
    #endif
    #if defined(__LINUX_ALSA__)
        if( driverLower == "alsa" )
            api = ma_backend_alsa;
    #endif
    #if defined(__LINUX_PULSE__)
        if( driverLower == "pulse" || driverLower == "pulseaudio")
            api = ma_backend_pulseaudio;
    #endif
    #if defined(__LINUX_OSS__)
        if( driverLower == "oss" )
            api = ma_backend_oss;
    #endif
    #if defined(__UNIX_JACK__)
        if( driverLower == "jack" )
            api = ma_backend_jack;
    #endif
    #if defined(__MACOSX_CORE__)
        if( driverLower == "coreaudio" || driverLower == "core" || driverLower == "core audio" )
            api = ma_backend_coreaudio;
    #endif

        // check for Dummy
        if( driverLower == "(dummy)" )
            api = ma_backend_custom; // TODO

        // XXX: add swap between ALSA, PULSE, OSS? and JACK
        if( api == ma_backend_null )
        {
            EM_error2( 0, "unsupported audio driver: %s", driver );
        }
    }

    // 1.5.0.0 (nshaheed) if the audio driver is UNSPECIFIED then
    // return a default driver. This will depend on OS and, in the
    // case of linux, which drivers chuck is being compiled with.
    if( api == ma_backend_null )
    {
    #if defined(__PLATFORM_WINDOWS__)
        // traditional chuck default behavior:
        // DirectSound is most general albeit high-latency
        api = ma_backend_dsound;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_ALSA__)
        api = ma_backend_alsa;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_PULSE__)
        api = ma_backend_pulseaudio;
    #elif defined(__PLATFORM_LINUX__) && defined(__LINUX_OSS__)
        api = ma_backend_oss;
    #elif defined(__PLATFORM_LINUX__) && defined(__UNIX_JACK__)
        api = ma_backend_jack;
    #elif defined(__MACOSX_CORE__)
        api = ma_backend_coreaudio;
    #endif
    }

    return api;
}




//-----------------------------------------------------------------------------
// name: apiToDriverName()
// desc: get driver name from API
//-----------------------------------------------------------------------------
static const char * apiToDriverName( t_CKUINT api )
{
    return ma_get_backend_name( (ma_backend)api );
}




//-----------------------------------------------------------------------------
// name: ma_format_name()
// desc: retrive human-readable description of sample format
//-----------------------------------------------------------------------------
const char* ma_format_name(ma_format format) {
    switch (format) {
        case ma_format_unknown:  return "all";
        case ma_format_u8:  return "8-bit uint";
        case ma_format_s16: return "16-bit int";
        case ma_format_s24: return "24-bit int";
        case ma_format_s32: return "32-bit int";
        case ma_format_f32: return "32-bit float";
        default: return "unknown";
    }
}




//-----------------------------------------------------------------------------
// name: print()
// desc: print info for an audio device
//-----------------------------------------------------------------------------
void print( const ma_device_info * info, bool isOutput )
{
    EM_error2b( 0, "device name (%s) = \"%s\"", isOutput ? "output" : "input", info->name );
    bool probed = (info->nativeDataFormatCount > 0);
    if( !probed ) {
        EM_error2b( 0, "probe [failed]..." );
        return;
    }

    EM_error2b( 0, "probe [success]..." );
    EM_error2b( 0, "default (%s): %s", isOutput ? "output" : "input", info->isDefault ? "YES" : "NO");
    EM_error2b( 0, "natively supported formats: " );

    // data formats on device
    for( int i = 0; i < info->nativeDataFormatCount; ++i )
    {
        string s;
        // add data format
        s += ma_format_name(info->nativeDataFormats[i].format);
        s += ", channels: ";
        s += (info->nativeDataFormats[i].channels == 0) ? "(any)" : std::to_string(info->nativeDataFormats[i].channels);
        s += ", sample rate: ";

        // if set to 0, all sample rates are supported
        s += (info->nativeDataFormats[i].sampleRate == 0) ? "(any)" : std::to_string(info->nativeDataFormats[i].sampleRate) + " Hz";
    }
}




//-----------------------------------------------------------------------------
// name: probe()
// desc: probe audio devices by driver
//-----------------------------------------------------------------------------
void ChuckAudio::probe( const char * driver )
{
    // the context
    ma_context context;
    // open context
    if( !open_context( driver, context ) ) return;

    // get backend name string
    const char * backend_name = apiToDriverName( context.backend );

    // output devices
    ma_device_info * output_infos = NULL;
    // number of audio output devices
    ma_uint32 output_count = 0;
    // input devices
    ma_device_info * input_infos = NULL;
    // number of audio input devices
    ma_uint32 input_count = 0;

    // get devices
    ma_result res = ma_context_get_devices( &context, &output_infos, &output_count, &input_infos, &input_count );
    if( res != MA_SUCCESS ) {
        EM_error2( 0, "(audio) probe error..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        goto done;
    }

    // print
    EM_error2b( 0, "[%s] driver found %d audio output device(s)...", TC::green(backend_name,TRUE).c_str(), output_count );
    EM_error2b( 0, "" );

    // loop over output devices
    for( ma_uint32 output_idx = 0; output_idx < output_count; output_idx++ )
    {
        // get device info pointer from array
        ma_device_info * device = output_infos + output_idx;
        // print
        EM_print2blue( "------( audio output device: %d )------\n", output_idx+1 );
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_playback, &device->id, device );
        // print the device info
        print( device, true );
    }

    // reset -- what does this do -- not sure, therefore let's keep it
    EM_reset_msg();

    // print
    EM_error2b( 0, "[%s] driver found %d audio input device(s)...", TC::green(backend_name,TRUE).c_str(), input_count );
    EM_error2b( 0, "" );

    // loop over output devices
    for( ma_uint32 input_idx = 0; input_idx < input_count; input_idx++ )
    {
        // get device info pointer from array
        ma_device_info* device = input_infos + input_idx;
        // print
        EM_print2blue( "------( audio input device: %d )------\n", input_idx+1 );
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_capture, &device->id, device );
        // print the device info
        print( device, false );
    }

done:
    // clean up the context
    ma_context_uninit(&context);
}




//-----------------------------------------------------------------------------
// name: open_context()
// desc: open an audio context
//-----------------------------------------------------------------------------
static bool open_context( const char * driver, ma_context & context )
{
    // get backend driver by name
    ma_backend backend = (ma_backend)driverNameToApi(driver);

    // check
    if( backend == ma_backend_null )
    {
        // output as error
        EM_error2( 0, "unrecognized driver name '%s'", driver ? driver : "[NULL]" );
        // done
        return false;
    }

    // context config
    ma_context_config config = ma_context_config_init();
    // set client name (useful for JACK only)
    config.jack.pClientName = "ChucK";
    // initialize context
    ma_result res = ma_context_init( &backend, 1, &config, &context );
    if( res != MA_SUCCESS )
    {
        EM_error2( 0, "(audio) initialization error..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        return false;
    }

    return true;
}




//-----------------------------------------------------------------------------
// name: device_named()
// desc: get device number by name; needs_dac/adc prompts further checks on
//       requested device having > 0 channels
//-----------------------------------------------------------------------------
t_CKINT ChuckAudio::output_device_named( const std::string & driver, const std::string & name )
{
    // the context
    ma_context context;
    // open context
    if( !open_context( driver.c_str(), context ) ) return -1;

    // output devices
    ma_device_info * output_infos = NULL;
    // number of audio output devices
    ma_uint32 output_count = 0;
    // input devices
    ma_device_info * input_infos_NO = NULL;
    // number of audio input devices
    ma_uint32 input_count_NO = 0;
    
    // the device number
    int device_no = -1;

    // get devices
    ma_result res = ma_context_get_devices( &context, &output_infos, &output_count, &input_infos_NO, &input_count_NO );
    if( res != MA_SUCCESS ) {
        EM_error2( 0, "(audio) error getting devices..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        goto done;
    }

    // loop over output devices
    for( ma_uint32 output_idx = 0; output_idx < output_count; output_idx++ )
    {
        // get device info pointer from array
        ma_device_info * device = output_infos + output_idx;
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_playback, &device->id, device );
        // c++ string
        string lhs = tolower(string(device->name));
        string rhs = tolower(name);
        // direct comparison
        if( lhs == rhs )
        {
            device_no = output_idx + 1;
            goto done;
        }
    }
    
    // loop over output devices
    for( ma_uint32 output_idx = 0; output_idx < output_count; output_idx++ )
    {
        // get device info pointer from array
        ma_device_info * device = output_infos + output_idx;
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_playback, &device->id, device );
        // c++ string
        string lhs = tolower(string(device->name));
        string rhs = tolower(name);
        // substring search
        if( lhs.find(rhs) != std::string::npos )
        {
            device_no = output_idx + 1;
            goto done;
        }
    }

done:
    // uninit
    ma_context_uninit( &context );

    // done
    return device_no;
}




//-----------------------------------------------------------------------------
// name: device_named()
// desc: get device number by name; needs_dac/adc prompts further checks on
//       requested device having > 0 channels
//-----------------------------------------------------------------------------
t_CKINT ChuckAudio::input_device_named( const std::string & driver, const std::string & name )
{
    // the context
    ma_context context;
    // open context
    if( !open_context( driver.c_str(), context ) ) return -1;

    // output devices
    ma_device_info * output_infos_NO = NULL;
    // number of audio output devices
    ma_uint32 output_count_NO = 0;
    // input devices
    ma_device_info * input_infos = NULL;
    // number of audio input devices
    ma_uint32 input_count = 0;

    // the device number
    int device_no = -1;

    // get devices
    ma_result res = ma_context_get_devices( &context, &output_infos_NO, &output_count_NO, &input_infos, &input_count );
    if( res != MA_SUCCESS ) {
        EM_error2( 0, "(audio) error getting devices..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        goto done;
    }

    // loop over input devices
    for( ma_uint32 input_idx = 0; input_idx < input_count; input_idx++ )
    {
        // get device info pointer from array
        ma_device_info * device = input_infos + input_idx;
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_playback, &device->id, device );
        // c++ string
        string lhs = tolower(string(device->name));
        string rhs = tolower(name);
        // direct comparison
        if( lhs == rhs )
        {
            device_no = input_idx + 1;
            goto done;
        }
    }
    
    // loop over output devices
    for( ma_uint32 input_idx = 0; input_idx < input_count; input_idx++ )
    {
        // get device info pointer from array
        ma_device_info * device = input_infos + input_idx;
        // retrieve device info
        ma_context_get_device_info( &context, ma_device_type_playback, &device->id, device );
        // c++ string
        string lhs = tolower(string(device->name));
        string rhs = tolower(name);
        // substring search
        if( lhs.find(rhs) != std::string::npos )
        {
            device_no = input_idx + 1;
            goto done;
        }
    }

done:
    // clean up context
    ma_context_uninit( &context );

    // done
    return device_no;
}




//-----------------------------------------------------------------------------
// name: defaultDriverApi()
// desc: get default audio driver number, i.e., RtAudio::Api enum
//-----------------------------------------------------------------------------
t_CKUINT ChuckAudio::defaultDriverApi()
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
t_CKUINT ChuckAudio::driverNameToApi( const char * driver )
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
    return apiToDriverName( num );
}




//-----------------------------------------------------------------------------
// name: numDrivers()
// desc: get number of compiled audio driver APIs
//-----------------------------------------------------------------------------
t_CKUINT ChuckAudio::numDrivers()
{
    // get enabled backends
    size_t enabledBackendCount = 0;
    ma_get_enabled_backends( NULL, MA_BACKEND_COUNT, &enabledBackendCount );

    // done
    return (t_CKUINT)enabledBackendCount;
}




//-----------------------------------------------------------------------------
// name: getDriver()
// desc: get info on a particular driver
//-----------------------------------------------------------------------------
//ChuckAudioDriverInfo ChuckAudio::getDriverInfo( t_CKUINT n )
//{
//    // check
//    if( n >= numDrivers() ) return ChuckAudioDriverInfo();
//
//    ChuckAudioDriverInfo info;
//    info.driver = rtaudio_compiled_apis[n];
//    info.name = rtaudio_api_names[info.driver][0];
//    info.userFriendlyName = rtaudio_api_names[info.driver][1];
//
//    return info;
//}




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
//static t_CKBOOL supportSampleRate( RtAudio::DeviceInfo & device_info, t_CKINT sample_rate )
//{
//    // loop over supported sample rates for a device
//    for( t_CKINT i = 0; i < device_info.sampleRates.size(); i++ )
//    {
//        // look for a match
//        if( device_info.sampleRates[i] == sample_rate )
//        {
//            return TRUE;
//        }
//    }
//
//    // no match
//    return FALSE;
//}




//-----------------------------------------------------------------------------
// name: findMatchingInputDevice()
// desc: find an input device with matching chans and sample rate
//       1.5.0.0 (ge) | added
//-----------------------------------------------------------------------------
//static t_CKINT findMatchingInputDevice( RtAudio * rtaudio, t_CKINT numInputChans,
//                                        t_CKINT sample_rate, t_CKBOOL allowMoreChans )
//{
//    // get number of devices
//    t_CKINT num_devices = rtaudio->getDeviceCount();
//    // struct to hold info for a device
//    RtAudio::DeviceInfo device_info;
//
//    // find first device with at least the requested channel count (and sample rate match)
//    for( t_CKUINT i = 0; i < num_devices; i++ )
//    {
//        // get device info
//        device_info = rtaudio->getDeviceInfo((unsigned int)i);
//        // check sample rate
//        if( supportSampleRate( device_info, sample_rate ) )
//        {
//            // check channels; the allowMoreChans toggles exact match or lenient match
//            if( (allowMoreChans == FALSE && device_info.inputChannels == numInputChans) ||
//                (allowMoreChans == TRUE && device_info.inputChannels >= numInputChans) )
//            {
//                // log
//                EM_log( CK_LOG_INFO, "found alternate input device: %d...", i );
//                // return device number
//                return i;
//            }
//        }
//    }
//
//    // no match
//    return -1;
//}




//-----------------------------------------------------------------------------
// name: device_support_channel_count()
// desc: test if a device supports a particular number of channels
//-----------------------------------------------------------------------------
static t_CKBOOL device_support_channel_count( ma_device_info * info, t_CKUINT channels )
{
    for( long i = 0; i < info->nativeDataFormatCount; i++ )
    {
        // support at least the desired # of channels
        if( info->nativeDataFormats[i].channels >= channels )
            return TRUE;
        // support "any" # of channels
        if( info->nativeDataFormats[i].channels == 0 )
            return TRUE;
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: ensureBuffers()
// desc: ensure that chuck audio buffers are big enough for a given frame size
//       if not, will re-allocate audio buffers
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::ensureBuffers( t_CKUINT frameSize )
{
    // check
    if( frameSize <= m_frame_size ) return TRUE;

    // update to max of old*2 vs new
    m_frame_size = ck_max(m_frame_size*2,frameSize);

    // greater of dac/adc channels, and no less than 2
    t_CKUINT num_channels_max = ck_min( ck_max( m_num_channels_out, m_num_channels_out ), 2 );

    // log
    EM_log( CK_LOG_HERALD, "allocating audio buffers for %d x %d samples...",
            m_frame_size, num_channels_max );

    // realloc
    m_buffer_out = (SAMPLE *)realloc( m_buffer_out, m_frame_size*num_channels_max*sizeof(SAMPLE) );
    m_buffer_in = (SAMPLE *)realloc( m_buffer_in, m_frame_size*num_channels_max*sizeof(SAMPLE) );

    // allocate buffers
    m_buffer_out = new SAMPLE[m_frame_size*num_channels_max];
    m_buffer_in = new SAMPLE[m_frame_size*num_channels_max];
    memset( m_buffer_out, 0, m_frame_size * sizeof(SAMPLE) * num_channels_max );
    memset( m_buffer_in, 0, m_frame_size * sizeof(SAMPLE) * num_channels_max );

    return TRUE;
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
                                 t_CKUINT frame_size,
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
    // zero out for now
    m_frame_size = 0;
    // not used in miniaudio backend
    m_num_buffers = num_buffers;
    m_start = 0;
    m_go = 0;
    m_silent = 0;
    
    // remember callback
    m_audio_cb = callback;
    // remember user data to pass to callback
    // m_cb_user_data = data;

    // open context
    if( !open_context( driver, m_context ) ) return FALSE;

    // get backend name string
    m_driver_name = apiToDriverName( m_context.backend );

    // output devices
    ma_device_info * output_infos = NULL;
    // number of audio output devices
    ma_uint32 output_count = 0;
    // input devices
    ma_device_info * input_infos = NULL;
    // number of audio input devices
    ma_uint32 input_count = 0;

    // default device indices
    t_CKINT default_output_device = -1;
    t_CKINT default_input_device = -1;

    // get devices
    ma_result res = ma_context_get_devices( &m_context, &output_infos, &output_count, &input_infos, &input_count );
    if( res != MA_SUCCESS ) {
        EM_error2( 0, "(audio) error enumerating devices..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        return FALSE;
    }

    // populate output device infos
    for( ma_uint32 output_idx = 0; output_idx < output_count; output_idx++ ) {
        ma_device_info * device = output_infos + output_idx;
        ma_context_get_device_info( &m_context, ma_device_type_playback, &device->id, device );
        if( device->isDefault ) default_output_device = output_idx;
    }

    // populate input device infos
    for( ma_uint32 input_idx = 0; input_idx < input_count; input_idx++) {
        ma_device_info* device = input_infos + input_idx;
        ma_context_get_device_info( &m_context, ma_device_type_capture, &device->id, device );
        if( device->isDefault ) default_input_device = input_idx;
    }

    // check output device number; 0 means "default"
    bool useDefaultOut = ( m_dac_n == 0 );
    // check input device number; 0 means "default"
    bool useDefaultIn = ( m_adc_n == 0 );
    // total number of audio devices (input and output)
    unsigned int num_devices = 0;

    // miniaudio parameters to be used later
    ma_result result;
    ma_device_config deviceConfig;
    ma_device_type flow;

    // log
    EM_log( CK_LOG_FINE, "initializing real-time audio..." );
    // push indent
    EM_pushlog();

    // convert 1-based ordinal to 0-based ordinal (added 1.3.0.0)
    // note: this is to preserve previous devices numbering after RtAudio change
    if( m_num_channels_out > 0 )
    {
        // ensure correct channel count if default device is requested
        if( useDefaultOut )
        {
            // check for output (channels)
            if( !device_support_channel_count( output_infos + default_output_device, m_num_channels_out ) )
            {
                // find first device with at least the requested channel count
                m_dac_n = -1;
                for( long i = 0; i < output_count; i++ )
                {
                    if( device_support_channel_count( output_infos + i, m_num_channels_out ) )
                    {
                        // turn off default flag since we are no longer using the default output device
                        useDefaultOut = FALSE;
                        // set new output device index
                        m_dac_n = i;
                        // done
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
            else
            {
                // set device to default output device index
                m_dac_n = default_output_device;
            }
        }
        else
        {
            // offset to 0-index device number
            m_dac_n--;
        }
    }
        
    // convert 1-based ordinal to 0-based ordinal
    if( m_num_channels_in > 0 )
    {
        // if we are requesting the default input device
        if( useDefaultIn )
        {
            // check for input (channels)
            if( !device_support_channel_count( input_infos + default_input_device, m_num_channels_in ) )
            {
                // special case: for systems with 1-channel default input audio device -- e.g., some MacOS laptops
                if( m_num_channels_in == 2 && device_support_channel_count( input_infos + default_input_device, 1 ) )
                {
                    // it's okay to use default; miniaudio should be able to open mono input device using 2 channels
                }
                else // need further matching
                {
                    // find first device with at least the requested channel count
                    m_adc_n = -1;
                    for( long i = 0; i < input_count; i++ )
                    {
                        // check device support
                        if( device_support_channel_count( input_infos + i, m_num_channels_in ) )
                        {
                            // turn off default flag since we are no longer using the default input device
                            useDefaultIn = FALSE;
                            // set device number
                            m_adc_n = i;
                            // done
                            break;
                        }
                    }

                    // for input, if no match found by this point, we just gonna try to open half-duplex
                    if( m_adc_n < 0 )
                    {
                        // set to 0
                        m_num_channels_in = 0;
                        // log
                        EM_log( CK_LOG_HERALD, "cannot find compatible audio input device..." );
                        EM_pushlog();
                            EM_log(CK_LOG_HERALD, "defaulting to half-duplex (no audio input)...");
                        EM_poplog();
                    }
                }
            }
            else
            {
                // set device to default input device index
                m_adc_n = default_input_device;
            }
        }
        else
        {
            // offset to 0-index device number
            m_adc_n--;
        }
    }
    
    // the dac and adc devices we've chosen to initialize | 1.5.0.0 (ge)
    if( m_num_channels_out > 0 && m_dac_n < num_devices )
    {
        // check we have a valid output device number
        assert( m_dac_n >= 0 );
        // get the device info
        ma_device_info * dac_info = output_infos + m_dac_n;
        // copy names (if valid)
        m_dac_name = dac_info->name;
    }
    if( m_num_channels_in > 0 && m_adc_n < num_devices )
    {
        // check we have a valid input device number
        assert( m_adc_n >= 0 );
        // get the device info
        ma_device_info * adc_info = input_infos + m_adc_n;
        // copy names (if valid)
        m_adc_name = adc_info->name;
    }

    // log
    EM_log( CK_LOG_FINE, "[%s] driver opening %d input %d output...",
            m_driver_name.c_str(), m_num_channels_in, m_num_channels_out );

    // how to open device; default to full duplex
    flow = ma_device_type_duplex;
    // check for playback only
    if( m_num_channels_out && !m_num_channels_in ) flow = ma_device_type_playback;
    // check for capture only
    else if( !m_num_channels_out && m_num_channels_in ) flow = ma_device_type_capture;

    // initialize device config
    deviceConfig = ma_device_config_init( flow );
    // IMPORTANT: if NULL...
    // 1) this will open to the default outut device AND
    // 2) will attempt to track if user switches system default output device
    // TODO: see if this a problem for channels > 2 (e.g., laptop orchestra hemis)
    deviceConfig.playback.pDeviceID  = useDefaultOut ? NULL : &((output_infos+m_dac_n)->id);
    // use 32-bit float; (there is no option currently for 64-bit float)
    deviceConfig.playback.format     = ma_format_f32;
    // # of outuput channels; should be verified by this point
    deviceConfig.playback.channels   = (ma_uint32) m_num_channels_out;
    // (see playback above)
    deviceConfig.capture.pDeviceID   = useDefaultIn ? NULL : &((input_infos+m_adc_n)->id);
    // same format as output
    deviceConfig.capture.format      = deviceConfig.playback.format;
    // # of input channels
    deviceConfig.capture.channels    = (ma_uint32) m_num_channels_in;
    // the audio callback function pointer
    deviceConfig.dataCallback       = cb;
    // system sample rate (miniaudio could implicitly convert to match this sample rate)
    deviceConfig.sampleRate         = (ma_uint32)sample_rate;
    // I/O buffer size (this is a hint to miniaudio; actual frame sizes could vary)
    deviceConfig.periodSizeInFrames = (ma_uint32)frame_size;

    // initialize the device
    result = ma_device_init( &m_context, &deviceConfig, &m_device );
    if( result != MA_SUCCESS ) {
        // print error
        EM_error2( 0, "(audio) error initializing device..." );
        EM_error2( 0, "(audio) |- devices output: '%d:%s' input: '%d:%s'", m_dac_n+1, m_dac_name.c_str(), m_adc_n+1, m_adc_name.c_str() );
        EM_error2( 0, "(audio) |- channels out: '%d' input: '%d'", m_num_channels_out, m_num_channels_in );
        EM_error2( 0, "(audio) |- sample rate: '%d'", m_sample_rate );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( result ) );
    }

    // only if m_num_channels_in is set to 1
    if( m_num_channels_in == 1 )
    {
        // flag this for expansion in the callback
        m_expand_in_mono2stereo = TRUE;
        // log
        EM_log( CK_LOG_INFO, "using 1-channel audio input; will copy to stereo..." );
    }

    // pop indent
    EM_poplog();

    // ensure buffers
    if( !ensureBuffers( frame_size ) )
    {
        // print error
        EM_error2( 0, "(audio) cannot allocate buffers..." );
        EM_error2( 0, "(audio) |- out of memory (framesize: %d)", frame_size );
        goto error;
    }

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
// name: cb()
// desc: callback function from the underlying audio backend
//-----------------------------------------------------------------------------
void ChuckAudio::cb( ma_device * pDevice, void * pOutput,
                     const void * pInput, ma_uint32 frameSize )
{
    // check and if needed re-alloc audio buffers
    ensureBuffers( frameSize );

    // length, in bytes of output
    t_CKUINT len = frameSize * sizeof(SAMPLE) * m_num_channels_out;
    
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
        // set audio thread priority; NOTE: disabled as miniaudio seems to handle this
        // XThreadUtil::set_priority( XThreadUtil::our_priority );

        // zero out the first output buffer
        memset( pOutput, 0, frameSize * sizeof(SAMPLE32) * m_num_channels_out );

        // set flag
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
        return;
    }

    // we are expecting 32-bit float ALWAYS, regardless of __CHUCK_USE_64_BIT_SAMPLE__
    SAMPLE32 * theInput = (SAMPLE32 *)pInput;
    SAMPLE32 * theOutput = (SAMPLE32 *)pOutput;

    // copy input to local buffer
    if( m_num_channels_in )
    {
        // added 1.4.0.1 (ge) -- special case in=1; out=2
        if( m_expand_in_mono2stereo )
        {
            for( int i = 0; i < frameSize; i++ )
            {
                // copy mono sample into stereo buffer
                m_buffer_in[i*2] = m_buffer_in[i*2+1] = (SAMPLE)theInput[i];
            }
        }
        else
        {
#ifdef __CHUCK_USE_64_BIT_SAMPLE__
            for( t_CKUINT i = 0; i < frameSize*m_num_channels_in; i++ ) {
                // copy next sample, converting 32-bit to 64-bit
                m_buffer_in[i] = (SAMPLE)theInput[i];
            }
#else
            // assume 32-bit
            memcpy( m_buffer_in, theInput, len );
#endif

            // copy to extern
            if( m_extern_in ) memcpy( m_extern_in, m_buffer_in, len );
        }
    }

    // timestamp
    if( g_do_watchdog ) g_watchdog_time = get_current_time( TRUE );

    // call the audio cb (in which the chuck VM will run and fill the output buffer)
    m_audio_cb( m_buffer_in, m_buffer_out, frameSize,
        m_num_channels_in, m_num_channels_out, NULL );

    // copy local buffer to be rendered
    // REFACTOR-2017: TODO: m_end was the signal to end, what should it be now?
    if( m_start ) // REFACTOR-2017: now using m_start to gate this
    {
#ifdef __CHUCK_USE_64_BIT_SAMPLE__
            for( t_CKUINT i = 0; i < frameSize*m_num_channels_out; i++ ) {
                // copy next sample, converting 64-bit to 32-bit
                theOutput[i] = (SAMPLE32)m_buffer_out[i];
            }
#else
            // assume 32-bit
            memcpy( theOutput, m_buffer_out, len );
#endif

    }
    // set all elements of local buffer to silence
    else
    {
        // zero out; NOTE: explicitly use SAMPLE32 here
        memset( theOutput, 0, frameSize * sizeof(SAMPLE32) * m_num_channels_out );
    }

    // copy to extern
    if( m_extern_out ) memcpy( m_extern_out, m_buffer_out, len );
}




//-----------------------------------------------------------------------------
// name: start()
// desc: start host audio
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::start( )
{
    // check if already started
    if( m_start ) return TRUE;

    // start device
    ma_result res = ma_device_start( &m_device );
    // check
    if( res != MA_SUCCESS )
    {
        EM_error2( 0, "(audio) cannot start device..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        EM_error2( 0, "(audio) |- devices output: '%d:%s' input: '%d:%s'", m_dac_n+1, m_dac_name.c_str(), m_adc_n+1, m_adc_name.c_str() );
        EM_error2( 0, "(audio) |- channels out: '%d' input: '%d'", m_num_channels_out, m_num_channels_in );
        EM_error2( 0, "(audio) |- sample rate: '%d'", m_sample_rate );
        return FALSE;
    }

    // set to true
    m_start = TRUE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: stop host audio
//-----------------------------------------------------------------------------
t_CKBOOL ChuckAudio::stop( )
{
    // check if already started
    if( !m_start ) return TRUE;

    // start device
    ma_result res = ma_device_stop( &m_device );
    // check
    if( res != MA_SUCCESS )
    {
        EM_error2( 0, "(audio) cannot stop device..." );
        EM_error2( 0, "(audio) |- '%s'", ma_result_description( res ) );
        EM_error2( 0, "(audio) |- devices output: '%d:%s' input: '%d:%s'", m_dac_n+1, m_dac_name.c_str(), m_adc_n+1, m_adc_name.c_str() );
        EM_error2( 0, "(audio) |- channels out: '%d' input: '%d'", m_num_channels_out, m_num_channels_in );
        EM_error2( 0, "(audio) |- sample rate: '%d'", m_sample_rate );
        return FALSE;
    }

    // REFACTOR-2017: set end flag
    m_silent = TRUE;
    // set to true
    m_start = FALSE;

    return TRUE;
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
        // stop the audio
        stop();

        // wait for thread to wrap up | 1.5.1.3 (ge) added
        if( msWait > 0 ) ck_usleep( msWait * 1000 );
    }

    // clean up device
    ma_device_uninit( &m_device );
    // clean up context
    ma_context_uninit( &m_context );
    // zero out structs
    CK_ZERO_STRUCT( m_device );
    CK_ZERO_STRUCT( m_context );

    // deallocate | 1.5.0.0 (ge) added
    CK_SAFE_FREE( m_buffer_in );
    CK_SAFE_FREE( m_buffer_out );

    // unflag
    m_init = FALSE;
    m_start = FALSE;
}
