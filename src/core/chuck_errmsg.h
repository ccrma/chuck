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
//       2002 version by Ge Wang, based on Andrew Appel's Tiger code
//       2017 addition by Jacy Atherton
//       2023 refactor and code highlight by Ge Wang
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Summer 2002 to 2022 and ...
//-----------------------------------------------------------------------------
#ifndef __CHUCK_ERRORMSG_H__
#define __CHUCK_ERRORMSG_H__

#include "chuck_def.h"
#include <stdio.h>


// log levels
#define CK_LOG_ALL              10 // set this to log everything
#define CK_LOG_FINEST           9
#define CK_LOG_FINER            8
#define CK_LOG_FINE             7
#define CK_LOG_DEBUG            6  // 1.5.0.5 was: CK_LOG_CONFIG
#define CK_LOG_INFO             5
#define CK_LOG_WARNING          4
#define CK_LOG_SEVERE           3
#define CK_LOG_SYSTEM           2
#define CK_LOG_CORE             1
#define CK_LOG_NONE             0  // set this to log nothing


// C linkage
#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

//-----------------------------------------------------------------------------
// the many ways to report errors; tools for an error prone world
//-----------------------------------------------------------------------------
// log macros -- generally more efficient than calling EM_log() directly...
// since this macro conditionally computes args based on log level
#define CK_LOG( level, ... ) do{ if(level <= g_loglevel) \
                                 { EM_log( level, __VA_ARGS__ ); } }while(0)

// output log message
void EM_log( t_CKINT, c_constr, ... );
// set log level [CK_LOG_NONE, CK_LOG_ALL]
void EM_setlog( t_CKINT level );
// push the log indentation
void EM_pushlog();
// pop log indentation
void EM_poplog();
// actual level
extern t_CKINT g_loglevel;

// [%s]:line(%d).char(%d):
void EM_error( t_CKINT, c_constr, ... );
// [%s]:line(%d): (terminal colors mode)
void EM_error2( t_CKINT, c_constr, ... );
// [%s]:line(%d): (classic mode)
void EM_error2b( t_CKINT, c_constr, ... );
// prints message, no line number
void EM_error3( c_constr, ... );
// prints exception message
void EM_exception( c_constr, ... );

// like EM_error2() minus line arg and error
void EM_print2vanilla( c_constr, ... );
// green edition
void EM_print2green( c_constr, ... );
// blue edition
void EM_print2blue( c_constr, ... );
// orange edition
void EM_print2orange( c_constr, ... );
// magenta edition
void EM_print2magenta( c_constr, ... );

// clear last error message
void EM_reset_msg();
// prepare for new file before a file
void EM_start_filename( c_constr filename );
// clear state after a file
void EM_reset_filename();
// get last erorr
const char * EM_lasterror();

// get filename portion of path; e.g., mini("foo/bar.ck") -> "bar.ck"
const char * mini( const char * path );
const char * mini_type( const char * str );




//-----------------------------------------------------------------------------
// things connected with lexer and parser
//-----------------------------------------------------------------------------
// variables
extern t_CKINT EM_tokPos;
extern t_CKINT EM_lineNum;

// EM_extLineNum is synced with EM_lineNum in lexer/parser phase, then synced
// with scanner/typechecker (EM_lineNum is not synced with scanner/typechecker)
extern t_CKINT EM_extLineNum;

// advance state when new line is encountered
void EM_newline( t_CKINT pos );


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

// macros
#define CK_FPRINTF_STDOUT(...) ck_fprintf_stdout(__VA_ARGS__)
#define CK_FPRINTF_STDERR(...) ck_fprintf_stderr(__VA_ARGS__)
#define CK_FFLUSH_STDOUT() ck_fflush_stdout()
#define CK_FFLUSH_STDERR() ck_fflush_stderr()
#define CK_VFPRINTF_STDOUT(message, ap) ck_vfprintf_stdout(message, ap)
#define CK_VFPRINTF_STDERR(message, ap) ck_vfprintf_stderr(message, ap)

