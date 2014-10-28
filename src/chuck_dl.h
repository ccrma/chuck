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
#include "chuck_oo.h"
#include <string>
#include <vector>
#include <map>


// major version must be the same between chuck:chugin
#define CK_DLL_VERSION_MAJOR (0x0005)
// minor version of chugin must be less than or equal to chuck's
#define CK_DLL_VERSION_MINOR (0x0001)
#define CK_DLL_VERSION_MAKE(maj,min) ((t_CKUINT)(((maj) << 16) | (min)))
#define CK_DLL_VERSION_GETMAJOR(v) (((v) >> 16) & 0xFFFF)
#define CK_DLL_VERSION_GETMINOR(v) ((v) & 0xFFFF)
#define CK_DLL_VERSION (CK_DLL_VERSION_MAKE(CK_DLL_VERSION_MAJOR, CK_DLL_VERSION_MINOR))




extern char g_default_chugin_path[];
extern char g_chugin_path_envvar[];



// forward references
struct Chuck_DL_Query;
struct Chuck_DL_Class;
struct Chuck_DL_Func;
struct Chuck_DL_Value;
struct Chuck_DL_Ctrl;
union  Chuck_DL_Return;
struct Chuck_DLL;
struct Chuck_UGen;
struct Chuck_UAna;
struct Chuck_UAnaBlobProxy;
struct Chuck_DL_MainThreadHook;
namespace Chuck_DL_Api { struct Api; }


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
#define GET_CK_OBJECT(ptr)     (*(Chuck_Object **)ptr)
#define GET_CK_STRING(ptr)     (*(Chuck_String **)ptr)

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
#define GET_NEXT_OBJECT(ptr)   (*((Chuck_Object **&)ptr)++)
#define GET_NEXT_STRING(ptr)   (*((Chuck_String **&)ptr)++)

// param conversion
#define SET_CK_FLOAT(ptr,v)      (*(t_CKFLOAT *&)ptr=v)
#define SET_CK_SINGLE(ptr,v)     (*(float *&)ptr=v)
#define SET_CK_DOUBLE(ptr,v)     (*(double *&)ptr=v)
#define SET_CK_INT(ptr,v)        (*(t_CKINT *&)ptr=v)
#define SET_CK_UINT(ptr,v)       (*(t_CKUINT *&)ptr=v)
#define SET_CK_TIME(ptr,v)       (*(t_CKTIME *&)ptr=v)
#define SET_CK_DUR(ptr,v)        (*(t_CKDUR *&)ptr=v)
#define SET_CK_STRING(ptr,v)     (*(Chuck_String **&)ptr=v)

// param conversion with pointer advance
#define SET_NEXT_FLOAT(ptr,v)    (*((t_CKFLOAT *&)ptr)++=v)
#define SET_NEXT_SINGLE(ptr,v)   (*((float *&)ptr)++=v)
#define SET_NEXT_DOUBLE(ptr,v)   (*((double *&)ptr)++=v)
#define SET_NEXT_INT(ptr,v)      (*((t_CKINT *&)ptr)++=v)
#define SET_NEXT_UINT(ptr,v)     (*((t_CKUINT *&)ptr)++=v)
#define SET_NEXT_TIME(ptr,v)     (*((t_CKTIME *&)ptr)++=v)
#define SET_NEXT_DUR(ptr,v)      (*((t_CKDUR *&)ptr)++=v)
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
#define OBJ_MEMBER_OBJECT(obj,offset)   (*(Chuck_Object **)OBJ_MEMBER_DATA(obj,offset))
#define OBJ_MEMBER_STRING(obj,offset)   (*(Chuck_String **)OBJ_MEMBER_DATA(obj,offset))


// chuck dll export linkage and calling convention
#if defined (__PLATFORM_WIN32__) 
  #define CK_DLL_LINKAGE extern "C" __declspec( dllexport )
#else
  #define CK_DLL_LINKAGE extern "C" 
#endif 

// calling convention of functions provided by chuck to the dll
#if defined(__WINDOWS_DS__)
  #define CK_DLL_CALL    _cdecl
#else
  #define CK_DLL_CALL
#endif

typedef const Chuck_DL_Api::Api *CK_DL_API;

