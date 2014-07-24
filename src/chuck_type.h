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
// file: chuck_type.h
// desc: chuck type-system / type-checker
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original
//       Autumn 2005 - rewrite
//-----------------------------------------------------------------------------
#ifndef __CHUCK_TYPE_H__
#define __CHUCK_TYPE_H__

#include "chuck_def.h"
#include "chuck_absyn.h"
#include "chuck_oo.h"
#include "chuck_dl.h"
#include "chuck_errmsg.h"



//-----------------------------------------------------------------------------
// name: enum te_Type
// desc: basic, default ChucK types
//-----------------------------------------------------------------------------
typedef enum {
    // general types
    te_int, te_uint, te_single, te_float, te_double, te_time, te_dur,
    te_complex, te_polar, te_string, te_thread, te_shred, te_class,
    te_function, te_object, te_user, te_array, te_null, te_ugen, te_uana, 
    te_event, te_void, te_stdout, te_stderr, te_adc, te_dac, te_bunghole, 
    te_uanablob, te_io, te_fileio, te_chout, te_cherr, te_multi
} te_Type;




//-----------------------------------------------------------------------------
// name: enum te_HowMuch
// desc: how much to scan/type check
//-----------------------------------------------------------------------------
typedef enum { 
    te_do_all = 0, te_do_classes_only, te_do_no_classes
} te_HowMuch;




//-----------------------------------------------------------------------------
// name: struct Chuck_Scope
// desc: scoping structure
//-----------------------------------------------------------------------------
template <class T>
struct Chuck_Scope
{
public:
    // constructor
    Chuck_Scope() { this->push(); }
    // desctructor
    ~Chuck_Scope() { this->pop(); }

    // push scope
    void push() { scope.push_back( new std::map<S_Symbol, Chuck_VM_Object *> ); }

    // pop scope
    void pop()
    {
        assert( scope.size() != 0 );
        // TODO: release contents of scope.back()
        delete scope.back(); scope.pop_back();
    }

    // reset the scope
    void reset()
    { scope.clear(); this->push(); }
    
    // atomic commit
    void commit()
    {
        assert( scope.size() != 0 );        
        std::map<S_Symbol, Chuck_VM_Object *>::iterator iter;

        // go through buffer    
        for( iter = commit_map.begin(); iter != commit_map.end(); iter++ )
        {
            // add to front/where
            (*scope.front())[(*iter).first] = (*iter).second;
        }

        // clear
        commit_map.clear();
    }

    // roll back since last commit or beginning
    void rollback()
    {
        assert( scope.size() != 0 );
        std::map<S_Symbol, Chuck_VM_Object *>::iterator iter;

        // go through buffer    
        for( iter = commit_map.begin(); iter != commit_map.end(); iter++ )
        {
            // release
            (*iter).second->release();
        }

        // clear
        commit_map.clear();
    }

    // add
    void add( const std::string & xid, Chuck_VM_Object * value )
    { this->add( insert_symbol(xid.c_str()), value ); }
    void add( S_Symbol xid, Chuck_VM_Object * value )
    {
        assert( scope.size() != 0 );
        // add if back is NOT front
        if( scope.back() != scope.front() )
            (*scope.back())[xid] = value;
        // add for commit
        else commit_map[xid] = value;
        // add reference
        SAFE_ADD_REF(value);
    }

    // lookup id
    T operator []( const std::string & xid )
    { return (T)this->lookup( xid ); }
    T lookup( const std::string & xid, t_CKINT climb = 1 )
    { return (T)this->lookup( insert_symbol(xid.c_str()), climb ); }
    // -1 base, 0 current, 1 climb
    T lookup( S_Symbol xid, t_CKINT climb = 1 )
    {
        Chuck_VM_Object * val = NULL; assert( scope.size() != 0 );

        if( climb == 0 )
        {
            val = (*scope.back())[xid];
            // look in commit buffer if the back is the front
            if( !val && scope.back() == scope.front() 
                && (commit_map.find(xid) != commit_map.end()) ) 
                val = commit_map[xid];
        }
        else if( climb > 0 )
        {
            for( t_CKUINT i = scope.size(); i > 0; i-- )
            { if( ( val = (*scope[i-1])[xid] ) ) break; }

            // look in commit buffer
            if( !val && (commit_map.find(xid) != commit_map.end()) )
                val = commit_map[xid];
        }
        else
        {
            val = (*scope.front())[xid];
            // look in commit buffer
            if( !val && (commit_map.find(xid) != commit_map.end()) )
                val = commit_map[xid];
        }

        return (T)val;
    }

