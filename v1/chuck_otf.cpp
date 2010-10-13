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
// file: chuck_otf.cpp
// desc: on-the-fly programming utilities
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------

#include "chuck_otf.h"
#include "util_network.h"




//-----------------------------------------------------------------------------
// name: otf_hton( )
// desc: ...
//-----------------------------------------------------------------------------
void otf_hton( Net_Msg * msg )
{
    msg->header = htonl( msg->header );
    msg->type = htonl( msg->type );
    msg->param = htonl( msg->param );
    msg->param2 = htonl( msg->param2 );
    msg->param3 = htonl( msg->param3 );
    msg->length = htonl( msg->length );
}




//-----------------------------------------------------------------------------
// name: otf_ntoh( )
// desc: ...
//-----------------------------------------------------------------------------
void otf_ntoh( Net_Msg * msg )
{
    msg->header = ntohl( msg->header );
    msg->type = ntohl( msg->type );
    msg->param = ntohl( msg->param );
    msg->param2 = ntohl( msg->param2 );
    msg->param3 = ntohl( msg->param3 );
    msg->length = ntohl( msg->length );
}

