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
// file: errmsg.h
// desc: functions used in all phases of the compiler to give error messages
//       based on Andrew Appel's Tiger code
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on code by: Andrew Appel (appel@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_ERRORMSG_H__
#define __CHUCK_ERRORMSG_H__

#include "chuck_def.h"
#include <stdio.h>


#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif


extern t_CKBOOL EM_anyErrors;
extern int EM_tokPos;
extern int EM_lineNum;

// EM_extLineNum is synced with EM_lineNum in lexer/parser phase, then synced 
// with scanner/typechecker (EM_lineNum is not synced with scanner/typechecker)
extern int EM_extLineNum; 

void EM_newline( );

// levels
#define CK_LOG_CRAZY            10 // set this to log everything
#define CK_LOG_FINEST           9
#define CK_LOG_FINER            8
#define CK_LOG_FINE             7
#define CK_LOG_CONFIG           6
#define CK_LOG_INFO             5
#define CK_LOG_WARNING          4
#define CK_LOG_SEVERE           3
#define CK_LOG_SYSTEM           2
#define CK_LOG_CORE             1
#define CK_LOG_NONE             0  // set this to log nothing


//-----------------------------------------------------------------------------
// REFACTOR-2017: (jack) -- callback printing
//-----------------------------------------------------------------------------
// printing
void ck_fprintf_stdout( const char * format, ... );
void ck_fprintf_stderr( const char * format, ... );
void ck_fflush_stdout();
void ck_fflush_stderr();
void ck_vfprintf_stdout( const char * format, va_list args );
void ck_vfprintf_stderr( const char * format, va_list args );

// callbacks
void ck_set_stdout_callback( void (*callback)(const char *) );
void ck_set_stderr_callback( void (*callback)(const char *) );

#define CK_FPRINTF_STDOUT(...) ck_fprintf_stdout(__VA_ARGS__)
#define CK_FPRINTF_STDERR(...) ck_fprintf_stderr(__VA_ARGS__)
#define CK_FFLUSH_STDOUT() ck_fflush_stdout()
#define CK_FFLUSH_STDERR() ck_fflush_stderr()
#define CK_VFPRINTF_STDOUT(message, ap) ck_vfprintf_stdout(message, ap)
#define CK_VFPRINTF_STDERR(message, ap) ck_vfprintf_stderr(message, ap)

#ifdef __cplusplus
// c++: custom stream-like thing
extern "C++" {
#include <sstream>

class ChuckOutStream
{
public:
    ChuckOutStream( bool isErr );
    ~ChuckOutStream();

    // there's probably a way to do this with templates or something
    // this is not that way
    ChuckOutStream& operator<<( const std::string val );
    ChuckOutStream& operator<<( const char * val );
    ChuckOutStream& operator<<( const double val );
    ChuckOutStream& operator<<( const float val );
    ChuckOutStream& operator<<( const unsigned long long val );
    ChuckOutStream& operator<<( const long long val );
    ChuckOutStream& operator<<( const unsigned long val );
    ChuckOutStream& operator<<( const long val );
    ChuckOutStream& operator<<( const unsigned int val );
    ChuckOutStream& operator<<( const int val );
    ChuckOutStream& operator<<( const bool val );

    void set_callback( void (*callback)(const char *) );

private:
    void flush();
    std::stringstream m_stream;
    void (*m_callback)(const char *);
    bool m_isErr;
};


extern ChuckOutStream g_ck_stdoutstream;
extern ChuckOutStream g_ck_stderrstream;

#define CK_STDCOUT g_ck_stdoutstream
#define CK_STDCERR g_ck_stderrstream
#define CK_STDENDL std::string("\n")

}
#else
// c: oh well
#define CK_STDCOUT std::cout
#define CK_STDCERR std::cerr
#define CK_STDENDL std::endl

#endif
//-----------------------------------------------------------------------------
// here endeth the callback printing
//-----------------------------------------------------------------------------




void EM_log( t_CKINT, c_constr, ... );
void EM_setlog( t_CKINT );
void EM_pushlog();
void EM_poplog();

// actual level
extern t_CKINT g_loglevel;
// macro to compare
#define DO_LOG(x) ( x <= g_loglevel )

void EM_error( int, c_constr, ... );
void EM_error2( int, c_constr, ... );
void EM_error2b( int, c_constr, ... );
void EM_error3( c_constr, ... );
void EM_impossible( c_constr, ... );
t_CKBOOL EM_reset( c_constr filename, FILE * fd );
void EM_change_file( c_constr filename );
const char * EM_lasterror();
void EM_reset_msg();

const char * mini( const char * str );
const char * mini_type( const char * str );

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif


#endif
