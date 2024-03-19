/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
#include "chuck_instr.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "util_platforms.h"
#include "util_string.h"

#ifndef __DISABLE_HID__
#include "hidio_sdl.h"
#endif

#ifndef __DISABLE_MIDI__
#include "midiio_rtmidi.h"
#endif

#ifndef __DISABLE_SERIAL__
#include "util_serial.h"
#endif

#ifndef __PLATFORM_WINDOWS__
#include <sys/select.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#else
#include <io.h>
#include "dirent_win32.h"
#endif // __PLATFORM_WINDOWS__

#include <fcntl.h>
#include <math.h>
#include <stdint.h> // 1.5.0.5

#include <iomanip>
#include <iostream>
using namespace std;


#ifdef __PLATFORM_WINDOWS__
typedef BYTE uint8_t;
typedef WORD uint16_t;
// ge: this needed in earlier/some versions of windows
// 1.5.0.7 (ge) make "__WINDOWS_MODERN__" default
#ifdef __WINDOWS_OLDSCHOOL__ // was: #ifndef __WINDOWS_MODERN__
typedef __int32 int32_t; // 1.4.1.0 (ge) added
typedef DWORD uint32_t;
#endif
#endif

// for legacy use of disable fileio | 1.5.1.5
#ifdef __DISABLE_FILEIO__
  // auto define new, more granular macro
  #ifndef __DISABLE_ASYNCH_IO__
  #define __DISABLE_ASYNCH_IO__
  #endif
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
    std::string doc = "Base class for other IO classes such as FileIO, StdOut and StdErr. Besides IO.newline(), it’s unlikely you need to use this class directly.";

    // log
    EM_log( CK_LOG_HERALD, "class 'IO'" );
    
    // init as base class
    // TODO: ctor/dtor?
    // TODO: replace dummy with pure function
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc.c_str() ) )
        return FALSE;

    // add good()
    func = make_new_mfun( "int", "good", io_dummy_int );
    func->doc = "Returns whether IO is ready for reading.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add close()
    func = make_new_mfun( "void", "close", io_dummy_void );
    func->doc = "Close the currently open IO.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add flush()
    func = make_new_mfun( "void", "flush", io_dummy_void );
    func->doc = "Write any buffered output.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add mode(int)
    func = make_new_mfun( "int", "mode", io_dummy_int );
    func->add_arg( "int", "flag" );
    func->doc = "Set the current IO mode; either IO.MODE_ASYNC or IO.MODE_SYNC.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add mode()
    func = make_new_mfun( "int", "mode", io_dummy_int );
    func->doc = "Get the current IO mode; either IO.MODE_ASYNC or IO.MODE_SYNC.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // TODO: add this later?
    // add read()
    // func = make_new_mfun( "string", "read", io_dummy_string );
    // func->add_arg( "int", "length" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readLine()
    func = make_new_mfun( "string", "readLine", io_dummy_string );
    func->doc = "Read until an end-of-line character.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readInt()
    func = make_new_mfun( "int", "readInt", io_dummy_int );
    func->add_arg( "int", "flags" );
    func->doc = "Read and return the next integer; binary mode: 'flags' denotes the bit-size of int (IO.INT8, IO.INT16, or IO.INT32).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readFloat()
    func = make_new_mfun( "float", "readFloat", io_dummy_float );
    func->add_arg( "int", "flags" );
    func->doc = "Read and return the next floating point value; binary mode: 'flags' denotes the size of float to read (IO.FLOAT32 or IO.FLOAT64).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add eof()
    func = make_new_mfun( "int", "eof", io_dummy_int );
    func->doc = "Return whether end-of-file has been reached; the opposite of .more().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", io_dummy_int );
    func->doc = "Return whether there is more to read; the opposite of .eof().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(string)
    func = make_new_mfun( "void", "write", io_dummy_void );
    func->add_arg( "string", "val" );
    func->doc = "Write string 'val'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(int)
    func = make_new_mfun( "void", "write", io_dummy_void );
    func->add_arg( "int", "val" );
    func->doc = "Write integer 'val'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(float)
    func = make_new_mfun( "void", "write", io_dummy_void );
    func->add_arg( "float", "val" );
    func->doc = "Write floating point number 'val'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add newline
    func = make_new_sfun( "string", "newline", io_newline );
    func->doc = "newline character; same as IO.nl().";
    if( !type_engine_import_sfun( env, func ) ) goto error;
    // add nl
    func = make_new_sfun( "string", "nl", io_newline );
    func->doc = "newline character; same as IO.newline().";
    if( !type_engine_import_sfun( env, func ) ) goto error;
    // hmm
    // func = make_new_sfun( "string", "newlineEx2VistaHWNDVisualFoxProA", io_newline );
    // func->doc = "_";
    // if( !type_engine_import_sfun( env, func ) ) goto error;
    // new line string
    initialize_object( g_newline, env->ckt_string, NULL, NULL );
    // add reference, since we are keeping a reference of it | 1.5.1.7
    CK_SAFE_ADD_REF( g_newline );
    // also lock it | 1.5.1.7
    g_newline->lock();
    // set as newline
    g_newline->set( "\n" );

    // add TYPE_ASCII
    if( !type_engine_import_svar( env, "int", "ASCII",
                                 TRUE, (t_CKUINT)&Chuck_IO::TYPE_ASCII, "Flag denoting ASCII IO mode." ) ) goto error;
    // add TYPE_BINARY
    if( !type_engine_import_svar( env, "int", "BINARY",
                                 TRUE, (t_CKUINT)&Chuck_IO::TYPE_BINARY, "Flag denoting binary IO mode." ) ) goto error;
    // add INT8
    if( !type_engine_import_svar( env, "int", "INT8",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT8, "Flag denoting 8-bit integer type." ) ) goto error;
    // add INT16
    if( !type_engine_import_svar( env, "int", "INT16",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT16, "Flag denoting 16-bit integer type." ) ) goto error;
    // add INT24 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "INT24",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT24, "Flag denoting 24-bit integer type." ) ) goto error;
    // add INT32
    if( !type_engine_import_svar( env, "int", "INT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT32, "Flag denoting 32-bit integer type." ) ) goto error;
    // add INT64 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "INT64",
                                 TRUE, (t_CKUINT)&Chuck_IO::INT64, "Flag denoting 64-bit integer type." ) ) goto error;
    // add SINT8 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "SINT8",
                                 TRUE, (t_CKUINT)&Chuck_IO::SINT8, "Flag denoting 8-bit signed integer type." ) ) goto error;
    // add SINT16 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "SINT16",
                                 TRUE, (t_CKUINT)&Chuck_IO::SINT16, "Flag denoting 16-bit signed integer type." ) ) goto error;
    // add SINT24 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "SINT24",
                                 TRUE, (t_CKUINT)&Chuck_IO::SINT24, "Flag denoting 24-bit signed integer type." ) ) goto error;
    // add SINT32 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "SINT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::SINT32, "Flag denoting 32-bit signed integer type." ) ) goto error;
    // add SINT64 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "SINT64",
                                 TRUE, (t_CKUINT)&Chuck_IO::SINT64, "Flag denoting 64-bit signed integer type." ) ) goto error;
    // add UINT8 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "UINT8",
                                 TRUE, (t_CKUINT)&Chuck_IO::UINT8, "Flag denoting 8-bit unsigned integer type." ) ) goto error;
    // add UINT16 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "UINT16",
                                 TRUE, (t_CKUINT)&Chuck_IO::UINT16, "Flag denoting 16-bit unsigned integer type." ) ) goto error;
    // add UINT24 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "UINT24",
                                 TRUE, (t_CKUINT)&Chuck_IO::UINT24, "Flag denoting 24-bit unsigned integer type." ) ) goto error;
    // add UINT32 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "UINT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::UINT32, "Flag denoting 32-bit unsigned integer type." ) ) goto error;
    // add UINT64 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "UINT64",
                                 TRUE, (t_CKUINT)&Chuck_IO::UINT64, "Flag denoting 64-bit unsigned integer type." ) ) goto error;
    // add FLOAT32 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "FLOAT32",
                                 TRUE, (t_CKUINT)&Chuck_IO::FLOAT32, "Flag denoting 32-bit floating point type." ) ) goto error;
    // add FLOAT64 | 1.5.0.1 (ge) added
    if( !type_engine_import_svar( env, "int", "FLOAT64",
                                 TRUE, (t_CKUINT)&Chuck_IO::FLOAT64, "Flag denoting 64-bit floating point type." ) ) goto error;
    // add MODE_SYNC
    if( !type_engine_import_svar( env, "int", "MODE_SYNC",
                                 TRUE, (t_CKUINT)&Chuck_IO::MODE_SYNC, "Flag denoting synchronous IO." ) ) goto error;
#ifndef __DISABLE_ASYNCH_IO__
    // add MODE_ASYNC
    if( !type_engine_import_svar( env, "int", "MODE_ASYNC",
                                 TRUE, (t_CKUINT)&Chuck_IO::MODE_ASYNC, "Flag denoting asychronous IO." ) ) goto error;
