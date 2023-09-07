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
// name: chuck_dl.cpp
// desc: chuck dynamic linking header
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//          Ari Lazier (alazier@cs.princeton.edu)
//          Spencer Salazar (spencer@ccrma.stanford.edu)
// mac os code based on apple's open source
//
// date: spring 2004 - 1.1
//       spring 2005 - 1.2
//-----------------------------------------------------------------------------
#include "chuck_dl.h"
#include "chuck_errmsg.h"
#include "chuck_type.h"
#include "chuck_instr.h"
#include "chuck_compile.h"
#include "chuck_vm.h"
#include "chuck.h"
#include <sstream>
using namespace std;




#if defined(__PLATFORM_APPLE__)
char g_default_chugin_path[] = "/usr/local/lib/chuck:/Library/Application Support/ChucK/chugins:~/Library/Application Support/ChucK/chugins:~/.chuck/lib";
#elif defined(__PLATFORM_WINDOWS__)
char g_default_chugin_path[] = "C:\\Windows\\system32\\ChucK;C:\\Program Files\\ChucK\\chugins;C:\\Program Files (x86)\\ChucK\\chugins;C:\\Users\\%USERNAME%\\Documents\\ChucK\\chugins";
#else // Linux / Cygwin
char g_default_chugin_path[] = "/usr/local/lib/chuck:~/.chuck/lib";
#endif

char g_chugin_path_envvar[] = "CHUCK_CHUGIN_PATH";




//-----------------------------------------------------------------------------
// internal implementation of query functions
//-----------------------------------------------------------------------------



t_CKUINT ck_builtin_declversion() { return CK_DLL_VERSION; }



//-----------------------------------------------------------------------------
// name: ck_setname()
// desc: set the name of the module
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_setname( Chuck_DL_Query * query, const char * name )
{
    // set the name
    query->dll_name = name;
}




//-----------------------------------------------------------------------------
// name: ck_begin_class()
// desc: begin class/namespace, can be nexted
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_begin_class( Chuck_DL_Query * query, const char * name, const char * parent )
{
    // find parent mvar offset
    t_CKINT parent_offset = 0;
    if(parent && parent[0] != '\0')
    {
        a_Id_List parent_path = str2list( parent );
        if( !parent_path )
        {
            // error
            EM_error2( 0, "class import: ck_begin_class: unknown parent type '%s'",
                      query->curr_class->name.c_str(), name, parent );
            return;
        }

        // find parent type
        Chuck_Type * ck_parent_type = type_engine_find_type( query->env(), parent_path );
        // clean up locally created id list
        delete_id_list( parent_path );
        // not found
        if( !ck_parent_type )
        {
            // error
            EM_error2( 0, "class import: ck_begin_class: unable to find parent type '%s'", ck_parent_type );
            return;
        }

        parent_offset = ck_parent_type->obj_size;
    }

    // push
    query->stack.push_back( query->curr_class );
    // allocate
    Chuck_DL_Class * c = new Chuck_DL_Class;

    // add class
    if( query->curr_class )
    {
        // recursive
        query->curr_class->classes.push_back( c );
    }
    // 1.3.2.0: do not save class for later import (will import it on class close)
    // else
    // // first level
    // query->classes.push_back( c );

    // remember info
    c->name = name ? name : "";
    c->parent = parent ? parent : "";
    c->current_mvar_offset = parent_offset;

    // if more info is available | 1.5.1.3 (ge)
    if( query->dll_ref && string(query->dll_ref->filepath()) != "" ) {
        // set chugin file path for error reporting
        c->hint_dll_filepath = query->dll_ref->filepath();
    } else {
        // set DLL name for error reporting
        c->hint_dll_filepath = query->dll_name;
    }

    // curr
    query->curr_class = c;
    query->curr_func = NULL;
}




//-----------------------------------------------------------------------------
// name: ck_add_ctor()
// desc: add constructor, can be followed by add_arg
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_ctor( Chuck_DL_Query * query, f_ctor ctor )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_ctor invoked without begin_class..." );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = "[ctor]";
    f->type = "void";
    f->ctor = ctor;

    // add
    query->curr_class->ctors.push_back( f );
    query->curr_func = f;
}




//-----------------------------------------------------------------------------
// name: ck_add_dtor()
// add destructor, no args allowed
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_dtor( Chuck_DL_Query * query, f_dtor dtor )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_dtor invoked without begin_class..." );
        return;
    }

    // make sure there are no duplicates
    if( query->curr_class->dtor )
    {
        // error
        EM_error2( 0, "class import: multiple dtor added ..." );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = "[dtor]";
    f->type = "void";
    f->dtor = dtor;

    // add
    query->curr_class->dtor = f;
    // set
    query->curr_func = NULL;
}




//-----------------------------------------------------------------------------
// name: ck_add_mfun()
// desc: add member function, can be followed by add_arg
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_mfun( Chuck_DL_Query * query, f_mfun addr,
                              const char * type, const char * name )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_mfun invoked without begin_class..." );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = name;
    f->type = type;
    f->mfun = addr;

    // add
    query->curr_class->mfuns.push_back( f );
    query->curr_func = f;
}




