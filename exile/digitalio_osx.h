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
// name: digitalio_osx.h
// desc: digitalio
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ari Lazier (alazier@cs.princeton.edu)
//-----------------------------------------------------------------------------
#ifndef __DIGITAL_IO_H__
#define __DIGITAL_IO_H__


#include <CoreAudio/CoreAudio.h>
#include <pthread.h>
#include <unistd.h>




//-----------------------------------------------------------------------------
// define and forward references
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

// defaults
#define BUFFER_SIZE_OUT_DEFAULT      512
#define BUFFER_SIZE_IN_DEFAULT       512
#define BUFFER_SIZE_DEFAULT          512
#define NUM_OUT_BUFFERS_DEFAULT      4
#define NUM_IN_BUFFERS_DEFAULT       4
#define NUM_BUFFERS_DEFAULT          4
#define NUM_CHANNELS_DEFAULT         2       // number of channels
#define SAMPLING_RATE_DEFAULT        44100   // sampling rate
#define BITS_PER_SAMPLE_DEFAULT      32      // sample size
#define DEVICE_NUM_OUT_DEFAULT       2
#define DEVICE_NUM_IN_DEFAULT        2

// sample types
#define SAMPLE_SHORT         short
#define SAMPLE_FLOAT         float
#define SAMPLE_INTERNAL      SAMPLE_FLOAT
#define SAMPLE               SAMPLE_FLOAT
#define S_MAX                1.0f  // max value for float
#define S_MIN                -1.0f // min value for float

// external to internal sample conversion
#define SAMPLE_TO_INTERNAL(s) ( s )
#define INTERNAL_TO_SAMPLE(s) ( s )

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
    static DWORD__ bps( ) { return m_bps; }
    static DWORD__ buffer_size( ) { return m_buffer_size; }
    static DWORD__ num_buffers( ) { return m_num_buffers; }

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
    DWORD__ render( AudioBufferList * buffer_list );

protected:
    BOOL__ m_init_out;
    BOOL__ m_render_start;
    
    DWORD__ m_num_channels;
    DWORD__ m_sampling_rate;
    DWORD__ m_bps;

    AudioDeviceID m_device_handle;
    DWORD__ m_device_num;
    pthread_mutex_t m_mutex;
    DWORD__ m_block;
    DWORD__ m_out2;
    
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
     DWORD__ capture( const AudioBufferList * buffer_list );

protected:
    BOOL__ m_init_in;
    BOOL__ m_capture_start;

    DWORD__ m_num_channels;
    DWORD__ m_sampling_rate;
    DWORD__ m_bps;

    AudioDeviceID m_device_handle;
    DWORD__ m_device_num;
    pthread_mutex_t m_mutex;
    DWORD__ m_block;
    DWORD__ m_in2;

    InternalBuffer * m_in_buffers;
    DWORD__ m_num_in_buffers;
    DWORD__ m_in_buffer_size;
    DWORD__ m_curr_buffer_in;
    SAMPLE_INTERNAL * m_data_ptr_in;
    SAMPLE_INTERNAL * m_data_max_in;

    inline BOOL__ prepare_tick_in();
};




//-----------------------------------------------------------------------------
// name: AudioBuffer__
// desc: ...
//-----------------------------------------------------------------------------
class AudioBuffer__
{
public:
    AudioBuffer__( DWORD__ size = 0 );

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
// name: AudioBuffer__In
// desc: ...
//-----------------------------------------------------------------------------
class AudioBuffer__In : public TickIn, public AudioBuffer__
{
public:
    AudioBuffer__In( DWORD__ size = 0 );
    virtual ~AudioBuffer__In();

public:
    virtual BOOL__ reset();
    virtual BOOL__ tick_in( SAMPLE * in );
    virtual BOOL__ tick_in( SAMPLE * l, SAMPLE * r );
    virtual BOOL__ tick_in( SAMPLE * in, DWORD__ n );
};




//-----------------------------------------------------------------------------
// name: AudioBuffer__Out
// desc: ...
//-----------------------------------------------------------------------------
class AudioBuffer__Out : public TickOut, public AudioBuffer__
{
public:
    AudioBuffer__Out( DWORD__ size = 0 );
    virtual ~AudioBuffer__Out();

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

