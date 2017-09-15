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
// file: chuck_type.cpp
// desc: chuck type-system / type-checker
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original
//       Autumn 2005 - rewrite
//-----------------------------------------------------------------------------
#include "chuck_type.h"
#include "chuck_parse.h"
#include "chuck_scan.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_lang.h"
#include "util_string.h"
#include "ugen_xxx.h"

using namespace std;




//-----------------------------------------------------------------------------
// default types
//-----------------------------------------------------------------------------
// REFACTOR-2017: exile again!
//Chuck_Type t_void( te_void, "void", NULL, 0 );
//Chuck_Type t_int( te_int, "int", NULL, sizeof(t_CKINT) );
//Chuck_Type t_float( te_float, "float", NULL, sizeof(t_CKFLOAT) );
//Chuck_Type t_time( te_time, "time", NULL, sizeof(t_CKTIME) );
//Chuck_Type t_dur( te_dur, "dur", NULL, sizeof(t_CKTIME) );
//Chuck_Type t_complex( te_complex, "complex", NULL, sizeof(t_CKCOMPLEX) );
//Chuck_Type t_polar( te_polar, "polar", NULL, sizeof(t_CKPOLAR) );
//Chuck_Type t_vec3( te_vec3, "vec3", NULL, sizeof(t_CKVEC3) ); // 1.3.5.3
//Chuck_Type t_vec4( te_vec4, "vec4", NULL, sizeof(t_CKVEC4) ); // 1.3.5.3
//Chuck_Type t_null( te_null, "@null", NULL, sizeof(void *) );
//Chuck_Type t_function( te_function, "@function", &t_object, sizeof(void *) );
//Chuck_Type t_object( te_object, "Object", NULL, sizeof(void *) );
//Chuck_Type t_array( te_array, "@array", &t_object, sizeof(void *) );
//Chuck_Type t_string( te_string, "string", &t_object, sizeof(void *) );
//Chuck_Type t_event( te_event, "Event", &t_object, sizeof(void *) );
//Chuck_Type t_ugen( te_ugen, "UGen", &t_object, sizeof(void *) );
//Chuck_Type t_uana( te_uana, "UAna", &t_ugen, sizeof(void *) );
//Chuck_Type t_uanablob( te_uanablob, "UAnaBlob", &t_object, sizeof(void *) );
//Chuck_Type t_shred( te_shred, "Shred", &t_object, sizeof(void *) );
//Chuck_Type t_io( te_io, "IO", &t_event, sizeof(void *) );
//Chuck_Type t_fileio( te_fileio, "FileIO", &t_io, sizeof(void *) );
//Chuck_Type t_chout( te_chout, "StdOut", &t_io, sizeof(void *) );
//Chuck_Type t_cherr( te_cherr, "StdErr", &t_io, sizeof(void *) );
//Chuck_Type t_thread( te_thread, "Thread", &t_object, sizeof(void *) );
//Chuck_Type t_class( te_class, "Class", &t_object, sizeof(void *) );

// exile
//struct Chuck_Type t_adc = { te_adc, "adc", &t_ugen, t_ugen.size };
//struct Chuck_Type t_dac = { te_dac, "dac", &t_ugen, t_ugen.size };
//struct Chuck_Type t_bunghole = { te_bunghole, "bunghole", &t_ugen, t_ugen.size };
//struct Chuck_Type t_midiout = { te_midiout, "midiout", &t_object, sizeof(void *) };
//struct Chuck_Type t_midiin = { te_midiin, "midiin", &t_object, sizeof(void *) };
//struct Chuck_Type t_stdout = { te_stdout, "@stdout", &t_object, sizeof(void *) };
//struct Chuck_Type t_stderr ={ te_stdout, "@stderr", &t_object, sizeof(void *) };
//
//struct Chuck_Type t_uint = { te_uint, "uint", NULL, sizeof(t_CKUINT) };
//struct Chuck_Type t_single = { te_single, "single", NULL, sizeof(float) };
//struct Chuck_Type t_double = { te_double, "double", NULL, sizeof(double) };
//struct Chuck_Type t_code = { te_code, "code", NULL, sizeof(void *) };
//struct Chuck_Type t_tuple = { te_tuple, "tuple", NULL, sizeof(void *) };
//struct Chuck_Type t_pattern = { te_pattern, "pattern", &t_object, sizeof(void *) };
//struct Chuck_Type t_transport = { te_transport, "transport", &t_object, sizeof(void *) };
//struct Chuck_Type t_host = { te_host, "host", &t_object, sizeof(void *) };




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_stmt_list( Chuck_Env * env, a_Stmt_List list );
t_CKBOOL type_engine_check_stmt( Chuck_Env * env, a_Stmt stmt );
t_CKBOOL type_engine_check_if( Chuck_Env * env, a_Stmt_If stmt );
t_CKBOOL type_engine_check_for( Chuck_Env * env, a_Stmt_For stmt );
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
t_CKTYPE type_engine_check_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKTYPE type_engine_check_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKTYPE type_engine_check_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKTYPE type_engine_check_exp_unary( Chuck_Env * env, a_Exp_Unary unary );
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

// helper
a_Func_Def make_dll_as_fun( Chuck_DL_Func * dl_fun, t_CKBOOL is_static,
                            t_CKBOOL is_base_primtive );




