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
// name: chuck_io.cpp
// desc: chuck i/o
//
// authors: Spencer Salazar (spencer@ccrma.stanford.edu)
//    date: Summer 2012
//-----------------------------------------------------------------------------
#include "chuck_io.h"
#include "chuck_errmsg.h"
#include "util_serial.h"
#include "chuck_instr.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#ifndef __DISABLE_HID__
#include "hidio_sdl.h"
#endif

#ifndef __DISABLE_MIDI__
#include "midiio_rtmidi.h"
#endif

#ifndef __WINDOWS_DS__
#include <sys/select.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#else
#include <io.h>
#endif // __WINDOWS_DS__
#include <fcntl.h>
#include <math.h>

#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;


#ifdef __WINDOWS_DS__
typedef BYTE uint8_t;
typedef WORD uint16_t;
// ge: this needed in earlier/some versions of windows
#ifndef __WINDOWS_MODERN__
typedef __int32 int32_t; // 1.4.1.0 (ge) added
typedef DWORD uint32_t;
#endif
#endif 

#ifdef __PLATFORM_LINUX__
#include <stdint.h>
#endif

// global
static Chuck_String * g_newline = new Chuck_String();




//-----------------------------------------------------------------------------
// name: init_class_io()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_io( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'IO'" );
    
    // init as base class
    // TODO: ctor/dtor?
    // TODO: replace dummy with pure function
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL ) )
        return FALSE;
    
    // add good()
    func = make_new_mfun( "int", "good", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "void", "close", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add flush()
    func = make_new_mfun( "void", "flush", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode(int)
    func = make_new_mfun( "int", "mode", io_dummy );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode()
    func = make_new_mfun( "int", "mode", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // TODO: add this later?
    // add read()
    // func = make_new_mfun( "string", "read", io_dummy );
    // func->add_arg( "int", "length" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readLine()
    func = make_new_mfun( "string", "readLine", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt()
    func = make_new_mfun( "int", "readInt", io_dummy );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", io_dummy );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add eof()
    func = make_new_mfun( "int", "eof", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add more()
    func = make_new_mfun( "int", "more", io_dummy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(string)
    func = make_new_mfun( "void", "write", io_dummy );
    func->add_arg( "string", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(int)
    func = make_new_mfun( "void", "write", io_dummy );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(float)
    func = make_new_mfun( "void", "write", io_dummy );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add newline
    func = make_new_sfun( "string", "newline", io_newline );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    func = make_new_sfun( "string", "nl", io_newline );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    func = make_new_sfun( "string", "newlineEx2VistaHWNDVisualFoxProA", io_newline );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    initialize_object( g_newline, env->t_string );
    g_newline->set( "\n" );
    
    // add READ_INT32
    if( !type_engine_import_svar( env, "int", "READ_INT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT32 ) ) goto error;
    
    // add READ_INT16
    if( !type_engine_import_svar( env, "int", "READ_INT16",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT16 ) ) goto error;
    
    // add READ_INT8
    if( !type_engine_import_svar( env, "int", "READ_INT8",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT8 ) ) goto error;
    
    // add INT32
    if( !type_engine_import_svar( env, "int", "INT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT32 ) ) goto error;
    
    // add INT16
    if( !type_engine_import_svar( env, "int", "INT16",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT16 ) ) goto error;
    
    // add INT8
    if( !type_engine_import_svar( env, "int", "INT8",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT8 ) ) goto error;
    
    // add MODE_SYNC
    if( !type_engine_import_svar( env, "int", "MODE_SYNC",
                                 TRUE, (t_CKUINT)&Chuck_IO::MODE_SYNC ) ) goto error;
    
    // add MODE_ASYNC
    if( !type_engine_import_svar( env, "int", "MODE_ASYNC",
                                 TRUE, (t_CKUINT)&Chuck_IO::MODE_ASYNC ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




#ifndef __DISABLE_FILEIO__
//-----------------------------------------------------------------------------
// name: init_class_fileio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_fileio( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'FileIO'" );
    
    // init as base class
    // TODO: ctor/dtor?
    // TODO: replace dummy with pure function
    if( !type_engine_import_class_begin( env, type, env->global(), fileio_ctor, fileio_dtor ) )
        return FALSE;
    
    // add open(string)
    func = make_new_mfun( "int", "open", fileio_open );
    func->add_arg( "string", "path" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add open(string, flags)
    func = make_new_mfun( "int", "open", fileio_openflags );
    func->add_arg( "string", "path" );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add good()
    func = make_new_mfun( "int", "good", fileio_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "void", "close", fileio_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add flush()
    func = make_new_mfun( "void", "flush", fileio_flush );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode()
    func = make_new_mfun( "int", "mode", fileio_getmode );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode(int)
    func = make_new_mfun( "int", "mode", fileio_setmode );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add size()
    func = make_new_mfun( "int", "size", fileio_size );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add seek(int)
    func = make_new_mfun( "void", "seek", fileio_seek );
    func->add_arg( "int", "pos" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add tell()
    func = make_new_mfun( "int", "tell", fileio_tell );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isDir()
    func = make_new_mfun( "int", "isDir", fileio_isdir );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add dirList()
    func = make_new_mfun( "string[]", "dirList", fileio_dirlist );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add read()
    // func = make_new_mfun( "string", "read", fileio_read );
    // func->add_arg( "int", "length" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readLine()
    func = make_new_mfun( "string", "readLine", fileio_readline );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt()
    // func = make_new_mfun( "int", "readInt", fileio_readint );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt(int)
    func = make_new_mfun( "int", "readInt", fileio_readintflags );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", fileio_readfloat );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add eof()
    func = make_new_mfun( "int", "eof", fileio_eof );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add more()
    func = make_new_mfun( "int", "more", fileio_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(string)
    func = make_new_mfun( "void", "write", fileio_writestring );
    func->add_arg( "string", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(int)
    func = make_new_mfun( "void", "write", fileio_writeint );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(int,flags)
    func = make_new_mfun( "void", "write", fileio_writeintflags );
    func->add_arg( "int", "val" );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(float)
    func = make_new_mfun( "void", "write", fileio_writefloat );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add FLAG_READ_WRITE
    if( !type_engine_import_svar( env, "int", "READ_WRITE",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::FLAG_READ_WRITE ) ) goto error;
    
    // add FLAG_READONLY
    if( !type_engine_import_svar( env, "int", "READ",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::FLAG_READONLY ) ) goto error;
    
    // add FLAG_WRITEONLY
    if( !type_engine_import_svar( env, "int", "WRITE",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::FLAG_WRITEONLY ) ) goto error;
    
    // add FLAG_APPEND
    if( !type_engine_import_svar( env, "int", "APPEND",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::FLAG_APPEND ) ) goto error;
    
    // add TYPE_ASCII
    if( !type_engine_import_svar( env, "int", "ASCII",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::TYPE_ASCII ) ) goto error;
    
    // add TYPE_BINARY
    if( !type_engine_import_svar( env, "int", "BINARY",
                                 TRUE, (t_CKUINT)&Chuck_IO_File::TYPE_BINARY ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}
#endif // __DISABLE_FILEIO__




//-----------------------------------------------------------------------------
// name: init_class_chout()
// desc: added 1.3.0.0 as full class
//-----------------------------------------------------------------------------
t_CKBOOL init_class_chout( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'chout'" );
    
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL ) )
        return FALSE;
    
    // add good()
    func = make_new_mfun( "int", "good", chout_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "void", "close", chout_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add flush()
    func = make_new_mfun( "void", "flush", chout_flush );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode()
    func = make_new_mfun( "int", "mode", chout_getmode );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode(int)
    func = make_new_mfun( "int", "mode", chout_setmode );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readLine()
    func = make_new_mfun( "string", "readLine", chout_readline );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt()
    // func = make_new_mfun( "int", "readInt", chout_readint );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt(int)
    func = make_new_mfun( "int", "readInt", chout_readintflags );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", chout_readfloat );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add eof()
    func = make_new_mfun( "int", "eof", chout_eof );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add more()
    func = make_new_mfun( "int", "more", chout_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(string)
    func = make_new_mfun( "void", "write", chout_writestring );
    func->add_arg( "string", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(int)
    func = make_new_mfun( "void", "write", chout_writeint );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(float)
    func = make_new_mfun( "void", "write", chout_writefloat );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_cherr()
// desc: added 1.3.0.0 -- as full class
//-----------------------------------------------------------------------------
t_CKBOOL init_class_cherr( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'cherr'" );
    
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL ) )
        return FALSE;
    
    // add good()
    func = make_new_mfun( "int", "good", cherr_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "void", "close", cherr_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add flush()
    func = make_new_mfun( "void", "flush", cherr_flush );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode()
    func = make_new_mfun( "int", "mode", cherr_getmode );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add mode(int)
    func = make_new_mfun( "int", "mode", cherr_setmode );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readLine()
    func = make_new_mfun( "string", "readLine", cherr_readline );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt()
    // func = make_new_mfun( "int", "readInt", cherr_readint );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readInt(int)
    func = make_new_mfun( "int", "readInt", cherr_readintflags );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", cherr_readfloat );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add eof()
    func = make_new_mfun( "int", "eof", cherr_eof );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add more()
    func = make_new_mfun( "int", "more", cherr_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(string)
    func = make_new_mfun( "void", "write", cherr_writestring );
    func->add_arg( "string", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(int)
    func = make_new_mfun( "void", "write", cherr_writeint );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write(float)
    func = make_new_mfun( "void", "write", cherr_writefloat );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}






// static
static t_CKUINT MidiIn_offset_data = 0;
t_CKUINT MidiMsg_offset_data1 = 0;
t_CKUINT MidiMsg_offset_data2 = 0;
t_CKUINT MidiMsg_offset_data3 = 0;
t_CKUINT MidiMsg_offset_when = 0;
static t_CKUINT MidiOut_offset_data = 0;
//-----------------------------------------------------------------------------
// name: init_class_Midi()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_Midi( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    
    // init base class
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, "MidiMsg", "Object",
                                        env->global(), NULL, NULL ) )
        return FALSE;
    
    // add member variable
    MidiMsg_offset_data1 = type_engine_import_mvar( env, "int", "data1", FALSE );
    if( MidiMsg_offset_data1 == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    MidiMsg_offset_data2 = type_engine_import_mvar( env, "int", "data2", FALSE );
    if( MidiMsg_offset_data2 == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    MidiMsg_offset_data3 = type_engine_import_mvar( env, "int", "data3", FALSE );
    if( MidiMsg_offset_data3 == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    MidiMsg_offset_when = type_engine_import_mvar( env, "dur", "when", FALSE );
    if( MidiMsg_offset_when == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
#ifndef __DISABLE_MIDI__
    // init base class
    if( !type_engine_import_class_begin( env, "MidiIn", "Event",
                                        env->global(), MidiIn_ctor, MidiIn_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", MidiIn_open );
    func->add_arg( "int", "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add open() (added 1.3.0.0)
    func = make_new_mfun( "int", "open", MidiIn_open_named );
    func->add_arg( "string", "name" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add good()
    func = make_new_mfun( "int", "good", MidiIn_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add num()
    func = make_new_mfun( "int", "num", MidiIn_num );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add name()
    func = make_new_mfun( "string", "name", MidiIn_name );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add printerr()
    func = make_new_mfun( "void", "printerr", MidiIn_printerr );
    func->add_arg( "int", "print_or_not" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add recv()
    func = make_new_mfun( "int", "recv", MidiIn_recv );
    func->add_arg( "MidiMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add can_wait()
    func = make_new_mfun( "int", "can_wait", MidiIn_can_wait );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add member variable
    MidiIn_offset_data = type_engine_import_mvar( env, "int", "@MidiIn_data", FALSE );
    if( MidiIn_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    // init base class
    if( !type_engine_import_class_begin( env, "MidiOut", "Object",
                                        env->global(), MidiOut_ctor, MidiOut_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", MidiOut_open );
    func->add_arg( "int", "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add open() (added 1.3.0.0)
    func = make_new_mfun( "int", "open", MidiOut_open_named );
    func->add_arg( "string", "name" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add good()
    func = make_new_mfun( "int", "good", MidiOut_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add num()
    func = make_new_mfun( "int", "num", MidiOut_num );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add name()
    func = make_new_mfun( "string", "name", MidiOut_name );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add printerr()
    func = make_new_mfun( "void", "printerr", MidiOut_printerr );
    func->add_arg( "int", "print_or_not" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add send()
    func = make_new_mfun( "int", "send", MidiOut_send );
    func->add_arg( "MidiMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add member variable
    MidiOut_offset_data = type_engine_import_mvar( env, "int", "@MidiOut_data", FALSE );
    if( MidiOut_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
#endif // __DISABLE_MIDI__
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




#ifndef __DISABLE_HID__
// static
static t_CKUINT HidIn_offset_data = 0;

static t_CKUINT HidMsg_offset_device_type = 0;
static t_CKUINT HidMsg_offset_device_num = 0;
static t_CKUINT HidMsg_offset_type = 0;
static t_CKUINT HidMsg_offset_which = 0;
static t_CKUINT HidMsg_offset_idata = 0;
static t_CKUINT HidMsg_offset_fdata = 0;
static t_CKUINT HidMsg_offset_when = 0;
static t_CKUINT HidMsg_offset_deltax = 0;
static t_CKUINT HidMsg_offset_deltay = 0;
static t_CKUINT HidMsg_offset_axis_position = 0; // deprecated
static t_CKUINT HidMsg_offset_axis_position2 = 0;
static t_CKUINT HidMsg_offset_scaled_axis_position = 0; // deprecated
static t_CKUINT HidMsg_offset_hat_position = 0;
static t_CKUINT HidMsg_offset_cursorx = 0;
static t_CKUINT HidMsg_offset_cursory = 0;
static t_CKUINT HidMsg_offset_scaledcursorx = 0;
static t_CKUINT HidMsg_offset_scaledcursory = 0;
static t_CKUINT HidMsg_offset_x = 0;
static t_CKUINT HidMsg_offset_y = 0;
static t_CKUINT HidMsg_offset_z = 0;
static t_CKUINT HidMsg_offset_touchx = 0; // added 1.3.0.0
static t_CKUINT HidMsg_offset_touchy = 0; // added 1.3.0.0
static t_CKUINT HidMsg_offset_touchsize = 0; // added 1.3.0.0
static t_CKUINT HidMsg_offset_ascii = 0;
static t_CKUINT HidMsg_offset_key = 0;

static t_CKUINT HidOut_offset_data = 0;

//-----------------------------------------------------------------------------
// name: init_class_HID()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_HID( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    
    // init base class
    if( !type_engine_import_class_begin( env, "HidMsg", "Object",
                                        env->global(), NULL, NULL ) )
        return FALSE;
    
    // add member variable
    HidMsg_offset_device_type = type_engine_import_mvar( env, "int", "deviceType", FALSE );
    if( HidMsg_offset_device_type == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_device_num = type_engine_import_mvar( env, "int", "deviceNum", FALSE );
    if( HidMsg_offset_device_num == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_type = type_engine_import_mvar( env, "int", "type", FALSE );
    if( HidMsg_offset_type == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_which = type_engine_import_mvar( env, "int", "which", FALSE );
    if( HidMsg_offset_which == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_idata = type_engine_import_mvar( env, "int", "idata", FALSE );
    if( HidMsg_offset_idata == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_fdata = type_engine_import_mvar( env, "float", "fdata", FALSE );
    if( HidMsg_offset_fdata == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_when = type_engine_import_mvar( env, "time", "when", FALSE );
    if( HidMsg_offset_when == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_deltax = type_engine_import_mvar( env, "int", "deltaX", FALSE );
    if( HidMsg_offset_deltax == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_deltay = type_engine_import_mvar( env, "int", "deltaY", FALSE );
    if( HidMsg_offset_deltay == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_cursorx = type_engine_import_mvar( env, "int", "cursorX", FALSE );
    if( HidMsg_offset_cursorx == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_cursory = type_engine_import_mvar( env, "int", "cursorY", FALSE );
    if( HidMsg_offset_cursory == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_scaledcursorx = type_engine_import_mvar( env, "float", "scaledCursorX", FALSE );
    if( HidMsg_offset_scaledcursorx == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_scaledcursory = type_engine_import_mvar( env, "float", "scaledCursorY", FALSE );
    if( HidMsg_offset_scaledcursory == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_x = type_engine_import_mvar( env, "int", "x", FALSE );
    if( HidMsg_offset_x == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_y = type_engine_import_mvar( env, "int", "y", FALSE );
    if( HidMsg_offset_y == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_z = type_engine_import_mvar( env, "int", "z", FALSE );
    if( HidMsg_offset_z == CK_INVALID_OFFSET ) goto error;
    
    // add member variable (added 1.3.0.0)
    HidMsg_offset_touchx = type_engine_import_mvar( env, "float", "touchX", FALSE );
    if( HidMsg_offset_touchx == CK_INVALID_OFFSET ) goto error;
    
    // add member variable (added 1.3.0.0)
    HidMsg_offset_touchy = type_engine_import_mvar( env, "float", "touchY", FALSE );
    if( HidMsg_offset_touchy == CK_INVALID_OFFSET ) goto error;
    
    // add member variable (added 1.3.0.0)
    HidMsg_offset_touchsize = type_engine_import_mvar( env, "float", "touchSize", FALSE );
    if( HidMsg_offset_touchsize == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_axis_position = type_engine_import_mvar( env, "int", "axis_position", FALSE );
    if( HidMsg_offset_axis_position == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_axis_position2 = type_engine_import_mvar( env, "float", "axisPosition", FALSE );
    if( HidMsg_offset_axis_position2 == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_hat_position = type_engine_import_mvar( env, "int", "hatPosition", FALSE );
    if( HidMsg_offset_hat_position == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_ascii = type_engine_import_mvar( env, "int", "ascii", FALSE );
    if( HidMsg_offset_ascii == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_key = type_engine_import_mvar( env, "int", "key", FALSE );
    if( HidMsg_offset_key == CK_INVALID_OFFSET ) goto error;
    
    // add member variable
    HidMsg_offset_scaled_axis_position = type_engine_import_mvar( env, "float", "scaled_axis_position", FALSE );
    if( HidMsg_offset_scaled_axis_position == CK_INVALID_OFFSET ) goto error;
    
    // add is_axis_motion()
    func = make_new_mfun( "int", "is_axis_motion", HidMsg_is_axis_motion ); // deprecated
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isAxisMotion()
    func = make_new_mfun( "int", "isAxisMotion", HidMsg_is_axis_motion );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add is_button_down()
    func = make_new_mfun( "int", "is_button_down", HidMsg_is_button_down ); // deprecated
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isButtonDown()
    func = make_new_mfun( "int", "isButtonDown", HidMsg_is_button_down );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add is_button_up()
    func = make_new_mfun( "int", "is_button_up", HidMsg_is_button_up ); // deprecated
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isButtonUp()
    func = make_new_mfun( "int", "isButtonUp", HidMsg_is_button_up );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add is_mouse_motion()
    func = make_new_mfun( "int", "is_mouse_motion", HidMsg_is_mouse_motion ); // deprecated
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isMouseMotion()
    func = make_new_mfun( "int", "isMouseMotion", HidMsg_is_mouse_motion );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add is_hat_motion()
    func = make_new_mfun( "int", "is_hat_motion", HidMsg_is_hat_motion ); // deprecated
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isHatMotion()
    func = make_new_mfun( "int", "isHatMotion", HidMsg_is_hat_motion );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add isWheelMotion()
    func = make_new_mfun( "int", "isWheelMotion", HidMsg_is_wheel_motion );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    
    // register deprecate
    type_engine_register_deprecate( env, "HidIn", "Hid" );
    
    // init base class Hid (copy of HidIn + constants)
    if( !type_engine_import_class_begin( env, "Hid", "Event",
                                        env->global(), HidIn_ctor, HidIn_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", HidIn_open );
    func->add_arg( "int", "type" );
    func->add_arg( "int", "num" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add open()
    func = make_new_mfun( "int", "open", HidIn_open_named );
    func->add_arg( "string", "name" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add openJoystick()
    func = make_new_mfun( "int", "openJoystick", HidIn_open_joystick );
    func->add_arg( "int", "num" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add openMouse()
    func = make_new_mfun( "int", "openMouse", HidIn_open_mouse );
    func->add_arg( "int", "num" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add openKeyboard()
    func = make_new_mfun( "int", "openKeyboard", HidIn_open_keyboard );
    func->add_arg( "int", "num" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add openTiltSensor()
    func = make_new_mfun( "int", "openTiltSensor", HidIn_open_tiltsensor );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add good()
    func = make_new_mfun( "int", "good", HidIn_good );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add num()
    func = make_new_mfun( "int", "num", HidIn_num );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add name()
    func = make_new_mfun( "string", "name", HidIn_name );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add printerr()
    func = make_new_mfun( "void", "printerr", HidIn_printerr );
    func->add_arg( "int", "print_or_not" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add recv()
    func = make_new_mfun( "int", "recv", HidIn_recv );
    func->add_arg( "HidMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add read()
    func = make_new_mfun( "int", "read", HidIn_read );
    func->add_arg( "int", "type" );
    func->add_arg( "int", "which" );
    func->add_arg( "HidMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add send()
    func = make_new_mfun( "int", "send", HidIn_send );
    func->add_arg( "HidMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add can_wait()
    func = make_new_mfun( "int", "can_wait", HidIn_can_wait );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readTiltSensor()
    func = make_new_sfun( "int[]", "readTiltSensor", HidIn_read_tilt_sensor );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add globalTiltPollRate()
    func = make_new_sfun( "dur", "globalTiltPollRate", HidIn_ctrl_tiltPollRate );
    func->add_arg( "dur", "d" );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add globalTiltPollRate()
    func = make_new_sfun( "dur", "globalTiltPollRate", HidIn_cget_tiltPollRate );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add startCursorTrack()
    func = make_new_sfun( "int", "startCursorTrack", HidIn_start_cursor_track );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add stopCursorTrack()
    func = make_new_sfun( "int", "stopCursorTrack", HidIn_stop_cursor_track );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add member variable
    HidIn_offset_data = type_engine_import_mvar( env, "int", "@Hid_data", FALSE );
    if( HidIn_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // add static member variable joystick
    if( type_engine_import_svar( env, "int", "JOYSTICK", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_JOYSTICK ) == FALSE )
        goto error;
    
    // add static member variable keyboard
    if( type_engine_import_svar( env, "int", "KEYBOARD", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_KEYBOARD ) == FALSE )
        goto error;
    
    // add static member variable mouse
    if( type_engine_import_svar( env, "int", "MOUSE", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_MOUSE ) == FALSE )
        goto error;
    
    // add static member variable wii_remote
    if( type_engine_import_svar( env, "int", "WII_REMOTE", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_WIIREMOTE ) == FALSE )
        goto error;
    
    // add static member variable wii_remote
    if( type_engine_import_svar( env, "int", "TILT_SENSOR", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_TILTSENSOR ) == FALSE )
        goto error;
    
    // add static member variable tablet
    if( type_engine_import_svar( env, "int", "TABLET", TRUE,
                                ( t_CKUINT ) &CK_HID_DEV_TABLET ) == FALSE )
        goto error;
    
    // add static member variable axisMotion
    if( type_engine_import_svar( env, "int", "AXIS_MOTION", TRUE,
                                ( t_CKUINT ) &CK_HID_JOYSTICK_AXIS ) == FALSE )
        goto error;
    
    // add static member variable buttonDown
    if( type_engine_import_svar( env, "int", "BUTTON_DOWN", TRUE,
                                ( t_CKUINT ) &CK_HID_BUTTON_DOWN ) == FALSE )
        goto error;
    
    // add static member variable buttonUp
    if( type_engine_import_svar( env, "int", "BUTTON_UP", TRUE,
                                ( t_CKUINT ) &CK_HID_BUTTON_UP ) == FALSE )
        goto error;
    
    // add static member variable joystickHat
    if( type_engine_import_svar( env, "int", "JOYSTICK_HAT", TRUE,
                                ( t_CKUINT ) &CK_HID_JOYSTICK_HAT ) == FALSE )
        goto error;
    
    // add static member variable JOYSTICK_BALL
    if( type_engine_import_svar( env, "int", "JOYSTICK_BALL", TRUE,
                                ( t_CKUINT ) &CK_HID_JOYSTICK_BALL ) == FALSE )
        goto error;
    
    // add static member variable mouseMotion
    if( type_engine_import_svar( env, "int", "MOUSE_MOTION", TRUE,
                                ( t_CKUINT ) &CK_HID_MOUSE_MOTION ) == FALSE )
        goto error;
    
    // add static member variable mouseWheel
    if( type_engine_import_svar( env, "int", "MOUSE_WHEEL", TRUE,
                                ( t_CKUINT ) &CK_HID_MOUSE_WHEEL ) == FALSE )
        goto error;
    
    // add static member variable DEVICE_CONNECTED
    if( type_engine_import_svar( env, "int", "DEVICE_CONNECTED", TRUE,
                                ( t_CKUINT ) &CK_HID_DEVICE_CONNECTED ) == FALSE )
        goto error;
    
    // add static member variable DEVICE_DISCONNECTED
    if( type_engine_import_svar( env, "int", "DEVICE_DISCONNECTED", TRUE,
                                ( t_CKUINT ) &CK_HID_DEVICE_DISCONNECTED ) == FALSE )
        goto error;
    
    // add static member variable ACCELEROMETER
    if( type_engine_import_svar( env, "int", "ACCELEROMETER", TRUE,
                                ( t_CKUINT ) &CK_HID_ACCELEROMETER ) == FALSE )
        goto error;
    
    // add static member variable LED
    if( type_engine_import_svar( env, "int", "LED", TRUE,
                                ( t_CKUINT ) &CK_HID_LED ) == FALSE )
        goto error;
    
    // add static member variable LED
    if( type_engine_import_svar( env, "int", "FORCE_FEEDBACK", TRUE,
                                ( t_CKUINT ) &CK_HID_FORCE_FEEDBACK ) == FALSE )
        goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    /*
     // init base class
     if( !type_engine_import_class_begin( env, "HidOut", "Object",
     env->global(), HidOut_ctor ) )
     return FALSE;
     
     // add open()
     func = make_new_mfun( "int", "open", HidOut_open );
     func->add_arg( "int", "port" );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add good()
     func = make_new_mfun( "int", "good", HidOut_good );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add num()
     func = make_new_mfun( "int", "num", HidOut_num );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add name()
     func = make_new_mfun( "string", "name", HidOut_name );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add printerr()
     func = make_new_mfun( "void", "printerr", HidOut_printerr );
     func->add_arg( "int", "print_or_not" );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add send()
     func = make_new_mfun( "int", "send", HidOut_send );
     func->add_arg( "HidMsg", "msg" );
     if( !type_engine_import_mfun( env, func ) ) goto error;
     
     // add member variable
     HidOut_offset_data = type_engine_import_mvar( env, "int", "@HidOut_data", FALSE );
     if( HidOut_offset_data == CK_INVALID_OFFSET ) goto error;
     
     // end the class import
     type_engine_import_class_end( env );
     */
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}
#endif // __DISABLE_HID__





#ifndef __DISABLE_MIDI__
// static
static t_CKUINT MidiRW_offset_data = 0;
static t_CKUINT MidiMsgOut_offset_data = 0;
static t_CKUINT MidiMsgIn_offset_data = 0;
//-----------------------------------------------------------------------------
// name: init_class_MidiRW()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_MidiRW( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    
    // init base class
    if( !type_engine_import_class_begin( env, "MidiRW", "Object",
                                        env->global(), MidiRW_ctor, MidiRW_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", MidiRW_open );
    func->add_arg( "string", "filename" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "int", "close", MidiRW_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write()
    func = make_new_mfun( "int", "write", MidiRW_write );
    func->add_arg( "MidiMsg", "msg" );
    func->add_arg( "time", "t" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add read()
    func = make_new_mfun( "int", "read", MidiRW_read );
    func->add_arg( "MidiMsg", "msg" );
    //func->add_arg( "time", "t" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add member variable
    MidiRW_offset_data = type_engine_import_mvar( env, "int", "@MidiRW_data", FALSE );
    if( MidiRW_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    // init base class
    if( !type_engine_import_class_begin( env, "MidiMsgOut", "Object",
                                        env->global(), MidiMsgOut_ctor, MidiMsgOut_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", MidiMsgOut_open );
    func->add_arg( "string", "filename" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "int", "close", MidiMsgOut_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add write()
    func = make_new_mfun( "int", "write", MidiMsgOut_write );
    func->add_arg( "MidiMsg", "msg" );
    func->add_arg( "time", "t" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add member variable
    MidiMsgOut_offset_data = type_engine_import_mvar( env, "int", "@MidiMsgOut_data", FALSE );
    if( MidiMsgOut_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    // init base class
    if( !type_engine_import_class_begin( env, "MidiMsgIn", "Object",
                                        env->global(), MidiMsgIn_ctor, MidiMsgIn_dtor ) )
        return FALSE;
    
    // add open()
    func = make_new_mfun( "int", "open", MidiMsgIn_open );
    func->add_arg( "string", "filename" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add close()
    func = make_new_mfun( "int", "close", MidiMsgIn_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add read()
    func = make_new_mfun( "int", "read", MidiMsgIn_read );
    func->add_arg( "MidiMsg", "msg" );
    //func->add_arg( "time", "t" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add member variable
    MidiMsgIn_offset_data = type_engine_import_mvar( env, "int", "@MidiMsgIn_data", FALSE );
    if( MidiMsgIn_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    // initialize
    // HidInManager::init();
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}
#endif // __DISABLE_MIDI__






//-----------------------------------------------------------------------------
// IO API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( io_dummy )
{
    EM_error3( "(IO): internal error! inside an abstract function! help!" );
    EM_error3( "    note: please hunt down someone (e.g., Ge) to fix this..." );
    assert( FALSE );
    RETURN->v_int = 0;
}

CK_DLL_SFUN( io_newline )
{
    RETURN->v_string = g_newline;
}


#ifndef __DISABLE_FILEIO__
//-----------------------------------------------------------------------------
// FileIO API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( fileio_ctor )
{ }

CK_DLL_DTOR( fileio_dtor )
{ }

CK_DLL_MFUN( fileio_open )
{
    std::string filename = GET_NEXT_STRING(ARGS)->str();
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKINT default_flags =
    Chuck_IO_File::FLAG_READ_WRITE | Chuck_IO_File::TYPE_ASCII;
    
    RETURN->v_int = f->open(filename, default_flags);
}

CK_DLL_MFUN( fileio_openflags )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    std::string filename = GET_NEXT_STRING(ARGS)->str();
    t_CKINT flags = GET_NEXT_INT(ARGS);
    
    RETURN->v_int = f->open(filename, flags);
}

CK_DLL_MFUN( fileio_close )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    f->close();
}

CK_DLL_MFUN( fileio_good )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->good();
}

CK_DLL_MFUN( fileio_flush )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    f->flush();
}

CK_DLL_MFUN( fileio_getmode )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->mode();
}

CK_DLL_MFUN( fileio_setmode )
{
    t_CKINT flag = GET_NEXT_INT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    f->mode( flag );
    RETURN->v_int = 0;
}

CK_DLL_MFUN( fileio_size )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->size();
}

CK_DLL_MFUN( fileio_seek )
{
    t_CKINT pos = GET_NEXT_INT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    f->seek(pos);
}

CK_DLL_MFUN( fileio_tell )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->tell();
}

CK_DLL_MFUN( fileio_isdir )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->isDir();
}

CK_DLL_MFUN( fileio_dirlist )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    Chuck_Array4 * a = f->dirList();
    RETURN->v_object = a;
}

/*
 CK_DLL_MFUN( fileio_read )
 {
 t_CKINT len = GET_NEXT_INT(ARGS);
 Chuck_IO_File * f = (Chuck_IO_File *)SELF;
 
 Chuck_String * s = f->read( len );
 RETURN->v_object = s;
 }
 */

CK_DLL_MFUN( fileio_readline )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    Chuck_String * ret = f->readLine();
    RETURN->v_object = ret;
}

CK_DLL_MFUN( fileio_readint )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKINT defaultflags = Chuck_IO::INT32;
    
    /* (ATODO: doesn't look like asynchronous reading will work)
     if (f->mode() == Chuck_IO::MODE_ASYNC)
     {
     // set up arguments
     Chuck_IO::async_args *args = new Chuck_IO::async_args;
     args->RETURN = (void *)RETURN;
     args->fileio_obj = f;
     args->intArg = defaultflags;
     // set shred to wait for I/O completion
     f->m_asyncEvent->wait( SHRED, SHRED->vm_ref );
     // start thread
     bool ret = f->m_thread->start( f->readInt_thread, (void *)args );
     if (!ret) {
     cerr << "m_thread->start failed; recreating m_thread" << endl;
     delete f->m_thread;
     f->m_thread = new XThread;
     ret = f->m_thread->start( f->readInt_thread, (void *)args );
     if (!ret) {
     EM_error3( "(FileIO): failed to start thread for asynchronous mode I/O" );
     }
     }
     } else {*/
    t_CKINT ret = f->readInt( defaultflags );
    RETURN->v_int = ret;
    //}
    // ATODO: Debug
    //sleep(1);
    //cerr << "fileio_readint exiting" << endl;
}

CK_DLL_MFUN( fileio_readintflags )
{
    t_CKINT flags = GET_NEXT_INT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKINT ret = f->readInt( flags );
    
    RETURN->v_int = ret;
}

CK_DLL_MFUN( fileio_readfloat )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKFLOAT ret = f->readFloat();
    RETURN->v_float = ret;
}

CK_DLL_MFUN( fileio_eof )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKBOOL ret = f->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( fileio_more )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKBOOL ret = !f->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( fileio_writestring )
{
    std::string val = GET_NEXT_STRING(ARGS)->str();
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->stringArg = std::string(val);
        // set shred to wait for I/O completion
        assert( SHRED != NULL );
        f->m_asyncEvent->wait( SHRED, SHRED->vm_ref );
        // start thread
        bool ret = f->m_thread->start( f->writeStr_thread, (void *)args );
        if (!ret) {
            // for some reason, the XThread object needs to be
            // deleted and reconstructed every time after call #375
            delete f->m_thread;
            f->m_thread = new XThread;
            ret = f->m_thread->start( f->writeStr_thread, (void *)args );
            if (!ret) {
                EM_error3( "(FileIO): failed to start thread for asynchronous mode I/O" );
            }
        }
    } else {
        f->write(val);
    }
}

CK_DLL_MFUN( fileio_writeint )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->intArg = val;
        // set shred to wait for I/O completion
        assert( SHRED != NULL );
        f->m_asyncEvent->wait( SHRED, SHRED->vm_ref );
        // start thread
        bool ret = f->m_thread->start( f->writeInt_thread, (void *)args );
        if (!ret) {
            // for some reason, the XThread object needs to be
            // deleted and reconstructed every time after call #375
            delete f->m_thread;
            f->m_thread = new XThread;
            ret = f->m_thread->start( f->writeInt_thread, (void *)args );
            if (!ret) {
                EM_error3( "(FileIO): failed to start thread for asynchronous mode I/O" );
            }
        }
    } else {
        f->write(val);
    }
}

CK_DLL_MFUN( fileio_writeintflags )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    t_CKINT flags = GET_NEXT_INT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // TODO: pass flags in args
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->intArg = val;
        // set shred to wait for I/O completion
        assert( SHRED != NULL );
        f->m_asyncEvent->wait( SHRED, SHRED->vm_ref );
        // start thread
        bool ret = f->m_thread->start( f->writeInt_thread, (void *)args );
        if (!ret) {
            // for some reason, the XThread object needs to be
            // deleted and reconstructed every time after call #375
            delete f->m_thread;
            f->m_thread = new XThread;
            ret = f->m_thread->start( f->writeInt_thread, (void *)args );
            if (!ret) {
                EM_error3( "(FileIO): failed to start thread for asynchronous mode I/O" );
            }
        }
    } else {
        f->write(val, flags);
    }
}

CK_DLL_MFUN( fileio_writefloat )
{
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->floatArg = val;
        // set shred to wait for I/O completion
        assert( SHRED != NULL );
        f->m_asyncEvent->wait( SHRED, SHRED->vm_ref );
        // start thread
        bool ret = f->m_thread->start( f->writeFloat_thread, (void *)args );
        if (!ret) {
            // for some reason, the XThread object needs to be
            // deleted and reconstructed every time after call #375
            delete f->m_thread;
            f->m_thread = new XThread;
            ret = f->m_thread->start( f->writeFloat_thread, (void *)args );
            if (!ret) {
                EM_error3( "(FileIO): failed to start thread for asynchronous mode I/O" );
            }
        }
    } else {
        f->write(val);
    }
}
#endif // __DISABLE_FILEIO__


//-----------------------------------------------------------------------------
// Chout API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( chout_close )
{
    // problem
    CK_FPRINTF_STDERR( "[chuck]: cannot close 'chout'...\n" );
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->close();
}

CK_DLL_MFUN( chout_good )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    RETURN->v_int = c->good();
}

CK_DLL_MFUN( chout_flush )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->flush();
}

CK_DLL_MFUN( chout_getmode )
{
    // problem
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    RETURN->v_int = c->mode();
}

CK_DLL_MFUN( chout_setmode )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->mode( GET_NEXT_INT(ARGS) );
    RETURN->v_int = c->mode();
}

/*
 CK_DLL_MFUN( chout_read )
 {
 t_CKINT len = GET_NEXT_INT(ARGS);
 Chuck_IO_File * f = (Chuck_IO_File *)SELF;
 
 Chuck_String * s = f->read( len );
 RETURN->v_object = s;
 }
 */

CK_DLL_MFUN( chout_readline )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    Chuck_String * ret = c->readLine();
    RETURN->v_object = ret;
}

CK_DLL_MFUN( chout_readint )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    RETURN->v_int = c->readInt( Chuck_IO::INT32 );
}

CK_DLL_MFUN( chout_readintflags )
{
    t_CKINT flags = GET_NEXT_INT(ARGS);
    
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    t_CKINT ret = c->readInt( flags );
    
    RETURN->v_int = ret;
}

CK_DLL_MFUN( chout_readfloat )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    t_CKFLOAT ret = c->readFloat();
    RETURN->v_float = ret;
}

CK_DLL_MFUN( chout_eof )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    t_CKBOOL ret = c->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( chout_more )
{
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    t_CKBOOL ret = !c->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( chout_writestring )
{
    std::string val = GET_NEXT_STRING(ARGS)->str();
    
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->write( val );
}

CK_DLL_MFUN( chout_writeint )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->write(val);
}

CK_DLL_MFUN( chout_writefloat )
{
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->write(val);
}




//-----------------------------------------------------------------------------
// Cherr API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( cherr_close )
{
    // problem
    CK_FPRINTF_STDERR( "[chuck]: cannot close 'cherr'...\n" );
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    c->close();
}

CK_DLL_MFUN( cherr_good )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    RETURN->v_int = c->good();
}

CK_DLL_MFUN( cherr_flush )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    c->flush();
}

CK_DLL_MFUN( cherr_getmode )
{
    // problem
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    RETURN->v_int = c->mode();
}

CK_DLL_MFUN( cherr_setmode )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    c->mode( GET_NEXT_INT(ARGS) );
    RETURN->v_int = c->mode();
}

/*
 CK_DLL_MFUN( cherr_read )
 {
 t_CKINT len = GET_NEXT_INT(ARGS);
 Chuck_IO_File * f = (Chuck_IO_File *)SELF;
 
 Chuck_String * s = f->read( len );
 RETURN->v_object = s;
 }
 */

CK_DLL_MFUN( cherr_readline )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    Chuck_String * ret = c->readLine();
    RETURN->v_object = ret;
}

CK_DLL_MFUN( cherr_readint )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    RETURN->v_int = c->readInt( Chuck_IO::INT32 );
}

CK_DLL_MFUN( cherr_readintflags )
{
    t_CKINT flags = GET_NEXT_INT(ARGS);
    
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    t_CKINT ret = c->readInt( flags );
    
    RETURN->v_int = ret;
}

CK_DLL_MFUN( cherr_readfloat )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    t_CKFLOAT ret = c->readFloat();
    RETURN->v_float = ret;
}

CK_DLL_MFUN( cherr_eof )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    t_CKBOOL ret = c->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( cherr_more )
{
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    t_CKBOOL ret = !c->eof();
    RETURN->v_int = ret;
}

CK_DLL_MFUN( cherr_writestring )
{
    std::string val = GET_NEXT_STRING(ARGS)->str();
    
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    c->write( val );
}

CK_DLL_MFUN( cherr_writeint )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    
    Chuck_IO_Chout * c = SHRED->vm_ref->chout();
    c->write(val);
}

CK_DLL_MFUN( cherr_writefloat )
{
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    
    Chuck_IO_Cherr * c = SHRED->vm_ref->cherr();
    c->write(val);
}




#ifndef __DISABLE_MIDI__

//-----------------------------------------------------------------------------
// MidiIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiIn_ctor )
{
    MidiIn * min = new MidiIn;
    min->SELF = SELF;
    OBJ_MEMBER_INT(SELF, MidiIn_offset_data) = (t_CKINT)min;
}

CK_DLL_DTOR( MidiIn_dtor )
{
    delete (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    OBJ_MEMBER_INT(SELF, MidiIn_offset_data) = 0;
}

CK_DLL_MFUN( MidiIn_open )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    t_CKINT port = GET_CK_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, port );
}

CK_DLL_MFUN( MidiIn_open_named ) // added 1.3.0.0
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    Chuck_String * name = GET_CK_STRING(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, name->str() );
}

CK_DLL_MFUN( MidiIn_good )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    RETURN->v_int = (t_CKINT)min->good();
}

CK_DLL_MFUN( MidiIn_num )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    RETURN->v_int = min->num();
}

CK_DLL_MFUN( MidiIn_printerr )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    t_CKINT print_or_not = GET_CK_INT(ARGS);
    min->set_suppress( !print_or_not );
}

CK_DLL_MFUN( MidiIn_name )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    // TODO: memory leak, please fix, Thanks.
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    // only if valid
    if( min->good() )
        a->set( min->min->getPortName( min->num() ) );
    RETURN->v_string = a;
}

CK_DLL_MFUN( MidiIn_recv )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    Chuck_Object * fake_msg = GET_CK_OBJECT(ARGS);
    MidiMsg the_msg;
    RETURN->v_int = min->recv( &the_msg );
    if( RETURN->v_int )
    {
        OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1) = the_msg.data[0];
        OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2) = the_msg.data[1];
        OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3) = the_msg.data[2];
    }
}


CK_DLL_MFUN( MidiIn_can_wait )
{
    MidiIn * min = (MidiIn *)OBJ_MEMBER_INT(SELF, MidiIn_offset_data);
    RETURN->v_int = min->empty();
}


//-----------------------------------------------------------------------------
// MidiOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiOut_ctor )
{
    OBJ_MEMBER_INT(SELF, MidiOut_offset_data) = (t_CKUINT)new MidiOut;
}

CK_DLL_DTOR( MidiOut_dtor )
{
    delete (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    OBJ_MEMBER_INT(SELF, MidiOut_offset_data) = 0;
}

CK_DLL_MFUN( MidiOut_open )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    t_CKINT port = GET_CK_INT(ARGS);
    RETURN->v_int = mout->open( port );
}

CK_DLL_MFUN( MidiOut_open_named ) // added 1.3.0.0
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    Chuck_String * name = GET_CK_STRING(ARGS);
    RETURN->v_int = mout->open( name->str() );
}

CK_DLL_MFUN( MidiOut_good )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    RETURN->v_int = (t_CKINT)mout->good();
}

CK_DLL_MFUN( MidiOut_num )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    RETURN->v_int = mout->num();
}

CK_DLL_MFUN( MidiOut_name )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    // TODO: memory leak, please fix, Thanks.
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    // only if valid
    if( mout->good() )
        a->set( mout->mout->getPortName( mout->num() ) );
    RETURN->v_string = a;
}

CK_DLL_MFUN( MidiOut_printerr )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    t_CKINT print_or_not = GET_CK_INT(ARGS);
    mout->set_suppress( !print_or_not );
}

CK_DLL_MFUN( MidiOut_send )
{
    MidiOut * mout = (MidiOut *)OBJ_MEMBER_INT(SELF, MidiOut_offset_data);
    Chuck_Object * fake_msg = GET_CK_OBJECT(ARGS);
    MidiMsg the_msg;
    the_msg.data[0] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1);
    the_msg.data[1] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2);
    the_msg.data[2] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3);
    RETURN->v_int = mout->send( &the_msg );
}

#endif // __DISABLE_MIDI__


#ifndef __DISABLE_HID__
//-----------------------------------------------------------------------------
// HidMsg API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( HidMsg_is_axis_motion )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_JOYSTICK_AXIS ? 1 : 0 );
}

CK_DLL_MFUN( HidMsg_is_button_down )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_BUTTON_DOWN ? 1 : 0 );
}

CK_DLL_MFUN( HidMsg_is_button_up )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_BUTTON_UP ? 1 : 0 );
}

CK_DLL_MFUN( HidMsg_is_mouse_motion )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_MOUSE_MOTION ? 1 : 0 );
}

CK_DLL_MFUN( HidMsg_is_hat_motion )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_JOYSTICK_HAT ? 1 : 0 );
}

CK_DLL_MFUN( HidMsg_is_wheel_motion )
{
    RETURN->v_int = ( ( t_CKINT ) OBJ_MEMBER_INT( SELF, HidMsg_offset_type ) ==
                     CK_HID_MOUSE_WHEEL ? 1 : 0 );
}

//-----------------------------------------------------------------------------
// HidIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HidIn_ctor )
{
    HidIn * min = new HidIn;
    min->SELF = SELF;
    OBJ_MEMBER_INT(SELF, HidIn_offset_data) = (t_CKINT)min;
}

CK_DLL_DTOR( HidIn_dtor )
{
    delete (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    OBJ_MEMBER_INT(SELF, HidIn_offset_data) = 0;
}

CK_DLL_MFUN( HidIn_open )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT type = GET_NEXT_INT(ARGS);
    t_CKINT num = GET_NEXT_INT(ARGS);
    // CK_FPRINTF_STDERR( "HidIn_open %li %li\n", type, num );
    RETURN->v_int = min->open( SHRED->vm_ref, type, num );
}

CK_DLL_MFUN( HidIn_open_named )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    Chuck_String * name = GET_NEXT_STRING(ARGS);
    std::string s = name->str();
    RETURN->v_int = min->open( SHRED->vm_ref, s );
}

CK_DLL_MFUN( HidIn_open_joystick )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_JOYSTICK, num );
}

CK_DLL_MFUN( HidIn_open_mouse )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_MOUSE, num );
}

CK_DLL_MFUN( HidIn_open_keyboard )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_KEYBOARD, num );
}

CK_DLL_MFUN( HidIn_open_tiltsensor )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_TILTSENSOR, 0 );
}

CK_DLL_MFUN( HidIn_good )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    RETURN->v_int = (t_CKINT)min->good();
}

CK_DLL_MFUN( HidIn_num )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    RETURN->v_int = min->num();
}

CK_DLL_MFUN( HidIn_printerr )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT print_or_not = GET_CK_INT(ARGS);
    min->set_suppress( !print_or_not );
}

CK_DLL_MFUN( HidIn_name )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    // TODO: memory leak, please fix, Thanks.
    // Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    // only if valid
    // if( min->good() )
    //     a->str = min->phin->getPortName( min->num() );
    // TODO: is null problem?
    RETURN->v_string = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    RETURN->v_string->set( min->name() );
}

CK_DLL_MFUN( HidIn_recv )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    Chuck_Object * fake_msg = GET_CK_OBJECT(ARGS);
    HidMsg the_msg;
    RETURN->v_int = min->recv( &the_msg );
    if( RETURN->v_int )
    {
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_device_type) = the_msg.device_type;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_device_num) = the_msg.device_num;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_type) = the_msg.type;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_which) = the_msg.eid;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_idata) = the_msg.idata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_fdata) = the_msg.fdata[0];
        
        // mouse motion specific member variables
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_deltax) = the_msg.idata[0];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_deltay) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_cursorx) = the_msg.idata[2];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_cursory) = the_msg.idata[3];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaledcursorx) = the_msg.fdata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaledcursory) = the_msg.fdata[1];
        
        // axis motion specific member variables
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_axis_position) = the_msg.idata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaled_axis_position) = the_msg.fdata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_axis_position2) = the_msg.fdata[0];
        
        // hat motion specific variables
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_hat_position) = the_msg.idata[0];
        
        // keyboard specific variables
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_key) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_ascii) = the_msg.idata[2];
        
        // accelerometer (tilt sensor, wii remote) specific members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_x) = the_msg.idata[0];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_y) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_z) = the_msg.idata[2];
        
        // multitouch stuff - added 1.3.0.0
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_touchx) = the_msg.fdata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_touchy) = the_msg.fdata[1];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_touchsize) = the_msg.fdata[2];
    }
}

