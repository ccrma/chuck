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
// file: chuck_otf.cpp
// desc: on-the-fly programming utilities
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2004
//-----------------------------------------------------------------------------
#include "chuck_otf.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"
#include "chuck.h"
#include "util_math.h"
#include "util_thread.h"
#include "util_string.h"
#include "util_platforms.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#ifndef __PLATFORM_WINDOWS__
  #include <unistd.h>
  // linux 64-bit byte ordering | 1.5.1.5
  #if defined(__PLATFORM_LINUX__)
    #include <endian.h>
    #ifndef htonll
      #define htonll htobe64
    #endif
    #ifndef ntohll
      #define ntohll be64toh
    #endif
  #endif
#else // 2022 QTSIN
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
#endif

using namespace std;

// log level
t_CKUINT g_otf_log = CK_LOG_INFO;




//-----------------------------------------------------------------------------
// name: otf_hton( )
// desc: host to network endian byte-ordering
//-----------------------------------------------------------------------------
void otf_hton( OTF_Net_Msg * msg )
{
    msg->header = htonll( msg->header );
    msg->type = htonll( msg->type );
    msg->param = htonll( msg->param );
    msg->param2 = htonll( msg->param2 );
    msg->param3 = htonll( msg->param3 );
    msg->length = htonll( msg->length );
}




//-----------------------------------------------------------------------------
// name: otf_ntoh( )
// desc: network to host endian byte-ordering
//-----------------------------------------------------------------------------
void otf_ntoh( OTF_Net_Msg * msg )
{
    msg->header = ntohll( msg->header );
    msg->type = ntohll( msg->type );
    msg->param = ntohll( msg->param );
    msg->param2 = ntohll( msg->param2 );
    msg->param3 = ntohll( msg->param3 );
    msg->length = ntohll( msg->length );
}




//-----------------------------------------------------------------------------
// name: recv_file()
// desc: receive a file over network
//-----------------------------------------------------------------------------
FILE * recv_file( const OTF_Net_Msg & msg, ck_socket sock )
{
    OTF_Net_Msg buf;

    // make a temp file
    FILE * fd = ck_tmpfile();
    // check if successful
    if( !fd )
    {
        EM_error2( 0, "OTF nable to create temp file, skipping..." );
        return NULL;
    }

    do {
        // msg
        if( !ck_recv( sock, (char *)&buf, sizeof(buf) ) ) goto error;
        // network to host byte ordering
        otf_ntoh( &buf );
        // write
        fwrite( buf.buffer, sizeof(char), buf.length, fd );
        // while there is more
    }while( buf.param2 > 0 );

    // check for error
    if( buf.param2 == CK_NET_ERROR )
    {
        // this was a problem
        EM_log( CK_LOG_INFO, "(via otf): received error flag, dropping..." );
        // done
        goto error;
    }

    // return file descriptor
    return fd;

error:
    // close file descriptor
    fclose( fd );
    // zero it out
    fd = NULL;
    return NULL;
}




