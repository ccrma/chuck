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
// name: chuck_dl.h
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
#ifndef __CHUCK_DL_H__
#define __CHUCK_DL_H__

#include "chuck_def.h"
#include "chuck_absyn.h"
#include "chuck_carrier.h"
#include "chuck_oo.h"
#include <string>
#include <vector>




// major version must be the same between chuck:chugin
#define CK_DLL_VERSION_MAJOR (0x0009)
// minor version of chugin must be less than or equal to chuck's
#define CK_DLL_VERSION_MINOR (0x0000)
#define CK_DLL_VERSION_MAKE(maj,min) ((t_CKUINT)(((maj) << 16) | (min)))
#define CK_DLL_VERSION_GETMAJOR(v) (((v) >> 16) & 0xFFFF)
#define CK_DLL_VERSION_GETMINOR(v) ((v) & 0xFFFF)
#define CK_DLL_VERSION (CK_DLL_VERSION_MAKE(CK_DLL_VERSION_MAJOR, CK_DLL_VERSION_MINOR))




extern char g_default_chugin_path[];
extern char g_chugin_path_envvar[];



// DL forward references
struct Chuck_DL_Query;
struct Chuck_DL_Class;
struct Chuck_DL_Func;
struct Chuck_DL_Value;
struct Chuck_DL_Ctrl;
union  Chuck_DL_Return;
struct Chuck_DL_MainThreadHook;
struct Chuck_DLL;
struct Chuck_DL_Api;
// un-namespaced Chuck_DL_Api | 1.5.1.5 (ge)
// namespace Chuck_DL_Api { struct Api; }

// object forward references
struct Chuck_UGen;
struct Chuck_UAna;
struct Chuck_UAnaBlobProxy;


// param conversion - to extract values from ARGS to functions
#define GET_CK_FLOAT(ptr)      (*(t_CKFLOAT *)ptr)
#define GET_CK_SINGLE(ptr)     (*(float *)ptr)
#define GET_CK_DOUBLE(ptr)     (*(double *)ptr)
#define GET_CK_INT(ptr)        (*(t_CKINT *)ptr)
#define GET_CK_UINT(ptr)       (*(t_CKUINT *)ptr)
#define GET_CK_TIME(ptr)       (*(t_CKTIME *)ptr)
#define GET_CK_DUR(ptr)        (*(t_CKDUR *)ptr)
#define GET_CK_COMPLEX(ptr)    (*(t_CKCOMPLEX *)ptr)
#define GET_CK_POLAR(ptr)      (*(t_CKPOLAR *)ptr)
#define GET_CK_VEC2(ptr)       (*(t_CKVEC2 *)ptr)
#define GET_CK_VEC3(ptr)       (*(t_CKVEC3 *)ptr)
#define GET_CK_VEC4(ptr)       (*(t_CKVEC4 *)ptr)
#define GET_CK_VECTOR(ptr)     (*(t_CKVECTOR *)ptr)
#define GET_CK_OBJECT(ptr)     (*(Chuck_Object **)ptr)
#define GET_CK_STRING(ptr)     (*(Chuck_String **)ptr)
#define GET_CK_STRING_SAFE(ptr) std::string( GET_CK_STRING(ptr)->c_str() )

// param conversion with pointer advance
#define GET_NEXT_FLOAT(ptr)    (*((t_CKFLOAT *&)ptr)++)
#define GET_NEXT_SINGLE(ptr)   (*((float *&)ptr)++)
#define GET_NEXT_DOUBLE(ptr)   (*((double *&)ptr)++)
#define GET_NEXT_INT(ptr)      (*((t_CKINT *&)ptr)++)
#define GET_NEXT_UINT(ptr)     (*((t_CKUINT *&)ptr)++)
#define GET_NEXT_TIME(ptr)     (*((t_CKTIME *&)ptr)++)
#define GET_NEXT_DUR(ptr)      (*((t_CKDUR *&)ptr)++)
#define GET_NEXT_COMPLEX(ptr)  (*((t_CKCOMPLEX *&)ptr)++)
#define GET_NEXT_POLAR(ptr)    (*((t_CKPOLAR *&)ptr)++)
#define GET_NEXT_VEC2(ptr)     (*((t_CKVEC2 *&)ptr)++)
#define GET_NEXT_VEC3(ptr)     (*((t_CKVEC3 *&)ptr)++)
#define GET_NEXT_VEC4(ptr)     (*((t_CKVEC4 *&)ptr)++)
#define GET_NEXT_VECTOR(ptr)   (*((t_CKVECTOR *&)ptr)++)
#define GET_NEXT_OBJECT(ptr)   (*((Chuck_Object **&)ptr)++)
#define GET_NEXT_STRING(ptr)   (*((Chuck_String **&)ptr)++)
#define GET_NEXT_STRING_SAFE(ptr) std::string( GET_NEXT_STRING(ptr)->c_str() )

// param conversion
#define SET_CK_FLOAT(ptr,v)      (*(t_CKFLOAT *&)ptr=v)
#define SET_CK_SINGLE(ptr,v)     (*(float *&)ptr=v)
#define SET_CK_DOUBLE(ptr,v)     (*(double *&)ptr=v)
#define SET_CK_INT(ptr,v)        (*(t_CKINT *&)ptr=v)
#define SET_CK_UINT(ptr,v)       (*(t_CKUINT *&)ptr=v)
#define SET_CK_TIME(ptr,v)       (*(t_CKTIME *&)ptr=v)
#define SET_CK_DUR(ptr,v)        (*(t_CKDUR *&)ptr=v)
#define SET_CK_VEC2(ptr,v)       (*(t_CKVEC2 *&)ptr=v)
#define SET_CK_VEC3(ptr,v)       (*(t_CKVEC3 *&)ptr=v)
#define SET_CK_VEC4(ptr,v)       (*(t_CKVEC4 *&)ptr=v)
#define SET_CK_VECTOR(ptr,v)     (*(t_CKVECTOR *&)ptr=v)
#define SET_CK_STRING(ptr,v)     (*(Chuck_String **&)ptr=v)

