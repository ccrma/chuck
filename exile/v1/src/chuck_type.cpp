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
// file: chuck_types.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

#include "chuck_type.h"
#include "chuck_instr.h"
#include "chuck_vm.h"
#include "chuck_dl.h"
#include "chuck_errmsg.h"
#include "chuck_symbol.h"




//-----------------------------------------------------------------------------
// name: struct t_Env_
// desc: type check env
//-----------------------------------------------------------------------------
struct t_Env_
{
    // lookup tables
    S_table value;
    S_table type;
    S_table function;
    S_table ugen;
    S_table name_space;
    S_table class_defs;
    S_table addr;
    
    // scope maps
    vector<map<S_Symbol, S_Symbol> *> scope;

    // data
    S_Symbol name;
    t_Env parent;
    t_CKBOOL is_class;
    map<Chuck_DLL *, bool> dlls;
    Chuck_VM * vm;
    
    // state
    t_CKBOOL is_global;
    a_Func_Def func_def;
    S_Symbol nspc_name;
    t_CKBOOL is_nspc;
    t_Env child;
    
    t_Env_() { this->scope_push(); }
    ~t_Env_() { this->scope_pop(); }
    // functions
    void scope_push()
    { scope.push_back( new map<S_Symbol, S_Symbol> ); }
    void scope_pop()
    { if( scope.size() ) { delete scope.back(); scope.pop_back(); } }
    void scope_add( S_Symbol id )
    { assert( scope.size() ); (*scope.back())[id] = id; }
    t_CKBOOL scope_lookup( S_Symbol id )
    { assert( scope.size() ); return (*scope.back())[id] != NULL; }
};




//-----------------------------------------------------------------------------
// name: struct t_Class_Info_
// desc: class def
//-----------------------------------------------------------------------------
struct t_Class_Info_
{
    t_Type type;
    string name;
    vector<t_Type> fields;
    vector<a_Func_Def> funcs;

    // constructor
    t_Class_Info_() { type = NULL; }
};




// types
struct t_Type_ t_void = { te_void, "void", NULL, 0 };
struct t_Type_ t_int = { te_int, "int", NULL, sizeof(t_CKINT) };
struct t_Type_ t_uint = { te_uint, "uint", NULL, sizeof(t_CKUINT) };
// NOTE: float is single
struct t_Type_ t_single = { te_single, "single", NULL, sizeof(float) };
struct t_Type_ t_float = { te_float, "float", NULL, sizeof(double) };
struct t_Type_ t_double = { te_double, "double", NULL, sizeof(double) };
// note hard code in the next two lines
struct t_Type_ t_time = { te_time, "time", NULL, sizeof(t_CKTIME) };
struct t_Type_ t_dur = { te_dur, "dur", NULL, sizeof(t_CKTIME) };
struct t_Type_ t_function = { te_function, "function", NULL, sizeof(void *) };
struct t_Type_ t_null = { te_null, "NULL", NULL, sizeof(void *) };
struct t_Type_ t_unit = { te_unit, "()", NULL, sizeof(int) };
struct t_Type_ t_code = { te_code, "code", NULL, sizeof(void *) };
struct t_Type_ t_object = { te_object, "object", NULL, sizeof(void *) };
struct t_Type_ t_class = { te_class, "class", NULL, sizeof(void *) };
struct t_Type_ t_array = { te_array, "array", NULL, sizeof(void *) };
struct t_Type_ t_tuple = { te_tuple, "tuple", NULL, sizeof(void *) };
struct t_Type_ t_string = { te_string, "string", NULL, sizeof(void *) };
struct t_Type_ t_pattern = { te_pattern, "pattern", &t_object, sizeof(void *) };
struct t_Type_ t_thread = { te_thread, "thread", &t_object, sizeof(void *) };
struct t_Type_ t_shred = { te_shred, "shred", &t_object, sizeof(void *) };
struct t_Type_ t_ugen = { te_ugen, "ugen", &t_object, sizeof(void *) };
struct t_Type_ t_transport = { te_transport, "transport", &t_object, sizeof(void *) };
struct t_Type_ t_host = { te_host, "host", &t_object, sizeof(void *) };
struct t_Type_ t_midiout = { te_midiout, "midiout", &t_object, sizeof(void *) };
struct t_Type_ t_midiin = { te_midiin, "midiin", &t_object, sizeof(void *) };
struct t_Type_ t_adc = { te_adc, "adc", &t_ugen, t_ugen.size };
struct t_Type_ t_dac = { te_dac, "dac", &t_ugen, t_ugen.size };
struct t_Type_ t_bunghole = { te_bunghole, "bunghole", &t_ugen, t_ugen.size };


// system types (internal - cannot instantiate)
struct t_Type_ t_system_time = 
    { __te_system_time__, "@__system_time__", &t_object, sizeof(void *) };
struct t_Type_ t_system_out = 
    { __te_system_out__, "@__system_out__", &t_object, sizeof(void *) };
struct t_Type_ t_system_err =
    { __te_system_err__, "@__system_err__", &t_object, sizeof(void *) };
struct t_Type_ t_system_namespace =
    { __te_system_namespace__, "@__system_namespace__", NULL, sizeof(void *) };


// forward reference
t_Env type_engine_init();
t_CKBOOL type_engine_shutdown( t_Env env );
t_CKBOOL type_engine_check_prog( t_Env env, a_Program prog );
t_CKBOOL type_engine_check_func_def( t_Env env, a_Func_Def func_def );
t_CKBOOL type_engine_check_func_def_import( t_Env nspc, a_Func_Def func_def );
t_CKBOOL type_engine_check_ugen_def_import( t_Env nspc, Chuck_UGen_Info * ugen );
t_CKBOOL type_engine_check_value_import( t_Env nspc, S_Symbol name, S_Symbol type, void * addr );
t_CKBOOL type_engine_check_stmt_list( t_Env env, a_Stmt_List list );
t_CKBOOL type_engine_check_stmt( t_Env env, a_Stmt stmt );
t_Type type_engine_check_exp( t_Env env, a_Exp exp );
t_Type type_engine_check_primary( t_Env env, a_Exp_Primary exp );
t_Type type_engine_check_op( t_Env env, ae_Operator op, 
                             t_Type left, t_Type right );
t_Type type_engine_check_exp_binary( t_Env env, a_Exp_Binary binary );
t_Type type_engine_check_exp_unary( t_Env env, a_Exp_Unary unary );
t_Type type_engine_check_exp_cast( t_Env env, a_Exp_Cast cast );
t_Type type_engine_check_exp_postfix( t_Env env, a_Exp_Postfix postfix );
t_Type type_engine_check_exp_dur( t_Env env, a_Exp_Dur dur );
t_Type type_engine_check_exp_array( t_Env env, a_Exp_Array array );
t_Type type_engine_check_exp_func_call( t_Env env, a_Exp_Func_Call func_call );
t_Type type_engine_check_exp_dot_member( t_Env env, a_Exp_Dot_Member member );
t_Type type_engine_check_exp_if( t_Env env, a_Exp_If exp_if );
t_Type type_engine_check_exp_decl( t_Env env, a_Exp_Decl decl );
t_Type type_engine_check_exp_namespace( t_Env env, a_Exp_Namespace name_space );
t_CKBOOL type_engine_check_if( t_Env env, a_Stmt_If stmt );
t_CKBOOL type_engine_check_for( t_Env env, a_Stmt_For stmt );
t_CKBOOL type_engine_check_code_segment( t_Env env, a_Stmt_Code stmt );
t_CKBOOL type_engine_check_while( t_Env env, a_Stmt_While stmt );
t_CKBOOL type_engine_check_until( t_Env env, a_Stmt_Until stmt );
t_CKBOOL type_engine_check_switch( t_Env env, a_Stmt_Switch stmt );
t_CKBOOL type_engine_check_return( t_Env env, a_Stmt_Return stmt );
t_CKBOOL type_engine_check_cast_valid( t_Env env, t_Type to, t_Type from );