CK_DLL_MFUN( HidIn_read )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT type = GET_NEXT_INT(ARGS);
    t_CKINT num = GET_NEXT_INT(ARGS);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    
    HidMsg the_msg;
    
    RETURN->v_int = min->read( type, num, &the_msg );
    
    if( RETURN->v_int )
    {
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_device_type) = the_msg.device_type;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_device_num) = the_msg.device_num;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_type) = the_msg.type;
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_which) = the_msg.eid;
        
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_idata) = the_msg.idata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_fdata) = the_msg.fdata[0];
        
        // mouse motion specific member members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_deltax) = the_msg.idata[0];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_deltay) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_cursorx) = the_msg.idata[2];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_cursory) = the_msg.idata[3];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaledcursorx) = the_msg.fdata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaledcursory) = the_msg.fdata[1];
        
        // joystick axis specific member members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_axis_position) = the_msg.idata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_scaled_axis_position) = the_msg.fdata[0];
        OBJ_MEMBER_FLOAT(fake_msg, HidMsg_offset_axis_position2) = the_msg.fdata[0];
        
        // joystick hat specific member members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_hat_position) = the_msg.idata[0];
        
        // keyboard specific members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_key) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_ascii) = the_msg.idata[2];
        
        // accelerometer (tilt sensor, wii remote) specific members
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_x) = the_msg.idata[0];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_y) = the_msg.idata[1];
        OBJ_MEMBER_INT(fake_msg, HidMsg_offset_z) = the_msg.idata[2];
    }
}