    // get list of top level
    void get_toplevel( std::vector<Chuck_VM_Object *> & out )
    {
        assert( scope.size() != 0 );
        std::map<S_Symbol, Chuck_VM_Object *>::iterator iter;

        // clear the out
        out.clear();
        // get the front of the array
        std::map<S_Symbol, Chuck_VM_Object *> * m = scope.front();

        // go through map
        for( iter = m->begin(); iter != m->end(); iter++ )
        {
            // add
            out.push_back( (*iter).second );
        }
    }
    
    // get list of top level
    void get_level( int level, std::vector<Chuck_VM_Object *> & out )
    {
        assert( scope.size() >= level );
        std::map<S_Symbol, Chuck_VM_Object *>::iterator iter;
        
        // clear the out
        out.clear();
        // get the front of the array
        std::map<S_Symbol, Chuck_VM_Object *> * m = &commit_map;
        
        // go through map
        for( iter = m->begin(); iter != m->end(); iter++ )
        {
            // add
            out.push_back( (*iter).second );
        }
    }
    

protected:
    std::vector<std::map<S_Symbol, Chuck_VM_Object *> *> scope;
    std::map<S_Symbol, Chuck_VM_Object *> commit_map;
};


// forward reference
struct Chuck_Type;
struct Chuck_Value;
struct Chuck_Func;
struct Chuck_Multi;
struct Chuck_VM;
struct Chuck_VM_Code;

struct Chuck_DLL;


//-----------------------------------------------------------------------------
// name: struct Chuck_Namespace
// desc: Chuck Namespace containing semantic information
//-----------------------------------------------------------------------------
struct Chuck_Namespace : public Chuck_VM_Object
{
    // maps
    Chuck_Scope<Chuck_Type *> type;
    Chuck_Scope<Chuck_Value *> value;
    Chuck_Scope<Chuck_Func *> func;

    // virtual table
    Chuck_VTable obj_v_table;
    // static data segment
    t_CKBYTE * class_data;
    // static data segment size
    t_CKUINT class_data_size;

    // name
    std::string name;
    // top-level code
    Chuck_VM_Code * pre_ctor;
    // destructor
    Chuck_VM_Code * dtor;
    // type that contains this
    Chuck_Namespace * parent;
    // address offset
    t_CKUINT offset;

    // constructor
    Chuck_Namespace() { pre_ctor = NULL; dtor = NULL; parent = NULL; offset = 0; 
                        class_data = NULL; class_data_size = 0; }
    // destructor
    virtual ~Chuck_Namespace() {
        /* TODO: SAFE_RELEASE( this->parent ); */
        /* TODO: SAFE_DELETE( pre_ctor ); */
        /* TODO: SAFE_DELETE( dtor ); */
    }

    // look up type
    Chuck_Type * lookup_type( const std::string & name, t_CKINT climb = 1 );
    Chuck_Type * lookup_type( S_Symbol name, t_CKINT climb = 1 );
    // look up value
    Chuck_Value * lookup_value( const std::string & name, t_CKINT climb = 1 );
    Chuck_Value * lookup_value( S_Symbol name, t_CKINT climb = 1 );
    // look up func
    Chuck_Func * lookup_func( const std::string & name, t_CKINT climb = 1 );
    Chuck_Func * lookup_func( S_Symbol name, t_CKINT climb = 1 );

    // commit the maps
    void commit() { 
        EM_log( CK_LOG_FINER, "committing namespace: '%s'...", name.c_str() );
        type.commit(); value.commit(); func.commit();
    }
    
