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
    te_none = 0, te_int, te_uint, te_single, te_float, te_double, te_time, te_dur,
    te_complex, te_polar, te_string, te_thread, te_shred, te_class,
    te_function, te_object, te_user, te_array, te_null, te_ugen, te_uana,
    te_event, te_void, te_stdout, te_stderr, te_adc, te_dac, te_bunghole,
    te_uanablob, te_io, te_fileio, te_chout, te_cherr, te_multi,
    te_vec2, te_vec3, te_vec4, te_vector, te_auto
} te_Type;




//-----------------------------------------------------------------------------
// name: enum te_GlobalType
// desc: ChucK types for global vars: int, float, (subclass of) Event
//       (REFACTOR-2017)
//-----------------------------------------------------------------------------
typedef enum { te_globalTypeNone,
    te_globalInt, te_globalFloat, te_globalString, te_globalEvent,
    te_globalUGen, te_globalObject,
    // symbol: not used for declarations, only for later lookups :/
    te_globalArraySymbol
} te_GlobalType;




//-----------------------------------------------------------------------------
// name: enum te_HowMuch
// desc: how much to scan/type check
//-----------------------------------------------------------------------------
typedef enum {
    te_do_all = 0, te_do_classes_only, te_do_no_classes
} te_HowMuch;




//-----------------------------------------------------------------------------
// name: enum te_Origin | 1.5.0.0 (ge) added
// desc: where something (e.g., a Type) originates
//-----------------------------------------------------------------------------
typedef enum {
    te_originUnknown = 0,
    te_originBuiltin, // in core
    te_originChugin, // in imported chugin
    te_originImport, // library CK code
    te_originUserDefined, // in user chuck code
    te_originGenerated // generated (e.g., array types like int[][][][])
} te_Origin;




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
        CK_SAFE_ADD_REF(value);
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
            {
                val = (*scope[i - 1])[xid];
                if( val ) break;
            }

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

    // test if a name has been "mangled", e.g., "toString@0@Object"
    static t_CKBOOL is_mangled( const std::string & name )
    {
        // check for @ in the name, which would not be possible for names in language
        return ( name.find("@") != std::string::npos );
    }

    // get list of top level
    void get_toplevel( std::vector<Chuck_VM_Object *> & out, t_CKBOOL includeMangled = TRUE )
    {
        // pass on
        get_level( 0, out, includeMangled );
    }

    // get list of top level
    void get_level( int level, std::vector<Chuck_VM_Object *> & out, t_CKBOOL includeMangled = TRUE )
    {
        assert( scope.size() > level );
        // clear the out
        out.clear();

        // our iterator
        std::map<S_Symbol, Chuck_VM_Object *>::iterator iter;
        // get func map
        std::map<S_Symbol, Chuck_VM_Object *> * m = NULL;

        // 1.5.0.5 (ge) modified: actually use level
        m = scope[level];
        // go through map
        for( iter = m->begin(); iter != m->end(); iter++ )
        {
            // check mangled name
            if( !includeMangled && is_mangled( std::string(S_name((*iter).first))) )
                continue;
            // add
            out.push_back( (*iter).second );
        }

        // if level 0 then also include commit map | 1.5.0.5 (ge) added
        if( level == 0 )
        {
            m = &commit_map;
            // go through map
            for( iter = m->begin(); iter != m->end(); iter++ )
            {
                // check mangled name
                if( !includeMangled && is_mangled( std::string(S_name((*iter).first))) )
                    continue;
                // add
                out.push_back( (*iter).second );
            }
        }
    }

protected:
    std::vector<std::map<S_Symbol, Chuck_VM_Object *> *> scope;
    std::map<S_Symbol, Chuck_VM_Object *> commit_map;
};




