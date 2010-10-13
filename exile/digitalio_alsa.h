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


#include <alsa/asoundlib.h>
#include <pthread.h>
#include <unistd.h>




//-----------------------------------------------------------------------------
// define and forward references
//-----------------------------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

// defaults
#define BUFFER_SIZE_DEFAULT          512
#define BUFFER_SIZE_OUT_DEFAULT      512
#define BUFFER_SIZE_IN_DEFAULT       512
#define NUM_BUFFERS_DEFAULT          4
#define NUM_OUT_BUFFERS_DEFAULT      4
#define NUM_IN_BUFFERS_DEFAULT       4
#define NUM_CHANNELS_DEFAULT         2       // number of channels
#define SAMPLING_RATE_DEFAULT        48000   // sampling rate
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

// external to internal sample conversion
#define SAMPLE_TO_INTERNAL(s) ( (SAMPLE_INTERNAL)((SAMPLE)S_MAX * s) )
#define INTERNAL_TO_SAMPLE(s) ( (SAMPLE)((SAMPLE)s / S_MAX) )

// types
#define DWORD__                unsigned int
#define UINT__                 DWORD__
#define BOOL__                 DWORD__
#define FLOAT__                float
#define BYTE__                 unsigned char

#ifndef FALSE
  #define FALSE                0
#endif
#ifndef TRUE
  #define TRUE                 1
#endif

// forward references
class InternalBuffer;




//-----------------------------------------------------------------------------
// name: Digitalio
// desc: ...
//-----------------------------------------------------------------------------
class Digitalio
{
public:
    static BOOL__ initialize( DWORD__ num_channels = NUM_CHANNELS_DEFAULT,
                              DWORD__ sampling_rate = SAMPLING_RATE_DEFAULT,
                              DWORD__ bps = BITS_PER_SAMPLE_DEFAULT,
                              DWORD__ buffer_size = BUFFER_SIZE_DEFAULT,
                              DWORD__ num_buffers = NUM_BUFFERS_DEFAULT );
    static void shutdown();

public:
    static DWORD__ sampling_rate( ) { return m_sampling_rate; }
    static DWORD__ num_channels( ) { return m_num_channels; }

public: // data
    static BOOL__ m_init;
    static DWORD__ m_num_channels;
    static DWORD__ m_sampling_rate;
    static DWORD__ m_bps;
    static DWORD__ m_buffer_size;
    static DWORD__ m_num_buffers;
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
    virtual BOOL__ reset() { return true; }
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
    virtual BOOL__ reset() { return true; }
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
    BOOL__ initialize();
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
    
    DWORD__ m_num_channels;
    DWORD__ m_sampling_rate;
    DWORD__ m_bps;
    
    snd_pcm_t * m_device_handle;
    DWORD__ m_device_num;
    
    InternalBuffer * m_out_buffers;
    DWORD__ m_num_out_buffers;
    DWORD__ m_out_buffer_size;
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
    BOOL__ initialize();
    void cleanup();

    BOOL__ start();
    BOOL__ stop();

public:
    virtual BOOL__ tick_in( SAMPLE * s );
    virtual BOOL__ tick_in( SAMPLE * l, SAMPLE * r );
    virtual BOOL__ tick_in( SAMPLE * samples, DWORD__ n );

public:
     DWORD__ capture( );

protected:
    BOOL__ m_init_in;
    BOOL__ m_capture_start;

    DWORD__ m_num_channels;
    DWORD__ m_sampling_rate;
    DWORD__ m_bps;

    snd_pcm_t * m_device_handle;
    DWORD__ m_device_num;

    InternalBuffer * m_in_buffers;
    DWORD__ m_num_in_buffers;
    DWORD__ m_in_buffer_size;
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
    SAMPLE_INTERNAL * data( );
    DWORD__ size() const;

protected:
    SAMPLE_INTERNAL * m_data;
    DWORD__ m_size;
};




#endif

