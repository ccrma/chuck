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
// file: digitalio_alsa.cpp
// desc: digitalio for alsa
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "digitalio_alsa.h"
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
                              DWORD__ bps, DWORD__ buffer_size,
                              DWORD__ num_buffers )
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
    
    m_device_handle = NULL;
    m_device_num = 0;
    
    m_out_buffers = NULL;
    m_num_out_buffers = 0;
    m_out_buffer_size = 0;
    m_curr_buffer_out = 0;
    
    m_data_ptr_out = NULL;
    m_data_max_out = NULL;
    m_index = 0;    
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
BOOL__ DigitalOut::initialize()
{
    char msg[1024];
    int err = 0;
    char name[256] = "";
    snd_pcm_hw_params_t * hw_params = NULL;
    unsigned int device_channels_max = 0, device_channels_min = 0;
    int dirs = 0;
    
    // check if already    
    if( m_init_out )
    {
        sprintf( msg, "error: initialize(); DigitalOut already initialized..." );
        goto error;
    }
    
    // set the parameters
    m_num_channels = Digitalio::m_num_channels;
    m_sampling_rate = Digitalio::m_sampling_rate;
    m_bps = Digitalio::m_bps;
    m_out_buffer_size = Digitalio::m_buffer_size;
    m_num_out_buffers = Digitalio::m_num_buffers;
    m_device_num = 0;
    
    // allocate the buffers
    m_out_buffers = new InternalBuffer[m_num_out_buffers];
    for( int i = 0; i < m_num_out_buffers; i++ )
        m_out_buffers[i].initialize( m_out_buffer_size * sizeof(SAMPLE_INTERNAL) * 
                                     m_num_channels );

    m_curr_buffer_out = 0;
    m_data_ptr_out = m_out_buffers[0].data();
    m_data_max_out = m_data_ptr_out + m_out_buffer_size * m_num_channels;
    m_index = 0;

    // device name
    sprintf( name, "hw:%d", m_device_num );

    // open the device
    err = snd_pcm_open( &m_device_handle, name, 
                        SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC );
    if( err < 0 )
    {
        sprintf( msg, "error opening device '%s' for playback: %s", name,
                 snd_strerror( err ) );
        goto error;
    }

    // allocate the hw params
    snd_pcm_hw_params_alloca( &hw_params );
    err = snd_pcm_hw_params_any( m_device_handle, hw_params );
    if( err < 0 )
    {
        sprintf( msg, "error getting parameters on device '%s': %s",
                 name, snd_strerror( err ) );
        goto error;
    }

    // set interleaved mode for access
    if( !snd_pcm_hw_params_test_access( m_device_handle, hw_params,
         SND_PCM_ACCESS_RW_INTERLEAVED ) )
        snd_pcm_hw_params_set_access( m_device_handle, hw_params,
            SND_PCM_ACCESS_RW_INTERLEAVED ); 
    else
    {
        sprintf( msg, "error: cannot set interleaved access\n" );
        goto error;
    }

    // test and set the format
    if( !snd_pcm_hw_params_test_format( m_device_handle, hw_params, 
                                        SND_PCM_FORMAT_S16 ) )
        snd_pcm_hw_params_set_format( m_device_handle, hw_params,
                                      SND_PCM_FORMAT_S16 );
    else
    {
        sprintf( msg, "error: cannot set format: signed 16-bit" );
        goto error;
    }
    if( err < 0 )
    {
        sprintf( msg, "error: cannot get channels min: %s",
                 snd_strerror( err ) );
        goto error;
    }

    // set the sample rate
    err = snd_pcm_hw_params_set_rate( m_device_handle, hw_params, 
                                      m_sampling_rate, 0 );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set sample rate %d: %s",
                 m_sampling_rate, snd_strerror( err ) );
        goto error;
    }

    // channels
    err = snd_pcm_hw_params_get_channels_min( hw_params, &device_channels_min );
    if( err < 0 ) {
        sprintf( msg, "error: cannot get channels min: %s", snd_strerror( err ) );
        goto error;
    }
    
    err = snd_pcm_hw_params_get_channels_max( hw_params, &device_channels_max );
    if( err < 0 ) {
        sprintf( msg, "error: cannot get channels max: %s", snd_strerror( err ) );
        goto error;
    }

    if( m_num_channels < device_channels_min )
        m_num_channels = device_channels_min;
    else if( m_num_channels > device_channels_max )
        m_num_channels = device_channels_max;
    
    err = snd_pcm_hw_params_set_channels( m_device_handle, hw_params,
                                          m_num_channels );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set channels to %d: %s",
                 m_num_channels, snd_strerror( err ) );
        goto error;
    }

    // periods
    err = snd_pcm_hw_params_set_periods( m_device_handle, hw_params, 2, 0 );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set period of 2: %s", snd_strerror( err ) );
        goto error;
    }

    // set the period size
    snd_pcm_uframes_t period_size;
    err = snd_pcm_hw_params_get_period_size_min( hw_params, &period_size, &dirs );
    // if( err > buffer_size ) buffer_size = err;
    err = snd_pcm_hw_params_set_period_size( m_device_handle, hw_params,
                                             m_out_buffer_size, 0 );

    // set the hw params
    err = snd_pcm_hw_params( m_device_handle, hw_params );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set hw params: %s", snd_strerror( err ) );
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
    
    // prepare the device
    if( snd_pcm_state( m_device_handle ) != SND_PCM_STATE_PREPARED )
    {
        err = snd_pcm_prepare( m_device_handle );
        if( err < 0 )
        {
            sprintf( msg, "error: cannot prepare device for playback: %s",
                     snd_strerror( err ) );
            goto error;
        }
    }

    // set the flag    
    m_render_start = TRUE;
    
    return TRUE;
    
