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
// file: digitalio_win32.cpp
// desc: digitalio for win32
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "digitalio_win32.h"
// #include <dxerr8.h>



BOOL__ Digitalio::m_init = FALSE;
LPDIRECTSOUND Digitalio::m_ds = NULL;
LPDIRECTSOUNDBUFFER Digitalio::m_primary = NULL;
WAVEFORMATEX Digitalio::m_wfx;




//-----------------------------------------------------------------------------
// name: initialize()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ Digitalio::initialize( DWORD__ num_channels, DWORD__ sampling_rate,
                              DWORD__ bps )
{
    HRESULT hr;
    DSBUFFERDESC desc;
    HWND hwnd = NULL;
    
    if( m_init )
        return FALSE;

    if( !hwnd )
        hwnd = GetForegroundWindow();

    // init wave
    ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) );
    m_wfx.wFormatTag = WAVE_FORMAT_PCM;
    m_wfx.nChannels = (WORD)num_channels;
    m_wfx.nSamplesPerSec = sampling_rate;
    m_wfx.wBitsPerSample = (WORD)bps;
    m_wfx.nBlockAlign = m_wfx.wBitsPerSample / 8 * m_wfx.nChannels;
    m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

    // init desc
    ZeroMemory( &desc, sizeof(DSBUFFERDESC) );
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    // create directsound
    if FAILED( hr = DirectSoundCreate( NULL, &m_ds, NULL ) )
    {
        // DXTRACE_ERR( "DirectSoundCreate8", hr );
        goto cleanup;
    }

    // set the cooperative level
    if FAILED( hr = m_ds->SetCooperativeLevel( hwnd, DSSCL_PRIORITY ) )
    {
        // DXTRACE_ERR( "SetCooperativeLevel", hr );
        goto cleanup;
    }

    // compact onboard memory
    if FAILED( hr = m_ds->Compact() )
    {
        // DXTRACE_ERR( "Compact", hr );
        goto cleanup;
    }

    // create primary buffer
    if FAILED( hr = m_ds->CreateSoundBuffer( &desc, &m_primary, NULL ) )
    {
        // DXTRACE_ERR( "CreateSoundBuffer", hr );
        goto cleanup;
    }

    // set the primary buffer format
    if FAILED( hr = m_primary->SetFormat( &m_wfx ) )
    {
        // DXTRACE_ERR( "SetFormat", hr );
        goto cleanup;
    }

    // play the primary buffer
    // m_primary->Play( 0, 0, DSBPLAY_LOOPING );

    return m_init = TRUE;

