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
#include "rtaudio.h"
#if defined(__WINDOWS_DS__) && !defined(__WINDOWS_PTHREAD__)
#include <windows.h>
#else
#include <unistd.h>
#endif




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

    m_num_channels_out = num_channels;
    m_num_channels_in = num_channels;
    m_sampling_rate = sampling_rate;
    m_bps = bps;
    m_buffer_size = buffer_size;
    m_num_buffers = num_buffers;
    m_start = 0;
    m_tick_count = 0;
    m_go = 0;
    m_end = 0;

    // allocate RtAudio
    try { m_rtaudio = new RtAudio( ); }
    catch( RtError err )
    {
        // problem finding audio devices, most likely
        EM_error2( 0, "%s", err.getMessageString() );
        return m_init = FALSE;
    }

    // open device
    try {
        m_rtaudio->openStream(
            m_dac_n, m_num_channels_out, m_adc_n, m_num_channels_in,
            RTAUDIO_FLOAT32, sampling_rate,
            (int *)&m_buffer_size, num_buffers );
        if( m_use_cb )
            m_rtaudio->setStreamCallback( &cb, NULL );
    } catch( RtError err ) {
        try {
            m_buffer_size = buffer_size;
            // try output only
            m_rtaudio->openStream(
                m_dac_n, m_num_channels_out, 0, 0,
                RTAUDIO_FLOAT32, sampling_rate,
                (int *)&m_buffer_size, num_buffers );
            if( m_use_cb )
                m_rtaudio->setStreamCallback( &cb, NULL );
        } catch( RtError err ) {
            EM_error2( 0, "%s", err.getMessageString() );
            SAFE_DELETE( m_rtaudio );
            return m_init = FALSE;
        }
    }

    if( m_use_cb )
    {
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
    DWORD__ n = 6;
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
        // copy local buffer to be rendered
        if( m_out_ready && !m_end ) memcpy( buffer, m_buffer_out, len );
        // set all elements of local buffer to silence
        else memset( buffer, 0, len);
    }
    else  // initial condition
    {
//#ifndef __WINDOWS_DS__
        if( !m_go && Chuck_VM::our_priority != 0x7fffffff )
            Chuck_VM::set_priority( Chuck_VM::our_priority, NULL );
//#endif
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
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::start( )
{
    try{ if( !m_start )
              m_rtaudio->startStream();
         m_start = TRUE;
    } catch( RtError err ){ return FALSE; }
    
    return m_start;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::stop( )
{
    try{ if( m_start )
             m_rtaudio->stopStream();
         m_start = FALSE;
    } catch( RtError err ){ return FALSE; }

    return !m_start;
}




//-----------------------------------------------------------------------------
// name: tick()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::tick( )
{
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
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void Digitalio::shutdown()
{
    if( !m_init ) return;
    if( m_start ) m_rtaudio->stopStream();
    
    m_rtaudio->closeStream();
    SAFE_DELETE( m_rtaudio );
    m_init = FALSE;
    m_start = FALSE;
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
    // set pointer to beginning of local buffer
    m_data_ptr_out = Digitalio::m_use_cb ? Digitalio::m_buffer_out
        : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
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

    // synchronize
    Digitalio::m_out_ready = TRUE;
    // synchronize
    while( Digitalio::m_out_ready )
        usleep( 250 );
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
    // set the buffer to the beginning of the local buffer
    m_data_ptr_in = Digitalio::m_use_cb ? m_data
        : (SAMPLE *)Digitalio::audio()->getStreamBuffer();
    // calculate past buffer
    m_data_max_in = m_data_ptr_in + 
        Digitalio::buffer_size() * Digitalio::num_channels_in();
    // set the read pointer to the local pointer
    Digitalio::m_read_ptr = &m_data_ptr_in;

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

    t_CKUINT n = 4;
    while( !Digitalio::m_in_ready && n-- )
        usleep( 250 );

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
