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
// function prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Chuck_DL_Query implementation for chugin-side interface
//-----------------------------------------------------------------------------
Chuck_VM * CK_DLL_CALL ck_get_vm( Chuck_DL_Query * query );
CK_DL_API CK_DLL_CALL ck_get_api( Chuck_DL_Query * query );
Chuck_Env * CK_DLL_CALL ck_get_env( Chuck_DL_Query * query );
Chuck_Compiler * CK_DLL_CALL ck_get_compiler( Chuck_DL_Query * query );
Chuck_Carrier * CK_DLL_CALL ck_get_carrier( Chuck_DL_Query * query );
void CK_DLL_CALL ck_setname( Chuck_DL_Query * query, const char * name, t_CKBOOL attendSpecial );
void CK_DLL_CALL ck_setinfo( Chuck_DL_Query * query, const char * key, const char * value, t_CKBOOL attendSpecial );
const char * CK_DLL_CALL ck_getinfo( Chuck_DL_Query * query, const char * key );

void CK_DLL_CALL ck_add_arg( Chuck_DL_Query * query, const char * type, const char * name );
void CK_DLL_CALL ck_throw_exception( const char * exception, const char * desc, Chuck_VM_Shred * shred );
void CK_DLL_CALL ck_em_log( t_CKINT level, const char * text );
void CK_DLL_CALL ck_remove_all_shreds( Chuck_VM * vm );
Chuck_DL_Api::Type CK_DLL_CALL ck_type_lookup( Chuck_VM * vm, const char * name );
t_CKBOOL CK_DLL_CALL ck_type_isequal( Chuck_Type * lhs, Chuck_Type * rhs );
t_CKBOOL CK_DLL_CALL ck_type_isa( Chuck_Type * lhs, Chuck_Type * rhs );
void CK_DLL_CALL ck_callback_on_instantiate( f_callback_on_instantiate callback, Chuck_Type * base_type, Chuck_VM * vm, t_CKBOOL shouldSetShredOrigin );
ckte_Origin ck_origin_hint( Chuck_Type * type );
const char * CK_DLL_CALL ck_type_name( Chuck_Type * type );
const char * CK_DLL_CALL ck_type_base_name( Chuck_Type * type );
Chuck_VM_Shred * CK_DLL_CALL ck_get_origin_shred( Chuck_Object * object );
void CK_DLL_CALL ck_set_origin_shred( Chuck_Object * object, Chuck_VM_Shred * shred );
Chuck_VM_Shred * CK_DLL_CALL ck_shred_parent( Chuck_VM_Shred * shred );




//-----------------------------------------------------------------------------
// internal implementation of query functions
//-----------------------------------------------------------------------------
t_CKUINT CK_DLL_CALL ck_builtin_declversion() { return CK_DLL_VERSION; }




//-----------------------------------------------------------------------------
// name: ck_builtin_info()
// desc: builtin info func for internal modules
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_builtin_info( Chuck_DL_Query * query )
{
    // set nothing here
}




//-----------------------------------------------------------------------------
// name: ck_setname()
// desc: set the name of the module/chugin
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_setname( Chuck_DL_Query * query, const char * name, t_CKBOOL attendSpecial )
{
    // set the name
    query->dll_name = name ? name : "[no name]";
    // also set in info, but attendSpecial=FALSE to avoid infinite recursion
    if( attendSpecial ) ck_setinfo( query, CHUGIN_INFO_NAME, name, FALSE );
}
// overload to get around function pointer business for f_setname
void CK_DLL_CALL ck_setname( Chuck_DL_Query * query, const char * name )
{ ck_setname( query, name, TRUE ); }




//-----------------------------------------------------------------------------
// name: ck_setinfo() | 1.5.2.0
// desc: set info (by key) of the module/chugin
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_setinfo( Chuck_DL_Query * query, const char * key, const char * value, t_CKBOOL attendSpecial )
{
    // check for null key
    if( !key ) return;

    // check for null value
    if( !value )
    {
        // check for presence of entry
        if( query->dll_info.find( key ) != query->dll_info.end() )
        {
            // erase the entry
            query->dll_info.erase( key );
        }
    }
    else
    {
        // add or update it
        query->dll_info[key] = value;
    }

    // check for special keys
    if( attendSpecial )
    {
        // also set name, but attendSpecial=FALSE to avoid infinite recursion
        if( string(key) == CHUGIN_INFO_NAME ) ck_setname( query, value, FALSE );
    }
}
// overload to get around function pointer business for f_setinfo
void CK_DLL_CALL ck_setinfo( Chuck_DL_Query * query, const char * key, const char * value )
{ ck_setinfo( query, key, value, TRUE ); }




//-----------------------------------------------------------------------------
// name: ck_geinfo() | 1.5.2.0
// desc: get info (by key) of the module/chugin
//-----------------------------------------------------------------------------
const char * CK_DLL_CALL ck_getinfo( Chuck_DL_Query * query, const char * key )
{
    // check for null key
    if( !key ) return "";
    // check for presence of entry
    if( query->dll_info.find( key ) == query->dll_info.end() ) return "";
    // return
    return query->dll_info[key].c_str();
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
    query->curr_var = NULL; // 1.5.2.0
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
    f->name = "@construct";
    f->type = "void";
    f->ctor = ctor;
    f->fpKind = ae_fp_ctor; // 1.5.2.0

    // add
    query->curr_class->ctors.push_back( f );
    query->curr_func = f;
    query->curr_var = NULL; // 1.5.2.0
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
    f->name = "@destruct";
    f->type = "void";
    f->dtor = dtor;
    f->fpKind = ae_fp_dtor; // 1.5.2.0

    // add
    query->curr_class->dtor = f;
    query->curr_func = NULL;
    query->curr_var = NULL; // 1.5.2.0
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
    f->fpKind = ae_fp_mfun; // 1.5.2.0

    // add
    query->curr_class->mfuns.push_back( f );
    query->curr_func = f;
    query->curr_var = NULL; // 1.5.2.0
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
    f->fpKind = ae_fp_sfun; // 1.5.2.0

    // add
    query->curr_class->sfuns.push_back( f );
    query->curr_func = f;
    query->curr_var = NULL; // 1.5.2.0
}