#endif
    // add FLAG_READONLY
    if( !type_engine_import_svar( env, "int", "READ",
                                  TRUE, (t_CKUINT)&Chuck_IO::FLAG_READONLY, "Flag denoting read mode." ) ) goto error;
    // add FLAG_WRITEONLY
    if( !type_engine_import_svar( env, "int", "WRITE",
                                  TRUE, (t_CKUINT)&Chuck_IO::FLAG_WRITEONLY, "Flag denoting write mode." ) ) goto error;
    // add FLAG_APPEND
    if( !type_engine_import_svar( env, "int", "APPEND",
                                  TRUE, (t_CKUINT)&Chuck_IO::FLAG_APPEND, "Flag denoting append mode." ) ) goto error;
    // add FLAG_READ_WRITE
    if( !type_engine_import_svar( env, "int", "READ_WRITE",
                                  TRUE, (t_CKUINT)&Chuck_IO::FLAG_READ_WRITE, "Flag denoting read/write mode." ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_fileio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_fileio( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_HERALD, "class 'FileIO'" );

    // init as base class
    // TODO: ctor/dtor?
    // TODO: replace dummy with pure function
    if( !type_engine_import_class_begin( env, type, env->global(), fileio_ctor, fileio_dtor,
        "File input and output utilities for reading, writing, seeking, etc. See examples for usage." ) )
        return FALSE;

    // add open(string)
    func = make_new_mfun( "int", "open", fileio_open );
    func->add_arg( "string", "path" );
    func->doc = "Open a file by name (and by default in ASCII mode).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add open(string, flags)
    func = make_new_mfun( "int", "open", fileio_openflags );
    func->add_arg( "string", "path" );
    func->add_arg( "int", "flags" );
    func->doc = "Open a file by name with flags (bitwise combinations of IO.READ, IO.WRITE, IO.READ_WRITE, IO_APPEND, IO.ASCII, IO.BINARY).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add good()
    func = make_new_mfun( "int", "good", fileio_good );
    func->doc = "Returns whether the file is ready for reading.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add close()
    func = make_new_mfun( "void", "close", fileio_close );
    func->doc = "Close (and flush) the currently open file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add flush()
    func = make_new_mfun( "void", "flush", fileio_flush );
    func->doc = "Write any buffered output to file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add mode()
    func = make_new_mfun( "int", "mode", fileio_getmode );
    func->doc = "Get file IO mode (IO.MODE_SYNC or IO.MODE_ASYNC).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add mode(int)
    func = make_new_mfun( "int", "mode", fileio_setmode );
    func->add_arg( "int", "flag" );
    func->doc = "Set file IO mode (IO.MODE_SYNC or IO.MODE_ASYNC).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add size()
    func = make_new_mfun( "int", "size", fileio_size );
    func->doc = "Return the size of the file in bytes, or -1 if no file is opened or if a directory is opened.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add seek(int)
    func = make_new_mfun( "void", "seek", fileio_seek );
    func->add_arg( "int", "pos" );
    func->doc = "Seek to a specified byte offset in file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add tell()
    func = make_new_mfun( "int", "tell", fileio_tell );
    func->doc = "Return the byte read offset of the file, or -1 if no file is opened.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isDir()
    func = make_new_mfun( "int", "isDir", fileio_isdir );
    func->doc = "Return if the open file is a directory.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add dirList()
    func = make_new_mfun( "string[]", "dirList", fileio_dirlist );
    func->doc = "Get an array of file names in an open directory.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add read()
    // func = make_new_mfun( "string", "read", fileio_read );
    // func->add_arg( "int", "length" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readLine()
    func = make_new_mfun( "string", "readLine", fileio_readline );
    func->doc = "Read and return the next line from file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readInt()
    // func = make_new_mfun( "int", "readInt", fileio_readint );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readInt(int)
    func = make_new_mfun( "int", "readInt", fileio_readintflags );
    func->add_arg( "int", "flags" );
    func->doc = "Read and return an integer; binary mode: 'flags' specifies int size to read (IO.INT8, IO.INT16, IO.INT32 default to unsigned values; for signed integers use IO.SINT8, IO.SINT16, IO.SINT32).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readFloat()
    func = make_new_mfun( "float", "readFloat", fileio_readfloat );
    func->doc = "Read and return the next floating point value.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readFloat()
    func = make_new_mfun( "float", "readFloat", fileio_readfloatflags );
    func->add_arg( "int", "flags" );
    func->doc = "Read and return the next floating point value; if binary mode: 'flags' denotes the size of float to read (IO.FLOAT32 or IO.FLOAT64).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add eof()
    func = make_new_mfun( "int", "eof", fileio_eof );
    func->doc = "Return whether end-of-file has been reached; the opposite of .more().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", fileio_more );
    func->doc = "Return whether there is more to read; the opposite of .eof().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(string)
    func = make_new_mfun( "void", "write", fileio_writestring );
    func->add_arg( "string", "val" );
    func->doc = "Write a string to file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(int)
    func = make_new_mfun( "void", "write", fileio_writeint );
    func->add_arg( "int", "val" );
    func->doc = "Write an integer to file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(int,flags)
    func = make_new_mfun( "void", "write", fileio_writeintflags );
    func->add_arg( "int", "val" );
    func->add_arg( "int", "flags" );
    func->doc = "Write integer value to file; binary mode: int size specified by 'flags' (IO.INT8, IO.INT16, IO.INT32).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(float)
    func = make_new_mfun( "void", "write", fileio_writefloat );
    func->add_arg( "float", "val" );
    func->doc = "Write floating point value to file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add write(float)
    func = make_new_mfun( "void", "write", fileio_writefloatflags );
    func->add_arg( "float", "val" );
    func->add_arg( "int", "flags" );
    func->doc = "Write floating point value to file; binary mode: flags indicate float size (IO.FLOAT32 or IO.FLOAT64).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add autoPrefixAndExt() | 1.5.1.5 (ge)
    func = make_new_mfun( "void", "autoPrefixExtension", file_ctrl_autoPrefixAndExtension ); //! set auto prefix and extension string
    func->add_arg( "string", "prefix" );
    func->add_arg( "string", "extension" );
    func->doc = "set auto prefix and extension for \"special:auto\" filename generation (applicable to file writing only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add autoPrefix()
    func = make_new_mfun( "string", "autoPrefix", file_cget_autoPrefix ); //! get auto prefix string
    func->doc = "get auto prefix for \"special:auto\" filename generation (applicable to file writing only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add autoExtension()
    func = make_new_mfun( "string", "autoExtension", file_cget_autoExtension ); //! get auto extension string
    func->doc = "get auto extension for \"special:auto\" filename generation (applicable to file writing only).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add filename()
    func = make_new_mfun( "string", "filename", file_cget_filename ); //! get auto extension string
    func->doc = "get current filename.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add expandPath | 1.5.1.3
    func = make_new_sfun( "string", "expandPath", fileio_expandpath_impl );
    func->add_arg( "string", "path" );
    func->doc = "expand platform-specific filepath to an absolute path, which is returned. On macOS and Linux expandPath() will attempt to resolve `~` or `~[username]`; on Windows expandPath() will attempt to resolve %USERNAME%. (Known issue: (macOS) expandPath currently introduced an audio click; it recommended to call expandPath() at the beginning; e.g., expanding path ahead of time could avoid a click instead of calling Machine.add() on a filepath with `~`.)";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "io/chout.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-float.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-int.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-line.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-str.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-tokens.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/seek.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/write.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/write2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/write-auto.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/read-byte.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "io/write-byte.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_chout()
// desc: added 1.3.0.0 as full class
//-----------------------------------------------------------------------------
t_CKBOOL init_class_chout( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_HERALD, "class 'chout'" );

    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, "Do not instantiate this class directly! Use 'chout' instead." ) )
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

    // add readInt(int)
    func = make_new_mfun( "int", "readInt", chout_readintflags );
    func->add_arg( "int", "flags" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", chout_readfloat );
    // func->add_arg( "int", "flags" );
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
    EM_log( CK_LOG_HERALD, "class 'cherr'" );

    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, "Do not instantiate this class directly! Use 'cherr' instead." ) )
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

    // add readInt(int)
    // func = make_new_mfun( "int", "readInt", cherr_readintflags );
    // func->add_arg( "int", "flags" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readFloat()
    // func = make_new_mfun( "float", "readFloat", cherr_readfloat );
    // func->add_arg( "int", "flags" );
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
    std::string doc;

    // doc string
    doc = "Creates a message for sending and receiving Midi information.";
    
    // init base class
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, "MidiMsg", "Object",
                                         env->global(), NULL, NULL, doc.c_str() ) )
        return FALSE;

    // add member variable
    doc = "First byte of a Midi message, usually a status byte or command byte.";
    MidiMsg_offset_data1 = type_engine_import_mvar( env, "int", "data1", FALSE, doc.c_str() );
    if( MidiMsg_offset_data1 == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Second byte of a Midi message, usually a note value.";
    MidiMsg_offset_data2 = type_engine_import_mvar( env, "int", "data2", FALSE, doc.c_str() );
    if( MidiMsg_offset_data2 == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Third byte of a Midi message, usually a velocity value.";
    MidiMsg_offset_data3 = type_engine_import_mvar( env, "int", "data3", FALSE, doc.c_str() );
    if( MidiMsg_offset_data3 == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Time when the MidiMsg occurred, relative to the start of the file (only valid for MidiFileIn).";
    MidiMsg_offset_when = type_engine_import_mvar( env, "dur", "when", FALSE, doc.c_str() );
    if( MidiMsg_offset_when == CK_INVALID_OFFSET ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "midi/gomidi.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/gomidi2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/polyfony.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/polyfony2.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

#ifndef __DISABLE_MIDI__
    // init base class
    
    // doc string
    doc = "Class that can be ChucKed to now as an event. When receiving a message, an event is signaled and Midi information can be read.";
    
    if( !type_engine_import_class_begin( env, "MidiIn", "Event",
                                         env->global(), MidiIn_ctor, MidiIn_dtor, doc.c_str() ) )
        return FALSE;

    // add open()
    func = make_new_mfun( "int", "open", MidiIn_open );
    func->add_arg( "int", "port" );
    func->doc = "Open Midi device using a port number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add open() (added 1.3.0.0)
    func = make_new_mfun( "int", "open", MidiIn_open_named );
    func->add_arg( "string", "name" );
    func->doc = "Open Midi device using the device's name.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add good()
    func = make_new_mfun( "int", "good", MidiIn_good );
    func->doc = "Return true (1) if a device has been opened for this instance and there was no error connecting to it. Return false (0) if a device has not been opened or there was an error opening a device.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add num()
    func = make_new_mfun( "int", "num", MidiIn_num );
    func->doc = "Return the device number of the device (i.e. the number passed to MidiIn/MidiOut.open).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add name()
    func = make_new_mfun( "string", "name", MidiIn_name );
    func->doc = "Return the Midi device's name as string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add printerr()
    func = make_new_mfun( "void", "printerr", MidiIn_printerr );
    func->add_arg( "int", "print_or_not" );
    func->doc = "Set error printing (1 for on, 0 for off). On by default.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add recv()
    func = make_new_mfun( "int", "recv", MidiIn_recv );
    func->add_arg( "MidiMsg", "msg" );
    func->doc = "Return into the MidiMsg argument the next message in the queue from the device. Return 0 if the queue is empty or 1 if a message was in the queue and returned in the argument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add can_wait()
    func = make_new_mfun( "int", "can_wait", MidiIn_can_wait );
    func->doc = "(internal) used by virtual machine for synthronization.";
    // func->doc = "Return 1 if the device has no more messages in its queue from the device, 0 if it has more messages.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add member variable
    MidiIn_offset_data = type_engine_import_mvar( env, "int", "@MidiIn_data", FALSE );
    if( MidiIn_offset_data == CK_INVALID_OFFSET ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "midi/gomidi.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/gomidi2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/polyfony.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "midi/polyfony2.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // doc string
    doc = "Class for sending out Midi messages.";
    // init base class
    if( !type_engine_import_class_begin( env, "MidiOut", "Object",
                                         env->global(), MidiOut_ctor, MidiOut_dtor, doc.c_str() ) )
        return FALSE;

    // add open()
    func = make_new_mfun( "int", "open", MidiOut_open );
    func->add_arg( "int", "port" );
    func->doc = "Open Midi device using a port number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add open() (added 1.3.0.0)
    func = make_new_mfun( "int", "open", MidiOut_open_named );
    func->add_arg( "string", "name" );
    func->doc = "Open Midi device using the device's name.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add good()
    func = make_new_mfun( "int", "good", MidiOut_good );
    func->doc = "Return true (1) if a device has been opened for this instance and there was no error connecting to it. Return false (0) if a device has not been opened or there was an error opening a device.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add num()
    func = make_new_mfun( "int", "num", MidiOut_num );
    func->doc = "Return the device number of the device (i.e. the number passed to MidiIn/MidiOut.open).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add name()
    func = make_new_mfun( "string", "name", MidiOut_name );
    func->doc = "Return the Midi device's name as string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add printerr()
    func = make_new_mfun( "void", "printerr", MidiOut_printerr );
    func->add_arg( "int", "print_or_not" );
    func->doc = "Set error printing (1 for on, 0 for off). On by default.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add send()
    func = make_new_mfun( "int", "send", MidiOut_send );
    func->add_arg( "MidiMsg", "msg" );
    func->doc = "Send out a MidiMsg message.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "midi/midiout.ck" ) ) goto error;

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
    std::string doc;

    // doc string
    doc = "Helper class for receiving HID information; e.g., which key was pressed, joystick position, etc.";

    // init base class
    if( !type_engine_import_class_begin( env, "HidMsg", "Object",
                                         env->global(), NULL, NULL, doc.c_str() ) )
        return FALSE;

    // add member variable
    doc = "Device type that produced the message.";
    HidMsg_offset_device_type = type_engine_import_mvar( env, "int", "deviceType", FALSE, doc.c_str() );
    if( HidMsg_offset_device_type == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Device number that produced the message.";
    HidMsg_offset_device_num = type_engine_import_mvar( env, "int", "deviceNum", FALSE, doc.c_str() );
    if( HidMsg_offset_device_num == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "A number representing the message type.";
    HidMsg_offset_type = type_engine_import_mvar( env, "int", "type", FALSE, doc.c_str() );
    if( HidMsg_offset_type == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Code (platform dependent) associated with a keyboard key.";
    HidMsg_offset_which = type_engine_import_mvar( env, "int", "which", FALSE, doc.c_str() );
    if( HidMsg_offset_which == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Integer data generated from the Hid device.";
    HidMsg_offset_idata = type_engine_import_mvar( env, "int", "idata", FALSE, doc.c_str() );
    if( HidMsg_offset_idata == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Float data generate from the Hid device.";
    HidMsg_offset_fdata = type_engine_import_mvar( env, "float", "fdata", FALSE, doc.c_str() );
    if( HidMsg_offset_fdata == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Time when the HidMsg occurred, relative to the start of the file.";
    HidMsg_offset_when = type_engine_import_mvar( env, "time", "when", FALSE, doc.c_str() );
    if( HidMsg_offset_when == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Change in X-axis of pointing device.";
    HidMsg_offset_deltax = type_engine_import_mvar( env, "int", "deltaX", FALSE, doc.c_str() );
    if( HidMsg_offset_deltax == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Change in Y-axis of pointing device.";
    HidMsg_offset_deltay = type_engine_import_mvar( env, "int", "deltaY", FALSE, doc.c_str() );
    if( HidMsg_offset_deltay == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Position of X-axis of pointing device.";
    HidMsg_offset_cursorx = type_engine_import_mvar( env, "int", "cursorX", FALSE, doc.c_str() );
    if( HidMsg_offset_cursorx == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Position of Y-axis of pointing device.";
    HidMsg_offset_cursory = type_engine_import_mvar( env, "int", "cursorY", FALSE, doc.c_str() );
    if( HidMsg_offset_cursory == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Position of X-axis of pointing device, scaled between 0.0 and 1.0.";
    HidMsg_offset_scaledcursorx = type_engine_import_mvar( env, "float", "scaledCursorX", FALSE, doc.c_str() );
    if( HidMsg_offset_scaledcursorx == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Position of Y-axis of pointing device, scaled between 0.0 and 1.0.";
    HidMsg_offset_scaledcursory = type_engine_import_mvar( env, "float", "scaledCursorY", FALSE, doc.c_str() );
    if( HidMsg_offset_scaledcursory == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Change in X-axis of pointing device (same as deltaX).";
    HidMsg_offset_x = type_engine_import_mvar( env, "int", "x", FALSE, doc.c_str() );
    if( HidMsg_offset_x == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Change in Y-axis of pointing device (same as deltaY).";
    HidMsg_offset_y = type_engine_import_mvar( env, "int", "y", FALSE, doc.c_str() );
    if( HidMsg_offset_y == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Change in Z-axis of pointing device.";
    HidMsg_offset_z = type_engine_import_mvar( env, "int", "z", FALSE, doc.c_str() );
    if( HidMsg_offset_z == CK_INVALID_OFFSET ) goto error;

    // add member variable (added 1.3.0.0)
    doc = "Position of X-axis of pointing device, scaled between 0.0 and 1.0.";
    HidMsg_offset_touchx = type_engine_import_mvar( env, "float", "touchX", FALSE, doc.c_str() );
    if( HidMsg_offset_touchx == CK_INVALID_OFFSET ) goto error;

    // add member variable (added 1.3.0.0)
    doc = "Position of X-axis of pointing device, scaled between 0.0 and 1.0.";
    HidMsg_offset_touchy = type_engine_import_mvar( env, "float", "touchY", FALSE, doc.c_str() );
    if( HidMsg_offset_touchy == CK_INVALID_OFFSET ) goto error;

    // add member variable (added 1.3.0.0)
    doc = "Multi-touch size.";
    HidMsg_offset_touchsize = type_engine_import_mvar( env, "float", "touchSize", FALSE, doc.c_str() );
    if( HidMsg_offset_touchsize == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Joystick axis position (int).";
    HidMsg_offset_axis_position = type_engine_import_mvar( env, "int", "axis_position", FALSE, doc.c_str() );
    if( HidMsg_offset_axis_position == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Joystick axis position (float).";
    HidMsg_offset_axis_position2 = type_engine_import_mvar( env, "float", "axisPosition", FALSE, doc.c_str() );
    if( HidMsg_offset_axis_position2 == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Joystick hat position.";
    HidMsg_offset_hat_position = type_engine_import_mvar( env, "int", "hatPosition", FALSE, doc.c_str() );
    if( HidMsg_offset_hat_position == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "ASCII value associated with a keyboard key.";
    HidMsg_offset_ascii = type_engine_import_mvar( env, "int", "ascii", FALSE, doc.c_str() );
    if( HidMsg_offset_ascii == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Code (USB) for a keyboard key.";
    HidMsg_offset_key = type_engine_import_mvar( env, "int", "key", FALSE, doc.c_str() );
    if( HidMsg_offset_key == CK_INVALID_OFFSET ) goto error;

    // add member variable
    doc = "Position of the primary pointing device, scaled between 0.0 and 1.0.";
    HidMsg_offset_scaled_axis_position = type_engine_import_mvar( env, "float", "scaled_axis_position", FALSE, doc.c_str() );
    if( HidMsg_offset_scaled_axis_position == CK_INVALID_OFFSET ) goto error;

    // add is_axis_motion()
    func = make_new_mfun( "int", "is_axis_motion", HidMsg_is_axis_motion ); // deprecated
    func->doc = "(Deprecated; use .isAxisMotion() instead).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isAxisMotion()
    func = make_new_mfun( "int", "isAxisMotion", HidMsg_is_axis_motion );
    func->doc = "Return whether this message is an axis-motion event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add is_button_down()
    func = make_new_mfun( "int", "is_button_down", HidMsg_is_button_down ); // deprecated
    func->doc = "(Deprecated; use .isButtonDown() instead).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isButtonDown()
    func = make_new_mfun( "int", "isButtonDown", HidMsg_is_button_down );
    func->doc = "Return whether this message is a button-down event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add is_button_up()
    func = make_new_mfun( "int", "is_button_up", HidMsg_is_button_up ); // deprecated
    func->doc = "(Deprecated; use .isButtonUp() instead).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isButtonUp()
    func = make_new_mfun( "int", "isButtonUp", HidMsg_is_button_up );
    func->doc = "Return whether this message is a button-up event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add is_mouse_motion()
    func = make_new_mfun( "int", "is_mouse_motion", HidMsg_is_mouse_motion ); // deprecated
    func->doc = "(Deprecated; use .isMouseMotion() instead).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isMouseMotion()
    func = make_new_mfun( "int", "isMouseMotion", HidMsg_is_mouse_motion );
    func->doc = "Return whether this message is a mouse-motion event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add is_hat_motion()
    func = make_new_mfun( "int", "is_hat_motion", HidMsg_is_hat_motion ); // deprecated
    func->doc = "(Deprecated; use .isHatMotion() instead).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isHatMotion()
    func = make_new_mfun( "int", "isHatMotion", HidMsg_is_hat_motion );
    func->doc = "Return whether this message is a hat-motion event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isWheelMotion()
    func = make_new_mfun( "int", "isWheelMotion", HidMsg_is_wheel_motion );
    func->doc = "Return whether this message is a wheel-motion event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "hid/kb.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/keyboard-organ.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse-abs.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse-fm.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-fm.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-noise.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-shake.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    // register deprecate
    type_engine_register_deprecate( env, "HidIn", "Hid" );

    // init base class Hid (copy of HidIn + constants)
    if( !type_engine_import_class_begin( env, "Hid", "Event",
                                         env->global(), HidIn_ctor, HidIn_dtor, "Class for interacting with human-interface devices (HIDs) such as keyboards, mice, gamepads, joysticks, etc." ) )
        return FALSE;

    // add examples
    if( !type_engine_import_add_ex( env, "hid/kb.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/keyboard-organ.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse-abs.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/mouse-fm.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-fm.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-noise.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "hid/joy-shake.ck" ) ) goto error;

    // add open()
    func = make_new_mfun( "int", "open", HidIn_open );
    func->add_arg( "int", "type" );
    func->add_arg( "int", "num" );
    func->doc = "Open a HID device by device number ('num') and type ('type'). See static member variables for possible types.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add open()
    func = make_new_mfun( "int", "open", HidIn_open_named );
    func->add_arg( "string", "name" );
    func->doc = "Open a HID device by name.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openJoystick()
    func = make_new_mfun( "int", "openJoystick", HidIn_open_joystick );
    func->add_arg( "int", "num" );
    func->doc = "Open a joystick/gamepad by device number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openJoystick()
    func = make_new_mfun( "int", "openJoystick", HidIn_open_joystick_2 );
    func->add_arg( "int", "num" );
    func->add_arg( "int", "suppressErrMsg" );
    func->doc = "Open a joystick/gamepad by device number, with option (true/false) to suppress error messages.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openMouse()
    func = make_new_mfun( "int", "openMouse", HidIn_open_mouse );
    func->add_arg( "int", "num" );
    func->doc = "Open a mouse/trackpad by device number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openMouse()
    func = make_new_mfun( "int", "openMouse", HidIn_open_mouse_2 );
    func->add_arg( "int", "num" );
    func->add_arg( "int", "suppressErrMsg" );
    func->doc = "Open a mouse/trackpad by device number, with option (true/false) to suppress error messages.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openKeyboard()
    func = make_new_mfun( "int", "openKeyboard", HidIn_open_keyboard );
    func->add_arg( "int", "num" );
    func->doc = "Open a keyboard by device number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openKeyboard() | 1.5.1.5 (ge & andrew)
    func = make_new_mfun( "int", "openKeyboard", HidIn_open_keyboard_2 );
    func->add_arg( "int", "num" );
    func->add_arg( "int", "suppressErrMsg" );
    func->doc = "Open a keyboard by device number, with option (true/false) to suppress error messages.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add openTiltSensor()
    func = make_new_mfun( "int", "openTiltSensor", HidIn_open_tiltsensor );
    func->doc = "Open a tilt-sensor by device number.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add good()
    func = make_new_mfun( "int", "good", HidIn_good );
    func->doc = "Get whether a device has been successfully opened on this HID instance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add num()
    func = make_new_mfun( "int", "num", HidIn_num );
    func->doc = "Get the number of the currently open device; returns -1 if no device is open on this HID instance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add name()
    func = make_new_mfun( "string", "name", HidIn_name );
    func->doc = "Get the name of the currently open device; return empty string (\"\") if no device is open on this HID instance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add printerr()
    func = make_new_mfun( "void", "printerr", HidIn_printerr );
    func->add_arg( "int", "toPrintOrNot" );
    func->doc = "Set whether to print errors (default is YES).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add recv()
    func = make_new_mfun( "int", "recv", HidIn_recv );
    func->add_arg( "HidMsg", "msg" );
    func->doc = "Receive the next available HidMsg.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add read()
    func = make_new_mfun( "int", "read", HidIn_read );
    func->add_arg( "int", "type" );
    func->add_arg( "int", "which" );
    func->add_arg( "HidMsg", "msg" );
    func->doc = "Read the next HidMsg from device of type 'type' with device id 'which'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add send()
    func = make_new_mfun( "int", "send", HidIn_send );
    func->add_arg( "HidMsg", "msg" );
    func->doc = "Send a HidMsg to device; return whether the operation was successful.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add can_wait()
    func = make_new_mfun( "int", "can_wait", HidIn_can_wait );
    func->doc = "(internal) used by virtual machine for synthronization.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add readTiltSensor()
    func = make_new_sfun( "int[]", "readTiltSensor", HidIn_read_tilt_sensor );
    func->doc = "Read tilt-sensor and return as an int array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add globalTiltPollRate()
    func = make_new_sfun( "dur", "globalTiltPollRate", HidIn_ctrl_tiltPollRate );
    func->add_arg( "dur", "d" );
    func->doc = "Set tilt-sensor poll rate.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add globalTiltPollRate()
    func = make_new_sfun( "dur", "globalTiltPollRate", HidIn_cget_tiltPollRate );
    func->doc = "Get tilt-sensor poll rate.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add startCursorTrack()
    func = make_new_sfun( "int", "startCursorTrack", HidIn_start_cursor_track );
    func->doc = "Start cursor tracking; return whether the request was successful.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add stopCursorTrack()
    func = make_new_sfun( "int", "stopCursorTrack", HidIn_stop_cursor_track );
    func->doc = "Stop cursor tracking; return whether the request was successful.";
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
CK_DLL_MFUN( io_dummy_int )
{
    // EM_error3( "(IO): internal error! inside an abstract function! help!" );
    // EM_error3( "    note: please hunt down someone (e.g., Ge) to fix this..." );
    // assert( FALSE );
    RETURN->v_int = 0;
}

CK_DLL_MFUN( io_dummy_float )
{
    // return 0.0
    RETURN->v_float = 0;
}

CK_DLL_MFUN( io_dummy_string )
{
    // return NULL
    RETURN->v_string = NULL;
}

CK_DLL_MFUN( io_dummy_void )
{
    // do nothing
}

CK_DLL_SFUN( io_newline )
{
    RETURN->v_string = g_newline;
}


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
    Chuck_IO::FLAG_READ_WRITE | Chuck_IO::TYPE_ASCII;

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
    Chuck_ArrayInt * a = f->dirList();
    RETURN->v_object = a;
}

CK_DLL_MFUN( file_ctrl_autoPrefixAndExtension )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    Chuck_String * prefix = GET_NEXT_STRING(ARGS);
    Chuck_String * extension = GET_NEXT_STRING(ARGS);

    f->m_autoPrefix = prefix ? prefix->str() : "";
    f->m_autoExtension = extension ? extension->str() : "";
}

CK_DLL_MFUN( file_cget_autoPrefix )
{
    // get this
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    // create string, no ref count here, as we are not holding on to it
    RETURN->v_object = ck_create_string( VM, f->m_autoPrefix.c_str(), FALSE );
}

CK_DLL_MFUN( file_cget_autoExtension )
{
    // get this
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    // create string, no ref count here, as we are not holding on to it
    RETURN->v_object = ck_create_string( VM, f->m_autoExtension.c_str(), FALSE );
}

CK_DLL_MFUN( file_cget_filename )
{
    // get this
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    // create string, no ref count here, as we are not holding on to it
    RETURN->v_object = ck_create_string( VM, f->filename().c_str(), FALSE );
}

// expandPath | 1.5.1.3
CK_DLL_SFUN( fileio_expandpath_impl )
{
    Chuck_String * ckPath = GET_CK_STRING(ARGS);
    // get arg
    string path = ckPath != NULL ? ckPath->str() : "";
    // expand; don't ensure path
    string expanded = expand_filepath( path, FALSE );
    // instantiate and initalize object
    Chuck_String * string = (Chuck_String *)instantiate_and_initialize_object( VM->env()->ckt_string, VM );
    // set the value
    string->set( expanded );
    // return
    RETURN->v_string = string;
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
        if (!ret)
        {
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
    RETURN->v_int = f->readInt( flags );
}

CK_DLL_MFUN( fileio_readint8 )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->readInt( Chuck_IO::INT8 );
}

CK_DLL_MFUN( fileio_readint16 )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->readInt( Chuck_IO::INT16 );
}

CK_DLL_MFUN( fileio_readint32 )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_int = f->readInt( Chuck_IO::INT32 );
}

CK_DLL_MFUN( fileio_readfloat )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    RETURN->v_float = f->readFloat();
}

CK_DLL_MFUN( fileio_readfloatflags )
{
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;
    t_CKINT flags = GET_NEXT_INT(ARGS);
    RETURN->v_float = f->readFloat( flags );
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

#ifndef __DISABLE_ASYNCH_IO__ // 1.5.0.0 (ge) | made more granular (e.g., for WebChucK)
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
#else
    f->write(val);
#endif // __DISABLE_ASYNCH_IO__
}

CK_DLL_MFUN( fileio_writeint )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;

#ifndef __DISABLE_ASYNCH_IO__ // 1.5.0.0 (ge) | made more granular (e.g., for WebChucK)
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
#else
    f->write(val);
#endif // __DISABLE_ASYNCH_IO__
}

CK_DLL_MFUN( fileio_writeintflags )
{
    t_CKINT val = GET_NEXT_INT(ARGS);
    t_CKINT flags = GET_NEXT_INT(ARGS);
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;

#ifndef __DISABLE_ASYNCH_IO__  // 1.5.0.0 (ge) | made more granular (e.g., for WebChucK)
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
#else
    f->write(val, flags);
#endif // __DISABLE_ASYNCH_IO__

}

CK_DLL_MFUN( fileio_writefloat )
{
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    t_CKINT flagsDefault = Chuck_IO::FLOAT32;
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;

#ifndef __DISABLE_ASYNCH_IO__  // 1.5.0.0 (ge) | made more granular (e.g., for WebChucK)
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->floatArg = val;
        args->intArg = flagsDefault; // 1.5.0.1 (ge) add default
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
        f->write( val, flagsDefault );
    }
#else
    f->write( val, flagsDefault );
#endif //__DISABLE_ASYNCH_IO__
}

CK_DLL_MFUN( fileio_writefloatflags )
{
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    t_CKINT flags = GET_NEXT_INT(ARGS);
    Chuck_IO_File * f = (Chuck_IO_File *)SELF;

#ifndef __DISABLE_ASYNCH_IO__  // 1.5.0.0 (ge) | made more granular (e.g., for WebChucK)
    if (f->mode() == Chuck_IO::MODE_ASYNC)
    {
        // set up arguments
        Chuck_IO::async_args *args = new Chuck_IO::async_args;
        args->RETURN = (void *)RETURN;
        args->fileio_obj = f;
        args->floatArg = val;
        args->intArg = flags; // 1.5.0.1 (ge) added
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
        f->write( val,flags );
    }
#else
    f->write( val,flags );
#endif //__DISABLE_ASYNCH_IO__
}




//-----------------------------------------------------------------------------
// Chout API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( chout_close )
{
    // problem
    // CK_FPRINTF_STDERR( "[chuck]: cannot close 'chout'...\n" );
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

    // 1.5.0.0 | (ge) added a single newline -> trigger chout flush
    if( val == "\n" ) c->flush();
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
    // CK_FPRINTF_STDERR( "[chuck]: cannot close 'cherr'...\n" );
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
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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
    // set CK_HID_DEV_COUNT as a special flag
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_COUNT, s );
}

CK_DLL_MFUN( HidIn_open_joystick )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_JOYSTICK, num );
}

CK_DLL_MFUN( HidIn_open_joystick_2 )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    t_CKINT suppressErrMsg = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_JOYSTICK, num, suppressErrMsg );
}

CK_DLL_MFUN( HidIn_open_mouse )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_MOUSE, num );
}

CK_DLL_MFUN( HidIn_open_mouse_2 )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    t_CKINT suppressErrMsg = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_MOUSE, num, suppressErrMsg );
}

CK_DLL_MFUN( HidIn_open_keyboard )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_KEYBOARD, num );
}

CK_DLL_MFUN( HidIn_open_keyboard_2 )
{
    HidIn * min = (HidIn *)OBJ_MEMBER_INT(SELF, HidIn_offset_data);
    t_CKINT num = GET_NEXT_INT(ARGS);
    t_CKINT suppressErrMsg = GET_NEXT_INT(ARGS);
    RETURN->v_int = min->open( SHRED->vm_ref, CK_HID_DEV_KEYBOARD, num, suppressErrMsg );
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
    // Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // only if valid
    // if( min->good() )
    //     a->str = min->phin->getPortName( min->num() );
    // TODO: is null problem?
    RETURN->v_string = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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

    Chuck_ArrayInt * array = new Chuck_ArrayInt( FALSE, 3 );
    initialize_object( array, VM->env()->ckt_array, SHRED, VM ); // 1.5.0.0 (ge) added
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
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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




// constants
const t_CKINT Chuck_IO::TYPE_ASCII = 0x1;
const t_CKINT Chuck_IO::TYPE_BINARY = 0x2;
const t_CKINT Chuck_IO::FLOAT32 = 0x10;
const t_CKINT Chuck_IO::FLOAT64 = 0x20;
const t_CKINT Chuck_IO::INT8 = 0x100;
const t_CKINT Chuck_IO::INT16 = 0x200;
const t_CKINT Chuck_IO::INT24 = 0x400;
const t_CKINT Chuck_IO::INT32 = 0x800;
const t_CKINT Chuck_IO::INT64 = 0x1000;
const t_CKINT Chuck_IO::SINT8 = 0x2000;
const t_CKINT Chuck_IO::SINT16 = 0x4000;
const t_CKINT Chuck_IO::SINT24 = 0x8000;
const t_CKINT Chuck_IO::SINT32 = 0x10000;
const t_CKINT Chuck_IO::SINT64 = 0x20000;
const t_CKINT Chuck_IO::UINT8 = 0x40000;
const t_CKINT Chuck_IO::UINT16 = 0x80000;
const t_CKINT Chuck_IO::UINT24 = 0x100000;
const t_CKINT Chuck_IO::UINT32 = 0x200000;
const t_CKINT Chuck_IO::UINT64 = 0x400000;
const t_CKINT Chuck_IO::FLAG_READONLY = 0x100;
const t_CKINT Chuck_IO::FLAG_WRITEONLY = 0x200;
const t_CKINT Chuck_IO::FLAG_READ_WRITE = 0x400;
const t_CKINT Chuck_IO::FLAG_APPEND = 0x800;

#ifndef __DISABLE_THREADS__
const t_CKINT Chuck_IO::MODE_SYNC = 0;
const t_CKINT Chuck_IO::MODE_ASYNC = 1;
#else
const t_CKINT Chuck_IO::MODE_SYNC = 1;
const t_CKINT Chuck_IO::MODE_ASYNC = 0;
#endif




//-----------------------------------------------------------------------------
// name: Chuck_IO Constructor
// desc: Empty because you cannot construct a Chuck_IO object
//-----------------------------------------------------------------------------
Chuck_IO::Chuck_IO() : m_asyncEvent( NULL )
{
#ifndef __DISABLE_THREADS__
    m_thread = NULL;
#endif
}




//-----------------------------------------------------------------------------
// name: Chuck_IO Destructor
// desc: Empty becuase you cannot destruct a Chuck_IO object
//-----------------------------------------------------------------------------
Chuck_IO::~Chuck_IO()
{ }




//-----------------------------------------------------------------------------
// name: Chuck_IO_File()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_IO_File::Chuck_IO_File( Chuck_VM * vm )
{
    m_vmRef = vm;
    // zero things out
    m_flags = 0;
    m_iomode = MODE_SYNC;
    m_path = "";
    m_dir = NULL;
    m_dir_start = 0;
    m_asyncEvent = new Chuck_Event;
    initialize_object( m_asyncEvent, vm->env()->ckt_event, NULL, vm );
#ifndef __DISABLE_THREADS__
    m_thread = new XThread;
#endif

    // initialize prefix and extensions for auto | 1.5.1.5
    m_autoPrefix = "chuck-file";
    m_autoExtension = "txt";
}




//-----------------------------------------------------------------------------
// name: ~Chuck_IO_File()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_IO_File::~Chuck_IO_File()
{
    // clean up
    this->close();
    delete m_asyncEvent;
#ifndef __DISABLE_THREADS__
    delete m_thread;
#endif
}




//-----------------------------------------------------------------------------
// name: open
// desc: open file from disk
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_IO_File::open( const string & path, t_CKINT flags )
{
    // the filename
    string theFilename = path;

    // log
    EM_log( CK_LOG_INFO, "FileIO: opening file from disk..." );
    EM_log( CK_LOG_INFO, "FileIO: path: %s", theFilename.c_str() );
    EM_pushlog();

    // if no flag specified, make it READ by default
    if( !(flags & FLAG_READ_WRITE) &&
        !(flags & FLAG_READONLY) &&
        !(flags & FLAG_WRITEONLY) &&
        !(flags & FLAG_APPEND) )
    {
        flags |= FLAG_READONLY;
    }

    // if both read and write, enable read and write
    if( flags & FLAG_READONLY && flags & FLAG_WRITEONLY )
    {
        flags ^= FLAG_READONLY;
        flags ^= FLAG_WRITEONLY;
        flags |= FLAG_READ_WRITE;
    }

    // set open flags
    ios_base::openmode theMode;

    // check flags for errors
    if( (flags & TYPE_ASCII) &&
        (flags & TYPE_BINARY) )
    {
        EM_error3( "[chuck](via FileIO): cannot open file in both ASCII and binary mode" );
        goto error;
    }

    if( (flags & FLAG_READ_WRITE) &&
        (flags & FLAG_READONLY) )
    {
        EM_error3( "[chuck](via FileIO): conflicting flags: READ_WRITE and READ" );
        goto error;
    }

    if( (flags & FLAG_READ_WRITE) &&
        (flags & FLAG_WRITEONLY) )
    {
        EM_error3( "[chuck](via FileIO): conflicting flags: READ_WRITE and WRITE" );
        goto error;
    }

    if( (flags & FLAG_READ_WRITE) &&
        (flags & FLAG_APPEND) )
    {
        EM_error3( "[chuck](via FileIO): conflicting flags: READ_WRITE and APPEND" );
        goto error;
    }

    if( (flags & FLAG_WRITEONLY) &&
        (flags & FLAG_READONLY) )
    {
        EM_error3( "[chuck](via FileIO): conflicting flags: WRITE and READ" );
        goto error;
    }

    if( (flags & FLAG_APPEND) &&
        (flags & FLAG_READONLY) )
    {
        EM_error3( "[chuck](via FileIO): conflicting flags: APPEND and FLAG_READ" );
        goto error;
    }

    if( flags & FLAG_READ_WRITE )
        theMode = ios_base::in | ios_base::out;
    else if( flags & FLAG_READONLY )
        theMode = ios_base::in;
    else if( flags & FLAG_APPEND )
        theMode = ios_base::out | ios_base::app;
    else if( flags & FLAG_WRITEONLY )
        theMode = ios_base::out | ios_base::trunc;

    if( flags & TYPE_BINARY )
        theMode |= ios_base::binary;

    // close first
    if( m_io.is_open() )
        this->close();

    // special
    if( strstr( theFilename.c_str(), "special:auto" ) )
    {
        // check output
        if( theMode & ios_base::out )
        {
            // generate auto name
            theFilename = autoFilename( this->m_autoPrefix, this->m_autoExtension );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): \"special:auto\" can only be used for output" );
            goto error;
        }
    }

    // try to open as a dir first (fixed 1.3.0.0 removed warning)
    m_dir = opendir( theFilename.c_str() );
    if( m_dir )
    {
        EM_poplog();
        return TRUE;
    }

    // not a dir, create file if it does not exist unless flag is
    // readonly
    if( !(flags & FLAG_READONLY) )
    {
        m_io.open( theFilename.c_str(), ios_base::in );
        if( m_io.fail() )
        {
            m_io.clear();
            m_io.open( theFilename.c_str(), ios_base::out | ios_base::trunc );
            m_io.close();
        }
        else
            m_io.close();
    }

    //open file
    m_io.open( theFilename.c_str(), theMode );

    // seek to beginning if necessary
    if( flags & FLAG_READ_WRITE )
    {
        m_io.seekp( 0 );
        m_io.seekg( 0 );
    }

    /* ATODO: Ge's code
     // windows sucks for being creative in the wrong places
     #ifdef __PLATFORM_WINDOWS__
     // if( flags ^ Chuck_IO::TRUNCATE && flags | Chuck_IO::READ ) nMode |= ios::nocreate;
     m_io.open( theFilename.c_str(), nMode );
     #else
     m_io.open( theFilename.c_str(), (_Ios_Openmode)nMode );
     #endif
     */

     // check for error
    if( !(m_io.is_open()) )
    {
        // EM_error3( "[chuck](via FileIO): cannot open file: '%s'", theFilename.c_str() );
        goto error;
    }

    // set path
    m_path = theFilename;
    // set flags
    m_flags = flags;
    if( !(flags & TYPE_BINARY) )
        m_flags |= Chuck_IO_File::TYPE_ASCII; // ASCII is default
    // set mode
    m_iomode = MODE_SYNC;

    // pop
    EM_poplog();

    return TRUE;

error:

    // pop
    EM_poplog();

    // reset
    m_path = "";
    m_flags = 0;
    m_iomode = MODE_SYNC;
    m_io.clear();
    m_io.close();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: close
// desc: close file
//-----------------------------------------------------------------------------
void Chuck_IO_File::close()
{
    // log
    EM_log( CK_LOG_INFO, "FileIO: closing file '%s'...", m_path.c_str() );
    // close it
    m_io.close();
    m_flags = 0;
    m_path = "";
    m_iomode = Chuck_IO::MODE_SYNC;
    if( m_dir ) {
        closedir( m_dir );
        m_dir = NULL;
        m_dir_start = 0;
    }
}




//-----------------------------------------------------------------------------
// name: good()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_IO_File::good()
{
    return m_dir || m_io.is_open();
    // return (!m_dir) && (!m_io.is_open());
}




//-----------------------------------------------------------------------------
// name: flush()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_IO_File::flush()
{
    // sanity
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot flush on directory" );
        return;
    }
    m_io.flush();
}




//-----------------------------------------------------------------------------
// name: mode()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_IO_File::mode()
{
    // sanity
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot get mode on directory" );
        return -1;
    }
    return m_iomode;
}




//-----------------------------------------------------------------------------
// name: mode( t_CKINT flag )
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_IO_File::mode( t_CKINT flag )
{
    // sanity
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot set mode on directory" );
        return;
    }
    if( (flag != Chuck_IO::MODE_ASYNC) && (flag != Chuck_IO::MODE_SYNC) )
    {
        EM_error3( "[chuck](via FileIO): invalid mode flag" );
        return;
    }

    m_iomode = flag;
}




//-----------------------------------------------------------------------------
// name: size()
// desc: Returns the size of the file in bytes, or -1 if no file is opened or
//       if a directory is opened.
//-----------------------------------------------------------------------------
t_CKINT Chuck_IO_File::size()
{
    if( !(m_io.is_open()) ) return -1;
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot get size on a directory" );
        return -1;
    }

    // no easy way to find file size in C++
    // have to seek to end, report position
    FILE * stream = fopen( m_path.c_str(), "r" );
    fseek( stream, 0L, SEEK_END );
    long endPos = ftell( stream );
    fclose( stream );
    return endPos;
}




//-----------------------------------------------------------------------------
// name: seek( t_CKINT pos )
// desc: Seeks to the specified byte offset in the file.
//-----------------------------------------------------------------------------
void Chuck_IO_File::seek( t_CKINT pos )
{
    if( !(m_io.is_open()) )
    {
        EM_error3( "[chuck](via FileIO): cannot seek: no file is open" );
        return;
    }
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot seek on a directory" );
        return;
    }
    // 1.5.0.0 (ge) | added since seekg fails if EOF already reached
    m_io.clear();
    m_io.seekg( pos );
    m_io.seekp( pos );
}




//-----------------------------------------------------------------------------
// name: tell()
// desc: Returns the byte offset into the file, or -1 if no file is opened.
//-----------------------------------------------------------------------------
t_CKINT Chuck_IO_File::tell()
{
    if( !(m_io.is_open()) )
        return -1;
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot tell on directory" );
        return -1;
    }

    return m_io.tellg();
}




//-----------------------------------------------------------------------------
// name: isDir()
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_IO_File::isDir()
{
    return m_dir != NULL;
}




//-----------------------------------------------------------------------------
// name: dirList()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_ArrayInt * Chuck_IO_File::dirList()
{
    // sanity
    if( !m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot get list: no directory open" );
        Chuck_ArrayInt * ret = new Chuck_ArrayInt( TRUE, 0 );
        initialize_object( ret, m_vmRef->env()->ckt_array, NULL, m_vmRef );
        return ret;
    }

    // fill vector with entry names
    rewinddir( m_dir );
    std::vector<Chuck_String *> entrylist;

    // 1.5.0.0 (ge) | #chunreal UE-forced refactor
    struct dirent * ent = readdir( m_dir );
    // was: while( (ent = readdir( m_dir ) ) <-- in ge's opinion this is cleaner splitting readir() into two places
    while( ent != NULL ) // fixed 1.3.0.0: removed warning
    {
        // pass NULL as shred ref
        Chuck_String * s = (Chuck_String *)instantiate_and_initialize_object( m_vmRef->env()->ckt_string, NULL, m_vmRef );
        s->set( std::string( ent->d_name ) );
        if( s->str() != ".." && s->str() != "." )
        {
            // don't include .. and . in the list
            entrylist.push_back( s );
        }
        // #chunreal refactor
        ent = readdir( m_dir );
    }

    // make array
    Chuck_ArrayInt * array = new Chuck_ArrayInt( true, entrylist.size() );
    initialize_object( array, m_vmRef->env()->ckt_array, NULL, m_vmRef );
    for( int i = 0; i < entrylist.size(); i++ )
        array->set( i, (t_CKUINT)entrylist[i] );
    return array;
}




//-----------------------------------------------------------------------------
// name: read( t_CKINT length )
// desc: ...
//-----------------------------------------------------------------------------
/*Chuck_String * Chuck_IO_File::read( t_CKINT length )
{
    // sanity
    if (!(m_io.is_open())) {
        EM_error3( "[chuck](via FileIO): cannot read: no file open" );
        return new Chuck_String( "" );
    }

    if (m_io.fail()) {
        EM_error3( "[chuck](via FileIO): cannot read: I/O stream failed" );
        return new Chuck_String( "" );
    }

    if ( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot read on a directory" );
        return new Chuck_String( "" );
    }

    char buf[length+1];
    m_io.read( buf, length );
    buf[m_io.gcount()] = '\0';
    string s( buf );
    return new Chuck_String( s );
}*/




//-----------------------------------------------------------------------------
// name: readLine()
// desc: read line
//-----------------------------------------------------------------------------
Chuck_String * Chuck_IO_File::readLine()
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot readLine: no file open" );
        return new Chuck_String( "" );
    }

    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot readLine: I/O stream failed" );
        return new Chuck_String( "" );
    }

    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot readLine on directory" );
        return new Chuck_String( "" );
    }

    string s;
    getline( m_io, s );

    // chuck str
    Chuck_String * str = new Chuck_String( s );
    // initialize | 1.5.0.0 (ge) | added initialize_object
    initialize_object( str, m_vmRef->env()->ckt_string, NULL, m_vmRef );

    // note this chuck string still needs to be initialized
    return str;
}




