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
// file: chuck.h
// desc: chuck engine header; VM + compiler + state; no audio I/O
//       REFACTOR-2017
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: fall 2017
//
// additional authors:
//       Jack Atherton (lja@ccrma.stanford.edu)
//       Spencer Salazar (spencer@ccrma.stanford.edu)
//-----------------------------------------------------------------------------
#include "chuck.h"
#include "chuck_errmsg.h"
#include "chuck_io.h"
#include "chuck_globals.h" // added 1.4.1.0

#ifndef __DISABLE_OTF_SERVER__
#include "chuck_otf.h"
#include "ulib_machine.h"
#endif

#ifndef __DISABLE_NETWORK__
#include "util_network.h"
#endif

#include "util_string.h"
#include "ugen_stk.h"

#ifndef __PLATFORM_WIN32__
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>   // added 1.3.0.0
#else
#include <direct.h>      // added 1.3.0.0
#define MAXPATHLEN (255) // addec 1.3.0.0
#endif // #ifndef __PLATFORM_WIN32__




// chuck param defaults
#define CHUCK_PARAM_SAMPLE_RATE_DEFAULT            "44100"
#define CHUCK_PARAM_INPUT_CHANNELS_DEFAULT         "2"
#define CHUCK_PARAM_OUTPUT_CHANNELS_DEFAULT        "2"
#define CHUCK_PARAM_VM_ADAPTIVE_DEFAULT            "0"
#define CHUCK_PARAM_VM_HALT_DEFAULT                "0"
#define CHUCK_PARAM_OTF_ENABLE_DEFAULT             "0"
#define CHUCK_PARAM_OTF_PORT_DEFAULT               "8888"
#define CHUCK_PARAM_DUMP_INSTRUCTIONS_DEFAULT      "0"
#define CHUCK_PARAM_DEPRECATE_LEVEL_DEFAULT        "1"
#define CHUCK_PARAM_WORKING_DIRECTORY_DEFAULT      ""
#define CHUCK_PARAM_CHUGIN_ENABLE_DEFAULT          "1"
#define CHUCK_PARAM_HINT_IS_REALTIME_AUDIO_DEFAULT "0"
#ifndef __PLATFORM_WIN32__
// 1.4.1.0 (ge) changed to ""; was "/usr/local/lib/chuck"
// redundant with g_default_chugin_path, which already contains
#define CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT       ""
#else // __PLATFORM_WIN32__
// 1.4.1.0 (ge) changed to ""; "C:\\Program Files\\ChucK\\chugins"
// redundant with g_default_chugin_path, which already contains
#define CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT       ""
#endif // __PLATFORM_WIN32__
#define CHUCK_PARAM_USER_CHUGINS_DEFAULT        std::list<std::string>()
#define CHUCK_PARAM_USER_CHUGIN_DIRECTORIES_DEFAULT std::list<std::string>()



// chuck statics
const char ChucK::VERSION[] = "1.4.1.1 (numchucks)";
t_CKUINT ChucK::o_numVMs = 0;
t_CKBOOL ChucK::o_isGlobalInit = FALSE;
t_CKBOOL ChucK::enableSystemCall = FALSE;




//-----------------------------------------------------------------------------
// name: version()
// desc: get chuck version
//-----------------------------------------------------------------------------
const char * ChucK::version()
{
    return VERSION;
}




#ifndef __DISABLE_OTF_SERVER__
//-----------------------------------------------------------------------------
// name: intSize()
// desc: get chuck int size (in bits)
//-----------------------------------------------------------------------------
t_CKUINT ChucK::intSize()
{
    return machine_intsize();
}
#endif




//-----------------------------------------------------------------------------
// name: ChucK()
// desc: constructor
//-----------------------------------------------------------------------------
ChucK::ChucK()
{
    // instantiate the carrier!
    m_carrier = new Chuck_Carrier();
    // set reference back to this
    m_carrier->chuck = this;
    // increment the numChucKs
    o_numVMs++;
    // initialize default params
    initDefaultParams();
    // did user init?
    m_init = FALSE;
    // did user start ChucK?
    m_started = FALSE;
    // zero out the hook
    m_hook = NULL; // m_hook = nullptr
    
    // global init, if needed
    if( !o_isGlobalInit ) globalInit();
}




