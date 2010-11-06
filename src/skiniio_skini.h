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
// file: skiniio.h
// desc: midi io header
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#ifndef __SKINI_IO_H__
#define __SKINI_IO_H__

#include "chuck_def.h"
#include "skini.h" // actually, the skini class is in ugen_stk.cpp, so this is wrong.
#include "util_buffers.h"




//-----------------------------------------------------------------------------
// stuff
//-----------------------------------------------------------------------------
/*#define MIDI_NOTEON         0x90
#define MIDI_NOTEOFF        0x80
#define MIDI_POLYPRESS      0xa0
#define MIDI_CTRLCHANGE     0xb0
#define MIDI_PROGCHANGE     0xc0
#define MIDI_CHANPRESS      0xd0
#define MIDI_PITCHBEND      0xe0
#define MIDI_ALLNOTESOFF    0x7b
*/



//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
//struct SkiniMsg
//{
//	//! A message structure to store and pass parsed SKINI messages.
//	long type;                         /*!< The message type, as defined in SKINI.msg. */
//   long channel;                      /*!< The message channel (not limited to 16!). */
//    float time;	                       /*!< The message time stamp in seconds (delta or absolute). */
//    std::vector<float> floatValues;    /*!< The message values read as floats (values are type-specific). */
//    std::vector<long> intValues;       /*!< The message values read as ints (number and values are type-specific). */
//    std::string remainder;             /*!< Any remaining message data, read as ascii text. */
//
//   // Default constructor.
//    SkiniMsg()
//      :type(0), channel(0), time(0.0), floatValues(2), intValues(2) {}
//};

//uh...
//#define SkiniMsg Skini::Message 

struct SkiniMsg
{
	int type;
	int channel;
	float time;
	float data1; // floatValues[0] 
	float data2; // floatValues[1]
};


//-----------------------------------------------------------------------------
// name: class SkiniOut
// desc: midi out
//-----------------------------------------------------------------------------
class SkiniOut
{
public:
    SkiniOut();
    ~SkiniOut();

public:
    t_CKBOOL open( const char * filename = NULL );
    t_CKBOOL close();

public:
    t_CKUINT  send( const SkiniMsg * msg );

/*public:
    t_CKUINT  noteon( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT  noteoff( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  velocity );
    t_CKUINT  polypress( t_CKUINT  channel, t_CKUINT  note, t_CKUINT  pressure );
    t_CKUINT  ctrlchange( t_CKUINT  channel, t_CKUINT  ctrl_num, t_CKUINT  ctrl_val );
    t_CKUINT  progchange( t_CKUINT  channel, t_CKUINT  patch );
    t_CKUINT  chanpress( t_CKUINT  channel, t_CKUINT  pressure );
    t_CKUINT  pitchbend( t_CKUINT  channel, t_CKUINT  bend_val );
    t_CKUINT  allnotesoff( t_CKUINT  channel );
*/
public:
	Skini * skout;
	//std::vector<unsigned char> m_msg;
	char * m_filename;
	t_CKBOOL m_valid;
};




//-----------------------------------------------------------------------------
// name: class SkiniIn
// desc: midi
//-----------------------------------------------------------------------------
class SkiniIn : public Chuck_Event
{
public:
    SkiniIn();
    ~SkiniIn();

public:
    t_CKBOOL open( const char * filename = NULL );
    t_CKBOOL close();

public:
    t_CKUINT  recv( SkiniMsg * msg );

public:
	char * m_filename;
	t_CKBOOL m_valid;
	Skini * skin;

	//CBuffer * m_buffer;
	//t_CKUINT m_read_index;
	//RtMidiIn * min;
	//t_CKUINT m_device_num;
	//t_CKUINT m_ref_count;
	//Chuck_Object * SELF;
    // static void CALLBACK cb_midi_input( HMIDIIN hm_in, t_CKUINT  msg,
    //                  DWORD instance, DWORD param1, DWORD param2 );
};


#endif
