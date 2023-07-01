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
// file: errmsg.cpp
// desc: functions used in all phases of the compiler to give error messages
//       based on Andrew Appel's Tiger code
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on code by: Andrew Appel (appel@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#include "chuck_errmsg.h"
#include "chuck_utils.h"
#include "util_platforms.h"
#include "util_string.h"

#ifndef __DISABLE_THREADS__
#include "util_thread.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sstream>
#include <iostream>
using namespace std;


// global
t_CKINT EM_tokPos = 0;
t_CKINT EM_lineNum = 1;
t_CKINT EM_extLineNum = 1;

// local global
static const char * the_filename = "";
static t_CKINT the_lineNum = 1;
#define CK_ERR_BUF_LENGTH 2048
static char g_buffer[CK_ERR_BUF_LENGTH] = "";
static std::string g_lasterror = "";

// log globals
t_CKINT g_loglevel = CK_LOG_CORE;
t_CKINT g_logstack = 0;
#ifndef __DISABLE_THREADS__
XMutex g_logmutex;
#endif

// more local globals
static const size_t g_buffer2_size = 1024;
static char g_buffer2[g_buffer2_size] = "";
std::stringstream g_stdout_stream;
std::stringstream g_stderr_stream;

// local global callbacks
void (*g_stdout_callback)(const char *) = NULL;
void (*g_stderr_callback)(const char *) = NULL;

// file info | 1.5.0.5 (ge)
static CompileFileSource g_currentFile;
// whether to highlight
static t_CKBOOL g_highlight_on_error = TRUE;

// name
static const char * g_str[] = {
    "NONE",         // 0
    "CKCORE",       // 1
    "SYSTEM",       // 2
    "SEVERE",       // 3
    "WARN!!",       // 4
    "INFORM",       // 5
    "CONFIG",       // 6
    "FINE!!",       // 7
    "FINER!",       // 8
    "FINEST",       // 9
    "ALL!!"           // 10
};




//-----------------------------------------------------------------------------
// name: intList
// desc: a linked list of token positions by line
//-----------------------------------------------------------------------------
typedef struct intList { t_CKINT i; struct intList *rest; } *IntList;
// for the current file
static IntList the_linePos = NULL;
// constructor
static IntList intList( t_CKINT i, IntList rest )
{
    IntList l = (IntList)checked_malloc(sizeof *l);
    l->i=i; l->rest=rest;
    return l;
}
//-----------------------------------------------------------------------------
// called by lexer on new line
// 1.5.0.5 (ge) updated to take a pos argument
//-----------------------------------------------------------------------------
void EM_newline( t_CKINT pos )
{
    the_lineNum++;
    EM_lineNum++;
    EM_extLineNum++;
    the_linePos = intList( pos, the_linePos );

    // debug print
    // cerr << "line: " << EM_lineNum << " pos: " << pos << endl;
}




//-----------------------------------------------------------------------------
// content after rightmost '/'
//-----------------------------------------------------------------------------
const char * mini( const char * str )
{
    t_CKINT len = strlen( str );
    const char * p = str + len;
    while( p != str && *p != '/' && *p != '\\' ) p--;
    return ( p == str || strlen(p+1) == 0 ? p : p+1 );
}




//-----------------------------------------------------------------------------
// content after 'struct'
//-----------------------------------------------------------------------------
const char * mini_type( const char * str )
{
    if( !strncmp( str, "struct ", 7 ) ) str += 7;
    const char * p = str;
    while( *p && *p >= '0' && *p <= '9' ) p++;
    return p;
}




//-----------------------------------------------------------------------------
// name: lastErrorCat()
// desc: concatenate into last error string buffer
//-----------------------------------------------------------------------------
static void lastErrorCat( const std::string & str )
{
    // concat
    g_lasterror += str;
}




//-----------------------------------------------------------------------------
// clear last error
//-----------------------------------------------------------------------------
void EM_reset_msg()
{
    g_lasterror = "";
}




//-----------------------------------------------------------------------------
// whether to highlight code on compiler error
//-----------------------------------------------------------------------------
void EM_highlight_on_error( t_CKBOOL yesOrNo )
{
    g_highlight_on_error = yesOrNo;
}




//-----------------------------------------------------------------------------
// name: EM_printLineInCode() | 1.5.0.5 (ge) added
// desc: output a particular line of code, with optional char pos for caret ^
//-----------------------------------------------------------------------------
void EM_printLineInCode( t_CKINT lineNumber, t_CKINT charNumber )
{
    // check
    if( !g_highlight_on_error || lineNumber <= 0 ) return;

    // get error line
    std::string line = g_currentFile.getLine(lineNumber);
    // spaces before caret
    t_CKINT spaces = charNumber - 1;

    // if not empty
    if( line != "" )
    {
        // replace tabs with a single space
        line = replace_tabs( line, ' ' );

        // if line too long, roughly want to display it no further
        if( line.length() > 78 || spaces > 32 )
        {
            // quote the line
            line = snippet( line, 78, 32, spaces );
        }

        // print the line
        CK_FPRINTF_STDERR( "  %s\n  ", line.c_str() );

        // if valid char position to print caret
        if( spaces >= 0 )
        {
            // print spaces
            for( t_CKINT i = 0; i < spaces; i++ ) CK_FPRINTF_STDERR( " " );
            // print caret
            CK_FPRINTF_STDERR( "%s", TC::green("^",TRUE).c_str() );
        }

        // more space
        CK_FPRINTF_STDERR( "\n" );
    }
}




//-----------------------------------------------------------------------------
// name: EM_printLineInCode() | 1.5.0.5 (ge) added
// desc: overloaded version
//-----------------------------------------------------------------------------
void EM_printLineInCode( t_CKINT lineNumber )
{
    EM_printLineInCode( lineNumber, -1 );
}




//-----------------------------------------------------------------------------
// name: EM_error()
// format: [%s]:line(%d).char(%d):
// desc: work horse of compiler and system error reporting
//-----------------------------------------------------------------------------
void EM_error( t_CKINT pos, const char * message, ... )
{
    va_list ap;
    IntList lines = the_linePos;
    t_CKINT num = the_lineNum;
    t_CKINT actualPos = -1;

    if( pos > 0 )
    {
        // search for line containing pos
        while( lines && lines->i >= pos )
        {
            lines = lines->rest;
            num--;
        }
        // calculate actual pos on line
        if( lines ) actualPos = pos - lines->i;
    }

    // debug print
    // cerr << "EM_error(): pos: " << pos << " actualPos:" << actualPos << " the_lineNum: " << the_lineNum << endl;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    // print [chuck]:
    CK_FPRINTF_STDERR( "[%s]:", TC::orange(*the_filename ? mini(the_filename) : "chuck", true).c_str() );
    // buffer the above
    snprintf( g_buffer, CK_ERR_BUF_LENGTH, "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    lastErrorCat( g_buffer );
    
    // if found
    if( pos > 0 && actualPos > 0 )
    {
        // print line(LINE).char(CHAR):
        CK_FPRINTF_STDERR( TC::orange("line(%d).char(%d):", true).c_str(), (int)num, (int)actualPos );
        // buffer the above
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, "line(%d).char(%d):", (int)num, (int)actualPos );
        lastErrorCat( g_buffer );
    }
    // print space
    CK_FPRINTF_STDERR( " " );
    // buffer the above
    lastErrorCat( " " );

    // print the message
    va_start(ap, message);
    CK_VFPRINTF_STDERR( TC::red(message, true).c_str(), ap );
    va_end(ap);
    // buffer the message
    va_start(ap, message);
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end(ap);
    lastErrorCat( g_buffer );

    // endl
    CK_FPRINTF_STDERR( "\n");
    // flush
    CK_FFLUSH_STDERR();

    // print (btw lines could be NULL, for example on an empty program)
    if( actualPos > 0 ) EM_printLineInCode( num, actualPos );
}




//-----------------------------------------------------------------------------
// name: EM_error2()
// format: [%s]:line(%d):
// desc: error output (variant 2, with terminal colors)
//-----------------------------------------------------------------------------
void EM_error2( t_CKINT pos, const char * message, ... )
{
    va_list ap;
    IntList lines = the_linePos;
    t_CKINT line = the_lineNum;
    t_CKINT actualPos = -1;

    // do if pos is positive
    if( pos > 0 )
    {
        while( lines && lines->i >= pos )
        {
            lines = lines->rest;
            line--;
        }

        if( lines )
        {
            actualPos = pos - lines->i;

            // debug print
            // std::cerr << "lineNum: " << line << " lines->i: " << lines->i << " pos: " << pos << std::endl;
        }
    }

    // save
    EM_extLineNum = line;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    // is bold?
    t_CKBOOL bold = true;

    // header
    std::string prefix = *the_filename ? mini(the_filename) : "chuck";
    // print header
    if( *the_filename ) CK_FPRINTF_STDERR( "[%s]:", TC::orange(prefix, bold).c_str() );
    else CK_FPRINTF_STDERR( "[%s]:", prefix.c_str() );
    snprintf( g_buffer, CK_ERR_BUF_LENGTH, "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    lastErrorCat( g_buffer );

    // if given a valid line number
    if( pos )
    {
        CK_FPRINTF_STDERR( TC::orange("line(%d):",bold).c_str(), (int)line );
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, "line(%d):", (int)line );
        lastErrorCat( g_buffer );

        // print error only if there is non-zero line number
        CK_FPRINTF_STDERR( " %s", TC::red("error:", true).c_str() );
        lastErrorCat( " error:" );
    }
    CK_FPRINTF_STDERR( " " );
    lastErrorCat( " " );

    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::orange(message,bold).c_str(), ap );
    va_end( ap );

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();

    // print
    if( pos ) EM_printLineInCode( line, actualPos );
}