//-----------------------------------------------------------------------------
// name: ~ChucK()
// desc: destructor
//-----------------------------------------------------------------------------
ChucK::~ChucK()
{
    // shutdown
    shutdown();
    // decrement the numChucKs
    o_numVMs--;
    // clean up the carrier
    SAFE_DELETE( m_carrier );
}




// REFACTOR-2017: TODO Ge: Implementation of this?
//-----------------------------------------------------------------------------
// name: ck_param_type
// desc: enum for type of param (int, float, string)
//-----------------------------------------------------------------------------
enum ck_param_type
{
    ck_param_int, ck_param_float, ck_param_string, ck_param_string_list
};




//-----------------------------------------------------------------------------
// name: ck_param_types
// desc: storage for types of param (int, float, string)
//-----------------------------------------------------------------------------
std::map< std::string, ck_param_type> ck_param_types;




//-----------------------------------------------------------------------------
// name: initDefaultParams()
// desc: initialize default params
//-----------------------------------------------------------------------------
void ChucK::initDefaultParams()
{
    m_params[CHUCK_PARAM_SAMPLE_RATE] = CHUCK_PARAM_SAMPLE_RATE_DEFAULT;
    m_params[CHUCK_PARAM_INPUT_CHANNELS] = CHUCK_PARAM_INPUT_CHANNELS_DEFAULT;
    m_params[CHUCK_PARAM_OUTPUT_CHANNELS] = CHUCK_PARAM_OUTPUT_CHANNELS_DEFAULT;
    m_params[CHUCK_PARAM_VM_ADAPTIVE] = CHUCK_PARAM_VM_ADAPTIVE_DEFAULT;
    m_params[CHUCK_PARAM_VM_HALT] = CHUCK_PARAM_VM_HALT_DEFAULT;
    m_params[CHUCK_PARAM_OTF_ENABLE] = CHUCK_PARAM_OTF_ENABLE_DEFAULT;
    m_params[CHUCK_PARAM_OTF_PORT] = CHUCK_PARAM_OTF_PORT_DEFAULT;
    m_params[CHUCK_PARAM_DUMP_INSTRUCTIONS] = CHUCK_PARAM_DUMP_INSTRUCTIONS_DEFAULT;
    m_params[CHUCK_PARAM_DEPRECATE_LEVEL] = CHUCK_PARAM_DEPRECATE_LEVEL_DEFAULT;
    m_params[CHUCK_PARAM_WORKING_DIRECTORY] = CHUCK_PARAM_WORKING_DIRECTORY_DEFAULT;
    m_params[CHUCK_PARAM_CHUGIN_DIRECTORY] = CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT;
    m_params[CHUCK_PARAM_CHUGIN_ENABLE] = CHUCK_PARAM_CHUGIN_ENABLE_DEFAULT;
    m_listParams[CHUCK_PARAM_USER_CHUGINS] = CHUCK_PARAM_USER_CHUGINS_DEFAULT;
    m_listParams[CHUCK_PARAM_USER_CHUGIN_DIRECTORIES] = CHUCK_PARAM_USER_CHUGIN_DIRECTORIES_DEFAULT;
    m_params[CHUCK_PARAM_HINT_IS_REALTIME_AUDIO] = CHUCK_PARAM_HINT_IS_REALTIME_AUDIO_DEFAULT;
    
    ck_param_types[CHUCK_PARAM_SAMPLE_RATE]             = ck_param_int;
    ck_param_types[CHUCK_PARAM_INPUT_CHANNELS]          = ck_param_int;
    ck_param_types[CHUCK_PARAM_OUTPUT_CHANNELS]         = ck_param_int;
    ck_param_types[CHUCK_PARAM_VM_ADAPTIVE]             = ck_param_int;
    ck_param_types[CHUCK_PARAM_VM_HALT]                 = ck_param_int;
    ck_param_types[CHUCK_PARAM_OTF_ENABLE]              = ck_param_int;
    ck_param_types[CHUCK_PARAM_OTF_PORT]                = ck_param_int;
    ck_param_types[CHUCK_PARAM_DUMP_INSTRUCTIONS]       = ck_param_int;
    ck_param_types[CHUCK_PARAM_DEPRECATE_LEVEL]         = ck_param_int;
    ck_param_types[CHUCK_PARAM_WORKING_DIRECTORY]       = ck_param_string;
    ck_param_types[CHUCK_PARAM_CHUGIN_DIRECTORY]        = ck_param_string;
    ck_param_types[CHUCK_PARAM_CHUGIN_ENABLE]           = ck_param_int;
    ck_param_types[CHUCK_PARAM_USER_CHUGINS]            = ck_param_string_list;
    ck_param_types[CHUCK_PARAM_USER_CHUGIN_DIRECTORIES] = ck_param_string_list;
    ck_param_types[CHUCK_PARAM_HINT_IS_REALTIME_AUDIO]  = ck_param_int;
}




