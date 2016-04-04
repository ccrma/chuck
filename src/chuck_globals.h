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
#include "chuck_dl.h"
#include "util_network.h"
#include "util_thread.h"




// forward declarations
struct Chuck_VM;
struct Chuck_Compiler;
class  Chuck_Shell;
class  BBQ;


// exports
// current version
extern const char CK_VERSION[];
// global virtual machine
extern Chuck_VM * g_vm;
// global compiler
extern Chuck_Compiler * g_compiler;
// the shell
extern Chuck_Shell * g_shell;
// global BBQ audio layer, ge: 1.3.5.3
extern BBQ * g_bbq;
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
// flag for enabling shell, ge: 1.3.5.3
extern t_CKBOOL g_enable_shell;
// flag for audio enable, ge: 1.3.5.3
extern t_CKBOOL g_enable_realtime_audio;
// added 1.3.2.0 // moved from VM 1.3.5.3
extern f_mainthreadhook g_main_thread_hook;
extern f_mainthreadquit g_main_thread_quit;
extern void * g_main_thread_bindle;


// global detach
extern "C" void all_detach();
// global stop, ge: 1.3.5.3
extern "C" t_CKBOOL all_stop();
// sigpipe function
extern "C" void signal_pipe( int sig_num );
// main thread hook, ge: 1.3.5.3
extern "C" t_CKBOOL clear_main_thread_hook();
extern "C" t_CKBOOL set_main_thread_hook( f_mainthreadhook hook,
                                          f_mainthreadquit quit, void * bindle );



#endif