//-----------------------------------------------------------------------------
// name: ck_add_sfun()
// desc: add static function, can be followed by add_arg
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_sfun( Chuck_DL_Query * query, f_sfun addr,
                              const char * type, const char * name )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_sfun invoked without begin_class..." );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = name;
    f->type = type;
    f->sfun = addr;

    // add
    query->curr_class->sfuns.push_back( f );
    query->curr_func = f;
}




//-----------------------------------------------------------------------------
// name: ck_add_mvar()
// desc: add member var
//-----------------------------------------------------------------------------
t_CKUINT CK_DLL_CALL ck_add_mvar( Chuck_DL_Query * query,
                                  const char * type, const char * name,
                                  t_CKBOOL is_const )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_mvar invoked without begin_class..." );
        return CK_INVALID_OFFSET;
    }

    a_Id_List path = str2list( type );
    if( !path )
    {
        // error
        EM_error2( 0, "class import: add_mvar: mvar %s.%s has unknown type '%s'",
                   query->curr_class->name.c_str(), name, type );
        return CK_INVALID_OFFSET;
    }

    // find type
    Chuck_Type * ck_type = type_engine_find_type( query->env(), path );
    // clean up locally created id list
    delete_id_list( path );
    // not found
    if( !ck_type )
    {
        // error
        EM_error2( 0, "class import: add_mvar: unable to find type '%s'", type );
        return CK_INVALID_OFFSET;
    }

    // allocate
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->name = name;
    v->type = type;
    v->is_const = is_const;

    // add
    query->curr_class->mvars.push_back( v );
    query->curr_func = NULL;
    query->last_var = v;

    t_CKUINT offset = query->curr_class->current_mvar_offset;
    query->curr_class->current_mvar_offset = type_engine_next_offset( query->curr_class->current_mvar_offset,
                                                                      ck_type );

    return offset;
}




//-----------------------------------------------------------------------------
// name: ck_add_svar()
// desc: add static var
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_svar( Chuck_DL_Query * query, const char * type, const char * name,
                              t_CKBOOL is_const, void * addr )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_svar invoked without begin_class..." );
        return;
    }

    // allocate
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->name = name;
    v->type = type;
    v->is_const = is_const;
    v->static_addr = addr;

    // add
    query->curr_class->svars.push_back( v );
    query->curr_func = NULL;

    query->last_var = v;
}




//-----------------------------------------------------------------------------
// name: ck_add_arg()
// desc: add argument to function
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_arg( Chuck_DL_Query * query, const char * type, const char * name )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_arg invoked without begin_class..." );
        return;
    }

    // make sure there is function
    if( !query->curr_func )
    {
        // error
        EM_error2( 0, "class import: add_arg can only follow 'ctor', 'mfun', 'sfun', 'arg'..." );
        return;
    }

    // allocate
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->name = name;
    v->type = type;

    // add
    query->curr_func->args.push_back( v );
}




//-----------------------------------------------------------------------------
// name: ck_add_ugen_func()
// desc: (ugen only) add tick and pmsg functions
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_ugen_func( Chuck_DL_Query * query, f_tick ugen_tick, f_pmsg ugen_pmsg, t_CKUINT num_in, t_CKUINT num_out )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_ugen_func invoked without begin_class..." );
        return;
    }

    // make sure tick not defined already
    if( query->curr_class->ugen_tick && ugen_tick )
    {
        // error
        EM_error2( 0, "class import: ugen_tick already defined..." );
        return;
    }

    // make sure pmsg not defined already
    if( query->curr_class->ugen_pmsg && ugen_pmsg )
    {
        // error
        EM_error2( 0, "class import: ugen_pmsg already defined..." );
        return;
    }

    // set
    if( ugen_tick ) query->curr_class->ugen_tick = ugen_tick;
    if( ugen_pmsg ) query->curr_class->ugen_pmsg = ugen_pmsg;
    query->curr_class->ugen_num_in = num_in;
    query->curr_class->ugen_num_out = num_out;
    query->curr_func = NULL;
}




//-----------------------------------------------------------------------------
// name: ck_add_ugen_funcf()
// desc: (ugen only) add tick and pmsg functions
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_ugen_funcf( Chuck_DL_Query * query, f_tickf ugen_tickf, f_pmsg ugen_pmsg, t_CKUINT num_in, t_CKUINT num_out )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_ugen_func invoked without begin_class..." );
        return;
    }

    // make sure tick not defined already
    if( query->curr_class->ugen_tickf && ugen_tickf )
    {
        // error
        EM_error2( 0, "class import: ugen_tick already defined..." );
        return;
    }

    // make sure pmsg not defined already
    if( query->curr_class->ugen_pmsg && ugen_pmsg )
    {
        // error
        EM_error2( 0, "class import: ugen_pmsg already defined..." );
        return;
    }

    // set
    if( ugen_tickf ) query->curr_class->ugen_tickf = ugen_tickf;
    if( ugen_pmsg ) query->curr_class->ugen_pmsg = ugen_pmsg;
    query->curr_class->ugen_num_in = num_in;
    query->curr_class->ugen_num_out = num_out;
    query->curr_func = NULL;
}




