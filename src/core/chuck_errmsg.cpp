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
// file: errmsg.cpp
// desc: functions used in all phases of the compiler to give error messages
//       based on Andrew Appel's Tiger code
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// based on code by: Andrew Appel (appel@cs.princeton.edu)
// date: Summer 2002
//-----------------------------------------------------------------------------
#include "chuck_errmsg.h"
#include "chuck_symbol.h" // for checked_malloc()
#include "chuck.h" // for access to ChucK container object
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

// current filename
static const char * the_filename = "";
// current line number
static t_CKINT the_lineNum = 1;
// current chuck
static ChucK * the_chuck = NULL;

// a local global string buffer for snprintf
#define CK_ERR_BUF_LENGTH 2048
static char g_buffer[CK_ERR_BUF_LENGTH] = "";
// last error
static std::string g_lasterror = "";
// for code snippet output | 1.5.2.0
static std::string g_codestr = "";
// for output error to str | 1.5.2.0
static std::string g_error2str = "";

// log globals
t_CKINT g_loglevel = CK_LOG_CORE;
t_CKINT g_logstack = 0;
#ifndef __DISABLE_THREADS__
XMutex g_logmutex;
#endif

// more local globals
std::stringstream g_stdout_stream;
std::stringstream g_stderr_stream;
// 1.5.0.5 (ge) increase g_buffer2_size from 1024 to 8192
// to accomodate longer, potentially multiline output strings
static const size_t g_buffer2_size = 8192;
static char g_buffer2[g_buffer2_size] = "";

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
    "HERALD",       // 3 <-- 1.5.1.3 changed from "SEVERE" to "HERALD"
    "WARN!!",       // 4
    "INFORM",       // 5
    "DEBUG!",       // 6 <-- 1.5.0.5 changed from "CONFIG" to "DEBUG!"
    "FINE!!",       // 7
    "FINER!",       // 8
    "FINEST",       // 9
    "ALL!!"           // 10
};




//-----------------------------------------------------------------------------
// name: EM_set_current_chuck()
// set current ChucK; used to query current params
//-----------------------------------------------------------------------------
void EM_set_current_chuck( ChucK * ck )
{
    // set as current chuck
    the_chuck = ck;
}




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
// name: EM_highlight_on_error() | 1.5.0.5 (ge) added
// desc: whether to highligh code on compiler error
//       this defaults to true, but may be disabled when helpful
//       (e.g., to match output for automated testing)
//-----------------------------------------------------------------------------
t_CKBOOL EM_highlight_on_error()
{
    // if we have a chuck reference
    if( the_chuck )
    {
        // update
        g_highlight_on_error = the_chuck->getParamInt( CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR );
    }

    // return
    return g_highlight_on_error;
}




