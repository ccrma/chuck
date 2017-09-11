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
#include "chuck_otf.h"

#include "ugen_osc.h"
#include "ugen_xxx.h"
#include "ugen_filter.h"
#include "ugen_stk.h"
#include "uana_xform.h"
#include "uana_extract.h"
#include "ulib_machine.h"
#include "ulib_math.h"
#include "ulib_std.h"
#include "ulib_opsc.h"
#include "ulib_regex.h"
#include "chuck_io.h"

#if defined(__PLATFORM_WIN32__)
#include "dirent_win32.h"
#endif
//#if defined(__WINDOWS_PTHREAD__)
#include <sys/stat.h>
//#endif

using namespace std;




// function prototypes
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler );
t_CKBOOL load_external_modules( Chuck_Compiler * compiler, 
                                const char * extension, 
                                std::list<std::string> & chugin_search_paths,
                                std::list<std::string> & named_dls);
t_CKBOOL load_module( Chuck_Compiler * compiler, Chuck_Env * env, f_ck_query query, const char * name, const char * nspc );




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
t_CKBOOL Chuck_Compiler::initialize( std::list<std::string> & chugin_search_paths,
                                     std::list<std::string> & named_dls )
{
    // log
    EM_log( CK_LOG_SYSTEM, "initializing compiler..." );
    // push indent level
    EM_pushlog();

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
    
    // load external libs
    if( !load_external_modules( this, ".chug", chugin_search_paths, named_dls ) )
        goto error;
    
    // pop indent
    EM_poplog();

    return TRUE;

error:
    // clean up
    this->shutdown();

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
t_CKBOOL Chuck_Compiler::go( const string & filename, FILE * fd, const char * str_src, const string & full_path )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    EM_reset_msg();
    
    // check to see if resolve dependencies automatically
    if( !m_auto_depend )
    {
        // normal (note: full_path added 1.3.0.0)
        ret = this->do_normal( filename, fd, str_src, full_path );
        return ret;
    }
    else // auto
    {
        // parse the code
        if( !chuck_parse( filename.c_str(), fd, str_src ) )
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
        if( !(code = context->code()) )
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
    context->progress = Chuck_Context::P_ALL;

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
    if( !(code = emit_engine_emit_prog( emitter, g_program , te_do_classes_only )) )
        return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL;

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
    if( !(code = emit_engine_emit_prog( emitter, g_program, te_do_no_classes )) )
        return FALSE;

    // set the state of the context to done
    context->progress = Chuck_Context::P_ALL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: do_normal()
// desc: compile normally without auto-depend
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Compiler::do_normal( const string & filename, FILE * fd, const char * str_src, const string & full_path )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    // parse the code
    if( !chuck_parse( filename.c_str(), fd, str_src ) )
        return FALSE;

    // make the context
    context = type_engine_make_context( g_program, filename );
    if( !context ) return FALSE;
    
    // remember full path (added 1.3.0.0)
    context->full_path = full_path;

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
    if( !(code = emit_engine_emit_prog( emitter, g_program, te_do_all )) )
    { ret = FALSE; goto cleanup; }

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
    if( (query_failed = !(dll->load( query ) && dll->query())) ||
        !type_engine_add_dll( env, dll, nspc ) )
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
    
//#ifndef __DISABLE_MIDI__
    if( !init_class_Midi( env ) ) goto error;
    if( !init_class_MidiRW( env ) ) goto error;
//#endif // __DISABLE_MIDI__

    // load
    EM_log( CK_LOG_SEVERE, "module osc..." );
    load_module( compiler, env, osc_query, "osc", "global" );
    EM_log( CK_LOG_SEVERE, "module xxx..." );
    load_module( compiler, env, xxx_query, "xxx", "global" );
    EM_log( CK_LOG_SEVERE, "module filter..." );
    load_module( compiler, env, filter_query, "filter", "global" );
    EM_log( CK_LOG_SEVERE, "module STK..." );
    load_module( compiler, env, stk_query, "stk", "global" );
    EM_log( CK_LOG_SEVERE, "module xform..." );
    load_module( compiler, env, xform_query, "xform", "global" );
    EM_log( CK_LOG_SEVERE, "module extract..." );
    load_module( compiler, env, extract_query, "extract", "global" );
    
    // load
    EM_log( CK_LOG_SEVERE, "class 'machine'..." );
    if( !load_module( compiler, env, machine_query, "Machine", "global" ) ) goto error;
    machine_init( compiler, otf_process_msg );
    EM_log( CK_LOG_SEVERE, "class 'std'..." );
    if( !load_module( compiler, env, libstd_query, "Std", "global" ) ) goto error;
    EM_log( CK_LOG_SEVERE, "class 'math'..." );
    if( !load_module( compiler, env, libmath_query, "Math", "global" ) ) goto error;

// ge: these currently don't compile on "modern" windows versions (1.3.5.3)
#ifndef __WINDOWS_MODERN__
    EM_log( CK_LOG_SEVERE, "class 'opsc'..." );
    if( !load_module( compiler, env, opensoundcontrol_query, "opsc", "global" ) ) goto error;
#endif
    EM_log( CK_LOG_SEVERE, "class 'RegEx'..." );
    if( !load_module( compiler, env, regex_query, "RegEx", "global" ) ) goto error;
    // if( !load_module( compiler, env, net_query, "net", "global" ) ) goto error;
    
    if( !init_class_HID( env ) ) goto error;
    if( !init_class_serialio( env ) ) goto error;
        
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
    
    if((query_failed = !(dll->load(dl_path) && dll->query())) ||
       !type_engine_add_dll2(env, dll, "global"))
    {
        EM_log(CK_LOG_SEVERE, "error loading chugin '%s', skipping", name);
        if(query_failed)
            EM_log(CK_LOG_SEVERE, "error from chuck_dl: '%s'", dll->last_error());
        delete dll;
        
        return FALSE;
    }
    else
    {
        compiler->m_dlls.push_back(dll);
        return TRUE;
    }        
}




//-----------------------------------------------------------------------------
// name: extension_matches()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKBOOL extension_matches( const char * filename, const char * extension )
{
    t_CKUINT extension_length = strlen(extension);
    t_CKUINT filename_length = strlen(filename);
    
    return strncmp( extension, filename+(filename_length-extension_length), 
                    extension_length) == 0;
}




//-----------------------------------------------------------------------------
// name: load_external_modules_in_directory()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_external_modules_in_directory( Chuck_Compiler * compiler,
                                             const char * directory,
                                             const char * extension )
{
    static const t_CKBOOL RECURSIVE_SEARCH = false;
    
    DIR * dir = opendir(directory);
    
    if( dir )
    {
        // log
        EM_log( CK_LOG_INFO, "examining directory '%s' for chugins", directory );
        
        struct dirent *de = NULL;
        
        while( (de = readdir(dir)) )
        {
            t_CKBOOL is_regular = false;
            t_CKBOOL is_directory = false;
            
#if defined(__PLATFORM_WIN32__)
            is_directory = de->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            is_regular = ((de->data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) || 
                          (de->data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ||
                          (de->data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE));
#elif defined(__WINDOWS_PTHREAD__) // Cygwin -- doesn't have dirent d_type
            std::string absolute_path = std::string(directory) + "/" + de->d_name;
            struct stat st;
            if( stat(absolute_path.c_str(), &st) == 0 )
            {
                is_directory = st.st_mode & S_IFDIR;
                is_regular = st.st_mode & S_IFREG;
            }
            else
            {
                // uhh ... 
                EM_log( CK_LOG_INFO, 
                        "unable to stat file '%s', ignoring for chugins", 
                        absolute_path.c_str() );
                continue;
            }
#else
            is_directory = de->d_type == DT_DIR;
            is_regular = de->d_type == DT_REG;
#endif
            
            if( is_regular ) // TODO: follow links?
            {
                if( extension_matches(de->d_name, extension) )
                {
                    std::string absolute_path = std::string(directory) + "/" + de->d_name;
                    
                    load_external_module_at_path(compiler, de->d_name, 
                                                 absolute_path.c_str());
                }
                else if( extension_matches(de->d_name, ".ck") )
                {
                    std::string absolute_path = std::string(directory) + "/" + de->d_name;
                    compiler->m_cklibs_to_preload.push_back(absolute_path);
                }
            }
            else if( RECURSIVE_SEARCH && is_directory )
            {
                // recurse
                // TODO: max depth?
                if( strncmp(de->d_name, ".", sizeof(".")) != 0 &&
                    strncmp(de->d_name, "..", sizeof("..")) != 0 )
                {
                    std::string absolute_path = std::string(directory) + "/" + de->d_name;
                    load_external_modules_in_directory(compiler, 
                                                       absolute_path.c_str(),
                                                       extension);
                }
            }
        }
        
        // close
        closedir( dir );
    }
    else
    {
        // log (1.3.1.2: changed to 2 lines to stay within 80 chars)
        EM_log( CK_LOG_INFO, "unable to open directory '%s'...", directory );
        EM_log( CK_LOG_INFO, "(ignoring for chugins...)" );
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load_external_modules()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL load_external_modules( Chuck_Compiler * compiler, 
                                const char * extension, 
                                std::list<std::string> & chugin_search_paths,
                                std::list<std::string> & named_dls )
{
    // log
    EM_log( CK_LOG_SEVERE, "loading chugins" );
    // push indent level
    EM_pushlog();
    
    // get env
    Chuck_Env * env = compiler->env();
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[external]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );
    
    /* first load dynamic libraries explicitly named on the command line */
    
    for(std::list<std::string>::iterator i_dl = named_dls.begin();
        i_dl != named_dls.end(); i_dl++)
    {
        std::string & dl_path = *i_dl;
        if(!extension_matches(dl_path.c_str(), extension))
            dl_path += extension;
        
        load_external_module_at_path(compiler, dl_path.c_str(),
                                     dl_path.c_str());
    }
    
    /* now recurse through search paths and load any DLs or .ck files found */
    
    for(std::list<std::string>::iterator i_sp = chugin_search_paths.begin();
        i_sp != chugin_search_paths.end(); i_sp++)
    {
        load_external_modules_in_directory(compiler, (*i_sp).c_str(), extension);
    }    
        
    // clear context
    type_engine_unload_context( env );
    
    // commit what is in the type checker at this point
    env->global()->commit();
    
    // pop indent level
    EM_poplog();
    
    return TRUE;
   /* 
error:
    
    // probably dangerous: rollback
    env->global()->rollback();
    
    // clear context
    type_engine_unload_context( env );
    
    // pop indent level
    EM_poplog();
    
    return FALSE;
	*/
}

