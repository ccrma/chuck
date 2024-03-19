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
// file: chuck.cpp
// desc: interface for ChucK core: compiler + virtual machine + synthesis
//       |- possible to instantiate multiple ChucK instances
//       |- each ChucK instance contains a fully functioning ChucK runtime
//       |- the ChucK class interface is primary point of integration between
//          ChucK core and ChucK hosts (e.g., command-line ChucK,
//          miniAudicle, WebChucK, Chunity, Chunreal, and any C++ program
//          using ChucK core as a component).
//       |- see src/host-examples/ for sample integration code for ChucK core
//       |- see chuck, miniAudicle, WebChucK, Chunity etc. for their
//          respective integration of ChucK core
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: fall 2017
//
// additional authors:
//       Jack Atherton (lja@ccrma.stanford.edu)
//       Spencer Salazar (spencer@ccrma.stanford.edu)
//
// (part of The Big Refactor of 2017; codename `numchucks`; #REFACTOR-2017)
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

#include "util_platforms.h"
#include "util_string.h"
#include "ugen_stk.h"

#ifndef __PLATFORM_WINDOWS__
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <direct.h>      // added 1.3.0.0
#endif // #ifndef __PLATFORM_WINDOWS__




// chuck param defaults
#define CHUCK_PARAM_SAMPLE_RATE_DEFAULT            "44100"
#define CHUCK_PARAM_INPUT_CHANNELS_DEFAULT         "2"
#define CHUCK_PARAM_OUTPUT_CHANNELS_DEFAULT        "2"
#define CHUCK_PARAM_VM_ADAPTIVE_DEFAULT            "0"
#define CHUCK_PARAM_VM_HALT_DEFAULT                "0"
#define CHUCK_PARAM_OTF_ENABLE_DEFAULT             "0"
#define CHUCK_PARAM_OTF_PORT_DEFAULT               "8888"
#define CHUCK_PARAM_OTF_PRINT_WARNINGS_DEFAULT     "0"
#define CHUCK_PARAM_DUMP_INSTRUCTIONS_DEFAULT      "0"
#define CHUCK_PARAM_AUTO_DEPEND_DEFAULT            "0"
#define CHUCK_PARAM_DEPRECATE_LEVEL_DEFAULT        "1"
#define CHUCK_PARAM_WORKING_DIRECTORY_DEFAULT      ""
#define CHUCK_PARAM_CHUGIN_ENABLE_DEFAULT          "1"
#define CHUCK_PARAM_IS_REALTIME_AUDIO_HINT_DEFAULT "0"
#ifndef __PLATFORM_WINDOWS__
// 1.4.1.0 (ge) changed to ""; was "/usr/local/lib/chuck"
// redundant with g_default_chugin_path, which already contains
#define CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT       ""
#else // __PLATFORM_WINDOWS__
// 1.4.1.0 (ge) changed to ""; "C:\\Program Files\\ChucK\\chugins"
// redundant with g_default_chugin_path, which already contains
#define CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT       ""
#endif // __PLATFORM_WINDOWS__
#define CHUCK_PARAM_USER_CHUGINS_DEFAULT        std::list<std::string>()
#define CHUCK_PARAM_USER_CHUGIN_DIRECTORIES_DEFAULT std::list<std::string>()
#define CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR_DEFAULT "1"
#define CHUCK_PARAM_TTY_COLOR_DEFAULT              "0"
#define CHUCK_PARAM_TTY_WIDTH_HINT_DEFAULT         "80"




// chuck statics
const char ChucK::VERSION[] = CHUCK_VERSION_STRING;
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




//-----------------------------------------------------------------------------
// name: intSize()
// desc: get chuck int size (in bits)
//-----------------------------------------------------------------------------
t_CKUINT ChucK::intSize()
{
    return machine_intsize();
}




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
    CK_SAFE_DELETE( m_carrier );
}