//-----------------------------------------------------------------------------
// forward references
//-----------------------------------------------------------------------------
struct Chuck_Type;
struct Chuck_Value;
struct Chuck_Func;
struct Chuck_Multi;
struct Chuck_VM;
struct Chuck_VM_Code;
struct Chuck_VM_MFunInvoker;
struct Chuck_DLL;
// operator loading structs | 1.5.1.5
struct Chuck_Op_Registry;
struct Chuck_Op_Semantics;
struct Chuck_Op_Overload;




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
        /* TODO: CK_SAFE_RELEASE( this->parent ); */
        /* TODO: CK_SAFE_DELETE( pre_ctor ); */
        /* TODO: CK_SAFE_DELETE( dtor ); */
    }

    // look up value
    Chuck_Value * lookup_value( const std::string & name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );
    Chuck_Value * lookup_value( S_Symbol name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );
    // look up type
    Chuck_Type * lookup_type( const std::string & name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );
    Chuck_Type * lookup_type( S_Symbol name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );
    // look up func
    Chuck_Func * lookup_func( const std::string & name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );
    Chuck_Func * lookup_func( S_Symbol name, t_CKINT climb = 1, t_CKBOOL stayWithinClassDef = FALSE );

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
    void get_funcs( std::vector<Chuck_Func *> & out, t_CKBOOL includeManged = TRUE );
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
    // context namespace
    Chuck_Namespace * nspc;
    // error - means to free nspc too
    t_CKBOOL has_error;

    // AST parse tree (does not persist past context unloading)
    a_Program parse_tree;
    // AST public class def if any (does not persist past context unloading)
    a_Class_Def public_class_def;

    // progress
    enum { P_NONE = 0, P_CLASSES_ONLY, P_ALL_DONE };
    // progress in scan / type check / emit
    t_CKUINT progress;

    // things to release with the context
    std::vector<Chuck_VM_Object *> new_types;
    std::vector<Chuck_VM_Object *> new_values;
    std::vector<Chuck_VM_Object *> new_funcs;
    std::vector<Chuck_VM_Object *> new_nspc;

public:
    // decouple from AST (called when a context is compiled)
    // called when a context is finished and being unloaded | 1.5.0.5 (ge)
    void decouple_ast();

public:
    // constructor
    Chuck_Context() { parse_tree = NULL; nspc = new Chuck_Namespace;
                      public_class_def = NULL; has_error = FALSE;
                      progress = P_NONE; }
    // destructor
    virtual ~Chuck_Context();

public:
    // get the top-level code
    Chuck_VM_Code * code() { return nspc->pre_ctor; }

    // special alloc
    Chuck_Type * new_Chuck_Type( Chuck_Env * env );
    Chuck_Value * new_Chuck_Value( Chuck_Type * t, const std::string & name );
    Chuck_Func * new_Chuck_Func();
    Chuck_Namespace * new_Chuck_Namespace();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Op_Registry | 1.5.1.5 (ge) added
// desc: operator overloading registry
//-----------------------------------------------------------------------------
struct Chuck_Op_Registry
{
public:
    // constructor
    Chuck_Op_Registry();
    // destructor
    ~Chuck_Op_Registry();

public:
    // add semantics for particular operator
    Chuck_Op_Semantics * add( ae_Operator op );
    // get semantics for particular operator
    Chuck_Op_Semantics * lookup( ae_Operator op );
    // can overload as binary?
    t_CKBOOL binaryOverloadable( ae_Operator op );
    // can overload as unary prefix?
    t_CKBOOL unaryPreOverloadable( ae_Operator op );
    // can overload as unary postfix?
    t_CKBOOL unaryPostOverloadable( ae_Operator op );

public:
    // push registry stack; mark all unmarked overloads with current ID
    // returns pushID associated with this push
    t_CKUINT push();
    // remove all overload greater than previous pushID
    // return how many levels were popped (1 or 0)
    t_CKUINT pop();
    // reset pop local overload state (everything above publicID)
    void reset2local();
    // reset pop beyond public state (everything above preserveID)
    void reset2public();
    // (system) preserve current state (cannot be popped normally)
    void preserve();
    // (system) remove preserve status (allowing pops for all stack levels)
    void unpreserve();
    // get the current stack ID
    t_CKUINT stackLevel() const { return m_stackID; }

public:
    // reserve builtin overloads
    void reserve( Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs, t_CKBOOL commute = FALSE );
    // reserve prefix
    void reserve( ae_Operator op, Chuck_Type * type );
    // reserve postfix
    void reserve( Chuck_Type * type, ae_Operator op );

    // add binary operator overload: lhs OP rhs
    t_CKBOOL add_overload( Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs, Chuck_Func * func, 
                           te_Origin origin, const std::string & originName = "", t_CKINT originWhere = 0, t_CKBOOL isPublic = FALSE );
    // add prefix unary operator overload: OP rhs
    t_CKBOOL add_overload( ae_Operator op, Chuck_Type * rhs, Chuck_Func * func,
                           te_Origin origin, const std::string & originName = "", t_CKINT originWhere = 0, t_CKBOOL isPublic = FALSE );
    // add postfix unary operator overload: lhs OP
    t_CKBOOL add_overload( Chuck_Type * lhs, ae_Operator op, Chuck_Func * func,
                           te_Origin origin, const std::string & originName = "", t_CKINT originWhere = 0, t_CKBOOL isPublic = FALSE );