CK_DLL_MFUN( HidIn_send )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    
    HidMsg the_msg;
    the_msg.device_type = OBJ_MEMBER_INT( fake_msg, HidMsg_offset_device_type );
    the_msg.device_num = OBJ_MEMBER_INT( fake_msg, HidMsg_offset_device_num );
    the_msg.type = OBJ_MEMBER_INT( fake_msg, HidMsg_offset_type );
    the_msg.eid = OBJ_MEMBER_INT( fake_msg, HidMsg_offset_which );
    the_msg.idata[0] = OBJ_MEMBER_INT( fake_msg, HidMsg_offset_idata );
    
    RETURN->v_int = min->send( &the_msg );
}

CK_DLL_MFUN( HidIn_can_wait )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    RETURN->v_int = min->empty();
}

CK_DLL_SFUN( HidIn_read_tilt_sensor )
{
    static HidIn * hi;
    static t_CKBOOL hi_good = TRUE;
    
    Chuck_Array4 * array = new Chuck_Array4( FALSE, 3 );
    array->set( 0, 0 );
    array->set( 1, 0 );
    array->set( 2, 0 );
    
    // TODO: reference count?
    array->add_ref();
    RETURN->v_object = array;
    
    if( hi_good == FALSE )
        return;
    
    if( !hi )
    {
        hi = new HidIn;
        if( !hi->open( SHRED->vm_ref, CK_HID_DEV_TILTSENSOR, 0 ) )
        {
            hi_good = FALSE;
            return;
        }
    }
    
    HidMsg msg;
    
    if( !hi->read( CK_HID_ACCELEROMETER, 0, &msg ) )
    {
        return;
    }
    
    array->set( 0, msg.idata[0] );
    array->set( 1, msg.idata[1] );
    array->set( 2, msg.idata[2] );
}