// macro for defining ChucK DLL export functions
// example: CK_DLL_EXPORT(int) foo() { return 1; }
#define CK_DLL_EXPORT(type) CK_DLL_LINKAGE type CK_DLL_CALL
// macro for declaring version of ChucK DL a given DLL links to
// example: CK_DLL_DECLVERSION
#define CK_DLL_DECLVERSION CK_DLL_EXPORT(t_CKUINT) ck_version() { return CK_DLL_VERSION; }
// macro for defining ChucK DLL export query-functions
// example: CK_DLL_QUERY
#ifndef __CK_DLL_STATIC__
#define CK_DLL_QUERY(name) CK_DLL_DECLVERSION CK_DLL_EXPORT(t_CKBOOL) ck_query( Chuck_DL_Query * QUERY )
#else 
#define CK_DLL_QUERY(name) CK_DLL_DECLVERSION CK_DLL_EXPORT(t_CKBOOL) ck_##name_query( Chuck_DL_Query * QUERY )
#endif // __CK_DLL_STATIC__
// macro for defining ChucK DLL export allocator
// example: CK_DLL_ALLOC(foo)
#define CK_DLL_ALLOC(name) CK_DLL_EXPORT(Chuck_Object *) name( Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export constructors
// example: CK_DLL_CTOR(foo)
#define CK_DLL_CTOR(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export destructors
// example: CK_DLL_DTOR(foo)
#define CK_DLL_DTOR(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export member functions
// example: CK_DLL_MFUN(foo)
#define CK_DLL_MFUN(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export static functions
// example: CK_DLL_SFUN(foo)
#define CK_DLL_SFUN(name) CK_DLL_EXPORT(void) name( void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen tick functions
// example: CK_DLL_TICK(foo)
#define CK_DLL_TICK(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, SAMPLE in, SAMPLE * out, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen multi-channel tick functions
// example: CK_DLL_TICKF(foo)
#define CK_DLL_TICKF(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, SAMPLE * in, SAMPLE * out, t_CKUINT nframes, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen ctrl functions
// example: CK_DLL_CTRL(foo)
#define CK_DLL_CTRL(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen cget functions
// example: CK_DLL_CGET(foo)
#define CK_DLL_CGET(name) CK_DLL_EXPORT(void) name( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export ugen pmsg functions
// example: CK_DLL_PMSG(foo)
#define CK_DLL_PMSG(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, const char * MSG, void * ARGS, Chuck_VM_Shred * SHRED, CK_DL_API API )
// macro for defining ChucK DLL export uana tock functions
// example: CK_DLL_TOCK(foo)
#define CK_DLL_TOCK(name) CK_DLL_EXPORT(t_CKBOOL) name( Chuck_Object * SELF, Chuck_UAna * UANA, Chuck_UAnaBlobProxy * BLOB, Chuck_VM_Shred * SHRED, CK_DL_API API )


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
typedef Chuck_Object * (CK_DLL_CALL * f_alloc)( Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_ctor)( Chuck_Object * SELF, void * ARGS, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_dtor)( Chuck_Object * SELF, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_mfun)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_sfun)( void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API );
// ugen specific
typedef t_CKBOOL (CK_DLL_CALL * f_tick)( Chuck_Object * SELF, SAMPLE in, SAMPLE * out, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKBOOL (CK_DLL_CALL * f_tickf)( Chuck_Object * SELF, SAMPLE * in, SAMPLE * out, t_CKUINT nframes, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_ctrl)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKVOID (CK_DLL_CALL * f_cget)( Chuck_Object * SELF, void * ARGS, Chuck_DL_Return * RETURN, Chuck_VM_Shred * SHRED, CK_DL_API API );
typedef t_CKBOOL (CK_DLL_CALL * f_pmsg)( Chuck_Object * SELF, const char * MSG, void * ARGS, Chuck_VM_Shred * SHRED, CK_DL_API API );
// uana specific
typedef t_CKBOOL (CK_DLL_CALL * f_tock)( Chuck_Object * SELF, Chuck_UAna * UANA, Chuck_UAnaBlobProxy * BLOB, Chuck_VM_Shred * SHRED, CK_DL_API API );
// "main thread" hook
typedef t_CKBOOL (CK_DLL_CALL * f_mainthreadhook)( void * bindle );
// "main thread" quit (stop running hook)
typedef t_CKBOOL (CK_DLL_CALL * f_mainthreadquit)( void * bindle );
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
// ** add a ugen control
typedef void (CK_DLL_CALL * f_add_ugen_ctrl)( Chuck_DL_Query * query, f_ctrl ctrl, f_cget cget, 
                                              const char * type, const char * name );
// end class/namespace - must correspondent with begin_class.  returns false on error
typedef t_CKBOOL (CK_DLL_CALL * f_end_class)( Chuck_DL_Query * query );
// register 
typedef Chuck_DL_MainThreadHook * (CK_DLL_CALL * f_create_main_thread_hook)( Chuck_DL_Query * query, f_mainthreadhook hook, f_mainthreadquit quit, void * bindle );
    
// documentation
// set current class documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_class)( Chuck_DL_Query * query, const char * doc );
// add example of current class
typedef t_CKBOOL (CK_DLL_CALL * f_add_example)( Chuck_DL_Query * query, const char * ex );
// set current function documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_func)( Chuck_DL_Query * query, const char * doc );
// set last mvar documentation
typedef t_CKBOOL (CK_DLL_CALL * f_doc_var)( Chuck_DL_Query * query, const char * doc );
}


//-----------------------------------------------------------------------------
// name: struct Chuck_DL_Query
// desc: passed to module
//-----------------------------------------------------------------------------
struct Chuck_DL_Query
{
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
    // (ugen only) add ctrl parameters
    f_add_ugen_ctrl add_ugen_ctrl;
    // end class/namespace, compile it
    f_end_class end_class;
    
