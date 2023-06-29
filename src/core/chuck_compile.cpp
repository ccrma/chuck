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
// file: chuck_compile.cpp
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2005 - original
//-----------------------------------------------------------------------------
#include "chuck_compile.h"
#include "chuck_lang.h"
#include "chuck_errmsg.h"
#include "chuck_io.h"
#ifndef  __DISABLE_OTF_SERVER__
#include "chuck_otf.h"
#endif

#include "ugen_osc.h"
#include "ugen_xxx.h"
#include "ugen_filter.h"
#include "ugen_stk.h"
#include "uana_xform.h"
#include "uana_extract.h"

#include "ulib_ai.h"
#include "ulib_doc.h"
#include "ulib_machine.h"
#include "ulib_math.h"
#include "ulib_std.h"
#include "util_string.h"

#ifndef __DISABLE_NETWORK__
#include "ulib_opsc.h"
#endif

#if defined(__PLATFORM_WIN32__)
  #include "dirent_win32.h"
#endif

//#if defined(__WINDOWS_PTHREAD__)
#include <sys/stat.h>
//#endif

#include <string>
#include <vector>
#include <algorithm>
using namespace std;




// function prototypes
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler );
t_CKBOOL load_external_modules( Chuck_Compiler * compiler,
                                const char * extension,
                                std::list<std::string> & chugin_search_paths,
                                std::list<std::string> & named_dls);
t_CKBOOL load_module( Chuck_Compiler * compiler, Chuck_Env * env,
                      f_ck_query query, const char * name, const char * nspc );