    // rollback the maps
    void rollback() { 
        EM_log( CK_LOG_FINER, "rolling back namespace: '%s'...", name.c_str() );
        type.rollback(); value.rollback(); func.rollback();
    }

    // get top level types
    void get_types( std::vector<Chuck_Type *> & out );
    // get top level values
    void get_values( std::vector<Chuck_Value *> & out );
    // get top level functions
    void get_funcs( std::vector<Chuck_Func *> & out );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Context
// desc: runtime type information pertaining to a file
//-----------------------------------------------------------------------------
struct Chuck_Context : public Chuck_VM_Object  
{
    // src_name
    std::string filename;
    // full filepath (if available) -- added 1.3.0.0
    std::string full_path;
    // parse tree
    a_Program parse_tree;
    // context namespace
    Chuck_Namespace * nspc;
    // public class def if any
    a_Class_Def public_class_def;
    // error - means to free nspc too
    t_CKBOOL has_error;

    // progress
    enum { P_NONE = 0, P_CLASSES_ONLY, P_ALL };
    // progress in scan / type check / emit
    t_CKUINT progress;

    // things to release with the context
    std::vector<Chuck_VM_Object *> new_types;
    std::vector<Chuck_VM_Object *> new_values;
    std::vector<Chuck_VM_Object *> new_funcs;
    std::vector<Chuck_VM_Object *> new_nspc;

    // commit map
    std::map<Chuck_Namespace *, Chuck_Namespace *> commit_map;
    // add for commit/rollback
    void add_commit_candidate( Chuck_Namespace * nspc );
    // commit
    void commit();
    // rollback
    void rollback();

    // constructor
    Chuck_Context() { parse_tree = NULL; nspc = new Chuck_Namespace; 
                      public_class_def = NULL; has_error = FALSE;
                      progress = P_NONE; }
    // destructor
    virtual ~Chuck_Context();
    // get the top-level code
    Chuck_VM_Code * code() { return nspc->pre_ctor; }

    // special alloc
    Chuck_Type * new_Chuck_Type();
    Chuck_Value * new_Chuck_Value( Chuck_Type * t, const std::string & name );
    Chuck_Func * new_Chuck_Func();
    Chuck_Namespace * new_Chuck_Namespace();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Env
// desc: chuck env with type info
//-----------------------------------------------------------------------------
struct Chuck_Env : public Chuck_VM_Object
{
public:
    static t_CKBOOL startup();
    static Chuck_Env * instance();
    static t_CKBOOL shutdown();

private:
    static Chuck_Env * our_instance;
    // constructor
    Chuck_Env( )
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
    }

protected:
    // global namespace
    Chuck_Namespace * global_nspc;
    // global context
    Chuck_Context global_context;
    // user-global namespace
    Chuck_Namespace * user_nspc;

public:
    // global namespace
    Chuck_Namespace * global() { return global_nspc; }
    // global namespace
    Chuck_Namespace * user()
    {
        if(user_nspc == NULL)
            return global_nspc;
        else
            return user_nspc;
    }
    
    // namespace stack
    std::vector<Chuck_Namespace *> nspc_stack;
    // expression namespace
    Chuck_Namespace * curr;
    // class stack
    std::vector<Chuck_Type *> class_stack;
    // current class definition
    Chuck_Type * class_def;
    // current function definition
    Chuck_Func * func;
    // how far nested in a class definition
    t_CKUINT class_scope;

    // current contexts in memory
    std::vector<Chuck_Context *> contexts;
    // current context
    Chuck_Context * context;

    // control scope (for break, continue)
    std::vector<a_Stmt> breaks;

    // reserved words
    std::map<std::string, t_CKBOOL> key_words;
    std::map<std::string, t_CKBOOL> key_types;
    std::map<std::string, t_CKBOOL> key_values;

    // deprecated types
    std::map<std::string, std::string> deprecated;
    // level - 0:stop, 1:warn, 2:ignore
    t_CKINT deprecate_level;

    // destructor
    virtual ~Chuck_Env() { }

