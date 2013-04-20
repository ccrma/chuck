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
// file: chuck_otf.h
// desc: on-the-fly programming utilities
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#ifndef __CHUCK_OTF_H__
#define __CHUCK_OTF_H__

#include "chuck_def.h"
#include "util_network.h"
#include <memory.h>


// defines
#define NET_HEADER      0x8c8cc8c8
// buffer size
#define NET_BUFFER_SIZE 512
// error value
#define NET_ERROR       0xffffffff
// forward
struct Chuck_VM;
struct Chuck_Compiler;


//-----------------------------------------------------------------------------
// name: struct Net_Msg()
// desc: ...
//-----------------------------------------------------------------------------
struct Net_Msg
{
    t_CKUINT header;
    t_CKUINT type;
    t_CKUINT param;
    t_CKUINT param2;
    t_CKUINT param3;
    t_CKUINT length;
    char buffer[NET_BUFFER_SIZE];

    Net_Msg() { this->clear(); }
    void clear() { header = NET_HEADER; type = param = param2 = param3 = length = 0;
                   memset( buffer, 0, sizeof(buffer) ); }
};


// host to network
void otf_hton( Net_Msg * msg );
// network to host
void otf_ntoh( Net_Msg * msg );

// process incoming message
t_CKUINT otf_process_msg( Chuck_VM * vm, Chuck_Compiler * compiler, 
                          Net_Msg * msg, t_CKBOOL immediate, void * data );

// send command
int otf_send_cmd( int argc, const char ** argv, t_CKINT & i, const char * host, int port, int * is_otf = NULL );
// send file to remote host
int otf_send_file( const char * filename, Net_Msg & msg, const char * op, ck_socket sock );
// connect
ck_socket otf_send_connect( const char * host, int port );

// callback
void * otf_cb( void * p );


extern const char * poop[];
extern long poop_size;
extern t_CKUINT g_otf_log;



#endif