//-----------------------------------------------------------------------------
// name: Chuck_Compiler()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Compiler::Chuck_Compiler()
{
    emitter = NULL;
    code = NULL;

    // REFACTOR-2017: add carrier
    m_carrier = NULL;
    // initialize
    m_auto_depend = FALSE;

    // origin hint | 1.5.0.0 (ge) added
    m_originHint = te_originUnknown;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Compiler()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Compiler::~Chuck_Compiler()
{
    // call shutdown
    this->shutdown();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize the compiler
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::initialize()
{
    // set origin hint
    m_originHint = te_originBuiltin;

    // allocate the type checker
    Chuck_Env * env = type_engine_init( m_carrier );
    // add reference
    env->add_ref();

    // allocate the emitter
    emitter = emit_engine_init( env );
    // add reference
    emitter->add_ref();
    // set auto depend to 0
    m_auto_depend = FALSE;

    // load internal libs
    if( !load_internal_modules( this ) )
        goto error;

    // unset origin hint
    m_originHint = te_originUnknown;

    return TRUE;

error:
    // clean up | 1.4.1.0 (ge) removed -- should be cleaned up outside
    // this->shutdown();

    // pop indent
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: shutdown()
// desc: shutdown the compiler
//-----------------------------------------------------------------------------
void Chuck_Compiler::shutdown()
{
    // log
    EM_log( CK_LOG_SYSTEM, "shutting down compiler..." ) ;
    // push indent
    EM_pushlog();

    // TODO: free
    type_engine_shutdown( env() );
    // TODO: check if emitter gets cleaned up
    // emit_engine_shutdown( emitter );
    emitter = NULL;
    code = NULL;
    m_auto_depend = FALSE;
    m_recent.clear();

    for(std::list<Chuck_DLL *>::iterator i = m_dlls.begin();
        i != m_dlls.end(); i++)
    {
        delete (*i);
    }

    m_dlls.clear();

    // pop indent
    EM_poplog();
}





//-----------------------------------------------------------------------------
// name: bind()
// desc: bind a new type system module, via query function
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::bind( f_ck_query query_func, const std::string & name,
                               const std::string & nspc )
{
    // log
    EM_log( CK_LOG_SYSTEM, "on-demand binding compiler module '%s'...",
            name.c_str() );
    // push indent level
    EM_pushlog();

    // get env
    Chuck_Env * env = this->env();
    // make context
    Chuck_Context * context = type_engine_make_context(
        NULL, string("@[bind:") + name + string("]") );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // do it
    if( !load_module( this, env, query_func, name.c_str(), nspc.c_str() ) ) goto error;

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    // pop indent level
    EM_poplog();

    return TRUE;

error:

    // probably dangerous: rollback
    env->global()->rollback();

    // clear context
    type_engine_unload_context( env );

    // pop indent level
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: set_auto_depend()
// desc: auto dependency resolve for types
//-----------------------------------------------------------------------------
void Chuck_Compiler::set_auto_depend( t_CKBOOL v )
{
    // log
    EM_log( CK_LOG_SYSTEM, "type dependency resolution: %s",
            v ? "AUTO" : "MANUAL" );
    m_auto_depend = v;
}




//-----------------------------------------------------------------------------
// name: go()
// desc: parse, type-check, and emit a program
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::go( const string & filename,
                             const string & full_path,
                             const string & codeLiteral )
{
    t_CKBOOL ret = TRUE;
    // Chuck_Context * context = NULL;

    EM_reset_msg();

    // check to see if resolve dependencies automatically
    if( !m_auto_depend )
    {
        // normal (note: full_path added 1.3.0.0)
        ret = this->do_normal_depend( filename, codeLiteral, full_path );
    }
    else // auto depend
    {
        // call auto-depend compile (1.4.1.0)
        ret = this->do_auto_depend( filename, codeLiteral, full_path );
    }

    // 1.4.1.0 (ge) | added to unset the fileName reference, which determines
    // how messages print to console (e.g., [file.ck]: or [chuck]:)
    EM_change_file( NULL );

    return ret;
}




//-----------------------------------------------------------------------------
// name: set_file2parse()
// desc: set a FILE * for one-time use on next go(); see header for details
//-----------------------------------------------------------------------------
void Chuck_Compiler::set_file2parse( FILE * fd, t_CKBOOL autoClose )
{
    // call down to parser
    ::fd2parse_set( fd, autoClose );
}




//-----------------------------------------------------------------------------
// name: resolve()
// desc: resolve type automatically - if auto_depend is off, return FALSE
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::resolve( const string & type )
{
    t_CKBOOL ret = TRUE;

    // check auto_depend
    if( !m_auto_depend )
        return FALSE;

    // look up if name is already parsed



    return ret;
}




//-----------------------------------------------------------------------------
// name: setReplaceDac()
// desc: tell the compiler whether dac should be replaced in scripts
//       with the name of an external UGen, and if so which one
//-----------------------------------------------------------------------------
void Chuck_Compiler::setReplaceDac( t_CKBOOL shouldReplaceDac,
    const std::string & replacement )
{
    emitter->should_replace_dac = shouldReplaceDac;
    emitter->dac_replacement = replacement;
}




//-----------------------------------------------------------------------------
// name: do_entire_file()
// desc: parse, type-check, and emit a program
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_entire_file( Chuck_Context * context )
{
    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env(), g_program, te_do_all ) )
         return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), g_program, te_do_all ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), g_program, te_do_all ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_all ) )
        return FALSE;

    // emit (pass 4)
    if( !emit_engine_emit_prog( emitter, g_program ) )
        return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL_DONE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: do_only_classes()
// desc: compile only classes definitions
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_only_classes( Chuck_Context * context )
{
    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env(), g_program, te_do_classes_only ) )
        return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), g_program, te_do_classes_only ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), g_program, te_do_classes_only ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_classes_only ) )
        return FALSE;

    // emit (pass 4)
    code = emit_engine_emit_prog( emitter, g_program , te_do_classes_only );
    if( !code ) return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL_DONE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: do_all_except_classes()
// desc: compile everything except classes
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_all_except_classes( Chuck_Context * context )
{
    // 0th scan only deals with classes, so is not needed

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), g_program, te_do_no_classes ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), g_program, te_do_no_classes ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_no_classes ) )
        return FALSE;

    // emit (pass 4)
    code = emit_engine_emit_prog( emitter, g_program, te_do_no_classes );
    if( !code ) return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL_DONE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: do_normal_depend()