//-----------------------------------------------------------------------------
// name: otf_process_msg()
// desc: process incoming OTF mes from network
//       FYI previous to 1.5.1.3, this was also called by Machine.add() etc...
//       however, but no longer, as of 1.5.1.3 Machine directly calls in VM,
//       removing dependency on OTF and the socket code
//-----------------------------------------------------------------------------
t_CKUINT otf_process_msg( Chuck_VM * vm, Chuck_Compiler * compiler,
                          OTF_Net_Msg * msg, t_CKBOOL immediate, void * data )
{
    Chuck_Msg * cmd = new Chuck_Msg;
    Chuck_VM_Code * code = NULL;
    FILE * fd = NULL;
    t_CKUINT ret = 0;

    // CK_FPRINTF_STDERR( "UDP message recv...\n" );
    if( msg->type == CK_MSG_ADD || msg->type == CK_MSG_REPLACE )
    {
        string filename;
        vector<string> args;

        // parse out command line arguments
        if( !extract_args( msg->buffer, filename, args ) )
        {
            // error
            EM_error2( 0, "malformed filename with argument list..." );
            EM_error2( 0, "    -->  '%s'", msg->buffer );
            CK_SAFE_DELETE(cmd);
            goto cleanup;
        }

        // copy de-escaped filename
        strcpy( msg->buffer, filename.c_str() );
        // copy args if needed
        if( args.size() > 0 )
        {
            cmd->args = new vector<string>;
            *(cmd->args) = args;
        }

        // see if entire file is on the way
        if( msg->param2 && msg->param2 != CK_NET_ERROR )
        {
            fd = recv_file( *msg, (ck_socket)data );
            if( !fd )
            {
                EM_error2( 0, "incoming source transfer '%s' failed...",
                    mini(msg->buffer) );
                CK_SAFE_DELETE(cmd);
                goto cleanup;
            }
        }

        // construct full path to be associated with the file so me.sourceDir() works
        // (added 1.3.5.2)
        std::string full_path = get_full_path( msg->buffer );

        // construct a target to be compiled | 1.5.4.0 (ge)
        Chuck_CompileTarget * target = new Chuck_CompileTarget();
        // set file descriptor
        target->fd2parse = fd;
        // set fields
        target->filename = msg->buffer;
        target->absolutePath = full_path;

        // parse, type-check, and emit; compiler will memory-manage target (no need to delete here)
        if( !compiler->compile( target ) )
        {
            CK_SAFE_DELETE(cmd);
            goto cleanup;
        }

        // get the code
        code = compiler->output();
        // (re) name it | 1.5.0.5 (ge) update from '+=' to '='
        code->name = string(msg->buffer);

        // set the flags for the command
        cmd->type = msg->type;
        cmd->code = code;
        // copy param
        if( cmd->type == CK_MSG_REPLACE )
            cmd->param = msg->param;

        // interpret param3 | 1.5.1.5 (nshaheed & ge)
        if( msg->param3 )
        {
            // flag this for always-add replace
            cmd->alwaysAdd = TRUE;
            // if add message
            if( msg->type == CK_MSG_ADD )
            {
                // rewrite as an always-add replace message
                cmd->type = CK_MSG_REPLACE;
                // copy the param for replace
                cmd->param = msg->param;
            }
        }
    }
    else if( msg->type == CK_MSG_STATUS || msg->type == CK_MSG_REMOVE || msg->type == CK_MSG_REMOVEALL
             || msg->type == CK_MSG_EXIT || msg->type == CK_MSG_TIME || msg->type == CK_MSG_RESET_ID
             || msg->type == CK_MSG_CLEARVM )
    {
        cmd->type = msg->type;
        cmd->param = msg->param;
    }
    else if( msg->type == CK_MSG_ABORT )
    {
        // halt and clear current shred
        vm->abort_current_shred();
        // short circuit
        ret = 1;
        CK_SAFE_DELETE(cmd);
        goto cleanup;
    }
    else
    {
        EM_error2( 0, "unrecognized incoming command from network: '%li'", cmd->type );
        CK_SAFE_DELETE(cmd);
        goto cleanup;
    }

    // immediate
    if( immediate )
    {
        // process message immediately
        ret = vm->process_msg( cmd );
    }
    else
    {
        // queue message
        vm->queue_msg( cmd, 1 );
        // return code
        ret = 1;
    }

cleanup:
    // close file handle
    // if( fd ) fclose( fd );
    // 1.5.4.1 (ge) not needed, file handle should be cleaned up as part of compile-target

    return ret;
}