    // reset
    void reset( )
    {
        // TODO: release stack items?
        nspc_stack.clear();
        nspc_stack.push_back( this->global() );
        if(user_nspc != NULL)
            nspc_stack.push_back( this->user_nspc );
        // TODO: release stack items?
        class_stack.clear(); class_stack.push_back( NULL );
        // should be at top level
        assert( context == &global_context );
        // assign : TODO: release curr? class_def? func?
        // TODO: need another function, since this is called from constructor
        if(user_nspc != NULL)
            curr = this->user();
        else
            curr = this->global();
        class_def = NULL; func = NULL;
        // make sure this is 0
        class_scope = 0;
    }
    
    void load_user_namespace()
    {
        // user namespace
        user_nspc = new Chuck_Namespace;
        user_nspc->name = "[user]";
        user_nspc->parent = global_nspc;
        SAFE_ADD_REF(global_nspc);
        SAFE_ADD_REF(user_nspc);
    }
    
    void clear_user_namespace()
    {
        if(user_nspc) SAFE_RELEASE(user_nspc->parent);
        SAFE_RELEASE(user_nspc);
        load_user_namespace();
        this->reset();
    }

    // top
    Chuck_Namespace * nspc_top( )
    { assert( nspc_stack.size() > 0 ); return nspc_stack.back(); }
    Chuck_Type * class_top( )
    { assert( class_stack.size() > 0 ); return class_stack.back(); }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_UGen_Info
// desc: ugen info stored with ugen types
//-----------------------------------------------------------------------------
struct Chuck_UGen_Info : public Chuck_VM_Object
{
    // tick function pointer
    f_tick tick;
    // multichannel/vector tick function pointer (added 1.3.0.0)
    f_tickf tickf;
    // pmsg function pointer
    f_pmsg pmsg;
    // number of incoming channels
    t_CKUINT num_ins;
    // number of outgoing channels
    t_CKUINT num_outs;
    
    // for uana, NULL for ugen
    f_tock tock;
    // number of incoming ana channels
    t_CKUINT num_ins_ana;
    // number of outgoing channels
    t_CKUINT num_outs_ana;