//-----------------------------------------------------------------------------
// name: Chuck_Env()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Env::Chuck_Env( )
{
    // lock from being deleted
    global_context.lock();
    // make reference
    context = &global_context; SAFE_ADD_REF(context);
    // make name
    context->filename = "@[global]";
    // remember
    global_nspc = global_context.nspc; SAFE_ADD_REF(global_nspc);
    // deprecated stuff
    deprecated.clear(); deprecate_level = 1;
    user_nspc = NULL;
    // clear
    this->reset();
    
    // initialize types
    t_void = new Chuck_Type( this, te_void, "void", NULL, 0 );
    t_int = new Chuck_Type( this, te_int, "int", NULL, sizeof(t_CKINT) );
    t_float = new Chuck_Type( this, te_float, "float", NULL, sizeof(t_CKFLOAT) );
    t_time = new Chuck_Type( this, te_time, "time", NULL, sizeof(t_CKTIME) );
    t_dur = new Chuck_Type( this, te_dur, "dur", NULL, sizeof(t_CKTIME) );
    t_complex = new Chuck_Type( this, te_complex, "complex", NULL, sizeof(t_CKCOMPLEX) );
    t_polar = new Chuck_Type( this, te_polar, "polar", NULL, sizeof(t_CKPOLAR) );
    t_vec3 = new Chuck_Type( this, te_vec3, "vec3", NULL, sizeof(t_CKVEC3) ); // 1.3.5.3
    t_vec4 = new Chuck_Type( this, te_vec4, "vec4", NULL, sizeof(t_CKVEC4) ); // 1.3.5.3
    t_null = new Chuck_Type( this, te_null, "@null", NULL, sizeof(void *) );
    t_object = new Chuck_Type( this, te_object, "Object", NULL, sizeof(void *) );
    t_function = new Chuck_Type( this, te_function, "@function", t_object, sizeof(void *) );
    t_array = new Chuck_Type ( this, te_array, "@array", t_object, sizeof(void *) );
    t_string = new Chuck_Type( this, te_string, "string", t_object, sizeof(void *) );
    t_event = new Chuck_Type( this, te_event, "Event", t_object, sizeof(void *) );
    t_ugen = new Chuck_Type( this, te_ugen, "UGen", t_object, sizeof(void *) );
    t_uana = new Chuck_Type( this, te_uana, "UAna", t_ugen, sizeof(void *) );
    t_uanablob = new Chuck_Type( this, te_uanablob, "UAnaBlob", t_object, sizeof(void *) );
    t_shred = new Chuck_Type( this, te_shred, "Shred", t_object, sizeof(void *) );
    t_io = new Chuck_Type( this, te_io, "IO", t_event, sizeof(void *) );
    t_fileio = new Chuck_Type( this, te_fileio, "FileIO", t_io, sizeof(void *) );
    t_chout = new Chuck_Type( this, te_chout, "StdOut", t_io, sizeof(void *) );
    t_cherr = new Chuck_Type( this, te_cherr, "StdErr", t_io, sizeof(void *) );
    t_thread = new Chuck_Type( this, te_thread, "Thread", t_object, sizeof(void *) );
    t_class = new Chuck_Type( this, te_class, "Class", t_object, sizeof(void *) );
    
    // REFACTOR-2017: carrier
    m_carrier = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Env()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Env::~Chuck_Env()
{
    // REFACTOR-2017: now part of Env instance
    SAFE_RELEASE( t_object->info );
    SAFE_RELEASE( t_array->info );
    SAFE_RELEASE( t_string->info );
    SAFE_RELEASE( t_ugen->info );
    SAFE_RELEASE( t_uanablob->info );
    SAFE_RELEASE( t_uana->info );
    SAFE_RELEASE( t_shred->info );
    SAFE_RELEASE( t_event->info );
    SAFE_RELEASE( t_class->info );
    SAFE_RELEASE( t_thread->info );
    SAFE_RELEASE( t_io->info );
    SAFE_RELEASE( t_fileio->info );
    SAFE_RELEASE( t_chout->info );  // added 1.3.0.0
    SAFE_RELEASE( t_cherr->info );  // added 1.3.0.0
    SAFE_RELEASE( t_vec3->info );  // added 1.3.5.3
    SAFE_RELEASE( t_vec4->info );  // added 1.3.5.3
    SAFE_RELEASE( t_dac->info ); // REFACTOR-2017
    SAFE_RELEASE( t_adc->info ); // REFACTOR-2017
    
    SAFE_DELETE( t_object );
    SAFE_DELETE( t_array );
    SAFE_DELETE( t_string );
    SAFE_DELETE( t_ugen );
    SAFE_DELETE( t_uanablob );
    SAFE_DELETE( t_uana );
    SAFE_DELETE( t_shred );
    SAFE_DELETE( t_event );
    SAFE_DELETE( t_class );
    SAFE_DELETE( t_thread );
    SAFE_DELETE( t_io );
    SAFE_DELETE( t_fileio );
    SAFE_DELETE( t_chout );  // added 1.3.0.0
    SAFE_DELETE( t_cherr );  // added 1.3.0.0
    SAFE_DELETE( t_vec3 );  // added 1.3.5.3
    SAFE_DELETE( t_vec4 );  // added 1.3.5.3
    SAFE_DELETE( t_dac ); // REFACTOR-2017
    SAFE_DELETE( t_adc ); // REFACTOR-2017
}




//-----------------------------------------------------------------------------
// name: type_engine_init()
// desc: initialize a type engine
//-----------------------------------------------------------------------------
Chuck_Env * type_engine_init( Chuck_Carrier * carrier )
{
    // log
    EM_log( CK_LOG_SEVERE, "initializing type checker..." );
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
    // and store carrier in env
    env->set_carrier( carrier );

    // enter the default global type mapping : lock VM objects to catch deletion
    env->global()->type.add( env->t_void->name, env->t_void );          env->t_void->lock();
    env->global()->type.add( env->t_int->name, env->t_int );            env->t_int->lock();
    env->global()->type.add( env->t_float->name, env->t_float );        env->t_float->lock();
    env->global()->type.add( env->t_time->name, env->t_time );          env->t_time->lock();
    env->global()->type.add( env->t_dur->name, env->t_dur );            env->t_dur->lock();
    env->global()->type.add( env->t_complex->name, env->t_complex );    env->t_complex->lock();
    env->global()->type.add( env->t_polar->name, env->t_polar );        env->t_polar->lock();
    env->global()->type.add( env->t_vec3->name, env->t_vec3 );          env->t_vec3->lock();
    env->global()->type.add( env->t_vec4->name, env->t_vec4 );          env->t_vec4->lock();
    env->global()->type.add( env->t_object->name, env->t_object );      env->t_object->lock();
    env->global()->type.add( env->t_string->name, env->t_string );      env->t_string->lock();
    env->global()->type.add( env->t_ugen->name, env->t_ugen );          env->t_ugen->lock();
    env->global()->type.add( env->t_uana->name, env->t_uana );          env->t_uana->lock();
    env->global()->type.add( env->t_uanablob->name, env->t_uanablob );  env->t_uanablob->lock();
    env->global()->type.add( env->t_shred->name, env->t_shred );        env->t_shred->lock();
    env->global()->type.add( env->t_thread->name, env->t_thread );      env->t_thread->lock();
    env->global()->type.add( env->t_function->name, env->t_function );  env->t_function->lock();
    env->global()->type.add( env->t_class->name, env->t_class );        env->t_class->lock();
    env->global()->type.add( env->t_array->name, env->t_array );        env->t_array->lock();
    env->global()->type.add( env->t_event->name, env->t_event );        env->t_event->lock();
    env->global()->type.add( env->t_io->name, env->t_io );              env->t_io->lock();
    env->global()->type.add( env->t_fileio->name, env->t_fileio );      env->t_fileio->lock();
    env->global()->type.add( env->t_chout->name, env->t_chout );        env->t_chout->lock();
    env->global()->type.add( env->t_cherr->name, env->t_cherr );        env->t_cherr->lock();

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

    // add internal classes
    EM_log( CK_LOG_SEVERE, "adding base classes..." );
    EM_pushlog();
    init_class_object( env, env->t_object );
    init_class_array( env, env->t_array );
    init_class_string( env, env->t_string );
    init_class_ugen( env, env->t_ugen );
    init_class_blob( env, env->t_uanablob );
    init_class_uana( env, env->t_uana );
    init_class_shred( env, env->t_shred );
    init_class_event( env, env->t_event );
    init_class_io( env, env->t_io );
    init_class_fileio( env, env->t_fileio );
    init_class_chout( env, env->t_chout ); // 1.3.0.0
    init_class_cherr( env, env->t_cherr ); // 1.3.0.0
    init_class_vec3( env, env->t_vec3 ); // 1.3.5.3
    init_class_vec4( env, env->t_vec4 ); // 1.3.5.3

    EM_log( CK_LOG_SEVERE, "class 'class'" );
    env->t_class->info = new Chuck_Namespace;
    env->t_class->info->add_ref();
    env->t_thread->info = new Chuck_Namespace;
    env->t_thread->info->add_ref();

    // pop indent
    EM_poplog();

    // default global values
    env->global()->value.add( "null", new Chuck_Value( env->t_null, "null", new void *(NULL), TRUE ) );
    env->global()->value.add( "NULL", new Chuck_Value( env->t_null, "NULL", new void *(NULL), TRUE ) );
    env->global()->value.add( "t_zero", new Chuck_Value( env->t_time, "time_zero", new t_CKDUR(0.0), TRUE ) );
    env->global()->value.add( "d_zero", new Chuck_Value( env->t_dur, "dur_zero", new t_CKDUR(0.0), TRUE ) );
    env->global()->value.add( "samp", new Chuck_Value( env->t_dur, "samp", new t_CKDUR(samp), TRUE ) );
    env->global()->value.add( "ms", new Chuck_Value( env->t_dur, "ms", new t_CKDUR(ms), TRUE ) );
    env->global()->value.add( "second", new Chuck_Value( env->t_dur, "second", new t_CKDUR(second), TRUE ) );
    env->global()->value.add( "minute", new Chuck_Value( env->t_dur, "minute", new t_CKDUR(minute), TRUE ) );
    env->global()->value.add( "hour", new Chuck_Value( env->t_dur, "hour", new t_CKDUR(hour), TRUE ) );
    env->global()->value.add( "day", new Chuck_Value( env->t_dur, "day", new t_CKDUR(day), TRUE ) );
    env->global()->value.add( "week", new Chuck_Value( env->t_dur, "week", new t_CKDUR(week), TRUE ) );
    env->global()->value.add( "true", new Chuck_Value( env->t_int, "true", new t_CKINT(1), TRUE ) );
    env->global()->value.add( "false", new Chuck_Value( env->t_int, "false", new t_CKINT(0), TRUE ) );
    env->global()->value.add( "maybe", new Chuck_Value( env->t_int, "maybe", new t_CKFLOAT(.5), FALSE ) );
    env->global()->value.add( "pi", new Chuck_Value( env->t_float, "pi", new t_CKFLOAT(ONE_PI), TRUE ) );
    env->global()->value.add( "global", new Chuck_Value( env->t_class, "global", env->global(), TRUE ) );
    env->global()->value.add( "chout", new Chuck_Value( env->t_io, "chout", new Chuck_IO_Chout( carrier ), TRUE ) );
    env->global()->value.add( "cherr", new Chuck_Value( env->t_io, "cherr", new Chuck_IO_Cherr( carrier ), TRUE ) );

    // TODO: can't use the following now is local to shred
    // env->global()->value.add( "now", new Chuck_Value( env->t_time, "now", &(vm->shreduler()->now_system), TRUE ) );

    /*
    S_enter( e->value, insert_symbol( "machine" ), env->t_null );
    S_enter( e->value, insert_symbol( "language" ), env->t_null );
    S_enter( e->value, insert_symbol( "compiler" ), env->t_null );
    S_enter( e->value, insert_symbol( "chout" ), env->t_system_out );
    S_enter( e->value, insert_symbol( "cherr" ), env->t_system_err );
    S_enter( e->value, insert_symbol( "stdout" ), env->t_system_out );
    S_enter( e->value, insert_symbol( "stderr" ), env->t_system_err );
    S_enter( e->value, insert_symbol( "midiout" ), env->t_midiout );
    S_enter( e->value, insert_symbol( "midiin" ), env->t_midiin );
    S_enter( e->value, insert_symbol( "adc" ), env->t_adc );
    S_enter( e->value, insert_symbol( "dac" ), env->t_dac );
    S_enter( e->value, insert_symbol( "bunghole" ), env->t_bunghole );
    S_enter( e->value, insert_symbol( "blackhole" ), env->t_bunghole );
    S_enter( e->value, insert_symbol( "endl" ), env->t_string );
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

    // pop indent level
    EM_poplog();

    return env;
}



//-----------------------------------------------------------------------------
// name: type_engine_shutdown()
// desc: ...
//-----------------------------------------------------------------------------
void type_engine_shutdown( Chuck_Env * env )
{
    // log
    EM_log( CK_LOG_SEVERE, "shutting down type checker..." );
    
    // shut it down
    SAFE_DELETE( env );
    
    // log
    EM_log( CK_LOG_SEVERE, "type checker shutdown complete." );
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
    Chuck_Context * context = new Chuck_Context;
    // save a reference to the parse tree
    context->parse_tree = prog;
    // set name
    context->filename = filename;

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
    EM_log( CK_LOG_FINER, "(pass 3) type-checking context '%s'...",
        context->filename.c_str() );
    // push indent
    EM_pushlog();
    // how much
    EM_log( CK_LOG_FINER, "target: %s", howmuch2str( how_much ) );

    // make sure there is a context
    if( !env->context )
    {
        // error
        EM_error2( 0, "internal error: env->context NULL!" );
        ret = FALSE; goto done;
    }

    // parse tree
    prog = context->parse_tree;
    if( !prog )
    {
        // error
        EM_error2( 0, "internal error: context->parse_tree NULL!" );
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
            EM_error2( prog->linepos,
                "internal error: unrecognized program section in type checker..." );
            ret = FALSE;
            break;
        }

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
    EM_log( CK_LOG_FINER, "unloading context '%s'...",
        env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    EM_log( CK_LOG_FINER, "restoring context '%s'...",
        env->contexts.back()->filename.c_str() );
    // assert( env->context->has_error == FALSE );
    // pop the context scope
    env->context->nspc->value.pop();
    // restore the current namespace
    env->curr = env->nspc_stack.back();
    // pop the namespace stack
    env->nspc_stack.pop_back();
    // release the context
    env->context->release();
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
            EM_error2( stmt->linepos, 
                "internal compiler error - no stmt type '%i'!", stmt->s_type );
            ret = FALSE;
            break;
    }

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
        if( isa( stmt->cond->type, env->t_io ) ) break;

        // error
        EM_error2( stmt->cond->linepos,
            "invalid type '%s' in if condition", stmt->cond->type->name.c_str() );
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
        EM_error2( stmt->linepos,
                  "empty for loop condition..." );
        EM_error2( stmt->linepos,
                  "...(note: explicitly use 'true' if it's the intent)" );
        EM_error2( stmt->linepos,
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
        if( isa( stmt->c2->stmt_exp->type, env->t_io ) ) break;

        // error
        EM_error2( stmt->c2->stmt_exp->linepos,
            "invalid type '%s' in for condition", stmt->c2->stmt_exp->type->name.c_str() );
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
        if( isa( stmt->cond->type, env->t_io ) ) break;

        // error
        EM_error2( stmt->cond->linepos,
            "invalid type '%s' in while condition", stmt->cond->type->name.c_str() );
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
        if( isa( stmt->cond->type, env->t_io ) ) break;

        // error
        EM_error2( stmt->cond->linepos,
            "invalid type '%s' in until condition", stmt->cond->type->name.c_str() );
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
    Chuck_Type * type = NULL;

    // check the conditional
    if( !(type = type_engine_check_exp( env, stmt->cond )) )
        return FALSE;
        
    // ensure the type in conditional is int (different from other loops)
    if( isa( type, env->t_float ) )
    {
        // cast
        stmt->cond->cast_to = env->t_int;
    }
    else if( !isa( type, env->t_int ) ) // must be int
    {
        EM_error2( stmt->linepos,
            "loop * conditional must be of type 'int'..." );
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
    EM_error2( stmt->linepos, "switch not implemented..." );

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
        EM_error2( br->linepos,
            "'break' found outside of for/while/until/switch..." );
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
        EM_error2( cont->linepos,
            "'continue' found outside of for/while/until..." );
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
        EM_error2( stmt->linepos, 
            "'return' statement found outside function definition" );
        return FALSE;
    }
    
    // check the type of the return
    if( stmt->val )
        ret_type = type_engine_check_exp( env, stmt->val );
    else
        ret_type = env->t_void;

    // check to see that return type matches the prototype
    if( ret_type && !isa( ret_type, env->func->def->ret_type ) )
    {
        EM_error2( stmt->linepos,
            "function '%s' was defined with return type '%s' -- but returning type '%s'",
            env->func->name.c_str(), env->func->def->ret_type->c_name(),
            ret_type->c_name() );
        return FALSE;
    }

    return ret_type != NULL;
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
            EM_error2( curr->linepos,
                "internal compiler error - no expression type '%i'...",
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

    // type check the lhs and rhs
    t_CKTYPE left = type_engine_check_exp( env, cl );
    t_CKTYPE right = type_engine_check_exp( env, cr );
    
    // if either fails, then return NULL
    if( !left || !right )
        return NULL;

    // cross chuck
    while( cr )
    {
        // type check the pair
        if( !(ret = type_engine_check_op( env, binary->op, cl, cr, binary )) )
            return NULL;

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
        EM_error2( exp->linepos,
            "cannot '%s' from/to a multi-variable declaration", op_str );
        return FALSE;
    }

    return TRUE;
}


// helper macros
#define LR( L, R )      if( (left->xid == L) && (right->xid == R) )
#define COMMUTE( L, R ) if( ( (left->xid == L) && (right->xid == R) ) || \
                            ( (left->xid == R) && (right->xid == L) ) )


//-----------------------------------------------------------------------------
// name: type_engine_check_op()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs, 
                               a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;
    assert( left && right );

    // make sure not involve multiple declarations (for now)
    if( !type_engine_ensure_no_multi_decl( lhs, op2str(op) ) ||
        !type_engine_ensure_no_multi_decl( rhs, op2str(op) ) )
        return FALSE;

    // if lhs is multi-value, then check separately
    if( (lhs->next && op != ae_op_chuck /*&& !isa( right, env->t_function)*/ ) || rhs->next )
    {
        // TODO: implement this
        EM_error2( lhs->linepos, 
            "multi-value (%s) operation not supported/implemented...",
            op2str(op));
        return NULL;
    }

    // file I/O
    /*
    if( op == ae_op_arrow_left || op == ae_op_arrow_right )
    {
        // check left
        if( isa( left, env->t_io ) )
        {
            // check for assignment
            if( op == ae_op_arrow_right && lhs->s_meta != ae_meta_var )
            {
                // error
                EM_error2( rhs->linepos,
                    "cannot perform I/O assignment via '->' to non-mutable value" );
                return NULL;
            }

            // check right
            if( isa( right, env->t_int ) || isa( right, env->t_float ) ||
                isa( right, env->t_string ) )
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
            if( isa( left, env->t_string ) && isa( right, env->t_object ) && !isa( right, env->t_string) )
                right = rhs->cast_to = env->t_string;
            else if( isa( left, env->t_object ) && isa( right, env->t_string ) && !isa( left, env->t_string) )
                left = lhs->cast_to = env->t_string;
        case ae_op_minus:
                LR( te_vec3, te_vec4 ) left = lhs->cast_to = env->t_vec4;
                else LR( te_vec4, te_vec3 ) right = rhs->cast_to = env->t_vec4;
        case ae_op_times:
        case ae_op_divide:
        case ae_op_lt:
        case ae_op_le:
        case ae_op_gt:
        case ae_op_ge:
        case ae_op_eq:
        case ae_op_neq:
            // complex
            LR( te_int, te_complex ) left = lhs->cast_to = env->t_complex;
            else LR( te_complex, te_int ) right = rhs->cast_to = env->t_complex;
            LR( te_float, te_complex ) left = lhs->cast_to = env->t_complex;
            else LR( te_complex, te_float ) right = rhs->cast_to = env->t_complex;
            
            // polar
            LR( te_int, te_polar ) left = lhs->cast_to = env->t_polar;
            else LR( te_complex, te_int ) right = rhs->cast_to = env->t_polar;
            LR( te_float, te_complex ) left = lhs->cast_to = env->t_polar;
            else LR( te_complex, te_float ) right = rhs->cast_to = env->t_polar;
        case ae_op_percent:
            // mark for cast
            LR( te_int, te_float ) left = lhs->cast_to = env->t_float;
            else LR( te_float, te_int ) right = rhs->cast_to = env->t_float;
        break;
        
        default: break;
        }

        // no commute - int/float
        switch( op )
        {
        case ae_op_plus_chuck:
            // Object.toString
            if( isa( left, env->t_object ) && isa( right, env->t_string ) && !isa( left, env->t_string ) )
                left = lhs->cast_to = env->t_string;
        case ae_op_minus_chuck:
        case ae_op_times_chuck:
        case ae_op_divide_chuck:
        case ae_op_percent_chuck:
            // mark for cast
            LR( te_int, te_float ) left = lhs->cast_to = env->t_float;
            LR( te_float, te_complex ) left = lhs->cast_to = env->t_complex;
            LR( te_float, te_polar ) left = lhs->cast_to = env->t_polar;
            LR( te_int, te_complex ) left = lhs->cast_to = env->t_complex;
            LR( te_int, te_polar ) left = lhs->cast_to = env->t_polar;
        break;
        
        default: break;
        }
        
        // int/dur and int/vectors
        if( op == ae_op_times )
        {
            LR( te_int, te_dur ) left = lhs->cast_to = env->t_float;
            else LR( te_dur, te_int ) right = rhs->cast_to = env->t_float;
            // vectors, 1.3.5.3
            else LR( te_int, te_vec3 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_int, te_vec4 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_vec3, te_int ) right = rhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_vec4, te_int ) right = rhs->cast_to = env->t_float; // 1.3.5.3
        }
        else if( op == ae_op_divide )
        {
            LR( te_dur, te_int ) right = rhs->cast_to = env->t_float;
            // vectors, 1.3.5.3
            else LR( te_int, te_vec3 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_int, te_vec4 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_vec3, te_int ) right = rhs->cast_to = env->t_float; // 1.3.5.3
            else LR( te_vec4, te_int ) right = rhs->cast_to = env->t_float; // 1.3.5.3
        }
        
        // op_chuck
        if( op == ae_op_times_chuck )
        {
            LR( te_int, te_vec3 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            LR( te_int, te_vec4 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
        }
        else if( op == ae_op_divide_chuck )
        {
            LR( te_int, te_vec3 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
            LR( te_int, te_vec4 ) left = lhs->cast_to = env->t_float; // 1.3.5.3
        }

        // array
        if( op == ae_op_shift_left && isa( left, env->t_array ) )
        {
            // type of array
            Chuck_Type * atype = left->array_type;
            // check type
            if( isa( atype, env->t_float ) )
            {
                if( right->xid == te_int ) right = rhs->cast_to = env->t_float;
            }
            else if( isa( atype, env->t_complex ) )
            {
                if( right->xid == te_int || right->xid == te_float 
                    || right->xid == te_polar ) right = rhs->cast_to = env->t_complex;
            }
            else if( isa( atype, env->t_polar ) )
            {
                if( right->xid == te_int || right->xid == te_float 
                    || right->xid == te_complex ) right = rhs->cast_to = env->t_complex;
            }
        }
    }
    
    // objects
    switch( op )
    {
    case ae_op_plus:
    case ae_op_plus_chuck:
        // take care of string
        if( isa( left, env->t_string ) )
        {
            // right is string or int/float
            if( isa( right, env->t_string ) || isa( right, env->t_int )
                || isa( right, env->t_float ) )
                break;
        }
        else if( isa( left, env->t_string ) || isa( left, env->t_int )
                 || isa( left, env->t_float ) )
        {
            // right is string
            if( isa( right, env->t_string ) )
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
        if( isa( left, env->t_object ) ) {
            EM_error2( lhs->linepos, "cannot perform '%s' on object references...",
                op2str(op) );
            return NULL;
        }
        if( isa( right, env->t_object ) ) {
            EM_error2( lhs->linepos, "cannot perform '%s' on object references...",
                op2str(op) );
            return NULL;
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
            EM_error2( lhs->linepos,
                "cannot assign '%s' on types '%s' %s '%s'...",
                op2str( op ), left->c_name(), op2str( op ), right->c_name() );
            EM_error2( lhs->linepos,
                "...(reason: --- right-side operand is not mutable)" );
            return NULL;
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
        return type_engine_check_op_unchuck( env, lhs, rhs );
    
    case ae_op_upchuck:
        return type_engine_check_op_upchuck( env, lhs, rhs );

    case ae_op_at_chuck:
        return type_engine_check_op_at_chuck( env, lhs, rhs );

    case ae_op_plus_chuck:
        if( isa( left, env->t_string ) && isa( right, env->t_string ) ) return env->t_string;
        if( isa( left, env->t_int ) && isa( right, env->t_string ) ) return env->t_string;
        if( isa( left, env->t_float ) && isa( right, env->t_string ) ) return env->t_string;
    case ae_op_plus:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_dur, te_dur ) return env->t_dur;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        // COMMUTE( te_float, te_complex ) return env->t_complex;
        // COMMUTE( te_float, te_polar ) return env->t_polar;
        LR( te_vec3, te_vec3 ) return env->t_vec3; // 1.3.5.3
        LR( te_vec4, te_vec4 ) return env->t_vec4; // 1.3.5.3
        COMMUTE( te_vec3, te_vec4 ) return env->t_vec4; // 1.3.5.3
        COMMUTE( te_dur, te_time ) return env->t_time;
        if( isa( left, env->t_string ) && isa( right, env->t_string ) ) return env->t_string;
        if( isa( left, env->t_string ) && isa( right, env->t_int ) ) return env->t_string;
        if( isa( left, env->t_string ) && isa( right, env->t_float ) ) return env->t_string;
        if( isa( left, env->t_int ) && isa( right, env->t_string ) ) return env->t_string;
        if( isa( left, env->t_float ) && isa( right, env->t_string ) ) return env->t_string;
    break;

    case ae_op_minus:
        LR( te_time, te_time ) return env->t_dur;
        LR( te_time, te_dur ) return env->t_time;
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_dur, te_dur ) return env->t_dur;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        // COMMUTE( te_float, te_complex ) return env->t_complex;
        // COMMUTE( te_float, te_polar ) return env->t_polar;
        LR( te_vec3, te_vec3 ) return env->t_vec3; // 1.3.5.3
        LR( te_vec4, te_vec4 ) return env->t_vec4; // 1.3.5.3
        COMMUTE( te_vec3, te_vec4 ) return env->t_vec4; // 1.3.5.3
    break;

    // take care of non-commutative
    case ae_op_minus_chuck:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_dur, te_dur ) return env->t_dur;
        LR( te_dur, te_time ) return env->t_time;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        // COMMUTE( te_float, te_complex ) return env->t_complex;
        // COMMUTE( te_float, te_polar ) return env->t_polar;
        LR( te_vec3, te_vec3 ) return env->t_vec3; // 1.3.5.3
        LR( te_vec4, te_vec4 ) return env->t_vec4; // 1.3.5.3
        COMMUTE( te_vec3, te_vec4 ) return env->t_vec4; // 1.3.5.3
    break;

    case ae_op_times:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        LR( te_vec3, te_vec3 ) return env->t_vec3; // 1.3.5.3
        LR( te_vec4, te_vec4 ) return env->t_vec4; // 1.3.5.3
        COMMUTE( te_float, te_dur ) return env->t_dur;
        // COMMUTE( te_float, te_complex ) return env->t_complex;
        // COMMUTE( te_float, te_polar ) return env->t_polar;
        COMMUTE( te_float, te_vec3 ) return env->t_vec3; // 1.3.5.3
        COMMUTE( te_float, te_vec4 ) return env->t_vec4; // 1.3.5.3
    break;

    case ae_op_times_chuck:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_float, te_dur ) return env->t_dur;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        LR( te_float, te_vec3 ) return env->t_vec3; // 1.3.5.3
        LR( te_int, te_vec4 ) return env->t_vec4; // 1.3.5.3
    break;

    case ae_op_divide:
        LR( te_dur, te_dur ) return env->t_float;
        LR( te_time, te_dur ) return env->t_float;
        LR( te_dur, te_float ) return env->t_dur;
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
        LR( te_vec3, te_float ) return env->t_vec3;
        LR( te_vec4, te_float ) return env->t_vec4;
    break;

    // take care of non-commutative
    case ae_op_divide_chuck:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_float, te_dur ) return env->t_dur;
        LR( te_complex, te_complex ) return env->t_complex;
        LR( te_polar, te_polar ) return env->t_polar;
    break;

    case ae_op_eq:
        // null
        // if( isa( left, env->t_object ) && isa( right, env->t_null ) ) return env->t_int;
        // if( isa( left, env->t_null ) && isa( right, env->t_object ) ) return env->t_int;
    case ae_op_lt:
    case ae_op_gt:
    case ae_op_le:
        // file output
        if( isa( left, env->t_io ) )
        {
            if( isa( right, env->t_int ) ) return left;
            else if( isa( right, env->t_float ) ) return left;
            else if( isa( right, env->t_string ) ) return left;
            else // error
            {
                EM_error2( lhs->linepos, "on suitable IO action for '%s' <= '%s'...",
                    left->c_name(), right->c_name() );
                return NULL;
            }
        }
    case ae_op_ge:
    case ae_op_neq:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_int;
        LR( te_dur, te_dur ) return env->t_int;
        LR( te_time, te_time ) return env->t_int;
        LR( te_complex, te_complex ) return env->t_int;
        LR( te_polar, te_polar ) return env->t_int;
        // COMMUTE( te_float, te_complex ) return env->t_int;
        // COMMUTE( te_float, te_polar ) return env->t_int;
        LR( te_vec3, te_vec3 ) return env->t_int; // 1.3.5.3
        LR( te_vec4, te_vec4 ) return env->t_int; // 1.3.5.3
        COMMUTE( te_vec3, te_vec4 ) return env->t_int; // 1.3.5.3
        if( isa( left, env->t_object ) && isa( right, env->t_object ) ) return env->t_int;
    break;

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
    case ae_op_shift_left:
        // prepend || append
        if( isa( left, env->t_array ) )
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
        // shift
        LR( te_int, te_int ) return env->t_int;
    break;

    case ae_op_percent:
        LR( te_time, te_dur ) return env->t_dur;
        LR( te_dur, te_dur ) return env->t_dur;
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
    break;

    // take of non-commutative
    case ae_op_percent_chuck:
        LR( te_int, te_int ) return env->t_int;
        LR( te_float, te_float ) return env->t_float;
        LR( te_dur, te_dur ) return env->t_dur;
    break;

    default: break;
    }

    // no match
    EM_error2( lhs->linepos,
        "cannot resolve operator '%s' on types '%s' and '%s'",
        op2str( op ), left->c_name(), right->c_name() );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs,
                                     a_Exp_Binary binary )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // chuck to function
    if( isa( right, env->t_function ) )
    {
        // treat this function call
        return type_engine_check_exp_func_call( env, rhs, lhs, binary->ck_func, binary->linepos );
    }

    // multi-value not supported beyond this for now
    if( lhs->next || rhs->next )
    {
        EM_error2( lhs->linepos,
            "multi-value (=>) operation not supported/implemented..." );
        return NULL;
    }

    // ugen => ugen
    // ugen[] => ugen[]
    if( ( isa( left, env->t_ugen ) || ( isa( left, env->t_array ) && isa( left->array_type, env->t_ugen ) ) ) &&
        ( isa( right, env->t_ugen ) || ( isa( right, env->t_array ) && isa( right->array_type, env->t_ugen ) ) ) )
    {
        t_CKTYPE left_ugen_type = NULL;
        t_CKTYPE right_ugen_type = NULL;
        
        if( isa( left, env->t_array ) )
        {
            left_ugen_type = left->array_type;
            
            if( left->array_depth > 1 )
            {
                EM_error2( lhs->linepos, "array ugen type has more than one dimension - can only => one-dimensional array of mono ugens" );
                return NULL;
            }
            
            if( left_ugen_type->ugen_info->num_outs > 1 )
            {
                // error
                EM_error2( lhs->linepos,
                           "array ugen type '%s' has more than one output channel - can only => one-dimensional array of mono ugens",
                           left_ugen_type->c_name() );
                return NULL;
            }
        }
        else
        {
            left_ugen_type = left;
        }
        
        if( isa( right, env->t_array ) )
        {
            right_ugen_type = right->array_type;
            
            if( right->array_depth > 1 )
            {
                EM_error2( rhs->linepos,
                           "array ugen type has more than one dimension - can only => one-dimensional array of mono ugens" );
                return NULL;
            }
            
            if( right_ugen_type->ugen_info->num_ins > 1 )
            {
                // error
                EM_error2( rhs->linepos,
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
            EM_error2( lhs->linepos,
                "ugen's of type '%s' have no output - cannot => to another ugen...",
                left_ugen_type->c_name() );
            return NULL;
        }
        else if( right_ugen_type->ugen_info->num_ins == 0 )
        {
            // error
            EM_error2( rhs->linepos,
                "ugen's of type '%s' have no input - cannot => from another ugen...",
                right_ugen_type->c_name() );
            return NULL;
        }

        return right;
    }
    
    

    // time advance ( dur => now )
    if( isa( left, env->t_dur ) && isa( right, env->t_time ) && rhs->s_meta == ae_meta_var
        && rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        return right;
    }

    // event wait ( Event => now )
    if( isa( left, env->t_event ) && isa( right, env->t_time ) && rhs->s_meta == ae_meta_var
        && rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        return right;
    }
    
    // input ( IO => int ), (IO => float)
    if( isa( left, env->t_io ) )
    {
        // right hand side
        if( rhs->s_meta != ae_meta_var )
        {
            // error
            EM_error2( rhs->linepos,
                "cannot perform I/O assignment via '=>' to non-mutable value" );
            return NULL;
        }

        // check right
        if( isa( right, env->t_int ) || isa( right, env->t_float ) )
        {
            // emit ref
            rhs->emit_var = TRUE;
            return left;
        }
        
        if( isa( right, env->t_string ) )
        {
            return left;
        }
    }

    // object.toString

    // implicit cast
    LR( te_int, te_float ) left = lhs->cast_to = env->t_float;
    LR( te_int, te_complex ) left = lhs->cast_to = env->t_complex;
    LR( te_int, te_polar ) left = lhs->cast_to = env->t_polar;
    LR( te_vec3, te_vec4 ) left = lhs->cast_to = env->t_vec4;

    // assignment or something else
    if( isa( left, right ) )
    {
        // basic types?
        if( type_engine_check_primitive( env, left ) || isa( left, env->t_string ) )
        {
            // if the right is a decl, then make sure ref
            if( isa( left, env->t_string ) && rhs->s_type == ae_exp_decl )
            {
                rhs->decl.type->ref = TRUE;
            }
            // TODO: const
            // assigment?
            if( rhs->s_meta == ae_meta_var )
            {
                // emit ref - remember for emitter
                rhs->emit_var = TRUE;
                // right side
                return right;
            }

            // error
            EM_error2( lhs->linepos,
                "cannot chuck/assign '=>' on types '%s' => '%s'...",
                left->c_name(), right->c_name() );
            EM_error2( lhs->linepos,
                "...(reason: right-side operand is not mutable)" );
            return NULL;
        }
        // aggregate types
        else
        {
            // TODO: check overloading of =>

            // no match
            EM_error2( lhs->linepos,
                "cannot resolve operator '=>' on types '%s' and '%s'...",
                left->c_name(), right->c_name() );
            EM_error2( lhs->linepos,
                "...(note: use '@=>' for object reference assignment)" );
            return NULL;
        }
    }

    // TODO: check overloading of =>

    // no match
    EM_error2( lhs->linepos,
        "cannot resolve operator '=>' on types '%s' and '%s'...",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    t_CKTYPE left = lhs->type, right = rhs->type;
    
    // ugen =< ugen
    if( isa( left, env->t_ugen ) && isa( right, env->t_ugen ) ) return right;
    
    // TODO: check overloading of =<
    
    // no match
    EM_error2( lhs->linepos,
        "no suitable resolution for binary operator '=<'..." );
    EM_error2( lhs->linepos, 
        "...on types '%s' and '%s'",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_upchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    t_CKTYPE left = lhs->type, right = rhs->type;
    
    // uana =^ uana
    if( isa( left, env->t_uana ) && isa( right, env->t_uana ) ) return right;
    
    // TODO: check overloading of =^
    
    // no match
    EM_error2( lhs->linepos,
        "no suitable resolution for binary operator '=^'..." );
    EM_error2( lhs->linepos, 
        "...on types '%s' and '%s'",
        left->c_name(), right->c_name() );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_op_at_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    t_CKTYPE left = lhs->type, right = rhs->type;

    // static
    //if( isa( left, env->t_class ) )
    //{
    //    EM_error2( lhs->linepos,
    //        "cannot assign '@=>' using static class as left operand..." );
    //    return NULL;
    //}
    //else if( isa( right, env->t_class ) )
    //{
    //    EM_error2( rhs->linepos,
    //        "cannot assign '@=>' using static class as right operand..." );
    //    return NULL;
    //}

    // make sure mutable
    if( rhs->s_meta != ae_meta_var )
    {
        EM_error2( lhs->linepos,
            "cannot assign '@=>' on types '%s' @=> '%s'...",
            left->c_name(), right->c_name() );
        EM_error2( lhs->linepos,
            "...(reason: --- right-side operand is not mutable)" );
        return NULL;
    }

    // if the right is a decl, then make sure ref
    if( rhs->s_type == ae_exp_decl )
    {
        rhs->decl.type->ref = TRUE;
    }

    // implicit cast
    LR( te_int, te_float ) left = lhs->cast_to = env->t_float;

    // primitive
    if( !isa( left, right ) )
    {
        EM_error2( lhs->linepos,
            "cannot assign '@=>' on types '%s' @=> '%s'...",
            left->c_name(), right->c_name() );
        EM_error2( lhs->linepos,
            "...(reason: --- incompatible types for assignment)" );
        return NULL;
    }

    // assign
    rhs->emit_var = TRUE;

    return right;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_unary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_unary( Chuck_Env * env, a_Exp_Unary unary )
{
    Chuck_Type * t = NULL;

    // make sure
    if( unary->exp )
    {
        // assert( unary->op == ae_op_new );
        t = type_engine_check_exp( env, unary->exp );
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
                EM_error2( unary->linepos,
                    "prefix unary operator '%s' cannot "
                    "be used on non-mutable data-types...", op2str( unary->op ) );
                return NULL;
            }
            
            // assign
            unary->exp->emit_var = TRUE;

            // check type
            if( isa( t, env->t_int ) || isa( t, env->t_float ) )
                return t;
                
            // TODO: check overloading
        break;
        
        case ae_op_minus:
            // float
            if( isa( t, env->t_float ) ) return t;
        case ae_op_tilda:
        case ae_op_exclamation:
            // int
            if( isa( t, env->t_int ) ) return t;
        break;
        
        case ae_op_spork:
            // spork shred (by function call)
            if( unary->exp && unary->exp->s_type == ae_exp_func_call ) return env->t_shred;
            // spork shred (by code segment)
            // else if( unary->code ) return env->t_shred;
            // got a problem
            else
            {
                 EM_error2( unary->linepos,
                     "only function calls can be sporked..." );
                 return NULL;
            }
        break;

        case ae_op_new:
            // look up the type
            // t = env->curr->lookup_type( unary->type->xid->xid, TRUE );
            t = type_engine_find_type( env, unary->type->xid );
            if( !t )
            {
                EM_error2( unary->linepos,
                    "... in 'new' expression ..." );
                return NULL;
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
                    EM_error2( unary->linepos, "cannot use empty [] with 'new'..." );
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
                    env->t_array,  // the array base class, usually env->t_array
                    unary->array->depth,  // the depth of the new type
                    t,  // the 'array_type'
                    env->curr  // the owner namespace
                );

                // TODO: ref?
            }

            // make sure the type is not a primitive
            if( isa( t, env->t_int ) || isa( t, env->t_float ) || isa( t, env->t_dur ) 
                || isa( t, env->t_time ) )
            {
                EM_error2( unary->linepos,
                    "cannot instantiate/(new) primitive type '%s'...",
                    t->c_name() );
                EM_error2( unary->linepos,
                    "...(primitive types: 'int', 'float', 'time', 'dur')" );
                return NULL;
            }

            // make sure the type is not a reference
            if( unary->type->ref && !unary->array )
            {
                EM_error2( unary->linepos,
                    "cannot instantiate/(new) single object references (@)..." );
                return NULL;
            }

            // return the type
            return t;
        break;

        default: break;
    }
    
    // no match
    EM_error2( unary->linepos,
        "no suitable resolution for prefix unary operator '%s' on type '%s...",
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
                    EM_error2( exp->linepos,
                        "keyword 'this' can be used only inside class definition..." );
                    return NULL;
                }

                // in member func
                if( env->func && !env->func->is_member )
                {
                    EM_error2( exp->linepos,
                        "keyword 'this' cannot be used inside static functions..." );
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
                t = env->t_shred;
            }
            else if( str == "now" ) // now
            {
                // assignable in a strongly timed way
                exp->self->s_meta = ae_meta_var;
                // time
                t = env->t_time;
            }
            else if( str == "dac" ) // dac
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->t_dac;
            }
            else if( str == "adc" ) // adc
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->t_adc;
            }
            else if( str == "blackhole" ) // blackhole
            {
                // non assignable
                exp->self->s_meta = ae_meta_value;
                // ugen
                t = env->t_ugen;
            }
            else if( str == "null" || str == "NULL" ) // null / NULL
            {
                // not assignable
                exp->self->s_meta = ae_meta_value;
                // refers to null
                t = env->t_null;
            }
            else  // look up
            {
                // look in local scope first
                // v = env->curr->lookup_value( exp->var, FALSE );
                v = type_engine_find_value( env, S_name(exp->var), FALSE );
                if( !v )
                {
                    // if in class
                    if( env->class_def )
                    {
                        // see if in parent
                        v = type_engine_find_value( env->class_def->parent, exp->var );
                    }

                    // still not found
                    if( !v )
                    {
                        // look globally
                        // v = env->curr->lookup_value( exp->var, TRUE );
                        v = type_engine_find_value( env, S_name(exp->var), TRUE, exp->linepos );
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
                                if( env->func->def->static_decl == ae_key_static &&
                                    v->is_member && !v->is_static )
                                {
                                    EM_error2( exp->linepos,
                                        "non-static member '%s' used from static function...", S_name(exp->var) );
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
                            EM_error2( exp->linepos,
                                "undefined variable '%s'...", S_name(exp->var) );
                            return NULL;
                        }
                        else
                        {
                            EM_error2( exp->linepos,
                                "undefined variable/member '%s' in class/namespace '%s'...",
                                S_name(exp->var), env->class_def->name.c_str() );
                            return NULL;
                        }
                    }
                }

                // make sure v is legit as this point
                if( !v->is_decl_checked )
                {
                    EM_error2( exp->linepos,
                        "variable/member '%s' is used before declaration...",
                        S_name(exp->var) );
                    return NULL;
                }
                
                // 1.3.1.0: hack catch "pi"
                // if( v->name == "pi" )
                // {
                //    // check level
                //    if( env->deprecate_level < 2 )
                //    {
                //        EM_error2( exp->linepos, "deprecated: '%s' --> use: '%s'...",
                //                   "pi", "Math.PI" );
                //    }
                // }

                // the type
                t = v->type;
                // remember
                exp->value = v;
            }
        break;

        // int
        case ae_primary_num:
            t = env->t_int;
        break;

        // float
        case ae_primary_float:
            t = env->t_float;
        break;

        // string
        case ae_primary_str:
            // escape the thing
            if( !escape_str( exp->str, exp->linepos ) )
                return NULL;

            // a string
            t = env->t_string;
        break;

        // char
        case ae_primary_char:
            // check escape sequences
            if( str2char( exp->chr, exp->linepos ) == -1 )
                return NULL;
            
            // a string
            t = env->t_int;
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
            // make sure not l-value
            if( exp->exp->s_type == ae_exp_decl )
            {
                EM_error2( exp->linepos,
                    "cannot use <<< >>> on variable declarations..." );
                return NULL;
            }

            t = type_engine_check_exp( env, exp->exp );
        break;

        // nil
        case ae_primary_nil:
            t = env->t_void;
        break;

        // no match
        default:
            EM_error2( exp->linepos,
                "internal error - unrecognized primary type '%i'...", exp->s_type );
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
        EM_error2( exp->linepos,
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
                if( isa( t, env->t_int ) && isa( type, env->t_float ) )
                {
                    // cast from int to float
                    e->cast_to = type;
                }
                else
                {
                    // incompatible
                    EM_error2( e->linepos, "array init [...] contains incompatible types..." );
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
        env->t_array,  // the array base class, usually env->t_array
        type->array_depth + 1,  // the depth of the new type
        type->array_depth ? type->array_type : type,  // the 'array_type'
        env->curr  // the owner namespace
    );

    /*
    // create the new type
    t = env->context->new_Chuck_Type();
    // set the xid
    t->xid = te_array;
    // set the name
    t->name = type->name;
    // set the parent
    t->parent = env->t_array;
    // is a ref
    t->size = t_array.size;
    // set the array depth
    t->array_depth = type->array_depth + 1;
    // set the base type
    t->array_type = type->array_depth ? type->array_type : type;
    // TODO: verify the following is correct
    // set namespace
    t->info = t_array->info;
    // add reference
    t->info->add_ref();
    // set owner
    t->owner = env->curr;
    */

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
        EM_error2( exp->linepos,
            "missing imaginary component in complex value...\n"
            "    --> format: #(re,im)" );
        return NULL;
    }
    // check if we have too much
    if( val->im->next != NULL )
    {
        EM_error2( exp->linepos,
            "extraneous arguments in complex value...\n"
            "    --> format #(re,im)" );
        return NULL;
    }

    // find types (only need real, since imag is linked after real)
    if( !(type_re = type_engine_check_exp( env, val->re )) )
        // || !(type_im = type_engine_check_exp( env, val->im )) ) 
        return NULL;

    // imaginary
    type_im = val->im->type;

    // check types
    if( !isa( type_re, env->t_float ) && !isa( type_re, env->t_int ) )
    {
        EM_error2( exp->linepos,
            "invalid type '%s' in real component of complex value...\n"
            "    (must be of type 'int' or 'float')", type_re->c_name() );
        return NULL;
    }
    if( !isa( type_im, env->t_float ) && !isa( type_im, env->t_int ) )
    {
        EM_error2( exp->linepos,
            "invalid type '%s' in imaginary component of complex value...\n"
            "    (must be of type 'int' or 'float')", type_im->c_name() );
        return NULL;
    }

    // implcit cast
    if( isa( type_re, env->t_int ) ) val->re->cast_to = env->t_float;
    if( isa( type_im, env->t_int ) ) val->im->cast_to = env->t_float;

    return env->t_complex;
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
        EM_error2( exp->linepos,
            "missing phase component in polar value...\n"
            "    --> format: %%(mag,phase)" );
        return NULL;
    }
    // check if we have extra
    if( val->phase->next != NULL )
    {
        EM_error2( exp->linepos,
            "extraneous arguments in polar value...\n"
            "    --> format: %%(mag,phase)" );
        return NULL;
    }

    // find types
    if( !(type_mod = type_engine_check_exp( env, val->mod )) ||
        !(type_phase = type_engine_check_exp( env, val->phase )) )
        return NULL;

    // check types
    if( !isa( type_mod, env->t_float ) && !isa( type_mod, env->t_int ) )
    {
        EM_error2( exp->linepos,
            "invalid type '%s' in magnitude component of polar value...\n"
            "    (must be of type 'int' or 'float')", type_mod->c_name() );
        return NULL;
    }
    if( !isa( type_phase, env->t_float ) && !isa( type_phase, env->t_int ) )
    {
        EM_error2( exp->linepos,
            "invalid type '%s' in phase component of polar value...\n"
            "    (must be of type 'int' or 'float')", type_phase->c_name() );
        return NULL;
    }

    // implcit cast
    if( isa( type_mod, env->t_int ) ) val->mod->cast_to = env->t_float;
    if( isa( type_phase, env->t_int ) ) val->phase->cast_to = env->t_float;

    return env->t_polar;
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
        EM_error2( exp->linepos,
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
        if( !(t = type_engine_check_exp(env, e )) )
            return NULL;
        // implicit cast
        if( isa( t, env->t_int ) ) e->cast_to = env->t_float;
        else if( !isa( t, env->t_float ) )
        {
            EM_error2( exp->linepos,
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
    if( val->numdims < 4 )
        return env->t_vec3;
    
    // vector 4d
    return env->t_vec4;
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
    // t_CKTYPE t2 = env->curr->lookup_type( cast->type->xid->xid, TRUE );
    t_CKTYPE t2 = type_engine_find_type( env, cast->type->xid );
    if( !t2 )
    {
        EM_error2( cast->linepos, "... in cast expression ..." );
        return NULL;
    }
    
    // check if cast valid
    if( !type_engine_check_cast_valid( env, t2, t ) )
    {
        EM_error2( cast->linepos,
            "invalid cast to '%s' from '%s'...",
            S_name( cast->type->xid->xid ), t->c_name() );
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
    if( isa( to, env->t_float ) && isa( from, env->t_int ) ) return TRUE;
    if( isa( to, env->t_int ) && isa( from, env->t_float ) ) return TRUE;
    if( isa( to, env->t_complex ) && isa( from, env->t_int ) ) return TRUE;
    if( isa( to, env->t_polar ) && isa( from, env->t_int ) ) return TRUE;
    if( isa( to, env->t_complex ) && isa( from, env->t_float ) ) return TRUE;
    if( isa( to, env->t_polar ) && isa( from, env->t_float ) ) return TRUE;
    if( isa( to, env->t_complex ) && isa( from, env->t_polar ) ) return TRUE;
    if( isa( to, env->t_polar ) && isa( from, env->t_complex ) ) return TRUE;
    if( isa( to, env->t_vec3 ) && isa( from, env->t_vec4 ) ) return TRUE;
    if( isa( to, env->t_vec4 ) && isa( from, env->t_vec3 ) ) return TRUE;

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
    if( !isa( base, env->t_int ) && !isa( base, env->t_float ) )
    {
        EM_error2( dur->base->linepos,
            "invalid type '%s' in prefix of dur expression...\n"
            "    (must be of type 'int' or 'float')", base->c_name() );
        return NULL;
    }
    
    // check unit type
    if( !isa( unit, env->t_dur ) )
    {
        EM_error2( dur->unit->linepos,
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
                EM_error2( postfix->exp->linepos,
                    "postfix operator '%s' cannot be used on non-mutable data-type...",
                    op2str( postfix->op ) );
                return NULL;
            }
            
            postfix->exp->emit_var = TRUE;
            // TODO: mark somewhere we need to post increment
            
            // check type
            if( isa( t, env->t_int ) || isa( t, env->t_float ) )
                return t;
        break;
        
        default:
            // no match
            EM_error2( postfix->linepos,
                "internal compiler error: unrecognized postfix '%i'", postfix->op );
        return NULL;
    }
    
    // no match
    EM_error2( postfix->linepos,
        "no suitable resolutation for postfix operator '%s' on type '%s'...",
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
    if( !cond || !if_exp || !else_exp ) return NULL;
    
    // check the type
    if( !isa( cond, env->t_int ) ) return NULL;
    
    // make sure the if and else have compatible types
    // TODO: the lesser of two types
    if( !( *if_exp == *else_exp ) )
    {
        EM_error2( exp_if->linepos,
            "incompatible types '%s' and '%s' in if expression...",
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
        if( !isa( exp->type, env->t_int ) )
        {
            EM_error2( exp->linepos,
                "incompatible array subscript type '%s'...",
                exp->type->name.c_str() );
            return FALSE;
        }

        exp = exp->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_decl( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_decl( Chuck_Env * env, a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;
    Chuck_Type * type = NULL;
    Chuck_Value * value = NULL;
    t_CKBOOL is_obj = FALSE;
    t_CKBOOL is_ref = FALSE;

    // loop through the variables
    while( list != NULL )
    {
        // get the decl
        var_decl = list->var_decl;

        // (if at class_scope) check if in parent
        // TODO: sort
        if( env->class_def && env->class_scope == 0 && ( value =
            type_engine_find_value( env->class_def->parent, var_decl->xid ) ) )
        {
            EM_error2( var_decl->linepos,
                "'%s' has already been defined in super class '%s'...",
                S_name(var_decl->xid), value->owner_class->c_name() );
            return NULL;
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
        is_ref = decl->type->ref;

        // if array, then check to see if empty []
        if( var_decl->array && var_decl->array->exp_list != NULL )
        {
            // instantiate object, including array
            if( !type_engine_check_exp( env, var_decl->array->exp_list ) )
                return FALSE;

            // check the subscripts
            if( !type_engine_check_array_subscripts( env, var_decl->array->exp_list ) )
                return FALSE;
        }

        // member?
        if( value->is_member )
        {
            // offset
            value->offset = env->curr->offset;

            /*******************************************************************************
             * spencer: added this into function to provide the same logic path
             * for type_engine_check_exp_decl() and ck_add_mvar() when they determine
             * offsets for mvars 
             ******************************************************************************/
            // move the offset (TODO: check the size)
            env->curr->offset = type_engine_next_offset( env->curr->offset, type );
            // env->curr->offset += type->size;
        }
        else if( decl->is_static ) // static
        {
            // base scope
            if( env->class_def == NULL || env->class_scope > 0 )
            {
                EM_error2( decl->linepos,
                    "static variables must be declared at class scope..." );
                return FALSE;
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
                EM_error2( var_decl->linepos,
                    "cannot declare static non-primitive objects (yet)..." );
                EM_error2( var_decl->linepos,
                    "...(hint: declare as ref (@) & initialize outside for now)" );
                return FALSE;
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
// name: type_engine_print_exp_dot_member()
// desc: ...
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

    // is the base a class/namespace or a variable
    base_static = isa( member->t_base, env->t_class );
    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;

    // this
    str = S_name(member->xid);

    return string(the_base->c_name()) + std::string(".") + str;
}




//-----------------------------------------------------------------------------
// name: find_func_match_actual()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Func * find_func_match_actual( Chuck_Env * env, Chuck_Func * up, a_Exp args,
                                     t_CKBOOL implicit, t_CKBOOL specific )
{
    a_Exp e;
    a_Arg_List e1;
    t_CKUINT count;
    Chuck_Func * func;
    t_CKBOOL match = FALSE;

    // see if args is nil
    if( args && args->type == env->t_void )
        args = NULL;

    // up is the list of functions in single class / namespace
    while( up )
    {
        func = up;
        // loop
        while( func )
        {
            e = args;
            e1 = func->def->arg_list;
            count = 1;

            // check arguments against the definition
            while( e )
            {
                // check for extra arguments
                if( e1 == NULL ) goto moveon;

                // get match
                match = specific ? e->type == e1->type : isa( e->type, e1->type );

                // no match
                if( !match )
                {
                    // TODO: fix this for overload implicit cast (multiple matches)
                    if( implicit && e->type == env->t_int && e1->type == env->t_float )
                    {
                        // int to float
                        e->cast_to = env->t_float;
                    }
                    else goto moveon; // type mismatch
                }

                e = e->next;
                e1 = e1->next;
                count++;
            }

            // check for extra arguments
            if( e1 == NULL ) return func;

moveon:
            // next func
            func = func->next;
        }

        // go up
        if( up->up ) up = up->up->func_ref;
        else up = NULL;
    }

    // not found
    return NULL;
}




//-----------------------------------------------------------------------------
// name: find_func_match()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Func * find_func_match( Chuck_Env * env, Chuck_Func * up, a_Exp args )
{
    Chuck_Func * func = NULL;

    // try to find specific
    func = find_func_match_actual( env, up, args, FALSE, TRUE );
    if( func ) return func;

    // try to find specific with implicit
    func = find_func_match_actual( env, up, args, TRUE, TRUE );
    if( func ) return func;

    // try to find non-specific
    func = find_func_match_actual( env, up, args, FALSE, FALSE );
    if( func ) return func;

    // try to find non-specific with implicit
    func = find_func_match_actual( env, up, args, TRUE, FALSE );
    if( func ) return func;

    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args, 
                                          t_CKFUNC & ck_func, int linepos )
{
    Chuck_Func * func = NULL;
    Chuck_Func * up = NULL;

    // type check the func
    t_CKTYPE f = exp_func->type = type_engine_check_exp( env, exp_func );
    if( !f ) return NULL;

    // void type for args
    t_CKTYPE a = env->t_void;

    // make sure we have a function
    if( !isa( f, env->t_function ) )
    {
        EM_error2( exp_func->linepos,
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
    func = find_func_match( env, up, args );

    // no func
    if( !func )
    {
        // if primary
        if( exp_func->s_type == ae_exp_primary && exp_func->primary.s_type == ae_primary_var )
        {
            EM_error2( exp_func->linepos,
                "argument type(s) do not match:" );
            EM_error2( exp_func->linepos,
                "... for function '%s(...)' ...",
                S_name(exp_func->primary.var) );
        }
        else if( exp_func->s_type == ae_exp_dot_member )
        {
            EM_error2( exp_func->linepos,
                "arguments type(s) do not match:" );
            EM_error2( exp_func->linepos,
                "... for function '%s(...)' ...",
                type_engine_print_exp_dot_member( env, &exp_func->dot_member ).c_str() );
        }
        else
        {
            EM_error2( exp_func->linepos,
                "argument type(s) do not match for function ..." );
        }

        EM_error2( exp_func->linepos,
            "...(please check the argument types)" );

        return NULL;
    }

    // recheck the type with new name
    if( exp_func->s_type == ae_exp_primary && exp_func->primary.s_type == ae_primary_var )
    {
        // set the new name
        // TODO: clear old
        exp_func->primary.var = insert_symbol(func->name.c_str());
        // make sure the type is still the name
        if( *exp_func->type != *type_engine_check_exp( env, exp_func ) )
        {
            // error
            EM_error2( exp_func->linepos,
                "internal error: function type different on second check..." );
            return NULL;
        }
    }
    else if( exp_func->s_type == ae_exp_dot_member )
    {
        // set the new name
        // TODO: clear old
        exp_func->dot_member.xid = insert_symbol(func->name.c_str());
        /*
        // TODO: figure if this is necessary - it type checks things twice!
        // make sure the type is still the name
        if( *exp_func->type != *type_engine_check_exp( env, exp_func ) )
        {
            // error
            EM_error2( exp_func->linepos,
                "internal error: function type different on second check..." );
            return NULL;
        }
        */
    }
    else assert( FALSE );

    ck_func = func;

    return func->def->ret_type;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKTYPE type_engine_check_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call )
{
    // type check it
    return type_engine_check_exp_func_call( env, func_call->func, func_call->args,
                                            func_call->ck_func, func_call->linepos );
}




//-----------------------------------------------------------------------------
// name: type_engine_check_exp_dot_member_special()
// desc: check special case for complex, polar, vec3, vec4; ge: 1.3.5.3
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
                EM_error2( member->base->linepos,
                          "cannot assign value to literal complex value..." );
                return NULL;
            }
            
            return env->t_float;
        }
        else
        {
            // not valid
            EM_error2( member->linepos,
                      "type '%s' has no member named '%s'...", member->t_base->c_name(), str.c_str() );
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
                EM_error2( member->base->linepos,
                          "cannot assign value to literal polar value..." );
                return NULL;
            }
            
            return env->t_float;
        }
        else
        {
            // not valid
            EM_error2( member->linepos,
                      "type '%s' has no member named '%s'...", member->t_base->c_name(), str.c_str() );
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
                EM_error2( member->base->linepos,
                           "cannot assign value to literal %s value...",
                           member->t_base->c_name() );
                return NULL;
            }
            
            return env->t_float;
        }
        // vec4 only
        else if( member->t_base->xid == te_vec4 && (str == "w" || str == "a") )
        {
            // check addressing consistency (ISSUE: emit_var set after!)
            if( member->self->emit_var && member->base->s_meta != ae_meta_var )
            {
                // error
                EM_error2( member->base->linepos,
                           "cannot assign value to literal %s value...",
                           member->t_base->c_name() );
                return NULL;
            }
            
            return env->t_float;
        }
        else
        {
            // check function
            goto check_func;
        }
    }
    
    // should not get here
    EM_error2( member->base->linepos,
               "type checer internal error in special literal..." );

    return NULL;

check_func:
    
    Chuck_Value * value = NULL;
    Chuck_Type * the_base = NULL;
    t_CKBOOL base_static = FALSE;

    // is the base a class/namespace or a variable
    base_static = isa( member->t_base, env->t_class );
    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;
    
    // check base; 1.3.5.3
    if( member->base->s_meta == ae_meta_value ) // is literal
    {
        // error
        EM_error2( member->base->linepos,
                  "cannot call function from literal %s value...",
                  member->t_base->c_name() );
        return NULL;
    }
    
    // find the value
    value = type_engine_find_value( the_base, member->xid );
    if( !value )
    {
        // can't find member
        EM_error2( member->base->linepos,
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
        case te_vec3:
        case te_vec4:
            return type_engine_check_exp_dot_member_special( env, member );
            break;
    }

    // is the base a class/namespace or a variable
    base_static = isa( member->t_base, env->t_class );
    // actual type
    the_base = base_static ? member->t_base->actual_type : member->t_base;

    // have members?
    if( !the_base->info )
    {
        // base type does not have members
        EM_error2( member->base->linepos,
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
            EM_error2( member->linepos,
                "keyword 'this' must be associated with object instance..." );
            return NULL;
        }
        // in member func
        if( env->func && !env->func->is_member )
        {
            EM_error2( member->linepos,
                "keyword 'this' cannot be used inside static functions..." );
            return NULL;
        }

        return env->class_def;
    }

    // find the value
    value = type_engine_find_value( the_base, member->xid );
    if( !value )
    {
        // can't find member
        EM_error2( member->base->linepos,
            "class '%s' has no member '%s'",
            the_base->c_name(), S_name(member->xid) );
        return NULL;
    }

    // make sure
    if( base_static && value->is_member )
    {
        // this won't work
        EM_error2( member->linepos,
            "cannot access member '%s.%s' without object instance...",
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
        EM_error2( array->linepos,
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
        if( !isa( e->type, env->t_int ) && !isa( e->type, env->t_string ) )
        {
            // not int or string
            EM_error2( e->linepos,
                "array index %i must be of type 'int' or 'string', not '%s'",
                depth, e->type->c_name() );
            return NULL;
        }

        // advance the list
        e = e->next;
    }
    //TODO: Catch commas inside array subscripts earlier on, perhaps in .y
    if( array->indices->depth != depth )
    {
      EM_error2( array->linepos,
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
        // partial
        t = array->base->type->copy( env );
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
    t_CKTYPE the_class = NULL;
    // the parent class
    t_CKTYPE t_parent = NULL;
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;

    // make sure inheritance
    // TODO: sort!
    if( class_def->ext )
    {
        // if extend
        if( class_def->ext->extend_id )
        {
            // find the type
            t_parent = type_engine_find_type( env, class_def->ext->extend_id );
            if( !t_parent )
            {
                EM_error2( class_def->ext->linepos,
                    "undefined super class '%s' in definition of class '%s'",
                    type_path(class_def->ext->extend_id), S_name(class_def->name->xid) );
                return FALSE;
            }

            // must not be primitive
            if( isprim( env, t_parent ) )
            {
                EM_error2( class_def->ext->linepos,
                    "cannot extend primitive type '%s'",
                    t_parent->c_name() );
                EM_error2( class_def->ext->linepos,
                    "...(note: primitives types are 'int', 'float', 'time', and 'dur')" );
                return FALSE;
            }

            // if not complete
            if( t_parent->is_complete == FALSE )
            {
                EM_error2( class_def->ext->linepos,
                    "cannot extend incomplete type '%s'",
                    t_parent->c_name() );
                EM_error2( class_def->ext->linepos,
                    "...(note: the parent's declaration must preceed child's)" );
                return FALSE;
            }
        }

        // TODO: interface
    }

    // by default object
    if( !t_parent ) t_parent = env->t_object;

    // check for fun
    assert( env->context != NULL );
    assert( class_def->type != NULL );
    assert( class_def->type->info != NULL );

    // retrieve the new type (created in scan_class_def)
    the_class = class_def->type;

    // set fields not set in scan
    the_class->parent = t_parent;
    // inherit ugen_info data from parent PLD
    the_class->ugen_info = t_parent->ugen_info;
    // set the beginning of data segment to after the parent
    the_class->info->offset = t_parent->obj_size;
    // duplicate the parent's virtual table
    the_class->info->obj_v_table = t_parent->info->obj_v_table;

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
            env->class_def->has_constructor |= (body->section->stmt_list->stmt != NULL);
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
    if( !ret )
    {
        // delete the class definition
        SAFE_RELEASE( class_def->type );
        // set the thing to NULL
        the_class = NULL;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_check_func_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_func_def( Chuck_Env * env, a_Func_Def f )
{
    Chuck_Value * value = NULL;
    Chuck_Func * func = NULL;

    Chuck_Type * parent = NULL;
    Chuck_Value * override = NULL;
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
        EM_error2( f->linepos,
            "nested function definitions are not (yet) allowed" );
        return FALSE;
    }

    // get the func
    func = f->ck_func;
    // get the value
    value = func->value_ref;
    // make sure
    assert( func != NULL && value != NULL );

    // look up the value in the parent class
    if( env->class_def && 
        ( override = type_engine_find_value( env->class_def->parent, f->name ) ) )
    {
        // see if the target is a function
        if( !isa( override->type, env->t_function ) )
        {
            EM_error2( f->linepos, "function name '%s' conflicts with previously defined value...",
                S_name(f->name) );
            EM_error2( f->linepos, "from super class '%s'...", override->owner_class->c_name() );
            return FALSE;
        }
    }

    // make sure a code segment is in stmt - else we should push scope
    assert( !f->code || f->code->s_type == ae_stmt_code );

    // if override
    if( override )
    {
        // make reference to parent
        // TODO: ref count
        func->up = override;
    }

    // make sure return type is not NULL
    assert( f->ret_type != NULL );

    // only class functions can be pure
    if( !env->class_def && f->static_decl == ae_key_abstract )
    {
        EM_error2( f->linepos, "non-class function cannot be declared as 'pure'..." );
        EM_error2( f->linepos, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // figure out if the function has code associated with it
    if( f->s_type == ae_func_user ) has_code = ( f->code != NULL );
    else has_code = (f->dl_func_ptr != NULL); // imported

    // if interface, then cannot have code
    if( env->class_def && env->class_def->def && env->class_def->def->iface && has_code )
    {
        EM_error2( f->linepos, "interface function signatures cannot contain code..." );
        EM_error2( f->linepos, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // if pure, then cannot have code
    if( f->static_decl == ae_key_abstract && has_code )
    {
        EM_error2( f->linepos, "'pure' function signatures cannot contain code..." );
        EM_error2( f->linepos, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // yeah
    if( f->static_decl != ae_key_abstract && !has_code )
    {
        EM_error2( f->linepos, "function declaration must contain code..." );
        EM_error2( f->linepos, "(unless in interface, or is declared 'pure')" );
        EM_error2( f->linepos, "...at function '%s'", S_name(f->name) );
        goto error;
    }

    // if overriding super class function, then check signatures
    if( env->class_def )
    {
        // get parent
        parent = env->class_def->parent;
        while( parent && !parent_match )
        {
            if(( v = type_engine_find_value( env->class_def->parent, f->name ) ))
            {
                // see if the target is a function
                if( !isa( v->type, env->t_function ) )
                {
                    EM_error2( f->linepos, "function name '%s' conflicts with previously defined value...",
                        S_name(f->name) );
                    EM_error2( f->linepos, "from super class '%s'...", v->owner_class->c_name() );
                    goto error;
                }

                // parent func
                parent_func = v->func_ref;

                // go through all overloading
                while( parent_func && !parent_match )
                {
                    // match the prototypes
                    string err;
                    if( !type_engine_compat_func( f, parent_func->def, f->linepos, err, FALSE ) )
                    {
                        // next
                        parent_func = parent_func->next;
                        // move on
                        continue;
                    }
                    /*{
                        EM_error2( f->linepos,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            value->owner_class->c_name(), S_name(f->name) );
                        if( err != "" ) EM_error2( f->linepos, "...(reason: %s)", err.c_str() );
                        goto error;
                    }*/

                    // see if parent function is static
                    if( parent_func->def->static_decl == ae_key_static )
                    {
                        EM_error2( f->linepos,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name), 
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->linepos,
                            "...(reason: '%s.%s' is declared as 'static')",
                            v->owner_class->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // see if function is static
                    if( f->static_decl == ae_key_static )
                    {
                        EM_error2( f->linepos,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name), 
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->linepos,
                            "...(reason: '%s.%s' is declared as 'static')",
                            env->class_def->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // see if function is pure
                    if( f->static_decl == ae_key_abstract )
                    {
                        EM_error2( f->linepos,
                            "function '%s.%s' resembles '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name), 
                            v->owner_class->c_name(), S_name(f->name) );
                        EM_error2( f->linepos,
                            "...(reason: '%s.%s' is declared as 'pure')",
                            env->class_def->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // make sure returns are equal
                    if( *(f->ret_type) != *(parent_func->def->ret_type) )
                    {
                        EM_error2( f->linepos, "function signatures differ in return type..." );
                        EM_error2( f->linepos,
                            "function '%s.%s' matches '%s.%s' but cannot override...",
                            env->class_def->c_name(), S_name(f->name),
                            v->owner_class->c_name(), S_name(f->name) );
                        goto error;
                    }

                    // match
                    parent_match = TRUE;

                    // update virtual table
                    func->vt_index = parent_func->vt_index;
                    assert( func->vt_index < env->curr->obj_v_table.funcs.size() );
                    env->curr->obj_v_table.funcs[func->vt_index] = func;
                    // update name
                    func_name = parent_func->name;
                    func->name = func_name;
                    value->name = func_name;
                }
            }

            // move to next parent
            parent = parent->parent;        
        }
    }

    // if not override, make entry in virtual table
    if( func->is_member && !parent_match )
    {
        // remember virtual table index
        func->vt_index = env->curr->obj_v_table.funcs.size();
        // append to virtual table
        env->curr->obj_v_table.funcs.push_back( func );
    }

    // set the current function to this
    env->func = func;
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
            EM_error2( arg_list->linepos, "in function '%s':", S_name(f->name) );
            EM_error2( arg_list->linepos, "argument %i '%s' is already defined in this scope",
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
        EM_error2( 0, "...in function '%s'", S_name(f->name) );
        goto error;
    }

    // if imported, add the stack depth
    if( f->s_type == ae_func_builtin )
    {
        // set the stack depth?
        func->code->stack_depth = f->stack_depth;
        // if member add room for this
        // done: if( func->is_member ) func->code->stack_depth += sizeof(t_CKUINT);
    }

    // pop the value stack
    env->curr->value.pop();
    
    // clear the env's function definition
    env->func = NULL;

    return TRUE;

error:

    // clean up
    env->func = NULL;
    // func->release();

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: lookup_type()
// desc: lookup type in the env
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Namespace::lookup_type( const string & name, t_CKINT climb )
{
    Chuck_Type * t = type.lookup( name, climb );
    if( climb > 0 && !t && parent )
        return parent->lookup_type( name, climb );
    return t;
}




//-----------------------------------------------------------------------------
// name: lookup_type()
// desc: lookup type in the env
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Namespace::lookup_type( S_Symbol name, t_CKINT climb )
{
    Chuck_Type * t = type.lookup( name, climb );
    if( climb > 0 && !t && parent )
        return parent->lookup_type( name, climb );
    return t;
}




//-----------------------------------------------------------------------------
// name: lookup_value()
// desc: lookup value in the env
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Namespace::lookup_value( const string & name, t_CKINT climb )
{
    Chuck_Value * v = value.lookup( name, climb );
    if( climb > 0 && !v && parent )
        return parent->lookup_value( name, climb );
    return v;
}




//-----------------------------------------------------------------------------
// name: lookup_value()
// desc: lookup value in the env
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Namespace::lookup_value( S_Symbol name, t_CKINT climb )
{
    Chuck_Value * v = value.lookup( name, climb );
    if( climb > 0 && !v && parent )
        return parent->lookup_value( name, climb );
    return v;
}




//-----------------------------------------------------------------------------
// name: lookup_func()
// desc: lookup func in the env
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Namespace::lookup_func( const string & name, t_CKINT climb )
{
    Chuck_Func * f = func.lookup( name, climb );
    if( climb > 0 && !f && parent )
        return parent->lookup_func( name, climb );
    return f;
}




//-----------------------------------------------------------------------------
// name: lookup_func()
// desc: lookup func in the env
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Namespace::lookup_func( S_Symbol name, t_CKINT climb )
{
    Chuck_Func * f = func.lookup( name, climb );
    if( climb > 0 && !f && parent )
        return parent->lookup_func( name, climb );
    return f;
}




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
void Chuck_Namespace::get_funcs( vector<Chuck_Func *> & out )
{
    // temporary vector
    vector<Chuck_VM_Object *> list;
    // get it from the scope
    this->func.get_level( 0, list );
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
        if( lhs.name != rhs.name ) return FALSE;
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
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL equals( Chuck_Type * lhs, Chuck_Type * rhs ) { return (*lhs) == (*rhs); }




//-----------------------------------------------------------------------------
// name: operator <=
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL operator <=( const Chuck_Type & lhs, const Chuck_Type & rhs )
{
    // check to see if type L == type R
    if( lhs == rhs ) return TRUE;
    
    // if lhs is a child of rhs
    const Chuck_Type * curr = lhs.parent;
    while( curr )
    {
        if( *curr == rhs ) return TRUE;
        curr = curr->parent;
    }

    // if lhs is null and rhs is a object
    if( (lhs == *(lhs.m_env->t_null)) && (rhs <= *(rhs.m_env->t_object)) ) return TRUE;
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: isa()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL isa( Chuck_Type * lhs, Chuck_Type * rhs ) { return (*lhs) <= (*rhs); }




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
        SAFE_DELETE( nspc );

        // delete the types - can't do this since the type system and vm still use
        // for( t_CKINT i = 0; i < new_types.size(); i++ )
        //    new_types[i]->release();

        // clear it
        new_types.clear();
    }

    // TODO: delete abstract syntax tree * 
}




//-----------------------------------------------------------------------------
// name: add_commit_candidate()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Context::add_commit_candidate( Chuck_Namespace * nspc )
{
    // add for commit
    commit_map[nspc] = nspc;
}




//-----------------------------------------------------------------------------
// name: commit()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Context::commit()
{
    std::map<Chuck_Namespace *, Chuck_Namespace *>::iterator iter;

    // loop through
    for( iter = commit_map.begin(); iter != commit_map.end(); iter++ )
    {
        // commit
        (*iter).second->commit();
    }
    
    // clear
    commit_map.clear();
}




//-----------------------------------------------------------------------------
// name: rollback()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Context::rollback()
{
    std::map<Chuck_Namespace *, Chuck_Namespace *>::iterator iter;

    // loop through
    for( iter = commit_map.begin(); iter != commit_map.end(); iter++ )
    {
        // rollback
        (*iter).second->rollback();
    }

    // clear
    commit_map.clear();
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
// name: type_engine_check_primitive()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_primitive( Chuck_Env * env, Chuck_Type * type )
{
    return ( isa(type, env->t_int) || isa(type, env->t_float) || isa(type, env->t_dur) ||
             isa(type, env->t_time) || isa(type, env->t_complex) || isa(type, env->t_polar) ||
             isa(type, env->t_vec3) || isa(type, env->t_vec4) )
             && ( type->array_depth == 0 );
}
t_CKBOOL isprim( Chuck_Env * env, Chuck_Type * type )
{   return type_engine_check_primitive( env, type ); }
t_CKBOOL isobj( Chuck_Env * env, Chuck_Type * type )
{   return !type_engine_check_primitive( env, type ); }
t_CKBOOL isfunc( Chuck_Env * env, Chuck_Type * type )
{   return isa( type, env->t_function ); }
t_CKBOOL iskindofint( Chuck_Env * env, Chuck_Type * type ) // added 1.3.1.0
{   return isa( type, env->t_int ) || isobj( env, type ); }
t_CKUINT getkindof( Chuck_Env * env, Chuck_Type * type ) // added 1.3.1.0
{
    // the kind (1.3.1.0)
    t_CKUINT kind = kindof_VOID;

    // check size
    if( type->size == sz_INT && iskindofint(env, type) )
        kind = kindof_INT;
    else if( type->size == sz_FLOAT )
        kind = kindof_FLOAT;
    else if( type->size == sz_COMPLEX )
        kind = kindof_COMPLEX;
    else if( type->size == sz_VEC3 )
        kind = kindof_VEC3;
    else if( type->size == sz_VEC4 )
        kind = kindof_VEC4;
    
    // done
    return kind;
}




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
const char * type_path( a_Id_List path )
{
    static string str;

    // clear it
    str = "";
    // loop over path
    while( path )
    {
        // concatenate
        str += S_name(path->xid);
        // add .
        if( path->next ) str += ".";
        // advance
        path = path->next;
    }

    return str.c_str();
}




//-----------------------------------------------------------------------------
// name: type_engine_find_type()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_type( Chuck_Namespace * nspc, S_Symbol xid )
{
    Chuck_Type * type = NULL;
    if( !nspc) return NULL;
    // -1 for base
    if(( type = nspc->lookup_type( xid, -1 ) )) return type;
    return NULL;
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
Chuck_Type * type_engine_find_deprecated_type( Chuck_Env * env, a_Id_List path )
{
    // S_Symbol xid = NULL;
    // Chuck_Type * t = NULL;
    std::string actual;

    // find mapping
    if( !type_engine_get_deprecate( env, S_name(path->xid), actual ) )
        return NULL;

    // get base type
    Chuck_Type * type = env->curr->lookup_type( actual, TRUE );
    if( !type ) return NULL;
    else
    {
        // check level
        if( env->deprecate_level < 2 )
        {
            EM_error2( path->linepos, "deprecated: '%s' --> use: '%s'...",
                type_path( path ), actual.c_str() );
        }
    }

    return type;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_type()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_find_type( Chuck_Env * env, a_Id_List path )
{
    S_Symbol xid = NULL;
    Chuck_Type * t = NULL;
    // get base type
    Chuck_Type * type = env->curr->lookup_type( path->xid, TRUE );
    if( !type )
    {
        // check level
        if( env->deprecate_level > 0 )
            type = type_engine_find_deprecated_type( env, path );

        // error
        if( !type )
        {
            EM_error2( path->linepos, "undefined type '%s'...",
                type_path( path ) );
            return NULL;
        }
    }
    // start the namespace
    Chuck_Namespace * nspc = type->info;
    path = path->next;

    // loop
    while( path != NULL )
    {
        // get the id
        xid = path->xid;
        // look for the type in the namespace
        t = type_engine_find_type( nspc, xid );
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
            EM_error2( path->linepos, "undefined type '%s'...",
                type_path( path ) );
            EM_error2( path->linepos,
                "...(cannot find class '%s' in namespace '%s')",
                S_name(xid), nspc->name.c_str() );
            return NULL;
        }

        // set the type
        type = t;
        // update nspc
        if( type ) nspc = type->info;
        // advance
        path = path->next;
    }

    return type;
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
    if(( value = type->info->lookup_value( xid, -1 ) )) return value;
    if( type->parent ) return type_engine_find_value( type->parent, xid );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: type_engine_find_value()
// desc: from env...
//-----------------------------------------------------------------------------
Chuck_Value * type_engine_find_value( Chuck_Env * env, const string & xid,
                                      t_CKBOOL climb, int linepos )
{
    Chuck_Value * value = NULL;
    string actual;

    // look up
    if(( value = env->curr->lookup_value( xid, climb ) )) return value;

    // see if deprecated if climb
    if( climb )
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
                EM_error2( linepos, "deprecated: '%s' --> use: '%s'...",
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
Chuck_Namespace * type_engine_find_nspc( Chuck_Env * env, a_Id_List path )
{
    Chuck_Namespace * nspc = NULL;
    // if the first if global, move to the next
    if( path && !strcmp( S_name(path->xid), "global" ) ) path = path->next;
    // global namespace
    if( path == NULL ) return env->global();
    // find the type
    Chuck_Type * type = type_engine_find_type( env, path );
    if( type == NULL ) return NULL;
    // copy the nspc
    nspc = type->info;
    if( nspc == NULL )
    {
        // primitive
        if( isprim( env, type ) )
        {
            // error
            EM_error2( 0, "primitive type '%s' has no namespace and cannot be extended...",
                type->c_name() );
            return NULL;
        }
        else
        {
            // internal error
            EM_error2( 0, "internal error: type '%s' without namespace...",
                type->c_name() );
            return NULL;
        }
    }

    return nspc;
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
            EM_error2( pos, "(both must be one of public/private/protected/function)..." );
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
            if( print ) EM_error2( pos, "function signatures differ in argument %i's type...", count );
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
            "function signatures differ in number of arguments..." );
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
        EM_error2( 0, "during import: class '%s' already imported...", type->c_name() );
        return NULL;
    }

    // allocate namespace for type
    type->info = new Chuck_Namespace;
    // add reference
    SAFE_ADD_REF(type->info);
    // name it
    type->info->name = type->name;
    // set the parent namespace
    type->info->parent = where;
    // add reference
    SAFE_ADD_REF(type->info->parent);

    // if pre constructor
    if( pre_ctor != 0 )
    {
        // flag it
        type->has_constructor = TRUE;
        // allocate vm code for pre_ctor
        type->info->pre_ctor = new Chuck_VM_Code;
        // add pre_ctor
        type->info->pre_ctor->native_func = (t_CKUINT)pre_ctor;
        // mark type as ctor
        type->info->pre_ctor->native_func_type = Chuck_VM_Code::NATIVE_CTOR;
        // specify that we need this
        type->info->pre_ctor->need_this = TRUE;
        // no arguments to preconstructor other than self
        type->info->pre_ctor->stack_depth = sizeof(t_CKUINT);
    }

    // if destructor
    if( dtor != 0 )
    {
        // flag it
        type->has_destructor = TRUE;
        // allocate vm code for dtor
        type->info->dtor = new Chuck_VM_Code;
        // add dtor
        type->info->dtor->native_func = (t_CKUINT)dtor;
        // mark type as dtor
        type->info->dtor->native_func_type = Chuck_VM_Code::NATIVE_DTOR;
        // specify that we need this
        type->info->dtor->need_this = TRUE;
        // no arguments to destructor other than self
        type->info->dtor->stack_depth = sizeof(t_CKUINT);
    }

    // set the beginning of the data segment after parent
    if( type->parent )
    {
        type->info->offset = type->parent->obj_size;
        // duplicate parent's virtual table
        assert( type->parent->info != NULL );
        type->info->obj_v_table = type->parent->info->obj_v_table;
    }

    // set the owner namespace
    type->owner = where;
    // add reference
    SAFE_ADD_REF(type->owner);
    // check if primitive
    if( !isprim( env, type ) ) // 1.3.5.3 (primitives already have size!)
    {
        // set the size, which is always the width of a pointer
        type->size = sizeof(t_CKUINT);
    }
    // set the object size
    type->obj_size = 0; // TODO

    // flag as complete
    type->is_complete = TRUE;
    // make type
    type_type = env->t_class->copy( env );
    type_type->actual_type = type;
    // SAFE_REF_ASSIGN( type_type->actual_type, type );

    // make value
    value = new Chuck_Value( type_type, type->name );
    value->owner = where;
    // SAFE_REF_ASSIGN( value->owner, where );
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
    type->add_ref();

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
        parent = env->t_object;
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
    EM_error2( 0, "... during import of class '%s'", name );
    // free
    SAFE_DELETE( type );

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
    if( !(type = type_engine_import_class_begin( env, name, parent, where, pre_ctor, dtor, doc ) ) )
        return NULL;

    // make sure parent is ugen
    assert( type->parent != NULL );
    if( !isa( type->parent, env->t_ugen ) )
    {
        // error
        EM_error2( 0,
            "imported class '%s' does not have a ugen as parent",
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
    if( num_ins != 0xffffffff ) info->num_ins = num_ins;
    if( num_outs != 0xffffffff ) info->num_outs = num_outs;
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
                                          0xffffffff, 0xffffffff, doc );
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
    if( !(type = type_engine_import_ugen_begin( env, name, parent, where, pre_ctor, dtor,
                                                tick, pmsg, num_ins, num_outs, doc ) ) )
        return NULL;

    // make sure parent is ugen
    assert( type->parent != NULL );
    if( !isa( type->parent, env->t_uana ) )
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
    if( num_ins_ana != 0xffffffff ) info->num_ins_ana = num_ins_ana;
    if( num_outs_ana != 0xffffffff ) info->num_outs_ana = num_outs_ana;

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
        EM_error2( 0, "import: too many class_end called..." );
        return FALSE;
    }

    // set the object size
    env->class_def->obj_size = env->class_def->info->offset;

    // pop the class
    env->class_def = env->class_stack.back();
    env->class_stack.pop_back();
    // pop the namesapce
    env->curr = env->nspc_stack.back();
    env->nspc_stack.pop_back();
        
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
        EM_error2( 0, 
            "import error: import_mfun '%s' invoked between begin/end",
            mfun->name.c_str() );
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
        EM_error2( 0, 
            "import error: import_sfun '%s' invoked between begin/end",
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
            "import error: import_mvar '%s' invoked between begin/end",
            name );
        return CK_INVALID_OFFSET;
    }

    // make path
    t_CKUINT array_depth = 0;
    a_Id_List path = str2list( type, array_depth );
    if( !path )
    {
        // error
        EM_error2( 0, "... during mvar import '%s.%s'...", 
            env->class_def->c_name(), name );
        return CK_INVALID_OFFSET;
    }
    // make type decl
    a_Type_Decl type_decl = new_type_decl( path, FALSE, 0 );
    // check for array
    if( array_depth )
    {
        // add the array
        type_decl->array = new_array_sub( NULL, 0 );
        // set the depth
        type_decl->array->depth = array_depth;
    }
    // make var decl
    a_Var_Decl var_decl = new_var_decl( (char *)name, NULL, 0 );
    
    // added 2013-10-22 - spencer
    // allow array-type mvars
    // check for array
    if( array_depth )
    {
        // add the array
        var_decl->array = new_array_sub( NULL, 0 );
        // set the depth
        var_decl->array->depth = array_depth;
    }

    // make var decl list
    a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0 );
    // make exp decl
    a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, FALSE, 0 );
    // add it
    if( !type_engine_scan1_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_scan2_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_check_exp_decl( env, &exp_decl->decl ) )
    {
        delete_id_list( path );
        return CK_INVALID_OFFSET;
    }
    
    if( doc != NULL )
        var_decl->value->doc = doc;
    
    // cleanup
    delete_id_list( path );

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
            "import error: import_svar '%s' invoked between begin/end",
            name );
        return FALSE;
    }

    // make path
    a_Id_List path = str2list( type );
    if( !path )
    {
        // error
        EM_error2( 0, "... during svar import '%s.%s'...", 
            env->class_def->c_name(), name );
        return FALSE;
    }

    // make type decl
    a_Type_Decl type_decl = new_type_decl( path, FALSE, 0 );
    // make var decl
    a_Var_Decl var_decl = new_var_decl( (char *)name, NULL, 0 );
    // make var decl list
    a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0 );
    // make exp decl
    a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, TRUE, 0 );
    // add addr
    var_decl->addr = (void *)addr;
    // add it
    if( !type_engine_scan1_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_scan2_exp_decl( env, &exp_decl->decl ) ||
        !type_engine_check_exp_decl( env, &exp_decl->decl ) )
    {
        delete_id_list( path );
        return FALSE;
    }
    
    if( doc != NULL )
        var_decl->value->doc = doc;
    
    // cleanup
    delete_id_list( path );

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
                   "import error: import_add_ex '%s' invoked between begin/end",
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
// name: init_special()
// desc: ...
//-----------------------------------------------------------------------------
void init_special( Chuck_VM_Object * obj )
{
    // reference - this is done when the reference is assigned
    // obj->add_ref();

    // add to vector
    if( obj->m_v_ref ) obj->m_v_ref->push_back( obj );
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Type()
// desc: instantiate new chuck type
//-----------------------------------------------------------------------------
Chuck_Type * Chuck_Context::new_Chuck_Type( Chuck_Env * env )
{
    // allocate
    Chuck_Type * type = new Chuck_Type( env );
    if( !type ) return NULL;
    // set v ref
    type->m_v_ref = &new_types;
    // initialize it
    init_special( type );

    return type;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Value()
// desc: instantiate new chuck value
//-----------------------------------------------------------------------------
Chuck_Value * Chuck_Context::new_Chuck_Value( Chuck_Type * t,
                                              const string & name )
{
    // allocate
    Chuck_Value * value = new Chuck_Value( t, name );
    if( !value ) return NULL;
    // set v ref
    value->m_v_ref = &new_values;
    // initialize it
    init_special( value );

    return value;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Func()
// desc: instantiate new chuck func
//-----------------------------------------------------------------------------
Chuck_Func * Chuck_Context::new_Chuck_Func()
{
    // allocate
    Chuck_Func * func = new Chuck_Func;
    if( !func ) return NULL;
    // set v ref
    func->m_v_ref = &new_funcs;
    // initialize it
    init_special( func );

    return func;
}




//-----------------------------------------------------------------------------
// name: new_Chuck_Namespace()
// desc: instantiate new chuck namespace
//-----------------------------------------------------------------------------
Chuck_Namespace * Chuck_Context::new_Chuck_Namespace()
{
    // allocate
    Chuck_Namespace * nspc = new Chuck_Namespace;
    if( !nspc ) return NULL;
    // set v ref
    nspc->m_v_ref = &new_nspc;
    // initialize it
    init_special( nspc );

    return nspc;
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
    // set the id
    t->xid = te_array;
    // set the name
    t->name = base_type->name;
    // set the parent
    t->parent = array_parent;
    // add reference
    SAFE_ADD_REF(t->parent);
    // is a ref
    t->size = array_parent->size;
    // set the array depth
    t->array_depth = depth;
    // set the base type
    t->array_type = base_type;
    // add reference
    SAFE_ADD_REF(t->array_type);
    // set the namesapce
    t->info = array_parent->info;
    // add reference
    SAFE_ADD_REF(t->info);
    // set owner
    t->owner = owner_nspc;
    // add reference
    SAFE_ADD_REF(t->owner);

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
            EM_error2( array->linepos,
                "invalid format for array init [...][...]..." );
            return FALSE;
        }
        else if( array->err_num == 2 )
        {
            EM_error2( array->linepos,
                "partially empty array init [...][]..." );
            return FALSE;
        }
        else
        {
            EM_error2( array->linepos,
                "internal error: unrecognized array error..." );
            return FALSE;
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: str2list()
// desc: convert str to list
//-----------------------------------------------------------------------------
a_Id_List str2list( const string & path )
{
    t_CKUINT dummy;
    return str2list( path, dummy );
}




//-----------------------------------------------------------------------------
// name: str2list()
// desc: convert str to list
//-----------------------------------------------------------------------------
a_Id_List str2list( const string & path, t_CKUINT & array_depth )
{
    t_CKINT len = path.length();
    t_CKINT i, j;
    string curr;
    a_Id_List list = NULL;
    char last = '\0';

    // look for []
    array_depth = 0;
    while( len > 2 && path[len-1] == ']' && path[len-2] == '[' )
    {
        // flag it
        array_depth++;
        // shorten len
        len -= 2;
    }

    // loop backwards
    for( i = len - 1; i >= 0; i-- )
    {
        char c = path[i];
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
                EM_error2( 0, "illegal character '%c' in path '%s'...",
                    c, path.c_str() );
                // delete
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
                list = prepend_id_list( (char *)curr.c_str(), list, 0 );
                // clear
                curr = "";
            }
            else
            {
                // error
                EM_error2( 0, "path '%s' must not begin or end with '.'",
                    path.c_str() );
                // delete
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
            EM_error2( 0, "...at argument '%i'...", i+1 );
            // delete the arg list
            // delete_arg_list( arg_list );
            return NULL;
        }

        // type
        type_decl = new_type_decl( type_path, FALSE, 0 );
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
            array_sub = new_array_sub( NULL, 0 );
        
            for( int i = 1; i < array_depth; i++ )
                array_sub = prepend_array_sub( array_sub, NULL, 0 );
        }

        var_decl = new_var_decl( (char *)arg->name.c_str(), array_sub, 0 );

        // make new arg
        arg_list = prepend_arg_list( type_decl, var_decl, arg_list, 0 );
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
        EM_error2( 0, "...during function import '%s' (type)...", 
            dl_fun->name.c_str() );
        goto error;
    }

    // type decl
    // old: type_decl = new_type_decl( type_path, 1, 0 );
    type_decl = new_type_decl( type_path, 0, 0 );
    assert( type_decl );
    if( !type_decl )
    {
        // error
        EM_error2( 0, "...during function import '%s' (type2)...",
            dl_fun->name.c_str() );
        // delete list
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
        a_Array_Sub array_sub = new_array_sub( NULL, 0 );
        
        for( int i = 1; i < array_depth; i++ )
            array_sub = prepend_array_sub( array_sub, NULL, 0 );
        
        type_decl = add_type_decl_array( type_decl, array_sub, 0 );
    }

    // name of the function
    name = dl_fun->name.c_str();
    // arg list
    arg_list = make_dll_arg_list( dl_fun );
    if( dl_fun->args.size() > 0 && !arg_list )
    {
        // error
        EM_error2( 0, "...during function import '%s' (arg_list)...",
            dl_fun->name.c_str() );
        // delete type_decl
        // delete_type_decl( type_decl );
        type_decl = NULL;
        goto error;
    }

    // make a func_def
    func_def = new_func_def( func_decl, static_decl, type_decl,
                             (char *)name, arg_list, NULL, 0 );
    // mark the function as imported (instead of defined in ChucK)
    func_def->s_type = ae_func_builtin;
    // copy the function pointer - the type doesn't matter here
    // ... since we copying into a void * - so mfun is used
    func_def->dl_func_ptr = (void *)dl_fun->mfun;

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
    a_Id_List path = str2list( dest );
    if( !path ) goto error;

    // find the namespace to put the import
    nspc = type_engine_find_nspc( env, path );
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
        // ignored for now... -spencer
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
            a_Id_List path = str2list( cl->svars[j]->type.c_str() );
            // make type decl
            a_Type_Decl type_decl = new_type_decl( path, FALSE, 0 );
            // make var decl
            a_Var_Decl var_decl = new_var_decl( cl->svars[j]->name.c_str(), NULL, 0 );
            // make var decl list
            a_Var_Decl_List var_decl_list = new_var_decl_list( var_decl, 0 );
            // make exp decl
            a_Exp exp_decl = new_exp_decl( type_decl, var_decl_list, TRUE, 0 );
            // add addr
            var_decl->addr = (void *)cl->svars[j]->static_addr;
            // prepend exp stmt to stmt list
            svar_decls = prepend_stmt_list( new_stmt_from_expression( exp_decl, 0 ), svar_decls, 0 );
        }

        // if there are any declarations, prepend them to body
        if( svar_decls )
            body = prepend_class_body( new_section_stmt( svar_decls, 0 ), body, 0 );
        
        // go through funs backwards, and prepend
        for( t_CKINT k = (t_CKINT)the_funs.size() - 1; k >= 0; k-- )
        {
            // add to body
            body = prepend_class_body( new_section_func_def( the_funs[k], 0 ), body, 0 );
        }
        
        // construct class
        def = new_class_def( ae_key_public, name, ext, body, 0 );
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

    // free the path
    delete_id_list( path );

    return TRUE;

error:
    // error
    EM_error2( 0, "...(in object import '%s' in DLL '%s')", 
        query ? ( query->dll_name == "" ? query->dll_name.c_str() : "[empty]" ) : "[null]", dll->name() );

    // free the path
    delete_id_list( path );
    
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
            EM_log(CK_LOG_SEVERE,
                   "error importing class '%s' from dynamic library (%s)",
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
    if(c->ctors.size() > 0)
        ctor = c->ctors[0]; // TODO: uh, is more than one possible?
    
    if((c->ugen_tick || c->ugen_tickf) && c->ugen_num_out)
    {
        // begin import as ugen
        if(!type_engine_import_ugen_begin(env, c->name.c_str(),
                                          c->parent.c_str(), env->global(),
                                          ctor ? (f_ctor) ctor->addr : NULL,
                                          dtor ? (f_dtor) dtor->addr : NULL,
                                          c->ugen_tick, c->ugen_tickf, c->ugen_pmsg,
                                          c->ugen_num_in, c->ugen_num_out,
                                          c->doc.length() > 0 ? c->doc.c_str() : NULL ))
            goto error;
    }
    else
    {
        // begin import as normal class (non-ugen)
        if(!type_engine_import_class_begin(env, c->name.c_str(),
                                           c->parent.c_str(), env->global(),
                                           ctor ? (f_ctor) ctor->addr : NULL,
                                           dtor ? (f_dtor) dtor->addr : NULL,
                                           c->doc.length() > 0 ? c->doc.c_str() : NULL))
            goto error;
    }
    
    int j;
    
    // import member variables
    for(j = 0; j < c->mvars.size(); j++)
    {
        Chuck_DL_Value * mvar = c->mvars[j];
        if(type_engine_import_mvar(env, mvar->type.c_str(),
                                   mvar->name.c_str(), mvar->is_const,
                                   mvar->doc.size() ? mvar->doc.c_str() : NULL)
           == CK_INVALID_OFFSET)
            goto error;
    }
    
    // import static variables
    for(j = 0; j < c->svars.size(); j++)
    {
        Chuck_DL_Value * svar = c->svars[j];
        if(!type_engine_import_svar(env, svar->type.c_str(), svar->name.c_str(),
                                    svar->is_const, (t_CKUINT) svar->static_addr,
                                    svar->doc.size() ? svar->doc.c_str() : NULL))
            goto error;
    }
    
    // import member functions
    for(j = 0; j < c->mfuns.size(); j++)
    {
        Chuck_DL_Func * func = c->mfuns[j];
        if(!type_engine_import_mfun(env, func)) goto error;
    }
    
    // import static functions
    for(j = 0; j < c->sfuns.size(); j++)
    {
        Chuck_DL_Func * func = c->sfuns[j];
        if(!type_engine_import_sfun(env, func)) goto error;
    }
    
    // import examples (if any)
    for(j = 0; j < c->examples.size(); j++)
    {
        if(!type_engine_import_add_ex(env, c->examples[j].c_str())) goto error;
    }
    
    // end class import
    type_engine_import_class_end(env);
    
    return TRUE;
    
error:
    type_engine_import_class_end(env);
    
    return FALSE;
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
    g_escape['\''] = '\'';
    g_escape['"'] = '"';
    g_escape['\\'] = '\\';
    g_escape['a'] = (char)7; // audible bell
    g_escape['b'] = (char)8; // back space
    g_escape['f'] = (char)12; // form feed
    g_escape['n'] = (char)10; // new line
    g_escape['r'] = (char)13; // carriage return
    g_escape['t'] = (char)9; // horizontal tab
    g_escape['v'] = (char)11; // vertical tab

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
                EM_error2( linepos, "invalid: string ends with escape charactor '\\'" );
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
