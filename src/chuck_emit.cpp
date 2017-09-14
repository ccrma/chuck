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
// file: chuck_emit.cpp
// desc: chuck instruction emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - first version
//       Autumn 2004 - redesign
//-----------------------------------------------------------------------------
#include "chuck_emit.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include <sstream>
#include <iostream>

using namespace std;




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt_list( Chuck_Emitter * emit, a_Stmt_List list );
t_CKBOOL emit_engine_emit_stmt( Chuck_Emitter * emit, a_Stmt stmt, t_CKBOOL pop = TRUE );
t_CKBOOL emit_engine_emit_if( Chuck_Emitter * emit, a_Stmt_If stmt );
t_CKBOOL emit_engine_emit_for( Chuck_Emitter * emit, a_Stmt_For stmt );
t_CKBOOL emit_engine_emit_while( Chuck_Emitter * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_do_while( Chuck_Emitter * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_until( Chuck_Emitter * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_do_until( Chuck_Emitter * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_loop( Chuck_Emitter * emit, a_Stmt_Loop stmt );
t_CKBOOL emit_engine_emit_break( Chuck_Emitter * emit, a_Stmt_Break br );
t_CKBOOL emit_engine_emit_continue( Chuck_Emitter * emit, a_Stmt_Continue cont );
t_CKBOOL emit_engine_emit_return( Chuck_Emitter * emit, a_Stmt_Return stmt );
t_CKBOOL emit_engine_emit_switch( Chuck_Emitter * emit, a_Stmt_Switch stmt );
t_CKBOOL emit_engine_emit_exp( Chuck_Emitter * emit, a_Exp exp, t_CKBOOL doAddRef = FALSE );
t_CKBOOL emit_engine_emit_exp_binary( Chuck_Emitter * emit, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op( Chuck_Emitter * emit, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op_unchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_op_upchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_op_at_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emitter * emit, a_Exp_Unary unary );
t_CKBOOL emit_engine_emit_exp_primary( Chuck_Emitter * emit, a_Exp_Primary exp );
t_CKBOOL emit_engine_emit_exp_cast( Chuck_Emitter * emit, a_Exp_Cast cast );
t_CKBOOL emit_engine_emit_exp_postfix( Chuck_Emitter * emit, a_Exp_Postfix postfix );
t_CKBOOL emit_engine_emit_exp_dur( Chuck_Emitter * emit, a_Exp_Dur dur );
t_CKBOOL emit_engine_emit_exp_array( Chuck_Emitter * emit, a_Exp_Array array );
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emitter * emit, Chuck_Func * func,
                                         Chuck_Type * type, int linepos, t_CKBOOL spork = FALSE );
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emitter * emit, a_Exp_Func_Call func_call,
                                         t_CKBOOL spork = FALSE );
t_CKBOOL emit_engine_emit_func_args( Chuck_Emitter * emit, a_Exp_Func_Call func_call );
t_CKBOOL emit_engine_emit_exp_dot_member( Chuck_Emitter * emit, a_Exp_Dot_Member member );
t_CKBOOL emit_engine_emit_exp_if( Chuck_Emitter * emit, a_Exp_If exp_if );
t_CKBOOL emit_engine_emit_exp_decl( Chuck_Emitter * emit, a_Exp_Decl decl, t_CKBOOL first_exp );
t_CKBOOL emit_engine_emit_array_lit( Chuck_Emitter * emit, a_Array_Sub array );
t_CKBOOL emit_engine_emit_complex_lit( Chuck_Emitter * emit, a_Complex val );
t_CKBOOL emit_engine_emit_polar_lit( Chuck_Emitter * emit, a_Polar val );
t_CKBOOL emit_engine_emit_vec_lit( Chuck_Emitter * emit, a_Vec val );
t_CKBOOL emit_engine_emit_code_segment( Chuck_Emitter * emit, a_Stmt_Code stmt,
                                        t_CKBOOL push = TRUE );
t_CKBOOL emit_engine_emit_func_def( Chuck_Emitter * emit, a_Func_Def func_def );
t_CKBOOL emit_engine_emit_class_def( Chuck_Emitter * emit, a_Class_Def class_def );
t_CKBOOL emit_engine_pre_constructor( Chuck_Emitter * emit, Chuck_Type * type );
t_CKBOOL emit_engine_instantiate_object( Chuck_Emitter * emit, Chuck_Type * type,
                                         a_Array_Sub array, t_CKBOOL is_ref );
t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Exp_Func_Call exp );
t_CKBOOL emit_engine_emit_cast( Chuck_Emitter * emit, Chuck_Type * to, Chuck_Type * from );
t_CKBOOL emit_engine_emit_symbol( Chuck_Emitter * emit, S_Symbol symbol, 
                                  Chuck_Value * v, t_CKBOOL emit_var, int linepos );
// disabled until further notice (added 1.3.0.0)
// t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Stmt stmt );




//-----------------------------------------------------------------------------
// name: emit_engine_init()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Emitter * emit_engine_init( Chuck_Env * env )
{
    // log
    EM_log( CK_LOG_SEVERE, "initializing emitter..." );

    // TODO: ensure this in a better way
    assert( sizeof(t_CKUINT) == sizeof(void *) );

    // allocate new emit
    Chuck_Emitter * emit = new Chuck_Emitter;
    // set the reference
    emit->env = env;

    return emit;
}




//-----------------------------------------------------------------------------
// name: emit_engine_shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_shutdown( Chuck_Emitter *& emit )
{
    if( !emit ) return FALSE;

    // log
    EM_log( CK_LOG_SYSTEM, "shutting down emitter..." );

    // delete
    delete emit;
    emit = NULL;

    return TRUE;    
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_prog()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Code * emit_engine_emit_prog( Chuck_Emitter * emit, a_Program prog,
                                       te_HowMuch how_much )
{
    // make sure the code is NULL
    assert( emit->code == NULL );
    // make sure the stack is empty
    assert( emit->stack.size() == 0 );
    // make sure there is a context to emit
    assert( emit->env->context != NULL );
    // make sure no code
    assert( emit->env->context->nspc->pre_ctor == NULL );

    // log
    EM_log( CK_LOG_FINER, "(pass 4) emitting VM code '%s'...",
        emit->env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    // log how much
    EM_log( CK_LOG_FINER, "target: %s", howmuch2str( how_much ) );

    // return
    t_CKBOOL ret = TRUE;
    // allocate the code
    emit->code = new Chuck_Code;
    // set the current context
    emit->context = emit->env->context;
    // set the namespace
    emit->nspc = emit->context->nspc;
    // reset the func
    emit->func = NULL;
    // clear the code stack
    emit->stack.clear();
    // name the code
    // emit->code->name = "(main)";
    // whether code need this
    emit->code->need_this = TRUE;
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;
    // push global scope (added 1.3.0.0)
    emit->push_scope();

    // loop over the program sections
    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt: // code section
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // emit statement list
            ret = emit_engine_emit_stmt_list( emit, prog->section->stmt_list );
            break;

        case ae_section_func: // function definition
            // if only classes, then skip
            if( how_much == te_do_classes_only ) break;
            // check function definition
            ret = emit_engine_emit_func_def( emit, prog->section->func_def );
            break;

        case ae_section_class: // class definition
            // if no classes, then skip
            if( how_much == te_do_no_classes ) break;
            // check class definition
            ret = emit_engine_emit_class_def( emit, prog->section->class_def );
            break;

        default: // bad
            EM_error2( 0,
                "internal compiler error: unrecognized program section..." );
            ret = FALSE;
            break;
        }

        // the next
        prog = prog->next;
    }

    if( ret )
    {
        // pop global scope (added 1.3.0.0)
        emit->pop_scope();
        // append end of code
        emit->append( new Chuck_Instr_EOC );
        // make sure
        assert( emit->context->nspc->pre_ctor == NULL );
        // converted to virtual machine code
        emit->context->nspc->pre_ctor = emit_to_code( emit->code, NULL, emit->dump );
        // add reference
        emit->context->nspc->pre_ctor->add_ref();
    }

    // clear the code
    delete emit->code;
    emit->code = NULL;

    // pop indent
    EM_poplog();

    // return the code
    return emit->context->nspc->pre_ctor;
}




//-----------------------------------------------------------------------------
// name: emit_to_code()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_VM_Code * emit_to_code( Chuck_Code * in,
                              Chuck_VM_Code * out,
                              t_CKBOOL dump )
{
    // log
    EM_log( CK_LOG_FINER, "emitting code: %d VM instructions...",
            in->code.size() );
    // allocate the vm code
    Chuck_VM_Code * code = out ? out : new Chuck_VM_Code;
    // make sure
    assert( code->num_instr == 0 );
    // size
    code->num_instr = in->code.size();
    // allocate instruction pointers+
    code->instr = new Chuck_Instr *[code->num_instr];
    // set the stack depth
    code->stack_depth = in->stack_depth;
    // set whether code need this base pointer
    code->need_this = in->need_this;
    // set name
    code->name = in->name;
    // set filename for me.sourceDir() (added 1.3.0.0)
    code->filename = in->filename;

    // copy
    for( t_CKUINT i = 0; i < code->num_instr; i++ )
        code->instr[i] = in->code[i];

    // dump
    if( dump )
    {
        // name of what we are dumping
        EM_error2( 0, "dumping %s:", in->name.c_str() );

        // uh
        EM_error2( 0, "-------" );
        for( t_CKUINT i = 0; i < code->num_instr; i++ )
            EM_error2( 0, "[%i] %s( %s )", i, 
               code->instr[i]->name(), code->instr[i]->params() );
        EM_error2( 0, "-------\n" );
    }

    return code;
}




//-----------------------------------------------------------------------------
// name:
// desc: ...
//-----------------------------------------------------------------------------
// t_CKBOOL emit_engine_addr_map( Chuck_Emitter * emit, Chuck_VM_Shred * shred );




//-----------------------------------------------------------------------------
// name:
// desc: ...
//-----------------------------------------------------------------------------
// t_CKBOOL emit_engine_resolve( );




//-----------------------------------------------------------------------------
// name: emit_engine_emit_stmt_list()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt_list( Chuck_Emitter * emit, a_Stmt_List list )
{
    t_CKBOOL ret = TRUE;

    // emit->push();
    while( list && ret )
    {
        ret = emit_engine_emit_stmt( emit, list->stmt );
        list = list->next;
    }
    // emit->pop();

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_stmt()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt( Chuck_Emitter * emit, a_Stmt stmt, t_CKBOOL pop )
{
    // empty stmt list
    if( !stmt )
        return TRUE;

    // return
    t_CKBOOL ret = TRUE;

    // loop over statements
    switch( stmt->s_type )
    {
        case ae_stmt_exp:  // expression statement
            // emit it
            ret = emit_engine_emit_exp( emit, stmt->stmt_exp );
            if( !ret )
                return FALSE;
            // need to pop the final value from stack
            if( ret && pop && stmt->stmt_exp->type->size > 0 )
            {
                // sanity
                assert( stmt->stmt_exp->cast_to == NULL );

                // exp
                a_Exp exp = stmt->stmt_exp;
                // hack
                if( exp->s_type == ae_exp_primary && exp->primary.s_type == ae_primary_hack )
                    exp = exp->primary.exp;

                // HACK!
                while( exp )
                {
                    // if decl, then expect only one word per var
                    // added 1.3.1.0: iskindofint -- since on some 64-bit systems sz_INT == sz_FLOAT
                    if( exp->s_type == ae_exp_decl )
                        // (added 1.3.1.0 -- multiply by type size; #64-bit)
                        // (fixed 1.3.1.2 -- uh... decl should also be int-sized, so changed to INT/WORD)
                        emit->append( new Chuck_Instr_Reg_Pop_Word4( exp->decl.num_var_decls * sz_INT / sz_WORD ) );
                    else if( exp->type->size == sz_INT && iskindofint(emit->env, exp->type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                        emit->append( new Chuck_Instr_Reg_Pop_Word );
                    else if( exp->type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                        emit->append( new Chuck_Instr_Reg_Pop_Word2 );
                    else if( exp->type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                        emit->append( new Chuck_Instr_Reg_Pop_Word3 );
                    else if( exp->type->size == sz_VEC3 ) // ge: added 1.3.5.3
                        emit->append( new Chuck_Instr_Reg_Pop_Word4(sz_VEC3/sz_WORD) );
                    else if( exp->type->size == sz_VEC4 ) // ge: added 1.3.5.3
                        emit->append( new Chuck_Instr_Reg_Pop_Word4(sz_VEC4/sz_WORD) );
                    else
                    {
                        EM_error2( exp->linepos,
                                   "(emit): internal error: %i byte stack item unhandled...",
                                   exp->type->size );
                        return FALSE;
                    }

                    // go
                    exp = exp->next;
                }
            }
            break;

        case ae_stmt_if:  // if statement
            ret = emit_engine_emit_if( emit, &stmt->stmt_if );
            break;

        case ae_stmt_for:  // for statement
            ret = emit_engine_emit_for( emit, &stmt->stmt_for );
            break;

        case ae_stmt_while:  // while statement
            if( stmt->stmt_while.is_do )
                ret = emit_engine_emit_do_while( emit, &stmt->stmt_while );
            else
                ret = emit_engine_emit_while( emit, &stmt->stmt_while );
            break;
        
        case ae_stmt_until:  // until statement
            if( stmt->stmt_until.is_do )
                ret = emit_engine_emit_do_until( emit, &stmt->stmt_until );
            else
                ret = emit_engine_emit_until( emit, &stmt->stmt_until );
            break;

        case ae_stmt_loop:  // loop statement
            ret = emit_engine_emit_loop( emit, &stmt->stmt_loop );
            break;

        case ae_stmt_switch:  // switch statement
            // not implemented
            ret = FALSE;
            break;

        case ae_stmt_break:  // break statement
            ret = emit_engine_emit_break( emit, &stmt->stmt_break );
            break;

        case ae_stmt_code:  // code segment
            ret = emit_engine_emit_code_segment( emit, &stmt->stmt_code );
            break;
            
        case ae_stmt_continue:  // continue statement
            ret = emit_engine_emit_continue( emit, &stmt->stmt_continue );
            break;
            
        case ae_stmt_return:  // return statement
            ret = emit_engine_emit_return( emit, &stmt->stmt_return );
            break;
            
        case ae_stmt_case:  // case statement
            // not implemented
            ret = FALSE;
            // ret = emit_engine_emit_case( emit, &stmt->stmt_case );
            break;
       
        case ae_stmt_gotolabel:  // goto label statement
            // not implemented
            ret = FALSE;
            // ret = emit_engine_emit_gotolabel( emit, &stmt->stmt_case );
            break;
        
        default:  // bad
            EM_error2( stmt->linepos, 
                 "(emit): internal error: unhandled statement type '%i'...",
                 stmt->s_type );
            break;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_if( Chuck_Emitter * emit, a_Stmt_If stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL, * op2 = NULL;

    // push the stack, allowing for new local variables
    emit->push_scope();

    // emit the condition
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;

    // type of the condition
    switch( stmt->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Eq_double( 0 );
        break;
        
    default:
        // check for IO
        if( isa( stmt->cond->type, emit->env->t_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->linepos,
            "(emit): internal error: unhandled type '%s' in if condition",
            stmt->cond->type->name.c_str() );
        return FALSE;
    }

    if( !ret ) return FALSE;

    // append the op
    emit->append( op );

    // ge: 2012 april: scope for if body (added 1.3.0.0)
    {
        // push the stack, allowing for new local variables
        emit->push_scope();
        // emit the body
        ret = emit_engine_emit_stmt( emit, stmt->if_body );
        if( !ret )
            return FALSE;
        // pop stack
        emit->pop_scope();
    }

    // emit the skip to the end
    emit->append( op2 = new Chuck_Instr_Goto(0) );
    
    // set the op's target
    op->set( emit->next_index() );

    // ge: 2012 april: scope for else body (added 1.3.0.0)
    {
        // push the stack, allowing for new local variables
        emit->push_scope();
        // emit the body
        ret = emit_engine_emit_stmt( emit, stmt->else_body );
        if( !ret )
            return FALSE;
        // pop stack
        emit->pop_scope();
    }

    // set the op2's target
    op2->set( emit->next_index() );

    // pop stack
    emit->pop_scope();
    
    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_for()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_for( Chuck_Emitter * emit, a_Stmt_For stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;

    // push the stack
    emit->push_scope();

    // emit the cond
    ret = emit_engine_emit_stmt( emit, stmt->c1 );
    if( !ret )
        return FALSE;

    // the start index
    t_CKUINT start_index = emit->next_index();
    t_CKUINT cont_index = 0;
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // emit the cond - keep the result on the stack
    emit_engine_emit_stmt( emit, stmt->c2, FALSE );

    // could be NULL
    if( stmt->c2 )
    {
        switch( stmt->c2->stmt_exp->type->xid )
        {
        case te_int:
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int( 0 );
            break;
        case te_float:
        case te_dur:
        case te_time:
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
            op = new Chuck_Instr_Branch_Eq_double( 0 );
            break;
        
        default:
            // check for IO
            if( isa( stmt->c2->stmt_exp->type, emit->env->t_io ) )
            {
                // push 0
                emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
                op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
                break;
            }

            EM_error2( stmt->c2->stmt_exp->linepos,
                "(emit): internal error: unhandled type '%s' in for conditional",
                stmt->c2->stmt_exp->type->name.c_str() );
            return FALSE;
        }
        // append the op
        emit->append( op );
    }

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();
        
    // continue here
    cont_index = emit->next_index();

    // emit the action
    if( stmt->c3 )
    {
        ret = emit_engine_emit_exp( emit, stmt->c3 );
        if( !ret )
            return FALSE;

        // HACK!
        // count the number of words to pop
        a_Exp e = stmt->c3;
        t_CKUINT num_words = 0;
        while( e )
        {
            if( e->type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                num_words += sz_FLOAT / sz_WORD; // changed to compute number of words; 1.3.1.0
            else if( e->type->size == sz_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                num_words += sz_INT / sz_WORD; // changed to compute number of words; 1.3.1.0
            else if( e->type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                num_words += sz_COMPLEX / sz_WORD; // changed to compute number of words; 1.3.1.0
            else if( e->type->size != 0 )
            {
                EM_error2( e->linepos,
                    "(emit): internal error: non-void type size %i unhandled...",
                    e->type->size );
                return FALSE;
            }

            // advance
            e = e->next;
        }
        
        // pop (changed to Chuck_Instr_Reg_Pop_Word4 in 1.3.1.0)
        if( num_words > 0 ) emit->append( new Chuck_Instr_Reg_Pop_Word4( num_words ) );
    }

    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );

    // could be NULL
    if( stmt->c2 )
        // set the op's target
        op->set( emit->next_index() );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( cont_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_while()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_while( Chuck_Emitter * emit, a_Stmt_While stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;

    // push stack
    emit->push_scope();

    // get the index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;
    
    // the condition
    switch( stmt->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Eq_double( 0 );
        break;
        
    default:
        // check for IO
        if( isa( stmt->cond->type, emit->env->t_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->linepos,
            "(emit): internal error: unhandled type '%s' in while conditional",
            stmt->cond->type->name.c_str() );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();
    
    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();
    
    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );
    
    // set the op's target
    op->set( emit->next_index() );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( start_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_do_while()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_do_while( Chuck_Emitter * emit, a_Stmt_While stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    t_CKUINT start_index = emit->next_index();
    
    // push stack
    emit->push_scope();

    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;
    
    // the condition
    switch( stmt->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Neq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Neq_double( 0 );
        break;
        
    default:
        // check for IO
        if( isa( stmt->cond->type, emit->env->t_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->linepos,
            "(emit): internal error: unhandled type '%s' in do/while conditional",
            stmt->cond->type->c_name() );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // set the op's target
    op->set( start_index );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( start_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();
    
    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_until()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_until( Chuck_Emitter * emit, a_Stmt_Until stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;

    // push stack
    emit->push_scope();

    // get index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;

    // condition
    switch( stmt->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Neq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Neq_double( 0 );
        break;
        
    default:
        // check for IO
        if( isa( stmt->cond->type, emit->env->t_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Neq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->linepos,
            "(emit): internal error: unhandled type '%s' in until conditional",
            stmt->cond->type->name.c_str() );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();

    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );
    
    // set the op's target
    op->set( emit->next_index() );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( start_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();
    
    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_do_until()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_do_until( Chuck_Emitter * emit, a_Stmt_Until stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;

    // push stack
    emit->push_scope();

    // the index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;

    // condition
    switch( stmt->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Eq_double( 0 );
        break;

    default:
        EM_error2( stmt->cond->linepos,
             "(emit): internal error: unhandled type '%s' in do/until conditional",
             stmt->cond->type->name.c_str() );
        return FALSE;
    }

    // append the op
    emit->append( op );

    // set the op's target
    op->set( start_index );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( start_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();
    
    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_loop()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_loop( Chuck_Emitter * emit, a_Stmt_Loop stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    
    Chuck_Instr * instr = NULL;

    // push stack
    emit->push_scope();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond );
    if( !ret )
        return FALSE;

    // initialize our loop counter (1.3.5.3)
    emit->append( new Chuck_Instr_Init_Loop_Counter() );

    // get the index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // push the value of the loop counter
    emit->append( instr = new Chuck_Instr_Reg_Push_Loop_Counter_Deref() );
    instr->set_linepos( stmt->linepos );

    // get the type, taking cast into account
    Chuck_Type * type = stmt->cond->cast_to ? stmt->cond->cast_to : stmt->cond->type;

    // the condition
    switch( type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_int( 0 );
        break;

    default:
        EM_error2( stmt->cond->linepos,
            "(emit): internal error: unhandled type '%s' in while conditional",
            type->name.c_str() );

        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // decrement the counter
    emit->append( instr = new Chuck_Instr_Dec_Loop_Counter() );
    instr->set_linepos( stmt->linepos );

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;
    
    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();
    
    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );
    
    // set the op's target
    op->set( emit->next_index() );
    
    // pop the loop counter
    emit->append( new Chuck_Instr_Pop_Loop_Counter );

    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( start_index );
        emit->code->stack_cont.pop_back();
    }

    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    // pop stack
    emit->pop_scope();
    // pop continue stack
    emit->code->stack_cont.pop_back();
    // pop break stack
    emit->code->stack_break.pop_back();

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_break()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_break( Chuck_Emitter * emit, a_Stmt_Break br )
{
    // append
    Chuck_Instr_Goto * op = new Chuck_Instr_Goto( 0 );
    emit->append( op );
    // remember
    emit->code->stack_break.push_back( op );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_continue()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_continue( Chuck_Emitter * emit, a_Stmt_Continue cont )
{
    // append
    Chuck_Instr_Goto * op = new Chuck_Instr_Goto( 0 );
    emit->append( op );
    // remember
    emit->code->stack_cont.push_back( op );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_return()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_return( Chuck_Emitter * emit, a_Stmt_Return stmt )
{
    if( !emit_engine_emit_exp( emit, stmt->val ) )
        return FALSE;

    // emit return
    // emit->append( new Chuck_Instr_Func_Return );

    // determine where later
    Chuck_Instr_Goto * instr = new Chuck_Instr_Goto( 0 );
    emit->append( instr );
    emit->code->stack_return.push_back( instr );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name:
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_switch( Chuck_Emitter * emit, a_Stmt_Switch stmt );




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp()
// desc: (doAddRef added 1.3.0.0 -- typically this is set to TRUE for function
//        calls so that pointers on the reg is accounted for;  this is important
//        in case the object is released/reclaimed before the value is used;
//        on particular case is when sporking with a local object as argument)
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp( Chuck_Emitter * emit, a_Exp exp, t_CKBOOL doAddRef )
{
    // for now...
    // assert( exp->next == NULL );

    // loop over 
    while( exp )
    {
        switch( exp->s_type )
        {
        case ae_exp_binary:
            if( !emit_engine_emit_exp_binary( emit, &exp->binary ) )
                return FALSE;
            break;

        case ae_exp_unary:
            if( !emit_engine_emit_exp_unary( emit, &exp->unary ) )
                return FALSE;
            break;

        case ae_exp_cast:
            if( !emit_engine_emit_exp_cast( emit, &exp->cast ) )
                return FALSE;
            break;

        case ae_exp_postfix:
            if( !emit_engine_emit_exp_postfix( emit, &exp->postfix ) )
                return FALSE;
            break;

        case ae_exp_dur:
            if( !emit_engine_emit_exp_dur( emit, &exp->dur ) )
                return FALSE;
            break;

        case ae_exp_primary:
            if( !emit_engine_emit_exp_primary( emit, &exp->primary ) )
                return FALSE;
            break;

        case ae_exp_array:
            if( !emit_engine_emit_exp_array( emit, &exp->array ) )
                return FALSE;
            break;

        case ae_exp_func_call:
            if( !emit_engine_emit_exp_func_call( emit, &exp->func_call ) )
                return FALSE;
            break;

        case ae_exp_dot_member:
            if( !emit_engine_emit_exp_dot_member( emit, &exp->dot_member ) )
                return FALSE;
            break;

        case ae_exp_if:
            if( !emit_engine_emit_exp_if( emit, &exp->exp_if ) )
                return FALSE;
            break;

        case ae_exp_decl:
            if( !emit_engine_emit_exp_decl( emit, &exp->decl, FALSE ) )
                return FALSE;
            break;

        //case ae_exp_namespace:
        //    if( !emit_engine_emit_exp_namespace( emit, &exp->name_space ) )
        //        return FALSE;
        //    break;

        default:
            EM_error2( exp->linepos, 
                 "(emit): internal error: unhandled expression type '%i'...",
                 exp->s_type );
            return FALSE;
        }

        // implicit cast
        if( exp->cast_to != NULL )
            if( !emit_engine_emit_cast( emit, exp->cast_to, exp->type ) )
                return FALSE;
        
        // check if we need to handle ref (added 1.3.0.0)
        // (NOTE: cast shouldn't matter since pointer width should remain constant)
        if( doAddRef && isobj( emit->env, exp->type ) )
        {
            // add ref in place on the stack
            emit->append( new Chuck_Instr_Reg_AddRef_Object3() );
        }

        exp = exp->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_binary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_binary( Chuck_Emitter * emit, a_Exp_Binary binary )
{
    // sanity
    assert( binary->self->emit_var == FALSE );

    t_CKBOOL left = FALSE;
    t_CKBOOL right = FALSE;
    
    // hack to allow && to terminate as soon as possible - spencer
    // i.e. in ( 0 && f() ) the function f() will not be called
    // (obviates need for boolean AND instruction)
    if( binary->op == ae_op_and )
    {
        Chuck_Instr_Branch_Op * op;
        
        // push default result
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        
        left = emit_engine_emit_exp( emit, binary->lhs );
        if( !left )
            return FALSE;

        // compare to 0; use default result if zero
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        emit->append( op = new Chuck_Instr_Branch_Eq_int( 0 ) );

        // pop default result
        emit->append( new Chuck_Instr_Reg_Pop_Word );

        // result of whole expression is now result of rhs
        right = emit_engine_emit_exp( emit, binary->rhs );
        if( !right )
            return FALSE;

        // set branch location
        op->set( emit->next_index() );
        
        return TRUE;
    }
    
    // hack to allow || to terminate as soon as possible - spencer
    // i.e. in ( 1 || f() ) the function f() will not be called
    // (obviates need for boolean OR instruction)
    else if( binary->op == ae_op_or )
    {
        Chuck_Instr_Branch_Op * op;
        
        // push default result
        emit->append( new Chuck_Instr_Reg_Push_Imm( 1 ) );
        
        left = emit_engine_emit_exp( emit, binary->lhs );
        if( !left )
            return FALSE;        
        
        // compare to 0; use default result if non-zero
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        emit->append( op = new Chuck_Instr_Branch_Neq_int( 0 ) );
        
        // pop default result
        emit->append( new Chuck_Instr_Reg_Pop_Word );
        
        // result of whole expression is now result of rhs
        right = emit_engine_emit_exp( emit, binary->rhs );
        if( !right )
            return FALSE;
        
        // set branch location
        op->set( emit->next_index() );
        
        return TRUE;
    }

    // whether to track object references on stack (added 1.3.0.2)
    t_CKBOOL doRef = FALSE;
    // check to see if this is a function call (added 1.3.0.2)
    if( isa( binary->rhs->type, emit->env->t_function ) )
    {
        // take care of objects in terms of reference counting
        doRef = TRUE;
    }

    // emit (doRef added 1.3.0.2)
    left = emit_engine_emit_exp( emit, binary->lhs, doRef );
    right = emit_engine_emit_exp( emit, binary->rhs );

    // check
    if( !left || !right )
        return FALSE;

    // emit the op
    if( !emit_engine_emit_op( emit, binary->op, binary->lhs, binary->rhs, binary ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op()
// desc: emit binary operator
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op( Chuck_Emitter * emit, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    // any implicit cast happens before this
    Chuck_Type * t_left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * t_right = rhs->cast_to ? rhs->cast_to : rhs->type;
    // get the types of the left and right
    te_Type left = t_left->xid;
    te_Type right = t_right->xid;
    // op
    Chuck_Instr * instr = NULL;

    // emit op
    switch( op )
    {    
    // ----------------------------- num --------------------------------------
    case ae_op_plus:
        // time + dur
        if( ( left == te_dur && right == te_time ) ||
            ( left == te_time && right == te_dur ) )
        {
            emit->append( instr = new Chuck_Instr_Add_double );
        }
        // string + string
        else if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
        {
            // concatenate
            emit->append( instr = new Chuck_Instr_Add_string );
            instr->set_linepos( lhs->linepos );
        }
        // left: string
        else if( isa( t_left, emit->env->t_string ) )
        {
            // + int
            if( isa( t_right, emit->env->t_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_string_int );
                instr->set_linepos( lhs->linepos );
            }
            else if( isa( t_right, emit->env->t_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_string_float );
                instr->set_linepos( lhs->linepos );
            }
            else
            {
                EM_error2( lhs->linepos,
                    "(emit): internal error: unhandled op '%s' %s '%s'",
                    t_left->c_name(), op2str( op ), t_right->c_name() );
                return FALSE;
            }
        }
        // right: string
        else if( isa( t_right, emit->env->t_string ) )
        {
            // + int
            if( isa( t_left, emit->env->t_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_int_string );
                instr->set_linepos( rhs->linepos );
            }
            else if( isa( t_left, emit->env->t_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_float_string );
                instr->set_linepos( rhs->linepos );
            }
            else
            {
                EM_error2( lhs->linepos,
                    "(emit): internal error: unhandled op '%s' %s '%s'",
                    t_left->c_name(), op2str( op ), t_right->c_name() );
                return FALSE;
            }
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Add_int );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Add_double );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Add_complex );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Add_polar );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Add_vec3 );
                break;
                case te_vec4:
                emit->append( instr = new Chuck_Instr_Add_vec4 );
                break;

            default: break;
            }
        }
        break;
    
    case ae_op_plus_chuck:
        // time advance
        if( left == te_dur && right == te_time && rhs->s_meta == ae_meta_var &&
            rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
        {
            // add the two
            emit->append( new Chuck_Instr_Add_double );
            // advance time (TODO: verify having two instr is OK)
            emit->append( instr = new Chuck_Instr_Time_Advance );
            instr->set_linepos( lhs->linepos );
        }
        // time + dur
        else if( ( left == te_dur && right == te_time ) ||
            ( left == te_time && right == te_dur ) )
        {
            emit->append( instr = new Chuck_Instr_Add_double_Assign );
        }
        // string + string
        else if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
        {
            // concatenate
            emit->append( instr = new Chuck_Instr_Add_string_Assign );
            instr->set_linepos( lhs->linepos );
        }
        // right: string
        else if( isa( t_right, emit->env->t_string ) )
        {
            // + int
            if( isa( t_left, emit->env->t_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_int_string_Assign );
                // null exceptions will occur from string: use rhs, not lhs
                instr->set_linepos( rhs->linepos );
            }
            else if( isa( t_left, emit->env->t_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_float_string_Assign );
                instr->set_linepos( rhs->linepos );
            }
            else
            {
                EM_error2( lhs->linepos,
                    "(emit): internal error: unhandled op '%s' %s '%s'",
                    t_left->c_name(), op2str( op ), t_right->c_name() );
                return FALSE;
            }
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Add_int_Assign );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Add_double_Assign );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Add_complex_Assign );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Add_polar_Assign );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Add_vec3_Assign );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_Add_vec4_Assign );
                break;

            default: break;
            }
        }
        break;

    case ae_op_minus:
        if( ( left == te_time && right == te_dur ) ) // time - dur = time
            emit->append( instr = new Chuck_Instr_Minus_double );
        else if( ( left == te_time && right == te_time ) ) // time - time = dur
            emit->append( instr = new Chuck_Instr_Minus_double );
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Minus_int );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Minus_double );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Minus_complex );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Minus_polar );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Minus_vec3 );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_Minus_vec4 );
                break;

            default: break;
            }
        }
        break;
    
    case ae_op_minus_chuck:
        if( ( left == te_dur && right == te_time ) ) // time - dur = time
            emit->append( instr = new Chuck_Instr_Minus_double_Assign );
        else if( ( left == te_time && right == te_time ) ) // time - time = dur
            emit->append( instr = new Chuck_Instr_Minus_double_Assign );
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Minus_int_Assign );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Minus_double_Assign );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Minus_complex_Assign );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Minus_polar_Assign );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Minus_vec3_Assign );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_Minus_vec4_Assign );
                break;

            default: break;
            }
        }
        break;

    case ae_op_times:
        // check
        if( left == te_float && right == te_vec3 )
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec3 );
        }
        else if( left == te_vec3 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec3_Times_float );
        }
        else if( left == te_float && right == te_vec4 )
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec4 );
        }
        else if( left == te_vec4 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec4_Times_float );
        }
        else // other cases
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Times_int );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Times_double );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Times_complex );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Times_polar );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_XProduct_vec3 );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_XProduct_vec4 );
                break;

            default: break;
            }
        }
        break;
    
    case ae_op_times_chuck:
        // check
        if( left == te_float && right == te_vec3 )
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec3_Assign );
        }
        else if( left == te_float && right == te_vec4 )
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec4_Assign );
        }
        else
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Times_int_Assign );
                break;
            case te_float:
                emit->append( instr = new Chuck_Instr_Times_double_Assign );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Times_complex_Assign );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Times_polar_Assign );
                break;

            default: break;
            }
        }
        break;

    case ae_op_divide:
        if( ( left == te_time && right == te_dur ) ) // time / dur = float
            emit->append( instr = new Chuck_Instr_Divide_double );
        else if( left == te_vec3 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec3_Divide_float );
        }
        else if( left == te_vec4 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec4_Divide_float );
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Divide_int );
                instr->set_linepos( rhs->linepos );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Divide_double );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Divide_complex );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Divide_polar );
                break;

            default: break;
            }       
        }
        break;
    
    case ae_op_divide_chuck:
        // reverse
        if( ( left == te_dur && right == te_time ) ) // time / dur = float
            emit->append( instr = new Chuck_Instr_Divide_double_Assign );
        else
        if( left == te_float && right == te_vec3 )
        {
            emit->append( instr = new Chuck_Instr_vec3_Divide_float_Assign );
        }
        else if( left == te_float && right == te_vec4 )
        {
            emit->append( instr = new Chuck_Instr_vec4_Divide_float_Assign );
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Divide_int_Assign );
                instr->set_linepos( lhs->linepos );
                break;
            case te_float:
                emit->append( instr = new Chuck_Instr_Divide_double_Assign );
                break;
            case te_complex:
                emit->append( instr = new Chuck_Instr_Divide_complex_Assign );
                break;
            case te_polar:
                emit->append( instr = new Chuck_Instr_Divide_polar_Assign );
                break;

            default: break;
            }
        }
        break;

    case ae_op_s_or:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Or );
            break;
        
        default: break;
        }
        break;
    
    case ae_op_s_or_chuck:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Or_Assign );
            break;
        
        default: break;
        }
        break;

    case ae_op_s_and:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_And );
            break;

        default: break;
        }
        break;

    case ae_op_s_and_chuck:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_And_Assign );
            break;

        default: break;
        }
        break;

    case ae_op_shift_left:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Shift_Left );
            break;

        case te_array:
            // check size (1.3.1.0: changed to getkindof)
            emit->append( instr = new Chuck_Instr_Array_Append( getkindof( emit->env, t_left->array_type ) ) );
            instr->set_linepos( lhs->linepos );
            break;

        default: break;
        }
        break;

    case ae_op_shift_left_chuck:
        // reverse
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Shift_Left_Assign );
            break;

        default: break;
        }
        break;
    
    case ae_op_shift_right:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Shift_Right );
            break;

        // case te_array:
            // emit->append( instr = new Chuck_Instr_Array_Prepend( t_left->array_type->size ) );
            // break;

        default: break;
        }
        break;

    case ae_op_shift_right_chuck:
        // reverse
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Shift_Right_Assign );
            break;

        default: break;
        }
        break;

    case ae_op_percent:
        if( ( left == te_time && right == te_dur ) ) // time % dur = dur
        {
            emit->append( instr = new Chuck_Instr_Mod_double );
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Mod_int );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Mod_double );
                break;

            default: break;
            }
        }
        break;

    case ae_op_percent_chuck:
        // reverse
        if( ( left == te_dur && right == te_time ) ) // time % dur = dur
        {
            emit->append( instr = new Chuck_Instr_Mod_double_Assign );
        }
        else // other types
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Mod_int_Assign );
                break;
            case te_float:
            case te_dur:
                emit->append( instr = new Chuck_Instr_Mod_double_Assign );
                break;

            default: break;
            }
        }
        break;

    case ae_op_s_xor:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Xor );
            break;
        default: break;
        }
        break;
    
    case ae_op_s_xor_chuck:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Binary_Xor_Assign );
            break;
        default: break;
        }
        break;

    // ----------------------------- chuck -------------------------------------
    case ae_op_chuck:
    {
        a_Exp cl = lhs, cr = rhs;

        if( !emit_engine_emit_op_chuck( emit, cl, cr, binary ) )
            return FALSE;

        return TRUE;
    }
    
    case ae_op_unchuck:
    {
        a_Exp cl = lhs, cr = rhs;
        
        // TODO: cross chuck
        assert( cl->next == NULL && cr->next == NULL );
        while( cr )
        {
            cl = lhs;
            while( cl )
            {
                if( !emit_engine_emit_op_unchuck( emit, cl, cr ) )
                    return FALSE;
                cl = cl->next;
            }
            
            cr = cr->next;
        }

        return TRUE;
    }
    
    case ae_op_upchuck:
    {
        a_Exp cl = lhs, cr = rhs;
        
        // TODO: cross chuck
        assert( cl->next == NULL && cr->next == NULL );
        while( cr )
        {
            cl = lhs;
            while( cl )
            {
                if( !emit_engine_emit_op_upchuck( emit, cl, cr ) )
                    return FALSE;
                cl = cl->next;
            }
            
            cr = cr->next;
        }

        return TRUE;
    }

    case ae_op_at_chuck:
    {
        a_Exp cl = lhs, cr = rhs;
        
        // TODO: cross chuck
        assert( cl->next == NULL && cr->next == NULL );
        while( cr )
        {
            cl = lhs;
            while( cl )
            {
                if( !emit_engine_emit_op_at_chuck( emit, cl, cr ) )
                    return FALSE;
                cl = cl->next;
            }
            
            cr = cr->next;
        }

        return TRUE;
    }
    
    case ae_op_s_chuck:
    break;

    // -------------------------------- bool -----------------------------------        
    case ae_op_eq:
        if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string )
            && !isa( t_left, emit->env->t_null ) && !isa( t_right, emit->env->t_null ) ) // !null
        {
            emit->append( instr = new Chuck_Instr_Op_string( op ) );
            instr->set_linepos( lhs->linepos );
        }
        else if( isa( t_left, emit->env->t_object ) && isa( t_right, emit->env->t_object ) )
            emit->append( instr = new Chuck_Instr_Eq_int );
        else
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Eq_int );
                break;
            case te_float:
            case te_dur:
            case te_time:
                emit->append( instr = new Chuck_Instr_Eq_double );
                break;
            case te_complex:
            case te_polar:
                emit->append( instr = new Chuck_Instr_Eq_complex );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Eq_vec3 );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_Eq_vec4 );
                break;

            default: break;
            }
        }
        break;
    
    case ae_op_neq:
        if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string )
            && !isa( t_left, emit->env->t_null ) && !isa( t_right, emit->env->t_null ) ) // !null
            // added 1.3.2.0 (spencer)
        {
            emit->append( instr = new Chuck_Instr_Op_string( op ) );
            instr->set_linepos( lhs->linepos );
        }
        else if( isa( t_left, emit->env->t_object ) && isa( t_right, emit->env->t_object ) )
            emit->append( instr = new Chuck_Instr_Neq_int );
        else
        {
            switch( left )
            {
            case te_int:
                emit->append( instr = new Chuck_Instr_Neq_int );
                break;
            case te_float:
            case te_dur:
            case te_time:
                emit->append( instr = new Chuck_Instr_Neq_double );
                break;
            case te_complex:
            case te_polar:
                emit->append( instr = new Chuck_Instr_Neq_complex );
                break;
            case te_vec3:
                emit->append( instr = new Chuck_Instr_Neq_vec3 );
                break;
            case te_vec4:
                emit->append( instr = new Chuck_Instr_Neq_vec4 );
                break;

            default: break;
            }
        }
        break;
    
    case ae_op_lt:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Lt_int );
            break;
        case te_float:
        case te_dur:
        case te_time:
            emit->append( instr = new Chuck_Instr_Lt_double );
            break;

        default:
            if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->linepos );
            }
            break;
        }
        break;
    
    case ae_op_le:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Le_int );
            break;
        case te_float:
        case te_dur:
        case te_time:
            emit->append( instr = new Chuck_Instr_Le_double );
            break;

        default:
            if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->linepos );
            }
            else if( isa( t_left, emit->env->t_io ) )
            {
                // output
                if( isa( t_right, emit->env->t_int ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_int );
                    instr->set_linepos( rhs->linepos );
                }
                else if( isa( t_right, emit->env->t_float ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_float );
                    instr->set_linepos( rhs->linepos );
                }
                else if( isa( t_right, emit->env->t_string ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_string );
                    instr->set_linepos( rhs->linepos );
                }
            }
            break;
        }
        break;
    
    case ae_op_gt:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Gt_int );
            break;
        case te_float:
        case te_dur:
        case te_time:
            emit->append( instr = new Chuck_Instr_Gt_double );
            break;

        default:
            if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->linepos );
            }
            break;
        }
        break;
    
    case ae_op_ge:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Ge_int );
            break;
        case te_float:
        case te_dur:
        case te_time:
            emit->append( instr = new Chuck_Instr_Ge_double );
            break;

        default:
            if( isa( t_left, emit->env->t_string ) && isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->linepos );
            }
            break;
        }
        break;
    
    case ae_op_and:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_And );
            break;

        default: break;
        }
        break;
    
    case ae_op_or:
        switch( left )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_Or );
            break;

        default: break;
        }
        break;

        //------------------------------- IO ----------------------------------
    case ae_op_arrow_left:
        switch( right )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_IO_out_int );
            instr->set_linepos( rhs->linepos );
            break;
        case te_float:
            emit->append( instr = new Chuck_Instr_IO_out_float );
            instr->set_linepos( rhs->linepos );
            break;
        default:
            if( isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_IO_out_string );
                instr->set_linepos( rhs->linepos );
            }
            break;
        }
        break;

    case ae_op_arrow_right:
        switch( right )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_IO_in_int );
            instr->set_linepos( rhs->linepos );
            break;
        case te_float:
            emit->append( instr = new Chuck_Instr_IO_in_float );
            instr->set_linepos( rhs->linepos );
            break;
        default:
            if( isa( t_right, emit->env->t_string ) )
            {
                emit->append( instr = new Chuck_Instr_IO_in_string );
                instr->set_linepos( rhs->linepos );
            }
            break;
        }
        break;

        //---------------------------- (error) --------------------------------
    default:
        EM_error2( lhs->linepos,
            "(emit): internal error: unhandled op '%s' %s '%s'",
            t_left->c_name(), op2str( op ), t_right->c_name() );
        return FALSE;
    }
    
    // make sure emit
    if( !instr )
    {
        EM_error2( lhs->linepos,
            "(emit): internal error: unhandled op '%s' %s '%s'",
            t_left->c_name(), op2str( op ), t_right->c_name() );
        return FALSE;
    }
        
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    // any implicit cast happens before this
    Chuck_Type * left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * right = rhs->cast_to ? rhs->cast_to : rhs->type;
    
    // the pointer
    Chuck_Instr * instr = NULL;
    
    // ugen => ugen
    if( isa( left, emit->env->t_ugen ) && isa( right, emit->env->t_ugen ) )
    {
        // link, flag as NOT unchuck
        emit->append( instr = new Chuck_Instr_UGen_Link( FALSE ) );
        instr->set_linepos( lhs->linepos );
        // done
        return TRUE;
    }
    
    // ugen[] => ugen[] (or permutation)
    if( ( isa( left, emit->env->t_ugen ) || ( isa( left, emit->env->t_array ) && isa( left->array_type, emit->env->t_ugen ) ) ) &&
        ( isa( right, emit->env->t_ugen ) || ( isa( right, emit->env->t_array ) && isa( right->array_type, emit->env->t_ugen ) ) ) )
    {
        // link, flag as NOT unchuck
        emit->append( instr = new Chuck_Instr_UGen_Array_Link( isa( left, emit->env->t_array ), isa( right, emit->env->t_array ) ) );
        instr->set_linepos( lhs->linepos );
        // done
        return TRUE;
    }
    
    // time advance
    if( isa( left, emit->env->t_dur ) && isa( right, emit->env->t_time ) && rhs->s_meta == ae_meta_var )
    {
        // add the two
        emit->append( new Chuck_Instr_Add_double );

        // see if rhs is 'now'
        if( rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
        {
            // advance time
            emit->append( instr = new Chuck_Instr_Time_Advance );
            instr->set_linepos( lhs->linepos );
        }

        return TRUE;
    }
    
    // time advance
    if( isa( left, emit->env->t_event ) && isa( right, emit->env->t_time ) && rhs->s_meta == ae_meta_var &&
        rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        // pop now
        emit->append( new Chuck_Instr_Reg_Pop_Word2 );
        emit->append( instr = new Chuck_Instr_Event_Wait );
        instr->set_linepos( lhs->linepos );

        return TRUE;
    }
    
    // input
    if( isa( left, emit->env->t_io ) )
    {
        if( isa( right, emit->env->t_int ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_int );
            instr->set_linepos( rhs->linepos );
	    return TRUE;
        }
        else if( isa( right, emit->env->t_float ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_float );
            instr->set_linepos( rhs->linepos );
	    return TRUE;
        }
        else if( isa( right, emit->env->t_string ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_string );
            instr->set_linepos( rhs->linepos );
	    return TRUE;
        }
    }

    // func call
    if( isa( right, emit->env->t_function ) )
    {
        assert( binary->ck_func != NULL );
        
        // emit
        return emit_engine_emit_exp_func_call( emit, binary->ck_func, binary->self->type, binary->linepos );
    }

    // assignment or something else
    if( isa( left, right ) )
    {
        // basic types?
        if( type_engine_check_primitive( emit->env, left ) || isa( left, emit->env->t_string ) )
        {
            // use at assign
            return emit_engine_emit_op_at_chuck( emit, lhs, rhs );
        }
    }

    // TODO: check overloading of =>
    // TODO: deal with const

    // no match
    EM_error2( lhs->linepos,
        "(emit): internal error: unhandled '=>' on types '%s' and '%s'...",
        left->c_name(), right->c_name() );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_unchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs )
{
    // any implicit cast happens before this
    Chuck_Type * left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * right = rhs->cast_to ? rhs->cast_to : rhs->type;
    
    // if ugen
    if( isa( left, emit->env->t_ugen ) && isa( right, emit->env->t_ugen ) )
    {
        // no connect
        emit->append( new Chuck_Instr_UGen_UnLink );
    }
    else
    {
        EM_error2( lhs->linepos,
            "(emit): internal error: unhandled '=<' on types '%s' and '%s'",
            left->c_name(), right->c_name() );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_upchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_upchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs )
{
    // any implicit cast happens before this
    Chuck_Type * left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * right = rhs->cast_to ? rhs->cast_to : rhs->type;

    // if ugen
    if( isa( left, emit->env->t_uana ) && isa( right, emit->env->t_uana ) )
    {
        // connect; flag it as unchuck
        Chuck_Instr * instr = NULL;
        emit->append( instr = new Chuck_Instr_UGen_Link( TRUE ) );
        instr->set_linepos( lhs->linepos );
    }
    else
    {
        EM_error2( lhs->linepos,
            "(emit): internal error: unhandled '=^' on types '%s' and '%s'",
            left->c_name(), right->c_name() );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_at_chuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_at_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs )
{
    // any implicit cast happens before this
    Chuck_Type * left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * right = rhs->cast_to ? rhs->cast_to : rhs->type;
    
    // assignment or something else
    if( isa( left, right ) )
    {
        // basic types?
        if( type_engine_check_primitive( emit->env, left ) || isa( left, emit->env->t_string ) )
        {
            // assigment?
            if( rhs->s_meta != ae_meta_var )
            {
                EM_error2( lhs->linepos,
                    "(emit): internal error: assignment to non-variable..." );
                return FALSE;
            }

            // see if rhs is 'now' - time => now
            if( rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
            {
                // pop the now addr
                emit->append( new Chuck_Instr_Reg_Pop_Word2 );
                // advance time
                Chuck_Instr * instr = NULL;
                emit->append( instr = new Chuck_Instr_Time_Advance );
                instr->set_linepos( lhs->linepos );
            }
            else if( isa( left, emit->env->t_string ) ) // string
            {
                // assign string
                emit->append( new Chuck_Instr_Assign_String );
            }
            else
            {
                // assign primitive
                // added 1.3.1.0: iskindofint -- since on some 64-bit systems, sz_INT == sz_FLOAT
                if( right->size == sz_INT && iskindofint(emit->env, right) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    emit->append( new Chuck_Instr_Assign_Primitive );
                else if( right->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    emit->append( new Chuck_Instr_Assign_Primitive2 );
                else if( right->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    emit->append( new Chuck_Instr_Assign_Primitive4 );
                else if( right->size == sz_VEC3 ) // ge: added 1.3.5.3
                    emit->append( new Chuck_Instr_Assign_PrimitiveVec3 );
                else if( right->size == sz_VEC4 ) // ge: added 1.3.5.3
                    emit->append( new Chuck_Instr_Assign_PrimitiveVec4 );
                else
                {
                    EM_error2( rhs->linepos,
                        "(emit): internal error: assignment to type of size %i...",
                        right->size );
                }
            }

            return TRUE;
        }
        else // objects
        {
            // assign object
            emit->append( new Chuck_Instr_Assign_Object );

            return TRUE;
        }
    }

    // TODO: check overloading of =>
    // TODO: deal with const

    // no match
    EM_error2( lhs->linepos,
        "(emit): internal error: unhandled '@=>' on types '%s' and '%s'...",
        left->c_name(), right->c_name() );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_unary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emitter * emit, a_Exp_Unary unary )
{
    if( unary->op != ae_op_spork && !emit_engine_emit_exp( emit, unary->exp ) )
        return FALSE;

    // get type
    Chuck_Type * t = unary->self->type;
    assert( t != NULL );

    // emit the operator
    switch( unary->op )
    {
    case ae_op_plusplus:
        // make sure assignment is legal
        if( unary->self->s_meta != ae_meta_var )  // TODO: const
        {
            EM_error2( unary->self->linepos,
                "(emit): target for '++' not mutable..." );
            return FALSE;
        }

        // increment
        if( equals( unary->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_PreInc_int );
        else
        {
            EM_error2( unary->linepos, 
                "(emit): internal error: unhandled type '%s' for pre '++'' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_minusminus:
        // make sure assignment is legal
        if( unary->self->s_meta != ae_meta_var )  // TODO: const
        {
            EM_error2( unary->self->linepos,
                "(emit): target for '--' not mutable..." );
            return FALSE;
        }

        // decrement
        if( equals( unary->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_PreDec_int );
        else
        {
            EM_error2( unary->linepos, 
                "(emit): internal error: unhandled type '%s' for pre '--' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_tilda:
        // complement
        if( equals( unary->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_Complement_int );
        else
        {
            EM_error2( unary->linepos, 
                "(emit): internal error: unhandled type '%s' for '~' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_exclamation:
        // !
        if( equals( unary->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_Not_int );
        else
        {
            EM_error2( unary->linepos, 
                "(emit): internal error: unhandled type '%s' for '!' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;
        
    case ae_op_minus:
        // negate
        if( equals( unary->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_Negate_int );
        else if( equals( unary->exp->type, emit->env->t_float ) )
            emit->append( new Chuck_Instr_Negate_double );
        else
        {
            EM_error2( unary->linepos, 
                "(emit): internal error: unhandled type '%s' for unary '-' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_spork:
        // spork ~ func()
        if( unary->exp && unary->exp->s_type == ae_exp_func_call )
        {
            if( !emit_engine_emit_spork( emit, &unary->exp->func_call ) )
                return FALSE;
        }
        // spork ~ { ... }
        // else if( unary->code )
        // {
        //     if( !emit_engine_emit_spork( emit, unary->code ) )
        //         return FALSE;
        // }
        else
        {
            EM_error2( unary->linepos,
                "(emit): internal error: sporking non-function call..." );
            return FALSE;
        }
        break;

    case ae_op_new:

        // if this is an object
        if( isobj( emit->env, t ) )
        {
            // instantiate object, including array
            if( !emit_engine_instantiate_object( emit, t, unary->array, unary->type->ref ) )
                return FALSE;
        }
        break;

    default:
        EM_error2( unary->linepos, 
            "(emit): internal error: unhandled unary op '%s",
            op2str( unary->op ) );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_primary()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_primary( Chuck_Emitter * emit, a_Exp_Primary exp )
{
    t_CKUINT temp;
    t_CKDUR dur;
    Chuck_String * str = NULL;

    // find out exp
    switch( exp->s_type )
    {
    case ae_primary_var:
        if( exp->var == insert_symbol( "now" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Now );
        }
        else if( exp->var == insert_symbol( "this" ) )
        {
            // TODO: verify this is in the right scope
            emit->append( new Chuck_Instr_Reg_Push_This );
        }
        else if( exp->var == insert_symbol( "me" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Me );
        }
        else if( exp->var == insert_symbol( "true" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm( 1 ) );
        }
        else if( exp->var == insert_symbol( "false" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        }
        else if( exp->var == insert_symbol("maybe") )
        {
            emit->append( new Chuck_Instr_Reg_Push_Maybe() );
        }
        else if( exp->var == insert_symbol("null") ||
                 exp->var == insert_symbol("NULL") )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        }
        else if( exp->var == insert_symbol( "pi" ) )
        {
            double pi = 3.14159265358979323846;
            emit->append( new Chuck_Instr_Reg_Push_Imm2( pi ) );
        }
        else if( exp->var == insert_symbol( "dac" ) )
        {
            emit->append( new Chuck_Instr_DAC );
        }
        else if( exp->var == insert_symbol( "adc" ) )
        {
            emit->append( new Chuck_Instr_ADC );
        }
        else if( exp->var == insert_symbol( "bunghole" ) )
        {
            emit->append( new Chuck_Instr_Bunghole );
        }
        else if( exp->var == insert_symbol( "blackhole" ) )
        {
            emit->append( new Chuck_Instr_Bunghole );
        }
        else if( exp->var == insert_symbol( "chout" ) )
        {
            emit->append( new Chuck_Instr_Chout );
        }
        else if( exp->var == insert_symbol( "cherr" ) )
        {
            emit->append( new Chuck_Instr_Cherr );
        }
        else if( emit->find_dur( S_name(exp->var), &dur ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm2( dur ) );
        }
        else
        {
            // emit the symbol
            return emit_engine_emit_symbol( 
                emit, exp->var, exp->value, exp->self->emit_var, exp->linepos );
        }
        break;
    
    case ae_primary_num:
        memcpy( &temp, &exp->num, sizeof(temp) );
        emit->append( new Chuck_Instr_Reg_Push_Imm( temp ) );
        break;
        
    case ae_primary_float:
        emit->append( new Chuck_Instr_Reg_Push_Imm2( exp->fnum ) );
        break;
        
    case ae_primary_str:
        // TODO: fix this
        str = new Chuck_String();
        if( !str || !initialize_object( str, emit->env->t_string ) )
        {
            // error (TODO: why is this a SAFE_RELEASE and not SAFE_DELETE?)
            SAFE_RELEASE( str );
            // error out
            CK_FPRINTF_STDERR( 
                "[chuck](emitter): OutOfMemory: while allocating string literal '%s'\n", exp->str );
            return FALSE;
        }
        str->set( exp->str );
        temp = (t_CKUINT)str;
        emit->append( new Chuck_Instr_Reg_Push_Imm( temp ) );
        // add reference for string literal (added 1.3.0.2)
        str->add_ref();
        break;
            
    case ae_primary_char:
        emit->append( new Chuck_Instr_Reg_Push_Imm( str2char(exp->chr, exp->linepos ) ) );
        break;

    case ae_primary_array:
        if( !emit_engine_emit_array_lit( emit, exp->array ) )
            return FALSE;
        break;

    case ae_primary_complex:
        if( !emit_engine_emit_complex_lit( emit, exp->complex ) )
            return FALSE;
        break;
    
    case ae_primary_polar:
        if( !emit_engine_emit_polar_lit( emit, exp->polar ) )
            return FALSE;
        break;
    
    case ae_primary_vec:
        if( !emit_engine_emit_vec_lit( emit, exp->vec ) )
            return FALSE;
        break;
        
    case ae_primary_exp:
        if( !emit_engine_emit_exp( emit, exp->exp ) )
            return FALSE;
        break;

    case ae_primary_hack:
        {
            a_Exp e = exp->exp;
            // emit the expression
            if( !emit_engine_emit_exp( emit, e ) )
                return FALSE;
            std::vector<Chuck_Type *> types;

            while( e != NULL )
            {
                types.push_back( e->type );
                e = e->next;
            }
            
            // emit hack
            emit->append( new Chuck_Instr_Gack( types ) );

            break;
        }
        
    // ge (april 2012): nil is void, so nothing to emit; TODO: check it (added 1.3.0.0)
    case ae_primary_nil:
        break;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_array_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_array_lit( Chuck_Emitter * emit, a_Array_Sub array )
{
    // go through and emit the expressions
    if( !emit_engine_emit_exp( emit, array->exp_list ) )
        return FALSE;

    // count the number
    a_Exp e = array->exp_list;
    t_CKUINT count = 0;
    // loop over
    while( e ) { count++; e = e->next; }

    // the type
    e = array->exp_list;
    Chuck_Type * type = e->cast_to ? e->cast_to : e->type;

    // construct array dynamically
    Chuck_Instr * instr = NULL;
    emit->append( instr = new Chuck_Instr_Array_Init( emit->env, type, count ) );
    instr->set_linepos( array->linepos );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_complex_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_complex_lit( Chuck_Emitter * emit, a_Complex val )
{
    // go through and emit the real and imaginary
    if( !emit_engine_emit_exp( emit, val->re ) )
        return FALSE;
    
    // imaginary (not needed since linked after real
    // if( !emit_engine_emit_exp( emit, val->im ) )
    //     return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_polar_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_polar_lit( Chuck_Emitter * emit, a_Polar val )
{
    // go through and emit the mod and phase
    if( !emit_engine_emit_exp( emit, val->mod ) )
        return FALSE;
    
    // phase (not needed since linked after mod)
    // if( !emit_engine_emit_exp( emit, val->phase ) )
    //     return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_vec_lit()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_vec_lit( Chuck_Emitter * emit, a_Vec val )
{
    // go through and emit the mod and phase
    if( !emit_engine_emit_exp( emit, val->args ) )
        return FALSE;

    // zero pad if needed
    t_CKINT n = 3 - val->numdims;
    // any remaining
    while( n > 0 )
    {
        // emit a zero float
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0 ) );
        // decrement
        n--;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_cast( Chuck_Emitter * emit, a_Exp_Cast cast )
{
    Chuck_Type * to = cast->self->type;
    Chuck_Type * from = cast->exp->type;

    // emit the exp
    if( !emit_engine_emit_exp( emit, cast->exp ) )
        return FALSE;

    // the actual work to be done
    return emit_engine_emit_cast( emit, to, from );
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_cast()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_cast( Chuck_Emitter * emit, 
                                Chuck_Type * to, Chuck_Type * from )
{
    // if type is already the same
    if( equals( to, from ) )
        return TRUE;

    // int to float
    if( equals( to, emit->env->t_int ) && equals( from, emit->env->t_float ) )
        emit->append( new Chuck_Instr_Cast_double2int );
    // float to int
    else if( equals( to, emit->env->t_float ) && equals( from, emit->env->t_int ) )
        emit->append( new Chuck_Instr_Cast_int2double );
    else if( equals( to, emit->env->t_complex ) && equals( from, emit->env->t_int ) )
        emit->append( new Chuck_Instr_Cast_int2complex );
    else if( equals( to, emit->env->t_polar ) && equals( from, emit->env->t_int ) )
        emit->append( new Chuck_Instr_Cast_int2polar );
    else if( equals( to, emit->env->t_complex ) && equals( from, emit->env->t_float ) )
        emit->append( new Chuck_Instr_Cast_double2complex );
    else if( equals( to, emit->env->t_polar ) && equals( from, emit->env->t_float ) )
        emit->append( new Chuck_Instr_Cast_double2polar );
    else if( equals( to, emit->env->t_polar ) && equals( from, emit->env->t_complex ) )
        emit->append( new Chuck_Instr_Cast_complex2polar );
    else if( equals( to, emit->env->t_complex ) && equals( from, emit->env->t_polar ) )
        emit->append( new Chuck_Instr_Cast_polar2complex );
    else if( equals( to, emit->env->t_vec3 ) && equals( from, emit->env->t_vec4 ) )
        emit->append( new Chuck_Instr_Cast_vec4tovec3 );
    else if( equals( to, emit->env->t_vec4 ) && equals( from, emit->env->t_vec3 ) )
        emit->append( new Chuck_Instr_Cast_vec3tovec4 );
    else if( equals( to, emit->env->t_string ) && isa( from, emit->env->t_object ) && !isa( from, emit->env->t_string ) )
        emit->append( new Chuck_Instr_Cast_object2string );
    // up cast - do nothing
    else if( !isa( to, from ) && !isa( from, to ) )
    {
        EM_error2( 0, "(emit): internal error: cannot cast type '%s' to '%s'",
             from->c_name(), to->c_name() );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_postfix()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_postfix( Chuck_Emitter * emit, a_Exp_Postfix postfix )
{
    // emit the exp
    if( !emit_engine_emit_exp( emit, postfix->exp ) )
        return FALSE;

    // emit
    switch( postfix->op )
    {
    case ae_op_plusplus:
        if( equals( postfix->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_PostInc_int );
        else
        {
            EM_error2( postfix->linepos,
                "(emit): internal error: unhandled type '%s' for post '++' operator",
                postfix->exp->type->c_name() );
            return FALSE;
        }
    break;

    case ae_op_minusminus:
        if( equals( postfix->exp->type, emit->env->t_int ) )
            emit->append( new Chuck_Instr_PostDec_int );
        else
        {
            EM_error2( postfix->linepos,
                "(emit): internal error: unhandled type '%s' for post '--' operator",
                postfix->exp->type->c_name() );
            return FALSE;
        }
    break;

    default:
        EM_error2( postfix->linepos,
            "(emit): internal error: unhandled postfix operator '%s'",
            op2str( postfix->op ) );
        return FALSE;
    }        
     
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_dur()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dur( Chuck_Emitter * emit, a_Exp_Dur dur )
{
    // emit base
    if( !emit_engine_emit_exp( emit, dur->base ) )
        return FALSE;

    // cast
    if( equals( dur->base->type, emit->env->t_int ) )
        emit->append( new Chuck_Instr_Cast_int2double );

    // emit unit
    if( !emit_engine_emit_exp( emit, dur->unit ) )
        return FALSE;
        
    // multiply
    emit->append( new Chuck_Instr_Times_double );
        
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_array( Chuck_Emitter * emit, a_Exp_Array array )
{
    Chuck_Type * type = NULL, * base_type = NULL;
    t_CKUINT depth = 0;
    a_Array_Sub sub = NULL;
    a_Exp exp = NULL;
    t_CKBOOL is_var = FALSE;
    t_CKBOOL is_str = FALSE;

    // get the type
    type = array->self->type;
    // get the base type
    base_type = array->base->type;
    // emit as addr?
    is_var = array->self->emit_var;
    // get the dimension
    depth = base_type->array_depth - type->array_depth;
    // make sure
    if( depth == 0 )
    {
        EM_error2( array->linepos,
            "(emit): internal error: array with 0 depth..." );
        return FALSE;
    }
    // get the sub
    sub = array->indices;
    if( !sub )
    {
        EM_error2( array->linepos,
            "(emit): internal error: NULL array sub..." );
        return FALSE;
    }
    // get the exp list
    exp = sub->exp_list;
    if( !exp )
    {
        EM_error2( array->linepos,
            "(emit): internal error: NULL array exp..." );
        return FALSE;
    }

    // emit the base (it should be a pointer to array)
    if( !emit_engine_emit_exp( emit, array->base ) )
        return FALSE;

    // emit the exp list
    if( !emit_engine_emit_exp( emit, exp ) )
        return FALSE;

    // find out first element
    if( isa( exp->type, emit->env->t_string ) )
        is_str = TRUE;

    // make sure
    // ISSUE: 64-bit (fixed 1.3.1.0)
    if( type->size != sz_INT && type->size != sz_FLOAT && type->size != sz_COMPLEX &&
        type->size != sz_VEC3 && type->size != sz_VEC4 )
    {
        EM_error2( array->linepos,
            "(emit): internal error: array with datasize of %i...", type->size );
        return FALSE;
    }

    // check the depth
    if( depth == 1 )
    {
        // the pointer
        Chuck_Instr * instr = NULL;
        // emit the array access (1.3.1.0: use getkindof instead of type->size)
        if( is_str )
            emit->append( instr = new Chuck_Instr_Array_Map_Access( getkindof(emit->env, type), is_var ) );
        else
            emit->append( instr = new Chuck_Instr_Array_Access( getkindof(emit->env, type), is_var ) );
        instr->set_linepos( array->linepos );
    }
    else
    {
        // the pointer
        Chuck_Instr_Array_Access_Multi * aam = NULL;
        // emit the multi array access (1.3.1.0: use getkindof instead of type->size)
        emit->append( aam = new Chuck_Instr_Array_Access_Multi( depth, getkindof(emit->env, type), is_var ) );
        aam->set_linepos( array->linepos );
        // add type info (1.3.1.0) -- to support mixed string & int indexing (thanks Robin Haberkorn)
        a_Exp e = exp;
        while( e )
        {
            // check if string
            aam->indexIsAssociative().push_back( isa( exp->type, emit->env->t_string ) );
            // next
            e = e->next;
        }
    }

    // TODO: variable?

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emitter * emit,
                                         Chuck_Func * func,
                                         Chuck_Type * type,
                                         int linepos,
                                         t_CKBOOL spork )
{
    // is a member?
    t_CKBOOL is_member = func->is_member;

    // if member
    //if( is_member )
    //{
    //    // this
    //    emit->append( new Chuck_Instr_Reg_Push_This );
    //}

    // translate to code
    emit->append( new Chuck_Instr_Func_To_Code );
    // emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)func->code ) );
    // push the local stack depth - local variables
    emit->append( new Chuck_Instr_Reg_Push_Imm( emit->code->frame->curr_offset ) );

    // TODO: member functions and static functions
    // call the function
    t_CKUINT size = type->size;
    t_CKUINT kind = getkindof( emit->env, type ); // added 1.3.1.0
        
    // the pointer
    Chuck_Instr * instr = NULL;
    if( func->def->s_type == ae_func_builtin )
    {
        // ISSUE: 64-bit (fixed 1.3.1.0)
        if( size == 0 || size == sz_INT || size == sz_FLOAT || size == sz_COMPLEX ||
            size == sz_VEC3 || sz_VEC4 )
        {
            // is member (1.3.1.0: changed to use kind instead of size)
            if( is_member )
                emit->append( instr = new Chuck_Instr_Func_Call_Member( kind ) );
            else
                emit->append( instr = new Chuck_Instr_Func_Call_Static( kind ) );
        }
        else
        {
            EM_error2( linepos,
                       "(emit): internal error: %i func call not handled",
                       size );
            return FALSE;
        }
    }
    else
    {
        emit->append( instr = new Chuck_Instr_Func_Call );
    }
    instr->set_linepos(linepos);

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_func_args()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_func_args( Chuck_Emitter * emit,
                                     a_Exp_Func_Call func_call )
{
    // emit the args (TRUE for doAddRef added 1.3.0.0)
    if( !emit_engine_emit_exp( emit, func_call->args, TRUE ) )
    {
        EM_error2( func_call->linepos,
                   "(emit): internal error in emitting function call arguments..." );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_func_call()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emitter * emit,
                                         a_Exp_Func_Call func_call,
                                         t_CKBOOL spork )
{
    // note: spork situations are now taken care in exp_spork...
    // please look at that one before modifying this one!

    // make sure there are args, and not sporking
    if( func_call->args && !spork )
    {
        if( !emit_engine_emit_func_args( emit, func_call ) )
            return FALSE;
    }

    // emit func
    if( !emit_engine_emit_exp( emit, func_call->func ) )
    {
        EM_error2( func_call->linepos,
                   "(emit): internal error in evaluating function call..." );
        return FALSE;
    }
    
    // the rest
    return emit_engine_emit_exp_func_call( emit, func_call->ck_func, func_call->ret_type,
                                           func_call->linepos, spork );
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_dot_member_special()
// desc: emit special dot member: complex, polar, vec3, vec4
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dot_member_special( Chuck_Emitter * emit,
                                                  a_Exp_Dot_Member member )
{
    // whether to emit addr or value
    t_CKBOOL emit_addr = member->self->emit_var;
    
    // special case: complex and polar
    if( member->t_base->xid == te_complex )
    {
        // mark to emit addr
        if( member->base->s_meta == ae_meta_var )
            member->base->emit_var = TRUE;
        else if( emit_addr )
        {
            EM_error2( member->base->linepos,
                      "(emit): cannot assign value to literal 'complex' value..." );
            return FALSE;
        }
        
        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;
        
        string str = S_name(member->xid);
        // check
        if( str == "re" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr ) );
        else if( str == "im" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr ) );
        else
            goto check_func;
        
        // done
        return TRUE;
    }
    else if( member->t_base->xid == te_polar )
    {
        // mark to emit addr
        if( member->base->s_meta == ae_meta_var )
            member->base->emit_var = TRUE;
        else if( emit_addr )
        {
            EM_error2( member->base->linepos,
                      "(emit): cannot assign value to literal 'polar' value..." );
            return FALSE;
        }
        
        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;
        
        string str = S_name(member->xid);
        // check
        if( str == "mag" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr ) );
        else if( str == "phase" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr ) );
        else
            goto check_func;
        
        // done
        return TRUE;
    }
    else if( member->t_base->xid == te_vec3 || member->t_base->xid == te_vec4 )
    {
        // mark to emit addr
        if( member->base->s_meta == ae_meta_var )
            member->base->emit_var = TRUE;
        else if( emit_addr )
        {
            EM_error2( member->base->linepos,
                      "(emit): cannot assign value to literal '%s' value...",
                      member->t_base->c_name() );
            return FALSE;
        }
        
        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;
        
        string str = S_name(member->xid);
        // check for .xyz[w] .rgb[a] .value/goal/slew
        if( str == "x" || str == "r" || str == "value" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr ) );
        else if( str == "y" || str == "g" || str == "goal" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr ) );
        else if( str == "z" || str == "b" || str == "slew" )
            emit->append( new Chuck_Instr_Dot_Cmp_Third( member->base->s_meta == ae_meta_var, emit_addr ) );
        else if( member->t_base->xid == te_vec4 && ( str == "w" || str == "a" ) )
            emit->append( new Chuck_Instr_Dot_Cmp_Fourth( member->base->s_meta == ae_meta_var, emit_addr ) );
        else
            goto check_func;
    
        // done
        return TRUE;
    }

    // should not get here
    EM_error2( member->base->linepos,
              "(emit): internal error in lit_special()..." );

    return FALSE;
    
//-----------------------------------------------------------------------------
// goto: check for special member functions, 1.3.5.3
//-----------------------------------------------------------------------------
check_func:
    
    // the type of the base
    Chuck_Type * t_base = member->t_base;
    // is the base a class/namespace or a variable
    t_CKBOOL base_static = isa( member->t_base, emit->env->t_class );
    // a function
    Chuck_Func * func = NULL;
    // a non-function value
    Chuck_Value * value = NULL;
    // the offset
    t_CKUINT offset = 0;
    
    // check error
    if( base_static )
    {
        // should not get here
        EM_error2( member->base->linepos,
                  "(emit): internal error in lit_special(): base static" );
        // done
        return FALSE;
    }
        
    if( !isfunc( emit->env, member->self->type ) )
    {
        // should not get here
        EM_error2( member->base->linepos,
                  "(emit): internal error in lit_special(): not member func" );
        // done
        return FALSE;
    }
    
    // get the func
    value = type_engine_find_value( t_base, member->xid );
    func = value->func_ref;
    // make sure it's there
    assert( func != NULL );
    
    // NOTE: base already emitted earier in this function (and as var)

    // check base; 1.3.5.3
    if( member->base->s_meta == ae_meta_value ) // is literal
    {
        // dup the value as pointer (as faux-'this' pointer)
        emit->append( new Chuck_Instr_Reg_Dup_Last_As_Pointer( t_base->size / sz_WORD ) );
    }
    else // normal object
    {
        // dup the base pointer (as 'this' argument -- special case primitive)
        emit->append( new Chuck_Instr_Reg_Dup_Last );
    }
    
    // find the offset for virtual table
    offset = func->vt_index;
    // emit the function
    emit->append( new Chuck_Instr_Dot_Primitive_Func( (t_CKUINT)func ) );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_dot_member()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dot_member( Chuck_Emitter * emit,
                                          a_Exp_Dot_Member member )
{
    // the type of the base
    Chuck_Type * t_base = NULL;
    // whether to emit addr or value
    t_CKBOOL emit_addr = member->self->emit_var;
    // is the base a class/namespace or a variable
    t_CKBOOL base_static = isa( member->t_base, emit->env->t_class );
    // a function
    Chuck_Func * func = NULL;
    // a non-function value
    Chuck_Value * value = NULL;
    // the offset
    t_CKUINT offset = 0;
    
    // the pointer
    Chuck_Instr * instr = NULL;

    // type of the base
    t_CKUINT tbase_xid = member->t_base->xid;
    // switch on it
    switch( tbase_xid )
    {
        case te_complex:
        case te_polar:
        case te_vec3:
        case te_vec4:
            // emit special
            return emit_engine_emit_exp_dot_member_special( emit, member );
            // done
            break;
    }
    
    // actual type - if base is class name its type is actually 'class'
    //               to get the actual type use actual_type
    t_base = base_static ? member->t_base->actual_type : member->t_base;

    // make sure that the base type is object
    assert( t_base->info != NULL );

    // if base is static
    if( !base_static )
    {
        // if is a func
        if( isfunc( emit->env, member->self->type ) )
        {
            // get the func
            value = type_engine_find_value( t_base, member->xid );
            func = value->func_ref;
            // make sure it's there
            assert( func != NULL );

            // is the func static?
            if( func->is_member )
            {
                // emit the base (TODO: return on error?)
                emit_engine_emit_exp( emit, member->base );
                // dup the base pointer (as 'this' argument)
                emit->append( new Chuck_Instr_Reg_Dup_Last );
                // find the offset for virtual table
                offset = func->vt_index;
                // emit the function
                emit->append( instr = new Chuck_Instr_Dot_Member_Func( offset ) );
                instr->set_linepos( member->linepos );
            }
            else
            {
                // emit the type
                emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );
                // emit the static function
                emit->append( new Chuck_Instr_Dot_Static_Func( func ) );
            }
        }
        else
        {
            // get the value
            // value = t_base->info->lookup_value( member->xid, FALSE );
            value = type_engine_find_value( t_base, member->xid );
            // make sure it's there
            assert( value != NULL );

            // find the offset for data
            offset = value->offset;

            // is the value static?
            if( value->is_member )
            {
                // emit the base (TODO: return on error?)
                emit_engine_emit_exp( emit, member->base );
                // lookup the member (1.3.1.0: changed to use getkindof instead of type size)
                emit->append( instr = new Chuck_Instr_Dot_Member_Data(
                    offset, getkindof(emit->env, member->self->type), emit_addr ) );
                instr->set_linepos( member->linepos );
            }
            else
            {
                // if builtin or not
                if( value->addr )
                {
                    // emit builtin (1.3.1.0: changed to use getkindof instead of type size)
                    emit->append( new Chuck_Instr_Dot_Static_Import_Data(
                        value->addr, getkindof(emit->env, member->self->type), emit_addr ) );
                }
                else
                {
                    // emit the type
                    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );
                    // emit the static value (1.3.1.0: changed to use getkindof in addition to size)
                    emit->append( new Chuck_Instr_Dot_Static_Data(
                        offset, member->self->type->size, getkindof(emit->env, member->self->type), emit_addr ) );
                }
            }
        }
    }
    else // static
    {
        // emit the type
        // commented out so built-in static member variables don't have an 
        // extra thing on the stack - spencer
        //emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );

        // if is a func
        if( isfunc( emit->env, member->self->type ) )
        {
            // emit the type - spencer
            emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );
            // get the func
            func = t_base->info->lookup_func( member->xid, FALSE );
            // make sure it's there
            assert( func != NULL );
            // emit the function
            emit->append( new Chuck_Instr_Dot_Static_Func( func ) );
        }
        else
        {
            // get the value
            value = t_base->info->lookup_value( member->xid, FALSE );
            // make sure it's there
            assert( value != NULL );

            // if builtin
            if( value->addr )
            {
                // emit (1.3.1.0: changed to use getkindof instead of type size)
                emit->append( new Chuck_Instr_Dot_Static_Import_Data(
                    value->addr, getkindof(emit->env, member->self->type), emit_addr ) );
            }
            else
            {
                // emit the type - spencer
                emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );
                // find the offset for data
                offset = value->offset;
                // emit the member (1.3.1.0: changed to use getkindof in addition to size)
                emit->append( new Chuck_Instr_Dot_Static_Data(
                    offset, member->self->type->size, getkindof(emit->env, member->self->type), emit_addr ) );
            }
        }
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_if()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_if( Chuck_Emitter * emit, a_Exp_If exp_if )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL, * op2 = NULL;

    // push the stack, allowing for new local variables
    emit->push_scope();

    // emit the condition
    ret = emit_engine_emit_exp( emit, exp_if->cond );
    if( !ret )
        return FALSE;

    // type of the condition
    switch( exp_if->cond->type->xid )
    {
    case te_int:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_int( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Eq_double( 0 );
        break;            
        
    default:
        EM_error2( exp_if->cond->linepos,
            "(emit): internal error: unhandled type '%s' in if condition",
            exp_if->cond->type->name.c_str() );
        return FALSE;
    }

    if( !ret ) return FALSE;

    // append the op
    emit->append( op );

    // emit the body
    ret = emit_engine_emit_exp( emit, exp_if->if_exp );
    if( !ret )
        return FALSE;

    // emit the skip to the end
    emit->append( op2 = new Chuck_Instr_Goto(0) );
    
    // set the op's target
    op->set( emit->next_index() );

    // emit the body
    ret = emit_engine_emit_exp( emit, exp_if->else_exp );
    if( !ret )
        return FALSE;

    // pop stack
    emit->pop_scope();

    // set the op2's target
    op2->set( emit->next_index() );

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_pre_constructor()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_pre_constructor( Chuck_Emitter * emit, Chuck_Type * type )
{
    // parent first pre constructor
    if( type->parent != NULL )
        emit_engine_pre_constructor( emit, type->parent );

    // pre constructor
    if( type->has_constructor )
    {
        // make sure
        assert( type->info->pre_ctor != NULL );
        // append instruction
        emit->append( new Chuck_Instr_Pre_Constructor( type->info->pre_ctor,
            emit->code->frame->curr_offset ) );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_pre_constructor_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_pre_constructor_array( Chuck_Emitter * emit, Chuck_Type * type )
{
    // alloc should have put all objects to made in linear list, on stack
    Chuck_Instr_Pre_Ctor_Array_Top * top = NULL;
    Chuck_Instr_Pre_Ctor_Array_Bottom * bottom = NULL;
    // get start index
    t_CKUINT start_index = emit->next_index();
    // append first part of pre ctor
    emit->append( top = new Chuck_Instr_Pre_Ctor_Array_Top( type ) );
    // call pre constructor
    emit_engine_pre_constructor( emit, type );
    // append second part of the pre ctor
    emit->append( bottom = new Chuck_Instr_Pre_Ctor_Array_Bottom );
    // set the goto of the first one
    top->set( emit->next_index() );
    // set the goto for the second one
    bottom->set( start_index );
    // clean up code
    emit->append( new Chuck_Instr_Pre_Ctor_Array_Post );

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: emit_engine_instantiate_object()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_instantiate_object( Chuck_Emitter * emit, Chuck_Type * type,
                                         a_Array_Sub array, t_CKBOOL is_ref )
{
    // if array
    if( type->array_depth )
    {
        // emit indices
        emit_engine_emit_exp( emit, array->exp_list );
        // emit array allocation
        Chuck_Instr * instr = NULL;
        emit->append( instr = new Chuck_Instr_Array_Alloc( emit->env,
            type->array_depth, type->array_type, emit->code->frame->curr_offset,
            is_ref ) );
        instr->set_linepos( array->linepos );

        // handle constructor
        if( isobj( emit->env, type->array_type ) && !is_ref )
        {
            // call pre constructor for array
            emit_engine_pre_constructor_array( emit, type->array_type );
        }
    }
    else if( !is_ref ) // not array
    {
        // emit object instantiation code, include pre constructor
        emit->append( new Chuck_Instr_Instantiate_Object( type ) );
        
        // call pre constructor
        emit_engine_pre_constructor( emit, type );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_decl()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_decl( Chuck_Emitter * emit, a_Exp_Decl decl,
                                    t_CKBOOL first_exp )
{
    a_Var_Decl_List list = decl->var_decl_list;
    a_Var_Decl var_decl = NULL;
    Chuck_Value * value = NULL;
    Chuck_Type * type = NULL;
    Chuck_Local * local = NULL;
    t_CKBOOL is_obj = FALSE;
    t_CKBOOL is_ref = FALSE;
    t_CKBOOL is_init = FALSE;
    
    t_CKTYPE t = type_engine_find_type( emit->env, decl->type->xid );
    te_ExternalType externalType;
    
    if( decl->is_external )
    {
        if( isa( t, emit->env->t_int ) )
        {
            externalType = te_externalInt;
        }
        else if( isa( t, emit->env->t_float ) )
        {
            externalType = te_externalFloat;
        }
        else if( isa( t, emit->env->t_event ) )
        {
            // kind-of-event (te_Type for this would be te_user, which is not helpful)
            externalType = te_externalEvent;
        }
        else
        {
            // fail if type unsupported
            EM_error2( decl->linepos, (std::string("unsupported type for external keyword: ") + t->name).c_str() );
            EM_error2( decl->linepos, "... (supported types: int, float, Event)" );
            return FALSE;
        }
    }

    // loop through vars
    while( list )
    {
        // the var
        var_decl = list->var_decl;
        // get the value determined in type checker
        value = var_decl->value;
        // get the type of the value
        type = value->type;
        // is the variable a reference
        is_obj = isobj( emit->env, type );
        // do alloc or not
        is_ref = decl->type->ref;
        // not init
        is_init = FALSE;

        // if this is an object
        if( is_obj )
        {
            // if array, then check to see if empty []
            if( list->var_decl->array )
            {
                if( list->var_decl->array->exp_list )
                {
                    // set
                    is_init = TRUE;
                    // instantiate object, including array
                    if( !emit_engine_instantiate_object( emit, type, list->var_decl->array, is_ref ) )
                        return FALSE;
                }
            }
            else if( !is_ref )
            {
                // REFACTOR-2017: don't emit instructions to instantiate
                // external variables -- they are init/instantiated
                // during emit (see below in this function)
                if( !decl->is_external )
                {
                    // set
                    is_init = TRUE;
                    // instantiate object (not array)
                    if( !emit_engine_instantiate_object( emit, type, list->var_decl->array, is_ref ) )
                        return FALSE;
                }
            }
        }

        // if not init and first exp
        /* if( !is_init && first_exp )
        {
            // push 0
            if( type->size == sz_INT ) // ISSUE: 64-bit
                emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            else if( type->size == sz_FLOAT ) // ISSUE: 64-bit
                emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
            else if( type->size == sz_COMPLEX ) // ISSUE: 64-bit
                emit->append( new Chuck_Instr_Reg_Push_Imm4( 0.0, 0.0 ) );
            else
            {
                EM_error2( decl->linepos,
                    "(emit): unhandle decl size of '%i'...",
                    type->size );
                return FALSE;
            }
        }*/

        // put in the value

        // member
        if( value->is_member )
        {
            // zero out location in object, and leave addr on operand stack
            // added 1.3.1.0: iskindofint -- on some 64-bit systems, sz_int == sz_FLOAT
            if( type->size == sz_INT && iskindofint(emit->env, type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Alloc_Member_Word( value->offset ) );
            else if( type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Alloc_Member_Word2( value->offset ) );
            else if( type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Alloc_Member_Word4( value->offset ) );
            else if( type->size == sz_VEC3 ) // ge: added 1.3.5.3
                emit->append( new Chuck_Instr_Alloc_Member_Vec3( value->offset ) );
            else if( type->size == sz_VEC4 ) // ge: added 1.3.5.3
                emit->append( new Chuck_Instr_Alloc_Member_Vec4( value->offset) );
            else
            {
                EM_error2( decl->linepos,
                    "(emit): internal error: unhandled decl size of '%i'...",
                    type->size );
                return FALSE;
            }
        }
        else // not member
        {
            // not in class
            if( !emit->env->class_def || !decl->is_static )
            {
                // allocate a place on the local stack
                // (added 1.3.0.0 -- is_obj for tracking objects ref count on stack)
                local = emit->alloc_local( type->size, value->name,
                    is_ref, is_obj, decl->is_external );
                if( !local )
                {
                    EM_error2( decl->linepos,
                        "(emit): internal error: cannot allocate local '%s'...",
                        value->name.c_str() );
                    return FALSE;
                }

                // put in the value
                value->offset = local->offset;

                // REFACTOR-2017: external declaration
                if( decl->is_external )
                {
                    Chuck_Instr_Alloc_Word_External * instr = new Chuck_Instr_Alloc_Word_External();
                    instr->m_name = value->name;
                    instr->m_type = externalType;
                    instr->set_linepos( decl->linepos );
                    emit->append( instr );
                    
                    // if it's an event, we need to initialize it and check if the exact type matches
                    if( externalType == te_externalEvent )
                    {
                        // init and construct it now!
                        if( !emit->env->vm()->init_external_event( value->name, t ) )
                        {
                            // if the type doesn't exactly match (different kinds of Event), then fail.
                            EM_error2( decl->linepos,
                                "(emit): external Event '%s' has different type '%s' than already existing external Event of the same name",
                                value->name.c_str(), t->name.c_str() );
                            return FALSE;
                        }
                    }
                    
                }
                // zero out location in memory, and leave addr on operand stack
                // TODO: this is wrong for static
                // BAD:
                // FIX:
                // added 1.3.1.0: iskindofint -- since on some 64-bit systems, sz_INT == sz_FLOAT
                else if( type->size == sz_INT && iskindofint(emit->env, type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                {
                    // (added 1.3.0.0 -- is_obj)
                    Chuck_Instr_Alloc_Word * instr = new Chuck_Instr_Alloc_Word( local->offset, is_obj );
                    instr->set_linepos( decl->linepos );
                    emit->append( instr );
                }
                else if( type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                {
                    Chuck_Instr_Alloc_Word2 * instr = new Chuck_Instr_Alloc_Word2( local->offset );
                    instr->set_linepos( decl->linepos );
                    emit->append( instr );
                }
                else if( type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                {
                    emit->append( new Chuck_Instr_Alloc_Word4( local->offset ) );
                }
                else if( type->size == sz_VEC3 ) // ge: added 1.3.5.3
                {
                    emit->append( new Chuck_Instr_Alloc_Vec3(local->offset) );
                }
                else if( type->size == sz_VEC4 ) // ge: added 1.3.5.3
                {
                    emit->append( new Chuck_Instr_Alloc_Vec4(local->offset) );
                }
                else
                {
                    EM_error2( decl->linepos,
                        "(emit): internal error: unhandled decl size of '%i'...",
                        type->size );
                    return FALSE;
                }
            }
            else // static
            {
                // emit the type
                emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)emit->env->class_def ) );
                // emit the static value (1.3.1.0: changed to use getkindof in addition to size)
                emit->append( new Chuck_Instr_Dot_Static_Data(
                    value->offset, value->type->size, getkindof(emit->env, value->type), TRUE ) );
            }
        }

        // set is_init
        is_init = FALSE;

        // if object, assign
        if( is_obj )
        {
            // if array
            if( var_decl->array )
            {
                // if not []
                if( var_decl->array->exp_list )
                {
                    // set
                    is_init = TRUE;
                    // assign
                    emit->append( new Chuck_Instr_Assign_Object );
                }
            }
            else if( !is_ref )
            {
                // REFACTOR-2017: don't add an Assign_Object instruction for
                // external objects -- they should be instantiated during emit,
                // not during runtime and therefore don't need an assign instr
                if( !decl->is_external )
                {
                    // set
                    is_init = TRUE;
                    // assign the object
                    emit->append( new Chuck_Instr_Assign_Object );
                }
            }
        }

        // if not init and first exp
        /* if( !is_init && first_exp )
        {
            // if obj
            if( is_obj )
                emit->append( new Chuck_Instr_Assign_Object );
            // size 4 primitive
            // added 1.3.1.0: iskindofint -- since on some 64-bit systems, sz_INT == sz_FLOAT
            else if( type->size == sz_INT && iskindofint(type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Assign_Primitive );
            // size 8 primitive
            else if( type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Assign_Primitive2 );
            // size 16 primitive
            else if( type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Assign_Primitive4 );
            else
                assert( FALSE );
        }*/
        
        list = list->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_code_segment()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_code_segment( Chuck_Emitter * emit, 
                                        a_Stmt_Code stmt, t_CKBOOL push )
{
    a_Stmt_List list = stmt->stmt_list;

    // loop through
    while( list )
    {
        // emit the statement
        if( !emit_engine_emit_stmt( emit, list->stmt ) )
            return FALSE;

        // next
        list = list->next;
    }

    // TODO: push
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_func_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_func_def( Chuck_Emitter * emit, a_Func_Def func_def )
{
    // get the func
    Chuck_Func * func = func_def->ck_func;
    // get the value
    Chuck_Value * value = func->value_ref;
    // get the type
    Chuck_Type * type = value->type;
    // local
    Chuck_Local * local = NULL;

    // make sure it's the same one
    //Chuck_Func * func2 = emit->env->context->nspc.lookup_func( func_def->name, FALSE );
    //if( func != func2 )
    //{
    //    EM_error2( func_def->linepos,
    //        "(emit): ambiguous function resolution for %s...",
    //        S_name(func_def->name) );
    //    return FALSE;
    //}

    // make sure the code is empty
    if( func->code != NULL )
    {
        EM_error2( func_def->linepos,
            "(emit): function '%s' already emitted...",
            S_name(func_def->name) );
        return FALSE;
    }

    // make sure we are not in a function already
    if( emit->env->func != NULL )
    {
        EM_error2( func_def->linepos,
            "(emit): internal error: nested function definition..." );
        return FALSE;
    }

    // if not part of a class
    if( !emit->env->class_def )
    {
        // put function on stack
        // ge: added FALSE to the 'is_obj' argument, 2012 april (added 1.3.0.0)
        local = emit->alloc_local( value->type->size, value->name, TRUE, FALSE, FALSE );
        // remember the offset
        value->offset = local->offset;
        // write to mem stack
        emit->append( new Chuck_Instr_Mem_Set_Imm( value->offset, (t_CKUINT)func ) );
    }

    // set the func
    emit->env->func = func;
    // push the current code
    emit->stack.push_back( emit->code );
    // make a new one
    emit->code = new Chuck_Code;
    // name the code
    emit->code->name = emit->env->class_def ? emit->env->class_def->name + "." : "";
    emit->code->name += func->name + "( ... )";
    // set whether need this
    emit->code->need_this = func->is_member;
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;

    // go through the args
    a_Arg_List a = func_def->arg_list;
    t_CKBOOL is_obj = FALSE; // (added 1.3.0.0)
    t_CKBOOL is_ref = FALSE;

    // if member (non-static) function
    if( func->is_member )
    {
        // get the size
        emit->code->stack_depth += sizeof(t_CKUINT);
        // add this
        // ge: added FALSE to the 'is_obj' argument, 2012 april (added 1.3.0.0)
        if( !emit->alloc_local( sizeof(t_CKUINT), "this", TRUE, FALSE, FALSE ) )
        {
            EM_error2( a->linepos,
                "(emit): internal error: cannot allocate local 'this'..." );
            return FALSE;
        }
    }
    
    // ge: 2012 april: added push scope (added 1.3.0.0)
    emit->push_scope();

    // loop through args
    while( a )
    {
        // get the value
        value = a->var_decl->value;
        // get the type
        type = value->type;
        // is object? (added ge: 2012 april | added 1.3.0.0)
        is_obj = isobj( emit->env, type );
        // get ref
        is_ref = a->type_decl->ref;
        // get the size
        emit->code->stack_depth += type->size;
        // allocate a place on the local stack
        // ge: added 'is_obj' 2012 april
        local = emit->alloc_local( type->size, value->name, is_ref, is_obj, FALSE );
        if( !local )
        {
            EM_error2( a->linepos,
                "(emit): internal error: cannot allocate local '%s'...",
                value->name.c_str() );
            return FALSE;
        }
        // remember the offset
        value->offset = local->offset;

        // advance
        a = a->next;
    }

    // TODO: make sure the calculated stack depth is the same as func_def->stack depth
    // taking into account member function
    
    // add references for objects in the arguments (added 1.3.0.0)
    // NOTE: this isn't in use since currently the caller is reference counting
    // the arguments -- this is to better support sporking, which is more
    // asynchronous.  the code below is left in as reference for callee ref counting
    // emit->addref_on_scope();

    // emit the code
    if( !emit_engine_emit_stmt( emit, func_def->code, FALSE ) )
        return FALSE;
    
    // added by spencer June 2014 (1.3.5.0) 
    // ensure return
    if( func_def->ret_type && func_def->ret_type != emit->env->t_void )
    {
        emit->append( new Chuck_Instr_Reg_Push_Imm(0) );
        
        Chuck_Instr_Goto * instr = new Chuck_Instr_Goto( 0 );
        emit->append( instr );
        emit->code->stack_return.push_back( instr );
    }
    
    // ge: pop scope (2012 april | added 1.3.0.0)
    // TODO: ge 2012 april: pop scope? clean up function arguments? are argument properly ref counted?    
    emit->pop_scope();

    // set the index for next instruction for return statements
    for( t_CKUINT i = 0; i < emit->code->stack_return.size(); i++ )
        emit->code->stack_return[i]->set( emit->next_index() );
    // clear the return stack
    emit->code->stack_return.clear();
    // emit return statement
    emit->append( new Chuck_Instr_Func_Return );

    // vm code
    func->code = emit_to_code( emit->code, NULL, emit->dump );
    // add reference
    func->code->add_ref();
    
    // unset the func
    emit->env->func = NULL;
    // delete the code
    SAFE_DELETE( emit->code );
    // pop the code
    assert( emit->stack.size() );
    emit->code = emit->stack.back();
    emit->stack.pop_back();

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_class_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_class_def( Chuck_Emitter * emit, a_Class_Def class_def )
{
    // get the type
    Chuck_Type * type = class_def->type;
    // the return type
    t_CKBOOL ret = TRUE;
    // the class body
    a_Class_Body body = class_def->body;
    
    // make sure the code is empty
    if( type->info->pre_ctor != NULL && type->info->pre_ctor->instr != NULL )
    {
        EM_error2( class_def->linepos,
            "(emit): class '%s' already emitted...",
            type->name.c_str() );
        return FALSE;
    }

    // make sure we are not in a class already
    //if( emit->env->class_def != NULL )
    //{
    //    EM_error2( class_def->linepos,
    //        "(emit): internal error: nested class definition..." );
    //    return FALSE;
    //}

    // set the class
    emit->env->class_stack.push_back( emit->env->class_def );
    emit->env->class_def = type;
    // push the current code
    emit->stack.push_back( emit->code );
    // make a new one
    emit->code = new Chuck_Code;
    // name the code
    emit->code->name = string("class ") + type->name;
    // whether code needs this
    emit->code->need_this = TRUE;
    // if has constructor
    // if( type->has_constructor ) type->info->pre_ctor = new Chuck_VM_Code;
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;
 
    // get the size
    emit->code->stack_depth += sizeof(t_CKUINT);
    // add this
    // ge: added TRUE to the 'is_obj' argument, 2012 april (added 1.3.0.0)
    // TODO: verify this is right, and not over-ref counted / cleaned up?
    if( !emit->alloc_local( sizeof(t_CKUINT), "this", TRUE, TRUE, FALSE ) )
    {
        EM_error2( class_def->linepos,
            "(emit): internal error: cannot allocate local 'this'..." );
        return FALSE;
    }

    // emit the body
    while( body && ret )
    {
        // check the section
        switch( body->section->s_type )
        {
        case ae_section_stmt:
            ret = emit_engine_emit_stmt_list( emit, body->section->stmt_list );
            break;
        
        case ae_section_func:
            ret = emit_engine_emit_func_def( emit, body->section->func_def );
            break;
        
        case ae_section_class:
            ret = emit_engine_emit_class_def( emit, body->section->class_def );
            //EM_error2( body->section->class_def->linepos,
            //    "nested class definitions are not yet supported..." );
            //ret = FALSE;
            break;
        }
        
        // move to the next section
        body = body->next;
    }

    // if ok
    if( ret )
    {
        // emit return statement
        emit->append( new Chuck_Instr_Func_Return );
        // vm code
        type->info->pre_ctor = emit_to_code( emit->code, type->info->pre_ctor, emit->dump );
        // add reference
        type->info->pre_ctor->add_ref();
        // allocate static
        type->info->class_data = new t_CKBYTE[type->info->class_data_size];
        // verify
        if( !type->info->class_data )
        {
            // we have a problem
            CK_FPRINTF_STDERR( 
                "[chuck](emitter): OutOfMemory: while allocating static data '%s'\n", type->c_name() );
            // flag
            ret = FALSE;
        }
        else
        {
            // zero it out
            memset( type->info->class_data, 0, type->info->class_data_size );
        }
    }

    // check again
    if( !ret )
    {
        // clean
        SAFE_DELETE( type->info->pre_ctor );
    }

    // unset the class
    emit->env->class_def = emit->env->class_stack.back();
    emit->env->class_stack.pop_back();
    // delete the code
    SAFE_DELETE( emit->code );
    // pop the code
    assert( emit->stack.size() );
    emit->code = emit->stack.back();
    emit->stack.pop_back();

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_spork()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Exp_Func_Call exp )
{
    // spork
    Chuck_Instr_Mem_Push_Imm * op = NULL;
    
    // if member function, push this
    // TODO: this is a hack - what if exp is not func_call?
    // if( exp->ck_func->is_member )
    //     emit->append( new Chuck_Instr_Reg_Push_This );
    
    // evaluate args on sporker shred
    if( !emit_engine_emit_func_args( emit, exp ) )
        return FALSE;
    
    // emit func pointer on sporker shred
    if( !emit_engine_emit_exp( emit, exp->func ) )
    {
        EM_error2( exp->linepos,
                  "(emit): internal error in evaluating function call..." );
        return FALSE;
    }
    
    // push the current code
    emit->stack.push_back( emit->code );
    // make a new one (spork~exp shred)
    emit->code = new Chuck_Code;
    // handle need this
    emit->code->need_this = exp->ck_func->is_member;
    // name it: e.g. spork~foo [line 5]
    std::ostringstream name;
    name << "spork~"
         << exp->ck_func->name.substr( 0, exp->ck_func->name.find( "@" ))
         << " [line " << exp->linepos << "]";
    emit->code->name = name.str();
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;
    // push op
    op = new Chuck_Instr_Mem_Push_Imm( 0 );
    // emit the stack depth - we don't know this yet
    emit->append( op );
    
    // call the func on sporkee shred
    if( !emit_engine_emit_exp_func_call(
                                        emit,
                                        exp->ck_func,
                                        exp->ret_type,
                                        exp->linepos,
                                        TRUE ) )
        return FALSE;
    
    // emit the function call, with special flag
    // if( !emit_engine_emit_exp_func_call( emit, exp, TRUE ) )
    //     return FALSE;
    
    // done
    emit->append( new Chuck_Instr_EOC );
    // set the stack depth now that we know
    op->set( emit->code->stack_depth );
    
    // emit it
    Chuck_VM_Code * code = emit_to_code( emit->code, NULL, emit->dump );
    // remember it
    exp->ck_vm_code = code;
    // add reference
    exp->ck_vm_code->add_ref();
    // code->name = string("spork~exp");
    
    // restore the code to sporker shred
    assert( emit->stack.size() > 0 );
    emit->code = emit->stack.back();
    // pop
    emit->stack.pop_back();
    
    a_Exp e = exp->args;
    t_CKUINT size = 0;
    while( e ) { size += e->cast_to ? e->cast_to->size : e->type->size; e = e->next; }
    
    // handle member function
    // TODO: this is a hack - what if exp is not func_call?
    // if( emit->code->need_this )
    //     size += sz_INT; // (changed 1.3.1.0: 4 to sz_INT)

    // emit instruction that will put the code on the stack
    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)code ) );
    // emit spork instruction - this will copy, func, args, this
    emit->append( new Chuck_Instr_Spork( size ) );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_spork()
// desc: ...
//-----------------------------------------------------------------------------
//t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Stmt stmt )
//{
//    // push the current code
//    emit->stack.push_back( emit->code );
//    // make a new one (spork~exp shred)
//    emit->code = new Chuck_Code;
//    // handle need this
//    emit->code->need_this = emit->env->class_def ? TRUE : FALSE;
//    // name it
//    emit->code->name = "spork~exp";
//    // keep track of full path (added 1.3.0.0)
//    emit->code->filename = emit->context->full_path;
//    
//    // call the code on sporkee shred
//    if( !emit_engine_emit_stmt( emit, stmt, TRUE ) )
//        return FALSE;
//    
//    // done
//    emit->append( new Chuck_Instr_EOC );
//    
//    // emit it
//    Chuck_VM_Code * code = emit_to_code( emit->code, NULL, emit->dump );
//    
//    // restore the code to sporker shred
//    assert( emit->stack.size() > 0 );
//    emit->code = emit->stack.back();
//    // pop
//    emit->stack.pop_back();
//    
//    if( code->need_this )
//    {
//        // push this if needed
//        emit->append( new Chuck_Instr_Reg_Push_This );
//    }
//    // emit instruction that will put the code on the stack
//    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)code ) );
//    // emit spork instruction - this will copy, func, args, this
//    emit->append( new Chuck_Instr_Spork_Stmt( 0 ) );
//    
//    return TRUE;
//}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_symbol()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_symbol( Chuck_Emitter * emit, S_Symbol symbol, 
                                  Chuck_Value * v, t_CKBOOL emit_var,
                                  int linepos )
{
    // look up the value
    // Chuck_Value * v = emit->env->curr->lookup_value( symbol, TRUE );
    // it should be there
    if( !v )
    {
        // internal error
        EM_error2( linepos,
            "(emit): internal error: undefined symbol '%s'...",
            S_name(symbol) );
        return FALSE;
    }
    
    // if external, find what type
    // (due to user classes, this info is only available during emit)
    te_ExternalType external_type;
    if( v->is_external )
    {
        if( isa( v->type, emit->env->t_int ) )
        {
            external_type = te_externalInt;
        }
        else if( isa( v->type, emit->env->t_float ) )
        {
            external_type = te_externalFloat;
        }
        else if( isa( v->type, emit->env->t_event ) )
        {
            external_type = te_externalEvent;
        }
        else
        {
            // internal error
            EM_error2( linepos,
                "(emit): internal error: unknown external type '%s'...",
                v->type->name.c_str() );
            return FALSE;
        }
    }

    // if part of class - this only works because x.y is handled separately
    if( v->owner_class && (v->is_member || v->is_static) )
    {
        // make sure talking about the same class
        // this doesn't work since the owner class could be a super class
        // assert( v->owner_class == emit->env->class_def );
        //
        // try this (thanks Robin Davies)
        assert( isa( emit->env->class_def, v->owner_class ) /* || is_global( v->owner_class ) */ );

        // emit as this.v
        // BUG: passing "this" below might be bad if the exp is freed and string with it
        a_Exp base = new_exp_from_id( (char *)"this", linepos );
        a_Exp dot = new_exp_from_member_dot( base, (char *)v->name.c_str(), linepos );
        base->type = v->owner_class;
        dot->type = v->type;
        dot->dot_member.t_base = v->owner_class;
        dot->emit_var = emit_var;
        // emit it
        if( !emit_engine_emit_exp_dot_member( emit, &dot->dot_member ) )
        {
            // internal error
            EM_error2( linepos,
                "(emit): internal error: symbol transformation failed..." );
            return FALSE;
        }

        return TRUE;
    }

    // var or value
    if( emit_var )
    {
        // emit as addr
        if( v->is_external )
        {
            emit->append( new Chuck_Instr_Reg_Push_External_Addr( v->name, external_type ) );
        }
        else
        {
            emit->append( new Chuck_Instr_Reg_Push_Mem_Addr( v->offset, v->is_context_global ) );
        }
    }
    else
    {
        // special case
        if( v->func_ref )
            emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)v->func_ref ) );
        else if( v->is_external )
        {
            emit->append( new Chuck_Instr_Reg_Push_External( v->name, external_type ) );
        }
        // check size
        // (added 1.3.1.0: iskindofint -- since in some 64-bit systems, sz_INT == sz_FLOAT)
        else if( v->type->size == sz_INT && iskindofint(emit->env, v->type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem2( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem4( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_VEC3 ) // ge: added 1.3.5.3
            emit->append( new Chuck_Instr_Reg_Push_Mem_Vec3( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_VEC4 ) // ge: added 1.3.5.3
            emit->append( new Chuck_Instr_Reg_Push_Mem_Vec4( v->offset, v->is_context_global ) );
        else
        {
            // internal error
            EM_error2( linepos,
                "(emit): internal error: symbol '%s' has size '%i'...",
                S_name(symbol), v->type->size );
            return FALSE;
        }
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: addref_on_scope()
// desc: add references to locals on current scope (added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Emitter::addref_on_scope()
{
    // sanity
    assert( code != NULL );
    // clear locals
    locals.clear();
    // get the current scope
    code->frame->get_scope( locals );
    
    // iterate over locals
    for( int i = 0; i < locals.size(); i++ )
    {
        // get it
        Chuck_Local * local = locals[i];
        // check to see if it's an object
        if( local->is_obj )
        {
            // REFACTOR-2017: Don't do if local is external
            // Note: I don't think addref_on_scope() is used anywhere,
            // but I am doing this to mirror pop_scope() below, which IS used
            if( !local->is_external )
            {
                // emit instruction to add reference
                this->append( new Chuck_Instr_AddRef_Object2( local->offset ) );
            }
        }
    }
}




//-----------------------------------------------------------------------------
// name: pop_scope()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Emitter::pop_scope( )
{
    // sanity
    assert( code != NULL );
    // clear locals
    locals.clear();
    // get locals
    code->frame->pop_scope( locals );

    // ge (2012 april): attempt to free locals (added 1.3.0.0)
    // was -- TODO: free locals -- chievo!
    for( int i = 0; i < locals.size(); i++ )
    {
        // get local
        Chuck_Local * local = locals[i];
        // CK_STDCERR << "local: " << local->offset << " " << local->is_obj << CK_STDENDL;
        // check to see if it's an object
        if( local->is_obj )
        {
            // (REFACTOR-2017: don't release external objects)
            if( !local->is_external )
            {
                // emit instruction to release the object
                this->append( new Chuck_Instr_Release_Object2( local->offset ) );
            }
        }
        
        // reclaim local; null out to be safe
        SAFE_DELETE( local ); locals[i] = NULL;
    }
    
    // clear it
    locals.clear();
}




//-----------------------------------------------------------------------------
// name: find_dur()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Emitter::find_dur( const string & name, t_CKDUR * out )
{
    // sanity
    assert( env != NULL );
    assert( out != NULL );

    // zero
    *out = 0.0;
    // get value from env
    Chuck_Value * value = env->global()->lookup_value( name, FALSE );
    if( !value || !equals( value->type, env->t_dur ) ) return FALSE;
    // copy
    *out = *( (t_CKDUR *)value->addr );
    
    return TRUE;
}




/*
//-----------------------------------------------------------------------------
// name: emit_engine_instantiate_object()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_instantiate_object( Chuck_Emitter * emit, Chuck_Type * type,
                                         a_Array_Sub array, t_CKBOOL is_ref )
{
    // if array
    if( type->array_depth )
    {
        // emit indices
        emit_engine_emit_exp( emit, array->exp_list );
        // emit array allocation
        emit->append( new Chuck_Instr_Array_Alloc( 
            type->array_depth, type->array_type, emit->code->frame->curr_offset,
            is_ref ) );

        // handle constructor
        //if( isobj( emit->env, type->array_type ) && !is_ref )
        //{
        //    // TODO:
        //    EM_error2( array->linepos, "internal error: object array constructor not impl..." );
        //    return FALSE;
        //}
    }
    else if( !is_ref ) // not array
    {
        // if ugen
        //if( isa( type, emit->env->t_ugen ) )
        //{
        //    // get the ugen info
        //    Chuck_UGen_Info * info = decl->self->type->ugen;
        //    if( !info )
        //    {
        //        EM_error2( decl->linepos,
        //            "(emit): internal error: undefined ugen type '%s'",
        //            type->name.c_str() );
        //        return FALSE;
        //    }
        //    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)info ) );
        //    emit->append( new Chuck_Instr_UGen_Alloc() );
        //}
        //else
        //{

        // emit object instantiation code, include pre constructor
        emit->append( new Chuck_Instr_Instantiate_Object( type ) );

        //}
        
        // call pre constructor
        emit_engine_pre_constructor( emit, type );

        // constructor
        //if( type->has_constructor )
        //{
        //    // make sure
        //    assert( type->info->pre_ctor != NULL );
        //    // push this
        //    emit->append( new Chuck_Instr_Reg_Dup_Last );
        //    // push pre-constructor
        //    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)type->info->pre_ctor ) );
        //    // push frame offset
        //    emit->append( new Chuck_Instr_Reg_Push_Imm( emit->code->frame->curr_offset ) );
        //    // call the function
        //    if( type->info->pre_ctor->native_func != NULL )
        //        emit->append( new Chuck_Instr_Func_Call_Member( 0 ) );
        //    else
        //        emit->append( new Chuck_Instr_Func_Call );
        //}
    }

    return TRUE;
}*/