//-----------------------------------------------------------------------------
// name: EM_outputLineInCode() | 1.5.0.5 (ge) added
// desc: output a particular line of code, with optional char pos for caret ^
//-----------------------------------------------------------------------------
const char * EM_outputLineInCode( t_CKINT lineNumber, t_CKINT charNumber )
{
    // clear
    g_codestr = "";

    // check
    if( !EM_highlight_on_error() || lineNumber <= 0 ) return "";

    // get error line
    std::string line = g_currentFile.getLine(lineNumber);

    // detect empty file
    if( lineNumber == 1 && trim(line) == "" )
    {
        return "(empty file)\n";
    }

    // spaces before caret
    t_CKINT spaces = charNumber - 1;

    // if not empty
    if( line != "" )
    {
        // replace tabs with a single space
        line = replace_tabs( line, ' ' );

        // line position
        string posLine = ck_itoa(lineNumber);
        // spaces due to lineNumber + [] + space
        t_CKINT posLineIndent = posLine.length() + 3;

        // get terminal width; -2 for the indentation below
        t_CKUINT WIDTH = ck_ttywidth();
        // check for 0; this one time, emscripten ioctl() ran amok...
        if( WIDTH == 0 ) WIDTH = ck_ttywidth_default();
        // subtract
        WIDTH -= posLineIndent;
        // compute caret pos
        t_CKUINT CARET_POS = (t_CKUINT)(WIDTH/3.0);

        // check for some mininum
        if( WIDTH < posLineIndent + 4 )
        {
            // base defaults
            WIDTH = 80; CARET_POS = 28;
            // check if update
            if( the_chuck )
            {
                // get params stored in chuck; could have been set from host
                WIDTH = the_chuck->getParamInt( CHUCK_PARAM_TTY_WIDTH_HINT );
                CARET_POS = (t_CKUINT)(WIDTH/3.0);
            }
        }
        // any padding on the right
        else{ WIDTH -= 1; CARET_POS -= 1; }

        // if line too long, snippet it
        if( line.length() > WIDTH || spaces > CARET_POS )
        {
            t_CKINT leftTrimmed = 0;
            t_CKINT rightTrimmed = 0;
            // quote the line
            line = snippet( line, WIDTH, CARET_POS, spaces, &leftTrimmed, &rightTrimmed );

            // beyond certain length, replace end(s) with ...
            if( line.length() > 32 )
            {
                // left
                if( leftTrimmed >= 3 ) line.replace( 0, 3, "..." );
                // right
                if( rightTrimmed >= 3 ) line.replace( line.length()-3, 3, "..." );
            }
        }

        // account for lineNumber width + [] + space
        spaces += posLineIndent;

        // print the line with indentation e.g., [5]
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, "[%s] %s\n", TC::blue(posLine,TRUE).c_str(), line.c_str() );
        g_codestr += g_buffer;

        // if valid char position to print caret
        if( spaces >= 0 )
        {
            // print spaces
            for( t_CKINT i = 0; i < spaces; i++ ) g_codestr += " ";
            // print caret
            g_codestr += TC::green("^",TRUE);
        }

        // more space
        g_codestr += "\n";
    }

    return g_codestr.c_str();
}




