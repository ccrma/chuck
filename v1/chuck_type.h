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
// file: chuck_type.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_TYPE_H__
#define __CHUCK_TYPE_H__

#include "chuck_absyn.h"




//-----------------------------------------------------------------------------
// type engine enum
//-----------------------------------------------------------------------------
typedef enum {
    // general types
    te_int, te_uint, te_single, te_float, te_double, te_string, te_thread, 
    te_shred, te_time, te_dur, te_array, te_function, te_ugen, te_pattern,
    te_transport, te_host, te_class, te_unit, te_object, te_null, te_code,
    te_tuple, te_midiin, te_midiout, te_adc, te_dac, te_bunghole,

    // system types (internal - cannot instantiate)
    te_void, __te_system_out__, __te_system_err__, __te_system_time__,
    __te_system_namespace__, __te_system_class__, __te_system_user__
} te_Type;




//-----------------------------------------------------------------------------
// type engine struct
//-----------------------------------------------------------------------------

// type struct
struct t_Type_
{ 
    // type id
    te_Type type;
    // type name
    c_str name;
    // class info
    t_Type parent;
    // size
    unsigned int size;
    // array depth
    unsigned int array_depth;
    // dynamic
    unsigned int dyn;
};




//-----------------------------------------------------------------------------
// type engine api
//-----------------------------------------------------------------------------
typedef struct t_Env_ * t_Env;
typedef struct t_Namespace_Info_ * t_Namespace_Info;
typedef struct t_Namespace_Value_ * t_Namespace_Value;
typedef struct t_Class_Info_ * t_Class_Info;

// advanced reference
struct Chuck_DLL;
struct Chuck_UGen_Info;
class  Chuck_VM;

// api
t_Env type_engine_init( Chuck_VM * vm );
t_CKBOOL type_engine_shutdown( t_Env env );
void type_engine_begin( t_Env env );
void type_engine_end( t_Env env );
t_CKBOOL type_engine_check_prog( t_Env env, a_Program prog );
t_Type type_engine_check_exp( t_Env env, a_Exp exp );
t_CKBOOL type_engine_check_stmt( t_Env env, a_Stmt stmt );
t_CKBOOL type_engine_add_dll( t_Env env, Chuck_DLL * dll, const char * nspc );
t_Namespace_Info type_engine_get_namespace( t_Env env, const char * nspc );

t_Type lookup_type( t_Env env, S_Symbol type_name, t_CKBOOL climb = TRUE );
t_Type lookup_value( t_Env env, S_Symbol type_name, t_CKBOOL climb = TRUE );
a_Func_Def lookup_func( t_Env env, S_Symbol func_name, t_CKBOOL climb = TRUE );
Chuck_UGen_Info * lookup_ugen( t_Env env, S_Symbol ugen_name, t_CKBOOL climb = TRUE );
t_Env lookup_namespace( t_Env env, S_Symbol nspc_name, t_CKBOOL climb = TRUE );
void * lookup_addr( t_Env env, S_Symbol value_name, t_CKBOOL climb = TRUE );
t_CKBOOL isa( t_Type a, t_Type b );




#endif
