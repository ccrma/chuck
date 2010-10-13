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
// file: skiniio_skini.cpp
// desc: skini io
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <vector>
#include "skiniio_skini.h"
#include "chuck_errmsg.h"




//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
//#define BUFFER_SIZE 8192

//std::vector<RtMidiIn *> MidiInManager::the_mins;
//std::vector<CBuffer *> MidiInManager::the_bufs;
//std::vector<RtMidiOut *> MidiOutManager::the_mouts;



//-----------------------------------------------------------------------------
// name: SkiniOut()
// desc: constructor
//-----------------------------------------------------------------------------
SkiniOut::SkiniOut()
{
	skout = new Skini;
    m_filename = NULL;
	m_valid = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~SkiniOut()
// desc: destructor
//-----------------------------------------------------------------------------
SkiniOut::~SkiniOut()
{
    if( skout ) this->close();
	SAFE_DELETE( skout );
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send skini message
//-----------------------------------------------------------------------------
t_CKUINT SkiniOut::send( const SkiniMsg * msg )
{
	if( !m_valid ) return 0;

	// write message in file
	skout->write_message( msg->type, msg->time, msg->channel, msg->data1, msg->data2 );

	return 3;
}




//-----------------------------------------------------------------------------
// name: open
// desc: open skini output
//-----------------------------------------------------------------------------
t_CKBOOL SkiniOut::open( const char * filename )
{
    // close if already opened
    if( m_valid )
        this->close();
	
	m_valid = skout->setFile( filename );
	//if( m_valid ) strcpy( m_filename, filename );

	return m_valid;
}




//-----------------------------------------------------------------------------
// name: close( )
// desc: close skini output
//-----------------------------------------------------------------------------
t_CKBOOL SkiniOut::close( )
{
    if( !m_valid )
        return FALSE;

    // close
	//MidiOutManager::close( this );

	m_valid = FALSE;
	m_filename = NULL;

	skout->close_file();

    return TRUE;
}



/*
//-----------------------------------------------------------------------------
// name: noteon()
// desc: note on message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::noteon( t_CKUINT channel, t_CKUINT note, t_CKUINT velocity )
{ return this->send( (t_CKBYTE)(MIDI_NOTEON + channel), note, velocity ); }




//-----------------------------------------------------------------------------
// name: noteoff()
// desc: note off message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::noteoff( t_CKUINT channel, t_CKUINT note, t_CKUINT velocity )
{ return this->send( (t_CKBYTE)(MIDI_NOTEOFF + channel), note, velocity ); }




//-----------------------------------------------------------------------------
// name: polypress()
// desc: polypress message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::polypress( t_CKUINT channel, t_CKUINT note, t_CKUINT pressure )
{ return this->send( (t_CKBYTE)(MIDI_POLYPRESS + channel), note, pressure ); }




//-----------------------------------------------------------------------------
// name: ctrlchange()
// desc: ctrl change message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::ctrlchange( t_CKUINT channel, t_CKUINT ctrl_num, t_CKUINT ctrl_val )
{ return this->send( (t_CKBYTE)(MIDI_CTRLCHANGE + channel), ctrl_num, ctrl_val ); }




//-----------------------------------------------------------------------------
// name: progchange()
// desc: prog change message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::progchange( t_CKUINT channel, t_CKUINT patch )
{ return this->send( (t_CKBYTE)(MIDI_PROGCHANGE + channel), patch, 0 ); }




//-----------------------------------------------------------------------------
// name: chanpress()
// desc: chan press
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::chanpress( t_CKUINT channel, t_CKUINT pressure )
{ return this->send( (t_CKBYTE)(MIDI_CHANPRESS + channel), pressure, 0 ); }




//-----------------------------------------------------------------------------
// name: pitchbend()
// desc: pitch bend
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::pitchbend( t_CKUINT channel, t_CKUINT bend_val )
{
	assert( FALSE );
	return 0;
//    return this->send( (t_CKBYTE)(MIDI_PITCHBEND + channel),
//                       (t_CKBYTE)(HIBYTE( bend_val << 1 )),
//                       (t_CKBYTE)(LOBYTE( bend_val & 0x7f )) );
}




//-----------------------------------------------------------------------------
// name: allnotesoff()
// desc: allnotesoff
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::allnotesoff( t_CKUINT channel )
{
    return this->send( (t_CKBYTE)(MIDI_CTRLCHANGE + channel), 
                       (t_CKBYTE)(MIDI_ALLNOTESOFF), 0 );
}
*/



//-----------------------------------------------------------------------------
// name: MidiIn()
// desc: constructor
//-----------------------------------------------------------------------------
SkiniIn::SkiniIn()
{
	m_filename = NULL;
	m_valid = FALSE;
	//m_read_index = 0;
	//m_buffer = NULL;
	//SELF = NULL;
}




//-----------------------------------------------------------------------------
// name: ~SkiniIn()
// desc: destructor
//-----------------------------------------------------------------------------
SkiniIn::~SkiniIn( )
{
    this->close();
	// SAFE_DELETE( min );
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
t_CKBOOL SkiniIn::open( const char * filename )
{
    // close if already opened
    if( m_valid )
        this->close();

	// open
	m_valid = skin->setFile( filename );
	//if( m_valid ) strcpy( m_filename, filename );
	return m_valid;
}



/*
MidiInManager::MidiInManager()
{
	the_mins.resize( 1024 );
	the_bufs.resize( 1024 );
}


MidiInManager::~MidiInManager()
{
	// yeah right
}


t_CKBOOL MidiInManager::open( MidiIn * min, t_CKINT device_num )
{
    // see if port not already open
	if( device_num >= the_mins.capacity() || !the_mins[device_num] )
	{

		// allocate the buffer
		CBuffer * cbuf = new CBuffer;
		if( !cbuf->initialize( BUFFER_SIZE, sizeof(MidiMsg) ) )
		{
			EM_error2( 0, "MidiIn: couldn't allocate CBuffer for port %i...", device_num );
			delete cbuf;
			return FALSE;
		}

		// allocate
		RtMidiIn * rtmin = new RtMidiIn;
		try {
			rtmin->openPort( device_num );
			rtmin->setCallback( cb_midi_input, cbuf );
		} catch( RtError & err ) {
			// print it
			EM_error2( 0, "MidiIn: couldn't open MIDI port %i...", device_num );
			EM_error2( 0, "...(%s)", err.getMessage().c_str() );
			delete cbuf;
			return FALSE;
		}

		// resize?
		if( device_num >= the_mins.capacity() )
		{
			t_CKINT size = the_mins.capacity() * 2;
			if( device_num >= size ) size = device_num + 1;
			the_mins.resize( size );
			the_bufs.resize( size );
		}

		// put cbuf and rtmin in vector for future generations
        the_mins[device_num] = rtmin;
		the_bufs[device_num] = cbuf;
	}

	// found
	min->m_buffer = the_bufs[device_num];
	// get an index into your (you are min here) own buffer, 
	// and a free ticket to your own workshop
	min->m_read_index = min->m_buffer->join( (Chuck_Event *)min->SELF );
    min->m_device_num = (t_CKUINT)device_num;

	// done
	return TRUE;
}
*/


//-----------------------------------------------------------------------------
// name: close()
// desc: close
//-----------------------------------------------------------------------------
t_CKBOOL SkiniIn::close()
{
    if( !m_valid )
        return FALSE;

	// close
	// MidiInManager::close( this );

	m_valid = FALSE;
	m_filename = NULL;
	skin->close_file();

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: get()
// desc: get message
//-----------------------------------------------------------------------------
t_CKUINT SkiniIn::recv( SkiniMsg * msg )
{
	if( !m_valid ) return FALSE;
    //return m_buffer->get( msg, 1, m_read_index );
	
	Skini::Message mess;
	msg->type = skin->nextMessage( mess ); // what????
	msg->time = mess.time;
	msg->channel = mess.channel;
	if( mess.floatValues.size() > 0 )
		msg->data1 = mess.floatValues[0];
	if( mess.floatValues.size() > 1 )
		msg->data2 = mess.floatValues[1];

	return 0;
}
