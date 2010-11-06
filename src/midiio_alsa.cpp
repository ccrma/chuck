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
// file: midiio_alsa.cpp
// desc: midi io alsa implementation
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "midiio_alsa.h"
#include <stdio.h>
#include <stdlib.h>




//-----------------------------------------------------------------------------
// name: MidiOut()
// desc: constructor
//-----------------------------------------------------------------------------
MidiOut::MidiOut()
{
    UINT__ m_device_num = 0;
    m_midi_out = NULL;
}




//-----------------------------------------------------------------------------
// name: ~MidiOut()
// desc: destructor
//-----------------------------------------------------------------------------
MidiOut::~MidiOut()
{
    this->close();
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open a device
//-----------------------------------------------------------------------------
BOOL__ MidiOut::open( int device_num )
{
    int err = 0;
    char buffer[256];

    m_device_num = device_num;

    // open the midi
    sprintf( buffer, "hw:0,%d", m_device_num );    
    err = snd_rawmidi_open( NULL, &m_midi_out, buffer, 0 );
    
    return err == 0;
}




//-----------------------------------------------------------------------------
// name: close()
// desc: close the device
//-----------------------------------------------------------------------------
BOOL__ MidiOut::close( )
{
    // send everything in buffer
    snd_rawmidi_drain( m_midi_out );
    
    // close midi out
    snd_rawmidi_close( m_midi_out );

    return true;
}




//-----------------------------------------------------------------------------
// name: drain()
// desc: ...
//-----------------------------------------------------------------------------
BOOL__ MidiOut::drain()
{
    snd_rawmidi_drain( m_midi_out );

    return true;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 1 BYTE__ midi message
//-----------------------------------------------------------------------------
UINT__ MidiOut::send( BYTE__ status )
{
    // send
    snd_rawmidi_write( m_midi_out, &status, 1 );

    return true;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 2 BYTE__ midi message
//-----------------------------------------------------------------------------
UINT__ MidiOut::send( BYTE__ status, BYTE__ data1 )
{
    // send
    snd_rawmidi_write( m_midi_out, &status, 1 );
    snd_rawmidi_write( m_midi_out, &data1, 1 );
    
    return true;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 3 BYTE__ midi message
//-----------------------------------------------------------------------------
UINT__ MidiOut::send( BYTE__ status, BYTE__ data1, BYTE__ data2 )
{
    // send the three BYTE__s
    snd_rawmidi_write( m_midi_out, &status, 1 );
    snd_rawmidi_write( m_midi_out, &data1, 1 );
    snd_rawmidi_write( m_midi_out, &data2, 1 );
    
    return true;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send midi message
//-----------------------------------------------------------------------------
UINT__ MidiOut::send( const MidiMsg * msg )
{
    return this->send( msg->data[0], msg->data[1], msg->data[2] );
}




//-----------------------------------------------------------------------------
// name: MidiIn()
// desc: constructor
//-----------------------------------------------------------------------------
MidiIn::MidiIn()
{
    m_midi_in = NULL;
    m_device_num = 0;
    pthread_mutex_init( &m_mutex, NULL );
}




//-----------------------------------------------------------------------------
// name: ~MidiIn()
// desc: destructor
//-----------------------------------------------------------------------------
MidiIn::~MidiIn( )
{
    this->close();
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
BOOL__ MidiIn::open( int device_num )
{
    int err = 0;
    char buffer[256];

    m_device_num = device_num;

    // open the raw midi
    sprintf( buffer, "hw:0,%d", m_device_num );  
    err = snd_rawmidi_open( &m_midi_in, NULL, buffer, 0 );

    // initialize the buffer
    m_buffer.initialize( 1024, sizeof( MidiMsg ) );

    // thread
    pthread_create( &m_cb_thread_id, NULL, midi_in_cb, this );

    return err == 0;
}




//-----------------------------------------------------------------------------
// name: close()
// desc: close
//-----------------------------------------------------------------------------
BOOL__ MidiIn::close()
{
    snd_rawmidi_drain( m_midi_in );
    snd_rawmidi_close( m_midi_in );

    pthread_cancel( m_cb_thread_id );
    pthread_mutex_destroy( &m_mutex );

    return true;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get message
//-----------------------------------------------------------------------------
UINT__ MidiIn::recv( MidiMsg * msg )
{
    UINT__ r = 0;
    //pthread_mutex_lock( &m_mutex );
    r = m_buffer.get( msg, 1 );
    //pthread_mutex_unlock( &m_mutex );

    return r;
}




//-----------------------------------------------------------------------------
// name: midi_in_cb()
// desc: ...
//-----------------------------------------------------------------------------
void * MidiIn::midi_in_cb( void * arg )
{
    MidiIn * min = (MidiIn *)arg;
    BYTE__ byte = 0;
    int n = 0, num_args = 0, num_left = 0;
    MidiMsg msg;

    while( true )
    {
        // get the next BYTE__
        n = snd_rawmidi_read( min->m_midi_in, &byte, 1 );
        if( n < 0 )
        {
            // encounter error
            fprintf( stdout, "error: rawmidi_read...\n" );
            continue;
        }

        while( n > 0 )
        {
            if( byte & 0x80 ) // status byte
            {
                if( (byte & 0xf0) == 0xf0 ) // system msg
                {
                    n--;
                    continue;
                }
            
                if( ( (byte & 0xf0) == 0xc0 ) || ( (byte & 0xf0) == 0xd0 ) )
                    num_args = 1;
                else
                    num_args = 2;

                msg.data[3] = 0;
                msg.data[2] = byte;
                msg.data[1] = 0;
                msg.data[0] = 0;
                num_left = num_args;
            }
            else // data byte
            {
                if( num_left == num_args )
                    msg.data[1] = byte;
                else
                    msg.data[0] = byte;

                num_left--;

                if( !num_left )
                {
                    if( ((msg.data[2] & 0xf0) == 0xc0) || ((msg.data[2] & 0xf0) == 0xd0) )
                        num_left = 1;
                    else
                        num_left = 2;

                    //pthread_mutex_lock( &min->m_mutex );
                    min->m_buffer.put( &msg, 1 );
                    //pthread_mutex_unlock( &min->m_mutex );
                }
            }
        n--;
        }
    }

    return NULL;
}
