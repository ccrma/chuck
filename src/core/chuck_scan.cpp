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
// file: chuck_scan.cpp
// desc: chuck type-system / type-checker pre-scan
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Summer 2005 - original
//-----------------------------------------------------------------------------
#include "chuck_scan.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "util_string.h"

#include <string>
using namespace std;




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan0_class_def( Chuck_Env * env, a_Class_Def class_def );

t_CKBOOL type_engine_scan1_stmt_list( Chuck_Env * env, a_Stmt_List list, t_CKBOOL * retrieveAllControlPathsReturn = NULL );
t_CKBOOL type_engine_scan1_stmt( Chuck_Env * env, a_Stmt stmt );
t_CKBOOL type_engine_scan1_if( Chuck_Env * env, a_Stmt_If stmt );
t_CKBOOL type_engine_scan1_for( Chuck_Env * env, a_Stmt_For stmt );
t_CKBOOL type_engine_scan1_foreach( Chuck_Env * env, a_Stmt_ForEach stmt );
t_CKBOOL type_engine_scan1_while( Chuck_Env * env, a_Stmt_While stmt );
t_CKBOOL type_engine_scan1_until( Chuck_Env * env, a_Stmt_Until stmt );
t_CKBOOL type_engine_scan1_loop( Chuck_Env * env, a_Stmt_Loop stmt );
t_CKBOOL type_engine_scan1_break( Chuck_Env * env, a_Stmt_Break br );
t_CKBOOL type_engine_scan1_continue( Chuck_Env * env, a_Stmt_Continue cont );
t_CKBOOL type_engine_scan1_return( Chuck_Env * env, a_Stmt_Return stmt );
t_CKBOOL type_engine_scan1_switch( Chuck_Env * env, a_Stmt_Switch stmt );
t_CKBOOL type_engine_scan1_exp( Chuck_Env * env, a_Exp exp );
t_CKBOOL type_engine_scan1_exp_binary( Chuck_Env * env, a_Exp_Binary binary );
t_CKBOOL type_engine_scan1_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL type_engine_scan1_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL type_engine_scan1_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan1_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan1_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan1_exp_unary( Chuck_Env * env, a_Exp_Unary unary );
t_CKBOOL type_engine_scan1_exp_primary( Chuck_Env * env, a_Exp_Primary exp );
t_CKBOOL type_engine_scan1_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKBOOL type_engine_scan1_exp_cast( Chuck_Env * env, a_Exp_Cast cast );
t_CKBOOL type_engine_scan1_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix );
t_CKBOOL type_engine_scan1_exp_dur( Chuck_Env * env, a_Exp_Dur dur );
t_CKBOOL type_engine_scan1_exp_array( Chuck_Env * env, a_Exp_Array array );
t_CKBOOL type_engine_scan1_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call );
t_CKBOOL type_engine_scan1_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                          t_CKFUNC & ck_func, int linepos );
t_CKBOOL type_engine_scan1_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member );
t_CKBOOL type_engine_scan1_exp_if( Chuck_Env * env, a_Exp_If exp_if );
t_CKBOOL type_engine_scan1_exp_decl( Chuck_Env * env, a_Exp_Decl decl );
t_CKBOOL type_engine_scan1_array_subscripts( Chuck_Env * env, a_Exp exp_list );
t_CKBOOL type_engine_scan1_cast_valid( Chuck_Env * env, t_CKTYPE to, t_CKTYPE from );
t_CKBOOL type_engine_scan1_code_segment( Chuck_Env * env, a_Stmt_Code stmt, t_CKBOOL push = TRUE );
t_CKBOOL type_engine_scan1_func_def( Chuck_Env * env, a_Func_Def func_def );
t_CKBOOL type_engine_scan1_class_def( Chuck_Env * env, a_Class_Def class_def );

t_CKBOOL type_engine_scan2_stmt_list( Chuck_Env * env, a_Stmt_List list );
t_CKBOOL type_engine_scan2_stmt( Chuck_Env * env, a_Stmt stmt );
t_CKBOOL type_engine_scan2_if( Chuck_Env * env, a_Stmt_If stmt );
t_CKBOOL type_engine_scan2_for( Chuck_Env * env, a_Stmt_For stmt );
t_CKBOOL type_engine_scan2_foreach( Chuck_Env * env, a_Stmt_ForEach stmt );
t_CKBOOL type_engine_scan2_while( Chuck_Env * env, a_Stmt_While stmt );
t_CKBOOL type_engine_scan2_loop( Chuck_Env * env, a_Stmt_Loop stmt );
t_CKBOOL type_engine_scan2_until( Chuck_Env * env, a_Stmt_Until stmt );
t_CKBOOL type_engine_scan2_break( Chuck_Env * env, a_Stmt_Break br );
t_CKBOOL type_engine_scan2_continue( Chuck_Env * env, a_Stmt_Continue cont );
t_CKBOOL type_engine_scan2_return( Chuck_Env * env, a_Stmt_Return stmt );
t_CKBOOL type_engine_scan2_switch( Chuck_Env * env, a_Stmt_Switch stmt );
t_CKBOOL type_engine_scan2_exp( Chuck_Env * env, a_Exp exp );
t_CKBOOL type_engine_scan2_exp_binary( Chuck_Env * env, a_Exp_Binary binary );
t_CKBOOL type_engine_scan2_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL type_engine_scan2_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL type_engine_scan2_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan2_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan2_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs );
t_CKBOOL type_engine_scan2_exp_unary( Chuck_Env * env, a_Exp_Unary unary );
t_CKBOOL type_engine_scan2_exp_primary( Chuck_Env * env, a_Exp_Primary exp );
t_CKBOOL type_engine_scan2_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp );
t_CKBOOL type_engine_scan2_exp_cast( Chuck_Env * env, a_Exp_Cast cast );
t_CKBOOL type_engine_scan2_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix );
t_CKBOOL type_engine_scan2_exp_dur( Chuck_Env * env, a_Exp_Dur dur );
t_CKBOOL type_engine_scan2_exp_array( Chuck_Env * env, a_Exp_Array array );
t_CKBOOL type_engine_scan2_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call );
t_CKBOOL type_engine_scan2_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                          t_CKFUNC & ck_func, int linepos );
t_CKBOOL type_engine_scan2_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member );
t_CKBOOL type_engine_scan2_exp_if( Chuck_Env * env, a_Exp_If exp_if );
t_CKBOOL type_engine_scan2_exp_decl( Chuck_Env * env, a_Exp_Decl decl );
t_CKBOOL type_engine_scan2_array_subscripts( Chuck_Env * env, a_Exp exp_list );
t_CKBOOL type_engine_scan2_cast_valid( Chuck_Env * env, t_CKTYPE to, t_CKTYPE from );
t_CKBOOL type_engine_scan2_code_segment( Chuck_Env * env, a_Stmt_Code stmt, t_CKBOOL push = TRUE );
t_CKBOOL type_engine_scan2_func_def( Chuck_Env * env, a_Func_Def func_def );
t_CKBOOL type_engine_scan2_class_def( Chuck_Env * env, a_Class_Def class_def );