//-----------------------------------------------------------------------------
// name: EM_printLineInCode() | 1.5.0.5 (ge) added
// desc: output a particular line of code, with optional char pos for caret ^
//-----------------------------------------------------------------------------
void EM_printLineInCode( t_CKINT lineNumber, t_CKINT charNumber )
{
    CK_FPRINTF_STDERR( "%s", EM_outputLineInCode(lineNumber, charNumber) );
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
// name: EM_error_prefix_begin()
// desc: begin the error prefix, e.g, "[chuck" or "[FILE.ck"
//-----------------------------------------------------------------------------
static void EM_error_prefix_begin( t_CKBOOL colorTTY = TRUE, t_CKBOOL bold = FALSE )
{
    // set color TTY mode
    if( colorTTY ) { TC::on(); } else { TC::off(); }

    // check filename
    t_CKBOOL hasFilename = (*the_filename) != '\0';

    // prefix text
    std::string prefix = hasFilename ? mini(the_filename) : "[chuck";

    // print prefix
    if( hasFilename ) CK_FPRINTF_STDERR( "%s", TC::orange(prefix, bold).c_str() );
    else CK_FPRINTF_STDERR( "%s", prefix.c_str() );

    // print to string
    snprintf( g_buffer, CK_ERR_BUF_LENGTH, "%s", prefix.c_str() );
    // concat into last error
    lastErrorCat( g_buffer );

    // if off reenable color TTY
    if( !colorTTY ) TC::on();
}




//-----------------------------------------------------------------------------
// name: EM_error_prefix_end()
// desc: close the error prefix, e.g, "]:"
//-----------------------------------------------------------------------------
static void EM_error_prefix_end( t_CKBOOL colorTTY = TRUE, t_CKBOOL bold = FALSE )
{
    // set color TTY mode
    if( colorTTY ) { TC::on(); } else { TC::off(); }

    // check filename
    t_CKBOOL hasFilename = (*the_filename) != '\0';

    // prefix text
    std::string prefix_end = hasFilename ? TC::orange(": ",bold).c_str() : "]: ";

    // print space
    CK_FPRINTF_STDERR( prefix_end.c_str() );
    // buffer the above
    lastErrorCat( prefix_end.c_str() );

    // if off reenable color TTY
    if( !colorTTY ) TC::on();
}




//-----------------------------------------------------------------------------
// name: EM_error()
// format: [%s:%d:%d]: // was [%s].line(%d).char(%d):
// desc: work horse of compiler and system error reporting
//-----------------------------------------------------------------------------
void EM_error( t_CKINT pos, const char * message, ... )
{
    va_list ap;
    IntList lines = the_linePos;
    t_CKINT line = the_lineNum;
    t_CKINT actualPos = -1;
    t_CKBOOL bold = TRUE;
    // declare each time to pick up any TC state
    string COLON = TC::orange(":",bold);

    if( pos > 0 )
    {
        // search for line containing pos
        while( lines && lines->i >= pos )
        {
            lines = lines->rest;
            line--;
        }
        // calculate actual pos on line
        if( lines ) actualPos = pos - lines->i;
    }

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );
    // debug print
    // cerr << "EM_error(): pos: " << pos << " actualPos:" << actualPos << " the_lineNum: " << the_lineNum << endl;

    // begin prefix, e.g, "FILE.ck" or "[chuck"
    EM_error_prefix_begin( TRUE, bold );
    // if found
    if( pos > 0 && actualPos > 0 )
    {
        CK_FPRINTF_STDERR( "%s%s%s%s", COLON.c_str(), TC::blue(ck_itoa(line),bold).c_str(), COLON.c_str(), TC::green(ck_itoa(actualPos),bold).c_str() );
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, ":%d:%d", (int)line, (int)actualPos );
        lastErrorCat( g_buffer );
    }
    // end prefix, e.g, ": " or "]: "
    EM_error_prefix_end( TRUE, bold );

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

    // print code on error
    if( pos > 0 ) EM_printLineInCode( line, actualPos );
}




//-----------------------------------------------------------------------------
// name: EM_error2()
// format: [%s:%d:%d]: // was [%s]:line(%d):
// desc: error output (variant 2, with terminal colors)
//-----------------------------------------------------------------------------
void EM_error2( t_CKINT pos, const char * message, ... )
{
    va_list ap;
    IntList lines = the_linePos;
    t_CKINT line = the_lineNum;
    t_CKINT actualPos = -1;
    t_CKBOOL bold = TRUE;
    // declare each time to pick up any TC state
    string COLON = TC::orange(":",bold);

    // do if pos is positive
    if( pos > 0 )
    {
        // search for line containing pos
        while( lines && lines->i >= pos )
        {
            lines = lines->rest;
            line--;
        }
        // calculate actual pos on line
        if( lines ) actualPos = pos - lines->i;
    }

    // save
    EM_extLineNum = line;
    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    // begin prefix, e.g, "FILE.ck" or "[chuck"
    EM_error_prefix_begin( TRUE, bold );
    // if given a valid line number
    if( pos )
    {
        CK_FPRINTF_STDERR( "%s%s%s%s", COLON.c_str(), TC::blue(ck_itoa(line),bold).c_str(), COLON.c_str(), TC::green(ck_itoa(actualPos),bold).c_str() );
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, ":%d:%d", (int)line, (int)actualPos );
        lastErrorCat( g_buffer );
    }
    // end prefix, e.g, ": " or "]: "
    EM_error_prefix_end( TRUE, bold );

    if( pos )
    {
        // check for special message types | 1.5.1.3
        const char * msgType = "error: ";
        // copy into c++ string
        string msg = message;
        // look for special strings
        if( msg.find("deprecated: ") == 0 )
        {
            // update msgType
            msgType = "deprecated: ";
            // skip the msgType
            message += string(msgType).length();
        }

        // print error only if there is non-zero line number
        CK_FPRINTF_STDERR( "%s", TC::red(msgType,true).c_str() );
        lastErrorCat( msgType );
    }

    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::orange(message,bold).c_str(), ap );
    va_end( ap );

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();

    // print code on error
    if( pos > 0 ) EM_printLineInCode( line, actualPos );
}




