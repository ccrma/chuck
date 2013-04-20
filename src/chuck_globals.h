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
// file: chuck_globals.h
// desc: global variables slum
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: spring 2006
//-----------------------------------------------------------------------------
#ifndef __CHUCK_GLOBALS_H__
#define __CHUCK_GLOBALS_H__

#include "chuck_def.h"
#include "util_network.h"
#include "util_thread.h"




// forward declarations
struct Chuck_VM;
struct Chuck_Compiler;
class  Chuck_Shell;


// exports
// current version
extern const char CK_VERSION[];
// global virtual machine
extern Chuck_VM * g_vm;
// global compiler
extern Chuck_Compiler * g_compiler;
// the shell
extern Chuck_Shell * g_shell;
// global variables
extern t_CKUINT g_sigpipe_mode;
// global socket
extern ck_socket g_sock;
// global port
extern t_CKINT g_port;
// real-time watchdog
extern t_CKBOOL g_do_watchdog;
// countermeasure priority
extern t_CKUINT g_watchdog_countermeasure_priority;
// watchdog timeout
extern t_CKFLOAT g_watchdog_timeout;
// thread id for whatever
extern CHUCK_THREAD g_tid_whatever;
// flag for providing Std.system( string )
extern t_CKBOOL g_enable_system_cmd;


// global detach
extern "C" void all_detach();
// sigpipe function
extern "C" void signal_pipe( int sig_num );




#endif