//-----------------------------------------------------------------------------
// name: setParam()
// desc: set an int param by name
//-----------------------------------------------------------------------------
bool ChucK::setParam( const std::string & name, t_CKINT value )
{
    if( m_params.count( name ) > 0 && ck_param_types[name] == ck_param_int )
    {
        std::ostringstream s;
        s << value;
        m_params[name] = s.str();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// name: setParamFloat()
// desc: set a float param by name
//-----------------------------------------------------------------------------
bool ChucK::setParamFloat( const std::string & name, t_CKFLOAT value )
{
    if( m_params.count( name ) > 0 && ck_param_types[name] == ck_param_float )
    {
        std::ostringstream s;
        s << value;
        m_params[name] = s.str();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// name: setParam()
// desc: set a string param by name
//-----------------------------------------------------------------------------
bool ChucK::setParam( const std::string & name, const std::string & value )
{
    if( m_params.count( name ) > 0 && ck_param_types[name] == ck_param_string )
    {
        m_params[name] = value;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// name: setParam()
// desc: set a string list param by name
//-----------------------------------------------------------------------------
bool ChucK::setParam( const std::string & name, const std::list< std::string > & value )
{
    if( m_listParams.count( name ) > 0 &&
        ck_param_types[name] == ck_param_string_list )
    {
        m_listParams[name] = value;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// name: getParamInt()
// desc: get an int param
//-----------------------------------------------------------------------------
t_CKINT ChucK::getParamInt( const std::string & key )
{
    t_CKINT result = 0;
    if( m_params.count( key ) > 0 && ck_param_types[key] == ck_param_int )
    {
        std::istringstream s( m_params[key] );
        s >> result;
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: getParamFloat()
// desc: get a float param
//-----------------------------------------------------------------------------
t_CKFLOAT ChucK::getParamFloat( const std::string & key )
{
    t_CKFLOAT result = 0;
    if( m_params.count( key ) > 0 && ck_param_types[key] == ck_param_float )
    {
        std::istringstream s( m_params[key] );
        s >> result;
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: getParamString()
// desc: get a string param
//-----------------------------------------------------------------------------
std::string ChucK::getParamString( const std::string & key )
{
    if( m_params.count( key ) > 0 && ck_param_types[key] == ck_param_string )
    {
        return m_params[key];
    }
    else
    {
        return "";
    }
}




//-----------------------------------------------------------------------------
// name: getParamStringList()
// desc: get a string param
//-----------------------------------------------------------------------------
std::list< std::string > ChucK::getParamStringList( const std::string & key )
{
    if( m_listParams.count( key ) > 0 &&
        ck_param_types[key] == ck_param_string_list )
    {
        return m_listParams[key];
    }
    else
    {
        return std::list< std::string >();
    }
}




//-----------------------------------------------------------------------------
// name: init()
// desc: initialize ChucK (using params)
//-----------------------------------------------------------------------------
bool ChucK::init()
{
    // sanity check
    if( m_init == TRUE )
    {
        CK_FPRINTF_STDERR( "[chuck]: VM already initialized...\n" );
        return false;
    }

    // initialize VM
    if( !initVM() ) goto cleanup;
    
    // initialize compiler
    if( !initCompiler() ) goto cleanup;
    
    // initialize chugin system
    if( !initChugins() ) goto cleanup;

    // initialize OTF programming system
    if( !initOTF() ) goto cleanup;
    
    // did user init?
    m_init = TRUE;
    
    return true;
    
cleanup:
    // shutdown, dealloc
    shutdown();
    
    return false;
}




//-----------------------------------------------------------------------------
// name: initVM()
// desc: initialize VM
//-----------------------------------------------------------------------------
bool ChucK::initVM()
{
    // get VM params
    t_CKUINT srate = getParamInt( CHUCK_PARAM_SAMPLE_RATE );
    t_CKUINT outs = getParamInt( CHUCK_PARAM_OUTPUT_CHANNELS );
    t_CKUINT ins = getParamInt( CHUCK_PARAM_INPUT_CHANNELS );
    t_CKUINT adaptiveSize = getParamInt( CHUCK_PARAM_VM_ADAPTIVE );
    t_CKBOOL halt = getParamInt( CHUCK_PARAM_VM_HALT ) != 0;
    
    // instantiate VM
    m_carrier->vm = new Chuck_VM();
    // reference back to carrier
    m_carrier->vm->setCarrier( m_carrier );
    // initialize VM
    if( !m_carrier->vm->initialize( srate, outs, ins, adaptiveSize, halt ) )
    {
        CK_FPRINTF_STDERR( "[chuck]: %s\n", m_carrier->vm->last_error() );
        return false;
    }
    
    return true;
}




//-----------------------------------------------------------------------------
// name: initCompiler()
// desc: initialize compiler
//-----------------------------------------------------------------------------
bool ChucK::initCompiler()
{
    // get compiler params
    t_CKBOOL dump = getParamInt( CHUCK_PARAM_DUMP_INSTRUCTIONS ) != 0;
    t_CKBOOL auto_depend = getParamInt( CHUCK_PARAM_AUTO_DEPEND ) != 0;
    t_CKUINT deprecate = getParamInt( CHUCK_PARAM_DEPRECATE_LEVEL );
    std::string workingDir = getParamString( CHUCK_PARAM_WORKING_DIRECTORY );

    // instantiate compiler
    m_carrier->compiler = new Chuck_Compiler();
    // reference back to carrier
    m_carrier->compiler->setCarrier( m_carrier );
    
    // initialize compiler
    if( !m_carrier->compiler->initialize() )
    {
        CK_FPRINTF_STDERR( "[chuck]: compiler failed to initialize...\n" );
        return false;
    }
    // set dump flag
    m_carrier->compiler->emitter->dump = dump;
    // set auto depend flag (for type checker) | currently must be FALSE
    m_carrier->compiler->set_auto_depend( auto_depend );
    // set deprecation level
    m_carrier->env->deprecate_level = deprecate;

    // VM + type system integration (needs to be done after compiler)
    if( !m_carrier->vm->initialize_synthesis() )
    {
        CK_FPRINTF_STDERR( "[chuck]: %s\n", m_carrier->vm->last_error() );
        return false;
    }

    std::string cwd;
    char cstr_cwd[MAXPATHLEN];
    
    // figure out current working directory (added 1.3.0.0)
    // is this needed for current path to work correctly?!
    if( getcwd(cstr_cwd, MAXPATHLEN) == NULL )
    {
        // uh...
        EM_log( CK_LOG_SEVERE, "error: unable to determine current working directory!" );
    }
    else
    {
        // make c++ string
        cwd = std::string(cstr_cwd);
        // add trailing "/"
        cwd += '/';
        // deferring this step until later, and only for Windows
        // cwd = normalize_directory_separator(cwd) + "/";

        // 1.4.1.1 (ge) added -- expand for things like ~
        workingDir = expand_filepath( workingDir, TRUE );

        // 1.4.1.0 (ge) added -- test for absolute path
        // 1.4.1.1 (ge) deals with windows absolute paths: e.g., "C:\"
        // TODO: deals with windows network paths: e.g., "\\server\"???
        if( is_absolute_path(workingDir) )
        {
            // do nothing here
        }
#ifdef __ANDROID__
        else if( workingDir.rfind("jar:", 0) == 0 )
        {
            // if workingDir is a JAR URL, make sure it ends on /
            if( *workingDir.rbegin() != '/' )
            {
                workingDir = workingDir + "/";
            }
            // log it
            // EM_log( CK_LOG_INFO, "setting current working directory: " );
        }
#endif
        else
        {
            // update
            workingDir = cwd + workingDir;
        }

#ifdef __PLATFORM_WIN32__
        // normalize path separators, only for windows; in case UNIX-style paths contains actual backslashes
        workingDir = normalize_directory_separator(workingDir);
#endif
        // check if need to add /
        // (note if workingDir is empty string, then this leaves it alone)
        if( workingDir.length() > 0 && (workingDir[workingDir.length() - 1] != '/') )
        {
            // append
            workingDir = workingDir + "/";
        }
        // update to current working directory
        setParam( CHUCK_PARAM_WORKING_DIRECTORY, workingDir );
        // log it
        EM_log( CK_LOG_INFO, "setting current working directory: " );

        // push log
        EM_pushlog();
        // print the working directory
        EM_log( CK_LOG_INFO, "%s", workingDir.c_str() );
        // pop log
        EM_poplog();
    }


    return true;
}




//-----------------------------------------------------------------------------
// name: initChugin()
// desc: initialize chugin system
//-----------------------------------------------------------------------------
bool ChucK::initChugins()
{
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;
    
    // whether or not chug should be enabled (added 1.3.0.0)
    if( getParamInt( CHUCK_PARAM_CHUGIN_ENABLE ) != 0 )
    {
        // chugin dur
        std::string chuginDir = getParamString( CHUCK_PARAM_CHUGIN_DIRECTORY );
        // list of search pathes (added 1.3.0.0)
        std::list<std::string> dl_search_path = getParamStringList( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES );
        if( chuginDir != std::string("") )
        {
            dl_search_path.push_back( chuginDir );
        }
        // list of individually named chug-ins (added 1.3.0.0)
        std::list<std::string> named_dls = getParamStringList( CHUCK_PARAM_USER_CHUGINS );

        // log
        EM_log( CK_LOG_SYSTEM, "loading chugins..." );
        // push indent level
        EM_pushlog();
        // load external libs
        if( !compiler()->load_external_modules( ".chug", dl_search_path, named_dls ) )
        {
            // pop indent level
            EM_poplog();
            // clean up
            goto error;
        }
        // pop log
        EM_poplog();

        // log
        EM_log( CK_LOG_SYSTEM, "pre-loading ChucK libs..." );
        EM_pushlog();
        
        // iterate over list of ck files that the compiler found
        for( std::list<std::string>::iterator j =
            compiler()->m_cklibs_to_preload.begin();
            j != compiler()->m_cklibs_to_preload.end(); j++)
        {
            // the filename
            std::string filename = *j;
            
            // log
            EM_log( CK_LOG_SEVERE, "preloading '%s'...", filename.c_str() );
            // push indent
            EM_pushlog();
            
            // SPENCERTODO: what to do for full path
            std::string full_path = filename;
            
            // parse, type-check, and emit
            if( compiler()->go( filename, NULL, NULL, full_path ) )
            {
                // TODO: how to compilation handle?
                //return 1;
                
                // get the code
                code = compiler()->output();
                // name it - TODO?
                // code->name += string(argv[i]);
                
                // spork it
                shred = vm()->spork( code, NULL, TRUE );
            }
            
            // pop indent
            EM_poplog();
        }
        
        // clear the list of chuck files to preload
        compiler()->m_cklibs_to_preload.clear();

        // pop log
        EM_poplog();
        
        return true;
    }
    else
    {
        // log | 1.4.1.0 (ge) commented out; printing earlier
        EM_log( CK_LOG_SYSTEM, "chugin system: OFF" );
    }

    // load user namespace
    m_carrier->env->load_user_namespace();

    return true;

error: // 1.4.1.0 (ge) added
    // any cleanup goes here; none for now
    return false;
}




//-----------------------------------------------------------------------------
// name: initOTF()
// desc: init OTF programming system
//-----------------------------------------------------------------------------
bool ChucK::initOTF()
{
#ifndef __DISABLE_OTF_SERVER__
    // server
    if( getParamInt( CHUCK_PARAM_OTF_ENABLE ) != 0 )
    {
        m_carrier->otf_port = getParamInt( CHUCK_PARAM_OTF_PORT );
        // log
        EM_log( CK_LOG_SYSTEM, "starting listener on port: %d...",
               m_carrier->otf_port );
        
        // start tcp server
        m_carrier->otf_socket = ck_tcp_create( 1 );
        if( !m_carrier->otf_socket ||
            !ck_bind( m_carrier->otf_socket, m_carrier->otf_port ) ||
            !ck_listen( m_carrier->otf_socket, 10 ) )
        {
            CK_FPRINTF_STDERR( "[chuck]: cannot bind to tcp port %li...\n", m_carrier->otf_port );
            ck_close( m_carrier->otf_socket );
            m_carrier->otf_socket = NULL;
        }
        else
        {
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
            pthread_create( &m_carrier->otf_thread, NULL, otf_cb, m_carrier );
#else
            m_carrier->otf_thread = CreateThread( NULL, 0,
                                                  (LPTHREAD_START_ROUTINE)otf_cb,
                                                  m_carrier, 0, 0 );
#endif
        }
    }
    else
    {
        // log
        EM_log( CK_LOG_SYSTEM, "OTF server/listener: OFF" );
    }
#endif
    return true;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown ChucK instance
//-----------------------------------------------------------------------------
bool ChucK::shutdown()
{
    // log
    EM_log( CK_LOG_SYSTEM, "shutting down ChucK instance..." );

    // stop VM
    if( m_carrier != NULL && m_carrier->vm != NULL  )
    {
        m_carrier->vm->stop();

        // wait for it to stop...
        while( m_carrier->vm->running() )
        {
            usleep(1000);
        }
    }

    // free vm, compiler, friends
    // first, otf
    // REFACTOR-2017 TODO: le_cb?
    
    // STK-specific
    stk_detach( m_carrier );
    
#ifndef __DISABLE_OTF_SERVER__
    // cancel otf thread
    if( m_carrier->otf_thread )
    {
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
        pthread_cancel( m_carrier->otf_thread );
#else
        CloseHandle( m_carrier->otf_thread ); // doesn't cancel thread
#endif
        m_carrier->otf_thread = 0; // signals thread shutdown
    }

    // close otf socket
    if( m_carrier->otf_socket ) ck_close( m_carrier->otf_socket );
    
    // reset
    m_carrier->otf_socket = NULL;
    m_carrier->otf_port = 0;
#endif // __DISABLE_OTF_SERVER__

    // TODO: a different way to unlock?
    // unlock all objects to delete chout, cherr
    Chuck_VM_Object::unlock_all();
    SAFE_RELEASE( m_carrier->chout );
    SAFE_RELEASE( m_carrier->cherr );
    // relock
    Chuck_VM_Object::lock_all();
    // clean up vm, compiler
    SAFE_DELETE( m_carrier->vm );
    SAFE_DELETE( m_carrier->compiler );
    m_carrier->env = NULL;

    // clear flag
    m_init = FALSE;
    // clear flag
    m_started = FALSE;

    // done
    return true;
}




//-----------------------------------------------------------------------------
// name: compileFile()
// desc: compile a file (can be called anytime)
//-----------------------------------------------------------------------------
bool ChucK::compileFile( const std::string & path, const std::string & argsTogether, int count )
{
    // sanity check
    if( !m_carrier->compiler )
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: compileFile() invoked before initialization ...\n" );
        return false;
    }
    
    std::string filename;
    std::vector<std::string> args;
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;
    
    // log
    EM_log( CK_LOG_FINE, "compiling '%s'...", path.c_str() );
    // push indent
    EM_pushlog();
    
    // append
    std::string theThing = path + ":" + argsTogether;
#ifdef __ANDROID__
    if( theThing.rfind("jar:", 0) == 0 )
    {
        const size_t jarPrefixLen = strlen("jar:");
        const size_t secondColon = path.find(':', jarPrefixLen);
        if( secondColon == std::string::npos )
        {
            theThing = std::string("jar\\:") + theThing.substr(jarPrefixLen);
        }
        else
        {
            theThing = std::string("jar\\:") + theThing.substr(jarPrefixLen,
                secondColon - jarPrefixLen) + std::string("\\:") +
                theThing.substr(secondColon + 1);
        }
    }
#endif // __ANDROID__
    
    // parse out command line arguments
    if( !extract_args( theThing, filename, args ) )
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: malformed filename with argument list...\n" );
        CK_FPRINTF_STDERR( "    -->  '%s'", theThing.c_str() );
        return false;
    }
    
    // construct full path to be associated with the file so me.sourceDir() works
    // (added 1.3.0.0)
    std::string full_path = get_full_path(filename);
    
    // parse, type-check, and emit (full_path added 1.3.0.0)
    if( !m_carrier->compiler->go( filename, NULL, NULL, full_path ) )
        return false;
    
    // get the code
    code = m_carrier->compiler->output();
    // name it
    code->name += path;
    
    // log
    EM_log( CK_LOG_FINE, "sporking %d %s...", count,
           count == 1 ? "instance" : "instances" );
    
    // spork it
    while( count-- )
    {
        #ifndef __EMSCRIPTEN__
        // spork (for now, spork_immediate arg is always false)
        shred = m_carrier->vm->spork( code, NULL, FALSE );
        #else
        // spork (in emscripten, need to spork immediately so can get shred id)
        shred = m_carrier->vm->spork( code, NULL, TRUE );
        #endif
        
        // add args
        shred->args = args;
    }
    
    // pop indent
    EM_poplog();
    
    // reset the parser
    reset_parse();
    
    return true;
}




//-----------------------------------------------------------------------------
// name: compileCode()
// desc: compile code directly
//-----------------------------------------------------------------------------
bool ChucK::compileCode( const std::string & code, const std::string & argsTogether, int count)
{
    // sanity check
    if( !m_carrier->compiler )
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: compileCode() invoked before initialization ...\n" );
        return false;
    }
    
    std::vector<std::string> args;
    Chuck_VM_Code * vm_code = NULL;
    Chuck_VM_Shred * shred = NULL;
    
    // log
    EM_log( CK_LOG_FINE, "compiling code from string..." );
    // push indent
    EM_pushlog();
    
    // falsify filename / path for various logs
    std::string theThing = "compiled.code:" + argsTogether;
    std::string fakefakeFilename = "<result file name goes here>";
    
    // parse out command line arguments
    if( !extract_args( theThing, fakefakeFilename, args ) )
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: malformed filename with argument list...\n" );
        CK_FPRINTF_STDERR( "    -->  '%s'", theThing.c_str() );
        return false;
    }
    
    // working directory
    std::string workingDir = getParamString( CHUCK_PARAM_WORKING_DIRECTORY );
    
    // construct full path to be associated with the file so me.sourceDir() works
    std::string full_path = workingDir + "/compiled.code";
    // log
    EM_log( CK_LOG_FINE, "full path: %s...", full_path.c_str() );
    
    // parse, type-check, and emit (full_path added 1.3.0.0)
    if( !m_carrier->compiler->go( "<compiled.code>", NULL, code.c_str(), full_path ) )
        return false;
    
    // get the code
    vm_code = m_carrier->compiler->output();
    // name it (no path to append)
    vm_code->name += "compiled.code";
    
    // log
    EM_log( CK_LOG_FINE, "sporking %d %s...", count,
            count == 1 ? "instance" : "instances" );
    
    // spork it
    while( count-- )
    {
        #ifndef __EMSCRIPTEN__
        // spork (for now, spork_immediate arg is always false)
        shred = m_carrier->vm->spork( vm_code, NULL, FALSE );
        #else
        // spork (in emscripten, need to spork immediately so can get shred id)
        shred = m_carrier->vm->spork( vm_code, NULL, TRUE );
        #endif
        // add args
        shred->args = args;
    }
    
    // pop indent
    EM_poplog();

    // reset the parser
    reset_parse();

    return true;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: start chuck instance
//-----------------------------------------------------------------------------
bool ChucK::start()
{
    // sanity check
    if( m_carrier->vm == NULL )
    {
        CK_FPRINTF_STDERR( "[chuck]: cannot start, VM not initialized...\n" );
        return false;
    }

    // if already started
    if( m_started )
    {
        // return VM running state
        return m_carrier->vm->running();
    }
    
    // start the VM!
    if( !m_carrier->vm->running() ) m_carrier->vm->start();

    // set flag
    m_started = TRUE;

    // return state
    return m_carrier->vm->running();
}




//-----------------------------------------------------------------------------
// name: run()
// desc: run engine (call from host callback)
//-----------------------------------------------------------------------------
void ChucK::run( SAMPLE * input, SAMPLE * output, int numFrames )
{
    // make sure we started...
    if( !m_started ) this->start();
    
    // call the callback
    m_carrier->vm->run( numFrames, input, output );
}



//-----------------------------------------------------------------------------
// name: globals()
// desc: returns VM Globals Manager
//-----------------------------------------------------------------------------
Chuck_Globals_Manager * ChucK::globals()
{
    // check pointer
    if( !m_carrier->vm ) return NULL;
    // check if running
    if( !m_carrier->vm->running() ) return NULL;

    // return the thing!
    return m_carrier->vm->globals_manager();
}




//-----------------------------------------------------------------------------
// name: setChoutCallback()
// desc: provide a callback where Chout print statements are routed
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::setChoutCallback( void (* callback)(const char *) )
{
    if( !m_init || m_carrier->chout == NULL ) return FALSE;
    m_carrier->chout->set_output_callback( callback );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setCherrCallback()
// desc: provide a callback where Cherr print statements are routed
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::setCherrCallback( void (* callback)(const char *) )
{
    if( !m_init || m_carrier->cherr == NULL ) return FALSE;
    m_carrier->cherr->set_output_callback( callback );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setStdoutCallback()
// desc: provide a callback where stdout print statements are routed
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::setStdoutCallback( void (* callback)(const char *) )
{
    ck_set_stdout_callback( callback );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setStderrCallback()
// desc: provide a callback where <<< >>> and stdout print statements are routed
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::setStderrCallback( void (* callback)(const char *) )
{
    ck_set_stderr_callback( callback );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: globalInit()
// desc: init all things once (for all ChucK instances)
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::globalInit()
{
    // sanity check
    if( o_isGlobalInit ) return FALSE;
    
    // nothing to do, for now
    
    // set flag
    o_isGlobalInit = TRUE;
    
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: globalCleanup()
// desc: clean up all things
//-----------------------------------------------------------------------------
void ChucK::globalCleanup()
{
    // sanity check
    if( !o_isGlobalInit ) return;
    // set flag
    o_isGlobalInit = FALSE;
    
    // log
    EM_log( CK_LOG_INFO, "detaching all resources..." );
    // push
    EM_pushlog();
    
    //#ifndef __DISABLE_MIDI__
    // close midi file handles
    // midirw_detach();
    //#endif // __DISABLE_MIDI__
    
    #ifndef __ALTER_HID__
    // shutdown HID
    HidInManager::cleanup();
    #endif // __ALTER_HID__
    
    #ifndef __DISABLE_SERIAL__
    // shutdown serial
    Chuck_IO_Serial::shutdown();
    #endif

    #ifndef __DISABLE_KBHIT__
    // shutdown kb loop
    // KBHitManager::shutdown();
    #endif // __DISABLE_KBHIT__

    // pop
    EM_poplog();
    
    // REFACTOR-2017 TODO Ge:
    // stop le_cb, ...?
}




//-----------------------------------------------------------------------------
// set log level -- this should eventually be per-VM?
//-----------------------------------------------------------------------------
void ChucK::setLogLevel( t_CKINT level )
{
    EM_setlog( level );
}




//-----------------------------------------------------------------------------
// get log level -- also per-VM?
//-----------------------------------------------------------------------------
t_CKINT ChucK::getLogLevel()
{
    return g_loglevel;
}




//-----------------------------------------------------------------------------
// name: poop()
// desc: --poop
//-----------------------------------------------------------------------------
void ChucK::poop()
{
    #ifndef __DISABLE_OTF_SERVER__
    ::uh();
    #endif
}




//-----------------------------------------------------------------------------
// name: setMainThreadHook()
// desc: set a function pointer to call from the main thread loop
//       e.g., for graphics, MAUI, potentially from a chug-in
//-----------------------------------------------------------------------------
bool ChucK::setMainThreadHook(Chuck_DL_MainThreadHook * hook)
{
    m_hook = hook;
    return true;
}




//-----------------------------------------------------------------------------
// name: getMainThreadHook()
// desc: get pointer to current function to be called from main loop
//-----------------------------------------------------------------------------
Chuck_DL_MainThreadHook * ChucK::getMainThreadHook()
{
    return m_hook;
}