    // look up binary operator overload: lhs OP rhs
    Chuck_Op_Overload * lookup_overload( Chuck_Type * lhs, ae_Operator op, Chuck_Type * rhs );
    // look up prefix unary operator overload: OP rhs
    Chuck_Op_Overload * lookup_overload( ae_Operator op, Chuck_Type * rhs );
    // look up postfix unary operator overload: lhs OP
    Chuck_Op_Overload * lookup_overload( Chuck_Type * lhs, ae_Operator op );

protected:
    // remove all overload greater than pushID
    // return how many levels were popped
    t_CKUINT pop( t_CKUINT pushID );
    // map of operator to its semantic mappings
    std::map<ae_Operator, Chuck_Op_Semantics *> m_operatorMap;
    // operator overload stack level
    t_CKUINT m_stackID;
    // system-level operator preservation ID (cannot be reset; use unpreserve to undo)
    t_CKUINT m_stackPreserveID;

public:
    // user-level public operator overload (can be reset)
    static const t_CKUINT STACK_PUBLIC_ID;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_TypePair | 1.5.1.5 (ge) added
// desc: a type pair, used as key in operator overload map
//-----------------------------------------------------------------------------
struct Chuck_TypePair
{
    // left hand side type
    Chuck_Type * lhs;
    // right hand side type
    Chuck_Type * rhs;

    // constructor
    Chuck_TypePair( Chuck_Type * LHS = NULL, Chuck_Type * RHS = NULL ) : lhs(LHS), rhs(RHS) { }
    // copy constructor
    Chuck_TypePair( const Chuck_TypePair & other ) : lhs(other.lhs), rhs(other.rhs) { }
    // operator
    bool operator <( const Chuck_TypePair & other ) const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Op_Semantics | 1.5.1.5 (ge) added
// desc: all overloading information for a particualr operator
//-----------------------------------------------------------------------------
struct Chuck_Op_Semantics
{
protected:
    // the operator
    ae_Operator m_op;
    // is binary overloadable?
    bool is_overloadable_binary;
    // is unary prefix overloadable?
    bool is_overloadable_unary_pre;
    // is unary postfix overloadable?
    bool is_overloadable_unary_post;
    // binary overload map
    std::map<Chuck_TypePair, Chuck_Op_Overload *> overloads;

public:
    // constructor
    Chuck_Op_Semantics( ae_Operator op = ae_op_none );
    // destructor
    ~Chuck_Op_Semantics();

public:
    // configure how operator could be overloaded
    void configure( bool binary_OL, bool unary_pre_OL, bool unary_post_OL );
    // is binary overloadable
    bool isBinaryOL() const { return is_overloadable_binary; }
    bool isUnaryPreOL() const { return is_overloadable_unary_pre; }
    bool isUnaryPostOL() const { return is_overloadable_unary_post; }

public:
    // add overload
    void add( Chuck_Op_Overload * overload );
    // remove overloads with mark > pushID
    void removeAbove( t_CKUINT pushID );
    // squash all overloads marks to pushID
    void squashTo( t_CKUINT pushID );
    // retrieve all overloads for an operator
    void getOverloads( std::vector<const Chuck_Op_Overload *> & results );
    // get entry by types
    Chuck_Op_Overload * getOverload( Chuck_Type * lhs, Chuck_Type * rhs );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Op_Overload | 1.5.1.5 (ge) added
// desc: a particular overloading
//-----------------------------------------------------------------------------
struct Chuck_Op_Overload
{
    // where this overload was defined
    te_Origin m_origin;
    // origin name
    std::string m_originName;
    // origin parse position (if applicable)
    t_CKINT m_originWhere;

protected:
    // the operator
    ae_Operator m_op;
    // which kind of overload?
    te_Op_OverloadKind m_kind;
    // the function to call
    Chuck_Func * m_func;
    // left hand side
    Chuck_Type * m_lhs;
    // right hand side
    Chuck_Type * m_rhs;
    // ID associated with when this overload was created
    t_CKUINT m_pushID;
    // is reserved
    t_CKBOOL m_isReserved;