CK_DLL_SFUN( HidIn_ctrl_tiltPollRate )
{
    // get srate
    if( !SHRED || !SHRED->vm_ref )
    {
        // problem
        CK_FPRINTF_STDERR( "[chuck](via HID): can't set tiltPollRate on NULL shred/VM...\n" );
        RETURN->v_dur = 0;
        return;
    }
    t_CKFLOAT srate = SHRED->vm_ref->srate();
    
    // get arg
    t_CKDUR v = GET_NEXT_DUR( ARGS );
    
    // get in microseconds
    t_CKINT usec = (t_CKINT)( v / srate * 1000000 );
    
    // make sure it's nonnegative
    if( usec < 0 ) usec = 0;
    
    // go
    RETURN->v_dur = TiltSensor_setPollRate( usec ) * srate / 1000000;
}

CK_DLL_SFUN( HidIn_cget_tiltPollRate )
{
    // get srate
    if( !SHRED || !SHRED->vm_ref )
    {
        // problem
        CK_FPRINTF_STDERR( "[chuck](via HID): can't get tiltPollRate on NULL shred/VM...\n" );
        RETURN->v_dur = 0;
        return;
    }
    t_CKFLOAT srate = SHRED->vm_ref->srate();
    
    RETURN->v_dur = TiltSensor_getPollRate() * srate / 1000000;
}

