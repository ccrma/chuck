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
// name: digitalio.h
// desc: digitalio
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#ifndef __DIGITAL_IO_H__
#define __DIGITAL_IO_H__

#include <windows.h>
#include <dsound.h>




//-----------------------------------------------------------------------------
// define and forward references
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

// defaults
#define BUFFER_SIZE_OUT_DEFAULT      512
#define BUFFER_SIZE_IN_DEFAULT       512
#define NUM_OUT_BUFFERS_DEFAULT      4
#define NUM_IN_BUFFERS_DEFAULT       4
#define NUM_CHANNELS_DEFAULT         2       // number of channels
#define SAMPLING_RATE_DEFAULT        44100   // sampling rate
#define BITS_PER_SAMPLE_DEFAULT      16      // sample size
#define DEVICE_NUM_OUT_DEFAULT       0
#define DEVICE_NUM_IN_DEFAULT        0

// sample types
#define SAMPLE_SHORT         short
#define SAMPLE_FLOAT         float
#define SAMPLE_INTERNAL      SAMPLE_SHORT
#define SAMPLE               SAMPLE_FLOAT
#define S_MAX                0x7fff  // max value for 16 bit
#define S_MIN                -0x7fff // min value for 16 bit

// external to internal sample
#define SAMPLE_TO_INTERNAL(s) ( (SAMPLE_INTERNAL)((SAMPLE)S_MAX * s) )
#define INTERNAL_TO_SAMPLE(s) ( (SAMPLE)((SAMPLE)s / S_MAX) )

// forward
class InternalBuffer;
class EventOutBuffer;
class EventInBuffer;

// types
#define DWORD__                DWORD
#define UINT__                 unsigned long
#define BOOL__                 DWORD__
#define FLOAT__                float
#define BYTE__                 unsigned char

#ifndef FALSE
  #define FALSE                0
#endif
#ifndef TRUE
  #define TRUE                 1
#endif




//-----------------------------------------------------------------------------
// name: Digitalio
// desc: ...
//-----------------------------------------------------------------------------
class Digitalio
{
public:
    static BOOL__ initialize( DWORD__ num_channels = NUM_CHANNELS_DEFAULT,
                              DWORD__ sampling_rate = SAMPLING_RATE_DEFAULT,
                              DWORD__ bps = BITS_PER_SAMPLE_DEFAULT );
    static void shutdown();

public:
    static DWORD__ sampling_rate( ) { return (DWORD__)m_wfx.nSamplesPerSec; }
    static DWORD__ num_channels( ) { return (DWORD__)m_wfx.nChannels; }

public: // data
    static BOOL__ m_init;
    static LPDIRECTSOUND m_ds;
    static LPDIRECTSOUNDBUFFER m_primary;
    static WAVEFORMATEX m_wfx;
};




//-----------------------------------------------------------------------------
// name: TickOut
// desc: ...
//-----------------------------------------------------------------------------
class TickOut
{
public:
    virtual ~TickOut() {}

public:
    virtual BOOL__ reset() { return TRUE; }
    virtual BOOL__ tick_out( SAMPLE s ) = 0;
    virtual BOOL__ tick_out( SAMPLE l, SAMPLE r ) = 0;
    virtual BOOL__ tick_out( const SAMPLE * out, DWORD__ n ) = 0;
};




//-----------------------------------------------------------------------------
// name: TickIn
// desc: ...
//-----------------------------------------------------------------------------
class TickIn
{
public:
    virtual ~TickIn() { }

public:
    virtual BOOL__ reset() { return TRUE; }
    virtual BOOL__ tick_in( SAMPLE * in ) = 0;
    virtual BOOL__ tick_in( SAMPLE * l, SAMPLE * r ) = 0;
    virtual BOOL__ tick_in( SAMPLE * in, DWORD__ n ) = 0;

    virtual SAMPLE tick( )
    { SAMPLE in; return ( tick_in( &in ) ? in : (SAMPLE)0.0f ); }
};