//-----------------------------------------------------------------------------
// name: ck_add_ugen_funcf_auto_num_channels()
// desc: (ugen only) add tick and pmsg functions. specify num channels by vm.
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_ugen_funcf_auto_num_channels( Chuck_DL_Query * query,
    f_tickf ugen_tickf, f_pmsg ugen_pmsg )
{
    ck_add_ugen_funcf( query, ugen_tickf, ugen_pmsg,
        query->vm()->m_num_adc_channels,
        query->vm()->m_num_dac_channels
    );
}




//-----------------------------------------------------------------------------
// name: ck_add_ugen_ctrl()
// desc: (ugen only) add ctrl parameters
//-----------------------------------------------------------------------------
//void CK_DLL_CALL ck_add_ugen_ctrl( Chuck_DL_Query * query, f_ctrl ugen_ctrl, f_cget ugen_cget,
//                                   const char * type, const char * name )
//{
//    // this is no longer used
//    EM_error2( 0, "class import: obselete ck_add_ugen_ctrl invoked..." );
//    return;
//
//    // make sure there is class
//    if( !query->curr_class )
//    {
//        // error
//        EM_error2( 0, "class import: add_ugen_func invoked without begin_class..." );
//        return;
//    }
//
//    // allocate
//    Chuck_DL_Ctrl * c = new Chuck_DL_Ctrl;
//    c->name = name;
//    c->type = type;
//    c->ctrl = ugen_ctrl;
//    c->cget = ugen_cget;
//
//    // set
//    query->curr_func = NULL;
//}




//-----------------------------------------------------------------------------
// name: ck_end_class()
// desc: end class/namespace, compile it
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_end_class( Chuck_DL_Query * query )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: end_class invoked without begin_class..." );
        return FALSE;
    }

    // type check the class?
    // 1.3.2.0: import class into type engine if at top level
    if( query->stack.size() == 1 ) // top level class
    {
        if( !type_engine_add_class_from_dl( query->env(), query->curr_class ) )
        {
            // should already be message
            //EM_log(CK_LOG_SEVERE, "error importing class '%s' into type engine",
            // query->curr_class->name.c_str());

            // pop
            assert( query->stack.size() );
            query->curr_class = query->stack.back();
            query->stack.pop_back();

            // flag the query with error
            query->errorEncountered = TRUE;

            return FALSE;
        }
    }

    // pop
    assert( query->stack.size() );
    query->curr_class = query->stack.back();
    query->stack.pop_back();

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_create_main_thread_hook()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_MainThreadHook * CK_DLL_CALL ck_create_main_thread_hook( Chuck_DL_Query * query,
                                                                  f_mainthreadhook hook,
                                                                  f_mainthreadquit quit,
                                                                  void * bindle )
{
    return new Chuck_DL_MainThreadHook( hook, quit, bindle, query->carrier() );
}