    // set left hand side type
    void setLHS( Chuck_Type * type );
    // set right hand side type
    void setRHS( Chuck_Type * type );
    // zero out
    void zero();

public:
    // constructor: binary
    Chuck_Op_Overload( Chuck_Type * LHS, ae_Operator op, Chuck_Type * RHS, Chuck_Func * func );
    // constructor: postfix
    Chuck_Op_Overload( Chuck_Type * LHS, ae_Operator op, Chuck_Func * func );
    // constructor: prefix
    Chuck_Op_Overload( ae_Operator op, Chuck_Type * RHS, Chuck_Func * func );
    // copy constructor
    Chuck_Op_Overload( const Chuck_Op_Overload & other );
    // destructor
    ~Chuck_Op_Overload();
    // operator < for overload
    bool operator <( const Chuck_Op_Overload & other ) const;

    // update origin info
    void updateOrigin( te_Origin origin, const std::string & name = "", t_CKINT where = 0 );
    // update overload stack push ID
    void mark( t_CKUINT pushID );
    // set as reserved
    void updateReserved( t_CKBOOL isReserved ) { m_isReserved = isReserved; }
    // get is reserved
    t_CKBOOL reserved() const { return m_isReserved; }

public:
    // get op
    ae_Operator op() const { return m_op; }
    // get the kind of overload
    te_Op_OverloadKind kind() const { return m_kind; }
    // get func
    Chuck_Func * func() const { return m_func; }
    // get left hand side type
    Chuck_Type * lhs() const { return m_lhs; }
    // get right hand side type
    Chuck_Type * rhs() const { return m_rhs; }
    // get overload stack push ID
    t_CKUINT pushID() const { return m_pushID; }
    // has origin been set?
    t_CKBOOL hasOrigin() const { return m_origin != te_originUnknown; }
    // overloading natively handled? (e.g., in chuck_type)
    t_CKBOOL isNative() const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Env
// desc: chuck type environment; one per VM instance
//-----------------------------------------------------------------------------
struct Chuck_Env : public Chuck_VM_Object
{
public:
    // constructor
    Chuck_Env();
    // destructor
    virtual ~Chuck_Env();

public:
    // initialize
    t_CKBOOL init();
    // cleanup
    void cleanup();

    // reset the Env
    void reset();
    // load user namespace
    void load_user_namespace();
    // clear user namespace
    void clear_user_namespace();
    // check whether the context is the global context
    t_CKBOOL is_global();
    // global namespace
    Chuck_Namespace * global();
    // user namespace, if there is one (if not, return global)
    Chuck_Namespace * user();
    // get namespace at top of stack
    Chuck_Namespace * nspc_top();
    // get type at top of type stack
    Chuck_Type * class_top();

public:
    // REFACTOR-2017: carrier and accessors
    void set_carrier( Chuck_Carrier * carrier ) { m_carrier = carrier; }
    Chuck_VM * vm() { return m_carrier ? m_carrier->vm : NULL; }
    Chuck_Compiler * compiler() { return m_carrier ? m_carrier->compiler : NULL; }

protected:
    Chuck_Carrier * m_carrier;

protected:
    // global namespace
    Chuck_Namespace * global_nspc;
    // global context
    Chuck_Context global_context;
    // user-global namespace
    Chuck_Namespace * user_nspc;

public:
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
    // are we in a spork operation
    t_CKBOOL sporking;

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

    // operators mapping registry | 1.5.1.5
    Chuck_Op_Registry op_registry;

    // deprecated types
    std::map<std::string, std::string> deprecated;
    // level - 0:stop, 1:warn, 2:ignore
    t_CKINT deprecate_level;

public:
    // REFACTOR-2017: public types
    Chuck_Type * ckt_void;
    Chuck_Type * ckt_auto; // 1.5.0.8
    Chuck_Type * ckt_int;
    Chuck_Type * ckt_float;
    Chuck_Type * ckt_time;
    Chuck_Type * ckt_dur;
    Chuck_Type * ckt_complex;
    Chuck_Type * ckt_polar;
    Chuck_Type * ckt_vec2;
    Chuck_Type * ckt_vec3;
    Chuck_Type * ckt_vec4;
    Chuck_Type * ckt_null;
    Chuck_Type * ckt_function;
    Chuck_Type * ckt_object;
    Chuck_Type * ckt_array;
    Chuck_Type * ckt_string;
    Chuck_Type * ckt_event;
    Chuck_Type * ckt_ugen;
    Chuck_Type * ckt_uana;
    Chuck_Type * ckt_uanablob;
    Chuck_Type * ckt_shred;
    Chuck_Type * ckt_io;
    Chuck_Type * ckt_fileio;
    Chuck_Type * ckt_chout;
    Chuck_Type * ckt_cherr;
    Chuck_Type * ckt_class;
    Chuck_Type * ckt_dac;
    Chuck_Type * ckt_adc;