//-----------------------------------------------------------------------------
// name: ChannelIO
// desc: ...
//-----------------------------------------------------------------------------
class ChannelIO
{
public:
    virtual ~ChannelIO() { }

public:
    virtual SAMPLE _( SAMPLE x ) = 0;
};




//-----------------------------------------------------------------------------
// name: class DigitalOut
// desc: ...
//-----------------------------------------------------------------------------
class DigitalOut : public TickOut
{
public:
    DigitalOut();
    ~DigitalOut();

public:
    BOOL__ initialize( DWORD__ device_num = DEVICE_NUM_OUT_DEFAULT,
                       DWORD__ num_channels = NUM_CHANNELS_DEFAULT,
                       DWORD__ sampling_rate = SAMPLING_RATE_DEFAULT,
                       DWORD__ bps = BITS_PER_SAMPLE_DEFAULT,
                       DWORD__ buffer_size = BUFFER_SIZE_OUT_DEFAULT,
                       DWORD__ num_buffers = NUM_OUT_BUFFERS_DEFAULT );
    void cleanup();

    BOOL__ start();
    BOOL__ stop();

public:
    virtual BOOL__ tick_out( SAMPLE s );
    virtual BOOL__ tick_out( SAMPLE l, SAMPLE r );
    virtual BOOL__ tick_out( const SAMPLE * samples, DWORD__ n );

public:
    DWORD__ render();

protected:
    BOOL__ m_init_out;
    BOOL__ m_render_start;

    DWORD__ m_out_buffer_size;
    DWORD__ m_num_out_buffers;
    InternalBuffer ** m_out_buffers;
    EventOutBuffer * m_main_out_buffer;
    DWORD__ m_curr_buffer_out;
    SAMPLE_INTERNAL * m_data_ptr_out;
    SAMPLE_INTERNAL * m_data_max_out;
    DWORD__ m_index;

    inline BOOL__ prepare_tick_out();
};




//-----------------------------------------------------------------------------
// name: class DigitalIn
// desc: ...
//-----------------------------------------------------------------------------
class DigitalIn : public TickIn
{
public:
    DigitalIn();
    ~DigitalIn();

public: // in
    BOOL__ initialize( DWORD__ device_number = DEVICE_NUM_IN_DEFAULT,
                       DWORD__ num_channels = NUM_CHANNELS_DEFAULT,
                       DWORD__ sampling_rate = SAMPLING_RATE_DEFAULT,
                       DWORD__ bps = BITS_PER_SAMPLE_DEFAULT,
                       DWORD__ buffer_size = BUFFER_SIZE_IN_DEFAULT,
                       DWORD__ num_buffers = NUM_IN_BUFFERS_DEFAULT );
    void cleanup();

    BOOL__ start();
    BOOL__ stop();

public:
    virtual BOOL__ tick_in( SAMPLE * s );
    virtual BOOL__ tick_in( SAMPLE * l, SAMPLE * r );
    virtual BOOL__ tick_in( SAMPLE * samples, DWORD__ n );

public:
    static DWORD__ CALLBACK cb_capture( void * param );

protected:
    BOOL__ m_init_in;
    BOOL__ m_capture_start;
    HANDLE m_capture_thread;

    DWORD__ m_in_buffer_size;
    DWORD__ m_num_in_buffers;
    InternalBuffer ** m_in_buffers;
    EventInBuffer * m_main_in_buffer;
    DWORD__ m_curr_buffer_in;
    SAMPLE_INTERNAL * m_data_ptr_in;
    SAMPLE_INTERNAL * m_data_max_in;

    inline BOOL__ prepare_tick_in();
};




//-----------------------------------------------------------------------------
// name: AudioBuffer
// desc: ...
//-----------------------------------------------------------------------------
class AudioBuffer
{
public:
    AudioBuffer( DWORD__ size = 0 );

public:
    BOOL__ initialize( DWORD__ size );
    void cleanup();
    SAMPLE * data();
    DWORD__ size();

protected:
    SAMPLE * m_data;
    DWORD__ m_size;