//-----------------------------------------------------------------------------
// name: ck_doc_class()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_doc_class( Chuck_DL_Query * query, const char * doc )
{
// #ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_class)
        query->curr_class->doc = doc;
    else
        return FALSE;
// #endif // CK_DOC

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: ck_add_example()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_add_example( Chuck_DL_Query * query, const char * ex )
{
// #ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_class)
        query->curr_class->examples.push_back(ex);
    else
        return FALSE;
// #endif // CK_DOC

    return TRUE;
}

// set current function documentation
t_CKBOOL CK_DLL_CALL ck_doc_func( Chuck_DL_Query * query, const char * doc )
{
// #ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_func)
        query->curr_func->doc = doc;
    else
        return FALSE;
// #endif // CK_DOC

    return TRUE;
}

// set last mvar documentation
t_CKBOOL CK_DLL_CALL ck_doc_var( Chuck_DL_Query * query, const char * doc )
{
// #ifdef CK_DOC // disable unless CK_DOC
    if(query->last_var)
        query->last_var->doc = doc;
    else
        return FALSE;
// #endif // CK_DOC

    return TRUE;
}



//------------------------------------------------------------------------------
// alternative functions to make stuff
//------------------------------------------------------------------------------
Chuck_DL_Func * make_new_mfun( const char * t, const char * n, f_mfun mfun )
{   return new Chuck_DL_Func( t, n, (t_CKUINT)mfun ); }
Chuck_DL_Func * make_new_sfun( const char * t, const char * n, f_sfun sfun )
{   return new Chuck_DL_Func( t, n, (t_CKUINT)sfun ); }
Chuck_DL_Value * make_new_arg( const char * t, const char * n )
{   return new Chuck_DL_Value( t, n ); }
Chuck_DL_Value * make_new_mvar( const char * t, const char * n, t_CKBOOL c )
{   return new Chuck_DL_Value( t, n, c ); }
Chuck_DL_Value * make_new_svar( const char * t, const char * n, t_CKBOOL c, void * a )
{   return new Chuck_DL_Value( t, n, c, a ); }




//-----------------------------------------------------------------------------
// name: load()
// desc: load dynamic link library
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::load( const char * filename, const char * func, t_CKBOOL lazy )
{
    // open
    m_handle = dlopen( filename, lazy ? RTLD_LAZY : RTLD_NOW );

    // still not there
    if( !m_handle )
    {
        m_last_error = dlerror();
        return FALSE;
    }

    // save the filename
    m_filename = filename;
    m_done_query = FALSE;
    m_func = func;

    // if not lazy, do it
    if( !lazy && !this->query() )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: load()
// desc: load dynamic link library
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::load( f_ck_query query_func, t_CKBOOL lazy )
{
    m_query_func = query_func;
    m_version_func = ck_builtin_declversion;
    m_done_query = FALSE;
    m_func = "ck_query";

    // if not lazy, do it
    if( !lazy && !this->query() )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: good()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::good() const
{
    return ( m_handle != NULL || m_query_func != NULL );
}




//-----------------------------------------------------------------------------
// name: query()
// desc: ...
//-----------------------------------------------------------------------------
const Chuck_DL_Query * Chuck_DLL::query()
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "dynamic link library not loaded for query...";
        return NULL;
    }

    // return if there already
    if( m_done_query )
        return &m_query;

    // get the address of the DL version function from the DLL
    if( !m_version_func )
        m_version_func = (f_ck_declversion)this->get_addr( CK_DECLVERSION_FUNC );
    if( !m_version_func )
        m_version_func = (f_ck_declversion)this->get_addr( (string("_")+CK_DECLVERSION_FUNC).c_str() );
    if( !m_version_func )
    {
        m_last_error = string("no version function found in dll '") + m_filename + string("'");
        return NULL;
    }

    // check version
    t_CKUINT dll_version = m_version_func();
    // get major and minor version numbers
    m_versionMajor = CK_DLL_VERSION_GETMAJOR(dll_version);
    m_versionMinor = CK_DLL_VERSION_GETMINOR(dll_version);
    // is version ok
    t_CKBOOL version_ok = FALSE;
    // major version must be same; minor version must less than or equal
    if( m_versionMajor == CK_DLL_VERSION_MAJOR && m_versionMinor <= CK_DLL_VERSION_MINOR)
        version_ok = TRUE;

    // if version not okay
    if( !version_ok )
    {
        // construct error string
        ostringstream oss;
        oss << "chugin version incompatible..." << endl
            << "chugin path: '" << m_filename << "'" << endl
            << "chugin version: " << m_versionMajor << "." << m_versionMinor
            << " VS host version: " << CK_DLL_VERSION_MAJOR << "." << CK_DLL_VERSION_MINOR;
        m_last_error = oss.str();
        return NULL;
    }

    // get the address of the query function from the DLL
    if( !m_query_func )
        m_query_func = (f_ck_query)this->get_addr( m_func.c_str() );
    if( !m_query_func )
        m_query_func = (f_ck_query)this->get_addr( (string("_")+m_func).c_str() );
    if( !m_query_func )
    {
        m_last_error = string("no query function found in dll '") + m_filename + string("'");
        return NULL;
    }

    // get the address of the attach function from the DLL
    /* if( !m_attach_func )
        m_attach_func = (f_ck_attach)this->get_addr( "ck_attach" );
    if( !m_attach_func )
        m_attach_func = (f_ck_attach)this->get_addr( "_ck_attach" );

    // get the address of the detach function from the DLL
    if( !m_detach_func )
        m_detach_func = (f_ck_detach)this->get_addr( "ck_detach" );
    if( !m_detach_func )
        m_detach_func = (f_ck_detach)this->get_addr( "_ck_detach" ); */

    // do the query
    m_query.clear();
    if( !m_query_func( &m_query ) || m_query.errorEncountered )
    {
        m_last_error = string("unsuccessful query in dll '") + m_filename
                       + string("'");
        return NULL;
    }

    // load the proto table
    /* Chuck_DL_Proto * proto;
    m_name2proto.clear();
    for( t_CKUINT i = 0; i < m_query.dll_exports.size(); i++ )
    {
        proto = &m_query.dll_exports[i];
        if( m_name2proto[proto->name] )
        {
            m_last_error = string("duplicate export name '") + proto->name
                           + string("'");
            return NULL;
        }

        // get the addr
        if( !proto->addr )
            proto->addr = (f_ck_func)this->get_addr( (char *)proto->name.c_str() );
        if( !proto->addr )
        {
            m_last_error = string("no addr associated with export '") +
                           proto->name + string("'");
            return NULL;
        }

        // put in the lookup table
        m_name2proto[proto->name] = proto;
    }

    // load the proto table
    Chuck_UGen_Info * info;
    m_name2ugen.clear();
    for( t_CKUINT j = 0; j < m_query.ugen_exports.size(); j++ )
    {
        info = &m_query.ugen_exports[j];
        if( m_name2ugen[info->name] )
        {
            m_last_error = string("duplicate export ugen name '") + info->name
                           + string("'");
            return NULL;
        }

        // put in the lookup table
        m_name2ugen[info->name] = info;
    }*/

    m_done_query = TRUE;

    // call attach
    // if( m_attach_func ) m_attach_func( 0, NULL );

    return &m_query;
}




//-----------------------------------------------------------------------------
// name: probe()
// desc: probe the dll without fully loading its content
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::probe()
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "dynamic link library not loaded for query...";
        return FALSE;
    }

    // get the address of the DL version function from the DLL
    if( !m_version_func )
        m_version_func = (f_ck_declversion)this->get_addr( CK_DECLVERSION_FUNC );
    if( !m_version_func )
        m_version_func = (f_ck_declversion)this->get_addr( (string("_")+CK_DECLVERSION_FUNC).c_str() );
    if( !m_version_func )
    {
        m_last_error = string("no version function found in dll '") + m_filename + string("'");
        return FALSE;
    }

    // check version
    t_CKUINT dll_version = m_version_func();
    // get major and minor version numbers
    m_versionMajor = CK_DLL_VERSION_GETMAJOR(dll_version);
    m_versionMinor = CK_DLL_VERSION_GETMINOR(dll_version);

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: unload()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::unload()
{
    if( !m_handle && !m_query_func )
    {
        m_last_error = "cannot unload dynamic library - nothing open...";
        return FALSE;
    }

    // if( m_detach_func ) m_detach_func( 0, NULL );

    if( m_handle )
    {
        dlclose( m_handle );
        m_handle = NULL;
    }
    else
        m_query_func = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_addr()
// desc: ...
//-----------------------------------------------------------------------------
void * Chuck_DLL::get_addr( const char * symbol )
{
    if( !m_handle )
    {
        m_last_error = "cannot find addr from dynamic library - nothing open...";
        return FALSE;
    }

    return dlsym( m_handle, symbol );
}




//-----------------------------------------------------------------------------
// name: last_error()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_DLL::last_error() const
{
    return m_last_error.c_str();
}




//-----------------------------------------------------------------------------
// name: name()
// desc: return name given to dll
//-----------------------------------------------------------------------------
const char * Chuck_DLL::name() const
{
    return m_id.c_str();
}




//-----------------------------------------------------------------------------
// name: filepath()
// desc: return the file path
//-----------------------------------------------------------------------------
const char * Chuck_DLL::filepath() const
{
    return m_filename.c_str();
}




//-----------------------------------------------------------------------------
// name: versionMajor()
// desc: get version major
//-----------------------------------------------------------------------------
t_CKUINT Chuck_DLL::versionMajor()
{
    // probe dll
    if( !this->probe() ) return 0;
    // return
    return m_versionMajor;
}




//-----------------------------------------------------------------------------
// name: versionMinor()
// desc: get version minor
//-----------------------------------------------------------------------------
t_CKUINT Chuck_DLL::versionMinor()
{
    // probe dll
    if( !this->probe() ) return 0;
    // return
    return m_versionMinor;
}




//-----------------------------------------------------------------------------
// name: compatible()
// desc: if version compatible between chugin and host
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::compatible()
{
    // probe dll
    if( !this->probe() ) return FALSE;
    // major version must be same between chugin and host
    // chugin minor version must less than or equal host minor version
    if( m_versionMajor == CK_DLL_VERSION_MAJOR && m_versionMinor <= CK_DLL_VERSION_MINOR )
    { return TRUE; }
    else {
        m_last_error = string("version incompatible with host ") +
                       itoa(CK_DLL_VERSION_MAJOR) + "." + itoa(CK_DLL_VERSION_MINOR);
        return FALSE;
    }
}




//-----------------------------------------------------------------------------
// name: Chuck_DL_Query
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Query::Chuck_DL_Query( Chuck_Carrier * carrier, Chuck_DLL * dll )
{
    // set the pointers to functions so the module can call
    setname = ck_setname;
    begin_class = ck_begin_class;
    add_ctor = ck_add_ctor;
    add_dtor = ck_add_dtor;
    add_mfun = ck_add_mfun;
    add_sfun = ck_add_sfun;
    add_mvar = ck_add_mvar;
    add_svar = ck_add_svar;
    add_arg = ck_add_arg;
    add_ugen_func = ck_add_ugen_func;
    add_ugen_funcf = ck_add_ugen_funcf;
    add_ugen_funcf_auto_num_channels = ck_add_ugen_funcf_auto_num_channels;
    // add_ugen_ctrl = ck_add_ugen_ctrl; // not used
    end_class = ck_end_class;
    doc_class = ck_doc_class;
    doc_func = ck_doc_func;
    doc_var = ck_doc_var;
    add_ex = ck_add_example; // 1.5.0.0 (ge) added
    create_main_thread_hook = ck_create_main_thread_hook;
    m_carrier = carrier;
    dll_ref = dll; // 1.5.1.3 (ge) added

    dll_name = "[noname]";
    reserved = NULL;
    curr_class = NULL;
    curr_func = NULL;
    // memset(reserved2, NULL, sizeof(void*)*RESERVED_SIZE);

    // 1.5.0.4 (ge) allow carrier to be NULL (for query purposes)
    if( m_carrier != NULL )
    {
        // get the actual runtime sample rate
        srate = m_carrier->vm->srate();
    }
    else
    {
        // set to something invalid...
        // (instead of default sample rate, which could be harder to notice / debug)
        srate = 0;
    }

    // get DL API reference | 1.5.1.4
    m_api = Chuck_DL_Api::Api::instance();

    linepos = 0;
    errorEncountered = FALSE;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: clear the query
//-----------------------------------------------------------------------------
void Chuck_DL_Query::clear()
{
    // set to 0
    dll_name = "[noname]";
    // line pos
    linepos = 0;
    // delete classes
    for( t_CKUINT i = 0; i < classes.size(); i++ ) CK_SAFE_DELETE( classes[i] );
    // clear
    classes.clear();
}




//-----------------------------------------------------------------------------
// name: ~Chuck_DL_Class()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Class::~Chuck_DL_Class()
{
    t_CKUINT i;

    // delete mfuns
    for( i = 0; i < mfuns.size(); i++ ) delete mfuns[i];
    // delete sfuns
    for( i = 0; i < sfuns.size(); i++ ) delete sfuns[i];
    // delete mvars
    for( i = 0; i < mvars.size(); i++ ) delete mvars[i];
    // delete svars
    for( i = 0; i < svars.size(); i++ ) delete svars[i];
    // delete classes
    for( i = 0; i < classes.size(); i++ ) delete classes[i];
}




//-----------------------------------------------------------------------------
// name: ~Chuck_DL_Func()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Func::~Chuck_DL_Func()
{
    for( t_CKUINT i = 0; i < args.size(); i++ )
        delete args[i];
}




//-----------------------------------------------------------------------------
// Main Thread Hook stuff
//-----------------------------------------------------------------------------
t_CKBOOL ck_mthook_activate(Chuck_DL_MainThreadHook *hook)
{
    hook->m_carrier->chuck->setMainThreadHook(hook);
    hook->m_active = true;
    return hook->m_active;
}

t_CKBOOL ck_mthook_deactivate(Chuck_DL_MainThreadHook *hook)
{
    hook->m_carrier->chuck->setMainThreadHook(NULL);
    hook->m_active = false;
    return hook->m_active;
}

//-----------------------------------------------------------------------------
// name: Chuck_DL_MainThreadHook()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_MainThreadHook::Chuck_DL_MainThreadHook( f_mainthreadhook hook, f_mainthreadquit quit,
                                                  void * bindle, Chuck_Carrier * carrier )
  : activate(ck_mthook_activate),
    deactivate(ck_mthook_deactivate),
    m_carrier(carrier),
    m_hook(hook),
    m_quit(quit),
    m_bindle(bindle),
    m_active(FALSE)
{ }




//-----------------------------------------------------------------------------
// Chuck_DL_Api stuff
//-----------------------------------------------------------------------------
// note on function hooks, e.g., create_string:
//       the implementation is always to be compiled as part of the host
//       (e.g., chuck or miniAudicle), and never as part of the client
//       (e.g., a chugin). The latter is given a function pointer (hook) at
//       module load-time; this ensures that the actual instantiation is in
//       alignment with whatever host is loading the module.
//-----------------------------------------------------------------------------
namespace Chuck_DL_Api
{
    Api Api::g_api;
}


//-----------------------------------------------------------------------------
// name: ck_srate() | add 1.5.1.4
// desc: host-side hook implementation for getting system srate
//-----------------------------------------------------------------------------
static t_CKUINT ck_srate( Chuck_VM * vm )
{
    return vm->srate();
}


//-----------------------------------------------------------------------------
// name: ck_get_srate() | legacy as of 1.5.1.4
// desc: host-side hook implementation for getting system srate
//-----------------------------------------------------------------------------
static t_CKUINT ck_get_srate( CK_DL_API api, Chuck_VM_Shred * shred )
{
    return shred->vm_ref->srate();
}


//-----------------------------------------------------------------------------
// name: create_event_buffer() | 1.5.1.4 (ge, andrew) added
// desc: host-side hoook implemenation for
//       creatinga new lock-free one-producer, one-consumer buffer
//-----------------------------------------------------------------------------
static CBufferSimple * ck_create_event_buffer( Chuck_VM * vm )
{
    return vm->create_event_buffer();
}

//-----------------------------------------------------------------------------
// name: queue_event() | 1.5.1.4 (ge, andrew) added
// desc: host-side hoook implemenation for queuing an event
//       NOTE num_msg must be 1; buffer created using create_event_buffer()
//-----------------------------------------------------------------------------
static t_CKBOOL ck_queue_event( Chuck_VM * vm, Chuck_Event * event, t_CKINT num_msg, CBufferSimple * buffer )
{
    return vm->queue_event( event, num_msg, buffer );
}


//-----------------------------------------------------------------------------
// name: ck_get_type()
// desc: host-side hook implementation for retrieving a type by name
//-----------------------------------------------------------------------------
static Chuck_DL_Api::Type ck_get_type( CK_DL_API api, Chuck_VM_Shred * shred, const char * name )
{
    Chuck_Env * env = shred->vm_ref->env();
    a_Id_List list = new_id_list( name, 0, 0 /*, NULL*/ ); // TODO: nested types
    Chuck_Type * t = type_engine_find_type( env, list );
    delete_id_list( list );
    return (Chuck_DL_Api::Type)t;
}


//-----------------------------------------------------------------------------
// name: ck_create()
// desc: host-side hook implementation for instantiating and initializing
//       a ChucK object by type
//-----------------------------------------------------------------------------
static Chuck_DL_Api::Object ck_create( CK_DL_API api, Chuck_VM_Shred * shred, Chuck_DL_Api::Type t )
{
    // check | 1.5.0.1 (ge) changed; used to be assert t != NULL
    if( t == NULL )
    {
        // print error message
        EM_error2( 0, "DL_Api:object:ck_create: NULL object reference." );
        // done
        return NULL;
    }

    // type
    Chuck_Type * type = (Chuck_Type *)t;
    // instantiate and initialize
    Chuck_Object * o = instantiate_and_initialize_object( type, shred->vm_ref );
    // done
    return (Chuck_DL_Api::Object)o;
}


//-----------------------------------------------------------------------------
// name: ck_create_string()
// desc: host-side hook implementation for creating a chuck string
//-----------------------------------------------------------------------------
static Chuck_DL_Api::String ck_create_string( CK_DL_API api, Chuck_VM_Shred * shred, const char * cstr )
{
    // instantiate and initalize object
    Chuck_String * string = (Chuck_String *)instantiate_and_initialize_object( shred->vm_ref->env()->ckt_string, shred->vm_ref );
    // set the value
    string->set( cstr ? cstr : "" );
    // return reference
    return (Chuck_DL_Api::String)string;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar()
// desc: retrieve a class's member variable offset
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar(Chuck_DL_Api::Object o, const char * name, te_Type basic_type, t_CKINT & offset)
{
    // check | 1.5.0.1 (ge) added
    if( o == NULL )
    {
        // put error here
        EM_error2(0, "get mvar: ck_get_mvar: NULL object reference.");
        return FALSE;
    }

    std::string mvar(name);
    Chuck_Object* obj = (Chuck_Object*)o;
    Chuck_Type* type = obj->type_ref;
    // check type
    if (type->info == NULL)
    {
        // put error here
        EM_error2(0, "get mvar: ck_get_mvar: object has no type info");
        return FALSE;
    }

    vector<Chuck_Value*> vars;
    Chuck_Value* var = NULL;
    type->info->get_values(vars);
    // iterate over retrieved functions
    for (vector<Chuck_Value*>::iterator v = vars.begin(); v != vars.end(); v++)
    {
        // get pointer to chuck value
        Chuck_Value* value = *v;
        // check for NULL
        if (value == NULL) continue;
        // see if name matches
        if (value->name != mvar) continue;
        // see if name is correct type
        if (value->type->xid != basic_type) continue;
        // see if var is a member var
        if (!value->is_member) continue;

        // ladies and gentlemen, we've got it
        var = value;
        // done
        break;
    }

    // make error
    if( !var )
    {
        EM_error2(0, "get mvar: ck_get_mvar: member variable %s not found", mvar.c_str());
        return FALSE;
    }
    // return offset of var
    offset = var->offset;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_int()
// desc: retrieve a class's member variable of type int
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_int(CK_DL_API api, Chuck_DL_Api::Object obj, const char* name, t_CKINT & value)
{
    // default value
    value = 0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_int, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object* ck_obj = (Chuck_Object*)obj;
        // get int at offset
        value = OBJ_MEMBER_INT(ck_obj, offset); // get offset
    }
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_float()
// desc: retrieve a class's member variable of type float
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_float( CK_DL_API api, Chuck_DL_Api::Object obj, const char* name, t_CKFLOAT & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_float, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object* ck_obj = (Chuck_Object*)obj;
        // get float at offset
        value = OBJ_MEMBER_FLOAT(ck_obj, offset);
    }
    // done
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_dur()
// desc: retrieve a class's member variable of type dur
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_dur( CK_DL_API api, Chuck_DL_Api::Object obj, const char * name, t_CKDUR & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_dur, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object* ck_obj = (Chuck_Object*)obj;
        // get dur at offset
        value = OBJ_MEMBER_DUR(ck_obj, offset);
    }
    // done
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_time()
// desc: retrieve a class's member variable of type time
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_time( CK_DL_API api, Chuck_DL_Api::Object obj, const char * name, t_CKTIME & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_time, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object * ck_obj = (Chuck_Object*)obj;
        // get time value at offset
        value = OBJ_MEMBER_TIME(ck_obj, offset);
    }
    // done
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_string()
// desc: retrieve a class's member variable of type string
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_string( CK_DL_API api, Chuck_DL_Api::Object obj, const char * name, Chuck_DL_Api::String & str )
{
    // default value
    str = NULL;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_string, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object * ck_obj = (Chuck_Object*)obj;
        // get string at offset
        str = OBJ_MEMBER_STRING(ck_obj, offset);
    }
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_get_mvar_object()
// desc: retrieve a class's member variable of type object
//-----------------------------------------------------------------------------
static t_CKBOOL ck_get_mvar_object( CK_DL_API api, Chuck_DL_Api::Object obj, const char * name, Chuck_DL_Api::Object & object )
{
    // default
    object = NULL;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // TODO how to do this?
    t_CKBOOL success = ck_get_mvar(obj, name, te_object, offset);
    // check
    if (success)
    {
        // get object
        Chuck_Object * ck_obj = (Chuck_Object*)obj;
        // get object at offset
        object = OBJ_MEMBER_OBJECT(ck_obj, offset);
    }
    // done
    return success;
}


//-----------------------------------------------------------------------------
// name: ck_set_string()
// desc: set a chuck string
//-----------------------------------------------------------------------------
static t_CKBOOL ck_set_string( CK_DL_API api, Chuck_DL_Api::String s, const char * str )
{
    // check | 1.5.0.1 (ge) changed from assert s != NULL
    if( s == NULL ) return FALSE;
    // cast to string
    Chuck_String * string = (Chuck_String *) s;
    // action
    string->set( str != NULL ? str : "" );
    // done
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_array4_size()
// desc: get size of an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL ck_array4_size( CK_DL_API api, Chuck_DL_Api::Array4 a, t_CKINT & value )
{
    // default value
    value = 0;
    // check
    if( a == NULL ) return FALSE;

    // cast to array4
    Chuck_Array4 * array = (Chuck_Array4 *)a;

    value = array->size();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_array4_push_back()
// desc: push back an element into an array | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL ck_array4_push_back( CK_DL_API api, Chuck_DL_Api::Array4 a, t_CKUINT value )
{
    // check
    if( a == NULL ) return FALSE;
    // cast to array4
    Chuck_Array4 * array = (Chuck_Array4 *)a;
    // action
    array->push_back( value );
    // done
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_array4_get_idx()
// desc: get an indexed element from an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL ck_array4_get_idx( CK_DL_API api, Chuck_DL_Api::Array4 a, t_CKINT idx, t_CKUINT & value )
{
    // check
    if( a == NULL ) return FALSE;
    // cast to array4
    Chuck_Array4 * array = (Chuck_Array4 *)a;
    // action
    return array->get( idx, &value );
}


//-----------------------------------------------------------------------------
// name: ck_array4_get()
// desc: get a keyed element from an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL ck_array4_get_key( CK_DL_API api, Chuck_DL_Api::Array4 a, const std::string& key, t_CKUINT & value )
{
    // check
    if( a == NULL ) return FALSE;
    // cast to array4
    Chuck_Array4 * array = (Chuck_Array4 *)a;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// constructor for the VMApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::Api::VMApi::VMApi() :
get_srate(ck_get_srate),
srate(ck_srate),
create_event_buffer(ck_create_event_buffer),
queue_event(ck_queue_event)
{ }




//-----------------------------------------------------------------------------
// constructor for the ObjectApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::Api::ObjectApi::ObjectApi() :
get_type(ck_get_type),
create(ck_create),
create_string(ck_create_string),
get_mvar_int(ck_get_mvar_int),
get_mvar_float(ck_get_mvar_float),
get_mvar_dur(ck_get_mvar_dur),
get_mvar_time(ck_get_mvar_time),
get_mvar_string(ck_get_mvar_string),
get_mvar_object(ck_get_mvar_object),
set_string(ck_set_string),
array4_size(ck_array4_size),
array4_push_back(ck_array4_push_back),
array4_get_idx(ck_array4_get_idx),
array4_get_key(ck_array4_get_key)
{ }




// windows
#if defined(__PLATFORM_WINDOWS__)
#include <system_error> // std::system_category() | 1.5.1.4
extern "C"
{
#ifndef __CHUNREAL_ENGINE__
  #include <windows.h>
#else
  // 1.5.0.0 (ge) | #chunreal
  // unreal engine on windows disallows including windows.h
  #include "Windows/MinWindows.h"
#endif // #ifndef __CHUNREAL_ENGINE__

void *dlopen( const char *path, int mode )
{
#ifndef __CHUNREAL_ENGINE__
    return (void *)LoadLibrary(path);
#else
    // 1.5.0.0 (ge) | #chunreal; explicitly call ASCII version
    // the build envirnment seems to force UNICODE
    return (void *)LoadLibraryA(path);
#endif
}

int dlclose( void *handle )
{
    FreeLibrary((HMODULE)handle);
    return 1;
}

void *dlsym( void * handle, const char *symbol )
{
    return (void *)GetProcAddress((HMODULE)handle, symbol);
}

const char * dlerror( void )
{
    // get windows error code
    int error = GetLastError();
    // no error
    if( error == 0 ) return NULL;
    // 1.5.1.4 (azaday) convert error code to system message
    std::string error_msg = std::system_category().message( error );
    // 1.4.2.0 (ge) changed to snprintf
    snprintf( dlerror_buffer, DLERROR_BUFFER_LENGTH, "(%i) %s", error, error_msg.c_str() );
    // return error buffer
    return dlerror_buffer;
}
}
#endif