//-----------------------------------------------------------------------------
// name: otf_send_file()
// desc: send a chuck program over network
//-----------------------------------------------------------------------------
t_CKINT otf_send_file( const char * fname, OTF_Net_Msg & msg, const char * op,
                       ck_socket dest )
{
    FILE * fd = NULL;
    struct stat fs;
    string filename;
    vector<string> args;
    string buf;
    t_CKUINT amountRead = 0;

    // use this to send | 1.5.1.5
    OTF_Net_Msg msg2send;

    // parse out command line arguments
    if( !extract_args( fname, filename, args ) )
    {
        // error
        EM_error2( 0, "malformed filename + argument list..." );
        EM_error2( 0, "    -->  '%s'", fname );
        return FALSE;
    }

    // filename and any args
    strcpy( msg.buffer, fname );

    // test it
    buf = filename;
    fd = ck_openFileAutoExt( buf, ".ck" );
    if( !fd )
    {
        EM_error2( 0, "cannot open file '%s' for [%s]...", filename.c_str(), op );
        return FALSE;
    }

    // make a target
    Chuck_CompileTarget * target = new Chuck_CompileTarget();
    target->fd2parse = fd;
    target->filename = filename;

    // set current target
    EM_setCurrentTarget( target );

    // check to see if at least parses
    if( !chuck_parse( target ) )
    {
        // error message
        EM_error2( 0, "(parse error) skipping file '%s' for [%s]...", filename.c_str(), op );
        // reset parser (clean up) | 1.5.1.5
        reset_parse();
        // clean up target, including closing the fd
        CK_SAFE_DELETE( target );
        return FALSE;
    }
    // reset parser (clean up) | 1.5.1.5
    reset_parse();

    // stat it
    memset( &fs, 0, sizeof(fs) );
    stat( buf.c_str(), &fs );
    fseek( fd, 0, SEEK_SET );

    // log
    EM_log( CK_LOG_INFO, "sending TCP file %s, size=%d", filename.c_str(), (t_CKUINT)fs.st_size );
    EM_pushlog();

    // send the first packet
    msg.param2 = (t_CKUINT)fs.st_size;
    msg.length = 0;
    // copy it (to preserve the endianness of the original msg)
    msg2send = msg;
    // host to network byte ordering
    otf_hton( &msg2send );
    // go
    ck_send( dest, (char *)&msg2send, sizeof(msg2send) );

    // send the whole thing
    t_CKUINT left = (t_CKUINT)fs.st_size;
    while( left )
    {
        // log
        EM_log( CK_LOG_FINER, "%03d bytes left...", left );
        // amount to send
        msg.length = left > CK_NET_BUFFER_SIZE ? CK_NET_BUFFER_SIZE : left;
        // read
        amountRead = fread( msg.buffer, sizeof(char), msg.length, fd );
        // check for error
        if( !amountRead )
        {
            // error encountered
            EM_error2( 0, "error while reading '%s'...", filename.c_str() );
            // mark done
            left = 0;
            // error flag
            msg.param2 = CK_NET_ERROR;
        }
        else
        {
            // amount left
            left -= amountRead;
            // what's left
            msg.param2 = left;
        }

        // log
        EM_log( CK_LOG_FINER, "sending buffer %03d length %03d...", amountRead, msg.length );
        // copy it (to preserve the endianness of the original msg)
        msg2send = msg;
        // host to network byte order
        otf_hton( &msg2send );
        // send it
        ck_send( dest, (char *)&msg2send, sizeof(msg2send) );
    }

    // pop log
    EM_poplog();

    // clean up target, including closing the fd
    CK_SAFE_DELETE( target );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: otf_send_connect()
// desc: ...
//-----------------------------------------------------------------------------
ck_socket otf_send_connect( const char * host, t_CKINT port )
{
    // log
    EM_log( CK_LOG_INFO, "otf connect: %s:%i", host, port );

    ck_socket sock = ck_tcp_create( 0 );
    if( !sock )
    {
        EM_error2( 0, "cannot open socket to send command..." );
        return NULL;
    }

    if( strcmp( host, "127.0.0.1" ) )
        EM_log( g_otf_log, "connecting to %s on port %i via TCP...", host, port );

    if( !ck_connect( sock, host, (int)port ) )
    {
        EM_error2( 0, "cannot open TCP socket on %s:%i...", host, port );
        ck_close( sock );
        return NULL;
    }

    ck_send_timeout( sock, 0, 2000000 );

    return sock;
}




//-----------------------------------------------------------------------------
// name: otf_send_cmd()
// desc: parse for OTF commands from args and send as OTF messages
//-----------------------------------------------------------------------------
t_CKINT otf_send_cmd( t_CKINT argc, const char ** argv, t_CKINT & i,
                      const char * host, t_CKINT port, t_CKINT * is_otf )
{
    OTF_Net_Msg msg;
    // REFACTOR-2017 TODO: wat is global sigpipe mode
    // g_sigpipe_mode = 1;
    int tasks_total = 0, tasks_done = 0;
    ck_socket dest = NULL;
    if( is_otf ) *is_otf = TRUE;

    // log
    EM_log( CK_LOG_INFO, "examining otf command '%s'...", argv[i] );

    if( !strcmp( argv[i], "--add" ) || !strcmp( argv[i], "+" ) )
    {
        if( ++i >= argc )
        {
            EM_error2( 0, "not enough arguments following [add]..." );
            goto error;
        }

        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        do {
            // log
            EM_log( CK_LOG_INFO, "sending file:args '%s' for ADD...", mini(argv[i]) );
            msg.type = CK_MSG_ADD;
            msg.param = 1;
            tasks_done += otf_send_file( argv[i], msg, "add", dest );
            tasks_total++;
        } while( ++i < argc );
        // log
        EM_poplog();

        if( !tasks_done )
            goto error;
    }
    else if( !strcmp( argv[i], "--remove" ) || !strcmp( argv[i], "-" ) )
    {
        if( ++i >= argc )
        {
            EM_error2( 0, "not enough arguments following [remove]..." );
            goto error;
        }

        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        do {
            // log
            EM_log( CK_LOG_INFO, "requesting REMOVE of shred '%i'..." );
            // set param
            msg.param = ck_atoul( argv[i] );
            // set type
            msg.type = CK_MSG_REMOVE;
            // host to network byte ordering
            otf_hton( &msg );
            ck_send( dest, (char *)&msg, sizeof(msg) );
        } while( ++i < argc );
        // log
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        // log
        EM_log( CK_LOG_INFO, "requesting REMOVE LAST shred..." );
        msg.param = CK_NO_VALUE;
        msg.type = CK_MSG_REMOVE;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        // log
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--replace" ) || !strcmp( argv[i], "=" ) )
    {
        if( ++i >= argc )
        {
            EM_error2( 0, "not enough arguments following [replace]..." );
            goto error;
        }

        if( i <= 0 )
            msg.param = CK_NO_VALUE;
        else
        {
            // arg as c++ string
            string arg = trim(argv[i]);
            // check for '%' | 1.5.1.5 (nshaheed & ge)
            if( arg.length() && arg[0] == '%' )
            {
                // convert into number
                msg.param = ck_atoul( argv[i]+1 );
                // flag this for always add (whether the remove ID is present or not)
                msg.param3 = TRUE;
            }
            else // no '%'
            {
                // convert
                msg.param = ck_atoul( argv[i] );
            }
        }

        if( ++i >= argc )
        {
            EM_error2( 0, "not enough arguments following [replace]..." );
            goto error;
        }

        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting REPLACE shred '%i' with '%s'...", msg.param, mini(argv[i]) );
        msg.type = CK_MSG_REPLACE;
        if( !otf_send_file( argv[i], msg, "replace", dest ) )
            goto error;
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--removeall" ) || !strcmp( argv[i], "--remall" ) || !strcmp( argv[i], "--remove.all" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting REMOVE ALL..." );
        msg.type = CK_MSG_REMOVEALL;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--clear.vm" ) || !strcmp( argv[i], "--clear-vm" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting CLEAR VM..." );
        msg.type = CK_MSG_CLEARVM;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--kill" ) || !strcmp( argv[i], "--exit" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting EXIT..." );
        msg.type = CK_MSG_REMOVEALL;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        msg.type = CK_MSG_EXIT;
        msg.param = (i+1)<argc ? ck_atoul(argv[++i]) : 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--time" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting TIME..." );
        msg.type = CK_MSG_TIME;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--rid" ) || !strcmp( argv[i], "--reset.id" ) || !strcmp( argv[i], "--reset-id" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting RESET ID..." );
        msg.type = CK_MSG_RESET_ID;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--status" ) || !strcmp( argv[i], "^" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting STATUS..." );
        msg.type = CK_MSG_STATUS;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else if( !strcmp( argv[i], "--abort.shred" ) || !strcmp( argv[i], "--abort-shred" ) )
    {
        if( !(dest = otf_send_connect( host, port )) ) return 0;
        EM_pushlog();
        EM_log( CK_LOG_INFO, "requesting ABORT SHRED..." );
        msg.type = CK_MSG_ABORT;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        EM_poplog();
    }
    else
    {
        if( is_otf ) *is_otf = FALSE;
        return 0;
    }

    // send
    msg.type = CK_MSG_DONE;
    otf_hton( &msg );
    // log
    EM_log( CK_LOG_INFO, "otf sending request..." );
    // send done message
    ck_send( dest, (char *)&msg, sizeof(msg) );

    // set timeout
    ck_recv_timeout( dest, 0, 2000000 );
    // log
    EM_log( CK_LOG_INFO, "otf awaiting reply..." );
    // reply
    if( ck_recv( dest, (char *)&msg, sizeof(msg) ) )
    {
        // network to host byte ordering
        otf_ntoh( &msg );
        if( !msg.param )
        {
            EM_error2( 0, "REMOTE oeration failed..." );
            EM_error2( 0, "reason: %s",
                ( strstr( (char *)msg.buffer, ":" ) ? strstr( (char *)msg.buffer, ":" ) + 1 : (char *)msg.buffer ) );
        }
        else
        {
            EM_log( CK_LOG_INFO, "reply received..." );
        }
    }
    else
    {
        EM_error2( 0, "remote operation timed out..." );
    }
    // close the sock
    ck_close( dest );

    // exit
    // exit( msg.param );

    return 1;