    // Chuck_Type * ckt_thread;
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
// name: struct Chuck_Value_Dependency
// desc: a value dependency records when accessing a value; this is used to
//       for dependency tracking of file-top-level and class-top-level variables
//       accessed from within functions or class pre-constructor code;
//
// example:
// ```
// 5 => int foo;
// fun void bar()
// {
//     // below is a dependency to foo
//     <<< foo >>>;
// }
// ```
//-----------------------------------------------------------------------------
struct Chuck_Value_Dependency
{
    // value we are tracking
    Chuck_Value * value;
    // code position of dependency
    t_CKUINT where;
    // position where the use occurs (from within func or class)
    t_CKUINT use_where;

public:
    // default
    Chuck_Value_Dependency() : value(NULL), where(0), use_where(0) { }
    // constructor
    Chuck_Value_Dependency( Chuck_Value * argValue, t_CKUINT argUseWhere = 0 );
    // copy constructor | NOTE: needed to properly ref-count
    Chuck_Value_Dependency( const Chuck_Value_Dependency & rhs );
    // destructor
    virtual ~Chuck_Value_Dependency();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Value_Dependency_Graph
// desc: data structure of value dependencies, direct and remote
//-----------------------------------------------------------------------------
struct Chuck_Value_Dependency_Graph
{
public:
    // add a direct dependency
    void add( const Chuck_Value_Dependency & dep );
    // add a remote (recursive) dependency
    void add( Chuck_Value_Dependency_Graph * graph );
    // clear all dependencies | to be called when all dependencies are met
    // for example, at the successful compilation of a context (e.g., a file)
    // after this, calls to locate() will return NULL, indicating no dependencies
    // NOTE dependency analysis is for within-context only, and is not needed
    // across contexts (e.g., files) | 1.5.1.1 (ge) added
    void clear();
    // look for a dependency that occurs AFTER a particular code position
    // this function crawls the graph, taking care in the event of cycles
    const Chuck_Value_Dependency * locate( t_CKUINT pos, t_CKBOOL isClassDef = FALSE );

public:
    // constructor
    Chuck_Value_Dependency_Graph() : token(0) { }

protected:
    // search token, for cycle detection (not reentrant, thread-wise)
    t_CKUINT token;
    // vector of dependencies
    std::vector<Chuck_Value_Dependency> directs;
    // vector of recursive dependency graphs
    // take care regarding circular dependency
    std::vector<Chuck_Value_Dependency_Graph *> remotes;

protected:
    // locate non-recursive
    const Chuck_Value_Dependency * locateLocal( t_CKUINT pos, t_CKBOOL isClassDef );
    // reset search tokens
    void resetRecursive( t_CKUINT value = 0 );
    // locate recursive
    const Chuck_Value_Dependency * locateRecursive( t_CKUINT pos, t_CKBOOL isClassDef, t_CKUINT searchToken = 1 );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Type
// desc: class containing information about a type
// note: 1.5.0.0 (ge) Chuck_VM_Object -> Chuck_Object
//-----------------------------------------------------------------------------
struct Chuck_Type : public Chuck_Object
{
    // type id
    te_Type xid;
    // type name (FYI use this->str() for full name including []s for array)
    std::string base_name;
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
    // origin hint
    te_Origin originHint;

    // reference to environment RE-FACTOR 2017
    Chuck_Env * env_ref;

    // (within-context, e.g., a ck file) dependency tracking | 1.5.0.8
    Chuck_Value_Dependency_Graph depends;