t_CKBOOL type_engine_check_exp_mem( t_Env env, a_Exp exp );
t_CKBOOL type_engine_check_primary_mem( t_Env env, a_Exp_Primary exp );
t_CKBOOL type_engine_check_exp_dot_member_mem( t_Env env, a_Exp_Dot_Member member );
t_CKBOOL type_engine_check_exp_array_mem( t_Env env, a_Exp_Array array );
t_CKBOOL type_engine_check_exp_func_call_mem( t_Env env, a_Exp_Func_Call func_call );

// make a new type
t_Type make_type( const char * name, t_Type parent, unsigned int size )
{
    t_Type t = (t_Type)checked_malloc(sizeof(struct t_Type_));
    t->type = __te_system_user__;
    t->name = (char *)checked_malloc(strlen(name)+1);
    strcpy( t->name, name );
    t->parent = parent;
    t->size = size;
    t->dyn = TRUE;
    
    return t;
}

// dup type
t_Type dup_type( t_Type type )
{
    t_Type t = make_type( type->name, type->parent, type->size );
    t->type = type->type;
    return t;
}

// isa
t_CKBOOL isa( t_Type a, t_Type b )
{
    if( !a || !b ) return FALSE;
    if( a->type != __te_system_user__ )
        if( a->type == b->type ) return TRUE;
    else
        if( !strcmp( a->name, b->name ) ) return TRUE;
    return isa( a->parent, b );
}



//-----------------------------------------------------------------------------
// name: new_namespace()
// desc: initialize a namespace
//-----------------------------------------------------------------------------
t_Env new_namespace( const char * name, t_Env parent, unsigned size )
{
    // allocate a new env
    t_Env e = new t_Env_;
    
    // allocate tables
    e->type = S_empty2( size );
    e->value = S_empty2( size );
    e->function = S_empty2( size );
    e->ugen = S_empty2( size );
    e->name_space = S_empty2( size );
    e->addr = S_empty2( size );
    e->name = insert_symbol( name );
    e->parent = parent;
    e->vm = ( parent ? parent->vm : NULL );
    e->is_class = FALSE; // this is a namespace

    // init the state
    e->is_global = TRUE;
    e->func_def = NULL;
    e->nspc_name = NULL;
    e->is_nspc = TRUE;
    e->child = NULL;
    
    return e;
}




//-----------------------------------------------------------------------------
// name: type_engine_init()
// desc: initialize a type engine
//-----------------------------------------------------------------------------
t_Env type_engine_init( Chuck_VM * vm )
{
    // allocate a new env
    t_Env e = new_namespace( "global", NULL, 65437 );  
    e->vm = vm;

    // enter the type mapping
    S_enter( e->type, insert_symbol(t_void.name), &t_void );
    S_enter( e->type, insert_symbol(t_int.name), &t_int );
    S_enter( e->type, insert_symbol(t_uint.name), &t_uint );
    S_enter( e->type, insert_symbol(t_float.name), &t_float );
    S_enter( e->type, insert_symbol(t_time.name), &t_time );
    S_enter( e->type, insert_symbol(t_dur.name), &t_dur );
    S_enter( e->type, insert_symbol(t_function.name), &t_function );
    S_enter( e->type, insert_symbol(t_null.name), &t_null );
    S_enter( e->type, insert_symbol(t_unit.name), &t_unit );
    S_enter( e->type, insert_symbol(t_code.name), &t_code );
    S_enter( e->type, insert_symbol(t_object.name), &t_object );
    S_enter( e->type, insert_symbol(t_class.name), &t_class );
    S_enter( e->type, insert_symbol(t_array.name), &t_array );
    S_enter( e->type, insert_symbol(t_tuple.name), &t_tuple );
    S_enter( e->type, insert_symbol(t_string.name), &t_string );
    S_enter( e->type, insert_symbol(t_pattern.name), &t_pattern );
    S_enter( e->type, insert_symbol(t_thread.name), &t_thread );
    S_enter( e->type, insert_symbol(t_shred.name), &t_shred );
    S_enter( e->type, insert_symbol(t_transport.name), &t_transport );
    S_enter( e->type, insert_symbol(t_host.name), &t_host );
    S_enter( e->type, insert_symbol(t_midiout.name), &t_midiout );
    S_enter( e->type, insert_symbol(t_midiin.name), &t_midiin );
    S_enter( e->type, insert_symbol(t_ugen.name), &t_ugen );
    // S_enter( e->type, insert_symbol(t_adc.name), &t_adc );
    // S_enter( e->type, insert_symbol(t_dac.name), &t_dac );

    // null
    S_enter( e->value, insert_symbol( "null" ), &t_null );
    // time
    S_enter( e->value, insert_symbol( "now" ), &t_time );
    S_enter( e->value, insert_symbol( "start" ), &t_time );
    // dur
    S_enter( e->value, insert_symbol( "samp" ), &t_dur );
    S_enter( e->value, insert_symbol( "ms" ), &t_dur );
    S_enter( e->value, insert_symbol( "second" ), &t_dur );
    S_enter( e->value, insert_symbol( "minute" ), &t_dur );
    S_enter( e->value, insert_symbol( "hour" ), &t_dur );
    S_enter( e->value, insert_symbol( "day" ), &t_dur );
    S_enter( e->value, insert_symbol( "week" ), &t_dur );
    
    S_enter( e->value, insert_symbol( "language" ), &t_null );
    S_enter( e->value, insert_symbol( "compiler" ), &t_null );
    S_enter( e->value, insert_symbol( "machine" ), &t_null );

    S_enter( e->value, insert_symbol( "chout" ), &t_system_out );
    S_enter( e->value, insert_symbol( "cherr" ), &t_system_err );
    S_enter( e->value, insert_symbol( "stdout" ), &t_system_out );
    S_enter( e->value, insert_symbol( "stderr" ), &t_system_err );
    S_enter( e->value, insert_symbol( "midiout" ), &t_midiout );
    S_enter( e->value, insert_symbol( "midiin" ), &t_midiin );
    S_enter( e->value, insert_symbol( "adc" ), &t_adc );
    S_enter( e->value, insert_symbol( "dac" ), &t_dac );
    S_enter( e->value, insert_symbol( "bunghole" ), &t_bunghole );
    S_enter( e->value, insert_symbol( "blackhole" ), &t_bunghole );
    
    S_enter( e->value, insert_symbol( "true" ), &t_int );
    S_enter( e->value, insert_symbol( "false" ), &t_int );
    S_enter( e->value, insert_symbol( "maybe" ), &t_int );
    S_enter( e->value, insert_symbol( "pi" ), &t_float );
    S_enter( e->value, insert_symbol( "endl" ), &t_string );
    
    // global namespace
    S_enter( e->value, insert_symbol( "global" ), &t_system_namespace );
    S_enter( e->name_space, insert_symbol( "global" ), e );

    return e;
}