//-----------------------------------------------------------------------------
// name: readInt( t_CKINT flags )
// desc: ...
//-----------------------------------------------------------------------------
t_CKINT Chuck_IO_File::readInt( t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot readInt: no file open" );
        return 0;
    }

    if( m_io.eof() ) {
        EM_error3( "[chuck](via FileIO): cannot readInt: EOF reached" );
        return 0;
    }

    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot read on directory" );
        return 0;
    }

    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
        return 0;
    }

    // check mode ASCII vs. binary
    if( m_flags & TYPE_ASCII )
    {
        // ASCII
        t_CKINT val = 0;
        m_io >> val;
        // if (m_io.fail())
        //     EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
        return val;
    }
    else if( m_flags & TYPE_BINARY )
    {
        // binary
        if( flags & Chuck_IO::INT8 || flags & Chuck_IO::UINT8 ) // 1.5.0.1 (ge) added UINT
        {
            // unsigned 8-bit
            uint8_t i; // 1.5.0.1 (ge) changed to int8_t from unsigned char
            m_io.read( (char *)&i, 1 );
            if( m_io.gcount() != 1 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::INT16 || flags & Chuck_IO::UINT16 ) // 1.5.0.1 (ge) added UINT
        {
            // unsigned 16-bit
            uint16_t i; // 1.5.0.1 (ge) changed to uint16_t from t_CKINT
            m_io.read( (char *)&i, 2 );
            if( m_io.gcount() != 2 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::INT32 || flags & Chuck_IO::UINT32 ) // 1.5.0.1 (ge) added UINT
        {
            // unsigned 32-bit
            uint32_t i; // 1.5.0.1 (ge) changed to uint32_t from t_CKINT
            m_io.read( (char *)&i, 4 );
            if( m_io.gcount() != 4 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::INT64 || flags & Chuck_IO::UINT64 ) // 1.5.0.1 (ge) added 64-bit
        {
            // unsigned 64-bit
            uint64_t i; // 1.5.0.1 (ge) added
            m_io.read( (char *)&i, 8 );
            if( m_io.gcount() != 8 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i; // TODO: handle signed vs unsigned
        }
        else if( flags & Chuck_IO::SINT8 ) // 1.5.0.1 (ge) added SINT
        {
            // signed 8-bit
            int8_t i;
            m_io.read( (char *)&i, 1 );
            if( m_io.gcount() != 1 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::SINT16 ) // 1.5.0.1 (ge) added SINT
        {
            // signed 16-bit
            int16_t i;
            m_io.read( (char *)&i, 2 );
            if( m_io.gcount() != 2 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::SINT32 ) // 1.5.0.1 (ge) added SINT
        {
            // signed 32-bit
            int32_t i;
            m_io.read( (char *)&i, 4 );
            if( m_io.gcount() != 4 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else if( flags & Chuck_IO::SINT64 ) // 1.5.0.1 (ge) added SINT
        {
            // signed 64-bit
            int64_t i;
            m_io.read( (char *)&i, 4 );
            if( m_io.gcount() != 8 )
                EM_error3( "[chuck](via FileIO): cannot readInt: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readInt: I/O stream failed" );
            return (t_CKINT)i;
        }
        else
        {
            EM_error3( "[chuck](via FileIO): readInt error: invalid/unsupported int size flag" );
            return 0;
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): readInt error: invalid ASCII/binary flag" );
        return 0;
    }
}




//-----------------------------------------------------------------------------
// name: readFloat()
// desc: read next as floating point value; could be ASCII or BINARY
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_IO_File::readFloat()
{
    return this->readFloat( Chuck_IO::FLOAT32 );
}




//-----------------------------------------------------------------------------
// name: readFloat()
// desc: read next as floating point value; could be ASCII or BINARY
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_IO_File::readFloat( t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot readFloat: no file open" );
        return 0;
    }
    if( m_io.eof() ) {
        EM_error3( "[chuck](via FileIO): cannot readFloat: EOF reached" );
        return 0;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot readFloat: I/O stream failed" );
        return 0;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot read a directory" );
        return 0;
    }

    // check mode
    if( m_flags & TYPE_ASCII )
    {
        // ASCII
        t_CKFLOAT val = 0;
        m_io >> val;
        // if (m_io.fail())
        //     EM_error3( "[chuck](via FileIO): cannot readFloat: I/O stream failed" );
        return val;

    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1
        if( flags & Chuck_IO::FLOAT32 )
        {
            float i;
            m_io.read( (char *)&i, sizeof( float ) );
            if( m_io.gcount() != sizeof( float ) )
                EM_error3( "[chuck](via FileIO): cannot readFloat: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readFloat: I/O stream failed" );
            return (t_CKFLOAT)i;
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            double i;
            m_io.read( (char *)&i, sizeof( double ) );
            if( m_io.gcount() != sizeof( double ) )
                EM_error3( "[chuck](via FileIO): cannot readFloat: not enough bytes left" );
            else if( m_io.fail() )
                EM_error3( "[chuck](via FileIO): cannot readFloat: I/O stream failed" );
            return (t_CKFLOAT)i;
        }
        else
        {
            EM_error3( "[chuck](via FileIO): readFloat error: invalid/unsupported datatype size flag" );
            return 0;
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): readFloat error: invalid ASCII/binary flag" );
        return 0;
    }
}




//-----------------------------------------------------------------------------
// name: readString()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_IO_File::readString( std::string & str )
{
    // set
    str = "";

    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot readString: no file open" );
        return FALSE;
    }

    if( m_io.eof() ) {
        EM_error3( "[chuck](via FileIO): cannot readString: EOF reached" );
        return FALSE;
    }

    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot read on directory" );
        return FALSE;
    }

    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot readString: I/O stream failed" );
        return FALSE;
    }

    if( m_flags & TYPE_ASCII ) {
        // ASCII
        m_io >> str;
        return TRUE;
    }
    else if( m_flags & TYPE_BINARY ) {
        EM_error3( "[chuck](via FileIO): readString not supported for binary mode" );
        return FALSE;
    }
    else {
        EM_error3( "[chuck](via FileIO): readInt error: invalid ASCII/binary flag" );
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
/* (ATODO: doesn't look like asynchronous reads will work)
//-----------------------------------------------------------------------------
THREAD_RETURN( THREAD_TYPE Chuck_IO_File::read_thread ) (void * data)
{
    // (ATODO: test this)
    cerr << "In thread" << endl;
    async_args * args = (async_args *)data;
    Chuck_String * ret = (Chuck_String *)(args->RETURN);
    ret = args->fileio_obj->read( args->intArg );
    cerr << "Called FileIO.read(int)" << endl;
    args->fileio_obj->m_asyncEvent->broadcast(); // wake up
    cerr << "Broadcasted on event" << endl;
    cerr << "Sleeping" << endl;
    sleep( 5 );
    cerr << "Woke up" << endl;
    delete args;
    cerr << "Deleted args" << endl;

    return (THREAD_RETURN)0;
}

THREAD_RETURN( THREAD_TYPE Chuck_IO_File::readLine_thread ) (void * data)
{
    // not yet implemented
    return NULL;
}

THREAD_RETURN( THREAD_TYPE Chuck_IO_File::readInt_thread ) (void * data)
{
    // (ATODO: test this)
    async_args * args = (async_args *)data;
    Chuck_DL_Return * ret = (Chuck_DL_Return *)(args->RETURN);
    ret->v_int = args->fileio_obj->readInt( args->intArg );
    cerr << "Called readInt, set ret->v_int to " << ret->v_int << endl;
    args->fileio_obj->m_asyncEvent->broadcast(); // wake up
    cerr << "Called broadcast" << endl;
    delete args;

    return (THREAD_RETURN)0;
}

THREAD_RETURN( THREAD_TYPE Chuck_IO_File::readFloat_thread ) (void * data)
{
    // not yet implemented
    return NULL;
}
*/
//-----------------------------------------------------------------------------




 //-----------------------------------------------------------------------------
 // name: eof()
 // desc: end of file?
 //-----------------------------------------------------------------------------
t_CKBOOL Chuck_IO_File::eof()
{
    if( !m_io.is_open() )
    {
        // EM_error3( "[chuck](via FileIO): cannot check eof: no file open" );
        return TRUE;
    }
    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot check eof on directory" );
        return TRUE;
    }

    // return EOF conditions (1.5.0.0: peek() was added since eof() is set
    // ONLY AFTER an effort is made to read and no more data is left)
    // https://stackoverflow.com/questions/4533063/how-does-ifstreams-eof-work
    return m_io.eof() || m_io.fail() || m_io.peek() == EOF;
}




//-----------------------------------------------------------------------------
// name: write( const std::string & val )
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_IO_File::write( const std::string & val )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }

    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }

    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    m_io.write( val.c_str(), val.size() );

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}




//-----------------------------------------------------------------------------
// name: write( t_CKINT val )
// desc: write an integer value to file
//-----------------------------------------------------------------------------
void Chuck_IO_File::write( t_CKINT val )
{
    // 0 flags means write (if binary will write in native int size)
    this->write( val, 0 );

//    // sanity
//    if( !(m_io.is_open()) ) {
//        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
//        return;
//    }
//
//    if( m_io.fail() ) {
//        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
//        return;
//    }
//
//    if( m_dir )
//    {
//        EM_error3( "[chuck](via FileIO): cannot write on directory" );
//        return;
//    }
//
//    if( m_flags & TYPE_ASCII ) {
//        m_io << val;
//    }
//    else if( m_flags & TYPE_BINARY ) {
//        m_io.write( (char *)&val, sizeof( t_CKINT ) );
//    }
//    else {
//        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
//    }
//
//    if( m_io.fail() ) { // check both before and after write if stream is ok
//        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
//    }
}




//-----------------------------------------------------------------------------
// name: write( t_CKINT val )
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_IO_File::write( t_CKINT val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }

    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }

    if( m_dir )
    {
        EM_error3( "[chuck](via FileIO): cannot write on directory" );
        return;
    }

    // check file mode
    if( m_flags & TYPE_ASCII ) // ASCII mode
    {
        // insert into file stream
        m_io << val;
    }
    else if( m_flags & TYPE_BINARY ) // BINARY mode
    {
        // check datatype size flags
        // 1.5.0.1 (ge) modified to new signed/unsigned handling
        if( flags & Chuck_IO::INT8 || flags & Chuck_IO::UINT8 )
        {
            // unsigned 8-bit
            uint8_t v = val;
            m_io.write( (char *)&v, 1 );
        }
        else if( flags & Chuck_IO::INT16 || flags & Chuck_IO::UINT16 )
        {
            // unsigned 16-bit
            uint16_t v = val;
            m_io.write( (char *)&v, 2 );
        }
        else if( flags & Chuck_IO::INT32 || flags & Chuck_IO::UINT32 )
        {
            // unsigned 32-bit
            uint32_t v = (uint32_t)val;
            m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::INT64 || flags & Chuck_IO::UINT64 )
        {
            // unsigned 64-bit
            uint64_t v = val;
            m_io.write( (char *)&v, 8 );
        }
        else if( flags & Chuck_IO::SINT8 )
        {
            // signed 8-bit
            int8_t v = val;
            m_io.write( (char *)&v, 1 );
        }
        else if( flags & Chuck_IO::SINT16 )
        {
            // signed 16-bit
            int16_t v = val;
            m_io.write( (char *)&v, 2 );
        }
        else if( flags & Chuck_IO::SINT32 )
        {
            // signed 32-bit
            int32_t v = (uint32_t)val;
            m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::SINT64 )
        {
            // signed 32-bit
            int64_t v = val;
            m_io.write( (char *)&v, 8 );
        }
        else
        {
            // write to native int size | 1.5.1.0
            m_io.write( (char *)&val, sizeof(t_CKINT) );
        }
    }
    else {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}




