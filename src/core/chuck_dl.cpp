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
#include <sstream>
using namespace std;




#if defined(__MACOSX_CORE__)
char g_default_chugin_path[] = "/usr/local/lib/chuck:/Library/Application Support/ChucK/ChuGins:~/Library/Application Support/ChucK/ChuGins";
#elif defined(__PLATFORM_WIN32__)
char g_default_chugin_path[] = "C:\\WINDOWS\\system32\\ChucK;C:\\Program Files\\ChucK\\chugins;C:\\Program Files (x86)\\ChucK\\chugins";
#else // Linux/Cygwin
char g_default_chugin_path[] = "/usr/local/lib/chuck";
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
        
        Chuck_Type * ck_parent_type = type_engine_find_type( query->env(), parent_path );
        
        delete_id_list( parent_path );
        
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
        // recursive
        query->curr_class->classes.push_back( c );
    // 1.3.2.0: do not save class for later import (will import it on class close)
//    else
//        // first level
//        query->classes.push_back( c );

    // remember info
    c->name = name;
    c->parent = parent;
    c->current_mvar_offset = parent_offset;
    
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
    
    Chuck_Type * ck_type = type_engine_find_type( query->env(), path );
    
    delete_id_list( path );
    
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
void CK_DLL_CALL ck_add_ugen_ctrl( Chuck_DL_Query * query, f_ctrl ugen_ctrl, f_cget ugen_cget,
                                   const char * type, const char * name )
{
    // make sure there is class
    if( !query->curr_class )
    {
        // error
        EM_error2( 0, "class import: add_ugen_func invoked without begin_class..." );
        return;
    }
    
    // allocate
    Chuck_DL_Ctrl * c = new Chuck_DL_Ctrl;
    c->name = name;
    c->type = type;
    c->ctrl = ugen_ctrl;
    c->cget = ugen_cget;
    
    // set
    query->curr_func = NULL;
}




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
            EM_log(CK_LOG_SEVERE, "[chuck](DL): error importing class '%s' into type engine",
                   query->curr_class->name.c_str());
            
            // pop
            assert( query->stack.size() );
            query->curr_class = query->stack.back();
            query->stack.pop_back();
            
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
// name: ck_doc_class()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_doc_class( Chuck_DL_Query * query, const char * doc )
{
#ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_class)
        query->curr_class->doc = doc;
    else
        return FALSE;
#endif // CK_DOC
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: ck_add_example()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_add_example( Chuck_DL_Query * query, const char * ex )
{
#ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_class)
        query->curr_class->examples.push_back(ex);
    else
        return FALSE;
#endif // CK_DOC
    
    return TRUE;
}

// set current function documentation
t_CKBOOL CK_DLL_CALL ck_doc_func( Chuck_DL_Query * query, const char * doc )
{
#ifdef CK_DOC // disable unless CK_DOC
    if(query->curr_func)
        query->curr_func->doc = doc;
    else
        return FALSE;
#endif // CK_DOC
    
    return TRUE;
}

// set last mvar documentation
t_CKBOOL CK_DLL_CALL ck_doc_var( Chuck_DL_Query * query, const char * doc )
{
#ifdef CK_DOC // disable unless CK_DOC
    if(query->last_var)
        query->last_var->doc = doc;
    else
        return FALSE;
#endif // CK_DOC
    
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
const Chuck_DL_Query * Chuck_DLL::query( )
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
        m_last_error = string("no version function found in dll '") 
        + m_filename + string("'");
        return NULL;
    }
    
    // check version
    t_CKUINT dll_version = m_version_func();
    t_CKBOOL version_ok = FALSE;
    // oops lets have version 4 be ok actually
    if(CK_DLL_VERSION_GETMAJOR(dll_version) == 4)
        version_ok = TRUE;
    // major version must be same
    // minor version must less than or equal
    if(CK_DLL_VERSION_GETMAJOR(dll_version) == CK_DLL_VERSION_MAJOR &&
       CK_DLL_VERSION_GETMINOR(dll_version) <= CK_DLL_VERSION_MINOR)
        version_ok = TRUE;
    
    if(!version_ok)
        // SPENCERTODO: do they need to be equal, or can dll_version be < ?
    {
        ostringstream oss;
        oss << "DL version not supported: " 
        << CK_DLL_VERSION_GETMAJOR(dll_version)
        << "."
        << CK_DLL_VERSION_GETMINOR(dll_version)
        << " in '"
        << m_filename
        << "'";
        
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
        m_last_error = string("no query function found in dll '") 
                       + m_filename + string("'");
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
    if( !m_query_func( &m_query ) )
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
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_DLL::name() const
{
    return m_id.c_str();
}


//const t_CKUINT Chuck_DL_Query::RESERVED_SIZE;

//-----------------------------------------------------------------------------
// name: Chuck_DL_Query
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Query::Chuck_DL_Query( Chuck_Carrier * carrier )
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
    add_ugen_ctrl = ck_add_ugen_ctrl;
    end_class = ck_end_class;
    doc_class = ck_doc_class;
    doc_func = ck_doc_func;
    doc_var = ck_doc_var;
    m_carrier = carrier;
    
//    memset(reserved2, NULL, sizeof(void*)*RESERVED_SIZE);
    
    dll_name = "[noname]";
    reserved = NULL;
    curr_class = NULL;
    curr_func = NULL;
    
    srate = carrier->vm->srate();

    linepos = 0;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_DL_Query::clear()
{
    // set to 0
    dll_name = "[noname]";
    // line pos
    linepos = 0;
    // delete classes
    for( t_CKUINT i = 0; i < classes.size(); i++ ) delete classes[i];
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




/*******************************************************************************
 
 Chuck_DL_Api stuff 
 
*******************************************************************************/

namespace Chuck_DL_Api
{
    Api Api::g_api;
}


static t_CKUINT ck_get_srate(CK_DL_API api, Chuck_VM_Shred * shred)
{
    return shred->vm_ref->srate();
}

static Chuck_DL_Api::Type ck_get_type( CK_DL_API api, Chuck_VM_Shred * shred, std::string & name )
{
    Chuck_Env * env = shred->vm_ref->env();
    a_Id_List list = new_id_list( name.c_str(), 0 ); // TODO: nested types
    
    Chuck_Type * t = type_engine_find_type( env, list );
    
    delete_id_list( list );
    
    return ( Chuck_DL_Api::Type ) t;
}

static Chuck_DL_Api::Object ck_create( CK_DL_API api, Chuck_VM_Shred * shred, Chuck_DL_Api::Type t )
{
    assert( t != NULL );
    
    Chuck_Type * type = ( Chuck_Type * ) t;
    Chuck_Object * o = instantiate_and_initialize_object( type, shred->vm_ref );
    
    return ( Chuck_DL_Api::Object ) o;
}

static Chuck_DL_Api::String ck_create_string( CK_DL_API api, Chuck_VM_Shred * shred, std::string & str )
{
    Chuck_String * string = ( Chuck_String * ) instantiate_and_initialize_object( shred->vm_ref->env()->t_string, shred->vm_ref );
    
    string->set( str );
    
    return ( Chuck_DL_Api::String ) string;
}

static t_CKBOOL ck_get_mvar_int( CK_DL_API api, Chuck_DL_Api::Object, std::string &, t_CKINT & )
{
    return TRUE;
}

static t_CKBOOL ck_get_mvar_float( CK_DL_API api, Chuck_DL_Api::Object, std::string &, t_CKFLOAT & )
{
    return TRUE;
}

static t_CKBOOL ck_get_mvar_dur( CK_DL_API api, Chuck_DL_Api::Object, std::string &, t_CKDUR & )
{
    return TRUE;
}

static t_CKBOOL ck_get_mvar_time( CK_DL_API api, Chuck_DL_Api::Object, std::string &, t_CKTIME & )
{
    return TRUE;
}

static t_CKBOOL ck_get_mvar_string( CK_DL_API api, Chuck_DL_Api::Object, std::string &, Chuck_DL_Api::String & )
{
    return TRUE;
}

static t_CKBOOL ck_get_mvar_object( CK_DL_API api, Chuck_DL_Api::Object, std::string &, Chuck_DL_Api::Object & )
{
    return TRUE;
}

static t_CKBOOL ck_set_string( CK_DL_API api, Chuck_DL_Api::String s, std::string & str )
{
    assert( s != NULL );
    
    Chuck_String * string = ( Chuck_String * ) s;
    string->set( str );
    
    return TRUE;
}


Chuck_DL_Api::Api::VMApi::VMApi() :
get_srate(ck_get_srate)
{ }


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
set_string(ck_set_string)
{ }


// windows
#if defined(__WINDOWS_DS__)
extern "C"
{
#include <windows.h>

void *dlopen( const char *path, int mode )
{
    return (void *)LoadLibrary(path);
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
    int error = GetLastError();
    if( error == 0 ) return NULL;
    sprintf( dlerror_buffer, "%i", error );
    return dlerror_buffer;
}
}
#endif