error:

    // if sock was opened
    if( dest )
    {
        msg.type = CK_MSG_DONE;
        otf_hton( &msg );
        ck_send( dest, (char *)&msg, sizeof(msg) );
        ck_close( dest );
    }

    // exit( 1 );

    return 0;
}




//-----------------------------------------------------------------------------
// name: otf_recv_cb()
// desc: OTF server thread for receiving OTF message over network
//-----------------------------------------------------------------------------
void * otf_recv_cb( void * p )
{
    OTF_Net_Msg msg;
    OTF_Net_Msg ret;
    ck_socket client;
    int n;

    // REFACTOR-2017: get per-VM carrier
    Chuck_Carrier * carrier = (Chuck_Carrier *)p;

    // catch SIGINT
    // signal( SIGINT, signal_int );
#ifndef __PLATFORM_WINDOWS__
    // catch SIGPIPE
// REFACTOR 2017: TODO register global signal_pipe function
//    signal( SIGPIPE, signal_pipe );
#endif

    while( true )
    {
        // REFACTOR-2017: change g_sock to per-VM socket
        client = ck_accept( carrier->otf_socket );

        // check for thread shutdown, potentially occurred during ck_accept
        if( !carrier->otf_thread )
            break;

        if( !client )
        {
            if( carrier->vm )
                EM_log( CK_LOG_INFO, "[chuck]: socket error during accept()..." );
            ck_usleep( 40000 );
            // ck_close( client );  don't close NULL client
            continue;
        }
        msg.clear();
        // set time out
        ck_recv_timeout( client, 0, 5000000 );
        // receive packet
        n = ck_recv( client, (char *)&msg, sizeof(msg) );
        // network to host byte ordering
        otf_ntoh( &msg );
        // check
        if( n != sizeof(msg) )
        {
            EM_error2( 0, "0-length packet..." );
            ck_usleep( 40000 );
            ck_close( client );
            continue;
        }

        // check header
        if( msg.header != CK_NET_HEADER )
        {
            EM_error2( 0, "header discrepancy: possible client/server version mismatch..." );
            ck_close( client );
            continue;
        }

        // while not done
        while( msg.type != CK_MSG_DONE )
        {
            if( carrier->vm )
            {
                // process incoming OTF message
                if( !otf_process_msg( carrier->vm, carrier->compiler, &msg, FALSE, client ) )
                {
                    // problem
                    ret.param = FALSE;
                    // error message
                    strcpy( (char *)ret.buffer, EM_lasterror() );
                    // drain network data
                    while( msg.type != CK_MSG_DONE && n )
                    {
                        // get next packet
                        n = ck_recv( client, (char *)&msg, sizeof(msg) );
                        // network to host byte ordering (not that it matters here; just draining)
                        otf_ntoh( &msg );
                    }
                    break;
                }
                else
                {
                    // made it
                    ret.param = TRUE;
                    // success code
                    strcpy( (char *)ret.buffer, "success" );
                    // receive next packet (done msg)
                    n = ck_recv( client, (char *)&msg, sizeof(msg) );
                    // network to host byte ordering
                    otf_ntoh( &msg );
                }
            }
            else
            {
                // wait if no VM on the carrier
                ck_usleep( 10000 );
            }
        }

        // host to network byte ordering
        otf_hton( &ret );
        // send return acknoledgement
        ck_send( client, (char *)&ret, sizeof(ret) );
        // close socket to client
        ck_close( client );
    }

    // reach here only if thread shutdown
    return NULL;
}