    // name
    std::string name;
    // current class
    Chuck_DL_Class * curr_class;
    // current function
    Chuck_DL_Func * curr_func;
    // collection of class
    std::vector<Chuck_DL_Class *> classes;
    // stack
    std::vector<Chuck_DL_Class * >stack;
    
    // name of dll
    std::string dll_name;
    // dll
    Chuck_DLL * dll_ref;
    // reserved
    void * reserved;
    // sample rate
    t_CKUINT srate;
    // bsize
    t_CKUINT bufsize;
    // line pos
    int linepos;
    
    // added 1.3.2.0
    f_create_main_thread_hook create_main_thread_hook;
    
    // added 1.3.5
    Chuck_DL_Value * last_var;
    f_doc_class doc_class;
    f_doc_func doc_func;
    f_doc_var doc_var;
    f_add_example add_ex;
    
    // constructor
    Chuck_DL_Query();
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
    
    t_CKUINT ugen_num_in, ugen_num_out;
    
    std::string doc;
    std::vector<std::string> examples;
    
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
    
    std::string doc;

    // constructor
    Chuck_DL_Value() { is_const = FALSE; static_addr = NULL; }
    Chuck_DL_Value( const char * t, const char * n, t_CKBOOL c = FALSE, void * a = NULL )
    { name = n; type = t; is_const = c; static_addr = a; }
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
    union { f_ctor ctor; f_dtor dtor; f_mfun mfun; f_sfun sfun; t_CKUINT addr; };
    // arguments
    std::vector<Chuck_DL_Value *> args;
    
    std::string doc;
    
    // constructor
    Chuck_DL_Func() { ctor = NULL; }
    Chuck_DL_Func( const char * t, const char * n, t_CKUINT a )
    { name = n; type = t; addr = a; }
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
    Chuck_Object * v_object;
    Chuck_String * v_string;
    
    Chuck_DL_Return() { v_complex.re = 0.0; v_complex.im = 0.0; }
};


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
    t_CKBOOL unload( );
    // query the content of the dll
    const Chuck_DL_Query * query( );
    // is good
    t_CKBOOL good() const;
    // name
    const char * name() const;
    
public:
    // constructor
    Chuck_DLL( const char * xid = NULL )
        : m_handle(NULL), m_id(xid ? xid : ""),
        m_done_query(FALSE), m_query_func(NULL), m_version_func(NULL)
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
};

struct Chuck_DL_MainThreadHook
{
public:
    Chuck_DL_MainThreadHook(f_mainthreadhook hook, f_mainthreadquit quit,
                            void * bindle, Chuck_VM * vm);
    t_CKBOOL (* const activate)(Chuck_DL_MainThreadHook *);
    t_CKBOOL (* const deactivate)(Chuck_DL_MainThreadHook *);
    
    Chuck_VM * const m_vm;
    f_mainthreadhook const m_hook;
    f_mainthreadquit const m_quit;
    void * const m_bindle;
    t_CKBOOL m_active;
};


/* API to ChucK's innards */


namespace Chuck_DL_Api
{
    
typedef void * Object;
typedef void * Type;
typedef void * String;

struct Api
{
public:
    static Api g_api;
    static inline const Api * instance() { return &g_api; }
    
    struct VMApi
    {
        VMApi();
        
        t_CKUINT (* const get_srate)();
    } * const vm;
    
    struct ObjectApi
    {
        ObjectApi();
        
    private:
        Type (* const get_type)( std::string &name );

        Object (* const create)( Type type );
        
        String (* const create_string)( std::string &value );
        
        t_CKBOOL (* const get_mvar_int)( Object object, std::string &name, t_CKINT &value );
        t_CKBOOL (* const get_mvar_float)( Object object, std::string &name, t_CKFLOAT &value );
        t_CKBOOL (* const get_mvar_dur)( Object object, std::string &name, t_CKDUR &value );
        t_CKBOOL (* const get_mvar_time)( Object object, std::string &name, t_CKTIME &value );
        t_CKBOOL (* const get_mvar_string)( Object object, std::string &name, String &value );
        t_CKBOOL (* const get_mvar_object)( Object object, std::string &name, Object &value );
        
        t_CKBOOL (* const set_string)( String string, std::string &value );
        
    } * const object;
    
    Api() :
    vm(new VMApi),
    object(new ObjectApi)
    {}
    
private:
    Api(Api &a) :
    vm(a.vm),
    object(a.object)
    { assert(0); };
    
    Api &operator=(Api &a) { assert(0); return a; }
};
    
}



// dlfcn interface
#if defined(__MACOSX_CORE__)
#include <AvailabilityMacros.h>
#endif

// dlfcn interface, panther or below
#if defined(__MACOSX_CORE__) && MAC_OS_X_VERSION_MAX_ALLOWED <= 1030

#error ChucK not support on Mac OS X 10.3 or lower

#elif defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__) 
	 	 	 	 
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
          static char dlerror_buffer[128]; 
         
          #ifdef __cplusplus 
          } 
          #endif 

#else
  #include "dlfcn.h"
#endif



#endif