    // constructor
    Chuck_UGen_Info()
    { tick = NULL; tickf = NULL; pmsg = NULL; num_ins = num_outs = 1; 
      tock = NULL; num_ins_ana = num_outs_ana = 1; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Type
// desc: class containing information about a type
//-----------------------------------------------------------------------------_id
struct Chuck_Type : public Chuck_VM_Object
{
    // type id
    te_Type xid;
    // type name
    std::string name;
    // type parent (could be NULL)
    Chuck_Type * parent;
    // size (in bytes)
    t_CKUINT size;
    // owner of the type
    Chuck_Namespace * owner;
    // array type
    union { Chuck_Type * array_type; Chuck_Type * actual_type; };
    // array size (equals 0 means not array, else dimension of array)
    t_CKUINT array_depth;
    // object size (size in memory)
    t_CKUINT obj_size;
    // type info
    Chuck_Namespace * info;
    // func info
    Chuck_Func * func;
    // def
    a_Class_Def def;
    // ugen
    Chuck_UGen_Info * ugen_info;
    // copy
    t_CKBOOL is_copy;
    // defined
    t_CKBOOL is_complete;
    // has pre constructor
    t_CKBOOL has_constructor;
    // has destructor
    t_CKBOOL has_destructor;
    // custom allocator
    f_alloc allocator;
    
    // documentation
    std::string doc;
    // example files
    std::vector<std::string> examples;

public:
    // constructor
    Chuck_Type( te_Type _id = te_null, const std::string & _n = "", 
                Chuck_Type * _p = NULL, t_CKUINT _s = 0 )
    {
        xid = _id; name = _n; parent = _p; size = _s; owner = NULL; 
        array_type = NULL; array_depth = 0; obj_size = 0;
        info = NULL; func = NULL; def = NULL; is_copy = FALSE; 
        ugen_info = NULL; is_complete = TRUE; has_constructor = FALSE;
        has_destructor = FALSE;
        allocator = NULL;
    }

    // destructor
    virtual ~Chuck_Type() { reset(); }
    
    // reset
    void reset()
    {
        // fprintf( stderr, "type: %s %i\n", c_name(), (t_CKUINT)this );
        xid = te_void; 
        size = array_depth = obj_size = 0;
        is_copy = FALSE;

        // free only if not locked: to prevent garbage collection after exit
        if( !this->m_locked )
        {
            // TODO: uncomment this, fix it to behave correctly
            // release references
            // SAFE_RELEASE(parent);
            // SAFE_RELEASE(array_type);
            SAFE_RELEASE(info);
            // SAFE_RELEASE(owner);
            // SAFE_RELEASE(func);
            // SAFE_RELEASE(ugen_info);
        }
    }   
    
    // assignment - this does not touch the Chuck_VM_Object
    const Chuck_Type & operator =( const Chuck_Type & rhs )
    {
        // release first
        this->reset();

        // copy
        this->xid = rhs.xid;
        this->name = rhs.name;
        this->parent = rhs.parent;
        this->obj_size = rhs.obj_size;
        this->size = rhs.size;
        this->def = rhs.def;
        this->is_copy = TRUE;
        this->array_depth = rhs.array_depth;
        this->array_type = rhs.array_type;
        // SAFE_ADD_REF(this->array_type);
        this->func = rhs.func;
        // SAFE_ADD_REF(this->func);
        this->info = rhs.info;
        SAFE_ADD_REF(this->info);
        this->owner = rhs.owner;
        // SAFE_ADD_REF(this->owner);

        return *this;
    }

    // copy
    Chuck_Type * copy( Chuck_Env * env ) const
    { Chuck_Type * n = env->context->new_Chuck_Type();
      *n = *this; return n; }
    
    // to string
    std::string ret;
    const std::string & str()
    { ret = name;
      for( t_CKUINT i = 0; i < array_depth; i++ ) ret += std::string("[]");
      return ret; }
    // to c
    const char * c_name()
    { return str().c_str(); }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Value
// desc: a variable in scope
//-----------------------------------------------------------------------------
struct Chuck_Value : public Chuck_VM_Object
{
    // type
    Chuck_Type * type;
    // name
    std::string name;
    // offset
    t_CKUINT offset;
    // addr
    void * addr;
    // const?
    t_CKBOOL is_const;
    // member?
    t_CKBOOL is_member;
    // static?
    t_CKBOOL is_static;  // do something
    // is context-global?
    t_CKBOOL is_context_global;
    // is decl checked
    t_CKBOOL is_decl_checked;
    // 0 = public, 1 = protected, 2 = private
    t_CKUINT access;
    // owner
    Chuck_Namespace * owner;
    // owner (class)
    Chuck_Type * owner_class;
    // remember function pointer - if this is a function
    Chuck_Func * func_ref;
    // overloads
    t_CKINT func_num_overloads;
	
    // documentation
    std::string doc;

    // constructor
    Chuck_Value( Chuck_Type * t, const std::string & n, void * a = NULL,
                 t_CKBOOL c = FALSE, t_CKBOOL acc = 0, Chuck_Namespace * o = NULL,
                 Chuck_Type * oc = NULL, t_CKUINT s = 0 )
    { type = t; SAFE_ADD_REF(type); // add reference
      name = n; offset = s; 
      is_const = c; access = acc;
      owner = o; SAFE_ADD_REF(o); // add reference
      owner_class = oc; SAFE_ADD_REF(oc); // add reference
      addr = a; is_member = FALSE;
      is_static = FALSE; is_context_global = FALSE;
      is_decl_checked = TRUE; // only set to false in certain cases
      func_ref = NULL; func_num_overloads = 0; }

    // destructor
    virtual ~Chuck_Value()
    {
        // release
        // SAFE_RELEASE( type );
        // SAFE_RELEASE( owner ):
        // SAFE_RELEASE( owner_class );
        // SAFE_RELEASE( func_ref );
    }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Func
// desc: function definition
//-----------------------------------------------------------------------------
struct Chuck_Func : public Chuck_VM_Object
{
    // name
    std::string name;
    // func def from parser
    a_Func_Def def;
    // code
    Chuck_VM_Code * code;
    // imported code
    Chuck_DL_Func * dl_code;
    // member
    t_CKBOOL is_member;
    // virtual table index
    t_CKUINT vt_index;
    // rember value
    Chuck_Value * value_ref;
    // for overloading
    Chuck_Func * next;
    // for overriding
    Chuck_Value * up;
	
    // documentation
    std::string doc;

    // constructor
    Chuck_Func() { def = NULL; code = NULL; is_member = FALSE; vt_index = 0xffffffff; 
                   value_ref = NULL; dl_code = NULL; next = NULL; up = NULL; }

    // destructor
    virtual ~Chuck_Func()
    { }
};




//-----------------------------------------------------------------------------
// primary chuck type checker interface
//-----------------------------------------------------------------------------
// initialize the type engine
Chuck_Env * type_engine_init( Chuck_VM * vm );
// shutdown the type engine
void type_engine_shutdown( Chuck_Env * env );
// load a context to be type-checked or emitted
t_CKBOOL type_engine_load_context( Chuck_Env * env, Chuck_Context * context );
// unload a context after being emitted
t_CKBOOL type_engine_unload_context( Chuck_Env * env );

// type check a program into the env
t_CKBOOL type_engine_check_prog( Chuck_Env * env, a_Program prog, 
                                 const std::string & filename );
// make a context
Chuck_Context * type_engine_make_context( a_Program prog,
                                          const std::string & filename );
// type check a context into the env
t_CKBOOL type_engine_check_context( Chuck_Env * env,
                                    Chuck_Context * context,
                                    te_HowMuch how_much = te_do_all );
// type check a statement
t_CKBOOL type_engine_check_stmt( Chuck_Env * env, a_Stmt stmt );
// type check an expression
t_CKTYPE type_engine_check_exp( Chuck_Env * env, a_Exp exp );
// add an chuck dll into the env
t_CKBOOL type_engine_add_dll( Chuck_Env * env, Chuck_DLL * dll, const std::string & nspc );
// second version: use type_engine functions instead of constructing AST (added 1.3.0.0)
t_CKBOOL type_engine_add_dll2( Chuck_Env * env, Chuck_DLL * dll, const std::string & dest );
// import class based on Chuck_DL_Class (added 1.3.2.0)
t_CKBOOL type_engine_add_class_from_dl( Chuck_Env * env, Chuck_DL_Class * c );
// type equality
t_CKBOOL operator ==( const Chuck_Type & lhs, const Chuck_Type & rhs );
t_CKBOOL operator !=( const Chuck_Type & lhs, const Chuck_Type & rhs );
t_CKBOOL equals( Chuck_Type * lhs, Chuck_Type * rhs );
t_CKBOOL operator <=( const Chuck_Type & lhs, const Chuck_Type & rhs );
t_CKBOOL isa( Chuck_Type * lhs, Chuck_Type * rhs );
t_CKBOOL isprim( Chuck_Type * type );
t_CKBOOL isobj( Chuck_Type * type );
t_CKBOOL isfunc( Chuck_Type * type );
t_CKBOOL iskindofint( Chuck_Type * type ); // added 1.3.1.0: this includes int + pointers
t_CKUINT getkindof( Chuck_Type * type ); // added 1.3.1.0: to get the kindof a type

// import
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, Chuck_Type * type, 
                                             Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor = NULL,
                                             const char * doc = NULL );
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, const char * name, const char * parent,
                                             Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor = NULL,
                                             const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tickf tickf, f_pmsg pmsg,  // (tickf added 1.3.0.0)
                                            t_CKUINT num_ins = 0xffffffff, t_CKUINT num_outs = 0xffffffff,
                                            const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_pmsg pmsg,
                                            t_CKUINT num_ins = 0xffffffff, t_CKUINT num_outs = 0xffffffff,
                                            const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_pmsg pmsg,  const char * doc );
Chuck_Type * type_engine_import_uana_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tock tock, f_pmsg pmsg,
                                            t_CKUINT num_ins = 0xffffffff, t_CKUINT num_outs = 0xffffffff,
                                            t_CKUINT num_ins_ana = 0xffffffff, t_CKUINT num_outs_ana = 0xffffffff,
                                            const char * doc = NULL );
t_CKBOOL type_engine_import_mfun( Chuck_Env * env, Chuck_DL_Func * mfun );
t_CKBOOL type_engine_import_sfun( Chuck_Env * env, Chuck_DL_Func * sfun );
t_CKUINT type_engine_import_mvar( Chuck_Env * env, const char * type, 
                                  const char * name, t_CKUINT is_const,
                                  const char * doc = NULL );
t_CKBOOL type_engine_import_svar( Chuck_Env * env, const char * type,
                                  const char * name, t_CKUINT is_const,
                                  t_CKUINT addr, const char * doc = NULL );
t_CKBOOL type_engine_import_ugen_ctrl( Chuck_Env * env, const char * type, const char * name,
                                       f_ctrl ctrl, t_CKBOOL write, t_CKBOOL read );
t_CKBOOL type_engine_import_add_ex( Chuck_Env * env, const char * ex );
t_CKBOOL type_engine_import_class_end( Chuck_Env * env );
t_CKBOOL type_engine_register_deprecate( Chuck_Env * env, 
                                         const std::string & former, const std::string & latter );

// helpers
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, const std::string & xid, int pos );
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, S_Symbol xid, int pos );
t_CKBOOL type_engine_check_primitive( Chuck_Type * type );
t_CKBOOL type_engine_compat_func( a_Func_Def lhs, a_Func_Def rhs, int pos, std::string & err, t_CKBOOL print = TRUE );
t_CKBOOL type_engine_get_deprecate( Chuck_Env * env, const std::string & from, std::string & to );
Chuck_Type  * type_engine_find_common_anc( Chuck_Type * lhs, Chuck_Type * rhs );
Chuck_Type  * type_engine_find_type( Chuck_Env * env, a_Id_List path );
Chuck_Value * type_engine_find_value( Chuck_Type * type, const std::string & xid );
Chuck_Value * type_engine_find_value( Chuck_Type * type, S_Symbol xid );
Chuck_Value * type_engine_find_value( Chuck_Env * env, const std::string & xid, t_CKBOOL climb, int linepos = 0 );
Chuck_Namespace * type_engine_find_nspc( Chuck_Env * env, a_Id_List path );
/*******************************************************************************
 * spencer: added this into function to provide the same logic path
 * for type_engine_check_exp_decl() and ck_add_mvar() when they determine
 * offsets for mvars -- added 1.3.0.0
 ******************************************************************************/
t_CKUINT type_engine_next_offset( t_CKUINT current_offset, Chuck_Type * type );
// array verify
t_CKBOOL verify_array( a_Array_Sub array );
// make array type
Chuck_Type * new_array_type( Chuck_Env * env, Chuck_Type * array_parent,
                             t_CKUINT depth, Chuck_Type * base_type,
                             Chuck_Namespace * owner_nspc );
// conversion
const char * type_path( a_Id_List path );
a_Id_List str2list( const std::string & path );
a_Id_List str2list( const std::string & path, t_CKBOOL & is_array );
const char * howmuch2str( te_HowMuch how_much );
t_CKBOOL escape_str( char * str_lit, int linepos );
t_CKINT str2char( const char * char_lit, int linepos );

// default types
extern Chuck_Type t_void;
extern Chuck_Type t_int;
extern Chuck_Type t_float;
extern Chuck_Type t_time;
extern Chuck_Type t_dur;
extern Chuck_Type t_complex;
extern Chuck_Type t_polar;
extern Chuck_Type t_object;
extern Chuck_Type t_null;
extern Chuck_Type t_string;
extern Chuck_Type t_array;
extern Chuck_Type t_shred;
extern Chuck_Type t_thread;
extern Chuck_Type t_function;
extern Chuck_Type t_class;
extern Chuck_Type t_event;
extern Chuck_Type t_io;
extern Chuck_Type t_fileio;
extern Chuck_Type t_chout;
extern Chuck_Type t_cherr;
extern Chuck_Type t_ugen;
extern Chuck_Type t_uana;
extern Chuck_Type t_uanablob;




#endif
