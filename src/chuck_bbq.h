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
// file: chuck_bbq.h
// desc: bbq header
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_BBQ_H__
#define __CHUCK_BBQ_H__

// currently ChucK uses RtAudio | __CHUCK_NATIVE_AUDIO__ not in use
#if defined(__CHUCK_NATIVE_AUDIO__)
  #if defined(__LINUX_ALSA__)
    #include "digiio_alsa.h"
    #include "midiio_alsa.h"
  #elif defined(__LINUX_JACK__)
    #include "digiio_jack.h"
    #include "midiio_alsa.h"
  #elif defined(__LINUX_OSS__)
    #include "digiio_oss.h"
    #include "midiio_oss.h"
  #elif defined(__MACOSX_CORE__)
    #include "digiio_osx.h"
    #include "midiio_osx.h"
  #elif defined(__WINDOWS_DS__)
    #include "digiio_win32.h"
    #include "midiio_win32.h"
  #else
    #error "must define one:\
    __LINUX_ALSA__ __LINUX_JACK__ __LINUX_OSS__ __MACOSX_CORE__ __WINDOWS_DS__"
  #endif
#else
  #include "digiio_rtaudio.h"
  #include "midiio_rtmidi.h"

  #if defined(__LINUX_ALSA__)
  #elif defined(__LINUX_JACK__)
  #elif defined(__LINUX_OSS__)
  #elif defined(__MACOSX_CORE__)
  #elif defined(__WINDOWS_DS__)
  #elif defined(__WINDOWS_ASIO__)
  #else
    #error "must define one:\
    __LINUX_ALSA__ __LINUX_JACK__ __LINUX_OSS__ __MACOSX_CORE__ __WINDOWS_DS__"
  #endif
#endif

struct Chuck_VM;



//-----------------------------------------------------------------------------
// name: class BBQ
// desc: ...
//-----------------------------------------------------------------------------
class BBQ
{
public:
    BBQ();
    ~BBQ();

public:
    BOOL__ initialize( DWORD__ num_dac_channels,
                       DWORD__ num_adc_channels,
                       DWORD__ sampling_rate,
                       DWORD__ bps,
                       DWORD__ buffer_size,
                       DWORD__ num_buffers,
                       DWORD__ dac,
                       DWORD__ adc,
                       DWORD__ block,
                       Chuck_VM * vm_ref,
                       BOOL__ rt_audio,
                       void * callback = NULL,
                       void * data = NULL,
                       // force_srate added 1.3.1.2
                       BOOL__ force_srate = FALSE );
    void set_srate( DWORD__ srate );
    void set_bufsize( DWORD__ bufsize );
    void set_numbufs( DWORD__ numbufs );
    void set_inouts( DWORD__ adc, DWORD__ dac );
    void set_chans( DWORD__ ins, DWORD__ outs );
    void shutdown();

public:
    DigitalOut * digi_out();
    DigitalIn  * digi_in();
    MidiOut    * midi_out( DWORD__ index = 0 );
    MidiIn     * midi_in( DWORD__ index = 0 );
    
public:
    DWORD__    in_count( DWORD__ index = 0 )
    { return index < m_max_midi_device ? m_in_count[index] : 0; }
    DWORD__    out_count( DWORD__ index = 0 )
    { return index < m_max_midi_device ? m_out_count[index] : 0;  }

protected:
    DigitalOut * m_digi_out;
    DigitalIn  * m_digi_in;
    MidiOut    ** m_midi_out;
    MidiIn     ** m_midi_in;
    DWORD__    * m_in_count;
    DWORD__    * m_out_count;
    DWORD__    m_max_midi_device;
};




//-----------------------------------------------------------------------------
// name: class RangeMapper
// desc: ...
//-----------------------------------------------------------------------------
class RangeMapper
{
public:
    RangeMapper( SAMPLE low_l, SAMPLE high_l, SAMPLE low_r, SAMPLE high_r );

public:
    virtual SAMPLE map( SAMPLE in );

protected:
    float m_slope;
    float m_inter;

    SAMPLE m_low_l;
    SAMPLE m_high_l;
    SAMPLE m_low_r;
    SAMPLE m_high_r;
};




#endif