CK_DLL_SFUN( HidIn_start_cursor_track )
{
    RETURN->v_int = !Mouse_start_cursor_track();
}

CK_DLL_SFUN( HidIn_stop_cursor_track )
{
    RETURN->v_int = !Mouse_stop_cursor_track();
}

//-----------------------------------------------------------------------------
// HidOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HidOut_ctor )
{
    OBJ_MEMBER_INT(SELF, HidOut_offset_data) = (t_CKUINT)new HidOut;
}

CK_DLL_DTOR( HidOut_dtor )
{
    delete (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    OBJ_MEMBER_INT(SELF, HidOut_offset_data) = 0;
}

CK_DLL_MFUN( HidOut_open )
{
    HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    t_CKINT num = GET_CK_INT(ARGS);
    RETURN->v_int = mout->open( num );
}

CK_DLL_MFUN( HidOut_good )
{
    HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    RETURN->v_int = (t_CKINT)mout->good();
}

CK_DLL_MFUN( HidOut_num )
{
    HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    RETURN->v_int = mout->num();
}

CK_DLL_MFUN( HidOut_name )
{
    // HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    // TODO: memory leak, please fix, Thanks.
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    // only if valid
    // if( mout->good() )
    //     a->str = mout->mout->getPortName( mout->num() );
    RETURN->v_string = a;
}

CK_DLL_MFUN( HidOut_printerr )
{
    HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    t_CKINT print_or_not = GET_CK_INT(ARGS);
    mout->set_suppress( !print_or_not );
}

CK_DLL_MFUN( HidOut_send )
{
    HidOut * mout = (HidOut *)OBJ_MEMBER_INT(SELF, HidOut_offset_data);
    // Chuck_Object * fake_msg = GET_CK_OBJECT(ARGS);
    HidMsg the_msg;
    // the_msg.data[0] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, HidMsg_offset_data1);
    // the_msg.data[1] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, HidMsg_offset_data2);
    // the_msg.data[2] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, HidMsg_offset_data3);
    RETURN->v_int = mout->send( &the_msg );
}
#endif // __DISABLE_HID__


#ifndef __DISABLE_MIDI__
//-----------------------------------------------------------------------------
// MidiRW API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiRW_ctor )
{
#ifndef __DISABLE_MIDI__
    OBJ_MEMBER_INT(SELF, MidiRW_offset_data) = (t_CKUINT)new MidiRW;
#endif // __DISABLE_MIDI__
}

CK_DLL_DTOR( MidiRW_dtor )
{
#ifndef __DISABLE_MIDI__
    delete (MidiRW *)OBJ_MEMBER_INT(SELF, MidiRW_offset_data);
    OBJ_MEMBER_INT(SELF, MidiRW_offset_data) = 0;
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiRW_open )
{
#ifndef __DISABLE_MIDI__
    MidiRW * mrw = (MidiRW *)OBJ_MEMBER_INT(SELF, MidiRW_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    RETURN->v_int = mrw->open( filename );
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiRW_close )
{
#ifndef __DISABLE_MIDI__
    MidiRW * mrw = (MidiRW *)OBJ_MEMBER_INT(SELF, MidiRW_offset_data);
    RETURN->v_int = mrw->close();
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiRW_read )
{
#ifndef __DISABLE_MIDI__
    MidiRW * mrw = (MidiRW *)OBJ_MEMBER_INT(SELF, MidiRW_offset_data);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    MidiMsg the_msg;
    t_CKTIME time = 0.0;
    RETURN->v_int = mrw->read( &the_msg, &time );
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1) = the_msg.data[0];
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2) = the_msg.data[1];
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3) = the_msg.data[2];
    OBJ_MEMBER_TIME(fake_msg, MidiMsg_offset_when) = time;
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiRW_write )
{
#ifndef __DISABLE_MIDI__
    MidiRW * mrw = (MidiRW *)OBJ_MEMBER_INT(SELF, MidiRW_offset_data);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    t_CKTIME time = GET_NEXT_TIME(ARGS);
    MidiMsg the_msg;
    the_msg.data[0] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1);
    the_msg.data[1] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2);
    the_msg.data[2] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3);
    RETURN->v_int = mrw->write( &the_msg, &time );
#endif // __DISABLE_MIDI__
}


//-----------------------------------------------------------------------------
// MidiMsgOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsgOut_ctor )
{
#ifndef __DISABLE_MIDI__
    OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data) = (t_CKUINT)new MidiMsgOut;
#endif // __DISABLE_MIDI__
}

CK_DLL_DTOR( MidiMsgOut_dtor )
{
#ifndef __DISABLE_MIDI__
    delete (MidiMsgOut *)OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data);
    OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data) = 0;
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgOut_open )
{
#ifndef __DISABLE_MIDI__
    MidiMsgOut * mrw = (MidiMsgOut *)OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    RETURN->v_int = mrw->open( filename );
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgOut_close )
{
#ifndef __DISABLE_MIDI__
    MidiMsgOut * mrw = (MidiMsgOut *)OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data);
    RETURN->v_int = mrw->close();
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgOut_write )
{
#ifndef __DISABLE_MIDI__
    MidiMsgOut * mrw = (MidiMsgOut *)OBJ_MEMBER_INT(SELF, MidiMsgOut_offset_data);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    t_CKTIME time = GET_NEXT_TIME(ARGS);
    MidiMsg the_msg;
    the_msg.data[0] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1);
    the_msg.data[1] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2);
    the_msg.data[2] = (t_CKBYTE)OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3);
    RETURN->v_int = mrw->write( &the_msg, &time );
#endif // __DISABLE_MIDI__
}


//-----------------------------------------------------------------------------
// MidiMsgIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsgIn_ctor )
{
#ifndef __DISABLE_MIDI__
    OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data) = (t_CKUINT)new MidiMsgIn;
#endif // __DISABLE_MIDI__
}

CK_DLL_DTOR( MidiMsgIn_dtor )
{
#ifndef __DISABLE_MIDI__
    delete (MidiMsgIn *)OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data);
    OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data) = 0;
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgIn_open )
{
#ifndef __DISABLE_MIDI__
    MidiMsgIn * mrw = (MidiMsgIn *)OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    RETURN->v_int = mrw->open( filename );
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgIn_close )
{
#ifndef __DISABLE_MIDI__
    MidiMsgIn * mrw = (MidiMsgIn *)OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data);
    RETURN->v_int = mrw->close();
#endif // __DISABLE_MIDI__
}

CK_DLL_MFUN( MidiMsgIn_read )
{
#ifndef __DISABLE_MIDI__
    MidiMsgIn * mrw = (MidiMsgIn *)OBJ_MEMBER_INT(SELF, MidiMsgIn_offset_data);
    Chuck_Object * fake_msg = GET_NEXT_OBJECT(ARGS);
    MidiMsg the_msg;
    t_CKTIME time = 0.0;
    RETURN->v_int = mrw->read( &the_msg, &time );
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data1) = the_msg.data[0];
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data2) = the_msg.data[1];
    OBJ_MEMBER_INT(fake_msg, MidiMsg_offset_data3) = the_msg.data[2];
    OBJ_MEMBER_TIME(fake_msg, MidiMsg_offset_when) = time;
#endif // __DISABLE_MIDI__
}
#endif // __DISABLE_MIDI__




#ifndef __DISABLE_SERIAL__
// available baud rates
const t_CKUINT Chuck_IO_Serial::CK_BAUD_2400   = 2400;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_4800   = 4800;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_9600   = 9600;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_19200  = 19200;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_38400  = 38400;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_7200   = 7200;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_14400  = 14400;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_28800  = 28800;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_57600  = 57600;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_76800  = 76800;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_115200 = 115200;
const t_CKUINT Chuck_IO_Serial::CK_BAUD_230400 = 230400;

const t_CKUINT Chuck_IO_Serial::TYPE_NONE = 0;
const t_CKUINT Chuck_IO_Serial::TYPE_BYTE = 1;
const t_CKUINT Chuck_IO_Serial::TYPE_WORD = 2;
const t_CKUINT Chuck_IO_Serial::TYPE_INT = 3;
const t_CKUINT Chuck_IO_Serial::TYPE_FLOAT = 4;
const t_CKUINT Chuck_IO_Serial::TYPE_STRING = 5;
const t_CKUINT Chuck_IO_Serial::TYPE_LINE = 6;
const t_CKUINT Chuck_IO_Serial::TYPE_WRITE = 100;

std::list<Chuck_IO_Serial *> Chuck_IO_Serial::s_serials;

//    static const t_CKUINT TYPE_NONE = 0;
//    static const t_CKUINT TYPE_BYTE = 1;
//    static const t_CKUINT TYPE_WORD = 2;
//    static const t_CKUINT TYPE_INT = 3;
//    static const t_CKUINT TYPE_FLOAT = 4;
//    static const t_CKUINT TYPE_STRING = 5;
//    static const t_CKUINT TYPE_LINE = 6;
//    static const t_CKUINT TYPE_WRITE = 100;

const t_CKUINT CHUCK_IO_DEFAULT_BUFSIZE = 1024;
const char * CHUCK_IO_SCANF_STRING = "%1024s";

CK_DLL_MFUN(serialio_canWait);

void * Chuck_IO_Serial::shell_read_cb( void *_this )
{
    Chuck_IO_Serial *cereal = (Chuck_IO_Serial *) _this;
    
    cereal->read_cb();
    
    return NULL;
}

void * Chuck_IO_Serial::shell_write_cb( void *_this )
{
    Chuck_IO_Serial *cereal = (Chuck_IO_Serial *) _this;
    
    cereal->write_cb();
    
    return NULL;
}

void Chuck_IO_Serial::shutdown()
{
    EM_log(CK_LOG_INFO, "shutting down serial devices");

    for(std::list<Chuck_IO_Serial *>::iterator i = s_serials.begin(); i != s_serials.end(); i++)
    {
        (*i)->close();
    }
}


Chuck_IO_Serial::Chuck_IO_Serial( Chuck_VM * vm ) :
m_asyncRequests(CircularBuffer<Request>(32)),
m_asyncResponses(CircularBuffer<Request>(32)),
m_asyncWriteRequests(CircularBuffer<Request>(32)),
m_writeBuffer(1024)
{
    m_fd = -1;
    m_cfd = NULL;
    
    m_io_buf_max = CHUCK_IO_DEFAULT_BUFSIZE;
    m_io_buf = new unsigned char[m_io_buf_max];
    m_io_buf_pos = m_io_buf_available = 0;
    
    m_tmp_buf_max = CHUCK_IO_DEFAULT_BUFSIZE;
    m_tmp_buf = new unsigned char[m_tmp_buf_max];
    
    m_read_thread = NULL;
    m_event_buffer = NULL;
    
    m_write_thread = NULL;
    m_do_write_thread = TRUE;

    m_do_exit = FALSE;
    
    s_serials.push_back(this);
    
    m_vmRef = vm;
}

Chuck_IO_Serial::~Chuck_IO_Serial()
{
    m_do_read_thread = FALSE;
    m_do_write_thread = FALSE;
    SAFE_DELETE(m_read_thread);
    if( m_event_buffer )
        m_vmRef->destroy_event_buffer( m_event_buffer );
    
    close();
    
    SAFE_DELETE(m_io_buf);
    m_io_buf_max = 0;
    m_io_buf_pos = m_io_buf_available = 0;
    
    SAFE_DELETE(m_tmp_buf);
    m_tmp_buf_max = 0;
    
    s_serials.remove(this);
}

t_CKBOOL Chuck_IO_Serial::ready()
{
#ifndef __WINDOWS_DS__
    struct pollfd pollfds;
    pollfds.fd = m_fd;
    pollfds.events = POLLIN;
    pollfds.revents = 0;
    
    int result = poll(&pollfds, 1, 0);
    
    if( result > 0 && (pollfds.revents & POLLIN) ) return TRUE;
    else return FALSE; // TODO: error handling
#else
    return FALSE;
#endif
}

t_CKBOOL Chuck_IO_Serial::open( const t_CKUINT i, t_CKINT flags, t_CKUINT baud )
{
    vector<string> ports = SerialIOManager::availableSerialDevices();
    
    if( i < ports.size() )
    {
        const string &path = ports[i];
        
        open(path, flags, baud);
    }
    else
    {
        EM_log(CK_LOG_WARNING, "(Serial.open): error: invalid port number");
        return FALSE;
    }
    
    return TRUE;
}