    SAMPLE * m_ptr_curr;
    SAMPLE * m_ptr_end;
};




//-----------------------------------------------------------------------------
// name: AudioBufferIn
// desc: ...
//-----------------------------------------------------------------------------
class AudioBufferIn : public TickIn, public AudioBuffer
{
public:
    AudioBufferIn( DWORD__ size = 0 );
    virtual ~AudioBufferIn();

public:
    virtual BOOL__ reset();
    virtual BOOL__ tick_in( SAMPLE * in );
    virtual BOOL__ tick_in( SAMPLE * l, SAMPLE * r );
    virtual BOOL__ tick_in( SAMPLE * in, DWORD__ n );
};




//-----------------------------------------------------------------------------
// name: AudioBufferOut
// desc: ...
//-----------------------------------------------------------------------------
class AudioBufferOut : public TickOut, public AudioBuffer
{
public:
    AudioBufferOut( DWORD__ size = 0 );
    virtual ~AudioBufferOut();

public:
    virtual BOOL__ reset();
    virtual BOOL__ tick_out( SAMPLE s );
    virtual BOOL__ tick_out( SAMPLE l, SAMPLE r );
    virtual BOOL__ tick_out( const SAMPLE * out, DWORD__ n );
};




//-----------------------------------------------------------------------------
// name: InternalBuffer
// desc: sample buffer
//-----------------------------------------------------------------------------
class InternalBuffer
{
public:
    InternalBuffer();
    ~InternalBuffer();

public:
    BOOL__ initialize( DWORD__ buffer_size );
    void cleanup();

public:
    BOOL__ clear();
    void synch();
    void signal();
    void reset();

public:
    SAMPLE_INTERNAL * data( DWORD__ channel = 0 );
    DWORD__ size() const;

protected:
    SAMPLE_INTERNAL * m_data;
    DWORD__ m_size;
    HANDLE m_event;
};




//-----------------------------------------------------------------------------
// name: class EventOutBuffer
// desc: sample buffer with event
//-----------------------------------------------------------------------------
class EventOutBuffer
{
public:
    EventOutBuffer( LPDIRECTSOUND ds );
    ~EventOutBuffer();

public:
    BOOL__ initialize( WAVEFORMATEX * wfx, DWORD__ buffer_size, 
                     DWORD__ block_size );
    void cleanup();

public:
    BOOL__ wait();
    BOOL__ swap( InternalBuffer * src, DWORD__ offset, DWORD__ size );
    BOOL__ cswap( InternalBuffer * src );
    void cswap_reset();

public:
    LPDIRECTSOUNDBUFFER buffer();

protected:
    LPDIRECTSOUNDBUFFER m_buffer;
    LPDIRECTSOUND m_ds;
    WAVEFORMATEX m_wfx;
    DSBUFFERDESC m_desc;

    DWORD__ m_block_size;
    DWORD__ m_offset;
};




//-----------------------------------------------------------------------------
// name: class EventInBuffer
// desc: sample buffer with event
//-----------------------------------------------------------------------------
class EventInBuffer
{
public:
    EventInBuffer();
    ~EventInBuffer();

public:
    BOOL__ initialize( WAVEFORMATEX * wfx, DWORD__ buffer_size, 
                     DWORD__ block_size );
    void cleanup();

public:
    BOOL__ wait();
    BOOL__ swap( InternalBuffer * src, DWORD__ offset, DWORD__ size );
    BOOL__ cswap( InternalBuffer * src );
    void cswap_reset();

public:
    LPDIRECTSOUNDCAPTUREBUFFER buffer();

protected:
    LPDIRECTSOUNDCAPTURE m_dsc;
    LPDIRECTSOUNDCAPTUREBUFFER m_buffer;
    WAVEFORMATEX m_wfx;
    DSCBUFFERDESC m_desc;

    DWORD__ m_block_size;
    DWORD__ m_offset;
};




#endif