//-----------------------------------------------------------------------------
// name: type_engine_scan0_prog()
// desc: data in env should be ready : type discovery
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan0_prog( Chuck_Env * env, a_Program prog,
                                 te_HowMuch how_much )
{
    t_CKBOOL ret = TRUE;

    if( !prog )
        return FALSE;

    // log
    EM_log( CK_LOG_FINER, "(pass 0) type discovery scan..." );
                          // env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    // log how much
    EM_log( CK_LOG_FINEST, "target: %s", howmuch2str( how_much ) );

    // go through each of the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            // do nothing
            break;

        case ae_section_func:
            // do nothing
            break;

        case ae_section_class:
            // if no classes, then skip
            if( how_much == te_do_no_classes ) break;
            // make global, if marked public
            if( prog->section->class_def->decl == ae_key_public )
            {
                // make sure the context has no public class
                if( env->context->public_class_def != NULL )
                {
                    EM_error2( prog->section->class_def->where,
                        "more than one 'public' class defined..." );
                    ret = FALSE;
                    continue;
                }

                // make global
                prog->section->class_def->home = env->user();
                // remember
                env->context->public_class_def = prog->section->class_def;
            }
            // scan the class definition
            ret = type_engine_scan0_class_def( env, prog->section->class_def );
            break;

        default:
            EM_error2( prog->where,
                "(internal error) unrecognized program section in type checker pre-scan..." );
            ret = FALSE;
            break;
        }

        // next section
        prog = prog->next;
    }

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan0_class_def()
// desc: add defined class to env - this is the main goal of scan0
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan0_class_def( Chuck_Env * env, a_Class_Def class_def )
{
    // make new type for class def
    t_CKTYPE the_class = NULL;
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;

    // log
    EM_log( CK_LOG_FINER, "scanning class definition '%s'...",
        S_name(class_def->name->xid) );
    // push indent
    EM_pushlog();

    // if nspc is attached to class_def, that means the class_def is to be
    // put in that namespace.  this is usually the case when doing import
    if( class_def->home != NULL )
    {
        // log
        EM_log( CK_LOG_FINER, "target namespace: '%s'",
            class_def->home->name.c_str() );
        // set the new type as current
        env->nspc_stack.push_back( env->curr );
        env->curr = class_def->home;
    }

    // make sure class not already in namespace
    if( env->curr->lookup_type( class_def->name->xid, FALSE ) )
    {
        EM_error2( class_def->name->where,
            "class/type '%s' is already defined in namespace '%s'",
            S_name(class_def->name->xid), env->curr->name.c_str() );
        ret = FALSE; goto done;
    }

    // check if reserved
    if( type_engine_check_reserved( env, class_def->name->xid, class_def->name->where ) )
    {
        EM_error2( class_def->name->where, "...in class definition '%s'",
            S_name(class_def->name->xid) );
        ret = FALSE; goto done;
    }

    // allocate new type
    assert( env->context != NULL );
    the_class = env->context->new_Chuck_Type( env );
    // add reference
    CK_SAFE_ADD_REF( the_class );
    // set the fields
    the_class->xid = te_user;
    the_class->base_name = S_name(class_def->name->xid);
    the_class->owner = env->curr; CK_SAFE_ADD_REF(the_class->owner);
    the_class->array_depth = 0;
    the_class->size = sizeof(void *);
    the_class->obj_size = 0;  // TODO:
    the_class->info = env->context->new_Chuck_Namespace();
    CK_SAFE_ADD_REF( the_class->info );
    the_class->info->name = the_class->base_name;
    // if public class, then set parent to context
    // ... allowing the class to address current context
    if( env->context->public_class_def == class_def )
    { the_class->info->parent = env->context->nspc; }
    else { the_class->info->parent = env->curr; }
    // TODO: add ref to the parent?
    the_class->func = NULL;
    // 1.5.0.5 (ge) commented out; the AST is cleaned up after every compilation;
    // would need to make deep copy if want to keep around
    // the_class->def = class_def;
    // add code
    the_class->info->pre_ctor = new Chuck_VM_Code;
    CK_SAFE_ADD_REF( the_class->info->pre_ctor );
    // name | 1.5.2.0 (ge) added
    the_class->info->pre_ctor->name = string("class ") + the_class->base_name;
    // add to env
    env->curr->type.add( the_class->base_name, the_class );  // URGENT: make this global
    // incomplete
    the_class->is_complete = FALSE;

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
            break;

        case ae_section_func:
            break;

        case ae_section_class:
            // do the class
            ret = type_engine_scan0_class_def( env, body->section->class_def );
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

    // if things checked out
    if( ret )
    {
        Chuck_Value * value = NULL;
        Chuck_Type * type = NULL;

        // allocate value
        type = env->ckt_class->copy( env, env->context );
        type->actual_type = the_class;
        value = env->context->new_Chuck_Value( type, the_class->base_name );
        value->owner = env->curr; CK_SAFE_ADD_REF(value->owner);
        value->is_const = TRUE;
        value->is_member = FALSE;
        // add to env
        env->curr->value.add( the_class->base_name, value );

        // remember
        class_def->type = the_class;
    }

    // if nspc is attached to class_def, that means the class_def is to be
    // put in that namespace.  this is usually the case when doing import
    // we undo that extra namespace layer here...
    if( class_def->home != NULL )
    {
        // pop the namesapce
        env->curr = env->nspc_stack.back();
        env->nspc_stack.pop_back();
    }
    else // set the current namespace as home
    {
        // set curr as home
        class_def->home = env->curr;
    }

    // initialize the Type info object | 1.5.0.0 (ge) added
    initialize_object( the_class, env->ckt_class, NULL, env->vm() );

done:

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_prog()
// desc: data in env should be ready
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_prog( Chuck_Env * env, a_Program prog,
                                 te_HowMuch how_much )
{
    t_CKBOOL ret = TRUE;

    if( !prog )
        return FALSE;

    // log
    EM_log( CK_LOG_FINER, "(pass 1) type resolution scan..." );
                          // env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    // log how much
    EM_log( CK_LOG_FINEST, "target: %s", howmuch2str( how_much ) );

    // go through each of the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // scan the statements
            ret = type_engine_scan1_stmt_list( env, prog->section->stmt_list );
            break;

        case ae_section_func:
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // scan the function definition
            ret = type_engine_scan1_func_def( env, prog->section->func_def );
            break;

        case ae_section_class:
            // if no classes, then skip
            if( how_much == te_do_no_classes ) break;
            // scan the class definition
            ret = type_engine_scan1_class_def( env, prog->section->class_def );
            break;

        default:
            EM_error2( prog->where,
                "(internal error) unrecognized program section in type checker pre-scan..." );
            ret = FALSE;
            break;
        }

        // next section
        prog = prog->next;
    }

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_stmt_list()
// desc: check a statement list; retrieveAllControlPathsReturn is pointer
//       to retrieve whether the statement list control paths all return
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_stmt_list( Chuck_Env * env, a_Stmt_List list,
                                      t_CKBOOL * retrieveAllControlPathsReturn )
{
    // last non-NULL statement
    a_Stmt lastStmt = NULL;
    // set to NULL to begin with
    if( retrieveAllControlPathsReturn ) *retrieveAllControlPathsReturn = FALSE;

    // type check the stmt_list
    while( list )
    {
        // the current statement
        if( !type_engine_scan1_stmt( env, list->stmt ) )
            return FALSE;

        // add if not NULL
        if( retrieveAllControlPathsReturn && list->stmt != NULL )
            lastStmt = list->stmt;

        // advance to the next statement
        list = list->next;
    }

    // set to the last non-NULL stmt | 1.5.1.0
    // handle cases with multiple ending ;;;
    //     public class Class { fun void foo(){ samp => now;; } }
    if( retrieveAllControlPathsReturn && lastStmt )
        *retrieveAllControlPathsReturn = lastStmt->allControlPathsReturn;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_stmt(()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_stmt( Chuck_Env * env, a_Stmt stmt )
{
    t_CKBOOL ret = FALSE;
    t_CKBOOL allControlPathsReturn = FALSE; // 1.5.1.0

    if( !stmt )
        return TRUE;

    // the type of stmt
    switch( stmt->s_type )
    {
        case ae_stmt_if:
            // count scope to help determine class member
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_if( env, &stmt->stmt_if );
            env->curr->value.pop();
            env->class_scope--;

            // both control paths must end with return for this to be true | 1.5.1.0
            if( stmt->stmt_if.if_body )
            {
                // check if there is ELSE-BODY
                if( stmt->stmt_if.else_body )
                {
                    // both IF-BODY and ELSE-BODY should verify all control paths return
                    allControlPathsReturn = stmt->stmt_if.if_body->allControlPathsReturn
                                         && stmt->stmt_if.else_body->allControlPathsReturn;
                }
                else
                {
                    // if no ELSE-BODY, then no guarantee all control paths return
                    allControlPathsReturn = FALSE;
                }
            }
            break;

        case ae_stmt_for:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_for( env, &stmt->stmt_for );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_foreach:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_foreach( env, &stmt->stmt_foreach );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_while:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_while( env, &stmt->stmt_while );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_until:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_until( env, &stmt->stmt_until );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_loop:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan1_loop( env, &stmt->stmt_loop );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_exp:
            ret = type_engine_scan1_exp( env, stmt->stmt_exp );
            break;

        case ae_stmt_return:
            ret = type_engine_scan1_return( env, &stmt->stmt_return );
            // this one should be easy, since this is literal return stmt | 1.5.1.0
            allControlPathsReturn = TRUE;
            break;

        case ae_stmt_code:
            env->class_scope++;
            ret = type_engine_scan1_code_segment( env, &stmt->stmt_code );
            env->class_scope--;

            // check if code segment (e.g., { stmt_list }) fulfills this | 1.5.1.0
            allControlPathsReturn = stmt->stmt_code.allControlPathsReturn;
            break;

        case ae_stmt_break:
            ret = type_engine_scan1_break( env, &stmt->stmt_break );
            break;

        case ae_stmt_continue:
            ret = type_engine_scan1_continue( env, &stmt->stmt_continue );
            break;

        case ae_stmt_switch:
            env->class_scope++;
            ret = type_engine_scan1_switch( env, &stmt->stmt_switch );
            env->class_scope--;
            break;

        case ae_stmt_case:
            // ret = type_engine_scan1_case( env, &stmt->stmt_case );
            break;

        case ae_stmt_gotolabel:
            // ret = type_engine_scan1_gotolabel( env, &stmt->goto_label );
            break;

        default:
            EM_error2( stmt->where,
                "internal compiler error (pre-scan) - no stmt type '%i'!", stmt->s_type );
            ret = FALSE;
            break;
    }

    // set whether all control paths associated with this stmt return | 1.5.1.0
    stmt->allControlPathsReturn = allControlPathsReturn;

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_if( Chuck_Env * env, a_Stmt_If stmt )
{
    // check the conditional
    if( !type_engine_scan1_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: ensure that conditional has valid type

    // check if
    if( !type_engine_scan1_stmt( env, stmt->if_body ) )
        return FALSE;

    // check else, if there is one
    if( stmt->else_body )
        if( !type_engine_scan1_stmt( env, stmt->else_body ) )
            return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_for()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_for( Chuck_Env * env, a_Stmt_For stmt )
{
    // check the initial
    if( !type_engine_scan1_stmt( env, stmt->c1 ) )
        return FALSE;

    // check the conditional
    if( !type_engine_scan1_stmt( env, stmt->c2 ) )
        return FALSE;

    // TODO: same as if - check conditional type valid

    // check the post
    if( stmt->c3 && !type_engine_scan1_exp( env, stmt->c3 ) )
        return FALSE;

    // check body
    if( !type_engine_scan1_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_foreach()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_foreach( Chuck_Env * env, a_Stmt_ForEach stmt )
{
    // check the iter part
    if( !type_engine_scan1_exp( env, stmt->theIter ) )
        return FALSE;

    // check the array part
    if( !type_engine_scan1_exp( env, stmt->theArray ) )
        return FALSE;

    // check body
    if( !type_engine_scan1_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_while()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_while( Chuck_Env * env, a_Stmt_While stmt )
{
    // check the conditional
    if( !type_engine_scan1_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan1_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_until()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_until( Chuck_Env * env, a_Stmt_Until stmt )
{
    // check the conditional
    if( !type_engine_scan1_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan1_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_loop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_loop( Chuck_Env * env, a_Stmt_Loop stmt )
{
    // check the conditional
    if( !type_engine_scan1_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan1_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_switch()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_switch( Chuck_Env * env, a_Stmt_Switch stmt )
{
    // TODO: implement this
    EM_error2( stmt->where, "switch not implemented..." );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_break()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_break( Chuck_Env * env, a_Stmt_Break br )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_continue()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_continue( Chuck_Env * env, a_Stmt_Continue cont )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_return()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_return( Chuck_Env * env, a_Stmt_Return stmt )
{
    t_CKBOOL ret = FALSE;

    // check the type of the return
    if( stmt->val )
        ret = type_engine_scan1_exp( env, stmt->val );
    else
        ret = TRUE;

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_code_segment()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_code_segment( Chuck_Env * env, a_Stmt_Code stmt,
                                         t_CKBOOL push )
{
    // class
    env->class_scope++;
    // push
    if( push ) env->curr->value.push(); // env->context->nspc.value.push();
    // do it
    t_CKBOOL t = type_engine_scan1_stmt_list( env, stmt->stmt_list, &stmt->allControlPathsReturn );
    // pop
    if( push ) env->curr->value.pop();  // env->context->nspc.value.pop();
    // class
    env->class_scope--;

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp( Chuck_Env * env, a_Exp exp )
{
    a_Exp curr = exp;
    t_CKBOOL ret = TRUE;

    // loop through parallel expressions
    while( curr )
    {
        // examine the syntax
        switch( curr->s_type )
        {
        case ae_exp_binary:
            ret = type_engine_scan1_exp_binary( env, &curr->binary );
        break;

        case ae_exp_unary:
            ret = type_engine_scan1_exp_unary( env, &curr->unary );
        break;

        case ae_exp_cast:
            ret = type_engine_scan1_exp_cast( env, &curr->cast );
        break;

        case ae_exp_postfix:
            ret = type_engine_scan1_exp_postfix( env, &curr->postfix );
        break;

        case ae_exp_dur:
            ret = type_engine_scan1_exp_dur( env, &curr->dur );
        break;

        case ae_exp_primary:
            ret = type_engine_scan1_exp_primary( env, &curr->primary );
        break;

        case ae_exp_array:
            ret = type_engine_scan1_exp_array( env, &curr->array );
        break;

        case ae_exp_func_call:
            ret = type_engine_scan1_exp_func_call( env, &curr->func_call );
        break;

        case ae_exp_dot_member:
            ret = type_engine_scan1_exp_dot_member( env, &curr->dot_member );
        break;

        case ae_exp_if:
            ret = type_engine_scan1_exp_if( env, &curr->exp_if );
        break;

        case ae_exp_decl:
            ret = type_engine_scan1_exp_decl( env, &curr->decl );
        break;

        default:
            EM_error2( curr->where,
                "internal compiler error - no expression type '%i'...",
                curr->s_type );
            return FALSE;
        }

        // error
        if( !ret )
            return FALSE;

        // advance to next expression
        curr = curr->next;
    }

    // return type
    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_binary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_binary( Chuck_Env * env, a_Exp_Binary binary )
{
    a_Exp cl = binary->lhs, cr = binary->rhs;

    // scan lhs
    t_CKBOOL left = type_engine_scan1_exp( env, cl );
    if( !left ) return FALSE;
    // scan rhs
    t_CKBOOL right = type_engine_scan1_exp( env, cr);
    if( !right ) return FALSE;

    // cross chuck
    while( cr )
    {
        // type check the pair
        if( !type_engine_scan1_op( env, binary->op, cl, cr, binary ) )
            return FALSE;

        cr = cr->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_op()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs,
                              a_Exp_Binary binary )
{
    // TODO: check for static here

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_op_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs,
                                    a_Exp_Binary binary )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_op_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_op_upchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_op_at_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_unary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_unary( Chuck_Env * env, a_Exp_Unary unary )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_primary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_primary( Chuck_Env * env, a_Exp_Primary exp )
{
    // check syntax
    switch( exp->s_type )
    {
        // variable
        case ae_primary_var:
        case ae_primary_num:
        case ae_primary_float:
        case ae_primary_str:
        case ae_primary_char:
            break;

        // array literal
        case ae_primary_array:
            type_engine_scan1_exp_array_lit( env, exp );
        break;

        // complex literal
        case ae_primary_complex:
            // if( !type_engine_scan1_exp_complex_lit( env, exp ) ) return FALSE;
        break;

        // polar literal
        case ae_primary_polar:
            // if( !type_engine_scan1_exp_polar_lit( env, exp ) ) return FALSE;
        break;

        // vector literal, ge: added 1.3.5.3
        case ae_primary_vec:
            // if( !type_engine_scan1_exp_vec_lit( env, exp ) ) return FALSE;
        break;

        // expression
        case ae_primary_exp:
            if( !type_engine_scan1_exp( env, exp->exp ) ) return FALSE;
        break;

        // hack
        case ae_primary_hack:
            // make sure not l-value
            if( exp->exp->s_type == ae_exp_decl )
            {
                EM_error2( exp->where,
                    "cannot use <<< >>> on variable declarations" );
                return FALSE;
            }

            // scan
            if( !type_engine_scan1_exp( env, exp->exp ) ) return FALSE;
        break;

        // nil (void)
        case ae_primary_nil:
        break;

        // no match
        default:
            EM_error2( exp->where,
                "(internal error) unrecognized primary type '%i'", exp->s_type );
            return FALSE;
            break;
    }

    return TRUE;}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_array_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // verify there are no errors from the parser...
    if( !verify_array( exp->array ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_cast( Chuck_Env * env, a_Exp_Cast cast )
{
    // check the exp
    t_CKBOOL t = type_engine_scan1_exp( env, cast->exp );
    if( !t ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_dur()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_dur( Chuck_Env * env, a_Exp_Dur dur )
{
    // type check the two components
    t_CKBOOL base = type_engine_scan1_exp( env, dur->base );
    t_CKBOOL unit = type_engine_scan1_exp( env, dur->unit );

    // make sure both type check
    if( !base || !unit ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_postfix()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix )
{
    // check the exp
    t_CKBOOL t = type_engine_scan1_exp( env, postfix->exp );
    if( !t ) return FALSE;

    // syntax
    // TODO: figure out ++/--
    switch( postfix->op )
    {
        case ae_op_plusplus:
        case ae_op_minusminus:
            // assignable?
            if( postfix->exp->s_meta != ae_meta_var )
            {
                EM_error2( postfix->exp->where,
                    "postfix operator '%s' cannot be used on non-mutable data-type...",
                    op2str( postfix->op ) );
                return FALSE;
            }

            // TODO: mark somewhere we need to post increment

            return TRUE;
        break;

        default:
            // no match
            EM_error2( postfix->where,
                "internal compiler error (pre-scan): unrecognized postfix '%i'", postfix->op );
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_if( Chuck_Env * env, a_Exp_If exp_if )
{
    // check the components
    t_CKBOOL cond = type_engine_scan1_exp( env, exp_if->cond );
    t_CKBOOL if_exp = type_engine_scan1_exp( env, exp_if->if_exp );
    t_CKBOOL else_exp = type_engine_scan1_exp( env, exp_if->else_exp );

    // make sure everything good
    if( !cond || !if_exp || !else_exp ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_array_subscripts( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_array_subscripts( Chuck_Env * env, a_Exp exp_list )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_decl( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_decl( Chuck_Env * env, a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;

    // look up the type
    // TODO: handle T a, b, c...
    // TODO: do we climb?
    t_CKTYPE t = type_engine_find_type( env, decl->type->xid );
    // if not found, try to resolve
    if( !t )
    {
        // resolve
        EM_error2( decl->where, "...in declaration" );
        return FALSE;
    }

    // loop through the variables
    while( list != NULL )
    {
        // get the decl
        var_decl = list->var_decl;
        // count
        decl->num_var_decls++;

        // scan constructor args | 1.5.2.0 (ge) added
        if( var_decl->ctor.args != NULL )
        {
            // disallow non-default constructors for 'global' Object decls
            if( decl->is_global )
            {
                // resolve
                EM_error2( var_decl->where, "'global' variables cannot invoke non-default constructors..." );
                return FALSE;
            }
            // type check the exp
            if( !type_engine_scan1_exp( env, var_decl->ctor.args ) )
                return FALSE;
        }

        // scan if array
        if( var_decl->array != NULL )
        {
            // verify there are no errors from the parser...
            if( !verify_array( var_decl->array ) )
                return FALSE;

            // may be partial and empty []
            if( var_decl->array->exp_list )
            {
                // type check the exp
                if( !type_engine_scan1_exp( env, var_decl->array->exp_list ) )
                    return FALSE;
                // make sure types are of int
                if( !type_engine_scan1_array_subscripts( env, var_decl->array->exp_list ) )
                    return FALSE;
            }
        }

        // 1.4.2.0 (ge) added: var_decl->ck_type = t;
        CK_SAFE_REF_ASSIGN( var_decl->ck_type, t );

        // the next var decl
        list = list->next;
    }

    // remember : decl->ck_type = t;
    CK_SAFE_REF_ASSIGN( decl->ck_type, t );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                         t_CKFUNC & ck_func, int linepos )
{
    // Chuck_Func * func = NULL;
    // Chuck_Func * up = NULL;

    // type check the func
    t_CKBOOL f = type_engine_scan1_exp( env, exp_func );
    if( !f ) return FALSE;

    // check the arguments
    if( args )
    {
        t_CKBOOL a = type_engine_scan1_exp( env, args );
        if( !a ) return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call )
{
    // type check it
    return type_engine_scan1_exp_func_call( env, func_call->func, func_call->args,
                                           func_call->ck_func, func_call->where );
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_dot_member()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member )
{
    // type check the base
    t_CKBOOL base = type_engine_scan1_exp( env, member->base );
    if( !base ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_exp_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_exp_array( Chuck_Env * env, a_Exp_Array array )
{
    // verify there are no errors from the parser...
    if( !verify_array( array->indices ) )
        return FALSE;

    // type check the base
    t_CKBOOL base = type_engine_scan1_exp( env, array->base );
    if( !base ) return FALSE;

    // type check the index
    t_CKBOOL index = type_engine_scan1_exp( env, array->indices->exp_list );
    if( !index ) return FALSE;

    // cycle through each exp
    // a_Exp e = array->indices->exp_list;
    // count the dimension
    // t_CKUINT depth = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_class_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_class_def( Chuck_Env * env, a_Class_Def class_def )
{
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;
    // the class
    Chuck_Type * the_class = class_def->type;

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
            // do the statements
            ret = type_engine_scan1_stmt_list( env, body->section->stmt_list );
            break;

        case ae_section_func:
            // do the function
            ret = type_engine_scan1_func_def( env, body->section->func_def );
            break;

        case ae_section_class:
            // do the class
            ret = type_engine_scan1_class_def( env, body->section->class_def );
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

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan1_func_def()
// desc: type check scan pass 1 for function definition
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan1_func_def( Chuck_Env * env, a_Func_Def f )
{
    a_Arg_List arg_list = NULL;
    t_CKUINT count = 0;
    t_CKBOOL is_static = (env->class_def != NULL) && (f->static_decl == ae_key_static);

    // substitute for @construct
    if( S_name(f->name) == string("@construct") )
    {
        // make sure we are in a class
        if( !env->class_def )
        {
            EM_error2( f->where, "@construct() can only be used within class definitions..." );
            return FALSE;
        }

        // substitute class name
        f->name = insert_symbol( env->class_def->base_name.c_str() );
    }
    // verify @destruct
    if( S_name(f->name) == string("@destruct") )
    {
        // make sure we are in a class
        if( !env->class_def )
        {
            EM_error2( f->where, "@destruct() can only be used within class definitions..." );
            return FALSE;
        }
        // make sure there are no arguments
        if( f->arg_list != NULL )
        {
            EM_error2( f->where, "@destruct() cannot accept arguments..." );
            return FALSE;
        }
        // substitute ~[class name]
        // f->name = insert_symbol( string("~"+env->class_def->base_name).c_str() );
    }
    // check if constructor
    t_CKBOOL isInCtor = isctor(env,f);
    // check if destructor
    t_CKBOOL isInDtor = isdtor(env,f);

    // check if reserved
    if( type_engine_check_reserved( env, f->name, f->where ) )
    {
        EM_error2( f->where, "...in function definition '%s'",
            S_name(f->name) );
        return FALSE;
    }

    // look up the return type
    f->ret_type = type_engine_find_type( env, f->type_decl->xid );
    // no return type
    if( !f->ret_type )
    {
        // TODO: try to resolve
        EM_error2( f->where, "...in return type of function '%s'", S_name(f->name) );
        goto error;
    }
    // add reference | 1.5.0.5
    CK_SAFE_ADD_REF( f->ret_type );

    // check if we are in a constructor
    if( isInCtor )
    {
        // check static
        if( is_static )
        {
            EM_error2( f->where, "constructor cannot be declared as 'static'..." );
            goto error;
        }
        // check return type (must be void)
        if( !isa(f->ret_type, env->ckt_void) )
        {
            EM_error2( f->where, "constructor must return void..." );
            goto error;
        }
    }

    if( isInDtor )
    {
        // check static
        if( is_static )
        {
            EM_error2( f->where, "destructor cannot be declared as 'static'..." );
            goto error;
        }
        // check return type (must be void)
        if( !isa(f->ret_type, env->ckt_void) )
        {
            EM_error2( f->where, "destructor must return void..." );
            goto error;
        }
    }

    // check if array
    if( f->type_decl->array != NULL )
    {
        // verify there are no errors from the parser...
        if( !verify_array( f->type_decl->array ) )
            return FALSE;

        Chuck_Type * t = NULL;
        Chuck_Type * t2 = f->ret_type;
        // should be partial and empty []
        if( f->type_decl->array->exp_list )
        {
            EM_error2( f->type_decl->array->where, "function '%s':", S_name(f->name) );
            EM_error2( f->type_decl->array->where, "return array type must be defined with empty []'s" );
            return FALSE;
        }

        // create the new array type
        t = new_array_type(
            env,  // the env
            env->ckt_array,  // the array base class
            f->type_decl->array->depth,  // the depth of the new type
            t2,  // the 'array_type'
            env->curr  // the owner namespace
        );

        // TODO: verify
        // set ref
        f->type_decl->ref = TRUE;
        // replace type : f->ret_type = t
        CK_SAFE_REF_ASSIGN( f->ret_type, t );
    }

    // look up types for the function arguments
    arg_list = f->arg_list;
    // count
    count = 1;
    // loop over arguments
    while( arg_list )
    {
        // look up in type table
        arg_list->type = type_engine_find_type( env, arg_list->type_decl->xid );
        // if not there, try to resolve
        if( !arg_list->type )
        {
            // TODO: try to resolve
            EM_error2( arg_list->where,
                "...in argument %i '%s' of function '%s(.)'",
                count, S_name(arg_list->var_decl->xid), S_name(f->name) );
            goto error;
        }

        // count
        count++;
        // next arg
        arg_list = arg_list->next;
    }

    // scan the code for types that need resolution
    assert( f->code == NULL || f->code->s_type == ae_stmt_code );
    if( f->code && !type_engine_scan1_code_segment( env, &f->code->stmt_code, FALSE ) )
    {
        EM_error2( 0, "...in function '%s'", S_name(f->name) );
        goto error;
    }

    // propagate from stmt_code to the outer stmt | 1.5.1.0
    if( f->code ) f->code->allControlPathsReturn = f->code->stmt_code.allControlPathsReturn;

    return TRUE;

error:

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_prog()
// desc: data in env should be ready
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_prog( Chuck_Env * env, a_Program prog,
                                   te_HowMuch how_much )
{
    t_CKBOOL ret = TRUE;

    if( !prog )
        return FALSE;

    // log
    EM_log( CK_LOG_FINER, "(pass 2) type verification scan..." );
                          // env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    // log how much
    EM_log( CK_LOG_FINEST, "target: %s", howmuch2str( how_much ) );

    // go through each of the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            // if classes only, then skip
            if( how_much == te_do_classes_only ) break;
            // scan the statements
            ret = type_engine_scan2_stmt_list( env, prog->section->stmt_list );
            break;

        case ae_section_func:
            // if classes only, then skip
            if( how_much == te_do_classes_only ) break;
            // scan the function definition
            ret = type_engine_scan2_func_def( env, prog->section->func_def );
            break;

        case ae_section_class:
            // if no classes, then skip
            if( how_much == te_do_no_classes ) break;
            // scan the class definition
            ret = type_engine_scan2_class_def( env, prog->section->class_def );
            break;

        default:
            EM_error2( prog->where,
                "(internal error) unrecognized program section in type checker pre-scan..." );
            ret = FALSE;
            break;
        }

        // next section
        prog = prog->next;
    }

    // pop indent
    EM_poplog();

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_stmt_list()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_stmt_list( Chuck_Env * env, a_Stmt_List list )
{
    // type check the stmt_list
    while( list )
    {
        // the current statement
        if( !type_engine_scan2_stmt( env, list->stmt ) )
            return FALSE;

        // advance to the next statement
        list = list->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_stmt(()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_stmt( Chuck_Env * env, a_Stmt stmt )
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
            ret = type_engine_scan2_if( env, &stmt->stmt_if );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_for:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan2_for( env, &stmt->stmt_for );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_foreach:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan2_foreach( env, &stmt->stmt_foreach );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_while:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan2_while( env, &stmt->stmt_while );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_until:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan2_until( env, &stmt->stmt_until );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_loop:
            env->class_scope++;
            env->curr->value.push();
            ret = type_engine_scan2_loop( env, &stmt->stmt_loop );
            env->curr->value.pop();
            env->class_scope--;
            break;

        case ae_stmt_exp:
            ret = type_engine_scan2_exp( env, stmt->stmt_exp );
            break;

        case ae_stmt_return:
            ret = type_engine_scan2_return( env, &stmt->stmt_return );
            break;

        case ae_stmt_code:
            env->class_scope++;
            ret = type_engine_scan2_code_segment( env, &stmt->stmt_code );
            env->class_scope--;
            break;

        case ae_stmt_break:
            ret = type_engine_scan2_break( env, &stmt->stmt_break );
            break;

        case ae_stmt_continue:
            ret = type_engine_scan2_continue( env, &stmt->stmt_continue );
            break;

        case ae_stmt_switch:
            env->class_scope++;
            ret = type_engine_scan2_switch( env, &stmt->stmt_switch );
            env->class_scope--;
            break;

        case ae_stmt_case:
            // ret = type_engine_scan2_case( env, &stmt->stmt_case );
            break;

        case ae_stmt_gotolabel:
            // ret = type_engine_scan2_gotolabel( env, &stmt->goto_label );
            break;

        default:
            EM_error2( stmt->where,
                "internal compiler error (pre-scan) - no stmt type '%i'!", stmt->s_type );
            ret = FALSE;
            break;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_if( Chuck_Env * env, a_Stmt_If stmt )
{
    // check the conditional
    if( !type_engine_scan2_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: ensure that conditional has valid type

    // check if
    if( !type_engine_scan2_stmt( env, stmt->if_body ) )
        return FALSE;

    // check else, if there is one
    if( stmt->else_body )
        if( !type_engine_scan2_stmt( env, stmt->else_body ) )
            return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_for()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_for( Chuck_Env * env, a_Stmt_For stmt )
{
    // check the initial
    if( !type_engine_scan2_stmt( env, stmt->c1 ) )
        return FALSE;

    // check the conditional
    if( !type_engine_scan2_stmt( env, stmt->c2 ) )
        return FALSE;

    // TODO: same as if - check conditional type valid

    // check the post
    if( stmt->c3 && !type_engine_scan2_exp( env, stmt->c3 ) )
        return FALSE;

    // check body
    if( !type_engine_scan2_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_foreach()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_foreach( Chuck_Env * env, a_Stmt_ForEach stmt )
{
    // check the array part first
    if( !type_engine_scan2_exp( env, stmt->theArray ) )
        return FALSE;

    // check the iter part
    if( !type_engine_scan2_exp( env, stmt->theIter ) )
        return FALSE;

    // check body
    if( !type_engine_scan2_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_while()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_while( Chuck_Env * env, a_Stmt_While stmt )
{
    // check the conditional
    if( !type_engine_scan2_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan2_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_until()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_until( Chuck_Env * env, a_Stmt_Until stmt )
{
    // check the conditional
    if( !type_engine_scan2_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan2_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_loop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_loop( Chuck_Env * env, a_Stmt_Loop stmt )
{
    // check the conditional
    if( !type_engine_scan2_exp( env, stmt->cond ) )
        return FALSE;

    // TODO: same as if - ensure the type in conditional is valid

    // check the body
    if( !type_engine_scan2_stmt( env, stmt->body ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_switch()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_switch( Chuck_Env * env, a_Stmt_Switch stmt )
{
    // TODO: implement this
    EM_error2( stmt->where, "switch not implemented..." );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_break()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_break( Chuck_Env * env, a_Stmt_Break br )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_continue()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_continue( Chuck_Env * env, a_Stmt_Continue cont )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_return()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_return( Chuck_Env * env, a_Stmt_Return stmt )
{
    t_CKBOOL ret = FALSE;

    // check the type of the return
    if( stmt->val )
        ret = type_engine_scan2_exp( env, stmt->val );
    else
        ret = TRUE;

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_code_segment()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_code_segment( Chuck_Env * env, a_Stmt_Code stmt,
                                        t_CKBOOL push )
{
    // class
    env->class_scope++;
    // push
    if( push ) env->curr->value.push(); // env->context->nspc.value.push();
    // do it
    t_CKBOOL t = type_engine_scan2_stmt_list( env, stmt->stmt_list );
    // pop
    if( push ) env->curr->value.pop();  // env->context->nspc.value.pop();
    // class
    env->class_scope--;

    return t;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp( Chuck_Env * env, a_Exp exp )
{
    a_Exp curr = exp;
    t_CKBOOL ret = TRUE;

    // loop through parallel expressions
    while( curr )
    {
        // examine the syntax
        switch( curr->s_type )
        {
        case ae_exp_binary:
            ret = type_engine_scan2_exp_binary( env, &curr->binary );
        break;

        case ae_exp_unary:
            ret = type_engine_scan2_exp_unary( env, &curr->unary );
        break;

        case ae_exp_cast:
            ret = type_engine_scan2_exp_cast( env, &curr->cast );
        break;

        case ae_exp_postfix:
            ret = type_engine_scan2_exp_postfix( env, &curr->postfix );
        break;

        case ae_exp_dur:
            ret = type_engine_scan2_exp_dur( env, &curr->dur );
        break;

        case ae_exp_primary:
            ret = type_engine_scan2_exp_primary( env, &curr->primary );
        break;

        case ae_exp_array:
            ret = type_engine_scan2_exp_array( env, &curr->array );
        break;

        case ae_exp_func_call:
            ret = type_engine_scan2_exp_func_call( env, &curr->func_call );
        break;

        case ae_exp_dot_member:
            ret = type_engine_scan2_exp_dot_member( env, &curr->dot_member );
        break;

        case ae_exp_if:
            ret = type_engine_scan2_exp_if( env, &curr->exp_if );
        break;

        case ae_exp_decl:
            ret = type_engine_scan2_exp_decl( env, &curr->decl );
        break;

        default:
            EM_error2( curr->where,
                "internal compiler error - no expression type '%i'...",
                curr->s_type );
            return FALSE;
        }

        // error
        if( !ret )
            return FALSE;

        // advance to next expression
        curr = curr->next;
    }

    // return type
    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_binary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_binary( Chuck_Env * env, a_Exp_Binary binary )
{
    a_Exp cl = binary->lhs, cr = binary->rhs;

    // scan lhs
    t_CKBOOL left = type_engine_scan2_exp( env, cl );
    if( !left ) return FALSE;
    // scan rhs
    t_CKBOOL right = type_engine_scan2_exp( env, cr);
    if( !right ) return FALSE;

    // cross chuck
    while( cr )
    {
        // type check the pair
        if( !type_engine_scan2_op( env, binary->op, cl, cr, binary ) )
            return FALSE;

        cr = cr->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_op()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_op( Chuck_Env * env, ae_Operator op, a_Exp lhs, a_Exp rhs,
                                 a_Exp_Binary binary )
{
    // TODO: check for static here

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_op_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_op_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs,
                                       a_Exp_Binary binary )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_op_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_op_unchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_op_upchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_op_upchuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_op_at_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_op_at_chuck( Chuck_Env * env, a_Exp lhs, a_Exp rhs )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_unary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_unary( Chuck_Env * env, a_Exp_Unary unary )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_primary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_primary( Chuck_Env * env, a_Exp_Primary exp )
{
    // check syntax
    switch( exp->s_type )
    {
        // variable
        case ae_primary_var:
        case ae_primary_num:
        case ae_primary_float:
        case ae_primary_str:
        case ae_primary_char:
            break;

        // array literal
        case ae_primary_array:
            type_engine_scan2_exp_array_lit( env, exp );
        break;

        // complex literal
        case ae_primary_complex:
            // if( !type_engine_scan2_exp_complex_lit( env, exp ) ) return FALSE;
        break;

        // polar literal
        case ae_primary_polar:
            // if( !type_engine_scan2_exp_polar_lit( env, exp ) ) return FALSE;
        break;

        // vector literal, ge: added 1.3.5.3
        case ae_primary_vec:
            // if( !type_engine_scan2_exp_vec_lit( env, exp ) ) return FALSE;
        break;

        // expression
        case ae_primary_exp:
            if( !type_engine_scan2_exp( env, exp->exp ) ) return FALSE;
        break;

        // hack
        case ae_primary_hack:
            // make sure not l-value (this should be checked in type_engine_scan1_exp_primary()
            assert( exp->exp->s_type != ae_exp_decl );
            // scan
            if( !type_engine_scan2_exp( env, exp->exp ) ) return FALSE;
        break;

        // nil (void)
        case ae_primary_nil:
        break;

        // no match
        default:
            EM_error2( exp->where,
                "internal error - unrecognized primary type '%i'", exp->s_type );
            return FALSE;
            break;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_array_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_array_lit( Chuck_Env * env, a_Exp_Primary exp )
{
    // verify there are no errors from the parser...
    if( !verify_array( exp->array ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_cast( Chuck_Env * env, a_Exp_Cast cast )
{
    // check the exp
    t_CKBOOL t = type_engine_scan2_exp( env, cast->exp );
    if( !t ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_dur()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_dur( Chuck_Env * env, a_Exp_Dur dur )
{
    // type check the two components
    t_CKBOOL base = type_engine_scan2_exp( env, dur->base );
    t_CKBOOL unit = type_engine_scan2_exp( env, dur->unit );

    // make sure both type check
    if( !base || !unit ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_postfix()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_postfix( Chuck_Env * env, a_Exp_Postfix postfix )
{
    // check the exp
    t_CKBOOL t = type_engine_scan2_exp( env, postfix->exp );
    if( !t ) return FALSE;

    // syntax
    // TODO: figure out ++/--
    switch( postfix->op )
    {
        case ae_op_plusplus:
        case ae_op_minusminus:
            // assignable?
            if( postfix->exp->s_meta != ae_meta_var )
            {
                EM_error2( postfix->exp->where,
                    "postfix operator '%s' cannot be used on non-mutable data-type...",
                    op2str( postfix->op ) );
                return FALSE;
            }

            // TODO: mark somewhere we need to post increment

            return TRUE;
        break;

        default:
            // no match
            EM_error2( postfix->where,
                "internal compiler error (pre-scan): unrecognized postfix '%i'", postfix->op );
            return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_if( Chuck_Env * env, a_Exp_If exp_if )
{
    // check the components
    t_CKBOOL cond = type_engine_scan2_exp( env, exp_if->cond );
    t_CKBOOL if_exp = type_engine_scan2_exp( env, exp_if->if_exp );
    t_CKBOOL else_exp = type_engine_scan2_exp( env, exp_if->else_exp );

    // make sure everything good
    if( !cond || !if_exp || !else_exp ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_array_subscripts( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_array_subscripts( Chuck_Env * env, a_Exp exp_list )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_decl_create() | 1.5.1.1 (ge)
//       *** adapted from type_engine_scan2_exp_decl() pre-1.5.0.8
//       *** which became type_engine_check_exp_decl_part1() in 1.5.0.8
// reason: 'auto' needs more context before it can be processed | 1.5.0.8 (ge)
//       however, class variables are meant to be accessible out-of-order
//       and needs decl created in an earlier pass | 1.5.1.1 (ge)
// desc: this can now be called either from type_scan if not 'auto' in order
//       to support accessing class member decls from anywhere (e.g., in the
//       same file, above the actual decl); or it can be deferred in the case
//       of 'auto' from chuck_type | 1.5.1.1 (ge)
// caveat: 'auto' is disallowed for now as a top-level class member decl type
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_decl_create( Chuck_Env * env, a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;
    Chuck_Type * type = NULL;
    Chuck_Value * value = NULL;
    t_CKBOOL do_alloc = TRUE;
    t_CKBOOL is_first_in_list = TRUE;

    // check to see if Part 1 was already processed | 1.5.1.0 (ge)
    // NOTE Part 1 (migrated here from scan2b_exp_decl() in 1.5.0.8 to support
    // 'auto', can only be run once as it creates and adds values into the
    // current scope; yet a decl expression could be checked more than once
    // due to chained chuck statements like: `440 => float f => osc.freq;`
    if( list && list->var_decl && list->var_decl->value )
        return TRUE;

    // retrieve the type
    type = decl->ck_type;
    // make sure it's not NULL
    assert( type != NULL );

    // check to see type is not void or auto
    // by this point, any 'auto' should have been resolved
    if( type->size == 0 )
    {
        // see if auto was declared
        if( isa( type, env->ckt_auto ) )
        {
            EM_error2( decl->type->where,
                "cannot use 'auto' type variable declaration here..." );
            EM_error2( 0, "(hint: 'auto' requires either initialization or specific contexts)" );
        }
        else
        {
            EM_error2( decl->where,
                "cannot declare variables of size '0' (i.e. 'void')..." );
        }
        return FALSE;
    }

    // T @ foo?
    do_alloc = !decl->type->ref;

    // make sure complete
    if( /*!t->is_complete &&*/ do_alloc )
    {
        // check to see if class inside itself
        if( env->class_def && equals( type, env->class_def ) && env->class_scope == 0 )
        {
            EM_error2( decl->where,
                "...(note: object of type '%s' declared inside itself)",
                type->c_name() );
            return FALSE;
        }
    }

    // primitive
    if( (isprim( env, type ) || isa( type, env->ckt_string )) && decl->type->ref )  // TODO: string
    {
        // check for string type
        t_CKBOOL isaStr = isa( type, env->ckt_string );
        // error
        EM_error2( decl->where,
            "cannot declare references (@) of%s type '%s'...",  isaStr ? "" : " primitive",
            type->c_name() );
        // more info
        if( !isaStr )
        {
            EM_error2( 0,
                       "...(primitive types: 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
        }
        else
        {
            EM_error2( 0,
                       "...(NOTE 'string' is a special Object whose operational semantics resemble both Object types and primitive types; e.g., instantiation and function argument-passing are like any other Object; however assignment '@=>' and '=>' are carried out by-value, as with primitive types such as 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
        }
        return FALSE;
    }

    // loop through the variables
    while( list )
    {
        // 1.4.2.0 (ge) | reset the type variable to the lead type in the decl
        // e.g., for cases like int x[2], y;
        // ...this is so y would not be associated with x's array type
        type = decl->ck_type;

        // get the decl
        var_decl = list->var_decl;
        // 1.4.2.0 (ge) | by default, copy the decl type reference bit
        // this could be overwritten later as appropriate, e.g., by array vars
        var_decl->ref = decl->type->ref;

        // check if reserved
        if( type_engine_check_reserved( env, var_decl->xid, var_decl->where ) )
        {
            EM_error2( var_decl->where,
                "...in variable declaration", S_name(var_decl->xid) );
            return FALSE;
        }

        // check if locally defined
        if( env->curr->lookup_value( var_decl->xid, FALSE ) )
        {
            EM_error2( var_decl->where,
                "'%s' has already been defined in the same scope",
                S_name(var_decl->xid) );
            return FALSE;
        }

        // check for constructor args | 1.5.2.0 (ge) added
        if( var_decl->ctor.args != NULL )
        {
            // type check the exp
            if( !type_engine_scan2_exp( env, var_decl->ctor.args ) )
                return FALSE;
        }

        // check if array
        if( var_decl->array != NULL )
        {
            // 1.4.2.0 (ge) was: decl->type->ref;
            var_decl->ref = ( var_decl->array->exp_list == NULL );
            // the declaration type | 1.4.2.0 (ge) fixed for multiple decl (e.g., int x[1], y[2];)
            Chuck_Type * t2 = decl->ck_type; // was: type, which won't work if more than one var declared

            // create the new array type
            type = new_array_type(
                env,  // the env
                env->ckt_array,  // the array base class
                var_decl->array->depth,  // the depth of the new type
                t2,  // the 'array_type'
                env->curr  // the owner namespace
            );

            // 1.4.2.0 (ge) | assign new array type to current var decl
            // for handling the following kind of multi-var declarations
            //   int x[1], y[2];
            //   int x, y[1];
            // set reference : var_decl->ck_type = type;
            CK_SAFE_REF_ASSIGN( var_decl->ck_type, type );

            // 1.4.2.0 (ge) | if one and only one variable, then update decl->ck_type
            // otherwise, the variables could have different array depths, and therefore different types
            // also note: cannot => to a multi-variable declaration (e.g., 5 => int x, y;)
            // this is to support array initialization (e.g., [ [1,2], [3,4] ] @=> int x[][];)
            if( is_first_in_list && list->next == NULL )
            {
                // set reference : var_decl->ck_type = type;
                CK_SAFE_REF_ASSIGN( decl->ck_type, type );
            }
        }

        // enter into value binding
        env->curr->value.add( var_decl->xid,
            value = env->context->new_Chuck_Value( type, S_name(var_decl->xid) ) );

        // remember the owner
        value->owner = env->curr; CK_SAFE_ADD_REF( value->owner );
        value->owner_class = env->func ? NULL : env->class_def;
        value->is_member = ( env->class_def != NULL &&
                             env->class_scope == 0 &&
                             env->func == NULL && !decl->is_static );
        value->is_context_global = ( env->class_def == NULL && env->func == NULL );
        value->addr = var_decl->addr;
        // flag it until the decl is checked
        value->is_decl_checked = FALSE;
        // flag as global
        value->is_global = decl->is_global;
        // flag as const | 1.5.1.3 (ge) added
        value->is_const = decl->is_const;

        // dependency tracking: remember the code position of the DECL | 1.5.0.8
        // NOTE track if context-global and not global, or class-member
        if( (value->is_context_global && !value->is_global ) || value->is_member )
            value->depend_init_where = var_decl->where;

        // remember the value
        var_decl->value = value;

        // the next var decl
        list = list->next;
        // 1.4.2.0 (ge) | added
        is_first_in_list = FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_decl( )
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_decl( Chuck_Env * env, a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;

    // loop through the variables
    while( list != NULL )
    {
        // get the decl
        var_decl = list->var_decl;

        // check if array
        if( var_decl->array != NULL )
        {
            // verify there are no errors from the parser...
            if( !verify_array( var_decl->array ) )
                return FALSE;

            if( var_decl->array->exp_list )
            {
                // scan2 the exp
                if( !type_engine_scan2_exp( env, var_decl->array->exp_list ) )
                    return FALSE;
                // make sure types are of int
                if( !type_engine_scan2_array_subscripts( env, var_decl->array->exp_list ) )
                    return FALSE;
            }
        }

        // advance
        list = list->next;
    }

    // check whether decl type is auto | 1.5.1.1
    if( !isa(decl->ck_type, env->ckt_auto) )
    {
        // not auto? go ahead and create the decl
        if( !type_engine_scan2_exp_decl_create( env, decl ) )
            return FALSE;
    }
    else // decl type is auto
    {
        // disallow 'auto' for top-level class member variables
        if( env->class_def && env->class_scope == 0 )
        {
            EM_error2( decl->type->where,
                "cannot use 'auto' type for top-level class variables..." );
            return FALSE;
        }

        // nothing more to do for now
        // defer decl creation until chuck_type has a chance to infer the auto type
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_func_call( Chuck_Env * env, a_Exp exp_func, a_Exp args,
                                            t_CKFUNC & ck_func, int linepos )
{
    // Chuck_Func * func = NULL;
    // Chuck_Func * up = NULL;

    // type check the func
    t_CKBOOL f = type_engine_scan2_exp( env, exp_func );
    if( !f ) return FALSE;

    // check the arguments
    if( args )
    {
        t_CKBOOL a = type_engine_scan2_exp( env, args );
        if( !a ) return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_func_call( Chuck_Env * env, a_Exp_Func_Call func_call )
{
    // type check it
    return type_engine_scan2_exp_func_call( env, func_call->func, func_call->args,
                                           func_call->ck_func, func_call->where );
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_dot_member()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_dot_member( Chuck_Env * env, a_Exp_Dot_Member member )
{
    // type check the base
    t_CKBOOL base = type_engine_scan2_exp( env, member->base );
    if( !base ) return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_exp_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_exp_array( Chuck_Env * env, a_Exp_Array array )
{
    // verify there are no errors from the parser...
    if( !verify_array( array->indices ) )
        return FALSE;

    // type check the base
    t_CKBOOL base = type_engine_scan2_exp( env, array->base );
    if( !base ) return FALSE;

    // type check the index
    t_CKBOOL index = type_engine_scan2_exp( env, array->indices->exp_list );
    if( !index ) return FALSE;

    // cycle through each exp
    // a_Exp e = array->indices->exp_list;
    // count the dimension
    // t_CKUINT depth = 0;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_class_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_class_def( Chuck_Env * env, a_Class_Def class_def )
{
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;
    // the class
    Chuck_Type * the_class = class_def->type;
    // make sure
    assert( the_class != NULL );

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
            // already flagged as having a constructor or not
            ret = type_engine_scan2_stmt_list( env, body->section->stmt_list );
            break;

        case ae_section_func:
            // do function
            ret = type_engine_scan2_func_def( env, body->section->func_def );
            break;

        case ae_section_class:
            // do the class
            ret = type_engine_scan2_class_def( env, body->section->class_def );
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

    // the parent class | 1.5.1.5 (ge) moved from chuck_type module for earlier info
    t_CKTYPE t_parent = NULL;

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
                EM_error2( class_def->ext->extend_id->where,
                    "undefined super class '%s' in definition of class '%s'",
                    type_path(class_def->ext->extend_id), S_name(class_def->name->xid) );
                return FALSE;
            }

            // must not be primitive
            if( isprim( env, t_parent ) )
            {
                EM_error2( class_def->ext->extend_id->where,
                    "cannot extend primitive type '%s'",
                    t_parent->c_name() );
                EM_error2( 0, "...(primitive types: 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
                return FALSE;
            }
        }

        // TODO: interface
    }

    // by default object
    if( !t_parent ) t_parent = env->ckt_object;

    // check for fun
    assert( env->context != NULL );
    assert( class_def->type != NULL );
    assert( class_def->type->info != NULL );

    // retrieve the new type (created in scan_class_def)
    the_class = class_def->type;

    // set fields not set in scan
    the_class->parent = t_parent; CK_SAFE_ADD_REF(t_parent);
    // inherit ugen_info data from parent PLD
    the_class->ugen_info = t_parent->ugen_info; CK_SAFE_ADD_REF(t_parent->ugen_info);

    return ret;
}




//-----------------------------------------------------------------------------
// name: type_engine_scan2_func_def()
// desc: type scan 2 for function definition
//-----------------------------------------------------------------------------
t_CKBOOL type_engine_scan2_func_def( Chuck_Env * env, a_Func_Def f )
{
    Chuck_Type * type = NULL;
    Chuck_Value * value = NULL;
    Chuck_Func * func = NULL;

    // Chuck_Type * parent = NULL;
    Chuck_Value * overload = NULL;
    // Chuck_Value * override = NULL;
    Chuck_Value * v = NULL;
    // Chuck_Func * parent_func = NULL;
    a_Arg_List arg_list = NULL;
    // t_CKBOOL parent_match = FALSE;
    string func_name = S_name(f->name);
    string orig_name = func_name;
    vector<Chuck_Value *> values;
    vector<a_Arg_List> symbols;
    t_CKUINT count = 0;
    Chuck_Func * overfunc = NULL;
    // t_CKBOOL has_code = FALSE;  // use this for both user and imported
    t_CKBOOL op_overload = ( f->op2overload != ae_op_none );
    // is in constructor? | 1.5.2.0
    t_CKBOOL isInCtor = isctor(env,f);
    // is in destructor? | 1.5.2.0
    t_CKBOOL isInDtor = isdtor(env,f);

    // see if we are already in a function definition
    if( env->func != NULL )
    {
        EM_error2( f->where,
            "nested function definitions are not (yet) allowed" );
        return FALSE;
    }

    // make sure a code segment is in stmt - else we should push scope
    assert( !f->code || f->code->s_type == ae_stmt_code );

    // look up the value in the current class (can shadow?)
    overload = env->curr->lookup_value( f->name, FALSE );
    if( overload )
    {
        // if value
        if( !isa( overload->type, env->ckt_function ) )
        {
            EM_error2( f->where,
                "function name '%s' is already used by another value", S_name(f->name) );
            return FALSE;
        }
        else
        {
            // overload
            if( !overload->func_ref )
            {
                // error
                EM_error2( f->where,
                    "(internal error) missing function '%s'",
                    overload->name.c_str() );
                return FALSE;
            }
        }
    }

    // if overload
    if( overload )
    {
        // make the new name
        func_name += "@" + ck_itoa( ++overload->func_num_overloads ) + "@" + env->curr->name;
    }
    else
    {
        // make name using 0
        func_name += "@0@" + env->curr->name;
    }

    // make sure a code segment is in stmt - else we should push scope
    assert( !f->code || f->code->s_type == ae_stmt_code );

    // make a new func object
    func = env->context->new_Chuck_Func();
    // set the name
    func->name = func_name;
    // set the base name (i.e., without designations); 1.4.1.0
    func->base_name = orig_name;
    // reference the function definition
    func->funcdef_connect( f ); // 1.5.0.5 (ge) part of new AST cleanup
    // note whether the function is marked as member
    func->is_member = (f->static_decl != ae_key_static) &&
                      (env->class_def != NULL);
    // note whether the function is marked as static (in class)
    func->is_static = (f->static_decl == ae_key_static) &&
                      (env->class_def != NULL);

    // always create the code (will be filled in later on, depending on builtin vs in-language etc.)
    // 1.5.2.0 (ge) moved up from ae_func_builtin
    func->code = new Chuck_VM_Code;
    // add reference
    CK_SAFE_ADD_REF( func->code );
    // add name | 1.5.2.0
    func->code->name = S_name(f->name);
    // copy the native code, for imported functions
    if( f->s_type == ae_func_builtin )
    {
        // whether the function needs 'this'
        func->code->need_this = func->is_member;
        // is static inside
        func->code->is_static = func->is_static;
        // set the function pointer
        func->code->native_func = (t_CKUINT)func->def()->dl_func_ptr;
        // set the function pointer kind | 1.5.2.0
        func->code->native_func_kind = func->def()->dl_fp_kind;
    }

    // make a new type for the function
    type = env->context->new_Chuck_Type( env );
    type->xid = te_function;
    type->base_name = "[function]";
    type->parent = env->ckt_function; // TODO: reference count the parent
    type->size = sizeof(void *);
    type->func = func;

    // make new value, with potential overloaded name
    value = env->context->new_Chuck_Value( type, func_name );
    // it is const
    value->is_const = TRUE;
    // remember the owner
    value->owner = env->curr; CK_SAFE_ADD_REF( value->owner );
    value->owner_class = env->class_def; CK_SAFE_ADD_REF( value->owner_class );
    value->is_member = func->is_member;
    // is global context
    value->is_context_global = env->class_def == NULL;
    // remember the func
    value->func_ref = func; CK_SAFE_ADD_REF( value->func_ref ); // add reference TODO: break cycle?
    // remember the value
    func->value_ref = value; CK_SAFE_ADD_REF( func->value_ref ); // add reference TODO: break cycle?

    // set the func
    f->ck_func = func; func->add_ref(); // add reference

    // if overload
    if( overload )
    {
        // add
        func->next = overload->func_ref->next;
        overload->func_ref->next = func;
    }

    // make sure
    assert( f->ret_type != NULL );

    // primitive
    if( (isprim( env, f->ret_type ) /*|| isa( f->ret_type, &t_string )*/)
        && f->type_decl->ref )  // TODO: string
    {
        EM_error2( f->type_decl->where,
            "cannot declare references (@) of primitive type '%s'...",
            f->ret_type->c_name() );
        EM_error2( f->type_decl->where,
            "...(primitive types: 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
        goto error;
    }

    // look up types for the function arguments
    arg_list = f->arg_list;
    // count
    count = 1;
    // make room for 'this'
    if( func->is_member )
    {
        f->stack_depth = sizeof(void *);
    }
    else if( func->is_static ) // 1.4.1.0 (ge) added
    {
        // make room for '@type' for static functions inside class def
        f->stack_depth = sizeof(void *);
    }

    // loop over arguments
    while( arg_list )
    {
        // make sure the type is not NULL - it should have been
        // set in scan1_func_def()...
        assert( arg_list->type != NULL );

        // make sure it's not void
        if( arg_list->type->size == 0 )
        {
            EM_error2( arg_list->where,
                "cannot declare variables of size '0' (i.e. 'void')..." );
            goto error;
        }

        // check if reserved
        if( type_engine_check_reserved( env, arg_list->var_decl->xid, arg_list->where ) )
        {
            EM_error2( arg_list->where, "in function '%s'", func->signature().c_str() ); // S_name(f->name) );
            goto error;
        }

        // look up in scope: later
        //if( env->curr->lookup_value( arg_list->var_decl->xid, FALSE ) )
        //{
        //    EM_error2( arg_list->where, "in function '%s':", S_name(f->name) );
        //    EM_error2( arg_list->where, "argument %i '%s' is already defined in this scope",
        //        count, S_name(arg_list->var_decl->xid) );
        //    goto error;
        //}

        // primitive
        if( (isprim( env, arg_list->type ) /*|| isa( arg_list->type, &t_string )*/)
            && arg_list->type_decl->ref )  // TODO: string
        {
            EM_error2( arg_list->type_decl->where,
                "cannot declare references (@) of primitive type '%s'...",
                arg_list->type->c_name() );
            EM_error2( arg_list->type_decl->where,
                "...(primitive types: 'int', 'float', 'time', 'dur', 'vec3', etc.)" );
            goto error;
        }

        // check if array
        if( arg_list->var_decl->array != NULL )
        {
            // verify there are no errors from the parser...
            if( !verify_array( arg_list->var_decl->array ) )
                return FALSE;

            Chuck_Type * t = arg_list->type;
            Chuck_Type * t2 = t;
            // should be partial and empty []
            if( arg_list->var_decl->array->exp_list )
            {
                EM_error2( arg_list->where, "in function '%s':", func->signature().c_str() ); // S_name(f->name) );
                EM_error2( arg_list->where, "argument %i '%s' must be defined with empty []'s",
                    count, S_name(arg_list->var_decl->xid) );
                goto error; // return FALSE;
            }

            // create the new array type
            t = new_array_type(
                env,  // the env
                env->ckt_array,  // the array base class
                arg_list->var_decl->array->depth,  // the depth of the new type
                t2,  // the 'array_type'
                env->curr  // the owner namespace
            );

            // set ref
            arg_list->type_decl->ref = TRUE;
            // set type : arg_list->type = t;
            CK_SAFE_REF_ASSIGN( arg_list->type, t );
        }

        // make new value
        v = env->context->new_Chuck_Value(
            arg_list->type, S_name(arg_list->var_decl->xid) );
        // remember the owner
        v->owner = env->curr; CK_SAFE_ADD_REF( v->owner );
        // function args not owned
        v->owner_class = NULL; CK_SAFE_ADD_REF( v->owner_class );
        v->is_member = FALSE;
        v->is_const = FALSE; // 1.5.1.3 (ge) added explicitly for future reference
        // later: add as value
        // symbols.push_back( arg_list );
        // values.push_back( v );
        // later: env->curr->value.add( arg_list->var_decl->xid, v );

        // stack
        v->offset = f->stack_depth;
        f->stack_depth += arg_list->type->size;

        // remember
        arg_list->var_decl->value = v;

        // count
        count++;
        // next arg
        arg_list = arg_list->next;
    }

    // if overloading
    if( overload != NULL )
    {
        // -----------------------
        // 1.5.2.0 (ge) reinstating ability for overloaded funcs
        // (including operators) to return different types
        // -----------------------
        // make sure return types match
        // 1.5.0.0 (ge) more precise error reporting
        // -----------------------
        // if( *(f->ret_type) != *(overload->func_ref->def()->ret_type) )
        // {
        //    EM_error2( f->where, "overloaded functions require matching return types..." );
        //    // check if in class definition
        //    if( env->class_def )
        //    {
        //        EM_error3( "    |- function in question: %s %s.%s(...)",
        //                   func->def()->ret_type->base_name.c_str(), env->class_def->c_name(), S_name(f->name) );
        //        EM_error3( "    |- previous defined as: %s %s.%s(...)",
        //                   overload->func_ref->def()->ret_type->base_name.c_str(), env->class_def->c_name(), S_name(f->name) );
        //    }
        //    else
        //    {
        //        EM_error3( "    |- function in question: %s %s(...)",
        //                   func->def()->ret_type->base_name.c_str(), S_name(f->name) );
        //        EM_error3( "    |- previous defined as: %s %s(...)",
        //                   overload->func_ref->def()->ret_type->base_name.c_str(), S_name(f->name) );
        //    }
        //    goto error;
        // }

        // -----------------------
        // make sure not duplicate
        // 1.5.0.0 (ge) added
        // -----------------------
        overfunc = overload->func_ref;
        // loop over overloaded functions
        while( overfunc != NULL )
        {
            // one of these could this newly defined function
            if( func != overfunc )
            {
                // compare argument lists
                a_Arg_List lhs = func->def()->arg_list;
                a_Arg_List rhs = overfunc->def()->arg_list;
                // check
                if( same_arg_lists(lhs, rhs) )
                {
                    ae_Operator op = f->op2overload;
                    // check if this is an overloading of both post and pre (e.g., -- and ++)
                    if( op_overload && env->op_registry.unaryPreOverloadable(op) &&
                                       env->op_registry.unaryPostOverloadable(op) )
                    {
                        // check what kind of unary overload
                        if( (f->overload_post && env->op_registry.lookup_overload(lhs->type,op) == NULL )
                           || (!f->overload_post && env->op_registry.lookup_overload(op,lhs->type) ) )
                        {
                            // next overfunc
                            overfunc = overfunc->next;
                            // keep going, this is ok
                            continue;
                        }
                    }

                    EM_error2( f->where, "cannot overload %s with identical arguments...", isInCtor ? "constructor" : "function" );
                    if( env->class_def )
                    {
                        EM_error3( "    |- '%s %s.%s(%s)' already defined elsewhere",
                                   func->def()->ret_type->base_name.c_str(), env->class_def->c_name(),
                                   orig_name.c_str(), arglist2string(func->def()->arg_list).c_str() );
                        // if a constructor definition
                        if( isInCtor )
                        {
                            EM_error3( "    |- (hint: possibly as '@construct(%s)' in class '%s')",
                                       arglist2string(func->def()->arg_list).c_str(), env->class_def->c_name() );
                        }
                    }
                    else
                    {
                        EM_error3( "    |- '%s %s(%s)' already defined elsewhere",
                                   func->def()->ret_type->base_name.c_str(), orig_name.c_str(),
                                   arglist2string(func->def()->arg_list).c_str() );
                    }
                    goto error;
                }
            }
            // next overloaded function
            overfunc = overfunc->next;
        }
    }

    // constructor | 1.5.2.0 (ge) added
    if( isInCtor )
    {
        // set constructor flag
        func->is_ctor = TRUE;
        // set in class as constructor; if not NULL, then a ctor is already set, and this is an overloading
        if( env->class_def->ctors_all == NULL )
        {
            CK_SAFE_REF_ASSIGN( env->class_def->ctors_all, func );
        }
        // check if default constructor
        func->is_default_ctor = (f->arg_list == NULL);
        // if no arguments, then set as base constructor
        if( func->is_default_ctor )
        {
            CK_SAFE_REF_ASSIGN( env->class_def->ctor_default, func );
        }
    }

    // destructor | 1.5.2.0 (ge) added
    if( isInDtor )
    {
        // verify
        if( env->class_def->dtor_the != NULL )
        {
            EM_error2( f->where, "(internal error): unexpected destructor found in '%s'...", env->class_def->c_name() );
            return FALSE;
        }
        // set destructor flag
        func->is_dtor = TRUE;
        // set in class as destructor
        CK_SAFE_REF_ASSIGN( env->class_def->dtor_the, func );
        // set invoker
        env->class_def->dtor_invoker = new Chuck_VM_DtorInvoker;
        // init invoker
        env->class_def->dtor_invoker->setup( func, env->vm() );
    }

    // operator overload | 1.5.1.5 (ge) added
    if( op_overload )
    {
        // scan operator overload
        if( !type_engine_scan_func_op_overload( env, f ) )
            return FALSE;
    }

    // add as value
    env->curr->value.add( value->name, value );
    // enter the name into the function table
    env->curr->func.add( func->name, func );

    // if not overload, add entries for orig name
    if( !overload )
    {
        env->curr->value.add( orig_name, value );
        env->curr->func.add( orig_name, func );
    }

    // set the current function to this
    env->func = func;
    // push the value stack
    env->curr->value.push();

    // scan the code
    assert( f->code == NULL || f->code->s_type == ae_stmt_code );
    if( f->code && !type_engine_scan2_code_segment( env, &f->code->stmt_code, FALSE ) )
    {
        EM_error2( 0, "...in function '%s'", func->signature().c_str() ); // S_name(f->name) );
        goto error;
    }

    // pop the value stack
    env->curr->value.pop();
    // clear the env's function definition
    env->func = NULL;

    return TRUE;

error:

    // clean up
    if( func )
    {
        // reset
        env->func = NULL;
        // break func_ref <-> value_ref cycle, at least here
        CK_SAFE_RELEASE( func->value_ref );
        // value, type, func shouldn't be released here | 1.5.1.3
        // should be cleanup with the overall context
        // CK_SAFE_RELEASE(value); CK_SAFE_RELEASE(type); CK_SAFE_RELEASE(func);
    }

    return FALSE;
}