// we got yer errors, yer problems, yer worst nightmares right here folks
const char * poop[] = {
    "[chuck]: bus error...",
    "[chuck]: segmentation fault, core dumped...",
    "[chuck]: access violation, NULL pointer...",
    "[chuck]: unhandled exception in chuck: 0xC0000005: access violation",
    "[chuck]: malloc: damage after normal block...",
    "[chuck]: virtual machine panic!!!",
    "[chuck]: confused by earlier errors, bailing out...",
    "[chuck]: lack of destructors have led to the unrecoverable mass build-up of trash\n"
    "         the chuck garbage collector will now run, deleting all files (bye.)",
    "[chuck]: calling Machine.crash()...",
    "[chuck]: an unknown fatal error has occurred. please restart your computer...",
    "[chuck]: an unknown fatal error has occurred. please reinstall something...",
    "[chuck]: an unknown fatal error has occurred. please update to chuck-3.0",
    "[chuck]: (internal error) unknown error",
    "[chuck]: (internal error) too many errors!!!",
    "[chuck]: error printing error message. cannot continue 2#%%HGAFf9a0x"
};
// poop size
long poop_size = sizeof( poop ) / sizeof( char * );




//-----------------------------------------------------------------------------
// name: uh()
// desc: ...
//-----------------------------------------------------------------------------
void uh( )
{
    ck_srandom( (unsigned)time( NULL ) );
    while( true )
    {
        int n = (int)(ck_random() / (float)CK_RANDOM_MAX * poop_size);
        printf( "%s\n", poop[n] );
        ck_usleep( (unsigned int)(ck_random() / (float)CK_RANDOM_MAX * 2000000) / 10 );
    }
}
