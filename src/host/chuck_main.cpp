/*----------------------------------------------------------------------------
 ChucK Concurrent, On-the-fly Audio Programming Language
 Compiler and Virtual Machine
 
 Copyright (c) 2003 Ge Wang and Perry R. Cook.  All rights reserved.
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
// file: chuck_main.cpp
// desc: chuck entry point | command line host
//
// v1.3.5.3: migration to chuck_system.* (see for more details)
//
// author: Ge Wang | http://www.gewang.com/
//         (see chuck_system.*)
//-----------------------------------------------------------------------------
#include "chuck.h"
#include "chuck_audio.h"
#include "chuck_console.h"
#include <signal.h>

#if defined(__PLATFORM_WIN32__)
  #include <windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL init_shell( Chuck_Shell * shell, Chuck_Shell_UI * ui, Chuck_VM * vm );
void * shell_cb( void * p );
bool go( int argc, const char ** argv );
void global_cleanup();
void all_stop();
void all_detach();
void usage();
void uh();
t_CKBOOL get_count( const char * arg, t_CKUINT * out );
void cb( t_CKSAMPLE * in, t_CKSAMPLE * out, t_CKUINT numFrames,
         t_CKUINT numInChans, t_CKUINT numOutChans, void * data );

// C functions
extern "C" void signal_int( int sig_num );
extern "C" void signal_pipe( int sig_num );



//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
// the one ChucK, for command line host
ChucK * the_chuck;
// thread id for shell
CHUCK_THREAD g_tid_shell = 0;
// default destination host name
char g_otf_dest[256] = "127.0.0.1";
// otf listener port
t_CKINT g_otf_port = 8888;

// the shell
Chuck_Shell * g_shell = NULL;
// global variables
t_CKUINT g_sigpipe_mode = 0;
// flag for Std.system( string )
t_CKBOOL g_enable_system_cmd = FALSE;
// flag for enabling shell, ge: 1.3.5.3
t_CKBOOL g_enable_shell = FALSE;
// flag for audio enable, ge: 1.3.5.3
t_CKBOOL g_enable_realtime_audio = TRUE;
// added 1.3.2.0 // moved from VM 1.3.5.3
f_mainthreadhook g_main_thread_hook = NULL;
f_mainthreadquit g_main_thread_quit = NULL;
void * g_main_thread_bindle = NULL;
// num vms running; used to quit le_cb thread // added 1.3.6.0
t_CKUINT g_num_vms_running = 0;



// priority stuff
#if defined(__MACOSX_CORE__)
t_CKINT g_priority = 80;
t_CKINT g_priority_low = 60;
#elif defined(__PLATFORM_WIN32__) && !defined(__WINDOWS_PTHREAD__)
t_CKINT g_priority = THREAD_PRIORITY_HIGHEST;
t_CKINT g_priority_low = THREAD_PRIORITY_HIGHEST;
#else
t_CKINT g_priority = 0x7fffffff;
t_CKINT g_priority_low = 0x7fffffff;
#endif




//-----------------------------------------------------------------------------
// name: main()
// desc: ...
//-----------------------------------------------------------------------------
#ifndef __CHUCK_NO_MAIN__
int main( int argc, const char ** argv )
#else
int chuck_main( int argc, const char ** argv )
#endif // __CHUCK_NO_MAIN__
{
    // parse, init, run!
    go( argc, argv );

    // clean up
    global_cleanup();
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: cb()
// desc: audio callback
//-----------------------------------------------------------------------------
void cb( t_CKSAMPLE * in, t_CKSAMPLE * out, t_CKUINT numFrames,
        t_CKUINT numInChans, t_CKUINT numOutChans, void * data )
{
    // TODO: check channel numbers
    
    // call up to ChucK
    the_chuck->run( in, out, numFrames );
}




//-----------------------------------------------------------------------------
// name: version()
// desc: ...
//-----------------------------------------------------------------------------
static void version()
{
    CK_FPRINTF_STDERR( "\n" );
    CK_FPRINTF_STDERR( "chuck version: %s\n", ChucK::version() );
    
    // platform string
    string platform = "";
    
#if defined(__PLATFORM_WIN32__)
    platform = "microsoft win32";
#elif defined(__WINDOWS_DS__)
    platform = "microsoft win32 + cygwin";
#elif defined(__LINUX_ALSA__)
    platform = "linux (alsa)";
#elif defined(__LINUX_OSS__)
    platform = "linux (oss)";
#elif defined(__LINUX_JACK__) || defined(__UNIX_JACK__)
    platform = "linux (jack)";
#elif defined(__LINUX_PULSE__)
    platform = "linux (pulse)";
#elif defined(__MACOSX_UB__)
    platform = "mac os x : universal binary";
#elif defined(__MACOSX_CORE__) && defined(__LITTLE_ENDIAN__)
    platform = "mac os x : intel";
#elif defined(__MACOSX_CORE__)
    platform = "mac os x : powerpc";
#else
    platform = "uh... unknown";
#endif
    
    CK_FPRINTF_STDERR( "   %s : %ld-bit\n", platform.c_str(), machine_intsize() );
    CK_FPRINTF_STDERR( "   http://chuck.cs.princeton.edu/\n" );
    CK_FPRINTF_STDERR( "   http://chuck.stanford.edu/\n\n" );
}



//-----------------------------------------------------------------------------
// name: usage()
// desc: ...
//-----------------------------------------------------------------------------
void usage()
{
    // (note: optional colon added 1.3.0.0)
    CK_FPRINTF_STDERR( "usage: chuck --[options|commands] [+-=^] file1 file2 file3 ...\n" );
    CK_FPRINTF_STDERR( "   [options] = halt|loop|audio|silent|dump|nodump|server|about|probe|\n" );
    CK_FPRINTF_STDERR( "               channels:<N>|out:<N>|in:<N>|dac:<N>|adc:<N>|\n" );
    CK_FPRINTF_STDERR( "               srate:<N>|bufsize:<N>|bufnum:<N>|shell|empty|\n" );
    CK_FPRINTF_STDERR( "               remote:<hostname>|port:<N>|verbose:<N>|level:<N>|\n" );
    CK_FPRINTF_STDERR( "               callback|deprecate:{stop|warn|ignore}|\n" );
    CK_FPRINTF_STDERR( "               chugin-load:{auto|off}|chugin-path:<path>|chugin:<name>\n" );
    CK_FPRINTF_STDERR( "   [commands] = add|remove|replace|remove.all|status|time|kill\n" );
    CK_FPRINTF_STDERR( "   [+-=^] = shortcuts for add, remove, replace, status\n" );
    version();
}




//-----------------------------------------------------------------------------
// name: get_count()
// desc: get the thing after -- or - as number
//-----------------------------------------------------------------------------
t_CKBOOL get_count( const char * arg, t_CKUINT * out )
{
    // no comment
    if( !strncmp( arg, "--", 2 ) ) arg += 2;
    else if( !strncmp( arg, "-", 1 ) ) arg += 1;
    else return FALSE;
    
    // end of string
    if( *arg == '\0' ) return FALSE;
    // not digit
    if( *arg < '0' || *arg > '9' ) return FALSE;
    
    // number
    *out = (t_CKUINT)atoi( arg );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: signal_int()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" void signal_int( int sig_num )
{
    // log
    CK_FPRINTF_STDERR( "[chuck]: cleaning up...\n" );
    // clean up everything!
    global_cleanup();
    // exit with code
    exit( 130 ); // 130 -> terminated by ctrl-c
}




//-----------------------------------------------------------------------------
// name: signal_pipe()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" void signal_pipe( int sig_num )
{
    fprintf( stderr, "[chuck]: sigpipe handled - broken pipe (no connection)...\n" );
    if( g_sigpipe_mode )
    {
        // clean up everything!
        global_cleanup();
        // later
        exit( 2 );
    }
}




//-----------------------------------------------------------------------------
// name: global_cleanup()
// desc: ...
//-----------------------------------------------------------------------------
void global_cleanup()
{
    // REFACTOR-2017: shut down audio system
    all_stop();

    // request MIDI, etc. files be closed
    // REFACTOR-2017: TODO; RESOLVED: used to be all_detach()
    // need to call this before deleting the_chuck/the VM
    ChucK::globalCleanup();
    
    // delete the chuck
    SAFE_DELETE( the_chuck );

    // wait for the shell, if it is running
    // does the VM reset its priority to normal before exiting?
    if( g_enable_shell )
        while( g_shell != NULL )
            usleep(10000);

    // REFACTOR-2017 TODO: Cancel otf, le_cb threads? Does this happen in ~ChucK()?
    // things don't work so good on windows...
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
//    if( g_tid_otf ) pthread_cancel( g_tid_otf );
//    if( g_tid_whatever ) pthread_cancel( g_tid_whatever );
#else
    // close handle
//    if( g_tid_otf ) CloseHandle( g_tid_otf );
#endif
}





//-----------------------------------------------------------------------------
// name: all_stop()
// desc: ...
//-----------------------------------------------------------------------------
void all_stop()
{
    if( g_enable_realtime_audio )
    {
        ChuckAudio::shutdown();
    }
    // REFACTOR-2017: TODO: other things? le_cb?
}




//-----------------------------------------------------------------------------
// name: init_shell()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_shell( Chuck_Shell * shell, Chuck_Shell_UI * ui, Chuck_VM * vm )
{
    // initialize shell UI
    if( !ui->init() )
    {
        CK_FPRINTF_STDERR( "[chuck]: error starting shell UI...\n" );
        return FALSE;
    }
    
    // initialize
    if( !shell->init( vm, ui ) )
    {
        CK_FPRINTF_STDERR( "[chuck]: error starting shell...\n" );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: shell_cb
// desc: thread routine
//-----------------------------------------------------------------------------
void * shell_cb( void * p )
{
    Chuck_Shell * shell;
    // log
    EM_log( CK_LOG_INFO, "starting thread routine for shell..." );
    
    // assuming this is absolutely necessary, an assert may be better
    assert( p != NULL );
    
    shell = ( Chuck_Shell * ) p;
    
    //atexit( wait_for_shell );
    
    // run the shell
    shell->run();
    
    // delete and set to NULL
    SAFE_DELETE( g_shell );
    // perhaps let shell destructor clean up mode and ui?
    
    EM_log( CK_LOG_INFO, "exiting thread routine for shell..." );
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: go()
// desc: parse args, init stuff, run!
//-----------------------------------------------------------------------------
bool go( int argc, const char ** argv )
{
    t_CKBOOL vm_halt = TRUE;
    t_CKINT srate = SAMPLE_RATE_DEFAULT;
    t_CKBOOL force_srate = FALSE; // added 1.3.1.2
    t_CKINT buffer_size = BUFFER_SIZE_DEFAULT;
    t_CKINT num_buffers = NUM_BUFFERS_DEFAULT;
    t_CKINT dac = 0;
    t_CKINT adc = 0;
    std::string dac_name = ""; // added 1.3.0.0
    std::string adc_name = ""; // added 1.3.0.0
    t_CKINT dac_chans = 2;
    t_CKINT adc_chans = 2;
    t_CKBOOL dump = FALSE;
    t_CKBOOL probe = FALSE;
    t_CKBOOL set_priority = FALSE;
    t_CKBOOL auto_depend = FALSE;
    t_CKBOOL block = FALSE;
    // t_CKBOOL enable_shell = FALSE;
    t_CKBOOL syntax_check_only = FALSE;
    t_CKBOOL no_vm = FALSE;
    t_CKBOOL load_hid = FALSE;
    t_CKBOOL enable_server = TRUE;
    t_CKBOOL do_watchdog = TRUE;
    t_CKINT  adaptive_size = 0;
    t_CKINT  log_level = CK_LOG_CORE;
    t_CKINT  deprecate_level = 1; // 1 == warn
    t_CKINT  chugin_load = 1; // 1 == auto (variable added 1.3.0.0)
    // whether to make this new VM the one that receives OTF commands
    t_CKBOOL update_otf_vm = TRUE;
    string   filename = "";
    vector<string> args;
    
    // list of search pathes (added 1.3.0.0)
    std::list<std::string> dl_search_path;
    // initial chug-in path (added 1.3.0.0)
    std::string initial_chugin_path;
    // if set as environment variable (added 1.3.0.0)
    if( getenv( g_chugin_path_envvar ) )
    {
        // get it from the env var
        initial_chugin_path = getenv( g_chugin_path_envvar );
    }
    else
    {
        // default it
        initial_chugin_path = g_default_chugin_path;
    }
    // parse the colon list into STL list (added 1.3.0.0)
    parse_path_list( initial_chugin_path, dl_search_path );
    // list of individually named chug-ins (added 1.3.0.0)
    std::list<std::string> named_dls;
    
#if defined(__DISABLE_WATCHDOG__)
    do_watchdog = FALSE;
#elif defined(__MACOSX_CORE__)
    do_watchdog = TRUE;
#elif defined(__PLATFORM_WIN32__) && !defined(__WINDOWS_PTHREAD__)
    do_watchdog = TRUE;
#else
    do_watchdog = FALSE;
#endif
    
    t_CKUINT files = 0;
    t_CKUINT count = 1;
    t_CKINT i;
    
    // set log level
    EM_setlog( log_level );
    
    // add myself to the list of Chuck_Systems that might need to be cleaned up
    // g_systems.push_back( this );
    
    
    //------------------------- COMMAND LINE ARGUMENTS -----------------------------
    
    // parse command line args
    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '-' || argv[i][0] == '+' ||
           argv[i][0] == '=' || argv[i][0] == '^' || argv[i][0] == '@' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d")
               || !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d") )
                continue;
            else if( get_count( argv[i], &count ) )
                continue;
            else if( !strcmp(argv[i], "--audio") || !strcmp(argv[i], "-a") )
                g_enable_realtime_audio = TRUE;
            else if( !strcmp(argv[i], "--silent") || !strcmp(argv[i], "-s") )
                g_enable_realtime_audio = FALSE;
            else if( !strcmp(argv[i], "--halt") || !strcmp(argv[i], "-t") )
                vm_halt = TRUE;
            else if( !strcmp(argv[i], "--loop") || !strcmp(argv[i], "-l") )
            {   vm_halt = FALSE; enable_server = TRUE; }
            else if( !strcmp(argv[i], "--server") )
                enable_server = TRUE;
            else if( !strcmp(argv[i], "--standalone") )
                enable_server = FALSE;
            else if( !strcmp(argv[i], "--callback") )
                block = FALSE;
            // blocking removed, ge: 1.3.5.3
            // else if( !strcmp(argv[i], "--blocking") )
            //     block = TRUE;
            else if( !strcmp(argv[i], "--syntax") ) {
                syntax_check_only = TRUE;
                g_enable_realtime_audio = FALSE;
            }
            else if( !strcmp(argv[i], "--hid") )
                load_hid = TRUE;
            else if( !strcmp(argv[i], "--shell") || !strcmp( argv[i], "-e" ) )
            {   g_enable_shell = TRUE; vm_halt = FALSE; }
            else if( !strcmp(argv[i], "--empty") )
                no_vm = TRUE;
            else if( !strncmp(argv[i], "--srate:", 8) ) // (added 1.3.0.0)
            {   srate = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : srate; force_srate = TRUE; }
            else if( !strncmp(argv[i], "--srate", 7) )
            {   srate = atoi( argv[i]+7 ) > 0 ? atoi( argv[i]+7 ) : srate; force_srate = TRUE; }
            else if( !strncmp(argv[i], "-r", 2) )
            {   srate = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : srate; force_srate = TRUE; }
            else if( !strncmp(argv[i], "--bufsize:", 10) ) // (added 1.3.0.0)
                buffer_size = atoi( argv[i]+10 ) > 0 ? atoi( argv[i]+10 ) : buffer_size;
            else if( !strncmp(argv[i], "--bufsize", 9) )
                buffer_size = atoi( argv[i]+9 ) > 0 ? atoi( argv[i]+9 ) : buffer_size;
            else if( !strncmp(argv[i], "-b", 2) )
                buffer_size = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : buffer_size;
            else if( !strncmp(argv[i], "--bufnum:", 9) ) // (added 1.3.0.0)
                num_buffers = atoi( argv[i]+9 ) > 0 ? atoi( argv[i]+9 ) : num_buffers;
            else if( !strncmp(argv[i], "--bufnum", 8) )
                num_buffers = atoi( argv[i]+8 ) > 0 ? atoi( argv[i]+8 ) : num_buffers;
            else if( !strncmp(argv[i], "-n", 2) )
                num_buffers = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : num_buffers;
            else if( !strncmp(argv[i], "--dac:", 6) ) // (added 1.3.0.0)
            {
                // advance pointer to beginning of argument
                const char * str = argv[i]+6;
                char * endptr = NULL;
                long dev = strtol(str, &endptr, 10);
                
                // check if arg was a number (added 1.3.0.0)
                if( endptr != NULL && *endptr == '\0' )
                {
                    // successful conversion to # -- clear adc_name
                    dac = dev;
                    dac_name = "";
                }
                // check if arg was not a number and not empty (added 1.3.0.0)
                else if( *str != '\0' )
                {
                    // incomplete conversion to #; see if its a possible device name
                    dac_name = std::string(str);
                }
                else
                {
                    // error
                    CK_FPRINTF_STDERR( "[chuck]: invalid arguments for '--dac:'\n" );
                    CK_FPRINTF_STDERR( "[chuck]: (see 'chuck --help' for more info)\n" );
                    exit( 1 );
                }
            }
            else if( !strncmp(argv[i], "--dac", 5) )
                dac = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--adc:", 6) ) // (added 1.3.0.0)
            {
                // advance pointer to beginning of argument
                const char * str = argv[i]+6;
                char * endptr = NULL;
                long dev = strtol(str, &endptr, 10);
                
                // check if arg was a number (added 1.3.0.0)
                if( endptr != NULL && *endptr == '\0' )
                {
                    // successful conversion to # -- clear adc_name
                    adc = dev;
                    adc_name = "";
                }
                // check if arg was number a number and not empty (added 1.3.0.0)
                else if( *str != '\0' )
                {
                    // incomplete conversion to #; see if its a possible device name
                    adc_name = std::string(str);
                }
                else
                {
                    // error
                    CK_FPRINTF_STDERR( "[chuck]: invalid arguments for '--adc:'\n" );
                    CK_FPRINTF_STDERR( "[chuck]: (see 'chuck --help' for more info)\n" );
                    exit( 1 );
                }
            }
            else if( !strncmp(argv[i], "--adc", 5) )
                adc = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 0;
            else if( !strncmp(argv[i], "--channels:", 11) ) // (added 1.3.0.0)
                dac_chans = adc_chans = atoi( argv[i]+11 ) > 0 ? atoi( argv[i]+11 ) : 2;
            else if( !strncmp(argv[i], "--channels", 10) )
                dac_chans = adc_chans = atoi( argv[i]+10 ) > 0 ? atoi( argv[i]+10 ) : 2;
            else if( !strncmp(argv[i], "-c", 2) )
                dac_chans = adc_chans = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : 2;
            else if( !strncmp(argv[i], "--out:", 6) ) // (added 1.3.0.0)
                dac_chans = atoi( argv[i]+6 ) > 0 ? atoi( argv[i]+6 ) : 2;
            else if( !strncmp(argv[i], "--out", 5) )
                dac_chans = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 2;
            else if( !strncmp(argv[i], "-o", 2) )
                dac_chans = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : 2;
            else if( !strncmp(argv[i], "--in:", 5) ) // (added 1.3.0.0)
                adc_chans = atoi( argv[i]+5 ) > 0 ? atoi( argv[i]+5 ) : 2;
            else if( !strncmp(argv[i], "--in", 4) )
                adc_chans = atoi( argv[i]+4 ) > 0 ? atoi( argv[i]+4 ) : 2;
            else if( !strncmp(argv[i], "-i", 2) )
                adc_chans = atoi( argv[i]+2 ) > 0 ? atoi( argv[i]+2 ) : 2;
            else if( !strncmp(argv[i], "--level:", 8) ) // (added 1.3.0.0)
            {   g_priority = atoi( argv[i]+8 ); set_priority = TRUE; }
            else if( !strncmp(argv[i], "--level", 7) )
            {   g_priority = atoi( argv[i]+7 ); set_priority = TRUE; }
            else if( !strncmp(argv[i], "--watchdog", 10) )
            {   g_watchdog_timeout = atof( argv[i]+10 );
                if( g_watchdog_timeout <= 0 ) g_watchdog_timeout = 0.5;
                do_watchdog = TRUE; }
            else if( !strncmp(argv[i], "--nowatchdog", 12) )
                do_watchdog = FALSE;
            else if( !strncmp(argv[i], "--remote:", 9) ) // (added 1.3.0.0)
                strcpy( g_otf_dest, argv[i]+9 );
            else if( !strncmp(argv[i], "--remote", 8) )
                strcpy( g_otf_dest, argv[i]+8 );
            else if( !strncmp(argv[i], "@", 1) )
                strcpy( g_otf_dest, argv[i]+1 );
            else if( !strncmp(argv[i], "--port:", 7) ) // (added 1.3.0.0)
                g_otf_port = atoi( argv[i]+7 );
            else if( !strncmp(argv[i], "--port", 6) )
                g_otf_port = atoi( argv[i]+6 );
            else if( !strncmp(argv[i], "-p", 2) )
                g_otf_port = atoi( argv[i]+2 );
            else if( !strncmp(argv[i], "--auto", 6) )
                auto_depend = TRUE;
            else if( !strncmp(argv[i], "-u", 2) )
                auto_depend = TRUE;
            else if( !strncmp(argv[i], "--log:", 6) ) // (added 1.3.0.0)
                log_level = argv[i][6] ? atoi( argv[i]+6 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--log", 5) )
                log_level = argv[i][5] ? atoi( argv[i]+5 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--verbose:", 10) ) // (added 1.3.0.0)
                log_level = argv[i][10] ? atoi( argv[i]+10 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--verbose", 9) )
                log_level = argv[i][9] ? atoi( argv[i]+9 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "-v", 2) )
                log_level = argv[i][2] ? atoi( argv[i]+2 ) : CK_LOG_INFO;
            else if( !strncmp(argv[i], "--adaptive:", 11) ) // (added 1.3.0.0)
                adaptive_size = argv[i][11] ? atoi( argv[i]+11 ) : -1;
            else if( !strncmp(argv[i], "--adaptive", 10) )
                adaptive_size = argv[i][10] ? atoi( argv[i]+10 ) : -1;
            else if( !strncmp(argv[i], "--deprecate", 11) )
            {
                // get the rest
                string arg = argv[i]+11;
                if( arg == ":stop" ) deprecate_level = 0;
                else if( arg == ":warn" ) deprecate_level = 1;
                else if( arg == ":ignore" ) deprecate_level = 2;
                else
                {
                    // error
                    CK_FPRINTF_STDERR( "[chuck]: invalid arguments for '--deprecate'...\n" );
                    CK_FPRINTF_STDERR( "[chuck]: ... (looking for :stop, :warn, or :ignore)\n" );
                    exit( 1 );
                }
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "--chugin-load:", sizeof("--chugin-load:")-1) )
            {
                // get the rest
                string arg = argv[i]+sizeof("--chugin-load:")-1;
                if( arg == "off" ) chugin_load = 0;
                else if( arg == "auto" ) chugin_load = 1;
                else
                {
                    // error
                    CK_FPRINTF_STDERR( "[chuck]: invalid arguments for '--chugin-load'...\n" );
                    CK_FPRINTF_STDERR( "[chuck]: ... (looking for :auto or :off)\n" );
                    exit( 1 );
                }
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "--chugin-path:", sizeof("--chugin-path:")-1) )
            {
                // get the rest
                dl_search_path.push_back( argv[i]+sizeof("--chugin-path:")-1 );
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "-G", sizeof("-G")-1) )
            {
                // get the rest
                dl_search_path.push_back( argv[i]+sizeof("-G")-1 );
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "--chugin:", sizeof("--chugin:")-1) )
            {
                named_dls.push_back(argv[i]+sizeof("--chugin:")-1);
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "-g", sizeof("-g")-1) )
            {
                named_dls.push_back(argv[i]+sizeof("-g")-1);
            }
            else if( !strcmp( argv[i], "--no-otf" ) )
            {
                // don't use this new vm for otf commands (use the previous one)
                update_otf_vm = FALSE;
            }
            else if( !strcmp( argv[i], "--probe" ) )
                probe = TRUE;
            else if( !strcmp( argv[i], "--poop" ) )
                uh();
            else if( !strcmp( argv[i], "--caution-to-the-wind" ) )
                g_enable_system_cmd = TRUE;
            else if( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
                    || !strcmp(argv[i], "--about") )
            {
                usage();
                exit( 2 );
            }
            else if( !strcmp( argv[i], "--version" ) )
            {
                version();
                exit( 2 );
            }
            else
            {
                // boost log level
                g_otf_log = CK_LOG_CORE;
                // flag
                int is_otf = FALSE;
                // log level
                EM_setlog( log_level );
                
                // do it
                if( otf_send_cmd( argc, argv, i, g_otf_dest, g_otf_port, &is_otf ) )
                    exit( 0 );
                
                // is otf
                if( is_otf ) exit( 1 );
                
                // done
                CK_FPRINTF_STDERR( "[chuck]: invalid flag '%s'\n", argv[i] );
                usage();
                exit( 1 );
            }
        }
        else // doesn't look like an argument
        {
            // increase number of files
            files++;
        }
    }
    
    // log level
    EM_setlog( log_level );
    
    // probe
    if( probe )
    {
        ChuckAudio::probe();
        
#ifndef __DISABLE_MIDI__
        EM_error2b( 0, "" );
        probeMidiIn();
        EM_error2b( 0, "" );
        probeMidiOut();
        EM_error2b( 0, "" );
#endif  // __DISABLE_MIDI__
        
        HidInManager::probeHidIn();
        
        // exit
        exit( 0 );
    }
    
    // set caution to wind
    ChucK::enableSystemCall = g_enable_system_cmd;
    
    // check buffer size
    buffer_size = ensurepow2( buffer_size );
    // check mode and blocking
    if( !g_enable_realtime_audio && !block ) block = TRUE;
    // audio, boost
    if( !set_priority && !block ) g_priority = g_priority_low;
    if( !set_priority && !g_enable_realtime_audio ) g_priority = 0x7fffffff;
    // set priority
    XThreadUtil::our_priority = g_priority;
    // set watchdog
    g_do_watchdog = do_watchdog;
    // set adaptive size
    if( adaptive_size < 0 ) adaptive_size = buffer_size;
    
    if( !files && vm_halt && !g_enable_shell )
    {
        CK_FPRINTF_STDERR( "[chuck]: no input files... (try --help)\n" );
        exit( 1 );
    }
    
    // shell initialization without vm
    if( g_enable_shell && no_vm )
    {
        // instantiate
        g_shell = new Chuck_Shell;
        // initialize
        if( !init_shell( g_shell, new Chuck_Console(), NULL ) )
            exit( 1 );
        // no vm is needed, just start running the shell now
        g_shell->run();
        // clean up
        SAFE_DELETE( g_shell );
        // done
        exit( 0 );
    }
    
    // make sure vm
    if( no_vm )
    {
        CK_FPRINTF_STDERR( "[chuck]: '--empty' can only be used with shell...\n" );
        exit( 1 );
    }
    
    // find dac_name if appropriate (added 1.3.0.0)
    if( dac_name.size() > 0 )
    {
        // check with RtAudio
        int dev = ChuckAudio::device_named( dac_name, TRUE, FALSE );
        if( dev >= 0 )
        {
            dac = dev;
        }
        else
        {
            CK_FPRINTF_STDERR( "[chuck]: unable to find dac '%s'...\n", dac_name.c_str() );
            CK_FPRINTF_STDERR( "[chuck]: | (try --probe to enumerate audio device info)\n" );
            exit( 1 );
        }
    }
    
    // find adc_name if appropriate (added 1.3.0.0)
    if( adc_name.size() > 0 )
    {
        // check with RtAudio
        int dev = ChuckAudio::device_named( adc_name, FALSE, TRUE );
        if( dev >= 0 )
        {
            adc = dev;
        }
        else
        {
            CK_FPRINTF_STDERR( "[chuck]: unable to find adc '%s'...\n", adc_name.c_str() );
            CK_FPRINTF_STDERR( "[chuck]: | (try --probe to enumerate audio device info)\n" );
            exit( 1 );
        }
    }
    
    //------------------------- VIRTUAL MACHINE SETUP -----------------------------
    // instantiate ChucK
    the_chuck = new ChucK();
    
    // set params
    the_chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, srate );
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, adc_chans );
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, dac_chans );
    the_chuck->setParam( CHUCK_PARAM_VM_ADAPTIVE, adaptive_size );
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT)(vm_halt) );
    the_chuck->setParam( CHUCK_PARAM_OTF_PORT, g_otf_port );
    the_chuck->setParam( CHUCK_PARAM_OTF_ENABLE, (t_CKINT)TRUE );
    the_chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)dump );
    the_chuck->setParam( CHUCK_PARAM_AUTO_DEPEND, (t_CKINT)auto_depend );
    the_chuck->setParam( CHUCK_PARAM_DEPRECATE_LEVEL, deprecate_level );
    the_chuck->setParam( CHUCK_PARAM_USER_CHUGINS, named_dls );
    the_chuck->setParam( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, dl_search_path );
    // set hint, so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_HINT_IS_REALTIME_AUDIO, g_enable_realtime_audio );
    the_chuck->setLogLevel( log_level );
    
    // initialize
    if( !the_chuck->init() )
    {
        CK_FPRINTF_STDERR( "[chuck]: failed to initialize...\n" );
        exit( 1 );
    }
    
    //--------------------------- AUDIO I/O SETUP ---------------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "initializing audio I/O..." );
    // push
    EM_pushlog();
    // log
    EM_log( CK_LOG_SYSTEM, "probing '%s' audio subsystem...", g_enable_realtime_audio ? "real-time" : "fake-time" );
    
    // initialize audio system
    if( g_enable_realtime_audio )
    {
        // TODO: refactor initialize() to take in the dac and adc nums
        ChuckAudio::m_adc_n = adc;
        ChuckAudio::m_dac_n = dac;
        t_CKBOOL retval = ChuckAudio::initialize( dac_chans, adc_chans,
            srate, buffer_size, num_buffers, cb, (void *)the_chuck, force_srate );
        // check
        if( !retval )
        {
            EM_log( CK_LOG_SYSTEM,
                   "cannot initialize audio device (use --silent/-s for non-realtime)" );
            // pop
            EM_poplog();
            // done
            exit( 1 );
        }
    }
    
    // log
    EM_log( CK_LOG_SYSTEM, "real-time audio: %s", g_enable_realtime_audio ? "YES" : "NO" );
    EM_log( CK_LOG_SYSTEM, "mode: %s", block ? "BLOCKING" : "CALLBACK" );
    EM_log( CK_LOG_SYSTEM, "sample rate: %ld", srate );
    EM_log( CK_LOG_SYSTEM, "buffer size: %ld", buffer_size );
    if( g_enable_realtime_audio )
    {
        EM_log( CK_LOG_SYSTEM, "num buffers: %ld", num_buffers );
        EM_log( CK_LOG_SYSTEM, "adc: %ld dac: %d", adc, dac );
        EM_log( CK_LOG_SYSTEM, "adaptive block processing: %ld", adaptive_size > 1 ? adaptive_size : 0 );
    }
    EM_log( CK_LOG_SYSTEM, "channels in: %ld out: %ld", adc_chans, dac_chans );
    
    // pop
    EM_poplog();

#ifndef __ALTER_HID__
    // pre-load hid
    if( load_hid ) HidInManager::init();
#endif // __ALTER_HID__
    
    // catch SIGINT
    signal( SIGINT, signal_int );
#ifndef __PLATFORM_WIN32__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif
    
    // shell initialization
    if( g_enable_shell )
    {
        // instantiate
        g_shell = new Chuck_Shell;
        // initialize
        if( !init_shell( g_shell, new Chuck_Console(), the_chuck->vm() ) )
            exit( 1 );
    }

    // reset count
    count = 1;
    
    // log
    EM_log( CK_LOG_SEVERE, "starting compilation..." );
    // push indent
    EM_pushlog();
    
    
    //------------------------- SOURCE COMPILATION --------------------------------
    
    // loop through and process each file
    for( i = 1; i < argc; i++ )
    {
        // make sure
        if( argv[i][0] == '-' || argv[i][0] == '+' )
        {
            if( !strcmp(argv[i], "--dump") || !strcmp(argv[i], "+d" ) )
                the_chuck->compiler()->emitter->dump = TRUE;
            else if( !strcmp(argv[i], "--nodump") || !strcmp(argv[i], "-d" ) )
                the_chuck->compiler()->emitter->dump = FALSE;
            else
                get_count( argv[i], &count );
            
            continue;
        }
        
        // compile it!
        the_chuck->compileFile( argv[i], "", count );
        
        // reset count
        count = 1;
    }
    
    // pop indent
    EM_poplog();
    
    if( syntax_check_only )
        return TRUE;

    // boost priority
    if( XThreadUtil::our_priority != 0x7fffffff )
    {
        // try
        if( !XThreadUtil::set_priority( XThreadUtil::our_priority ) )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: error setting thread priority...\n" );
            exit( 1 );
        }
    }
    
    
    //----------------------- SHELL SETUP -----------------------------
    
    // start shell on separate thread | REFACTOR-2017: per-VM?!?
    if( g_enable_shell )
    {
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
        pthread_create( &g_tid_shell, NULL, shell_cb, g_shell );
#else
        g_tid_shell = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)shell_cb, g_shell, 0, 0 );
#endif
    }
    
    //-------------------------- MAIN CHUCK LOOP!!! -----------------------------
    
    // log
    EM_log( CK_LOG_SYSTEM, "running main loop..." );
    // push indent
    EM_pushlog();
    
    // start it!
    the_chuck->start();
    
    // log
    EM_log( CK_LOG_SEVERE, "virtual machine running..." );
    // pop indent
    EM_poplog();
    
    // silent mode buffers
    SAMPLE * input = new SAMPLE[buffer_size*adc_chans];
    SAMPLE * output = new SAMPLE[buffer_size*dac_chans];
    // zero out
    memset( input, 0, sizeof(SAMPLE)*buffer_size*adc_chans );
    memset( output, 0, sizeof(SAMPLE)*buffer_size*dac_chans );
    
    // start audio
    if( g_enable_realtime_audio )
    {
        ChuckAudio::start();
    }
    
    // wait
    while( the_chuck->vmrunning() )
    {
        // real-time audio
        if( g_enable_realtime_audio )
        {
            Chuck_DL_MainThreadHook * hook = the_chuck->getMainThreadHook();
            if (hook)
                hook->m_hook(hook->m_bindle);
            else
                usleep( 10000 );
        }
        else // silent mode
        {
            // keep running as fast as possible
            the_chuck->run( input, output, buffer_size );
        }
    }
    
    Chuck_DL_MainThreadHook * hook = the_chuck->getMainThreadHook();
    if (hook)
        hook->m_quit(hook->m_bindle);

    return TRUE;
}

