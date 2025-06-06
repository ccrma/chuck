/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: util_console.cpp
// desc: utility for console I/O
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#include "util_console.h"
#include "chuck_errmsg.h"

#include <stdio.h>
#ifdef __USE_READLINE__
 #include <readline/readline.h>
#else
 #include <stdlib.h>
 #define CONSOLE_INPUT_BUFFER_SIZE 255
#endif

char * io_readline( const char * prompt )
{
#ifdef __USE_READLINE__

    // call the real readline
    return readline( prompt );

#else

    // insert our hack
    char * buf=(char *)malloc( CONSOLE_INPUT_BUFFER_SIZE * sizeof(char) );
    char * result;

    fputs( prompt, stdout );

    result = fgets( buf, CONSOLE_INPUT_BUFFER_SIZE, stdin );

    if( result == NULL )
    {
        free( buf );
        return NULL;
    }

    for( int i=0; i < CONSOLE_INPUT_BUFFER_SIZE; i++ )
        if(buf[i] == '\n' )
        {
            buf[i] = 0;
            break;
        }

    return buf;

#endif
}

void io_addhistory( const char * addme )
{
#ifdef __USE_READLINE__

    add_history( addme );

#else

    //do nothing

#endif
}


// code thanks to Luke Lin (wdlin@CCCA.NCTU.edu.tw)
// kb hit
#ifndef __PLATFORM_WINDOWS__
  #include <string.h>
  #include <termios.h>
  static struct termios g_save;
  #include <unistd.h>
  #include <sys/ioctl.h>
#else
  #include <conio.h>
#endif


// global
static t_CKINT g_c;
static t_CKBOOL g_init;


// on entering mode
t_CKBOOL kb_initscr()
{
    if( g_init ) return FALSE;

#ifndef __PLATFORM_WINDOWS__
    struct termios term;
    if( tcgetattr(0, &term) == -1 )
    { 
        EM_log( CK_LOG_HERALD, "(kbhit disabled): standard input not a tty!");
        return FALSE;
    }

    // log
    EM_log( CK_LOG_INFO, "starting kb hit immediate mode..." );

    g_save = term;

    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ECHO;

    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME]=0;

    tcsetattr( 0, TCSADRAIN, &term );
#endif

    g_init = TRUE;
    return TRUE;
}


// on exit
void kb_endwin()
{
    if( !g_init ) return;

#ifndef __PLATFORM_WINDOWS__
    tcsetattr( 0, TCSADRAIN, &g_save );
#endif

    g_init = FALSE;
}


// hit
t_CKINT kb_hit()
{
#ifndef __PLATFORM_WINDOWS__
    long ifkeyin;
    char c;
    ifkeyin = read( 0, &c, 1 );
    g_c = (t_CKINT)c;

    // log
    EM_log( CK_LOG_FINE, "kb hit! %i : %c", ifkeyin, c );

    return (ifkeyin);
#else
    return (t_CKINT)kbhit();
#endif
}


// get
t_CKINT kb_getch()
{
#ifndef __PLATFORM_WINDOWS__
    return g_c;
#else
    return (t_CKINT)::getch();
#endif
}


// ready
t_CKBOOL kb_ready()
{
    return g_init;
}