//-----------------------------------------------------------------------------
// name: do_make_args()
// desc: ...
//-----------------------------------------------------------------------------
a_Arg_List do_make_args( const vector<Chuck_Info_Param> & params, unsigned int index )
{
    a_Arg_List args = NULL;
    if( index >= params.size() )
        return NULL;

    if( index == (params.size()-1) )
        args = new_arg_list( new_type_decl( (char*)params[index].type.c_str(), 0 ),
                             (char*)params[index].name.c_str(), 0 );
    else
        args = prepend_arg_list(
            new_type_decl( (char*)params[index].type.c_str(), 0 ),
            (char*)params[index].name.c_str(),
            do_make_args( params, index + 1 ), 0 );
    return args;
}


// ctrl op
UGEN_CTRL ugen_ctrl_op( t_CKTIME now, void * data, void * value ) { }
UGEN_CGET ugen_cget_op( t_CKTIME now, void * data, void * out ) { }

// ctrl gain
UGEN_CTRL ugen_ctrl_gain( t_CKTIME now, void * data, void * value ) { }
UGEN_CGET ugen_cget_gain( t_CKTIME now, void * data, void * out ) { }

// cget last
UGEN_CGET ugen_cget_last( t_CKTIME now, void * data, void * out ) { }


//-----------------------------------------------------------------------------
// name: type_engine_add_dll()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_add_dll( t_Env env, Chuck_DLL * dll, const char * nspc )
{    
    // lookup the namesapce
    t_Env info = (t_Env)S_look( env->name_space,
                                insert_symbol( (char *)nspc ) );
    // if not there
    if( info && info->dlls[dll] )
        return FALSE;

    if( !info )
    {
        info = new_namespace( nspc, env, 211 );
        S_enter( env->value, insert_symbol(nspc), &t_system_namespace );
        env->scope_add( insert_symbol(nspc) );
        S_enter( env->name_space, insert_symbol(nspc), info );
    }

    // add all the prototypes
    Chuck_DL_Query * query = (Chuck_DL_Query *)dll->query();
    for( unsigned int i = 0; i < query->dll_exports.size(); i++ )
    {
        // the prototype
        Chuck_DL_Proto * proto = &query->dll_exports[i];

        // add func
        if( proto->is_func )
        {
            // type decl
            a_Type_Decl rtype = new_type_decl( (char *)proto->type.c_str(), 0 );
            // arg list
            a_Arg_List args = NULL;
            if( proto->params.size() )
                args = do_make_args( proto->params, 0 );
            // allocate a new function
            a_Func_Def func = new_func_def( 
                ae_func_func, rtype, (char*)proto->name.c_str(), args, NULL, 0 );
            // set the pointer
            func->builtin = (builtin_func_ptr)proto->addr;
            func->linepos = query->linepos;
            // type check it
            if( !type_engine_check_func_def_import( info, func ) )
            {
                // clean up
                info->dlls.erase( dll );
                return FALSE;
            }
        }
        else
        {
            // add value
            if( !type_engine_check_value_import( info,
                insert_symbol((char *)proto->name.c_str()),
                insert_symbol((char *)proto->type.c_str()),
                (void *)proto->addr ) )
            {
                info->dlls.erase( dll );
                return FALSE;
            }
        }
    }

    // add the unit generators
    for( unsigned int j = 0; j < query->ugen_exports.size(); j++ )
    {
        Chuck_UGen_Info * ugen = new Chuck_UGen_Info( query->ugen_exports[j] );
        
        if( !type_engine_check_ugen_def_import( env, ugen ) )
        {
            // clean up
            // info->dlls.erase( dll );
            return FALSE;
        }
    }

    // flag
    info->dlls[dll] = true;

    return TRUE;
}





