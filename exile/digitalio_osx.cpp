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
// file: digitalio_osx.cpp
// desc: digitalio for osx
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ari Lazier (alazier@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "digitalio_osx.h"
#include <iostream>
using namespace std;




// static
BOOL__ Digitalio::m_init = FALSE;
DWORD__ Digitalio::m_num_channels = NUM_CHANNELS_DEFAULT;
DWORD__ Digitalio::m_sampling_rate = SAMPLING_RATE_DEFAULT;
DWORD__ Digitalio::m_bps = BITS_PER_SAMPLE_DEFAULT;
DWORD__ Digitalio::m_buffer_size = BUFFER_SIZE_DEFAULT;
DWORD__ Digitalio::m_num_buffers = NUM_BUFFERS_DEFAULT;




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::initialize( DWORD__ num_channels, DWORD__ sampling_rate,
                              DWORD__ bps, DWORD__ buffer_size, DWORD__ num_buffers )
{
    if( m_init )
        return FALSE;

    m_num_channels = num_channels;
    m_sampling_rate = sampling_rate;
    m_bps = bps;
    m_buffer_size = buffer_size;
    m_num_buffers = num_buffers;
    
    return m_init = TRUE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void Digitalio::shutdown()
{
    m_init = FALSE;
}




//-----------------------------------------------------------------------------
// name: DigitalOut()
// desc: ...
//-----------------------------------------------------------------------------
DigitalOut::DigitalOut()
{
    m_init_out = FALSE;
    m_render_start = FALSE;
    
    m_num_channels = 0;
    m_sampling_rate = 0;
    m_bps = 0;
    
    m_device_handle = kAudioDeviceUnknown;
    m_device_num = 0;
    
    m_out_buffers = NULL;
    m_num_out_buffers = 0;
    m_out_buffer_size = 0;
    m_curr_buffer_out = 0;
    
    m_data_ptr_out = NULL;
    m_data_max_out = NULL;
    m_index = 0;
    
    pthread_mutex_init( &m_mutex, NULL );
    m_block = 0;
    m_out2 = 0;
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
// name: callback_handler_output()
// desc: ...
//-----------------------------------------------------------------------------
OSStatus callback_handler_output( AudioDeviceID inDevice,
                                  const AudioTimeStamp* inNow,
                                  const AudioBufferList* inInputData,
                                  const AudioTimeStamp* inInputTime,
                                  AudioBufferList* outOutputData,
                                  const AudioTimeStamp* inOutputTime, 
                                  void* infoPointer )
{    
    DigitalOut * out = (DigitalOut *)infoPointer;

    // cerr << ".";

    if( !out->render( outOutputData ) )
        return kAudioHardwareUnspecifiedError;
    
    return kAudioHardwareNoError;
}


                         
//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize audio out
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::initialize( DWORD__ device_number, DWORD__ num_channels,
                               DWORD__ sampling_rate, DWORD__ bps, 
                               DWORD__ buffer_size, DWORD__ num_buffers )
{
    char msg[1024];
    OSStatus err = noErr;
    UInt32 dataSize = 0;
    UInt32 the_buffer_size = 0;
    AudioStreamBasicDescription format;
    AudioBufferList * buffer_list = NULL;

    // check if already    
    if( m_init_out )
    {
        sprintf( msg, "error: initialize(); DigitalOut already initialized..." );
        goto error;
    }
    
    // set the parameters
    m_num_channels = num_channels;
    m_sampling_rate = sampling_rate;
    m_bps = bps;
    m_out_buffer_size = buffer_size;
    m_num_out_buffers = num_buffers;
    m_device_num = device_number;
    
    // allocate the buffers
    m_out_buffers = new InternalBuffer[m_num_out_buffers];
    for( int i = 0; i < m_num_out_buffers; i++ )
        m_out_buffers[i].initialize( m_out_buffer_size * sizeof(SAMPLE_INTERNAL) * 
                                     m_num_channels );

    m_curr_buffer_out = 0;
    m_data_ptr_out = m_out_buffers[0].data();
    m_data_max_out = m_data_ptr_out + m_out_buffer_size * m_num_channels; 
    m_index = 0;

    // get the default device
    dataSize = sizeof( AudioDeviceID );
    err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice,
                                    &dataSize, &m_device_handle );
    if( err != noErr )
    {
        sprintf( msg, "error opening output device device..." );
        goto error;
    }

    // configure the device
    err = AudioDeviceGetPropertyInfo( m_device_handle, 0, FALSE,
                                    kAudioDevicePropertyStreamConfiguration,
                                    &dataSize, NULL );
    if( err != noErr || dataSize <= 0 )
    {
        sprintf( msg, "error configuring audio device..." );
        goto error;
    }
    buffer_list = (AudioBufferList *)malloc(dataSize);
    err = AudioDeviceGetProperty( m_device_handle, 0, FALSE,
                                  kAudioDevicePropertyStreamConfiguration,
                                  &dataSize, buffer_list );
    if( err != noErr )
    {
        sprintf( msg, "error configuring audio device..." );
        goto error;     
    }

    AudioValueRange buffer_range;
    dataSize = sizeof(AudioValueRange);
    err = AudioDeviceGetProperty( m_device_handle, 0, FALSE,
                                  kAudioDevicePropertyBufferSizeRange,
                                  &dataSize, &buffer_range);
    if( err != noErr )
    {
        sprintf( msg, "error dealing with range" );
        goto error;
    }
    
    the_buffer_size = m_out_buffer_size * m_num_channels * sizeof(SAMPLE_INTERNAL);
    
    // set the buffer size
    dataSize = sizeof(UInt32);
    err = AudioDeviceSetProperty( m_device_handle, NULL, 0, FALSE,
                                  kAudioDevicePropertyBufferSize,
                                  dataSize, &the_buffer_size );
    if( err != noErr )
    {
        sprintf( msg, "error setting buffer size '%d'", the_buffer_size );
        goto error;
    }
    
    // set the sample rate
    dataSize = sizeof(AudioStreamBasicDescription);
    err = AudioDeviceGetProperty( m_device_handle, 0, FALSE,
                                  kAudioDevicePropertyStreamFormat,
                                  &dataSize, &format );
    if( err != noErr )
    {
        sprintf( msg, "error setting sampling rate..." );
        goto error;
    }
    
    format.mSampleRate = m_sampling_rate;
    format.mFormatID = kAudioFormatLinearPCM;
    err = AudioDeviceSetProperty( m_device_handle, NULL, 0, FALSE,
                                  kAudioDevicePropertyStreamFormat,
                                  dataSize, &format );
    if( err != noErr )
    {
        sprintf( msg, "error setting sampling rate '%d'", m_sampling_rate );
        goto error;
    }
    
    // register callback handler
    err = AudioDeviceAddIOProc( m_device_handle, callback_handler_output, this );
    if( err != noErr )
    {
        sprintf( msg, "error setting callback" );
        goto error;
    }

    // set the flag
    m_init_out = TRUE;
    
    return TRUE;

error:
    // print out the error
    cerr << msg << endl;
    
    // clean
    this->cleanup();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::start()
{
    int err;
    char msg[1024];

    pthread_mutex_lock( &m_mutex );

    if( !m_init_out )
    {
        sprintf( msg, "error: start(): DigitalOut not initialized..." );
        goto error;
    }

    // check to see if started already
    if( m_render_start )
    {
        sprintf( msg, "error: start(): DigitalOut already started..." );
        goto error;
    }
    
    // go
    err = AudioDeviceStart( m_device_handle, callback_handler_output );
    if( err != noErr )
    {
        sprintf( msg, "error starting audio device..." );
        goto error;
    }
    
    m_block = 0;
    m_out2 = 0;

    // set the flag    
    m_render_start = TRUE;
    
    pthread_mutex_unlock( &m_mutex );
    return TRUE;
    
error:
    // print out the error
    cerr << msg << endl;

    pthread_mutex_unlock( &m_mutex );
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::stop()
{
    int err;
    char msg[1024];

    // check to see if started
    if( !m_render_start )
    {
        sprintf( msg, "error: stop(): DigitalOut not started..." );
        goto error;
    }
    
    pthread_mutex_lock( &m_mutex );
    
    err = AudioDeviceStop( m_device_handle, callback_handler_output );
    if( err != noErr )
    {
        sprintf( msg, "error stopping audio device..." );
        goto error;
    }
    
    // set the flag
    m_render_start = FALSE;
    
    pthread_mutex_unlock( &m_mutex );
    return TRUE;

error:
    // print out the error
    cerr << msg << endl;

    pthread_mutex_unlock( &m_mutex );
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalOut::cleanup()
{    
    // check if started
    if( m_render_start )
        stop();


    
    // deallocate
    if( m_out_buffers )
        SAFE_DELETE_ARRAY( m_out_buffers );

    // set the flag    
    m_init_out = FALSE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: 1 channel
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::tick_out( SAMPLE sample )
{
    if( !prepare_tick_out() )
        return FALSE;

    *m_data_ptr_out++ = SAMPLE_TO_INTERNAL( sample );

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

    *m_data_ptr_out++ = SAMPLE_TO_INTERNAL( sample_l );
    *m_data_ptr_out++ = SAMPLE_TO_INTERNAL( sample_r );

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

    if( !n ) n = m_num_channels;
    while( n-- )
        *m_data_ptr_out++ = SAMPLE_TO_INTERNAL( *samples++ );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: prepare_tick_out()
// desc: data ptr ok
//-----------------------------------------------------------------------------
inline BOOL__ DigitalOut::prepare_tick_out()
{
    if( !m_init_out )
        return FALSE;

    if( m_data_ptr_out >= m_data_max_out )
    {
        if( !m_render_start && !start() )
            return FALSE;

        m_block++;
        while( m_block >= m_num_out_buffers )
            usleep( 5000 );

        m_curr_buffer_out = (m_curr_buffer_out + 1) % m_num_out_buffers;
        m_data_ptr_out = m_out_buffers[m_curr_buffer_out].data();
        m_data_max_out = m_data_ptr_out + m_out_buffer_size * m_num_channels;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render()
// desc: the render
//-----------------------------------------------------------------------------
DWORD__ DigitalOut::render( AudioBufferList * buffer_list )
{
    int err;
    char msg[1024] = "";

    while( !m_block )
        usleep( 5000 );
        
    // copy
    memcpy( buffer_list->mBuffers[0].mData,
            m_out_buffers[m_out2].data(),
            buffer_list->mBuffers[0].mDataByteSize );

    m_out2 = (m_out2 + 1) % m_num_out_buffers;
    
    m_block--;
    
    //cerr << ".";
    
    return TRUE;
    
error:

    cerr << msg << endl;
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: DigitalIn()
// desc: ...
//-----------------------------------------------------------------------------
DigitalIn::DigitalIn()
{
    m_init_in = FALSE;
    m_capture_start = FALSE;
    
    m_num_channels = 0;
    m_sampling_rate = 0;
    m_bps = 0;
    
    m_in_buffers = NULL;
    m_num_in_buffers = 0;
    m_in_buffer_size = 0;
    m_curr_buffer_in = 0;
    
    m_data_ptr_in = NULL;
    m_data_max_in = NULL;
    
    m_device_handle = kAudioDeviceUnknown;
    m_device_num = 0;
    
    pthread_mutex_init( &m_mutex, NULL );
    m_block = 0;
    m_in2 = 0;
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
// name: callback_handler_input()
// desc: ...
//-----------------------------------------------------------------------------
OSStatus callback_handler_input( AudioDeviceID inDevice,
                                 const AudioTimeStamp* inNow,
                                 const AudioBufferList* inInputData,
                                 const AudioTimeStamp* inInputTime,
                                 AudioBufferList* outOutputData,
                                 const AudioTimeStamp* inOutputTime, 
                                 void* infoPointer )
{    
    DigitalIn * in = (DigitalIn *)infoPointer;

    // cerr << "!";

    if( !in->capture( inInputData ) )
        return kAudioHardwareUnspecifiedError;
    
    return kAudioHardwareNoError;
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize audio in
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::initialize( DWORD__ device_number, DWORD__ num_channels, 
                              DWORD__ sampling_rate, DWORD__ bps, 
                              DWORD__ buffer_size, DWORD__ num_buffers )
{
    char msg[1024];
    OSStatus err = noErr;
    UInt32 dataSize = 0;
    UInt32 the_buffer_size = 0;
    AudioStreamBasicDescription format;
    AudioBufferList * buffer_list = NULL;

    // check if already    
    if( m_init_in )
    {
        sprintf( msg, "error: initialize(); DigitalIn already initialized..." );
        goto error;
    }
    
    // set the parameters
    m_num_channels = num_channels;
    m_sampling_rate = sampling_rate;
    m_bps = bps;
    m_in_buffer_size = buffer_size;
    m_num_in_buffers = num_buffers;
    m_device_num = device_number;
    
    // allocate the buffers
    m_in_buffers = new InternalBuffer[m_num_in_buffers];
    for( int i = 0; i < m_num_in_buffers; i++ )
        m_in_buffers[i].initialize( m_in_buffer_size * sizeof(SAMPLE_INTERNAL) * 
                                    m_num_channels );

    m_curr_buffer_in = 0;
    m_data_ptr_in = m_in_buffers[0].data();
    m_data_max_in = m_data_ptr_in + m_in_buffer_size * m_num_channels; 

    // get the default device
    dataSize = sizeof( AudioDeviceID );
    err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice,
                                    &dataSize, &m_device_handle );
    if( err != noErr )
    {
        sprintf( msg, "error opening input device device..." );
        goto error;
    }

    // configure the device
    err = AudioDeviceGetPropertyInfo( m_device_handle, 0, TRUE,
                                      kAudioDevicePropertyStreamConfiguration,
                                      &dataSize, NULL );
    if( err != noErr || dataSize <= 0 )
    {
        sprintf( msg, "error configuring audio device..." );
        goto error;
    }
    buffer_list = (AudioBufferList *)malloc(dataSize);
    err = AudioDeviceGetProperty( m_device_handle, 0, TRUE,
                                  kAudioDevicePropertyStreamConfiguration,
                                  &dataSize, buffer_list );
    if( err != noErr )
    {
        sprintf( msg, "error configuring audio device..." );
        goto error;     
    }

    AudioValueRange buffer_range;
    dataSize = sizeof(AudioValueRange);
    err = AudioDeviceGetProperty( m_device_handle, 0, TRUE,
                                  kAudioDevicePropertyBufferSizeRange,
                                  &dataSize, &buffer_range);
    if( err != noErr )
    {
        sprintf( msg, "error dealing with range" );
        goto error;
    }
    
    the_buffer_size = m_in_buffer_size * m_num_channels * sizeof(SAMPLE_INTERNAL);
    
    // set the buffer size
    dataSize = sizeof(UInt32);
    err = AudioDeviceSetProperty( m_device_handle, NULL, 0, TRUE,
                                  kAudioDevicePropertyBufferSize,
                                  dataSize, &the_buffer_size );
    if( err != noErr )
    {
        sprintf( msg, "error setting buffer size '%d'", the_buffer_size );
        goto error;
    }
    
    // set the sample rate
    dataSize = sizeof(AudioStreamBasicDescription);
    err = AudioDeviceGetProperty( m_device_handle, 0, TRUE,
                                  kAudioDevicePropertyStreamFormat,
                                  &dataSize, &format );
    if( err != noErr )
    {
        sprintf( msg, "error setting sampling rate..." );
        goto error;
    }
    
    format.mSampleRate = m_sampling_rate;
    format.mFormatID = kAudioFormatLinearPCM;
    err = AudioDeviceSetProperty( m_device_handle, NULL, 0, TRUE,
                                  kAudioDevicePropertyStreamFormat,
                                  dataSize, &format );
    if( err != noErr )
    {
        sprintf( msg, "error setting sampling rate '%d'", m_sampling_rate );
        goto error;
    }
    
    // register callback handler
    err = AudioDeviceAddIOProc( m_device_handle, callback_handler_input, this );
    if( err != noErr )
    {
        sprintf( msg, "error setting callback" );
        goto error;
    }
    
    // set the flag
    m_init_in = TRUE;
    
    return TRUE;

error:
    // print out the error
    cerr << msg << endl;
    
    // clean
    this->cleanup();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::start()
{
    int err;
    char msg[1024];

    // check if initialized
    if( !m_init_in )
    {
        sprintf( msg, "error: start(): DigitalIn not initialized..." );
        goto error;
    }
    
    // check if started already
    if( m_capture_start )
    {
        sprintf( msg, "error: start(): DigitalIn already started..." );
        goto error;
    }
    
    // go
    //err = AudioDeviceStart( m_device_handle, callback_handler_input );
    //if( err != noErr )
    //{
    //    sprintf( msg, "error starting audio device..." );
    //  goto error;
    //}
    
    m_block = 0;
    m_in2 = 1;
    
    // set the flag
    m_capture_start = TRUE;
    
    return TRUE;
    
error:
    // print out the error
    cerr << msg << endl;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: capture_stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::stop()
{
    int err;
    char msg[1024];
        
    // check to see if started
    if( !m_capture_start )
    {
        sprintf( msg, "error: stop(): DigitalIn not started..." );
        goto error;
    }
    
    err = AudioDeviceStop( m_device_handle, callback_handler_input );
    if( err != noErr )
    {
        sprintf( msg, "error stopping audio device..." );
        goto error;
    }
    
    // set the flag
    m_capture_start = FALSE;

error:
    // print out the error
    cerr << msg << endl;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalIn::cleanup()
{
    // check if started
    if( m_capture_start )
    {
        // stop
        stop();
    }
        
    // deallocate
    if( m_in_buffers )
        SAFE_DELETE_ARRAY( m_in_buffers );

    // set the flag    
    m_init_in = FALSE;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::tick_in( SAMPLE * s )
{
    if( !prepare_tick_in() )
        return 0;

    *s = INTERNAL_TO_SAMPLE( *m_data_ptr_in++ );

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

    *l = INTERNAL_TO_SAMPLE( *m_data_ptr_in++ );
    *r = INTERNAL_TO_SAMPLE( *m_data_ptr_in++ );

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

    if( !n ) n = m_num_channels;
    while( n-- )
        *sample++ = INTERNAL_TO_SAMPLE( *m_data_ptr_in++ );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: prepare_tick_in()
// desc: data ptr ok
//-----------------------------------------------------------------------------
inline BOOL__ DigitalIn::prepare_tick_in()
{
    if( !m_init_in )
        return FALSE;

    if( !m_capture_start ) if( !this->start() )
        return FALSE;

    if( m_data_ptr_in >= m_data_max_in )
    {
        while( !m_block )
            usleep( 5000 );

        //m_in_buffers[m_curr_buffer_in].clear();
        m_curr_buffer_in = (m_curr_buffer_in + 1) % m_num_in_buffers;
        m_data_ptr_in = m_in_buffers[m_curr_buffer_in].data();
        m_data_max_in = m_data_ptr_in + m_in_buffer_size * m_num_channels;
        
        m_block--;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: capture()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ DigitalIn::capture( const AudioBufferList * buffer_list )
{
    int err;
    char msg[1024] = "";
    
    while( m_block >= m_num_in_buffers )
        usleep( 5000 );

    // copy
    memcpy( m_in_buffers[m_in2].data(),
            buffer_list->mBuffers[0].mData,
            buffer_list->mBuffers[0].mDataByteSize );

    m_in2 = (m_in2 + 1) % m_num_in_buffers;
    
    m_block++;
    
    cerr << ".";
    
    return TRUE;
    
error:

    cerr << msg << endl;
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: AudioBuffer__()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer__::AudioBuffer__( DWORD__ size )
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
BOOL__ AudioBuffer__::initialize( DWORD__ size )
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
void AudioBuffer__::cleanup()
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
DWORD__ AudioBuffer__::size()
{
    return m_size;
}




//-----------------------------------------------------------------------------
// name: data()
// desc: ...
//-----------------------------------------------------------------------------
SAMPLE * AudioBuffer__::data()
{
    return m_data;
}




//-----------------------------------------------------------------------------
// name: AudioBuffer__In()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer__In::AudioBuffer__In( DWORD__ size )
    : AudioBuffer__( size )
{ }




//-----------------------------------------------------------------------------
// name: ~AudioBuffer__In()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer__In::~AudioBuffer__In()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBuffer__In::reset()
{
    m_ptr_curr = m_data;

    return TickIn::reset();
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBuffer__In::tick_in( SAMPLE * s )
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
BOOL__ AudioBuffer__In::tick_in( SAMPLE * l, SAMPLE * r )
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
BOOL__ AudioBuffer__In::tick_in( SAMPLE * in, DWORD__ n )
{
    if( !m_data || m_ptr_curr + n >= m_ptr_end )
        return FALSE;

    while( n-- )
        *in++ = *m_ptr_curr++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: AudioBuffer__Out()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer__Out::AudioBuffer__Out( DWORD__ size )
    : AudioBuffer__( size )
{ }




//-----------------------------------------------------------------------------
// name: ~AudioBuffer__Out()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer__Out::~AudioBuffer__Out()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBuffer__Out::reset()
{
    m_ptr_curr = m_data;

    return TickOut::reset();
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ AudioBuffer__Out::tick_out( SAMPLE s )
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
BOOL__ AudioBuffer__Out::tick_out( SAMPLE l, SAMPLE r )
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
BOOL__ AudioBuffer__Out::tick_out( const SAMPLE * out, DWORD__ n )
{
    if( !m_data || m_ptr_curr + n >= m_ptr_end )
        return FALSE;

    while( n-- )
        *m_ptr_curr++ = *out++;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: InternalBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
InternalBuffer::InternalBuffer()
{
    m_data = NULL;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: ~InternalBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
InternalBuffer::~InternalBuffer()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ InternalBuffer::initialize( DWORD__ buffer_size )
{
    this->cleanup();

    // allocate the buffer
    if( !(m_data = (SAMPLE_INTERNAL *)malloc(buffer_size)) )
        return FALSE;

    // set the buffer size
    m_size = buffer_size;

    // clear the buffer
    return this->clear();
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ InternalBuffer::clear()
{
    if( !m_data )
        return FALSE;

    // clear the memory
    memset( m_data, 0, m_size );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void InternalBuffer::cleanup()
{
    if( m_data )
    {
        free(m_data);
        m_data = NULL;
        m_size = 0;
    }    
}




//-----------------------------------------------------------------------------
// name: data()
// desc: get the buffer
//-----------------------------------------------------------------------------
SAMPLE_INTERNAL * InternalBuffer::data( )
{
    return m_data;
}




//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ InternalBuffer::size() const
{
    return m_size;
}




//-----------------------------------------------------------------------------
// name: synch()
// desc: wait for the buffer to be ready
//-----------------------------------------------------------------------------
void InternalBuffer::synch()
{
}




//-----------------------------------------------------------------------------
// name: signal()
// desc: signal the buffer to be used again
//-----------------------------------------------------------------------------
void InternalBuffer::signal()
{
}




//-----------------------------------------------------------------------------
// name: reset()
// desc:...
//-----------------------------------------------------------------------------
void InternalBuffer::reset()
{
}