t_CKBOOL Chuck_IO_Serial::open( const std::string & path, t_CKINT flags, t_CKUINT baud )
{
    if( flags & Chuck_IO_File::TYPE_BINARY )
    {
        m_flags = Chuck_IO_File::TYPE_BINARY;
    }
    else if( flags & Chuck_IO_File::TYPE_ASCII )
    {
        m_flags = Chuck_IO_File::TYPE_ASCII;
    }
    else
    {
        EM_log(CK_LOG_WARNING, "(Serial.open): warning: invalid binary/ASCII flag requested, defaulting to ASCII");
        m_flags = Chuck_IO_File::TYPE_ASCII;
    }
    
    int fd = ::open( path.c_str(), O_RDWR );
    if( fd < 0 )
    {
        EM_log(CK_LOG_WARNING, "(Serial.open): error: unable to open file at '%s'", path.c_str());
        return FALSE;
    }
    
    m_fd = fd;
    m_path = path;
    
    // set default baud rate
    setBaudRate(baud);
    
    m_cfd = fdopen(fd, "a+");
    m_iomode = MODE_ASYNC;
#ifndef WIN32
    fcntl(m_fd, F_SETFL, O_NONBLOCK);
#endif
    m_eof = FALSE;
    
    return TRUE;
}


t_CKBOOL Chuck_IO_Serial::good()
{
    return m_fd >= 0 && m_cfd != NULL;
}

void Chuck_IO_Serial::close()
{
    if(m_iomode == MODE_SYNC)
    {
        close_int();
    }
    else
    {
        m_do_exit = TRUE;
#ifdef WIN32
        m_read_thread->wait(-1, TRUE);
        close_int();
#else
        m_read_thread->wait(-1, FALSE);
#endif
    }
}

void Chuck_IO_Serial::close_int()
{
    EM_log(CK_LOG_INFO, "(Serial.close): closing serial device '%s'", m_path.c_str());
    
    if(good())
    {
        if(m_cfd)
            fclose(m_cfd);
        else
            ::close(m_fd);
        m_fd = -1;
        m_cfd = NULL;
    }
}

void Chuck_IO_Serial::flush()
{
    if(good())
    {
        if(m_iomode == MODE_SYNC)
            fflush(m_cfd);
        else
            ; // TODO
    }
}

t_CKBOOL Chuck_IO_Serial::can_wait()
{
    return m_asyncResponses.numElements() == 0;
}

t_CKINT Chuck_IO_Serial::mode()
{
    return m_iomode;
}

void Chuck_IO_Serial::mode( t_CKINT flag )
{
    if( flag == MODE_SYNC )
        m_iomode = MODE_SYNC;
    else if( flag == MODE_ASYNC )
        m_iomode = MODE_ASYNC;
}

// reading
t_CKINT Chuck_IO_Serial::readInt( t_CKINT flags )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readInt): warning: file not open");
        return 0;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readInt): warning: calling synchronous read function on asynchronous file");
        return 0;
    }
    
    long i = 0; // 1.4.1.1 (ge) changed from t_CKUINT; to clear warning for fscanf using %li
    
    if( m_flags & Chuck_IO_File::TYPE_BINARY )
    {
        if( flags & INT8 )
        {
            uint8_t byte = 0;
            if(!fread(&byte, 1, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");
            
            i = byte;
        }
        else if( flags & INT16 )
        {
            uint16_t word = 0;
            if(!fread(&word, 2, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");
            
            i = word;
        }
        else if( flags & INT32 )
        {
            uint32_t dword = 0;
            if(!fread(&dword, 4, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");
            
            i = dword;
        }
    }
    else
    {
        if(!fscanf(m_cfd, "%li", &i))
            EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");
    }
    
    return i;
}

t_CKFLOAT Chuck_IO_Serial::readFloat()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readFloat): warning: file not open");
        return 0;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readFloat): warning: calling synchronous read function on asynchronous file");
        return 0;
    }
    
    t_CKFLOAT f = 0;
    
    if( m_flags & Chuck_IO_File::TYPE_BINARY )
    {
        if(!fread(&f, 4, 1, m_cfd))
            EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
    }
    else
    {
        if(!fscanf(m_cfd, "%lf", &f))
            EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
    }
    
    return f;
}

t_CKBOOL Chuck_IO_Serial::readString( std::string & str )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readString): warning: file not open");
        return FALSE;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readString): warning: calling synchronous read function on asynchronous file");
        return FALSE;
    }
    
    if( m_flags & Chuck_IO_File::TYPE_BINARY )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readString): warning: cannot read string from binary file");
        return FALSE;
    }
    
    if(!fscanf(m_cfd, CHUCK_IO_SCANF_STRING, &m_tmp_buf))
    {
        EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
        return FALSE;
    }
    
    str = (char *) m_tmp_buf;
    
    return TRUE;
}


Chuck_String * Chuck_IO_Serial::readLine()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): warning: file not open");
        return NULL;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): warning: calling synchronous read function on asynchronous file");
        return NULL;
    }
    
    if( m_flags & Chuck_IO_File::TYPE_BINARY )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): warning: cannot read line from binary file");
        return NULL;
    }
    
    
    if(!fgets((char *)m_tmp_buf, m_tmp_buf_max, m_cfd))
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): error: from fgets");
        return NULL;
    }
    
    Chuck_String * str = new Chuck_String;
    initialize_object(str, m_vmRef->env()->t_string);
    
    str->set( string((char *)m_tmp_buf) );
    
    return str;
}


t_CKBOOL Chuck_IO_Serial::eof()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.eof): warning: file not open");
        return FALSE;
    }
    
    if( MODE_SYNC )
        return (m_eof = feof( m_cfd ));
    else
        return m_eof;
}


// writing
void Chuck_IO_Serial::write( const std::string & val )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: file not open");
        return;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();
        
        t_CKUINT len = val.length();
        for(t_CKUINT i = 0; i < len; i++)
            // TODO: efficiency
            m_writeBuffer.put(val[i]);
        
        Request r;
        r.m_type = TYPE_WRITE;
        r.m_val = 0;
        r.m_num = 0;
        r.m_status = Request::RQ_STATUS_PENDING;
        
        m_asyncWriteRequests.put(r);
    }
    else if( m_iomode == MODE_SYNC )
    {
        fprintf( m_cfd, "%s", val.c_str() );
    }
}


void Chuck_IO_Serial::write( t_CKINT val )
{
    write( val, 4 );
}


void Chuck_IO_Serial::write( t_CKINT val, t_CKINT size )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: file not open");
        return;
    }
    
    if(!(size == 1 || size == 2 || size == 4 || size == 8))
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: invalid int size %li, ignoring write request", size);
        return;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();
        
        if( m_flags & Chuck_IO_File::TYPE_ASCII )
        {
            // TODO: don't use m_tmp_buf (thread safety?)
#ifdef WIN32
            _snprintf((char *)m_tmp_buf, m_tmp_buf_max, "%li", (long)val);
            m_tmp_buf[m_tmp_buf_max - 1] = '\0'; // force NULL terminator -- see http://www.di-mgt.com.au/cprog.html#snprintf
#else
            snprintf((char *)m_tmp_buf, m_tmp_buf_max, "%li", val);
#endif       
            t_CKUINT len = strlen((char *)m_tmp_buf);
            for(t_CKUINT i = 0; i < len; i++)
                // TODO: efficiency
                m_writeBuffer.put(m_tmp_buf[i]);
            
            Request r;
            r.m_type = TYPE_WRITE;
            r.m_val = 0;
            r.m_num = 0;
            r.m_status = Request::RQ_STATUS_PENDING;
            
            m_asyncWriteRequests.put(r);
        }
        else
        {
            char * buf = (char *)&val;
            
            // assume 4-byte int
            for(int i = 0; i < size; i++)
                m_writeBuffer.put(buf[i]);
            
            Request r;
            r.m_type = TYPE_WRITE;
            r.m_val = 0;
            r.m_num = 0;
            r.m_status = Request::RQ_STATUS_PENDING;
            
            m_asyncWriteRequests.put(r);
        }
    }
    else if( m_iomode == MODE_SYNC )
    {
        if( m_flags & Chuck_IO_File::TYPE_ASCII )
        {
            fprintf( m_cfd, "%li", (long)val );
        }
        else
        {
            // assume 4-byte int
            char * buf = (char *) &val;
            fwrite(buf, 1, size, m_cfd);
        }
    }
}

void Chuck_IO_Serial::write( t_CKFLOAT val )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: file not open");
        return;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();
        
        if( m_flags & Chuck_IO_File::TYPE_ASCII )
        {
#ifdef WIN32
            _snprintf((char *)m_tmp_buf, m_tmp_buf_max, "%f", val);
            m_tmp_buf[m_tmp_buf_max - 1] = '\0'; // force NULL terminator -- see http://www.di-mgt.com.au/cprog.html#snprintf
#else
            snprintf((char *)m_tmp_buf, m_tmp_buf_max, "%f", val);
#endif       
            
            t_CKUINT len = strlen((char *)m_tmp_buf);
            for(t_CKUINT i = 0; i < len; i++)
                // TODO: efficiency
                m_writeBuffer.put(m_tmp_buf[i]);
            
            Request r;
            r.m_type = TYPE_WRITE;
            r.m_val = 0;
            r.m_num = 0;
            r.m_status = Request::RQ_STATUS_PENDING;
            
            m_asyncWriteRequests.put(r);
        }
        else
        {
            char * buf = (char *) &val;
            
            // assume 4-byte float
            for(int i = 0; i < 4; i++)
                m_writeBuffer.put(buf[i]);
            
            Request r;
            r.m_type = TYPE_WRITE;
            r.m_val = 0;
            r.m_num = 0;
            r.m_status = Request::RQ_STATUS_PENDING;
            
            m_asyncWriteRequests.put(r);
        }
    }
    else if( m_iomode == MODE_SYNC )
    {
        if( m_flags & Chuck_IO_File::TYPE_ASCII )
        {
            fprintf( m_cfd, "%f", val );
        }
        else
        {
            // assume 4-byte int
            char * buf = (char *) &val;
            fwrite(buf, 1, size, m_cfd);
        }
    }
}


void Chuck_IO_Serial::writeBytes( Chuck_Array4 * arr )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.writeBytes): warning: file not open");
        return;
    }
    
    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();
        
        int len = arr->size();
        for(int i = 0; i < len; i++)
        {
            // TODO: efficiency
            t_CKUINT byte;
            arr->get(i, &byte);
            m_writeBuffer.put((char) byte);
        }
        
        Request r;
        r.m_type = TYPE_WRITE;
        r.m_val = 0;
        r.m_num = 0;
        r.m_status = Request::RQ_STATUS_PENDING;
        
        m_asyncWriteRequests.put(r);
    }
    else if( m_iomode == MODE_SYNC )
    {
        int len = arr->size();
        for(int i = 0; i < len; i++)
        {
            // TODO: efficiency
            t_CKUINT byte;
            arr->get(i, &byte);
            fwrite(&byte, 1, 1, m_cfd);
        }
    }
}

void Chuck_IO_Serial::start_read_thread()
{
    if(m_read_thread == NULL)
    {
        m_read_thread = new XThread();
#ifdef WIN32
        m_read_thread->start((unsigned int (__stdcall *)(void*))shell_read_cb, this);
#else
        m_read_thread->start(shell_read_cb, this);
#endif         
        assert(m_event_buffer == NULL);
        m_event_buffer = m_vmRef->create_event_buffer();
    }
}


t_CKBOOL Chuck_IO_Serial::readAsync( t_CKUINT type, t_CKUINT num )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readAsync): warning: file not open");
        return FALSE;
    }
    
    start_read_thread();
    
    Request read;
    read.m_type = type;
    read.m_num = num;
    read.m_val = 0;
    read.m_status = Request::RQ_STATUS_PENDING;
    
    if(!m_asyncRequests.atMaximum() )
    {
        m_asyncRequests.put(read);
    }
    else
    {
        EM_log(CK_LOG_SEVERE, "(SerialIO.readAsync): warning: request buffer overflow, dropping read");
        return FALSE;
    }
    
    return TRUE;
}

Chuck_String * Chuck_IO_Serial::getLine()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getLine): warning: file not open");
        return NULL;
    }
    
    Request r;
    
    Chuck_String * str = NULL;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_LINE && r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        str = (Chuck_String *) r.m_val;
        m_asyncResponses.get(r);
    }
    
    return str;
}


t_CKINT Chuck_IO_Serial::getByte()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getByte): warning: file not open");
        return 0;
    }
    
    Request r;
    
    t_CKINT i = 0;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_BYTE && r.m_num == 1 &&
       r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        i = (t_CKINT) r.m_val;
        m_asyncResponses.get(r);
    }
    
    return i;
}

Chuck_Array * Chuck_IO_Serial::getBytes()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getBytes): warning: file not open");
        return NULL;
    }
    
    Request r;
    
    Chuck_Array * arr = NULL;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_BYTE && r.m_num > 1 &&
       r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        arr = (Chuck_Array *) r.m_val;
        initialize_object(arr, m_vmRef->env()->t_array);
        m_asyncResponses.get(r);
    }
    
    return arr;
}

Chuck_Array * Chuck_IO_Serial::getInts()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getInts): warning: file not open");
        return NULL;
    }

    Request r;
    
    Chuck_Array * arr = NULL;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_INT && r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        arr = (Chuck_Array *) r.m_val;
        initialize_object(arr, m_vmRef->env()->t_array);
        m_asyncResponses.get(r);
    }
    
    return arr;
}