//-----------------------------------------------------------------------------
// name: type_engine_shutdown()
// desc: shutdown a type engine
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_shutdown( t_Env env )
{
    // deallocate
    if( env->type ) free( env->type );
    if( env->value ) free( env->value );
    if( env->function ) free( env->function );
    if( env->ugen ) free( env->ugen );
    if( env->name_space ) free( env->name_space );

    delete env;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_prog()
// desc: type check a program
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_prog( t_Env env, a_Program prog )
{
    t_CKBOOL ret = TRUE;

    if( !prog )
        return FALSE;

    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            ret = type_engine_check_stmt_list( env, prog->section->stmt_list );
            break;
        
        case ae_section_func:
            ret = type_engine_check_func_def( env, prog->section->func_def );
            break;

        case ae_section_class:
            EM_error2( prog->linepos, "class def not impl!" );
            ret = FALSE;
            break;
        }

        prog = prog->next;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_begin() type_engine_end()
// desc: ...
//-----------------------------------------------------------------------------
void type_engine_begin( t_Env env )
{ S_beginScope( env->value ); env->scope_push(); }
void type_engine_end( t_Env env )
{ S_endScope( env->value ); env->scope_pop(); }




//-----------------------------------------------------------------------------
// name: type_engine_check_func_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_func_def( t_Env env, a_Func_Def f )
{
    // look up the value
    if( S_look( env->value, f->name ) )
    {
        EM_error2( f->linepos, 
            "function name '%s' is already used by another value", S_name(f->name) );
        return FALSE;
    }

    // enter the name into the value table
    S_enter( env->value, f->name, &t_function );
    env->scope_add( f->name );
    // enter the name into the function table
    S_enter( env->function, f->name, f );
    // user
    f->s_type = ae_func_user;

    // push the scope
    S_beginScope( env->value );
    env->scope_push( );
    // set global
    env->is_global = FALSE;
    // set the func def
    env->func_def = f;

    // make room for return address and pc
    S_enter( env->value, insert_symbol( "@__pc__" ), &t_uint );
    S_enter( env->value, insert_symbol( "@__mem_sp__" ), &t_uint );

    // look up the return type
    f->ret_type = (t_Type)S_look( env->type, f->type_decl->id );
    // no return type
    if( !f->ret_type )
    {
        EM_error2( f->linepos, "for function '%s':", S_name(f->name) );
        EM_error2( f->linepos, "undefined return type '%s'", S_name(f->type_decl->id) );
        return FALSE;
    }
    // f->ret_type->array_depth = f->type_decl->array;

    a_Arg_List arg_list = f->arg_list;
    unsigned int count = 1;
    f->stack_depth = 0;

    while( arg_list )
    {
        // look up in type table
        arg_list->type = (t_Type)S_look( env->type, arg_list->type_decl->id );
        if( !arg_list->type )
        {
            EM_error2( arg_list->linepos, "in function '%s':", S_name(f->name) );
            EM_error2( arg_list->linepos, "argument %i '%s' has undefined type '%s'", 
                count, S_name(arg_list->id), S_name(arg_list->type_decl->id) );
            return FALSE;
        }
        
        // look up in scope
        if( env->scope_lookup( arg_list->id ) )
        {
            EM_error2( arg_list->linepos, "in function '%s':", S_name(f->name) );
            EM_error2( arg_list->linepos, "argument %i '%s' is already defined in this scope",
                count, S_name(arg_list->id) );
            return FALSE;
        }

        // enter into value table
        S_enter( env->value, arg_list->id, arg_list->type );
        env->scope_add( arg_list->id );        

        // stack
        f->stack_depth += arg_list->type->size;

        // next arg
        arg_list = arg_list->next;
    }

    // type check the code
    if( !type_engine_check_stmt( env, f->code ) )
    {
        EM_error2( 0, "...in function '%s'.", S_name(f->name) );
        return FALSE;
    }

    // set global
    env->is_global = TRUE;
    // pop the scope
    S_endScope( env->value );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_func_def_import()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_func_def_import( t_Env info, a_Func_Def f )
{
    // look up the value
    if( lookup_value( info, f->name, FALSE ) )
    {
        EM_error2( f->linepos,
            "imported identifier '%s' is already in use in namespace '%s'",
            S_name(f->name), S_name(info->name) );
        return FALSE;
    }
    
    // look up the return type
    f->ret_type = lookup_type( info, f->type_decl->id );
    if( !f->ret_type )
    {
        EM_error2( f->linepos, "imported function '%s.%s':", S_name(info->name), S_name(f->name) );
        EM_error2( f->linepos, "undefined return type '%s'", S_name(f->type_decl->id) );
        return FALSE;
    }
    f->ret_type->array_depth = f->type_decl->array;
    
    a_Arg_List arg_list = f->arg_list;
    unsigned int count = 1;
    f->stack_depth = 0;
    
    while( arg_list )
    {
        // look up in type table
        arg_list->type = lookup_type( info, arg_list->type_decl->id );
        if( !arg_list->type )
        {
            EM_error2( arg_list->linepos, "in imported function '%s.%s':", 
                       S_name(info->name), S_name(f->name) );
            EM_error2( arg_list->linepos, "argument %i '%s' has undefined type '%s'", 
                       count, S_name(arg_list->id), S_name(arg_list->type_decl->id) );
            return FALSE;
        }
        
        // stack depth
        f->stack_depth += arg_list->type->size;
        
        // next arg
        arg_list = arg_list->next;
    }

    // enter the name into the value table
    S_enter( info->value, f->name, &t_function );
    // enter function into the function table
    S_enter( info->function, f->name, f );
    // builtin
    f->s_type = ae_func_builtin;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_ugen_def_import()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_ugen_def_import( t_Env info, Chuck_UGen_Info * ugen )
{
    map<string, bool> params;
    
    // look up the type
    if( lookup_type( info, insert_symbol(ugen->name.c_str()), FALSE ) )
    {
        EM_error2( ugen->linepos,
            "imported ugen '%s.%s': \n  type identifier '%s' is in use in namespace '%s'",
            S_name(info->name), ugen->name.c_str(), ugen->name.c_str(), S_name(info->name) );
        return FALSE;
    }
    
    // no return type
    if( !ugen->tick )
    {
        EM_error2( ugen->linepos, 
                   "imported ugen '%s.%s': no [tick] method defined",
                   S_name(info->name), ugen->name.c_str() );
        return FALSE;
    }
    
    // add default
    ugen->add( ugen_ctrl_op, ugen_cget_op, "int", "op" );
    ugen->add( ugen_ctrl_gain, ugen_cget_gain, "float", "gain" );
    ugen->add( NULL, ugen_cget_last, "float", "last" );

    // loop through ctrl parameters
    for( unsigned int i = 0; i < ugen->param_list.size(); i++ )
    {
        const Chuck_Info_Param * param = &ugen->param_list[i];
        
        // check the type
        t_Type t = lookup_type( info, insert_symbol(param->type.c_str()) );
        if( !t )
        {
            EM_error2( ugen->linepos,
                "imported ugen '%s.%s': unrecognized type '%s' for control parameter '%s'",
                S_name(info->name), ugen->name.c_str(), param->type.c_str(), param->name.c_str() );
            return FALSE;
        }

        // XXX - pld 
        // check for duplicates - unless ugen has declared a parent,
        // and we expect to find some duplicates.
        // in general, this just updates the param_map and the
        // older functions are 'forgotten' though still present 
        // in the param list
        if( ugen->parent == "" && params[param->name] )
        {
            EM_error2( ugen->linepos, 
                "imported ugen '%s.%s': duplicate control parameter name '%s'",
                S_name(info->name), ugen->name.c_str(), param->name.c_str() );
            return FALSE;
        }

        // make sure there is a function
        if( !param->ctrl_addr && !param->cget_addr )
        {
            EM_error2( ugen->linepos,
                "imported ugen '%s.%s': no ctrl or cget function defined for param '%s'",
                S_name(info->name), ugen->name.c_str(), param->name.c_str() );
            return FALSE;
        }
        
        params[param->name] = true;
    }
    
    // enter the name into the type table
    S_enter( info->type, insert_symbol(ugen->name.c_str()),
             make_type( ugen->name.c_str(), &t_ugen, t_ugen.size ) );
    // enter into the ugen table
    S_enter( info->ugen, insert_symbol(ugen->name.c_str()), (void *)ugen );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_value_import()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_value_import( t_Env info, S_Symbol name,
                                         S_Symbol type, void * addr )
{
    // lookup the value
    if( lookup_value( info, name, FALSE ) )
    {
        EM_error2( 0,
                   "imported identifier '%s' is already in use in namespace '%s'",
                   name, S_name(info->name) );
        return FALSE;
    }

    // lookup the type
    t_Type t = lookup_type( info, type );
    if( !t )
    {
        EM_error2( 0,
                   "imported identifier '%s.%s' has unrecognized type '%s'",
                   S_name(info->name), name, type );
        return FALSE;
    }

    // insert value
    S_enter( info->value, name, t );
    // insert addr
    S_enter( info->addr, name, addr );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_stmt_list()
// desc: type check a stmt list
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt_list( t_Env env, a_Stmt_List list )
{
    // push the scope
    env->scope_push();
    
    // type check the stmt_list
    while( list )
    {
        if( !type_engine_check_stmt( env, list->stmt ) )
        {
            //env->out() << "type checker: statement in list does not type "
            //           << "check..." << endl;
            return FALSE;
        }
        
        list = list->next;
    }
    
    // pop the scope
    env->scope_pop();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_stmt()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt( t_Env env, a_Stmt stmt )
{
    t_CKBOOL ret = TRUE;

    if( !stmt )
        return TRUE;

    switch( stmt->s_type )
    {
        case ae_stmt_if:
            // env->print( "stmt_if" );
            ret = type_engine_check_if( env, &stmt->stmt_if );
            break;

        case ae_stmt_for:
            // env->print( "stmt_for" );
            ret = type_engine_check_for( env, &stmt->stmt_for );
            break;

        case ae_stmt_while:
            // env->print( "stmt_while" );
            ret = type_engine_check_while( env, &stmt->stmt_while );
            break;
            
        case ae_stmt_until:
            // env->print( "stmt_until" );
            ret = type_engine_check_until( env, &stmt->stmt_until );
            break;
            
        case ae_stmt_exp:
            // evn->print( "stmt_exp" );
            ret = ( type_engine_check_exp( env, stmt->stmt_exp ) != NULL );
            break;

        case ae_stmt_switch:
            // env->print( "switch? not implemented" );
            ret = type_engine_check_switch( env, &stmt->stmt_switch );
            break;

        case ae_stmt_return:
            // env->print( "return" );
            ret = type_engine_check_return( env, &stmt->stmt_return );
            break;

        case ae_stmt_case:
            // env->print( "case" );
            // ret = type_engine_check_case( env, &stmt->stmt_case );
            break;

        case ae_stmt_gotolabel:
            // env->print( "gotolabel" );
            // ok
            break;

        case ae_stmt_break:
            // env->print( "break? not implemented" );
            // ok here
            break;

        case ae_stmt_continue:
            // env->print( "continue" );
            // ok
            break;

        case ae_stmt_code:
            // env->print( "code segment" );
            S_beginScope( env->value );
            ret = type_engine_check_code_segment( env, &stmt->stmt_code );
            S_endScope( env->value );
            break;

        //case ae_stmt_func:
        //    // env->print( "func_def" );
        //    ret = type_engine_check_func_def( env, stmt->stmt_func );
        //    break;
        
        default:
            EM_error2( stmt->linepos, 
                "internal compiler error - no stmt type '%i'!", stmt->s_type );
            break;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp()
// desc: type check an exp
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp( t_Env env, a_Exp exp )
{
    a_Exp curr = exp;

    while( curr )
    {
        curr->type = NULL;

        switch( curr->s_type )
        {
        case ae_exp_binary:
            curr->type = type_engine_check_exp_binary( env, &curr->binary );
        break;
    
        case ae_exp_unary:
            curr->type = type_engine_check_exp_unary( env, &curr->unary );
        break;
    
        case ae_exp_cast:
            curr->type = type_engine_check_exp_cast( env, &curr->cast );
        break;
    
        case ae_exp_postfix:
            curr->type = type_engine_check_exp_postfix( env, &curr->postfix );
        break;
    
        case ae_exp_dur:
            curr->type = type_engine_check_exp_dur( env, &curr->dur );
        break;
    
        case ae_exp_primary:
            curr->type = type_engine_check_primary( env, &curr->primary );
        break;
    
        case ae_exp_array:
            curr->type = type_engine_check_exp_array( env, &curr->array );
        break;
    
        case ae_exp_func_call:
            curr->type = type_engine_check_exp_func_call( env, &curr->func_call );
            curr->func_call.ret_type = curr->type;
        break;
    
        case ae_exp_dot_member:
            curr->type = type_engine_check_exp_dot_member( env, &curr->dot_member );
        break;
    
        case ae_exp_if:
            curr->type = type_engine_check_exp_if( env, &curr->exp_if );
        break;
    
        case ae_exp_decl:
            curr->type = type_engine_check_exp_decl( env, &curr->decl );
        break;

        case ae_exp_namespace:
            curr->type = type_engine_check_exp_namespace( env, &curr->name_space );
        break;
    
        /* case ae_exp_new:
            curr->exp_type = (t_Type)S_look( env->type, curr->a_new->type );
        break;
        */
        
        default:
            EM_error2( curr->linepos, "internal compiler error - no expression '%i'",
                curr->s_type );
            return NULL;
        }

        curr = curr->next;
    }

    // return type
    return exp->type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_primary()
// desc: check primary
//-----------------------------------------------------------------------------
t_Type type_engine_check_primary( t_Env env, a_Exp_Primary exp )
{
    t_Type type = NULL;
    
    switch( exp->s_type )
    {
    case ae_primary_var:
        {
            type = lookup_value( env, exp->var );
            if( !type )
                EM_error2( exp->linepos, "undefined variable '%s'",
                    S_name(exp->var) );
            else if( type->type == __te_system_namespace__ ||
                     type->type == __te_system_class__ )
            {
                env->nspc_name = exp->var;
                env->is_nspc = type->type == __te_system_namespace__;
                env->child = ( type->type == __te_system_namespace__ ?
                               lookup_namespace( env, exp->var ) : NULL );
            }
            else if( isa( type, &t_ugen ) )
            {
                env->nspc_name = exp->var;
            }
        }
        break;

    case ae_primary_num:
        type = &t_int;
        break;
        
    case ae_primary_uint:
        type = &t_uint;
        break;
        
    case ae_primary_float:
        type = &t_float;
        break;

    case ae_primary_str:
        type = &t_string;
        break;
    
    case ae_primary_exp:
        type = type_engine_check_exp( env, exp->exp );
        break;

    default:
        EM_error2( exp->linepos, 
            "internal compiler error - no primary expression '%i'", exp->s_type );
        break;
    }

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_chuck()
// desc: type check chuck operator
//-----------------------------------------------------------------------------
t_Type type_engine_check_op_chuck( t_Env env, t_Type left, t_Type right )
{
    if( (left->type == te_string) && (left->type == right->type) )
        return &t_string;
    if( (left->type == te_dur) && (right->type == left->type) )
        return &t_dur;
    if( (left->type == te_dur) && (right->type == te_time) )
        return &t_time;
    if( left->type == __te_system_out__ )
        return left;
    if( right->type == __te_system_out__ )
        return left;
    if( left->type == te_int && right->type == te_midiout )
        return left;
    if( left->type == te_midiout && right->type == te_int )
        return left;
    if( left->type == te_midiin && right->type == te_int )
        return right;
    if( left->type != te_null && left->type == right->type )
        return right;
    
    // system
    if( left->type == te_string && right->type == __te_system_namespace__ )
        return right;

    // TODO: bad
    if( left->type == te_int && right->type == te_uint )
        return left;
    if( left->type == te_uint && right->type == te_int )
        return left;
    if( left->parent && right->parent &&
        left->parent->type == te_ugen && right->parent->type == te_ugen )
        return right;

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_op_unchuck( t_Env env, t_Type left, t_Type right )
{
    if( left->parent && right->parent &&
        left->parent->type == te_ugen && right->parent->type == te_ugen )
        return &t_ugen;

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op()
// desc: type check op
//-----------------------------------------------------------------------------
t_Type type_engine_check_op( t_Env env, ae_Operator op, 
                             t_Type left, t_Type right )
{
    switch( op )
    {
    case ae_op_chuck:
        return type_engine_check_op_chuck( env, left, right );
        break;
    case ae_op_unchuck:
        return type_engine_check_op_unchuck( env, left, right );
        break;
    case ae_op_plus:
        if( ( (left->type == te_dur) && (right->type == te_time) ) ||
            ( (left->type == te_time) && (right->type == te_dur) ) )
            return &t_time;
    case ae_op_minus:
        if( (left->type == te_time) && (right->type == te_dur) )
            return &t_time;
        if( (left->type == te_dur) && (right->type == left->type) )
            return &t_dur;
        if( (left->type == te_time) && (right->type == left->type) ) // XXX time - time = dur
            return &t_dur;
    case ae_op_times:
    case ae_op_divide:
        if( (left->type == te_dur) && (right->type == left->type) )
            return &t_float;
    case ae_op_plus_chuck:
    case ae_op_minus_chuck:
    case ae_op_times_chuck:
    case ae_op_divide_chuck:
        if( (left->type == te_float) && (left->type == right->type) )
            return &t_float;
        if( (left->type == te_double) && (left->type == right->type) )
            return &t_double;
        if( (left->type == te_dur) && (right->type == te_float) )
            return &t_dur;
    
    case ae_op_lt:
    case ae_op_gt:
    case ae_op_le:
    case ae_op_ge:
    case ae_op_eq:
    case ae_op_neq:
        if( (left->type == te_int) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_uint) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_float) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_double) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_dur) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_time) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_int) && (right->type == te_uint) )
            return &t_int;
        if( (left->type == te_uint) && (right->type == te_int) )
            return &t_int;
    
    case ae_op_and:
    case ae_op_or:
    case ae_op_s_and:
    case ae_op_s_or:
    case ae_op_s_xor:
        if( op == ae_op_s_xor && ( (left->type == te_midiin) || (left->type == te_midiout) ) &&
            ( right->type == te_int ) )
            return left;
    case ae_op_shift_left:
    case ae_op_shift_right:
    case ae_op_percent:
    case ae_op_s_and_chuck:
    case ae_op_s_or_chuck:
    case ae_op_s_xor_chuck:
    case ae_op_shift_right_chuck:
    case ae_op_shift_left_chuck:
    case ae_op_percent_chuck:

    case ae_op_s_chuck:
    case ae_op_at_chuck:
        if( (left->type == te_int) && (left->type == right->type) )
            return &t_int;
        if( (left->type == te_uint) && (left->type == right->type) )
            return &t_uint;
        if( (left->type == te_time) && (right->type == te_dur) )
            return &t_dur;
        if( (left->type == te_dur) && (right->type == te_dur) )
            return &t_dur;
        break;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name:
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_binary( t_Env env, a_Exp_Binary binary )
{
    a_Exp cl = binary->lhs, cr = binary->rhs;
    t_Type ret = NULL;

    // hack
    if( binary->op == ae_op_chuck && cr->s_type == ae_exp_dot_member )
        cr->dot_member.flag = 1;
    
    t_Type left = type_engine_check_exp( env, cl );
    t_Type right = type_engine_check_exp( env, cr);
    
    if( !left || !right )
        return NULL;

    while( cr )
    {
        cl = binary->lhs;
        
        while( cl )
        {
            ret = type_engine_check_op( env, binary->op, cl->type, cr->type );
            if( !ret )
            {
                EM_error2( binary->linepos, 
                    "no suitable resolution for binary operator '%s' on types '%s' and '%s'",
                    op2str(binary->op), cl->type->name, cr->type->name );
                
                return NULL;
            }
            
            // special case
            if( binary->op == ae_op_chuck && cl->type->type == te_string && 
                cr->type->type == __te_system_namespace__ && cl->s_type == ae_exp_primary &&
                cl->primary.s_type == ae_primary_str )
            {
                t_Env e = env;
                while( e->parent ) e = e->parent;
                Chuck_DLL * dll = e->vm->dll_load( cl->primary.str, cl->primary.str );
                if( !dll && cl->primary.str[0] != '.' && cl->primary.str[0] != '/' )
                    dll = e->vm->dll_load( (string("./") + cl->primary.str).c_str(), 
                                           cl->primary.str );
                if( !dll )
                {
                    EM_error2( binary->linepos,
                        "ChucK DLL load failed...\n  [reason]: %s", e->vm->last_error() );
                    return NULL;
                }

                ((Chuck_DL_Query *)dll->query())->linepos = binary->linepos;
                type_engine_add_dll( e, dll, S_name(cr->name_space.name) );
            }
            
            cl = cl->next;
        }
        
        cr = cr->next;
    }
        
    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_unary
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_unary( t_Env env, a_Exp_Unary unary )
{
    t_Type t = type_engine_check_exp( env, unary->exp );
    
    switch( unary->op )
    {
    case ae_op_plusplus:
    case ae_op_minusminus:
        if( !type_engine_check_exp_mem( env, unary->exp ) )
        {
            EM_error2( unary->linepos, "non-lvalue for (prefix) operator ++ or --" );
            break;
        }
        
        if( t->type == te_int || t->type == te_uint || t->type == te_float )
            return t;
        else
        {
            EM_error2( unary->linepos, "bad type '%s' for unary operator '%s'",
                t->name, op2str(unary->op) );
        }
        break;
    case ae_op_tilda:
    case ae_op_exclamation:
    case ae_op_minus:
        if( t->type == te_int || t->type == te_uint )
            return t;
        if( unary->op == ae_op_minus && t->type == te_float )
            return t;
        else
        {
            EM_error2( unary->linepos, "bad type '%s' for unary operator '%s'",
                t->name, op2str(unary->op) );
        }
        break;
    case ae_op_spork:
        if( unary->exp->s_type == ae_exp_func_call )
            return &t_int;
        else
        {
            EM_error2( unary->linepos, "only function calls can be sporked" );            
        }
        break;
    default:
        EM_error2( unary->linepos, "unrecognized unary operator '%s'",
            op2str(unary->op) );
        break;
    }
        
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_cast_valid()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_cast_valid( t_Env env, t_Type to, t_Type from )
{
    if( to->type == from->type ) return TRUE;
    if( to->type == te_int && from->type == te_float ) return TRUE;
    if( to->type == te_float && from->type == te_int ) return TRUE;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_cast( t_Env env, a_Exp_Cast cast )
{
    t_Type t = type_engine_check_exp( env, cast->exp );
    if( !t ) return NULL;
    
    t_Type t2 = lookup_type( env, cast->type, TRUE );
    if( !t2 )
    {
        EM_error2( cast->linepos,
                   "casting to unknown type '%s'...", S_name(cast->type) );
        return NULL;
    }

    if( !type_engine_check_cast_valid( env, t2, t ) )
    {
        EM_error2( cast->linepos,
                   "cannot cast to type '%s' from '%s'...",
                   t2->name, t->name );
        return NULL;
    }
        
    return t2;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_postfix
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_postfix( t_Env env, a_Exp_Postfix postfix )
{
    t_Type t = type_engine_check_exp( env, postfix->exp );

    if( !t )
        return NULL;

    switch( postfix->op )
    {        
    case ae_op_plusplus:
    case ae_op_minusminus:
        if( !type_engine_check_exp_mem( env, postfix->exp ) )
        {
            EM_error2( postfix->linepos, "++/-- used on non-assignable value" );
            break;
        }
        
        if( t->type == te_int || t->type == te_uint || t->type == te_float )
            return t;
        else
        {
            EM_error2( postfix->linepos, "bad type '%s' for unary operator '%s'",
                t->name, op2str(postfix->op) );
        }
        break;
    default:
        EM_error2( postfix->linepos, 
            "internal compiler error - unrecognized postfix operator '%s'", op2str(postfix->op) );
        break;
    }
       
    // start here
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dur
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_dur( t_Env env, a_Exp_Dur dur )
{
    t_Type base = type_engine_check_exp( env, dur->base );
    t_Type unit = type_engine_check_exp( env, dur->unit );
    
    // check here
    if( !base || !unit )
        return NULL;
        
    // check
    if( base->type != te_float && base->type != te_int )
    {
        EM_error2( dur->base->linepos,
                   "base type '%s' illegal in dur expression",
                   base->name );
        return NULL;
    }
    
    // check
    if( unit->type != te_dur )
    {
        EM_error2( dur->unit->linepos,
                   "unit in dur expression has type '%s' - it must have type 'dur'",
                   unit->name );
        return NULL;
    }
    
    return &t_dur;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_array
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_array( t_Env env, a_Exp_Array array )
{
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_func_call( t_Env env, a_Exp_Func_Call func_call )
{
    a_Func_Def func = NULL;
    
    t_Type f = type_engine_check_exp( env, func_call->func );
    if( !f ) return NULL;
    t_Type a = &t_void;

    if( f->type != te_function )
    {
        EM_error2( func_call->linepos, "function call using a non-function value" );
        return NULL;
    }

    c_str name = "()";
    if( func_call->func->s_type == ae_exp_primary )
    {
        if( func_call->func->primary.s_type == ae_primary_var )
        {
            // find func
            func = lookup_func( env, func_call->func->primary.var );
            if( !func )
            {
                EM_error2( func_call->linepos,
                    "no function named '%s' defined", S_name(func_call->func->primary.var) );
                return NULL;
            }
        }
        else
        {
            EM_error2( func_call->linepos, "function call using illegal f-value" );
            return NULL;
        }

        name = S_name( func_call->func->primary.var );
    }
    else  // namespace or class
    {
        t_Env e = env->child;
        S_Symbol s = env->nspc_name;
        
        // make sure both there
        if( !e || !s )
        {
            EM_error2( func_call->linepos, "type checker: missing env/symbol in func call" );
            return NULL;
        }
        
        // find func
        func = lookup_func( e, s );
        if( !func )
        {
            EM_error2( func_call->linepos,
                "no function named '%s' defined in namespace/class '%s'",
                S_name(s), S_name(e->name) );
            return NULL;
        }

        name = S_name( func->name );
    }

    if( func_call->args )
    {
        a = type_engine_check_exp( env, func_call->args );
        if( !a ) return NULL;
    }

    a_Exp e = func_call->args;
    a_Arg_List e1 = func->arg_list;
    unsigned int count = 1;

    // check arguments
    while( e )
    {
        if( e1 == NULL )
        {
            EM_error2( func_call->linepos,
                "extra argument(s) in function call '%s' %i %s",
                name, e->s_type, e->type->name );
            return NULL;
        }

        if( e->type->type != e1->type->type )
        {
            EM_error2( func_call->linepos,
                "argument '%i' of function call '%s' has type '%s' -- expecting type '%s'",
                count, name, e->type->name, e1->type->name );
            return NULL;
        }

        e = e->next;
        e1 = e1->next;
        count++;
    }

    if( e1 != NULL )
    {
        EM_error2( func_call->linepos,
            "missing argument(s) in function call '%s', next arg: '%s %s'",
            S_name(func_call->func->primary.var), e1->type->name, S_name(e1->id) );
        return NULL;
    }

    return func->ret_type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dot_member
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_dot_member( t_Env env, a_Exp_Dot_Member member )
{
    t_Type t_base = type_engine_check_exp( env, member->base );

    if( !t_base )
        return NULL;
    
    if( t_base->type == __te_system_namespace__ )
    {
        // look for the namespace (name should be already resolved)
        t_Env info = lookup_namespace( env, env->nspc_name, FALSE );
        if( !info )
        {
            EM_error2( member->linepos,
                       "cannot find namespace '%s'",
                       S_name(env->nspc_name) );
            return NULL;
        }

        // find the member
        t_Type t_member = lookup_value( info, member->id, FALSE );
        if( !t_member )
        {
            EM_error2( member->linepos,
                       "cannot find member '%s' in namespace '%s'",
                       S_name(member->id), S_name(info->name) );
            return NULL;
        }
        
        // set the member
        env->child = info;
        env->nspc_name = member->id;

        return t_member;
    }
    else if( isa( t_base, &t_ugen ) )
    {
        // look for ugen
        Chuck_UGen_Info * info = lookup_ugen( env, insert_symbol(t_base->name), FALSE );
        if( !info )
        {
            EM_error2( member->linepos,
                       "cannot find unit generator '%s'",
                       t_base->name );
            return NULL;
        }
        
        // find member
        Chuck_Info_Param param = info->param_map[S_name(member->id)];
        if( param.type == "" )
        {
            EM_error2( member->linepos,
                       "cannot find control parameter '%s.%s'",
                       t_base->name, S_name(member->id) );
            return NULL;
        }
        
        env->nspc_name = NULL;

        // look for the type
        t_Type t_param = lookup_type( env, insert_symbol(param.type.c_str()) );
        if( !t_param )
        {
            EM_error2( member->linepos,
                       "type checker: internal error: cannot find param type '%s'",
                       param.type.c_str() );
            return NULL;
        }

        t_Type t = dup_type( t_param );
        // copy the function pointer
        member->data = (uint)param.ctrl_addr;
        member->data2 = (uint)param.cget_addr;
        if( !member->data && !member->data2 )
        {
            EM_error2( member->linepos,
                       "type checker: internal error: cannot find ugen ctrl or cget for '%s'",
                       param.name.c_str() );
            return NULL;
        }

        if( member->flag && !member->data )
        {
            EM_error2( member->linepos,
                       "type checker: cannot chuck values to '%s.%s' - it is read-only",
                       t_base->name, S_name(member->id) );
            return NULL;
        }
        else if( !member->flag && !member->data2 )
        {
            EM_error2( member->linepos,
                       "type checker: cannot use value from '%s.%s' - it is write-only",
                       t_base->name, S_name(member->id) );
            return NULL;
        }
        
        return t;
    }
    else
    {
        EM_error2( member->linepos,
            "internal error: class namespace not impl" );
        return NULL;
    }
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_if
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_if( t_Env env, a_Exp_If exp_if )
{
    t_Type cond = type_engine_check_exp( env, exp_if->cond );
    t_Type if_exp = type_engine_check_exp( env, exp_if->if_exp );
    t_Type else_exp = type_engine_check_exp( env, exp_if->else_exp );
    
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_decl()
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_decl( t_Env env, a_Exp_Decl decl )
{
    a_Var_Decl var_decl = decl->var_decl_list->var_decl;
    
    t_Type t = NULL, t2 = NULL;
    
    if( var_decl->isarray )
    {
        EM_error2( decl->linepos, "array not impl!" );
        t = &t_array;
        return NULL;
    }
    else
    {
        // look up the type in the type binding
        t = lookup_type( env, decl->type );

        // type there?
        if( t == NULL )
        {
            // error - no type
            EM_error2( decl->linepos, "undefined type '%s'",
                S_name(decl->type) );
            return NULL;
        }
        
        // check to see if value is there
        t2 = lookup_value( env, var_decl->id, FALSE );
        if( env->scope_lookup( var_decl->id ) )
        {
            // error - already defined in local scope
            EM_error2( decl->linepos, 
                "'%s' has already been defined in the same scope",
                S_name(var_decl->id) );
            return NULL;
        }

        // enter the type into the var->type value binding
        S_enter( env->value, var_decl->id, t );
        env->scope_add( var_decl->id );
    }

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_namespace( )
// desc: ...
//-----------------------------------------------------------------------------
t_Type type_engine_check_exp_namespace( t_Env env, a_Exp_Namespace name_space )
{
    return &t_system_namespace;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_if()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_if( t_Env env, a_Stmt_If stmt )
{
    // check the conditional
    if( !type_engine_check_exp( env, stmt->cond ) )
    {
        // env->out() << "type checker: bad type in if condition" << endl;
        return FALSE;
    }

    // check if then else, if there is one
    if( !type_engine_check_stmt( env, stmt->if_body ) )
    {
        // env->out() << "type checker: bad type in if statement" << endl;
        return FALSE;
    }
    
    if( stmt->else_body )
    {
        if( !type_engine_check_stmt( env, stmt->else_body ) )
        {
            // env->out() << "type checker: bad type in else statement" << endl;
            return FALSE;
        }
     }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_for()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_for( t_Env env, a_Stmt_For stmt )
{
    // check the conditional
    if( !type_engine_check_stmt( env, stmt->c1 ) )
    {
        // env->out() << "type checker: bad type in for exp 1" << endl;
        return FALSE;
    }

    // check the conditional
    if( !type_engine_check_stmt( env, stmt->c2 ) )
    {
        // env->out() << "type checker: bad type in for exp 2" << endl;
        return FALSE;
    }

    // check the conditional
    if( stmt->c3 && !type_engine_check_exp( env, stmt->c3 ) )
    {
        // env->out() << "type checker: bad type in for exp 3" << endl;
        return FALSE;
    }
    // check body
    if( !type_engine_check_stmt( env, stmt->body ) )
    {
        // env->out() << "type checker: bad type in for body" << endl;
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_code_segment()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_code_segment( t_Env env, a_Stmt_Code stmt )
{
    return type_engine_check_stmt_list( env, stmt->stmt_list );
}




//-----------------------------------------------------------------------------
// name: type_engine_check_while()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_while( t_Env env, a_Stmt_While stmt )
{
    if( !type_engine_check_exp( env, stmt->cond ) )
    {
        // env->out() << "type checker: while condition does not type check" << endl;
        return FALSE;
    }

    if( !type_engine_check_stmt( env, stmt->body ) )
    {
        // env->out() << "type checker: while statement does not type check" << endl;
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_until()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_until( t_Env env, a_Stmt_Until stmt )
{
    if( !type_engine_check_exp( env, stmt->cond ) )
    {
        // env->out() << "type checker: while condition does not type check" << endl;
        return FALSE;
    }

    if( !type_engine_check_stmt( env, stmt->body ) )
    {
        // env->out() << "type checker: while statement does not type check" << endl;
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_return()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_return( t_Env env, a_Stmt_Return stmt )
{
    t_Type ret_type = NULL;
    
    if( !env->func_def )
    {
        EM_error2( stmt->linepos, "'return' statement found outside function definition" );
        return FALSE;
    }
    
    if( stmt->val )
        ret_type = type_engine_check_exp( env, stmt->val );
    else
        ret_type = &t_void;

    if( ret_type && ret_type != env->func_def->ret_type )
    {
        EM_error2( stmt->linepos,
            "function '%s' was defined with return type '%s' -- but returning type '%s'",
            S_name(env->func_def->name), env->func_def->ret_type->name, ret_type->name );
        return FALSE;
    }

    return ret_type != NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_switch()
// desc: type check a stmt
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_switch( t_Env env, a_Stmt_Switch stmt )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: lookup_type()
// desc: ...
//-----------------------------------------------------------------------------
t_Type lookup_type( t_Env env, S_Symbol type_name, t_CKBOOL climb )
{
    t_Type t = (t_Type)S_look( env->type, type_name );
    if( climb && !t && env->parent )
        return lookup_type( env->parent, type_name, climb );
    return t;
}




//-----------------------------------------------------------------------------
// name: lookup_value()
// desc: ...
//-----------------------------------------------------------------------------
t_Type lookup_value( t_Env env, S_Symbol value_name, t_CKBOOL climb )
{
    t_Type t = (t_Type)S_look( env->value, value_name );
    if( climb && !t && env->parent )
        return lookup_value( env->parent, value_name, climb );
    return t;
}




//-----------------------------------------------------------------------------
// name: lookup_func()
// desc: ...
//-----------------------------------------------------------------------------
a_Func_Def lookup_func( t_Env env, S_Symbol func_name, t_CKBOOL climb )
{
    a_Func_Def f = (a_Func_Def)S_look( env->function, func_name );
    if( climb && !f && env->parent )
        return lookup_func( env->parent, func_name, climb );
    return f;
}




//-----------------------------------------------------------------------------
// name: lookup_ugen()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_UGen_Info * lookup_ugen( t_Env env, S_Symbol ugen_name, t_CKBOOL climb )
{
    Chuck_UGen_Info * ugen = (Chuck_UGen_Info *)S_look( env->ugen, ugen_name );
    if( climb && !ugen && env->parent )
        return lookup_ugen( env->parent, ugen_name, climb );
    return ugen;
}




//-----------------------------------------------------------------------------
// name: lookup_namespace()
// desc: ...
//-----------------------------------------------------------------------------
t_Env lookup_namespace( t_Env env, S_Symbol nspc, t_CKBOOL climb )
{
    t_Env e = (t_Env)S_look( env->name_space, nspc );
    if( climb && !e && env->parent )
        return lookup_namespace( env->parent, nspc, climb );
    return e;
}




//-----------------------------------------------------------------------------
// name: lookup_addr()
// desc: ...
//-----------------------------------------------------------------------------
void * lookup_addr( t_Env env, S_Symbol value, t_CKBOOL climb )
{
    void * v = S_look( env->addr, value );
    if( climb && !v && env->parent )
        return lookup_addr( env->parent, value, climb );
    return v;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_mem()
// desc: type check an exp memory
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_exp_mem( t_Env env, a_Exp exp )
{
    a_Exp curr = exp;
    t_CKBOOL val = TRUE;

    while( curr && val )
    {
        val = FALSE;
        switch( curr->s_type )
        {
        case ae_exp_primary:
            val = type_engine_check_primary_mem( env, &curr->primary );
        break;
    
        case ae_exp_array:
            val = type_engine_check_exp_array_mem( env, &curr->array );
        break;
    
        case ae_exp_func_call:
            val = type_engine_check_exp_func_call_mem( env, &curr->func_call );
        break;
    
        case ae_exp_dot_member:
            val = type_engine_check_exp_dot_member_mem( env, &curr->dot_member );
        break;
        }

        curr = curr->next;
    }

    // return type
    return val;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_mem()
// desc: type check an primary exp memory
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_primary_mem( t_Env env, a_Exp_Primary exp )
{
    t_CKBOOL val = FALSE;

    switch( exp->s_type )
    {
    case ae_primary_var:
        val = ( lookup_value( env, exp->var ) != NULL );
        break;
    
    case ae_primary_exp:
        val = type_engine_check_exp_mem( env, exp->exp );
        break;

    default:
        // env->out() << "type checker: bad primary [mem] values..." << endl;
        break;
    }

    return val;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dot_member_mem()
// desc: type check an dot exp memory
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_exp_dot_member_mem( t_Env env, a_Exp_Dot_Member member )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_array_mem()
// desc: type check an array exp memory
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_exp_array_mem( t_Env env, a_Exp_Array array )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call_mem()
// desc: type check an func call exp memory
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_exp_func_call_mem( t_Env env, a_Exp_Func_Call func_call )
{
    return FALSE;
}