// param conversion with pointer advance
#define SET_NEXT_FLOAT(ptr,v)    (*((t_CKFLOAT *&)ptr)++=v)
#define SET_NEXT_SINGLE(ptr,v)   (*((float *&)ptr)++=v)
#define SET_NEXT_DOUBLE(ptr,v)   (*((double *&)ptr)++=v)
#define SET_NEXT_INT(ptr,v)      (*((t_CKINT *&)ptr)++=v)
#define SET_NEXT_UINT(ptr,v)     (*((t_CKUINT *&)ptr)++=v)
#define SET_NEXT_TIME(ptr,v)     (*((t_CKTIME *&)ptr)++=v)
#define SET_NEXT_DUR(ptr,v)      (*((t_CKDUR *&)ptr)++=v)
#define SET_NEXT_VEC2(ptr,v)     (*((t_CKVEC2 *&)ptr)++=v)
#define SET_NEXT_VEC3(ptr,v)     (*((t_CKVEC3 *&)ptr)++=v)
#define SET_NEXT_VEC4(ptr,v)     (*((t_CKVEC4 *&)ptr)++=v)
#define SET_NEXT_VECTOR(ptr,v)   (*((t_CKVECTOR *&)ptr)++=v)
#define SET_NEXT_STRING(ptr,v)   (*((Chuck_String **&)ptr)++=v)

// param conversion - to extract values from object's data segment
#define OBJ_MEMBER_DATA(obj,offset)     (obj->data + offset)
#define OBJ_MEMBER_FLOAT(obj,offset)    (*(t_CKFLOAT *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_SINGLE(obj,offset)   (*(float *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_DOUBLE(obj,offset)   (*(double *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_INT(obj,offset)      (*(t_CKINT *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_UINT(obj,offset)     (*(t_CKUINT *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_TIME(obj,offset)     (*(t_CKTIME *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_DUR(obj,offset)      (*(t_CKDUR *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_VEC2(obj,offset)     (*(t_CKVEC2 *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_VEC3(obj,offset)     (*(t_CKVEC3 *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_VEC4(obj,offset)     (*(t_CKVEC4 *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_VECTOR(obj,offset)   (*(t_CKVECTOR *)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_OBJECT(obj,offset)   (*(Chuck_Object **)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_STRING(obj,offset)   (*(Chuck_String **)OBJ_MEMBER_DATA(obj,offset))


// chuck dll export linkage and calling convention
#if defined (__PLATFORM_WINDOWS__)
  #define CK_DLL_LINKAGE extern "C" __declspec( dllexport )
#else
  #define CK_DLL_LINKAGE extern "C"
#endif

// calling convention of functions provided by chuck to the dll
#if defined(__PLATFORM_WINDOWS__)
  #define CK_DLL_CALL    _cdecl
#else
  #define CK_DLL_CALL
#endif

typedef const Chuck_DL_Api * CK_DL_API;




//------------------------------------------------------------------------------
// name: union Chuck_DL_Return
// desc: dynamic link return function return struct
//------------------------------------------------------------------------------
union Chuck_DL_Return
{
    t_CKINT v_int;
    t_CKUINT v_uint;
    t_CKFLOAT v_float;
    t_CKDUR v_dur;
    t_CKTIME v_time;
    t_CKCOMPLEX v_complex;
    t_CKPOLAR v_polar;
    t_CKVEC2 v_vec2; // ge: added 1.5.1.7
    t_CKVEC3 v_vec3; // ge: added 1.3.5.3
    t_CKVEC4 v_vec4; // ge: added 1.3.5.3
    Chuck_Object * v_object;
    Chuck_String * v_string;

    Chuck_DL_Return() { v_vec4.x = v_vec4.y = v_vec4.z = v_vec4.w = 0; }
};




//------------------------------------------------------------------------------
// name: struct Chuck_DL_Arg
// desc: import / dynamic link function argument | 1.5.1.5
//------------------------------------------------------------------------------
struct Chuck_DL_Arg
{
    // which kind of data (e.g., int and object * are both kinds of ints)
    te_KindOf kind;
    // the data in a union; re-using DL_Return for this
    Chuck_DL_Return value;

    // constructor
    Chuck_DL_Arg() { kind = kindof_VOID; }
    // size in bytes
    t_CKUINT sizeInBytes()
    {
        // check data kind
        switch( kind )
        {
            case kindof_INT: return sz_INT;
            case kindof_FLOAT: return sz_FLOAT;
            case kindof_VEC2: return sz_VEC2;
            case kindof_VEC3: return sz_VEC3;
            case kindof_VEC4: return sz_VEC4;
            case kindof_VOID: return sz_VOID;
        }
        // unhandled
        return 0;
    }
};