    // documentation
    std::string doc;
    // example files
    std::vector<std::string> examples;

public:
    // constructor
    Chuck_Type( Chuck_Env * env,
                te_Type _id = te_null,
                const std::string & _n = "",
                Chuck_Type * _p = NULL,
                t_CKUINT _s = 0 );
    // destructor
    virtual ~Chuck_Type();
    // reset
    void reset();
    // assignment - this does not touch the Chuck_VM_Object
    const Chuck_Type & operator =( const Chuck_Type & rhs );
    // make a copy of this type struct
    Chuck_Type * copy( Chuck_Env * env, Chuck_Context * context ) const;

public:
    // to string: the full name of this type, e.g., "UGen" or "int[][]"
    const std::string & name();
    // to c string
    const char * c_name();

protected:
    // this for str() and c_name() use only
    std::string ret;

public: // apropos | 1.4.1.0 (ge)
    // generate info; output to console
    void apropos();
    // generate info; output to string
    void apropos( std::string & output );

public: // dump | 1.4.1.1 (ge)
    // generate object state; output to console
    void dump_obj( Chuck_Object * obj );
    // generate object state; output to string
    void dump_obj( Chuck_Object * obj, std::string & output );

protected: // apropos-related helper function
    // dump top level info
    void apropos_top( std::string & output, const std::string & prefix );
    // dump info about functions
    void apropos_funcs( std::string & output, const std::string & prefix, t_CKBOOL inherited );
    // dump info about vars
    void apropos_vars( std::string & output, const std::string & prefix, t_CKBOOL inherited );
    // dump info about examples
    void apropos_examples( std::string & output, const std::string & prefix );

public:
    // struct to hold callback on instantiate
    struct CallbackOnInstantiate
    {
        // whether to auto-set shred origin at instantiation;
        // see t_CKBOOL initialize_object( ... )
        t_CKBOOL shouldSetShredOrigin;
        // the callback
        f_callback_on_instantiate callback;
        // constructor
        CallbackOnInstantiate( f_callback_on_instantiate cb = NULL, t_CKBOOL setShredOrigin = FALSE )
        : callback(cb), shouldSetShredOrigin(setShredOrigin) { }
    };
    // register type instantiation callback
    void add_instantiate_cb( f_callback_on_instantiate cb, t_CKBOOL setShredOrigin );
    // unregister type instantiation callback
    void remove_instantiate_cb( f_callback_on_instantiate cb );
    // get vector of callbacks (including this and parents), return whether any requires setShredOrigin
    t_CKBOOL cbs_on_instantiate( std::vector<CallbackOnInstantiate> & results );

protected:
    // vector of callbacks on instantiation of this type (or its subclass)
    std::vector<CallbackOnInstantiate> m_cbs_on_instantiate;
    // internal get vector of callbacks (including this and parents), return whether any requires setShredOrigin
    t_CKBOOL do_cbs_on_instantiate( std::vector<CallbackOnInstantiate> & results );
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
    t_CKBOOL is_static; // do something
    // is context-global?
    t_CKBOOL is_context_global;
    // is decl checked
    t_CKBOOL is_decl_checked;
    // is global (added REFACTOR-2017)
    t_CKBOOL is_global;
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

    // dependency tracking | 1.5.0.8 (ge) added
    // code position of where this value is considered initialized
    // NOTE used to determine dependencies within a file context
    t_CKUINT depend_init_where; // 1.5.0.8

    // documentation
    std::string doc;

public:
    // constructor
    Chuck_Value( Chuck_Type * t, const std::string & n, void * a = NULL,
                 t_CKBOOL c = FALSE, t_CKBOOL acc = 0, Chuck_Namespace * o = NULL,
                 Chuck_Type * oc = NULL, t_CKUINT s = 0 );
    // destructor
    virtual ~Chuck_Value();
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Func
// desc: function definition
//-----------------------------------------------------------------------------
struct Chuck_Func : public Chuck_VM_Object
{
    // name (actual in VM name, e.g., "dump@0@Object")
    std::string name;
    // base name (without the designation, e.g., "dump"); 1.4.1.0
    std::string base_name;
    // get return type
    Chuck_Type * type() const;
    // human readable function signature: e.g., void Object.func( int foo, float bar[] );
    std::string signature( t_CKBOOL incFunDef = TRUE, t_CKBOOL incRetType = TRUE ) const;
    // code (included imported)
    Chuck_VM_Code * code;
    // member (inside class)
    t_CKBOOL is_member;
    // static (inside class)
    t_CKBOOL is_static;
    // virtual table index
    t_CKUINT vt_index;
    // remember value
    Chuck_Value * value_ref;
    // for overloading
    Chuck_Func * next;
    // for overriding
    Chuck_Value * up;