#if defined(_cplusplus) || defined(__cplusplus)
} // end C linkage
#endif




//-----------------------------------------------------------------------------
// begin the callback printing (c++)
//-----------------------------------------------------------------------------
#if defined(_cplusplus) || defined(__cplusplus)
extern "C++"
{
#include <sstream>

// forward reference | 1.5.0.5 (ge) added
class ChucK;

// set current ChucK; used to query params | 1.5.0.5
void EM_set_current_chuck( ChucK * ck );




//-----------------------------------------------------------------------------
// c++: custom stream-like thing | REFACTOR-2017 (jack)
//-----------------------------------------------------------------------------
class ChuckOutStream
{
public:
    ChuckOutStream( bool isErr );
    ~ChuckOutStream();

    // there's probably a way to do this with templates or something
    // this is not that way
    ChuckOutStream& operator<<( const std::string & val );
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

    // set callback to direct this object's output
    void set_callback( void (*callback)(const char *) );
    // flush | moved from private to public | 1.5.1.1
    void flush();

private:
    // the string stream
    std::stringstream m_stream;
    // the callback
    void (*m_callback)(const char *);
    // am I an error stream?
    bool m_isErr;
};

// stdout ("buffered")
extern ChuckOutStream g_ck_stdoutstream;
// stderr ("unbuffered")
extern ChuckOutStream g_ck_stderrstream;

#define CK_STDCOUT g_ck_stdoutstream
#define CK_STDCERR g_ck_stderrstream
#define CK_STDENDL std::string("\n")

} // end C++

#else // if not C++

// c: oh well
#define CK_STDCOUT std::cout
#define CK_STDCERR std::cerr
#define CK_STDENDL std::endl

#endif // #if defined(_cplusplus) || defined(__cplusplus)
//-----------------------------------------------------------------------------
// here endeth the callback printing
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// more c++ in a mix c/c++ land
//-----------------------------------------------------------------------------
#if defined(_cplusplus) || defined(__cplusplus)
extern "C++" {
//-----------------------------------------------------------------------------
// 1.5.0.5 (ge) added
// struct CompileFileSource
// desc: information about file currently being compiled
//       this is so we can load the file and give better compiler feedback
//-----------------------------------------------------------------------------
struct CompileFileSource
{
    // which of the following should we use?
    t_CKINT which; // 1:filepath, 2:codeLiteral, 3:FILE
    // file path
    std::string path;
    // pointer to code literal
    const char * code;
    // file descriptor
    FILE * file;

    // constructor
    CompileFileSource( int w = 0, const std::string & p = "", const char * c = NULL, FILE * f = NULL )
    : which(w), path(p), code(c), file(f)
    { }

    // reset
    void reset() { which = 0; path = ""; code = NULL; file = NULL; }
    // set
    void setPath( const std::string & f ) { path = f; which = 1;}
    void setCode( const char * c ) { code = c; which = 2;}
    void setFile( FILE * f ) { file = f; which = 3;}

    // get line from source
    std::string getLine( t_CKUINT n );
};

// set
void EM_setCurrentFileSource( const CompileFileSource & info );
void EM_cleanupCurrentFileSource();


//-----------------------------------------------------------------------------
// name: class SmartPushLog (added 1.4.1.0 spencer)
// desc: scoped EM_pushlog for convenience and auto-balancing of push/pop logs
// usage: create an instance of SmartPushLog when you want to push the
// log level; the log level will be automatically popped when the instance goes
// out of scope and the SmartPushLog is deconstructed.
//-----------------------------------------------------------------------------
class SmartPushLog
{
public:
    SmartPushLog()
    {
        EM_pushlog();
    }

    ~SmartPushLog()
    {
        EM_poplog();
    }
};


}
#endif // #if defined(_cplusplus) || defined(__cplusplus)


#endif // __CHUCK_ERRORMSG_H__