Chuck_Array * Chuck_IO_Serial::getFloats()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getFloats): warning: file not open");
        return NULL;
    }

    Request r;
    
    Chuck_Array * arr = NULL;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_FLOAT && r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        arr = (Chuck_Array *) r.m_val;
        initialize_object(arr, m_vmRef->env()->t_array);
        m_asyncResponses.get(r);
    }
    
    return arr;
}

Chuck_String * Chuck_IO_Serial::getString()
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.getString): warning: file not open");
        return NULL;
    }
    
    Request r;
    
    Chuck_String * str = NULL;
    
    if(m_asyncResponses.peek(r, 1) &&
       r.m_type == TYPE_STRING && r.m_status == Request::RQ_STATUS_SUCCESS)
    {
        str = (Chuck_String *) r.m_val;
        m_asyncResponses.get(r);
    }
    
    return str;
}


t_CKBOOL Chuck_IO_Serial::get_buffer(t_CKINT timeout_ms)
{
#ifndef WIN32    
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);
    
    timeval tv;
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = 1000 * (timeout_ms%1000);
    
    int result = select(m_fd+1, &fds, NULL, NULL, &tv);
    
    if(result > 0 && FD_ISSET(m_fd, &fds))
    {
        result = ::read(m_fd, m_io_buf, m_io_buf_max);
        if(result > 0)
        {
            m_io_buf_available = result;
            m_io_buf_pos = 0;
            
            EM_log(CK_LOG_FINE, "(SerialIO::get_buffer): read() returned %i bytes", result);
            
            return TRUE;
        }
        else
        {
            EM_log(CK_LOG_SEVERE, "(SerialIO::get_buffer): read() returned 0 bytes");
        }
    }
    else
    {
        EM_log(CK_LOG_FINE, "(SerialIO::get_buffer): select() timeout");
    }
    
    return FALSE;
    
#else

    int result = ::read(m_fd, m_io_buf, m_io_buf_max);
    if(result > 0)
    {
        m_io_buf_available = result;
        m_io_buf_pos = 0;
        
        return TRUE;
    }

    Sleep(timeout_ms);

    return FALSE;

#endif
}

// peek next byte
t_CKINT Chuck_IO_Serial::peek_buffer()
{
    // CK_FPRINTF_STDERR( "Chuck_IO_Serial::peek_buffer %i/%i\n", m_io_buf_pos, m_io_buf_available);
    
    if(m_io_buf_pos >= m_io_buf_available)
    {
        // refresh data
        while(!m_do_exit && !m_eof && !get_buffer(5))
            ;
        
        if(m_do_exit || m_eof)
            return -1;
    }
    
    return m_io_buf[m_io_buf_pos];
}

// consume next byte
// return -1 on error/exit condition
t_CKINT Chuck_IO_Serial::pull_buffer()
{
    // CK_FPRINTF_STDERR( "Chuck_IO_Serial::pull_buffer %i/%i\n", m_io_buf_pos, m_io_buf_available);
    
    if(m_io_buf_pos >= m_io_buf_available)
    {
        // refresh data
        while(!m_do_exit && !m_eof && !get_buffer(5))
            ;
        
        if(m_do_exit || m_eof)
            return -1;
    }
    
    return m_io_buf[m_io_buf_pos++];
}


t_CKINT Chuck_IO_Serial::buffer_bytes_to_tmp(t_CKINT num_bytes)
{
    t_CKUINT len = 0;
    while(len < num_bytes && !m_do_exit)
    {
        if(peek_buffer() == -1)
            break;
        
        m_tmp_buf[len++] = pull_buffer();
    }
    
    return len;
}