//-----------------------------------------------------------------------------
// name: ck_add_op_overload_binary()
// desc: add binary operator overload; args included
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_op_overload_binary( Chuck_DL_Query * query, f_gfun addr,
                                            const char * type, const char * opName,
                                            const char * lhsType, const char * lhsName,
                                            const char * rhsType, const char * rhsName )
{
    // look up operator
    ae_Operator op = str2op(opName);
    if( op == ae_op_none )
    {
        // error
        EM_error2( 0, "class import: add_op_overload_binary invoked with unsupported operator '%s'...", opName );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = string("@operator") + opName;
    f->type = type;
    f->gfun = addr;
    f->fpKind = ae_fp_gfun; // 1.5.2.0
    f->opOverloadKind = ckte_op_overload_BINARY;
    f->op2overload = op;

    // add
    query->op_overloads.push_back( f );
    query->curr_func = NULL;

    // add arg LHS
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->type = lhsType; v->name = lhsName;
    f->args.push_back( v );

    // add arg RHS
    v = new Chuck_DL_Value;
    v->type = rhsType; v->name = rhsName;
    f->args.push_back( v );
}




//-----------------------------------------------------------------------------
// name: ck_add_op_overload_prefix()
// desc: add unary (prefix) operator overload; args included
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_op_overload_prefix( Chuck_DL_Query * query, f_gfun addr,
                                            const char * type, const char * opName,
                                            const char * argType, const char * argName )
{
    // look up operator
    ae_Operator op = str2op(opName);
    if( op == ae_op_none )
    {
        // error
        EM_error2( 0, "class import: add_op_overload_prefix invoked with unsupported operator '%s'...", opName );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = string("@operator") + opName;
    f->type = type;
    f->gfun = addr;
    f->fpKind = ae_fp_gfun; // 1.5.2.0
    f->opOverloadKind = ckte_op_overload_UNARY_PRE;
    f->op2overload = op;

    // add
    query->op_overloads.push_back( f );
    query->curr_func = NULL;

    // add arg
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->type = argType; v->name = argName;
    f->args.push_back( v );
}




//-----------------------------------------------------------------------------
// name: ck_add_op_overload_postfix()
// desc: add unary (postfix) operator overload; args included
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_op_overload_postfix( Chuck_DL_Query * query, f_gfun addr,
                                             const char * type, const char * opName,
                                             const char * argType, const char * argName )
{
    // look up operator
    ae_Operator op = str2op(opName);
    if( op == ae_op_none )
    {
        // error
        EM_error2( 0, "class import: add_op_overload_postfix invoked with unsupported operator '%s'...", opName );
        return;
    }

    // allocate
    Chuck_DL_Func * f = new Chuck_DL_Func;
    f->name = string("@operator") + opName;
    f->type = type;
    f->gfun = addr;
    f->fpKind = ae_fp_gfun; // 1.5.2.0
    f->opOverloadKind = ckte_op_overload_UNARY_POST;
    f->op2overload = op;

    // add
    query->op_overloads.push_back( f );
    query->curr_func = NULL;

    // add arg
    Chuck_DL_Value * v = new Chuck_DL_Value;
    v->type = argType; v->name = argName;
    f->args.push_back( v );
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
    query->curr_var = v;

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
    query->curr_var = v;
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
            //EM_log(CK_LOG_HERALD, "error importing class '%s' into type engine",
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
// desc: create a main thread hook data structure
//-----------------------------------------------------------------------------
Chuck_DL_MainThreadHook * CK_DLL_CALL ck_create_main_thread_hook( Chuck_DL_Query * query,
                                                                  f_mainthreadhook hook,
                                                                  f_mainthreadquit quit,
                                                                  void * bindle )
{
    return new Chuck_DL_MainThreadHook( hook, quit, bindle, query->carrier() );
}

//-----------------------------------------------------------------------------
// name: ck_register_callback_on_shutdown() | 1.5.2.5 (ge) added
// desc: register a callback function to be called on host exit
//       (both natural or SIGINT)
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_register_callback_on_shutdown( Chuck_DL_Query * query, f_callback_on_shutdown cb, void * bindle )
{
    // register
    query->vm()->register_callback_on_shutdown( cb, bindle );
}

//-----------------------------------------------------------------------------
// name: ck_register_shreds_watcher()
// desc: register a callback function to receive notifications
//       from the VM about shreds (add, remove, etc.)
//       `options` is a bit-wised OR of ckvmShredsWatcherFlag
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_register_shreds_watcher( Chuck_DL_Query * query, f_shreds_watcher cb, t_CKUINT options, void * bindle )
{
    // subscribe
    query->vm()->subscribe_watcher( cb, options, bindle );
}

//-----------------------------------------------------------------------------
// unregister a shreds notification callback
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_unregister_shreds_watcher( Chuck_DL_Query * query, f_shreds_watcher cb )
{
    query->vm()->remove_watcher( cb );
}


//-----------------------------------------------------------------------------
// name: ck_doc_class()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_doc_class( Chuck_DL_Query * query, const char * doc )
{
    // if no current class
    if( !query->curr_class ) return FALSE;
    // set it
    query->curr_class->doc = doc;
    // done
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ck_add_example()
// desc: set current class documentation
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_add_example( Chuck_DL_Query * query, const char * ex )
{
    // if no current class
    if( !query->curr_class ) return FALSE;
    // append it
    query->curr_class->examples.push_back(ex);
    // done
    return TRUE;
}


// set current function documentation
t_CKBOOL CK_DLL_CALL ck_doc_func( Chuck_DL_Query * query, const char * doc )
{
    // if no current function
    if( !query->curr_func ) return FALSE;
    // set it
    query->curr_func->doc = doc;
    // done
    return TRUE;
}


// set last mvar documentation
t_CKBOOL CK_DLL_CALL ck_doc_var( Chuck_DL_Query * query, const char * doc )
{
    // if no current var
    if( !query->curr_var ) return FALSE;
    // set it
    query->curr_var->doc = doc;
    // done
    return TRUE;
}




//------------------------------------------------------------------------------
// alternative functions to make stuff
//------------------------------------------------------------------------------
Chuck_DL_Func * CK_DLL_CALL make_new_ctor( f_ctor ctor )
{   return new Chuck_DL_Func( "void", "@construct", (t_CKUINT)ctor, ae_fp_ctor ); }
Chuck_DL_Func * CK_DLL_CALL make_new_mfun( const char * t, const char * n, f_mfun mfun )
{   return new Chuck_DL_Func( t, n, (t_CKUINT)mfun, ae_fp_mfun ); }
Chuck_DL_Func * CK_DLL_CALL make_new_sfun( const char * t, const char * n, f_sfun sfun )
{   return new Chuck_DL_Func( t, n, (t_CKUINT)sfun, ae_fp_sfun ); }
Chuck_DL_Value * CK_DLL_CALL make_new_arg( const char * t, const char * n )
{   return new Chuck_DL_Value( t, n ); }
Chuck_DL_Value * CK_DLL_CALL make_new_mvar( const char * t, const char * n, t_CKBOOL c )
{   return new Chuck_DL_Value( t, n, c ); }
Chuck_DL_Value * CK_DLL_CALL make_new_svar( const char * t, const char * n, t_CKBOOL c, void * a )
{   return new Chuck_DL_Value( t, n, c, a ); }



//-----------------------------------------------------------------------------
// Chuck_DL_Query implementation for chugin-side interface
//-----------------------------------------------------------------------------
Chuck_VM * CK_DLL_CALL ck_get_vm( Chuck_DL_Query * query ) { return query->vm(); }
CK_DL_API CK_DLL_CALL ck_get_api( Chuck_DL_Query * query ) { return query->api(); }
Chuck_Env * CK_DLL_CALL ck_get_env( Chuck_DL_Query * query ) { return query->env(); }
Chuck_Compiler * CK_DLL_CALL ck_get_compiler( Chuck_DL_Query * query ) { return query->compiler(); }
Chuck_Carrier * CK_DLL_CALL ck_get_carrier( Chuck_DL_Query * query ) { return query->carrier(); }



//-----------------------------------------------------------------------------
// Chuck_DL_Query host-side functions
//-----------------------------------------------------------------------------
Chuck_VM * Chuck_DL_Query::vm() const { return m_carrier->vm; }
CK_DL_API Chuck_DL_Query::api() const { return m_api; } // 1.5.1.5
Chuck_Env * Chuck_DL_Query::env() const { return m_carrier->env; }
Chuck_Compiler * Chuck_DL_Query::compiler() const { return m_carrier->compiler; }
Chuck_Carrier * Chuck_DL_Query::carrier() const { return m_carrier; }




//-----------------------------------------------------------------------------
// name: load()
// desc: load module (chugin/dll) from filename
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
// desc: load module (internal) from query func
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::load( f_ck_query query_func, t_CKBOOL lazy )
{
    m_query_func = query_func;
    m_api_version_func = ck_builtin_declversion;
    m_info_func = ck_builtin_info;
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
    // return if there already
    if( m_done_query )
        return &m_query;

    // probe DLL, for version and info
    if( !probe() )
        return NULL;

    // is version ok
    t_CKBOOL version_ok = FALSE;
    // chugin major version must == host major version
    // chugin minor version must <= host minor version
    if( m_apiVersionMajor == CK_DLL_VERSION_MAJOR &&
        m_apiVersionMinor <= CK_DLL_VERSION_MINOR)
        version_ok = TRUE;

    // if version not okay
    if( !version_ok )
    {
        // construct error string
        ostringstream oss;
        oss << "chugin API version incompatible..." << endl
            << "chugin path: '" << m_filename << "'" << endl
            << "chugin API version: " << m_apiVersionMajor << "." << m_apiVersionMinor
            << " VS host API version: " << CK_DLL_VERSION_MAJOR << "." << CK_DLL_VERSION_MINOR;
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
        m_last_error = string("unsuccessful query in dll/module '") + (m_filename.length() ? m_filename : m_id)
                       + string("'");
        return NULL;
    }

    // set flag
    m_done_query = TRUE;

    // process any operator overloads | 1.5.1.5 (ge & andrew) chaos^2
    if( m_query.op_overloads.size() )
    {
        // log
        EM_log( CK_LOG_INFO, "processing operator overload in chugin '%s'", m_filename.c_str() );
        // push log
        EM_pushlog();
        // iterate over overloads
        for( t_CKUINT i = 0; i < m_query.op_overloads.size(); i++ )
        {
            // get the dl func def
            Chuck_DL_Func * f = m_query.op_overloads[i];
            // log
            EM_log( CK_LOG_INFO, "processing '%s'", f->name.c_str() );
            // try to import
            if( !type_engine_import_op_overload( m_query.env(), f ) )
            {
                m_last_error = string("unsuccessful operator overload '") + f->name +
                               string("' in dll '") + m_filename + string("'");
                return NULL;
            }
            // delete entry (and zero out the array element)
            CK_SAFE_DELETE( m_query.op_overloads[i] );
        }
        // clear
        m_query.op_overloads.clear();
        // pop log
        EM_poplog();
    }

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
    if( !m_api_version_func )
        m_api_version_func = (f_ck_declversion)this->get_addr( CK_DECLVERSION_FUNC );
    if( !m_api_version_func )
        m_api_version_func = (f_ck_declversion)this->get_addr( (string("_")+CK_DECLVERSION_FUNC).c_str() );
    if( !m_api_version_func )
    {
        m_last_error = string("no version function found in dll '") + m_filename + string("'");
        return FALSE;
    }

    // check version
    t_CKUINT dll_version = m_api_version_func();
    // get major and minor version numbers
    m_apiVersionMajor = CK_DLL_VERSION_GETMAJOR(dll_version);
    m_apiVersionMinor = CK_DLL_VERSION_GETMINOR(dll_version);

    // get the address of the DL info function from the DLL
    if( !m_info_func )
        m_info_func = (f_ck_info)this->get_addr( CK_INFO_FUNC );
    if( !m_info_func )
        m_info_func = (f_ck_info)this->get_addr( (string("_")+CK_INFO_FUNC).c_str() );

    // if info func found
    if( m_info_func )
    {
        // query the module for info
        m_info_func( &m_query );
    }

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
// name: getinfo()
// desc: get query info
//-----------------------------------------------------------------------------
string Chuck_DLL::getinfo( const string & key )
{
    return ck_getinfo( &m_query, key.c_str() );
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
    return m_apiVersionMajor;
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
    return m_apiVersionMinor;
}




//-----------------------------------------------------------------------------
// name: compatible()
// desc: if version compatible between chugin and host
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_DLL::compatible()
{
    // probe dll
    if( !this->probe() ) return FALSE;
    // major AND minor version must match | 1.5.2.0 (ge)
    //   (was: major version must be same between chugin and host, and
    //   chugin minor version must less than or equal host minor version)
    if( m_apiVersionMajor == CK_DLL_VERSION_MAJOR &&
        m_apiVersionMinor == CK_DLL_VERSION_MINOR ) return TRUE;

    // error string
    m_last_error = string("incompatible API version: chugin (")
                        + ck_itoa(m_apiVersionMajor) + "." + ck_itoa(m_apiVersionMinor)
                        + string(") vs. host (")
                        + ck_itoa(CK_DLL_VERSION_MAJOR) + "." + ck_itoa(CK_DLL_VERSION_MINOR)
                        + ")";
    // done
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: Chuck_DL_Query
// desc: ...
//-----------------------------------------------------------------------------
Chuck_DL_Query::Chuck_DL_Query( Chuck_Carrier * carrier, Chuck_DLL * dll )
{
    // set the pointers to functions so the module can call
    ck_vm = ck_get_vm;
    ck_api = ck_get_api;
    setname = ck_setname;
    setinfo = ck_setinfo;
    begin_class = ck_begin_class;
    add_ctor = ck_add_ctor;
    add_dtor = ck_add_dtor;
    add_mfun = ck_add_mfun;
    add_sfun = ck_add_sfun;
    add_mvar = ck_add_mvar;
    add_svar = ck_add_svar;
    add_arg = ck_add_arg;
    add_op_overload_binary = ck_add_op_overload_binary;
    add_op_overload_prefix = ck_add_op_overload_prefix;
    add_op_overload_postfix = ck_add_op_overload_postfix;
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
    register_shreds_watcher = ck_register_shreds_watcher; // 1.5.1.5 (ge & andrew)
    unregister_shreds_watcher = ck_unregister_shreds_watcher; // 1.5.1.5 (ge & andrew)
    register_callback_on_shutdown = ck_register_callback_on_shutdown; // 1.5.2.5 (ge)
    m_carrier = carrier;
    dll_ref = dll; // 1.5.1.3 (ge) added

    dll_name = "[no name]";
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
    // get DL API reference | 1.5.1.5
    m_api = Chuck_DL_Api::instance();

    // clear error flag
    errorEncountered = FALSE;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: clear the query
//-----------------------------------------------------------------------------
void Chuck_DL_Query::clear()
{
    // set to no name
    dll_name = "[no name]";

    // delete classes
    for( t_CKUINT i = 0; i < classes.size(); i++ ) CK_SAFE_DELETE( classes[i] );
    // delete operator overloads
    for( t_CKUINT i = 0; i < op_overloads.size(); i++ ) CK_SAFE_DELETE( op_overloads[i] );

    // clear
    classes.clear();
    op_overloads.clear();
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
    // clean up
    for( t_CKUINT i = 0; i < args.size(); i++ )
        CK_SAFE_DELETE( args[i] );
    // clear
    args.clear();
    // zero
    opOverloadKind = ckte_op_overload_NONE;
    op2overload = ae_op_none;
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
Chuck_DL_Api Chuck_DL_Api::g_api;


//-----------------------------------------------------------------------------
// name: ck_srate() | add 1.5.1.5
// desc: host-side hook implementation for getting system srate
//-----------------------------------------------------------------------------
static t_CKUINT CK_DLL_CALL ck_srate( Chuck_VM * vm )
{
    return vm->srate();
}


//-----------------------------------------------------------------------------
// name: ck_now() | add 1.5.1.5
// desc: host-side hook implementation for getting chuck system now
//-----------------------------------------------------------------------------
static t_CKTIME CK_DLL_CALL ck_now( Chuck_VM * vm )
{
    return vm->now();
}


//-----------------------------------------------------------------------------
// name: create_event_buffer() | 1.5.1.5 (ge, andrew) added
// desc: host-side hoook implemenation for
//       creatinga new lock-free one-producer, one-consumer buffer
//-----------------------------------------------------------------------------
static CBufferSimple * CK_DLL_CALL ck_create_event_buffer( Chuck_VM * vm )
{
    return vm->create_event_buffer();
}


//-----------------------------------------------------------------------------
// name: queue_event() | 1.5.1.5 (ge, andrew) added
// desc: host-side hoook implemenation for queuing an event
//       NOTE num_msg must be 1; buffer created using create_event_buffer()
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_queue_event( Chuck_VM * vm, Chuck_Event * event, t_CKINT num_msg, CBufferSimple * buffer )
{
    return vm->queue_event( event, num_msg, buffer );
}


//-----------------------------------------------------------------------------
// name: ck_get_type()
// desc: host-side hook implementation for retrieving a type by name
//-----------------------------------------------------------------------------
static Chuck_DL_Api::Type CK_DLL_CALL ck_get_type( Chuck_Object * object )
{
    return object->type_ref;
}


//-----------------------------------------------------------------------------
// name: ck_get_vtable_offset()
// desc: function pointer get_type()
//-----------------------------------------------------------------------------
t_CKINT CK_DLL_CALL ck_get_vtable_offset( Chuck_VM * vm, Chuck_Type * t, const char * valueName )
{
    // find the offset for value by name
    Chuck_Value * value = type_engine_find_value( t, valueName );
    // value not found
    if( !value || !value->func_ref ) return -1;
    // return it
    return value->func_ref->vt_index;
}


//-----------------------------------------------------------------------------
// add reference count
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_add_ref( Chuck_DL_Api::Object object )
{
    Chuck_Object * obj = (Chuck_Object *)object;
    CK_SAFE_ADD_REF(obj);
}


//-----------------------------------------------------------------------------
// release reference count
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_release( Chuck_DL_Api::Object object )
{
    Chuck_Object * obj = (Chuck_Object *)object;
    CK_SAFE_RELEASE(obj);
}


//-----------------------------------------------------------------------------
// get reference count
//-----------------------------------------------------------------------------
t_CKUINT CK_DLL_CALL ck_refcount( Chuck_DL_Api::Object object )
{
    Chuck_Object * obj = (Chuck_Object *)object;
    if( !obj ) return 0;
    return obj->refcount();
}




//-----------------------------------------------------------------------------
// helper function
//-----------------------------------------------------------------------------
Chuck_Object * do_ck_create( Chuck_VM_Shred * shred, Chuck_VM * vm, Chuck_DL_Api::Type t, vector<t_CKINT> & caps, t_CKBOOL addRef )
{
    // type
    Chuck_Type * type = (Chuck_Type *)t;
    // object
    Chuck_Object * object = NULL;

    // check if type is some kind of array
    if( isa(type, vm->env()->ckt_array) )
    {
        // 1.5.2.0 (nshaheed) added
        t_CKINT index = 0;
        t_CKBOOL is_object = isa(type->array_type, vm->env()->ckt_object);
        // get array depth
        t_CKUINT depth = type->array_depth;
        // create capacity
        t_CKINT * capacity = new t_CKINT[depth];
        // make sure
        assert( depth == caps.size() );
        // iterate and copy
        for( t_CKUINT i = 0; i < depth; i++ ) capacity[i] = caps[i];
        // allocate array
        object = do_alloc_array( vm, shred,
                                 &capacity[depth - 1], capacity,
                                 getkindof(vm->env(), type->array_type),
                                 is_object, NULL, index, type );
        // clean up
        CK_SAFE_DELETE_ARRAY( capacity );
    }
    else
    {
        // instantiate and initialize
        if( shred ) object = instantiate_and_initialize_object( type, shred, vm );
        else object = instantiate_and_initialize_object( type, vm );
    }

    // if requested to add ref
    if( object && addRef ) CK_SAFE_ADD_REF(object);

    // done
    return object;
}




//-----------------------------------------------------------------------------
// name: ck_create_with_shred()
// desc: host-side hook implementation for instantiating and initializing
//       a ChucK object by type, with reference to a parent shred;
//       the shred reference is useful in certain scenarios:
//       if 1) create a chuck-side class 2) with a member function that
//       is then invoked from c++ (e.g., using ck_invoke_mfun_immediate_mode)
//       and 3) that member function references global scope variables
//-----------------------------------------------------------------------------
static Chuck_DL_Api::Object CK_DLL_CALL ck_create_with_shred( Chuck_VM_Shred * shred, Chuck_DL_Api::Type t, t_CKBOOL addRef )
{
    // check | 1.5.0.1 (ge) changed; used to be assert t != NULL
    if( t == NULL )
    {
        // print error message
        EM_error2( 0, "DL_Api:object:ck_create_with_shred: NULL object reference." );
        // done
        return NULL;
    }

    // type
    Chuck_Type * type = (Chuck_Type *)t;
    // do_alloc_array() expects a stack of intial capacities for
    // each dimension of the array. Because this is only expected
    // to be used through the chuck_dl API, which does not support
    // setting the capacity explicitly, only empty arrays are
    // initialized | 1.5.2.0
    vector<t_CKINT> caps;
    // check for array type
    if( type->array_depth )
        for( t_CKUINT i = 0; i < type->array_depth; i++ )
            caps.push_back(0);

    // process and return
    return (Chuck_DL_Api::Object)do_ck_create( shred, shred->vm_ref, t, caps, addRef );
}




//-----------------------------------------------------------------------------
// name: ck_create_without_shred()
// desc: host-side hook implementation for instantiating and initializing
//       a ChucK object by type, without a parent shred; see ck_create_with_shred()
//       for more details
//-----------------------------------------------------------------------------
static Chuck_DL_Api::Object CK_DLL_CALL ck_create_without_shred( Chuck_VM * vm, Chuck_DL_Api::Type t, t_CKBOOL addRef )
{
    // check | 1.5.0.1 (ge) changed; used to be assert t != NULL
    if( t == NULL )
    {
        // print error message
        EM_error2( 0, "DL_Api:object:ck_create_without_shred: NULL object reference." );
        // done
        return NULL;
    }

    // type
    Chuck_Type * type = (Chuck_Type *)t;
    // do_alloc_array() expects a stack of intial capacities for
    // each dimension of the array. Because this is only expected
    // to be used through the chuck_dl API, which does not support
    // setting the capacity explicitly, only empty arrays are
    // initialized | 1.5.2.0
    vector<t_CKINT> caps;
    // check for array type
    if( type->array_depth )
        for( t_CKUINT i = 0; i < type->array_depth; i++ )
            caps.push_back(0);

    // process and return
    return (Chuck_DL_Api::Object)do_ck_create( NULL, vm, t, caps, addRef );
}




//-----------------------------------------------------------------------------
// name: ck_create_string()
// desc: host-side hook implementation for creating a chuck string
//-----------------------------------------------------------------------------
Chuck_String * CK_DLL_CALL ck_create_string( Chuck_VM * vm, const char * cstr, t_CKBOOL addRef )
{
    // instantiate and initalize object
    Chuck_String * ckstr = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, vm );
    // if successfully instantiated
    if( ckstr )
    {
        // set the value
        ckstr->set( cstr ? cstr : "" );
        // if requested to add ref
        if( addRef ) CK_SAFE_ADD_REF(ckstr);
    }
    // return reference
    return ckstr;
}




//-----------------------------------------------------------------------------
// name: ck_obj_data() | 1.5.2.0 (ge) added
// desc: compute pointer to data segment + offset; use for member variable access
//-----------------------------------------------------------------------------
void * CK_DLL_CALL ck_obj_data( Chuck_Object * object, t_CKUINT byteOffset )
{
    // chuck for NULL
    if( !object ) return NULL;
    // return pointer
    return ((t_CKBYTE *)(object->data)) + byteOffset;
}




//-----------------------------------------------------------------------------
// name: ck_get_origin_shred()
// desc: get origin shred
//-----------------------------------------------------------------------------
Chuck_VM_Shred * CK_DLL_CALL ck_get_origin_shred( Chuck_Object * object )
{
    // check for NULL
    if( !object ) return NULL;
    // get it
    return object->originShred();
}




//-----------------------------------------------------------------------------
// name: ck_set_origin_shred()
// desc: set origin shred
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_set_origin_shred( Chuck_Object * object, Chuck_VM_Shred * shred )
{
    // check for NULL
    if( !object ) return;
    // set it
    object->setOriginShred( shred );
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar()
// desc: retrieve a class's member variable offset
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar(Chuck_DL_Api::Object o, const char * name, te_Type basic_type, t_CKINT & offset)
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
        EM_error2(0, "get_mvar(): member variable %s not found", mvar.c_str());
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
static t_CKBOOL CK_DLL_CALL ck_get_mvar_int( Chuck_DL_Api::Object obj, const char * name, t_CKINT & value )
{
    // default value
    value = 0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_int, offset);
    // check get int at offset
    if( success ) value = OBJ_MEMBER_INT(obj, offset); // get offset

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_float()
// desc: retrieve a class's member variable of type float
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_float( Chuck_DL_Api::Object obj, const char* name, t_CKFLOAT & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_float, offset);
    // check, get float at offset
    if( success ) value = OBJ_MEMBER_FLOAT(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_dur()
// desc: retrieve a class's member variable of type dur
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_dur( Chuck_DL_Api::Object obj, const char * name, t_CKDUR & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_dur, offset);
    // check, get dur at offset
    if( success ) value = OBJ_MEMBER_DUR(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_time()
// desc: retrieve a class's member variable of type time
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_time( Chuck_DL_Api::Object obj, const char * name, t_CKTIME & value )
{
    // default value
    value = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_time, offset);
    // check, get time value at offset
    if( success ) value = OBJ_MEMBER_TIME(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_vec2()
// desc: retrieve a class's member variable of type vec2/complex/polar
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_vec2( Chuck_DL_Api::Object obj, const char * name, t_CKVEC2 & value )
{
    // default value
    value.x = value.y = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_vec2, offset);
    // check, get vec2 value at offset
    if( success ) value = OBJ_MEMBER_VEC2(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_vec3()
// desc: retrieve a class's member variable of type vec3
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_vec3( Chuck_DL_Api::Object obj, const char * name, t_CKVEC3 & value )
{
    // default value
    value.x = value.y = value.z = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_vec3, offset);
    // check, get vec3 value at offset
    if( success ) value = OBJ_MEMBER_VEC3(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_vec4()
// desc: retrieve a class's member variable of type vec4
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_vec4( Chuck_DL_Api::Object obj, const char * name, t_CKVEC4 & value )
{
    // default value
    value.x = value.y = value.z = value.w = 0.0;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_vec4, offset);
    // check, get vec4 value at offset
    if( success ) value = OBJ_MEMBER_VEC4(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_object()
// desc: retrieve a class's member variable of type object
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_object( Chuck_DL_Api::Object obj, const char * name, Chuck_DL_Api::Object & object )
{
    // default
    object = NULL;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // TODO how to do this?
    t_CKBOOL success = ck_get_mvar(obj, name, te_object, offset);
    // check, get object at offset
    if( success ) object = OBJ_MEMBER_OBJECT(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_get_mvar_string()
// desc: retrieve a class's member variable of type string
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_get_mvar_string( Chuck_DL_Api::Object obj, const char * name, Chuck_DL_Api::String & str )
{
    // default value
    str = NULL;
    // check | 1.5.0.1 (ge) changed from assert obj != NULL
    if( obj == NULL ) return FALSE;

    t_CKINT offset = -1;
    // get mvar
    t_CKBOOL success = ck_get_mvar(obj, name, te_string, offset);
    // check, get string at offset
    if( success ) str = OBJ_MEMBER_STRING(obj, offset);

    // done
    return success;
}




//-----------------------------------------------------------------------------
// name: ck_string_set()
// desc: set a chuck string
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_string_set( Chuck_DL_Api::String s, const char * str )
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
// name: ck_string_get()
// desc: get a chuck string's internal c-string
//-----------------------------------------------------------------------------
static const char * CK_DLL_CALL ck_string_get( Chuck_String * str )
{
    // check
    if( str == NULL ) return NULL;
    // get the internal c string
    return str->str().c_str();
}




//-----------------------------------------------------------------------------
// name: ck_array_int_size()
// desc: get size of an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_int_size( Chuck_DL_Api::ArrayInt array )
{
    // check
    if( array == NULL ) return 0;
    // return size
    return array->size();
}




//-----------------------------------------------------------------------------
// name: ck_array_int_get_idx()
// desc: get an indexed element from an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_int_get_idx( Chuck_DL_Api::ArrayInt array, t_CKINT idx )
{
    // value
    t_CKINT value = 0;
    // try to retrive value by index
    if( array ) array->get( idx, &value );
    // return value
    return value;
}




//-----------------------------------------------------------------------------
// name: ck_array_int_get_key()
// desc: get a keyed element from an array | 1.5.1.3 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_int_get_key( Chuck_DL_Api::ArrayInt array, const char * key, t_CKINT & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// name: ck_array_int_push_back()
// desc: push back an element into an array | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_int_push_back( Chuck_DL_Api::ArrayInt array, t_CKINT value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    array->push_back( (t_CKUINT)value );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_array_int_clear()
// desc: clear array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static void CK_DLL_CALL ck_array_int_clear( Chuck_DL_Api::ArrayInt array )
{
    // check
    if( array == NULL ) return;
    // clear
    array->clear();
}




//-----------------------------------------------------------------------------
// name: ck_array_float_size()
// desc: get size of an array | 1.5.1.8 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_float_size( Chuck_DL_Api::ArrayFloat array )
{
    // check
    if( array == NULL ) return 0;
    // done
    return array->size();
}




//-----------------------------------------------------------------------------
// name: ck_array_float_get_idx()
// desc: get an indexed element from an array | 1.5.1.8 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKFLOAT CK_DLL_CALL ck_array_float_get_idx( Chuck_DL_Api::ArrayFloat array, t_CKINT idx )
{
    // value
    t_CKFLOAT value = 0;
    // attempt to retrieve value
    if( array ) array->get( idx, &value );
    // action
    return value;
}




//-----------------------------------------------------------------------------
// name: ck_array_float_get_key()
// desc: get a keyed element from an array | 1.5.1.8 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_float_get_key( Chuck_DL_Api::ArrayFloat array, const char * key, t_CKFLOAT & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// name: ck_array_float_push_back()
// desc: push back an element into an array | 1.5.1.8 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_float_push_back( Chuck_DL_Api::ArrayFloat a, t_CKFLOAT value )
{
    // check
    if( a == NULL ) return FALSE;
    // cast to array_float
    Chuck_ArrayFloat * array = (Chuck_ArrayFloat *)a;
    // action
    array->push_back( value );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_array_float_clear()
// desc: clear array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static void CK_DLL_CALL ck_array_float_clear( Chuck_DL_Api::ArrayFloat array )
{
    // check
    if( array == NULL ) return;
    // clear
    array->clear();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec2_size()
// desc: get size of an array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_vec2_size( Chuck_DL_Api::ArrayVec2 array )
{
    // check
    if( array == NULL ) return 0;
    // return size
    return array->size();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec2_get_idx()
// desc: get indexed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKVEC2 CK_DLL_CALL ck_array_vec2_get_idx( Chuck_DL_Api::ArrayVec2 array, t_CKINT idx )
{
    // value
    t_CKVEC2 value; value.x = value.y = 0;
    // try to retrive value by index
    if( array ) array->get( idx, &value );
    // return value
    return value;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec2_get_key()
// desc: get keyed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec2_get_key( Chuck_DL_Api::ArrayVec2 array, const char * key, t_CKVEC2 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// name: ck_array_vec2_push_back()
// desc: push back an element into an array | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec2_push_back( Chuck_DL_Api::ArrayVec2 array, const t_CKVEC2 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    array->push_back( value );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec2clear()
// desc: clear array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static void CK_DLL_CALL ck_array_vec2_clear( Chuck_DL_Api::ArrayVec2 array )
{
    // check
    if( array == NULL ) return;
    // clear
    array->clear();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec3_size()
// desc: get size of an array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_vec3_size( Chuck_DL_Api::ArrayVec3 array )
{
    // check
    if( array == NULL ) return 0;
    // return size
    return array->size();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec3_get_idx()
// desc: get indexed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKVEC3 CK_DLL_CALL ck_array_vec3_get_idx( Chuck_DL_Api::ArrayVec3 array, t_CKINT idx )
{
    // value
    t_CKVEC3 value; value.x = value.y = value.z = 0;
    // try to retrive value by index
    if( array ) array->get( idx, &value );
    // return value
    return value;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec3_get_key()
// desc: get keyed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec3_get_key( Chuck_DL_Api::ArrayVec3 array, const char * key, t_CKVEC3 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// name: ck_array_vec3_push_back()
// desc: push back an element into an array | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec3_push_back( Chuck_DL_Api::ArrayVec3 array, const t_CKVEC3 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    array->push_back( value );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec3clear()
// desc: clear array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static void CK_DLL_CALL ck_array_vec3_clear( Chuck_DL_Api::ArrayVec3 array )
{
    // check
    if( array == NULL ) return;
    // clear
    array->clear();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec4_size()
// desc: get size of an array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKINT CK_DLL_CALL ck_array_vec4_size( Chuck_DL_Api::ArrayVec4 array )
{
    // check
    if( array == NULL ) return 0;
    // return size
    return array->size();
}




//-----------------------------------------------------------------------------
// name: ck_array_vec4_get_idx()
// desc: get indexed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKVEC4 CK_DLL_CALL ck_array_vec4_get_idx( Chuck_DL_Api::ArrayVec4 array, t_CKINT idx )
{
    // value
    t_CKVEC4 value; value.x = value.y = value.z = value.w = 0;
    // try to retrive value by index
    if( array ) array->get( idx, &value );
    // return value
    return value;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec4_get_key()
// desc: get keyed element from array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec4_get_key( Chuck_DL_Api::ArrayVec4 array, const char * key, t_CKVEC4 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    return array->get( key, &value );
}




//-----------------------------------------------------------------------------
// name: ck_array_vec4_push_back()
// desc: push back an element into an array | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
static t_CKBOOL CK_DLL_CALL ck_array_vec4_push_back( Chuck_DL_Api::ArrayVec4 array, const t_CKVEC4 & value )
{
    // check
    if( array == NULL ) return FALSE;
    // action
    array->push_back( value );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: ck_array_vec4clear()
// desc: clear array | 1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
static void CK_DLL_CALL ck_array_vec4_clear( Chuck_DL_Api::ArrayVec4 array )
{
    // check
    if( array == NULL ) return;
    // clear
    array->clear();
}




//-----------------------------------------------------------------------------
// constructor for the VMApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::VMApi::VMApi() :
srate(ck_srate),
now(ck_now),
create_event_buffer(ck_create_event_buffer),
queue_event(ck_queue_event),
invoke_mfun_immediate_mode(ck_invoke_mfun_immediate_mode),
throw_exception(ck_throw_exception),
em_log(ck_em_log),
remove_all_shreds(ck_remove_all_shreds)
{ }




//-----------------------------------------------------------------------------
// constructor for the ObjectApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::ObjectApi::ObjectApi() :
get_type(ck_get_type),
add_ref(ck_add_ref),
release(ck_release),
refcount(ck_refcount),
create(ck_create_with_shred),
create_without_shred(ck_create_without_shred),
create_string(ck_create_string),
data(ck_obj_data),
get_origin_shred(ck_get_origin_shred),
set_origin_shred(ck_set_origin_shred),
get_mvar_int(ck_get_mvar_int),
get_mvar_float(ck_get_mvar_float),
get_mvar_dur(ck_get_mvar_dur),
get_mvar_time(ck_get_mvar_time),
get_mvar_vec2(ck_get_mvar_vec2),
get_mvar_vec3(ck_get_mvar_vec3),
get_mvar_vec4(ck_get_mvar_vec4),
get_mvar_string(ck_get_mvar_string),
get_mvar_object(ck_get_mvar_object),
set_string(ck_string_set),
str(ck_string_get),
array_int_size(ck_array_int_size),
array_int_get_idx(ck_array_int_get_idx),
array_int_get_key(ck_array_int_get_key),
array_int_push_back(ck_array_int_push_back),
array_int_clear(ck_array_int_clear),
array_float_size(ck_array_float_size),
array_float_get_idx(ck_array_float_get_idx),
array_float_get_key(ck_array_float_get_key),
array_float_push_back(ck_array_float_push_back),
array_float_clear( ck_array_float_clear ),
array_vec2_size(ck_array_vec2_size),
array_vec2_get_idx(ck_array_vec2_get_idx),
array_vec2_get_key(ck_array_vec2_get_key),
array_vec2_push_back(ck_array_vec2_push_back),
array_vec2_clear( ck_array_vec2_clear ),
array_vec3_size(ck_array_vec3_size),
array_vec3_get_idx(ck_array_vec3_get_idx),
array_vec3_get_key(ck_array_vec3_get_key),
array_vec3_push_back(ck_array_vec3_push_back),
array_vec3_clear( ck_array_vec3_clear ),
array_vec4_size(ck_array_vec4_size),
array_vec4_get_idx(ck_array_vec4_get_idx),
array_vec4_get_key(ck_array_vec4_get_key),
array_vec4_push_back(ck_array_vec4_push_back),
array_vec4_clear( ck_array_vec4_clear )
{ }




//-----------------------------------------------------------------------------
// constructor for the TypeApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::TypeApi::TypeApi() :
lookup(ck_type_lookup),
get_vtable_offset(ck_get_vtable_offset),
is_equal(ck_type_isequal),
isa(ck_type_isa),
callback_on_instantiate(ck_callback_on_instantiate),
origin_hint(ck_origin_hint),
name(ck_type_name),
base_name(ck_type_base_name)
{ }




//-----------------------------------------------------------------------------
// constructor for the ShredApi; connects function pointers to host-side impl
//-----------------------------------------------------------------------------
Chuck_DL_Api::ShredApi::ShredApi() :
parent(ck_shred_parent)
{ }



//-----------------------------------------------------------------------------
// name: ck_invoke_mfun_immediate_mode()
// desc: directly call a chuck member function
//       (supports both native (c++) and user (chuck)
//-----------------------------------------------------------------------------
Chuck_DL_Return CK_DLL_CALL ck_invoke_mfun_immediate_mode( Chuck_Object * obj, t_CKUINT func_vt_offset, Chuck_VM * vm, Chuck_VM_Shred * caller_shred, Chuck_DL_Arg * args_list, t_CKUINT numArgs )
{
    Chuck_DL_Return RETURN;
    // check objt
    if( !obj ) return RETURN;
    // verify bounds
    if( func_vt_offset >= obj->vtable->funcs.size() )
    {
        EM_error3( "(internal error) ck_invoke_mfun_immediate_mode() encountered invalid virtual function index: %lu", func_vt_offset );
        return RETURN;
    }

    // get the member function
    Chuck_Func * func = obj->vtable->funcs[func_vt_offset];
    // get the code for the function
    Chuck_VM_Code * code = func->code;
    // check whether native or user
    if( code->native_func )
    {
        // native func (defined in c++)
        f_mfun f = (f_mfun)code->native_func;
        // pack the args_list into memory
        func->pack_cache( args_list, numArgs );
        // call the function | added 1.3.0.0: the DL API instance
        f( obj, func->args_cache, &RETURN, vm, caller_shred, Chuck_DL_Api::instance() );
    }
    else
    {
        // user func (defined in chuck)
        // attach invoker, if needed
        if( !func->setup_invoker(func_vt_offset, vm, caller_shred ) )
        {
            // error
            EM_error3( "ck_invoke_mfun_immediate_mode() cannot set up invoker for: %s", func->signature(FALSE,TRUE).c_str() );
            return RETURN;
        }
        // pack the args_list into vector
        vector<Chuck_DL_Arg> args_vector;
        // iterate over c-style array
        for( t_CKUINT i = 0; i < numArgs; i++ ) args_vector.push_back(args_list[i]);
        // invoke the invoker
        func->invoker_mfun->invoke( obj, args_vector, caller_shred );
    }

    // return it
    return RETURN;
}




//-----------------------------------------------------------------------------
// name: ck_throw_exception()
// desc: throw an exception, if shred is passed it, it will be halted
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_throw_exception( const char * exception, const char * desc,
                                     Chuck_VM_Shred * shred )
{
    // constructor string
    string e = exception ? string(exception) : "[unnamed]";
    if( desc ) e += ": " + string(desc);

    // display
    EM_exception( e.c_str() );
    // if shred passed in
    if( shred )
    {
        // halt shred
        shred->is_running = FALSE;
        // mark as done
        shred->is_done = TRUE;
    }
}




//-----------------------------------------------------------------------------
// name: ck_em_log()
// desc: host impl: print to chuck logger
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_em_log( t_CKINT level, const char * text )
{
    // check
    if( !text ) return;
    // display
    EM_log( level, text );
}




//-----------------------------------------------------------------------------
// name: ck_remove_all_shreds()
// desc: host impl for system function: remove all shreds in VM; use with care
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_remove_all_shreds( Chuck_VM * vm )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REMOVEALL;
    // remove all shreds
    vm->queue_msg( msg );
}




//-----------------------------------------------------------------------------
// name: ck_type_lookup()
// desc: host-side hook implementation for retrieving a type by name
//-----------------------------------------------------------------------------
Chuck_DL_Api::Type CK_DLL_CALL ck_type_lookup( Chuck_VM * vm, const char * name )
{
    // get the type
    Chuck_Env * env = vm->env();
    a_Id_List list = new_id_list( name, 0, 0 /*, NULL*/ ); // TODO: nested types
    Chuck_Type * t = type_engine_find_type( env, list );
    delete_id_list( list );
    // return
    return (Chuck_DL_Api::Type)t;
}




//-----------------------------------------------------------------------------
// name: ck_type_isequal()
// desc: test if two types are equivalent
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_type_isequal( Chuck_Type * lhs, Chuck_Type * rhs )
{
    if( !lhs || !rhs ) return FALSE;
    return *lhs == *rhs;
}




//-----------------------------------------------------------------------------
// name: ck_type_isa()
// desc: test if lhs is a type of rhs
//-----------------------------------------------------------------------------
t_CKBOOL CK_DLL_CALL ck_type_isa( Chuck_Type * lhs, Chuck_Type * rhs )
{
    if( !lhs || !rhs ) return FALSE;
    return ::isa( lhs, rhs );
}




//-----------------------------------------------------------------------------
// name: ck_callback_on_instantiate()
// desc: register a callback to be invoked whenever a base-type (or its subclass)
//       is instantiated, with option for type system to auto-set shred origin if available
//-----------------------------------------------------------------------------
void CK_DLL_CALL ck_callback_on_instantiate( f_callback_on_instantiate callback,
    Chuck_Type * base_type, Chuck_VM * vm, t_CKBOOL shouldSetShredOrigin )
{
    // register the callback with chuck type
    base_type->add_instantiate_cb( callback, shouldSetShredOrigin );
}




//-----------------------------------------------------------------------------
// name: ck_origin_hint()
// desc: where did the type originate? e.g., chugin, builtin, user-defined, etc.
//-----------------------------------------------------------------------------
ckte_Origin CK_DLL_CALL ck_origin_hint( Chuck_Type * type )
{
    // null
    if( !type ) return ckte_origin_UNKNOWN;
    // return origin hint
    return type->originHint;
}




//-----------------------------------------------------------------------------
// name: ck_type_name()
// desc: get type name (full, with decorations such as [])
//-----------------------------------------------------------------------------
const char * CK_DLL_CALL ck_type_name( Chuck_Type * type )
{
    // null
    if( !type ) return "";
    // return type name
    return type->c_name();
}




//-----------------------------------------------------------------------------
// name: ck_type_base_name()
// desc: get type base name (no [] decorations)
//-----------------------------------------------------------------------------
const char * CK_DLL_CALL ck_type_base_name( Chuck_Type * type )
{
    // null
    if( !type ) return "";
    // return origin hint
    return type->base_name.c_str();
}




//-----------------------------------------------------------------------------
// name: ck_shred_parent()
// desc: where did the type originate? e.g., chugin, builtin, user-defined, etc.
//-----------------------------------------------------------------------------
Chuck_VM_Shred * CK_DLL_CALL ck_shred_parent( Chuck_VM_Shred * shred )
{
    // null
    if( !shred ) return NULL;
    // return origin hint
    return shred->parent;
}




//-----------------------------------------------------------------------------
// windows translation
//-----------------------------------------------------------------------------
#if defined(__PLATFORM_WINDOWS__)
#include <system_error> // std::system_category() | 1.5.1.5
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
    // 1.5.1.5 (azaday) convert error code to system message
    std::string error_msg = std::system_category().message( error );
    // 1.4.2.0 (ge) changed to snprintf
    snprintf( dlerror_buffer, DLERROR_BUFFER_LENGTH, "(%i) %s", error, error_msg.c_str() );
    // return error buffer
    return dlerror_buffer;
}

} // extern "C"
#endif