error:
    // print out the error
    cerr << msg << endl;

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
    
    // drain
    err = snd_pcm_drain( m_device_handle );
    if( err < 0 )
    {
        sprintf( msg, "error: drain failed: %s", snd_strerror( err ) );
        goto error;
    }
       
    // set the flag
    m_render_start = FALSE;

error:
    // print out the error
    cerr << msg << endl;

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

    // close the handle
    if( m_device_handle )
    {
        snd_pcm_close( m_device_handle );
        m_device_handle = NULL;
    }
    
    // deallocate
    if( m_out_buffers )
        SAFE_DELETE_ARRAY( m_out_buffers );

    // set the flag    
    m_init_out = FALSE;
}




int a = 0;
//-----------------------------------------------------------------------------
// name: tick_out()
// desc: 1 channel
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::tick_out( SAMPLE sample )
{    
    a++;
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
    static int a = 0;
    if( !m_init_out )
        return FALSE;

    if( m_data_ptr_out >= m_data_max_out )
    {
        if( !m_render_start && !start() )
            return FALSE;

        // cout << ::a << " ";
        this->render();
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
DWORD__ DigitalOut::render()
{
    int err;
    char msg[1024] = "";

    // write data    
    err = snd_pcm_writei( m_device_handle, 
                          m_out_buffers[m_curr_buffer_out].data(), m_out_buffer_size );
    if( err < m_out_buffer_size )
    {
        if( snd_pcm_state( m_device_handle ) == SND_PCM_STATE_XRUN )
        {
            sprintf( msg, "alsa buffer underrun..." );
            cerr << msg << endl;
            
            err = snd_pcm_prepare( m_device_handle );
            if( err < 0 )
            {
                sprintf( msg, "error: cannot prepare device after underrun..." );
                goto error;
            }
        }
        else
        {
            sprintf( msg, "error: write during state '%s'...", 
                snd_pcm_state_name( snd_pcm_state( m_device_handle ) ) );
            goto error;
        }
    }
    
    // cerr << ".";
    
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
    
    m_device_handle = NULL;
    m_device_num = 0;
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
BOOL__ DigitalIn::initialize()
{
    char msg[1024];
    int err = 0;
    char name[256] = "";
    snd_pcm_hw_params_t * hw_params = NULL;
    unsigned int device_channels_max = 0, device_channels_min = 0;
    int dirs = 0;
    
    // check if already    
    if( m_init_in )
    {
        sprintf( msg, "error: initialize(); DigitalIn already initialized..." );
        goto error;
    }
    
    // set the parameters
    m_num_channels = Digitalio::m_num_channels;
    m_sampling_rate = Digitalio::m_sampling_rate;
    m_bps = Digitalio::m_bps;
    m_in_buffer_size = Digitalio::m_buffer_size;
    m_num_in_buffers = Digitalio::m_num_buffers;
    m_device_num = 0;
    
    // allocate the buffers
    m_in_buffers = new InternalBuffer[m_num_in_buffers];
    for( int i = 0; i < m_num_in_buffers; i++ )
        m_in_buffers[i].initialize( m_in_buffer_size * sizeof(SAMPLE_INTERNAL) *
                                    m_num_channels );

    m_curr_buffer_in = 0;
    m_data_ptr_in = m_in_buffers[0].data();
    m_data_max_in = m_data_ptr_in; 

    // device name
    sprintf( name, "hw:%d", m_device_num );

    // open the device
    err = snd_pcm_open( &m_device_handle, name, 
                        SND_PCM_STREAM_CAPTURE, SND_PCM_ASYNC );
    if( err < 0 )
    {
        sprintf( msg, "error opening device '%s' for capture: %s", name,
                 snd_strerror( err ) );
        goto error;
    }

    // allocate the hw params
    snd_pcm_hw_params_alloca( &hw_params );
    err = snd_pcm_hw_params_any( m_device_handle, hw_params );
    if( err < 0 )
    {
        sprintf( msg, "error getting parameters on device '%s': %s",
                 name, snd_strerror( err ) );
        goto error;
    }

    // set interleaved mode for access
    if( !snd_pcm_hw_params_test_access( m_device_handle, hw_params,
         SND_PCM_ACCESS_RW_INTERLEAVED ) )
        snd_pcm_hw_params_set_access( m_device_handle, hw_params,
            SND_PCM_ACCESS_RW_INTERLEAVED ); 
    else
    {
        sprintf( msg, "error: cannot set interleaved access\n" );
        goto error;
    }

    // test and set the format
    if( !snd_pcm_hw_params_test_format( m_device_handle, hw_params, 
                                        SND_PCM_FORMAT_S16 ) )
        snd_pcm_hw_params_set_format( m_device_handle, hw_params,
                                      SND_PCM_FORMAT_S16 );
    else
    {
        sprintf( msg, "error: cannot set format: signed 16-bit" );
        goto error;
    }

    // set the sample rate
    err = snd_pcm_hw_params_set_rate( m_device_handle, hw_params, 
                                      m_sampling_rate, 0 );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set sample rate %d: %s",
                 m_sampling_rate, snd_strerror( err ) );
        goto error;
    }

    // channels
    err = snd_pcm_hw_params_get_channels_min( hw_params, &device_channels_min );
    if( err < 0 ) {
        sprintf( msg, "error: cannot get channels min: %s", snd_strerror( err ) );
        goto error;
    }
        
    err = snd_pcm_hw_params_get_channels_max( hw_params, &device_channels_max );
    if( err < 0 ) {
        sprintf( msg, "error: cannot get channels max: %s", snd_strerror( err ) );
        goto error;
    }
        
    if( m_num_channels < device_channels_min )
        m_num_channels = device_channels_min;
    else if( m_num_channels > device_channels_max )
        m_num_channels = device_channels_max;
    
    err = snd_pcm_hw_params_set_channels( m_device_handle, hw_params,
                                          m_num_channels );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set channels to %d: %s",
                 m_num_channels, snd_strerror( err ) );
        goto error;
    }

    // periods
    err = snd_pcm_hw_params_set_periods( m_device_handle, hw_params, 2, 0 );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set period of 2: %s", snd_strerror( err ) );
        goto error;
    }

    // set the period size
    snd_pcm_uframes_t period_size;
    err = snd_pcm_hw_params_get_period_size_min( hw_params, &period_size, &dirs );
    // if( err > buffer_size ) buffer_size = err;
    err = snd_pcm_hw_params_set_period_size( m_device_handle, hw_params,
                                             m_in_buffer_size, 0 );

    // set the hw params
    err = snd_pcm_hw_params( m_device_handle, hw_params );
    if( err < 0 )
    {
        sprintf( msg, "error: cannot set hw params: %s", snd_strerror( err ) );
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
    
    // prepare the device
    if( snd_pcm_state( m_device_handle ) != SND_PCM_STATE_PREPARED )
    {
        err = snd_pcm_prepare( m_device_handle );
        if( err < 0 )
        {
            sprintf( msg, "error: cannot prepare device for capture: %s",
                     snd_strerror( err ) );
            goto error;
        }
    }

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
    
    // drain
    err = snd_pcm_drain( m_device_handle );
    if( err < 0 )
    {
        sprintf( msg, "error: drain failed: %s", snd_strerror( err ) );
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
    
    // close the handle
    if( m_device_handle )
    {
        snd_pcm_close( m_device_handle );
        m_device_handle = NULL;
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
        m_in_buffers[m_curr_buffer_in].clear();
        m_curr_buffer_in = (m_curr_buffer_in + 1) % m_num_in_buffers;
        m_data_ptr_in = m_in_buffers[m_curr_buffer_in].data();
        m_data_max_in = m_data_ptr_in + m_in_buffer_size * m_num_channels;
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
    int err;
    char msg[1024] = "";

    // read data    
    err = snd_pcm_readi( m_device_handle, 
                         m_in_buffers[m_curr_buffer_in].data(), m_in_buffer_size );
    if( err < m_in_buffer_size )
    {
        if( snd_pcm_state( m_device_handle ) == SND_PCM_STATE_XRUN )
        {
            sprintf( msg, "alsa buffer overrun..." );
            err = snd_pcm_prepare( m_device_handle );
            if( err < 0 )
            {
                sprintf( msg, "error: cannot prepare device after overrun..." );
                goto error;
            }
        }
        else
        {
            sprintf( msg, "error: write during state '%s'...", 
                snd_pcm_state_name( snd_pcm_state( m_device_handle ) ) );
            goto error;
        }
    }
    
    // cerr << "." << *m_data_ptr_in << endl;;
    
    return TRUE;
    
error:

    cerr << msg << endl;
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: AudioBuffer()
// desc: ...
//-----------------------------------------------------------------------------
AudioBuffer::AudioBuffer( DWORD__ size )
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
BOOL__ AudioBuffer::initialize( DWORD__ size )
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
void AudioBuffer::cleanup()
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
DWORD__ AudioBuffer::size()
{
    return m_size;
}




//-----------------------------------------------------------------------------
// name: data()
// desc: ...
//-----------------------------------------------------------------------------
SAMPLE * AudioBuffer::data()
{
    return m_data;
}




//-----------------------------------------------------------------------------
// name: AudioBufferIn()
// desc: ...
//-----------------------------------------------------------------------------
AudioBufferIn::AudioBufferIn( DWORD__ size )
    : AudioBuffer( size )
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
    : AudioBuffer( size )
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