t_CKBOOL Chuck_IO_Serial::handle_line(Chuck_IO_Serial::Request &r)
{
    t_CKUINT len = 0;
    Chuck_String * str = NULL;
    
    while(!m_do_exit)
    {
        if(peek_buffer() == -1)
            break;
        
        if(peek_buffer() == '\r')
        {
            // consume newline character
            pull_buffer();
            if(peek_buffer() == '\n') pull_buffer(); // handle \r\n
            break;
        }
        
        if(peek_buffer() == '\n')
        {
            // consume newline character
            pull_buffer();
            if(peek_buffer() == '\r') pull_buffer(); // handle \n\r (unlikely)
            break;
        }
                
        m_tmp_buf[len++] = pull_buffer();
        
        if(len >= m_tmp_buf_max)
        {
            EM_log(CK_LOG_WARNING, "(Serial.handle_line): warning: line exceeds buffer length, truncating to %i characters", m_tmp_buf_max);
            break;
        }
    }
    
    if(m_do_exit)
        goto error;
    // TODO: eof
    
    str = new Chuck_String;
    str->set( std::string((char *)m_tmp_buf, len) );
    
    r.m_val = (t_CKUINT) str;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_SUCCESS;

    return TRUE;
    
error:
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;
    
#ifdef WIN32
    HANDLE hFile = (HANDLE) _get_osfhandle(m_fd);
    DWORD error;
    ClearCommError(hFile, &error, NULL);
    
    PurgeComm(hFile, PURGE_RXCLEAR);
#endif
    
    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_string(Chuck_IO_Serial::Request & r)
{
    return FALSE;
}

t_CKBOOL Chuck_IO_Serial::handle_float_ascii(Chuck_IO_Serial::Request & r)
{
    t_CKFLOAT val = 0;
    int numRead = 0;
    Chuck_Array8 * array = new Chuck_Array8(0);
    
    for(int i = 0; i < r.m_num && !m_do_exit; i++)
    {
        t_CKUINT len = 0;
        
        while(!m_do_exit)
        {
            if(peek_buffer() == -1)
                break;
            
            // TODO: '\r'?
            int c = peek_buffer();
            
            if(isdigit(c) || c=='.' || (len==0 && (c=='-' || c=='+')))
            {
                m_tmp_buf[len++] = pull_buffer();
            }
            else if(len > 0)
            {
                m_tmp_buf[len++] = '\0';
                val = strtod((char *)m_tmp_buf, NULL);
                
                numRead++;
                array->push_back(val);
                
                break;
            }
        }
    }
    
    if(m_do_exit || m_eof)
        goto error;

    r.m_num = numRead;
    r.m_val = (t_CKUINT) array;
    r.m_status = Request::RQ_STATUS_SUCCESS;
    
    return TRUE;
    
error:
    SAFE_DELETE(array);
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;
    
    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_int_ascii(Chuck_IO_Serial::Request & r)
{
    t_CKINT val = 0;
    int numRead = 0;
    Chuck_Array4 * array = new Chuck_Array4(FALSE, 0);
    for(int i = 0; i < r.m_num; i++)
    {
        t_CKUINT len = 0;
        
        while(!m_do_exit)
        {
            if(peek_buffer() == -1)
                break;
            
            int c = pull_buffer();
            
            if(isdigit(c) || (len==0 && (c=='-' || c=='+')))
            {
                m_tmp_buf[len++] = c;
            }
            else if(len > 0)
            {
                m_tmp_buf[len++] = '\0';
                val = strtol((char *)m_tmp_buf, NULL, 10);
                
                numRead++;
                array->push_back(val);
                
                break;
            }
        }
    }
    
    if(m_do_exit || m_eof)
        goto error;
    
    r.m_num = numRead;
    r.m_val = (t_CKUINT) array;
    r.m_status = Request::RQ_STATUS_SUCCESS;
    
    return TRUE;
    
error:
    SAFE_DELETE(array);
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;
    
    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_byte(Chuck_IO_Serial::Request & r)
{
    // binary
    int size = 1;    
    int num = r.m_num;
    t_CKUINT val = 0;
    
    if(size*num > m_tmp_buf_max)
    {
        int new_num = (int) floorf(((float)m_tmp_buf_max)/size);
        EM_log(CK_LOG_WARNING, "SerialIO.read_cb: error: request size %i too large (%i bytes), truncating to %i",
               num, size*num, new_num);
        num = new_num;
    }
    
    t_CKINT len = buffer_bytes_to_tmp(num);
    
    if(m_do_exit || m_eof)
        goto error;

    r.m_num = len;
    
    if(r.m_num == 1)
        val = m_tmp_buf[0];
    else
    {
        Chuck_Array4 * array = new Chuck_Array4(FALSE, r.m_num);
        for(int i = 0; i < r.m_num; i++)
        {
            array->set(i, m_tmp_buf[i]);
        }
        
        val = (t_CKUINT) array;
    }
    
    r.m_val = val;
    r.m_status = Request::RQ_STATUS_SUCCESS;
    
    return TRUE;
    
error:
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;
    
    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_float_binary(Chuck_IO_Serial::Request & r)
{
    // binary
    int size = 4; // SPENCERTODO: should these be based on arch (64 bit)?
    assert(sizeof(t_CKSINGLE) == 4);
    
    int num = r.m_num;
    
    if(size*num > m_tmp_buf_max)
    {
        int new_num = (int) floorf(((float)m_tmp_buf_max)/size);
        EM_log(CK_LOG_WARNING, "(SerialIO.read_cb): error: request size %i too large (%i bytes), truncating to %i",
               num, size*num, new_num);
        num = new_num;
    }
    
    r.m_num = buffer_bytes_to_tmp(size*num)/size;
    
    t_CKUINT val = 0;
    t_CKSINGLE * m_floats = (t_CKSINGLE *) m_tmp_buf;
    
    Chuck_Array8 * array = new Chuck_Array8(r.m_num);
    for(int i = 0; i < r.m_num; i++)
    {
        array->set(i, m_floats[i]);
    }
        
    val = (t_CKUINT) array;
    
    r.m_val = val;
    r.m_status = Request::RQ_STATUS_SUCCESS;
    
    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_int_binary(Chuck_IO_Serial::Request & r)
{
    // binary
    int size = 4; // SPENCERTODO: should these be based on arch (64 bit)?
    
    int num = r.m_num;
    
    if(size*num > m_tmp_buf_max)
    {
        int new_num = (int) floorf(((float)m_tmp_buf_max)/size);
        EM_log(CK_LOG_WARNING, "SerialIO.read_cb: error: request size %i too large (%i bytes), truncating to %i",
               num, size*num, new_num);
        num = new_num;
    }
    
    r.m_num = buffer_bytes_to_tmp(size*num)/size;
    
    t_CKUINT val = 0;
    uint32_t * m_ints = (uint32_t *) m_tmp_buf;
    
    Chuck_Array4 * array = new Chuck_Array4(FALSE, r.m_num);
    for(int i = 0; i < r.m_num; i++)
    {
        array->set(i, m_ints[i]);
    }
    
    val = (t_CKUINT) array;
    
    r.m_val = val;
    r.m_status = Request::RQ_STATUS_SUCCESS;
    
    return TRUE;
}


void Chuck_IO_Serial::read_cb()
{
    m_do_read_thread = TRUE;
    
    m_write_thread = new XThread;
#ifdef WIN32
        m_read_thread->start((unsigned int (__stdcall *)(void*))shell_write_cb, this);
#else
	m_write_thread->start(shell_write_cb, this);
#endif
    
    while(m_do_read_thread && !m_do_exit)
    {
        Request r;
        int num_responses = 0;
        
        while(m_asyncRequests.get(r) && m_do_read_thread && !m_do_exit)
        {
            if( m_asyncResponses.atMaximum() )
            {
                EM_log(CK_LOG_SEVERE, "SerialIO.read_cb: error: response buffer overflow, dropping read");
                continue;
            }
            
            if(m_flags & Chuck_IO_File::TYPE_ASCII)
            {
                switch(r.m_type)
                {
                    case TYPE_LINE:
                        handle_line(r);
                        break;
                        
                    case TYPE_STRING:
                        handle_string(r);
                        break;
                        
                    case TYPE_INT:
                        handle_int_ascii(r);
                        break;
                        
                    case TYPE_FLOAT:
                        handle_float_ascii(r);
                        break;
                        
                    default:
                        // this shouldnt happen
                        r.m_type = TYPE_NONE;
                        r.m_num = 0;
                        r.m_status = Request::RQ_STATUS_INVALID;
                        r.m_val = 0;
                        EM_log(CK_LOG_WARNING, "SerialIO.read_cb: error: invalid request");
                }
            }
            else
            {
                // binary
                switch(r.m_type)
                {
                    case TYPE_BYTE:
                        handle_byte(r);
                        break;
                    case TYPE_INT:
                        handle_int_binary(r);
                        break;
                    case TYPE_FLOAT:
                        handle_float_binary(r);
                        break;
                        
                    default:
                        // this shouldnt happen
                        r.m_type = TYPE_NONE;
                        r.m_num = 0;
                        r.m_status = Request::RQ_STATUS_INVALID;
                        r.m_val = 0;
                        EM_log(CK_LOG_WARNING, "SerialIO.read_cb: error: invalid request");
                }
            }
            
            m_asyncResponses.put(r);
            num_responses++;
        }
        
        if(m_asyncResponses.numElements() > 0)
            queue_broadcast(m_event_buffer);
        
        usleep(100);
    }
    
    m_write_thread->wait(-1);
    
    close_int();
}

void Chuck_IO_Serial::write_cb()
{
    m_do_write_thread = TRUE;
    
    char *tmp_writethread_buf = new char[CHUCK_IO_DEFAULT_BUFSIZE];
    t_CKINT tmp_writethread_buf_max = CHUCK_IO_DEFAULT_BUFSIZE;
    
    while(m_do_write_thread && !m_do_exit)
    {
        Request r;
        
        while(m_asyncWriteRequests.get(r) && m_do_write_thread && !m_do_exit)
        {
            if(r.m_type == TYPE_WRITE)
            {
                int numBytes = 0;
                char c;
                while(m_writeBuffer.get(c))
                {
                    tmp_writethread_buf[numBytes] = c;
                    numBytes++;
                }
                
                assert(numBytes < tmp_writethread_buf_max);
                
                if(numBytes)
                {
                    fwrite(tmp_writethread_buf, 1, numBytes, m_cfd);
                    fflush(m_cfd);
                }
            }
        }
        
        // todo: replace with semaphore?
        usleep(100);
    }
    
    delete[] tmp_writethread_buf;
    tmp_writethread_buf = NULL;
}


t_CKBOOL Chuck_IO_Serial::setBaudRate( t_CKUINT rate )
{
#ifndef WIN32
    // set serial IO baud rate
    struct termios tios;
    tcgetattr(m_fd, &tios);
    cfsetispeed(&tios, rate);
    cfsetospeed(&tios, rate);
    tcsetattr(m_fd, TCSAFLUSH, &tios);
    
    return TRUE;
#else
    DCB dcb;
    COMMTIMEOUTS timeouts;
    HANDLE hFile = (HANDLE) _get_osfhandle(m_fd);

    bool was_error = false;
    DWORD error_flag = 0;

    if(!hFile)
    {
        goto error;
    }

    if(!GetCommTimeouts(hFile, &timeouts))
    {
        goto error;
    }

    timeouts.ReadIntervalTimeout = 10;
    timeouts.WriteTotalTimeoutConstant = 5000;

    if(!SetCommTimeouts(hFile, &timeouts))
    {
        goto error;
    }

    if(!GetCommState(hFile, &dcb))
    {
        goto error;
    }

    // set baud rate
    dcb.BaudRate = rate;

    // set important stuff
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;

    if(!SetCommState(hFile, &dcb))
    {
        //goto error;
        was_error = true;
        ClearCommError(hFile, &error_flag, NULL);
    }

    // set everything else
    if(!GetCommState(hFile, &dcb))
    {
        goto error;
    }

    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = TRUE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fAbortOnError = TRUE;
    dcb.Parity = NOPARITY;
    
    if(!SetCommState(hFile, &dcb))
    {
        //goto error;
        was_error = true;
        ClearCommError(hFile, &error_flag, NULL);
    }

    if(!was_error)
        return TRUE;

error:

    DWORD error = GetLastError();

    return FALSE;

#endif
}


t_CKUINT Chuck_IO_Serial::getBaudRate()
{
#ifndef WIN32
    // set serial IO baud rate
    struct termios tios;
    tcgetattr(m_fd, &tios);
    speed_t rate = cfgetispeed(&tios);
    return rate;
#else

    DCB dcb;
    HANDLE hFile = (HANDLE) _get_osfhandle(m_fd);

    if(!hFile)
    {
        goto error;
    }

    if(!GetCommState(hFile, &dcb))
    {
        goto error;
    }

    return dcb.BaudRate;

error:
    return 0;

#endif
}


//-----------------------------------------------------------------------------
// serialio API
//-----------------------------------------------------------------------------
CK_DLL_SFUN( serialio_list );
CK_DLL_ALLOC( serialio_alloc );
CK_DLL_CTOR( serialio_ctor );
CK_DLL_DTOR( serialio_dtor );
CK_DLL_MFUN( serialio_open );
CK_DLL_MFUN( serialio_close );
CK_DLL_MFUN( serialio_ready );
CK_DLL_MFUN( serialio_readLine );
CK_DLL_MFUN( serialio_onLine );
CK_DLL_MFUN( serialio_onByte );
CK_DLL_MFUN( serialio_onBytes );
CK_DLL_MFUN( serialio_onInts );
CK_DLL_MFUN( serialio_onFloats );
CK_DLL_MFUN( serialio_getLine );
CK_DLL_MFUN( serialio_getByte );
CK_DLL_MFUN( serialio_getBytes );
CK_DLL_MFUN( serialio_getInts );
CK_DLL_MFUN( serialio_setBaudRate );
CK_DLL_MFUN( serialio_getBaudRate );
CK_DLL_MFUN( serialio_writeByte );
CK_DLL_MFUN( serialio_writeBytes );

CK_DLL_MFUN( serialio_flush )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    cereal->flush();
}


//-----------------------------------------------------------------------------
// name: init_class_serialio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_serialio( Chuck_Env * env )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    std::string doc = "serial input/output. popularly used to communicate with systems like Arduino.";
    
    Chuck_Type * type = type_engine_import_class_begin( env, "SerialIO", "IO",
                                                        env->global(), serialio_ctor, serialio_dtor, doc.c_str() );
    // TODO: ctor/dtor?
    if( !type )
        return FALSE;
    
    // HACK; SPENCERTODO: better way to set this
    type->allocator = serialio_alloc;
    
    // add list()
    func = make_new_sfun( "string[]", "list", serialio_list );
    func->doc = "get list of available serial devices.";
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    func = make_new_mfun("int", "open", serialio_open);
    func->add_arg("int", "i");
    func->add_arg("int", "baud");
    func->add_arg("int", "mode");
    func->doc = "open serial device i with specified baud rate and mode (binary or ASCII).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun("void", "close", serialio_close);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add ready (data available)
    func = make_new_mfun("int", "dataAvailable", serialio_ready);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add readLine
    func = make_new_mfun("string", "readLine", serialio_readLine);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onLine
    func = make_new_mfun("SerialIO", "onLine", serialio_onLine);
    func->doc = "wait for one line (ASCII mode only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onByte
    func = make_new_mfun("SerialIO", "onByte", serialio_onByte);
    func->doc = "wait for one byte (binary mode only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onBytes
    func = make_new_mfun("SerialIO", "onBytes", serialio_onBytes);
    func->add_arg("int", "num");
    func->doc = "wait for requested number of bytes (binary mode only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onInts
    func = make_new_mfun("SerialIO", "onInts", serialio_onInts);
    func->add_arg("int", "num");
    func->doc = "wait for requested number of ints (ASCII or binary mode).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onInts
    func = make_new_mfun("SerialIO", "onFloats", serialio_onFloats);
    func->add_arg("int", "num");
    func->doc = "wait for requested number of floats (ASCII or binary mode).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getLine
    func = make_new_mfun("string", "getLine", serialio_getLine);
    func->doc = "get next requested line.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getByte
    func = make_new_mfun("int", "getByte", serialio_getByte);
    func->doc = "get next requested byte. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getBytes
    func = make_new_mfun("int[]", "getBytes", serialio_getBytes);
    func->doc = "get next requested number of bytes. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getInts
    func = make_new_mfun("int[]", "getInts", serialio_getInts);
    func->doc = "get next requested number of integers. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add writeByte
    func = make_new_mfun("void", "writeByte", serialio_writeByte);
    func->add_arg("int", "b");
    func->doc = "write a single byte.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add writeBytes
    func = make_new_mfun("void", "writeBytes", serialio_writeBytes);
    func->add_arg("int[]", "b");
    func->doc = "write array of bytes.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add setBaudRate
    func = make_new_mfun("int", "baudRate", serialio_setBaudRate);
    func->add_arg("int", "r");
    func->doc = "set baud rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getBaudRate
    func = make_new_mfun("int", "baudRate", serialio_getBaudRate);
    func->doc = "get current baud rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add getBaudRate
    func = make_new_mfun("void", "flush", serialio_flush);
    func->doc = "flush the IO buffer.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add can_wait
    // func = make_new_mfun("int", "can_wait", serialio_canWait);
    // func->doc = "";
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add baud rate constants
    type_engine_import_svar(env, "int", "B2400",   TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_2400, "2400 baud");
    type_engine_import_svar(env, "int", "B4800",   TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_4800, "4800 baud");
    type_engine_import_svar(env, "int", "B9600",   TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_9600, "9600 baud");
    type_engine_import_svar(env, "int", "B19200",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_19200, "19200 baud");
    type_engine_import_svar(env, "int", "B38400",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_38400, "38400 baud");
    type_engine_import_svar(env, "int", "B7200",   TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_7200, "7200 baud");
    type_engine_import_svar(env, "int", "B14400",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_14400, "14400 baud");
    type_engine_import_svar(env, "int", "B28800",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_28800, "28800 baud");
    type_engine_import_svar(env, "int", "B57600",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_57600, "57600 baud");
    type_engine_import_svar(env, "int", "B76800",  TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_76800, "76800 baud");
    type_engine_import_svar(env, "int", "B115200", TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_115200, "115200 baud");
    type_engine_import_svar(env, "int", "B230400", TRUE, (t_CKUINT) &Chuck_IO_Serial::CK_BAUD_230400, "230400 baud");

    type_engine_import_svar(env, "int", "BINARY", TRUE, (t_CKUINT) &Chuck_IO_File::TYPE_BINARY, "Binary mode");
    type_engine_import_svar(env, "int", "ASCII", TRUE, (t_CKUINT) &Chuck_IO_File::TYPE_ASCII, "ASCII mode");

    // add examples
    if( !type_engine_import_add_ex( env, "serial/byte.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/bytes.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/ints-bin.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/ints.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/lines.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/list.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/write-bytes.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/write.ck" ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}



CK_DLL_SFUN( serialio_list )
{
    vector<string> devices = SerialIOManager::availableSerialDevices();
    
    // ISSUE: 64-bit
    Chuck_Array4 * array = new Chuck_Array4(TRUE, 0);
    initialize_object( array, SHRED->vm_ref->env()->t_array );
    
    for(vector<string>::iterator i = devices.begin(); i != devices.end(); i++)
    {
        Chuck_String * name = new Chuck_String(*i);
        initialize_object(name, SHRED->vm_ref->env()->t_string);
        array->push_back((t_CKUINT) name);
    }
    
    RETURN->v_object = array;
}


CK_DLL_ALLOC( serialio_alloc )
{
    return new Chuck_IO_Serial( SHRED->vm_ref );
}

CK_DLL_CTOR( serialio_ctor )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    SHRED->add_serialio(cereal);
}

CK_DLL_DTOR( serialio_dtor )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    if(cereal)
    {
        SHRED->remove_serialio(cereal);
        cereal->close();
    }
}

CK_DLL_MFUN( serialio_open )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    t_CKINT num = GET_NEXT_INT(ARGS);
    t_CKINT baud = GET_NEXT_INT(ARGS);
    t_CKINT mode = GET_NEXT_INT(ARGS);
    
    RETURN->v_int = cereal->open(num, mode, baud);
}

CK_DLL_MFUN( serialio_close )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    cereal->close();
}

CK_DLL_MFUN( serialio_ready )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    RETURN->v_int = cereal->ready();
}

CK_DLL_MFUN( serialio_readLine )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    RETURN->v_string = cereal->readLine();
}


CK_DLL_MFUN( serialio_onLine )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    cereal->readAsync(Chuck_IO_Serial::TYPE_LINE, 1);
    
    RETURN->v_object = cereal;
}


CK_DLL_MFUN( serialio_onByte )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    cereal->readAsync(Chuck_IO_Serial::TYPE_BYTE, 1);
    
    RETURN->v_object = cereal;
}


CK_DLL_MFUN( serialio_onBytes )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    t_CKINT num = GET_NEXT_INT(ARGS);
    
    cereal->readAsync(Chuck_IO_Serial::TYPE_BYTE, num);
    
    RETURN->v_object = cereal;
}


CK_DLL_MFUN( serialio_onInts )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    t_CKINT num = GET_NEXT_INT(ARGS);
    
    cereal->readAsync(Chuck_IO_Serial::TYPE_INT, num);
    
    RETURN->v_object = cereal;
}


CK_DLL_MFUN( serialio_onFloats )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    
    t_CKINT num = GET_NEXT_INT(ARGS);
    
    cereal->readAsync(Chuck_IO_Serial::TYPE_FLOAT, num);
    
    RETURN->v_object = cereal;
}


CK_DLL_MFUN( serialio_getLine )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_string = cereal->getLine();
}


CK_DLL_MFUN( serialio_getByte )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_int = cereal->getByte();
}


CK_DLL_MFUN( serialio_getBytes )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_object = cereal->getBytes();
}


CK_DLL_MFUN( serialio_getInts )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_object = cereal->getInts();
}


CK_DLL_MFUN( serialio_setBaudRate )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    cereal->setBaudRate(GET_NEXT_INT(ARGS));
    RETURN->v_int = cereal->getBaudRate();
}


CK_DLL_MFUN( serialio_getBaudRate )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_int = cereal->getBaudRate();
}


CK_DLL_MFUN( serialio_writeByte )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    t_CKINT byte = GET_NEXT_INT(ARGS);
    cereal->write(byte, 1);
}


CK_DLL_MFUN( serialio_writeBytes )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) SELF;
    Chuck_Array4 * arr = (Chuck_Array4 *) GET_NEXT_OBJECT(ARGS);
    cereal->writeBytes(arr);
}

CK_DLL_MFUN(serialio_canWait)
{
    Chuck_IO_Serial *cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_int = cereal->can_wait();
}

#endif // __DISABLE_SERIAL__