    // (within-context, e.g., a ck file) dependency tracking | 1.5.0.8
    Chuck_Value_Dependency_Graph depends;

    // documentation
    std::string doc;

public:
    // pack c-style array of DL_Args into args cache
    t_CKBOOL pack_cache( Chuck_DL_Arg * dlargs, t_CKUINT numArgs );
    // args cache (used by c++ to chuck function calls) | 1.5.1.5
    t_CKBYTE * args_cache;
    // size of args cache
    t_CKUINT args_cache_size;
    // setup invoker for this fun (for calling chuck function from c++)
    t_CKBOOL setup_invoker( t_CKUINT vtable_offet, Chuck_VM * vm, Chuck_VM_Shred * shred );
    // associate mfun invoker (if applicable)
    Chuck_VM_MFunInvoker * invoker_mfun;

protected:
    // AST func def from parser | 1.5.0.5 (ge) moved to protected
    // access through funcdef_*() functions
    a_Func_Def m_def;

public:
    // connect (called when this func is type checked)
    void funcdef_connect( a_Func_Def f );
    // severe references to AST (called after compilation, before AST cleanup)
    // make a fresh partial deep copy of m_def ONLY IF it is owned by AST
    void funcdef_decouple_ast();
    // cleanup funcdef (if/when this function is cleaned up)
    void funcdef_cleanup();

public:
    // get the func def (do not keep a reference to this...
    // as contents may shift during and after compilation)
    a_Func_Def def() const { return m_def; }

public:
    // constructor
    Chuck_Func()
    {
        m_def = NULL;
        code = NULL;
        is_member = FALSE;
        is_static = FALSE;
        vt_index = CK_NO_VALUE;
        value_ref = NULL;
        /*dl_code = NULL;*/
        next = NULL;
        up = NULL;
        args_cache = NULL;
        args_cache_size = 0;
        invoker_mfun = NULL;
    }