//-----------------------------------------------------------------------------
// name: initDefaultParams()
// desc: initialize default params
//-----------------------------------------------------------------------------
void ChucK::initDefaultParams()
{
    initParam( CHUCK_PARAM_VERSION, CHUCK_VERSION_STRING, ck_param_string, TRUE );
    initParam( CHUCK_PARAM_SAMPLE_RATE, CHUCK_PARAM_SAMPLE_RATE_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_INPUT_CHANNELS, CHUCK_PARAM_INPUT_CHANNELS_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_OUTPUT_CHANNELS, CHUCK_PARAM_OUTPUT_CHANNELS_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_VM_ADAPTIVE, CHUCK_PARAM_VM_ADAPTIVE_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_VM_HALT, CHUCK_PARAM_VM_HALT_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_OTF_ENABLE, CHUCK_PARAM_OTF_ENABLE_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_OTF_PORT, CHUCK_PARAM_OTF_PORT_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_OTF_PRINT_WARNINGS, CHUCK_PARAM_OTF_PRINT_WARNINGS_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_DUMP_INSTRUCTIONS, CHUCK_PARAM_DUMP_INSTRUCTIONS_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_AUTO_DEPEND, CHUCK_PARAM_AUTO_DEPEND_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_DEPRECATE_LEVEL, CHUCK_PARAM_DEPRECATE_LEVEL_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_WORKING_DIRECTORY, CHUCK_PARAM_WORKING_DIRECTORY_DEFAULT, ck_param_string );
    initParam( CHUCK_PARAM_CHUGIN_DIRECTORY, CHUCK_PARAM_CHUGIN_DIRECTORY_DEFAULT, ck_param_string );
    initParam( CHUCK_PARAM_CHUGIN_ENABLE, CHUCK_PARAM_CHUGIN_ENABLE_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, CHUCK_PARAM_IS_REALTIME_AUDIO_HINT_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR, CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_TTY_COLOR, CHUCK_PARAM_TTY_COLOR_DEFAULT, ck_param_int );
    initParam( CHUCK_PARAM_TTY_WIDTH_HINT, CHUCK_PARAM_TTY_WIDTH_HINT_DEFAULT, ck_param_int );

    // initialize list params manually (take care to use tolower())
    m_listParams[tolower(CHUCK_PARAM_USER_CHUGINS)]             = CHUCK_PARAM_USER_CHUGINS_DEFAULT;
    m_param_types[tolower(CHUCK_PARAM_USER_CHUGINS)]            = ck_param_string_list;
    m_listParams[tolower(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES)]  = CHUCK_PARAM_USER_CHUGIN_DIRECTORIES_DEFAULT;
    m_param_types[tolower(CHUCK_PARAM_USER_CHUGIN_DIRECTORIES)] = ck_param_string_list;
}




//-----------------------------------------------------------------------------
// name: initParam()
// desc: internal helper -- initialize a param
//-----------------------------------------------------------------------------
void ChucK::initParam( const std::string & name, const std::string & value,
                       ck_param_type typeEnum, t_CKBOOL isReadOnly )
{
    // get lowercase version
    std::string n = tolower(name);
    // insert into map
    m_params[n] = value;
    // remember type
    m_param_types[n] = typeEnum;
    // read only?
    if( isReadOnly ) m_readOnly[n] = isReadOnly;
}




//-----------------------------------------------------------------------------
// name: readOnlyParam()
// desc: internal helper -- check read-only
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::readOnlyParam( const std::string & name )
{
    // check
    return m_readOnly.find(tolower(name)) != m_readOnly.end();
}




//-----------------------------------------------------------------------------
// name: matchParam()
// desc: internal helper -- match parameter name (case-insensitive)
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::matchParam( const std::string & lhs, const std::string & rhs )
{
    // return lower-case equal
    return tolower(lhs) == tolower(rhs);
}




//-----------------------------------------------------------------------------
// name: enactParam()
// desc: enact processing, as needed, for certain params
//-----------------------------------------------------------------------------
void ChucK::enactParam( const std::string & name, t_CKINT value )
{
    // check and set
    if( matchParam(name,CHUCK_PARAM_TTY_COLOR) )
    {
        // set the global override switch
        TC::globalDisableOverride( !value );
    }
    else if( matchParam(name,CHUCK_PARAM_TTY_WIDTH_HINT) )
    {
        // set default
        ck_ttywidth_setdefault( value );
    }
}




