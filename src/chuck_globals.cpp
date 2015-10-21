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
// file: chuck_globals.cpp
// desc: global variables slum
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: spring 2006
//-----------------------------------------------------------------------------
#include "chuck_globals.h"
#include "chuck_bbq.h"
#include "chuck_errmsg.h"
#include "ugen_stk.h"
#include "ulib_std.h"
#include "hidio_sdl.h"
#include "chuck_io.h"
#include <stdlib.h>
#include <stdio.h>


// current version
const char CK_VERSION[] = "1.3.5.2 (chimera)";

// global virtual machine
Chuck_VM * g_vm = NULL;
// global compiler
Chuck_Compiler * g_compiler = NULL;
// the shell
Chuck_Shell * g_shell = NULL;
// global variables
t_CKUINT g_sigpipe_mode = 0;
// default socket
ck_socket g_sock = NULL;
// default port
t_CKINT g_port = 8888;
// real-time watch dog
t_CKBOOL g_do_watchdog = FALSE;
// countermeasure
#ifdef __MACOSX_CORE__
t_CKUINT g_watchdog_countermeasure_priority = 10;
#elif defined(__PLATFORM_WIN32__) && !defined(__WINDOWS_PTHREAD__)
t_CKUINT g_watchdog_countermeasure_priority = THREAD_PRIORITY_LOWEST;
#else
t_CKUINT g_watchdog_countermeasure_priority = 0;
#endif
// watchdog timeout
t_CKFLOAT g_watchdog_timeout = 0.5;
// thread id for whatever
CHUCK_THREAD g_tid_whatever = 0;
// flag for Std.system( string )
t_CKBOOL g_enable_system_cmd = FALSE;




//-----------------------------------------------------------------------------
// name: all_detach()
// desc: called during cleanup to close all open file handles
//-----------------------------------------------------------------------------
extern "C" void all_detach()
{
    // log
    EM_log( CK_LOG_INFO, "detaching all resources..." );
    // push
    EM_pushlog();
    // close stk file handles
    stk_detach( 0, NULL );
#ifndef __DISABLE_MIDI__
    // close midi file handles
    midirw_detach();
#endif // __DISABLE_MIDI__
#ifndef __DISABLE_KBHIT__
    // shutdown kb loop
    KBHitManager::shutdown();
#endif // __DISABLE_KBHIT__
#ifndef __ALTER_HID__
    // shutdown HID
    HidInManager::cleanup();
#endif // __ALTER_HID__
    
    Chuck_IO_Serial::shutdown();
    // pop
    EM_poplog();
}


//-----------------------------------------------------------------------------
// name: signal_pipe()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" void signal_pipe( int sig_num )
{
    fprintf( stderr, "[chuck]: sigpipe handled - broken pipe (no connection)...\n" );
    if( g_sigpipe_mode )
    {
        all_detach();
        // exit( 2 );
    }
}