cleanup:
    Digitalio::shutdown();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void Digitalio::shutdown()
{
    // shutdown primary buffer
    if( m_primary )
        m_primary->Stop();

    SAFE_RELEASE( m_primary );
    SAFE_RELEASE( m_ds );

    ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) );
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
    m_out_buffers = NULL;
    m_main_out_buffer = NULL;
    m_out_buffer_size = 0;
    m_num_out_buffers = 0;
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
BOOL__ DigitalOut::initialize( DWORD__ device_num, DWORD__ num_channels,
                               DWORD__ sampling_rate, DWORD__ bps,
                               DWORD__ buffer_size, DWORD__ num_buffers )
{
    if( !Digitalio::m_init && !Digitalio::initialize() )
        return FALSE;

    UINT__ i;

    // save the values
    m_out_buffer_size = buffer_size;
    m_num_out_buffers = num_buffers;

    m_out_buffers = new InternalBuffer * [m_num_out_buffers];
    for( i = 0; i < m_num_out_buffers; i++ )
    {
        m_out_buffers[i] = new InternalBuffer( );
        m_out_buffers[i]->initialize( m_out_buffer_size * num_channels * sizeof(SAMPLE) );
    }

    // ebuffer
    m_main_out_buffer = new EventOutBuffer( Digitalio::m_ds );
    if( !m_main_out_buffer->initialize( &Digitalio::m_wfx, m_out_buffer_size 
         * m_num_out_buffers * num_channels * sizeof(SAMPLE), 
         m_out_buffer_size * num_channels * sizeof(SAMPLE) ) )
        return FALSE;

    // data pointers
    m_curr_buffer_out = 0;
    m_data_ptr_out = m_out_buffers[0]->data();
    m_data_max_out = (SAMPLE_INTERNAL *)( (DWORD__)m_data_ptr_out + 
                                          m_out_buffers[0]->size() );

    return m_init_out = TRUE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::start()
{
    if( !m_init_out && !this->initialize() )
        return FALSE;

    // already started
    if( m_render_start )
        return FALSE;

    EventOutBuffer * eb = m_main_out_buffer;
    InternalBuffer ** sb = m_out_buffers;
    
    // reset the cswap
    eb->cswap_reset();
    // cswap the first part in
    eb->cswap( sb[0] );
    // play the buffer
    eb->buffer()->Play( 0, 0, DSBPLAY_LOOPING );

    m_index = 0;
    m_render_start = TRUE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stop()
// desc: render
//-----------------------------------------------------------------------------
BOOL__ DigitalOut::stop()
{
    if( !m_render_start )
        return FALSE;

    // flag
    m_render_start = FALSE;

    // stop the buffer
    m_main_out_buffer->buffer()->Stop();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalOut::cleanup()
{
    if( m_render_start )
        this->stop();

    DWORD__ i;
    for( i = 0; i < m_num_out_buffers; i++ )
        SAFE_DELETE( m_out_buffers[i] );
    SAFE_DELETE_ARRAY( m_out_buffers );
    SAFE_DELETE( m_main_out_buffer );

    m_num_out_buffers = 0;
    m_out_buffer_size = 0;

    m_curr_buffer_out = 0;
    m_data_ptr_out = NULL;
    m_data_max_out = NULL;

    m_init_out = FALSE;
    m_index = 0;
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

    if( !n ) n = Digitalio::m_wfx.nChannels;
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

        render();
        m_curr_buffer_out = (m_curr_buffer_out + 1) % m_num_out_buffers;
        m_data_ptr_out = m_out_buffers[m_curr_buffer_out]->data();
        m_data_max_out = (SAMPLE_INTERNAL *)( (DWORD__)m_data_ptr_out + 
                         m_out_buffers[m_curr_buffer_out]->size() );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: render()
// desc: the render
//-----------------------------------------------------------------------------
DWORD__ DigitalOut::render()
{
    if( !m_render_start )
        return FALSE;

    EventOutBuffer * eb = m_main_out_buffer;
    InternalBuffer ** sb = m_out_buffers;
    
    // increment index
    m_index = (m_index + 1) % m_num_out_buffers;
    // wait for the event
    eb->wait();
    // swap the next
    eb->cswap( sb[m_index] );
    // clear the buffer
    sb[m_index]->clear();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: DigitalIn()
// desc: ...
//-----------------------------------------------------------------------------
DigitalIn::DigitalIn()
{
    m_init_in = FALSE;
    m_capture_start = FALSE;
    m_capture_thread = FALSE;
    m_in_buffers = NULL;
    m_main_in_buffer = NULL;
    m_in_buffer_size = 0;
    m_num_in_buffers = 0;
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
BOOL__ DigitalIn::initialize( DWORD__ device_number, DWORD__ num_channels,
                              DWORD__ sampling_rate, DWORD__ bps,
                              DWORD__ buffer_size, DWORD__ num_buffers )
{
    if( !Digitalio::m_init && !Digitalio::initialize() )
        return FALSE;

    UINT__ i;

    // save the values
    m_in_buffer_size = buffer_size;
    m_num_in_buffers = num_buffers;

    m_in_buffers = new InternalBuffer * [m_num_in_buffers];
    for( i = 0; i < m_num_in_buffers; i++ )
    {
        m_in_buffers[i] = new InternalBuffer( );
        m_in_buffers[i]->initialize( m_in_buffer_size );
    }

    // ebuffer
    m_main_in_buffer = new EventInBuffer( );
    if( !m_main_in_buffer->initialize( &Digitalio::m_wfx, m_in_buffer_size 
         * m_num_in_buffers, m_in_buffer_size ) )
        return FALSE;

    // data pointers
    m_curr_buffer_in = m_num_in_buffers - 1;
    m_data_ptr_in = 0;
    m_data_max_in = 0;

    // not started yet
    m_capture_start = FALSE;

    return m_init_in = TRUE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::start()
{
    if( !m_init_in && !this->initialize() )
        return FALSE;

    if( m_capture_start )
        return FALSE;

    DWORD__ id;

    m_capture_start = TRUE;

    // fork
    m_capture_thread = CreateThread( NULL, 0, cb_capture, this, 0, &id );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: capture_stop()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ DigitalIn::stop()
{
    if( !m_capture_start )
        return FALSE;

    m_capture_start = FALSE;

    WaitForSingleObject( m_capture_thread, INFINITE );

    CloseHandle( m_capture_thread );
    m_capture_thread = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void DigitalIn::cleanup()
{
    if( m_capture_start )
        this->stop();

    DWORD__ i;
    for( i = 0; i < m_num_in_buffers; i++ )
        SAFE_DELETE( m_in_buffers[i] );
    SAFE_DELETE_ARRAY( m_in_buffers );
    SAFE_DELETE( m_main_in_buffer );

    m_num_in_buffers = 0;
    m_in_buffer_size = 0;

    m_curr_buffer_in = 0;
    m_data_ptr_in = NULL;
    m_data_max_in = NULL;

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

    if( !n ) n = Digitalio::m_wfx.nChannels;
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
        m_curr_buffer_in = (m_curr_buffer_in + 1) % m_num_in_buffers;
        m_data_ptr_in = m_in_buffers[m_curr_buffer_in]->data();
        m_data_max_in = (SAMPLE_INTERNAL *)( (DWORD__)m_data_ptr_in + 
                               m_in_buffers[m_curr_buffer_in]->size() );
        m_in_buffers[m_curr_buffer_in]->synch();
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cb_capture()
// desc: ...
//-----------------------------------------------------------------------------
DWORD__ CALLBACK DigitalIn::cb_capture( void * param )
{
    DigitalIn * dig_in = (DigitalIn *)param;
    DWORD__ index = 0;
    EventInBuffer * eb = dig_in->m_main_in_buffer;
    InternalBuffer ** sb = dig_in->m_in_buffers;
    
    // reset the cswap
    eb->cswap_reset();
    // play the buffer
    eb->buffer()->Start( DSCBSTART_LOOPING );

    DWORD__ c = 0;

    // loop
    while( dig_in->m_capture_start )
    {
        // clear the buffer
        sb[index]->clear();
        // wait for the event
        eb->wait();
        // swap the next
        eb->cswap( sb[index] );
        // signal the next buffer
        sb[index]->signal();
        // increment index
        index = (index + 1) % dig_in->m_num_in_buffers;
    }

    // stop the buffer
    eb->buffer()->Stop();

    return 0;
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
    ZeroMemory( m_data, size * sizeof(SAMPLE) );

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
    m_event = NULL;
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
    m_event = CreateEvent( NULL, FALSE, FALSE, NULL );

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
    ZeroMemory( m_data, m_size );    

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
        CloseHandle( m_event );
        m_event = NULL;
    }
}




//-----------------------------------------------------------------------------
// name: data()
// desc: get the buffer
//-----------------------------------------------------------------------------
SAMPLE_INTERNAL * InternalBuffer::data( DWORD__ channel )
{
    return m_data + channel;
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
    WaitForSingleObject( m_event, INFINITE );
}




//-----------------------------------------------------------------------------
// name: signal()
// desc: signal the buffer to be used again
//-----------------------------------------------------------------------------
void InternalBuffer::signal()
{
    SetEvent( m_event );
}




//-----------------------------------------------------------------------------
// name: reset()
// desc:...
//-----------------------------------------------------------------------------
void InternalBuffer::reset()
{
    signal();
    ZeroMemory( m_data, m_size );        
}




//-----------------------------------------------------------------------------
// name: EventOutBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
EventOutBuffer::EventOutBuffer( LPDIRECTSOUND ds )
{
    ds->AddRef();
    m_ds = ds;

    m_buffer = NULL;
    ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) );
    ZeroMemory( &m_desc, sizeof(DSBUFFERDESC) );
    m_offset = 0;
    m_block_size = 0;
}




//-----------------------------------------------------------------------------
// name: ~EventOutBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
EventOutBuffer::~EventOutBuffer()
{
    cleanup();
    SAFE_RELEASE( m_ds );
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: allocate
//-----------------------------------------------------------------------------
BOOL__ EventOutBuffer::initialize( WAVEFORMATEX * wfx, DWORD__ buffer_size,
                                   DWORD__ block_size )
{
    HRESULT hr;

    // cleanup first
    cleanup();

    // check
    if( !wfx )
        return FALSE;

    m_desc.dwSize = sizeof(DSBUFFERDESC);
    m_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS
                     | DSBCAPS_LOCHARDWARE;
                     // ( period ? DSBCAPS_CTRLPOSITIONNOTIFY : 0 );
    m_desc.dwBufferBytes = buffer_size;
    m_desc.lpwfxFormat = wfx;

    // create buffer, force hardware mixing
    if FAILED( hr = m_ds->CreateSoundBuffer( &m_desc, &m_buffer, NULL ) )
    {
        // force software mixing
        m_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS
                       | DSBCAPS_LOCSOFTWARE;
        if FAILED( hr = m_ds->CreateSoundBuffer( &m_desc, &m_buffer, NULL ) )
        {
            // DXTRACE_ERR( "CreateSoundBuffer", hr );
            return FALSE;
        }
    }

    memcpy( &m_wfx, wfx, sizeof(WAVEFORMATEX) );
    m_block_size = block_size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void EventOutBuffer::cleanup()
{
    SAFE_RELEASE( m_buffer );
    m_block_size = 0;
    m_offset = 0;
}




//-----------------------------------------------------------------------------
// name: wait()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ EventOutBuffer::wait()
{
    if( !m_buffer )
        return FALSE;

    DWORD__ r, w, offset = m_offset;

    m_buffer->GetCurrentPosition( &r, &w );
    if( r < offset ) r += m_desc.dwBufferBytes;

    float t;
    while( r < offset + m_block_size )
    {
        t = ((offset + m_block_size) - r) * 900.0f;
        t /= Digitalio::m_wfx.nAvgBytesPerSec;
        if( t < 1.0f ) t = 1.0f;
        Sleep( (DWORD__)t );

        m_buffer->GetCurrentPosition( &r, &w );
        if( r < offset ) r += m_desc.dwBufferBytes;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: swap()
// desc: swap a section of the buffer
//-----------------------------------------------------------------------------
BOOL__ EventOutBuffer::swap( InternalBuffer * src, DWORD__ offset, DWORD__ size )
{
    HRESULT hr;

    if( !src || !m_buffer )
        return FALSE;

    void * lp_locked_buffer = NULL, * lp_locked_buffer2 = NULL, * lp_src;
    DWORD__ locked_buffer_size = 0, locked_buffer_size2 = 0;

    // lock the buffer
    if( FAILED( hr = m_buffer->Lock( offset, size, &lp_locked_buffer, 
                                     &locked_buffer_size, &lp_locked_buffer2,
                                     &locked_buffer_size2, 0 ) ) )
    {
        // DXTRACE_ERR( "Lock", hr );
        return FALSE;
    }

    // get the src data
    lp_src = src->data();
    
    // copy it
    memcpy( lp_locked_buffer, lp_src, locked_buffer_size );

    // wrap
    if( locked_buffer_size < size )
        memcpy( lp_locked_buffer2, (void *)((DWORD__)lp_src + locked_buffer_size),
                locked_buffer_size2 );

    // unlock the buffer
    m_buffer->Unlock( lp_locked_buffer, locked_buffer_size, lp_locked_buffer2,
                      ( locked_buffer_size < size ? locked_buffer_size2 : 0 ) );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cswap()
// desc: auto offset for circular
//-----------------------------------------------------------------------------
BOOL__ EventOutBuffer::cswap( InternalBuffer * src )
{
    DWORD__ offset = m_offset;
    m_offset += src->size();
    m_offset %= m_desc.dwBufferBytes;

    return swap( src, offset, src->size() );
}




//-----------------------------------------------------------------------------
// name: cswap_reset()
// desc: reset cswap
//-----------------------------------------------------------------------------
void EventOutBuffer::cswap_reset()
{
    m_offset = 0;
}




//-----------------------------------------------------------------------------
// name: buffer()
// desc: get the buffer
//-----------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER EventOutBuffer::buffer()
{
    return m_buffer;
}




//-----------------------------------------------------------------------------
// name: EventInBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
EventInBuffer::EventInBuffer( )
{
    m_buffer = NULL;
    ZeroMemory( &m_wfx, sizeof(WAVEFORMATEX) );
    ZeroMemory( &m_desc, sizeof(DSCBUFFERDESC) );
    m_block_size = 0;
    m_offset = 0;
    m_dsc = NULL;
}




//-----------------------------------------------------------------------------
// name: ~EventInBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
EventInBuffer::~EventInBuffer()
{
    cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: allocate
//-----------------------------------------------------------------------------
BOOL__ EventInBuffer::initialize( WAVEFORMATEX * wfx, DWORD__ buffer_size,
                                DWORD__ block_size )
{
    HRESULT hr;

    // cleanup first
    cleanup();

    // check
    if( !wfx )
        return FALSE;

    if FAILED( hr = DirectSoundCaptureCreate( NULL, &m_dsc, NULL ) )
    {
        // DXTRACE_ERR( "DirectSoundCaptureCreate8", hr );
        return FALSE;
    }

    m_desc.dwSize = sizeof(DSCBUFFERDESC);
    m_desc.dwFlags = 0;
    m_desc.dwBufferBytes = buffer_size;
    m_desc.lpwfxFormat = wfx;


    if FAILED( hr = m_dsc->CreateCaptureBuffer( &m_desc, &m_buffer, NULL ) )
    {
        // DXTRACE_ERR( "CreateCaptureBuffer", hr );
        return FALSE;
    }

    memcpy( &m_wfx, wfx, sizeof(WAVEFORMATEX) );
    m_block_size = block_size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: wait()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ EventInBuffer::wait()
{
    if( !m_buffer )
        return FALSE;

    DWORD__ c, r, offset = m_offset;
    float t;

    m_buffer->GetCurrentPosition( &r, &c );
    if( r < offset ) r += m_desc.dwBufferBytes;

    while( r < offset + m_block_size )
    {
        t = ((offset + m_block_size) - r) * 900.0f;
        t /= Digitalio::m_wfx.nAvgBytesPerSec;
        if( t < 1.0f ) t = 1.0f;
        Sleep( (DWORD__)t );

        m_buffer->GetCurrentPosition( &r, &c );
        if( r < offset ) r += m_desc.dwBufferBytes;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void EventInBuffer::cleanup()
{
    SAFE_RELEASE( m_buffer );
    SAFE_RELEASE( m_dsc );

    m_block_size = 0;
    m_offset = 0;
}




//-----------------------------------------------------------------------------
// name: swap()
// desc: swap a section of the buffer
//-----------------------------------------------------------------------------
BOOL__ EventInBuffer::swap( InternalBuffer * src, DWORD__ offset, DWORD__ size )
{
    HRESULT hr;

    if( !src || !m_buffer )
        return FALSE;

    void * lp_locked_buffer = NULL, * lp_locked_buffer2 = NULL, * lp_src;
    DWORD__ locked_buffer_size = 0, locked_buffer_size2 = 0;

    // lock the buffer
    if( FAILED( hr = m_buffer->Lock( offset, size, &lp_locked_buffer, 
                                     &locked_buffer_size, &lp_locked_buffer2,
                                     &locked_buffer_size2, 0 ) ) )
    {
        // DXTRACE_ERR( "Lock", hr );
        return FALSE;
    }

    // get the src data
    lp_src = src->data();
    
    // copy it
    memcpy( lp_src, lp_locked_buffer, locked_buffer_size );

    // wrap
    if( locked_buffer_size < size )
        memcpy( (void *)((DWORD__)lp_src + locked_buffer_size), lp_locked_buffer2,
                locked_buffer_size2 );

    // unlock the buffer
    m_buffer->Unlock( lp_locked_buffer, locked_buffer_size, lp_locked_buffer2,
                      ( locked_buffer_size < size ? locked_buffer_size2 : 0 ) );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cswap()
// desc: auto offset for circular
//-----------------------------------------------------------------------------
BOOL__ EventInBuffer::cswap( InternalBuffer * src )
{
    DWORD__ offset = m_offset;
    m_offset += src->size();
    m_offset %= m_desc.dwBufferBytes;

    return swap( src, offset, src->size() );
}




//-----------------------------------------------------------------------------
// name: cswap_reset()
// desc: reset cswap
//-----------------------------------------------------------------------------
void EventInBuffer::cswap_reset()
{
    m_offset = 0;
}




//-----------------------------------------------------------------------------
// name: buffer()
// desc: get the buffer
//-----------------------------------------------------------------------------
LPDIRECTSOUNDCAPTUREBUFFER EventInBuffer::buffer()
{
    return m_buffer;
}