//-----------------------------------------------------------------------------
// name: setParam()
// desc: set an int param by name
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::setParam( const std::string & name, t_CKINT value )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check read-only
    if( readOnlyParam(key) ) return FALSE;
    // check key
    if( m_params.count(key) > 0 && m_param_types[key] == ck_param_int )
    {
        // enact processing, as needed
        enactParam( key, value );
        // insert into map
        m_params[key] = ck_itoa(value);
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
t_CKBOOL ChucK::setParamFloat( const std::string & name, t_CKFLOAT value )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check read-only
    if( readOnlyParam(key) ) return FALSE;
    // check
    if( m_params.count( key ) > 0 && m_param_types[key] == ck_param_float )
    {
        // insert into map
        m_params[key] = ck_ftoa( value, 32 );
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
t_CKBOOL ChucK::setParam( const std::string & name, const std::string & value )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check read-only
    if( readOnlyParam(key) ) return FALSE;
    // check
    if( m_params.count(key) > 0 && m_param_types[key] == ck_param_string )
    {
        m_params[key] = value;
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
t_CKBOOL ChucK::setParam( const std::string & name, const std::list< std::string > & value )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check read-only
    if( readOnlyParam(key) ) return FALSE;
    // check
    if( m_listParams.count( key ) > 0 &&
        m_param_types[key] == ck_param_string_list )
    {
        m_listParams[key] = value;
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
t_CKINT ChucK::getParamInt( const std::string & name )
{
    // lower case for consistency
    std::string key = tolower(name);
    t_CKINT result = 0;
    // check
    if( m_params.count( key ) > 0 && m_param_types[key] == ck_param_int )
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
t_CKFLOAT ChucK::getParamFloat( const std::string & name )
{
    // lower case for consistency
    std::string key = tolower(name);
    t_CKFLOAT result = 0;
    // check
    if( m_params.count( key ) > 0 && m_param_types[key] == ck_param_float )
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
std::string ChucK::getParamString( const std::string & name )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check
    if( m_params.count( key ) > 0 && m_param_types[key] == ck_param_string )
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
std::list< std::string > ChucK::getParamStringList( const std::string & name )
{
    // lower case for consistency
    std::string key = tolower(name);
    // check
    if( m_listParams.count( key ) > 0 &&
        m_param_types[key] == ck_param_string_list )
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
t_CKBOOL ChucK::init()
{
    // sanity check
    if( m_init == TRUE )
    {
        EM_error2( 0, "VM already initialized..." );
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
t_CKBOOL ChucK::initVM()
{
    // get VM params
    t_CKUINT srate = getParamInt( CHUCK_PARAM_SAMPLE_RATE );
    t_CKUINT outs = getParamInt( CHUCK_PARAM_OUTPUT_CHANNELS );
    t_CKUINT ins = getParamInt( CHUCK_PARAM_INPUT_CHANNELS );
    t_CKUINT adaptiveSize = getParamInt( CHUCK_PARAM_VM_ADAPTIVE );
    t_CKBOOL halt = getParamInt( CHUCK_PARAM_VM_HALT ) != 0;

    // instantiate VM
    m_carrier->vm = new Chuck_VM();
    // add reference (this will be released on shutdown)
    CK_SAFE_ADD_REF( m_carrier->vm );
    // reference back to carrier
    m_carrier->vm->setCarrier( m_carrier );
    // initialize VM
    if( !m_carrier->vm->initialize( srate, outs, ins, adaptiveSize, halt ) )
    {
        EM_error2( 0, "%s", m_carrier->vm->last_error() );
        return false;
    }

    return true;
}




//-----------------------------------------------------------------------------
// name: initCompiler()
// desc: initialize compiler
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::initCompiler()
{
    // get compiler params
    t_CKBOOL dump = getParamInt( CHUCK_PARAM_DUMP_INSTRUCTIONS ) != 0;
    t_CKBOOL auto_depend = getParamInt( CHUCK_PARAM_AUTO_DEPEND ) != 0;
    t_CKUINT deprecate = getParamInt( CHUCK_PARAM_DEPRECATE_LEVEL );
    std::string workingDir = getParamString( CHUCK_PARAM_WORKING_DIRECTORY );

    // log
    EM_log( CK_LOG_SYSTEM, "initializing compiler..." );
    // push indent level
    EM_pushlog();

    // instantiate compiler
    m_carrier->compiler = new Chuck_Compiler();
    // reference back to carrier
    m_carrier->compiler->setCarrier( m_carrier );

    // initialize compiler
    if( !m_carrier->compiler->initialize() )
    {
        EM_error2( 0, "compiler failed to initialize..." );
        return false;
    }
    // set dump flag
    m_carrier->compiler->emitter->dump = dump;
    // set auto depend flag (for type checker) | currently must be FALSE
    m_carrier->compiler->set_auto_depend( auto_depend );
    // set deprecation level
    m_carrier->env->deprecate_level = deprecate;

    // pop indent for compiler
    EM_poplog();
    // log
    EM_log( CK_LOG_SYSTEM, "initializing synthesis engine..." );
    // push indent for synthesis
    EM_pushlog();

    // VM + type system integration (needs to be done after compiler)
    if( !m_carrier->vm->initialize_synthesis() )
    {
        EM_error2( 0, "%s", m_carrier->vm->last_error() );
        return false;
    }

    // pop indent
    EM_poplog();

    std::string cwd;
    char * cstr_cwd = NULL; // 1.5.0.0 (barak) | was: char cstr_cwd[MAXPATHLEN];

    // figure out current working directory (added 1.3.0.0)
    // is this needed for current path to work correctly?!
    // was: if( getcwd(cstr_cwd, MAXPATHLEN) == NULL )
    // let getcwd allocate memory | 1.5.0.0 (barak)
    if( (cstr_cwd = getcwd(NULL, 0)) == NULL )
    {
        // uh...
        EM_log( CK_LOG_HERALD, "error: unable to determine current working directory!" );
    }
    else
    {
        // make c++ string
        cwd = std::string(cstr_cwd);
        // reclaim memory from getcwd
        CK_SAFE_FREE(cstr_cwd);
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

#ifdef __PLATFORM_WINDOWS__
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
t_CKBOOL ChucK::initChugins()
{
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;

    // print whether chugins enabled
    EM_log( CK_LOG_SYSTEM, "chugin system: %s", getParamInt( CHUCK_PARAM_CHUGIN_ENABLE ) ? "ON" : "OFF" );

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

        EM_pushlog();
        // print host version
        EM_log( CK_LOG_HERALD, TC::green("host API version: %d.%d", true).c_str(), CK_DLL_VERSION_MAJOR, CK_DLL_VERSION_MINOR );
        EM_poplog();

        //---------------------------------------------------------------------
        // set origin hint | 1.5.0.0 (ge) added
        m_carrier->compiler->m_originHint = ckte_origin_CHUGIN;
        //---------------------------------------------------------------------
        // log
        EM_log( CK_LOG_SYSTEM, "loading chugins..." );
        // push indent level
        // EM_pushlog();

        // chugin extension
        std::string extension = ".chug";
#ifdef __EMSCRIPTEN__
        // webchugins have extension ".chug.wasm" | 1.5.2.0 (terryzfeng) added
        extension = "chug.wasm";
#endif
        // load external libs | 1.5.0.4 (ge) enabled recursive search
        if( !compiler()->load_external_modules( extension.c_str(), dl_search_path, named_dls, TRUE ) )
        {
            // clean up
            goto error;
        }

        // pop log
        // EM_poplog();

        //---------------------------------------------------------------------
        // set origin hint | 1.5.0.0 (ge) added
        m_carrier->compiler->m_originHint = ckte_origin_IMPORT;
        //---------------------------------------------------------------------
        // log
        EM_log( CK_LOG_SYSTEM, "loading chuck extensions..." );
        EM_pushlog();

        // iterate over list of ck files that the compiler found
        for( std::list<std::string>::iterator j =
            compiler()->m_cklibs_to_preload.begin();
            j != compiler()->m_cklibs_to_preload.end(); j++)
        {
            // the filename
            std::string filename = *j;

            // log
            EM_log( CK_LOG_HERALD, "preloading '%s'...", filename.c_str() );
            // push indent
            EM_pushlog();

            // SPENCERTODO: what to do for full path
            std::string full_path = filename;

            // parse, type-check, and emit
            if( compiler()->go( filename, full_path ) )
            {
                // preserve op overloads | 1.5.1.5
                compiler()->env()->op_registry.preserve();
                // get the code
                code = compiler()->output();
                // name it | 1.5.2.0 (ge)
                code->name = std::string("pre-load ck file: ") + filename;

                // spork it
                shred = vm()->spork( code, NULL, TRUE );
            }
            else // did not compile
            {
                // undo any op overloads | 1.5.1.5
                compiler()->env()->op_registry.reset2local();
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

    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return true;

error: // 1.4.1.0 (ge) added

    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return false;
}




//-----------------------------------------------------------------------------
// name: probeChugins()
// desc: probe chugin system and print output
//-----------------------------------------------------------------------------
void ChucK::probeChugins()
{
    // ck files to pre load
    std::list<std::string> ck_libs_to_preload;
    // host verison
    std::ostringstream ostr; ostr << CK_DLL_VERSION_MAJOR << "." << CK_DLL_VERSION_MINOR;

    // print whether chugins enabled
    EM_log( CK_LOG_SYSTEM, "chugin system: %s", getParamInt( CHUCK_PARAM_CHUGIN_ENABLE ) ? "ON" : "OFF" );
    // print host version
    EM_log( CK_LOG_SYSTEM, "chugin host API version: %s", TC::green(ostr.str(),true).c_str() );
    // push
    EM_pushlog();
    // print version compatibility information
    EM_log( CK_LOG_SYSTEM, "chugin major version must == host major version" );
    EM_log( CK_LOG_SYSTEM, "chugin minor version must <= host minor version" );
    // print chuck language version
    EM_log( CK_LOG_SYSTEM, "language version: %s", TC::green(CHUCK_VERSION_STRING,true).c_str() );
    // pop
    EM_poplog();

    // chugin dur
    std::string chuginDir = getParamString( CHUCK_PARAM_CHUGIN_DIRECTORY );
    // list of search pathes (added 1.3.0.0)
    std::list<std::string> dl_search_path = getParamStringList( CHUCK_PARAM_USER_CHUGIN_DIRECTORIES );
    if( chuginDir != "" )
    {
        // add to search path
        dl_search_path.push_back( chuginDir );
    }
    // list of individually named chug-ins (added 1.3.0.0)
    std::list<std::string> named_dls = getParamStringList( CHUCK_PARAM_USER_CHUGINS );

    // log
    EM_log( CK_LOG_SYSTEM, "probing chugins (.chug)..." );
    // push indent level
    // EM_pushlog();

    // chugin extension
    std::string extension = ".chug";
#ifdef __EMSCRIPTEN__
    // webchugins have extension ".chug.wasm" | 1.5.2.0 (terryzfeng) added
    extension = "chug.wasm";
#endif

    // load external libs
    if( !Chuck_Compiler::probe_external_modules( extension.c_str(), dl_search_path, named_dls, TRUE, ck_libs_to_preload ) )
    {
        // warning
        EM_log( CK_LOG_SYSTEM, "error probing chugins..." );
    }
    // pop log
    // EM_poplog();

    // log
    EM_log( CK_LOG_SYSTEM, "probing auto-load chuck files (.ck)..." );
    EM_pushlog();

    // iterate over list of ck files that the compiler found
    for( std::list<std::string>::iterator j = ck_libs_to_preload.begin();
         j != ck_libs_to_preload.end(); j++ )
    {
        // the filename
        std::string filename = *j;
        // log
        EM_log( CK_LOG_SYSTEM, "[%s] '%s'...", TC::green("FOUND",true).c_str(), filename.c_str() );
    }

    // check
    if( ck_libs_to_preload.size() == 0 )
        EM_log( CK_LOG_INFO, "(no auto-load chuck files found)" );

    // pop log
    EM_poplog();
}




//-----------------------------------------------------------------------------
// name: initOTF()
// desc: init OTF programming system
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::initOTF()
{
#ifndef __DISABLE_OTF_SERVER__
    // server
    if( getParamInt( CHUCK_PARAM_OTF_ENABLE ) != 0 )
    {
        // get the port
        m_carrier->otf_port = getParamInt( CHUCK_PARAM_OTF_PORT );
        // log
        EM_log( CK_LOG_SYSTEM, "starting OTF command listener on port: %d...",
                m_carrier->otf_port );

        // start tcp server
        m_carrier->otf_socket = ck_tcp_create( 1 );
        if( !m_carrier->otf_socket ||
            !ck_bind( m_carrier->otf_socket, (int)m_carrier->otf_port ) ||
            !ck_listen( m_carrier->otf_socket, 10 ) )
        {
            // get how to handle the warning | 1.5.1.2
            t_CKINT printInsteadOfLog = getParamInt( CHUCK_PARAM_OTF_PRINT_WARNINGS )
                                        && getLogLevel() < CK_LOG_SYSTEM;
            // check
            if( printInsteadOfLog )
            {
                // old school print warning
                EM_error2( 0, "cannot bind to TCP port %li...", m_carrier->otf_port );
                // provide more information
                EM_error2( 0, " |- (cause: another chuck VM may currently have port %li open)", m_carrier->otf_port );
                EM_error2( 0, " |- (implication: this VM cannot receive OTF commands)" );
            }
            else
            {
                // push log
                EM_pushlog();

                // new school log warning, as the warning can be confusing / cause automation issues | 1.5.1.2
                // FYI see: https://github.com/ccrma/chuck/issues/352 (thanks @barak)
                EM_log( CK_LOG_SYSTEM, TC::orange("cannot bind to TCP port %li...", true).c_str(), m_carrier->otf_port );
                // log more information
                EM_log( CK_LOG_INFO, "possible cause: network device unavailable..." );
                EM_log( CK_LOG_INFO, "or another program currently has port %li open...", m_carrier->otf_port );
                EM_pushlog();
                EM_log( CK_LOG_INFO, "(e.g., another chuck VM)" );
                EM_poplog();
                EM_log( CK_LOG_INFO, "implication: this VM cannot receive OTF commands" );

                // pop log
                EM_poplog();
            }

            ck_close( m_carrier->otf_socket );
            m_carrier->otf_socket = NULL;
        }
        else
        {
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
            pthread_create( &m_carrier->otf_thread, NULL, otf_recv_cb, m_carrier );
#else
            m_carrier->otf_thread = CreateThread( NULL, 0,
                                                  (LPTHREAD_START_ROUTINE)otf_recv_cb,
                                                  m_carrier, 0, 0 );
#endif
        }
    }
    else
    {
        // log
        EM_log( CK_LOG_SYSTEM, "OTF command listener: OFF" );
    }
#endif
    return true;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown ChucK instance
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::shutdown()
{
    // log
    EM_log( CK_LOG_SYSTEM, "shutting down ChucK instance..." );
    // push
    EM_pushlog();

    // stop VM
    if( m_carrier != NULL && m_carrier->vm != NULL  )
    {
        m_carrier->vm->stop();

        // wait for it to stop...
        while( m_carrier->vm->running() )
        {
            ck_usleep(1000);
        }
    }

    // free vm, compiler, friends
    // first, otf
    // REFACTOR-2017 TODO: le_cb?

    // STK-specific
    stk_detach( m_carrier );

#ifndef __DISABLE_OTF_SERVER__
    // cancel otf thread
    if( m_carrier!= NULL )
    {
        // stop otf thread
        if( m_carrier->otf_thread )
        {
#if !defined(__PLATFORM_WINDOWS__) || defined(__WINDOWS_PTHREAD__)
            pthread_cancel( m_carrier->otf_thread );
#else
            CloseHandle( m_carrier->otf_thread ); // doesn't cancel thread
#endif
            m_carrier->otf_thread = 0; // signals thread shutdown
        }

        // close otf socket
        if( m_carrier->otf_socket )
        {
            ck_close( m_carrier->otf_socket );
        }

        // reset
        m_carrier->otf_socket = NULL;
        m_carrier->otf_port = 0;
    }
#endif // __DISABLE_OTF_SERVER__

    // TODO: a different way to unlock?
    // unlock all objects to delete chout, cherr
    Chuck_VM_Object::unlock_all();
    // ensure we have a carrier
    if( m_carrier != NULL )
    {
        CK_SAFE_RELEASE( m_carrier->chout );
        CK_SAFE_RELEASE( m_carrier->cherr );
    }
    // relock
    Chuck_VM_Object::lock_all();

    // ensure we have a carrier
    if( m_carrier != NULL )
    {
        // initiate VM shutdown but don't delete VM yet
        if( m_carrier->vm ) m_carrier->vm->shutdown();
        // delete compiler, including type system (m_carrier->env)
        CK_SAFE_DELETE( m_carrier->compiler );
        // verify the env pointer is NULL
        assert( m_carrier->env == NULL );
        // release VM (itself an Object)
        CK_SAFE_RELEASE( m_carrier->vm );
    }

    // clear flag
    m_init = FALSE;
    // clear flag
    m_started = FALSE;

    // log
    EM_log( CK_LOG_SYSTEM, "ChucK instance shutdown complete." );
    // pop
    EM_poplog();

    // done
    return true;
}




//-----------------------------------------------------------------------------
// name: compileFile()
// desc: compile a file -> chuck bytecode -> spork as new shred
//       `argsTogether` is appended to `path` as additional arguments
//       the format of `argsTogether` is ':'-separated items e.g., "1:foo:5"
//       NOTE: `path` could have arguments already, e.g., "program.ck:5.5:bar"
//       if `immediate` == TRUE, the new shred(s) is shreduled immediately
//           (on the calling thread)
//       if `immediate` == FALSE, the new shreds(s) is queued and shreduled on
//           the next time step (by the VM compute/audio thread)
//       `count` specifies how many instances of the new shred to spork
//       returns TRUE if compilation successful (even if count == 0)
//       returns FALSE if compilation unsuccessful
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::compileFile( const std::string & path,
                             const std::string & argsTogether,
                             t_CKUINT count, t_CKBOOL immediate,
                             std::vector<t_CKUINT> * shredIDs)
{
    // clear | 1.5.0.8
    if( shredIDs ) shredIDs->clear();

    // sanity check
    if( !m_carrier->compiler )
    {
        // error
        EM_error2( 0, "compileFile() invoked before initialization..." );
        return FALSE;
    }

    std::string filename;
    std::vector<std::string> args;
    Chuck_VM_Code * code = NULL;
    Chuck_VM_Shred * shred = NULL;
    std::string thePath;
    std::string full_path;

    //-------------------------------------------------------------------------
    // set origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_USERDEFINED;
    //-------------------------------------------------------------------------

    // log
    EM_log( CK_LOG_INFO, "compiling '%s'...", path.c_str() );
    // push indent
    EM_pushlog();

    // path expansion (e.g., ~ for unix systems)
    thePath = expand_filepath(path);
    // log if the path exansion is different
    if( path != thePath )
    { EM_log( CK_LOG_FINE, "expanding path '%s'...", thePath.c_str() ); }

    // append
    std::string theThing = thePath + ":" + argsTogether;
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
        EM_error2( 0, "malformed filename with argument list..." );
        EM_error2( 0, "    -->  '%s'", theThing.c_str() );
        goto error;
    }

    // construct full path to be associated with the file so me.sourceDir() works
    // (added 1.3.0.0)
    full_path = get_full_path(filename);

    // parse, type-check, and emit (full_path added 1.3.0.0)
    if( !m_carrier->compiler->go( filename, full_path ) )
        goto error;

    // get the code
    code = m_carrier->compiler->output();
    // (re) name it | 1.5.0.5 (ge) update from '+=' to '='
    code->name = path;

    // log
    EM_log( CK_LOG_FINE, "sporking %d %s...", count,
           count == 1 ? "instance" : "instances" );

    // spork it
    while( count > 0 )
    {
        // spork shred from code; shredule immediately or deferred;
        // as of 1.5.0.8, spork allocates the shred ID regardless...
        // previously for emscripten `immediate` => TRUE; this is no longer necessary
        shred = m_carrier->vm->spork( code, NULL, immediate );

        // add args
        shred->args = args;
        // append the new ID | 1.5.0.8
        if( shredIDs ) shredIDs->push_back( shred->xid );
        // decrement count
        count--;
    }

    // pop indent
    EM_poplog();
    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return TRUE;

error: // 1.5.0.0 (ge) added

    // pop indent
    EM_poplog();
    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: compileCode()
// desc: compile code/text -> chuck bytecode -> spork as new shred
//       `argsTogether` is appended to `path` as arguments
//       the format of `argsTogether` is ':'-separated items e.g., "1:foo:5"
//       if `immediate` == TRUE, the new shred(s) is shreduled immediately
//           (on the calling thread)
//       if `immediate` == FALSE, the new shreds(s) is queued and shreduled on
//           the next time step (on the VM compute/audio thread)
//       returns TRUE if compilation successful (even if count == 0)
//       returns FALSE if compilation unsuccessful
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::compileCode( const std::string & code,
                             const std::string & argsTogether,
                             t_CKUINT count, t_CKBOOL immediate,
                             std::vector<t_CKUINT> * shredIDs )
{
    // clear | 1.5.0.8
    if( shredIDs ) shredIDs->clear();

    // sanity check
    if( !m_carrier->compiler )
    {
        // error
        EM_error2( 0, "compileCode() invoked before initialization..." );
        return FALSE;
    }

    std::vector<std::string> args;
    Chuck_VM_Code * vm_code = NULL;
    Chuck_VM_Shred * shred = NULL;
    std::string workingDir;
    std::string full_path;

    //-------------------------------------------------------------------------
    // set origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_USERDEFINED;
    //-------------------------------------------------------------------------

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
        EM_error2( 0, "malformed filename with argument list..." );
        EM_error2( 0, "    -->  '%s'", theThing.c_str() );
        goto error;
    }

    // working directory
    workingDir = getParamString( CHUCK_PARAM_WORKING_DIRECTORY );

    // construct full path to be associated with the file so me.sourceDir() works
    full_path = workingDir + "/compiled.code";
    // log
    EM_log( CK_LOG_FINE, "full path: %s...", full_path.c_str() );

    // parse, type-check, and emit (full_path added 1.3.0.0)
    if( !m_carrier->compiler->go( "<compiled.code>", full_path, code ) )
       goto error;

    // get the code
    vm_code = m_carrier->compiler->output();
    // (re) name it (no path to append) | 1.5.0.5 (ge) update from '+=' to '='
    vm_code->name = "compiled.code";

    // log
    EM_log( CK_LOG_FINE, "sporking %d %s...", count,
            count == 1 ? "instance" : "instances" );

    // spork it
    while( count > 0 )
    {
        // spork shred from code; shredule immediately or deferred;
        // as of 1.5.0.8, spork allocates the shred ID regardless...
        // previously for emscripten `immediate` => TRUE; this is no longer necessary
        shred = m_carrier->vm->spork( vm_code, NULL, immediate );

        // add args
        shred->args = args;
        // append the new ID | 1.5.0.8
        if( shredIDs ) shredIDs->push_back( shred->xid );
        // decrement count
        count--;
    }

    // pop indent
    EM_poplog();
    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return TRUE;

error: // 1.5.0.0 (ge) added

    // pop indent
    EM_poplog();
    // unset origin hint | 1.5.0.0 (ge) added
    m_carrier->compiler->m_originHint = ckte_origin_UNKNOWN;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: start()
// desc: start chuck instance
//-----------------------------------------------------------------------------
t_CKBOOL ChucK::start()
{
    // sanity check
    if( m_carrier->vm == NULL )
    {
        EM_error2( 0, "cannot start, VM not initialized..." );
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
// desc: run ChucK and synthesize audio for `numFrames`
//       `input`: the incoming input array of audio samples
//       `output`: the outgoing output array of audio samples
//       `numFrames` : the number of audio frames to run
//-----------------------------------------------------------------------------
void ChucK::run( const SAMPLE * input, SAMPLE * output, t_CKINT numFrames )
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
    HidInManager::cleanup( 100 );
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
t_CKBOOL ChucK::setMainThreadHook(Chuck_DL_MainThreadHook * hook)
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




//-----------------------------------------------------------------------------
// name: toggleGlobalColorTextoutput
// set whether chuck will generate color output for its messages
// this will set the corresponding parameter as well
//-----------------------------------------------------------------------------
void ChucK::toggleGlobalColorTextoutput( t_CKBOOL onOff )
{
    this->setParam( CHUCK_PARAM_TTY_COLOR, onOff );
}