//-----------------------------------------------------------------------------
// name: EM_error2b()
// format: [%s:%d:%d]: // was [%s]:line(%d):
// desc: error output -- like variant 2, but without color teriminal output
//-----------------------------------------------------------------------------
void EM_error2b( t_CKINT line, const char * message, ... )
{
    va_list ap;

    EM_extLineNum = line;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    // begin prefix, e.g, "FILE.ck" or "[chuck"
    EM_error_prefix_begin( FALSE );
    // if given a valid line number
    if( line > 0 )
    {
        CK_FPRINTF_STDERR( ":%d", (int)line );
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, ":%d", (int)line );
        lastErrorCat( g_buffer );
    }
    // end prefix, e.g, ": " or "]: "
    EM_error_prefix_end( FALSE );

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
    if( line > 0 ) EM_printLineInCode( line );
}




//-----------------------------------------------------------------------------
// name: EM_error2str() | 1.5.2.0
// desc: generate error output to string (with terminal colors if enabled)
//-----------------------------------------------------------------------------
const char * EM_error2str( t_CKINT pos, t_CKBOOL outputPrefix, const char * message, ... )
{
    va_list ap;
    IntList lines = the_linePos;
    t_CKINT line = the_lineNum;
    t_CKINT actualPos = -1;
    t_CKBOOL bold = TRUE;
    // declare each time to pick up any TC state
    string COLON = TC::orange(":",bold);
    // reset
    g_error2str = "";

    // do if pos is positive
    if( pos > 0 )
    {
        // search for line containing pos
        while( lines && lines->i >= pos )
        {
            lines = lines->rest;
            line--;
        }
        // calculate actual pos on line
        if( lines ) actualPos = pos - lines->i;
    }

    // begin prefix, e.g, "FILE.ck" or "[chuck"
    t_CKBOOL hasFilename = (*the_filename) != '\0';
    if( outputPrefix ) g_error2str += hasFilename ? mini(the_filename) : "[chuck";
    // if given a valid line number
    if( pos )
    {
        snprintf( g_buffer, CK_ERR_BUF_LENGTH, "%s%s%s%s", COLON.c_str(), TC::blue(ck_itoa(line),bold).c_str(), COLON.c_str(), TC::green(ck_itoa(actualPos),bold).c_str() );
        g_error2str += g_buffer;
    }
    // end prefix, e.g, ": " or "]: "
    if( outputPrefix ) g_error2str += hasFilename ? TC::orange(": ",bold).c_str() : "]: ";

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );
    g_error2str += g_buffer + string("\n");

    // print code on error
    if( pos > 0 ) g_error2str += EM_outputLineInCode( line, actualPos );

    // return
    return g_error2str.c_str();
}




