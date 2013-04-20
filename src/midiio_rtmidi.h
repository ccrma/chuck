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
// file: midiio_rtmidi.h
// desc: midi io header
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#ifndef __MIDI_IO_H__
#define __MIDI_IO_H__

#include "chuck_def.h"
#ifndef __DISABLE_MIDI__
#include "rtmidi.h"
#endif
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
    t_CKBYTE data[4];
//    t_CKUINT dw;
};




// forward reference
class RtMidiOut;
class RtMidiIn;




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
    t_CKBOOL open( t_CKUINT device_num = 0 );
    t_CKBOOL open( const std::string & name );
    t_CKBOOL close();
    t_CKBOOL good() { return m_valid; }
    t_CKINT  num() { return m_valid ? (t_CKINT)m_device_num : -1; }
    
public:
    void     set_suppress( t_CKBOOL print_or_not )
    { m_suppress_output = print_or_not; }
    t_CKBOOL get_suppress()
    { return m_suppress_output; }

public:
    t_CKUINT send( t_CKBYTE status );
    t_CKUINT send( t_CKBYTE status, t_CKBYTE data1 );
    t_CKUINT send( t_CKBYTE status, t_CKBYTE data1, t_CKBYTE data2 );
    t_CKUINT send( const MidiMsg * msg );

public:
    t_CKUINT noteon( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT noteoff( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT polypress( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  pressure );
    t_CKUINT ctrlchange( t_CKUINT  channel, t_CKUINT  ctrl_num, t_CKUINT  ctrl_val );
    t_CKUINT progchange( t_CKUINT  channel, t_CKUINT  patch );
    t_CKUINT chanpress( t_CKUINT  channel, t_CKUINT  pressure );
    t_CKUINT pitchbend( t_CKUINT  channel, t_CKUINT  bend_val );
    t_CKUINT allnotesoff( t_CKUINT  channel );

public:
    RtMidiOut * mout;
    std::vector<unsigned char> m_msg;
    t_CKUINT m_device_num;
    t_CKBOOL m_valid;
    t_CKBOOL m_suppress_output;
};




//-----------------------------------------------------------------------------
// name: class MidiIn
// desc: midi
//-----------------------------------------------------------------------------
class MidiIn : public Chuck_Event
{
public:
    MidiIn();
    ~MidiIn();

public:
    t_CKBOOL open( t_CKUINT device_num = 0 );
    t_CKBOOL open( const std::string & name );
    t_CKBOOL close();
    t_CKBOOL good() { return m_valid; }
    t_CKINT  num() { return m_valid ? (t_CKINT)m_device_num : -1; }

public:
    void     set_suppress( t_CKBOOL print_or_not )
    { m_suppress_output = print_or_not; }
    t_CKBOOL get_suppress()
    { return m_suppress_output; }

public:
    t_CKBOOL empty();
    t_CKUINT recv( MidiMsg * msg );

public:
    CBufferAdvance * m_buffer;
    t_CKUINT m_read_index;
    RtMidiIn * min;
    t_CKBOOL m_valid;
    t_CKUINT m_device_num;
    Chuck_Object * SELF;
    t_CKBOOL m_suppress_output;
};


void probeMidiIn();
void probeMidiOut();


class MidiInManager
{
public:
    static t_CKBOOL open( MidiIn * min, t_CKINT device_num );
    static t_CKBOOL open( MidiIn * min, const std::string & name );
    static t_CKBOOL close( MidiIn * min );

    static void cb_midi_input( double deltatime, std::vector<unsigned char> * msg,
                               void *userData );
protected:
    MidiInManager();
    ~MidiInManager();

    static std::vector<RtMidiIn *> the_mins;
    static std::vector<CBufferAdvance *> the_bufs;
    
    static CBufferSimple * m_event_buffer;
};


class MidiOutManager
{
public:
    static t_CKBOOL open( MidiOut * mout, t_CKINT device_num );
    static t_CKBOOL open( MidiOut * mout, const std::string & name );
    static t_CKBOOL close( MidiOut * mout );

protected:
    MidiOutManager();
    ~MidiOutManager();

    static std::vector<RtMidiOut *> the_mouts;
};


//-----------------------------------------------------------------------------
// name: class MidiRW
// desc: reads and writes midi messages from file
//-----------------------------------------------------------------------------
class MidiRW
{
public:
    MidiRW();
    ~MidiRW();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL read( MidiMsg * msg, t_CKTIME * time );
    t_CKBOOL write( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};

// closes all MidiRW file handles
t_CKBOOL midirw_detach( );


//-----------------------------------------------------------------------------
// name: class MidiMsgOut
// desc: reads midi messages from file
//-----------------------------------------------------------------------------
class MidiMsgOut
{
public:
    MidiMsgOut();
    ~MidiMsgOut();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL write( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};


//-----------------------------------------------------------------------------
// name: class MidiMsgIn
// desc: reads midi messages from file
//-----------------------------------------------------------------------------
class MidiMsgIn
{
public:
    MidiMsgIn();
    ~MidiMsgIn();

public:
    t_CKBOOL open( const char * filename );
    t_CKBOOL close();

public:
    t_CKBOOL read( MidiMsg * msg, t_CKTIME * time );

protected:
    FILE * file;
};


#endif