//-----------------------------------------------------------------------------
// name: EM_error2b()
// format: [%s]:line(%d):
// desc: error output -- like variant 2, but without color teriminal output
//-----------------------------------------------------------------------------
void EM_error2b( t_CKINT line, const char * message, ... )
{
    va_list ap;

    EM_extLineNum = line;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    snprintf( g_buffer, CK_ERR_BUF_LENGTH, "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    lastErrorCat( g_buffer );

    // if given a valid line number
    if( line > 0 )
    {
        CK_FPRINTF_STDERR( "line(%d):", (int)line );
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, "line(%d):", (int)line );
        lastErrorCat( g_buffer );
    }
    CK_FPRINTF_STDERR( " " );
    lastErrorCat( " " );

    va_start( ap, message );
    CK_VFPRINTF_STDERR( message, ap );
    va_end( ap );

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();

    // print
    EM_printLineInCode( line );
}




//-----------------------------------------------------------------------------
// name: EM_error3()
// desc: prints message, no line number
//-----------------------------------------------------------------------------
void EM_error3( const char * message, ... )
{
    va_list ap;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    g_buffer[0] = '\0';
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::orange(message,true).c_str(), ap );
    va_end( ap );

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_print2vanilla()
// format: like EM_error() minus the error and the line
//-----------------------------------------------------------------------------
void EM_print2vanilla( const char * message, ... )
{
    va_list ap;

    // print
    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    CK_FPRINTF_STDERR( " " );

    // print
    va_start( ap, message );
    CK_VFPRINTF_STDERR( message, ap );
    va_end( ap );

    // flush
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_print2green()
// format: like EM_error() minus the error and the line
//-----------------------------------------------------------------------------
void EM_print2green( const char * message, ... )
{
    va_list ap;

    // print
    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    CK_FPRINTF_STDERR( " " );

    // print
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::green(message,true).c_str(), ap );
    va_end( ap );

    // flush
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_print2blue()
// format: like EM_error() minus the error and the line
//-----------------------------------------------------------------------------
void EM_print2blue( const char * message, ... )
{
    va_list ap;

    // print
    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    CK_FPRINTF_STDERR( " " );

    // print
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::blue(message,true).c_str(), ap );
    va_end( ap );

    // flush
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_print2orange()
// format: like EM_error() minus the error and the line
//-----------------------------------------------------------------------------
void EM_print2orange( const char * message, ... )
{
    va_list ap;

    // print
    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    CK_FPRINTF_STDERR( " " );

    // print
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::orange(message,true).c_str(), ap );
    va_end( ap );

    // flush
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_log()
// desc: output log message at a particular log level
//-----------------------------------------------------------------------------
void EM_log( t_CKINT level, const char * message, ... )
{
    va_list ap;

    if( level > CK_LOG_ALL ) level = CK_LOG_ALL;
    else if( level <= CK_LOG_NONE ) level = CK_LOG_NONE + 1;

    // check level
    if( level > g_loglevel ) return;

    #ifndef __DISABLE_THREADS__
    g_logmutex.acquire();
    #endif

    TC::off();
    CK_FPRINTF_STDERR( "[%s:%s:%s]: ",
                       TC::blue("chuck", true).c_str(),
                       TC::blue( itoa(level), false ).c_str(),
                       TC::blue( g_str[level] ).c_str() );
    TC::on();

    // if( g_logstack ) CK_FPRINTF_STDERR( " " );
    for( int i = 0; i < g_logstack; i++ )
        CK_FPRINTF_STDERR( " | " );

    va_start( ap, message );
    CK_VFPRINTF_STDERR( message, ap );
    va_end( ap );

    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();

    #ifndef __DISABLE_THREADS__
    g_logmutex.release();
    #endif
}




//-----------------------------------------------------------------------------
// name: EM_setlog()
// desc: set log level
//-----------------------------------------------------------------------------
void EM_setlog( t_CKINT level )
{
    if( level > CK_LOG_ALL ) level = CK_LOG_ALL;
    else if( level < CK_LOG_NONE ) level = CK_LOG_NONE;
    g_loglevel = level;

    // log this
    EM_log( CK_LOG_INFO, "setting log level to: %li (%s)...", level, g_str[level] );
}




//-----------------------------------------------------------------------------
// name: EM_pushlog()
// desc: push log indent
//-----------------------------------------------------------------------------
void EM_pushlog()
{
    g_logstack++;
}




//-----------------------------------------------------------------------------
// name: EM_pushlog()
// desc: pop log indent
//-----------------------------------------------------------------------------
void EM_poplog()
{
    g_logstack--;
    if( g_logstack < 0 ) g_logstack = 0;
}




//-----------------------------------------------------------------------------
// name: EM_start_filename()
// desc: prepare new filename before chuck_parse()
//-----------------------------------------------------------------------------
void EM_start_filename( const char * fname )
{
    the_filename = fname ? fname : (c_str)"";
    the_lineNum = 1;
    EM_lineNum = 1;
    EM_extLineNum = 1;

    // free the intList
    IntList curr = NULL;
    while( the_linePos )
    {
        curr = the_linePos;
        the_linePos = the_linePos->rest;
        // free
        free( curr );
    }

    // make new intList
    the_linePos = intList( 0, NULL );
}




//-----------------------------------------------------------------------------
// name: EM_start_filename()
// desc: reset filename, usually after chuck_parse; this affects error output
//-----------------------------------------------------------------------------
void EM_reset_filename()
{
    // set
    the_filename = (c_str)"";
    // more set
    the_lineNum = 0;
    EM_lineNum = 0;
    EM_extLineNum = 0;
}




//-----------------------------------------------------------------------------
// name: EM_lasterror()
// desc: return last error
//-----------------------------------------------------------------------------
const char * EM_lasterror()
{
    return g_lasterror.c_str();
}




//-----------------------------------------------------------------------------
// name: EM_setCurrentFileSource()
// desc: set where current file being compile comes from; used for highlighting
//       code to go with compiler error messages
//-----------------------------------------------------------------------------
void EM_setCurrentFileSource( const CompileFileSource & source )
{
    // set
    g_currentFile = source;
}




//-----------------------------------------------------------------------------
// name: EM_cleanupCurrentFileSource()
// desc: clean up the current file source
//-----------------------------------------------------------------------------
void EM_cleanupCurrentFileSource()
{
    g_currentFile.reset();
}




//-----------------------------------------------------------------------------
// name: getLineFromCode()
// desc: return a specific line from string of actual code
//-----------------------------------------------------------------------------
std::string getLineFromCode( const std::string & code, t_CKUINT n )
{
    // stream stream
    std::istringstream strin( code );
    std::string line;
    t_CKBOOL found = FALSE;
    t_CKUINT theLine = 0;

    // go through line by line
    while( std::getline( strin, line ) )
    {
        theLine++;
        if( theLine == n ) { found = TRUE; break; };
    }

    // make sure
    return found ? line: "";
}




//-----------------------------------------------------------------------------
// name: getLineFromFile()
// desc: return a specific line from a presumed open FILE descriptor
//       this will modify the position of the FILE descriptor; assume that
//       place is not longer needed as this is usually called on error
//-----------------------------------------------------------------------------
std::string getLineFromFile( FILE * fd, t_CKUINT n )
{
    // read lines using POSIX function getline
    // this code won't work on Windows
    char * line = NULL;
    size_t len = 0;
    t_CKBOOL found = FALSE;
    std::string ret;

    // rewind the FILE descriptor
    rewind( fd );

    t_CKUINT theLine = 0;
    // get line; c getline includes the delimiter (e.g., '\n'), unless EOF
    // whereas c++ getline does not...so it goes
    while( ck_getline( &line, &len, fd ) != -1 )
    {
        theLine++;
        if( theLine == n ) { found = TRUE; break; };
    }

    // if we have a match
    if( found )
    {
        // copy
        ret = line;
        t_CKINT i = ret.length()-1;
        // go through any trail endline
        while( ret[i] == '\n' || ret[i] == '\r' ) i--;
        // move one back
        i++;
        // get rid of the trailing delimiter
        if( i >= 0 ) ret[i] = '\0';
    }

    // free
    SAFE_FREE( line );

    // return
    return ret;
}




//-----------------------------------------------------------------------------
// name: getLine()
// desc: get line from source
//-----------------------------------------------------------------------------
std::string CompileFileSource::getLine( t_CKUINT n )
{
    std::string retval = "";

    // which type?
    switch( which )
    {
    case 2:
        retval = getLineFromCode( code, n );
        break;
    case 3:
        retval = getLineFromFile( file, n );
        break;
    default:
        break;
    }

    // empty?
    // if( retval == "" )
    //     CK_FPRINTF_STDERR( "[chuck]: (parser) cannot retrieve source to display!\n" );

    return retval;
}




//-----------------------------------------------------------------------------
// I/O redirect: stdout
//-----------------------------------------------------------------------------
void ck_fprintf_stdout( const char * format, ... )
{
    // evaluate the format string
    va_list args;
    va_start( args, format );
    vsnprintf( g_buffer2, g_buffer2_size, format, args );
    va_end( args );

    // send the formatted string to the buffer
    g_stdout_stream << g_buffer2;

    // try flushing
    ck_fflush_stdout();
}




//-----------------------------------------------------------------------------
// I/O redirect: stderr
//-----------------------------------------------------------------------------
void ck_fprintf_stderr( const char * format, ... )
{
    // evaluate the format string
    va_list args;
    va_start( args, format );
    vsnprintf( g_buffer2, g_buffer2_size, format, args );
    va_end( args );

    // send the formatted string to the buffer
    g_stderr_stream << g_buffer2;

    // try flushing
    ck_fflush_stderr();
}




//-----------------------------------------------------------------------------
// I/O flush stdout
//-----------------------------------------------------------------------------
void ck_fflush_stdout()
{
    // no callback? just flush it
    if( g_stdout_callback == NULL )
    {
        // send to stdout
        fprintf( stdout, "%s", g_stdout_stream.str().c_str() );
        fflush( stdout );

        // and clear buffer
        g_stdout_stream.str( std::string() );
    }
    else
    {
        // yes callback.
        // check if stream contains a newline
        if( g_stdout_stream.str().find( '\n' ) != std::string::npos )
        {
            // if so, emit to callback
            g_stdout_callback( g_stdout_stream.str().c_str() );

            // and clear buffer
            g_stdout_stream.str( std::string() );
        }
    }
}




//-----------------------------------------------------------------------------
// I/O flush stderr
//-----------------------------------------------------------------------------
void ck_fflush_stderr()
{
    // no callback? just flush it
    if( g_stderr_callback == NULL )
    {
        // send to stderr
        fprintf( stderr, "%s", g_stderr_stream.str().c_str() );
        fflush( stderr );

        // and clear buffer
        g_stderr_stream.str( std::string() );
    }
    else
    {
        // yes callback.
        // check if stream contains a newline
        if( g_stderr_stream.str().find( '\n' ) != std::string::npos )
        {
            // if so, emit to callback
            g_stderr_callback( g_stderr_stream.str().c_str() );

            // and clear buffer
            g_stderr_stream.str( std::string() );
        }
    }
}




//-----------------------------------------------------------------------------
// I/O redirect: stdout from format string output
//-----------------------------------------------------------------------------
void ck_vfprintf_stdout( const char * format, va_list args )
{
    // evaluate the format string
    vsnprintf( g_buffer2, g_buffer2_size, format, args );

    // send the formatted string to the buffer
    g_stdout_stream << g_buffer2;

    // try flushing
    ck_fflush_stdout();
}




//-----------------------------------------------------------------------------
// I/O redirect: stderr from format string output
//-----------------------------------------------------------------------------
void ck_vfprintf_stderr( const char * format, va_list args )
{
    // evaluate the format string
    vsnprintf( g_buffer2, g_buffer2_size, format, args );

    // send the formatted string to the buffer
    g_stderr_stream << g_buffer2;

    // try flushing
    ck_fflush_stderr();
}




//-----------------------------------------------------------------------------
// I/O redirect: set stdout callback; this is useful for redirecting output
// from chuck to miniAudicle, Chunity, or any other host system with its
// own way of outputing
//-----------------------------------------------------------------------------
void ck_set_stdout_callback( void (*callback)(const char *) )
{
    g_stdout_callback = callback;
#ifdef __cplusplus
    g_ck_stdoutstream.set_callback( callback );
#endif
}




//-----------------------------------------------------------------------------
// I/O redirect: set stderr callback; this is useful for redirecting output
// from chuck to miniAudicle, Chunity, or any other host system with its
// own way of outputing
//-----------------------------------------------------------------------------
void ck_set_stderr_callback( void (*callback)(const char *) )
{
    g_stderr_callback = callback;
#ifdef __cplusplus
    g_ck_stderrstream.set_callback( callback );
#endif
}




//-----------------------------------------------------------------------------
// implementation for ChuckOutStream | REFACTOR-2017 Jack Atherton
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C++" {

ChuckOutStream::ChuckOutStream( bool isErr )
{
    m_callback = NULL;
    m_isErr = isErr;
}

ChuckOutStream::~ChuckOutStream()
{
}

ChuckOutStream& ChuckOutStream::operator<<( const std::string val )
{
    m_stream << val;
    if( m_isErr || (val == CK_STDENDL) ) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const char * val )
{
    m_stream << val;
    if( m_isErr || std::string(val) == CK_STDENDL ) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<(const double val)
{
    m_stream << val;
    if( m_isErr ) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const float val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const unsigned long long val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const long long val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const unsigned long val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const long val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const unsigned int val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const int val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const bool val )
{
    m_stream << val;
    if (m_isErr) { this->flush(); }
    return *this;
}

void ChuckOutStream::set_callback( void (*callback)( const char * ) )
{
    m_callback = callback;
}

void ChuckOutStream::flush()
{
    if( m_callback != NULL )
    {
        // send to callback
        m_callback( m_stream.str().c_str() );
    }
    else
    {
        // print it
        if( m_isErr )
        {
            // to cerr
            std::cerr << m_stream.str().c_str();
            std::cerr.flush();
        }
        else
        {
            // to cout
            std::cout << m_stream.str().c_str();
            std::cout.flush();
        }
    }
    // clear buffer
    m_stream.str( std::string() );
}

ChuckOutStream g_ck_stdoutstream( FALSE );
ChuckOutStream g_ck_stderrstream( TRUE );
}

#endif