//-----------------------------------------------------------------------------
// name: EM_error3()
// desc: prints message, no prefix, no line number
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
// name: EM_error3b()
// desc: prints message, no prefix, no line number, no color
//-----------------------------------------------------------------------------
void EM_error3b( const char * message, ... )
{
    va_list ap;

    // separate errmsgs with newlines
    if( g_lasterror != "" ) lastErrorCat( "\n" );

    g_buffer[0] = '\0';
    va_start( ap, message );
    CK_VFPRINTF_STDERR( message, ap );
    va_end( ap );

    va_start( ap, message );
    vsnprintf( g_buffer, CK_ERR_BUF_LENGTH, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    CK_FPRINTF_STDERR( "\n" );
    CK_FFLUSH_STDERR();
}




//-----------------------------------------------------------------------------
// name: EM_exception()
// desc: prints exception message
//-----------------------------------------------------------------------------
void EM_exception( const char * message, ... )
{
    va_list ap;

    // preamble
    CK_FPRINTF_STDERR( "[%s]:(%s) ",
                       TC::orange("chuck",true).c_str(),
                       TC::red("EXCEPTION",true).c_str() );

    // actual exception message text
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::orange(message,true).c_str(), ap );
    va_end( ap );

    // end line and flush to be extra sure
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
// name: EM_print2magenta()
// format: like EM_error() minus the error and the line
//-----------------------------------------------------------------------------
void EM_print2magenta( const char * message, ... )
{
    va_list ap;

    // print
    CK_FPRINTF_STDERR( "[%s]:", *the_filename ? mini(the_filename) : "chuck" );
    CK_FPRINTF_STDERR( " " );

    // print
    va_start( ap, message );
    CK_VFPRINTF_STDERR( TC::magenta(message,true).c_str(), ap );
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
                       TC::blue( ck_itoa(level), false ).c_str(),
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
// name: EM_log_opts()
// desc: output log message at a particular log level (no NL)
//-----------------------------------------------------------------------------
void EM_log_opts( t_CKINT level, enum em_LogOpts options, const char * message, ... )
{
    va_list ap;

    if( level > CK_LOG_ALL ) level = CK_LOG_ALL;
    else if( level <= CK_LOG_NONE ) level = CK_LOG_NONE + 1;

    // check level
    if( level > g_loglevel ) return;

    // whether to print prefix
    t_CKBOOL prefix = !(options & EM_LOG_NO_PREFIX);
    // whether to print newline at end
    t_CKBOOL nl = !(options & EM_LOG_NO_NEWLINE);

    #ifndef __DISABLE_THREADS__
    g_logmutex.acquire();
    #endif

    // check option
    if( prefix )
    {
        TC::off();
        CK_FPRINTF_STDERR( "[%s:%s:%s]: ",
                           TC::blue("chuck", true).c_str(),
                           TC::blue( ck_itoa(level), false ).c_str(),
                           TC::blue( g_str[level] ).c_str() );
        TC::on();

        // if( g_logstack ) CK_FPRINTF_STDERR( " " );
        for( int i = 0; i < g_logstack; i++ )
            CK_FPRINTF_STDERR( " | " );
    }

    va_start( ap, message );
    CK_VFPRINTF_STDERR( message, ap );
    va_end( ap );

    // print newline
    if( nl ) CK_FPRINTF_STDERR( "\n" );

    // flush
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
    CK_SAFE_FREE( line );

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

    // NOTE: currently the output size is limitd by g_buffer2_size, which
    // will cut off longer strings (e.g., trying to print a deep stack trace)
    // might look into vasprintf(), which dynamically allocates the
    // final string buffer, accomodates arbitrarily long string; the downside
    // is that vasprintf dynamically allocates memory each time
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

    // NOTE: currently the output size is limitd by g_buffer2_size, which
    // will cut off longer strings (e.g., trying to print a deep stack trace)
    // might look into vasprintf(), which dynamically allocates the
    // final string buffer, accomodates arbitrarily long string; the downside
    // is that vasprintf dynamically allocates memory each time
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

ChuckOutStream& ChuckOutStream::operator<<( const std::string & val )
{
    m_stream << val;
    if( m_isErr || (val == CK_STDENDL)
        /* || val.find("\n") != std::string::npos */ )
    { this->flush(); }
    return *this;
}

ChuckOutStream& ChuckOutStream::operator<<( const char * val )
{
    m_stream << val;
    string s = val;
    if( m_isErr || s == CK_STDENDL
        /* || s.find("\n") != std::string::npos */ )
    { this->flush(); }
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
