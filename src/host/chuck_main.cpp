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
#include "util_platforms.h"
#include "util_string.h"
#include <signal.h>

#if defined(__PLATFORM_WINDOWS__)
  #include <windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif

using namespace std;




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL init_shell( Chuck_Shell * shell, Chuck_Shell_UI * ui, Chuck_VM * vm );
void * shell_cb( void * p );
t_CKBOOL go( int argc, const char ** argv );
t_CKBOOL global_cleanup();
void all_stop();
void all_detach();
void usage();
void uh();
t_CKBOOL get_count( const char * arg, t_CKUINT * out );
void cb( SAMPLE * in, SAMPLE * out, t_CKUINT numFrames,
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
// shell shutdown flag
t_CKBOOL g_shutdown_shell = FALSE;
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
#if defined(__PLATFORM_APPLE__)
t_CKINT g_priority = XThreadUtil::get_max_priority(); // was 80
t_CKINT g_priority_low = XThreadUtil::get_min_priority(); // was 60
#elif defined(__PLATFORM_WINDOWS__) && !defined(__WINDOWS_PTHREAD__)
t_CKINT g_priority = THREAD_PRIORITY_HIGHEST;
t_CKINT g_priority_low = THREAD_PRIORITY_HIGHEST;
#else
t_CKINT g_priority = 0x7fffffff;
t_CKINT g_priority_low = 0x7fffffff;
#endif




//-----------------------------------------------------------------------------
// name: main()
// desc: command line chuck entry point
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
    if( !global_cleanup() )
    {
        // we got here possibly because global_cleanup() is underway on
        // another thread; wait a bit and give actual cleaner a chance to
        // finish | 1.5.0.4 (ge) added
        ck_usleep( 50000 );
    }
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: cb()
// desc: audio callback
//-----------------------------------------------------------------------------
void cb( SAMPLE * in, SAMPLE * out, t_CKUINT numFrames,
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
    CK_FPRINTF_STDERR( "%s", TC::reset().c_str() );
    CK_FPRINTF_STDERR( "chuck version: %s\n", ChucK::version() );
    
    // platform string
    string platform = "";
    // drivers string
    string drivers = "";
    // binary architecture string;
    string archs = "";
    // print space
    t_CKINT space = 0;

#if defined(__PLATFORM_WINDOWS__)
    platform = "Microsoft Windows";
#elif defined(__PLATFORM_LINUX__)
    platform = "Linux";
#elif defined(__PLATFORM_APPLE__)
    platform = "macOS";
#elif defined(__PLATFORM_EMSCRIPTEN__)
    platform = "web";
#endif

// windows related
#if defined(__WINDOWS_ASIO__)
    drivers += string( space ? " | " : "" ) + "ASIO"; space++;
#endif
#if defined(__WINDOWS_DS__)
    drivers += string(space ? " | " : "") + "DirectSound"; space++;
#endif
#if defined(__WINDOWS_WASAPI__)
    drivers += string(space ? " | " : "") + "WASAPI"; space++;
#endif

// mac
#if defined(__MACOSX_CORE__)
    drivers += string( space ? " | " : "" ) + "CoreAudio"; space++;
#endif

// linux related
#if defined(__LINUX_ALSA__)
    drivers += string(space ? " | " : "") + "ALSA"; space++;
#endif
#if defined(__LINUX_OSS__)
    drivers += string(space ? " | " : "") + "OSS"; space++;
#endif
#if defined(__LINUX_PULSE__)
    drivers += string(space ? " | " : "") + "PulseAudio"; space++;
#endif

// linux / neutral
#if defined(__LINUX_JACK__) || defined(__UNIX_JACK__)
    drivers += string( space ? " | " : "" ) + "JACK"; space++;
#endif

    // no drivers
    if( space == 0 ) drivers = "(none)";

// check for universal binary
#if defined(__MACOS_UB__)
    archs = " [universal binary]";
#endif

    CK_FPRINTF_STDERR( "   %s | %ld-bit%s\n", platform.c_str(), machine_intsize(), archs.c_str() );
    CK_FPRINTF_STDERR( "   audio driver%s: %s\n", space>1 ? "s" : "", drivers.c_str() );
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
    CK_FPRINTF_STDERR( "%s", TC::reset().c_str() );
    CK_FPRINTF_STDERR( "\nusage: %s --[%s|%s] [%s] file1 file2 ...\n\n", "chuck", TC::orange("options").c_str(), TC::blue("commands").c_str(), TC::blue("+-=^").c_str() );
    CK_FPRINTF_STDERR( "%s", TC::set_orange().c_str() );
    CK_FPRINTF_STDERR( "    [options] = halt|loop|audio|silent|dump|nodump|about|probe|\n" );
    CK_FPRINTF_STDERR( "                channels:<N>|out:<N>|in:<N>|dac:<N>|adc:<N>|driver:<name>|\n" );
    CK_FPRINTF_STDERR( "                srate:<N>|bufsize:<N>|bufnum:<N>|shell|empty|\n" );
    CK_FPRINTF_STDERR( "                remote:<hostname>|port:<N>|verbose:<N>|level:<N>|\n" );
    CK_FPRINTF_STDERR( "                callback|deprecate:{stop|warn|ignore}|chugin-probe|\n" );
    CK_FPRINTF_STDERR( "                chugin-load:{on|off}|chugin-path:<path>|chugin:<name>\n" );
    CK_FPRINTF_STDERR( "                --color:{on|off}|--no-color\n" );
    CK_FPRINTF_STDERR( "%s", TC::set_blue().c_str() );
    CK_FPRINTF_STDERR( "   [commands] = add|remove|replace|remove.all|status|time|\n" );
    CK_FPRINTF_STDERR( "                clear.vm|reset.id|abort.shred|exit\n" );
    CK_FPRINTF_STDERR( "       [+-=^] = shortcuts for add, remove, replace, status\n" );
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
// desc: handler for ctrl-c (SIGINT). NB: on windows this is triggered
//       in a separate thread. global_cleanup requires a mutex.
//       ... or another synchronization mechanism
//-----------------------------------------------------------------------------
extern "C" void signal_int( int sig_num )
{
    // log
    CK_FPRINTF_STDERR( "[chuck]: cleaning up...\n" );

    // clean up
    if( !global_cleanup() )
    {
        // we got here possibly because global_cleanup() is underway on
        // another thread; wait a bit and give actual cleaner a chance to
        // finish | 1.5.0.4 (ge) added
        ck_usleep( 50000 );
    }

    // exit with code
    exit( 130 ); // 130 -> terminated by ctrl-c
}




//-----------------------------------------------------------------------------
// name: signal_pipe()
// desc: ...
//-----------------------------------------------------------------------------
extern "C" void signal_pipe( int sig_num )
{
    CK_FPRINTF_STDERR( "[chuck]: sigpipe handled - broken pipe (no connection)...\n" );
    if( g_sigpipe_mode )
    {
        // log
        CK_FPRINTF_STDERR( "[chuck]: cleaning up...\n" );
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
t_CKBOOL global_cleanup()
{
    // make sure we don't double clean
    static bool s_already_cleaning = false;

    // check | 1.5.0.0 (ge) added
    if( s_already_cleaning )
    {
        // log | 1.5.0.0 (ge) added
        EM_log( CK_LOG_INFO, "additional global cleanup not needed..." );
        // done
        return FALSE;
    }

    // log | 1.5.0.0 (ge) added
    EM_log( CK_LOG_SYSTEM, "global cleanup initiating..." );
    // set flag
    s_already_cleaning = true;

    // static XMutex s_mutex;
    // s_mutex.acquire();

    // REFACTOR-2017: shut down audio system
    all_stop();

    // request MIDI, etc. files be closed
    // REFACTOR-2017: TODO; RESOLVED: used to be all_detach()
    // need to call this before deleting the_chuck/the VM
    ChucK::globalCleanup();
    
    // delete the chuck
    CK_SAFE_DELETE( the_chuck );

    // delete shell and set to NULL | 1.4.1.0
    if( g_enable_shell ) CK_SAFE_DELETE( g_shell );

    // wait for the shell, if it is running
    // does the VM reset its priority to normal before exiting?
    //if( g_enable_shell )
    //    while( g_shell != NULL )
    //        ck_usleep(10000);

    // REFACTOR-2017 TODO: Cancel otf, le_cb threads? Does this happen in ~ChucK()?
    // things don't work so good on windows...
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
//    if( g_tid_otf ) pthread_cancel( g_tid_otf );
//    if( g_tid_whatever ) pthread_cancel( g_tid_whatever );
#else
    // close handle
//    if( g_tid_otf ) CloseHandle( g_tid_otf );
#endif

    // 1.5.0.0 (ge) | commented
    // s_mutex.release();

    return TRUE;
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
// name: check_shell_shutdown()
// desc: check if we need to take action on shell
//-----------------------------------------------------------------------------
t_CKBOOL check_shell_shutdown()
{
    if( g_enable_shell && g_shutdown_shell )
    {
        // set to false so we don't do this multiple time
        g_shutdown_shell = FALSE;
        // cleanup
        global_cleanup();
        // return yes
        return TRUE;
    }
    
    return FALSE;
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
    
    shell = (Chuck_Shell *) p;
    
    //atexit( wait_for_shell );
    
    // run the shell
    shell->run();
    
    // flag to effect global shutdown from main thread
    g_shutdown_shell = TRUE;

    EM_log( CK_LOG_INFO, "exiting thread routine for shell..." );
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: go()
// desc: parse args, init stuff, run!
//-----------------------------------------------------------------------------
t_CKBOOL go( int argc, const char ** argv )
{
    t_CKBOOL vm_halt = TRUE;
    t_CKINT srate = CK_SAMPLE_RATE_DEFAULT;
    t_CKBOOL force_srate = FALSE; // added 1.3.1.2
    t_CKINT buffer_size = CK_BUFFER_SIZE_DEFAULT;
    t_CKINT num_buffers = CK_NUM_BUFFERS_DEFAULT;
    t_CKINT dac = 0;
    t_CKINT adc = 0;
    std::string dac_name = ""; // added 1.3.0.0
    std::string adc_name = ""; // added 1.3.0.0
    t_CKINT dac_chans = 2;
    t_CKINT adc_chans = 2;
    t_CKINT adc_chans_before_rtaudio = adc_chans; // added 1.5.0.0
    t_CKBOOL dump = FALSE;
    t_CKBOOL probe = FALSE;
    t_CKBOOL probe_chugs = FALSE;
    t_CKBOOL doAbout = FALSE;
    t_CKBOOL doVersion = FALSE;
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
    // whether to print code quotes for compiler message | 1.5.0.5 (ge) added
    t_CKBOOL suppress_error_quote = FALSE;
    // color terminal output | 1.5.0.5 (ge) added
    t_CKBOOL colorTerminal = TRUE;
    string   filename = "";
    vector<string> args;
    // audio driver | 1.5.0.0
    string   audio_driver = "";
    // dac and adc devices names | 1.5.0.0
    string   dac_device_name = "";
    string   adc_device_name = "";
    // default driver name | 1.5.0.1
    if( g_enable_realtime_audio )
        audio_driver = ChuckAudio::defaultDriverName();
    // error message string
    string errorMessage1;
    string errorMessage2;

    // do any ANSI code config | 1.5.0.5 (ge) added
    // (e.g., windows need to explicitly be configured to process
    // escape sequences, e.g., for color console output)
    // NOTE: do this early in case printing is desired
    ck_configANSI_ESCcodes();

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
#elif defined(__PLATFORM_APPLE__)
    do_watchdog = TRUE;
#elif defined(__PLATFORM_WINDOWS__) && !defined(__WINDOWS_PTHREAD__)
    do_watchdog = TRUE;
#else
    do_watchdog = FALSE;
#endif

    t_CKUINT files = 0;
    t_CKUINT count = 1;
    t_CKINT i;

    //------------------------- COMMAND-LINE ARGUMENTS -----------------------------
    
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
            else if( !strncmp(argv[i], "--driver:", 9))
                audio_driver = argv[i]+9;
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
                    errorMessage1 = "invalid arguments for '--dac:'";
                    errorMessage2 = "(see 'chuck --help' for more info)";
                    break;
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
                    errorMessage1 = "invalid arguments for '--adc:'";
                    errorMessage2 = "(see 'chuck --help' for more info)";
                    break;
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
                string arg = tolower(argv[i]+11);
                if( arg == ":stop" ) deprecate_level = 0;
                else if( arg == ":warn" ) deprecate_level = 1;
                else if( arg == ":ignore" ) deprecate_level = 2;
                else
                {
                    // error
                    errorMessage1 = "invalid arguments for '--deprecate:'";
                    errorMessage2 = " |- looking for STOP, WARN, or IGNORE";
                    break;
                }
            }
            // (added 1.3.0.0)
            else if( !strncmp(argv[i], "--chugin-load:", sizeof("--chugin-load:")-1) )
            {
                // get the rest
                string arg = tolower(argv[i]+sizeof("--chugin-load:")-1);
                if( arg == "off" ) chugin_load = 0;
                else if( arg == "on" || arg == "auto") chugin_load = 1;
                else
                {
                    // error
                    errorMessage1 = "invalid arguments for '--chugin-load:'";
                    errorMessage2 = " |- looking for ON, OFF, or AUTO";
                    break;
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
            else if( !strncmp(argv[i], "-g", sizeof("-g")-1) )
            {
                named_dls.push_back(argv[i]+sizeof("-g")-1);
            }
            // (added 1.5.0.5)
            else if( !strncmp(argv[i], "--color:", sizeof("--color:")-1) )
            {
                // get the rest
                string arg = tolower(argv[i]+sizeof("--color:")-1);
                if( arg == "off" ) colorTerminal = false;
                else if( arg == "on" || arg == "auto") colorTerminal = true;
                else
                {
                    // error
                    errorMessage1 = "invalid arguments for '--color:'...";
                    errorMessage2 = "...(looking for ON, OFF, or AUTO)";
                    break;
                }
            }
            else if( tolower(argv[i]) == "--color" )
                colorTerminal = TRUE;
            else if( tolower(argv[i]) == "--no-color" )
                colorTerminal = FALSE;
            // (added 1.3.0.0)
            else if( !strcmp( argv[i], "--no-otf" ) )
            {
                // don't use this new vm for otf commands (use the previous one)
                update_otf_vm = FALSE;
            }
            else if( !strcmp( argv[i], "--probe" ) )
                probe = TRUE;
            else if( !strcmp( argv[i], "--chugin-probe" ) )
                probe_chugs = TRUE;
            else if( !strcmp( argv[i], "--poop" ) )
                uh();
            else if( !strcmp( argv[i], "--caution-to-the-wind" ) )
                g_enable_system_cmd = TRUE;
            else if( !strcmp( argv[i], "--disable-error-show-code" ) )
                suppress_error_quote = TRUE;
            else if( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
                    || !strcmp(argv[i], "--about") )
            {
                doAbout= TRUE;
            }
            else if( !strcmp( argv[i], "--version" ) )
            {
                doVersion = TRUE;
            }
            else
            {
                // boost log level
                g_otf_log = CK_LOG_CORE;
                // flag
                t_CKINT is_otf = FALSE;
                // log level
                EM_setlog( log_level );
                
                // do it
                if( otf_send_cmd( argc, argv, i, g_otf_dest, g_otf_port, &is_otf ) )
                    exit( 0 );
                
                // is otf
                if( is_otf ) exit( 1 );
                
                // done
                errorMessage1 = string("invalid flag '") + argv[i] + "' (see --help/-h)";
                break;
            }
        }
        else // doesn't look like an argument
        {
            // increase number of files
            files++;
        }
    }

    // check if we output to TTY (teletype char at a time)
    // if not disable color teriminal to avoid outputing
    // ANSI escape codes to the output stream, which would
    // show up in | and >
    if( !ck_isatty() ) colorTerminal = false;

    //------------------ CHUCK PRE-INITIALIZATION ---------------------
    // instantiate a ChucK!
    the_chuck = new ChucK();

    // set the color terminal global setting
    the_chuck->setParam( CHUCK_PARAM_TTY_COLOR, colorTerminal );

    // set log level so things can start logging
    the_chuck->setLogLevel( log_level );
    //-----------------------------------------------------------------

    // check for error message from command-line arguments
    if( errorMessage1 != "" ) EM_error2( 0, errorMessage1.c_str() );
    if( errorMessage2 != "" ) EM_error2( 0, errorMessage2.c_str() );
    // if either
    if( errorMessage1.length() || errorMessage2.length() )
    {
        // see if need to print usage
        if( doAbout ) usage();
        // bail for now
        exit(1);
    }

    //-----------------------------------------------------------------
    // about
    //-----------------------------------------------------------------
    if( doAbout )
    {
        usage();
        exit( 2 );
    }
    //-----------------------------------------------------------------
    // version
    //-----------------------------------------------------------------
    if( doVersion )
    {
        version();
        exit( 2 );
    }
    //-----------------------------------------------------------------
    // probe
    //-----------------------------------------------------------------
    if( probe )
    {
        // ensure log level is at least SYSTEM
        if( the_chuck->getLogLevel() < CK_LOG_SYSTEM )
            the_chuck->setLogLevel( CK_LOG_SYSTEM );

        // probe default/selected audio driver
        ChuckAudio::probe( audio_driver.c_str() );
    
#ifndef __DISABLE_MIDI__
        // probe MIDI input and output devices
        EM_error2b( 0, "" );
        probeMidiIn();
        EM_error2b( 0, "" );
        probeMidiOut();
        EM_error2b( 0, "" );
#endif  // __DISABLE_MIDI__

        // probe HID devices
        HidInManager::probeHidIn();

        // done probe
        exit( 0 );
    }

    // set caution to wind
    ChucK::enableSystemCall = g_enable_system_cmd;

    // check buffer size
    buffer_size = ck_ensurepow2( buffer_size );
    // check mode and blocking
    if( !g_enable_realtime_audio && !block ) block = TRUE;
    // audio, boost 1.4.1.0 (ge) commented out
    // if( !set_priority && !block ) g_priority = g_priority_low;
    if( !set_priority && !g_enable_realtime_audio ) g_priority = 0x7fffffff;
    // set priority
    XThreadUtil::our_priority = g_priority;
    // set watchdog
    g_do_watchdog = do_watchdog;
    // set adaptive size
    if( adaptive_size < 0 ) adaptive_size = buffer_size;

    if( !files && vm_halt && !g_enable_shell && !probe_chugs )
    {
        EM_error2( 0, "no input files... (try --help/-h)" );
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
        CK_SAFE_DELETE( g_shell );
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
    if( !probe_chugs && dac_name.size() > 0 )
    {
        // check with RtAudio
        t_CKINT dev = ChuckAudio::device_named( audio_driver.c_str(), dac_name, TRUE, FALSE );
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
    if( !probe_chugs && adc_name.size() > 0 )
    {
        // check with RtAudio
        t_CKINT dev = ChuckAudio::device_named( audio_driver.c_str(), adc_name, FALSE, TRUE );
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


    //-------------------- VIRTUAL MACHINE SETUP (PART 1) ---------------------
    // set chugins parameters
    the_chuck->setParam( CHUCK_PARAM_CHUGIN_ENABLE, chugin_load );
    the_chuck->setParam( CHUCK_PARAM_USER_CHUGINS, named_dls );
    the_chuck->setParam( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES, dl_search_path );

    //-----------------------------------------------------------------
    // probe chugins | 1.5.0.4 (ge) added
    //-----------------------------------------------------------------
    if( probe_chugs )
    {
        // ensure log level is at least SYSTEM
        if( the_chuck->getLogLevel() < CK_LOG_SYSTEM )
            the_chuck->setLogLevel( CK_LOG_SYSTEM );

        // probe chugins (.chug and .ck modules)
        // print/log what ChucK would load with current settings
        the_chuck->probeChugins();

        // done with probe
        exit( 0 );
    }

    // log
    EM_log( CK_LOG_SYSTEM, "initializing chuck..." );
    // push
    EM_pushlog();


    //--------------------------- AUDIO I/O SETUP -----------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "initializing system settings..." );
    // push
    EM_pushlog();
    // log
    // EM_log( CK_LOG_SYSTEM, "setting up for '%s' audio I/O...", g_enable_realtime_audio ? "real-time" : "silent mode" );
    
    // remember this, since adc_chans could be altered when initialization real-time audio
    adc_chans_before_rtaudio = adc_chans;

    // initialize audio system
    if( g_enable_realtime_audio )
    {
        // initialize real-time audio
        t_CKBOOL retval = ChuckAudio::initialize( dac, adc, dac_chans, adc_chans,
            srate, buffer_size, num_buffers, cb, (void *)the_chuck, force_srate, audio_driver.c_str() );
        // check return code
        if( !retval )
        {
            EM_log( CK_LOG_SYSTEM, "cannot initialize audio device..." );
            EM_log( CK_LOG_SYSTEM, "| (use --probe see list of available audio devices)" );
            EM_log( CK_LOG_SYSTEM, "| (use --dac/--adc to explicitly select output/input devices)" );
            EM_log( CK_LOG_SYSTEM, "| (use --silent/-s for non-realtime)" );
            EM_log( CK_LOG_SYSTEM, "| (use --help for more details)" );
            // pop
            EM_poplog();
            // done
            exit( 1 );
        }

        // these could have been updated during initialization
        adc = ChuckAudio::m_adc_n+1;
        dac = ChuckAudio::m_dac_n+1;
        adc_chans = ChuckAudio::m_num_channels_in;
        dac_chans = ChuckAudio::m_num_channels_out;
        audio_driver = ChuckAudio::m_driver_name;
        adc_device_name = ChuckAudio::m_adc_name;
        dac_device_name = ChuckAudio::m_dac_name;
        srate = ChuckAudio::m_sample_rate;
        buffer_size = ChuckAudio::buffer_size();
        num_buffers = ChuckAudio::num_buffers();
    }
    
    // pop
    EM_poplog();


    //-------------------- VIRTUAL MACHINE SETUP (PART 2) ---------------------
    // set params (some, like sample rate, could have been updated during
    // real-time audio initialization, so we are setting actual params here)
    // set the actual sample rate; needed to map to in-language durations
    the_chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, srate );
    // use the request number for adc # channels
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, adc_chans_before_rtaudio );
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, dac_chans );
    the_chuck->setParam( CHUCK_PARAM_VM_ADAPTIVE, adaptive_size );
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, (t_CKINT)(vm_halt) );
    the_chuck->setParam( CHUCK_PARAM_OTF_PORT, g_otf_port );
    the_chuck->setParam( CHUCK_PARAM_OTF_ENABLE, (t_CKINT)TRUE );
    the_chuck->setParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, (t_CKINT)dump );
    the_chuck->setParam( CHUCK_PARAM_AUTO_DEPEND, (t_CKINT)auto_depend );
    the_chuck->setParam( CHUCK_PARAM_DEPRECATE_LEVEL, deprecate_level );
    // set hint, so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, g_enable_realtime_audio );
    // enable or disable highlighting code on compiler error
    the_chuck->setParam( CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR, (t_CKINT)!suppress_error_quote );

    // initialize
    if( !the_chuck->init() )
    {
        CK_FPRINTF_STDERR( "[chuck]: failed to initialize...\n" );
        exit( 1 );
    }

    // timestamp
    EM_log( CK_LOG_SYSTEM, "local time: %s", timestamp_formatted().c_str() );


    //------------------ PRINT VERIFIED AUDIO SETTING -------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "initializing audio I/O..." );
    // push
    EM_pushlog();
    // log
    EM_log( CK_LOG_SYSTEM, "real-time audio: %s", g_enable_realtime_audio ? "YES" : "NO" );
    EM_log( CK_LOG_SYSTEM, "mode: %s", block ? "BLOCKING" : "CALLBACK" );
    EM_log( CK_LOG_SYSTEM, "sample rate: %ld", srate );
    EM_log( CK_LOG_SYSTEM, "buffer size: %ld", buffer_size );
    if( g_enable_realtime_audio )
    {
        EM_log( CK_LOG_SYSTEM, "num buffers: %ld", num_buffers );
        EM_log( CK_LOG_SYSTEM, "adaptive block processing: %ld", adaptive_size > 1 ? adaptive_size : 0 );
        EM_log( CK_LOG_SYSTEM, "audio driver: %s", audio_driver != "" ? audio_driver.c_str() : "(unspecified)");
        EM_log( CK_LOG_SYSTEM, "adc:[%d] \"%s\"", adc, adc_device_name.c_str() );
        EM_log( CK_LOG_SYSTEM, "dac:[%d] \"%s\"", dac, dac_device_name.c_str() );
        // EM_log( CK_LOG_SYSTEM, "adc: %ld dac: %d", adc, dac );
    }
    EM_log( CK_LOG_SYSTEM, "channels in: %ld out: %ld", adc_chans, dac_chans );
    // pop
    EM_poplog();


    //------------------------- HID RELATED SETUP -----------------------------
#ifndef __ALTER_HID__
    // pre-load hid
    if( load_hid ) HidInManager::init();
#endif // __ALTER_HID__

    
    //------------------------- CLI RELATED SETUP -----------------------------
    // catch SIGINT
    signal( SIGINT, signal_int );
#ifndef __PLATFORM_WINDOWS__
    // catch SIGPIPE
    signal( SIGPIPE, signal_pipe );
#endif

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

    // -------------------------- SHELL SETUP ---------------------------------
    // shell initialization
    if( g_enable_shell )
    {
        // instantiate
        g_shell = new Chuck_Shell;
        // initialize
        if( !init_shell( g_shell, new Chuck_Console(), the_chuck->vm() ) )
            exit( 1 );

        // flag
        g_shutdown_shell = FALSE;

        // NOTE: shell to be started later below
    }

    // version
    EM_log( CK_LOG_SYSTEM,  "chuck version: %s", TC::green(the_chuck->version(), true).c_str() );

    // pop log
    EM_poplog();


    //----------------------- ENTERING CHUCK RUNTIME --------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "starting chuck..." );

    // start it!
    the_chuck->start();

    // push indent
    EM_pushlog();


    //------------------------- SOURCE COMPILATION ----------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "starting compilation..." );
    // push indent
    EM_pushlog();

    // reset count
    count = 1;

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
    
    // syntax check only mode
    if( syntax_check_only )
    {
        // pop log
        EM_poplog();
        // log message
        EM_log( CK_LOG_SYSTEM, "syntax checking mode complete; exiting..." );
        // done
        return TRUE;
    }

    // debug print: print all vm objects
    CK_VM_DEBUGGER( print_all_objects() );
    // debug print: print vm objects stats
    CK_VM_DEBUGGER( print_stats() );


    //------------------------- STARTING AUDIO I/O ----------------------------
    // silent mode buffers
    SAMPLE * input = new SAMPLE[buffer_size*adc_chans];
    SAMPLE * output = new SAMPLE[buffer_size*dac_chans];
    // zero out
    memset( input, 0, sizeof(SAMPLE)*buffer_size*adc_chans );
    memset( output, 0, sizeof(SAMPLE)*buffer_size*dac_chans );

    // if real-time audio mode
    if( g_enable_realtime_audio )
    {
        // log
        EM_log( CK_LOG_SYSTEM, "starting real-time audio..." );
        // start real-time audio I/O
        ChuckAudio::start();
    }

    
    //------------------------- MAIN CHUCK LOOP!!! ----------------------------
    // log
    EM_log( CK_LOG_SYSTEM, "starting virtual machine..." );
    EM_log( CK_LOG_SYSTEM, "starting main loop..." );

    // pop indent
    EM_poplog();

    // chuck shell
    // (deferring as late as possible to minimize output conflict with log)
    if( g_enable_shell )
    {
        // start shell on separate thread | REFACTOR-2017: per-VM?!?
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
        pthread_create( &g_tid_shell, NULL, shell_cb, g_shell );
#else
        g_tid_shell = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)shell_cb, g_shell, 0, 0 );
#endif
    }

    // run loop
    while( the_chuck->vm_running() )
    {
        // real-time audio
        if( g_enable_realtime_audio )
        {
            // NOTE: with real-time audio, chuck VM computation
            // is driven from the audio callback function, not
            // on this main-thread (keep-alive) loop.

            // get main thread hook, call it if there is one
            Chuck_DL_MainThreadHook * hook = the_chuck->getMainThreadHook();
            if( hook ) {
                hook->m_hook( hook->m_bindle );
            } else {
                ck_usleep( 10000 );
            }
        }
        else // silent mode
        {
            // keep running as fast as possible
            the_chuck->run( input, output, buffer_size );
        }
        
        // shell relate activity
        if( check_shell_shutdown() ) break;
    }

    // if chuck is still available; could have been cleaned up by shell_shutdown
    if( the_chuck != NULL )
    {
        // get main thread hook
        Chuck_DL_MainThreadHook * hook = the_chuck->getMainThreadHook();
        // call it if there is one
        if( hook ) { hook->m_quit(hook->m_bindle); }
    }

    return TRUE;
}