    // destructor
    virtual ~Chuck_Func();
};




//-----------------------------------------------------------------------------
// primary chuck type checker interface
//-----------------------------------------------------------------------------
// initialize the type engine
t_CKBOOL type_engine_init( Chuck_Carrier * carrier );
// shutdown the type engine
void type_engine_shutdown( Chuck_Carrier * carrier );
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
t_CKBOOL isprim( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL isobj( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL isfunc( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL isvoid( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL iskindofint( Chuck_Env * env, Chuck_Type * type ); // added 1.3.1.0: this includes int + pointers
te_KindOf getkindof( Chuck_Env * env, Chuck_Type * type ); // added 1.3.1.0: to get the kindof a type


//-----------------------------------------------------------------------------
// import
//-----------------------------------------------------------------------------
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, Chuck_Type * type,
                                             Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor = NULL,
                                             const char * doc = NULL );
Chuck_Type * type_engine_import_class_begin( Chuck_Env * env, const char * name, const char * parent,
                                             Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor = NULL,
                                             const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tickf tickf, f_pmsg pmsg,  // (tickf added 1.3.0.0)
                                            t_CKUINT num_ins = CK_NO_VALUE,
                                            t_CKUINT num_outs = CK_NO_VALUE,
                                            const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_pmsg pmsg,
                                            t_CKUINT num_ins = CK_NO_VALUE,
                                            t_CKUINT num_outs = CK_NO_VALUE,
                                            const char * doc = NULL );
Chuck_Type * type_engine_import_ugen_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_pmsg pmsg,  const char * doc );
Chuck_Type * type_engine_import_uana_begin( Chuck_Env * env, const char * name, const char * parent,
                                            Chuck_Namespace * where, f_ctor pre_ctor, f_dtor dtor,
                                            f_tick tick, f_tock tock, f_pmsg pmsg,
                                            t_CKUINT num_ins = CK_NO_VALUE,
                                            t_CKUINT num_outs = CK_NO_VALUE,
                                            t_CKUINT num_ins_ana = CK_NO_VALUE,
                                            t_CKUINT num_outs_ana = CK_NO_VALUE,
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
// add global operator overload | 1.5.1.5 (ge & andrew) chaos
t_CKBOOL type_engine_import_op_overload( Chuck_Env * env, Chuck_DL_Func * func );
t_CKBOOL type_engine_register_deprecate( Chuck_Env * env,
                                         const std::string & former, const std::string & latter );


//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, const std::string & xid, int pos );
t_CKBOOL type_engine_check_reserved( Chuck_Env * env, S_Symbol xid, int pos );
// 1.4.1.0 (ge): abilty to toggle reserved words for special cases, such as Math.pi co-existing with pi (use with care!)
t_CKVOID type_engine_enable_reserved( Chuck_Env * env, const std::string & xid, t_CKBOOL value );
t_CKBOOL type_engine_check_primitive( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL type_engine_check_const( Chuck_Env * env, a_Exp e, int pos ); // TODO
t_CKBOOL type_engine_compat_func( a_Func_Def lhs, a_Func_Def rhs, int pos, std::string & err, t_CKBOOL print = TRUE );
t_CKBOOL type_engine_get_deprecate( Chuck_Env * env, const std::string & from, std::string & to );
t_CKBOOL type_engine_is_base_static( Chuck_Env * env, Chuck_Type * baseType ); // 1.5.0.0 (ge) added
Chuck_Type  * type_engine_find_common_anc( Chuck_Type * lhs, Chuck_Type * rhs );
Chuck_Type  * type_engine_find_type( Chuck_Env * env, a_Id_List path );
Chuck_Type  * type_engine_find_type( Chuck_Env * env, const std::string & name ); // 1.5.0.0 (ge) added
Chuck_Value * type_engine_find_value( Chuck_Type * type, const std::string & xid );
Chuck_Value * type_engine_find_value( Chuck_Type * type, S_Symbol xid );
Chuck_Value * type_engine_find_value( Chuck_Env * env, const std::string & xid, t_CKBOOL climb, t_CKBOOL stayWithClassDef = FALSE, int linepos = 0 );
Chuck_Namespace * type_engine_find_nspc( Chuck_Env * env, a_Id_List path );
// convert a vector of type names to a vector of Types | 1.5.0.0 (ge) added
void type_engine_names2types( Chuck_Env * env, const std::vector<std::string> & typeNames, std::vector<Chuck_Type *> & types );
// check and process auto types | 1.5.0.8 (ge) added
t_CKBOOL type_engine_infer_auto( Chuck_Env * env, a_Exp_Decl decl, Chuck_Type * type );
// initialize operator overload subsystem | 1.5.1.5 (ge) added
t_CKBOOL type_engine_init_op_overload( Chuck_Env * env );
// verify an operator overload | 1.5.1.5 (ge) added
t_CKBOOL type_engine_scan_func_op_overload( Chuck_Env * env, a_Func_Def func_def );
// type-check an operator overload func def | 1.5.1.5 (ge) added
t_CKBOOL type_engine_check_func_op_overload( Chuck_Env * env, a_Func_Def func_def );




//-----------------------------------------------------------------------------
// spencer: added this into function to provide the same logic path
// for type_engine_check_exp_decl() and ck_add_mvar() when they determine
// offsets for mvars -- added 1.3.0.0
//-----------------------------------------------------------------------------
t_CKUINT type_engine_next_offset( t_CKUINT current_offset, Chuck_Type * type );
// array verify
t_CKBOOL verify_array( a_Array_Sub array );
// make array type
Chuck_Type * new_array_type( Chuck_Env * env, Chuck_Type * array_parent,
                             t_CKUINT depth, Chuck_Type * base_type,
                             Chuck_Namespace * owner_nspc );
// make type | 1.4.1.1 (nshaheed) added
Chuck_Type * new_array_element_type( Chuck_Env * env, Chuck_Type * base_type,
                                     t_CKUINT depth, Chuck_Namespace * owner_nspc);


//-----------------------------------------------------------------------------
// conversion
//-----------------------------------------------------------------------------
const char * type_path( a_Id_List path );
a_Id_List str2list( const std::string & path );
a_Id_List str2list( const std::string & path, t_CKBOOL & is_array );
const char * howmuch2str( te_HowMuch how_much );
t_CKBOOL escape_str( char * str_lit, int linepos );
t_CKINT str2char( const char * char_lit, int linepos );


//-----------------------------------------------------------------------------
// more helper functions for type scan and checking | 1.5.0.0 (ge) added
//-----------------------------------------------------------------------------
// compare two argument lists to see if they are the same (sequence of types)
t_CKBOOL same_arg_lists( a_Arg_List lhs, a_Arg_List rhs );
// generate a string from an argument list (types only)
std::string arglist2string( a_Arg_List list );




#endif
