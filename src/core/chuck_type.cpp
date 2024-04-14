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
// file: chuck_type.cpp
// desc: chuck type-system / type-checker
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original
//       Autumn 2005 - rewrite
//-----------------------------------------------------------------------------
#include "chuck_type.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck_io.h"
#include "chuck_lang.h"
#include "chuck_parse.h"
#include "chuck_scan.h"
#include "chuck_symbol.h"
#include "chuck_vm.h"
#include "ugen_xxx.h"
#include "util_string.h"

#include <limits.h>

#include <sstream>
#include <algorithm>
using namespace std;




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt_list( Chuck_Env * env, a_Stmt_List list );
t_CKBOOL type_engine_check_stmt( Chuck_Env * env, a_Stmt stmt );
t_CKBOOL type_engine_check_if( Chuck_Env * env, a_Stmt_If stmt );
t_CKBOOL type_engine_check_for( Chuck_Env * env, a_Stmt_For stmt );
t_CKBOOL type_engine_check_foreach( Chuck_Env * env, a_Stmt_ForEach stmt );
t_CKBOOL type_engine_check_while( Chuck_Env * env, a_Stmt_While stmt );
t_CKBOOL type_engine_check_until( Chuck_Env * env, a_Stmt_Until stmt );
t_CKBOOL type_engine_check_loop( Chuck_Env * env, a_Stmt_Loop stmt );
t_CKBOOL type_engine_check_break( Chuck_Env * env, a_Stmt_Break br );
t_CKBOOL type_engine_check_continue( Chuck_Env * env, a_Stmt_Continue cont );
t_CKBOOL type_engine_check_return( Chuck_Env * env, a_Stmt_Return stmt );
t_CKBOOL type_engine_check_switch( Chuck_Env * env, a_Stmt_Switch stmt );
t_CKTYPE type_engine_check_exp( Chuck_Env * env, a_Exp exp );
t_CKTYPE type_engine_check_exp_binary( Chuck_Env * env, a_Exp_Binary binary );
t_CKTYPE type_engine_check_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKTYPE type_engine_check_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKTYPE type_engine_check_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKTYPE type_engine_check_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKTYPE type_engine_check_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKTYPE type_engine_check_exp_unary( Chuck_Env * env, a_Exp_Unary unary );
t_CKTYPE type_engine_check_op_overload_binary( Chuck_Env * env, ae_Operator op, Chuck_Type * lhs, Chuck_Type * rhs, a_Exp_Binary binary ); // 1.5.1.5
t_CKTYPE type_engine_check_op_overload_unary( Chuck_Env * env, ae_Operator op, Chuck_Type * rhs, a_Exp_Unary unary ); // 1.5.1.5
t_CKTYPE type_engine_check_op_overload_postfix( Chuck_Env * env, Chuck_Type * lhs, ae_Operator op, a_Exp_Postfix post ); // 1.5.1.5
t_CKTYPE type_engine_check_exp_primary( Chuck_Env * env, a_Exp_Primary exp );
t_CKTYPE type_engine_check_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKTYPE type_engine_check_exp_complex_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKTYPE type_engine_check_exp_polar_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKTYPE type_engine_check_exp_vec_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKTYPE type_engine_check_exp_cast( Chuck_Env * env, a_Exp_Cast cast );
t_CKTYPE type_engine_check_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix );
t_CKTYPE type_engine_check_exp_dur( Chuck_Env * env, a_Exp_Dur dur );
t_CKTYPE type_engine_check_exp_array( Chuck_Env * env, a_Exp_Array array );
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call );
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                          t_CKFUNC & ck_func, int linepos );
t_CKTYPE type_engine_check_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member );
t_CKTYPE type_engine_check_exp_if( Chuck_Env * env, a_Exp_If exp_if );
t_CKTYPE type_engine_check_exp_decl( Chuck_Env * env, a_Exp_Decl decl );
t_CKBOOL type_engine_check_array_subscripts( Chuck_Env * env, a_Exp exp_list );
t_CKBOOL type_engine_check_cast_valid( Chuck_Env * env, t_CKTYPE to, t_CKTYPE from );
t_CKBOOL type_engine_check_code_segment( Chuck_Env * env, a_Stmt_Code stmt, t_CKBOOL push = TRUE );
t_CKBOOL type_engine_check_func_def( Chuck_Env * env, a_Func_Def func_def );
t_CKBOOL type_engine_check_class_def( Chuck_Env * env, a_Class_Def class_def );

// helpers
Chuck_Value * type_engine_check_const( Chuck_Env * env, a_Exp exp );
// convert dot member expression to string for printing
string type_engine_print_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member );
// type check constructor invocation | 1.5.2.0 (ge) added
Chuck_Func * type_engine_check_ctor_call( Chuck_Env * env, Chuck_Type * type, a_Ctor_Call ctor, a_Array_Sub array, uint32_t where );
// make an chuck dll function into absyn function
a_Func_Def make_dll_as_fun( Chuck_DL_Func * dl_fun, t_CKBOOL is_static, t_CKBOOL is_base_primtive );
// make a partial deep copy, to separate type systems from AST
a_Func_Def partial_deep_copy_fn( a_Func_Def f );
a_Arg_List partial_deep_copy_args( a_Arg_List args );

// helper macros
#define CK_LR( L, R )      if( (left->xid == L) && (right->xid == R) )
#define CK_COMMUTE( L, R ) if( ( (left->xid == L) && (right->xid == R) ) || \
                            ( (left->xid == R) && (right->xid == L) ) )




