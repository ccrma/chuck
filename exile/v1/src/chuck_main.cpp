/*----------------------------------------------------------------------------
    ChucK Concurrent, On-the-fly Audio Programming Language
      Compiler and Virtual Machine

    Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
      http://chuck.cs.princeton.edu/
      http://soundlab.cs.princeton.edu/

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
// file: chuck_main.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <sys/stat.h>

#include <fstream>
using namespace std;

extern "C" int yyparse( void );

#include "chuck_type.h"
#include "chuck_emit.h"
#include "chuck_instr.h"
#include "chuck_vm.h"
#include "chuck_bbq.h"
#include "chuck_utils.h"
#include "chuck_errmsg.h"
#include "util_network.h"

#include <signal.h>
#ifndef __PLATFORM_WIN32__
#define CHUCK_THREAD pthread_t
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else 
#define CHUCK_THREAD HANDLE
#endif

#include "ugen_osc.h"
#include "ugen_xxx.h"
#include "ugen_filter.h"
#include "ugen_stk.h"
#include "ulib_machine.h"
#include "ulib_math.h"
#include "ulib_net.h"
#include "ulib_std.h"

// current version
#define CK_VERSION "1.1.5.6d"


#ifdef __PLATFORM_WIN32__
// 'C' specification necessary for windows to link properly
extern "C" a_Program g_program;
#else
extern a_Program g_program;
#endif

ck_socket g_sock;
Chuck_VM * g_vm = NULL;
t_Env g_env = NULL;
t_CKBOOL g_error = FALSE;
CHUCK_THREAD g_tid = 0;
char g_host[256] = "127.0.0.1";
int g_port = 8888;
#if defined(__MACOSX_CORE__)
  t_CKINT g_priority = 95;
#elif defined(__WINDOWS_DS__)
  t_CKINT g_priority = 0;
#else
  t_CKINT g_priority = 0x7fffffff;
#endif




//-----------------------------------------------------------------------------
// name: signal_int()
// desc: ...
//-----------------------------------------------------------------------------
void signal_int( int sig_num )
{
    if( g_vm )
    {
        Chuck_VM * vm = g_vm;
        g_vm = NULL;
        fprintf( stderr, "[chuck]: cleaning up...\n" );
        vm->stop();
        stk_detach( 0, NULL );
#ifndef __PLATFORM_WIN32__
        // pthread_kill( g_tid, 2 );
        if( g_tid ) pthread_cancel( g_tid );
        if( g_tid ) usleep( 50000 );
        delete( vm );
#else
        CloseHandle( g_tid );
#endif
        ck_close( g_sock );
    }

#ifndef __PLATFORM_WIN32__
//    pthread_join( g_tid, NULL );
#endif
    
    exit(2);
}




t_CKUINT g_sigpipe_mode = 0;
//-----------------------------------------------------------------------------
// name: signal_pipe()
// desc: ...
//-----------------------------------------------------------------------------
void signal_pipe( int sig_num )
{
    fprintf( stderr, "[chuck]: sigpipe handled - broken pipe (no connection)...\n" );
    if( g_sigpipe_mode ) exit( 1 );
}




//-----------------------------------------------------------------------------
// name: open_cat()
// desc: ...
//-----------------------------------------------------------------------------
FILE * open_cat( c_str fname )
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




char filename[1024] = "";
//-----------------------------------------------------------------------------
// name: parse()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL parse( c_str fname, FILE * fd = NULL )
{
    t_CKBOOL ret = FALSE;
    strcpy( filename, fname );

    // test it
    if( !fd )
    {
        fd = open_cat( filename );
        if( !fd ) strcpy( filename, fname );
    }

    // parse
    ret = EM_reset( filename, fd );
    if( ret == FALSE ) return FALSE;
    // TODO: clean g_program
    g_program = NULL;
    ret = (yyparse( ) == 0);

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_check()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_check( t_Env env, a_Program prog )
{
    t_CKBOOL ret = FALSE;

    type_engine_begin( env );
    ret = type_engine_check_prog( env, g_program );

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_code( Chuck_Emmission * emit, t_Env env, a_Program prog )
{
    t_CKBOOL ret = FALSE;

    ret = emit_engine_emit_prog( emit, g_program );
    type_engine_end( env );

    return ret;
}




//-----------------------------------------------------------------------------
// name: dump_instr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL dump_instr( Chuck_VM_Code * code )
{
    fprintf( stderr, "[chuck]: dumping src/shred '%s'...\n", code->name.c_str() );
    fprintf( stderr, "...\n" );

    for( unsigned int i = 0; i < code->num_instr; i++ )
        fprintf( stderr, "'%i' %s( %s )\n", i, 
            code->instr[i]->name(), code->instr[i]->params() );

    fprintf( stderr, "...\n\n" );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_module()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_module( t_Env env, f_ck_query query, const char * name, const char * nspc )
{
    Chuck_DLL * dll = NULL;
    
    // load osc
    dll = new Chuck_DLL( name );
    dll->load( query );
    if( !dll->query() || !type_engine_add_dll( env, dll, nspc ) )
    {
        fprintf( stderr, 
                 "[chuck]: internal error loading internal module '%s.%s'...\n", 
                 nspc, name );
        if( !dll->query() )
            fprintf( stderr, "       %s\n", dll->last_error() );
        g_error = TRUE;
        exit(1);
    }    

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
void usage()
{
    fprintf( stderr, "usage: chuck --[options|commands] [+-=^] file1 file2 file3 ...\n" );
    fprintf( stderr, "   [options] = halt|loop|audio|silent|dump|nodump|about|\n" );
    fprintf( stderr, "               srate<N>|bufsize<N>|bufnum<N>|dac<N>|adc<N>|\n" );
    fprintf( stderr, "               remote<hostname>|port<N>\n" );
    fprintf( stderr, "   [commands] = add|remove|replace|status|time|kill\n" );
    fprintf( stderr, "   [+-=^] = shortcuts for add, remove, replace, status\n\n" );
    fprintf( stderr, "chuck version: %s\n", CK_VERSION );
    fprintf( stderr, "   http://chuck.cs.princeton.edu/\n\n" );
}




//-----------------------------------------------------------------------------
// name: send_file()
// desc: ...
//-----------------------------------------------------------------------------
int send_file( const char * filename, Net_Msg & msg, const char * op )
{
    FILE * fd = NULL;
    struct stat fs;
    
    strcpy( msg.buffer, "" );
    //if( filename[0] != '/' )
    //{ 
    //    strcpy( msg.buffer, getenv("PWD") ? getenv("PWD") : "" );
    //    strcat( msg.buffer, getenv("PWD") ? "/" : "" );
    //}
    strcat( msg.buffer, filename );

    // test it
    fd = open_cat( (char *)msg.buffer );
    if( !fd )
    {
        fprintf( stderr, "[chuck]: cannot open file '%s' for [%s]...\n", filename, op );
        return FALSE;
    }
            
    if( !parse( (char *)msg.buffer, fd ) )
    {
        fprintf( stderr, "[chuck]: skipping file '%s' for [%s]...\n", filename, op );
        fclose( fd );
        return FALSE;
    }
            
    // stat it
    stat( msg.buffer, &fs );
    fseek( fd, 0, SEEK_SET );

    //fprintf(stderr, "sending TCP file %s\n", msg.buffer );
    // send the first packet
    msg.param2 = (t_CKUINT)fs.st_size;
    msg.length = 0;
    otf_hton( &msg );
    ck_send( g_sock, (char *)&msg, sizeof(msg) );

    // send the whole thing
    t_CKUINT left = (t_CKUINT)fs.st_size;
    while( left )
    {
        //fprintf(stderr,"file %03d bytes left ... ", left);
        // amount to send
        msg.length = left > NET_BUFFER_SIZE ? NET_BUFFER_SIZE : left;
        // read
        msg.param3 = fread( msg.buffer, sizeof(char), msg.length, fd );
        // amount left
        left -= msg.param3 ? msg.param3 : 0;
        msg.param2 = left;
        //fprintf(stderr, "sending fread %03d length %03d...\n", msg.param3, msg.length );
        // send it
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    
    // close
    fclose( fd );
    //fprintf(stderr, "done.\n", msg.buffer );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: recv_file()
// desc: ...
//-----------------------------------------------------------------------------
FILE * recv_file( const Net_Msg & msg, ck_socket sock )
{
    Net_Msg buf;
    
    // what is left
    t_CKUINT left = msg.param2;
    // make a temp file
    FILE * fd = tmpfile();

    do {
        // msg
        if( !ck_recv( sock, (char *)&buf, sizeof(buf) ) )
            goto error;
        otf_ntoh( &buf );
        // write
        fwrite( buf.buffer, sizeof(char), buf.length, fd );
    }while( buf.param2 );
    
    return fd;

error:
    fclose( fd );
    fd = NULL;
    return NULL;
}




//-----------------------------------------------------------------------------
// name: process_msg()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" t_CKUINT process_msg( Net_Msg * msg, t_CKBOOL immediate, void * data )
{
    Chuck_Msg * cmd = new Chuck_Msg;
    FILE * fd = NULL;
    
    // fprintf( stderr, "UDP message recv...\n" );
    if( msg->type == MSG_REPLACE || msg->type == MSG_ADD )
    {
        // see if entire file is on the way
        if( msg->param2 )
        {
            fd = recv_file( *msg, (ck_socket)data );
            if( !fd )
            {
                fprintf( stderr, "[chuck]: incoming source transfer '%s' failed...\n",
                    mini(msg->buffer) );
                return 0;
            }
        }
        
        // parse
        if( !parse( msg->buffer, fd ) )
            return 0;

        // type check
        if( !type_check( g_env, g_program ) )
            return 0;

        // emit
        Chuck_Emmission * emit = emit_engine_init( g_env );
        if( !emit_code( emit, g_env, g_program ) )
            return 0;

        // transform the code
        Chuck_VM_Code * code = emit_to_code( emit );
        code->name = msg->buffer;
        cmd->shred = new Chuck_VM_Shred;
        cmd->shred->initialize( code );
        cmd->shred->name = code->name;
        emit_engine_addr_map( emit, cmd->shred );
        emit_engine_resolve();
        emit_engine_shutdown( emit );

        // set the flags for the command
        cmd->type = msg->type;
        cmd->code = code;
        if( msg->type == MSG_REPLACE )
            cmd->param = msg->param;
    }
    else if( msg->type == MSG_STATUS || msg->type == MSG_REMOVE || msg->type == MSG_REMOVEALL
             || msg->type == MSG_KILL || msg->type == MSG_TIME )
    {
        cmd->type = msg->type;
        cmd->param = msg->param;
    }
    else
    {
        fprintf( stderr, "[chuck]: unrecognized incoming command from network: '%i'\n", cmd->type );
        SAFE_DELETE(cmd);
        return 0;
    }
    
    // immediate
    if( immediate )
        return g_vm->process_msg( cmd );

    g_vm->queue_msg( cmd, 1 );

    return 1;
}




//-----------------------------------------------------------------------------
// name: load_internal_modules()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_internal_modules( t_Env env )
{
    // load
    load_module( env, osc_query, "osc", "global" );
    load_module( env, xxx_query, "xxx", "global" );
    load_module( env, filter_query, "filter", "global" );
    load_module( env, stk_query, "stk", "global" );

    // load
    load_module( env, machine_query, "machine", "machine" );
    machine_init( g_vm, process_msg );
    load_module( env, libstd_query, "std", "std" );
    load_module( env, libmath_query, "math", "math" );
    load_module( env, net_query, "net", "net" );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: timer()
// desc: ...
//-----------------------------------------------------------------------------
void * timer( void * p )
{
    if ( p ) { 
        t_CKUINT t = *(t_CKUINT *)p;
        usleep( t );
    }
    fprintf( stderr, "[chuck]: operation timed out...\n" );
    exit(1);
}




//-----------------------------------------------------------------------------
// name: cb()
// desc: ...
//-----------------------------------------------------------------------------
void * cb( void * p )
{
    Net_Msg msg;
    Net_Msg ret;
    ck_socket client;
    int n;

#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif

    while( true )
    {
        client = ck_accept( g_sock );
        if( !client )
        {
            if( g_vm ) fprintf( stderr, "[chuck]: socket error during accept()...\n" );
            usleep( 40000 );
            ck_close( client );
            continue;
        }
        msg.clear();
        // set time out
        ck_recv_timeout( client, 0, 5000000 );
        n = ck_recv( client, (char *)&msg, sizeof(msg) );
        otf_ntoh( &msg );
        if( n != sizeof(msg) )
        {
            fprintf( stderr, "[chuck]: 0-length packet...\n", (int)client );
            usleep( 40000 );
            ck_close( client );
            continue;
        }
        
        if( msg.header != NET_HEADER )
        {
            fprintf( stderr, "[chuck]: header mismatch - possible endian lunacy...\n" );
            ck_close( client );
            continue;
        }

        while( msg.type != MSG_DONE )
        {
            if( g_vm )
            {
                if( !process_msg( &msg, FALSE, client ) )
                {
                    ret.param = FALSE;
                    strcpy( (char *)ret.buffer, EM_lasterror() );
                    while( msg.type != MSG_DONE && n )
                    {
                        n = ck_recv( client, (char *)&msg, sizeof(msg) );
                        otf_ntoh( &msg );
                    }
                    break;
                }
                else
                {
                    ret.param = TRUE;
                    strcpy( (char *)ret.buffer, "success" );
                    n = ck_recv( client, (char *)&msg, sizeof(msg) );
                    otf_ntoh( &msg );
                }
            }
        }
        
        otf_hton( &ret );
        ck_send( client, (char *)&ret, sizeof(ret) );
        ck_close( client );
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: send_connect()
// desc: ...
//-----------------------------------------------------------------------------
int send_connect()
{
    g_sock = ck_tcp_create( 0 );
    if( !g_sock )
    {
        fprintf( stderr, "[chuck]: cannot open socket to send command...\n" );
        return FALSE;
    }

    if( strcmp( g_host, "127.0.0.1" ) )
        fprintf( stderr, "[chuck]: connecting to %s on port %i via TCP...\n", g_host, g_port );
    
    if( !ck_connect( g_sock, g_host, g_port ) )
    {
        fprintf( stderr, "[chuck]: cannot open TCP socket on %s:%i...\n", g_host, g_port );
        return FALSE;
    }
    
    ck_send_timeout( g_sock, 0, 2000000 );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: send_cmd()
// desc: ...
//-----------------------------------------------------------------------------
int send_cmd( int argc, char ** argv, int  & i )
{
    Net_Msg msg;
    g_sigpipe_mode = 1;
    int ret = 0;
    int tasks_total = 0, tasks_done = 0;
	
    if( !strcmp( argv[i], "--add" ) || !strcmp( argv[i], "+" ) )
    {
        if( ++i >= argc )
        {
            fprintf( stderr, "[chuck]: not enough arguments following [add]...\n" );
            goto error;
        }

        if( !send_connect() ) return 0;
        do {
            msg.type = MSG_ADD;
            msg.param = 1;
            tasks_done += send_file( argv[i], msg, "add" );
            tasks_total++;
        } while( ++i < argc );
        
        if( !tasks_done )
            goto error;
    }
    else if( !strcmp( argv[i], "--remove" ) || !strcmp( argv[i], "-" ) )
    {
        if( ++i >= argc )
        {
            fprintf( stderr, "[chuck]: not enough arguments following [remove]...\n" );
            goto error;
        }

        if( !send_connect() ) return 0;
        do {
            msg.param = atoi( argv[i] );
            msg.type = MSG_REMOVE;
            otf_hton( &msg );
            ck_send( g_sock, (char *)&msg, sizeof(msg) );
        } while( ++i < argc );
    }
    else if( !strcmp( argv[i], "--" ) )
    {
        if( !send_connect() ) return 0;
        msg.param = 0xffffffff;
        msg.type = MSG_REMOVE;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    else if( !strcmp( argv[i], "--replace" ) || !strcmp( argv[i], "=" ) )
    {
        if( ++i >= argc )
        {
            fprintf( stderr, "[chuck]: not enough arguments following [replace]...\n" );
            goto error;
        }

        if( i <= 0 )
            msg.param = 0xffffffff;
        else
            msg.param = atoi( argv[i] );

        if( ++i >= argc )
        {
            fprintf( stderr, "[chuck]: not enough arguments following [replace]...\n" );
            goto error;
        }

        if( !send_connect() ) return 0;
        msg.type = MSG_REPLACE;
        if( !send_file( argv[i], msg, "replace" ) )
            goto error;
    }
    else if( !strcmp( argv[i], "--removeall" ) || !strcmp( argv[i], "--remall" ) )
    {
        if( !send_connect() ) return 0;
        msg.type = MSG_REMOVEALL;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    else if( !strcmp( argv[i], "--kill" ) )
    {
        if( !send_connect() ) return 0;
        msg.type = MSG_REMOVEALL;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
        msg.type = MSG_KILL;
        msg.param = (i+1)<argc ? atoi(argv[++i]) : 0;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    else if( !strcmp( argv[i], "--time" ) )
    {
        if( !send_connect() ) return 0;
        msg.type = MSG_TIME;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    else if( !strcmp( argv[i], "--status" ) || !strcmp( argv[i], "^" ) )
    {
        if( !send_connect() ) return 0;
        msg.type = MSG_STATUS;
        msg.param = 0;
        otf_hton( &msg );
        ck_send( g_sock, (char *)&msg, sizeof(msg) );
    }
    else
        return 0;
        
    // send
    msg.type = MSG_DONE;
    otf_hton( &msg );
    ck_send( g_sock, (char *)&msg, sizeof(msg) );

    // set timeout
    ck_recv_timeout( g_sock, 0, 2000000 );
    // reply
    if( ck_recv( g_sock, (char *)&msg, sizeof(msg) ) )
    {
        otf_ntoh( &msg );
        fprintf( stderr, "[chuck(remote)]: operation %s\n", ( msg.param ? "successful" : "failed (sorry)" ) );
        if( !msg.param )
            fprintf( stderr, "(reason): %s\n", 
                ( strstr( (char *)msg.buffer, ":" ) ? strstr( (char *)msg.buffer, ":" ) + 1 : (char *)msg.buffer ) ) ;
    }
    else
    {
        fprintf( stderr, "[chuck]: remote operation timed out...\n" );
    }
    // close the sock
    ck_close( g_sock );
    
    // exit
    exit( msg.param );

    return 1;
    
error:
    msg.type = MSG_DONE;
    otf_hton( &msg );
    ck_send( g_sock, (char *)&msg, sizeof(msg) );
    ck_close( g_sock );
    
    exit( 1 );

    return 0;
}



//-----------------------------------------------------------------------------
// name: next_power_2()
// desc: ...
// thanks: to Niklas Werner / music-dsp
//-----------------------------------------------------------------------------
t_CKUINT next_power_2( t_CKUINT n )
{
    t_CKUINT nn = n;
    for( ; n &= n-1; nn = n );
    return nn * 2;
}




//-----------------------------------------------------------------------------
// name: main()
// desc: ...
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    int i, a, files = 0;
    t_CKBOOL enable_audio = TRUE;
    t_CKBOOL vm_halt = TRUE;
    t_CKUINT srate = SAMPLING_RATE_DEFAULT;
    t_CKUINT buffer_size = BUFFER_SIZE_DEFAULT;
    t_CKUINT num_buffers = 8;
    t_CKUINT dac = 0;
    t_CKUINT adc = 0;
    t_CKBOOL set_priority = FALSE;

    // catch SIGINT
    signal( SIGINT, signal_int );
#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif

    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' || argv[i][0] == '+' ||
            argv[i][0] == '=' || argv[i][0] == '^' || argv[i][0] == '@' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d")
                || !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d") )
                continue;
            else if( !strcmp(argv[i], "--audio") || !strcmp(argv[i], "-a") )
                enable_audio = TRUE;
            else if( !strcmp(argv[i], "--silent") || !strcmp(argv[i], "-s") )
                enable_audio = FALSE;
            else if( !strcmp(argv[i], "--halt") || !strcmp(argv[i], "-t") )
                vm_halt = TRUE;
            else if( !strcmp(argv[i], "--loop") || !strcmp(argv[i], "-l") )
                vm_halt = FALSE;
            else if( !strncmp(argv[i], "--srate", 7) )
                srate = atoi( argv[i]+7 ) > 0 ? atoi( argv[i]+7 ) : srate;
            else if( !strncmp(argv[i], "-r", 2) )
                srate = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : srate;
            else if( !strncmp(argv[i], "--bufsize", 9) )
                buffer_size = atoi( argv[i]+9 ) > 0 ? atoi( argv[i]+9 ) : buffer_size;
            else if( !strncmp(argv[i], "-b", 2) )
                buffer_size = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : buffer_size;
            else if( !strncmp(argv[i], "--bufnum", 8) )
                num_buffers = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : num_buffers;
            else if( !strncmp(argv[i], "-n", 2) )
                num_buffers = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : num_buffers;
            else if( !strncmp(argv[i], "--dac", 5) )
                dac = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--adc", 5) )
                adc = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--level", 7) )
            {   g_priority = atoi( argv[i]+7 ); set_priority = TRUE; }
            else if( !strncmp(argv[i], "--remote", 8) )
                strcpy( g_host, argv[i]+8 );
            else if( !strncmp(argv[i], "@", 1) )
                strcpy( g_host, argv[i]+1 );
            else if( !strncmp(argv[i], "--port", 6) )
                g_port = atoi( argv[i]+6 );
            else if( !strncmp(argv[i], "-p", 2) )
                g_port = atoi( argv[i]+2 );
            else if( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
                 || !strcmp(argv[i], "--about") )
            {
                usage();
                exit( 2 );
            }
            else if( a = send_cmd( argc, argv, i ) )
                exit( 0 );
            else
            {
                fprintf( stderr, "[chuck]: invalid flag '%s'\n", argv[i] );
                usage();
                exit( 1 );
            }
        }
        else
            files++;
    }
    
    // check buffer size
    buffer_size = next_power_2( buffer_size-1 );
    // if audio then boost priority
    if( !set_priority && !enable_audio ) g_priority = 0x7fffffff;
    // set priority
    Chuck_VM::our_priority = g_priority;

    if ( !files && vm_halt )
    {
        fprintf( stderr, "[chuck]: no input files... (try --help)\n" );
        exit( 1 );
    }

    // allocate the vm
    Chuck_VM * vm = g_vm = new Chuck_VM;
    if( !vm->initialize( enable_audio, vm_halt, srate, buffer_size,
                         num_buffers, dac, adc, g_priority ) )
    {
        fprintf( stderr, "[chuck]: %s\n", vm->last_error() );
        exit( 1 );
    }

    // allocate the type system
    g_env = type_engine_init( vm );
    // set the env
    vm->set_env( g_env );
    // load modules
    if( !load_internal_modules( g_env ) || g_error )
        exit( 1 );
    
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;
    t_CKBOOL dump = FALSE;
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' || argv[i][0] == '+' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d" ) )
                dump = TRUE;
            else if( !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d" ) )
                dump = FALSE;
            
            continue;
        }
    
        // parse
        if( !parse( argv[i] ) )
            return 1;

        // type check
        if( !type_check( g_env, g_program ) )
            return 1;

        // emit
        Chuck_Emmission * emit = emit_engine_init( g_env );
        if( !emit_code( emit, g_env, g_program ) )
            return 1;

        // transform the code
        code = emit_to_code( emit, dump );
        code->name = argv[i];

        // dump
        if( dump ) dump_instr( code );

        // spork it
        shred = vm->spork( code, NULL );
        
        // mem map it
        emit_engine_addr_map( emit, shred );

        // link local
        emit_engine_resolve( );
    
        // cleanup the emitter
        emit_engine_shutdown( emit );
    }

    // link
    // emit_engine_resolve_globals();
    
    // start tcp server
    g_sock = ck_tcp_create( 1 );
    if( !g_sock || !ck_bind( g_sock, g_port ) || !ck_listen( g_sock, 10 ) )
    {
        fprintf( stderr, "[chuck]: cannot bind to tcp port %i...\n", g_port );
        ck_close( g_sock );
        g_sock = NULL;
    }
    else
    {
#ifndef __PLATFORM_WIN32__
        pthread_create( &g_tid, NULL, cb, NULL );
#else
        g_tid = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cb, NULL, 0, 0);
#endif
    }

    // run the vm
    vm->run();

    // done - clean up
    vm->shutdown();
    usleep( 50000 );
    delete( vm );
    g_vm = NULL;
#ifndef __PLATFORM_WIN32__
    pthread_kill( g_tid, 2 );
#else
    CloseHandle( g_tid );
#endif
    usleep( 100000 );
    ck_close( g_sock );
        
    return 0;
}
