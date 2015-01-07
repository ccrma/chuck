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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "chuck_utils.h"
#include "chuck_errmsg.h"
#include "util_thread.h"


// global
int EM_tokPos = 0;
int EM_lineNum = 1;
int EM_extLineNum = 1;
t_CKBOOL anyErrors= FALSE;

// local global
static const char * fileName = "";
static int lineNum = 1;
static char g_buffer[1024] = "";
static const size_t LASTERROR_SIZE = 1024;
static char g_lasterror[LASTERROR_SIZE] = "[chuck]: (no error)";
static size_t g_lasterrorIndex = strlen(g_lasterror);
// log globals
int g_loglevel = CK_LOG_CORE;
int g_logstack = 0;
XMutex g_logmutex;

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


// intList
typedef struct intList {int i; struct intList *rest;} *IntList;
static IntList linePos=NULL;


static int lastErrorCat(const char * str)
{
    assert(g_lasterrorIndex <= LASTERROR_SIZE-1);
    
    size_t len = strlen(str);
    
    strncat(g_lasterror, str, LASTERROR_SIZE-g_lasterrorIndex-1);
    
    size_t appendCount = ck_min(LASTERROR_SIZE-g_lasterrorIndex-1, len);
    g_lasterrorIndex += appendCount;
    
    return appendCount > 0;
}


// constructor
static IntList intList( int i, IntList rest )
{
    IntList l = (IntList)checked_malloc(sizeof *l);
    l->i=i; l->rest=rest;
    return l;
}


// new line in lexer
void EM_newline(void)
{
    lineNum++;
    EM_lineNum++;
    EM_extLineNum++;
    linePos = intList(EM_tokPos, linePos);
}


// content after rightmost '/'
const char * mini( const char * str )
{
    int len = strlen( str );
    const char * p = str + len;
    while( p != str && *p != '/' && *p != '\\' ) p--;
    return ( p == str || strlen(p+1) == 0 ? p : p+1 );
}


// content after 'struct'
const char * mini_type( const char * str )
{
    if( !strncmp( str, "struct ", 7 ) ) str += 7;
    const char * p = str;
    while( *p && *p >= '0' && *p <= '9' ) p++;
    return p;
}

void EM_reset_msg()
{
    g_lasterror[0] = '\0';
    g_lasterrorIndex = 0;
}