// macro for defining ChucK DLL export functions
// example: CK_DLL_EXPORT(int) foo() { return 1; }
#define CK_DLL_EXPORT(type) CK_DLL_LINKAGE type CK_DLL_CALL
// macro for declaring version of ChucK DL a given DLL links to
// example: CK_DLL_DECLVERSION
#define CK_DLL_DECLVERSION CK_DLL_EXPORT(t_CKUINT) ck_version() { return CK_DLL_VERSION; }
// naming convention for static query functions
#define CK_DLL_QUERY_STATIC_NAME(name) ck_##name##_query
// macro for defining ChucK DLL export query-functions (static version)
#define CK_DLL_QUERY_STATIC(name) CK_DLL_EXPORT(t_CKBOOL) CK_DLL_QUERY_STATIC_NAME(name)( Chuck_DL_Query * QUERY )
// macro for defining ChucK DLL export query-functions
// example: CK_DLL_QUERY
#ifndef __CK_DLL_STATIC__
#define CK_DLL_QUERY(name) CK_DLL_DECLVERSION CK_DLL_EXPORT(t_CKBOOL) ck_query( Chuck_DL_Query * QUERY )
#else
#define CK_DLL_QUERY(name) CK_DLL_QUERY_STATIC(name)
#endif // __CK_DLL_STATIC__
// macro for defining ChucK DLL export allocator
// example: CK_DLL_ALLOC(foo)
#define CK_DLL_ALLOC(name) CK_DLL_EXPORT(Chuck_Object *) name( Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export constructors
// example: CK_DLL_CTOR(foo)
#define CK_DLL_CTOR(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export destructors
// example: CK_DLL_DTOR(foo)
#define CK_DLL_DTOR(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export member functions
// example: CK_DLL_MFUN(foo)
#define CK_DLL_MFUN(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export static functions
// example: CK_DLL_SFUN(foo) | 1.4.1.0 (ge) added TYPE to static prototype
#define CK_DLL_SFUN(name) CK_DLL_EXPORT(void) name( Chuck_Type * TYPE, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// example: CK_DLL_GFUN(foo) | 1.5.1.5 (ge & andrew) added for global-scope function, e.g., for op overloads
#define CK_DLL_GFUN(name) CK_DLL_EXPORT(void) name( void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen tick functions
// example: CK_DLL_TICK(foo)
#define CK_DLL_TICK(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, SAMPLE in, SAMPLE * out, CK_DL_API API )
// macro for defining ChucK DLL export ugen multi-channel tick functions
// example: CK_DLL_TICKF(foo)
#define CK_DLL_TICKF(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, SAMPLE * in, SAMPLE * out, t_CKUINT nframes, CK_DL_API API )
// macro for defining ChucK DLL export ugen ctrl functions
// example: CK_DLL_CTRL(foo)
#define CK_DLL_CTRL(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen cget functions
// example: CK_DLL_CGET(foo)
#define CK_DLL_CGET(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen pmsg functions
// example: CK_DLL_PMSG(foo)
#define CK_DLL_PMSG(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, const char * MSG, void * ARGS, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export uana tock functions
// example: CK_DLL_TOCK(foo)
#define CK_DLL_TOCK(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, Chuck_UAna * UANA, Chuck_UAnaBlobProxy * BLOB, CK_DL_API API )
// macro for defining Chuck DLL export shreds watcher functions
// example: CK_DLL_SHREDS_WATCHER(foo)
#define CK_DLL_SHREDS_WATCHER(name) CK_DLL_EXPORT(void) name( Chuck_VM_Shred * SHRED, t_CKINT CODE, t_CKINT PARAM, Chuck_VM * VM, void * BINDLE )
// macro for defining Chuck DLL export type on instantiate functions
// example: CK_DLL_TYPE_ON_INSTANTIATE(foo)
#define CK_DLL_TYPE_ON_INSTANTIATE(name) CK_DLL_EXPORT(void) name( Chuck_Object * OBJECT, Chuck_Type * TYPE, Chuck_VM_Shred * SHRED, Chuck_VM * VM )

// macros for DLL exports
// example: DLL_QUERY  query( Chuck_DL_Query * QUERY )
// example: DLL_FUNC   foo( void * ARGS, Chuck_DL_Return * RETURN )
// example: DLL_UGEN_F foo_tick( Chuck_UGen * SELF, SAMPLE, SAMPLE * out );
#define DLL_QUERY   CK_DLL_EXPORT(t_CKBOOL)
#define DLL_FUNC    CK_DLL_EXPORT(void)
#define UGEN_CTOR   CK_DLL_EXPORT(void *)
#define UGEN_DTOR   CK_DLL_EXPORT(void)
#define UGEN_TICK   CK_DLL_EXPORT(t_CKBOOL)
#define UGEN_PMSG   CK_DLL_EXPORT(t_CKBOOL)
#define UGEN_CTRL   CK_DLL_EXPORT(t_CKVOID)
#define UGEN_CGET   CK_DLL_EXPORT(t_CKVOID)
#define UANA_TOCK   CK_DLL_EXPORT(t_CKBOOL)


