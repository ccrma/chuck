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
// name: chuck_audio.h
// desc: chuck host digital audio I/O, using RtAudio (from Gary Scavone)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// RtAudio by: Gary Scavone
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_AUDIO_H__
#define __CHUCK_AUDIO_H__

#include "chuck_def.h"
#include <string>
#include "RtAudio/RtAudio.h"




//-----------------------------------------------------------------------------
// define, defaults, forward references
//-----------------------------------------------------------------------------
#define NUM_CHANNELS_DEFAULT         2       // number of channels
#define NUM_BUFFERS_DEFAULT          8       // number buffers
#define DEVICE_NUM_OUT_DEFAULT       0
#define DEVICE_NUM_IN_DEFAULT        0
// sample rate defaults by platform
#if defined(__PLATFORM_LINUX__)
  #define SAMPLE_RATE_DEFAULT      48000
  #define BUFFER_SIZE_DEFAULT        256
#elif defined(__PLATFORM_MACOSX__)
  #define SAMPLE_RATE_DEFAULT      44100
  #define BUFFER_SIZE_DEFAULT        256
#else
  #define SAMPLE_RATE_DEFAULT      44100
  #define BUFFER_SIZE_DEFAULT        512
#endif




// audio callback definition
typedef void (* f_audio_cb)( SAMPLE * input, SAMPLE * output,
    t_CKUINT numFrames, t_CKUINT numInChans, t_CKUINT numOutChans,
    void * userData );




// real-time watch dog
extern t_CKBOOL g_do_watchdog;
// countermeasure
extern t_CKUINT g_watchdog_countermeasure_priority;
// watchdog timeout
extern t_CKFLOAT g_watchdog_timeout;





//-----------------------------------------------------------------------------
// name: class ChuckAudio
// desc: chuck host audio I/O
//-----------------------------------------------------------------------------
class ChuckAudio
{
public:
    static t_CKBOOL initialize( t_CKUINT num_dac_channels,
                                t_CKUINT num_adc_channels,
                                t_CKUINT sample_rate,
                                t_CKUINT buffer_size,
                                t_CKUINT num_buffers,
                                f_audio_cb callback,
                                void * data,
                                // force_srate added 1.3.1.2
                                t_CKBOOL force_srate );
    static void shutdown();
    static t_CKBOOL start();
    static t_CKBOOL stop();

public: // watchdog stuff
    static t_CKBOOL watchdog_start();
    static t_CKBOOL watchdog_stop();

public: // device info
    // probe audio devices
    static void probe();
    // get device number by name?
    static t_CKUINT device_named( const std::string & name,
                                  t_CKBOOL needs_dac = FALSE,
                                  t_CKBOOL needs_adc = FALSE );

public:
    static t_CKUINT srate() { return m_sample_rate; }
    static t_CKUINT num_channels_out() { return m_num_channels_out; }
    static t_CKUINT num_channels_in() { return m_num_channels_in; }
    static t_CKUINT dac_num() { return m_dac_n; }
    static t_CKUINT adc_num() { return m_adc_n; }
    static t_CKUINT bps() { return m_bps; }
    static t_CKUINT buffer_size() { return m_buffer_size; }
    static t_CKUINT num_buffers() { return m_num_buffers; }
    static RtAudio * audio() { return m_rtaudio; }

    static void set_extern( SAMPLE * in, SAMPLE * out )
        { m_extern_in = in; m_extern_out = out; }
    static int cb( void * output_buffer, void * input_buffer, unsigned int buffer_size,
        double streamTime, RtAudioStreamStatus status, void * user_data );

public: // data
    static t_CKBOOL m_init;
    static t_CKBOOL m_start;
    static t_CKBOOL m_go; // counter of # of times callback called
    static t_CKBOOL m_silent; // if true, will output silence
    static t_CKBOOL m_expand_in_mono2stereo; // 1.4.0.1 for in:1 out:2
    static t_CKUINT m_num_channels_out;
    static t_CKUINT m_num_channels_in;
    static t_CKUINT m_num_channels_max; // the max of out/in
    static t_CKUINT m_sample_rate;
    static t_CKUINT m_bps;
    static t_CKUINT m_buffer_size;
    static t_CKUINT m_num_buffers;
    static SAMPLE * m_buffer_out;
    static SAMPLE * m_buffer_in;
    static SAMPLE * m_extern_in; // for things like audicle
    static SAMPLE * m_extern_out; // for things like audicle

    static RtAudio * m_rtaudio;
    static t_CKUINT m_dac_n;
    static t_CKUINT m_adc_n;
    static f_audio_cb m_audio_cb;
    static void * m_cb_user_data;
};




#endif
