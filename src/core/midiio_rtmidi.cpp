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
// file: midiio_rtmidi.cpp
// desc: midi io header
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#include "midiio_rtmidi.h"

#ifndef __DISABLE_MIDI__

#include "chuck_errmsg.h"
#include "chuck_vm.h"
#include <vector>
#include <map>
#include <fstream>




//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
#define MIDI_BUFFER_SIZE 8192

std::vector<RtMidiIn *> MidiInManager::the_mins;
std::vector<CBufferAdvance *> MidiInManager::the_bufs;
std::vector<RtMidiOut *> MidiOutManager::the_mouts;
std::map< Chuck_VM *, CBufferSimple * > MidiInManager::m_event_buffers;




//-----------------------------------------------------------------------------
// name: MidiOut()
// desc: constructor
//-----------------------------------------------------------------------------
MidiOut::MidiOut()
{
    mout = NULL;
    m_device_num = 0;
    m_valid = FALSE;
    m_suppress_output = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~MidiOut()
// desc: destructor
//-----------------------------------------------------------------------------
MidiOut::~MidiOut()
{
    if( mout ) this->close();
//    SAFE_DELETE( mout );
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 1 byte midi message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::send( t_CKBYTE status )
{
    return this->send( status, 0, 0 );
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 2 byte midi message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::send( t_CKBYTE status, t_CKBYTE data1 )
{
    return this->send( status, data1, 0 );
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send 3 byte midi message
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::send( t_CKBYTE status, t_CKBYTE data1, t_CKBYTE data2 )
{
    if( !m_valid ) return 0;

    // clear
    m_msg.clear();
    // add
    m_msg.push_back( status );
    m_msg.push_back( data1 );
    m_msg.push_back( data2 );

    mout->sendMessage( &m_msg );

    return 3;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send
//-----------------------------------------------------------------------------
t_CKUINT MidiOut::send( const MidiMsg * msg )
{
    if( !m_valid ) return 0;

    // clear
    m_msg.clear();
    // add
    m_msg.push_back( msg->data[0] );
    m_msg.push_back( msg->data[1] );
    m_msg.push_back( msg->data[2] );

    mout->sendMessage( &m_msg );

    return 3;
}




//-----------------------------------------------------------------------------
// name: open
// desc: open midi output
//-----------------------------------------------------------------------------
t_CKBOOL MidiOut::open( t_CKUINT device_num )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    return m_valid = MidiOutManager::open( this, (t_CKINT)device_num );
}




//-----------------------------------------------------------------------------
// name: open
// desc: open midi output
//-----------------------------------------------------------------------------
t_CKBOOL MidiOut::open( const std::string & name )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    return m_valid = MidiOutManager::open( this, name );
}




//-----------------------------------------------------------------------------
// name: close( )
// desc: close midi output
//-----------------------------------------------------------------------------
t_CKBOOL MidiOut::close( )
{
    if( !m_valid )
        return FALSE;

    // close
    // MidiOutManager::close( this );

    m_valid = FALSE;

    return TRUE;
}




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




//-----------------------------------------------------------------------------
// name: MidiIn()
// desc: constructor
//-----------------------------------------------------------------------------
MidiIn::MidiIn()
{
    min = NULL;
    m_device_num = 0;
    m_valid = FALSE;
    m_read_index = 0;
    m_buffer = NULL;
    m_suppress_output = FALSE;
    SELF = NULL;
}




//-----------------------------------------------------------------------------
// name: ~MidiIn()
// desc: destructor
//-----------------------------------------------------------------------------
MidiIn::~MidiIn( )
{
    this->close();
    // SAFE_DELETE( min );
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
t_CKBOOL MidiIn::open( Chuck_VM * vm, t_CKUINT device_num )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    // open
    return m_valid = MidiInManager::open( this, vm, (t_CKINT)device_num );
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
t_CKBOOL MidiIn::open( Chuck_VM * vm, const std::string & name )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    // open
    return m_valid = MidiInManager::open( this, vm, name );
}




MidiInManager::MidiInManager()
{
    the_mins.resize( 1024 );
    the_bufs.resize( 1024 );
}


MidiInManager::~MidiInManager()
{
    // yeah right
    //vm->destroy_event_buffer( m_event_buffer );
}


t_CKBOOL MidiInManager::open( MidiIn * min, Chuck_VM * vm, t_CKINT device_num )
{
    // see if port not already open
    if( device_num >= (t_CKINT)the_mins.capacity() || !the_mins[device_num] )
    {
        if( m_event_buffers.count( vm ) == 0 )
        {
            m_event_buffers[vm] = vm->create_event_buffer();
        }
        
        // allocate the buffer
        CBufferAdvance * cbuf = new CBufferAdvance;
        if( !cbuf->initialize( MIDI_BUFFER_SIZE, sizeof(MidiMsg), m_event_buffers[vm] ) )
        {
            if( !min->m_suppress_output )
                EM_error2( 0, "MidiIn: couldn't allocate CBuffer for port %i...", device_num );
            delete cbuf;
            return FALSE;
        }

        // allocate
        RtMidiIn * rtmin = new RtMidiIn;
        try {
            rtmin->openPort( device_num );
            rtmin->setCallback( cb_midi_input, cbuf );
        } catch( RtMidiError & err ) {
            if( !min->m_suppress_output )
            {
                // print it
                EM_error2( 0, "MidiIn: couldn't open MIDI port %i...", device_num );
                err.getMessage();
                // const char * e = err.getMessage().c_str();
                // EM_error2( 0, "...(%s)", err.getMessage().c_str() );
            }
            delete cbuf;
            return FALSE;
        }

        rtmin->ignoreTypes(false, false, false);

        // resize?
        if( device_num >= (t_CKINT)the_mins.capacity() )
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

    // set min
    min->min = the_mins[device_num];
    // found
    min->m_buffer = the_bufs[device_num];
    // get an index into your (you are min here) own buffer, 
    // and a free ticket to your own workshop
    min->m_read_index = min->m_buffer->join( (Chuck_Event *)min->SELF );
    min->m_device_num = (t_CKUINT)device_num;

    // done
    return TRUE;
}




t_CKBOOL MidiInManager::open( MidiIn * min, Chuck_VM * vm, const std::string & name )
{
    t_CKINT device_num = -1;
    
    try 
    {
        RtMidiIn * rtmin = new RtMidiIn;
        
        t_CKINT count = rtmin->getPortCount();
        for(t_CKINT i = 0; i < count; i++)
        {
            std::string port_name = rtmin->getPortName( i );
            if( port_name == name)
            {
                device_num = i;
                break;
            }
        }
        
        if( device_num == -1 )
        {
            // search by substring
            for(t_CKINT i = 0; i < count; i++)
            {
                std::string port_name = rtmin->getPortName( i );
                if( port_name.find( name ) != std::string::npos )
                {
                    device_num = i;
                    break;
                }
            }
        }
    }
    catch( RtMidiError & err )
    {
        if( !min->m_suppress_output )
        {
            // print it
            EM_error2( 0, "MidiOut: error locating MIDI port named %s", name.c_str() );
            err.getMessage();
            // const char * e = err.getMessage().c_str();
            // EM_error2( 0, "...(%s)", err.getMessage().c_str() );
        }
        return FALSE;
    }
    
    if(device_num == -1)
    {
        EM_error2( 0, "MidiOut: error locating MIDI port named %s", name.c_str() );
        return FALSE;
    }
    
    t_CKBOOL result = open( min, vm, device_num );
    
    return result;
}




//-----------------------------------------------------------------------------
// name: cleanup_buffer()
// desc: cleanup buffer
//-----------------------------------------------------------------------------
void MidiInManager::cleanup_buffer( Chuck_VM * vm )
{
    if( m_event_buffers.count( vm ) > 0 )
    {
        vm->destroy_event_buffer( m_event_buffers[vm] );
        m_event_buffers.erase( vm );
    }
}




//-----------------------------------------------------------------------------
// name: close()
// desc: close
//-----------------------------------------------------------------------------
t_CKBOOL MidiIn::close()
{
    if( !m_valid )
        return FALSE;

    // close
    // MidiInManager::close( this );

    m_valid = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: empty()
// desc: is empty?
//-----------------------------------------------------------------------------
t_CKBOOL MidiIn::empty()
{
    if( !m_valid ) return TRUE;
    return m_buffer->empty( m_read_index );
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get message
//-----------------------------------------------------------------------------
t_CKUINT MidiIn::recv( MidiMsg * msg )
{
    if( !m_valid ) return FALSE;
    return m_buffer->get( msg, 1, m_read_index );
}




//-----------------------------------------------------------------------------
// name: cb_midi_output
// desc: call back
//-----------------------------------------------------------------------------
void MidiInManager::cb_midi_input( double deltatime, std::vector<unsigned char> * msg,
                                   void * userData )
{
    unsigned int nBytes = msg->size();
    CBufferAdvance * cbuf = (CBufferAdvance *)userData;
    MidiMsg m;
    if( nBytes >= 1 ) m.data[0] = msg->at(0);
    if( nBytes >= 2 ) m.data[1] = msg->at(1);
    if( nBytes >= 3 ) m.data[2] = msg->at(2);

    // put in the buffer, make sure not active sensing
    if( m.data[2] != 0xfe )
    {
        cbuf->put( &m, 1 );
    }
}




//-----------------------------------------------------------------------------
// name: probeMidiIn()
// desc: ...
//-----------------------------------------------------------------------------
void probeMidiIn()
{
    RtMidiIn * min = NULL;

    try {
        min = new RtMidiIn;;
    } catch( RtMidiError & err ) {
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return;
    }

    // get num
    t_CKUINT num = min->getPortCount();
    EM_error2b( 0, "------( chuck -- %i MIDI inputs )------", num );
    EM_reset_msg();
    
    std::string s;
    for( t_CKUINT i = 0; i < num; i++ )
    {
        try { s = min->getPortName( i ); }
        catch( RtMidiError & err )
        { err.printMessage(); return; }
        EM_error2b( 0, "    [%i] : \"%s\"", i, s.c_str() );
        
        EM_reset_msg();
    }
}




//-----------------------------------------------------------------------------
// name: probeMidiOut()
// desc: ...
//-----------------------------------------------------------------------------
void probeMidiOut()
{
    RtMidiOut * mout =  NULL;

    try {
        mout = new RtMidiOut;
    } catch( RtMidiError & err ) {
        EM_error2b( 0, "%s", err.getMessage().c_str() );
        return;
    }

    // get num
    t_CKUINT num = mout->getPortCount();
    EM_error2b( 0, "------( chuck -- %i MIDI outputs )-----", num );
    std::string s;
    for( t_CKUINT i = 0; i < num; i++ )
    {
        try { s = mout->getPortName( i ); }
        catch( RtMidiError & err )
        { err.printMessage(); return; }
        EM_error2b( 0, "    [%i] : \"%s\"", i, s.c_str() );
    }
}


MidiOutManager::MidiOutManager()
{
    the_mouts.resize( 1024 );
}


MidiOutManager::~MidiOutManager()
{
    // yeah right
}


t_CKBOOL MidiOutManager::open( MidiOut * mout, t_CKINT device_num )
{
    // see if port not already open
    if( device_num >= (t_CKINT)the_mouts.capacity() || !the_mouts[device_num] )
    {
        // allocate
        RtMidiOut * rtmout = new RtMidiOut;
        try {
            rtmout->openPort( device_num );
        } catch( RtMidiError & err ) {
            if( !mout->m_suppress_output )
            {
                // print it
                EM_error2( 0, "MidiOut: couldn't open MIDI port %i...", device_num );
                err.getMessage();
                // const char * e = err.getMessage().c_str();
                // EM_error2( 0, "...(%s)", err.getMessage().c_str() );
            }
            return FALSE;
        }

        // resize?
        if( device_num >= (t_CKINT)the_mouts.capacity() )
        {
            t_CKINT size = the_mouts.capacity() * 2;
            if( device_num >= size ) size = device_num + 1;
            the_mouts.resize( size );
        }

        // put rtmout in vector for future generations
        the_mouts[device_num] = rtmout;
    }

    // found (always) (except when it doesn't get here)
    mout->mout = the_mouts[device_num];
    mout->m_device_num = (t_CKUINT)device_num;

    // done
    return TRUE;
}


t_CKBOOL MidiOutManager::open( MidiOut * mout, const std::string & name )
{
    t_CKINT device_num = -1;
    
    try 
    {
        RtMidiOut * rtmout = new RtMidiOut;
        
        t_CKINT count = rtmout->getPortCount();
        for(t_CKINT i = 0; i < count; i++)
        {
            std::string port_name = rtmout->getPortName( i );
            if( port_name == name )
            {
                device_num = i;
                break;
            }
        }
        
        if( device_num == -1 )
        {
            // search by substring
            for(t_CKINT i = 0; i < count; i++)
            {
                std::string port_name = rtmout->getPortName( i );
                if( port_name.find( name ) != std::string::npos )
                {
                    device_num = i;
                    break;
                }
            }
        }
    }
    catch( RtMidiError & err )
    {
        if( !mout->m_suppress_output )
        {
            // print it
            EM_error2( 0, "MidiOut: error locating MIDI port named %s", name.c_str() );
            err.getMessage();
            // const char * e = err.getMessage().c_str();
            // EM_error2( 0, "...(%s)", err.getMessage().c_str() );
        }
        return FALSE;
    }
    
    if(device_num == -1)
    {
        EM_error2( 0, "MidiOut: error locating MIDI port named %s", name.c_str() );
        return FALSE;
    }
    
    t_CKBOOL result = open( mout, device_num );
    
    return result;
}


//-----------------------------------------------------------------------------
// name: class MidiRW
// desc: reads and writes midi messages from file
//-----------------------------------------------------------------------------

static std::map<MidiRW *, MidiRW *> g_rw;

t_CKBOOL out_detach( );
t_CKBOOL midirw_detach( )
{
    std::map<MidiRW *, MidiRW *>::iterator iter;
    std::vector<MidiRW *> list;

    // log
    EM_log( CK_LOG_INFO, "detaching MIDI devices..." );

    for( iter = g_rw.begin(); iter != g_rw.end(); iter++ )
        list.push_back( (*iter).second );
    for( t_CKUINT i = 0; i < list.size(); i++ )
        list[i]->close();
    
    // TODO: release the MidiRW
    g_rw.clear();

    return out_detach( );
}

MidiRW::MidiRW() { file = NULL; }

MidiRW::~MidiRW() { this->close(); }

t_CKBOOL MidiRW::open( const char * filename )
{
    this->close();

    file = fopen( filename, "rb+" );
    if( file == NULL )
    {
        file = fopen( filename, "wb+" );
    }

    // add to hash
    g_rw[this] = this;

    return ( file != NULL );
}

t_CKBOOL MidiRW::close()
{
    if( !file ) return FALSE;

    t_CKBOOL value = fclose( file ) == 0;
    
    // remove from hash
    std::map<MidiRW *, MidiRW *>::iterator iter;
    iter = g_rw.find( this );
    g_rw.erase( iter );

    file = NULL;

    return value;
}

t_CKBOOL MidiRW::read( MidiMsg * msg, t_CKTIME * time )
{
    if( !file )
        return FALSE;

    // is it open? i don't know...
    
    t_CKBOOL m, t;
    
    // wouldn't it be cool if this worked?
    m = fread( msg, sizeof(MidiMsg), 1, file );
    t = fread( time, sizeof(t_CKTIME), 1, file );
    
    return m && t;
}


t_CKBOOL MidiRW::write( MidiMsg * msg, t_CKTIME * time )
{
    if( !file )
        return FALSE;

    t_CKBOOL m, t;

    m = fwrite( msg, sizeof(MidiMsg), 1, file );
    t = fwrite( time, sizeof(t_CKTIME), 1, file );
    fflush( file );

    return m && t;
}




//-----------------------------------------------------------------------------
// name: class MidiMsgOut
// desc: writes midi messages from file
//-----------------------------------------------------------------------------

static std::map<MidiMsgOut *, MidiMsgOut *> g_out;

t_CKBOOL out_detach( )
{
    std::map<MidiMsgOut *, MidiMsgOut *>::iterator iter;
    std::vector<MidiMsgOut *> list;

    for( iter = g_out.begin(); iter != g_out.end(); iter++ )
        list.push_back( (*iter).second );
    for( t_CKUINT i = 0; i < list.size(); i++ )
        list[i]->close();
    
    return TRUE;
}

MidiMsgOut::MidiMsgOut() { file = NULL; }

MidiMsgOut::~MidiMsgOut() { this->close(); }

t_CKBOOL MidiMsgOut::open( const char * filename )
{
    this->close();

    file = fopen( filename, "wb" );

    // add to hash
    g_out[this] = this;

    return ( file != NULL );
}

t_CKBOOL MidiMsgOut::close()
{
    if( !file ) return FALSE;

    t_CKBOOL value = fclose( file ) == 0;
    
    // remove from hash
    std::map<MidiMsgOut *, MidiMsgOut *>::iterator iter;
    iter = g_out.find( this );
    g_out.erase( iter );

    file = NULL;

    return value;
}

t_CKBOOL MidiMsgOut::write( MidiMsg * msg, t_CKTIME * time )
{
    if( !file )
        return FALSE;

    t_CKBOOL m, t;

    m = fwrite( msg, sizeof(MidiMsg), 1, file );
    t = fwrite( time, sizeof(t_CKTIME), 1, file );
    fflush( file );

    return m && t;
}




//-----------------------------------------------------------------------------
// name: class MidiMsgIn
// desc: reads midi messages from file
//-----------------------------------------------------------------------------

MidiMsgIn::MidiMsgIn() { file = NULL; }

MidiMsgIn::~MidiMsgIn() { this->close(); }

t_CKBOOL MidiMsgIn::open( const char * filename )
{
    this->close();

    file = fopen( filename, "rb" );

    return ( file != NULL );
}

t_CKBOOL MidiMsgIn::close()
{
    if( !file ) return FALSE;

    t_CKBOOL value = fclose( file ) == 0;
    
    file = NULL;

    return value;
}

t_CKBOOL MidiMsgIn::read( MidiMsg * msg, t_CKTIME * time )
{
    if( !file )
        return FALSE;

    // is it open? i don't know...
    
    t_CKBOOL m, t;
    
    // wouldn't it be cool if this worked?
    m = fread( msg, sizeof(MidiMsg), 1, file );
    t = fread( time, sizeof(t_CKTIME), 1, file );
    
    return m && t;
}

#else // __DISABLE_MIDI__

MidiOut::MidiOut()
{
    
}

MidiOut::~MidiOut()
{
}

t_CKBOOL MidiOut::open( t_CKUINT device_num )
{
    return TRUE;
}

MidiIn::MidiIn()
{
    
}

MidiIn::~MidiIn()
{
    
}

t_CKBOOL MidiIn::open( t_CKUINT device_num )
{
    return FALSE;
}

#endif // __DISABLE_MIDI__

//-----------------------------------------------------------------------------
// name: adapted from y-score-reader.cpp
// desc: reads song scores
//
// authors: Ge Wang (ge@ccrma.stanford.edu)
//    date: Winter 2010
//    version: 1.0
//-----------------------------------------------------------------------------
#include <iostream>
#include "midiio_rtmidi.h"
#include "ugen_stk.h"

using namespace stk;
using namespace std;




#pragma mark - MIDI helpers


//-----------------------------------------------------------------------------
// name: isNoteOff()
// desc: ...
//-----------------------------------------------------------------------------
static bool isNoteOff( std::vector<unsigned char> & shuttle )
{
    if( shuttle.size() >= 1 && shuttle[0] >> 4 == 0x8 ) // note off event
        return true;
    
    if( shuttle.size() >= 3 && shuttle[0] >> 4 == 0x9 && shuttle[2] == 0 ) // note on event with velocity 0
        return true;
    
    return false;
}




//-----------------------------------------------------------------------------
// name: isControl()
// desc: ...
//-----------------------------------------------------------------------------
static bool isControl( std::vector<unsigned char> & shuttle )
{
    if( shuttle.size() >= 1 && ( shuttle[0] & 0xf0 ) == 0xb0 )
        return true;
    
    return false;
}




//-----------------------------------------------------------------------------
// name: isMeta()
// desc: ...
//-----------------------------------------------------------------------------
static bool isMeta( std::vector<unsigned char> & shuttle )
{
    if( shuttle.size() >= 1 && shuttle[0] == 0xff )
        return true;
    
    return false;
}




//-----------------------------------------------------------------------------
// name: isProgram()
// desc: ...
//-----------------------------------------------------------------------------
static bool isProgram( std::vector<unsigned char> & shuttle )
{
    if( shuttle.size() >= 1 && ( shuttle[0] >= 0xC0 && shuttle[0] <= 0xCF ) )
        return true;
    
    return false;
}




//-----------------------------------------------------------------------------
// name: MidiScoreReader()
// desc: constructor
//-----------------------------------------------------------------------------
MidiScoreReader::MidiScoreReader()
: m_midiFile(NULL), m_velocity_scale(0.0f), m_numNonZeroTracks(0)
{
}




//-----------------------------------------------------------------------------
// name: ~MidiScoreReader()
// desc: destructor
//-----------------------------------------------------------------------------
MidiScoreReader::~MidiScoreReader()
{
    // clean up
    cleanup();
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: reclaim
//-----------------------------------------------------------------------------
void MidiScoreReader::cleanup()
{
    // clean up
    if( m_midiFile )
    {
        // pointer to event
        MidiNoteEvent * e = NULL;
        MidiNoteEvent * next = NULL;
        
        // clean up the tracks
        for( long i = 0; i < m_midiFile->getNumberOfTracks(); i++ )
        {
            // for each track
            for( long j = 0; j < m_events[i].size(); j++ )
            {
                // get track
                e = m_events[i][j];
                // delete simultaneous events
                while( e )
                {
                    // get next
                    next = e->simultaneous;
                    // delete current
                    delete e;
                    // advance
                    e = next;
                }
            }
            
            // clear the vectors
            m_events[i].clear();
        }
        
        // delete midiFile
        delete m_midiFile;
        m_midiFile = NULL;
        
        // clean the vector
        m_events.clear();
        m_countMaps.clear();
        m_nonZeroTrackIndices.clear();
        
        // clear the queue
        while( m_queue.size() > 0 )
            m_queue.pop_front();
    }
}




//-----------------------------------------------------------------------------
// name: load()
// desc: load a MIDI file
//-----------------------------------------------------------------------------
bool MidiScoreReader::load( const char * path, float velScale )
{
    // sanity check
    if( m_midiFile ) cleanup();
    
    // open midi file
    try
    {
        m_midiFile = new MidiFileIn( path );
        // print a little information about the file.
        CK_STDCERR << "[chuck]: MIDI file: " << path << CK_STDENDL;
        CK_STDCERR << "[chuck]:   | format = " << m_midiFile->getFileFormat() << CK_STDENDL;
        CK_STDCERR << "[chuck]:   | tracks = " << m_midiFile->getNumberOfTracks() << CK_STDENDL;
        CK_STDCERR << "[chuck]:   | seconds / ticks = " << m_midiFile->getTickSeconds() << CK_STDENDL;
        CK_STDCERR << "[chuck]:   | BPM = " << m_midiFile->getBPM() << CK_STDENDL;
    }
    catch ( StkError & e )
    {
        // you might want to do something more useful here.
        CK_STDCERR << "[chuck]: error loading MIDI file: " << path << CK_STDENDL;
        // delete
        cleanup();
        
        return false;
    }
    
    // set velocity scale
    m_velocity_scale = velScale;
    // reset
    m_numNonZeroTracks = 0;
    // clear
    m_nonZeroTrackIndices.clear();
    
    // allocate element for vector
    m_events.resize( m_midiFile->getNumberOfTracks() );
    m_lyricEvents.resize( m_midiFile->getNumberOfTracks() );
    m_indices.resize( m_midiFile->getNumberOfTracks() );
    m_countMaps.resize( m_midiFile->getNumberOfTracks() );
    // iterate and set
    for( long i = 0; i < m_midiFile->getNumberOfTracks(); i++ )
    {
        // load up the arrays
        loadTrack( i, m_events[i], m_lyricEvents[i] );
        // log
        string name = getTrackName( i );
        CK_FPRINTF_STDERR( "[chuck](MIDI file): track-%ld (%s): events: %lu low: %lu high: %lu\n",
               i, name.c_str(), m_events[i].size(), getLowestNote( i, NULL ), getHighestNote( i, NULL ) );
        // set indice
        m_indices[i] = 0;
        // add search pointer
        m_searchIndices.push_back( 0 );
        // count non zero
        if( m_events[i].size() )
        {
            // increment
            m_numNonZeroTracks++;
            // append
            m_nonZeroTrackIndices.push_back( i );
        }
    }
    
    return true;
}




//-----------------------------------------------------------------------------
// name: getNumTracks()
// desc: get the number of tracks
//-----------------------------------------------------------------------------
long MidiScoreReader::getNumTracks() const
{
    if( m_midiFile == NULL ) return 0;
    return m_midiFile->getNumberOfTracks();
}




//-----------------------------------------------------------------------------
// name: getNumTracksNonZero()
// desc: get number of tracks with more than 0 zero
//-----------------------------------------------------------------------------
long MidiScoreReader::getNumTracksNonZero() const
{
    if( m_midiFile == NULL ) return 0;
    return m_numNonZeroTracks;
}




//-----------------------------------------------------------------------------
// name: getTracksNonZero()
// desc: get a vector of tracks indices that have more than 0 events
//-----------------------------------------------------------------------------
const std::vector<long> & MidiScoreReader::getTracksNonZero() const
{
    return m_nonZeroTrackIndices;
}




//-----------------------------------------------------------------------------
// name: getBPM()
// desc: get BPM
//-----------------------------------------------------------------------------
double MidiScoreReader::getBPM() const
{
    if( m_midiFile == NULL ) return 0;
    return m_midiFile->getBPM();
}




//-----------------------------------------------------------------------------
// name: getLowestNote()
// desc: get lowest note for a track
//-----------------------------------------------------------------------------
long MidiScoreReader::getLowestNote( long track, const MidiNoteEvent * start )
{
    int lowestNote = 128;
    int startingPoint = 0;
    
    // get the starting point
    if( start != NULL )
    {
        for( int i = 0; i < m_events[track].size(); i++ )
        {
            if( m_events[track].at(i) == start )
            {
                startingPoint = i + 1;
                break;
            }
        }
    }
    
    for( int i = startingPoint; i < m_events[track].size(); i++ )
    {
        if( m_events[track].at(i)->data2 < lowestNote )
            lowestNote = m_events[track].at(i)->data2;
        
        if( m_events[track].at(i)->phrasemark ) break;
    }
    
    return lowestNote < 128 ? lowestNote : 0;
}




//-----------------------------------------------------------------------------
// name: getHighestNote()
// desc: get highest note for a track
//-----------------------------------------------------------------------------
long MidiScoreReader::getHighestNote( long track, const MidiNoteEvent *start )
{
    long highestNote = 0;
    long startingPoint = 0;
    
    // get the starting point
    if( start != NULL )
    {
        for( long i = 0; i < m_events[track].size(); i++ )
        {
            if( m_events[track].at(i) == start )
            {
                startingPoint = i+1;
                break;
            }
        }
    }
    
    for( long i = startingPoint; i < m_events[track].size(); i++ )
    {
        if( m_events[track].at(i)->data2 > highestNote )
            highestNote = m_events[track].at(i)->data2;
        
        if( m_events[track].at(i)->phrasemark ) break;
    }
    
    return highestNote;
}




//-----------------------------------------------------------------------------
// name: getTopEvent()
// desc: return current (top-level) note event on track
//-----------------------------------------------------------------------------
const MidiNoteEvent * MidiScoreReader::getTopEvent( long track, long offset )
{
    // sanity check
    if( !m_midiFile || track >= m_events.size() )
    {
        CK_STDCERR << "[chuck](MIDI file): cannot access top event data!" << CK_STDENDL;
        return NULL;
    }
    
    // the new index
    long index = m_indices[track] + offset;
    // check it
    if( index < 0 || index >= m_events[track].size() ) return NULL;
    // return it
    return m_events[track][index];
}




//-----------------------------------------------------------------------------
// name: scanEvent()
// desc: get the next event (respects simultaneous) + move to next
//-----------------------------------------------------------------------------
const MidiNoteEvent * MidiScoreReader::scanEvent( long track, bool & isNewSet )
{
    const MidiNoteEvent * e = NULL;
    isNewSet = false;
    // see if there is last event
    if( ( e = front() ) )
    {
        // pop it
        dequeue();
        // get simulaneous
        e = e->simultaneous;
    }
    // see if still NULL
    if( !e )
    {
        // get current
        e = getTopEvent( track );
        // set flag
        isNewSet = true;
    }
    // set the next event
    enqueue( e );
    
    return e;
}




//-----------------------------------------------------------------------------
// name: seek()
// desc: advance to an event on a track
//-----------------------------------------------------------------------------
bool MidiScoreReader::seek( long track, long numToAdvance )
{
    // sanity check
    if( !m_midiFile || track >= m_events.size() )
    {
        // CK_STDCERR << "[chuck](MIDI file): cannot seek current data!" << CK_STDENDL;
        return false;
    }
    
    // the next index
    long index = m_indices[track] + numToAdvance;
    // check it
    if( index < 0 ) index = 0;
    else if( index >= m_events[track].size() ) index = m_events[track].size();
    // update
    m_indices[track] = index;
    
    // return
    if( index >= m_events[track].size() ) return false;
    return true;
}




//-----------------------------------------------------------------------------
// name: seekToNoteOn()
// desc: advance to an event on a track
//-----------------------------------------------------------------------------
bool MidiScoreReader::seekToNoteOn( long track )
{
    // sanity check
    if( !m_midiFile || track >= m_events.size() )
    {
        // CK_STDCERR << "[chuck](MIDI file): cannot seek current data!" << CK_STDENDL;
        return false;
    }
    
    // event
    const MidiNoteEvent * e = NULL;
    long index = 0;
    
    // go
    do {
        // the next index
        index = m_indices[track] + 1;
        // check it
        if( index < 0 ) index = 0;
        else if( index >= m_events[track].size() ) index = m_events[track].size();
        // update
        m_indices[track] = index;
        // break
        if( index >= m_events[track].size() ) break;
        // set the e
        e = m_events[track][index];
    } while( (e->data1 >> 4 != 0x9) || (e->data3 == 0) );
    
    // return
    if( index >= m_events[track].size() ) return false;
    return true;
}




//-----------------------------------------------------------------------------
// name: rewind()
// desc: go to begining
//-----------------------------------------------------------------------------
void MidiScoreReader::rewind()
{
    // sanity check
    if( !m_midiFile ) return;
    
    // set all indices to 0
    for( long i = 0; i < m_midiFile->getNumberOfTracks(); i++ )
    {
        // clear
        m_indices[i] = 0;
        m_searchIndices[i] = 0;
    }
}




//-----------------------------------------------------------------------------
// name: getEvents()
// desc: get notes in a time window
//-----------------------------------------------------------------------------
void MidiScoreReader::getEvents( long track, double startTime, double endTime,
                                 vector<const MidiNoteEvent *> & result,
                                 bool includeSimultaneous )
{
    // clear the result
    result.clear();
    
    // sanity check
    if( !m_midiFile || track >= m_events.size() )
    {
        // don't alway print this
        // CK_STDCERR << "[chuck](MIDI file): cannot read track: " << track << "!" << CK_STDENDL;
        return;
    }
    
    // get the event
    const MidiNoteEvent * searchPtr = getTopEvent( track, m_searchIndices[track] );
    
    // advance the search pointer
    while( searchPtr && searchPtr->endTime < startTime )
    {
        // TODO: this doesn't handle the case where simultaneous note ends later than parent
        
        // increment index
        m_searchIndices[track]++;
        // get event
        searchPtr = getTopEvent( track, m_searchIndices[track] );
    }
    
    // check
    if( searchPtr == NULL )
        return;
    
    // the index
    unsigned long searchIndex = m_searchIndices[track];
    
    // scan for events within the window
    while( searchPtr )
    {
        // within window - this is some sexy logic, thanks Jieun and Tom
        // ge: added the <= for endTime
        if( searchPtr->endTime > startTime && searchPtr->startTime <= endTime )
        {
            // add to vector
            result.push_back( searchPtr );
        }
        
        // check
        if( includeSimultaneous && searchPtr->simultaneous )
        {
            // move to simultaneous - could have different end time
            searchPtr = searchPtr->simultaneous;
        }
        else // to the next parent (top level event)
        {
            // advance search index
            searchIndex++;
            // get event
            searchPtr = getTopEvent( track, searchIndex );
        }
    }
}




//-----------------------------------------------------------------------------
// name: isDone()
// desc: is done
//-----------------------------------------------------------------------------
bool MidiScoreReader::isDone( long track, double currTime )
{
    // sanity check
    if( track < 0 || track >= m_events.size() )
        return true;
    
    // number of events on a track
    long numEvents = m_events[track].size();
    // check time against last element
    if( numEvents <= 0 )
        return true;
    
    return m_events[track][numEvents-1]->endTime < currTime;
}




//-----------------------------------------------------------------------------
// name: applyControl()
// desc: applies control data to an existing event
//-----------------------------------------------------------------------------
bool MidiScoreReader::applyControl( long track, long data2, long data3, MidiNoteEvent * e )
{
    // sanity check
    if( !e ) return false;
    if( data2 < 0 || data2 > 127 ) return false;
    if( data3 < 0 || data3 > 127 ) return false;
    
    // flag
    bool found = false;
    
    if( data2 == 0x52 )
    {
        // gliss
        e->shouldBend = true;
        found = true;
        // increment
        incrementCount( track, "bend" );
        e->bendAmount = (int)data3;
    }
    
    if( data2 == 0x53 )
    {
        // gliss
        e->phrasemark = true;
        found = true;
        // increment
        incrementCount( track, "phrasemark" );
    }
    
    return found;
}




//-----------------------------------------------------------------------------
// name: loadTrack()
// desc: load a track from file
//-----------------------------------------------------------------------------
bool MidiScoreReader::loadTrack( long track, std::vector<MidiNoteEvent *> & data, std::vector<MidiLyricEvent *> & lyricData )
{
    // sanity check
    if( !m_midiFile || track >= m_events.size() )
    {
        // error
        CK_STDCERR << "[chuck](MIDI file): cannot read track: " << track << "!" << CK_STDENDL;
        return false;
    }
    
    // clear out the vectors
    data.clear();
    // clear out active MIDI notes
    m_activeNotes.clear();
    // a vector to hold the return information
    std::vector<unsigned char> shuttle;
    // piano event pointers
    MidiNoteEvent * e = NULL;
    MidiLyricEvent * le = NULL;
    // last event
    MidiNoteEvent * prev_e = NULL;
    MidiLyricEvent * prev_le = NULL;
    // parent event
    MidiNoteEvent * parent = NULL;
    
    int currentProgram = 27; // program defaults to electric gutiar (27) in case it is not set in the score
    
    // load next on the track
    try {
        // seconds accum
        double secondsAccum = 0;
        // loop
        while( true )
        {
            // get the next MIDI event
            unsigned long ticks = m_midiFile->getNextEvent( &shuttle, (unsigned int)track );
            
            // accumulate time
            secondsAccum += ticks * m_midiFile->getTickSeconds();
            
            // MIDI message spec: http://www.srm.com/qtma/davidsmidispec.html
            
            // check size (done?)
            if( shuttle.size() == 0 ) break;
            
            if( isNoteOff( shuttle ) )
            {
                // do note off
                handleNoteOff( shuttle[1], secondsAccum );
                continue;
            }
            else if( isControl( shuttle ) )
            {
                // apply control to previous
                applyControl( track, shuttle[1], shuttle[2], prev_e );
                continue;
            }
            else if( isMeta( shuttle ) )
            {
                // format: http://www.recordingblogs.com/sa/Wiki/tabid/88/Default.aspx?topic=MIDI+meta+messages
                // shuttle[1]: message type
                // shuttle[2]: number of bytes in custom data part
                // shuttle[3]: start of custom data part
                
                switch( shuttle[1] )
                {
                    case 1: // text
                    {
                        string text = string( (char *)&shuttle[3] /* name */, shuttle[2] /* length */ );
                        break;
                    }
                    case 3: // track name
                    {
                        string name = string( (char *)&shuttle[3] /* name */, shuttle[2] /* length */ );
                        setTrackName( track, name );
                        break;
                    }
                    case 5: // lyric
                    {
                        string lyric = string( (char *)&shuttle[3] /* name */, shuttle[2] /* length */ );
                        
                        le = new MidiLyricEvent;
                        le->lyric = lyric;
                        le->time = le->endTime = secondsAccum;
                        
                        if( prev_le )
                        {
                            prev_le->endTime = secondsAccum;
                        }
                        
                        lyricData.push_back( le );
                        
                        prev_le = le;
                        
                        break;
                    }
                }
                continue;
            }
            
            // store a program change in an event
            else if( isProgram( shuttle ) )
            {
                if( shuttle[1] >= 0 )
                    currentProgram = shuttle[1];
                continue;
            }
            
            // continue if it's anything but a note on
            if( shuttle[0] >> 4 != 0x9 || shuttle[2] == 0 ) continue;
            
            // instantiate
            e = new MidiNoteEvent();
            // fill
            e->data1 = (unsigned short)shuttle[0];
            e->data2 = (unsigned short)shuttle[1];
            e->data3 = (unsigned short)shuttle[2];
            // check
            if( (m_velocity_scale > 0) && e->data3 )
            {
                // scale
                e->data3 = e->data3 + m_velocity_scale * (127 - e->data3);
                // clamp
                if( e->data3 > 127 ) e->data3 = 127;
            }
            
            // fill the until next for prev
            if( prev_e ) prev_e->untilNext = e->time - prev_e->startTime;
            // advance time
            e->startTime = e->time = secondsAccum;
            // simultaneous
            e->simultaneous = NULL;
            // store current program
            e->programChange = currentProgram;
            
            // simultaneous or in series?
            if( prev_e != NULL && e->time == prev_e->time )
            {
                // sim
                prev_e->simultaneous = e;
                // point to parent
                e->parent = parent;
            }
            else
            {
                // increment
                incrementCount( track, "note" );
                // series
                data.push_back( e );
                // save as parent
                parent = e->parent = e;
            }
            
            // add to lookup
            handleNoteOn( e, e->data2 );
            
            // update
            prev_e = e;
        }
    }
    catch( StkError & )
    {
        // you might want to do something more useful here.
        CK_STDCERR << "[chuck](MIDI file): uh oh, error loading track!" << CK_STDENDL;
        
        // clear out active MIDI notes
        m_activeNotes.clear();
        
        return false;
    }
    
    // clear out active MIDI notes
    m_activeNotes.clear();
    
    return true;
}




//-----------------------------------------------------------------------------
// name: enqueue()
// desc: enqueue a event to remember
//-----------------------------------------------------------------------------
void MidiScoreReader::enqueue( const MidiNoteEvent * event )
{
    // TODO: synchronization?
    // while( event != NULL )
    // {
    m_queue.push_back( event );
    //    event = event->simultaneous;
    // }
}




//-----------------------------------------------------------------------------
// name: front()
// desc: get the front event
//-----------------------------------------------------------------------------
const MidiNoteEvent * MidiScoreReader::front() const
{
    if( m_queue.size() == 0 ) return NULL;
    else return m_queue.front();
}




//-----------------------------------------------------------------------------
// name: dequeue()
// desc: dequeue the event
//-----------------------------------------------------------------------------
void MidiScoreReader::dequeue()
{
    if( m_queue.size() > 0 )
        m_queue.pop_front();
}




//-----------------------------------------------------------------------------
// name: enqueue_front()
// desc: enqueue to front
//-----------------------------------------------------------------------------
void MidiScoreReader::enqueue_front( const MidiNoteEvent * e )
{
    m_queue.push_front( e );
}




//-----------------------------------------------------------------------------
// name: getNumEvents()
// desc: get number of events on a track
//-----------------------------------------------------------------------------
long MidiScoreReader::getNumEvents( long track ) const
{
    // sanity check
    if( track < 0 || track >= m_events.size() )
        return 0;
    
    // return the number of elements
    return m_events[track].size();
}




//-----------------------------------------------------------------------------
// name: getNoteEvents()
// desc: get an entire track's note events
//-----------------------------------------------------------------------------
static vector<MidiNoteEvent *> s_nullNoteVector;
const vector<MidiNoteEvent *> & MidiScoreReader::getNoteEvents( long track )
{
    // sanity check
    if( track < 0 || track >= m_events.size() )
        return s_nullNoteVector;
    
    return m_events[track];
}




//-----------------------------------------------------------------------------
// name: getLyricEvents()
// desc: get an entire track's lyric events
//-----------------------------------------------------------------------------
static vector<MidiLyricEvent *> s_nullLyricVector;
const vector<MidiLyricEvent *> & MidiScoreReader::getLyricEvents( long track )
{
    // sanity check
    if( track < 0 || track >= m_lyricEvents.size() )
        return s_nullLyricVector;
    
    return m_lyricEvents[track];
}




//-----------------------------------------------------------------------------
// name: getCount()
// desc: return a count associated with key
//-----------------------------------------------------------------------------
long MidiScoreReader::getCount( long track, const std::string & key )
{
    // sanity check
    if( track < 0 || track >= m_events.size() ) return 0;
    // find
    std::map< std::string, long >::iterator iter = m_countMaps[track].find( key );
    // not found
    if( iter == m_countMaps[track].end() ) return 0;
    // return value
    return iter->second;
}




//-----------------------------------------------------------------------------
// name: incrementCount()
// desc: increment a count associated with key
//-----------------------------------------------------------------------------
void MidiScoreReader::incrementCount( long track, const std::string & key )
{
    // sanity check
    if( track < 0 || track >= m_events.size() ) return;
    
    // find
    std::map< std::string, long >::iterator iter = m_countMaps[track].find( key );
    
    if( iter == m_countMaps[track].end() )
    {
        // if not found, add
        m_countMaps[track][key] = 1;
    }
    else
    {
        // if found, increment
        iter->second++;
    }
}




//-----------------------------------------------------------------------------
// name: handleNoteOn()
// desc: insert a note on into table
//-----------------------------------------------------------------------------
void MidiScoreReader::handleNoteOn( MidiNoteEvent * e, long note )
{
    // sanity check
    if( !e )
        return;
    
    // check to see if occupied
    if( m_activeNotes[note] != NULL )
    {
        // treat as a note-off for the same note
        handleNoteOff( note, e->time );
    }
    
    // insert
    m_activeNotes[note] = e;
}




//-----------------------------------------------------------------------------
// name: handleNoteOff()
// desc: insert a note off into table
//-----------------------------------------------------------------------------
void MidiScoreReader::handleNoteOff( long note, double time )
{
    // get the piano event
    MidiNoteEvent * e = m_activeNotes[note];
    // check
    if( !e )
        return;
    
    // set end time
    e->endTime = time;
    
    // set
    m_activeNotes[note] = NULL;
}




//-----------------------------------------------------------------------------
// name: setTrackName()
// desc: sets track name
//-----------------------------------------------------------------------------
void MidiScoreReader::setTrackName( long track, const std::string & name )
{
    // x-map it!
    m_trackToName[track] = name;
    m_nameToTrack[name] = track;
}




//-----------------------------------------------------------------------------
// name: getTrackName()
// desc: gets track name ("" if not present)
//-----------------------------------------------------------------------------
string MidiScoreReader::getTrackName( long track )
{
    map<long, string>::iterator itr = m_trackToName.find( track );
    
    if( itr == m_trackToName.end() )
        return "";
    
    return itr->second;
}




//-----------------------------------------------------------------------------
// name: getTrackForName()
// desc: gets the track with the given name (-1 if not present)
//-----------------------------------------------------------------------------
long MidiScoreReader::getTrackForName( const std::string & name )
{
    // look up the track index by name
    map<string, long>::iterator itr = m_nameToTrack.find( name );
    
    // didn't find
    if( itr == m_nameToTrack.end() )
        return -1;
    
    // make sure the track actually exists
    if( itr->second >= m_events.size() )
        return -1;
    
    return itr->second;
}