// [%s]:line(%d).char(%d): 
void EM_error( int pos, const char * message, ... )
{
    va_list ap;
    IntList lines = linePos;
    int num = lineNum;

    anyErrors = TRUE;
    while( lines && lines->i >= pos ) 
    {
        lines = lines->rest;
        num--;
    }
    
    // separate errmsgs with newlines
    if( g_lasterror[0] != '\0' ) lastErrorCat( "\n" );
    
    fprintf( stderr, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    sprintf( g_buffer, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    lastErrorCat( g_buffer );
    if(lines)
    {
        fprintf(stderr, "line(%d).char(%d):", num, pos-lines->i );
        sprintf( g_buffer, "line(%d).char(%d):", num, pos-lines->i );
        lastErrorCat( g_buffer );
    }
    fprintf(stderr, " " );
    lastErrorCat( " " );
    
    va_start(ap, message);
    vfprintf(stderr, message, ap);
    va_end(ap);

    va_start(ap, message);
    vsprintf( g_buffer, message, ap );
    va_end(ap);
    
    fprintf(stderr, "\n");
    fflush( stderr );
    lastErrorCat( g_buffer );
}


// [%s]:line(%d):
void EM_error2( int line, const char * message, ... )
{
    va_list ap;

    EM_extLineNum = line;

    // separate errmsgs with newlines
    if( g_lasterror[0] != '\0' ) lastErrorCat( "\n" );
    
    fprintf( stderr, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    sprintf( g_buffer, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    lastErrorCat( g_buffer );
    if(line)
    {
        fprintf( stderr, "line(%d):", line );
        sprintf( g_buffer, "line(%d):", line );
        lastErrorCat( g_buffer );
    }
    fprintf( stderr, " " );
    lastErrorCat( " " );

    va_start( ap, message );
    vfprintf( stderr, message, ap );
    va_end( ap );

    va_start( ap, message );
    vsprintf( g_buffer, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    fprintf( stderr, "\n" );
    fflush( stderr );
}


// [%s]:line(%d):
void EM_error2b( int line, const char * message, ... )
{
    va_list ap;
    
    EM_extLineNum = line;

    // separate errmsgs with newlines
    if( g_lasterror[0] != '\0' ) lastErrorCat( "\n" );
    
    fprintf( stderr, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    sprintf( g_buffer, "[%s]:", *fileName ? mini(fileName) : "chuck" );
    lastErrorCat( g_buffer );
    if(line)
    {
        fprintf( stderr, "line(%d):", line );
        sprintf( g_buffer, "line(%d):", line );
        lastErrorCat( g_buffer );
    }
    fprintf( stderr, " " );
    lastErrorCat( " " );

    va_start( ap, message );
    vfprintf( stderr, message, ap );
    va_end( ap );

    va_start( ap, message );
    vsprintf( g_buffer, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    fprintf( stdout, "\n" );
    fflush( stdout );
}


// 
void EM_error3( const char * message, ... )
{
    va_list ap;
    
    // separate errmsgs with newlines
    if( g_lasterror[0] != '\0' ) lastErrorCat( "\n" );
    
//    g_lasterror[0] = '\0';
    g_buffer[0] = '\0';

    va_start( ap, message );
    vfprintf( stderr, message, ap );
    va_end( ap );

    va_start( ap, message );
    vsprintf( g_buffer, message, ap );
    va_end( ap );

    lastErrorCat( g_buffer );
    fprintf( stderr, "\n" );
    fflush( stderr );
}


// log
void EM_log( int level, const char * message, ... )
{
    va_list ap;

    if( level > CK_LOG_CRAZY ) level = CK_LOG_CRAZY;
    else if( level <= CK_LOG_NONE ) level = CK_LOG_NONE + 1;

    // check level
    if( level > g_loglevel ) return;

    g_logmutex.acquire();
    fprintf( stderr, "[chuck]:" );
    fprintf( stderr, "(%i:%s): ", level, g_str[level] );

    // if( g_logstack ) fprintf( stderr, " " );
    for( int i = 0; i < g_logstack; i++ )
        fprintf( stderr, " | " );

    va_start( ap, message );
    vfprintf( stderr, message, ap );
    va_end( ap );

    fprintf( stderr, "\n" );
    fflush( stderr );
    g_logmutex.release();
}


// set log level
void EM_setlog( int level )
{
    if( level > CK_LOG_CRAZY ) level = CK_LOG_CRAZY;
    else if( level < CK_LOG_NONE ) level = CK_LOG_NONE;
    g_loglevel = level;

    // log this
    EM_log( CK_LOG_SYSTEM, "setting log level to: %i (%s)...", level, g_str[level] );
}

// push log
void EM_pushlog()
{
    g_logstack++;
}

// pop log
void EM_poplog()
{
    g_logstack--;
    if( g_logstack < 0 ) g_logstack = 0;
}

// prepare new file
t_CKBOOL EM_reset( const char * fname, FILE * fd )
{
    anyErrors = FALSE;
    fileName = fname ? fname : (c_str)"";
    lineNum = 1;
    EM_lineNum = 1;
    EM_extLineNum = 1;

    // free the intList
    IntList curr = NULL;
    while( linePos )
    {
        curr = linePos;
        linePos = linePos->rest;
        // free
        free( curr );
    }

    // make new intList
    linePos = intList( 0, NULL );

    return TRUE;
}


// change file
void EM_change_file( const char * fname )
{
    // set
    fileName = fname ? fname : (c_str)"";
    // more set
    lineNum = 0;
    EM_lineNum = 0;
    EM_extLineNum = 0;
}


// return last error
const char * EM_lasterror()
{
    return g_lasterror;
}