//-----------------------------------------------------------------------------
// dynamic linking class interface prototypes
//-----------------------------------------------------------------------------
extern "C" {
// query
typedef t_CKUINT (CK_DLL_CALL * f_ck_declversion)();
typedef t_CKBOOL (CK_DLL_CALL * f_ck_query)( Chuck_DL_Query * QUERY );
// object
typedef Chuck_Object * (CK_DLL_CALL * f_alloc)( Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_ctor)( Chuck_Object * SELF, void * ARGS, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_dtor)( Chuck_Object * SELF, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_mfun)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
// 1.4.1.0 (ge) added TYPE to static prototype
typedef t_CKVOID (CK_DLL_CALL * f_sfun)( Chuck_Type * TYPE, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
// 1.5.1.5 (ge & andrew) added for global-scope function, e.g., for op overloads
typedef t_CKVOID (CK_DLL_CALL * f_gfun)( void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
// ugen specific
typedef t_CKBOOL (CK_DLL_CALL * f_tick)( Chuck_Object * SELF, SAMPLE in, SAMPLE * out, CK_DL_API API );
typedef t_CKBOOL (CK_DLL_CALL * f_tickf)( Chuck_Object * SELF, SAMPLE * in, SAMPLE * out, t_CKUINT nframes, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_ctrl)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_cget)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKBOOL (CK_DLL_CALL * f_pmsg)( Chuck_Object * SELF, const char * MSG, void * ARGS, Chuck_VM * VM, Chuck_VM_Shred * SHRED, CK_DL_API API );
// uana specific
typedef t_CKBOOL (CK_DLL_CALL * f_tock)( Chuck_Object * SELF, Chuck_UAna * UANA, Chuck_UAnaBlobProxy * BLOB, CK_DL_API API );
// "main thread" hook
typedef t_CKBOOL (CK_DLL_CALL * f_mainthreadhook)( void * bindle );
// "main thread" quit (stop running hook)
typedef t_CKBOOL (CK_DLL_CALL * f_mainthreadquit)( void * bindle );
// shreds watcher callback
typedef void (CK_DLL_CALL * f_shreds_watcher)( Chuck_VM_Shred * SHRED, t_CKINT CODE, t_CKINT PARAM, Chuck_VM * VM, void * BINDLE );
// type instantiation callback
typedef void (CK_DLL_CALL * f_callback_on_instantiate)( Chuck_Object * OBJECT, Chuck_Type * TYPE, Chuck_VM_Shred * originShred, Chuck_VM * VM );
}


// default name in DLL/ckx to look for
#define CK_QUERY_FUNC        "ck_query"
// default name in DLL/ckx to look for
#define CK_DECLVERSION_FUNC  "ck_version"
// bad object data offset
#define CK_INVALID_OFFSET    0xffffffff


//-----------------------------------------------------------------------------
// chuck DLL query functions, implemented on chuck side for portability
//-----------------------------------------------------------------------------
extern "C" {
// set name of ckx
typedef void ( CK_DLL_CALL * f_setname)( Chuck_DL_Query * query, const char * name );

// begin class/namespace, can be nested
typedef void (CK_DLL_CALL * f_begin_class)( Chuck_DL_Query * query, const char * name, const char * parent );
// add constructor, must be betwen begin/end_class : use f_add_arg to add arguments immediately after
typedef void (CK_DLL_CALL * f_add_ctor)( Chuck_DL_Query * query, f_ctor ctor );
// add destructor - cannot have args
typedef void (CK_DLL_CALL * f_add_dtor)( Chuck_DL_Query * query, f_dtor dtor );
// add member function - args to follow
typedef void (CK_DLL_CALL * f_add_mfun)( Chuck_DL_Query * query, f_mfun mfun, const char * type, const char * name );
// add static function - args to follow
typedef void (CK_DLL_CALL * f_add_sfun)( Chuck_DL_Query * query, f_sfun sfun, const char * type, const char * name );
// add binary operator overload - args included
typedef void (CK_DLL_CALL * f_add_op_overload_binary)( Chuck_DL_Query * query, f_gfun gfun, const char * type, const char * op,
                                                       const char * lhsType, const char * lhsName, const char * rhsType, const char * rhsName );
// add unary (prefix) operator overload - arg included
typedef void (CK_DLL_CALL * f_add_op_overload_prefix)( Chuck_DL_Query * query, f_gfun gfun, const char * type, const char * op,
                                                       const char * argType, const char * argName );
// add unary (postfix) operator overload - args included
typedef void (CK_DLL_CALL * f_add_op_overload_postfix)( Chuck_DL_Query * query, f_gfun gfun, const char * type, const char * op,
                                                       const char * argType, const char * argName );
// add member variable
typedef t_CKUINT (CK_DLL_CALL * f_add_mvar)( Chuck_DL_Query * query,
             const char * type, const char * name, t_CKBOOL is_const ); // TODO: public/protected/private
// add static variable
typedef void (CK_DLL_CALL * f_add_svar)( Chuck_DL_Query * query,
             const char * type, const char * name,
             t_CKBOOL is_const, void * static_addr ); // TODO: public/protected/private
// add arg - follows ctor mfun sfun
typedef void (CK_DLL_CALL * f_add_arg)( Chuck_DL_Query * query, const char * type, const char * name );
// ** functions for adding unit generators, must extend ugen
typedef void (CK_DLL_CALL * f_add_ugen_func)( Chuck_DL_Query * query, f_tick tick, f_pmsg pmsg, t_CKUINT num_in, t_CKUINT num_out );
typedef void (CK_DLL_CALL * f_add_ugen_funcf)( Chuck_DL_Query * query, f_tickf tickf, f_pmsg pmsg, t_CKUINT num_in, t_CKUINT num_out );
typedef void (CK_DLL_CALL * f_add_ugen_funcf_auto_num_channels)( Chuck_DL_Query * query, f_tickf tickf, f_pmsg psmg );
// ** add a ugen control (not used) | 1.4.1.0 removed
//typedef void (CK_DLL_CALL * f_add_ugen_ctrl)( Chuck_DL_Query * query, f_ctrl ctrl, f_cget cget,
//                                              const char * type, const char * name );
// end class/namespace - must correspondent with begin_class.  returns false on error
typedef t_CKBOOL (CK_DLL_CALL * f_end_class)( Chuck_DL_Query * query );
// create main thread hook- used for executing a "hook" function in the main thread of a primary chuck instance
typedef Chuck_DL_MainThreadHook * (CK_DLL_CALL * f_create_main_thread_hook)( Chuck_DL_Query * query, f_mainthreadhook hook, f_mainthreadquit quit, void * bindle );
// register a callback function to receive notification from the VM about shreds (add, remove, etc.)
typedef void (CK_DLL_CALL * f_register_shreds_watcher)( Chuck_DL_Query * query, f_shreds_watcher cb, t_CKUINT options, void * bindle );
// unregister a shreds notification callback
typedef void (CK_DLL_CALL * f_unregister_shreds_watcher)( Chuck_DL_Query * query, f_shreds_watcher cb );

// documentation
// set current class documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_class)( Chuck_DL_Query * query, const char * doc );
// add example of current class
typedef t_CKBOOL (CK_DLL_CALL * f_add_example)( Chuck_DL_Query * query, const char * ex );
// set current function documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_func)( Chuck_DL_Query * query, const char * doc );
// set last mvar documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_var)( Chuck_DL_Query * query, const char * doc );

// shreds watcher flags; meant to bitwise-OR together in options
// these will also be passed back to the callback...
typedef enum {
    CKVM_SHREDS_WATCH_NONE = 0,
    CKVM_SHREDS_WATCH_SPORK = 1,
    CKVM_SHREDS_WATCH_REMOVE = 2,
    CKVM_SHREDS_WATCH_SUSPEND = 4,
    CKVM_SHREDS_WATCH_ACTIVATE = 8,
    CKVM_SHREDS_WATCH_ALL = 0x7fffffff
} ckvmShredsWatcherFlag;

} // end extern "C"





//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Query
// desc: passed to module
//-----------------------------------------------------------------------------
struct Chuck_DL_Query
{
protected:
    // REFACTOR-2017: carrier ref
    Chuck_Carrier * m_carrier;

public:
    // REFACTOR-2017: get associated compiler, vm, env
    Chuck_Compiler * compiler() const { return m_carrier->compiler; }
    Chuck_VM * vm() const { return m_carrier->vm; }
    Chuck_Env * env() const { return m_carrier->env; }
    Chuck_Carrier * carrier() const { return m_carrier; }
    CK_DL_API api() const { return m_api; } // 1.5.1.5

public:
    // function pointers - to be called from client module
    //   QUERY->setname( QUERY, ... );
    //
    // set the name of the module
    f_setname setname;
    // begin class/namespace, can be nexted
    f_begin_class begin_class;
    // add constructor, can be followed by add_arg
    f_add_ctor add_ctor;
    // add destructor, no args allowed
    f_add_dtor add_dtor;
    // add member function, can be followed by add_arg
    f_add_mfun add_mfun;
    // add static function, can be followed by add_arg
    f_add_sfun add_sfun;
    // add member variable
    f_add_mvar add_mvar;
    // add static variable
    f_add_svar add_svar;
    // add argument to function
    f_add_arg add_arg;
    // (ugen only) add tick and pmsg functions
    f_add_ugen_func add_ugen_func;
    // (ugen only) add tick and pmsg functions
    f_add_ugen_funcf add_ugen_funcf;
    // (ugen only) add tick and pmsg functions, specify channels by vm
    f_add_ugen_funcf_auto_num_channels add_ugen_funcf_auto_num_channels;
    // (ugen only) add ctrl parameters
    // f_add_ugen_ctrl add_ugen_ctrl;  // not used but needed for import for now
    // end class/namespace, compile it
    f_end_class end_class;

    // added 1.3.5
    Chuck_DL_Value * last_var;
    f_doc_class doc_class;
    f_doc_func doc_func;
    f_doc_var doc_var;
    f_add_example add_ex;

    // re-added 1.4.0.1
    f_create_main_thread_hook create_main_thread_hook;

    // add binary operator overload; args included | 1.5.1.5 (ge & andrew)
    f_add_op_overload_binary add_op_overload_binary;
    // add unary (prefix) operator overload; arg included
    f_add_op_overload_prefix add_op_overload_prefix;
    // add unary (postfix) operator overload; arg included
    f_add_op_overload_postfix add_op_overload_postfix;

    // register shred notifcations | 1.5.1.5 (ge & andrew)
    f_register_shreds_watcher register_shreds_watcher;
    // un-register shred notifcations | 1.5.1.5 (ge & andrew)
    f_unregister_shreds_watcher unregister_shreds_watcher;

public:
    //-------------------------------------------------------------------------
    // NOTE: everything below std::anything cannot be reliably accessed
    // by offset between dynamic modules, since std::anything could be variable
    // size -- put everything need to be accessed across modules above here!
    // discovered by the vigilant and forever traumatized Jack Atherton,
    // fixed during REFACTOR-2017; warning by the guilt-ridden Ge Wang
    //-------------------------------------------------------------------------
    // dll
    Chuck_DLL * dll_ref;
    // reserved
    void * reserved;
    // sample rate
    t_CKUINT srate;
    // line pos
    int linepos;
    // name of dll
    std::string dll_name;

    // current class
    Chuck_DL_Class * curr_class;
    // current function
    Chuck_DL_Func * curr_func;
    // name
    std::string name;
    // collection of class
    std::vector<Chuck_DL_Class *> classes;
    // stack
    std::vector<Chuck_DL_Class * > stack;

    // flag any error encountered during the query | 1.5.0.5 (ge) added
    t_CKBOOL errorEncountered;

    // DL API reference | 1.5.1.5
    CK_DL_API m_api;

    // collection of operator overloads
    std::vector<Chuck_DL_Func *> op_overloads;

    // constructor
    Chuck_DL_Query( Chuck_Carrier * carrier, Chuck_DLL * dll = NULL );
    // desctructor
    ~Chuck_DL_Query() { this->clear(); }
    // clear
    void clear();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Class
// desc: class built from module
//-----------------------------------------------------------------------------
struct Chuck_DL_Class
{
    // the name of the class
    std::string name;
    // the name of the parent
    std::string parent;
    // ctor
    std::vector<Chuck_DL_Func *> ctors;
    // dtor
    Chuck_DL_Func * dtor;
    // mfun
    std::vector<Chuck_DL_Func *> mfuns;
    // sfun
    std::vector<Chuck_DL_Func *> sfuns;
    // mdata
    std::vector<Chuck_DL_Value *> mvars;
    // sdata
    std::vector<Chuck_DL_Value *> svars;
    // ugen_tick
    f_tick ugen_tick;
    // ugen_tickf
    f_tickf ugen_tickf;
    // ugen_pmsg
    f_pmsg ugen_pmsg;
    // ugen_ctrl/cget
    std::vector<Chuck_DL_Ctrl *> ugen_ctrl;
    // uana_tock
    f_tock uana_tock;
    // collection of recursive classes
    std::vector<Chuck_DL_Class *> classes;
    // current mvar offset
    t_CKUINT current_mvar_offset;
    // # of ugen input and output channels
    t_CKUINT ugen_num_in, ugen_num_out;
    // ckdoc: class description
    std::string doc;
    // ckdoc: examples
    std::vector<std::string> examples;
    // origin string (e.g., filepath if coming from chugin DLL)
    std::string hint_dll_filepath;

    // constructor
    Chuck_DL_Class() { dtor = NULL; ugen_tick = NULL; ugen_tickf = NULL; ugen_pmsg = NULL; uana_tock = NULL; ugen_pmsg = NULL; current_mvar_offset = 0; ugen_num_in = ugen_num_out = 0; }
    // destructor
    ~Chuck_DL_Class();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Value
// desc: value from module
//-----------------------------------------------------------------------------
struct Chuck_DL_Value
{
    // the name of the value
    std::string name;
    // the type of the value
    std::string type;
    // is const
    t_CKBOOL is_const;
    // addr static
    void * static_addr;
    // description
    std::string doc;

    // constructor
    Chuck_DL_Value() { is_const = FALSE; static_addr = NULL; }
    Chuck_DL_Value( const char * t, const char * n, t_CKBOOL c = FALSE, void * a = NULL )
    { name = n; type = t; is_const = c; static_addr = a; }
};




//-----------------------------------------------------------------------------
// name: enum te_Op_OverloadKind | 1.5.1.5 (ge) added
// desc: enumeration for kinds of operator overload
//-----------------------------------------------------------------------------
enum te_Op_OverloadKind
{
    te_op_overload_none,
    te_op_overload_binary,    // LHS op RHS
    te_op_overload_unary_pre, //     op RHS
    te_op_overload_unary_post // LHS op
};




//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Func
// desc: function from module
//-----------------------------------------------------------------------------
struct Chuck_DL_Func
{
    // the name of the function
    std::string name;
    // the return type
    std::string type;
    // the pointer
    union { f_ctor ctor; f_dtor dtor; f_mfun mfun; f_sfun sfun; f_gfun gfun; t_CKUINT addr; };
    // arguments
    std::vector<Chuck_DL_Value *> args;
    // description
    std::string doc;
    // is this an operator overload? if so, which kind? | 1.5.1.5
    te_Op_OverloadKind opOverloadKind;
    // operator to overload | 1.5.1.5
    ae_Operator op2overload;

    // constructor
    Chuck_DL_Func() { ctor = NULL; opOverloadKind = te_op_overload_none; op2overload = ae_op_none; }
    Chuck_DL_Func( const char * t, const char * n, t_CKUINT a )
    { name = n; type = t; addr = a; opOverloadKind = te_op_overload_none; op2overload = ae_op_none; }
    // destructor
    ~Chuck_DL_Func();
    // add arg
    void add_arg( const char * t, const char * n )
    { args.push_back( new Chuck_DL_Value( t, n ) ); }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Ctrl
// desc: ctrl for ugen
//-----------------------------------------------------------------------------
struct Chuck_DL_Ctrl
{
    // the name of the ctrl
    std::string name;
    // the first type
    std::string type;
    // the types of the value
    std::vector<std::string> types;
    // ctrl
    f_ctrl ctrl;
    // cget
    f_cget cget;
};




//------------------------------------------------------------------------------
// alternative functions to make stuff
//------------------------------------------------------------------------------
Chuck_DL_Func * make_new_mfun( const char * t, const char * n, f_mfun mfun );
Chuck_DL_Func * make_new_sfun( const char * t, const char * n, f_sfun sfun );
Chuck_DL_Value * make_new_arg( const char * t, const char * n );
Chuck_DL_Value * make_new_mvar( const char * t, const char * n, t_CKBOOL c = FALSE );
Chuck_DL_Value * make_new_svar( const char * t, const char * n, t_CKBOOL c, void * a );




//-----------------------------------------------------------------------------
// name: struct Chuck_DLL
// desc: dynamic link library
//-----------------------------------------------------------------------------
struct Chuck_DLL : public Chuck_VM_Object
{
public:
    // load dynamic ckx/dll from filename
    t_CKBOOL load( const char * filename,
                   const char * func = CK_QUERY_FUNC,
                   t_CKBOOL lazy = FALSE );
    t_CKBOOL load( f_ck_query query_func, t_CKBOOL lazy = FALSE );
    // get address in loaded ckx
    void * get_addr( const char * symbol );
    // get last error
    const char * last_error() const;
    // unload the ckx
    t_CKBOOL unload();
    // query the content of the dll
    const Chuck_DL_Query * query();
    // probe information about dll without fully loading it
    t_CKBOOL probe();
    // is good
    t_CKBOOL good() const;
    // name
    const char * name() const;
    // full path
    const char * filepath() const;
    // get version major
    t_CKUINT versionMajor();
    // get version minor
    t_CKUINT versionMinor();
    // is version compatible between dll and host
    // major version must be same between chugin and host
    // chugin minor version must less than or equal host minor version
    t_CKBOOL compatible();

public:
    // constructor
    Chuck_DLL( Chuck_Carrier * carrier, const char * xid = NULL )
        : m_handle(NULL), m_id(xid ? xid : ""),
        m_done_query(FALSE), m_version_func(NULL), m_query_func(NULL),
        m_query( carrier, this ), m_versionMajor(0), m_versionMinor(0)
    { }
    // destructor
    ~Chuck_DLL() { this->unload(); }

protected:
    // data
    void * m_handle;
    std::string m_last_error;
    std::string m_filename;
    std::string m_id;
    std::string m_func;
    t_CKBOOL m_done_query;

    f_ck_declversion m_version_func;
    f_ck_query m_query_func;
    Chuck_DL_Query m_query;

protected: // addition info 1.5.0.4 (ge) added
    t_CKUINT m_versionMajor;
    t_CKUINT m_versionMinor;
};


struct Chuck_DL_MainThreadHook
{
public:
    Chuck_DL_MainThreadHook(f_mainthreadhook hook, f_mainthreadquit quit,
                            void * bindle, Chuck_Carrier * carrier);
    t_CKBOOL (CK_DLL_CALL * const activate)(Chuck_DL_MainThreadHook *);
    t_CKBOOL (CK_DLL_CALL * const deactivate)(Chuck_DL_MainThreadHook *);

    Chuck_Carrier * const m_carrier;
    f_mainthreadhook const m_hook;
    f_mainthreadquit const m_quit;
    void * const m_bindle;
    t_CKBOOL m_active;
};




// instantiating a chuck string
Chuck_String * CK_DLL_CALL ck_create_string( Chuck_VM * vm, const char * cstr, t_CKBOOL addRef );
//-----------------------------------------------------------------------------
// invoking chuck functions from c++
//-----------------------------------------------------------------------------
// directly invoke a chuck member function's native implementation from c++
// using object + vtable offset | 1.5.1.5 (ge & andrew)
// NOTE this will call the member function in IMMEDIATE MODE,
// marking it as a time-critical function when called in this manner;
// any time/event operations therein will throw an exception
Chuck_DL_Return CK_DLL_CALL ck_invoke_mfun_immediate_mode( Chuck_Object * obj, t_CKUINT func_vt_offset,
                                               Chuck_VM * vm, Chuck_VM_Shred * shred,
                                               Chuck_DL_Arg * ARGS, t_CKUINT numArgs );




//-----------------------------------------------------------------------------
// dynamic linking callable API to ChucK's innards
//-----------------------------------------------------------------------------
struct Chuck_DL_Api
{
    typedef Chuck_Object * Object;
    typedef Chuck_Type * Type;
    typedef Chuck_String * String;
    typedef Chuck_ArrayInt * ArrayInt; // 1.5.0.1 (ge) added

public:
    static Chuck_DL_Api g_api;
    static inline const Chuck_DL_Api * instance() { return &g_api; }

    // api to access host-side ChucK virtual machine
    struct VMApi
    {
        VMApi();
        // get sample rate | 1.5.1.5
        t_CKUINT (CK_DLL_CALL * const srate)( Chuck_VM * vm );
        // get chuck now | 1.5.1.5
        t_CKTIME (CK_DLL_CALL * const now)( Chuck_VM * vm );
        // create a new lock-free one-producer, one-consumer buffer | 1.5.1.5
        CBufferSimple * (CK_DLL_CALL * const create_event_buffer)( Chuck_VM * vm );
        // queue an event; num_msg must be 1; buffer should be created using create_event_buffer() above | 1.5.1.5
        t_CKBOOL (CK_DLL_CALL * const queue_event)( Chuck_VM * vm, Chuck_Event * event, t_CKINT num_msg, CBufferSimple * buffer );
        // invoke Chuck_Object member function (defined either in chuck or c++) | 1.5.1.5 (ge & andrew)
        // NOTE this will call the member function in IMMEDIATE MODE,
        // marking it as a time-critical function when called in this manner;
        // any time/event operations therein will throw an exception
        Chuck_DL_Return (CK_DLL_CALL * const invoke_mfun_immediate_mode)( Chuck_Object * obj, t_CKUINT func_vt_offset,
                                                                          Chuck_VM * vm, Chuck_VM_Shred * shred, Chuck_DL_Arg * ARGS, t_CKUINT numArgs );
        // throw an exception; if shred is passed in, it will be halted
        void (CK_DLL_CALL * const throw_exception)( const char * exception, const char * desc, Chuck_VM_Shred * shred );
        // log a message in the chuck logging system
        void (CK_DLL_CALL * const em_log)( t_CKINT level, const char * text );
        // system function: remove all shreds in VM; use with care
        void (CK_DLL_CALL * const remove_all_shreds)( Chuck_VM * vm );
    } * const vm;

    // api to access host-side ChucK objects
    struct ObjectApi
    {
        ObjectApi();

    // 1.5.0.0 (nshaheed and ge and anonymous pr-lab member) | changed from private to public
    // also changed all std::string & in this section to const char *
    // intent: this allows for chugins to access member variables and create chuck strings
    public:
        // function pointer get_type()
        Type (CK_DLL_CALL * const get_type)( Object object );
        // add reference count
        void (CK_DLL_CALL * const add_ref)( Object object );
        // release reference count
        void (CK_DLL_CALL * const release)( Object object );
        // get reference count
        t_CKUINT (CK_DLL_CALL * const refcount)( Object object );
        // instantiating and initializing a ChucK object by type, with reference to a parent shred
        // if addRef == TRUE the newly created object will have a reference count of 1; otherwise 0
        // NOTE set addRef to TRUE if you intend to keep a reference of the newly created object around (e.g., in the chugin)
        // NOTE set addRef to FALSE if the created object is to be returned without keeping a reference around
        Object (CK_DLL_CALL * const create)( Chuck_VM_Shred *, Type type, t_CKBOOL addRef );
        // instantiating and initializing a ChucK object by type, with no reference to a parent shred
        // if addRef == TRUE the newly created object will have a reference count of 1; otherwise 0
        Object (CK_DLL_CALL * const create_without_shred)( Chuck_VM *, Type type, t_CKBOOL addRef );
        // instantiate and initialize a ChucK string by type
        // if addRef == TRUE the newly created object will have a reference count of 1; otherwise 0
        String (CK_DLL_CALL * const create_string)( Chuck_VM *, const char * value, t_CKBOOL addRef );
        // get the origin shred
        Chuck_VM_Shred * (CK_DLL_CALL * const get_origin_shred)( Object object );
        // set the origin shred; this should only be invoked by system-level chugins; use with care
        void (CK_DLL_CALL * const set_origin_shred)( Object object, Chuck_VM_Shred * shred );
        // function pointers for get_mvar_*()
        t_CKBOOL (CK_DLL_CALL * const get_mvar_int)( Object object, const char * name, t_CKINT & value );
        t_CKBOOL (CK_DLL_CALL * const get_mvar_float)( Object object, const char * name, t_CKFLOAT & value );
        t_CKBOOL (CK_DLL_CALL * const get_mvar_dur)( Object object, const char * name, t_CKDUR & value );
        t_CKBOOL (CK_DLL_CALL * const get_mvar_time)( Object object, const char * name, t_CKTIME & value );
        t_CKBOOL (CK_DLL_CALL * const get_mvar_string)( Object object, const char * name, String & value );
        t_CKBOOL (CK_DLL_CALL * const get_mvar_object)( Object object, const char * name, Object & value );
        // function pointer for set_string()
        t_CKBOOL (CK_DLL_CALL * const set_string)( String string, const char * value );
        // array_int operations
        t_CKBOOL (CK_DLL_CALL * const array_int_size)( ArrayInt array, t_CKINT & value );
        t_CKBOOL (CK_DLL_CALL * const array_int_push_back)( ArrayInt array, t_CKUINT value );
        t_CKBOOL (CK_DLL_CALL * const array_int_get_idx)( ArrayInt array, t_CKINT idx, t_CKUINT & value );
        t_CKBOOL (CK_DLL_CALL * const array_int_get_key)( ArrayInt array, const std::string & key, t_CKUINT & value );
    } * const object;

    // access to host-side chuck types
    struct TypeApi
    {
        TypeApi();
        // look up type by name
        Type (CK_DLL_CALL * const lookup)( Chuck_VM *, const char * name );
        // get vtable offset for named function (if overloaded, returns first one); returns < 0 if not found
        t_CKINT (CK_DLL_CALL * const get_vtable_offset)( Chuck_VM *, Type type, const char * funcName );
        // test if two chuck types are equal
        t_CKBOOL (CK_DLL_CALL * const is_equal)(Type lhs, Type rhs);
        // test if lhs is a type of rhs (e.g., SinOsc is a type of UGen)
        t_CKBOOL (CK_DLL_CALL * const isa)(Type lhs, Type rhs);
        // register a callback to be invoked whenever a base-type (or its subclass) is instantiated, with option for type system to auto-set shred origin if available
        void (CK_DLL_CALL * const callback_on_instantiate)( f_callback_on_instantiate callback, Type base_type, Chuck_VM * vm, t_CKBOOL shouldSetShredOrigin );
    } * const type;

    // constructor
    Chuck_DL_Api() :
        vm(new VMApi),
        object(new ObjectApi),
        type(new TypeApi)
    { }

private:
    // make this object un-copy-able
    Chuck_DL_Api( Chuck_DL_Api & a ) :
        vm(a.vm),
        object(a.object),
        type(a.type)
    { assert(0); };
    // make this object un-copy-able, part 2
    Chuck_DL_Api & operator=( Chuck_DL_Api & a ) { assert(0); return a; }
};




// dlfcn interface
#if defined(__PLATFORM_APPLE__)
#include <AvailabilityMacros.h>
#endif

// dlfcn interface, panther or below
#if defined(__PLATFORM_APPLE__) && MAC_OS_X_VERSION_MAX_ALLOWED <= 1030

#error ChucK not support on Mac OS X 10.3 or lower

#elif defined(__PLATFORM_WINDOWS__)

          #ifdef __cplusplus
          extern "C" {
          #endif

          #define RTLD_LAZY         0x1
          #define RTLD_NOW          0x2
          #define RTLD_LOCAL        0x4
          #define RTLD_GLOBAL       0x8
          #define RTLD_NOLOAD       0x10
          #define RTLD_SHARED       0x20    /* not used, the default */
          #define RTLD_UNSHARED     0x40
          #define RTLD_NODELETE     0x80
          #define RTLD_LAZY_UNDEF   0x100

          void * dlopen( const char * path, int mode );
          void * dlsym( void * handle, const char * symbol );
          const char * dlerror( void );
          int dlclose( void * handle );
          // 1.4.2.0 (ge) added DLERROR_BUFFER_LENGTH
          // 1.5.1.5 (ge) increased DLERROR_BUFFER_LENGTH from 128 to 512
          #define DLERROR_BUFFER_LENGTH 512
          static char dlerror_buffer[DLERROR_BUFFER_LENGTH];

          #ifdef __cplusplus
          }
          #endif

#else
  #include "dlfcn.h"
#endif



#endif
