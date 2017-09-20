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
// file: chuck_parse.cpp
// desc: chuck parser interface (using flex and bison)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original in chuck_main.cpp
//       Autumn 2005 - this file
//-----------------------------------------------------------------------------
#include "chuck_parse.h"
#include "chuck_errmsg.h"
#include <string.h>
using namespace std;


// global
static char g_filename[1024] = "";

// external
extern "C" { 
    extern FILE *yyin;
}




//-----------------------------------------------------------------------------
// name: open_cat_ck()
// desc: ...
//-----------------------------------------------------------------------------
FILE * open_cat_ck( c_str fname )
{
    FILE * fd = NULL;
    if( !(fd = fopen( fname, "rb" )) )
        if( !strstr( fname, ".ck" ) && !strstr( fname, ".CK" ) )
        {
            strcat( fname, ".ck" );
            fd = fopen( fname, "rb" );
        }
    return fd;
}



//-----------------------------------------------------------------------------
// name: win32_tmpfile()
// desc: replacement for broken tmpfile() on Windows Vista + 7
//-----------------------------------------------------------------------------
#ifdef __PLATFORM_WIN32__

#include <windows.h>

FILE *win32_tmpfile()
{
    char tmp_path[MAX_PATH];
    char file_path[MAX_PATH];
    FILE * file = NULL;
    
    if(GetTempPath(256, tmp_path) == 0)
        return NULL;
    
    if(GetTempFileName(tmp_path, "mA", 0, file_path) == 0)
        return NULL;
    
    file = fopen(file_path, "wb+D");
    
    return file;
}

#endif



//-----------------------------------------------------------------------------
// name: chuck_parse()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL chuck_parse( c_constr fname, FILE * fd, c_constr code )
{
	t_CKBOOL clo = FALSE;
    t_CKBOOL ret = FALSE;

    // sanity check
    if( fd && code )
    {
        CK_FPRINTF_STDERR( "[chuck](via parser): (internal) both fd and code specified!\n" );
        return FALSE;
    }

    // prepare code
    if( code )
    {
        // !
        assert( fd == NULL );
        // generate temp file
#ifdef __PLATFORM_WIN32__
        fd = win32_tmpfile();
#else
        fd = tmpfile();
#endif
        // flag it to close
        clo = TRUE;
        // write
        fwrite( code, sizeof(char), strlen(code), fd );
    }

    /*
    // use code from memory buffer if its available
    if( code )
    {
        // copy name
        strcpy( g_filename, fname );
        // reset
        if( EM_reset( g_filename, NULL ) == FALSE ) goto cleanup;

        // TODO: clean g_program
        g_program = NULL;

        // clean
        yyrestart( NULL );

        // load string (yy_scan_string will copy the C string)
        YY_BUFFER_STATE ybs = yy_scan_string( code );
        if( !ybs ) goto cleanup;

        // parse
        if( !( yyparse() == 0 ) ) goto cleanup;
        
        // delete the lexer buffer
        yy_delete_buffer( ybs );

    }
    */

    // remember filename
    strcpy( g_filename, fname );
        
    // test it
    if( !fd ) {
        fd = open_cat_ck( g_filename );
        if( !fd ) strcpy( g_filename, fname );
        else clo = TRUE;
    }

    // reset
    if( EM_reset( g_filename, fd ) == FALSE ) goto cleanup;

    // lexer/parser
    // TODO: if( yyin ) { fclose( yyin ); yyin = NULL; }

    // if no fd, open
    if( !fd ) { fd = fopen( g_filename, "r" ); if( fd ) clo = TRUE; }
    // if still none
    if( !fd ) { EM_error2( 0, "no such file or directory" ); goto cleanup; }
    // set to beginning
    else fseek( fd, 0, SEEK_SET );

    // reset yyin to fd
    yyrestart( fd );
        
    // check
    if( yyin == NULL ) goto cleanup;
        
    // TODO: clean g_program
    g_program = NULL;
        
    // parse
    if( !(yyparse( ) == 0) ) goto cleanup;

    // flag success
    ret = TRUE;

cleanup:

    // done
	if( clo ) fclose( fd );

    return ret;
}




//------------------------------------------------------------------------------
// name: reset_parse()
// desc: ...
//------------------------------------------------------------------------------
void reset_parse( )
{
    // empty file name
    EM_change_file( NULL );
}




//-----------------------------------------------------------------------------
// name: parseLine()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL SyntaxQuery::parseLine( const std::string & line, SyntaxTokenList & tokens )
{
    // clear the token list
    tokens.howmany = 0;

    return TRUE;
}
