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
// file: midiio_osx.h
// desc: midi io header for osx
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ari Lazier (alazier@cs.princeton.edu)
//-----------------------------------------------------------------------------
#ifndef __MIDI_IO_H__
#define __MIDI_IO_H__


#include <CoreMIDI/CoreMIDI.h>
#include <pthread.h>
#include "util_buffers.h"


#ifndef CALLBACK
#define CALLBACK
#endif


//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
union MidiMsg
{
    BYTE__  data[4];
    DWORD__ dw;
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
    BOOL__ open( int device_num = 0 );
    BOOL__ close();
    const char * get_last_error() const;

public:
    UINT__ send( BYTE__ status );
    UINT__ send( BYTE__ status, BYTE__ data1 );
    UINT__ send( BYTE__ status, BYTE__ data1, BYTE__ data2 );
    UINT__ send( const MidiMsg * msg, DWORD__ length = 3 );
    BOOL__ drain();

protected:
    UINT__ m_device_num;
	MIDIClientRef m_midi_out;
	MIDIEndpointRef m_midi_endpoint;
	MIDIPortRef m_midi_port;
    char m_msg[1024];
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
    BOOL__ open( int device_num = 0 );
    BOOL__ close();
    const char * get_last_error() const;

public:
    UINT__ recv( MidiMsg * msg );

public:
    static void midi_in_cb( const MIDIPacketList * list,
							void * port_data,
							void * connect_data );

protected:
    UINT__ m_device_num;
    CBuffer m_buffer;
	MIDIClientRef m_midi_in;
	MIDIEndpointRef m_midi_endpoint;
	MIDIPortRef m_midi_port;

    pthread_t m_cb_thread_id;
    pthread_mutex_t m_mutex;
    
    char m_msg[1024];
};




#endif