//-----------------------------------------------------------------------------
// name: Chuck_Env()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Env::Chuck_Env()
{
    // lock from being deleted
    global_context.lock();
    // make reference
    context = &global_context; CK_SAFE_ADD_REF(context);
    // make name
    context->filename = "@[global]";
    // remember
    global_nspc = global_context.nspc; CK_SAFE_ADD_REF(global_nspc);
    // deprecated stuff
    deprecated.clear(); deprecate_level = 1;
    // zero out
    user_nspc = NULL;
    class_def = NULL;
    func = NULL;
    curr = NULL;
    sporking = FALSE;

    // clear
    this->reset();

    // REFACTOR-2017: carrier
    m_carrier = NULL;

    // zero out | 1.5.0.0 (ge) moved Chuck_Type instantiation to after carrier is set,
    // in order to access compiler (e.g., for originHint)
    ckt_void = NULL;
    ckt_auto = NULL;
    ckt_int = NULL;
    ckt_float = NULL;
    ckt_time = NULL;
    ckt_dur = NULL;
    ckt_complex = NULL;
    ckt_polar = NULL;
    ckt_vec2 = NULL;
    ckt_vec3 = NULL;
    ckt_vec4 = NULL;
    ckt_null = NULL;
    ckt_object = NULL;
    ckt_function = NULL;
    ckt_array = NULL;
    ckt_string = NULL;
    ckt_event = NULL;
    ckt_ugen = NULL;
    ckt_uana = NULL;
    ckt_uanablob = NULL;
    ckt_shred = NULL;
    ckt_io = NULL;
    ckt_fileio = NULL;
    ckt_chout = NULL;
    ckt_cherr = NULL;
    ckt_class = NULL;
    // ckt_thread = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Env()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Env::~Chuck_Env()
{
    // lock from being deleted
    global_context.unlock();

    // cleanup
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: init()
// desc: initialize | 1.5.0.0 (ge) added to defer Chuck_Type creations
//       for base types in order to access more information, e.g., the compiler
//       within the Chuck_Type constructor
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Env::init()
{
    // initialize base types
    ckt_void = new Chuck_Type( this, te_void, "void", NULL, 0 );
    ckt_auto = new Chuck_Type( this, te_auto, "auto", NULL, 0 );
    ckt_int = new Chuck_Type( this, te_int, "int", NULL, sizeof(t_CKINT) );
    ckt_float = new Chuck_Type( this, te_float, "float", NULL, sizeof(t_CKFLOAT) );
    ckt_time = new Chuck_Type( this, te_time, "time", NULL, sizeof(t_CKTIME) );
    ckt_dur = new Chuck_Type( this, te_dur, "dur", NULL, sizeof(t_CKTIME) );
    ckt_complex = new Chuck_Type( this, te_complex, "complex", NULL, sizeof(t_CKCOMPLEX) );
    ckt_polar = new Chuck_Type( this, te_polar, "polar", NULL, sizeof(t_CKPOLAR) );
    ckt_vec2 = new Chuck_Type( this, te_vec2, "vec2", NULL, sizeof(t_CKVEC2) ); // 1.5.1.7
    ckt_vec3 = new Chuck_Type( this, te_vec3, "vec3", NULL, sizeof(t_CKVEC3) ); // 1.3.5.3
    ckt_vec4 = new Chuck_Type( this, te_vec4, "vec4", NULL, sizeof(t_CKVEC4) ); // 1.3.5.3
    ckt_null = new Chuck_Type( this, te_null, "@null", NULL, sizeof(void *) );
    ckt_object = new Chuck_Type( this, te_object, "Object", NULL, sizeof(void *) );
    ckt_function = new Chuck_Type( this, te_function, "@function", ckt_object, sizeof(void *) );
    ckt_array = new Chuck_Type ( this, te_array, "@array", ckt_object, sizeof(void *) );
    ckt_string = new Chuck_Type( this, te_string, "string", ckt_object, sizeof(void *) );
    ckt_event = new Chuck_Type( this, te_event, "Event", ckt_object, sizeof(void *) );
    ckt_ugen = new Chuck_Type( this, te_ugen, "UGen", ckt_object, sizeof(void *) );
    ckt_uana = new Chuck_Type( this, te_uana, "UAna", ckt_ugen, sizeof(void *) );
    ckt_uanablob = new Chuck_Type( this, te_uanablob, "UAnaBlob", ckt_object, sizeof(void *) );
    ckt_shred = new Chuck_Type( this, te_shred, "Shred", ckt_object, sizeof(void *) );
    ckt_io = new Chuck_Type( this, te_io, "IO", ckt_event, sizeof(void *) );
    ckt_fileio = new Chuck_Type( this, te_fileio, "FileIO", ckt_io, sizeof(void *) );
    ckt_chout = new Chuck_Type( this, te_chout, "StdOut", ckt_io, sizeof(void *) );
    ckt_cherr = new Chuck_Type( this, te_cherr, "StdErr", ckt_io, sizeof(void *) );
    // ckt_thread = new Chuck_Type( this, te_thread, "Thread", ckt_object, sizeof(void *) );
    ckt_class = new Chuck_Type( this, te_class, "Type", ckt_object, sizeof(void *) );

    return true;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: clean up the Env; the opposite of init() | 1.5.0.1 (ge) added
//-----------------------------------------------------------------------------
void Chuck_Env::cleanup()
{
    // set static flag to unlock all types
    ckt_object->unlock_all();

    // TODO: free all types in the Env; taking into account inheritance
    // dependencies; i.e., parent types should be freed after inherited
    // types, as the latter has reference to the former

    // unlock each internal object type | 1.5.0.0 (ge) added
    // 1.5.0.1 (ge) re-ordered: parent dependencies are cleaned up later
    CK_SAFE_UNLOCK_DELETE(ckt_void);
    CK_SAFE_UNLOCK_DELETE(ckt_auto);
    CK_SAFE_UNLOCK_DELETE(ckt_int);
    CK_SAFE_UNLOCK_DELETE(ckt_float);
    CK_SAFE_UNLOCK_DELETE(ckt_time);
    CK_SAFE_UNLOCK_DELETE(ckt_dur);
    CK_SAFE_UNLOCK_DELETE(ckt_complex);
    CK_SAFE_UNLOCK_DELETE(ckt_polar);
    CK_SAFE_UNLOCK_DELETE(ckt_vec2);
    CK_SAFE_UNLOCK_DELETE(ckt_vec3);
    CK_SAFE_UNLOCK_DELETE(ckt_vec4);
    CK_SAFE_UNLOCK_DELETE(ckt_fileio);
    CK_SAFE_UNLOCK_DELETE(ckt_chout);
    CK_SAFE_UNLOCK_DELETE(ckt_cherr);
    CK_SAFE_UNLOCK_DELETE(ckt_io);
    CK_SAFE_UNLOCK_DELETE(ckt_dac);
    CK_SAFE_UNLOCK_DELETE(ckt_adc);
    CK_SAFE_UNLOCK_DELETE(ckt_ugen);
    CK_SAFE_UNLOCK_DELETE(ckt_uanablob);
    CK_SAFE_UNLOCK_DELETE(ckt_uana);
    CK_SAFE_UNLOCK_DELETE(ckt_function);
    CK_SAFE_UNLOCK_DELETE(ckt_string);
    CK_SAFE_UNLOCK_DELETE(ckt_shred);
    CK_SAFE_UNLOCK_DELETE(ckt_event);
    CK_SAFE_UNLOCK_DELETE(ckt_array);

    // Type and Object types go last, as every other ChucK_Type are also these
    // ckt_class and ckt_object have mutual dependencies, e.g.,
    // ckt_class->parent is ckt_object, while ckt_object->type_ref is ckt_class

    // break the dependency manually | 1.5.0.1 (ge) added
    // part 1: save the parent reference
    Chuck_Type * skip = ckt_object->type_ref != NULL ? ckt_object->type_ref->parent : NULL;
    // free the Type type
    CK_SAFE_UNLOCK_DELETE(ckt_class);

    // part 2: break the dependency manually | 1.5.0.1 (ge) added
    ckt_object->type_ref = NULL; // was: skip -- but ckt_object is a Chuck_Object...
    // and will try to to use type_ref->obj_mvars_offsets to cleanup, but aspects
    // of chuck_type could be already be deleted before ~Chuck_Object() is invoked,
    // including the obj_mvars_offsets vector; NOTE: this means that the final object
    // does not delete its string mvar (by default it is NULL, so works out)

    // finally, free the Object type
    CK_SAFE_UNLOCK_DELETE(ckt_object);
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: reset the type environment
//-----------------------------------------------------------------------------
void Chuck_Env::reset()
{
    // TODO: release stack items?
    nspc_stack.clear();
    // push global namespace
    nspc_stack.push_back( this->global() );
    // push user namespace
    if( user_nspc != NULL ) nspc_stack.push_back( this->user_nspc );
    // TODO: release stack items?
    class_stack.clear(); class_stack.push_back( NULL );
    // should be at top level
    assert( context == &global_context );

    // release curr? class_def? func?
    // 1.5.0.1 (ge) don't think these need ref counts; they are used as temporary variables

    // assign
    curr = (user_nspc != NULL) ? this->user() : this->global();
    // clear
    class_def = NULL; func = NULL;

    // make sure this is 0
    class_scope = 0;

    // reset operator registry; remove all op overloads that is local
    op_registry.reset2local();
}




//-----------------------------------------------------------------------------
// name: load_user_namespace()
// desc: load the user namespace
//-----------------------------------------------------------------------------
void Chuck_Env::load_user_namespace()
{
    // user namespace
    user_nspc = new Chuck_Namespace;
    user_nspc->name = "[user]";
    user_nspc->parent = global_nspc;
    CK_SAFE_ADD_REF(global_nspc);
    CK_SAFE_ADD_REF(user_nspc);
}




//-----------------------------------------------------------------------------
// name: clear_user_namespace()
// desc: clear the contents of the user namespace
//-----------------------------------------------------------------------------
void Chuck_Env::clear_user_namespace()
{
    // clean up user parent namespace
    if( user_nspc ) CK_SAFE_RELEASE(user_nspc->parent);
    // clean up user namesapce
    CK_SAFE_RELEASE( user_nspc );
    // load new user namespace
    load_user_namespace();
    // reset it
    this->reset();
    // reset operator overloads, including public (env->reset() only resets local)
    op_registry.reset2public();
}




//-----------------------------------------------------------------------------
// name: nspc_top()
// desc: get the namespace at the top of the stack
//-----------------------------------------------------------------------------
Chuck_Namespace * Chuck_Env::nspc_top()
{
    assert(nspc_stack.size() > 0); return nspc_stack.back();
}




//-----------------------------------------------------------------------------
// name: nspc_top()
// desc: get the class at the top of the stack
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Env::class_top()
{
    assert( class_stack.size() > 0 ); return class_stack.back();
}




//-----------------------------------------------------------------------------
// name: global()
// desc: get global namespace
//-----------------------------------------------------------------------------
Chuck_Namespace * Chuck_Env::global() { return global_nspc; }




//-----------------------------------------------------------------------------
// name: user()
// desc: get user namespace, if there is one (if not, return global)
//-----------------------------------------------------------------------------
Chuck_Namespace * Chuck_Env::user()
{
    // check if we have a user namespace here
    return user_nspc != NULL ? user_nspc : global();
}




//-----------------------------------------------------------------------------
// name: is_global()
// desc: check whether the context is the global context
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Env::is_global()
{
    return class_def == NULL && func == NULL && class_scope == 0;
}




//-----------------------------------------------------------------------------
// name: type_engine_init_object_special() | 1.5.0.0 (ge) added
// desc: special initialization of base types that have mutual dependencies
//       (e.g., with the Type base type)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_init_special( Chuck_Env * env, Chuck_Type * objT )
{
    // call initialize_object() to initialize objT itself as an instance of Object
    initialize_object( objT, env->ckt_class, NULL, env->vm() );

    // ensure namespace allocation
    if( objT->info == NULL )
    {
        EM_error3( "[chuck]: internal error initializing base class '%s'", objT->base_name.c_str() );
        return FALSE;
    }

    // vector of func
    vector<Chuck_Func *> funcs;
    // get funcs in Object (FALSE means don't include mangled duplicates)
    objT->info->get_funcs( funcs, FALSE );
    // iterate through
    for( t_CKINT i = 0; i < funcs.size(); i++ )
    {
        // get func
        Chuck_Func * f = funcs[i];
        // loop over (for overloaded functions)
        while( f )
        {
            // initialize each function type as an object instance
            // (special cases: these should not be initialized yet)
            initialize_object( f->value_ref->type, env->ckt_class, NULL, env->vm() );
            // next f
            f = f->next;
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_init()
// desc: initialize a type engine
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_init( Chuck_Carrier * carrier )
{
    // log
    EM_log( CK_LOG_HERALD, "initializing type checker..." );
    // push indent level
    EM_pushlog();

    // allocate a new env
    Chuck_Env * env = new Chuck_Env();
    // set the name of global namespace
    env->global()->name = "global";
    // set the current namespace to global
    env->curr = env->global();

    // REFACTOR-2017: store env in carrier
    carrier->env = env;
    // add reference
    CK_SAFE_ADD_REF( carrier->env );
    // and store carrier in env
    env->set_carrier( carrier );

    // initialize types | 1.5.0.0 (ge) moved here to after carrier set
    env->init();

    // enter the default global type mapping : lock VM objects to catch deletion
    env->global()->type.add( env->ckt_void->base_name, env->ckt_void );          env->ckt_void->lock();
    env->global()->type.add( env->ckt_auto->base_name, env->ckt_auto );          env->ckt_auto->lock();
    env->global()->type.add( env->ckt_int->base_name, env->ckt_int );            env->ckt_int->lock();
    env->global()->type.add( env->ckt_float->base_name, env->ckt_float );        env->ckt_float->lock();
    env->global()->type.add( env->ckt_time->base_name, env->ckt_time );          env->ckt_time->lock();
    env->global()->type.add( env->ckt_dur->base_name, env->ckt_dur );            env->ckt_dur->lock();
    env->global()->type.add( env->ckt_complex->base_name, env->ckt_complex );    env->ckt_complex->lock();
    env->global()->type.add( env->ckt_polar->base_name, env->ckt_polar );        env->ckt_polar->lock();
    env->global()->type.add( env->ckt_vec2->base_name, env->ckt_vec2 );          env->ckt_vec2->lock();
    env->global()->type.add( env->ckt_vec3->base_name, env->ckt_vec3 );          env->ckt_vec3->lock();
    env->global()->type.add( env->ckt_vec4->base_name, env->ckt_vec4 );          env->ckt_vec4->lock();
    env->global()->type.add( env->ckt_object->base_name, env->ckt_object );      env->ckt_object->lock();
    env->global()->type.add( env->ckt_string->base_name, env->ckt_string );      env->ckt_string->lock();
    env->global()->type.add( env->ckt_ugen->base_name, env->ckt_ugen );          env->ckt_ugen->lock();
    env->global()->type.add( env->ckt_uana->base_name, env->ckt_uana );          env->ckt_uana->lock();
    env->global()->type.add( env->ckt_uanablob->base_name, env->ckt_uanablob );  env->ckt_uanablob->lock();
    env->global()->type.add( env->ckt_shred->base_name, env->ckt_shred );        env->ckt_shred->lock();
    env->global()->type.add( env->ckt_function->base_name, env->ckt_function );  env->ckt_function->lock();
    env->global()->type.add( env->ckt_class->base_name, env->ckt_class );        env->ckt_class->lock();
    env->global()->type.add( env->ckt_array->base_name, env->ckt_array );        env->ckt_array->lock();
    env->global()->type.add( env->ckt_event->base_name, env->ckt_event );        env->ckt_event->lock();
    env->global()->type.add( env->ckt_io->base_name, env->ckt_io );              env->ckt_io->lock();
    env->global()->type.add( env->ckt_fileio->base_name, env->ckt_fileio );      env->ckt_fileio->lock();
    env->global()->type.add( env->ckt_chout->base_name, env->ckt_chout );        env->ckt_chout->lock();
    env->global()->type.add( env->ckt_cherr->base_name, env->ckt_cherr );        env->ckt_cherr->lock();
    // env->global()->type.add( env->ckt_thread->base_name, env->ckt_thread );   env->ckt_thread->lock();

    // dur value
    t_CKDUR samp = 1.0;
    // TODO:
    t_CKDUR second = carrier->vm->srate() * samp;
    // t_CKDUR second = 44100 * samp;
    t_CKDUR ms = second / 1000.0;
    t_CKDUR minute = second * 60.0;
    t_CKDUR hour = minute * 60.0;
    t_CKDUR day = hour * 24.0;
    t_CKDUR week = day * 7.0;
    // one billion years, a very long time
    // length of a sidereal year; https://en.wikipedia.org/wiki/Year
    t_CKDUR eon = day * 365.256363004 * 1000000000.0;

    // add internal classes
    EM_log( CK_LOG_HERALD, "adding base classes..." );
    EM_pushlog();

    // special: Object and Type, whose initializations mutually depend
    init_class_object( env, env->ckt_object );
    // special: @array and Type, whose initializations mutually depend
    init_class_array( env, env->ckt_array );
    // initialize the Type type
    init_class_type( env, env->ckt_class ); // 1.5.0.0
    // initialize of Object's and @array's Type objects are deferred until after init_class_type()
    type_engine_init_special( env, env->ckt_object );
    type_engine_init_special( env, env->ckt_array );

    // initialize the remaining internal classes
    init_class_string( env, env->ckt_string );
    init_class_ugen( env, env->ckt_ugen );
    init_class_blob( env, env->ckt_uanablob );
    init_class_uana( env, env->ckt_uana );
    init_class_shred( env, env->ckt_shred );
    init_class_event( env, env->ckt_event );
    init_class_io( env, env->ckt_io );
    init_class_fileio( env, env->ckt_fileio );
    init_class_chout( env, env->ckt_chout ); // 1.3.0.0
    init_class_cherr( env, env->ckt_cherr ); // 1.3.0.0
    init_class_vec2( env, env->ckt_vec2 ); // 1.5.1.7
    init_class_vec3( env, env->ckt_vec3 ); // 1.3.5.3
    init_class_vec4( env, env->ckt_vec4 ); // 1.3.5.3
    init_class_function(env, env->ckt_function ); // 1.5.0.0

    // initialize primitive types
    init_primitive_types( env );

    // thread
    // env->ckt_thread->info = new Chuck_Namespace;
    // env->ckt_thread->info->add_ref();

    // pop indent
    EM_poplog();

    // default global values
    env->global()->value.add( "null", new Chuck_Value( env->ckt_null, "null", new void *(NULL), TRUE ) );
    env->global()->value.add( "NULL", new Chuck_Value( env->ckt_null, "NULL", new void *(NULL), TRUE ) );
    env->global()->value.add( "t_zero", new Chuck_Value( env->ckt_time, "time_zero", new t_CKDUR(0.0), TRUE ) );
    env->global()->value.add( "d_zero", new Chuck_Value( env->ckt_dur, "dur_zero", new t_CKDUR(0.0), TRUE ) );
    env->global()->value.add( "samp", new Chuck_Value( env->ckt_dur, "samp", new t_CKDUR(samp), TRUE ) );
    env->global()->value.add( "ms", new Chuck_Value( env->ckt_dur, "ms", new t_CKDUR(ms), TRUE ) );
    env->global()->value.add( "second", new Chuck_Value( env->ckt_dur, "second", new t_CKDUR(second), TRUE ) );
    env->global()->value.add( "minute", new Chuck_Value( env->ckt_dur, "minute", new t_CKDUR(minute), TRUE ) );
    env->global()->value.add( "hour", new Chuck_Value( env->ckt_dur, "hour", new t_CKDUR(hour), TRUE ) );
    env->global()->value.add( "day", new Chuck_Value( env->ckt_dur, "day", new t_CKDUR(day), TRUE ) );
    env->global()->value.add( "week", new Chuck_Value( env->ckt_dur, "week", new t_CKDUR(week), TRUE ) );
    env->global()->value.add( "eon", new Chuck_Value( env->ckt_dur, "eon", new t_CKDUR(eon), TRUE ) );
    env->global()->value.add( "true", new Chuck_Value( env->ckt_int, "true", new t_CKINT(1), TRUE ) );
    env->global()->value.add( "false", new Chuck_Value( env->ckt_int, "false", new t_CKINT(0), TRUE ) );
    env->global()->value.add( "maybe", new Chuck_Value( env->ckt_int, "maybe", new t_CKFLOAT(.5), FALSE ) );
    env->global()->value.add( "pi", new Chuck_Value( env->ckt_float, "pi", new t_CKFLOAT(CK_ONE_PI), TRUE ) );
    env->global()->value.add( "global", new Chuck_Value( env->ckt_class, "global", env->global(), TRUE ) );
    env->global()->value.add( "chout", new Chuck_Value( env->ckt_io, "chout", new Chuck_IO_Chout( carrier ), TRUE ) );
    env->global()->value.add( "cherr", new Chuck_Value( env->ckt_io, "cherr", new Chuck_IO_Cherr( carrier ), TRUE ) );

    // TODO: can't use the following now is local to shred
    // env->global()->value.add( "now", new Chuck_Value( env->ckt_time, "now", &(vm->shreduler()->now_system), TRUE ) );

    /*
    S_enter( e->value, insert_symbol( "machine" ), env->ckt_null );
    S_enter( e->value, insert_symbol( "language" ), env->ckt_null );
    S_enter( e->value, insert_symbol( "compiler" ), env->ckt_null );
    S_enter( e->value, insert_symbol( "chout" ), env->ckt_system_out );
    S_enter( e->value, insert_symbol( "cherr" ), env->ckt_system_err );
    S_enter( e->value, insert_symbol( "stdout" ), env->ckt_system_out );
    S_enter( e->value, insert_symbol( "stderr" ), env->ckt_system_err );
    S_enter( e->value, insert_symbol( "midiout" ), env->ckt_midiout );
    S_enter( e->value, insert_symbol( "midiin" ), env->ckt_midiin );
    S_enter( e->value, insert_symbol( "adc" ), env->ckt_adc );
    S_enter( e->value, insert_symbol( "dac" ), env->ckt_dac );
    S_enter( e->value, insert_symbol( "bunghole" ), env->ckt_bunghole );
    S_enter( e->value, insert_symbol( "blackhole" ), env->ckt_bunghole );
    S_enter( e->value, insert_symbol( "endl" ), env->ckt_string );
    */

    // add reserve words
    env->key_words["for"] = TRUE;
    env->key_words["while"] = TRUE;
    env->key_words["until"] = TRUE;
    env->key_words["if"] = TRUE;
    env->key_words["else"] = TRUE;
    env->key_words["spork"] = TRUE;
    env->key_words["return"] = TRUE;
    env->key_words["class"] = TRUE;
    env->key_words["extends"] = TRUE;
    env->key_words["implements"] = TRUE;
    env->key_words["function"] = TRUE;
    env->key_words["fun"] = TRUE;
    env->key_words["public"] = TRUE;
    env->key_words["protected"] = TRUE;
    env->key_words["private"] = TRUE;
    env->key_words["static"] = TRUE;
    env->key_words["pure"] = TRUE;
    env->key_words["function"] = TRUE;
    env->key_words["fun"] = TRUE;
    env->key_words["break"] = TRUE;
    env->key_words["continue"] = TRUE;

    env->key_values["now"] = TRUE;
    env->key_values["true"] = TRUE;
    env->key_values["false"] = TRUE;
    env->key_values["maybe"] = TRUE;
    env->key_values["pi"] = TRUE;
    env->key_values["this"] = TRUE;
    env->key_values["super"] = TRUE;
    env->key_values["me"] = TRUE;
    env->key_values["samp"] = TRUE;
    env->key_values["ms"] = TRUE;
    env->key_values["second"] = TRUE;
    env->key_values["minute"] = TRUE;
    env->key_values["hour"] = TRUE;
    env->key_values["day"] = TRUE;
    env->key_values["week"] = TRUE;
    env->key_values["adc"] = TRUE;
    env->key_values["dac"] = TRUE;
    env->key_values["bunghole"] = TRUE;
    env->key_values["blackhole"] = TRUE;
    env->key_values["global"] = TRUE;
    env->key_values["chout"] = TRUE;
    env->key_values["cherr"] = TRUE;
    env->key_values["null"] = TRUE;
    env->key_values["NULL"] = TRUE;

    env->key_types["void"] = TRUE;
    env->key_types["same"] = TRUE;
    env->key_types["int"] = TRUE;
    env->key_types["float"] = TRUE;
    // env->key_types["dur"] = TRUE;
    // env->key_types["time"] = TRUE;
    // env->key_types["Object"] = TRUE;
    // env->key_types["string"] = TRUE;
    // env->key_types["Shred"] = TRUE;
    // env->key_types["Event"] = TRUE;
    // env->key_types["UGen"] = TRUE;
    // env->key_types["machine"] = TRUE;
    // env->key_types["language"] = TRUE;
    // env->key_types["compiler"] = TRUE;

    // ge and spencer reserve this one
    env->key_values["newlineEx2VistaHWNDVisualFoxProA"] = TRUE;

    // commit the global namespace
    env->global()->commit();

    // initialize operator mappings
    if( !type_engine_init_op_overload( env ) ) return FALSE;

    // pop indent level
    EM_poplog();

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: type_engine_shutdown()
// desc: shut down the type engine instance
//-----------------------------------------------------------------------------
void type_engine_shutdown( Chuck_Carrier * carrier )
{
    // log
    EM_log( CK_LOG_HERALD, "shutting down type system..." );
    // push
    EM_pushlog();

    // shut it down; this is system cleanup -- delete instead of release
    CK_SAFE_DELETE( carrier->env );

    // log
    EM_log( CK_LOG_HERALD, "type system shutdown complete." );
    // pop
    EM_poplog();
}




//-----------------------------------------------------------------------------
// name: type_engine_check_prog()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_prog( Chuck_Env * env, a_Program prog,
                                 const string & filename )
{
    t_CKBOOL ret = FALSE;

    // log
    EM_log( CK_LOG_FINE, "type-checking program '%s' - passes 0, 1, 2, 3...",
            filename.c_str() );
    // push indent
    EM_pushlog();

    // make the context
    Chuck_Context * context = type_engine_make_context( prog, filename );
    if( !context ) goto done;

    // reset the env
    env->reset();

    // load the context
    if( !type_engine_load_context( env, context ) )
    { ret = FALSE; goto done; }

    // 0th-scan (pass 0)
    if( !type_engine_scan0_prog( env, g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 1st-scan (pass 1)
    if( !type_engine_scan1_prog( env, g_program, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // 2nd-scan (pass 2)
    if( !type_engine_scan2_prog( env, prog, te_do_all ) )
    { ret = FALSE; goto cleanup; }

    // check the context (pass 3)
    if( !type_engine_check_context( env, context ) )
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
        ret = FALSE;
    }

done:

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_make_context()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Context * type_engine_make_context( a_Program prog, const string & filename )
{
    // log
    EM_log( CK_LOG_FINER, "creating context '%s'...", filename.c_str() );
    // push indent
    EM_pushlog();

    // each parse tree corresponds to a chuck context
    Chuck_Context * context = new Chuck_Context();
    // save a reference to the parse tree
    context->parse_tree = prog;
    // set name
    context->filename = filename;
    // set namespace to same name | 1.5.1.1
    context->nspc->name = filename;

    // pop indent
    EM_poplog();

    return context;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_context()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_context( Chuck_Env * env,
                                    Chuck_Context * context,
                                    te_HowMuch how_much )
{
    t_CKBOOL ret = TRUE;
    a_Program prog = NULL;

    // log
    EM_log( CK_LOG_FINER, "(pass 3) type-checking context..." );
                          // context->filename.c_str() );
    // push indent
    EM_pushlog();
    // how much
    EM_log( CK_LOG_FINEST, "target: %s", howmuch2str( how_much ) );

    // make sure there is a context
    if( !env->context )
    {
        // error
        EM_error2( 0, "(internal error) env->context NULL!" );
        ret = FALSE; goto done;
    }

    // parse tree
    prog = context->parse_tree;
    if( !prog )
    {
        // error
        EM_error2( 0, "(internal error) context->parse_tree NULL!" );
        ret = FALSE; goto done;
    }

    // go through each of the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // check the statements
            ret = type_engine_check_stmt_list( env, prog->section->stmt_list );
            break;

        case ae_section_func:
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // check the function definition
            ret = type_engine_check_func_def( env, prog->section->func_def );
            break;

        case ae_section_class:
            // if no classes, then skip
            if( how_much == te_do_no_classes ) break;
            // check the class definition
            ret = type_engine_check_class_def( env, prog->section->class_def );
            break;

        default:
            EM_error2( prog->where,
                "(internal error) unrecognized program section in type checker..." );
            ret = FALSE;
            break;
        }

        // next section
        prog = prog->next;
    }

    // make sure we still have the same context
    assert( env->contexts.size() != 0 );
    // assert( env->contexts.back() == context );

    // check to see if everything passed
    if( !ret )
    {
        // TODO: remove the effects of the context in the env
        // ---> insert code here <----

        // flag the context with error, so more stuff gets deleted
        env->context->has_error = TRUE;
    }

done:

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_load_context()
// desc: call this before context is type-checked
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_load_context( Chuck_Env * env, Chuck_Context * context )
{
    // log
    EM_log( CK_LOG_FINER, "loading context '%s'...", context->filename.c_str() );
    // push indent
    EM_pushlog();
    if( env->context ) EM_log( CK_LOG_FINER, "pushing existing context '%s'...",
                               env->context->filename.c_str() );
    // append the context to the env
    env->contexts.push_back( env->context );
    // make the context current
    env->context = context;
    // add reference
    env->context->add_ref();
    // push the context scope
    env->context->nspc->value.push();
    // push the current namespaces
    env->nspc_stack.push_back( env->curr );
    // set the parent
    context->nspc->parent = env->curr;
    // set the context's namespace as current
    env->curr = context->nspc;

    // pop indent
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_unload_context()
// desc: call this after context is emitted
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_unload_context( Chuck_Env * env )
{
    // make sure
    assert( env->context != NULL );
    assert( env->contexts.size() != 0 );

    // log
    EM_log( CK_LOG_FINER, "unloading context '%s'...", env->context->filename.c_str() );

    // push indent
    EM_pushlog();

    // log
    EM_log( CK_LOG_FINER, "decoupling namespace from AST..." );
    // decouple from AST, so we don't have references to deleted nodes
    // after AST cleanup | 1.5.0.5 (ge) added
    env->context->decouple_ast();
    // removing reference to AST tree, which is cleaned up elsewhere
    // see cleanup_AST() in the parser | 1.5.0.5 (ge) added
    env->context->parse_tree = NULL;
    // removing reference to public_class_def
    // this should only be used during compilation | 1.5.0.5 (ge) added
    env->context->public_class_def = NULL;

    // log
    EM_log( CK_LOG_FINER, "restoring context '%s'...", env->contexts.back()->filename.c_str() );
    // pop the context scope
    env->context->nspc->value.pop();
    // restore the current namespace
    env->curr = env->nspc_stack.back();
    // pop the namespace stack
    env->nspc_stack.pop_back();
    // release the context
    CK_SAFE_RELEASE( env->context );
    // restore context
    env->context = env->contexts.back();
    // pop the context
    env->contexts.pop_back();

    // make sure the nspc is ok
    assert( env->nspc_stack.size() != 0 );
    // assert( env->stack.back() == &(context->nspc) );

    // pop indent
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_stmt_list()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt_list( Chuck_Env * env, a_Stmt_List list )
{
    // type check the stmt_list
    while( list )
    {
        // the current statement
        if( !type_engine_check_stmt( env, list->stmt ) )
            return FALSE;

        // advance to the next statement
        list = list->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_stmt(()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt( Chuck_Env * env, a_Stmt stmt )
{
    t_CKBOOL ret = FALSE;

    if( !stmt )
        return TRUE;

    // push stmt | 1.5.1.7
    env->stmt_stack.push_back( stmt );

    // the type of stmt
    switch( stmt->s_type )
    {
        case ae_stmt_if:
            // count scope to help determine class member
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_if( env, &stmt->stmt_if );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_for:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_for( env, &stmt->stmt_for );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_foreach:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_foreach( env, &stmt->stmt_foreach );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_while:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_while( env, &stmt->stmt_while );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_until:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_until( env, &stmt->stmt_until );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_loop:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_check_loop( env, &stmt->stmt_loop );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_exp:
            ret = ( type_engine_check_exp( env, stmt->stmt_exp ) != NULL );
            break;

        case ae_stmt_return:
            ret = type_engine_check_return( env, &stmt->stmt_return );
            break;

        case ae_stmt_code:
            env->class_scope++;
            ret = type_engine_check_code_segment( env, &stmt->stmt_code );
            env->class_scope--;
            break;

        case ae_stmt_break:
            ret = type_engine_check_break( env, &stmt->stmt_break );
            break;

        case ae_stmt_continue:
            ret = type_engine_check_continue( env, &stmt->stmt_continue );
            break;

        case ae_stmt_switch:
            env->class_scope++;
            ret = type_engine_check_switch( env, &stmt->stmt_switch );
            env->class_scope--;
            break;

        case ae_stmt_case:
            // ret = type_engine_check_case( env, &stmt->stmt_case );
            break;

        case ae_stmt_gotolabel:
            // ret = type_engine_check_gotolabel( env, &stmt->goto_label );
            break;

        default:
            EM_error2( stmt->where,
                "internal compiler error - no stmt type '%i'!", stmt->s_type );
            ret = FALSE;
            break;
    }

    // pop stmt stack | 1.5.1.7
    env->stmt_stack.pop_back();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_if( Chuck_Env * env, a_Stmt_If stmt )
{
    // check the conditional
    if( !type_engine_check_exp( env, stmt->cond ) )
        return FALSE;

    // ensure that conditional has valid type
    switch( stmt->cond->type->xid )
    {
    case te_int:
    case te_float:
    case te_dur:
    case te_time:
        break;

    default:
        // check for IO
        if( isa( stmt->cond->type, env->ckt_io ) ) break;

        // error
        EM_error2( stmt->cond->where,
            "invalid type '%s' in if condition", stmt->cond->type->base_name.c_str() );
        return FALSE;
    }

    // check if
    if( !type_engine_check_stmt( env, stmt->if_body ) )
        return FALSE;

    // check else, if there is one
    if( stmt->else_body )
        if( !type_engine_check_stmt( env, stmt->else_body ) )
            return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_for()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_for( Chuck_Env * env, a_Stmt_For stmt )
{
    // check the initial
    if( !type_engine_check_stmt( env, stmt->c1 ) )
        return FALSE;

    // check the conditional
    if( !type_engine_check_stmt( env, stmt->c2 ) )
        return FALSE;

    // check for empty for loop conditional (added 1.3.0.0)
    if( stmt->c2 == NULL )
    {
        // error
        EM_error2( stmt->where,
                  "empty for loop condition..." );
        EM_error2( 0,
                  "...(note: explicitly use 'true' if it's the intent)" );
        EM_error2( 0,
                  "...(e.g., 'for( ; true; ){ /*...*/ }')" );
        return FALSE;
    }

    // ensure that conditional has valid type
    switch( stmt->c2->stmt_exp->type->xid )
    {
    case te_int:
    case te_float:
    case te_dur:
    case te_time:
        break;

    default:
        // check for IO
        if( isa( stmt->c2->stmt_exp->type, env->ckt_io ) ) break;

        // error
        EM_error2( stmt->c2->stmt_exp->where,
            "invalid type '%s' in for condition", stmt->c2->stmt_exp->type->base_name.c_str() );
        return FALSE;
    }

    // check the post
    if( stmt->c3 && !type_engine_check_exp( env, stmt->c3 ) )
        return FALSE;

    // for break and continue statement
    env->breaks.push_back( stmt->self );

    // check body
    // TODO: restore break stack? (same for other loops)
    if( !type_engine_check_stmt( env, stmt->body ) )
        return FALSE;

    // remove the loop from the stack
    assert( env->breaks.size() && env->breaks.back() == stmt->self );
    env->breaks.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_foreach() | 1.5.0.8 (ge) added
// desc: type check for( VAR : ARRAY )
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_foreach( Chuck_Env * env, a_Stmt_ForEach stmt )
{
    // check the array part
    if( !type_engine_check_exp( env, stmt->theArray ) )
        return FALSE;

    // get array type
    Chuck_Type * type_array = stmt->theArray->type;

    // make sure we have an array
    if( !type_array->array_depth )
    {
        // error
        EM_error2( stmt->theArray->where, "for( X : ARRAY ) expects ARRAY to be an array" );
        return FALSE;
    }

    // left should be a decl
    if( stmt->theIter->s_type == ae_exp_decl )
    {
        // process auto before we scan theIter
        if( !type_engine_infer_auto( env, &stmt->theIter->decl, stmt->theArray->type->array_type ) )
            return FALSE;
    }

    // check the iter part
    if( !type_engine_check_exp( env, stmt->theIter ) )
        return FALSE;

    // get iter type
    Chuck_Type * type_iter = stmt->theIter->type;

    // make sure lhs is a decl
    if( stmt->theIter->s_type != ae_exp_decl )
    {
        // type suggestion
        string suggest = type_array->base_name + " x";
        for( t_CKINT i = 0; i < type_array->array_depth-1; i++ ) suggest += "[]";

        // error
        EM_error2( stmt->theIter->where, "for( X : ARRAY ) expects X to be a declaration, e.g., '%s'", suggest.c_str() );
        return FALSE;
    }

    // make sure lhs has the appropriate array depth
    // NOTE to handle multiple-dimensional arrays:
    if( stmt->theIter->decl.num_var_decls > 1 )
    {
        // error
        EM_error2( stmt->theIter->where, "for( X : ARRAY ) X cannot declare more than one variable" );
        return FALSE;
    }

    // check if VAR is itself an array
    if( type_iter->array_depth > 0 )
    {
        // get var decl list
        a_Var_Decl_List list = stmt->theIter->decl.var_decl_list;
        // get var decl
        a_Var_Decl decl = list->var_decl;

        // make sure empty declaration []
        if( decl->array && decl->array->exp_list != NULL)
        {
            // error
            EM_error2( decl->array->exp_list->where, "for( X : ARRAY ) expects X to non-array or empty [] array declaration" );
            return FALSE;
        }

        // make sure the type matches
        if( !isa( type_array->array_type, type_iter->array_type ) )
        {
            // error
            EM_error2( stmt->theIter->where, "for( X : ARRAY ) type mismatch between X [%s] and ARRAY [%s]",
                       stmt->theIter->type->c_name(), stmt->theArray->type->c_name() );
            return FALSE;
        }
    }
    else
    {
        // make sure the type matches
        if( !isa( type_array->array_type, type_iter ) )
        {
            // error
            EM_error2( stmt->theIter->where, "for( X : ARRAY ) type mismatch between X [%s] and ARRAY [%s]",
                       stmt->theIter->type->c_name(), stmt->theArray->type->c_name() );
            return FALSE;
        }
    }

    // difference in array depth
    t_CKINT depth = type_array->array_depth - type_iter->array_depth;
    // VAR array depth should always be one less
    if( depth != 1 )
    {
        // error
        EM_error2( stmt->theIter->where, "for( X : ARRAY ) X [%s] must have one less array dimension than ARRAY [%s]",
                   stmt->theIter->type->c_name(), stmt->theArray->type->c_name() );
        return FALSE;
    }

    // if VAR is an Object type
    if( isobj(env, type_iter) )
    {
        // force VAR to be a reference decl for emitting
        // e.g., SinOsc x implicitly will be emitted as SinOsc @ x
        stmt->theIter->decl.var_decl_list->var_decl->force_ref = TRUE;
    }

    // for break and continue statement
    env->breaks.push_back( stmt->self );

    // check body
    // TODO: restore break stack? (same for other loops)
    if( !type_engine_check_stmt( env, stmt->body ) )
        return FALSE;

    // remove the loop from the stack
    assert( env->breaks.size() && env->breaks.back() == stmt->self );
    env->breaks.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_while()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_while( Chuck_Env * env, a_Stmt_While stmt )
{
    // check the conditional
    if( !type_engine_check_exp( env, stmt->cond ) )
        return FALSE;

    // ensure that conditional has valid type
    switch( stmt->cond->type->xid )
    {
    case te_int:
    case te_float:
    case te_dur:
    case te_time:
        break;

    default:
        // check for IO
        if( isa( stmt->cond->type, env->ckt_io ) ) break;

        // error
        EM_error2( stmt->cond->where,
            "invalid type '%s' in while condition", stmt->cond->type->base_name.c_str() );
        return FALSE;
    }

    // for break and continue statement
    env->breaks.push_back( stmt->self );

    // check the body
    if( !type_engine_check_stmt( env, stmt->body ) )
        return FALSE;

    // remove the loop from the stack
    assert( env->breaks.size() && env->breaks.back() == stmt->self );
    env->breaks.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_until()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_until( Chuck_Env * env, a_Stmt_Until stmt )
{
    // check the conditional
    if( !type_engine_check_exp( env, stmt->cond ) )
        return FALSE;

    // ensure that conditional has valid type
    switch( stmt->cond->type->xid )
    {
    case te_int:
    case te_float:
    case te_dur:
    case te_time:
        break;

    default:
        // check for IO
        if( isa( stmt->cond->type, env->ckt_io ) ) break;

        // error
        EM_error2( stmt->cond->where,
            "invalid type '%s' in until condition", stmt->cond->type->base_name.c_str() );
        return FALSE;
    }

    // for break and continue statement
    env->breaks.push_back( stmt->self );

    // check the body
    if( !type_engine_check_stmt( env, stmt->body ) )
        return FALSE;

    // remove the loop from the stack
    assert( env->breaks.size() && env->breaks.back() == stmt->self );
    env->breaks.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_loop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_loop( Chuck_Env * env, a_Stmt_Loop stmt )
{
    // check the conditional
    Chuck_Type * type = type_engine_check_exp( env, stmt->cond );
    if( !type ) return FALSE;

    // ensure the type in conditional is int (different from other loops)
    if( isa( type, env->ckt_float ) )
    {
        // cast
        stmt->cond->cast_to = env->ckt_int;
    }
    else if( !isa( type, env->ckt_int ) ) // must be int
    {
        EM_error2( stmt->where,
            "loop * conditional must be of type 'int'" );
        return FALSE;
    }

    // for break and continue statement
    env->breaks.push_back( stmt->self );

    // check the body
    if( !type_engine_check_stmt( env, stmt->body ) )
        return FALSE;

    // remove the loop from the stack
    assert( env->breaks.size() && env->breaks.back() == stmt->self );
    env->breaks.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_switch()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_switch( Chuck_Env * env, a_Stmt_Switch stmt )
{
    // TODO: implement this
    EM_error2( stmt->where, "switch not implemented" );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_break()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_break( Chuck_Env * env, a_Stmt_Break br )
{
    // check to see if inside valid stmt
    if( env->breaks.size() <= 0 )
    {
        EM_error2( br->where,
            "'break' found outside of for/while/until/switch" );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_continue()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_continue( Chuck_Env * env, a_Stmt_Continue cont )
{
    // check to see if inside valid loop
    if( env->breaks.size() <= 0 )
    {
        EM_error2( cont->where,
            "'continue' found outside of for/while/until" );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_return()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_return( Chuck_Env * env, a_Stmt_Return stmt )
{
    Chuck_Type * ret_type = NULL;

    // check to see if within function definition
    if( !env->func )
    {
        EM_error2( stmt->where,
            "'return' statement found outside function definition" );
        return FALSE;
    }

    // check the type of the return
    if( stmt->val )
        ret_type = type_engine_check_exp( env, stmt->val );
    else
        ret_type = env->ckt_void;

    t_CKTYPE left = ret_type;
    t_CKTYPE right = env->func->def()->ret_type;
    // if there is thing to return
    if( left )
    {
        // check implicit cast | 1.5.1.0
        CK_LR( te_int, te_float ) left = stmt->val->cast_to = env->ckt_float;

        // check to see that return type matches the prototype
        if( !isa( left, right ) )
        {
            // error
            EM_error2( stmt->where,
                "function '%s' was defined with return type '%s' -- but returning type '%s'",
                env->func->signature(FALSE).c_str(), env->func->def()->ret_type->c_name(),
                ret_type->c_name() );
            return FALSE;
        }
    }

    return ret_type != NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_infer_auto() | 1.5.0.8 (ge) added
// desc: process auto type
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_infer_auto( Chuck_Env * env, a_Exp_Decl decl, Chuck_Type * type )
{
    // make sure
    assert( type != NULL );
    assert( decl != NULL );

    // check first var decl, if not auto, then pass through
    if( !decl->ck_type || !isa(decl->ck_type, env->ckt_auto) )
    { return TRUE; }
    // if RHS is declared as auto, then check for invalid LHS types
    else if( isa(type,env->ckt_void) || isa(type,env->ckt_null) || isa(type,env->ckt_auto) )
    {
        EM_error2( decl->where,
            "cannot infer 'auto' type from '%s' type",
            type->c_name() );
        return FALSE;
    }

    // replace with inferred type
    CK_SAFE_REF_ASSIGN( decl->ck_type, type );
    // remember this was auto
    decl->is_auto = TRUE;

    // NOTE (likely) don't need to loop over decl->var_decl_list,
    // since we "cannot '=>' from/to a multi-variable declaration"

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_code_segment()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_code_segment( Chuck_Env * env, a_Stmt_Code stmt,
                                         t_CKBOOL push )
{
    // TODO: make sure this is in a function or is outside class

    // class
    env->class_scope++;
    // push
    if( push ) env->curr->value.push(); // env->context->nspc.value.push();
    // do it
    t_CKBOOL t = type_engine_check_stmt_list( env, stmt->stmt_list );
    // pop
    if( push ) env->curr->value.pop();  // env->context->nspc.value.pop();
    // class
    env->class_scope--;

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp( Chuck_Env * env, a_Exp exp )
{
    a_Exp curr = exp;

    // reset the group size
    exp->group_size = 0;

    // loop through parallel expressions
    while( curr )
    {
        // reset the type
        curr->type = NULL;
        // increment first exp's group size
        exp->group_size++;

        // examine the syntax
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
            curr->type = type_engine_check_exp_primary( env, &curr->primary );
        break;

        case ae_exp_array:
            curr->type = type_engine_check_exp_array( env, &curr->array );
        break;

        case ae_exp_func_call:
            curr->type = type_engine_check_exp_func_call( env, &curr->func_call );
            // set the return type
            curr->func_call.ret_type = curr->type;
            // add reference | 1.5.0.5
            CK_SAFE_ADD_REF( curr->func_call.ret_type );
            // check if return type is an Obj | 1.5.1.7
            if( curr->type && isobj( env, curr->type ) && env->stmt_stack.size() )
            {
                // increment # of objects in this stmt that needs release
                env->stmt_stack.back()->numObjsToRelease++;
            }
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

        default:
            EM_error2( curr->where,
                "internal compiler error - no expression type '%i'",
                curr->s_type );
            return NULL;
        }

        // error
        if( !curr->type )
            return NULL;

        // advance to next expression
        curr = curr->next;
    }

    // return type
    return exp->type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_binary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_binary( Chuck_Env * env, a_Exp_Binary binary )
{
    a_Exp cl = binary->lhs, cr = binary->rhs;
    t_CKTYPE ret = NULL;

    // type check the lhs
    t_CKTYPE left = type_engine_check_exp( env, cl );
    // check
    if( !left ) return NULL;

    // for 'auto' type assignment | 1.5.0.8 (ge) added
    if( cr->s_type == ae_exp_decl &&
        ( binary->op == ae_op_chuck || binary->op == ae_op_at_chuck ) )
    {
        // get type of the left hand side
        Chuck_Type * type = left;
        // if array use actual type
        if( type->array_depth ) type = type->array_type;

        // process auto before we scan the right hand side
        if( !type_engine_infer_auto( env, &cr->decl, type ) )
            return NULL;
    }

    // type check the rhs
    t_CKTYPE right = type_engine_check_exp( env, cr );
    // check
    if( !right ) return NULL;

    // cross chuck
    while( cr )
    {
        // type check the pair
        ret = type_engine_check_op( env, binary->op, cl, cr, binary );
        if( !ret ) return NULL;

        // next rhs
        cr = cr->next;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_ensure_no_multi_decl()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_ensure_no_multi_decl( a_Exp exp, const char * op_str )
{
    // go
    if( exp->s_type == ae_exp_decl && exp->decl.num_var_decls > 1 )
    {
        // multiple declarations on left side
        EM_error2( exp->where,
            "cannot '%s' from/to a multi-variable declaration", op_str );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op()
// desc: check binary operator
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs,
                               a_Exp_Binary binary )
{
     t_CKTYPE left = lhs->type, right = rhs->type;
     assert( left && right );

    // make sure not involve multiple declarations (for now)
    if( !type_engine_ensure_no_multi_decl( lhs, op2str(op) ) ||
        !type_engine_ensure_no_multi_decl( rhs, op2str(op) ) )
        return NULL;

    // if lhs is multi-value, then check separately
    if( (lhs->next && op != ae_op_chuck /*&& !isa( right, env->ckt_function)*/ ) || rhs->next )
    {
        // TODO: implement this
        EM_error2( binary->where,
            "multi-value (%s) operation not supported/implemented",
            op2str(op));
        return NULL;
    }

    // file I/O
    /*
    if( op == ae_op_arrow_left || op == ae_op_arrow_right )
    {
        // check left
        if( isa( left, env->ckt_io ) )
        {
            // check for assignment
            if( op == ae_op_arrow_right && lhs->s_meta != ae_meta_var )
            {
                // error
                EM_error2( rhs->where,
                    "cannot perform I/O assignment via '->' to non-mutable value" );
                return NULL;
            }

            // check right
            if( isa( right, env->ckt_int ) || isa( right, env->ckt_float ) ||
                isa( right, env->ckt_string ) )
                return left;
        }
    }
    */

    // implicit cast
    if( *left != *right )
    {
        // for some - int/float
        switch( op )
        {
        case ae_op_plus:
            // Object.toString
            if( isa( left, env->ckt_string ) && isa( right, env->ckt_object ) && !isa( right, env->ckt_string) )
            {
                // check for function (disallow for now, e.g., Math.fabs + "foo"
                if( isa(right, env->ckt_function) )
                {
                    // the name
                    string theName = right->base_name;
                    // position
                    t_CKINT where = rhs->where;
                    // check for dot
                    if( rhs->s_type == ae_exp_dot_member ) where = rhs->dot_member.where;
                    // report error
                    EM_error2( binary->where,
                               "cannot perform '+' on string and '%s'", theName.c_str() );
                    EM_error2( where,
                              "...(hint: to call the function, add '()' and any arguments)" );
                    return NULL;
                }
                // cast
                right = rhs->cast_to = env->ckt_string;
            }
            else if( isa( left, env->ckt_object ) && isa( right, env->ckt_string ) && !isa( left, env->ckt_string) )
            {
                if( isa(left, env->ckt_function) )
                {
                    // the name
                    string theName = left->base_name;
                    // position
                    t_CKINT where = lhs->where;
                    // check for dot
                    if( lhs->s_type == ae_exp_dot_member ) where = lhs->dot_member.where;
                    // check for function (disallow for now, e.g., Math.fabs + "foo"
                    EM_error2( binary->where,
                               "cannot perform '+' on '%s' and string", theName.c_str() );
                    EM_error2( where,
                               "...(hint: to call the function, add '()' and any arguments)" );
                    return NULL;
                }
                // cast left type to string
                left = lhs->cast_to = env->ckt_string;
            }
        case ae_op_minus:
            CK_LR( te_vec3, te_vec4 ) left = lhs->cast_to = env->ckt_vec4;
            else CK_LR( te_vec4, te_vec3 ) right = rhs->cast_to = env->ckt_vec4;
            else CK_LR( te_vec2, te_vec4 ) left = lhs->cast_to = env->ckt_vec4;
            else CK_LR( te_vec4, te_vec2 ) right = rhs->cast_to = env->ckt_vec4;
            else CK_LR( te_vec2, te_vec3 ) left = lhs->cast_to = env->ckt_vec3;
            else CK_LR( te_vec3, te_vec2 ) right = rhs->cast_to = env->ckt_vec3;
        case ae_op_times:
        case ae_op_divide:
        case ae_op_lt:
        case ae_op_le:
        case ae_op_gt:
        case ae_op_ge:
        case ae_op_eq:
        case ae_op_neq:
            // complex
            CK_LR( te_int, te_complex ) left = lhs->cast_to = env->ckt_complex;
            else CK_LR( te_complex, te_int ) right = rhs->cast_to = env->ckt_complex;
            CK_LR( te_float, te_complex ) left = lhs->cast_to = env->ckt_complex;
            else CK_LR( te_complex, te_float ) right = rhs->cast_to = env->ckt_complex;

            // polar
            CK_LR( te_int, te_polar ) left = lhs->cast_to = env->ckt_polar;
            else CK_LR( te_complex, te_int ) right = rhs->cast_to = env->ckt_polar;
            CK_LR( te_float, te_complex ) left = lhs->cast_to = env->ckt_polar;
            else CK_LR( te_complex, te_float ) right = rhs->cast_to = env->ckt_polar;
        case ae_op_percent:
            // mark for cast
            CK_LR( te_int, te_float ) left = lhs->cast_to = env->ckt_float;
            else CK_LR( te_float, te_int ) right = rhs->cast_to = env->ckt_float;
        break;

        default: break;
        }

        // no commute - int/float
        switch( op )
        {
        case ae_op_plus_chuck:
            // Object.toString
            if( isa( left, env->ckt_object ) && isa( right, env->ckt_string ) && !isa( left, env->ckt_string ) )
                left = lhs->cast_to = env->ckt_string;
        case ae_op_minus_chuck:
        case ae_op_times_chuck:
        case ae_op_divide_chuck:
        case ae_op_percent_chuck:
            // mark for cast
            CK_LR( te_int, te_float ) left = lhs->cast_to = env->ckt_float;
            CK_LR( te_float, te_complex ) left = lhs->cast_to = env->ckt_complex;
            CK_LR( te_float, te_polar ) left = lhs->cast_to = env->ckt_polar;
            CK_LR( te_int, te_complex ) left = lhs->cast_to = env->ckt_complex;
            CK_LR( te_int, te_polar ) left = lhs->cast_to = env->ckt_polar;
        break;

        default: break;
        }

        // int/dur and int/vectors
        if( op == ae_op_times )
        {
            CK_LR( te_int, te_dur ) left = lhs->cast_to = env->ckt_float;
            else CK_LR( te_dur, te_int ) right = rhs->cast_to = env->ckt_float;
            // vectors, 1.3.5.3
            else CK_LR( te_int, te_vec2 ) left = lhs->cast_to = env->ckt_float; // 1.5.1.7
            else CK_LR( te_int, te_vec3 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_int, te_vec4 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_vec2, te_int ) right = rhs->cast_to = env->ckt_float; // 1.5.1.7
            else CK_LR( te_vec3, te_int ) right = rhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_vec4, te_int ) right = rhs->cast_to = env->ckt_float; // 1.3.5.3
        }
        else if( op == ae_op_divide )
        {
            CK_LR( te_dur, te_int ) right = rhs->cast_to = env->ckt_float;
            // vectors, 1.3.5.3
            else CK_LR( te_int, te_vec2 ) left = lhs->cast_to = env->ckt_float; // 1.5.1.7
            else CK_LR( te_int, te_vec3 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_int, te_vec4 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_vec2, te_int ) right = rhs->cast_to = env->ckt_float; // 1.5.1.7
            else CK_LR( te_vec3, te_int ) right = rhs->cast_to = env->ckt_float; // 1.3.5.3
            else CK_LR( te_vec4, te_int ) right = rhs->cast_to = env->ckt_float; // 1.3.5.3
        }

        // op_chuck
        if( op == ae_op_times_chuck )
        {
            CK_LR( te_int, te_vec2 ) left = lhs->cast_to = env->ckt_float; // 1.5.1.7
            CK_LR( te_int, te_vec3 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            CK_LR( te_int, te_vec4 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
        }
        else if( op == ae_op_divide_chuck )
        {
            CK_LR( te_int, te_vec2 ) left = lhs->cast_to = env->ckt_float; // 1.5.1.7
            CK_LR( te_int, te_vec3 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
            CK_LR( te_int, te_vec4 ) left = lhs->cast_to = env->ckt_float; // 1.3.5.3
        }

        // array
        if( op == ae_op_shift_left && isa( left, env->ckt_array ) )
        {
            // type of array
            Chuck_Type * atype = left->array_type;
            // check type
            if( isa( atype, env->ckt_float ) )
            {
                if( right->xid == te_int ) right = rhs->cast_to = env->ckt_float;
            }
            else if( isa( atype, env->ckt_complex ) )
            {
                if( right->xid == te_int || right->xid == te_float
                    || right->xid == te_polar ) right = rhs->cast_to = env->ckt_complex;
            }
            else if( isa( atype, env->ckt_polar ) )
            {
                if( right->xid == te_int || right->xid == te_float
                    || right->xid == te_complex ) right = rhs->cast_to = env->ckt_complex;
            }
        }
    }

    // objects
    switch( op )
    {
    case ae_op_plus:
        // string + int/float
        if( isa( left, env->ckt_string ) )
        {
            // right is string or int/float
            if( isa( right, env->ckt_string ) || isa( right, env->ckt_int )
                || isa( right, env->ckt_float ) )
                break;
        }
    case ae_op_plus_chuck:
        // int/float + string
        if( isa( left, env->ckt_string ) || isa( left, env->ckt_int ) || isa( left, env->ckt_float ) )
        {
            // right is string
            if( isa( right, env->ckt_string ) )
                break;
        }
    case ae_op_minus:
    case ae_op_times:
    case ae_op_divide:
    //case ae_op_lt:
    //case ae_op_le:
    //case ae_op_gt:
    //case ae_op_ge:
    case ae_op_percent:
    case ae_op_minus_chuck:
    case ae_op_times_chuck:
    case ae_op_divide_chuck:
    case ae_op_percent_chuck:
        {
            // check overload | 1.5.1.5 (ge) added
            Chuck_Type * ret = type_engine_check_op_overload_binary( env, op, left, right, binary );
            // if we have a hit
            if( ret ) return ret;

            if( isa( left, env->ckt_object ) ) {
                EM_error2( binary->where, "cannot perform '%s' on object references",
                    op2str(op) );
                return NULL;
            }
            if( isa( right, env->ckt_object ) ) {
                EM_error2( binary->where, "cannot perform '%s' on object references",
                    op2str(op) );
                return NULL;
            }
        }
    break;

    default: break;
    }

    // make sure
    switch( op )
    {
    case ae_op_plus_chuck:
    case ae_op_minus_chuck:
    case ae_op_times_chuck:
    case ae_op_divide_chuck:
    case ae_op_percent_chuck:
    case ae_op_s_and_chuck:
    case ae_op_s_or_chuck:
    case ae_op_s_xor_chuck:
    case ae_op_shift_left_chuck:
    case ae_op_shift_right_chuck:
        // make sure mutable
        if( rhs->s_meta != ae_meta_var )
        {
            EM_error2( binary->where,
                "cannot assign '%s' on types '%s' %s '%s'...",
                op2str( op ), left->c_name(), op2str( op ), right->c_name() );
            EM_error2( rhs->where,
                "...(reason: right-side operand is not mutable)" );
            return NULL;
        }
        else
        {
            // check if rhs is a decl
            if( rhs->s_type == ae_exp_decl )
            {
                // error
                EM_error2( binary->where,
                    "cannot perform '%s' on a variable declaration...", op2str(op) );
                EM_error2( 0,
                    "...(hint: use '=>' instead to initialize the variable)" );
                return NULL;
            }

            // check if rhs is const
            if( rhs->s_type == ae_exp_primary )
            {
                Chuck_Value * v = type_engine_check_const( env, rhs );
                // check if const | 1.5.0.0 (ge) added
                if( v )
                {
                    // error
                    EM_error2( binary->where,
                        "cannot chuck/assign '%s' to '%s'...", op2str(op),
                        v->name.c_str() );
                    EM_error2( rhs->where,
                        "...(reason: '%s' is a constant, and is not assignable)", v->name.c_str() );
                    return NULL;
                }
            }
            else if( rhs->s_type == ae_exp_dot_member )
            {
                // catch things like `1 => Math.PI`
                Chuck_Value * v = type_engine_check_const( env, rhs );
                if( v )
                {
                    // the X.Y
                    string theVar = type_engine_print_exp_dot_member(env, &rhs->dot_member);
                    // error
                    EM_error2( binary->where,
                               "cannot chuck/assign '%s' to '%s'...", op2str(op), theVar.c_str() );
                    EM_error2( rhs->where,
                               "...(reason: '%s' is a constant, and is not assignable)", theVar.c_str() );
                    return NULL;
                }
            }
        }

        // mark to emit var instead of value
        rhs->emit_var = 1;

        break;

    default: break;
    }

    // based on the op
    switch( op )
    {
    case ae_op_chuck:
        return type_engine_check_op_chuck( env, lhs, rhs, binary );

    case ae_op_unchuck:
        return type_engine_check_op_unchuck( env, lhs, rhs, binary );

    case ae_op_upchuck:
        return type_engine_check_op_upchuck( env, lhs, rhs, binary );

    case ae_op_at_chuck:
        return type_engine_check_op_at_chuck( env, lhs, rhs, binary );

    case ae_op_plus_chuck:
        if( isa( left, env->ckt_string ) && isa( right, env->ckt_string ) ) return env->ckt_string;
        if( isa( left, env->ckt_int ) && isa( right, env->ckt_string ) ) return env->ckt_string;
        if( isa( left, env->ckt_float ) && isa( right, env->ckt_string ) ) return env->ckt_string;
    case ae_op_plus:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_dur, te_dur ) return env->ckt_dur;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        // CK_COMMUTE( te_float, te_complex ) return env->ckt_complex;
        // CK_COMMUTE( te_float, te_polar ) return env->ckt_polar;
        CK_LR( te_vec2, te_vec2 ) return env->ckt_vec2; // 1.5.1.7
        CK_LR( te_vec3, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_LR( te_vec4, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
        CK_COMMUTE( te_vec2, te_vec3 ) return env->ckt_vec3; // 1.5.1.7
        CK_COMMUTE( te_vec2, te_vec4 ) return env->ckt_vec4; // 1.5.1.7
        CK_COMMUTE( te_vec3, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
        CK_COMMUTE( te_dur, te_time ) return env->ckt_time;
        if( isa( left, env->ckt_string ) && isa( right, env->ckt_string ) ) return env->ckt_string;
        if( isa( left, env->ckt_string ) && isa( right, env->ckt_int ) ) return env->ckt_string;
        if( isa( left, env->ckt_string ) && isa( right, env->ckt_float ) ) return env->ckt_string;
        if( isa( left, env->ckt_int ) && isa( right, env->ckt_string ) ) return env->ckt_string;
        if( isa( left, env->ckt_float ) && isa( right, env->ckt_string ) ) return env->ckt_string;
    break;

    case ae_op_minus:
        CK_LR( te_time, te_time ) return env->ckt_dur;
        CK_LR( te_time, te_dur ) return env->ckt_time;
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_dur, te_dur ) return env->ckt_dur;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        // CK_COMMUTE( te_float, te_complex ) return env->ckt_complex;
        // CK_COMMUTE( te_float, te_polar ) return env->ckt_polar;
        CK_LR( te_vec2, te_vec2 ) return env->ckt_vec2; // 1.5.1.7
        CK_LR( te_vec3, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_LR( te_vec4, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
        CK_COMMUTE( te_vec2, te_vec3 ) return env->ckt_vec3; // 1.5.1.7
        CK_COMMUTE( te_vec2, te_vec4 ) return env->ckt_vec4; // 1.5.1.7
        CK_COMMUTE( te_vec3, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
    break;

    // take care of non-commutative
    case ae_op_minus_chuck:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_dur, te_dur ) return env->ckt_dur;
        CK_LR( te_dur, te_time ) return env->ckt_time;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        // CK_COMMUTE( te_float, te_complex ) return env->ckt_complex;
        // CK_COMMUTE( te_float, te_polar ) return env->ckt_polar;
        CK_LR( te_vec2, te_vec2 ) return env->ckt_vec3; // 1.5.1.7
        CK_LR( te_vec3, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_LR( te_vec4, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
        CK_COMMUTE( te_vec2, te_vec3 ) return env->ckt_vec3; // 1.5.1.7
        CK_COMMUTE( te_vec2, te_vec4 ) return env->ckt_vec4; // 1.5.1.7
        CK_COMMUTE( te_vec3, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
    break;

    case ae_op_times:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        CK_LR( te_vec3, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_LR( te_vec4, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
        CK_COMMUTE( te_float, te_dur ) return env->ckt_dur;
        // CK_COMMUTE( te_float, te_complex ) return env->ckt_complex;
        // CK_COMMUTE( te_float, te_polar ) return env->ckt_polar;
        CK_COMMUTE( te_float, te_vec2 ) return env->ckt_vec2; // 1.5.1.7
        CK_COMMUTE( te_float, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_COMMUTE( te_float, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
    break;

    case ae_op_times_chuck:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_float, te_dur ) return env->ckt_dur;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        CK_LR( te_float, te_vec2 ) return env->ckt_vec2; // 1.5.1.7
        CK_LR( te_float, te_vec3 ) return env->ckt_vec3; // 1.3.5.3
        CK_LR( te_float, te_vec4 ) return env->ckt_vec4; // 1.3.5.3
    break;

    case ae_op_divide:
        CK_LR( te_dur, te_dur ) return env->ckt_float;
        CK_LR( te_time, te_dur ) return env->ckt_float;
        CK_LR( te_dur, te_float ) return env->ckt_dur;
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        CK_LR( te_vec2, te_float ) return env->ckt_vec2;
        CK_LR( te_vec3, te_float ) return env->ckt_vec3;
        CK_LR( te_vec4, te_float ) return env->ckt_vec4;
    break;

    // take care of non-commutative
    case ae_op_divide_chuck:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_float, te_dur ) return env->ckt_dur;
        CK_LR( te_complex, te_complex ) return env->ckt_complex;
        CK_LR( te_polar, te_polar ) return env->ckt_polar;
        CK_LR( te_float, te_vec2 ) return env->ckt_vec2; // 1.5.1.7
        CK_LR( te_float, te_vec3 ) return env->ckt_vec3; // 1.5.1.7
        CK_LR( te_float, te_vec4 ) return env->ckt_vec4; // 1.5.1.7
    break;

    case ae_op_eq:
    case ae_op_neq:
        CK_LR( te_vec2, te_vec2 ) return env->ckt_int; // 1.5.1.7
        CK_LR( te_vec3, te_vec3 ) return env->ckt_int; // 1.3.5.3
        CK_LR( te_vec4, te_vec4 ) return env->ckt_int; // 1.3.5.3
        if( isa( left, env->ckt_object ) && isa( right, env->ckt_object ) ) return env->ckt_int;
    case ae_op_lt:
    case ae_op_le:
    {
        // file output
        if( op == ae_op_le &&  isa( left, env->ckt_io ) )
        {
            if( isa( right, env->ckt_int ) ) return left;
            else if( isa( right, env->ckt_float ) ) return left;
            else if( isa( right, env->ckt_string ) ) return left;
            else if( isa( right, env->ckt_complex ) ) return left;
            else if( isa( right, env->ckt_polar ) ) return left;
            else if( isa( right, env->ckt_vec2 ) ) return left;
            else if( isa( right, env->ckt_vec3 ) ) return left;
            else if( isa( right, env->ckt_vec4 ) ) return left;
            else // error
            {
                EM_error2( binary->where, "no suitable IO action for '%s' <= '%s'",
                          left->c_name(), right->c_name() );
                return NULL;
            }
        }
    }
    case ae_op_gt:
    case ae_op_ge:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_int;
        CK_LR( te_dur, te_dur ) return env->ckt_int;
        CK_LR( te_time, te_time ) return env->ckt_int;
        CK_LR( te_complex, te_complex ) return env->ckt_int;
        CK_LR( te_polar, te_polar ) return env->ckt_int;
        // CK_COMMUTE( te_float, te_complex ) return env->ckt_int;
        // CK_COMMUTE( te_float, te_polar ) return env->ckt_int;
        // CK_COMMUTE( te_vec2, te_vec3 ) return env->ckt_int; // 1.5.1.7
        // CK_COMMUTE( te_vec2, te_vec4 ) return env->ckt_int; // 1.5.1.7
        // CK_COMMUTE( te_vec3, te_vec4 ) return env->ckt_int; // 1.3.5.3

        // disallowed | 1.5.1.7 (moved to EQ and NEQ only)
        // if( isa( left, env->ckt_object ) && isa( right, env->ckt_object ) ) return env->ckt_int;
    break;

    case ae_op_shift_left:
        // prepend || append
        if( isa( left, env->ckt_array ) )
        {
            // sanity check
            assert( left->array_type != NULL );
            // check type
            if( isa( right, left->array_type ) ) return left;
            // both arrays
            if( right->array_depth > 0 && left->array_depth > 0 )
                if( right->array_type == left->array_type &&
                    right->array_depth + 1 == left->array_depth ) return left;
        }
    case ae_op_shift_right:
    case ae_op_s_and_chuck:
    case ae_op_s_or_chuck:
    case ae_op_s_xor_chuck:
    case ae_op_shift_right_chuck:
    case ae_op_shift_left_chuck:
        // the above are non-commutative
    case ae_op_and:
    case ae_op_or:
    case ae_op_s_xor:
    case ae_op_s_and:
    case ae_op_s_or:
        // shift
        CK_LR( te_int, te_int ) return env->ckt_int;
    break;

    case ae_op_percent:
        CK_LR( te_time, te_dur ) return env->ckt_dur;
        CK_LR( te_dur, te_dur ) return env->ckt_dur;
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
    break;

    // take of non-commutative
    case ae_op_percent_chuck:
        CK_LR( te_int, te_int ) return env->ckt_int;
        CK_LR( te_float, te_float ) return env->ckt_float;
        CK_LR( te_dur, te_dur ) return env->ckt_dur;
    break;

    default: break;
    }

    // check overload | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_binary( env, op, left, right, binary );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( binary->where,
        "cannot resolve operator '%s' on types '%s' and '%s'",
        op2str( op ), left->c_name(), right->c_name() );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_overload_binary()
// desc: type check binary operator overload
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_overload_binary( Chuck_Env * env, ae_Operator op,
                                               Chuck_Type * lhs, Chuck_Type * rhs,
                                               a_Exp_Binary binary )
{
    // look up
    Chuck_Op_Overload * overload = env->op_registry.lookup_overload( lhs, op, rhs );
    // check if we have overload
    if( !overload ) return NULL;

    // if reserved, then should handled elsewhere, return nil
    if( overload->reserved() ) return NULL;

    // remember the function
    binary->ck_overload_func = overload->func();
    // check it
    if( !binary->ck_overload_func )
    {
        // no match
        EM_error2( binary->where,
            "(internal error) missing operator '%s' implementation on types '%s' and '%s'...",
            op2str( op ), lhs->c_name(), rhs->c_name() );
        // done
        return NULL;
    }

    // get return type | 1.5.1.8 (ge & andrew) we are back
    Chuck_Type * rtype  = binary->ck_overload_func->type();
    // check if return type is an Obj
    if( rtype && isobj( env, rtype ) && env->stmt_stack.size() )
    {
        // increment # of objects in this stmt that needs release
        env->stmt_stack.back()->numObjsToRelease++;
    }

    // the return type
    return binary->ck_overload_func->type();
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_overload_unary()
// desc: type check unary (prefix) operator overload
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_overload_unary( Chuck_Env * env, ae_Operator op,
                                              Chuck_Type *  rhs, a_Exp_Unary unary )
{
    // look up
    Chuck_Op_Overload * overload = env->op_registry.lookup_overload( op, rhs );
    // check if we have overload
    if( !overload ) return NULL;

    // if reserved, then should handled elsewhere, return nil
    if( overload->reserved() ) return NULL;

    // the function
    unary->ck_overload_func = overload->func();
    // check it
    if( !unary->ck_overload_func )
    {
        // no match
        EM_error2( unary->where,
            "(internal error) missing unary (prefix) operator '%s' implementation on type '%s'",
            op2str( op ), rhs->c_name() );
        // done
        return NULL;
    }

    // get return type | 1.5.1.8 (ge & andrew) we are back
    Chuck_Type * rtype  = unary->ck_overload_func->type();
    // check if return type is an Obj
    if( rtype && isobj( env, rtype ) && env->stmt_stack.size() )
    {
        // increment # of objects in this stmt that needs release
        env->stmt_stack.back()->numObjsToRelease++;
    }

    // the return type
    return unary->ck_overload_func->type();
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_overload_postfix()
// desc: type check postfix operator overload
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_overload_postfix( Chuck_Env * env, Chuck_Type * lhs,
                                                ae_Operator op, a_Exp_Postfix postfix )
{
    // look up
    Chuck_Op_Overload * overload = env->op_registry.lookup_overload( lhs, op );
    // check if we have overload
    if( !overload ) return NULL;

    // if reserved, then should handled elsewhere, return nil
    if( overload->reserved() ) return NULL;

    // the function
    postfix->ck_overload_func = overload->func();
    // check it
    if( !postfix->ck_overload_func )
    {
        // no match
        EM_error2( postfix->where,
            "(internal error) missing postfix operator '%s' implementation on type '%s'",
            op2str( op ), lhs->c_name() );
        // done
        return NULL;
    }

    // get return type | 1.5.1.8 (ge & andrew) we are back
    Chuck_Type * rtype  = postfix->ck_overload_func->type();
    // check if return type is an Obj
    if( rtype && isobj( env, rtype ) && env->stmt_stack.size() )
    {
        // increment # of objects in this stmt that needs release
        env->stmt_stack.back()->numObjsToRelease++;
    }

    // the return type
    return postfix->ck_overload_func->type();
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_chuck()
// desc: type check chuck operator =>
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs,
                                     a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // chuck to function
    if( !isnull(env, right) && isa( right, env->ckt_function ) )
    {
        // treat this function call
        return type_engine_check_exp_func_call( env, rhs, lhs, binary->ck_func, binary->where );
    }

    // multi-value not supported beyond this for now
    if( lhs->next || rhs->next )
    {
        EM_error2( binary->where,
            "multi-value (=>) operation not supported/implemented" );
        return NULL;
    }

    // ugen => ugen
    // ugen[] => ugen[]
    if( ( isa( left, env->ckt_ugen ) || ( isa( left, env->ckt_array ) && isa( left->array_type, env->ckt_ugen ) ) ) &&
        ( isa( right, env->ckt_ugen ) || ( isa( right, env->ckt_array ) && isa( right->array_type, env->ckt_ugen ) ) ) )
    {
        t_CKTYPE left_ugen_type = NULL;
        t_CKTYPE right_ugen_type = NULL;

        if( isa( left, env->ckt_array ) )
        {
            // check LHS isn't empty array reference decl e.g., SinOsc foos[] => X | 1.5.1.3
            if( lhs->s_type == ae_exp_decl )
            {
                // get var_decl; first one should do
                a_Var_Decl var_decl = lhs->decl.var_decl_list->var_decl;
                // is array reference e.g., declared with empty dimensions
                t_CKBOOL is_array_ref = var_decl->array && (var_decl->array->exp_list == NULL);
                // check it
                if( is_array_ref )
                {
                    // error
                    EM_error2( var_decl->where,
                               "cannot connect '=>' from empty array '[ ]' declaration..." );
                    EM_error2( 0, "...(hint: declare '%s' as an non-empty array)", var_decl->value->name.c_str() );
                    EM_error2( 0, "...(or if assignment was the intent, use '@=>' instead)" );
                    return NULL;
                }
            }

            // array type
            left_ugen_type = left->array_type;

            // array depth
            if( left->array_depth > 1 )
            {
                EM_error2( lhs->where, "array ugen type has more than one dimension - can only => one-dimensional array of mono ugens" );
                return NULL;
            }

            // # input channels
            if( left_ugen_type->ugen_info->num_outs > 1 )
            {
                // error
                EM_error2( lhs->where,
                           "array ugen type '%s' has more than one output channel; can only => one-dimensional array of mono ugens",
                           left_ugen_type->c_name() );
                return NULL;
            }
        }
        else
        {
            left_ugen_type = left;
        }

        if( isa( right, env->ckt_array ) )
        {
            // check RHS isn't empty array reference decl e.g., X => SinOsc bars[] | 1.5.1.3
            if( rhs->s_type == ae_exp_decl )
            {
                // get var_decl; first one should do
                a_Var_Decl var_decl = rhs->decl.var_decl_list->var_decl;
                // is array reference e.g., declared with empty dimensions
                t_CKBOOL is_array_ref = var_decl->array && (var_decl->array->exp_list == NULL);
                // check it
                if( is_array_ref )
                {
                    // error
                    EM_error2( var_decl->where,
                               "cannot connect '=>' to empty array '[ ]' declaration..." );
                    EM_error2( 0, "...(hint: declare '%s' as an non-empty array)", var_decl->value->name.c_str() );
                    EM_error2( 0, "...(or if assignment was the intent, use '@=>' instead)" );
                    return NULL;
                }
            }

            // array type
            right_ugen_type = right->array_type;

            // check array depth
            if( right->array_depth > 1 )
            {
                EM_error2( rhs->where,
                           "array ugen type has more than one dimension - can only => one-dimensional array of mono ugens" );
                return NULL;
            }

            // check # input channels
            if( right_ugen_type->ugen_info->num_ins > 1 )
            {
                // error
                EM_error2( rhs->where,
                           "array ugen type '%s' has more than one input channel - can only => array of mono ugens",
                           right_ugen_type->c_name() );
                return NULL;
            }
        }
        else
        {
            right_ugen_type = right;
        }

        // make sure non-zero
        if( left_ugen_type->ugen_info->num_outs == 0 )
        {
            // error
            EM_error2( lhs->where,
                "ugen's of type '%s' have no output - cannot => to another ugen",
                left_ugen_type->c_name() );
            return NULL;
        }
        else if( right_ugen_type->ugen_info->num_ins == 0 )
        {
            // error
            EM_error2( rhs->where,
                "ugen's of type '%s' have no input - cannot => from another ugen",
                right_ugen_type->c_name() );
            return NULL;
        }

        return right;
    }

    // time advance ( dur => now )
    if( isa( left, env->ckt_dur ) && isa( right, env->ckt_time ) && rhs->s_meta == ae_meta_var
        && rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        return right;
    }

    // event wait ( Event => now )
    if( isa( left, env->ckt_event ) && isa( right, env->ckt_time ) && rhs->s_meta == ae_meta_var
        && rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        return right;
    }

    // input ( IO => int ), (IO => float)
    if( isa( left, env->ckt_io ) )
    {
        // right hand side
        if( rhs->s_meta != ae_meta_var )
        {
            // error
            EM_error2( rhs->where,
                "cannot perform I/O assignment via '=>' to non-mutable value" );
            return NULL;
        }

        // check right
        if( isa( right, env->ckt_int ) || isa( right, env->ckt_float ) )
        {
            // emit ref
            rhs->emit_var = TRUE;
            return left;
        }

        if( isa( right, env->ckt_string ) )
        {
            return left;
        }
    }

    // object.toString

    // implicit cast
    CK_LR( te_int, te_float ) left = lhs->cast_to = env->ckt_float;
    CK_LR( te_int, te_complex ) left = lhs->cast_to = env->ckt_complex;
    CK_LR( te_int, te_polar ) left = lhs->cast_to = env->ckt_polar;
    CK_LR( te_vec2, te_vec3 ) left = lhs->cast_to = env->ckt_vec3;
    CK_LR( te_vec2, te_vec4 ) left = lhs->cast_to = env->ckt_vec4;
    CK_LR( te_vec3, te_vec4 ) left = lhs->cast_to = env->ckt_vec4;

    // assignment or something else
    if( isa( left, right ) )
    {
        // basic types?
        if( type_engine_check_primitive( env, left ) || isa( left, env->ckt_string ) )
        {
            // if the right is a decl, then make sure ref
            if( isa( left, env->ckt_string ) && rhs->s_type == ae_exp_decl )
            {
                rhs->decl.type->ref = TRUE;
            }
            // assigment?
            if( rhs->s_meta == ae_meta_var )
            {
                // check if rhs is const
                if( rhs->s_type == ae_exp_primary )
                {
                    Chuck_Value * v = type_engine_check_const( env, rhs );
                    // check if const | 1.5.0.0 (ge) added
                    if( v )
                    {
                        // error
                        EM_error2( binary->where,
                            "cannot chuck/assign => to '%s'...",
                            v->name.c_str() );
                        EM_error2( rhs->where,
                            "...(reason: '%s' is a constant, and is not assignable)", v->name.c_str() );
                        return NULL;
                    }
                }
                else if( rhs->s_type == ae_exp_dot_member )
                {
                    // catch things like `1 => Math.PI`
                    Chuck_Value * v = type_engine_check_const( env, rhs );
                    if( v )
                    {
                        // the X.Y
                        string theVar = type_engine_print_exp_dot_member(env, &rhs->dot_member);
                        // error
                        EM_error2( binary->where,
                                   "cannot chuck/assign '=>' to '%s'...", theVar.c_str() );
                        EM_error2( rhs->where,
                                   "...(reason: '%s' is a constant, and is not assignable)", theVar.c_str() );
                        return NULL;
                    }
                }

                // emit ref - remember for emitter
                rhs->emit_var = TRUE;
                // right side
                return right;
            }

            // error
            EM_error2( binary->where,
                "cannot chuck/assign '=>' on types '%s' => '%s'...",
                left->c_name(), right->c_name() );
            EM_error2( rhs->where,
                "...(reason: right-side operand is not mutable)" );
            return NULL;
        }
        // aggregate types
        else
        {
            // check overloading of => | 1.5.1.5 (ge) added
            Chuck_Type * ret = type_engine_check_op_overload_binary( env, ae_op_chuck, left, right, binary );
            // if we have a hit
            if( ret ) return ret;

            // no match
            EM_error2( binary->where,
                "cannot resolve operator '=>' on types '%s' and '%s'...",
                left->c_name(), right->c_name() );
            EM_error2( binary->where,
                "...(note: use '@=>' for object reference assignment)" );
            return NULL;
        }
    }

    // check overloading of => | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_binary( env, ae_op_chuck, left, right, binary );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( binary->where,
        "cannot resolve operator '=>' on types '%s' and '%s'",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_unchuck()
// desc: type check unchuck operator =<
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // ugen =< ugen
    if( isa( left, env->ckt_ugen ) && isa( right, env->ckt_ugen ) ) return right;

    // check overloading of =< | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_binary( env, ae_op_unchuck, left, right, binary );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( binary->where,
        "cannot resolve operator '=<' on types '%s' and '%s'",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_upchuck()
// desc: type check upchuck operator =^
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // uana =^ uana
    if( isa( left, env->ckt_uana ) && isa( right, env->ckt_uana ) ) return right;

    // check overloading of =^ | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_binary( env, ae_op_upchuck, left, right, binary );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( binary->where,
        "cannot resolve operator '=^' on types '%s' and '%s'",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_at_chuck()
// desc: type check at-chuck operator @=>
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // static
    //if( isa( left, env->ckt_class ) )
    //{
    //    EM_error2( lhs->where,
    //        "cannot assign '@=>' using static class as left operand" );
    //    return NULL;
    //}
    //else if( isa( right, env->ckt_class ) )
    //{
    //    EM_error2( rhs->where,
    //        "cannot assign '@=>' using static class as right operand" );
    //    return NULL;
    //}

    // make sure mutable
    if( rhs->s_meta != ae_meta_var )
    {
        EM_error2( binary->where,
            "cannot assign '@=>' on types '%s' @=> '%s'...",
            left->c_name(), right->c_name() );
        EM_error2( rhs->where,
            "...(reason: right-side operand is not mutable)" );
        return NULL;
    }

    // if the right is a decl, then make sure ref
    if( rhs->s_type == ae_exp_decl )
    {
        // get var_decl; first one should do
        a_Var_Decl var_decl = rhs->decl.var_decl_list->var_decl;
        // is array reference e.g., declared with empty dimensions
        t_CKBOOL is_array_ref = var_decl->array && (var_decl->array->exp_list == NULL);
        // check if full array declaration, e.g., int foo[2] or Object bar[2][2]
        if( rhs->type->array_depth > 0 && !is_array_ref )
        {
            // this cases like [1,2] @=> int foo[2];
            // (basically if the rhs is an array decl with non-empty dimensions)
            EM_error2( var_decl->array->exp_list->where, "cannot assign '@=>' to full array declaration..." );
            t_CKBOOL is_ref = isobj(env, rhs->type->array_type) && rhs->decl.type->ref;
            string varName = S_name(rhs->decl.var_decl_list->var_decl->xid);
            string brackets;
            for( t_CKINT i = 0; i < rhs->type->array_depth; i++ ) brackets += "[ ]";
            EM_error2( 0, "...(hint: declare as empty array -- e.g., %s %s%s%s)",
                       rhs->type->base_name.c_str(), is_ref ? "@ " : "", varName.c_str(), brackets.c_str());
            return NULL;
        }

        // otherwise set ref
        rhs->decl.type->ref = TRUE;
        var_decl->ref = TRUE;
    }

    // implicit cast
    CK_LR( te_int, te_float ) left = lhs->cast_to = env->ckt_float;

    // primitive
    if( !isa( left, right ) )
    {
        EM_error2( binary->where,
            "cannot assign '@=>' on types '%s' @=> '%s'...",
            left->c_name(), right->c_name() );
        EM_error2( binary->where,
            "...(reason: incompatible types for assignment)" );
        return NULL;
    }

    // check overloading of @=> (disallowed for now) | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_binary( env, ae_op_at_chuck, left, right, binary );
    // if we have a hit
    if( ret ) return ret;

    // assign
    rhs->emit_var = TRUE;

    return right;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_unary()
// desc: type check (prefix) unary expression
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_unary( Chuck_Env * env, a_Exp_Unary unary )
{
    Chuck_Type * t = NULL;

    // make sure
    if( unary->exp )
    {
        // current sporking state | 1.5.0.8 (ge)
        t_CKBOOL sporkingSaved = env->sporking;
        // set sporking flag | 1.5.0.8 (ge)
        if( unary->op == ae_op_spork ) env->sporking = TRUE;

        // assert( unary->op == ae_op_new );
        t = type_engine_check_exp( env, unary->exp );

        // unset sporking flag | 1.5.0.8 (ge)
        if( unary->op == ae_op_spork ) env->sporking = sporkingSaved;

        // check returned type
        if( !t ) return NULL;
    }

    // check code stmt; this is to eventually support sporking of code (added 1.3.0.0)
    //if( unary->code )
    //{
    //    // check it!
    //    if( !type_engine_check_stmt( env, unary->code ) ) return NULL;
    //}

    // check the op
    switch( unary->op )
    {
        case ae_op_plusplus:
        case ae_op_minusminus:
            // assignable?
            if( unary->exp->s_meta != ae_meta_var )
            {
                EM_error2( unary->where,
                    "prefix unary operator '%s' cannot "
                    "be used on non-mutable data-types", op2str( unary->op ) );
                return NULL;
            }

            // check type
            if( !unary->ck_overload_func && ( isa( t, env->ckt_int ) /*|| isa( t, env->ckt_float )*/ ) )
            {
                // emit as variable instead of value
                unary->exp->emit_var = TRUE;
                return t;
            }

            // check type
            if( isa( t, env->ckt_int ) || isa( t, env->ckt_float ) )
                return t;

            // TODO: check overloading
        break;

        case ae_op_minus:
            // float
            if( isa( t, env->ckt_float ) ) return t;
        case ae_op_tilda:
        case ae_op_exclamation:
            // int
            if( isa( t, env->ckt_int ) ) return t;
        break;

        case ae_op_spork:
            // spork shred (by function call)
            if( unary->exp && unary->exp->s_type == ae_exp_func_call ) return env->ckt_shred;
            // spork shred (by code segment)
            // else if( unary->code ) return env->ckt_shred;
            // got a problem
            else
            {
                 EM_error2( unary->exp ? unary->exp->where : unary->where,
                     "only function calls can be sporked" );
                 return NULL;
            }
        break;

        case ae_op_new:
            // look up the type
            t = type_engine_find_type( env, unary->type->xid );
            if( !t )
            {
                // EM_error2( 0, "...in 'new' expression " );
                return NULL;
            }

            // dependency tracking | 1.5.0.8 (ge) added
            // if in a function definition
            if( env->func )
            {
                // dependency tracking: add the callee's dependencies
                env->func->depends.add( &t->depends );
            }
            else if( env->class_def ) // in a class definition
            {
                // dependency tracking: add the callee's dependencies
                env->class_def->depends.add( &t->depends );
            }

            // constructors | 1.5.2.0 (ge) added
            if( unary->ctor.invoked )
            {
                // type check any constructor args
                if( unary->ctor.args )
                {
                    // check the argument list
                    if( !type_engine_check_exp( env, unary->ctor.args ) )
                        return NULL;

                    // type check and get constructor function
                    unary->ctor.func = type_engine_check_ctor_call( env, t, &unary->ctor, unary->array, unary->where );
                    // check for error
                    if( !unary->ctor.func )
                        return NULL;
                }
            }

            // []
            if( unary->array )
            {
                // verify there are no errors from the parser...
                if( !verify_array( unary->array ) )
                    return NULL;

                // if empty
                if( !unary->array->exp_list )
                {
                    EM_error2( unary->where, "cannot use empty [] with 'new'" );
                    return NULL;
                }

                // type check the exp
                if( !type_engine_check_exp( env, unary->array->exp_list ) )
                    return NULL;
                // make sure types are of int
                if( !type_engine_check_array_subscripts( env, unary->array->exp_list ) )
                    return NULL;

                // create the new array type, replace t
                t = new_array_type(
                    env,  // the env
                    env->ckt_array,  // the array base class, usually env->ckt_array
                    unary->array->depth,  // the depth of the new type
                    t,  // the 'array_type'
                    env->curr  // the owner namespace
                );

                // TODO: ref?
            }

            // make sure the type is not a primitive
            if( isa( t, env->ckt_int ) || isa( t, env->ckt_float ) || isa( t, env->ckt_dur )
                || isa( t, env->ckt_time ) )
            {
                EM_error2( unary->type->where,
                    "cannot use 'new' on primitive type '%s'...",
                    t->c_name() );
                EM_error2( 0, "(primitive types: 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
                return NULL;
            }

            // make sure the type is not a reference
            if( unary->type->ref && !unary->array )
            {
                EM_error2( unary->where,
                    "cannot use 'new' on an individual object reference (@)" );
                return NULL;
            }

            // check if return type is an Obj | 1.5.1.8
            if( isobj( env, t ) && env->stmt_stack.size() )
            {
                // increment # of objects in this stmt that needs release
                env->stmt_stack.back()->numObjsToRelease++;
            }

            // return the type
            return t;
        break;

        default: break;
    }

    // check overloading of unary operator | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_unary( env, unary->op, t, unary );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( unary->where,
        "cannot resolve prefix operator '%s' on type '%s",
        op2str( unary->op ), t->c_name() );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_primary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_primary( Chuck_Env * env, a_Exp_Primary exp )
{
    t_CKTYPE t = NULL;
    Chuck_Value * v = NULL;
    string str;

    // check syntax
    switch( exp->s_type )
    {
        // variable
        case ae_primary_var:
            str = S_name(exp->var);

            // check it
            if( str == "this" ) // this
            {
                // in class def
                if( !env->class_def )
                {
                    EM_error2( exp->where,
                        "keyword 'this' cannot used outside class definition" );
                    return NULL;
                }

                // in member func
                if( env->func && !env->func->is_member )
                {
                    EM_error2( exp->where,
                        "keyword 'this' cannot be used inside static functions" );
                    return NULL;
                }

                // not assignable
                exp->self->s_meta = ae_meta_value;
                // whatever the class is
                t = env->class_def;
            }
            else if( str == "me" ) // me
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // refers to shred
                t = env->ckt_shred;
            }
            else if( str == "now" ) // now
            {
                // assignable in a strongly timed way
                exp->self->s_meta = ae_meta_var;
                // time
                t = env->ckt_time;
            }
            else if( str == "dac" ) // dac
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->ckt_dac;
            }
            else if( str == "adc" ) // adc
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->ckt_adc;
            }
            else if( str == "bunghole" ) // bunghole | reinstated 1.5.2.4 (ge)
            {
                // non assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->ckt_ugen;
            }
            else if( str == "blackhole" ) // blackhole
            {
                // non assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->ckt_ugen;
            }
            else if( str == "null" || str == "NULL" ) // null / NULL
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // refers to null
                t = env->ckt_null;
            }
            else if( str == "void" ) // void
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // refers to void
                t = env->ckt_void;
            }
            else  // look up
            {
                // NOTE: could be tricky | 1.5.1.3 (2023)
                // SEE: https://github.com/ccrma/chuck/issues/16 (from 2014)
                // value lookup priority (in case of variable shadowing)
                // 1) first look in the same scope
                // 2) if not found, and if inside a class def, look up-scope within class (but no further)
                // 3) if not found, and if inside a class def, look in parent (for inherited value)
                // 4) if still not found, look all the way up to global scope

                // look in local (same) scope first
                v = type_engine_find_value( env, S_name(exp->var), FALSE );
                if( !v )
                {
                    // if in class
                    if( env->class_def )
                    {
                        // look up scope up-to class top-level (but no further); i.e., stayWithClass == TRUE
                        v = type_engine_find_value( env, S_name(exp->var), TRUE, TRUE, exp->where );
                        // if still not found, see if in parent (inherited)
                        if( !v ) v = type_engine_find_value( env->class_def->parent, exp->var );
                    }

                    // still not found
                    if( !v )
                    {
                        // look globally (stayWithClass == FALSE)
                        v = type_engine_find_value( env, S_name(exp->var), TRUE, FALSE, exp->where );

                        // 1.5.0.8 (ge) added this check
                        // public classes cannot access values that are:
                        // file-context-global-scope (v->is_context_global)
                        // AND non-explictly-global !(v->is_global) variables
                        if( v && v->is_context_global && !v->is_global
                              && env->class_def && env->context->public_class_def
                              && env->context->public_class_def->decl == ae_key_public )
                        {
                            if( v->func_ref )
                            {
                                EM_error2( exp->where,
                                    "cannot call local function '%s' from within a public class", S_name( exp->var ) );
                            }
                            else
                            {
                                EM_error2( exp->where,
                                    "cannot access local variable '%s' from within a public class", S_name( exp->var ) );
                            }
                            return NULL;
                        }

                        // 1.5.2.0 (ge) added this check
                        // @destruct() cannot access values that are:
                        // file-context-global-scope (v->is_context_global)
                        // AND non-explictly-global !(v->is_global) variables
                        if( v && v->is_context_global && !v->is_global
                              && env->class_def && env->func && isdtor(env, env->func->def()) )
                        {
                            if( v->func_ref )
                            {
                                EM_error2( exp->where,
                                    "cannot call local function '%s' from within @destruct()", S_name( exp->var ) );
                            }
                            else
                            {
                                EM_error2( exp->where,
                                    "cannot access local variable '%s' from within @destruct()", S_name( exp->var ) );
                            }
                            return NULL;
                        }
                    }

                    // check
                    if( v )
                    {
                        // inside a class
                        if( env->class_def )
                        {
                            // inside a function definition
                            if( env->func )
                            {
                                // if func static, v not
                                if( env->func->is_static && v->is_member && !v->is_static )
                                {
                                    EM_error2( exp->where,
                                        "non-static member '%s' used from static function", S_name( exp->var ) );
                                    return NULL;
                                }
                            }
                        }
                    }
                    // error
                    else
                    {
                        // checking for class scope incorrect (thanks Robin Davies)
                        if( !env->class_def /* || env->class_scope > 0 */ )
                        {
                            EM_error2( exp->where,
                                "undefined variable '%s'", S_name(exp->var) );
                            return NULL;
                        }
                        else
                        {
                            EM_error2( exp->where,
                                "undefined variable/member '%s' in class/namespace '%s'",
                                S_name(exp->var), env->class_def->base_name.c_str() );
                            return NULL;
                        }
                    }
                }

                // make sure v is legit as this point
                // but only check under certain conditions
                // 1) file-top-level variable, invoked from file-top-level only
                // 2) class-level member, invoked from pre-ctor only
                if( !v->is_decl_checked && !env->func )
                {
                    if( v->is_context_global )
                    {
                        EM_error2( exp->where,
                            "variable '%s' is used before declaration",
                            S_name(exp->var) );
                        return NULL;
                    }
                    else if( v->is_member )
                    {
                        EM_error2( exp->where,
                            "class member '%s' is used before declaration",
                            S_name(exp->var) );
                        return NULL;
                    }
                }

                // dependency tracking
                if( v->depend_init_where > 0 )
                {
                    // in a function?
                    if( env->func )
                    {
                        env->func->depends.add( Chuck_Value_Dependency( v, exp->where ) );
                    }
                    // in a class def (pre-constructor, outside of functions)?
                    else if( env->class_def )
                    {
                        env->class_def->depends.add( Chuck_Value_Dependency( v, exp->where ) );
                    }
                }

                // the type
                t = v->type;
                // remember
                exp->value = v;
            }
        break;

        // int
        case ae_primary_num:
            t = env->ckt_int;
        break;

        // float
        case ae_primary_float:
            t = env->ckt_float;
        break;

        // string
        case ae_primary_str:
            // escape the thing
            if( !escape_str( exp->str, exp->where ) )
                return NULL;

            // a string
            t = env->ckt_string;
        break;

        // char
        case ae_primary_char:
            // check escape sequences
            if( str2char( exp->chr, exp->where ) == -1 )
                return NULL;

            // a string
            t = env->ckt_int;
            break;

        // array literal
        case ae_primary_array:
            t = type_engine_check_exp_array_lit( env, exp );
        break;

        // complex literal
        case ae_primary_complex:
            t = type_engine_check_exp_complex_lit( env, exp );
        break;

        // polar literal
        case ae_primary_polar:
            t = type_engine_check_exp_polar_lit( env, exp );
        break;

        // vector literal, ge: added 1.3.5.3
        case ae_primary_vec:
            t = type_engine_check_exp_vec_lit( env, exp );
        break;

        // expression
        case ae_primary_exp:
            t = type_engine_check_exp( env, exp->exp );
        break;

        // hack
        case ae_primary_hack:
            // make sure not l-value (this should be checked in type_engine_scan1_exp_primary()
            assert( exp->exp->s_type != ae_exp_decl );
            // type check
            t = type_engine_check_exp( env, exp->exp );
        break;

        // nil (void)
        case ae_primary_nil:
            t = env->ckt_void;
        break;

        // no match
        default:
            EM_error2( exp->where,
                "internal error - unrecognized primary type '%i'", exp->s_type );
        return NULL;
    }

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_array_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // make sure
    assert( exp->s_type == ae_primary_array );

    // type
    Chuck_Type * t = NULL, * type = NULL, * common = NULL;

    // verify there are no errors from the parser...
    if( !verify_array( exp->array ) )
        return NULL;

    // verify they are of same type - do this later?
    a_Exp e = exp->array->exp_list;

    // can't be []
    if( !e )
    {
        EM_error2( exp->where,
                   "must provide values/expressions for array [...]" );
        return NULL;
    }

    // go through the array and type check each
    if( !type_engine_check_exp( env, e ) )
        return NULL;

    // loop
    while( e )
    {
        // get the type
        t = e->type;

        // compare
        if( !type )
        {
            // first
            type = t;
        }
        else
        {
            // find common ancestor
            common = type_engine_find_common_anc( t, type );
            // update type
            if( common ) type = common;
            // no common
            else
            {
                // maybe one is int and other is float
                if( isa( t, env->ckt_int ) && isa( type, env->ckt_float ) )
                {
                    // cast from int to float
                    e->cast_to = type;
                }
                else
                {
                    // incompatible
                    EM_error2( e->where, "array init [...] contains incompatible types" );
                    return NULL;
                }
            }
        }

        // next exp
        e = e->next;
    }

    // make sure
    assert( type != NULL );

    // treat static and dynamic separately
    // exp->array->is_dynamic = !is_static_array_lit( env, exp->array->exp_list );

    // create the new array type
    t = new_array_type(
        env,  // the env
        env->ckt_array,  // the array base class, usually env->ckt_array
        type->array_depth + 1,  // the depth of the new type
        type->array_depth ? type->array_type : type,  // the 'array_type'
        env->curr  // the owner namespace
    );

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_complex_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_complex_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // make sure
    assert( exp->s_type == ae_primary_complex );

    // type
    Chuck_Type * type_re = NULL, * type_im = NULL;

    // get the complex
    a_Complex val = exp->complex;

    // check if we have enough
    if( val->im == NULL )
    {
        EM_error2( exp->where,
            "missing imaginary component in complex value...\n"
            "    --> format: #(re,im)" );
        return NULL;
    }
    // check if we have too much
    if( val->im->next != NULL )
    {
        EM_error2( exp->where,
            "extraneous arguments in complex value...\n"
            "    --> format #(re,im)" );
        return NULL;
    }

    // find types (only need real, since imag is linked after real)
    type_re = type_engine_check_exp( env, val->re );
    if( !type_re ) return NULL;

    // imaginary
    type_im = val->im->type;

    // check types
    if( !isa( type_re, env->ckt_float ) && !isa( type_re, env->ckt_int ) )
    {
        EM_error2( exp->where,
            "invalid type '%s' in real component of complex value...\n"
            "    (must be of type 'int' or 'float')", type_re->c_name() );
        return NULL;
    }
    if( !isa( type_im, env->ckt_float ) && !isa( type_im, env->ckt_int ) )
    {
        EM_error2( exp->where,
            "invalid type '%s' in imaginary component of complex value...\n"
            "    (must be of type 'int' or 'float')", type_im->c_name() );
        return NULL;
    }

    // implcit cast
    if( isa( type_re, env->ckt_int ) ) val->re->cast_to = env->ckt_float;
    if( isa( type_im, env->ckt_int ) ) val->im->cast_to = env->ckt_float;

    return env->ckt_complex;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_polar_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_polar_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // make sure
    assert( exp->s_type == ae_primary_polar );

    // type
    Chuck_Type * type_mod = NULL, * type_phase = NULL;

    // get the polar
    a_Polar val = exp->polar;

    // check if we have enough
    if( val->phase == NULL )
    {
        EM_error2( exp->where,
            "missing phase component in polar value...\n"
            "    --> format: %%(mag,phase)" );
        return NULL;
    }
    // check if we have extra
    if( val->phase->next != NULL )
    {
        EM_error2( exp->where,
            "extraneous arguments in polar value...\n"
            "    --> format: %%(mag,phase)" );
        return NULL;
    }

    // find types
    type_mod = type_engine_check_exp( env, val->mod );
    // this if is here due to evaluation for if( A || B ): if A==true, B is not evaluated
    if( type_mod ) type_phase = type_engine_check_exp( env, val->phase );
    // if either pointer is NULL return NULL
    if( !type_mod || !type_phase ) return NULL;
    // was previoiusly | 1.5.0.0 (ge and eito) #chunreal
    // if( !(type_mod = type_engine_check_exp( env, val->mod )) ||
    //     !(type_phase = type_engine_check_exp( env, val->phase )) )
    //     return NULL;

    // check types
    if( !isa( type_mod, env->ckt_float ) && !isa( type_mod, env->ckt_int ) )
    {
        EM_error2( exp->where,
            "invalid type '%s' in magnitude component of polar value...\n"
            "    (must be of type 'int' or 'float')", type_mod->c_name() );
        return NULL;
    }
    if( !isa( type_phase, env->ckt_float ) && !isa( type_phase, env->ckt_int ) )
    {
        EM_error2( exp->where,
            "invalid type '%s' in phase component of polar value...\n"
            "    (must be of type 'int' or 'float')", type_phase->c_name() );
        return NULL;
    }

    // implcit cast
    if( isa( type_mod, env->ckt_int ) ) val->mod->cast_to = env->ckt_float;
    if( isa( type_phase, env->ckt_int ) ) val->phase->cast_to = env->ckt_float;

    return env->ckt_polar;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_vec_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_vec_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // make sure
    assert( exp->s_type == ae_primary_vec );

    // type
    Chuck_Type * t = NULL;

    // get the polar
    a_Vec val = exp->vec;

    // check if we have extra
    if( val->numdims > 4 )
    {
        EM_error2( exp->where,
                   "vector dimensions not supported > 4...\n"
                  "    --> format: %@(x,y,z,w)" );
        return NULL;
    }

    // the argument
    a_Exp e = val->args;
    // count
    int count = 1;
    // loop over arguments
    while( e )
    {
        // get type
        t = type_engine_check_exp(env, e);
        if( !t ) return NULL;

        // implicit cast
        if( isa( t, env->ckt_int ) ) e->cast_to = env->ckt_float;
        else if( !isa( t, env->ckt_float ) )
        {
            EM_error2( exp->where,
                      "invalid type '%s' in vector value #%d...\n"
                      "    (must be of type 'int' or 'float')", t->c_name(), count );
            return NULL;
        }
        // increment
        count++;
        // advance
        e = e->next;
    }

    // check number of arguments
    if( val->numdims < 3 )
        return env->ckt_vec2;

    // check number of arguments
    if( val->numdims < 4 )
        return env->ckt_vec3;

    // vector 4d
    return env->ckt_vec4;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_cast( Chuck_Env * env, a_Exp_Cast cast )
{
    // check the exp
    t_CKTYPE t = type_engine_check_exp( env, cast->exp );
    if( !t ) return NULL;

    // the type to cast to
    t_CKTYPE t2 = type_engine_find_type( env, cast->type->xid );
    if( !t2 )
    {
        EM_error2( cast->where, "...in cast expression" );
        return NULL;
    }

    // check if cast valid
    if( !type_engine_check_cast_valid( env, t2, t ) )
    {
        EM_error2( cast->where,
            "invalid cast from '%s' to '%s'",
            t->c_name(), S_name( cast->type->xid->xid ) );
        return NULL;
    }

    return t2;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_cast_valid()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_cast_valid( Chuck_Env * env, t_CKTYPE to, t_CKTYPE from )
{
    // down cast
    if( isa( from, to ) ) return TRUE;

    // up cast
    if( isa( to, from ) ) return TRUE;

    // TODO: dynamic type checking

    // int to float, float to int
    if( isa( to, env->ckt_float ) && isa( from, env->ckt_int ) ) return TRUE;
    if( isa( to, env->ckt_int ) && isa( from, env->ckt_float ) ) return TRUE;
    if( isa( to, env->ckt_complex ) && isa( from, env->ckt_int ) ) return TRUE;
    if( isa( to, env->ckt_polar ) && isa( from, env->ckt_int ) ) return TRUE;
    if( isa( to, env->ckt_complex ) && isa( from, env->ckt_float ) ) return TRUE;
    if( isa( to, env->ckt_polar ) && isa( from, env->ckt_float ) ) return TRUE;
    if( isa( to, env->ckt_complex ) && isa( from, env->ckt_polar ) ) return TRUE;
    if( isa( to, env->ckt_polar ) && isa( from, env->ckt_complex ) ) return TRUE;
    if( isa( to, env->ckt_vec2 ) && isa( from, env->ckt_vec3 ) ) return TRUE;
    if( isa( to, env->ckt_vec2 ) && isa( from, env->ckt_vec4 ) ) return TRUE;
    if( isa( to, env->ckt_vec3 ) && isa( from, env->ckt_vec2 ) ) return TRUE;
    if( isa( to, env->ckt_vec3 ) && isa( from, env->ckt_vec4 ) ) return TRUE;
    if( isa( to, env->ckt_vec4 ) && isa( from, env->ckt_vec2 ) ) return TRUE;
    if( isa( to, env->ckt_vec4 ) && isa( from, env->ckt_vec3 ) ) return TRUE;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dur()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_dur( Chuck_Env * env, a_Exp_Dur dur )
{
    // type check the two components
    t_CKTYPE base = type_engine_check_exp( env, dur->base );
    t_CKTYPE unit = type_engine_check_exp( env, dur->unit );

    // make sure both type check
    if( !base || !unit ) return NULL;

    // check base type
    if( !isa( base, env->ckt_int ) && !isa( base, env->ckt_float ) )
    {
        EM_error2( dur->base->where,
            "invalid type '%s' in prefix of dur expression...\n"
            "    (must be of type 'int' or 'float')", base->c_name() );
        return NULL;
    }

    // check unit type
    if( !isa( unit, env->ckt_dur ) )
    {
        EM_error2( dur->unit->where,
            "invalid type '%s' in postfix of dur expression...\n"
            "    (must be of type 'dur')", unit->c_name() );
        return NULL;
    }

    return unit;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_postfix()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix )
{
    // check the exp
    t_CKTYPE t = type_engine_check_exp( env, postfix->exp );
    if( !t ) return NULL;

    // syntax
    switch( postfix->op )
    {
        case ae_op_plusplus:
        case ae_op_minusminus:
            // assignable?
            if( postfix->exp->s_meta != ae_meta_var )
            {
                EM_error2( postfix->exp->where,
                    "postfix operator '%s' cannot be used on non-mutable data-type",
                    op2str( postfix->op ) );
                return NULL;
            }

            // TODO: mark somewhere we need to post increment

            // check type
            if( !postfix->ck_overload_func && ( isa( t, env->ckt_int ) /*|| isa( t, env->ckt_float )*/ ) )
            {
                // emit as variable instead of value
                postfix->exp->emit_var = TRUE;
                return t;
            }
        break;

        default:
            // no match
            EM_error2( postfix->where,
                "internal compiler error: unrecognized postfix '%i'", postfix->op );
        return NULL;
    }

    // check overloading of postfix operator | 1.5.1.5 (ge) added
    Chuck_Type * ret = type_engine_check_op_overload_postfix( env, t, postfix->op, postfix );
    // if we have a hit
    if( ret ) return ret;

    // no match
    EM_error2( postfix->where,
        "cannot resolve postfix operator '%s' on type '%s'",
        op2str( postfix->op ), t->c_name() );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_if( Chuck_Env * env, a_Exp_If exp_if )
{
    // check the components
    t_CKTYPE cond = type_engine_check_exp( env, exp_if->cond );
    t_CKTYPE if_exp = type_engine_check_exp( env, exp_if->if_exp );
    t_CKTYPE else_exp = type_engine_check_exp( env, exp_if->else_exp );

    // make sure everything good
    if( !cond || !if_exp || !else_exp )
    {
        EM_error2( exp_if->where,
                   "type system (internal) error determining types in IF expression" );
        return NULL;
    }

    // check the type
    if( !isa( cond, env->ckt_int ) )
    {
        EM_error2( exp_if->where,
                   "non-integer conditional type '%s' in IF expression...", cond->base_name.c_str() );
        EM_error2( exp_if->where,
                   "...(note: check order of operation precedence)" );
        return NULL;
    }

    // make sure the if and else have compatible types
    // TODO: the lesser of two types
    if( !( *if_exp == *else_exp ) )
    {
        EM_error2( exp_if->where,
            "incompatible types '%s' and '%s' in if expression",
            if_exp->c_name(), else_exp->c_name() );
        return NULL;
    }

    return if_exp;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_array_subscripts( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_array_subscripts( Chuck_Env * env, a_Exp exp_list )
{
    a_Exp exp = exp_list;

    // loop through
    while( exp )
    {
        // if not int
        if( !isa( exp->type, env->ckt_int ) )
        {
            EM_error2( exp->where,
                "incompatible array subscript type '%s'",
                exp->type->base_name.c_str() );
            return FALSE;
        }

        exp = exp->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_decl_part1()
// desc: deferred from type_engine_scan2_exp_decl()
//       reason: 'auto' needs more context before it can processed | 1.5.0.8 (ge)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_exp_decl_part1( Chuck_Env * env, a_Exp_Decl decl )
{
    // try to create the decl (this should return if decl already created, or
    // produce error if decl type is auto and is still not resolved)
    return type_engine_scan2_exp_decl_create( env, decl );
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_decl_part2()
// desc: check variable declaration(s) expression
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_decl_part2( Chuck_Env * env, a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;
    Chuck_Type * type = NULL;
    Chuck_Value * value = NULL;
    t_CKBOOL is_obj = FALSE;
    t_CKBOOL is_ref = FALSE;
    // t_CKBOOL is_ref2 = FALSE;

    // loop through the variables
    while( list != NULL )
    {
        // get the decl
        var_decl = list->var_decl;

        // (if at class_scope) | TODO: sort
        if( env->class_def && env->class_scope == 0 )
        {
            // check if in parent
            value = type_engine_find_value( env->class_def->parent, var_decl->xid );
            if( value )
            {
                EM_error2( var_decl->where,
                    "'%s' has already been defined in super class '%s'",
                    S_name(var_decl->xid), value->owner_class->c_name() );
                return NULL;
            }
        }

        // get the value
        value = var_decl->value;
        // make sure
        assert( value != NULL );
        // get the type
        type = value->type;
        // make sure
        assert( type != NULL );
        // is object
        is_obj = isobj( env, type );
        // is ref
        is_ref = var_decl->ref;
        // 1.4.2.0 (ge) should it be this?? is_ref = var_decl->ref; (and not decl->type->ref?)
        // 1.4.2.0 (ge, later) yes, it should be var_decl->ref, because var_decl is per declaration...
        // e.g., float a[], b -- the entire line is a decl whereas a and b are individual var_decls

        // if instantiating an object
        if( is_obj )
        {
            // if in a function definition
            if( env->func )
            {
                // dependency tracking: add the callee's dependencies
                env->func->depends.add( &type->depends );
            }
            else if( env->class_def ) // in a class definition
            {
                // dependency tracking: add the callee's dependencies
                env->class_def->depends.add( &type->depends );
            }
        }

        // check for constructor args | 1.5.2.0 (ge) added
        // NOTE empty () is handled elsewhere as default constructor
        if( var_decl->ctor.args != NULL )
        {
            // type check and get constructor function
            var_decl->ctor.func = type_engine_check_ctor_call( env, type, &var_decl->ctor, var_decl->array, var_decl->where );
            // check for error
            if( !var_decl->ctor.func )
                return NULL;
        }

        // if array, then check to see if empty []
        if( var_decl->array && var_decl->array->exp_list != NULL )
        {
            // instantiate object, including array
            if( !type_engine_check_exp( env, var_decl->array->exp_list ) )
                return NULL;

            // check the subscripts
            if( !type_engine_check_array_subscripts( env, var_decl->array->exp_list ) )
                return NULL;
        }

        // member?
        if( value->is_member )
        {
            // offset
            value->offset = env->curr->offset;
            // if at class_scope and is object
            if( is_obj )
            {
                // cerr << "adding: " << value->name << " : " << value->offset << endl;
                // add it to the class | 1.5.2.0 (ge)
                env->class_def->obj_mvars_offsets.push_back( value->offset );
            }

            /*******************************************************************
             * spencer: added this into function to provide the same logic path
             * for type_engine_check_exp_decl() and ck_add_mvar() when they
             * determine offsets for mvars
             ******************************************************************/
            // move the offset (TODO: check the size)
            env->curr->offset = type_engine_next_offset( env->curr->offset, type );
            // env->curr->offset += type->size;
        }
        else if( decl->is_static ) // static
        {
            // base scope
            if( env->class_def == NULL || env->class_scope > 0 )
            {
                EM_error2( decl->where,
                    "static variables must be declared at class scope" );
                return NULL;
            }

            // flag
            value->is_static = TRUE;
            // offset
            value->offset = env->class_def->info->class_data_size;
            // move the size
            env->class_def->info->class_data_size += type->size;

            // if this is an object
            if( is_obj && !is_ref )
            {
                // for now - no good for static, since we need separate
                // initialization which we don't have
                EM_error2( var_decl->where,
                    "cannot declare static non-primitive objects (yet)..." );
                EM_error2( 0,
                    "...(hint: declare as reference (@) & initialize outside class for now)" );
                return NULL;
            }
        }
        else // local variable
        {
            // do nothing?
        }

        // mark the decl checked (see scan pass 2)
        value->is_decl_checked = TRUE;

        // add the value, if we are not at class scope
        // (otherwise they should already have been added)
        if( !env->class_def || env->class_scope > 0 )
        {
            // add as value
            env->curr->value.add( var_decl->xid, value );
        }

        // the next var decl
        list = list->next;
    }

    return decl->ck_type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_decl( )
// desc: check variable declaration(s) expression
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_decl( Chuck_Env * env, a_Exp_Decl decl )
{
    // deferred scan2 | 1.5.0.8 (ge)
    if( !type_engine_check_exp_decl_part1( env, decl ) )
        return NULL;

    // type check pass | 1.5.0.8 (ge)
    if( !type_engine_check_exp_decl_part2( env, decl ) )
        return NULL;

    return decl->ck_type;
}




//-----------------------------------------------------------------------------
// name: type_engine_print_exp_dot_member()
// desc: print a dot_member exp, e.g., Foo.bar
//-----------------------------------------------------------------------------
string type_engine_print_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member )
{
    // UNUSED: Chuck_Value * value = NULL;
    Chuck_Type * the_base = NULL;
    t_CKBOOL base_static = FALSE;
    string str;

    // type check the base
    member->t_base = type_engine_check_exp( env, member->base );
    if( !member->t_base ) return "[error]";

    // is the base a class/namespace or a variable | modified 1.5.0.0 (ge)
    base_static = type_engine_is_base_static( env, member->t_base );
    // base_static = isa( member->t_base, env->ckt_class );

    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;

    // this
    str = S_name(member->xid);

    return string(the_base->c_name()) + std::string(".") + str;
}




//-----------------------------------------------------------------------------
// name: struct NonspecificFuncMatch | 1.5.2.0 (ge) added
// desc: struct to store a func with a match score
//-----------------------------------------------------------------------------
struct NonspecificFuncMatch
{
    // the func
    Chuck_Func * func;
    // match score
    t_CKINT matchScore;
    // constructor
    NonspecificFuncMatch( Chuck_Func * f, t_CKINT score )
    : func(f), matchScore(score) { }
};




//-----------------------------------------------------------------------------
// name: find_best_nonspecifc_match()
// desc: check nonspecific function matches
//-----------------------------------------------------------------------------
Chuck_Func * find_best_nonspecifc_match( Chuck_Func * f, vector<NonspecificFuncMatch> & nonspecifics, uint32_t where )
{
    // empty vector
    if( nonspecifics.size() == 0 ) return NULL;

    t_CKINT min = CK_INT_MAX, minIndex = 0;
    // ambiguous funcs
    vector<Chuck_Func *> ambiguous;

    // find the func with the lowest score
    for( t_CKUINT i = 0; i < nonspecifics.size(); i++ )
    {
        // cerr << "nonspecific: " << nonspecifics[i].func->signature(FALSE,FALSE) << " " << nonspecifics[i].matchScore << endl;
        // compare
        if( nonspecifics[i].matchScore < min )
        {
            min = nonspecifics[i].matchScore;
            minIndex = i;
        }
    }

    // next, make sure the min score is unique
    for( t_CKUINT i = 0; i < nonspecifics.size(); i++ )
    {
        // compare (including the min)
        if( nonspecifics[i].matchScore == min )
        {
            // add to list of ambiguous
            ambiguous.push_back( nonspecifics[i].func );
        }
    }

    // see if we have any ambiguities beyond the guaranteed match
    if( ambiguous.size() > 1 )
    {
        // error
        EM_error2( where, "call to '%s' is ambiguous...", f->base_name.c_str() );
        // print candidates
        for( t_CKUINT i = 0; i < ambiguous.size(); i++ )
        {
            EM_error3( "candidate function:" );
            EM_error3b( "    %s { ... }\n", ambiguous[i]->signature(FALSE,FALSE).c_str() );
        }

        return NULL;
    }

    // return the func
    return nonspecifics[minIndex].func;
}




//-----------------------------------------------------------------------------
// name: find_func_match_actual()
// desc: match a function by arguments, with options
//-----------------------------------------------------------------------------
Chuck_Func * find_func_match_actual( Chuck_Env * env, Chuck_Func * up, a_Exp args,
                                     t_CKBOOL implicit, t_CKBOOL specific, t_CKBOOL & hasError,
                                     uint32_t where )
{
    a_Exp e = NULL;
    a_Arg_List e1 = NULL;
    t_CKUINT count = 0;
    Chuck_Func * theFunc = NULL;
    t_CKBOOL match = FALSE;
    // non-specific match score (only for specifc==FALSE) | 1.5.2.0
    t_CKINT matchScore = 0;
    // vector of non-specific matches for a particular class (not including parent or children)
    vector<NonspecificFuncMatch> nonspecifics;

    // reset error flag | 1.5.2.0
    hasError = FALSE;
    // see if args is nil
    if( args && args->type == env->ckt_void )
        args = NULL;

    // up is the list of functions in single class/namespace
    while( up )
    {
        // set the function
        theFunc = up;
        // clear the non-specific stuff | 1.5.2.0
        if( !specific ) nonspecifics.clear();

        // loop
        while( theFunc )
        {
            e = args;
            e1 = theFunc->def()->arg_list;
            count = 1;
            matchScore = 0;

            // check arguments against the definition
            while( e )
            {
                // check for extra arguments
                if( e1 == NULL ) goto moveon;

                // get match
                match = specific ? equals(e->type,e1->type) : isa(e->type,e1->type);
                // see
                if( !specific )
                {
                    // if a strict super-type match
                    if( match && !equals(e->type,e1->type) )
                    {
                        t_CKUINT levels = 0;
                        // get levels of inheritance between two types
                        isa_levels( *e->type, *e1->type, levels );
                        // update score
                        matchScore += levels;
                    }
                }

                // no match
                if( !match )
                {
                    // TODO: fix this for overload implicit cast (multiple matches)
                    if( implicit && e->type == env->ckt_int && e1->type == env->ckt_float )
                    {
                        // int to float
                        e->cast_to = env->ckt_float;
                    }
                    else
                    {
                        // type mismatch; move on
                        goto moveon;
                    }
                }

                e = e->next;
                e1 = e1->next;
                count++;
            }

            // check for extra arguments
            if( e1 == NULL )
            {
                if( !specific && matchScore > 0 )
                {
                    // push back and keep going to the next function in the namespace...
                    nonspecifics.push_back( NonspecificFuncMatch( theFunc, matchScore ) );
                }

                // if we have no nonspecifics at this point, good to return
                if( nonspecifics.size() == 0 ) return theFunc;
            }

moveon:
            // next func
            theFunc = theFunc->next;
        }

        // if at least one non-specific match, find the best
        // if there was an ambiguity or another issue, will print inside find_best_()
        if( nonspecifics.size() )
        {
            // find best match
            Chuck_Func * theMatch = find_best_nonspecifc_match( up, nonspecifics, where );
            // if no match, set error
            if( !theMatch ) hasError = TRUE;
            // return the result
            return theMatch;
        }

        // go up to parent class/namespace
        if( up->up ) up = up->up->func_ref;
        else up = NULL;
    }

    // not found
    return NULL;
}




//-----------------------------------------------------------------------------
// name: find_func_match()
// desc: match a function by arguments
//-----------------------------------------------------------------------------
Chuck_Func * find_func_match( Chuck_Env * env, Chuck_Func * up, a_Exp args, t_CKBOOL & hasError, uint32_t where )
{
    Chuck_Func * theFunc = NULL;

    // try to find specific
    theFunc = find_func_match_actual( env, up, args, FALSE, TRUE, hasError, where );
    if( theFunc || hasError ) return theFunc;

    // try to find specific with implicit
    theFunc = find_func_match_actual( env, up, args, TRUE, TRUE, hasError, where );
    if( theFunc || hasError ) return theFunc;

    // try to find non-specific
    theFunc = find_func_match_actual( env, up, args, FALSE, FALSE, hasError, where );
    if( theFunc || hasError ) return theFunc;

    // try to find non-specific with implicit
    theFunc = find_func_match_actual( env, up, args, TRUE, FALSE, hasError, where );
    if( theFunc || hasError ) return theFunc;

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call()
// desc: type check function call
//       (RELATED: type_engine_check_ctor_call())
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                          t_CKFUNC & ck_func, int linepos )
{
    Chuck_Func * theFunc = NULL;
    Chuck_Func * up = NULL;

    // type check the func
    t_CKTYPE f = exp_func->type = type_engine_check_exp( env, exp_func );
    if( !f ) return NULL;

    // void type for args
    t_CKTYPE a = env->ckt_void;

    // make sure not 'null'
    if( isnull(env, f) )
    {
        EM_error2( exp_func->where,
            "function call using a non-function value 'null'" );
        return NULL;
    }

    // make sure we have a function
    if( !isa( f, env->ckt_function ) )
    {
        EM_error2( exp_func->where,
            "function call using a non-function value" );
        return NULL;
    }

    // copy the func
    up = f->func;

    // check the arguments
    if( args )
    {
        a = type_engine_check_exp( env, args );
        if( !a ) return NULL;
    }

    // look for a match
    t_CKBOOL hasError = FALSE;
    theFunc = find_func_match( env, up, args, hasError, exp_func->where );

    // no func
    if( !theFunc )
    {
        // hasError implies an error has already been printed
        if( hasError ) return NULL;

        // if primary
        if( exp_func->s_type == ae_exp_primary && exp_func->primary.s_type == ae_primary_var )
        {
            EM_error2( exp_func->where,
                "argument type(s) do not match...\n...for function '%s(...)'...",
                S_name(exp_func->primary.var) );
        }
        else if( exp_func->s_type == ae_exp_dot_member )
        {
            EM_error2( exp_func->where,
                "argument type(s) do not match...\n...for function '%s(...)'...",
                type_engine_print_exp_dot_member( env, &exp_func->dot_member ).c_str() );
        }
        else
        {
            EM_error2( exp_func->where,
                "argument type(s) do not match for function..." );
        }

        EM_error2( 0,
            "...(please check the argument types)" );

        return NULL;
    }

    // recheck the type with new name
    if( exp_func->s_type == ae_exp_primary && exp_func->primary.s_type == ae_primary_var )
    {
        // set the new name
        // TODO: clear old
        exp_func->primary.var = insert_symbol(theFunc->name.c_str());
        // make sure the type is still the name
        if( *exp_func->type != *type_engine_check_exp( env, exp_func ) )
        {
            // error
            EM_error2( exp_func->where,
                "(internal error) function type different on second check" );
            return NULL;
        }
    }
    else if( exp_func->s_type == ae_exp_dot_member )
    {
        // set the new name
        // TODO: clear old
        exp_func->dot_member.xid = insert_symbol(theFunc->name.c_str());
        /*
        // TODO: figure if this is necessary - it type checks things twice!
        // make sure the type is still the name
        if( *exp_func->type != *type_engine_check_exp( env, exp_func ) )
        {
            // error
            EM_error2( exp_func->where,
                "(internal error) function type different on second check" );
            return NULL;
        }
        */
    }
    else assert( FALSE );

    // copy ck_func out (return by reference)
    ck_func = theFunc;

    // if sporking, then don't track dependencies...
    // up to the programmer to ensure correctness across spork and time
    if( !env->sporking )
    {
        // if in a function definition
        if( env->func )
        {
            // dependency tracking: add the callee's dependencies
            env->func->depends.add( &ck_func->depends );
        }
        else if( env->class_def ) // in a class definition
        {
            // dependency tracking: add the callee's dependencies
            env->class_def->depends.add( &ck_func->depends );
        }
    }

    return theFunc->def()->ret_type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call )
{
    // type check it
    return type_engine_check_exp_func_call( env, func_call->func, func_call->args,
                                            func_call->ck_func, func_call->where );
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dot_member_special()
// desc: check special case for complex, polar, vec3, vec4 (ge) 1.3.5.3
//       add support for vec2 (ge) 1.5.1.7
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_dot_member_special( Chuck_Env * env, a_Exp_Dot_Member member )
{
    // if complex or polar
    if( member->t_base->xid == te_complex ) // TODO: constants!!!
    {
        // get as string
        string str = S_name(member->xid);
        // verify member is either re or im
        if( str == "re" || str == "im" )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->where,
                          "cannot assign value to literal complex value" );
                return NULL;
            }

            return env->ckt_float;
        }
        else
        {
            // not valid
            EM_error2( member->where,
                      "type '%s' has no member named '%s'", member->t_base->c_name(), str.c_str() );
            return NULL;
        }
    }
    else if( member->t_base->xid == te_polar )
    {
        // get as string
        string str = S_name(member->xid);
        // verify member is either re or im
        if( str == "mag" || str == "phase" )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->where,
                          "cannot assign value to literal polar value" );
                return NULL;
            }

            return env->ckt_float;
        }
        else
        {
            // not valid
            EM_error2( member->where,
                      "type '%s' has no member named '%s'", member->t_base->c_name(), str.c_str() );
            return NULL;
        }
    }
    // vec2
    else if( member->t_base->xid == te_vec2 )
    {
        // get as string
        string str = S_name(member->xid);
        // verify member is either re or im
        if( str == "x" || str == "y" || str == "u" || str == "v" || str == "s" || str == "t" )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->where,
                          "cannot assign value to literal vec2 value" );
                return NULL;
            }

            return env->ckt_float;
        }
        else
        {
            // not valid
            EM_error2( member->where,
                      "type '%s' has no member named '%s'", member->t_base->c_name(), str.c_str() );
            return NULL;
        }
    }
    else if( member->t_base->xid == te_vec3 || member->t_base->xid == te_vec4 )
    {
        // get as string
        string str = S_name(member->xid);
        // verify member is either re or im
        if( str == "x" || str == "y" || str == "z" ||
            str == "r" || str == "g" || str == "b" ||
            str == "value" || str == "goal" || str == "slew" )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->where,
                           "cannot assign value to literal %s value",
                           member->t_base->c_name() );
                return NULL;
            }

            return env->ckt_float;
        }
        // vec4 only
        else if( member->t_base->xid == te_vec4 && (str == "w" || str == "a") )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->where,
                           "cannot assign value to literal %s value",
                           member->t_base->c_name() );
                return NULL;
            }

            return env->ckt_float;
        }
        else
        {
            // check function
            goto check_func;
        }
    }

    // should not get here
    EM_error2( member->base->where,
               "type checer internal error in special literal" );

    return NULL;

check_func:

    Chuck_Value * value = NULL;
    Chuck_Type * the_base = NULL;
    t_CKBOOL base_static = FALSE;

    // is the base a class/namespace or a variable | 1.5.0.0 (ge) modified to call
    base_static = type_engine_is_base_static( env, member->t_base );
    // base_static = isa( member->t_base, env->ckt_class );
    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;

    // check base; 1.3.5.3
    if( member->base->s_meta == ae_meta_value ) // is literal
    {
        // error
        EM_error2( member->base->where,
                  "cannot call function from literal %s value",
                  member->t_base->c_name() );
        return NULL;
    }

    // find the value
    value = type_engine_find_value( the_base, member->xid );
    if( !value )
    {
        // can't find member
        EM_error2( member->where,
                  "class '%s' has no member '%s'",
                  the_base->c_name(), S_name(member->xid) );
        return NULL;
    }

    return value->type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dot_member()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member )
{
    Chuck_Value * value = NULL;
    Chuck_Type * the_base = NULL;
    t_CKBOOL base_static = FALSE;
    string str;

    // type check the base
    member->t_base = type_engine_check_exp( env, member->base );
    if( !member->t_base ) return NULL;

    // check base type, ge: added 1.3.5.3
    t_CKUINT tbase_xid = (t_CKUINT)member->t_base->xid;
    switch( tbase_xid )
    {
        case te_complex:
        case te_polar:
        case te_vec2:
        case te_vec3:
        case te_vec4:
            return type_engine_check_exp_dot_member_special( env, member );
            break;
    }

    // is the base a class/namespace or a variable
    base_static = type_engine_is_base_static( env, member->t_base );
    // base_static = isa( member->t_base, env->ckt_class )
    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;

    // have members?
    if( !the_base->info )
    {
        // base type does not have members
        EM_error2( member->base->where,
            "type '%s' does not have members - invalid use in dot expression",
            the_base->c_name() );
        return NULL;
    }

    // this
    str = S_name(member->xid);
    if( str == "this" )
    {
        // uh
        if( base_static )
        {
            EM_error2( member->where,
                "keyword 'this' must be associated with object instance" );
            return NULL;
        }
        // in member func
        if( env->func && !env->func->is_member )
        {
            EM_error2( member->where,
                "keyword 'this' cannot be used inside static functions" );
            return NULL;
        }

        return env->class_def;
    }

    // find the value
    value = type_engine_find_value( the_base, member->xid );
    if( !value )
    {
        // can't find member
        EM_error2( member->where,
                   "class '%s' has no member '%s'",
                   the_base->c_name(), S_name(member->xid) );
        return NULL;
    }

    // make sure
    if( base_static && value->is_member )
    {
        // this won't work
        EM_error2( member->where,
            "cannot access member '%s.%s' without object instance",
            the_base->c_name(), S_name(member->xid) );
        return NULL;
    }

    return value->type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_array( Chuck_Env * env, a_Exp_Array array )
{
    // verify there are no errors from the parser...
    if( !verify_array( array->indices ) )
        return NULL;

    // type check the base
    t_CKTYPE t_base = type_engine_check_exp( env, array->base );
    if( !t_base ) return NULL;

    // dimension
    if( array->indices->depth > t_base->array_depth )
    {
        EM_error2( array->where,
            "array subscripts (%i) exceeds defined dimension (%i)",
            array->indices->depth, t_base->array_depth );
        return NULL;
    }

    // type check the index
    t_CKTYPE t_index = type_engine_check_exp( env, array->indices->exp_list );
    if( !t_index ) return NULL;

    // cycle through each exp
    a_Exp e = array->indices->exp_list;
    // count the dimension
    t_CKUINT depth = 0;

    while( e )
    {
        // increment
        depth++;
        // check if index is of valid type
        if( !isa( e->type, env->ckt_int ) && !isa( e->type, env->ckt_string ) )
        {
            // not int or string
            EM_error2( e->where,
                "array index %i must be of type 'int' or 'string', not '%s'",
                depth, e->type->c_name() );
            return NULL;
        }

        // advance the list
        e = e->next;
    }
    // TODO: Catch commas inside array subscripts earlier on, perhaps in .y
    if( array->indices->depth != depth )
    {
        a_Exp e2 = array->indices->exp_list;
        EM_error2( e2 ? e2->where : array->where,
                   "[..., ...] is invalid subscript syntax." );
        return NULL;
    }
    t_CKTYPE t = NULL;
    // make sure depth <= max
    if( depth == t_base->array_depth )
    {
        // the original type
        t = array->base->type->array_type;
    }
    else
    {
        // partial; context added in 1.5.1.1
        t = array->base->type->copy( env, env->context );
        // remainder
        t->array_depth -= depth;
    }

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_class_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_class_def( Chuck_Env * env, a_Class_Def class_def )
{
    // make new type for class def
    t_CKTYPE the_class = class_def->type;
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;

    // check if parent class definition is complete or not
    // NOTE this could potentially be remove if class defs can be processed
    // out of order they appear in file; potentially a relationship tree?
    if( the_class->parent->is_complete == FALSE )
    {
        EM_error2( class_def->ext->where,
            "cannot extend incomplete type '%s'",
            the_class->parent->c_name() );
        EM_error2( class_def->ext->where,
            "...(note: the parent's declaration must precede child's)" );
        // done
        return FALSE;
    }

    // NB the following should be done AFTER the parent is completely defined
    // --
    // set the beginning of data segment to after the parent
    the_class->info->offset = the_class->parent->obj_size;
    // duplicate the parent's virtual table
    the_class->info->obj_v_table = the_class->parent->info->obj_v_table;

    // set the new type as current
    env->nspc_stack.push_back( env->curr );
    env->curr = the_class->info;
    // push the class def
    env->class_stack.push_back( env->class_def );
    env->class_def = the_class;
    // reset the nest list
    env->class_scope = 0;

    // type check the body
    while( body && ret )
    {
        // check the section
        switch( body->section->s_type )
        {
        case ae_section_stmt:
            // flag as having a constructor
            env->class_def->has_pre_ctor |= (body->section->stmt_list->stmt != NULL);
            ret = type_engine_check_stmt_list( env, body->section->stmt_list );
            break;

        case ae_section_func:
            // set to complete
            env->class_def->is_complete = TRUE;
            ret = type_engine_check_func_def( env, body->section->func_def );
            // back
            env->class_def->is_complete = FALSE;
            break;

        case ae_section_class:
            // check it
            ret = type_engine_check_class_def( env, body->section->class_def );
            break;
        }

        // move to the next section
        body = body->next;
    }

    // pop the class
    env->class_def = env->class_stack.back();
    env->class_stack.pop_back();
    // pop the namesapce
    env->curr = env->nspc_stack.back();
    env->nspc_stack.pop_back();

    // if ok
    if( ret )
    {
        // set the object size
        the_class->obj_size = the_class->info->offset;
        // set complete
        the_class->is_complete = TRUE;
    }
    else // if not ok | if( !ret )
    {
        // delete the class definition
        CK_SAFE_RELEASE( class_def->type );
        // set the thing to NULL
        the_class = NULL;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_func_def()
// desc: type check function definition
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_func_def( Chuck_Env * env, a_Func_Def f )
{
    Chuck_Value * value = NULL;
    Chuck_Func * theFunc = NULL;

    Chuck_Type * parent = NULL;
    Chuck_Value * theOverride = NULL;
    Chuck_Value * v = NULL;
    Chuck_Func * parent_func = NULL;
    a_Arg_List arg_list = NULL;
    t_CKBOOL parent_match = FALSE;
    string func_name = S_name(f->name);
    t_CKUINT count = 0;
    t_CKBOOL has_code = FALSE;  // use this for both user and imported

    // see if we are already in a function definition
    if( env->func != NULL )
    {
        EM_error2( f->where,
            "nested function definitions are not (yet) allowed" );
        return FALSE;
    }

    // get the func
    theFunc = f->ck_func;
    // get the value
    value = theFunc->value_ref;
    // make sure
    assert( theFunc != NULL && value != NULL );

    // check
    if( env->class_def )
    {
        // look up the value in the parent class
        theOverride = type_engine_find_value( env->class_def->parent, f->name );
        // check if override
        if( theOverride )
        {
            // see if the target is a function
            if( !isa( theOverride->type, env->ckt_function ) )
            {
                EM_error2( f->where, "function name '%s' conflicts with previously defined value...",
                    S_name(f->name) );
                EM_error2( f->where, "from super class '%s'...", theOverride->owner_class->c_name() );
                return FALSE;
            }
        }
    }

    // make sure a code segment is in stmt - else we should push scope
    assert( !f->code || f->code->s_type == ae_stmt_code );

    // if override
    if( theOverride )
    {
        // make reference to parent
        theFunc->up = theOverride;
        // ref count | 1.5.2.0 (ge) added
        CK_SAFE_ADD_REF( theFunc->up );
    }

    // make sure return type is not NULL
    assert( f->ret_type != NULL );

    // only class functions can be pure
    if( !env->class_def && f->static_decl == ae_key_abstract )
    {
        EM_error2( f->where, "non-class function cannot be declared as 'pure'..." );
        EM_error2( f->where, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // figure out if the function has code associated with it
    if( f->s_type == ae_func_user ) has_code = ( f->code != NULL );
    else has_code = (f->dl_func_ptr != NULL); // imported

//    // if interface, then cannot have code | a_Class_Def class_def->def removed for now
//    if( env->class_def && env->class_def->def && env->class_def->def->iface && has_code )
//    {
//        EM_error2( f->where, "interface function signatures cannot contain code..." );
//        EM_error2( f->where, "...at function '%s'", S_name(f->name) );
//        goto error;
//    }

    // if pure, then cannot have code
    if( f->static_decl == ae_key_abstract && has_code )
    {
        EM_error2( f->where, "'pure' function signatures cannot contain code..." );
        EM_error2( f->where, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // yeah
    if( f->static_decl != ae_key_abstract && !has_code )
    {
        EM_error2( f->where, "function declaration must contain code..." );
        EM_error2( f->where, "(unless in interface, or is declared 'pure')" );
        EM_error2( f->where, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // if overriding super class function, then check signatures
    if( env->class_def )
    {
        // get parent
        parent = env->class_def->parent;
        while( parent && !parent_match )
        {
            v = type_engine_find_value( env->class_def->parent, f->name );
            if( v )
            {
                // see if the target is a function
                if( !isa( v->type, env->ckt_function ) )
                {
                    EM_error2( f->where, "function name '%s' conflicts with previously defined value...",
                        S_name(f->name) );
                    EM_error2( f->where, "from super class '%s'...", v->owner_class->c_name() );
                    goto error;
                }

                // parent func
                parent_func = v->func_ref;

                // go through all overloading
                while( parent_func && !parent_match )
                {
                    // match the prototypes
                    string err;
                    if( !type_engine_compat_func( f, parent_func->def(), f->where, err, FALSE ) )
                    {
                        // next
                        parent_func = parent_func->next;
                        // move on
                        continue;
                    }
                    /*{
                        EM_error2( f->where,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            value->owner_class->c_name(), S_name(f->name) );
                        if( err != "" ) EM_error2( f->where, "...(reason: %s)", err.c_str() );
                        goto error;
                    }*/

                    // see if parent function is static
                    if( parent_func->is_static )
                    {
                        EM_error2( f->where,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->where,
                            "...(reason: '%s.%s' is declared as 'static')",
                            v->owner_class->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // see if function is static
                    if( f->static_decl == ae_key_static )
                    {
                        EM_error2( f->where,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->where,
                            "...(reason: '%s.%s' is declared as 'static')",
                            env->class_def->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // see if function is pure
                    if( f->static_decl == ae_key_abstract )
                    {
                        EM_error2( f->where,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->where,
                            "...(reason: '%s.%s' is declared as 'pure')",
                            env->class_def->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // make sure returns are equal
                    if( *(f->ret_type) != *(parent_func->def()->ret_type) )
                    {
                        EM_error2( f->where, "function signatures differ in return type..." );
                        EM_error2( f->where,
                            "function '%s.%s' matches '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            v->owner_class->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // match
                    parent_match = TRUE;

                    // update virtual table
                    theFunc->vt_index = parent_func->vt_index;
                    assert( theFunc->vt_index < env->curr->obj_v_table.funcs.size() );
                    env->curr->obj_v_table.funcs[theFunc->vt_index] = theFunc;
                    // update name
                    func_name = parent_func->name;
                    theFunc->name = func_name;
                    value->name = func_name;
                }
            }

            // move to next parent
            parent = parent->parent;
        }
    }

    // if not override, make entry in virtual table
    if( theFunc->is_member && !parent_match )
    {
        // remember virtual table index
        theFunc->vt_index = env->curr->obj_v_table.funcs.size();
        // append to virtual table
        env->curr->obj_v_table.funcs.push_back( theFunc );
    }

    // set the current function to this
    env->func = theFunc;
    // push the value stack
    env->curr->value.push();

    // look up types for the function arguments
    arg_list = f->arg_list;
    // count
    count = 1;
    // loop over arguments
    while( arg_list )
    {
        // make sure type != NULL
        assert( arg_list->type != NULL );
        // get the value
        v = arg_list->var_decl->value;
        // make sure value != NULL
        assert( v != NULL );

        // look up in scope: later
        if( env->curr->lookup_value( arg_list->var_decl->xid, FALSE ) )
        {
            EM_error2( arg_list->where, "in function '%s':", theFunc->signature(FALSE,FALSE).c_str() ); // S_name(f->name) );
            EM_error2( arg_list->where, "argument %i '%s' is already defined in this scope",
                count, S_name(arg_list->var_decl->xid) );
            goto error;
        }

        // add as value
        env->curr->value.add( arg_list->var_decl->xid, v );

        // increment count
        count++;
        // next arg
        arg_list = arg_list->next;
    }

    // type check the code
    assert( f->code == NULL || f->code->s_type == ae_stmt_code );
    if( f->code && !type_engine_check_code_segment( env, &f->code->stmt_code, FALSE ) )
    {
        EM_error2( 0, "...in function '%s'", theFunc->signature(FALSE).c_str() ); // S_name(f->name) );
        goto error;
    }

    // check whether all control paths return
    if( f->ret_type && !isa(f->ret_type, env->ckt_void) &&
        f->code && !f->code->stmt_code.allControlPathsReturn )
    {
        EM_error2( f->type_decl->where, "not all control paths in '%s' return a value",
                   theFunc->signature(TRUE).c_str() ); // S_name(f->name) );
        goto error;
    }

    // if imported, add the stack depth
    if( f->s_type == ae_func_builtin )
    {
        // set the stack depth?
        theFunc->code->stack_depth = f->stack_depth;
        // if member add room for this
        // done: if( theFunc->is_member ) theFunc->code->stack_depth += sizeof(t_CKUINT);
    }

    // pop the value stack
    env->curr->value.pop();

    // clear the env's function definition
    env->func = NULL;

    return TRUE;

error:

    // clean up
    env->func = NULL;
    // theFunc->release();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: Chuck_Namespace()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Namespace::Chuck_Namespace()
{
    pre_ctor = NULL;
    pre_dtor = NULL;
    parent = NULL;
    offset = 0;
    class_data = NULL;
    class_data_size = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Namespace()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Namespace::~Chuck_Namespace()
{
    // release references
    CK_SAFE_RELEASE( pre_ctor );
    CK_SAFE_RELEASE( pre_dtor );
    // TODO: release ref
    // CK_SAFE_RELEASE( this->parent );
}




//-----------------------------------------------------------------------------
// name: lookup_value()
// desc: lookup value in the env; climb means to climb the scope and...
//       climb the namespace (but the latter only if currently in classdef
//       and !stayWithinClassDef; this is to be able to implement search
//       up scope within a class def, but not beyond the class def, so that
//       inherited members could be priortized over global-scope variables
//       of the same name | 1.5.1.3 (ge) added
//       see unit tests:
//           test/01-Basic/194-value-lookup-order.ck
//           test/01-Basic/195-value-lookup-order.ck
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Namespace::lookup_value( const string & theName, t_CKINT climb,
                                             t_CKBOOL stayWithinClassDef )
{
    // call the Symbol version
    return lookup_value( insert_symbol(theName.c_str()), climb, stayWithinClassDef );
}




//-----------------------------------------------------------------------------
// name: lookup_value()
// desc: lookup value in the env
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Namespace::lookup_value( S_Symbol theName, t_CKINT climb,
                                             t_CKBOOL stayWithinClassDef )
{
    // look up in current namespace
    Chuck_Value * v = value.lookup( theName, climb );

    // respect stayWithinClassDef; check if we are in class def using pre_ctor
    t_CKBOOL keepGoing = ( this->pre_ctor && stayWithinClassDef ) == FALSE;
    // climb up to parent namespace
    if( climb > 0 && !v && parent && keepGoing )
        return parent->lookup_value( theName, climb, stayWithinClassDef );
    return v;
}




//-----------------------------------------------------------------------------
// name: lookup_type()
// desc: lookup type in the env
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Namespace::lookup_type( const string & theName, t_CKINT climb,
                                           t_CKBOOL stayWithinClassDef )
{
    // call the Symbol version
    return lookup_type( insert_symbol(theName.c_str()), climb, stayWithinClassDef );
}




//-----------------------------------------------------------------------------
// name: lookup_type()
// desc: lookup type in the env
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Namespace::lookup_type( S_Symbol theName, t_CKINT climb,
                                           t_CKBOOL stayWithinClassDef )
{
    // remove arrays from name
    int depth = 0;
    S_Symbol oldName = theName;
    std::string theNameStr = S_name(theName);

    // parse for [] | 1.5.2.0
    while( theNameStr.size() >= 2 && theNameStr.substr(theNameStr.size() - 2, 2) == "[]" )
    {
        depth++;
        // remove "[]" from the name
        theNameStr.pop_back();
        theNameStr.pop_back();
    }

    // if this is an array, create an S_Symbol with only the base type
    // (i.e. "int[]" becomes "int" | 1.5.2.0
    if( depth )
    {
        theName = insert_symbol(theNameStr.c_str());
    }

    // find base type
    Chuck_Type * t = type.lookup( theName, climb );
    // respect stayWithinClassDef; check if we are in class def using pre_ctor
    t_CKBOOL keepGoing = ( this->pre_ctor && stayWithinClassDef ) == FALSE;
    // climb up to parent namespace
    if( climb > 0 && !t && parent && keepGoing )
        return parent->lookup_type( oldName, climb, stayWithinClassDef );

    // if this is an array, create an array type and return that | 1.5.2.0
    if( depth )
    {
        // base type
        Chuck_Type * baseT = t;
        // new array type
        t = new_array_type(baseT->env(), baseT->env()->ckt_array, depth, baseT, baseT->env()->curr);
    }

    // return t
    return t;
}




//-----------------------------------------------------------------------------
// name: lookup_func()
// desc: lookup func in the env
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Namespace::lookup_func( const string & theName, t_CKINT climb,
                                           t_CKBOOL stayWithinClassDef )
{
    // call the Symbol version
    return lookup_func( insert_symbol(theName.c_str()), climb, stayWithinClassDef );
}




//-----------------------------------------------------------------------------
// name: lookup_func()
// desc: lookup func in the env
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Namespace::lookup_func( S_Symbol theName, t_CKINT climb,
                                           t_CKBOOL stayWithinClassDef )
{
    Chuck_Func * f = this->func.lookup( theName, climb );
    // respect stayWithinClassDef; check if we are in class def using pre_ctor
    t_CKBOOL keepGoing = ( this->pre_ctor && stayWithinClassDef ) == FALSE;
    // climb up to parent namespace
    if( climb > 0 && !f && parent && keepGoing )
        return parent->lookup_func( theName, climb, stayWithinClassDef );
    return f;
}




//-----------------------------------------------------------------------------
// comparer
//-----------------------------------------------------------------------------
static bool comp_func_type_name( Chuck_Type * a, Chuck_Type * b )
{ return tolower(a->base_name) < tolower(b->base_name); }
//-----------------------------------------------------------------------------
// name: get_types()
// desc: get top level types
//-----------------------------------------------------------------------------
void Chuck_Namespace::get_types( vector<Chuck_Type *> & out )
{
    // temporary vector
    vector<Chuck_VM_Object *> list;
    // get it from the scope
    this->type.get_toplevel( list );

    // clear
    out.clear();
    // copy the list - cast
    for( t_CKUINT i = 0; i < list.size(); i++ )
        out.push_back( (Chuck_Type *)list[i] );

    // sort by string (the order is currently sorted by S_Symbol pointer value)
    sort( out.begin(), out.end(), comp_func_type_name );
}




//-----------------------------------------------------------------------------
// name: get_values()
// desc: get top level values
//-----------------------------------------------------------------------------
void Chuck_Namespace::get_values( vector<Chuck_Value *> & out )
{
    // temporary vector
    vector<Chuck_VM_Object *> list;
    // get it from the scope
    this->value.get_level( 0, list );
    // clear
    out.clear();

    // copy the list - cast
    for( t_CKUINT i = 0; i < list.size(); i++ )
        out.push_back( (Chuck_Value *)list[i] );
}




//-----------------------------------------------------------------------------
// name: get_funcs()
// desc: get top level functions
//-----------------------------------------------------------------------------
void Chuck_Namespace::get_funcs( vector<Chuck_Func *> & out, t_CKBOOL includeMangled )
{
    // temporary vector
    vector<Chuck_VM_Object *> list;
    // get it from the scope
    this->func.get_level( 0, list, includeMangled );
    // clear
    out.clear();

    // copy the list - cast
    for( t_CKUINT i = 0; i < list.size(); i++ )
        out.push_back( (Chuck_Func *)list[i] );
}




//-----------------------------------------------------------------------------
// name: operator ==
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL operator ==( const Chuck_Type & lhs, const Chuck_Type & rhs )
{
    // check id
    if( lhs.xid != rhs.xid ) return FALSE;
    // check array depth
    if( lhs.array_depth != rhs.array_depth ) return FALSE;
    // check array type
    if( lhs.array_depth && (*lhs.actual_type != *rhs.actual_type) ) return FALSE;

    // if user-defined type
    if( lhs.xid == te_user )
    {
        // check name
        if( lhs.base_name != rhs.base_name ) return FALSE;
        // check owner
        if( lhs.owner != rhs.owner ) return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: operator !=
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL operator !=( const Chuck_Type & lhs, const Chuck_Type & rhs )
{ return !( lhs == rhs ); }




//-----------------------------------------------------------------------------
// name: equals()
// desc: type equivalence test
//-----------------------------------------------------------------------------
t_CKBOOL equals( Chuck_Type * lhs, Chuck_Type * rhs ) { return (*lhs) == (*rhs); }




//-----------------------------------------------------------------------------
// name: operator <=
// desc: type equivalence and inheritance test; e.g., is LHS a kind of RHS?
//-----------------------------------------------------------------------------
t_CKBOOL operator <=( const Chuck_Type & lhs, const Chuck_Type & rhs )
{
    return isa( &lhs, &rhs );
}




//-----------------------------------------------------------------------------
// name: isa()
// desc: is LHS a kind of RHS?
//-----------------------------------------------------------------------------
t_CKBOOL isa( const Chuck_Type * lhs, const Chuck_Type * rhs )
{
    // verify
    assert( lhs != NULL && rhs != NULL );
    // need var but return value won't be used
    t_CKBOOL levels = 0;
    // check for isa
    return isa_levels( *lhs, *rhs, levels );
}




//-----------------------------------------------------------------------------
// name: isa_levels()
// desc: 1.5.2.0 (ge) also retrieve # of inheritance levels from lhs to rhs
//-----------------------------------------------------------------------------
t_CKBOOL isa_levels( const Chuck_Type & lhs, const Chuck_Type & rhs, t_CKUINT & levels )
{
    // reset levels
    levels = 0;
    // check to see if type L == type R
    if( lhs == rhs ) return TRUE;

    // if lhs is a child of rhs
    const Chuck_Type * curr = lhs.parent;
    while( curr )
    {
        levels++;
        if( *curr == rhs ) return TRUE;
        curr = curr->parent;
    }

    // back to 0
    levels = 0;
    // if lhs is null and rhs is a object | removed 1.5.1.7?
    if( (lhs == *(lhs.env()->ckt_null)) && (rhs <= *(rhs.env()->ckt_object)) ) return TRUE;

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: cleanup_objs_vec()
// desc: clean up a VM object vector, releasing references and clearing the vec
//-----------------------------------------------------------------------------
void cleanup_objs_vec( std::vector<Chuck_VM_Object *> & v )
{
    // loop over
    for( t_CKUINT i = 0; i < v.size(); i++ )
    {
        // release the object
        CK_SAFE_RELEASE( v[i] );
    }
    // clear the vector
    v.clear();
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Context()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Context::~Chuck_Context()
{
    // if has error, then delete nspc.  otherwise, the nspc stays as part of
    // the type system, since many things have been added to it
    if( has_error )
    {
        // release
        CK_SAFE_RELEASE( this->nspc );
        // done
        goto done;
    }

    // loop over types created in compiling this context | 1.5.1.1
    for( t_CKUINT i = 0; i < new_types.size(); i++ )
    {
        // cast, slightly sus
        Chuck_Type * t = (Chuck_Type *)new_types[i];
        // clear within-context value dependencies for new types
        t->depends.clear();
    }

    // loop over functions created in compiling this context | 1.5.1.1
    for( t_CKUINT i = 0; i < new_funcs.size(); i++ )
    {
        // cast, slightly sus
        Chuck_Func * f = (Chuck_Func *)new_funcs[i];
        // clear within-context value dependencies for new funcs
        f->depends.clear();
    }

done:
    // delete the new types, values, funcs, and nspc | 1.5.1.1
    cleanup_objs_vec( new_types );
    cleanup_objs_vec( new_values );
    cleanup_objs_vec( new_funcs );
    cleanup_objs_vec( new_nspc );
}




//-----------------------------------------------------------------------------
// name: decouple_ast()
// desc: decouple from AST (called when a context is compiled)
//-----------------------------------------------------------------------------
void Chuck_Context::decouple_ast()
{
    Chuck_Func * f = NULL;
    // iterate over Chuck_Func * added as part of this context
    for( t_CKINT i = 0; i < new_funcs.size(); i++ )
    {
        // get as func
        f = (Chuck_Func *)new_funcs[i];
        // decouple with intention of keep func def around
        f->funcdef_decouple_ast();
    }
}




//-----------------------------------------------------------------------------
// name: type_engine_check_reserved()
// desc: ...
//--------------------------------------------------------------------------
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, const string & xid, int pos )
{
    // key word?
    if( env->key_words[xid] )
    {
        EM_error2( pos, "illegal use of keyword '%s'.", xid.c_str() );
        return TRUE;
    }

    // key value?
    if( env->key_values[xid] )
    {
        EM_error2( pos, "illegal re-declaration of reserved value '%s'.", xid.c_str() );
        return TRUE;
    }

    // key type?
    if( env->key_types[xid] )
    {
        EM_error2( pos, "illegal use of reserved type id '%s'.", xid.c_str() );
        return TRUE;
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_reserved()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, S_Symbol xid, int pos )
{
    return type_engine_check_reserved( env, string(S_name(xid)), pos );
}




//-----------------------------------------------------------------------------
// name: type_engine_enable_reserved()
// desc: ...
//-----------------------------------------------------------------------------
t_CKVOID type_engine_enable_reserved( Chuck_Env * env, const std::string & xid, t_CKBOOL value )
{
    // find if the key exists
    if( env->key_values.find( xid ) != env->key_values.end() )
    {
        // set value
        env->key_values[xid] = value;
    }
}




//-----------------------------------------------------------------------------
// name: type_engine_check_const()
// desc: check whether exp is const; returns value is found and const
//-----------------------------------------------------------------------------
Chuck_Value * type_engine_check_const( Chuck_Env * env, a_Exp exp )
{
    // check if rhs is const
    if( exp->s_type == ae_exp_primary )
    {
        // get the associate value
        Chuck_Value * v = exp->primary.value;
        // check if const | 1.5.0.0 (ge) added
        if( v && v->is_const ) return v;
    }
    else if( exp->s_type == ae_exp_dot_member )
    {
        // catch things like `1 => Math.PI`
        a_Exp_Dot_Member member = &exp->dot_member;
        // is the base a class/namespace or a variable | 1.5.0.0 (ge) modified to call
        t_CKBOOL base_static = type_engine_is_base_static( env, member->t_base );
        // actual type
        Chuck_Type * the_base = base_static ? member->t_base->actual_type : member->t_base;

        // if value literal
        if( member->base->s_meta != ae_meta_value ) // is literal
        {
            // find the value
            Chuck_Value * value = type_engine_find_value( the_base, member->xid );
            // if const return value
            if( value && value->is_const ) return value;
        }
    }

    // no indication of const
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_ctor_call() | 1.5.2.0 (ge) added
// desc: type check constructor invocation; also see func_call()
//       (RELATED: type_engine_check_func_call())
//
// -- must be an Object (with caveats for primitive types, later), but not an array
// -- empty () is okay (will invoke default ctor if there is one)
// -- ctors are always incompatible with empty []
// -- must be able to find a constructor with matching args
//-----------------------------------------------------------------------------
Chuck_Func * type_engine_check_ctor_call( Chuck_Env * env, Chuck_Type * type, a_Ctor_Call ctorInfo,
                                          a_Array_Sub array, uint32_t where )
{
    // is an Object?
    t_CKBOOL is_obj = isobj( env, type );
    // is an array
    t_CKBOOL is_array = isa( type, env->ckt_array );
    // the array type
    Chuck_Type * actualType = is_array ? type->array_type : type;

    // constructors can only be called on Objects
    // TODO: handle primitive constructors
    if( !is_obj )
    {
        // error message
        EM_error2( where, "cannot invoke constructor on non-Object type '%s'...", actualType->c_name() );
        return NULL;
    }

    // constructors currently not allowed on array types
    // (NOTE but array creation with constructors is possible)
    if( isa(actualType, env->ckt_array) )
    {
        // error message
        EM_error2( where, "cannot invoke constructor on array types '%s'..." , actualType->c_name() );
        return NULL;
    }

    // check for empty array []
    if( array && !array->exp_list )
    {
        // error message
        EM_error2( where, "cannot invoke constructor on empty array [] declarations...", actualType->c_name() );
        return NULL;
    }

    // check the arguments
    if( ctorInfo->args )
    {
        if( !type_engine_check_exp( env, ctorInfo->args ) )
            return NULL;
    }

    // convert arg list expression to string
    string args2str = "";
    // for iterating through args
    a_Exp a = ctorInfo->args;
    // construct args type string
    while( a )
    {
        // append
        args2str += a->type->name();
        // next
        a = a->next;
        // check
        if( a ) args2str += ",";
    }

    // locate the constructor by type name
    Chuck_Func * funcGroup = actualType->ctors_all;
    // verify
    if( !funcGroup )
    {
        // internal error!
        EM_error2( where, "no constructors defined for '%s'...", actualType->c_name() );
        // bail out
        return NULL;
    }

    // look for a match
    t_CKBOOL hasError = FALSE;
    Chuck_Func * theCtor = find_func_match( env, funcGroup, ctorInfo->args, hasError, where );
    // no func
    if( !theCtor )
    {
        // hasError implies an error has already been printed
        if( hasError ) return NULL;
        // print error
        EM_error2( where, "no matching constructor found for %s(%s)...", actualType->c_name(), args2str.c_str() );
        // bail out
        return NULL;
    }

    // verify
    if( !isa( theCtor->type(), env->ckt_void ) )
    {
        // internal error!
        EM_error2( where, "(internal error) non-void return value for constructor %s(%s)", actualType->c_name(), args2str.c_str() );
        // bail out
        return NULL;
    }

    // return the function
    return theCtor;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_primitive()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_primitive( Chuck_Env * env, Chuck_Type * type )
{
    return ( isa(type, env->ckt_void) || isa(type, env->ckt_int) || isa(type, env->ckt_float) || isa(type, env->ckt_dur) ||
             isa(type, env->ckt_time) || isa(type, env->ckt_complex) || isa(type, env->ckt_polar) ||
             isa(type, env->ckt_vec2) || isa(type, env->ckt_vec3) || isa(type, env->ckt_vec4) )
             && ( type->array_depth == 0 );
}
t_CKBOOL isprim( Chuck_Env * env, Chuck_Type * type )
{   return type_engine_check_primitive( env, type ); }
t_CKBOOL isobj( Chuck_Env * env, Chuck_Type * type )
{   return !type_engine_check_primitive( env, type ); }
t_CKBOOL isfunc( Chuck_Env * env, Chuck_Type * type )
{   return isa( type, env->ckt_function ); }
t_CKBOOL iskindofint( Chuck_Env * env, Chuck_Type * type ) // added 1.3.1.0
{   return isa( type, env->ckt_int ) || isobj( env, type ); }
t_CKBOOL isvoid( Chuck_Env * env, Chuck_Type * type ) // added 1.5.0.0
{   return isa( type, env->ckt_void ); }
t_CKBOOL isnull( Chuck_Env * env, Chuck_Type * type ) // added 1.5.0.0
{   return equals( type, env->ckt_null ); }
te_KindOf getkindof( Chuck_Env * env, Chuck_Type * type ) // added 1.3.1.0
{
    // the kind (1.3.1.0)
    te_KindOf kind = kindof_VOID;

    // check size
    if( type->size == sz_INT && iskindofint(env, type) )
        kind = kindof_INT;
    else if( type->size == sz_FLOAT )
        kind = kindof_FLOAT;
    else if( type->size == sz_VEC2 )
        kind = kindof_VEC2;
    else if( type->size == sz_VEC3 )
        kind = kindof_VEC3;
    else if( type->size == sz_VEC4 )
        kind = kindof_VEC4;

    // done
    return kind;
}
t_CKBOOL isctor( Chuck_Env * env, a_Func_Def func_def ) // 1.5.2.0 (ge) added for constructors
{ return (env->class_def && S_name(func_def->name)==env->class_def->name()); }
t_CKBOOL isdtor( Chuck_Env * env, a_Func_Def func_def ) // 1.5.2.0 (ge) added for constructors
{ return (env->class_def && S_name(func_def->name)==(string("@destruct"))); }




//-----------------------------------------------------------------------------
// name: type_engine_find_common_anc()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_common_anc( Chuck_Type * lhs, Chuck_Type * rhs )
{
    // check to see if either is child of other
    if( isa( lhs, rhs ) ) return rhs;
    if( isa( rhs, lhs ) ) return lhs;

    // move up
    Chuck_Type * t = lhs->parent;

    // not at root
    while( t )
    {
        // check and see again
        if( isa( rhs, t ) ) return t;
        // move up
        t = t->parent;
    }

    // didn't find
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_path()
// desc: ...
//-----------------------------------------------------------------------------
const char * type_path( a_Id_List thePath )
{
    static string str;

    // clear it
    str = "";
    // loop over path
    while( thePath )
    {
        // concatenate
        str += S_name(thePath->xid);
        // add .
        if( thePath->next ) str += ".";
        // advance
        thePath = thePath->next;
    }

    return str.c_str();
}




//-----------------------------------------------------------------------------
// name: type_engine_find_type()
// desc: find type within a namespace
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_type( Chuck_Namespace * npsc, S_Symbol xid )
{
    Chuck_Type * type = NULL;
    if( !npsc ) return NULL;

    // -1 for base
    type = npsc->lookup_type( xid, -1 );
    if( type ) return type;

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_type()
// desc: find global type
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_type( Chuck_Env * env, const std::string & name )
{
    return type_engine_find_type( env->global(), insert_symbol(name.c_str()) );
}




//-----------------------------------------------------------------------------
// name: type_engine_get_deprecate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_get_deprecate( Chuck_Env * env,
                                    const string & from, string & to )
{
    // find mapping
    if( env->deprecated.find( from ) == env->deprecated.end() )
        return FALSE;

    // return
    to = env->deprecated[from];

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_deprecated_type()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_deprecated_type( Chuck_Env * env, a_Id_List thePath )
{
    // S_Symbol xid = NULL;
    // Chuck_Type * t = NULL;
    std::string actual;

    // find mapping
    if( !type_engine_get_deprecate( env, S_name(thePath->xid), actual ) )
        return NULL;

    // get base type
    Chuck_Type * type = env->curr->lookup_type( actual, TRUE );
    if( !type ) return NULL;
    else
    {
        // check level
        if( env->deprecate_level < 2 )
        {
            EM_error2( thePath->where, "deprecated: '%s' --> use: '%s' instead",
                type_path( thePath ), actual.c_str() );
        }
    }

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_type()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_type( Chuck_Env * env, a_Id_List thePath )
{
    S_Symbol xid = NULL;
    Chuck_Type * t = NULL;
    // get base type
    Chuck_Type * type = env->curr->lookup_type( thePath->xid, TRUE );
    if( !type )
    {
        // check level
        if( env->deprecate_level > 0 )
            type = type_engine_find_deprecated_type( env, thePath );

        // error
        if( !type )
        {
            EM_error2( thePath->where, "undefined type '%s'...",
                type_path( thePath ) );
            return NULL;
        }
    }
    // start the namespace
    Chuck_Namespace * theNpsc = type->info;
    thePath = thePath->next;

    // loop
    while( thePath != NULL )
    {
        // get the id
        xid = thePath->xid;
        // look for the type in the namespace
        t = type_engine_find_type( theNpsc, xid );
        // look in parent
        while( !t && type && type->parent )
        {
            t = type_engine_find_type( type->parent->info, xid );
            type = type->parent;
        }
        // can't find
        if( !t )
        {
            // error
            EM_error2( thePath->where, "undefined type '%s'...",
                type_path( thePath ) );
            EM_error2( thePath->where,
                "...(cannot find class '%s' in namespace '%s')",
                S_name(xid), theNpsc->name.c_str() );
            return NULL;
        }

        // set the type
        type = t;
        // update nspc
        if( type ) theNpsc = type->info;
        // advance
        thePath = thePath->next;
    }

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_names2types()
// desc: convert a vector of type names to a vector of Types
//-----------------------------------------------------------------------------
void type_engine_names2types( Chuck_Env * env,
                              const std::vector<std::string> & typeNames,
                              std::vector<Chuck_Type *> & types )
{
    // clear result vector
    types.clear();
    // resize
    types.resize( typeNames.size() );
    // iterate
    for( t_CKINT i = 0; i < typeNames.size(); i++ )
    {
        // lookup
        types[i] = type_engine_find_type( env, typeNames[i] );
    }
}




//-----------------------------------------------------------------------------
// name: type_engine_find_value()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Value * type_engine_find_value( Chuck_Type * type, S_Symbol xid )
{
    return type_engine_find_value( type, string(S_name(xid)) );
}




//-----------------------------------------------------------------------------
// name: type_engine_find_value()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Value * type_engine_find_value( Chuck_Type * type, const string & xid )
{
    Chuck_Value * value = NULL;

    if( !type ) return NULL;
    if( !type->info ) return NULL;
    // -1 for base
    value = type->info->lookup_value( xid, -1 );
    if( value ) return value;
    if( type->parent ) return type_engine_find_value( type->parent, xid );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_value()
// desc: search for value within Env, climbing up scope, but possibly staying
//       within the same class def if stayWithinClassDef == TRUE
//       1.5.1.3 (ge) added stayWithinClassDef logic
//-----------------------------------------------------------------------------
Chuck_Value * type_engine_find_value( Chuck_Env * env, const string & xid,
                                      t_CKBOOL climb, t_CKBOOL stayWithinClassDef,
                                      int linepos )
{
    Chuck_Value * value = NULL;
    string actual;

    // look up
    value = env->curr->lookup_value( xid, climb, stayWithinClassDef );
    if( value ) return value;

    // if climb, check deprecation deprecated if climb...
    // BUT do this only if !stayWithinClassDef, otherwise could yield false deprecation
    // i.e., when looking for deprecation, go all the way up or don't do it here
    if( climb && !stayWithinClassDef )
    {
        if( !type_engine_get_deprecate( env, xid, actual ) )
            return NULL;

        // get base type
        value = env->curr->lookup_value( actual, TRUE );
        if( !value ) return NULL;
        else
        {
            // check level
            if( env->deprecate_level < 2 )
            {
                EM_error2( linepos, "deprecated: '%s' --> use: '%s' instead",
                    xid.c_str(), actual.c_str() );
            }
        }
    }

    return value;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_nspc()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Namespace * type_engine_find_nspc( Chuck_Env * env, a_Id_List thePath )
{
    Chuck_Namespace * theNpsc = NULL;
    // if the first if global, move to the next
    if( thePath && !strcmp( S_name(thePath->xid), "global" ) ) thePath = thePath->next;
    // global namespace
    if( thePath == NULL ) return env->global();
    // find the type
    Chuck_Type * type = type_engine_find_type( env, thePath );
    if( type == NULL ) return NULL;
    // copy the nspc
    theNpsc = type->info;
    if( theNpsc == NULL )
    {
        // primitive
        if( isprim( env, type ) )
        {
            // error
            EM_error2( 0, "primitive type '%s' has no namespace and cannot be extended",
                type->c_name() );
            return NULL;
        }
        else
        {
            // internal error
            EM_error2( 0, "(internal error) type '%s' without namespace",
                type->c_name() );
            return NULL;
        }
    }

    return theNpsc;
}




//-----------------------------------------------------------------------------
// name: type_engine_next_offset()
// desc: compute next offset based on current offset and type
//-----------------------------------------------------------------------------
t_CKUINT type_engine_next_offset( t_CKUINT current_offset, Chuck_Type * type )
{
    return current_offset + type->size;
}




//-----------------------------------------------------------------------------
// name: type_engine_compat_func()
// desc: see if two function signatures are compatible
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_compat_func( a_Func_Def lhs, a_Func_Def rhs, int pos, string & err, t_CKBOOL print )
{
    // make sure public/private/protected/function match
    if( lhs->func_decl != rhs->func_decl )
    {
        if( print )
        {
            EM_error2( pos, "function signatures differ in access modifiers..." );
            EM_error2( pos, "(both must be one of public/private/protected/function)" );
        }
        return FALSE;
    }

    a_Arg_List e1 = lhs->arg_list;
    a_Arg_List e2 = rhs->arg_list;
    t_CKUINT count = 1;

    // check arguments against the definition
    while( e1 && e2 )
    {
        // make sure
        assert( e1->type && e2->type );

        // match types
        if( *e1->type != *e2->type )
        {
            if( print ) EM_error2( pos, "function signatures differ in argument %i's type", count );
            return FALSE;
        }

        e1 = e1->next;
        e2 = e2->next;
        count++;
    }

    // anything left
    if( e1 != NULL || e2 != NULL )
    {
        if( print ) EM_error2( pos,
            "function signatures differ in number of arguments" );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_class_begin()
// desc: import existing base class, such as Object or Event
//       must be completed by type_engine_import_class_end()
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, Chuck_Type * type,
                                             Chuck_Namespace * where,
                                             f_ctor pre_ctor, f_dtor dtor,
                                             const char * doc )
{
    Chuck_Value * value = NULL;
    Chuck_Type * type_type = NULL;

    // make sure there is not namesapce
    if( type->info != NULL )
    {
        // error
        EM_error2( 0, "during import: class '%s' already imported", type->c_name() );
        return NULL;
    }

    // allocate namespace for type
    type->info = new Chuck_Namespace;
    // add reference
    CK_SAFE_ADD_REF(type->info);
    // name it
    type->info->name = type->base_name;
    // set the parent namespace
    type->info->parent = where;
    // add reference
    CK_SAFE_ADD_REF(type->info->parent);

    // if pre constructor
    if( pre_ctor != 0 )
    {
        // flag it
        type->has_pre_ctor = TRUE;
        // allocate vm code for (imported) pre_ctor
        type->info->pre_ctor = new Chuck_VM_Code;
        // add ref | 1.5.2.0 (ge) added
        CK_SAFE_ADD_REF( type->info->pre_ctor );
        // add pre_ctor
        type->info->pre_ctor->native_func = (t_CKUINT)pre_ctor;
        // mark type as ctor
        type->info->pre_ctor->native_func_kind = ae_fp_ctor;
        // specify that we need this
        type->info->pre_ctor->need_this = TRUE;
        // no arguments to preconstructor other than self
        type->info->pre_ctor->stack_depth = sizeof(t_CKUINT);
        // add name | 1.5.2.0 (ge) added
        type->info->pre_ctor->name = string("class ") + type->base_name;
    }

    // if destructor
    if( dtor != 0 )
    {
        // flag it (needed since info could be shared with array types of this type, but this flag is only this type)
        type->has_pre_dtor = TRUE;
        // allocate vm code for dtor
        type->info->pre_dtor = new Chuck_VM_Code;
        // add ref | 1.5.2.0 (ge) added
        CK_SAFE_ADD_REF( type->info->pre_dtor );
        // add dtor
        type->info->pre_dtor->native_func = (t_CKUINT)dtor;
        // mark type as dtor
        type->info->pre_dtor->native_func_kind = ae_fp_dtor;
        // specify that we need this
        type->info->pre_dtor->need_this = TRUE;
        // no arguments to destructor other than self
        type->info->pre_dtor->stack_depth = sizeof(t_CKUINT);
        // add name | 1.5.2.0 (ge) added
        type->info->pre_dtor->name = string("class ") + type->base_name + string(" (destructor)");
    }

    // clear the object size
    type->obj_size = 0;
    // set the beginning of the data segment after parent
    if( type->parent )
    {
        type->info->offset = type->parent->obj_size;
        // duplicate parent's virtual table
        assert( type->parent->info != NULL );
        type->info->obj_v_table = type->parent->info->obj_v_table;
    }

    // set the owner namespace
    type->owner = where; CK_SAFE_ADD_REF(type->owner);
    // check if primitive
    if( !isprim( env, type ) ) // 1.3.5.3 (primitives already have size!)
    {
        // set the size, which is always the width of a pointer
        type->size = sizeof(t_CKUINT);
    }

    // flag as complete
    type->is_complete = TRUE;
    // make type; context added in 1.5.1.1
    type_type = env->ckt_class->copy( env, env->context );
    type_type->actual_type = type;
    // CK_SAFE_REF_ASSIGN( type_type->actual_type, type );

    // make value
    value = new Chuck_Value( type_type, type->base_name );
    value->owner = where; CK_SAFE_ADD_REF( value->owner );
    // CK_SAFE_REF_ASSIGN( value->owner, where );
    value->is_const = TRUE;
    value->is_member = FALSE;

    // add to env
    where->value.add( value->name, value );

    // make the type current
    env->nspc_stack.push_back( env->curr );
    env->curr = type->info;
    // push the class def
    env->class_stack.push_back( env->class_def );
    env->class_def = type;

    if( doc != NULL) type->doc = doc;

    // ref count
    CK_SAFE_ADD_REF(type);
    // type->add_ref();

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_class_begin()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, const char * name,
                                             const char * parent_str,
                                             Chuck_Namespace * where,
                                             f_ctor pre_ctor, f_dtor dtor,
                                             const char * doc )
{
    // which namespace
    Chuck_Type * parent = NULL;
    Chuck_Type * type = NULL;
    a_Id_List parent_list = NULL;

    // if parent is specified
    if( strcmp( parent_str, "") )
    {
        // get parent
        parent_list = str2list( parent_str );
        parent = type_engine_find_type( env, parent_list );
        if( !parent ) goto error;
    }
    else // if no parent specified, then extend Object
    {
        parent = env->ckt_object;
    }

    // allocate type
    type = new Chuck_Type( env, te_user, name, parent, sizeof(void *) );

    // add to namespace - TODO: handle failure, remove from where
    where->type.add( name, type );

    // do the rest
    if( !type_engine_import_class_begin( env, type, where, pre_ctor, dtor, doc ) )
        goto error;

    // done
    goto cleanup;

error:
    // error
    EM_error2( 0, "...during import of class '%s'", name );
    // free
    CK_SAFE_DELETE( type );

cleanup:
    // cleanup
    delete_id_list( parent_list );

    return type;
}


//-----------------------------------------------------------------------------
// name: type_engine_import_ugen_begin()
// desc: ...
// note: tickf added 1.3.0.0
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name,
                                            const char * parent, Chuck_Namespace * where,
                                            f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tickf tickf, f_pmsg pmsg,
                                            t_CKUINT num_ins, t_CKUINT num_outs,
                                            const char * doc )
{
    Chuck_Type * type = NULL;
    Chuck_UGen_Info * info = NULL;

    // construct class
    type = type_engine_import_class_begin( env, name, parent, where, pre_ctor, dtor, doc );
    if( !type ) return NULL;

    // make sure parent is ugen
    assert( type->parent != NULL );
    if( !isa( type->parent, env->ckt_ugen ) )
    {
        // error
        EM_error2( 0,
            "imported class '%s' adds ugen_func() but is not a 'UGen'",
            type->c_name() );
        return NULL;
    }

    // do the ugen part
    info = new Chuck_UGen_Info;
    info->add_ref();
    info->tick = type->parent->ugen_info->tick;
    info->tickf = type->parent->ugen_info->tickf; // added 1.3.0.0
    info->pmsg = type->parent->ugen_info->pmsg;
    info->num_ins = type->parent->ugen_info->num_ins;
    info->num_outs = type->parent->ugen_info->num_outs;
    if( tick ) info->tick = tick;
    if( tickf ) { info->tickf = tickf; info->tick = NULL; } // added 1.3.0.0
    if( pmsg ) info->pmsg = pmsg;
    if( num_ins != CK_NO_VALUE ) info->num_ins = num_ins;
    if( num_outs != CK_NO_VALUE ) info->num_outs = num_outs;
    // set in type
    type->ugen_info = info;

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_ugen_begin()
// desc: old version (added 1.3.0.0)
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name,
                                           const char * parent, Chuck_Namespace * where,
                                           f_ctor pre_ctor, f_dtor dtor,
                                           f_tick tick, f_pmsg pmsg,
                                           t_CKUINT num_ins, t_CKUINT num_outs,
                                           const char * doc )
{
    return type_engine_import_ugen_begin( env, name, parent, where,
                                          pre_ctor, dtor, tick, NULL, pmsg,
                                          num_ins, num_outs, doc );
}




//-----------------------------------------------------------------------------
// name: type_engine_import_ugen_begin()
// desc: doc version (added 1.3.5.0)
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name,
                                            const char * parent, Chuck_Namespace * where,
                                            f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_pmsg pmsg,
                                            const char * doc )
{
    return type_engine_import_ugen_begin( env, name, parent, where,
                                          pre_ctor, dtor, tick, NULL, pmsg,
                                          CK_NO_VALUE, CK_NO_VALUE, doc );
}




//-----------------------------------------------------------------------------
// name: type_engine_import_uana_begin()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_uana_begin( Chuck_Env * env, const char * name,
                                            const char * parent, Chuck_Namespace * where,
                                            f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tock tock, f_pmsg pmsg,
                                            t_CKUINT num_ins, t_CKUINT num_outs,
                                            t_CKUINT num_ins_ana, t_CKUINT num_outs_ana,
                                            const char * doc )
{
    Chuck_Type * type = NULL;
    Chuck_UGen_Info * info = NULL;

    // construct class
    type = type_engine_import_ugen_begin( env, name, parent, where, pre_ctor, dtor,
                                          tick, pmsg, num_ins, num_outs, doc );
    if( !type ) return NULL;

    // make sure parent is ugen
    assert( type->parent != NULL );
    if( !isa( type->parent, env->ckt_uana ) )
    {
        // error
        EM_error2( 0,
            "imported class '%s' does not have a uana as parent",
            type->c_name() );
        return NULL;
    }

    // do the info
    info = type->ugen_info;
    // sanity
    assert( info != NULL );
    // set
    if( tock ) info->tock = tock;
    if( num_ins_ana != CK_NO_VALUE ) info->num_ins_ana = num_ins_ana;
    if( num_outs_ana != CK_NO_VALUE ) info->num_outs_ana = num_outs_ana;

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_class_end()
// desc: finish import of a class
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_class_end( Chuck_Env * env )
{
    if( !env->class_def )
    {
        // error
        EM_error2( 0, "import: too many class_end called" );
        return FALSE;
    }

    // set the object size
    env->class_def->obj_size = env->class_def->info->offset;

    // for everything that's not the base Object type or arrays | 1.5.0.0
    // context: Object and Type mutually depend, so we will initialize
    // Object's Type object manually in type system initialization
    // same with arrays since Type's API involved array arguments
    if( env->class_def->base_name != env->ckt_object->base_name &&
        env->class_def->base_name != env->ckt_array->base_name )
    {
        // initialize the type as object | 1.5.0.0 (ge) added
        initialize_object( env->class_def, env->ckt_class, NULL, env->vm() );
    }

    // pop the class
    env->class_def = env->class_stack.back();
    env->class_stack.pop_back();
    // pop the namesapce
    env->curr = env->nspc_stack.back();
    env->nspc_stack.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_ctor() | 1.5.2.0 (ge)
// desc: import constructor (must be between class_begin/end)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_ctor( Chuck_Env * env, Chuck_DL_Func * ctor )
{
    a_Func_Def func_def = NULL;

    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0, "(import error) import_ctor() '%s' invoked outside begin/end",
                  env->context ? env->context->filename.c_str() : "[no context]" );
        return FALSE;
    }

    // make sure the type matches
    if( ctor->fpKind != ae_fp_ctor )
    {
        // error
        EM_error2( 0, "(import error) import_ctor() incompatible with (%s) '%s.%s'...",
                   fpkind2str(ctor->fpKind), env->class_def->base_name.c_str(), ctor->name.c_str() );
        return FALSE;
    }

    // set return type to void
    if( ctor->type == "" ) ctor->type = env->ckt_void->name();
    // set ctor name to class
    if( ctor->name == "" || ctor->name == "@construct" ) ctor->name = env->class_def->base_name;

    // make into func_def
    func_def = make_dll_as_fun( ctor, FALSE, FALSE );
    if( !func_def )
        return FALSE;

    // add the function to class
    if( !type_engine_scan1_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_scan2_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_check_func_def( env, func_def ) )
        return FALSE;

    if( ctor->doc.size() > 0 )
        func_def->ck_func->doc = ctor->doc;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_mfun()
// desc: import member function (must be between class_begin/end)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_mfun( Chuck_Env * env, Chuck_DL_Func * mfun )
{
    a_Func_Def func_def = NULL;

    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0, "(import error) import_mfun '%s' invoked outside begin/end",
                   mfun->name.c_str() );
        return FALSE;
    }

    // make sure the type matches
    if( mfun->fpKind != ae_fp_mfun )
    {
        // error
        EM_error2( 0, "(import error) import_mfun() incompatible with (%s) '%s.%s'...",
                   fpkind2str(mfun->fpKind), env->class_def->base_name.c_str(), mfun->name.c_str() );
        return FALSE;
    }

    // make into func_def
    func_def = make_dll_as_fun( mfun, FALSE, FALSE );
    if( !func_def )
        return FALSE;

    // add the function to class
    if( !type_engine_scan1_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_scan2_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_check_func_def( env, func_def ) )
        return FALSE;

    if( mfun->doc.size() > 0 )
        func_def->ck_func->doc = mfun->doc;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_sfun()
// desc: import static function (must be between class_begin/end)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_sfun( Chuck_Env * env, Chuck_DL_Func * sfun )
{
    a_Func_Def func_def = NULL;

    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0, "(import error) import_sfun '%s' invoked outside begin/end",
                   sfun->name.c_str() );
        return FALSE;
    }

    // make sure the type matches
    if( sfun->fpKind != ae_fp_sfun )
    {
        // error
        EM_error2( 0, "(import error) import_sfun() incompatible with (%s) '%s.%s'...",
                   fpkind2str(sfun->fpKind), env->class_def->base_name.c_str(), sfun->name.c_str() );
        return FALSE;
    }

    // make into func_def
    func_def = make_dll_as_fun( sfun, TRUE, FALSE );

    // add the function to class
    if( !type_engine_scan1_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_scan2_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_check_func_def( env, func_def ) )
        return FALSE;

    if( sfun->doc.size() > 0 )
        func_def->ck_func->doc = sfun->doc;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_mvar()
// desc: import member variable (must be between class_begin/end)
//-----------------------------------------------------------------------------
t_CKUINT type_engine_import_mvar( Chuck_Env * env, const char * type,
                                  const char * name, t_CKUINT is_const,
                                  const char * doc )
{
    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0,
            "(import error) import_mvar '%s' invoked outside begin/end",
            name );
        return CK_INVALID_OFFSET;
    }

    // make path
    t_CKUINT array_depth = 0;
    a_Id_List thePath = str2list( type, array_depth );
    if( !thePath )
    {
        // error
        EM_error2( 0, "...during mvar import '%s.%s'",
            env->class_def->c_name(), name );
        return CK_INVALID_OFFSET;
    }
    // make type decl
    a_Type_Decl type_decl = new_type_decl( thePath, FALSE, 0, 0 );
    // check for array
    if( array_depth )
    {
        // add the array
        type_decl->array = new_array_sub( NULL, 0, 0 );
        // set the depth
        type_decl->array->depth = array_depth;
    }
    // make var decl
    a_Var_Decl var_decl = new_var_decl( (char *)name, FALSE, NULL, NULL, 0, 0 ); // 1.5.2.0 (ge) add ctor arglist

    // added 2013-10-22 - spencer
    // allow array-type mvars
    // check for array
    if( array_depth )
    {
        // add the array
        var_decl->array = new_array_sub( NULL, 0, 0 );
        // set the depth
        var_decl->array->depth = array_depth;
    }

    // make var decl list
    a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0, 0 );
    // make exp decl
    a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, FALSE, (int)is_const, 0, 0 );
    // add it
    if( !type_engine_scan1_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_scan2_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_check_exp_decl( env, &exp_decl->decl ) )
    {
        // clean up locally created id list
        // TODO 2023 1.5.0.5 -- clean up the various decls?
        delete_id_list( thePath );
        return CK_INVALID_OFFSET;
    }

    if( doc != NULL )
        var_decl->value->doc = doc;

    // clean up locally created id list
    delete_id_list( thePath );
    // TODO 2023 1.5.0.5 -- clean up the various decls?

    // return the offset
    return var_decl->value->offset;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_svar()
// desc: import static variable (must be between class_begin/end)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_svar( Chuck_Env * env, const char * type,
                                  const char * name, t_CKUINT is_const,
                                  t_CKUINT addr, const char * doc )
{
    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0,
            "(import error) import_svar '%s' invoked outside begin/end",
            name );
        return FALSE;
    }

    // make path
    a_Id_List thePath = str2list( type );
    if( !thePath )
    {
        // error
        EM_error2( 0, "...during svar import '%s.%s'",
            env->class_def->c_name(), name );
        return FALSE;
    }

    // make type decl
    a_Type_Decl type_decl = new_type_decl( thePath, FALSE, 0, 0 );
    // make var decl
    a_Var_Decl var_decl = new_var_decl( (char *)name, FALSE, NULL, NULL, 0, 0 );  // 1.5.2.0 (ge) add ctor arglist
    // make var decl list
    a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0, 0 );
    // make exp decl
    a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, TRUE, (int)is_const, 0, 0 );
    // add addr
    var_decl->addr = (void *)addr;
    // add it
    if( !type_engine_scan1_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_scan2_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_check_exp_decl( env, &exp_decl->decl ) )
    {
        // clean up locally created id list
        delete_id_list( thePath );
        return FALSE;
    }

    // TODO 2023 1.5.0.5 -- clean up the various decls?

    if( doc != NULL )
        var_decl->value->doc = doc;

    // clean up locally created id list
    delete_id_list( thePath );

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: type_engine_import_add_ex()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_add_ex( Chuck_Env * env, const char * ex )
{
    // make sure we are in class
    if( !env->class_def )
    {
        // error
        EM_error2( 0,
                   "(import error) import_add_ex '%s' invoked between begin/end",
                   ex );
        return FALSE;
    }

    env->class_def->examples.push_back( ex );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_register_deprecate()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_register_deprecate( Chuck_Env * env,
                                         const string & former,
                                         const string & latter )
{
    env->deprecated[former] = latter;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_import_op_overload()
// desc: import operator overload function; add to global scope
//       NOTE this is typically called from chugin / builtin import
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_import_op_overload( Chuck_Env * env, Chuck_DL_Func * sfun )
{
    a_Func_Def func_def = NULL;

    // make sure we are in class
    if( env->class_def )
    {
        // error
        EM_error2( 0,
            "(import error) import_sfun '%s' invoked between begin/end",
            sfun->name.c_str() );
        return FALSE;
    }

    // make into func_def
    func_def = make_dll_as_fun( sfun, TRUE, FALSE );

    // add the function to class
    if( !type_engine_scan1_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_scan2_func_def( env, func_def ) )
        return FALSE;
    if( !type_engine_check_func_def( env, func_def ) )
        return FALSE;

    if( sfun->doc.size() > 0 )
        func_def->ck_func->doc = sfun->doc;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_init_op_overload_builtin() | 1.5.1.5 (ge) added
// desc: reserve builtin default operator overloads; this disallows certain
//       overloadings, e.g., int + int
//-----------------------------------------------------------------------------
// "operator overloading should extend the language, not mutate it."
//                       -- (paraphrased) Bjarne Stroustroup
//-----------------------------------------------------------------------------
void type_engine_init_op_overload_builtin( Chuck_Env * env )
{
    // log
    EM_log( CK_LOG_HERALD, "reserving default operator mappings..." );

    // the registry
    Chuck_Op_Registry * registry = &env->op_registry;

    //-------------------------------------------------------------------------
    // => (chuck) =< (unchuck) =^ (upchuck)
    //-------------------------------------------------------------------------
    // UGen => UGen (e.g., SinOsc x => NRev r)
    registry->reserve( env->ckt_ugen, ae_op_chuck, env->ckt_ugen );
    // dur => time (e.g., x=> now)
    registry->reserve( env->ckt_dur, ae_op_chuck, env->ckt_time );
    // time => time (e.g., x => now)
    registry->reserve( env->ckt_time, ae_op_chuck, env->ckt_time );
    // Event => time (e.g., x => now)
    registry->reserve( env->ckt_event, ae_op_chuck, env->ckt_time );
    // IO => int (e.g,. cherr => 2)
    registry->reserve( env->ckt_io, ae_op_chuck, env->ckt_int );
    // IO => float (e.g., cherr => 3.5)
    registry->reserve( env->ckt_io, ae_op_chuck, env->ckt_float );
    // IO => string (e.g., cherr => "hello")
    registry->reserve( env->ckt_io, ae_op_chuck, env->ckt_string );
    // UGen =< UGen (e.g., x =< dac)
    registry->reserve( env->ckt_ugen, ae_op_unchuck, env->ckt_ugen );
    // UAna =^ UAna (e.g., FFT fft =^ IFFT ifft)
    registry->reserve( env->ckt_uana, ae_op_upchuck, env->ckt_uana );

    //-------------------------------------------------------------------------
    // + - * /
    //-------------------------------------------------------------------------
    // +
    registry->reserve( env->ckt_int, ae_op_plus, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_plus, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_plus, env->ckt_dur );
    registry->reserve( env->ckt_dur, ae_op_plus, env->ckt_time ); // dur +=> time (e.g., 2::second +=> now)
    registry->reserve( env->ckt_time, ae_op_plus, env->ckt_dur );
    registry->reserve( env->ckt_complex, ae_op_plus, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_plus, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_plus, env->ckt_vec2 ); // 1.5.1.7
    registry->reserve( env->ckt_vec3, ae_op_plus, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_plus, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_plus, env->ckt_vec3, TRUE ); // commute | 1.5.1.7
    registry->reserve( env->ckt_vec2, ae_op_plus, env->ckt_vec4, TRUE ); // commute | 1.5.1.7
    registry->reserve( env->ckt_vec3, ae_op_plus, env->ckt_vec4, TRUE ); // commute
    registry->reserve( env->ckt_object, ae_op_plus, env->ckt_string ); // object +=> string
    registry->reserve( env->ckt_int, ae_op_plus, env->ckt_string, TRUE ); // int/float +=> string
    registry->reserve( env->ckt_float, ae_op_plus, env->ckt_string, TRUE ); // string +=> int/float
    // -
    registry->reserve( env->ckt_time, ae_op_minus, env->ckt_time );
    registry->reserve( env->ckt_time, ae_op_minus, env->ckt_dur );
    registry->reserve( env->ckt_int, ae_op_minus, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_minus, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_minus, env->ckt_dur );
    registry->reserve( env->ckt_complex, ae_op_minus, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_minus, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_minus, env->ckt_vec2 ); // 1.5.1.7
    registry->reserve( env->ckt_vec3, ae_op_minus, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_minus, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_minus, env->ckt_vec3, TRUE ); // commute | 1.5.1.7
    registry->reserve( env->ckt_vec2, ae_op_minus, env->ckt_vec4, TRUE ); // commute | 1.5.1.7
    registry->reserve( env->ckt_vec3, ae_op_minus, env->ckt_vec4, TRUE );
    // *
    registry->reserve( env->ckt_int, ae_op_times, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_times, env->ckt_float );
    registry->reserve( env->ckt_complex, ae_op_times, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_times, env->ckt_polar );
    /* no 2D cross product; use vec3 and vec4 */
    registry->reserve( env->ckt_vec3, ae_op_times, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_times, env->ckt_vec4 );
    registry->reserve( env->ckt_float, ae_op_times, env->ckt_vec2, TRUE ); // commute | 1.5.1.7
    registry->reserve( env->ckt_float, ae_op_times, env->ckt_vec3, TRUE );
    registry->reserve( env->ckt_float, ae_op_times, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_float, ae_op_times, env->ckt_dur, TRUE );
    // /
    registry->reserve( env->ckt_int, ae_op_divide, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_divide, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_divide, env->ckt_dur );
    registry->reserve( env->ckt_dur, ae_op_divide, env->ckt_float );
    registry->reserve( env->ckt_time, ae_op_divide, env->ckt_dur );
    registry->reserve( env->ckt_complex, ae_op_divide, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_divide, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_divide, env->ckt_float );
    registry->reserve( env->ckt_vec3, ae_op_divide, env->ckt_float );
    registry->reserve( env->ckt_vec4, ae_op_divide, env->ckt_float );

    //-------------------------------------------------------------------------
    // == != < > <= >=
    //-------------------------------------------------------------------------
    registry->reserve( env->ckt_int, ae_op_eq, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_eq, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_eq, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_eq, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_eq, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_eq, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_eq, env->ckt_vec2 );
    registry->reserve( env->ckt_vec3, ae_op_eq, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_eq, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_eq, env->ckt_vec3, TRUE );
    registry->reserve( env->ckt_vec2, ae_op_eq, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_vec3, ae_op_eq, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_eq, env->ckt_object );
    // !=
    registry->reserve( env->ckt_int, ae_op_neq, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_neq, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_neq, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_neq, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_neq, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_neq, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_neq, env->ckt_vec2 );
    registry->reserve( env->ckt_vec3, ae_op_neq, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_neq, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_neq, env->ckt_vec3, TRUE );
    registry->reserve( env->ckt_vec2, ae_op_neq, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_vec3, ae_op_neq, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_neq, env->ckt_object );
    // <
    registry->reserve( env->ckt_int, ae_op_lt, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_lt, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_lt, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_lt, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_lt, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_lt, env->ckt_polar );
    // registry->reserve( env->ckt_vec2, ae_op_lt, env->ckt_vec2 );
    // registry->reserve( env->ckt_vec3, ae_op_lt, env->ckt_vec3 );
    // registry->reserve( env->ckt_vec4, ae_op_lt, env->ckt_vec4 );
    // registry->reserve( env->ckt_vec2, ae_op_lt, env->ckt_vec3, TRUE );
    // registry->reserve( env->ckt_vec2, ae_op_lt, env->ckt_vec4, TRUE );
    // registry->reserve( env->ckt_vec3, ae_op_lt, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_lt, env->ckt_object );
    // >
    registry->reserve( env->ckt_int, ae_op_gt, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_gt, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_gt, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_gt, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_gt, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_gt, env->ckt_polar );
    // registry->reserve( env->ckt_vec2, ae_op_gt, env->ckt_vec2 );
    // registry->reserve( env->ckt_vec3, ae_op_gt, env->ckt_vec3 );
    // registry->reserve( env->ckt_vec4, ae_op_gt, env->ckt_vec4 );
    // registry->reserve( env->ckt_vec2, ae_op_gt, env->ckt_vec3, TRUE );
    // registry->reserve( env->ckt_vec2, ae_op_gt, env->ckt_vec4, TRUE );
    // registry->reserve( env->ckt_vec3, ae_op_gt, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_gt, env->ckt_object );
    // <=
    registry->reserve( env->ckt_int, ae_op_le, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_le, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_le, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_le, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_le, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_le, env->ckt_polar );
    // registry->reserve( env->ckt_vec2, ae_op_le, env->ckt_vec2 );
    // registry->reserve( env->ckt_vec3, ae_op_le, env->ckt_vec3 );
    // registry->reserve( env->ckt_vec4, ae_op_le, env->ckt_vec4 );
    // registry->reserve( env->ckt_vec2, ae_op_le, env->ckt_vec3, TRUE );
    // registry->reserve( env->ckt_vec2, ae_op_le, env->ckt_vec4, TRUE );
    // registry->reserve( env->ckt_vec3, ae_op_le, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_le, env->ckt_object );
    // IO <= rhs
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_int );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_float );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_dur );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_time );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_complex );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_polar );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_vec2 );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_vec3 );
    registry->reserve( env->ckt_io, ae_op_le, env->ckt_vec4 );
    // >=
    registry->reserve( env->ckt_int, ae_op_ge, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_ge, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_ge, env->ckt_dur );
    registry->reserve( env->ckt_time, ae_op_ge, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_ge, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_ge, env->ckt_polar );
    // registry->reserve( env->ckt_vec2, ae_op_ge, env->ckt_vec2 );
    // registry->reserve( env->ckt_vec3, ae_op_ge, env->ckt_vec3 );
    // registry->reserve( env->ckt_vec4, ae_op_ge, env->ckt_vec4 );
    // registry->reserve( env->ckt_vec2, ae_op_ge, env->ckt_vec3, TRUE );
    // registry->reserve( env->ckt_vec2, ae_op_ge, env->ckt_vec4, TRUE );
    // registry->reserve( env->ckt_vec3, ae_op_ge, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_ge, env->ckt_object );

    //-------------------------------------------------------------------------
    // && & || | ^ << >> %
    //-------------------------------------------------------------------------
    registry->reserve( env->ckt_int, ae_op_and, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_or, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_s_and, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_s_or, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_s_xor, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_shift_left, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_shift_right, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_percent, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_percent, env->ckt_float );
    registry->reserve( env->ckt_time, ae_op_percent, env->ckt_dur );
    registry->reserve( env->ckt_dur, ae_op_percent, env->ckt_dur );
    // TODO: look into array << int/float/etc. appends

    //-------------------------------------------------------------------------
    // +=> -=> *=> /=>
    //-------------------------------------------------------------------------
    // +=>
    registry->reserve( env->ckt_int, ae_op_plus_chuck, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_plus_chuck, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_plus_chuck, env->ckt_dur );
    registry->reserve( env->ckt_dur, ae_op_plus_chuck, env->ckt_time ); // dur +=> time (e.g., 2::second +=> now)
    registry->reserve( env->ckt_time, ae_op_plus_chuck, env->ckt_dur );
    registry->reserve( env->ckt_complex, ae_op_plus_chuck, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_plus_chuck, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_plus_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_vec3, ae_op_plus_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_plus_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_plus_chuck, env->ckt_vec3, TRUE );
    registry->reserve( env->ckt_vec2, ae_op_plus_chuck, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_vec3, ae_op_plus_chuck, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_object, ae_op_plus_chuck, env->ckt_string ); // object +=> string
    registry->reserve( env->ckt_int, ae_op_plus_chuck, env->ckt_string ); // int/float +=> string
    registry->reserve( env->ckt_float, ae_op_plus_chuck, env->ckt_string ); // string +=> int/float
    registry->reserve( env->ckt_string, ae_op_plus_chuck, env->ckt_int );
    registry->reserve( env->ckt_string, ae_op_plus_chuck, env->ckt_float );
    // -=>
    registry->reserve( env->ckt_int, ae_op_minus_chuck, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_minus_chuck, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_minus_chuck, env->ckt_dur );
    registry->reserve( env->ckt_dur, ae_op_minus_chuck, env->ckt_time );
    registry->reserve( env->ckt_complex, ae_op_minus_chuck, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_minus_chuck, env->ckt_polar );
    registry->reserve( env->ckt_vec2, ae_op_minus_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_vec3, ae_op_minus_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_vec4, ae_op_minus_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_vec2, ae_op_minus_chuck, env->ckt_vec3, TRUE );
    registry->reserve( env->ckt_vec2, ae_op_minus_chuck, env->ckt_vec4, TRUE );
    registry->reserve( env->ckt_vec3, ae_op_minus_chuck, env->ckt_vec4, TRUE );
    // *=>
    registry->reserve( env->ckt_int, ae_op_times_chuck, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_times_chuck, env->ckt_float );
    registry->reserve( env->ckt_complex, ae_op_times_chuck, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_times_chuck, env->ckt_polar );
    registry->reserve( env->ckt_float, ae_op_times_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_float, ae_op_times_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_float, ae_op_times_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_float, ae_op_times_chuck, env->ckt_dur );
    registry->reserve( env->ckt_int, ae_op_times_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_int, ae_op_times_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_int, ae_op_times_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_int, ae_op_times_chuck, env->ckt_dur );
    // /=>
    registry->reserve( env->ckt_int, ae_op_divide_chuck, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_divide_chuck, env->ckt_float );
    registry->reserve( env->ckt_float, ae_op_divide_chuck, env->ckt_dur );
    registry->reserve( env->ckt_float, ae_op_divide_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_float, ae_op_divide_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_float, ae_op_divide_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_int, ae_op_divide_chuck, env->ckt_vec2 );
    registry->reserve( env->ckt_int, ae_op_divide_chuck, env->ckt_vec3 );
    registry->reserve( env->ckt_int, ae_op_divide_chuck, env->ckt_vec4 );
    registry->reserve( env->ckt_int, ae_op_divide_chuck, env->ckt_dur );
    registry->reserve( env->ckt_complex, ae_op_divide_chuck, env->ckt_complex );
    registry->reserve( env->ckt_polar, ae_op_divide_chuck, env->ckt_polar );

    //-------------------------------------------------------------------------
    // &=> |=> ^=> >>=> <<=> %=>
    //-------------------------------------------------------------------------
    registry->reserve( env->ckt_int, ae_op_s_and_chuck, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_s_or_chuck, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_s_xor_chuck, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_shift_right_chuck, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_shift_left_chuck, env->ckt_int );
    registry->reserve( env->ckt_int, ae_op_percent_chuck, env->ckt_int );
    registry->reserve( env->ckt_float, ae_op_percent_chuck, env->ckt_float );
    registry->reserve( env->ckt_dur, ae_op_percent_chuck, env->ckt_dur );

    //-------------------------------------------------------------------------
    // prefix ++ -- - ~ ! new
    //-------------------------------------------------------------------------
    registry->reserve( NULL, ae_op_plusplus, env->ckt_int );
    registry->reserve( NULL, ae_op_plusplus, env->ckt_float );
    registry->reserve( NULL, ae_op_minusminus, env->ckt_int );
    registry->reserve( NULL, ae_op_minusminus, env->ckt_float );
    registry->reserve( NULL, ae_op_minus, env->ckt_int );
    registry->reserve( NULL, ae_op_minus, env->ckt_float );
    registry->reserve( NULL, ae_op_tilda, env->ckt_int );
    registry->reserve( NULL, ae_op_exclamation, env->ckt_int );
    registry->reserve( NULL, ae_op_new, env->ckt_object );

    //-------------------------------------------------------------------------
    // postfix ++ --
    //-------------------------------------------------------------------------
    registry->reserve( env->ckt_int, ae_op_plusplus, NULL );
    registry->reserve( env->ckt_float, ae_op_plusplus, NULL );
    registry->reserve( env->ckt_int, ae_op_minusminus, NULL );
    registry->reserve( env->ckt_float, ae_op_minusminus, NULL );

    // important: preserve all entries (or they will be cleared on next reset)
    registry->preserve();
}




//-----------------------------------------------------------------------------
// name: type_engine_init_op_overload() | 1.5.1.5 (ge) added
// desc: initialize operator overload
//       NOTE this is typically called from init_type_system()
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_init_op_overload( Chuck_Env * env )
{
    EM_log( CK_LOG_HERALD, "initializing operator mappings..." );
    EM_pushlog();

    // the registry
    Chuck_Op_Registry * registry = &env->op_registry;

    // for each overloadable operator, create semantics
    registry->add( ae_op_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_plus )->configure( TRUE, false, false );
    registry->add( ae_op_minus )->configure( TRUE, false, false );
    registry->add( ae_op_times )->configure( TRUE, false, false );
    registry->add( ae_op_divide )->configure( TRUE, false, false );
    registry->add( ae_op_percent )->configure( TRUE, false, false );
    registry->add( ae_op_eq )->configure( TRUE, false, false );
    registry->add( ae_op_neq )->configure( TRUE, false, false );
    registry->add( ae_op_lt )->configure( TRUE, false, false );
    registry->add( ae_op_le )->configure( TRUE, false, false );
    registry->add( ae_op_gt )->configure( TRUE, false, false );
    registry->add( ae_op_ge )->configure( TRUE, false, false );
    registry->add( ae_op_and )->configure( TRUE, false, false );
    registry->add( ae_op_or )->configure( TRUE, false, false );
    registry->add( ae_op_assign )->configure( false, false, false );
    registry->add( ae_op_exclamation )->configure( false, TRUE, false );
    registry->add( ae_op_s_or )->configure( TRUE, false, false );
    registry->add( ae_op_s_and )->configure( TRUE, false, false );
    registry->add( ae_op_s_xor )->configure( TRUE, false, false );
    registry->add( ae_op_plusplus )->configure( false, TRUE, TRUE );
    registry->add( ae_op_minusminus )->configure( false, TRUE, TRUE );
    registry->add( ae_op_dollar )->configure( TRUE, false, false );
    registry->add( ae_op_at_at )->configure( TRUE, false, false );
    registry->add( ae_op_plus_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_minus_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_times_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_divide_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_s_and_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_s_or_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_s_xor_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_shift_right_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_shift_left_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_percent_chuck )->configure( TRUE, false, false );
    registry->add( ae_op_shift_right )->configure( TRUE, false, false );
    registry->add( ae_op_shift_left )->configure( TRUE, false, false );
    registry->add( ae_op_tilda )->configure( false, false, false );
    registry->add( ae_op_new )->configure( false, false, false );
    registry->add( ae_op_coloncolon )->configure( TRUE, false, false );
    registry->add( ae_op_at_chuck )->configure( false, false, false );
    registry->add( ae_op_unchuck )->configure( TRUE, false, false );
    registry->add( ae_op_upchuck )->configure( TRUE, false, false );
    registry->add( ae_op_downchuck )->configure( TRUE, TRUE, TRUE );
    registry->add( ae_op_arrow_right )->configure( TRUE, false, false );
    registry->add( ae_op_arrow_left )->configure( TRUE, false, false );
    registry->add( ae_op_gruck_right )->configure( TRUE, false, false );
    registry->add( ae_op_gruck_left )->configure( TRUE, false, false );
    registry->add( ae_op_ungruck_right )->configure( TRUE, false, false );
    registry->add( ae_op_ungruck_left )->configure( TRUE, false, false );

    // mark built-in overload
    type_engine_init_op_overload_builtin( env );

    // pop log
    EM_poplog();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan_func_op_overload() | 1.5.1.5 (ge) added
// desc: verify an operator overload
//       NOTE this is typically called from scan2_func_def()
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan_func_op_overload( Chuck_Env * env, a_Func_Def f )
{
    // get the operator being overloaded
    ae_Operator op = f->op2overload;
    // if the func wasn't an op overload, then no problem
    if( op == ae_op_none ) return TRUE;

    // get operator semantics
    Chuck_Op_Semantics * semantics = env->op_registry.lookup( op );
    // make sure there is entry
    if( !semantics )
    {
        // error
        EM_error2( f->operWhere, "operator '%s' is not overloadable...", op2str(op) );
        return FALSE;
    }

    // count the number of arguments
    t_CKUINT numArgs = 0; a_Arg_List args = f->arg_list;
    while( args ) { numArgs++; args = args->next; }
    // get the LHS and RHS
    Chuck_Type * LHS = NULL;
    Chuck_Type * RHS = NULL;
    // set to front
    args = f->arg_list;

    // check if valid # of args
    if( numArgs > 2 )
    {
        // error
        EM_error2( f->operWhere, "too many arguments for overloading operator '%s' ...", op2str(op) );
        return FALSE;
    }
    // 2-arg (binary)
    else if( numArgs == 2 )
    {
        // check for unary post
        // unary postfix
        if( f->overload_post )
        {
            // error
            EM_error2( f->operWhere, "invalid number of arguments for overloading operator '%s' as postfix...", op2str(op) );
            EM_error2( 0, "...(hint: binary operators cannot be postfix; all postfix operators are unary)" );
            return FALSE;
        }

        // check if allowed by semantics
        if( !semantics->isBinaryOL() )
        {
            EM_error2( f->operWhere, "operator '%s' cannot be overloaded as binary operator...", op2str(op) );
            return FALSE;
        }

        // set LHS and RHS (since this is binary operator)
        LHS = args->type;
        RHS = args->next->type;
    }
    // 1-arg (unary)
    else if( numArgs == 1)
    {
        // check for post, e.g., x++
        if( f->overload_post )
        {
            // check if allowed by semantics
            if( !semantics->isUnaryPostOL() )
            {
                EM_error2( f->operWhere, "operator '%s' cannot be overloaded as unary (postfix) operator...", op2str(op) );
                return FALSE;
            }
            // set LHS (since this is unary POST)
            LHS = args->type;
        }
        else // pre, e.g., ++x
        {
            // check if allowed by semantics
            if( !semantics->isUnaryPreOL() )
            {
                EM_error2( f->operWhere, "operator '%s' cannot be overloaded as unary (prefix) operator...", op2str(op) );
                return FALSE;
            }
            // set RHS (since this is unary PRE)
            RHS = args->type;
        }
    }

    // get origin hint
    ckte_Origin originHint = ckte_origin_UNKNOWN;
    // get compiler
    Chuck_Compiler * compiler = env->compiler();
    if( compiler != NULL ) originHint = compiler->m_originHint;
    // origin string
    string originStr = env->context ? env->context->filename + ":" : "";

    // add overload
    return env->op_registry.add_overload( LHS, op, RHS, f->ck_func,
                                          originHint, originStr + S_name(f->name), (t_CKINT)f->operWhere,
                                          f->func_decl == ae_key_public );
}




//-----------------------------------------------------------------------------
// name: type_engine_check_func_op_overload()
// desc: type-check an operator overload | 1.5.1.5 (ge) added
//       NOTE this is typically called from check_func_def()
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_func_op_overload( Chuck_Env * env, ae_Operator op, a_Func_Def func_def )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Type()
// desc: instantiate new chuck type
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Context::new_Chuck_Type( Chuck_Env * env )
{
    // allocate
    Chuck_Type * theType = new Chuck_Type( env );
    if( !theType ) return NULL;

    // remember in context | 1.5.1.1
    this->new_types.push_back( theType ); CK_SAFE_ADD_REF( theType );

    // check if t_class has itself been initialized
    // this is only for types created before Type (t_class) initialized,
    // which should only be for the base Object type, which needs to be
    // initialize before the Type type is initialize, becaues Type is
    // a subclass of Object | 1.5.0.0 (ge) brain is bad
    if( env->ckt_class->info != NULL )
    {
        // initialize it as Type object | 1.5.0.0 (ge) added
        initialize_object( theType, env->ckt_class, NULL, env->vm() );
    }

    return theType;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Value()
// desc: instantiate new chuck value
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Context::new_Chuck_Value( Chuck_Type * t,
                                              const string & name )
{
    // allocate
    Chuck_Value * theValue = new Chuck_Value( t, name );
    if( !theValue ) return NULL;

    // remember in context | 1.5.1.1
    this->new_values.push_back( theValue ); CK_SAFE_ADD_REF( theValue );

    return theValue;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Func()
// desc: instantiate new chuck func
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Context::new_Chuck_Func()
{
    // allocate
    Chuck_Func * theFunc = new Chuck_Func;
    if( !theFunc ) return NULL;
    // remember in context | 1.5.1.1
    this->new_funcs.push_back( theFunc ); CK_SAFE_ADD_REF( theFunc );

    return theFunc;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Namespace()
// desc: instantiate new chuck namespace
//-----------------------------------------------------------------------------
Chuck_Namespace * Chuck_Context::new_Chuck_Namespace()
{
    // allocate
    Chuck_Namespace * theNspc = new Chuck_Namespace;
    if( !theNspc ) return NULL;
    // remember in context | 1.5.1.1
    this->new_nspc.push_back( theNspc ); CK_SAFE_ADD_REF( theNspc );

    return theNspc;
}




//-----------------------------------------------------------------------------
// name: new_array_type()
// desc: instantiate new chuck type for some kind of array
//-----------------------------------------------------------------------------
Chuck_Type * new_array_type( Chuck_Env * env, Chuck_Type * array_parent,
                             t_CKUINT depth, Chuck_Type * base_type,
                             Chuck_Namespace * owner_nspc )
{
    // make new type
    Chuck_Type * t = env->context->new_Chuck_Type( env );
    // 1.4.2.0 (ge) | added
    CK_SAFE_ADD_REF(t);
    // set the id
    t->xid = te_array;
    // set the name
    t->base_name = base_type->base_name;

    // add entire type heirarchy to t
    Chuck_Type * base_curr = base_type->parent;

    // 1.4.1.1 (nshaheed) added to allow declaring arrays with subclasses as elements (PR #211)
    // example: [ new SinOsc, new Sinosc ] @=> Osc arr[]; // this previously would fail type check
    Chuck_Type * t_curr = t;
    while( base_curr != NULL )
    {
        Chuck_Type * new_parent = new_array_element_type( env, base_curr, depth, owner_nspc );
        t_curr->parent = new_parent;
        CK_SAFE_ADD_REF(t_curr->parent );

        base_curr = base_curr->parent;
        t_curr = t_curr->parent;
    }
    // ???
    t_curr->parent = array_parent;
    // add reference
    CK_SAFE_ADD_REF(t_curr->parent);

    // is a ref
    t->size = array_parent->size;
    // propagate object size | 1.5.0.0 (ge) added
    t->obj_size = array_parent->obj_size;
    // set the array depth
    t->array_depth = depth;
    // set the base type
    t->array_type = base_type;
    // add reference
    CK_SAFE_ADD_REF(t->array_type);
    // set the namesapce
    t->info = array_parent->info;
    // add reference
    CK_SAFE_ADD_REF(t->info);
    // set owner
    t->owner = owner_nspc; CK_SAFE_ADD_REF(t->owner);

    // return the type
    return t;
}




//-----------------------------------------------------------------------------
// name: new_array_element_type()
// desc: instantiate new chuck type for use in arrays (nshaheed) added
//-----------------------------------------------------------------------------
Chuck_Type * new_array_element_type( Chuck_Env * env, Chuck_Type * base_type,
                                     t_CKUINT depth, Chuck_Namespace * owner_nspc)
{
    // make new type
    Chuck_Type * t = env->context->new_Chuck_Type( env );

    // set the id
    t->xid = te_array;
    // set the name
    t->base_name = base_type->base_name;
    // set the size
    t->size = base_type->size;
    // set the array depth
    t->array_depth = depth;
    // set actual type (for equality checking)
    t->actual_type = base_type;
    // set the array type
    if (base_type->array_type != NULL) {
      t->array_type = base_type->array_type;
      CK_SAFE_ADD_REF(t->array_type);
    }
    // set the namespace
    if (base_type->info != NULL) {
      t->info = base_type->info;
      CK_SAFE_ADD_REF(t->info);
    }
    // set owner
    t->owner = owner_nspc; CK_SAFE_ADD_REF(t->owner);

    // return the type
    return t;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Namespace()
// desc: instantiate new chuck namespace
//-----------------------------------------------------------------------------
t_CKBOOL verify_array( a_Array_Sub array )
{
    // verify there were no errors from the parser...
    if( array->err_num )
    {
        if( array->err_num == 1 )
        {
            EM_error2( array->where,
                "invalid format for array init [...][...]" );
            return FALSE;
        }
        else if( array->err_num == 2 )
        {
            EM_error2( array->where,
                "partially empty array init [...][]" );
            return FALSE;
        }
        else
        {
            EM_error2( array->where,
                "(internal error) unrecognized array error" );
            return FALSE;
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: str2list()
// desc: convert str to list
//-----------------------------------------------------------------------------
a_Id_List str2list( const string & thePath )
{
    t_CKUINT dummy;
    return str2list( thePath, dummy );
}




//-----------------------------------------------------------------------------
// name: str2list()
// desc: convert str to list
//-----------------------------------------------------------------------------
a_Id_List str2list( const string & thePath, t_CKUINT & array_depth )
{
    t_CKINT len = thePath.length();
    t_CKINT i, j;
    string curr;
    a_Id_List list = NULL;
    char last = '\0';

    // look for []
    array_depth = 0;
    while( len > 2 && thePath[len-1] == ']' && thePath[len-2] == '[' )
    {
        // flag it
        array_depth++;
        // shorten len
        len -= 2;
    }

    // loop backwards
    for( i = len - 1; i >= 0; i-- )
    {
        char c = thePath[i];
        // if not .
        if( c != '.' )
        {
            // check to make sure valid
            if( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' )
                || ( c == '_' ) || ( c >= '0' && c <= '9' ) )
            {
                // add
                curr += c;
            }
            else
            {
                // error
                EM_error2( 0, "illegal character '%c' in path '%s'",
                    c, thePath.c_str() );
                // clean up locally created id list
                delete_id_list( list );
                return NULL;
            }
        }

        // add
        if( c == '.' || i == 0 )
        {
            // make sure valid
            if( (i != 0 && last != '.' && last != '\0') ||
                (i == 0 && c != '.') )
            {
                char s;
                t_CKINT size = curr.length();
                // reverse
                for( j = 0; j < size/2; j++ )
                {
                    // swap
                    s = curr[j];
                    curr[j] = curr[size-j-1];
                    curr[size-j-1] = s;
                }
                // make a new id and put in list
                list = prepend_id_list( (char *)curr.c_str(), list, 0, 0 /*, NULL */ );
                // clear
                curr = "";
            }
            else
            {
                // error
                EM_error2( 0, "path '%s' must not begin or end with '.'",
                    thePath.c_str() );
                // clean up locally created id list
                delete_id_list( list );
                return NULL;
            }
        }

        // remember last
        last = c;
    }

    return list;
}




//-----------------------------------------------------------------------------
// name: partial_deep_copy_args()
// desc: partial deep copy of an arg list AST node
//-----------------------------------------------------------------------------
a_Arg_List partial_deep_copy_args( a_Arg_List list )
{
    if( !list ) return NULL;

    a_Arg_List copy = NULL;
    a_Var_Decl var_decl = NULL;

    // need name but not 'array' on var_decl
    // 1.5.2.0 (ge) added ctor_args=NULL, also not needed from the callee's perspective
    var_decl = new_var_decl( S_name(list->var_decl->xid), FALSE, NULL, NULL, list->var_decl->line, list->var_decl->where );
    // need var_decl by not type_decl
    copy = new_arg_list( NULL, var_decl, list->line, list->where );
    // set type and add reference
    copy->type = list->type; CK_SAFE_ADD_REF( copy->type );

    // go down the list
    copy->next = partial_deep_copy_args( list->next );

    // return
    return copy;
}




//-----------------------------------------------------------------------------
// name: partial_deep_copy()
// desc: partial deep copy of a func def AST node; useful and safer because
//       the original AST node will be cleaned up after compilation
//-----------------------------------------------------------------------------
a_Func_Def partial_deep_copy_fn( a_Func_Def f )
{
    a_Func_Def copy = new_func_def( f->func_decl,
                                    f->static_decl,
                                    NULL,
                                    S_name(f->name),
                                    partial_deep_copy_args(f->arg_list),
                                    NULL,
                                    FALSE, // from AST
                                    f->line,
                                    f->where );
    // copy ret_type and reference count
    copy->ret_type = f->ret_type; CK_SAFE_ADD_REF( copy->ret_type );
    // copy ck_func and reference count
    copy->ck_func = f->ck_func; CK_SAFE_ADD_REF( copy->ck_func );

    // copy DL func pointers
    copy->dl_func_ptr = f->dl_func_ptr;
    // copy other data
    copy->global = f->global;
    copy->stack_depth = f->stack_depth;

    return copy;
}




//-----------------------------------------------------------------------------
// name: make_dll_arg_list()
// desc: make an chuck dll function into arg list
//-----------------------------------------------------------------------------
a_Arg_List make_dll_arg_list( Chuck_DL_Func * dl_fun )
{
    a_Arg_List arg_list = NULL;
    a_Type_Decl type_decl = NULL;
    a_Var_Decl var_decl = NULL;
    a_Id_List type_path = NULL;
    a_Id_List name_path = NULL;
    a_Array_Sub array_sub = NULL;
    Chuck_DL_Value * arg = NULL;
    t_CKUINT array_depth = 0;
    t_CKUINT array_depth2 = 0;
    t_CKINT i = 0;

    // loop backwards
    for( i = dl_fun->args.size() - 1; i >= 0; i-- )
    {
        // clear array
        // added 1.3.2.0 - spencer
        array_depth = array_depth2 = 0;
        array_sub = NULL;

        // copy into variable
        arg = dl_fun->args[i];

        // path
        type_path = str2list( arg->type, array_depth );
        if( !type_path )
        {
            // error
            EM_error2( 0, "...at argument '%i'", i+1 );
            // delete the arg list
            // delete_arg_list( arg_list );
            return NULL;
        }

        // type
        type_decl = new_type_decl( type_path, FALSE, 0, 0 );
        // TODO: fix this
        assert( type_decl );

        // var
        name_path = str2list( arg->name, array_depth2 );


        // sanity
        if( array_depth && array_depth2 )
        {
            // error
            EM_error2( 0, "array subscript specified incorrectly for built-in module" );
            // TODO: cleanup
            return NULL;
        }

        // TODO: arrays?
        if( array_depth2 ) array_depth = array_depth2;
        if( array_depth )
        {
            array_sub = new_array_sub( NULL, 0, 0 );

            for( int j = 1; j < array_depth; j++ )
                array_sub = prepend_array_sub( array_sub, NULL, 0, 0 );
        }

        // make var decl
        // 1.5.2.0 (ge) add ctor arglist (NULL); consider enable+refactor to support ctor
        var_decl = new_var_decl( (char *)arg->name.c_str(), FALSE, NULL, array_sub, 0, 0 );

        // make new arg
        arg_list = prepend_arg_list( type_decl, var_decl, arg_list, 0, 0 );
    }

    return arg_list;
}




//-----------------------------------------------------------------------------
// name: make_dll_as_fun()
// desc: make an chuck dll function into absyn function
//-----------------------------------------------------------------------------
a_Func_Def make_dll_as_fun( Chuck_DL_Func * dl_fun,
                            t_CKBOOL is_static, t_CKBOOL is_primitive )
{
    a_Func_Def func_def = NULL;
    ae_Keyword func_decl = ae_key_func;
    ae_Keyword static_decl = ae_key_instance;
    a_Id_List type_path = NULL;
    a_Type_Decl type_decl = NULL;
    const char * name = NULL;
    a_Arg_List arg_list = NULL;
    t_CKUINT array_depth = 0;

    // fun decl TODO: fix this
    func_decl = ae_key_func;
    // static decl TODO: allow abstract
    static_decl = is_static ? ae_key_static : ae_key_instance;
    // path
    type_path = str2list( dl_fun->type, array_depth );
    if( !type_path )
    {
        // error
        EM_error2( 0, "...during function import '%s' (type)",
            dl_fun->name.c_str() );
        goto error;
    }

    // type decl
    // old: type_decl = new_type_decl( type_path, 1, 0 );
    type_decl = new_type_decl( type_path, 0, 0, 0 );
    assert( type_decl );
    if( !type_decl )
    {
        // error
        EM_error2( 0, "...during function import '%s' (type2)",
            dl_fun->name.c_str() );
        // clean up locally created id list
        delete_id_list( type_path );
        type_path = NULL;
        goto error;
    }

    // array types
    // this allows us to define built-in functions that return array types
    // however doing this without garbage collection is probably a bad idea
    // -spencer
    if( array_depth )
    {
        a_Array_Sub array_sub = new_array_sub( NULL, 0, 0 );

        for( int i = 1; i < array_depth; i++ )
            array_sub = prepend_array_sub( array_sub, NULL, 0, 0 );

        type_decl = add_type_decl_array( type_decl, array_sub, 0, 0 );
    }

    // name of the function
    name = dl_fun->name.c_str();
    // arg list
    arg_list = make_dll_arg_list( dl_fun );
    if( dl_fun->args.size() > 0 && !arg_list )
    {
        // error
        EM_error2( 0, "...during function import '%s' (arg_list)",
            dl_fun->name.c_str() );
        // delete type_decl
        // delete_type_decl( type_decl );
        type_decl = NULL;
        goto error;
    }

    // make a func_def | added from_ast=FALSE 1.5.0.5
    func_def = new_func_def( func_decl, static_decl, type_decl,
                             (char *)name, arg_list, NULL, FALSE, 0, 0 );
    // mark the function as imported (instead of defined in ChucK)
    func_def->s_type = ae_func_builtin;
    // copy the function pointer - the type doesn't matter here
    // ...since we copying into a void * - so addr is used
    func_def->dl_func_ptr = (void *)dl_fun->addr;
    // copy the function pointer kind | 1.5.2.0
    func_def->dl_fp_kind = dl_fun->fpKind;
    // copy the operator overload info | 1.5.1.5
    func_def->op2overload = dl_fun->op2overload;
    // set if unary postfix overload | 1.5.1.5
    func_def->overload_post = (dl_fun->opOverloadKind == ckte_op_overload_UNARY_POST);

    return func_def;

error:

    // clean up
    // if( !func_def ) delete_type_decl( type_decl );
    // else delete_func_def( func_def );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_add_dll()
// desc: add an chuck dll into the env
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_add_dll( Chuck_Env * env, Chuck_DLL * dll, const string & dest )
{
    // which namespace
    string where = ( dest == "" ? "global" : dest );
    Chuck_Namespace * nspc = NULL;
    const Chuck_DL_Query * query = NULL;
    t_CKUINT i, j;
    vector<a_Func_Def> the_funs;

    // convert to id list
    a_Id_List thePath = str2list( dest );
    if( !thePath ) goto error;

    // find the namespace to put the import
    nspc = type_engine_find_nspc( env, thePath );
    if( !nspc ) goto error;

    // get the query
    query = dll->query();

    // loop
    for( i = 0; i < query->classes.size(); i++ )
    {
        a_Class_Def def = NULL;
        a_Class_Ext ext = NULL;
        a_Class_Body body = NULL;
        // UNUSED: a_Section sec = NULL;
        a_Func_Def fun = NULL;
        a_Id_List name;
        a_Id_List parent;
        // UNUSED: Chuck_DL_Func * dl_fun = NULL;
        // UNUSED: Chuck_DL_Value * dl_val = NULL;
        // UNUSED: Chuck_DL_Ctrl * dl_ctrl = NULL;
        Chuck_DL_Class * cl = query->classes[i];
        assert( cl != NULL );

        // get name
        name = str2list( cl->name );
        if( !name ) goto error;
        // if parent is specified
        if( cl->parent != "" )
        {
            // get parent
            parent = str2list( cl->parent );
            if( !parent ) goto error;
        }
        else // if no parent specified, then extend Object
        {
            parent = str2list( "Object" );
            assert( parent != NULL );
        }

        // clear the funs vector
        the_funs.clear();

        // loop over member functions
        for( j = 0; j < cl->mfuns.size(); j++ )
        {
            // get the function from the dll
            fun = make_dll_as_fun( cl->mfuns[j], FALSE, FALSE );
            if( !fun ) goto error;
            // add to vector
            the_funs.push_back( fun );
        }

        // loop over static functions
        for( j = 0; j < cl->sfuns.size(); j++ )
        {
            // get the function from the dll
            fun = make_dll_as_fun( cl->sfuns[j], TRUE, FALSE );
            if( !fun ) goto error;
            // add to vector
            the_funs.push_back( fun );
        }

        // loop over member data
        // ignored for now...-spencer
        for( j = 0; j < cl->mvars.size(); j++ )
        {
        }

        // the next few lines take static member variables defined by the DLL
        // and creates a list of corresponding declarations to add to the
        // class definition
        // -spencer

        // static member variable declarations
        a_Stmt_List svar_decls = NULL;

        // loop over static data
        for( j = 0; j < cl->svars.size(); j++ )
        {
            // get type
            a_Id_List thePath2 = str2list( cl->svars[j]->type.c_str() );
            // make type decl
            a_Type_Decl type_decl = new_type_decl( thePath2, FALSE, 0, 0 );
            // make var decl
            // 1.5.2.0 (ge) add ctor arglist (NULL); consider enable+refactor to support ctor
            a_Var_Decl var_decl = new_var_decl( cl->svars[j]->name.c_str(), FALSE, NULL, NULL, 0, 0 );
            // make var decl list
            a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0, 0 );
            // make exp decl
            a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, TRUE, (int)cl->svars[j]->is_const, 0, 0 );
            // add addr
            var_decl->addr = (void *)cl->svars[j]->static_addr;
            // append exp stmt to stmt list
            svar_decls = append_stmt_list( svar_decls, new_stmt_from_expression( exp_decl, 0, 0 ), 0, 0 );
        }

        // if there are any declarations, prepend them to body
        if( svar_decls )
            body = prepend_class_body( new_section_stmt( svar_decls, 0, 0 ), body, 0, 0 );

        // go through funs backwards, and prepend
        for( t_CKINT k = (t_CKINT)the_funs.size() - 1; k >= 0; k-- )
        {
            // add to body
            body = prepend_class_body( new_section_func_def( the_funs[k], 0, 0 ), body, 0, 0 );
        }

        // construct class
        def = new_class_def( ae_key_public, name, ext, body, 0, 0 );
        // set where to add
        def->home = nspc;
        // TODO: mark the class as dll import?

        // scan type check it
        if( !type_engine_scan0_class_def( env, def ) ) goto error;
        if( !type_engine_scan1_class_def( env, def ) ) goto error;
        if( !type_engine_scan2_class_def( env, def ) ) goto error;
        if( !type_engine_check_class_def( env, def ) ) goto error;

        // TODO: clean up?
    }

    // clean up locally created id list
    delete_id_list( thePath );

    return TRUE;

error:
    // error
    EM_error2( 0, "...(in object import '%s' in DLL '%s')",
        query ? ( query->dll_name == "" ? query->dll_name.c_str() : "[empty]" ) : "[null]", dll->name() );

    // clean up locally created id list
    delete_id_list( thePath );

    // TODO: free the absyn stuff?

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_add_dll2()
// desc: add the DLL using type_engine functions (added 1.3.0.0)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_add_dll2( Chuck_Env * env, Chuck_DLL * dll,
                               const string &  )
{
    const Chuck_DL_Query * query = NULL;

    // get the query
    query = dll->query();

    for( int i = 0; i < query->classes.size(); i++ )
    {
        if( !type_engine_add_class_from_dl(env, query->classes[i]) )
        {
            EM_log(CK_LOG_HERALD,
                   TC::orange("error importing class '%s' from chugin (%s)",true).c_str(),
                   query->classes[i]->name.c_str(), dll->name());

            return FALSE;
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_add_class_from_dl()
// desc: add the DLL using type_engine functions (added 1.3.0.0)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_add_class_from_dl( Chuck_Env * env, Chuck_DL_Class * c )
{
    Chuck_DL_Func * ctor = NULL, * dtor = c->dtor;

    // test name | 1.5.1.3 (ge) added
    if( trim(c->name) == "" )
    {
        // error
        EM_log( CK_LOG_SYSTEM, TC::orange("imported chugin class with no name...").c_str() );
        // if more info
        if( c->hint_dll_filepath != "" )
            EM_log( CK_LOG_SYSTEM, TC::orange(" |- (origin: %s)").c_str(), c->hint_dll_filepath.c_str() );
        // before ugen begin, can just return
        return FALSE;
    }

    // if no parent name, set parent to "Object" | 1.5.1.3
    if( trim(c->parent) == "" ) c->parent = env->ckt_object->name();

    // check for duplicates | 1.5.0.0 (ge) added
    if( type_engine_find_type( env, c->name ) )
    {
        EM_log( CK_LOG_SYSTEM, TC::orange("(error) importing class '%s' from chugin...").c_str(), c->name.c_str() );
        EM_log( CK_LOG_SYSTEM, TC::orange(" |- type with the same name already exists").c_str() );
        // more info?
        if( c->hint_dll_filepath != "" )
        {
            // log the path
            EM_log( CK_LOG_SYSTEM, TC::orange(" |- (duplicate: %s)").c_str(), c->hint_dll_filepath.c_str() );
        }
        // before ugen begin, can just return
        return FALSE;
    }

    // check this wasn't added as an UGen without a tick | 1.5.1.3 (ge and nshaheed) added
    Chuck_Type * parent = type_engine_find_type( env, c->parent );
    // e.g., a chugin that named a UGen as parent but didn't call QUERY->add_ugen_func()
    if( (!c->ugen_tick && !c->ugen_tickf) && parent && isa(parent,env->ckt_ugen) )
    {
        // error
        EM_log( CK_LOG_SYSTEM, TC::orange("(error) imported class '%s' extends 'UGen'...").c_str(), c->name.c_str() );
        EM_log( CK_LOG_SYSTEM, TC::orange(" |- but does not contain UGen functions (e.g., tick)").c_str() );
        EM_log( CK_LOG_SYSTEM, TC::orange(" |- (hint: chugin missing QUERY->add_ugen_func()?)").c_str(), c->name.c_str() );
        // if more info
        if( c->hint_dll_filepath != "" )
            EM_log( CK_LOG_SYSTEM, TC::orange(" |- (origin: %s)").c_str(), c->hint_dll_filepath.c_str() );
        // before ugen begin, can just return
        return FALSE;
    }

    // sort constructors
    sort( c->ctors.begin(), c->ctors.end(), ck_comp_dl_func_args );

    // check constructor(s)
    for( t_CKUINT i = 0; i < c->ctors.size(); i++ )
    {
        // find default constructor
        if( c->ctors[i]->args.size() == 0 )
        {
            // remember it
            ctor = c->ctors[i];
            // should be only one
            break;
        }
    }

    // check whether to import as UGen or other
    if( (c->ugen_tick || c->ugen_tickf) && c->ugen_num_out )
    {
        // begin import as ugen
        if( !type_engine_import_ugen_begin( env, c->name.c_str(),
                                            c->parent.c_str(), env->global(),
                                            NULL, // ctor ? ctor->ctor : NULL, // ctors from DL added as type_engine_import_ctor()
                                            dtor ? dtor->dtor : NULL,
                                            c->ugen_tick, c->ugen_tickf, c->ugen_pmsg,
                                            c->ugen_num_in, c->ugen_num_out,
                                            c->doc.length() > 0 ? c->doc.c_str() : NULL ) )
            goto error;
    }
    else
    {
        // begin import as normal class (non-ugen)
        if( !type_engine_import_class_begin( env, c->name.c_str(),
                                             c->parent.c_str(), env->global(),
                                             NULL, // ctor ? ctor->ctor : NULL, // ctors from DL added as type_engine_import_ctor()
                                             dtor ? dtor->dtor : NULL,
                                             c->doc.length() > 0 ? c->doc.c_str() : NULL ) )
            goto error;
    }

    int j;

    // import member variables
    for( j = 0; j < c->mvars.size(); j++ )
    {
        Chuck_DL_Value * mvar = c->mvars[j];
        if( type_engine_import_mvar( env, mvar->type.c_str(), mvar->name.c_str(), mvar->is_const,
                                     mvar->doc.size() ? mvar->doc.c_str() : NULL ) == CK_INVALID_OFFSET )
            goto error;
    }

    // import static variables
    for( j = 0; j < c->svars.size(); j++ )
    {
        Chuck_DL_Value * svar = c->svars[j];
        if( !type_engine_import_svar( env, svar->type.c_str(), svar->name.c_str(),
                                      svar->is_const, (t_CKUINT)svar->static_addr,
                                      svar->doc.size() ? svar->doc.c_str() : NULL ) )
            goto error;
    }

    // import constructors
    for( j = 0; j < c->ctors.size(); j++ )
    {
        Chuck_DL_Func * theFunc = c->ctors[j];
        if( !type_engine_import_ctor( env, theFunc ) ) goto error;
    }

    // import member functions
    for( j = 0; j < c->mfuns.size(); j++ )
    {
        Chuck_DL_Func * theFunc = c->mfuns[j];
        if( !type_engine_import_mfun( env, theFunc ) ) goto error;
    }

    // import static functions
    for( j = 0; j < c->sfuns.size(); j++ )
    {
        Chuck_DL_Func * theFunc = c->sfuns[j];
        if( !type_engine_import_sfun( env, theFunc ) ) goto error;
    }

    // import examples (if any)
    for( j = 0; j < c->examples.size(); j++ )
    {
        if( !type_engine_import_add_ex( env, c->examples[j].c_str() ) ) goto error;
    }

    // end class import
    type_engine_import_class_end(env);

    return TRUE;

error:
    type_engine_import_class_end(env);

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_is_base_static() | 1.5.0.0 (ge) added
// desc: check for static func/member access using class; e.g., SinOsc.help()
//       this function was created after adding the complexity of t_class being
//       made available in the language (as the Type type)
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_is_base_static( Chuck_Env * env, Chuck_Type * baseType )
{
    // check
    if( baseType == NULL ) return FALSE;
    // check
    return isa( baseType, env->ckt_class ) && (baseType->actual_type != NULL);
}




static const char * g_howmuch[] = { "ALL", "CLASSES_ONLY", "ALL_EXCEPT_CLASSES" };
//-----------------------------------------------------------------------------
// name: howmuch2str()
// desc: ...
//-----------------------------------------------------------------------------
const char * howmuch2str( te_HowMuch how_much )
{
    if( how_much < 0 || how_much > te_do_no_classes ) return "[INVALID]";
    else return g_howmuch[how_much];
}




// table of escape characters
static char g_escape[256];
static t_CKBOOL g_escape_ready = FALSE;

//-----------------------------------------------------------------------------
// name: escape_table()
// desc: ...
//-----------------------------------------------------------------------------
void escape_table( )
{
    // escape
    g_escape[(t_CKUINT)'\''] = '\'';
    g_escape[(t_CKUINT)'"'] = '"';
    g_escape[(t_CKUINT)'\\'] = '\\';
    g_escape[(t_CKUINT)'a'] = (char)7; // audible bell
    g_escape[(t_CKUINT)'b'] = (char)8; // back space
    g_escape[(t_CKUINT)'f'] = (char)12; // form feed
    g_escape[(t_CKUINT)'n'] = (char)10; // new line
    g_escape[(t_CKUINT)'r'] = (char)13; // carriage return
    g_escape[(t_CKUINT)'t'] = (char)9; // horizontal tab
    g_escape[(t_CKUINT)'v'] = (char)11; // vertical tab

    // done
    g_escape_ready = TRUE;
}




//-----------------------------------------------------------------------------
// name: escape_str()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL escape_str( char * str_lit, int linepos )
{
    // create if not yet
    if( !g_escape_ready )
        escape_table( );

    // write pointer
    char * str = str_lit;
    // unsigned because we index array of 256
    unsigned char c, c2, c3;

    // iterate
    while( *str_lit )
    {
        // if \ encountered
        if( *str_lit == '\\' )
        {
            // advance pointer
            str_lit++;

            // make sure next char
            if( *str_lit == '\0' )
            {
                EM_error2( linepos, "invalid: string ends with escape character '\\'" );
                return FALSE;
            }

            // next characters
            c = *(str_lit);
            c2 = *(str_lit+1);

            // is octal?
            if( c >= '0' && c <= '7' )
            {
                // look at next
                if( c == '0' && ( c2 < '0' || c2 > '7' ) )
                    *str++ = '\0';
                else
                {
                    // get next
                    c3 = *(str_lit+2);

                    // all three should be within range
                    if( c2 >= '0' && c2 <= '7' && c3 >= '0' && c3 <= '7' )
                    {
                        // ascii value
                        *str++ = (c-'0') * 64 + (c2-'0') * 8 + (c3-'0');
                        // advance pointer
                        str_lit += 2;
                    }
                    else
                    {
                        EM_error2( linepos, "malformed octal escape sequence '\\%c%c%c'", c, c2, c3 );
                        return FALSE;
                    }
                }
            }
            else if( c == 'x' ) // is hex?
            {
                EM_error2( linepos, "hex escape sequence not (yet) supported (use octal!)");
                return FALSE;
            }
            else // is other?
            {
                // lookup
                if( g_escape[(int)c] )
                    *str++ = g_escape[c];
                else // error
                {
                    EM_error2( linepos, "unrecognized escape sequence '\\%c'", c );
                    return FALSE;
                }
            }
        }
        else
        {
            // char
            *str++ = *str_lit;
        }

        // advance pointer
        str_lit++;
    }

    // make sure
    assert( str <= str_lit );

    // terminate
    *str = '\0';

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: str2char
// desc: handle escape characters
//-----------------------------------------------------------------------------
t_CKINT str2char( const char * c, int linepos )
{
    if(c[0] == '\\')
    {
        switch(c[1])
        {
            case '0': return '\0';
            case '\'': return '\'';
            case '\\': return '\\';
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return 'v';

            default:
                EM_error2( linepos, "unrecognized escape sequence '\\%c'", c[1] );
                return -1;
        }
    }
    else
    {
        return c[0];
    }
}




//-----------------------------------------------------------------------------
// name: same_arg_lists()
// desc: compare two argument lists to see if they are the same
//       added 1.5.0.0 (ge) to catch duplicate functions
//-----------------------------------------------------------------------------
t_CKBOOL same_arg_lists( a_Arg_List lhs, a_Arg_List rhs )
{
    // go down the list
    while( lhs )
    {
        // if out of arguments, then argument lists are different
        if( !rhs ) return FALSE;
        // if any arguments are different, then argument lists are different
        if( (*lhs->type) != (*rhs->type) ) return FALSE;
        // TODO: if either is a subclass of the other: okay iff there are other differentiating arguments

        // advance
        lhs = lhs->next;
        rhs = rhs->next;
    }

    // at this point, lists are same if rhs is NULL
    return (rhs == NULL);
}




//-----------------------------------------------------------------------------
// name: arglist2string()
// desc: generate a string from an argument list (types only)
//       1.5.0.0 (ge) added
//-----------------------------------------------------------------------------
string arglist2string( a_Arg_List list )
{
    // return value
    string s = list ? " " : "";

    // go down the list
    while( list )
    {
        // concatenate
        s += list->type->base_name;
        // check
        if( list->next ) s += ",";
        // space
        s += " ";
        // advance
        list = list->next;
    }

    return s;
}




//-----------------------------------------------------------------------------
// name: Chuck_Value()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Value::Chuck_Value( Chuck_Type * t, const std::string & n, void * a,
    t_CKBOOL c, t_CKBOOL acc, Chuck_Namespace * o, Chuck_Type * oc, t_CKUINT s )
{
    type = t; CK_SAFE_ADD_REF( type ); // add reference
    name = n; offset = s;
    is_const = c; access = acc;
    owner = o; CK_SAFE_ADD_REF( owner ); // add reference
    owner_class = oc; CK_SAFE_ADD_REF( owner_class ); // add reference
    addr = a; is_member = FALSE;
    is_static = FALSE; is_context_global = FALSE;
    is_decl_checked = TRUE; // only set to false in certain cases
    is_global = FALSE;
    func_ref = NULL; func_num_overloads = 0;
    depend_init_where = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Value()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Value::~Chuck_Value()
{
    // release
    CK_SAFE_RELEASE( type );
    CK_SAFE_RELEASE( owner );
    CK_SAFE_RELEASE( owner_class );
    CK_SAFE_RELEASE( func_ref );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Func()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Func::~Chuck_Func()
{
    // delete our partial deep copy
    funcdef_cleanup();

    // release code | 1.5.1.0 (ge) added
    CK_SAFE_RELEASE( this->code );
    // release value ref
    CK_SAFE_RELEASE( this->value_ref );

    // delete args cache | 1.5.1.5
    CK_SAFE_DELETE_ARRAY( this->args_cache );
    this->args_cache_size = 0;

    // release invoker(s) | 1.5.1.5
    CK_SAFE_DELETE( this->invoker_mfun );
    // release next | 1.5.2.0
    CK_SAFE_RELEASE( this->next );
    // release up | 1.5.2.0
    CK_SAFE_RELEASE( this->up );

    // TODO: check if more references to release?
}




//-----------------------------------------------------------------------------
// name: type()
// desc: get the function's return type
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Func::type() const
{
    // check we have the necessary info
    if( !def() || !def()->ret_type ) return NULL;
    // return it
    return def()->ret_type;
}




//-----------------------------------------------------------------------------
// name: signature()
// desc: human readable function signature: e.g., Object.func( int foo, float bar[] );
// args: incFunDef -- controls whether to include the "fun" in "fun RET FUNC(..."
//       incRetType -- controls whether to include the return type
//-----------------------------------------------------------------------------
string Chuck_Func::signature( t_CKBOOL incFuncDef, t_CKBOOL incRetType ) const
{
    // check we have the necessary info
    if( !value_ref || !def() || !def()->ret_type )
        return "[function signature missing info]";

    // check if a member func
    string className = value_ref->owner_class ? value_ref->owner_class->name() + "." : "";
    // make signature so far
    string signature = className + base_name + "(";
    // the return type
    if( incRetType ) signature = def()->ret_type->name() + " " + signature;
    // the function keyword
    if( incFuncDef ) signature = string("fun") + " " + signature;

    // loop over arguments
    a_Arg_List list = def()->arg_list;
    // add space if there are any arguments
    if( list ) signature += " ";
    // loop
    while( list )
    {
        // arg type
        signature += list->type->base_name + " ";
        // arg name
        signature += S_name( list->var_decl->xid );
        // array
        for( t_CKUINT i = 0; i < list->type->array_depth; i++ )
            signature += string("[]");
        // comma
        if( list->next ) signature += ", ";
        // next
        list = list->next;
    }

    // add space if there are any arguments
    if( def()->arg_list ) signature += " ";
    // close
    signature += ")";

    // done
    return signature;
}




//-----------------------------------------------------------------------------
// name: ownerType()
// desc: get owner type: if func part of a class
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Func::ownerType() const
{
    // check we have the necessary info
    if( !value_ref ) return NULL;
    // return value's owner type
    return value_ref->owner_class;
}




//-----------------------------------------------------------------------------
// name: funcdef_connect()
// desc: connect a Func_Def (called when this func is type checked)
//-----------------------------------------------------------------------------
void Chuck_Func::funcdef_connect( a_Func_Def f )
{
    // cleanup first
    this->funcdef_cleanup();

    if( !f )
    {
        EM_error2( 0, "(internal error): Chuck_Func::funcdef_connect() NULL func def" );
        return;
    }

    // copy | NOTE the func_def could continue to be modified during compilation
    // after compilation, we keep this Func around but make deep copy of this->def
    this->m_def = f;

    // if AST owned, increment the ref
    if( f->ast_owned ) f->vm_refs++;

    // log
    EM_log( CK_LOG_FINEST, "funcdef_connect() for '%s' | AST-owned: %s vm_refs: %s",
            S_name(f->name), f->ast_owned ? "YES" : "NO", f->ast_owned ? ck_itoa(f->vm_refs).c_str() : "N/A" );
}




//-----------------------------------------------------------------------------
// name: funcdef_decouple_ast()
// desc: severe references to AST (called after compilation, before AST cleanup)
// make a fresh partial deep copy of m_def ONLY IF it is owned by AST
//-----------------------------------------------------------------------------
void Chuck_Func::funcdef_decouple_ast()
{
    // make copy
    a_Func_Def f = this->m_def;

    // if NULL
    if( !f ) return;

    // if AST owned
    if( f->ast_owned )
    {
        // make partial deep copy
        this->m_def = partial_deep_copy_fn( f );
        // decrement ref count
        f->vm_refs--;
    }

    // log
    EM_log( CK_LOG_FINEST, "funcdef_decouple_ast() for '%s' | AST-owned: %s vm_refs: %s",
            S_name(f->name), f->ast_owned ? "YES" : "NO", f->ast_owned ? ck_itoa(f->vm_refs).c_str() : "N/A" );
}




//-----------------------------------------------------------------------------
// name: funcdef_decouple_ast()
// desc: cleanup funcdef (if/when this function is cleaned up)
//-----------------------------------------------------------------------------
void Chuck_Func::funcdef_cleanup()
{
    // make copy
    a_Func_Def f = this->m_def;

    // if NULL
    if( !f ) return;

    // log
    EM_log( CK_LOG_FINEST, "funcdef_cleanup() for '%s' | AST-owned: %s vm_refs: %s",
            S_name(f->name), f->ast_owned ? "YES" : "NO", f->ast_owned ? ck_itoa(f->vm_refs).c_str() : "N/A" );

    // if AST owned
    if( f->ast_owned )
    {
        // decrement ref count
        f->vm_refs--;
    }
    else
    {
        // not AST owned, we created this, we delete this
        delete_func_def( f );
    }

    // zero out
    this->m_def = NULL;
}




//-----------------------------------------------------------------------------
// name: pack_cache() | 1.5.1.5
// desc: pack c-style array of DL_Args into args cache
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Func::pack_cache( Chuck_DL_Arg * dlargs, t_CKUINT numArgs )
{
    // data size in bytes
    t_CKUINT size = 0;
    // count the number of bytes needed
    for( t_CKUINT i = 0; i < numArgs; i++ )
        size += dlargs[i].sizeInBytes();
    // (re)allocate if needed
    if( size > args_cache_size )
    {
        CK_SAFE_DELETE_ARRAY( args_cache );
        args_cache = new t_CKBYTE[size];
        if( !args_cache ) {
            EM_error3( "error allocating argument cache of size '%lu'", size );
            return FALSE;
        }
        memset( args_cache, 0, size );
        args_cache_size = size;
    }

    // pointer for copying
    t_CKBYTE * here = args_cache;
    // iterate and copy
    for( t_CKUINT j = 0; j < numArgs; j++ )
    {
        switch( dlargs[j].kind )
        {
            case kindof_INT: memcpy( here, &dlargs[j].value.v_int, sizeof(dlargs[j].value.v_int) ); break;
            case kindof_FLOAT: memcpy( here, &dlargs[j].value.v_float, sizeof(dlargs[j].value.v_float) ); break;
            case kindof_VEC2: memcpy( here, &dlargs[j].value.v_vec2, sizeof(dlargs[j].value.v_vec2) ); break;
            case kindof_VEC3: memcpy( here, &dlargs[j].value.v_vec3, sizeof(dlargs[j].value.v_vec3) ); break;
            case kindof_VEC4: memcpy( here, &dlargs[j].value.v_vec4, sizeof(dlargs[j].value.v_vec4) ); break;

            // shouldn't get here
            case kindof_VOID:
                EM_error3( "(internal error) Chuck_Func.pack_cache() void argument encountered..." ); return FALSE;
        }
    }

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setup_invoker() | 1.5.1.5
// desc: setup invoker for this fun (for calling chuck function from c++)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Func::setup_invoker( t_CKUINT func_vt_offset, Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // if already setup
    if( invoker_mfun != NULL ) return TRUE;
    // check if member function
    if( !this->is_member ) return FALSE;
    // check if needed
    if( this->code->native_func ) return FALSE;
    // instantiate
    invoker_mfun = new Chuck_VM_MFunInvoker;
    // set up invoker
    invoker_mfun->setup( this, func_vt_offset, vm, shred );
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_Value_Dependency()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Value_Dependency::Chuck_Value_Dependency( Chuck_Value * argValue, t_CKUINT argUseWhere )
    : value(NULL)
{
    CK_SAFE_REF_ASSIGN( value, argValue );
    use_where = argUseWhere;
    // for convenience
    where = value ? value->depend_init_where : 0;
}




//-----------------------------------------------------------------------------
// name: Chuck_Value_Dependency()
// desc: copy constructor; DANGER: needed to properly ref-count
//-----------------------------------------------------------------------------
Chuck_Value_Dependency::Chuck_Value_Dependency( const Chuck_Value_Dependency & rhs )
    : value(NULL)
{
    CK_SAFE_REF_ASSIGN( value, rhs.value );
    use_where = rhs.use_where;
    // for convenience
    where = value ? value->depend_init_where : 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Value_Dependency()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Value_Dependency::~Chuck_Value_Dependency()
{
    // release if needed
    CK_SAFE_RELEASE( value );
}




//-----------------------------------------------------------------------------
// name: add()
// desc: add a direct dependency
//-----------------------------------------------------------------------------
void Chuck_Value_Dependency_Graph::add( const Chuck_Value_Dependency & dep )
{
    directs.push_back( dep );
}




//-----------------------------------------------------------------------------
// name: add()
// desc: add a remote (recursive) dependency
//-----------------------------------------------------------------------------
void Chuck_Value_Dependency_Graph::add( Chuck_Value_Dependency_Graph * graph )
{
    remotes.push_back( graph );
}




//-----------------------------------------------------------------------------
// name: locate()
// desc: locate dependency non-recursive
//-----------------------------------------------------------------------------
const Chuck_Value_Dependency * Chuck_Value_Dependency_Graph::locateLocal(
    t_CKUINT pos, Chuck_Type * fromClassDef )
{
    // don't worry it if pos == 0 (assume omni-present, which is all good)
    if( !pos ) return NULL;

    // value
    Chuck_Value * v = NULL;

    // loop over
    for( t_CKUINT i = 0; i < directs.size(); i++ )
    {
        // get value
        v = directs[i].value;

        // check
        if( !v ) continue;

        // look for any dependencies whose location is after pos
        if( pos < v->depend_init_where )
        {
            // usage NOT from within a class def; value in question NOT a class member OR
            // usage from within a class def; value in question is a class member of the same class
            if( (fromClassDef==NULL && v->is_member==FALSE) ||
                (fromClassDef && v->is_member && equals(v->owner_class, fromClassDef)) )
            {
                // return dependency
                return &directs[i];
            }
        }
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name: resetRecursive()
// desc: reset search tokens
//-----------------------------------------------------------------------------
void Chuck_Value_Dependency_Graph::resetRecursive( t_CKUINT value )
{
    // check
    if( token == value ) return;
    // set for self
    this->token = value;

    // pointer to hold graphs
    Chuck_Value_Dependency_Graph * graph = NULL;
    // loop over
    for( t_CKUINT i = 0; i < remotes.size(); i++ )
    {
        // copy pointer
        graph = remotes[i];
        // if not already visited, visit
        graph->resetRecursive( value );
   }
}




//-----------------------------------------------------------------------------
// name: locateRecursive()
// desc: crawl the remote graph, taking care to handle cycle
//-----------------------------------------------------------------------------
const Chuck_Value_Dependency * Chuck_Value_Dependency_Graph::locateRecursive(
    t_CKUINT pos, Chuck_Type * fromClassDef, t_CKUINT searchToken )
{
    // pointer to hold dep
    const Chuck_Value_Dependency * dep = NULL;
    // first search locally
    dep = locateLocal( pos, fromClassDef );
    // if found, done
    if( dep ) return dep;

    // set search token, for cycle detection
    this->token = searchToken;
    // pointer to hold graphs
    Chuck_Value_Dependency_Graph * graph = NULL;
    // loop over
    for( t_CKUINT i = 0; i < remotes.size(); i++ )
    {
        // copy pointer
        graph = remotes[i];
        // if not already visited, visit
        if( graph->token != searchToken )
            dep = graph->locateRecursive( pos, fromClassDef, searchToken );
        // if found, done
        if( dep ) return dep;
    }

    return NULL;
}




//-----------------------------------------------------------------------------
// name: locate()
// desc: look for a dependency that occurs AFTER a particular code position
//-----------------------------------------------------------------------------
const Chuck_Value_Dependency * Chuck_Value_Dependency_Graph::locate(
    t_CKUINT pos, Chuck_Type * fromClassDef )
{
    // reset search token
    resetRecursive();
    // recursive search
    return locateRecursive( pos, fromClassDef, 1 );
}




//-----------------------------------------------------------------------------
// name: clear() | 1.5.1.1
// desc: clear all dependencies | to be called when all dependencies are met
// for example, at the successful compilation of a context (e.g., a file)
// after this, calls to locate() will return NULL, indicating no dependencies
// NOTE dependency analysis is for within-context only, and is not needed
// across contexts (e.g., files)
//-----------------------------------------------------------------------------
void Chuck_Value_Dependency_Graph::clear()
{
    // direct dependencies
    this->directs.clear();
    // remote dependencies
    this->remotes.clear();
}




//-----------------------------------------------------------------------------
// name: Chuck_Type()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Type::Chuck_Type( Chuck_Env * env, te_Type _id, const std::string & _n,
                        Chuck_Type * _p, t_CKUINT _s )
{
    // copy env ref
    env_ref = env; CK_SAFE_ADD_REF( env_ref );
    xid = _id;
    base_name = _n;
    parent = _p; CK_SAFE_ADD_REF( parent );
    size = _s;
    owner = NULL;
    array_type = NULL;
    array_depth = 0;
    obj_size = 0;
    info = NULL;
    func = NULL; /* def = NULL; */
    is_copy = FALSE;
    ugen_info = NULL;
    is_complete = TRUE;
    has_pre_ctor = FALSE;
    has_pre_dtor = FALSE;
    ctors_all = NULL;
    ctor_default = NULL;
    dtor_the = NULL;
    dtor_invoker = NULL;
    allocator = NULL;

    // default
    originHint = ckte_origin_UNKNOWN;
    // set origin hint, if possible | 1.5.0.0 (ge) added
    Chuck_Compiler * compiler = env->compiler();
    if( compiler != NULL ) originHint = compiler->m_originHint;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Type()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Type::~Chuck_Type()
{
    // reset
    reset();
    // release env ref | 1.5.0.8
    CK_SAFE_RELEASE( env_ref );
}




//-----------------------------------------------------------------------------
// name: reset()
// desc: reset this type struct
//-----------------------------------------------------------------------------
void Chuck_Type::reset()
{
    // CK_FPRINTF_STDERR( "type: %s %i\n", c_name(), (t_CKUINT)this );
    xid = te_void;
    size = array_depth = obj_size = 0;
    is_copy = FALSE;

    // free only if not locked: to prevent garbage collection after exit
    if( !this->m_locked )
    {
        // release references
        CK_SAFE_RELEASE( info );
        CK_SAFE_RELEASE( owner );
        CK_SAFE_RELEASE( ctors_all ); // 1.5.2.0 (ge) added
        CK_SAFE_RELEASE( ctor_default ); // 1.5.2.0 (ge) added
        CK_SAFE_RELEASE( dtor_the ); // 1.5.2.0 (ge) added

        // TODO: uncomment this, fix it to behave correctly
        // TODO: make it safe to do this, as there are multiple instances of ->parent assignments without add-refs
        // TODO: verify this is valid for final shutdown sequence, including Chuck_Env::cleanup()
        // CK_SAFE_RELEASE( parent );
        // CK_SAFE_RELEASE( array_type );
        // CK_SAFE_RELEASE( ugen_info );
        // CK_SAFE_RELEASE( func );
    }
}




//-----------------------------------------------------------------------------
// name: operator =
// desc: assignment operator; this does not touch the Chuck_VM_Object
//-----------------------------------------------------------------------------
const Chuck_Type & Chuck_Type::operator =( const Chuck_Type & rhs )
{
    // release first
    this->reset();

    // copy
    this->xid = rhs.xid;
    this->base_name = rhs.base_name;
    this->parent = rhs.parent; CK_SAFE_ADD_REF(this->parent);
    this->obj_size = rhs.obj_size;
    this->size = rhs.size;
    this->is_copy = TRUE;
    this->array_depth = rhs.array_depth;
    this->array_type = rhs.array_type; CK_SAFE_ADD_REF(this->array_type);
    this->func = rhs.func; CK_SAFE_ADD_REF(this->func);
    this->info = rhs.info; CK_SAFE_ADD_REF(this->info);
    this->owner = rhs.owner; CK_SAFE_ADD_REF(this->owner);

    return *this;
}




//-----------------------------------------------------------------------------
// name: copy()
// desc: create a copy of this type struct
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Type::copy( Chuck_Env * env, Chuck_Context * context ) const
{
    // pointer
    Chuck_Type * n = NULL;
    // check context
    if( context != NULL )
    {
        // allocate new instance with context
        n = context->new_Chuck_Type( env );
    }
    else
    {
        // allocate new instance without context | 1.5.1.1
        n = new Chuck_Type( env );
    }
    // invoke = operator
    *n = *this;

    // return new instance
    return n;
}




//-----------------------------------------------------------------------------
// name: env() | 1.5.1.8 (ge) added method, avoid direct variable access
// desc: get env reference that contains this type
//-----------------------------------------------------------------------------
Chuck_Env * Chuck_Type::env() const
{
    return env_ref;
}




//-----------------------------------------------------------------------------
// name: vm() | 1.5.1.8 (ge) added method, avoid direct variable access
// desc: get VM reference associated with this type (via the env)
//-----------------------------------------------------------------------------
Chuck_VM * Chuck_Type::vm() const
{
    // no env ref, no vm ref
    if( !env_ref ) return NULL;
    // return env vm ref
    return env_ref->vm();
}




//-----------------------------------------------------------------------------
// name: name()
// desc: get the full name of this type, e.g., "UGen" or "int[][]"
//-----------------------------------------------------------------------------
const std::string & Chuck_Type::name()
{
    // start with base name
    ret = base_name;
    // add array levels as needed
    for( t_CKUINT i = 0; i < array_depth; i++ )
        ret += std::string("[]");
    // return it
    return ret;
}




//-----------------------------------------------------------------------------
// name: c_name()
// desc: return this->name() as C string
//-----------------------------------------------------------------------------
const char * Chuck_Type::c_name()
{
    return name().c_str();
}




//-----------------------------------------------------------------------------
// name: type_engine_has_implicit_def_ctor() | 1.5.2.0
// desc: determine whether type has implicit default constructor; helpful for ckdoc
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_has_implicit_def_ctor( Chuck_Type * type )
{
    t_CKBOOL implicitDefaultCtor = FALSE;

    // no for arrays
    if( isa( type, type->env()->ckt_array ) )
        return FALSE;

    // type pointer
    Chuck_Type * t = type;
    // check up the chain (don't include top-level Object in this check)
    do { // at least once if type is Object itself
        // check pre_ctor
        if( t->has_pre_ctor )
        {
            implicitDefaultCtor = TRUE;
            break;
        }

        // check
        if( type->info )
        {
            // check for mfuns and mvars (if we get here, type->info cannot be NULL)
            vector<Chuck_Func *> fs;
            type->info->get_funcs( fs );
            vector<Chuck_Value *> vs;
            type->info->get_values( vs );

            // iterate over functions
            for( vector<Chuck_Func *>::iterator f = fs.begin(); f != fs.end(); f++ )
            {
                // the function
                Chuck_Func * func = *f;
                if( func == NULL ) continue;
                // static or instance?
                if( !func->is_static )
                {
                    implicitDefaultCtor = TRUE;
                    break;
                }
            }
            // iterate over values
            for( vector<Chuck_Value *>::iterator v = vs.begin(); v != vs.end(); v++ )
            {
                // the function
                Chuck_Value * value = *v;
                if( value == NULL ) continue;
                // special internal values
                if( value->name.size() && value->name[0] == '@' )
                    continue;
                // value is a function
                if( isa( value->type, type->env()->ckt_function ) )
                    continue;

                // static or instance?
                if( !value->is_static )
                {
                    implicitDefaultCtor = TRUE;
                    break;
                }
            }
        }

        // up to parent type
        t = t->parent;
    } while( t && t != t->env()->ckt_object );

    return implicitDefaultCtor;
}




//-----------------------------------------------------------------------------
// name: apropos()
// desc: generate info; output to console | added 1.4.1.0 (ge)
//       portions of this adapted from ckdoc -- thanks be to Spencer Salazar
//-----------------------------------------------------------------------------
void Chuck_Type::apropos( std::string & output )
{
    // reset
    output = "";
    // line prefix
    string PREFIX = ""; // [chuck]: ";

    //-------------------------------------------------------------------------
    // top level info: name, inheritane, description
    //-------------------------------------------------------------------------
    string outputTop;
    // generate string
    apropos_top( outputTop, PREFIX );

    //-------------------------------------------------------------------------
    // functions and variables
    //-------------------------------------------------------------------------
    string outputAPI;
    // type
    Chuck_Type * type = this;
    // only the first
    t_CKBOOL inherited = FALSE;

    // handle arrays special
    // 1.4.1.1 (ge + nshaheed) if -> while; to skip intermediate array types,
    // which were added to handle array type-checking (see new_array_element_type())
    while( type->array_type )
    {
        // skip current one, which extend @array to avoid printing duplicates
        type = type->parent;
        // yes to inherited
        inherited = TRUE;
    }
    // got something
    while( type )
    {
        // string
        string temp;
        // generate string
        type->apropos_funcs( temp, PREFIX, inherited );
        // append
        outputAPI += temp;
        // generate string
        type->apropos_vars( temp, PREFIX, inherited );
        // append
        outputAPI += temp;
        // go up the inheritance chain
        type = type->parent;
        // all inherited from here
        inherited = TRUE;
    }

    //-------------------------------------------------------------------------
    // variables
    //-------------------------------------------------------------------------
    string outputVars;

    //-------------------------------------------------------------------------
    // examples
    //-------------------------------------------------------------------------
    string outputExamples;
    // generate string
    // apropos_examples( outputExamples, PREFIX );

    // done
    output = outputTop + outputAPI + outputExamples;
}




//-----------------------------------------------------------------------------
// name: apropos()
// desc: generate info; output to console | added 1.4.1.0 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::apropos()
{
    // the info
    string output;
    // get it
    this->apropos( output );
    // print it
    CK_STDCERR << output << CK_STDENDL;
}




//-----------------------------------------------------------------------------
// comparers
//-----------------------------------------------------------------------------
bool ck_comp_func( Chuck_Func * a, Chuck_Func * b )
{ return a->name < b->name; }
bool ck_comp_func_args( Chuck_Func * a, Chuck_Func * b )
{
    // if names not same
    if( a->name != b->name ) return a->name < b->name;
    // num args
    t_CKUINT numA = 0, numB = 0;
    // arguments
    a_Arg_List argsA = a->def()->arg_list, argsB = b->def()->arg_list;
    // count
    while( argsA ) { numA++; argsA = argsA->next; }
    while( argsB ) { numB++; argsB = argsB->next; }
    // compare num args
    return numA < numB;
}
bool ck_comp_value( Chuck_Value * a, Chuck_Value * b )
{
    string lowerA = tolower( a->name );
    string lowerB = tolower( b->name );
    // if not the same
    if( lowerA != lowerB ) return lowerA < lowerB;
    // if same, favor lower-case
    else return a->name > b->name;
}
bool ck_comp_dl_func_args(Chuck_DL_Func* a, Chuck_DL_Func* b)
{
    // if names not same
    if(a->name != b->name) return a->name < b->name;
    // num args
    t_CKUINT numA = a->args.size(), numB = b->args.size();
    // compare num args
    return numA < numB;
}




//-----------------------------------------------------------------------------
// name: apropos_top()
// desc: output top level info; added 1.4.1.0 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::apropos_top( std::string & output, const std::string & PREFIX )
{
    // make a string output stream
    ostringstream sout;
    // type
    Chuck_Type * type = this;
    // name str
    string nameStr = "* " + name();
    // check ugen info (note: all uanae are also ugens)
    if( this->ugen_info )
    {
        // check if uana | 1.4.2.0 (ge)
        if( this->ugen_info->tock ) nameStr += " (unit analyzer)";
        // check if UAna base class, which has no tock() | 1.4.2.0 (ge)
        else if( this->xid == te_uana ) nameStr += " (unit analyzer)";
        // otherwise
        else nameStr += " (unit generator)";
    }
 
    // check if array
    if( this->array_depth > 0 )
    {
        nameStr += " (";
        if( this->array_depth > 1 )
        {
            // sigh...to_string() on earlier windows / VC++ 2010
            ostringstream num;
            num << this->array_depth;
            nameStr += num.str() + "D ";
        }
        nameStr += this->base_name + " array)";
    }
    // print
    nameStr += " *";

    //-------------------------------------------------------------------------
    // top level info: name, inheritane, description
    //-------------------------------------------------------------------------
    // delimiter
    sout << PREFIX;
    for( int i = 0; i < nameStr.length(); i++ ) sout << "*";
    sout << endl;
    // type name
    sout << PREFIX << nameStr << endl;
    // delimiter
    sout << PREFIX;
    for( int i = 0; i < nameStr.length(); i++ ) sout << "*";
    sout << endl;
    // description
    if( this->doc != "" )
        sout << PREFIX << "  |- " << capitalize_and_periodize(this->doc) << "" << endl;
    // inheritance
    if( type->parent != NULL )
    {
        sout << PREFIX << "  |- (inheritance) " << name();
        while( type->parent != NULL )
        {
            // move up
            type = type->parent;
            // print
            sout << " -> " << type->name() << "";
        }
        sout << PREFIX << "" << endl;
        // set back to this
        type = this;
    }
    // delimiter
    // sout << PREFIX;
    // for( int i = 0; i < str().length(); i++ ) sout << "-";
    // sout << endl;
    // store
    output = sout.str();
}




//-----------------------------------------------------------------------------
// name: apropos_func_arg()
// desc: helper function for outputing function arguments
//-----------------------------------------------------------------------------
void apropos_func_arg( std::ostringstream & sout, a_Arg_List arg )
{
    // argument type
    sout << arg->type->name();
    // space
    sout << " ";
    // argument name
    sout << S_name(arg->var_decl->xid);
    // print comma, if there are more arguments after this
    if( arg->next != NULL )
        sout << ", ";
}




//-----------------------------------------------------------------------------
// name: apropos_func()
// desc: helper function for outputing function
//-----------------------------------------------------------------------------
void apropos_func( std::ostringstream & sout, Chuck_Func * theFunc,
                   const std::string & PREFIX )
{
    // print line prefix, if any
    sout << PREFIX;
    // print static
    sout << (theFunc->is_static ? "static " : "");
    // check if need to print return type
    if( !theFunc->is_ctor && !theFunc->is_dtor && theFunc->def() )
    {
        // print return type
        sout << theFunc->def()->ret_type->name();
        // space
        sout << " ";
    }
    // output function name
    sout << theFunc->base_name;
    // open paren
    sout << "(";
    // if we have args
    if( theFunc->def() && theFunc->def()->arg_list )
    {
        // extra space
        sout << " ";

        // argument list
        a_Arg_List args = theFunc->def()->arg_list;
        while( args != NULL )
        {
            // output arg
            apropos_func_arg( sout, args );
            // move to next
            args = args->next;
        }
        // extra space, if we are args
        if( theFunc->def()->arg_list ) sout << " ";
    }

    // close paren
    sout << ");" << endl;
    // output doc
    if( theFunc->doc != "" )
        sout << PREFIX << "    " << capitalize_and_periodize( theFunc->doc ) << endl;
    else if( theFunc->is_ctor && (!theFunc->def() || !theFunc->def()->arg_list) ) // default ctor?
        sout << PREFIX << "    " << capitalize_and_periodize( "Default constructor for " + theFunc->base_name ) << endl;
}




//-----------------------------------------------------------------------------
// name: apropos_funcs()
// desc: output info about funcs; added 1.4.1.0 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::apropos_funcs( std::string & output,
                                const std::string & PREFIX, t_CKBOOL inherited )
{
    // make a string output stream
    ostringstream sout;

    // check type info
    if( this->info )
    {
        // vector of functions
        vector<Chuck_Func *> funcs;
        // retrieve functions in this type
        this->info->get_funcs(funcs);

        // constructors
        vector<Chuck_Func *> ctors;
        // member functions
        vector<Chuck_Func *> mfuncs;
        // static functions
        vector<Chuck_Func *> sfuncs;
        // destructor
        Chuck_Func * dtor = NULL;
        // counter by name
        map<string, int> func_names;

        // iterate over retrieved functions
        for( vector<Chuck_Func *>::iterator f = funcs.begin(); f != funcs.end(); f++ )
        {
            // get pointer to chuck func
            Chuck_Func * theFunc = *f;
            // check for NULL
            if( theFunc == NULL ) continue;
            // see if name appeared before
            if( func_names.count(theFunc->name) )
            {
                // increment count associated with name
                func_names[theFunc->name]++;
                // done for now
                continue;
            }

            // create entry for name
            func_names[theFunc->name] = 1;

            // check for static declaration
            if( theFunc->is_static ) {
                // append to static funcs list
                sfuncs.push_back( theFunc );
            } else {
                // append to constructors | 1.5.2.0
                if( theFunc->is_ctor ) ctors.push_back( theFunc );
                // set as destructor | 1.5.2.0
                else if( theFunc->is_dtor ) dtor = theFunc;
                // append to static funcs list
                else mfuncs.push_back( theFunc );
            }
        }

        // if not inherited
        if( !inherited )
        {
            // sort
            sort( ctors.begin(), ctors.end(), ck_comp_func_args );
            // check if potentially insert default ctor
            t_CKBOOL insertDefaultCtor = type_engine_has_implicit_def_ctor( this );
            // have constructors?
            if( ctors.size() || insertDefaultCtor )
            {
                // type name
                string theName = this->name() + " " + "constructors";
                // number of '-'
                t_CKUINT n = theName.length(); t_CKUINT i;
                // output
                for( i = 0; i < n; i++ ) { sout << "-"; }
                sout << endl << theName << endl;
                for( i = 0; i < n; i++ ) { sout << "-"; }
                sout << endl;

                // add default constructor, if non-explicitly specified
                if( ((ctors.size() == 0 || (ctors.size() && ctors[0]->def()->arg_list))) && insertDefaultCtor )
                {
                    Chuck_Func ftemp;
                    ftemp.base_name = this->base_name;
                    ftemp.doc = "default constructor for " + this->base_name;
                    // output function content
                    apropos_func( sout, &ftemp, PREFIX );
                }

                // iterate over member functions
                for( vector<Chuck_Func *>::iterator f = ctors.begin(); f != ctors.end(); f++ )
                {
                    // pointer to chuck func
                    Chuck_Func * theFunc = *f;
                    // output function content
                    apropos_func( sout, theFunc, PREFIX );
                }
            }
        }

        // have static or member funcs?
        if( sfuncs.size() > 0 || mfuncs.size() > 0 )
        {
            // type name
            string theName = this->name() + " " + "functions" + (inherited ? " (inherited)" : "" );
            // number of '-'
            t_CKUINT n = theName.length(); t_CKUINT i;
            // output
            for( i = 0; i < n; i++ ) { sout << "-"; }
            sout << endl << theName << endl;
            for( i = 0; i < n; i++ ) { sout << "-"; }
            sout << endl;
        }

        // sort
        sort( mfuncs.begin(), mfuncs.end(), ck_comp_func );
        sort( sfuncs.begin(), sfuncs.end(), ck_comp_func );

        // one or more member functions?
        if( mfuncs.size() )
        {
            // iterate over member functions
            for( vector<Chuck_Func *>::iterator f = mfuncs.begin(); f != mfuncs.end(); f++ )
            {
                // pointer to chuck func
                Chuck_Func * theFunc = *f;
                // output function content
                apropos_func( sout, theFunc, PREFIX );
            }
        }

        // one or more static functions?
        if( sfuncs.size() )
        {
            // iterate over static functions
            for( vector<Chuck_Func *>::iterator f = sfuncs.begin(); f != sfuncs.end(); f++ )
            {
                // pointer to chuck func
                Chuck_Func * theFunc = *f;
                // output function content
                apropos_func( sout, theFunc, PREFIX );
            }
        }

        // has destructor? | 1.5.2.0
        if( dtor )
        {
            // output function content
            // apropos_func( sout, dtor, PREFIX );
        }
    }

    // output to string
    output = sout.str();
}




//-----------------------------------------------------------------------------
// name: apropos_var()
// desc: helper function for outputing variable
//-----------------------------------------------------------------------------
void apropos_var( std::ostringstream & sout, Chuck_Value * var,
                  const std::string & PREFIX )
{
    // print line prefix, if any
    sout << PREFIX;
    // print static
    sout << (var->is_static ? "static " : "");
    // print const
    sout << (var->is_const ? "const " : "");
    // print global
    sout << (var->is_global ? "global " : "");

    // output variable type
    sout << var->type->name() << " ";
    // output variable name
    sout << var->name << ";" << endl;

    // output doc
    if( var->doc != "" ) sout << PREFIX << "    " << capitalize_and_periodize(var->doc) << endl;
}




//-----------------------------------------------------------------------------
// name: apropos_vars()
// desc: output info about vars; added 1.4.1.0 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::apropos_vars( std::string & output, const std::string & PREFIX,
                               t_CKBOOL inherited )
{
    // make a string output stream
    ostringstream sout;

    // check type info
    if( this->info )
    {
        // vector of variables
        vector<Chuck_Value *> vars;
        // retrieve variables in this type
        this->info->get_values(vars);

        // member variables
        vector<Chuck_Value *> mvars;
        // static variables
        vector<Chuck_Value *> svars;

        // iterate over retrieved functions
        for( vector<Chuck_Value *>::iterator v = vars.begin(); v != vars.end(); v++ )
        {
            // get pointer to chuck value
            Chuck_Value * value = *v;
            // check for NULL
            if( value == NULL ) continue;
            // see if name if empty
            if( value->name.length() == 0 ) continue;
            // see if name is internally reserved
            if( value->name[0] == '@' ) continue;
            // see if name is a function
            if( value->type->base_name == "[function]" ) continue;

            // check for static declaration
            if( value->is_static ) {
                // append to static funcs list
                svars.push_back( value );
            } else {
                // append to static funcs list
                mvars.push_back( value );
            }
        }

        // see if we have any to print
        if( mvars.size() > 0 || svars.size() > 0 )
        {
            // type name
            string theName = this->name() + " " + "variables" + (inherited ? " (inherited)" : "" );
            // number of '-'
            t_CKUINT n = theName.length(); t_CKUINT i;
            // output
            for( i = 0; i < n; i++ ) { sout << "-"; }
            sout << endl << theName << endl;
            for( i = 0; i < n; i++ ) { sout << "-"; }
            sout << endl;
        }

        // sort
        sort( mvars.begin(), mvars.end(), ck_comp_value );
        sort( svars.begin(), svars.end(), ck_comp_value );

        // one or more static vars?
        if( mvars.size() )
        {
            // iterate over static vars
            for( vector<Chuck_Value *>::iterator v = mvars.begin(); v != mvars.end(); v++ )
            {
                // pointer to chuck func
                Chuck_Value * var = *v;
                // print line prefix, if any
                sout << PREFIX;
                // output var
                apropos_var( sout, var, PREFIX );
            }
        }

        // one or more static functions?
        if( svars.size() )
        {
            // iterate over static functions
            for( vector<Chuck_Value *>::iterator v = svars.begin(); v != svars.end(); v++ )
            {
                // pointer to chuck func
                Chuck_Value * var = *v;
                // output var
                apropos_var( sout, var, PREFIX );
            }
        }
    }

    // output to string
    output = sout.str();
}




//-----------------------------------------------------------------------------
// name: dump_obj()
// desc: generate object state; output to console | 1.4.1.1 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::dump_obj( Chuck_Object * obj )
{
    // the info
    string output;
    // get it
    this->dump_obj( obj, output );
    // print it
    CK_STDCERR << output << CK_STDENDL;
}




//-----------------------------------------------------------------------------
// name: dump_obj()
// desc: generate object state; output to string | 1.4.1.1 (ge)
//-----------------------------------------------------------------------------
void Chuck_Type::dump_obj( Chuck_Object * obj, std::string & output )
{
    // TODO
}




//-----------------------------------------------------------------------------
// operator < for type pairs
//-----------------------------------------------------------------------------
bool Chuck_TypePair::operator <( const Chuck_TypePair & other ) const
{
    string aL = this->lhs ? this->lhs->name() : "";
    string aR = this->rhs ? this->rhs->name() : "";
    string bL = other.lhs ? other.lhs->name() : "";
    string bR = other.rhs ? other.rhs->name() : "";

    // strict weak ordering
    // (aL, aR) < (bL, bR) iff (aL < bL) OR ( aL == bL && aR < bR )
    return (aL < bL) || (aL == bL && aR < bR);
}




// static instantiation
const t_CKUINT Chuck_Op_Registry::STACK_PUBLIC_ID = 2;
//-----------------------------------------------------------------------------
// name: Chuck_Op_Registry()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Op_Registry::Chuck_Op_Registry()
{
    // set to
    m_stackID = STACK_PUBLIC_ID+1;
    // set to 0 no preserve
    m_stackPreserveID = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Op_Register()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Op_Registry::~Chuck_Op_Registry()
{
    // remove preserve status
    this->unpreserve();
    // reset everything, including previously preserved
    this->pop( 0 );
}




//-----------------------------------------------------------------------------
// name: push()
// desc: push overload stack ID
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Op_Registry::push()
{
    // return current ID (and then increment)
    return ++m_stackID;
}




//-----------------------------------------------------------------------------
// name: pop()
// desc: remove all overload greater than pushID;
//       return how many levels were popped
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Op_Registry::pop( t_CKUINT pushID )
{
    // nothing to pop
    if( pushID > m_stackID ) return 0;
    // must be at or above stack preserve ID
    if( pushID < m_stackPreserveID ) pushID = m_stackPreserveID;
    // iterator
    map<ae_Operator, Chuck_Op_Semantics *>::iterator it;
    // iterate over all operators
    for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
    {
        // remove all overloads > pushID
        it->second->removeAbove( pushID );
    }
    // get difference
    t_CKUINT diff = m_stackID - pushID;
    // set push ID
    m_stackID = pushID+1;
    // return how many levels
    return diff;
}




//-----------------------------------------------------------------------------
// name: pop()
// desc: remove all overload greater than previous pushID
//       return how many levels were popped (1 or 0)
//-----------------------------------------------------------------------------
t_CKUINT Chuck_Op_Registry::pop()
{
    // current stack ID
    t_CKUINT aboveThisID = m_stackID;
    // decrement
    if( aboveThisID ) aboveThisID--;
    // can't normally pop beyond public
    if( aboveThisID < STACK_PUBLIC_ID ) aboveThisID = STACK_PUBLIC_ID;
    // pop above this
    return pop( aboveThisID );
}




//-----------------------------------------------------------------------------
// name: preserve()
// desc: preserve current state (cannot be popped beyond this point)
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::preserve()
{
    // set preserve stack ID to 1
    m_stackPreserveID = 1;
    // set stack ID to public for now (will push below)
    m_stackID = STACK_PUBLIC_ID;

    // squash all op overload IDs to
    std::map<ae_Operator, Chuck_Op_Semantics *>::iterator it;
    // iterate over all operator semantics
    for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
    {
        it->second->squashTo( m_stackPreserveID );
    }

    // push stack so all subsequent additions are beyond the preserve
    push();
}




//-----------------------------------------------------------------------------
// name: reset2local()
// desc: reset pop local overload state (everything above publicID)
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::reset2local()
{
    // pop everything above public
    pop( STACK_PUBLIC_ID );
}




//-----------------------------------------------------------------------------
// name: reset2public()
// desc: reset pop beyond public state (everything above preserveID)
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::reset2public()
{
    // pop everything above preserve
    pop( m_stackPreserveID );
}




//-----------------------------------------------------------------------------
// name: unpreserve()
// desc: remove preserve status (allowing pops for all stack levels)
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::unpreserve()
{
    // set back to 0
    m_stackPreserveID = 0;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: add semantics for particular operator
//-----------------------------------------------------------------------------
Chuck_Op_Semantics * Chuck_Op_Registry::add( ae_Operator op )
{
    // if not in map
    if( m_operatorMap.find( op ) == m_operatorMap.end() )
    {
        // create new
        m_operatorMap[op] = new Chuck_Op_Semantics( op );
    }

    // look it up
    return lookup( op );
}




//-----------------------------------------------------------------------------
// name: add()
// desc: get semantics for particular operator
//-----------------------------------------------------------------------------
Chuck_Op_Semantics * Chuck_Op_Registry::lookup( ae_Operator op )
{
    // if not in map
    if( m_operatorMap.find( op ) == m_operatorMap.end() )
    {
        // return empty
        return NULL;
    }

    // get it
    Chuck_Op_Semantics * semantics = m_operatorMap[op];
    // this should not happen
    assert( semantics != NULL );
    // return
    return semantics;
}


//-----------------------------------------------------------------------------
// name: binaryOverloadable()
// desc: can overload as binary?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::binaryOverloadable( ae_Operator op )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return FALSE;
    // return
    return semantics->isBinaryOL();
}




//-----------------------------------------------------------------------------
// name: unaryPreOverloadable()
// desc: can overload as unary prefix?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::unaryPreOverloadable( ae_Operator op )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return FALSE;
    // return
    return semantics->isUnaryPreOL();
}




//-----------------------------------------------------------------------------
// name: unaryPostOverloadable()
// desc: can overload as unary postfix?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::unaryPostOverloadable( ae_Operator op )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return FALSE;
    // return
    return semantics->isUnaryPostOL();
}




//-----------------------------------------------------------------------------
// name: reserve()
// desc: reserve builtin overloads for binary op
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::reserve( Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs, t_CKBOOL commute )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics )
    {
        EM_error3( "cannot reserve operator '%s'...", op2str(op) );
        return;
    }

    // first do the commute, if binary op AND type pointers aren't equal
    if( commute && lhs && rhs && lhs != rhs )
        reserve( rhs, op, lhs, FALSE );

    // check
    Chuck_Op_Overload * overload = semantics->getOverload( lhs, rhs );
    // check
    if( overload )
    {
        // check which kind
        if( overload->kind() == ckte_op_overload_BINARY )
            EM_error3( "binary operator '%s' already overloaded on types '%s' and '%s' (or their parents)...", op2str(op), lhs->c_name(), rhs->c_name() );
        else if( overload->kind() == ckte_op_overload_UNARY_PRE )
            EM_error3( "unary (prefix) operator '%s' already overloaded on type '%s' (or its parent)...", op2str(op), rhs->c_name() );
        else if( overload->kind() == ckte_op_overload_UNARY_POST )
            EM_error3( "unary (postfix) operator '%s' already overloaded on type '%s' (or its parent)...", op2str(op), lhs->c_name() );
        else
            EM_error3( "(internal error) operator '%s' already overloaded...", op2str(op) );
        return;
    }

    // create new overload
    overload = new Chuck_Op_Overload( lhs, op, rhs, NULL );
    // set origin
    overload->updateOrigin( ckte_origin_BUILTIN, "type system", 0 );
    // set reserved flag
    overload->updateReserved( TRUE );
    // set stack level ID
    overload->mark( m_stackID );
    // add it
    semantics->add( overload );
}




//-----------------------------------------------------------------------------
// name: reserve()
// desc: reserve builtin overloads for unary prefix op
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::reserve( ae_Operator op, Chuck_Type * type )
{
    reserve( NULL, op, type );
}




//-----------------------------------------------------------------------------
// name: reserve()
// desc: reserve builtin overloads for unary postfix
//-----------------------------------------------------------------------------
void Chuck_Op_Registry::reserve( Chuck_Type * type, ae_Operator op )
{
    reserve( type, op, NULL );
}




//-----------------------------------------------------------------------------
// name: add_overload()
// desc: add binary operator overload: lhs OP rhs
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::add_overload(
    Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs, Chuck_Func * func,
    ckte_Origin origin, const std::string & originName, t_CKINT originWhere,
    t_CKBOOL isPublic )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics )
    {
        EM_error2( originWhere, "cannot overload operator '%s'...", op2str(op) );
        return FALSE;
    }

    // check
    Chuck_Op_Overload * overload = semantics->getOverload( lhs, rhs );
    // check
    if( overload )
    {
        // check which kind
        if( overload->kind() == ckte_op_overload_BINARY )
            EM_error2( originWhere, "binary operator '%s' already overloaded on types '%s' and '%s' (or their parents)...", op2str(op), lhs->c_name(), rhs->c_name() );
        else if( overload->kind() == ckte_op_overload_UNARY_PRE )
            EM_error2( originWhere, "unary (prefix) operator '%s' already overloaded on type '%s' (or its parent)...", op2str(op), rhs->c_name() );
        else if( overload->kind() == ckte_op_overload_UNARY_POST )
            EM_error2( originWhere, "unary (postfix) operator '%s' already overloaded on type '%s' (or its parent)...", op2str(op), lhs->c_name() );
        else
            EM_error2( originWhere, "(internal error) operator '%s' already overloaded...", op2str(op) );

        return FALSE;
    }

    // create new overload
    overload = new Chuck_Op_Overload( lhs, op, rhs, func );
    // set origin
    overload->updateOrigin( origin, originName, originWhere );
    // set stack level ID
    overload->mark( isPublic ? STACK_PUBLIC_ID : m_stackID );
    // add it
    semantics->add( overload );

    // ok
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: add_overload()
// desc: add prefix unary operator overload: OP rhs
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::add_overload( ae_Operator op, Chuck_Type * rhs, Chuck_Func * func,
                       ckte_Origin origin, const std::string & originName, t_CKINT originWhere,
                       t_CKBOOL isPublic )
{
    return this->add_overload( NULL, op, rhs, func, origin, originName, originWhere, isPublic );
}




//-----------------------------------------------------------------------------
// name: add_overload()
// desc: add postfix unary operator overload: lhs OP
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Registry::add_overload( Chuck_Type * lhs, ae_Operator op, Chuck_Func * func,
                       ckte_Origin origin, const std::string & originName, t_CKINT originWhere,
                       t_CKBOOL isPublic )
{
    return this->add_overload( lhs, op, NULL, func, origin, originName, originWhere, isPublic );
}




//-----------------------------------------------------------------------------
// name: lookup_overload()
// desc: look up binary operator overload: lhs OP rhs
//-----------------------------------------------------------------------------
Chuck_Op_Overload * Chuck_Op_Registry::lookup_overload( Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return NULL;
    // return overload
    return semantics->getOverload( lhs, rhs );
}




//-----------------------------------------------------------------------------
// name: lookup_overload()
// desc: look up prefix unary operator overload: OP rhs
//-----------------------------------------------------------------------------
Chuck_Op_Overload * Chuck_Op_Registry::lookup_overload( ae_Operator op, Chuck_Type * rhs )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return NULL;
    // return overload
    return semantics->getOverload( NULL, rhs );
}




//-----------------------------------------------------------------------------
// name: lookup_overload()
// desc: look up postfix unary operator overload: lhs OP
//-----------------------------------------------------------------------------
Chuck_Op_Overload * Chuck_Op_Registry::lookup_overload( Chuck_Type * lhs, ae_Operator op )
{
    // get semantics
    Chuck_Op_Semantics * semantics = lookup( op );
    // check
    if( !semantics ) return NULL;
    // return overload
    return semantics->getOverload( lhs, NULL );
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Semantics()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Op_Semantics::Chuck_Op_Semantics( ae_Operator op )
{
    // set
    m_op = op;
    // defaults
    is_overloadable_binary = false;
    is_overloadable_unary_pre = false;
    is_overloadable_unary_post = false;
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Semantics()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Op_Semantics::~Chuck_Op_Semantics()
{
    EM_log( CK_LOG_DEBUG, "Semantics destructor" );
    // iterator
    map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it;
    // iterate
    for( it = overloads.begin(); it != overloads.end(); it++ )
    {
        // delete the overload
        CK_SAFE_DELETE( it->second );
    }
    // clear map
    overloads.clear();

    // set back to defaults
    m_op = ae_op_none;
    is_overloadable_binary = false;
    is_overloadable_unary_pre = false;
    is_overloadable_unary_post = false;
}




//-----------------------------------------------------------------------------
// name: configure()
// desc: configure how operator could be overloaded
//-----------------------------------------------------------------------------
void Chuck_Op_Semantics::configure( bool binary_OL, bool unary_pre_OL, bool unary_post_OL )
{
    is_overloadable_binary = binary_OL;
    is_overloadable_unary_pre = unary_pre_OL;
    is_overloadable_unary_post = unary_post_OL;
}




//-----------------------------------------------------------------------------
// name: add()
// desc: add overload
//-----------------------------------------------------------------------------
void Chuck_Op_Semantics::add( Chuck_Op_Overload * overload )
{
    // type pair
    Chuck_TypePair tp( overload->lhs(), overload->rhs() );
    // if currently in map?
    map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it = overloads.find( tp );
    // delete existing entry, if there is one
    if( it != overloads.end() )
    {
        // delete existing entry
        CK_SAFE_DELETE( it->second );
        // replace
        it->second = overload;
    }
    // otherwise
    else
    {
        // insert into map
        overloads[tp] = overload;
    }
}




//-----------------------------------------------------------------------------
// name: removeAbove()
// desc: remove overloads with mark > pushID
//-----------------------------------------------------------------------------
void Chuck_Op_Semantics::removeAbove( t_CKUINT pushID )
{
    // iterator
    map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it = overloads.begin();
    // iterate
    while( it != overloads.end() )
    {
        // check the mark
        if( it->second->pushID() > pushID )
        {
            // erase while iterating | c++11 or higher
            it = overloads.erase( it );
            // overloads.erase( it++ ); // before c++11
        }
        else
        {
            it++;
        }
    }
}




//-----------------------------------------------------------------------------
// name: squashTo()
// desc: squash each overload's mark to pushID
//-----------------------------------------------------------------------------
void Chuck_Op_Semantics::squashTo( t_CKUINT pushID )
{
    // iterator
    map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it = overloads.begin();
    // iterate
    while( it != overloads.end() )
    {
        // update
        it->second->mark( pushID );
        // next
        it++;
    }
}




// comparer for sorting with const Chuck_Op_Overload *
bool CkOpOverloadCmp( const Chuck_Op_Overload * lhs, const Chuck_Op_Overload * rhs )
   { return (*lhs) < (*rhs); }
//-----------------------------------------------------------------------------
// name: getOverloads()
// desc: retrieve all overloads for an operator
//-----------------------------------------------------------------------------
void Chuck_Op_Semantics::getOverloads( std::vector<const Chuck_Op_Overload *> & results )
{
    // clear results
    results.clear();

    // iterator
    map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it;
    // iterate
    for( it = overloads.begin(); it != overloads.end(); it++ )
    {
        // append the it
        results.push_back( it->second );
    }

    // sort it
    sort( results.begin(), results.end(), CkOpOverloadCmp );
}




//-----------------------------------------------------------------------------
// name: getOverload()
// desc: get overload entry by types
//-----------------------------------------------------------------------------
// get entry by types
Chuck_Op_Overload * Chuck_Op_Semantics::getOverload( Chuck_Type * lhs, Chuck_Type * rhs )
{
    // check which kind we are looking for
    ckte_Op_OverloadKind kind = ckte_op_overload_NONE;
    if( lhs && rhs ) kind = ckte_op_overload_BINARY;
    else if( !lhs && rhs ) kind = ckte_op_overload_UNARY_PRE;
    else if( lhs && !rhs ) kind = ckte_op_overload_UNARY_POST;
    else return NULL;

    // key
    Chuck_TypePair key( lhs, rhs );

    // look up, return NULL if not found
    std::map<Chuck_TypePair, Chuck_Op_Overload *>::iterator it = overloads.find( key );
    if( it != overloads.end() ) return it->second;

    // iterate over overloads
    for( it = overloads.begin(); it != overloads.end(); it++ )
    {
        // get overload
        Chuck_Op_Overload * overload = it->second;
        // see if matches the kind we are looking for
        if( overload->kind() != kind ) continue;

        // check
        if( overload->kind() == ckte_op_overload_BINARY )
        {
            // verify
            assert( overload->lhs() != NULL );
            assert( overload->rhs() != NULL );
            // check both lhs and rhs are respective children classes
            if( isa(lhs,overload->lhs()) && isa(rhs,overload->rhs()) )
                return overload;
        }
        else if( overload->kind() == ckte_op_overload_UNARY_PRE )
        {
            // verify
            assert( it->second->rhs() != NULL );
            // check rhs is child class
            if( isa(rhs,it->second->rhs()) )
                return it->second;
        }
        else if( overload->kind() == ckte_op_overload_UNARY_POST )
        {
            // verify
            assert( it->second->lhs() != NULL );
            // check lhs is child class
            if( isa(lhs,it->second->lhs()) )
                return it->second;
        }
        else
        {
            // error
            EM_error3( "(internal error) undefined overload kind in getOverload()..." );
            return NULL;
        }
    }

    // not found
    return NULL;
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Overload()
// desc: constructor for binary op overload
//-----------------------------------------------------------------------------
Chuck_Op_Overload::Chuck_Op_Overload( Chuck_Type * LHS, ae_Operator op, Chuck_Type * RHS, Chuck_Func * func )
{
    // zero out
    zero();

    // set op
    m_op = op;
    // set kind
    if( LHS && RHS ) m_kind = ckte_op_overload_BINARY;
    else if( !LHS && RHS ) m_kind = ckte_op_overload_UNARY_PRE;
    else if( LHS && !RHS ) m_kind = ckte_op_overload_UNARY_POST;
    else {
        // error
        EM_error3( "(internal error) NULL lhs and rhs in Chuck_Op_Overload constructor..." );
        return;
    }
    // set func
    CK_SAFE_REF_ASSIGN( m_func, func );
    // set lhs
    setLHS( LHS );
    // set rhs
    setRHS( RHS );
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Overload()
// desc: constructor for unary postfix op overload
//-----------------------------------------------------------------------------
Chuck_Op_Overload::Chuck_Op_Overload( Chuck_Type * LHS, ae_Operator op, Chuck_Func * func )
{
    // zero out
    zero();

    // set op
    m_op = op;
    // set as postfix
    m_kind = ckte_op_overload_UNARY_POST;
    // set func
    CK_SAFE_REF_ASSIGN( m_func, func );
    // set lhs
    setLHS( LHS );
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Overload()
// desc: constructor for unary prefix op overload
//-----------------------------------------------------------------------------
Chuck_Op_Overload::Chuck_Op_Overload( ae_Operator op, Chuck_Type * RHS, Chuck_Func * func )
{
    // zero out
    zero();

    // set op
    m_op = op;
    // set as prefix
    m_kind = ckte_op_overload_UNARY_PRE;
    // set func
    CK_SAFE_REF_ASSIGN( m_func, func );
    // set rhs
    setRHS( RHS );
}




//-----------------------------------------------------------------------------
// name: Chuck_Op_Overload()
// desc: copy constructor
//-----------------------------------------------------------------------------
Chuck_Op_Overload::Chuck_Op_Overload( const Chuck_Op_Overload & other )
{
    // zero out
    zero();

    m_op = other.m_op;
    m_kind = other.m_kind;
    // set func
    CK_SAFE_REF_ASSIGN( m_func, other.m_func );
    // set LHS
    setLHS( other.m_lhs );
    // set RHS
    setRHS( other.m_rhs );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Op_Overload()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Op_Overload::~Chuck_Op_Overload()
{
    // release
    CK_SAFE_RELEASE( m_lhs );
    CK_SAFE_RELEASE( m_rhs );
    CK_SAFE_RELEASE( m_func );
}




//-----------------------------------------------------------------------------
// name: updateOrigin()
// desc: update origin info
//-----------------------------------------------------------------------------
void Chuck_Op_Overload::updateOrigin( ckte_Origin origin, const string & name, t_CKINT where )
{
    m_origin = origin;
    m_originName = name;
    m_originWhere = where;
}




//-----------------------------------------------------------------------------
// name: mark()
// desc: set overload stack push ID
//-----------------------------------------------------------------------------
void Chuck_Op_Overload::mark( t_CKUINT pushID )
{
    m_pushID = pushID;
}




//-----------------------------------------------------------------------------
// name: isNative()
// desc: overloading natively handled? (e.g., in chuck_type)
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Op_Overload::isNative() const
{
    // check originated
    return m_origin == ckte_origin_BUILTIN;
}




//-----------------------------------------------------------------------------
// name: zero()
// desc: zero out (for initialization)
//-----------------------------------------------------------------------------
void Chuck_Op_Overload::zero()
{
    m_op = ae_op_none;
    m_kind = ckte_op_overload_NONE;
    m_func = NULL;
    m_origin = ckte_origin_UNKNOWN;
    m_originWhere = 0;
    m_lhs = NULL;
    m_rhs = NULL;
    m_pushID = 0;
    m_isReserved = FALSE;
}




//-----------------------------------------------------------------------------
// name: setLHS()
// desc: set left hand side
//-----------------------------------------------------------------------------
void Chuck_Op_Overload::setLHS( Chuck_Type * type )
{
    CK_SAFE_REF_ASSIGN( m_lhs, type );
}




//-----------------------------------------------------------------------------
// name: setRHS()
// desc: set right hand side
//-----------------------------------------------------------------------------
void Chuck_Op_Overload::setRHS( Chuck_Type * type )
{
    CK_SAFE_REF_ASSIGN( m_rhs, type );
}




//-----------------------------------------------------------------------------
// operator < for overload
//-----------------------------------------------------------------------------
bool Chuck_Op_Overload::operator <( const Chuck_Op_Overload & other ) const
{
    string aL = this->m_lhs ? this->m_lhs->name() : "";
    string aR = this->m_rhs ? this->m_rhs->name() : "";
    string bL = other.m_lhs ? other.m_lhs->name() : "";
    string bR = other.m_rhs ? other.m_rhs->name() : "";

    // strict weak ordering
    // (aL, aR) < (bL, bR) iff (aL < bL) OR ( aL == bL && aR < bR )
    return (aL < bL) || (aL == bL && aR < bR);
}




//-----------------------------------------------------------------------------
// name: add_instantiate_cb()
// desc: register type instantiation callback
//-----------------------------------------------------------------------------
void Chuck_Type::add_instantiate_cb( f_callback_on_instantiate cb, t_CKBOOL setShredOrigin )
{
    // avoid duplicate
    for( t_CKUINT i = 0; i < m_cbs_on_instantiate.size(); i++ )
    {
        // compare callback pointer
        if( cb == m_cbs_on_instantiate[i].callback ) return;
    }
    // append
    m_cbs_on_instantiate.push_back( CallbackOnInstantiate(cb, setShredOrigin) );
}




//-----------------------------------------------------------------------------
// name: remove_instantiate_cb()
// desc: unregister type instantiation callback
//-----------------------------------------------------------------------------
void Chuck_Type::remove_instantiate_cb( f_callback_on_instantiate cb )
{
    // iterator
    vector<CallbackOnInstantiate>::iterator it = m_cbs_on_instantiate.begin();
    // iterate
    while( it != m_cbs_on_instantiate.end() )
    {
        // check the callback
        if( (*it).callback == cb )
        {
            // erase while iterating | c++11 or higher
            it = m_cbs_on_instantiate.erase( it );
            // m_cbs_on_instantiate.erase( it++ ); // before c++11
        }
        else
        {
            it++;
        }
    }
}




//-----------------------------------------------------------------------------
// name: cbs_on_instantiate()
// desc: get vector of callbacks (including this and parents), return whether any requires setShredOrigin
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Type::cbs_on_instantiate( std::vector<CallbackOnInstantiate> & results )
{
    // clear
    results.clear();
    // process this
    return this->do_cbs_on_instantiate( results );
}




//-----------------------------------------------------------------------------
// name: do_cbs_on_instantiate()
// desc: internal get vector of callbacks (including this and parents), return whether any requires setShredOrigin
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Type::do_cbs_on_instantiate( std::vector<CallbackOnInstantiate> & results )
{
    // number of callbacks in total
    t_CKBOOL retval = 0;
    // process parents
    if( this->parent ) retval = this->parent->do_cbs_on_instantiate( results );
    // process this
    for( t_CKUINT i = 0; i < m_cbs_on_instantiate.size(); i++ )
    {
        // copy
        results.push_back( m_cbs_on_instantiate[i] );
        // if and set shred origin
        if( m_cbs_on_instantiate[i].shouldSetShredOrigin ) retval = TRUE;
    }
    // done
    return retval;
}