//-----------------------------------------------------------------------------
// name: write( t_CKFLOAT val )
// desc: write a floating point value
//-----------------------------------------------------------------------------
void Chuck_IO_File::write( t_CKFLOAT val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}




//-----------------------------------------------------------------------------
// name: write( t_CKFLOAT val, t_CKINT flags )
// desc: write a floating point value; binary mode will heed flags for size
//-----------------------------------------------------------------------------
void Chuck_IO_File::write( t_CKFLOAT val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << val;
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val;
            m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val;
            m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}

void Chuck_IO_File::write( const t_CKCOMPLEX & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_File::write( const t_CKCOMPLEX & val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << "#(" << val.re << "," << val.im << ")";
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val.re; m_io.write( (char *)&v, 4 );
                  v = (float)val.im; m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val.re; m_io.write( (char *)&v, 8 );
                   v = (double)val.im; m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}

void Chuck_IO_File::write( const t_CKPOLAR & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_File::write( const t_CKPOLAR & val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << "%(" << val.modulus << "," << val.phase << ")";
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val.modulus; m_io.write( (char *)&v, 4 );
                  v = (float)val.phase; m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val.modulus; m_io.write( (char *)&v, 8 );
                   v = (double)val.phase; m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}

void Chuck_IO_File::write( const t_CKVEC2 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_File::write( const t_CKVEC2 & val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << "@(" << val.x << "," << val.y << ")";
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val.x; m_io.write( (char *)&v, 4 );
                  v = (float)val.y; m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val.x; m_io.write( (char *)&v, 8 );
                   v = (double)val.y; m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}

void Chuck_IO_File::write( const t_CKVEC3 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_File::write( const t_CKVEC3 & val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << "@(" << val.x << "," << val.y << "," << val.z << ")";
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val.x; m_io.write( (char *)&v, 4 );
                  v = (float)val.y; m_io.write( (char *)&v, 4 );
                  v = (float)val.z; m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val.x; m_io.write( (char *)&v, 8 );
                   v = (double)val.y; m_io.write( (char *)&v, 8 );
                   v = (double)val.z; m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}

void Chuck_IO_File::write( const t_CKVEC4 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_File::write( const t_CKVEC4 & val, t_CKINT flags )
{
    // sanity
    if( !(m_io.is_open()) ) {
        EM_error3( "[chuck](via FileIO): cannot write: no file open" );
        return;
    }
    if( m_io.fail() ) {
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
        return;
    }
    if( m_dir ) {
        EM_error3( "[chuck](via FileIO): cannot write to a directory" );
        return;
    }

    // check ASCII or BINARY
    if( m_flags & TYPE_ASCII )
    {
        // insert into stream
        m_io << "@(" << val.x << "," << val.y << "," << val.z << "," << val.w << ")";
    }
    else if( m_flags & TYPE_BINARY )
    {
        // 1.5.0.1 (ge) add distinction between different float sizes
        if( flags & Chuck_IO::FLOAT32 )
        {
            // 32-bit
            float v = (float)val.x; m_io.write( (char *)&v, 4 );
                  v = (float)val.y; m_io.write( (char *)&v, 4 );
                  v = (float)val.z; m_io.write( (char *)&v, 4 );
                  v = (float)val.w; m_io.write( (char *)&v, 4 );
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            // 64-bit
            double v = (double)val.x; m_io.write( (char *)&v, 8 );
                   v = (double)val.y; m_io.write( (char *)&v, 8 );
                   v = (double)val.z; m_io.write( (char *)&v, 8 );
                   v = (double)val.w; m_io.write( (char *)&v, 8 );
        }
        else
        {
            EM_error3( "[chuck](via FileIO): writeFloat error: invalid/unsupport datatype size flag" );
        }
    }
    else
    {
        EM_error3( "[chuck](via FileIO): write error: invalid ASCII/binary flag" );
    }

    if( m_io.fail() ) { // check both before and after write if stream is ok
        EM_error3( "[chuck](via FileIO): cannot write: I/O stream failed" );
    }
}




#ifndef __DISABLE_THREADS__
// static helper functions for writing asynchronously
THREAD_RETURN( THREAD_TYPE Chuck_IO_File::writeStr_thread ) (void * data)
{
    async_args * args = (async_args *)data;
    args->fileio_obj->write( args->stringArg );
    Chuck_Event * e = args->fileio_obj->m_asyncEvent;
    delete args;
    e->broadcast_local(); // wake up
    e->broadcast_global();

    return (THREAD_RETURN)0;
}

THREAD_RETURN( THREAD_TYPE Chuck_IO_File::writeInt_thread ) (void * data)
{
    async_args * args = (async_args *)data;
    args->fileio_obj->write( args->intArg );
    args->fileio_obj->m_asyncEvent->broadcast_local(); // wake up
    args->fileio_obj->m_asyncEvent->broadcast_global();
    delete args;

    return (THREAD_RETURN)0;
}

THREAD_RETURN( THREAD_TYPE Chuck_IO_File::writeFloat_thread ) (void * data)
{
    async_args * args = (async_args *)data;
    args->fileio_obj->write( args->floatArg, args->intArg );
    args->fileio_obj->m_asyncEvent->broadcast_local(); // wake up
    args->fileio_obj->m_asyncEvent->broadcast_global();
    delete args;

    return (THREAD_RETURN)0;
}
#endif // __DISABLE_THREADS__




//-----------------------------------------------------------------------------
// name: Chuck_IO_Chout()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_IO_Chout::Chuck_IO_Chout( Chuck_Carrier * carrier )
{
    // store
    carrier->chout = this;
    // add ref
    this->add_ref();
    // zero out
    m_callback = NULL;
    // initialize (added 1.3.0.0)
    initialize_object( this, carrier->env->ckt_chout, NULL, carrier->vm );
    // lock so can't be deleted conventionally
    this->lock();
}

Chuck_IO_Chout::~Chuck_IO_Chout()
{
    m_callback = NULL;
}

void Chuck_IO_Chout::set_output_callback( void (*fp)(const char *) )
{
    m_callback = fp;
}

t_CKBOOL Chuck_IO_Chout::good()
{
    return m_callback != NULL || cout.good();
}

void Chuck_IO_Chout::close()
{ /* uh can't do it */
}

void Chuck_IO_Chout::flush()
{
    if( m_callback )
    {
        // send to callback
        m_callback( m_buffer.str().c_str() );
    }
    else
    {
        // send to cout
        g_ck_stdoutstream << m_buffer.str();
        // flush
        g_ck_stdoutstream.flush();
    }
    // clear buffer
    m_buffer.str( std::string() );
}

t_CKINT Chuck_IO_Chout::mode()
{
    return 0;
}

void Chuck_IO_Chout::mode( t_CKINT flag )
{ }

Chuck_String * Chuck_IO_Chout::readLine()
{
    return NULL;
}

t_CKINT Chuck_IO_Chout::readInt( t_CKINT flags )
{
    return 0;
}

t_CKFLOAT Chuck_IO_Chout::readFloat()
{
    return 0;
}

t_CKFLOAT Chuck_IO_Chout::readFloat( t_CKINT flags )
{
    return 0;
}

t_CKBOOL Chuck_IO_Chout::readString( std::string & str )
{
    str = "";
    return FALSE;
}

t_CKBOOL Chuck_IO_Chout::eof()
{
    return TRUE;
}

void Chuck_IO_Chout::write( const std::string & val )
// added 1.3.0.0: the flush
{
    // insert into stream
    m_buffer << val;
    // if there is an endline anywhere | 1.5.1.1
    if( val.find("\n") != string::npos ) flush();
    // previous: only if val == endline
    // if( val == "\n" ) flush();
}

void Chuck_IO_Chout::write( t_CKINT val )
{
    m_buffer << val;
}

void Chuck_IO_Chout::write( t_CKINT val, t_CKINT flags )
{
    // ignore flag for chout
    m_buffer << val;
}

void Chuck_IO_Chout::write( t_CKFLOAT val )
{
    m_buffer << val;
}

void Chuck_IO_Chout::write( t_CKFLOAT val, t_CKINT flags )
{
    // ignore flags for chout
    m_buffer << val;
}

void Chuck_IO_Chout::write( const t_CKCOMPLEX & val )
{
    // print complex value
    m_buffer << "#(" << val.re << "," << val.im << ")";
}

void Chuck_IO_Chout::write( const t_CKCOMPLEX & val, t_CKINT flags )
{
    // ignore flags for chout
    this->write( val );
}

void Chuck_IO_Chout::write( const t_CKPOLAR & val )
{
    // print polar value
    m_buffer << "%(" << val.modulus << "," << val.phase << ")";
}

void Chuck_IO_Chout::write( const t_CKPOLAR & val, t_CKINT flags )
{
    // ignore flags for chout
    this->write( val );
}

void Chuck_IO_Chout::write( const t_CKVEC2 & v )
{
    // print vec2 value
    m_buffer << "@(" << v.x << "," << v.y << ")";
}

void Chuck_IO_Chout::write( const t_CKVEC2 & v, t_CKINT flags )
{
    // ignore flags for chout
    this->write( v );
}

void Chuck_IO_Chout::write( const t_CKVEC3 & v )
{
    // print vec3 value
    m_buffer << "@(" << v.x << "," << v.y << "," << v.z << ")";
}

void Chuck_IO_Chout::write( const t_CKVEC3 & v, t_CKINT flags )
{
    // ignore flags for chout
    this->write( v );
}

void Chuck_IO_Chout::write( const t_CKVEC4 & v )
{
    // print vec4 value
    m_buffer << "@(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
}

void Chuck_IO_Chout::write( const t_CKVEC4 & v, t_CKINT flags )
{
    // ignore flags for chout
    this->write( v );
}




//-----------------------------------------------------------------------------
// name: Chuck_IO_Cherr()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_IO_Cherr::Chuck_IO_Cherr( Chuck_Carrier * carrier )
{
    // store
    carrier->cherr = this;
    // add ref
    this->add_ref();
    // zero out
    m_callback = NULL;
    // initialize (added 1.3.0.0)
    initialize_object( this, carrier->env->ckt_cherr, NULL, carrier->vm );
    // lock so can't be deleted conventionally
    this->lock();
}

Chuck_IO_Cherr::~Chuck_IO_Cherr()
{
    m_callback = NULL;
}

void Chuck_IO_Cherr::set_output_callback( void (*fp)(const char *) )
{
    m_callback = fp;
}

t_CKBOOL Chuck_IO_Cherr::good()
{
    return m_callback != NULL || cerr.good();
}

void Chuck_IO_Cherr::close()
{ /* uh can't do it */
}

void Chuck_IO_Cherr::flush()
{
    if( m_callback )
    {
        // send to callback
        m_callback( m_buffer.str().c_str() );
    }
    else
    {
        // send to cerr | this should auto-flush
        g_ck_stderrstream << m_buffer.str();
    }

    // clear buffer
    m_buffer.str( std::string() );
}

t_CKINT Chuck_IO_Cherr::mode()
{
    return 0;
}

void Chuck_IO_Cherr::mode( t_CKINT flag )
{ }

Chuck_String * Chuck_IO_Cherr::readLine()
{
    return NULL;
}

t_CKINT Chuck_IO_Cherr::readInt( t_CKINT flags )
{
    return 0;
}

t_CKFLOAT Chuck_IO_Cherr::readFloat()
{
    return 0;
}

t_CKFLOAT Chuck_IO_Cherr::readFloat( t_CKINT flags )
{
    return 0;
}

t_CKBOOL Chuck_IO_Cherr::readString( std::string & str )
{
    str = "";
    return FALSE;
}

t_CKBOOL Chuck_IO_Cherr::eof()
{
    return TRUE;
}

void Chuck_IO_Cherr::write( const std::string & val )
{
    // insert into stream
    m_buffer << val;
    // always flush for cerr | 1.5.0.0 (ge) added
    flush();
    // if( val.find("\n") != string::npos ) flush();
}

void Chuck_IO_Cherr::write( t_CKINT val )
{
    m_buffer << val;
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( t_CKINT val, t_CKINT flags )
{
    m_buffer << val;
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( t_CKFLOAT val )
{
    m_buffer << val;
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( t_CKFLOAT val, t_CKINT flags )
{
    // ignore flags for cherr
    m_buffer << val;
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKCOMPLEX & val )
{
    // print complex value
    m_buffer << "#(" << val.re << "," << val.im << ")";
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKCOMPLEX & val, t_CKINT flags )
{
    // ignore flags for cherr
    this->write( val );
}

void Chuck_IO_Cherr::write( const t_CKPOLAR & val )
{
    // print polar value
    m_buffer << "%(" << val.modulus << "," << val.phase << ")";
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKPOLAR & val, t_CKINT flags )
{
    // ignore flags for cherr
    this->write( val );
}

void Chuck_IO_Cherr::write( const t_CKVEC2 & v )
{
    // print vec2 value
    m_buffer << "@(" << v.x << "," << v.y << ")";
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKVEC2 & v, t_CKINT flags )
{
    // ignore flags for cherr
    this->write( v );
}

void Chuck_IO_Cherr::write( const t_CKVEC3 & v )
{
    // print vec3 value
    m_buffer << "@(" << v.x << "," << v.y << "," << v.z << ")";
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKVEC3 & v, t_CKINT flags )
{
    // ignore flags for cherr
    this->write( v );
}

void Chuck_IO_Cherr::write( const t_CKVEC4 & v )
{
    // print vec4 value
    m_buffer << "@(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
    flush(); // always flush for cerr | 1.5.0.0 (ge) added
}

void Chuck_IO_Cherr::write( const t_CKVEC4 & v, t_CKINT flags )
{
    // ignore flags for cherr
    this->write( v );
}




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
    CK_SAFE_DELETE(m_read_thread);
    if( m_event_buffer )
        m_vmRef->destroy_event_buffer( m_event_buffer );

    close();

    CK_SAFE_DELETE(m_io_buf);
    m_io_buf_max = 0;
    m_io_buf_pos = m_io_buf_available = 0;

    CK_SAFE_DELETE(m_tmp_buf);
    m_tmp_buf_max = 0;

    s_serials.remove(this);
}

t_CKBOOL Chuck_IO_Serial::ready()
{
#ifndef __PLATFORM_WINDOWS__
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
    if( flags & Chuck_IO::TYPE_BINARY )
    {
        m_flags = Chuck_IO::TYPE_BINARY;
    }
    else if( flags & Chuck_IO::TYPE_ASCII )
    {
        m_flags = Chuck_IO::TYPE_ASCII;
    }
    else
    {
        EM_log(CK_LOG_WARNING, "(Serial.open): warning: invalid binary/ASCII flag requested, defaulting to ASCII");
        m_flags = Chuck_IO::TYPE_ASCII;
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
#ifndef __PLATFORM_WINDOWS__
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
#ifdef __PLATFORM_WINDOWS__
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

    if( m_flags & Chuck_IO::TYPE_BINARY )
    {
        if( flags & Chuck_IO::INT8 || flags & Chuck_IO::UINT8 ) // 1.5.0.1 (ge) added UINT
        {
            uint8_t byte = 0;
            if(!fread(&byte, 1, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = byte;
        }
        else if( flags & Chuck_IO::INT16 || flags & Chuck_IO::UINT16 ) // 1.5.0.1 (ge) added UINT
        {
            uint16_t word = 0;
            if(!fread(&word, 2, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = word;
        }
        else if( flags & Chuck_IO::INT32 || flags & Chuck_IO::UINT32 ) // 1.5.0.1 (ge) added UINT
        {
            uint32_t dword = 0;
            if(!fread(&dword, 4, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = dword;
        }
        else if( flags & Chuck_IO::INT64 || flags & Chuck_IO::UINT64 ) // 1.5.0.1 (ge) added UINT and 64-bit
        {
            uint64_t val = 0;
            if(!fread(&val, 8, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = val;
        }
        else if( flags & Chuck_IO::SINT8 ) // 1.5.0.1 (ge) added SINT
        {
            int8_t byte = 0;
            if(!fread(&byte, 1, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = byte;
        }
        else if( flags & Chuck_IO::SINT16 ) // 1.5.0.1 (ge) added SINT
        {
            int16_t word = 0;
            if(!fread(&word, 2, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = word;
        }
        else if( flags & Chuck_IO::SINT32 ) // 1.5.0.1 (ge) added SINT
        {
            int32_t dword = 0;
            if(!fread(&dword, 4, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = dword;
        }
        else if( flags & Chuck_IO::SINT64 ) // 1.5.0.1 (ge) added SINT and 64-bit
        {
            int64_t val = 0;
            if(!fread(&val, 8, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");

            i = val;
        }
    }
    else
    {
        if(!fscanf(m_cfd, "%li", &i))
            EM_log(CK_LOG_WARNING, "(Serial.readInt): error: read failed");
    }

    return i;
}

t_CKFLOAT Chuck_IO_Serial::readFloat( )
{
    // 1.5.0.1 (ge) redirect
    return readFloat( Chuck_IO::FLOAT32 );
}

t_CKFLOAT Chuck_IO_Serial::readFloat( t_CKINT flags )
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

    if( m_flags & Chuck_IO::TYPE_BINARY )
    {
        // check size
        if( flags & Chuck_IO::FLOAT32 )
        {
            float v = 0.0f;
            if(!fread(&v, 4, 1, m_cfd))
                EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
            f = v;
        }
        else if( flags & Chuck_IO::FLOAT64 )
        {
            double v = 0.0;
            if( !fread(&v, 8, 1, m_cfd) )
                EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
            f = v;
        }
        else
        {
            // error message
            EM_log(CK_LOG_WARNING, "(Serial.readFloat): invalid or unsupported size flag");
        }
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

    if( m_flags & Chuck_IO::TYPE_BINARY )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readString): warning: cannot read string from binary file");
        return FALSE;
    }

    if(!fscanf(m_cfd, CHUCK_IO_SCANF_STRING, &m_tmp_buf))
    {
        EM_log(CK_LOG_WARNING, "(Serial.readFloat): error: read failed");
        return FALSE;
    }

    str = (char *)  m_tmp_buf;

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

    if( m_flags & Chuck_IO::TYPE_BINARY )
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): warning: cannot read line from binary file");
        return NULL;
    }

    if(!fgets((char *)m_tmp_buf, (int)m_tmp_buf_max, m_cfd))
    {
        EM_log(CK_LOG_WARNING, "(Serial.readLine): error: from fgets");
        return NULL;
    }

    Chuck_String * str = new Chuck_String;
    initialize_object(str, m_vmRef->env()->ckt_string, NULL, m_vmRef);

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
    write( val, Chuck_IO::INT32 );
}


void Chuck_IO_Serial::write( t_CKINT val, t_CKINT flags )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: file not open");
        return;
    }

    // check flags
    if( (flags & Chuck_IO::INT8 ) || (flags & Chuck_IO::INT16) ||
        (flags & Chuck_IO::INT32) || (flags & Chuck_IO::INT64) )
    {
        EM_log( CK_LOG_WARNING, "(Serial.write): warning: invalid int size flag, ignoring write request" );
        return;
    }

    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();

        if( m_flags & Chuck_IO::TYPE_ASCII )
        {
            // TODO: don't use m_tmp_buf (thread safety?)
#ifdef __PLATFORM_WINDOWS__
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
            // 1.5.0.1 (ge) check and handle individually
            uint8_t v8 = (uint8_t)val;
            uint16_t v16 = (uint16_t)val;
            uint32_t v32 = (uint32_t)val;
            uint64_t v64 = (uint64_t)val;
            char * buf = NULL;
            t_CKINT size = 0;

            if( flags & Chuck_IO::INT8 ) { buf = (char *)&v8; size = 1; }
            else if( flags & Chuck_IO::INT16 ) { buf = (char *)&v16; size = 2; }
            else if( flags & Chuck_IO::INT32 ) { buf = (char *)&v32; size = 4; }
            else if( flags & Chuck_IO::INT64 ) { buf = (char *)&v64; size = 8; }
            // other size values should be caught above

            // put into write buffer
            for(int i = 0; i < size; i++)
                m_writeBuffer.put( buf[i] );

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
        if( m_flags & Chuck_IO::TYPE_ASCII )
        {
            fprintf( m_cfd, "%li", (long)val );
        }
        else
        {
            // 1.5.0.1 (ge) check and handle individually
            if( flags & Chuck_IO::INT8 )
            {
                uint8_t v = (uint8_t)val;
                fwrite( (char*)&v, 1, 1, m_cfd );
            }
            else if( flags & Chuck_IO::INT16 )
            {
                uint16_t v = (uint16_t)val;
                fwrite( (char*)&v, 1, 2, m_cfd );
            }
            else if( flags & Chuck_IO::INT32 )
            {
                uint32_t v = (uint32_t)val;
                fwrite( (char*)&v, 1, 4, m_cfd );
            }
            else if( flags & Chuck_IO::INT64 )
            {
                uint64_t v = (uint64_t)val;
                fwrite( (char*)&v, 1, 8, m_cfd );
            }
            // other size values should be caught above
        }
    }
}

void Chuck_IO_Serial::write( t_CKFLOAT val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( t_CKFLOAT val, t_CKINT flags )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.write): warning: file not open");
        return;
    }

    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();

        if( m_flags & Chuck_IO::TYPE_ASCII )
        {
#ifdef __PLATFORM_WINDOWS__
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
            // 1.5.0.1 (ge) check datatype size flag
            if( flags & Chuck_IO::FLOAT32 )
            {
                float v = (float)val;
                char * buf = (char *)&v;
                for(int i = 0; i < sizeof(v); i++)
                    m_writeBuffer.put(buf[i]);
            }
            else if( flags & Chuck_IO::FLOAT64 )
            {
                double v = (double)val;
                char * buf = (char *)&v;
                for(int i = 0; i < sizeof(v); i++)
                    m_writeBuffer.put(buf[i]);
            }
            else
            {
                // error message
                EM_log(CK_LOG_WARNING, "(Serial.write): invalid or unsupported size flag");
            }

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
        if( m_flags & Chuck_IO::TYPE_ASCII )
        {
            fprintf( m_cfd, "%f", val );
        }
        else // BINARY
        {
            // 1.5.0.1 (ge) check datatype size flag
            if( flags & Chuck_IO::FLOAT32 )
            {
                float v = (float)val;
                char * buf = (char *)&v;
                for(int i = 0; i < sizeof(v); i++)
                    m_writeBuffer.put(buf[i]);
            }
            else if( flags & Chuck_IO::FLOAT64 )
            {
                double v = (double)val;
                char * buf = (char *)&v;
                for(int i = 0; i < sizeof(v); i++)
                    m_writeBuffer.put(buf[i]);
            }
            else
            {
                // error message
                EM_log(CK_LOG_WARNING, "(Serial.write): invalid or unsupported size flag");
            }
        }
    }
}

void Chuck_IO_Serial::write( const t_CKCOMPLEX & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( const t_CKCOMPLEX & val, t_CKINT flags )
{
    // NOTE this will NOT output chuck-format complex #(re,im) in ASCII mode
    this->write( val.re, flags );
    this->write( val.im, flags );
}

void Chuck_IO_Serial::write( const t_CKPOLAR & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( const t_CKPOLAR & val, t_CKINT flags )
{
    // NOTE this will NOT output chuck-format polar %(modulus,phase) in ASCII mode
    this->write( val.modulus, flags );
    this->write( val.phase, flags );
}

void Chuck_IO_Serial::write( const t_CKVEC2 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( const t_CKVEC2 & val, t_CKINT flags )
{
    // NOTE this will NOT output chuck-format vec2 @(x,y) in ASCII mode
    this->write( val.x, flags );
    this->write( val.y, flags );
}

void Chuck_IO_Serial::write( const t_CKVEC3 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( const t_CKVEC3 & val, t_CKINT flags )
{
    // NOTE this will NOT output chuck-format vec3 @(x,y,z) in ASCII mode
    this->write( val.x, flags );
    this->write( val.y, flags );
    this->write( val.z, flags );
}

void Chuck_IO_Serial::write( const t_CKVEC4 & val )
{
    this->write( val, Chuck_IO::FLOAT32 );
}

void Chuck_IO_Serial::write( const t_CKVEC4 & val, t_CKINT flags )
{
    // NOTE this will NOT output chuck-format vec4 @(x,y,z,w) in ASCII mode
    this->write( val.x, flags );
    this->write( val.y, flags );
    this->write( val.z, flags );
    this->write( val.w, flags );
}

void Chuck_IO_Serial::writeBytes( Chuck_ArrayInt * arr )
{
    if( !good() )
    {
        EM_log(CK_LOG_WARNING, "(Serial.writeBytes): warning: file not open");
        return;
    }

    if( m_iomode == MODE_ASYNC )
    {
        start_read_thread();

        t_CKINT len = arr->size();
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
        t_CKINT len = arr->size();
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
#ifdef __PLATFORM_WINDOWS__
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
        EM_log(CK_LOG_HERALD, "(SerialIO.readAsync): warning: request buffer overflow, dropping read");
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
        initialize_object(arr, m_vmRef->env()->ckt_array, NULL, m_vmRef);
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
        initialize_object(arr, m_vmRef->env()->ckt_array, NULL, m_vmRef);
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
        initialize_object(arr, m_vmRef->env()->ckt_array, NULL, m_vmRef);
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
#ifndef __PLATFORM_WINDOWS__
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_fd, &fds);

    timeval tv;
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = 1000 * (timeout_ms%1000);

    t_CKINT result = select(m_fd+1, &fds, NULL, NULL, &tv);

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
            EM_log(CK_LOG_HERALD, "(SerialIO::get_buffer): read() returned 0 bytes");
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

#ifdef __PLATFORM_WINDOWS__
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
    Chuck_ArrayFloat * array = new Chuck_ArrayFloat(0);

    for(int i = 0; i < r.m_num && !m_do_exit; i++)
    {
        t_CKUINT len = 0;

        while(!m_do_exit)
        {
            if(peek_buffer() == -1)
                break;

            // TODO: '\r'?
            int c = (int)peek_buffer();

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
    CK_SAFE_DELETE(array);
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;

    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_int_ascii(Chuck_IO_Serial::Request & r)
{
    t_CKINT val = 0;
    int numRead = 0;
    Chuck_ArrayInt * array = new Chuck_ArrayInt(FALSE, 0);
    initialize_object( array, m_vmRef->env()->ckt_array, NULL, m_vmRef ); // 1.5.0.0 (ge) added
    for(int i = 0; i < r.m_num; i++)
    {
        t_CKUINT len = 0;

        while(!m_do_exit)
        {
            if(peek_buffer() == -1)
                break;

            int c = (int)pull_buffer();

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
    CK_SAFE_DELETE(array);
    r.m_val = 0;
    r.m_status = Chuck_IO_Serial::Request::RQ_STATUS_FAILURE;

    return TRUE;
}

t_CKBOOL Chuck_IO_Serial::handle_byte(Chuck_IO_Serial::Request & r)
{
    // binary
    int size = 1;
    t_CKUINT num = r.m_num;
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
        Chuck_ArrayInt * array = new Chuck_ArrayInt(FALSE, r.m_num);
        initialize_object( array, m_vmRef->env()->ckt_array, NULL, m_vmRef ); // 1.5.0.0 (ge) added
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

    t_CKUINT num = r.m_num;

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

    Chuck_ArrayFloat * array = new Chuck_ArrayFloat(r.m_num);
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

    t_CKUINT num = r.m_num;

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

    Chuck_ArrayInt * array = new Chuck_ArrayInt(FALSE, r.m_num);
    initialize_object( array, m_vmRef->env()->ckt_array, NULL, m_vmRef ); // 1.5.0.0 (ge) added
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
#ifdef __PLATFORM_WINDOWS__
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
                EM_log(CK_LOG_HERALD, "SerialIO.read_cb: error: response buffer overflow, dropping read");
                continue;
            }

            if(m_flags & Chuck_IO::TYPE_ASCII)
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

        ck_usleep(100);
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
        ck_usleep(100);
    }

    delete[] tmp_writethread_buf;
    tmp_writethread_buf = NULL;
}


t_CKBOOL Chuck_IO_Serial::setBaudRate( t_CKUINT rate )
{
#ifndef __PLATFORM_WINDOWS__
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
#ifndef __PLATFORM_WINDOWS__
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
    func->doc = "get next requested byte.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getBytes
    func = make_new_mfun("int[]", "getBytes", serialio_getBytes);
    func->doc = "get next requested number of bytes.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getInts
    func = make_new_mfun("int[]", "getInts", serialio_getInts);
    func->doc = "get next requested number of integers.";
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
    // func->doc = "(internal) used by virtual machine for synthronization.";
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add file modes (moved into IO | 1.5.0.0 (ge))
    // type_engine_import_svar(env, "int", "BINARY", TRUE, (t_CKUINT) &Chuck_IO_File::TYPE_BINARY, "Binary mode");
    // type_engine_import_svar(env, "int", "ASCII", TRUE, (t_CKUINT) &Chuck_IO_File::TYPE_ASCII, "ASCII mode");

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

    // add examples
    if( !type_engine_import_add_ex( env, "serial/byte.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/bytes.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/ints-bin.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "serial/ints-ascii.ck" ) ) goto error;
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
    Chuck_ArrayInt * array = new Chuck_ArrayInt(TRUE, 0);
    initialize_object( array, SHRED->vm_ref->env()->ckt_array, SHRED, VM );

    for(vector<string>::iterator i = devices.begin(); i != devices.end(); i++)
    {
        Chuck_String * name = new Chuck_String(*i);
        initialize_object(name, SHRED->vm_ref->env()->ckt_string, SHRED, VM );
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
    Chuck_ArrayInt * arr = (Chuck_ArrayInt *) GET_NEXT_OBJECT(ARGS);
    cereal->writeBytes(arr);
}

CK_DLL_MFUN(serialio_canWait)
{
    Chuck_IO_Serial *cereal = (Chuck_IO_Serial *) SELF;
    RETURN->v_int = cereal->can_wait();
}

#endif // __DISABLE_SERIAL__
