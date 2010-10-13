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
// file: ulib_net.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include <iostream>
#include "ulib_net.h"
#include "util_network.h"
using namespace std;




//-----------------------------------------------------------------------------
// name: net_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY net_query( Chuck_DL_Query * QUERY )
{

    //! \sectionMain network
    /*! \nameinfo
    ChucK's network ugens
    */    

    // add netout
    //! UDP-based network audio transmitter
    QUERY->ugen_add( QUERY, "netout", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, netout_ctor, netout_dtor, netout_tick, NULL );
    // ctrl funcs
    QUERY->ugen_ctrl( QUERY, netout_ctrl_addr, netout_cget_addr, "string", "addr" ); //! target address
    QUERY->ugen_ctrl( QUERY, netout_ctrl_port, netout_cget_port, "int", "port" ); //! target port
    QUERY->ugen_ctrl( QUERY, netout_ctrl_size, netout_cget_size, "int", "size" ); //! packet size
    QUERY->ugen_ctrl( QUERY, netout_ctrl_name, netout_cget_name, "string", "name" ); //! name?
    
    // add netin
    //! UDP-based network audio receiver
    QUERY->ugen_add( QUERY, "netin", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, netin_ctor, netin_dtor, netin_tick, NULL );
    // ctrl funcs
    QUERY->ugen_ctrl( QUERY, netin_ctrl_port, netin_cget_port, "int", "port" ); //! set port to receive
    QUERY->ugen_ctrl( QUERY, netin_ctrl_name, netin_cget_name, "string", "name" ); //! name?

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: struct GigaMsg
// desc: ...
//-----------------------------------------------------------------------------
struct GigaMsg
{
    unsigned int type;
    unsigned int len;
    unsigned int seq_num;
  
    unsigned char * payload;

    // constructor
    GigaMsg() {
        type = len = seq_num = 0;
        payload = NULL;
    }

    // init
    void init( unsigned int _type, unsigned int _len )
    {
        type = _type;
        len = _len;
        payload = new unsigned char[len];
    }

    // destructor
    ~GigaMsg( )
    {
        if( payload )
        {
            delete [] payload;
	        payload = NULL;
        }
        
        type = 0;
        len = 0;
        seq_num = 0;
    }
};




//-----------------------------------------------------------------------------
// name: class GigaSend
// desc: ...
//-----------------------------------------------------------------------------
class GigaSend
{
public:
    GigaSend( );
    ~GigaSend( );

    t_CKBOOL connect( const char * hostname, int port );
    t_CKBOOL disconnect( );
    t_CKBOOL send( const t_CKBYTE * buffer );
    t_CKBOOL set_bufsize( t_CKUINT buffer_size );
    t_CKUINT get_bufsize( );
    t_CKBOOL good( );

    t_CKBOOL tick_out( SAMPLE sample );
    t_CKBOOL tick_out( SAMPLE l, SAMPLE r );
    t_CKBOOL tick_out( SAMPLE * samples, t_CKDWORD n );

    void set_redundancy( t_CKUINT n );
    t_CKUINT get_redundancy( );
    
    // data
    string m_hostname;
    int m_port;
    string m_name;

protected:
    ck_socket m_sock;
    t_CKUINT m_red;
    t_CKUINT m_buffer_size;
    GigaMsg m_msg;
    t_CKUINT m_len;
    t_CKBYTE m_buffer[0x8000];

    SAMPLE m_writebuf[0x8000];
    SAMPLE * m_ptr_w;
    SAMPLE * m_ptr_end;
};




//-----------------------------------------------------------------------------
// name: class GigaRecv
// desc: ...
//-----------------------------------------------------------------------------
class GigaRecv
{
public:
    GigaRecv( );
    ~GigaRecv( );

    t_CKBOOL listen( int port );
    t_CKBOOL disconnect( );
    t_CKBOOL recv( t_CKBYTE * buffer );
    t_CKBOOL expire();
    t_CKBOOL set_bufsize( t_CKUINT size );
    t_CKUINT get_bufsize( );
    t_CKBOOL good( );
    
    t_CKBOOL tick_in( SAMPLE * sample );
    t_CKBOOL tick_in( SAMPLE * l, SAMPLE * r );
    t_CKBOOL tick_in( SAMPLE * samples, t_CKDWORD n );
    
    // data
    int m_port;
    string m_name;

protected:
    ck_socket m_sock;
    t_CKUINT m_buffer_size;
    GigaMsg m_msg;
    t_CKUINT m_len;
    t_CKBYTE m_buffer[0x8000];
    
    SAMPLE m_readbuf[0x8000];
    SAMPLE * m_ptr_r;
    SAMPLE * m_ptr_end;
};




//-----------------------------------------------------------------------------
// name: GigaSend()
// desc: ...
//-----------------------------------------------------------------------------
GigaSend::GigaSend( )
{
    m_sock = NULL;
    m_red = 1;
    m_buffer_size = 0;
    m_hostname = "127.0.0.1";
    m_port = 8890;
    m_msg.seq_num = 0;
    m_ptr_w = m_writebuf;
    m_ptr_end = NULL;
}
t_CKBOOL GigaSend::good( ) { return m_sock != NULL; }




//-----------------------------------------------------------------------------
// name: ~GigaSend()
// desc: ...
//-----------------------------------------------------------------------------
GigaSend::~GigaSend( )
{
    this->disconnect();
}




//-----------------------------------------------------------------------------
// name: connect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::connect( const char * hostname, int port )
{
    if( m_sock )
        return FALSE;

    m_sock = ck_udp_create( );
    if( !ck_connect( m_sock, hostname, port ) )
    {
        cerr << "[chuck](via netout): error: cannot connect to '" << hostname << ":" 
	         << port << "'" << endl;
        return FALSE;
    }
    
    m_hostname = hostname;
    m_port = port;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_bufsize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::set_bufsize( t_CKUINT bufsize )
{
    m_buffer_size = bufsize * sizeof(SAMPLE);
    m_len = sizeof(GigaMsg) + m_buffer_size - sizeof( m_msg.payload );
    m_msg.type = 0;
    m_msg.len = bufsize;
    m_ptr_w = m_writebuf;
    m_ptr_end = m_writebuf + bufsize;

    return TRUE;
}
t_CKUINT GigaSend::get_bufsize() { return m_buffer_size; }




//-----------------------------------------------------------------------------
// name: disconnect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::disconnect( )
{
    if( !m_sock )
        return FALSE;

    ck_close( m_sock );
    m_sock = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_redundancy()
// desc: ...
//-----------------------------------------------------------------------------
void GigaSend::set_redundancy( t_CKUINT n )
{
    m_red = n;
}




//-----------------------------------------------------------------------------
// name: get_redundancy()
// desc: ...
//-----------------------------------------------------------------------------
t_CKUINT GigaSend::get_redundancy( )
{
    return m_red;
}




//-----------------------------------------------------------------------------
// name: send()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::send( const t_CKBYTE * buffer )
{
    m_msg.seq_num++;

    memcpy( m_buffer, &m_msg, sizeof( unsigned int ) * 3 );
    memcpy( m_buffer + sizeof( unsigned int ) * 3, buffer, m_buffer_size );

    for( int i = 0; i < m_red; i++ )
        ck_send( m_sock, (const char * )m_buffer, m_len );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: tick_out()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaSend::tick_out( SAMPLE sample )
{
    // send
    if( m_ptr_w >= m_ptr_end )
    {
        this->send( (t_CKBYTE *)m_writebuf );
        m_ptr_w = m_writebuf;
    }

    *m_ptr_w++ = sample;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: GigaRecv()
// desc: ...
//-----------------------------------------------------------------------------
GigaRecv::GigaRecv( )
{
    m_sock = NULL;
    m_buffer_size = 0;
    m_msg.seq_num = 1;
    m_port = 8890;
    m_ptr_r = NULL;
    m_ptr_end = NULL;
}
t_CKBOOL GigaRecv::good( ) { return m_sock != NULL; }




//-----------------------------------------------------------------------------
// name: ~GigaRecv()
// desc: ...
//-----------------------------------------------------------------------------
GigaRecv::~GigaRecv( )
{
    this->disconnect( );
}




//-----------------------------------------------------------------------------
// name: listen()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::listen( int port )
{
    if( m_sock )
        return FALSE;

    m_sock = ck_udp_create();

    // bind
    if( !ck_bind( m_sock, port ) )
    {
        cerr << "[chuck](via netin): error: cannot bind to port '" << port << "'" << endl;
        return FALSE;
    }
    
    m_port = port;
    m_msg.seq_num = 1;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: disconnect()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::disconnect( )
{
    if( !m_sock )
        return FALSE;
        
    ck_close( m_sock );
    m_sock = NULL;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: set_bufsize()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::set_bufsize( t_CKUINT bufsize )
{
    m_buffer_size = bufsize;
    m_len = sizeof(GigaMsg) + bufsize - sizeof( m_msg.payload );
    m_msg.type = 0;
    m_msg.len = m_len;

    return TRUE;
}
t_CKUINT GigaRecv::get_bufsize() { return m_buffer_size; }




//-----------------------------------------------------------------------------
// name: recv()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::recv( t_CKBYTE * buffer )
{
    GigaMsg * msg = (GigaMsg *)m_buffer;

    if( !m_sock )
        return FALSE;

    do{ ck_recv( m_sock, (char *)m_buffer, 0x8000 ); }
    while( msg->seq_num < m_msg.seq_num );

    if( msg->seq_num > (m_msg.seq_num + 1) )
        cerr << "[chuck](via netin): dropped packet, expect: " << m_msg.seq_num + 1
             << " got: " << msg->seq_num << endl;

    m_msg.seq_num = msg->seq_num;
    m_msg.len = msg->len;
    m_ptr_end = m_readbuf + msg->len;

    memcpy( buffer, m_buffer + sizeof( unsigned int ) * 3, m_msg.len * sizeof(SAMPLE) );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: expire()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::expire()
{
    m_msg.seq_num++;
	return true;
}




//-----------------------------------------------------------------------------
// name: tick_in()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL GigaRecv::tick_in( SAMPLE * sample )
{
    if( m_ptr_r >= m_ptr_end )
    {
        this->recv( (t_CKBYTE *)m_readbuf );
        m_ptr_r = m_readbuf;
    }
    
    *sample = *m_ptr_r++;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: netout
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CTOR netout_ctor( t_CKTIME now )
{
     GigaSend * out = new GigaSend;
     out->set_bufsize( 512 );

     return out;
}

UGEN_DTOR netout_dtor( t_CKTIME now, void * data )
{
     GigaSend * out = (GigaSend *)data;
     delete out;
}

UGEN_TICK netout_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
     GigaSend * x = (GigaSend *)data;
     *out = 0.0f;
     return x->tick_out( in );
}

UGEN_CTRL netout_ctrl_addr( t_CKTIME now, void * data, void * value )
{
     GigaSend * x = (GigaSend *)data;
     char * str = GET_NEXT_STRING(value);

     // check if the same and already good
     if( x->good() && !strcmp( x->m_hostname.c_str(), str ) )
         return;

     // connect
     x->disconnect();
     x->connect( str, x->m_port );
}

UGEN_CGET netout_cget_addr( t_CKTIME now, void * data, void * out )
{
    GigaSend * x = (GigaSend *)data;
    SET_NEXT_STRING( out, (char *)x->m_hostname.c_str() );
}

UGEN_CTRL netout_ctrl_port( t_CKTIME now, void * data, void * value )
{
    GigaSend * x = (GigaSend *)data;
    int port = GET_NEXT_INT(value);
    
    // check if the same and already connected
    if( x->good() && port == x->m_port )
        return;
        
    // connect
    x->disconnect();
    x->connect( x->m_hostname.c_str(), port );
}

UGEN_CGET netout_cget_port( t_CKTIME now, void * data, void * out )
{
    GigaSend * x = (GigaSend *)data;
    SET_NEXT_INT( out, x->m_port );
}

UGEN_CTRL netout_ctrl_size( t_CKTIME now, void * data, void * value )
{
    GigaSend * x = (GigaSend *)data;
    int size = GET_NEXT_INT(value);
    
    // sanity check
    if( size < 1 || size > 0x8000 )
    {
        cerr << "[chuck](via netout): invalid buffer size '"
             << size << "' (must be between 1 and 0x8000)" << endl;
        return;
    }
    
    x->set_bufsize( (t_CKDWORD)size );
}

UGEN_CGET netout_cget_size( t_CKTIME now, void * data, void * out )
{
    GigaSend * x = (GigaSend *)data;
    SET_NEXT_INT( out, x->get_bufsize() );
}

UGEN_CTRL netout_ctrl_name( t_CKTIME now, void * data, void * value )
{
    GigaSend * x = (GigaSend *)data;
    char * str = GET_NEXT_STRING(value);
    x->m_name = str;
}

UGEN_CGET netout_cget_name( t_CKTIME now, void * data, void * out )
{
    GigaSend * x = (GigaSend *)data;
    SET_NEXT_STRING( out, (char *)x->m_name.c_str() );
}




//-----------------------------------------------------------------------------
// name: netin
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CTOR netin_ctor( t_CKTIME now )
{
    GigaRecv * x = new GigaRecv;
    x->listen( x->m_port );
    
    return x;
}

UGEN_DTOR netin_dtor( t_CKTIME now, void * data )
{
    GigaRecv * x = (GigaRecv *)data;
    delete x;
}

UGEN_TICK netin_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    GigaRecv * x = (GigaRecv *)data;
    return x->tick_in( out );
}

UGEN_CTRL netin_ctrl_port( t_CKTIME now, void * data, void * value )
{
    GigaRecv * x = (GigaRecv *)data;
    int port = GET_NEXT_INT(value);
    
    // check if same and already connected
    if( x->good() && x->m_port == port )
        return;
    
    // connect
    x->disconnect( );
    x->listen( port );
}

UGEN_CGET netin_cget_port( t_CKTIME now, void * data, void * out )
{
    GigaRecv * x = (GigaRecv *)data;
    SET_NEXT_INT( out, x->m_port );
}

UGEN_CTRL netin_ctrl_name( t_CKTIME now, void * data, void * value )
{
    GigaRecv * x = (GigaRecv *)data;
    char * str = GET_NEXT_STRING(value);
    x->m_name = str;
}

UGEN_CGET netin_cget_name( t_CKTIME now, void * data, void * out )
{
    GigaRecv * x = (GigaRecv *)data;
    SET_NEXT_STRING( out, (char *)x->m_name.c_str() );
}
