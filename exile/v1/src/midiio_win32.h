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
// file: midiio.h
// desc: midi io header
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#ifndef __MIDI_IO_H__
#define __MIDI_IO_H__

#include <windows.h>
#include <mmsystem.h>
#include "util_buffers.h"




//-----------------------------------------------------------------------------
// stuff
//-----------------------------------------------------------------------------
#define MIDI_NOTEON         0x90
#define MIDI_NOTEOFF        0x80
#define MIDI_POLYPRESS      0xa0
#define MIDI_CTRLCHANGE     0xb0
#define MIDI_PROGCHANGE     0xc0
#define MIDI_CHANPRESS      0xd0
#define MIDI_PITCHBEND      0xe0
#define MIDI_ALLNOTESOFF    0x7b




//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
union MidiMsg
{
    BYTE  data[4];
    DWORD dw;
};




//-----------------------------------------------------------------------------
// name: class MidiOut
// desc: midi out
//-----------------------------------------------------------------------------
class MidiOut
{
public:
    MidiOut();
    ~MidiOut();

public:
    BOOL open( UINT device_num = 0 );
    BOOL close();

public:
    UINT send( BYTE status );
    UINT send( BYTE status, BYTE data1 );
    UINT send( BYTE status, BYTE data1, BYTE data2 );
    UINT send( const MidiMsg * msg );

public:
    UINT noteon( UINT channel, UINT note, UINT velocity );
    UINT noteoff( UINT channel, UINT note, UINT velocity );
    UINT polypress( UINT channel, UINT note, UINT pressure );
    UINT ctrlchange( UINT channel, UINT ctrl_num, UINT ctrl_val );
    UINT progchange( UINT channel, UINT patch );
    UINT chanpress( UINT channel, UINT pressure );
    UINT pitchbend( UINT channel, UINT bend_val );
    UINT allnotesoff( UINT channel );

public:
    static void CALLBACK cb_midi_output( HMIDIOUT hm_out, UINT msg,
                         DWORD instance, DWORD param1, DWORD param2 );

protected:
    UINT m_device_num;
    HMIDIOUT m_midi_out;
};




//-----------------------------------------------------------------------------
// name: class MidiIn
// desc: midi
//-----------------------------------------------------------------------------
class MidiIn
{
public:
    MidiIn();
    ~MidiIn();

public:
    BOOL open( UINT device_num = 0 );
    BOOL close();

public:
    UINT recv( MidiMsg * msg );

public:
    static void CALLBACK cb_midi_input( HMIDIIN hm_in, UINT msg,
                      DWORD instance, DWORD param1, DWORD param2 );

protected:
    UINT m_device_num;
    HMIDIIN m_midi_in;
    CBuffer m_buffer;
};




#endif
