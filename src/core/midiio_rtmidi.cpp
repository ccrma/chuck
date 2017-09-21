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
#define BUFFER_SIZE 8192

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
        if( !cbuf->initialize( BUFFER_SIZE, sizeof(MidiMsg), m_event_buffers[vm] ) )
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