// desc: compile normally without auto-depend
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_normal_depend( const string & filename,
                                           const string & codeLiteral,
                                           const string & full_path )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    // expand
    string theFilename = expand_filepath(filename);
    string theFullpath = expand_filepath(full_path);

    // parse the code
    if( !chuck_parse( theFilename, codeLiteral ) )
        return FALSE;

    // make the context
    context = type_engine_make_context( g_program, theFilename );
    if( !context ) return FALSE;

    // remember full path (added 1.3.0.0)
    context->full_path = theFullpath;

    // reset the env
    env()->reset();

    // load the context
    if( !type_engine_load_context( env(), context ) )
        return FALSE;

    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env(), g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env(), g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env(), g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // check the program (pass 3)
    if( !type_engine_check_context( env(), context, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // emit (pass 4)
    code = emit_engine_emit_prog( emitter, g_program, te_do_all );
    if( !code ) { ret = FALSE; goto cleanup; }

cleanup:

    // commit
    if( ret ) env()->global()->commit();
    // or rollback
    else env()->global()->rollback();

    // unload the context from the type-checker
    if( !type_engine_unload_context( env() ) )
    {
        EM_error2( 0, "internal error unloading context...\n" );
        return FALSE;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: do_auto_depend()
// desc: compile with auto-depend (TODO: this doesn't work yet, I don't think)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_auto_depend( const string & filename,
                                         const string & codeLiteral,
                                         const string & full_path )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    // parse the code
    if( !chuck_parse( filename, codeLiteral ) )
        return FALSE;

    // make the context
    context = type_engine_make_context( g_program, filename );
    if( !context ) return FALSE;

    // reset the env
    env()->reset();

    // load the context
    if( !type_engine_load_context( env(), context ) )
        return FALSE;

    // do entire file
    if( !do_entire_file( context ) )
    { ret = FALSE; goto cleanup; }

    // get the code
    code = context->code();
    if( !code )
    {
        ret = FALSE;
        EM_error2( 0, "internal error: context->code() NULL!" );
        goto cleanup;
    }

cleanup:

    // commit
    if( ret ) env()->global()->commit();
    // or rollback
    else env()->global()->rollback();

    // unload the context from the type-checker
    if( !type_engine_unload_context( env() ) )
    {
        EM_error2( 0, "internal error unloading context...\n" );
        return FALSE;
    }

    return ret;
}





//-----------------------------------------------------------------------------
// name: find_recent_path()
// desc: find recent context by path
//-----------------------------------------------------------------------------
Chuck_Context * Chuck_Compiler::find_recent_path( const string & path )
{
    return NULL;
}




//-----------------------------------------------------------------------------
// name: find_recent_type()
// desc: find recent context by type name
//-----------------------------------------------------------------------------
Chuck_Context * Chuck_Compiler::find_recent_type( const string & type )
{
    return NULL;
}




//-----------------------------------------------------------------------------
// name: add_recent_path()
// desc: add recent context by path
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::add_recent_path( const string & path,
                                          Chuck_Context * context )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: output()
// desc: get the code generated by the last do()
//-----------------------------------------------------------------------------
Chuck_VM_Code * Chuck_Compiler::output()
{
    return this->code;
}




//-----------------------------------------------------------------------------
// name: load_module()
// desc: load a dll and add it
//-----------------------------------------------------------------------------
t_CKBOOL load_module( Chuck_Compiler * compiler, Chuck_Env * env, f_ck_query query,
                      const char * name, const char * nspc )
{
    Chuck_DLL * dll = NULL;
    t_CKBOOL query_failed = FALSE;

    // load osc
    dll = new Chuck_DLL( compiler->carrier(), name );
    // (fixed: 1.3.0.0) query_failed now catches either failure of load or query
    query_failed = !(dll->load( query ) && dll->query());
    if( query_failed || !type_engine_add_dll( env, dll, nspc ) )
    {
        CK_FPRINTF_STDERR(
                 "[chuck]: internal error loading module '%s.%s'...\n",
                 nspc, name );
        if( query_failed )
            CK_FPRINTF_STDERR( "       %s", dll->last_error() );

        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_internal_modules()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler )
{
    // log
    EM_log( CK_LOG_SEVERE, "loading built-in modules..." );
    // push indent level
    EM_pushlog();

    // get env
    Chuck_Env * env = compiler->env();
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[internal]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // still compile MidiMsg even if __DISABLE_MIDI__
    if( !init_class_Midi( env ) ) goto error;
#ifndef __DISABLE_MIDI__
    if( !init_class_MidiRW( env ) ) goto error;
#endif // __DISABLE_MIDI__

    // load
    EM_log( CK_LOG_SEVERE, "module 'math'" );
    if( !load_module( compiler, env, libmath_query, "Math", "global" ) ) goto error;
    EM_log( CK_LOG_SEVERE, "module 'osc'" );
    load_module( compiler, env, osc_query, "osc", "global" );
    EM_log( CK_LOG_SEVERE, "module 'ai'" );
    if( !load_module( compiler, env, libai_query, "AI", "global" ) ) goto error;
    EM_log( CK_LOG_SEVERE, "module 'extract'" );
    load_module( compiler, env, extract_query, "extract", "global" );
    EM_log( CK_LOG_SEVERE, "module 'filter'" );
    load_module( compiler, env, filter_query, "filter", "global" );
    EM_log( CK_LOG_SEVERE, "module 'STK'" );
    load_module( compiler, env, stk_query, "stk", "global" );
    EM_log( CK_LOG_SEVERE, "module 'xform'" );
    load_module( compiler, env, xform_query, "xform", "global" );
    EM_log( CK_LOG_SEVERE, "module 'xxx'" );
    load_module( compiler, env, xxx_query, "xxx", "global" );
    EM_log( CK_LOG_SEVERE, "module 'std'" );
    if( !load_module( compiler, env, libstd_query, "Std", "global" ) ) goto error;

    // load
    EM_log( CK_LOG_SEVERE, "module 'machine'" );
    if( !load_module( compiler, env, machine_query, "Machine", "global" ) ) goto error;
    #ifndef __DISABLE_OTF_SERVER__
    machine_init( compiler, otf_process_msg );
    #endif

    #ifndef __DISABLE_NETWORK__
    EM_log( CK_LOG_SEVERE, "module 'opsc'" );
    if( !load_module( compiler, env, opensoundcontrol_query, "opsc", "global" ) ) goto error;
    #endif

    // deprecated:
    // if( !load_module( compiler, env, net_query, "net", "global" ) ) goto error;

    #ifndef __DISABLE_HID__
    EM_log( CK_LOG_SEVERE, "module 'HID'" );
    if( !init_class_HID( env ) ) goto error;
    #endif

    #ifndef __DISABLE_SERIAL__
    EM_log( CK_LOG_SEVERE, "module 'SerialIO'" );
    if( !init_class_serialio( env ) ) goto error;
    #endif

    EM_log( CK_LOG_SEVERE, "module 'CKDoc'" );
    if( !load_module( compiler, env, ckdoc_query, "CKDoc", "global" ) ) goto error;

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    // pop indent level
    EM_poplog();

    return TRUE;

error:

    // probably dangerous: rollback
    env->global()->rollback();

    // clear context
    type_engine_unload_context( env );

    // pop indent level
    EM_poplog();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: getDirEntryAttribute()
// desc: get directory entry attribute | 1.5.0.4 (ge) moved into function
//-----------------------------------------------------------------------------
t_CKBOOL getDirEntryAttribute( dirent * de, t_CKBOOL & isDirectory, t_CKBOOL & isRegular )
{
#if defined(__PLATFORM_WIN32__)
    isDirectory = de->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    isRegular = ((de->data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ||
                  (de->data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ||
                  (de->data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE));
#elif defined(__WINDOWS_PTHREAD__) // Cygwin -- doesn't have dirent d_type
    std::string absolute_path = std::string(directory) + "/" + de->d_name;
    struct stat st;
    if( stat(absolute_path.c_str(), &st) == 0 )
    {
        isDirectory = st.st_mode & S_IFDIR;
        isRegular = st.st_mode & S_IFREG;
    }
    else
    {
        // uhh ...
        EM_log( CK_LOG_INFO,
               "unable to open file '%s', ignoring for chugins",
               absolute_path.c_str() );
        return false;
    }
#else
    isDirectory = de->d_type == DT_DIR;
    isRegular = de->d_type == DT_REG;
#endif

    return true;
}




//-----------------------------------------------------------------------------
// name: struct ChuginFileInfo()
// desc: file info for loading
//-----------------------------------------------------------------------------
struct ChuginFileInfo
{
    string filename;
    string path;
    bool isBundle;

    // constructor
    ChuginFileInfo() { isBundle = false;  }

    // constructor
    ChuginFileInfo( const string & f, const string & p, bool bundle = false )
    {
        filename = f; path = p; isBundle = bundle;
    }
};
// compare function for sorting ChuginFileInfo
static bool comp_func_chuginfileinfo( const ChuginFileInfo & a, const ChuginFileInfo & b )
{
    return tolower( a.filename ) < tolower( b.filename );
}




//-----------------------------------------------------------------------------
// name: format_dir_name_for_display()
// desc: format directory name for display, considering underlying platform
//-----------------------------------------------------------------------------
static string format_dir_name_for_display( const string & path )
{
#ifndef __PLATFORM_WIN32__
    return path;
#else // not __PLATFORM_WIN32__
    return expand_filepath( path );
#endif
}



//-----------------------------------------------------------------------------
// name: scan_external_modules_in_directory()
// desc: scan all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL scan_external_modules_in_directory( const char * directory,
    const char * extension, t_CKBOOL recursiveSearch,
    vector<ChuginFileInfo> & chugins2load, vector<string> & dirs2search,
    vector<string> & ckfiles2load )
{
    // expand directory path
    string path = expand_filepath( string(directory), FALSE );
    // open the directory
    DIR * dir = opendir( path.c_str() );

    // cannot open
    if( !dir ) return FALSE;

    // do first read | 1.5.0.0 (ge + eito) #chunreal
    struct dirent * de = readdir( dir );
    // while( (de = readdir(dir)) ) <- UE5 forces us to not do this
    while( de != NULL )
    {
        t_CKBOOL is_regular = false;
        t_CKBOOL is_directory = false;
        // get attributes
        if( !getDirEntryAttribute( de, is_directory, is_regular ) ) continue;

        // check | TODO: follow links?
        if( is_regular )
        {
            // check for ck files to pre-load
            if( extension_matches( de->d_name, ".ck" ) )
            {
                // construct absolute path
                std::string absolute_path = path + "/" + de->d_name;
                // append file path
                ckfiles2load.push_back( absolute_path );
            }
            // check extension passed in
            else if( extension_matches( de->d_name, extension ) )
            {
                // construct absolute path
                std::string absolute_path = path + "/" + de->d_name;
                // append module
                chugins2load.push_back( ChuginFileInfo( de->d_name, absolute_path ) );
            }
        }
        // recursive search? into directories...
        // (whose name doesn't end in .chug, unless it was precisely .chug)
        else if( recursiveSearch && is_directory && subdir_ok2recurse( de->d_name, extension ) )
        {
            // recurse | TODO: max depth?
            if( strncmp( de->d_name, ".", sizeof( "." ) ) != 0 &&
                strncmp( de->d_name, "..", sizeof( ".." ) ) != 0 )
            {
                // construct absolute path (use the non-expanded path for consistency when printing)
                std::string absolute_path = string(directory) + "/" + de->d_name;
                // queue search in sub-directory
                dirs2search.push_back( absolute_path );
            }
        }
#ifdef __PLATFORM_MACOSX__
        else if( is_directory && extension_matches( de->d_name, extension ) )
        {
            // On macOS, a chugin can either be a regular file (a .dylib simply renamed to .chug)
            // or it can be a MODULE like using CMake with add_library(FooChugin MODULE foo.cpp)
            // If it's a module bundle, then because .chug is a nonstandard extension,
            // on the filesystem it shows up as a directory ending in .chug.
            // If we see one of these directories, we can dive directly to the Contents/MacOS subfolder
            // and look for a regular file. | 1.5.0.1 (dbraun) added
            std::string absolute_path = path + "/" + de->d_name + "/Contents/MacOS";
            // probe NAME.chug/Contents/MacOS/NAME
            string actualName = extension_removed( de->d_name, extension );
            // construct
            absolute_path += "/" + actualName;
            // get cstr
            const char * subdirectory = absolute_path.c_str();
            // load directly
            chugins2load.push_back( ChuginFileInfo( de->d_name, subdirectory, true ) );
        }
#endif // #ifdef __PLATFORM_MACOSX__

        // read next | 1.5.0.0 (ge) moved here due to #chunreal
        de = readdir( dir );
    }

    // close
    closedir( dir );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_external_module_at_path()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_external_module_at_path( Chuck_Compiler * compiler,
                                       const char * name,
                                       const char * dl_path )
{
    Chuck_Env * env = compiler->env();

    EM_log(CK_LOG_SEVERE, "loading chugin '%s'", name);

    Chuck_DLL * dll = new Chuck_DLL( compiler->carrier(), name );
    t_CKBOOL query_failed = FALSE;

    // try to load and query
    query_failed = !(dll->load(dl_path) && dll->query());
    if( query_failed || !type_engine_add_dll2(env, dll, "global"))
    {
        EM_pushlog();
        EM_log(CK_LOG_SEVERE, "cannot load '%s', skipping...", name);
        // EM_error2( 0, "error: cannot load '%s', skipping...", name );
        if( query_failed )
        {
            EM_log( CK_LOG_SEVERE, "reason: %s", dll->last_error() );
            // EM_error2( 0, "%s", dll->last_error() );
        }
        delete dll;
        EM_poplog();

        return FALSE;
    }
    else
    {
        compiler->m_dlls.push_back(dll);
        return TRUE;
    }
}




//-----------------------------------------------------------------------------
// name: load_external_modules_in_directory()
// desc: load all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL load_external_modules_in_directory( Chuck_Compiler * compiler,
                                             const char * directory,
                                             const char * extension,
                                             t_CKBOOL recursiveSearch )
{
    vector<ChuginFileInfo> chugins2load;
    vector<string> dirs2search;
    vector<string> ckfiles2load;

    // print directory to examine
    EM_log( CK_LOG_SEVERE, "searching '%s'", format_dir_name_for_display(directory).c_str() );
    // push
    EM_pushlog();

    // scan
    t_CKBOOL retval = scan_external_modules_in_directory( directory, extension, recursiveSearch, chugins2load, dirs2search, ckfiles2load );
    if( !retval )
    {
        // log (1.3.1.2: changed to 2 lines to stay within 80 chars)
        EM_log( CK_LOG_INFO, "unable to open directory..." );
        EM_log( CK_LOG_INFO, "ignoring for chugins..." );
        // pop
        EM_poplog();
        // go ahead and return true
        return TRUE;
    }

    // sort
    sort( chugins2load.begin(), chugins2load.end(), comp_func_chuginfileinfo );
    sort( dirs2search.begin(), dirs2search.end() );
    sort( ckfiles2load.begin(), ckfiles2load.end() );

    // loop over chugins to load
    for( t_CKINT i = 0; i < chugins2load.size(); i++ )
    {
        // load module
        t_CKBOOL retval = load_external_module_at_path( compiler, chugins2load[i].filename.c_str(),
            chugins2load[i].path.c_str() );
        // if no error
        if( chugins2load[i].isBundle && retval ) {
            // log
            EM_pushlog();
            EM_log( CK_LOG_INFO, "macOS bundle was detected..." );
            string shortenSubdir = chugins2load[i].path;
            shortenSubdir = shortenSubdir.substr( shortenSubdir.find( chugins2load[i].filename ) );
            EM_log( CK_LOG_INFO, "loaded %s", shortenSubdir.c_str() );
            EM_poplog();
        }
    }

    // loop over ck files to load (later)
    for( t_CKINT i = 0; i < ckfiles2load.size(); i++ )
    {
        // save for later
        compiler->m_cklibs_to_preload.push_back( ckfiles2load[i] );
    }

    // pop log
    EM_poplog();

    // loop over dirs2 to search | if not recursive, this should be empty
    for( t_CKINT i = 0; i < dirs2search.size(); i++ )
    {
        // search in dir
        load_external_modules_in_directory( compiler, dirs2search[i].c_str(), extension, recursiveSearch );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_external_modules()
// desc: load external modules (e.g., .chug and .ck library files)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::load_external_modules( const char * extension,
                                                std::list<std::string> & chugin_search_paths,
                                                std::list<std::string> & named_dls,
                                                t_CKBOOL recursiveSearch )
{
    // get env
    Chuck_Env * env = this->env();
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[external]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // first load dynamic libraries explicitly named on the command line
    for( std::list<std::string>::iterator i_dl = named_dls.begin();
         i_dl != named_dls.end(); i_dl++ )
    {
        // get chugin name
        std::string & dl_path = *i_dl;
        // check extension, append if no match
        if( !extension_matches(dl_path.c_str(), extension) )
            dl_path += extension;
        // load the module
        load_external_module_at_path( this, dl_path.c_str(), dl_path.c_str() );
    }

    // now recurse through search paths and load any DLs or .ck files found
    for( std::list<std::string>::iterator i_sp = chugin_search_paths.begin();
         i_sp != chugin_search_paths.end(); i_sp++ )
    {
        // search directory and load contents
        load_external_modules_in_directory( this, (*i_sp).c_str(), extension, recursiveSearch );
    }

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: probe_external_module_at_path()
// desc: probe external module
//-----------------------------------------------------------------------------
t_CKBOOL probe_external_module_at_path( const char * name, const char * dl_path )
{
    // create dynamic module
    Chuck_DLL * dll = new Chuck_DLL( NULL, name );

    // load the dll, lazy mode
    if( dll->load(dl_path, CK_QUERY_FUNC, TRUE) )
    {
        // probe it
        dll->probe();
        // if not compatible
        if( dll->compatible() )
        {
            // print
            EM_log( CK_LOG_SYSTEM, "[OK] chugin %s (%d.%d)", name, dll->versionMajor(), dll->versionMinor() );
        }
        else
        {
            // print
            EM_log( CK_LOG_SYSTEM, "[FAILED] chugin %s (%d.%d)", name, dll->versionMajor(), dll->versionMinor() );
            // push
            EM_pushlog();
            EM_log( CK_LOG_SYSTEM, "reason: %s", dll->last_error() );
            EM_poplog();

        }
    }
    else
    {
        // print
        EM_log( CK_LOG_SYSTEM, "[FAILED] chugin '%s' load...", name );
        // more info
        EM_pushlog();
        EM_log( CK_LOG_SYSTEM, "reason: %s", dll->last_error() );
        EM_poplog();
    }

    // done
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: probe_external_modules_in_directory()
// desc: probe all external modules by extension within a directory
//-----------------------------------------------------------------------------
t_CKBOOL probe_external_modules_in_directory( const char * directory,
                                              const char * extension,
                                              t_CKBOOL recursiveSearch,
                                              std::list<std::string> & ck_libs )
{
    vector<ChuginFileInfo> chugins2load;
    vector<string> dirs2search;
    vector<string> ckfiles2load;

    // print directory to examine
    EM_log( CK_LOG_SYSTEM, "searching '%s'", format_dir_name_for_display( directory ).c_str() );
    // push
    EM_pushlog();

    // scan
    t_CKBOOL retval = scan_external_modules_in_directory( directory, extension, recursiveSearch, chugins2load, dirs2search, ckfiles2load );
    if( !retval )
    {
        // log (1.3.1.2: changed to 2 lines to stay within 80 chars)
        EM_log( CK_LOG_INFO, "unable to open directory..." );
        EM_log( CK_LOG_INFO, "ignoring for chugins..." );
        // pop log
        EM_poplog();
        // go ahead and return true
        return TRUE;
    }

    // sort
    sort( chugins2load.begin(), chugins2load.end(), comp_func_chuginfileinfo );
    sort( dirs2search.begin(), dirs2search.end() );
    sort( ckfiles2load.begin(), ckfiles2load.end() );

    // loop over chugins to load
    for( t_CKINT i = 0; i < chugins2load.size(); i++ )
    {
        // load module
        t_CKBOOL retval = probe_external_module_at_path(
            chugins2load[i].filename.c_str(), chugins2load[i].path.c_str() );
        // if no error
        if( chugins2load[i].isBundle && retval ) {
            // log
            EM_pushlog();
            EM_log( CK_LOG_INFO, "macOS bundle was detected..." );
            string shortenSubdir = chugins2load[i].path;
            shortenSubdir = shortenSubdir.substr( shortenSubdir.find( chugins2load[i].filename ) );
            EM_log( CK_LOG_INFO, "loaded %s", shortenSubdir.c_str() );
            EM_poplog();
        }
    }

    // loop over ck files to load (later)
    for( t_CKINT i = 0; i < ckfiles2load.size(); i++ )
    {
        // save for later
        ck_libs.push_back( ckfiles2load[i] );
    }

    // pop log
    EM_poplog();

    // loop over dirs2 to search | if not recursive, this should be empty
    for( t_CKINT i = 0; i < dirs2search.size(); i++ )
    {
        // search in dir
        probe_external_modules_in_directory( dirs2search[i].c_str(), extension, recursiveSearch, ck_libs );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: probe_external_modules()
// desc: probe external modules (e.g., .chug and .ck library files)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::probe_external_modules( const char * extension,
                                                 std::list<std::string> & chugin_search_paths,
                                                 std::list<std::string> & named_dls,
                                                 t_CKBOOL recursiveSearch,
                                                 std::list<std::string> & ck_libs )
{
    // print
    EM_log( CK_LOG_SYSTEM, "probing specified chugins (e.g., via --chugin)..." );
    // push
    EM_pushlog();

    // first load dynamic libraries explicitly named on the command line
    for( std::list<std::string>::iterator i_dl = named_dls.begin();
         i_dl != named_dls.end(); i_dl++ )
    {
        // get chugin name
        std::string & dl_path = *i_dl;
        // check extension, append if no match
        if( !extension_matches(dl_path.c_str(), extension) )
            dl_path += extension;
        // load the module
        probe_external_module_at_path( dl_path.c_str(), dl_path.c_str() );
    }

    // check
    if( named_dls.size() == 0 )
        EM_log( CK_LOG_INFO, "(none specified)" );

    // pop
    EM_poplog();
    // print
    EM_log( CK_LOG_SYSTEM, "probing chugins in search paths..." );
    // push
    EM_pushlog();

    // now recurse through search paths and load any DLs or .ck files found
    for( std::list<std::string>::iterator i_sp = chugin_search_paths.begin();
         i_sp != chugin_search_paths.end(); i_sp++ )
    {
        // search directory and load contents
        probe_external_modules_in_directory( (*i_sp).c_str(), extension, recursiveSearch, ck_libs );
    }

    // pop
    EM_poplog();

    return TRUE;
}
