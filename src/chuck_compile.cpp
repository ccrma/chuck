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
// file: chuck_compile.cpp
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
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

using namespace std;




// function prototypes
t_CKBOOL load_internal_modules( Chuck_Compiler * compiler );
t_CKBOOL load_module( Chuck_Env * env, f_ck_query query, const char * name, const char * nspc );




//-----------------------------------------------------------------------------
// name: Chuck_Compiler()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Compiler::Chuck_Compiler()
{
    env = NULL;
    emitter = NULL;
    code = NULL;
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
t_CKBOOL Chuck_Compiler::initialize( Chuck_VM * vm )
{
    // log
    EM_log( CK_LOG_SYSTEM, "initializing compiler..." );
    // push indent level
    EM_pushlog();

    // allocate the type checker
    env = type_engine_init( vm );
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
    type_engine_shutdown( env );
    // emit_engine_shutdown( emitter );
    env = NULL;
    emitter = NULL;
    code = NULL;
    m_auto_depend = FALSE;
    m_recent.clear();

    // pop indent
    EM_poplog();
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
t_CKBOOL Chuck_Compiler::go( const string & filename, FILE * fd, const char * str_src )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    // check to see if resolve dependencies automatically
    if( !m_auto_depend )
    {
        // normal
        ret = this->do_normal( filename, fd, str_src );
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
        env->reset();

        // load the context
        if( !type_engine_load_context( env, context ) )
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
        if( ret ) env->global()->commit();
        // or rollback
        else env->global()->rollback();

        // unload the context from the type-checker
        if( !type_engine_unload_context( env ) )
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
    if( !type_engine_scan0_prog( env, g_program, te_do_all ) )
         return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env, g_program, te_do_all ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env, g_program, te_do_all ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env, context, te_do_all ) )
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
    if( !type_engine_scan0_prog( env, g_program, te_do_classes_only ) )
        return FALSE;

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env, g_program, te_do_classes_only ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env, g_program, te_do_classes_only ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env, context, te_do_classes_only ) )
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
    if( !type_engine_scan1_prog( env, g_program, te_do_no_classes ) )
        return FALSE;

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env, g_program, te_do_no_classes ) )
        return FALSE;

    // check the program (pass 3)
    if( !type_engine_check_context( env, context, te_do_no_classes ) )
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
t_CKBOOL Chuck_Compiler::do_normal( const string & filename, FILE * fd, const char * str_src )
{
    t_CKBOOL ret = TRUE;
    Chuck_Context * context = NULL;

    // parse the code
    if( !chuck_parse( filename.c_str(), fd, str_src ) )
        return FALSE;

    // make the context
    context = type_engine_make_context( g_program, filename );
    if( !context ) return FALSE;

    // reset the env
    env->reset();

    // load the context
    if( !type_engine_load_context( env, context ) )
        return FALSE;

    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env, g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env, g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env, g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // check the program (pass 3)
    if( !type_engine_check_context( env, context, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // emit (pass 4)
    if( !(code = emit_engine_emit_prog( emitter, g_program, te_do_all )) )
    { ret = FALSE; goto cleanup; }

cleanup:

    // commit
    if( ret ) env->global()->commit();
    // or rollback
    else env->global()->rollback();

    // unload the context from the type-checker
    if( !type_engine_unload_context( env ) )
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
t_CKBOOL load_module( Chuck_Env * env, f_ck_query query, 
                      const char * name, const char * nspc )
{
    Chuck_DLL * dll = NULL;
    t_CKBOOL query_failed = FALSE;
    
    // load osc
    dll = new Chuck_DLL( name );
    if( !dll->load( query ) || (query_failed = !dll->query()) || 
        !type_engine_add_dll( env, dll, nspc ) )
    {
        fprintf( stderr, 
                 "[chuck]: internal error loading module '%s.%s'...\n", 
                 nspc, name );
        if( query_failed )
            fprintf( stderr, "       %s\n", dll->last_error() );

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
    Chuck_Env * env = compiler->env;
    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[internal]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // load
    EM_log( CK_LOG_SEVERE, "module osc..." );
    load_module( env, osc_query, "osc", "global" );
    EM_log( CK_LOG_SEVERE, "module xxx..." );
    load_module( env, xxx_query, "xxx", "global" );
    EM_log( CK_LOG_SEVERE, "module filter..." );
    load_module( env, filter_query, "filter", "global" );
    EM_log( CK_LOG_SEVERE, "module STK..." );
    load_module( env, stk_query, "stk", "global" );
    EM_log( CK_LOG_SEVERE, "module xform..." );
    load_module( env, xform_query, "xform", "global" );
    EM_log( CK_LOG_SEVERE, "module extract..." );
    load_module( env, extract_query, "extract", "global" );

    // load
    EM_log( CK_LOG_SEVERE, "class 'machine'..." );
    if( !load_module( env, machine_query, "Machine", "global" ) ) goto error;
    machine_init( compiler, otf_process_msg );
    EM_log( CK_LOG_SEVERE, "class 'std'..." );
    if( !load_module( env, libstd_query, "Std", "global" ) ) goto error;
    EM_log( CK_LOG_SEVERE, "class 'math'..." );
    if( !load_module( env, libmath_query, "Math", "global" ) ) goto error;
    EM_log( CK_LOG_SEVERE, "class 'opsc'..." );
    if( !load_module( env, opensoundcontrol_query, "opsc", "global" ) ) goto error;
    // if( !load_module( env, net_query, "net", "global" ) ) goto error;

#ifndef __DISABLE_MIDI__
    if( !init_class_Midi( env ) ) goto error;
    if( !init_class_MidiRW( env ) ) goto error;
#endif // __DISABLE_MIDI__
    if( !init_class_HID( env ) ) goto error;

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
