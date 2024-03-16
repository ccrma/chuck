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
#include "chuck_globals.h" // added 1.4.1.0
#include "chuck_parse.h" // added 1.5.0.0
#include "util_string.h" // added 1.5.0.5
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
t_CKBOOL emit_engine_emit_foreach( Chuck_Emitter * emit, a_Stmt_ForEach stmt );
t_CKBOOL emit_engine_emit_while( Chuck_Emitter * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_do_while( Chuck_Emitter * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_until( Chuck_Emitter * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_do_until( Chuck_Emitter * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_loop( Chuck_Emitter * emit, a_Stmt_Loop stmt );
t_CKBOOL emit_engine_emit_break( Chuck_Emitter * emit, a_Stmt_Break br );
t_CKBOOL emit_engine_emit_continue( Chuck_Emitter * emit, a_Stmt_Continue cont );
t_CKBOOL emit_engine_emit_return( Chuck_Emitter * emit, a_Stmt_Return stmt );
t_CKBOOL emit_engine_emit_switch( Chuck_Emitter * emit, a_Stmt_Switch stmt );
t_CKBOOL emit_engine_emit_exp( Chuck_Emitter * emit, a_Exp exp, t_CKBOOL doAddRef = FALSE, a_Stmt enclosingStmt = NULL );
t_CKBOOL emit_engine_emit_exp_binary( Chuck_Emitter * emit, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op( Chuck_Emitter * emit, ae_Operator op, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary );
t_CKBOOL emit_engine_emit_op_unchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_op_upchuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_op_at_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_op_overload_binary( Chuck_Emitter * emit, a_Exp_Binary binary ); // 1.5.1.5
t_CKBOOL emit_engine_emit_op_overload_unary( Chuck_Emitter * emit, a_Exp_Unary unary ); // 1.5.1.5
t_CKBOOL emit_engine_emit_op_overload_postfix( Chuck_Emitter * emit, a_Exp_Postfix postfix ); // 1.5.1.5
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emitter * emit, a_Exp_Unary unary );
t_CKBOOL emit_engine_emit_exp_primary( Chuck_Emitter * emit, a_Exp_Primary exp );
t_CKBOOL emit_engine_emit_exp_cast( Chuck_Emitter * emit, a_Exp_Cast cast );
t_CKBOOL emit_engine_emit_exp_postfix( Chuck_Emitter * emit, a_Exp_Postfix postfix );
t_CKBOOL emit_engine_emit_exp_dur( Chuck_Emitter * emit, a_Exp_Dur dur );
t_CKBOOL emit_engine_emit_exp_array( Chuck_Emitter * emit, a_Exp_Array array );
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emitter * emit, Chuck_Func * func,
                                         Chuck_Type * type, t_CKUINT line, t_CKUINT where,
                                         t_CKBOOL spork = FALSE );
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
t_CKBOOL emit_engine_pre_constructor( Chuck_Emitter * emit, Chuck_Type * type, a_Ctor_Call ctor_info );
t_CKBOOL emit_engine_instantiate_object( Chuck_Emitter * emit, Chuck_Type * type,
                                         a_Ctor_Call ctor_info, a_Array_Sub array, t_CKBOOL is_ref,
                                         t_CKBOOL is_array_ref );
t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Exp_Func_Call exp );
t_CKBOOL emit_engine_emit_cast( Chuck_Emitter * emit, Chuck_Type * to, Chuck_Type * from, uint32_t where );
t_CKBOOL emit_engine_emit_symbol( Chuck_Emitter * emit, S_Symbol symbol,
                                  Chuck_Value * v, t_CKBOOL emit_var,
                                  t_CKUINT line, t_CKUINT where );
Chuck_Instr_Stmt_Start * emit_engine_track_stmt_refs_start( Chuck_Emitter * emit, a_Stmt stmt );
void emit_engine_track_stmt_refs_cleanup( Chuck_Emitter * emit, Chuck_Instr_Stmt_Start * start );
// disabled until further notice (added 1.3.0.0)
// t_CKBOOL emit_engine_emit_spork( Chuck_Emitter * emit, a_Stmt stmt );




//-----------------------------------------------------------------------------
// name: emit_engine_init()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Emitter * emit_engine_init( Chuck_Env * env )
{
    // log
    EM_log( CK_LOG_HERALD, "initializing emitter..." );

    // TODO: ensure this in a better way?
    // whatever t_CKUINT is defined as, it must be the same size as a pointer
    // system architecture can vary (e.g., 32-bit vs 64-bit) but this condition
    // is assumed to be true in the virtual machine
    assert( sizeof(t_CKUINT) == sizeof(void *) );

    // allocate new emit
    Chuck_Emitter * emit = new Chuck_Emitter;
    // set the reference
    emit->env = env;
    // add ref
    CK_SAFE_ADD_REF( emit->env );

    return emit;
}




//-----------------------------------------------------------------------------
// name: emit_engine_shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_shutdown( Chuck_Emitter *& emit )
{
    // check
    if( !emit ) return FALSE;

    // log
    EM_log( CK_LOG_SYSTEM, "shutting down emitter..." );
    // shut it down; this is system cleanup -- delete instead of release
    CK_SAFE_DELETE( emit );

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
    assert( emit->code_stack.size() == 0 );
    // make sure there is a context to emit
    assert( emit->env->context != NULL );
    // make sure no code
    assert( emit->env->context->nspc->pre_ctor == NULL );

    // log
    EM_log( CK_LOG_FINER, "(pass 4) emitting VM code..." );
                          // emit->env->context->filename.c_str() );
    // push indent
    EM_pushlog();
    // log how much
    EM_log( CK_LOG_FINEST, "target: %s", howmuch2str( how_much ) );

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
    emit->code_stack.clear();
    // whether code need this
    emit->code->need_this = TRUE;
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;
    // name the code
    emit->code->name = emit->code->filename != "" ? mini(emit->code->filename.c_str()) : "[anonymous]";
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

        // check success code
        if( !ret ) break;

        // the next
        prog = prog->next;
    }

    // 1.4.1.0 (jack): error-checking: was dac-replacement initted?
    // (see chuck_compile.h for an explanation on replacement dacs)
    if( emit->should_replace_dac )
    {
        if( !emit->env->vm()->globals_manager()->is_global_ugen_init( emit->dac_replacement ) )
        {
            EM_error2( 0, "compiler error: dac replacement '%s' was never initialized...",
                emit->dac_replacement.c_str() );
            EM_error2( 0, "...(hint: need to declare this variable as a global UGen)" );
            ret = FALSE;
        }

        // (also need to make sure it was constructed; see runtime error in
        // Chuck_Instr_Reg_Push_Global)
    }

    // check success code
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
    else // error | 1.5.0.8 (ge) added in case emit errors out
    {
        // clear the code stack
        Chuck_Code * code = NULL;
        // while stack not empty
        while( emit->code_stack.size() )
        {
            // get the top of the stack
            code = emit->code_stack.back();
            // manually delete the instructions
            for( t_CKUINT i = 0; i < code->code.size(); i++ )
            { CK_SAFE_DELETE( code->code[i] ); }
            // delete the code
            CK_SAFE_DELETE( code );
            // pop the stack
            emit->code_stack.pop_back();
        }
    }

    // clear the code
    CK_SAFE_DELETE( emit->code );

    // clear stmt_stack (no need to delete contents; only contains instructions that should be cleaned up above)
    emit->stmt_stack.clear();

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
    EM_log( CK_LOG_FINER, "emitting: %d VM instructions...",
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
    // set whether static inside class | 1.4.1.0 (ge) added
    code->is_static = in->is_static;
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
        EM_print2orange( "dumping %s:", in->name.c_str() );

        // uh
        EM_print2vanilla( "-------" );
        for( t_CKUINT i = 0; i < code->num_instr; i++ )
        {
            // check code str (PRE) | 1.5.0.0 (ge) added
            if( code->instr[i]->m_codestr_pre )
            {
                const vector<string> & codestr = *(code->instr[i]->m_codestr_pre);
                // loop
                for( t_CKINT c = 0; c < codestr.size(); c++ )
                {
                    // print the reconstructed code str
                    EM_print2blue( "%s", codestr[c].c_str() );
                }
            }

            // print the instruction
            EM_print2vanilla( "[%i] %s( %s )", i,
                              code->instr[i]->name(), code->instr[i]->params() );

            // check code str (POST) | 1.5.0.8 (ge) added
            if( code->instr[i]->m_codestr_post )
            {
                const vector<string> & codestr = *(code->instr[i]->m_codestr_post);
                // loop
                for( t_CKINT c = 0; c < codestr.size(); c++ )
                {
                    // print the reconstructed code str
                    EM_print2blue( "%s", codestr[c].c_str() );
                }
            }
        }
        EM_print2vanilla( "-------\n" );
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
// desc: emit a statement
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt( Chuck_Emitter * emit, a_Stmt stmt, t_CKBOOL pop )
{
    // empty stmt list
    if( !stmt )
        return TRUE;

    // return
    t_CKBOOL ret = TRUE;
    // next index
    t_CKUINT nextIndex = 0;
    // expression string
    string codestr;
    // closing codestr, as applicable
    string codestr_close;
    // codestr prefix, e.g., (/** loop conditional **/) for code dump
    string codestr_prefix = emit->codestr_context_top();
    t_CKBOOL hasPrefix = codestr_prefix.length() > 0;

    // get str
    codestr = absyn2str( stmt );
    // prefix?
    if( hasPrefix ) codestr = codestr_prefix + " " + codestr;
    // get next index | 1.5.0.0 (ge) added
    nextIndex = emit->next_index();

    // loop over statements
    switch( stmt->s_type )
    {
        case ae_stmt_exp:  // expression statement
        {
            // emit the expression(s) that make up this stmt
            ret = emit_engine_emit_exp( emit, stmt->stmt_exp, FALSE, stmt );
            if( !ret )
                return FALSE;

            // need to pop the final value from stack
            if( pop )
            {
                // exp
                a_Exp exp = stmt->stmt_exp;

                // HACK: this won't work if <<< >>> is not the first in an expression chain
                // <<< 1 >>>, 2, 3, 4 won't clean up the reg stack correctly
                // 1, <<< 2 >>>, 3 needs to be checked
                if( exp->s_type == ae_exp_primary && exp->primary.s_type == ae_primary_hack )
                    exp = exp->primary.exp;

                // need to pop in reverse order
                vector<a_Exp> pxe;
                // go
                while( exp )
                {
                    pxe.push_back( exp );
                    exp = exp->next;
                }

                // loop over the expressions that is the statement (prior to 1.5.0.0 comment: "HACK" hmmm...)
                for( t_CKINT i = pxe.size()-1; i >= 0; i-- )
                {
                    // exp to work with
                    exp = pxe[i];

                    // if zero size, move on
                    if( exp->type->size == 0 ) continue;
                    // make sure, the stmt-exp cannot be a cast-to
                    assert( exp->cast_to == NULL );

                    // if decl, then expect only one word per var
                    // added 1.3.1.0: iskindofint -- since on some 64-bit systems sz_INT == sz_FLOAT
                    if( exp->s_type == ae_exp_decl )
                    {
                        // (added 1.3.1.0 -- multiply by type size; #64-bit)
                        // (fixed 1.3.1.2 -- uh... decl should also be int-sized, so changed to INT/WORD)
                        emit->append( new Chuck_Instr_Reg_Pop_WordsMulti( exp->decl.num_var_decls * sz_INT / sz_WORD ) );
                    }
                    else if( exp->type->size == sz_INT && iskindofint(emit->env, exp->type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    {
                        // pop int value
                        // NOTE if obj left from exp being a func_call, the ref is now released by stmt mechanic | 1.5.1.7
                        emit->append( new Chuck_Instr_Reg_Pop_Int );

                        // is an object left on the stack for the stmt
                        // if( exp->s_type == ae_exp_func_call && isobj(emit->env, exp->type) )
                        // { emit->append( new Chuck_Instr_Release_Object3_Pop_Int ); }
                        // else // not an object
                        // { emit->append( new Chuck_Instr_Reg_Pop_Int ); }
                    }
                    else if( exp->type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    {
                        emit->append( new Chuck_Instr_Reg_Pop_Float );
                    }
                    else if( exp->type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    {
                        emit->append( new Chuck_Instr_Reg_Pop_Vec2ComplexPolar );
                    }
                    else if( exp->type->size == sz_VEC3 ) // ge: added 1.3.5.3
                    {
                        emit->append( new Chuck_Instr_Reg_Pop_WordsMulti(sz_VEC3/sz_WORD) );
                    }
                    else if( exp->type->size == sz_VEC4 ) // ge: added 1.3.5.3
                    {
                        emit->append( new Chuck_Instr_Reg_Pop_WordsMulti(sz_VEC4/sz_WORD) );
                    }
                    else
                    {
                        EM_error2( exp->where,
                                   "(emit): internal error: %i byte stack item unhandled...",
                                   exp->type->size );
                        return FALSE;
                    }
                }
            }

            // add semicolon to code str
            codestr += ";";
            break;
        }

        case ae_stmt_if:  // if statement
            ret = emit_engine_emit_if( emit, &stmt->stmt_if );
            codestr_close = "} /** " + codestr + " **/";
            codestr += " {";
            break;

        case ae_stmt_for:  // for statement
            ret = emit_engine_emit_for( emit, &stmt->stmt_for );
            codestr_close = "} /** " + codestr + " **/";
            codestr += " {";
            break;

        case ae_stmt_foreach:  // for statement
            ret = emit_engine_emit_foreach( emit, &stmt->stmt_foreach );
            codestr_close = "} /** " + codestr + " **/";
            codestr += " {";
            break;

        case ae_stmt_while:  // while statement
            if( stmt->stmt_while.is_do )
            {
                ret = emit_engine_emit_do_while( emit, &stmt->stmt_while );
                codestr_close = "} " + codestr + ";";
                codestr = "do { /** " + codestr + " **/";
            }
            else
            {
                ret = emit_engine_emit_while( emit, &stmt->stmt_while );
                codestr_close = "} /** " + codestr + " **/";
                codestr += " {";
            }
            break;

        case ae_stmt_until:  // until statement
            if( stmt->stmt_until.is_do )
            {
                ret = emit_engine_emit_do_until( emit, &stmt->stmt_until );
                codestr_close = "} " + codestr + ";";
                codestr = "do { /** " + codestr + " **/";
            }
            else
            {
                ret = emit_engine_emit_until( emit, &stmt->stmt_until );
                codestr_close = "} /** " + codestr + " **/";
                codestr += " {";
            }
            break;

        case ae_stmt_loop:  // loop statement
            ret = emit_engine_emit_loop( emit, &stmt->stmt_loop );
            codestr += " {";
            codestr_close = "}";
            break;

        case ae_stmt_break:  // break statement
            ret = emit_engine_emit_break( emit, &stmt->stmt_break );
            break;

        case ae_stmt_continue:  // continue statement
            ret = emit_engine_emit_continue( emit, &stmt->stmt_continue );
            break;

        case ae_stmt_return:  // return statement
            ret = emit_engine_emit_return( emit, &stmt->stmt_return );
            break;

        case ae_stmt_code:  // code segment
            ret = emit_engine_emit_code_segment( emit, &stmt->stmt_code );
            break;

        case ae_stmt_switch:  // switch statement
            // not implemented
            ret = FALSE;
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
            EM_error2( stmt->where,
                 "(emit): internal error: unhandled statement type '%i'...",
                 stmt->s_type );
            break;
    }

    // see if we added at least one instruction
    if( codestr != "" && nextIndex < emit->next_index() )
    {
        // PREPEND codestr (for instruction dump)
        emit->code->code[nextIndex]->prepend_codestr( codestr );
    }

    // see if need to add closing
    if( codestr_close != "" && nextIndex < emit->next_index()-1 )
    {
        // APPPEND closing
        emit->code->code[emit->next_index()-1]->append_codestr( codestr_close );
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
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
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
        if( isa( stmt->cond->type, emit->env->ckt_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->where,
            "(emit): internal error: unhandled type '%s' in if condition",
            stmt->cond->type->base_name.c_str() );
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

    // remember the next index
    t_CKUINT topOfElse = emit->next_index();
    // set the op's target
    op->set( emit->next_index() );

    // ge: 2012 april: scope for else body (added 1.3.0.0)
    {
        // push the stack, allowing for new local variables
        emit->push_scope();
        // emit the body (else)
        ret = emit_engine_emit_stmt( emit, stmt->else_body );
        if( !ret )
            return FALSE;
        // add else to dump string
        if( topOfElse < emit->next_index() )
            emit->code->code[topOfElse]->prepend_codestr( "} else {" );
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
// desc: for( C1; C2; C3 ) { BODY }
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_for( Chuck_Emitter * emit, a_Stmt_For stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    // codestr prefix for better description
    string codestr_prefix = "/** loop conditional **/";
    // codestr push the context for instruction description / dump
    emit->codestr_context_push( codestr_prefix );

    // push the stack
    emit->push_scope();

    // C1 could be empty
    if( stmt->c1 )
    {
        // emit the cond
        ret = emit_engine_emit_stmt( emit, stmt->c1 );
        if( !ret ) return FALSE;
    }

    // the start index
    t_CKUINT start_index = emit->next_index();
    t_CKUINT cont_index = 0;
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // emit the cond (C2) -- keep the result on the stack
    ret = emit_engine_emit_stmt( emit, stmt->c2, FALSE );
    if( !ret ) return FALSE;

    // should not be NULL; in chuck C2 is not optional
    // (enforced earlier in the compilation)
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
            if( isa( stmt->c2->stmt_exp->type, emit->env->ckt_io ) )
            {
                // push 0
                emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
                op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
                break;
            }

            EM_error2( stmt->c2->stmt_exp->where,
                "(emit): internal error: unhandled type '%s' in for conditional",
                stmt->c2->stmt_exp->type->base_name.c_str() );
            return FALSE;
        }
        // append the op
        emit->append( op );
    }

    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // pop the codestr context
    emit->codestr_context_pop();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret ) return FALSE;

    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();

    // continue here
    cont_index = emit->next_index();

    // emit the action (C3)
    if( stmt->c3 )
    {
        // get the code str
        string codestr = absyn2str( stmt->c3 );
        // get the index
        t_CKUINT c3_index = emit->next_index();
        // emit the expression
        ret = emit_engine_emit_exp( emit, stmt->c3, FALSE, stmt->self );
        if( !ret ) return FALSE;
        // add code str
        emit->code->code[c3_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
            else if( e->type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
                num_words += sz_VEC2 / sz_WORD; // changed to compute number of words; 1.3.1.0
            else if( e->type->size != 0 )
            {
                EM_error2( e->where,
                    "(emit): internal error: non-void type size %i unhandled...",
                    e->type->size );
                return FALSE;
            }

            // advance
            e = e->next;
        }

        // pop (changed to Chuck_Instr_Reg_Pop_WordsMulti in 1.3.1.0)
        if( num_words > 0 ) emit->append( new Chuck_Instr_Reg_Pop_WordsMulti( num_words ) );
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
// name: emit_engine_emit_foreach() | 1.5.0.8 (ge) added
// desc: emit instructions for( VAR : ARRAY )
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_foreach( Chuck_Emitter * emit, a_Stmt_ForEach stmt )
{
    Chuck_Instr_ForEach_Inc_And_Branch * op = NULL;
    t_CKBOOL ret = TRUE;
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop control **/";
    string codestr;
    t_CKUINT cond_index = 0;

    // push the stack
    emit->push_scope();

    //-------------------------------------------------------------------------
    // emit VAR
    //-------------------------------------------------------------------------
    // get the code str | 1.5.0.8
    codestr = absyn2str( stmt->theIter );
    // get the index
    cond_index = emit->next_index();
    // emit the iter part (can/should only be a decl; no releasing of stmt->exp dangling references)
    ret = emit_engine_emit_exp( emit, stmt->theIter /*, FALSE, stmt->self */ );
    if( !ret ) return FALSE;
    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

    // check everything is in place
    if( stmt->theIter->s_type != ae_exp_decl ||
        !stmt->theIter->decl.var_decl_list ||
        !stmt->theIter->decl.var_decl_list->var_decl ||
        !stmt->theIter->decl.var_decl_list->var_decl->value )
    {
        EM_error2( stmt->theIter->where,
                   "(emit): internal error: cannot accessing foreach ITER value..." );
        return FALSE;
    }

    // get the iter
    Chuck_Value * theIter = stmt->theIter->decl.var_decl_list->var_decl->value;

    //-------------------------------------------------------------------------
    // emit ARRAY
    //------------------------------------------------------------------------
    // get the code str | 1.5.0.8
    codestr = absyn2str( stmt->theArray );
    // get the index
    cond_index = emit->next_index();
    // track dangling refs, but instead of doing this inside the emit_exp(),
    // we need to this here, outside, since we must ensure that the array left
    // on the stack can't be released until after we make the assignment to
    // @foreach_array | 1.5.1.7
    Chuck_Instr_Stmt_Start * start = emit_engine_track_stmt_refs_start( emit, stmt->self );
    // emit the array part
    ret = emit_engine_emit_exp( emit, stmt->theArray /*, FALSE, stmt->self */ );
    if( !ret ) return FALSE;
    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

    //-------------------------------------------------------------------------
    // emit implicit ARRAY REF
    //-------------------------------------------------------------------------
    // allocate a local variable to hold the array pointer
    Chuck_Local * localArray = emit->alloc_local( emit->env->ckt_int->size, "@foreach_array",
                                                  TRUE, TRUE, FALSE );
    // check
    if( !localArray )
    {
        EM_error2( stmt->theArray->where,
                   "(emit): internal error: cannot allocate local @foreach_array..." );
        return FALSE;
    }
    // emit our local array reference
    emit->append( new Chuck_Instr_Alloc_Word( localArray->offset, TRUE ) );
    // copy object
    emit->append( new Chuck_Instr_Assign_Object() );
    // clean up any dangling object refs | 1.5.1.7
    emit_engine_track_stmt_refs_cleanup( emit, start );

    //-------------------------------------------------------------------------
    // emit implicit iter COUNTER
    //-------------------------------------------------------------------------
    // allocate a local variable to hold the iterator
    Chuck_Local * localCounter = emit->alloc_local( emit->env->ckt_int->size, "@foreach_counter",
                                                    FALSE, FALSE, FALSE );
    // check
    if( !localCounter )
    {
        EM_error2( stmt->where,
                   "(emit): internal error: cannot allocate local @foreach_counter..." );
        return FALSE;
    }
    // emit our local counter; borrow theIter's is_context_global
    emit->append( new Chuck_Instr_Alloc_Word( localCounter->offset, FALSE ) );

    //-------------------------------------------------------------------------
    // emit conditional foreach BRANCH instruction
    //-------------------------------------------------------------------------
    // the start index
    t_CKUINT start_index = emit->next_index();

    // get data kind and size relevant to array
    te_KindOf dataKind = getkindof( emit->env, stmt->theIter->type );
    t_CKUINT dataSize = stmt->theIter->type->size;
    // compare branch condition and increment loop counter
    op = new Chuck_Instr_ForEach_Inc_And_Branch( dataKind, dataSize );
    // emit the instruction
    emit->append( op );

    //-------------------------------------------------------------------------
    // add STACK markers
    //-------------------------------------------------------------------------
    t_CKUINT cont_index = 0;
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    //-------------------------------------------------------------------------
    // emit BODY
    //-------------------------------------------------------------------------
    // added 1.3.1.1: new scope just for loop body
    emit->push_scope();

    // emit the body
    ret = emit_engine_emit_stmt( emit, stmt->body );
    if( !ret )
        return FALSE;

    // added 1.3.1.1: pop scope for loop body
    emit->pop_scope();

    //-------------------------------------------------------------------------
    // mark CONTINUE point
    //-------------------------------------------------------------------------
    // continue here
    cont_index = emit->next_index();

    //-------------------------------------------------------------------------
    // emit foreach ARRAY and ITER state
    //-------------------------------------------------------------------------
    // emit our local iter VAR
    emit->append( new Chuck_Instr_Reg_Push_Mem_Addr( theIter->offset, theIter->is_context_global ) );
    // emit our local array reference; borrow theIter's is_context_global
    emit->append( new Chuck_Instr_Reg_Push_Mem( localArray->offset, theIter->is_context_global ) );
    // emit our local counter; borry theIter's is_context_global
    emit->append( new Chuck_Instr_Reg_Push_Mem_Addr( localCounter->offset, theIter->is_context_global ) );

    // mark it in the instruction description
    emit->code->code[cont_index]->prepend_codestr( codestr_prefix );

    //-------------------------------------------------------------------------
    // emit GOTO
    //-------------------------------------------------------------------------
    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );

    //-------------------------------------------------------------------------
    // immediately AFTER loop
    //-------------------------------------------------------------------------
    // set the op's target
    op->set( emit->next_index() );

    //-------------------------------------------------------------------------
    // deal with CONTINUEs
    //-------------------------------------------------------------------------
    // stack of continue
    while( emit->code->stack_cont.size() && emit->code->stack_cont.back() )
    {
        emit->code->stack_cont.back()->set( cont_index );
        emit->code->stack_cont.pop_back();
    }

    //-------------------------------------------------------------------------
    // deal with BREAKs
    //-------------------------------------------------------------------------
    // stack of break
    while( emit->code->stack_break.size() && emit->code->stack_break.back() )
    {
        emit->code->stack_break.back()->set( emit->next_index() );
        emit->code->stack_break.pop_back();
    }

    //-------------------------------------------------------------------------
    // end of SCOPE
    //-------------------------------------------------------------------------
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
// desc: while( COND ) { BODY }
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_while( Chuck_Emitter * emit, a_Stmt_While stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop conditional **/";

    // push stack
    emit->push_scope();

    // get the index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // get the code str | 1.5.0.8
    string codestr = absyn2str( stmt->cond );
    // get the index
    t_CKUINT cond_index = emit->next_index();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
    if( !ret ) return FALSE;

    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
        if( isa( stmt->cond->type, emit->env->ckt_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->where,
            "(emit): internal error: unhandled type '%s' in while conditional",
            stmt->cond->type->base_name.c_str() );
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
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop conditional **/";

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

    // get the code str | 1.5.0.8
    string codestr = absyn2str( stmt->cond );
    // get the index
    t_CKUINT cond_index = emit->next_index();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
    if( !ret ) return FALSE;

    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
        if( isa( stmt->cond->type, emit->env->ckt_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->where,
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
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop conditional **/";

    // push stack
    emit->push_scope();

    // get index
    t_CKUINT start_index = emit->next_index();
    // mark the stack of continue
    emit->code->stack_cont.push_back( NULL );
    // mark the stack of break
    emit->code->stack_break.push_back( NULL );

    // get the code str | 1.5.0.8
    string codestr = absyn2str( stmt->cond );
    // get the index
    t_CKUINT cond_index = emit->next_index();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
    if( !ret ) return FALSE;

    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
        if( isa( stmt->cond->type, emit->env->ckt_io ) )
        {
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Neq_int_IO_good( 0 );
            break;
        }

        EM_error2( stmt->cond->where,
            "(emit): internal error: unhandled type '%s' in until conditional",
            stmt->cond->type->base_name.c_str() );
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
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop conditional **/";

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

    // get the code str | 1.5.0.8
    string codestr = absyn2str( stmt->cond );
    // get the index
    t_CKUINT cond_index = emit->next_index();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
    if( !ret ) return FALSE;

    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
        EM_error2( stmt->cond->where,
             "(emit): internal error: unhandled type '%s' in do/until conditional",
             stmt->cond->type->base_name.c_str() );
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
    // codestr prefix for better description | 1.5.0.8
    string codestr_prefix = "/** loop conditional **/";

    Chuck_Instr * instr = NULL;

    // push stack
    emit->push_scope();

    // get the code str | 1.5.0.8
    string codestr = absyn2str( stmt->cond );
    // get the index
    t_CKUINT cond_index = emit->next_index();

    // emit the cond
    ret = emit_engine_emit_exp( emit, stmt->cond, FALSE, stmt->self );
    if( !ret ) return FALSE;

    // add code str | 1.5.0.8
    emit->code->code[cond_index]->prepend_codestr( codestr_prefix + " " + codestr );

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
    instr->set_linepos( stmt->line );

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
        EM_error2( stmt->cond->where,
            "(emit): internal error: unhandled type '%s' in while conditional",
            type->base_name.c_str() );

        return FALSE;
    }

    // append the op
    emit->append( op );

    // decrement the counter
    emit->append( instr = new Chuck_Instr_Dec_Loop_Counter() );
    instr->set_linepos( stmt->line );

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
    // track dangling refs, but instead of doing this inside the emit_exp(),
    // we need to this here, outside, since we must ensure that the object left
    // on the stack can't be released until after we make add_ref below | 1.5.1.7
    Chuck_Instr_Stmt_Start * start = emit_engine_track_stmt_refs_start( emit, stmt->self );

    // emit the value
    if( !emit_engine_emit_exp( emit, stmt->val /*, FALSE, stmt->self */ ) )
        return FALSE;

    // if return is an object type
    if( stmt->val && isobj( emit->env, stmt->val->type ) )
    {
        // add reference (to be released by the function caller)
        emit->append( new Chuck_Instr_Reg_AddRef_Object3 );
    }

    // clean up any dangling object refs | 1.5.1.7
    emit_engine_track_stmt_refs_cleanup( emit, start );

    // 1.5.0.0 (ge) | traverse and unwind current scope frames, release object
    // references given the current state of the stack frames; every `return`
    // statement needs its own scope unwinding; FYI: this does not change the
    // scope frames, which is needed to continue compilation for the rest
    // of the function; this does emit instructions (e.g., Release_Object2)
    // associated with this traversal action (also see 'break' and 'continue')
    emit->traverse_scope_on_jump();

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
// name: emit_engine_track_stmt_refs_start()
// desc: as needed start tracking stmt-level dangling object refs
//-----------------------------------------------------------------------------
Chuck_Instr_Stmt_Start * emit_engine_track_stmt_refs_start( Chuck_Emitter * emit, a_Stmt stmt )
{
    // the stmt_start to push into stack | 1.5.1.7
    Chuck_Instr_Stmt_Start * start = NULL;
    // see if we need to emit an instruction
    if( stmt && stmt->numObjsToRelease )
    {
        // emit statement release
        emit->append( start = new Chuck_Instr_Stmt_Start( stmt->numObjsToRelease ) );
        // push stack
        emit->stmt_stack.push_back( start );
    }
    // done; could be NULL
    return start;
}




//-----------------------------------------------------------------------------
// name: emit_engine_track_stmt_refs_cleanup()
// desc: as needed cleanup tracking stmt-level dangling object refs
//-----------------------------------------------------------------------------
void emit_engine_track_stmt_refs_cleanup( Chuck_Emitter * emit, Chuck_Instr_Stmt_Start * start )
{
    // if objs to release | 1.5.1.7
    if( start )
    {
        // emit statement release
        emit->append( new Chuck_Instr_Stmt_Cleanup( start ) );
        // pop stmt stack | 1.5.1.7 (ge) added
        emit->stmt_stack.pop_back();
    }
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp()
// desc: emit code for an expression
//       1.3.0.0 | added doAddRef -- typically this is set to TRUE for function
//        calls so that arguments on reg stack are accounted for; this is important
//        in case the object is released/reclaimed before the value is used;
//        one particular case is when sporking with a local object as argument)
//       1.5.1.7 | added enclosingStmt -- this should only be set by an enclosing
//        statement to track object references that needs releasing, e.g.,
//        returning Object from a function, or a `new` allocation; this argument
//        enables checking and emitting instructions to track and clean these
//        "dangling" reference at the stmt->exp level
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp( Chuck_Emitter * emit, a_Exp exp, t_CKBOOL doAddRef,
                               a_Stmt enclosingStmt )
{
    // for now...
    // assert( exp->next == NULL );

    // the stmt_start to push into stack | 1.5.1.7
    Chuck_Instr_Stmt_Start * start = emit_engine_track_stmt_refs_start( emit, enclosingStmt );

    // loop over
    while( exp )
    {
        // check expression format
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

            // if the function returns an Object | 1.5.1.7
            if( isobj( emit->env, exp->func_call.ret_type ) )
            {
                // the return needs to be released (later at the end of the stmt that contains this)
                Chuck_Instr_Stmt_Start * onStack = emit->stmt_stack.size() ? emit->stmt_stack.back() : NULL;
                // check it
                if( onStack )
                {
                    t_CKUINT offset = 0;
                    // acquire next offset
                    if( !onStack->nextOffset( offset ) ) return FALSE;
                    // append instruction
                    emit->append( new Chuck_Instr_Stmt_Remember_Object( onStack, offset ) );
                }
            }
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
            EM_error2( exp->where,
                 "(emit): internal error: unhandled expression type '%i'...",
                 exp->s_type );
            return FALSE;
        }

        // implicit cast
        if( exp->cast_to != NULL )
            if( !emit_engine_emit_cast( emit, exp->cast_to, exp->type, exp->where ) )
                return FALSE;

        // check if we need to handle ref (added 1.3.0.0)
        // (NOTE: cast shouldn't matter since pointer width should remain constant)
        if( doAddRef && isobj( emit->env, exp->type ) )
        {
            // add ref in place on the stack
            emit->append( new Chuck_Instr_Reg_AddRef_Object3() );
        }

        // next exp
        exp = exp->next;
    }

    // clean up any dangling object refs
    emit_engine_track_stmt_refs_cleanup( emit, start );

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
        emit->append( new Chuck_Instr_Reg_Pop_Int );

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
        emit->append( new Chuck_Instr_Reg_Pop_Int );

        // result of whole expression is now result of rhs
        right = emit_engine_emit_exp( emit, binary->rhs );
        if( !right )
            return FALSE;

        // set branch location
        op->set( emit->next_index() );

        return TRUE;
    }

    // whether to track object references on stack (added 1.3.0.2)
    t_CKBOOL doRefLeft = FALSE;
    t_CKBOOL doRefRight = FALSE;
    // check to see if this is a function call (added 1.3.0.2)
    // added ae_op_chuck and make sure not null, latter has type-equivalence with object types in certain contexts | 1.5.1.7
    if( binary->op == ae_op_chuck && isa( binary->rhs->type, emit->env->ckt_function ) && !isnull( emit->env, binary->rhs->type ) )
    {
        // take care of objects in terms of reference counting
        doRefLeft = TRUE;
    }
    // check operator overload | 1.5.1.5 (ge)
    t_CKBOOL op_overload = (binary->ck_overload_func != NULL);
    if( op_overload && isobj( emit->env, binary->lhs->type ) )
    {
        // treat as arguments
        doRefLeft = TRUE;
    }
    // check operator overload
    if( op_overload && isobj( emit->env, binary->rhs->type ) )
    {
        // treat as arguments
        doRefRight = TRUE;
    }

    // emit (doRef added 1.3.0.2)
    left = emit_engine_emit_exp( emit, binary->lhs, doRefLeft );
    right = emit_engine_emit_exp( emit, binary->rhs, doRefRight );

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

    // check operator overload; unlike in type_engine by this point we know
    // whether an operator should be using an explicit overloading
    if( binary->ck_overload_func )
    {
        // emit operator overload | 1.5.1.5 (ge) added
        return emit_engine_emit_op_overload_binary( emit, binary );
    }

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
        else if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
        {
            // concatenate
            emit->append( instr = new Chuck_Instr_Add_string );
            instr->set_linepos( lhs->line );
        }
        // left: string
        else if( isa( t_left, emit->env->ckt_string ) )
        {
            // + int
            if( isa( t_right, emit->env->ckt_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_string_int );
                instr->set_linepos( lhs->line );
            }
            else if( isa( t_right, emit->env->ckt_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_string_float );
                instr->set_linepos( lhs->line );
            }
            else
            {
                EM_error2( lhs->where,
                    "(emit): internal error: unhandled op '%s' %s '%s'",
                    t_left->c_name(), op2str( op ), t_right->c_name() );
                return FALSE;
            }
        }
        // right: string
        else if( isa( t_right, emit->env->ckt_string ) )
        {
            // + int
            if( isa( t_left, emit->env->ckt_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_int_string );
                instr->set_linepos( rhs->line );
            }
            else if( isa( t_left, emit->env->ckt_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_float_string );
                instr->set_linepos( rhs->line );
            }
            else
            {
                EM_error2( lhs->where,
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Add_vec2 );
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
            instr->set_linepos( lhs->line );
        }
        // time + dur
        else if( ( left == te_dur && right == te_time ) ||
            ( left == te_time && right == te_dur ) )
        {
            emit->append( instr = new Chuck_Instr_Add_double_Assign );
        }
        // string + string
        else if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
        {
            // concatenate
            emit->append( instr = new Chuck_Instr_Add_string_Assign );
            instr->set_linepos( lhs->line );
        }
        // right: string
        else if( isa( t_right, emit->env->ckt_string ) )
        {
            // + int
            if( isa( t_left, emit->env->ckt_int ) )
            {
                emit->append( instr = new Chuck_Instr_Add_int_string_Assign );
                // null exceptions will occur from string: use rhs, not lhs
                instr->set_linepos( rhs->line );
            }
            else if( isa( t_left, emit->env->ckt_float ) )
            {
                emit->append( instr = new Chuck_Instr_Add_float_string_Assign );
                instr->set_linepos( rhs->line );
            }
            else
            {
                EM_error2( lhs->where,
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Add_vec2_Assign );
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Minus_vec2 );
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Minus_vec2_Assign );
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
        if( left == te_float && right == te_vec2 ) // 1.5.1.7 (ge) added
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec2 );
        }
        else if( left == te_vec2 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec2_Times_float );
        }
        else if( left == te_float && right == te_vec3 )
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
        if( left == te_float && right == te_vec2 )
        {
            emit->append( instr = new Chuck_Instr_float_Times_vec2_Assign );
        }
        else if( left == te_float && right == te_vec3 )
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
        {
            emit->append( instr = new Chuck_Instr_Divide_double );
        }
        else if( left == te_vec2 && right == te_float )
        {
            emit->append( instr = new Chuck_Instr_vec2_Divide_float );
        }
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
                instr->set_linepos( rhs->line );
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
        {
            emit->append( instr = new Chuck_Instr_Divide_double_Assign );
        }
        else if( left == te_float && right == te_vec2 )
        {
            emit->append( instr = new Chuck_Instr_vec2_Divide_float_Assign );
        }
        else if( left == te_float && right == te_vec3 )
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
                instr->set_linepos( lhs->line );
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
        {
            // check size (1.3.1.0: changed to getkindof)
            t_CKUINT kind = getkindof( emit->env, t_left->array_type );
            // but if array depth > 1, we are actually dealing with pointers, hence back to INT
            if( t_left->array_depth > 1 ) kind = kindof_INT;
            emit->append( instr = new Chuck_Instr_Array_Append( kind ) );
            instr->set_linepos( lhs->line );
            break;
        }

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

    // -------------------------------- bool -----------------------------------
    case ae_op_eq:
        if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string )
            && !isa( t_left, emit->env->ckt_null ) && !isa( t_right, emit->env->ckt_null ) ) // !null
        {
            emit->append( instr = new Chuck_Instr_Op_string( op ) );
            instr->set_linepos( lhs->line );
        }
        else if( ( isa( t_left, emit->env->ckt_object ) && isa( t_right, emit->env->ckt_object ) ) ||
                 ( isa( t_left, emit->env->ckt_object ) && isnull( emit->env, t_right ) ) ||
                 ( isnull( emit->env,t_left ) && isa( t_right, emit->env->ckt_object ) ) ||
                 ( isnull( emit->env,t_left ) && isnull( emit->env, t_right ) ) )
        {
            // object reference compare
            emit->append( instr = new Chuck_Instr_Eq_int );
        }
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Eq_vec2 );
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
        if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string )
            && !isa( t_left, emit->env->ckt_null ) && !isa( t_right, emit->env->ckt_null ) ) // !null
            // added 1.3.2.0 (spencer)
        {
            emit->append( instr = new Chuck_Instr_Op_string( op ) );
            instr->set_linepos( lhs->line );
        }
        else if( ( isa( t_left, emit->env->ckt_object ) && isa( t_right, emit->env->ckt_object ) ) ||
                 ( isa( t_left, emit->env->ckt_object ) && isnull( emit->env, t_right ) ) ||
                 ( isnull( emit->env,t_left ) && isa( t_right, emit->env->ckt_object ) ) ||
                 ( isnull( emit->env,t_left ) && isnull( emit->env, t_right ) ) )
        {
            emit->append( instr = new Chuck_Instr_Neq_int );
        }
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
            case te_vec2:
                emit->append( instr = new Chuck_Instr_Neq_vec2 );
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
            if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->line );
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
            if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->line );
            }
            else if( isa( t_left, emit->env->ckt_io ) )
            {
                // output
                if( isa( t_right, emit->env->ckt_int ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_int );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_float ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_float );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_string ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_string );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_complex ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_complex );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_polar ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_polar );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_vec2 ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_vec2 );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_vec3 ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_vec3 );
                    instr->set_linepos( rhs->line );
                }
                else if( isa( t_right, emit->env->ckt_vec4 ) )
                {
                    emit->append( instr = new Chuck_Instr_IO_out_vec4 );
                    instr->set_linepos( rhs->line );
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
            if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->line );
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
            if( isa( t_left, emit->env->ckt_string ) && isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_Op_string( op ) );
                instr->set_linepos( lhs->line );
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
            instr->set_linepos( rhs->line );
            break;
        case te_float:
            emit->append( instr = new Chuck_Instr_IO_out_float );
            instr->set_linepos( rhs->line );
            break;
        default:
            if( isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_IO_out_string );
                instr->set_linepos( rhs->line );
            }
            break;
        }
        break;

    case ae_op_arrow_right:
        switch( right )
        {
        case te_int:
            emit->append( instr = new Chuck_Instr_IO_in_int );
            instr->set_linepos( rhs->line );
            break;
        case te_float:
            emit->append( instr = new Chuck_Instr_IO_in_float );
            instr->set_linepos( rhs->line );
            break;
        default:
            if( isa( t_right, emit->env->ckt_string ) )
            {
                emit->append( instr = new Chuck_Instr_IO_in_string );
                instr->set_linepos( rhs->line );
            }
            break;
        }
        break;

        //---------------------------- (error) --------------------------------
    default: break;
        EM_error2( binary->where,
            "(emit): internal error: unhandled op '%s' %s '%s'",
            t_left->c_name(), op2str(op), t_right->c_name() );
        return FALSE;
    }

    // make sure emit
    if( !instr )
    {
        EM_error2( binary->where,
            "(emit): internal error: unhandled op implementation '%s' %s '%s'",
            t_left->c_name(), op2str( op ), t_right->c_name() );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_stmt_remember_object() | 1.5.1.8
// desc: as needed (if type is Object), emit a Stmt_Remember_Object instruction
//       assuming that the greater Stmt_Start to Stmt_Cleanup mechanism is
//       handled correctly elsewhere
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt_remember_object( Chuck_Emitter * emit, Chuck_Type * type )
{
    // if the function returns an Object
    if( !isobj( emit->env, type ) )
        return FALSE;

    // the return needs to be released (later at the end of the stmt that contains this)
    Chuck_Instr_Stmt_Start * onStack = emit->stmt_stack.size() ? emit->stmt_stack.back() : NULL;
    // check it
    if( onStack )
    {
        t_CKUINT offset = 0;
        // acquire next offset
        if( !onStack->nextOffset( offset ) ) return FALSE;
        // append instruction
        emit->append( new Chuck_Instr_Stmt_Remember_Object( onStack, offset ) );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_overload_binary() | 1.5.1.5 (ge) added
// desc: emit binary operator overload
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_overload_binary( Chuck_Emitter * emit, a_Exp_Binary binary )
{
    // TODO: transforms local stack into args; add refs, variable mem to reg, etc.
    // push function pointer
    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)binary->ck_overload_func ) );
    // emit the function call
    if( !emit_engine_emit_exp_func_call( emit, binary->ck_overload_func, binary->self->type, binary->line, binary->where ) )
        return FALSE;

    // return type
    Chuck_Type * rtype = binary->ck_overload_func->type();
    // emit remember instr | 1.5.1.8
    if( isobj( emit->env, rtype ) ) emit_engine_emit_stmt_remember_object( emit, rtype );

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_overload_unary() | 1.5.1.5 (ge) added
// desc: emit unary (prefix) operator overload
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_overload_unary( Chuck_Emitter * emit, a_Exp_Unary unary )
{
    // TODO: transforms local stack into args; add refs, variable mem to reg, etc.
    // push function pointer
    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)unary->ck_overload_func ) );
    // emit the function call
    if( !emit_engine_emit_exp_func_call( emit, unary->ck_overload_func, unary->self->type, unary->line, unary->where ) )
        return FALSE;

    // return type
    Chuck_Type * rtype = unary->ck_overload_func->type();
    // emit remember instr | 1.5.1.8
    if( isobj( emit->env, rtype ) ) emit_engine_emit_stmt_remember_object( emit, rtype );

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_overload_postfix() | 1.5.1.5 (ge) added
// desc: emit unary (postfix) operator overload
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_overload_postfix( Chuck_Emitter * emit, a_Exp_Postfix postfix )
{
    // TODO: transforms local stack into args; add refs, variable mem to reg, etc.
    // push function pointer
    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)postfix->ck_overload_func ) );
    // emit the function call
    if( !emit_engine_emit_exp_func_call( emit, postfix->ck_overload_func, postfix->self->type, postfix->line, postfix->where ) )
        return FALSE;

    // return type
    Chuck_Type * rtype = postfix->ck_overload_func->type();
    // emit remember instr | 1.5.1.8
    if( isobj( emit->env, rtype ) ) emit_engine_emit_stmt_remember_object( emit, rtype );

    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op_chuck()
// desc: emit =>
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op_chuck( Chuck_Emitter * emit, a_Exp lhs, a_Exp rhs, a_Exp_Binary binary )
{
    // any implicit cast happens before this
    Chuck_Type * left = lhs->cast_to ? lhs->cast_to : lhs->type;
    Chuck_Type * right = rhs->cast_to ? rhs->cast_to : rhs->type;

    // the pointer
    Chuck_Instr * instr = NULL;

    // ugen => ugen
    if( isa( left, emit->env->ckt_ugen ) && isa( right, emit->env->ckt_ugen ) )
    {
        // link, flag as NOT upchuck
        emit->append( instr = new Chuck_Instr_UGen_Link( FALSE ) );
        instr->set_linepos( lhs->line );
        // done
        return TRUE;
    }

    // ugen[] => ugen[] (or permutation)
    if( ( isa( left, emit->env->ckt_ugen ) || ( isa( left, emit->env->ckt_array ) && isa( left->array_type, emit->env->ckt_ugen ) ) ) &&
        ( isa( right, emit->env->ckt_ugen ) || ( isa( right, emit->env->ckt_array ) && isa( right->array_type, emit->env->ckt_ugen ) ) ) )
    {
        // link, flag as NOT upchuck
        emit->append( instr = new Chuck_Instr_UGen_Array_Link( isa( left, emit->env->ckt_array ), isa( right, emit->env->ckt_array ), FALSE ) );
        instr->set_linepos( lhs->line );
        // done
        return TRUE;
    }

    // time advance
    if( isa( left, emit->env->ckt_dur ) && isa( right, emit->env->ckt_time ) && rhs->s_meta == ae_meta_var )
    {
        // add the two
        emit->append( new Chuck_Instr_Add_double );

        // see if rhs is 'now'
        if( rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
        {
            // advance time
            emit->append( instr = new Chuck_Instr_Time_Advance );
            instr->set_linepos( lhs->line );
        }

        return TRUE;
    }

    // time advance
    if( isa( left, emit->env->ckt_event ) && isa( right, emit->env->ckt_time ) && rhs->s_meta == ae_meta_var &&
        rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
    {
        // pop now
        emit->append( new Chuck_Instr_Reg_Pop_Float );
        emit->append( instr = new Chuck_Instr_Event_Wait );
        instr->set_linepos( lhs->line );

        return TRUE;
    }

    // input
    if( isa( left, emit->env->ckt_io ) )
    {
        if( isa( right, emit->env->ckt_int ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_int );
            instr->set_linepos( rhs->line );
            return TRUE;
        }
        else if( isa( right, emit->env->ckt_float ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_float );
            instr->set_linepos( rhs->line );
            return TRUE;
        }
        else if( isa( right, emit->env->ckt_string ) )
        {
            assert( rhs->s_meta == ae_meta_var );
            emit->append( instr = new Chuck_Instr_IO_in_string );
            instr->set_linepos( rhs->line );
            return TRUE;
        }
    }

    // func call
    // make sure not 'null' which also looks like any object | 1.5.1.7
    if( isa( right, emit->env->ckt_function ) && !isnull(emit->env,right) )
    {
        assert( binary->ck_func != NULL );

        // emit
        return emit_engine_emit_exp_func_call( emit, binary->ck_func, binary->self->type, binary->line, binary->where );
    }

    // assignment or something else
    if( isa( left, right ) )
    {
        // basic types?
        if( type_engine_check_primitive( emit->env, left ) || isa( left, emit->env->ckt_string ) )
        {
            // use at assign
            return emit_engine_emit_op_at_chuck( emit, lhs, rhs );
        }
    }

    // TODO: deal with const

    // no match
    EM_error2( lhs->where,
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
    if( isa( left, emit->env->ckt_ugen ) && isa( right, emit->env->ckt_ugen ) )
    {
        // no connect
        emit->append( new Chuck_Instr_UGen_UnLink );
    }
    else
    {
        EM_error2( lhs->where,
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
    if( isa( left, emit->env->ckt_uana ) && isa( right, emit->env->ckt_uana ) )
    {
        // connect; flag it as unchuck
        Chuck_Instr * instr = NULL;
        emit->append( instr = new Chuck_Instr_UGen_Link( TRUE ) );
        instr->set_linepos( lhs->line );
    }
    // uana[] =^ uana[] (or permutation)
    else if( ( isa( left, emit->env->ckt_uana ) || ( isa( left, emit->env->ckt_array ) && isa( left->array_type, emit->env->ckt_uana ) ) ) &&
        ( isa( right, emit->env->ckt_uana ) || ( isa( right, emit->env->ckt_array ) && isa( right->array_type, emit->env->ckt_uana ) ) ) )
    {
        // link, flag as upchuck
        Chuck_Instr * instr = NULL;
        emit->append( instr = new Chuck_Instr_UGen_Array_Link( isa( left, emit->env->ckt_array ), isa( right, emit->env->ckt_array ), TRUE ) );
        instr->set_linepos( lhs->line );
        // done
        return TRUE;
    }
    else
    {
        EM_error2( lhs->where,
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
        if( type_engine_check_primitive( emit->env, left ) || isa( left, emit->env->ckt_string ) )
        {
            // assigment?
            if( rhs->s_meta != ae_meta_var )
            {
                EM_error2( lhs->where,
                    "(emit): internal error: assignment to non-variable..." );
                return FALSE;
            }

            // see if rhs is 'now' - time => now
            if( rhs->s_type == ae_exp_primary && !strcmp( "now", S_name(rhs->primary.var) ) )
            {
                // pop the now addr
                emit->append( new Chuck_Instr_Reg_Pop_Float );
                // advance time
                Chuck_Instr * instr = NULL;
                emit->append( instr = new Chuck_Instr_Time_Advance );
                instr->set_linepos( lhs->line );
            }
            else if( isa( left, emit->env->ckt_string ) ) // string
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
                else if( right->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
                    emit->append( new Chuck_Instr_Assign_Primitive4 );
                else if( right->size == sz_VEC3 ) // ge: added 1.3.5.3
                    emit->append( new Chuck_Instr_Assign_PrimitiveVec3 );
                else if( right->size == sz_VEC4 ) // ge: added 1.3.5.3
                    emit->append( new Chuck_Instr_Assign_PrimitiveVec4 );
                else
                {
                    EM_error2( rhs->where,
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

    // TODO: deal with const

    // no match
    EM_error2( lhs->where,
        "(emit): internal error: unhandled '@=>' on types '%s' and '%s'...",
        left->c_name(), right->c_name() );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_unary()
// desc: emit unary (prefix) expression
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emitter * emit, a_Exp_Unary unary )
{
    // check operator overload | 1.5.1.5 (ge)
    t_CKBOOL op_overload = (unary->ck_overload_func != NULL);
    t_CKBOOL doRef = FALSE;
    if( op_overload && isobj( emit->env, unary->exp->type ) )
    {
        // treat as argument
        doRef = TRUE;
    }

    // exp
    if( unary->op != ae_op_spork && !emit_engine_emit_exp( emit, unary->exp, doRef ) )
        return FALSE;

    // get type
    Chuck_Type * t = unary->self->type;
    assert( t != NULL );

    // check overloading | 1.5.1.5 (ge) added
    if( unary->ck_overload_func )
    {
        // emit overloading | FYI spork can't be overloaded for now
        return emit_engine_emit_op_overload_unary( emit, unary );
    }

    // emit the operator
    switch( unary->op )
    {
    case ae_op_plusplus:
        // make sure assignment is legal
        if( unary->self->s_meta != ae_meta_var )  // TODO: const
        {
            EM_error2( unary->self->where,
                "(emit): target for '++' not mutable..." );
            return FALSE;
        }

        // increment
        if( equals( unary->exp->type, emit->env->ckt_int ) )
        {
            emit->append( new Chuck_Instr_PreInc_int );
        }
        else
        {
            // internal error
            EM_error2( unary->where,
                "(emit): internal error: unhandled type '%s' for pre '++'' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_minusminus:
        // make sure assignment is legal
        if( unary->self->s_meta != ae_meta_var )  // TODO: const
        {
            EM_error2( unary->self->where,
                "(emit): target for '--' not mutable..." );
            return FALSE;
        }

        // decrement
        if( equals( unary->exp->type, emit->env->ckt_int ) )
        {
            emit->append( new Chuck_Instr_PreDec_int );
        }
        else
        {
            // internal error
            EM_error2( unary->where,
                "(emit): internal error: unhandled type '%s' for pre '--' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_tilda:
        // complement
        if( equals( unary->exp->type, emit->env->ckt_int ) )
        {
            emit->append( new Chuck_Instr_Complement_int );
        }
        else
        {
            EM_error2( unary->where,
                "(emit): internal error: unhandled type '%s' for '~' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_exclamation:
        // !
        if( equals( unary->exp->type, emit->env->ckt_int ) )
        {
            emit->append( new Chuck_Instr_Not_int );
        }
        else
        {
            EM_error2( unary->where,
                "(emit): internal error: unhandled type '%s' for '!' operator",
                unary->exp->type->c_name() );
            return FALSE;
        }
        break;

    case ae_op_minus:
        // negate
        if( equals( unary->exp->type, emit->env->ckt_int ) )
            emit->append( new Chuck_Instr_Negate_int );
        else if( equals( unary->exp->type, emit->env->ckt_float ) )
            emit->append( new Chuck_Instr_Negate_double );
        else
        {
            EM_error2( unary->where,
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
            EM_error2( unary->where,
                "(emit): internal error: sporking non-function call..." );
            return FALSE;
        }
        break;

    case ae_op_new:
        // if this is an object
        if( isobj( emit->env, t ) )
        {
            // only check dependency violations if we are at a context-top-level
            // or class-top-level scope, i.e., not in a function definition
            if( !emit->env->func )
            {
                // dependency tracking: ensure object instantiation (e.g., Foo foo;) is not invoked
                // before dependencies are met | 1.5.0.8 (ge) added
                // NOTE if the type (t) originates from another file, this should
                // behave correctly and return NULL | 1.5.1.1 (ge) fixed
                const Chuck_Value_Dependency * unfulfilled = t->depends.locate( unary->where );
                // at least one unfulfilled
                if( unfulfilled )
                {
                    EM_error2( unary->type->where,
                              "'%s' instantiation at this point skips initialization of a needed variable:",
                              t->c_name() );
                    EM_error2( unfulfilled->where,
                              "...(note: this skipped variable initialization is needed by class '%s')",
                              t->c_name() );
                    EM_error2( unfulfilled->use_where,
                              "...(note: this is where the variable is used within class '%s')",
                              t->c_name() );
                    EM_error2( 0,
                              "...(hint: try instantiating '%s' after the variable initialization)", t->c_name() );
                    return FALSE;
                }
            }

            // should always be false; can't 'new int[]'...
            t_CKBOOL is_array_ref = FALSE;
            // instantiate object, including array
            if( !emit_engine_instantiate_object( emit, t, &unary->ctor, unary->array, unary->type->ref, is_array_ref ) )
                return FALSE;

            // the new needs to be addref, and released (later at the end of the stmt that contains this) | 1.5.1.8
            Chuck_Instr_Stmt_Start * onStack = emit->stmt_stack.size() ? emit->stmt_stack.back() : NULL;
            // check it
            assert( onStack != NULL );
            // offset variable
            t_CKUINT offset = 0;
            // acquire next offset
            if( !onStack->nextOffset( offset ) ) return FALSE;
            // append instruction, addRef=TRUE
            emit->append( new Chuck_Instr_Stmt_Remember_Object( onStack, offset, TRUE ) );
        }
        break;

    default:
        EM_error2( unary->where,
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
            double ckPi = 3.14159265358979323846;
            emit->append( new Chuck_Instr_Reg_Push_Imm2( ckPi ) );
        }
        else if( exp->var == insert_symbol( "dac" ) )
        {
            // 1.4.1.0 (jack): see chuck_compile.h for an explanation of
            // replacement dacs
            // should replace dac with global ugen?
            if( emit->should_replace_dac )
            {
                // push the global UGen on to the stack
                Chuck_Instr_Reg_Push_Global * instr =
                    new Chuck_Instr_Reg_Push_Global(
                        emit->dac_replacement, te_globalUGen );
                instr->set_linepos( exp->line );
                emit->append( instr );
            }
            else
            {
                // proceed as normal -- push the dac onto the stack~
                emit->append( new Chuck_Instr_DAC );
            }
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
                emit, exp->var, exp->value, exp->self->emit_var, exp->line, exp->where );
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
        if( !str || !initialize_object( str, emit->env->ckt_string, NULL, emit->env->vm() ) )
        {
            // error (TODO: why is this a CK_SAFE_RELEASE and not CK_SAFE_DELETE?)
            CK_SAFE_RELEASE( str );
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
        emit->append( new Chuck_Instr_Reg_Push_Imm( str2char(exp->chr, exp->where ) ) );
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
    emit->append( instr = new Chuck_Instr_Array_Init_Literal( emit->env, type, count ) );
    instr->set_linepos( array->line );

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

    // zero pad if needed, using smallest type: vec2 | 1.5.1.7
    t_CKINT n = sz_VEC2/sz_FLOAT - val->numdims;
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
    return emit_engine_emit_cast( emit, to, from, cast->where );
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_cast()
// desc: emit type cast instructions
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_cast( Chuck_Emitter * emit,
                                Chuck_Type * to, Chuck_Type * from,
                                uint32_t where )
{
    // if type is already the same
    if( equals( to, from ) )
        return TRUE;

    // int to float
    if( equals( to, emit->env->ckt_int ) && equals( from, emit->env->ckt_float ) )
        emit->append( new Chuck_Instr_Cast_double2int );
    // float to int
    else if( equals( to, emit->env->ckt_float ) && equals( from, emit->env->ckt_int ) )
        emit->append( new Chuck_Instr_Cast_int2double );
    else if( equals( to, emit->env->ckt_complex ) && equals( from, emit->env->ckt_int ) )
        emit->append( new Chuck_Instr_Cast_int2complex );
    else if( equals( to, emit->env->ckt_polar ) && equals( from, emit->env->ckt_int ) )
        emit->append( new Chuck_Instr_Cast_int2polar );
    else if( equals( to, emit->env->ckt_complex ) && equals( from, emit->env->ckt_float ) )
        emit->append( new Chuck_Instr_Cast_double2complex );
    else if( equals( to, emit->env->ckt_polar ) && equals( from, emit->env->ckt_float ) )
        emit->append( new Chuck_Instr_Cast_double2polar );
    else if( equals( to, emit->env->ckt_polar ) && equals( from, emit->env->ckt_complex ) )
        emit->append( new Chuck_Instr_Cast_complex2polar );
    else if( equals( to, emit->env->ckt_complex ) && equals( from, emit->env->ckt_polar ) )
        emit->append( new Chuck_Instr_Cast_polar2complex );
    else if( equals( to, emit->env->ckt_vec2 ) && equals( from, emit->env->ckt_vec3 ) ) // 1.5.1.7
        emit->append( new Chuck_Instr_Cast_vec3tovec2 );
    else if( equals( to, emit->env->ckt_vec2 ) && equals( from, emit->env->ckt_vec4 ) ) // 1.5.1.7
        emit->append( new Chuck_Instr_Cast_vec4tovec2 );
    else if( equals( to, emit->env->ckt_vec3 ) && equals( from, emit->env->ckt_vec2 ) ) // 1.5.1.7
        emit->append( new Chuck_Instr_Cast_vec2tovec3 );
    else if( equals( to, emit->env->ckt_vec4 ) && equals( from, emit->env->ckt_vec2 ) ) // 1.5.1.7
        emit->append( new Chuck_Instr_Cast_vec2tovec4 );
    else if( equals( to, emit->env->ckt_vec3 ) && equals( from, emit->env->ckt_vec4 ) )
        emit->append( new Chuck_Instr_Cast_vec4tovec3 );
    else if( equals( to, emit->env->ckt_vec4 ) && equals( from, emit->env->ckt_vec3 ) )
        emit->append( new Chuck_Instr_Cast_vec3tovec4 );
    else if( equals( to, emit->env->ckt_string ) && isa( from, emit->env->ckt_object ) && !isa( from, emit->env->ckt_string ) )
        emit->append( new Chuck_Instr_Cast_object2string );
    // verify runtime dynamic cast | 1.5.2.0 (ge) added
    else if( isobj(emit->env,to) && isa(to, from) )
    {
        // append instruction
        Chuck_Instr_Cast_Runtime_Verify * instr = NULL;
        emit->append( instr = new Chuck_Instr_Cast_Runtime_Verify(from,to) );
        // create format string %%s for output in case of exception
        instr->set_codeformat4exception( EM_error2str(where, TRUE, "cannot cast dynamic type '%%s' to '%%s'..." ) );
    }
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
    // check operator overload | 1.5.1.5 (ge)
    t_CKBOOL op_overload = (postfix->ck_overload_func != NULL);
    t_CKBOOL doRef = FALSE;
    if( op_overload && isobj( emit->env, postfix->exp->type ) )
    {
        // treat as argument
        doRef = TRUE;
    }

    // emit the exp
    if( !emit_engine_emit_exp( emit, postfix->exp, doRef ) )
        return FALSE;

    // check overloading | 1.5.1.5 (ge) added
    if( postfix->ck_overload_func )
    {
        // emit overloading
        return emit_engine_emit_op_overload_postfix( emit, postfix );
    }

    // emit
    switch( postfix->op )
    {
    case ae_op_plusplus:
        if( equals( postfix->exp->type, emit->env->ckt_int ) )
            emit->append( new Chuck_Instr_PostInc_int );
        else
        {
            EM_error2( postfix->where,
                "(emit): internal error: unhandled type '%s' for post '++' operator",
                postfix->exp->type->c_name() );
            return FALSE;
        }
    break;

    case ae_op_minusminus:
        if( equals( postfix->exp->type, emit->env->ckt_int ) )
            emit->append( new Chuck_Instr_PostDec_int );
        else
        {
            EM_error2( postfix->where,
                "(emit): internal error: unhandled type '%s' for post '--' operator",
                postfix->exp->type->c_name() );
            return FALSE;
        }
    break;

    default:
        EM_error2( postfix->where,
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
    if( equals( dur->base->type, emit->env->ckt_int ) )
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
        EM_error2( array->where,
            "(emit): internal error: array with 0 depth..." );
        return FALSE;
    }
    // get the sub
    sub = array->indices;
    if( !sub )
    {
        EM_error2( array->where,
            "(emit): internal error: NULL array sub..." );
        return FALSE;
    }
    // get the exp list
    exp = sub->exp_list;
    if( !exp )
    {
        EM_error2( array->where,
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
    if( isa( exp->type, emit->env->ckt_string ) )
        is_str = TRUE;

    // make sure
    // ISSUE: 64-bit (fixed 1.3.1.0)
    if( type->size != sz_INT && type->size != sz_FLOAT && type->size != sz_VEC2 &&
        type->size != sz_VEC3 && type->size != sz_VEC4 )
    {
        EM_error2( array->where,
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
        instr->set_linepos( array->line );
    }
    else
    {
        // the pointer
        Chuck_Instr_Array_Access_Multi * aam = NULL;
        // emit the multi array access (1.3.1.0: use getkindof instead of type->size)
        emit->append( aam = new Chuck_Instr_Array_Access_Multi( depth, getkindof(emit->env, type), is_var ) );
        aam->set_linepos( array->line );
        // add type info (1.3.1.0) -- to support mixed string & int indexing (thanks Robin Haberkorn)
        a_Exp e = exp;
        while( e )
        {
            // check if string
            aam->indexIsAssociative().push_back( isa( exp->type, emit->env->ckt_string ) );
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
                                         t_CKUINT line,
                                         t_CKUINT where,
                                         t_CKBOOL spork )
{
    // is a member?
    t_CKBOOL is_member = func->is_member;
    // is a static? (within class)
    t_CKBOOL is_static = func->is_static;

    // only check dependency violations if we are at a context-top-level
    // or class-top-level scope, i.e., not in a function definition
    // also, once sporked, it will be up the programmer to ensure intention
    if( !emit->env->func && !spork )
    {
        // dependency tracking: check if we invoke func before all its deps are initialized | 1.5.0.8 (ge) added
        // NOTE if func originates from another file, this should behave correctly and return NULL | 1.5.1.1 (ge) fixed
        // NOTE passing in emit->env->class_def, to differentiate dependencies across class definitions | 1.5.2.0 (ge) fixed
        const Chuck_Value_Dependency * unfulfilled = func->depends.locate( where, emit->env->class_def );
        // at least one unfulfilled dependency
        if( unfulfilled )
        {
            EM_error2( where,
                      "calling '%s()' at this point skips initialization of a needed variable:",
                      func->base_name.c_str() );
            EM_error2( unfulfilled->where,
                      "...(note: this skipped variable initialization is needed by '%s')",
                      func->signature().c_str() );
            EM_error2( unfulfilled->use_where,
                      "...(note: this is where the variable is used within '%s' or its subsidiaries)",
                      func->signature().c_str() );
            EM_error2( 0,
                      "...(hint: try calling '%s()' after the variable initialization)", func->base_name.c_str() );
            return FALSE;
        }
    }

    // translate to code
    emit->append( new Chuck_Instr_Func_To_Code );
    // emit->append( new Chuck_Instr_Reg_Push_Code( func->code ) );
    // push the local stack depth - local variables
    emit->append( new Chuck_Instr_Reg_Push_Imm( emit->code->frame->curr_offset ) );

    // call the function
    t_CKUINT size = type->size;
    t_CKUINT kind = getkindof( emit->env, type ); // added 1.3.1.0

    // the pointer
    Chuck_Instr * instr = NULL;
    if( func->def()->s_type == ae_func_builtin )
    {
        // ISSUE: 64-bit (fixed 1.3.1.0)
        if( size == 0 || size == sz_INT || size == sz_FLOAT || size == sz_VEC2 ||
            size == sz_VEC3 || sz_VEC4 )
        {
            // is member (1.3.1.0: changed to use kind instead of size)
            if( is_member )
                emit->append( instr = new Chuck_Instr_Func_Call_Member( kind, func ) );
            else if( is_static )
                emit->append( instr = new Chuck_Instr_Func_Call_Static( kind, func ) );
            else // 1.5.1.5 (ge & andrew) new planes of existence --> this is in global-scope (not global variable)
                emit->append( instr = new Chuck_Instr_Func_Call_Global( kind, func ) );
        }
        else
        {
            EM_error2( where,
                       "(emit): internal error: %i func call not handled",
                       size );
            return FALSE;
        }
    }
    else
    {
        emit->append( instr = new Chuck_Instr_Func_Call );
    }
    // set line position
    instr->set_linepos(line);

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
        EM_error2( func_call->where,
                   "(emit): internal error in emitting function call arguments..." );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_func_call()
// desc: emit function call from a_Exp_Func_Call
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
        EM_error2( func_call->where,
                   "(emit): internal error in evaluating function call..." );
        return FALSE;
    }

    // line and pos
    t_CKUINT line = func_call->line;
    t_CKUINT where = func_call->where;
    // if possible, get more accurate code position
    if( func_call->func->s_type == ae_exp_dot_member )
    {
        line = func_call->func->dot_member.line;
        where = func_call->func->dot_member.where;
    }

    // the rest
    return emit_engine_emit_exp_func_call( emit, func_call->ck_func, func_call->ret_type,
                                           line, where, spork );
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_dot_member_special()
// desc: emit special dot member: complex, polar, vec2, vec3, vec4
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
            EM_error2( member->base->where,
                      "(emit): cannot assign value to literal 'complex' value..." );
            return FALSE;
        }

        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;

        string str = S_name(member->xid);
        // check
        if( str == "re" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr, kindof_VEC2 ) );
        else if( str == "im" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr, kindof_VEC2 ) );
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
            EM_error2( member->base->where,
                      "(emit): cannot assign value to literal 'polar' value..." );
            return FALSE;
        }

        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;

        string str = S_name(member->xid);
        // check
        if( str == "mag" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr, kindof_VEC2 ) );
        else if( str == "phase" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr, kindof_VEC2 ) );
        else
            goto check_func;

        // done
        return TRUE;
    }
    else if( member->t_base->xid == te_vec2 )
    {
        // remember the kind
        te_KindOf kind = kindof_VEC2;
        // mark to emit addr
        if( member->base->s_meta == ae_meta_var )
            member->base->emit_var = TRUE;
        else if( emit_addr )
        {
            EM_error2( member->base->where,
                      "(emit): cannot assign value to literal '%s' value...",
                      member->t_base->c_name() );
            return FALSE;
        }

        // emit the base
        if( !emit_engine_emit_exp( emit, member->base ) )
            return FALSE;

        string str = S_name(member->xid);
        // check for .xy .us
        if( str == "x" || str == "u" || str == "s" )
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else if( str == "y" || str == "v" || str == "t" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else
            goto check_func;

        // done
        return TRUE;
    }
    else if( member->t_base->xid == te_vec3 || member->t_base->xid == te_vec4 )
    {
        // remember the kind
        te_KindOf kind = member->t_base->xid == te_vec3 ? kindof_VEC3 : kindof_VEC4;
        // mark to emit addr
        if( member->base->s_meta == ae_meta_var )
            member->base->emit_var = TRUE;
        else if( emit_addr )
        {
            EM_error2( member->base->where,
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
            emit->append( new Chuck_Instr_Dot_Cmp_First( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else if( str == "y" || str == "g" || str == "goal" )
            emit->append( new Chuck_Instr_Dot_Cmp_Second( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else if( str == "z" || str == "b" || str == "slew" )
            emit->append( new Chuck_Instr_Dot_Cmp_Third( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else if( member->t_base->xid == te_vec4 && ( str == "w" || str == "a" ) )
            emit->append( new Chuck_Instr_Dot_Cmp_Fourth( member->base->s_meta == ae_meta_var, emit_addr, kind ) );
        else
            goto check_func;

        // done
        return TRUE;
    }

    // should not get here
    EM_error2( member->base->where,
              "(emit): internal error in lit_special()..." );

    return FALSE;

//-----------------------------------------------------------------------------
// goto: check for special member functions, 1.3.5.3
//-----------------------------------------------------------------------------
check_func:

    // the type of the base
    Chuck_Type * t_base = member->t_base;
    // is the base a class/namespace or a variable
    t_CKBOOL base_static = type_engine_is_base_static( emit->env, member->t_base );
    // t_CKBOOL base_static = isa( member->ckt_base, emit->env->ckt_class );
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
        EM_error2( member->base->where,
                  "(emit): internal error in lit_special(): base static" );
        // done
        return FALSE;
    }

    if( !isfunc( emit->env, member->self->type ) )
    {
        // should not get here
        EM_error2( member->base->where,
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
        // dup the base pointer ('this' pointer as argument -- special case primitive)
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
    // is the base a class/namespace or a variable | 1.5.0.0 (ge) modified to func call
    t_CKBOOL base_static = type_engine_is_base_static( emit->env, member->t_base );
    // t_CKBOOL base_static = isa( member->ckt_base, emit->env->ckt_class );
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
        case te_vec2:
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

    // if base is static?
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
                // dup the base pointer ('this' pointer as argument)
                emit->append( new Chuck_Instr_Reg_Dup_Last );
                // find the offset for virtual table
                offset = func->vt_index;
                // emit the function
                emit->append( instr = new Chuck_Instr_Dot_Member_Func( offset ) );
                instr->set_linepos( member->line );
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
                instr->set_linepos( member->line );
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
        // emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );

        // if is a func
        if( isfunc( emit->env, member->self->type ) )
        {
            // emit the type - spencer
            emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)t_base ) );
            // get the func | 1.4.1.0 (ge) added looking in parent
            value = type_engine_find_value( t_base, member->xid );
            // get the function reference
            func = value->func_ref;
            // make sure it's there
            assert( func != NULL );
            // emit the function
            emit->append( new Chuck_Instr_Dot_Static_Func( func ) );
        }
        else
        {
            // get the func | 1.4.1.0 (ge) added looking in parent
            value = type_engine_find_value( t_base, member->xid );
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
        EM_error2( exp_if->cond->where,
            "(emit): internal error: unhandled type '%s' in if condition",
            exp_if->cond->type->base_name.c_str() );
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
// desc: emit instructions for type pre-constructor, for Object instantiation
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_pre_constructor( Chuck_Emitter * emit, Chuck_Type * type, a_Ctor_Call ctor_info )
{
    // parent first pre constructor
    if( type->parent != NULL )
    {
        // first emit parent pre and base constructor
        emit_engine_pre_constructor( emit, type->parent, NULL );
    }

    // pre constructor
    if( type->has_pre_ctor )
    {
        // make sure
        assert( type->info->pre_ctor != NULL );
        // append instruction
        emit->append( new Chuck_Instr_Pre_Constructor( type->info->pre_ctor,
            emit->code->frame->curr_offset ) );
    }

    // constructors
    if( ctor_info && ctor_info->func ) // if named
    {
        // verify
        if( !ctor_info->func->code )
        {
            EM_error3( "(internal error) missing VM code for constructor '%s'",
                       ctor_info->func->signature(FALSE,FALSE).c_str() );
            return FALSE;
        }

        // dup the base pointer ('this' pointer as argument)
        emit->append( new Chuck_Instr_Reg_Dup_Last );
        // emit arguments
        emit_engine_emit_exp( emit, ctor_info->args, TRUE );
        // push code
        emit->append( new Chuck_Instr_Reg_Push_Code( ctor_info->func->code ) );
        // push local stack depth corresponding to local variables
        emit->append( new Chuck_Instr_Reg_Push_Imm( emit->code->frame->curr_offset ) );

        // code format (native or in-language)
        if( ctor_info->func->code->native_func )
        {
            // append instruction
            emit->append( new Chuck_Instr_Func_Call_Member( kindof_VOID, ctor_info->func,
                                                            CK_FUNC_CALL_THIS_IN_FRONT ) );
        }
        else
        {
            // push offset
            emit->append( new Chuck_Instr_Func_Call( CK_FUNC_CALL_THIS_IN_FRONT ) );
        }
    }
    else if( type->ctor_default ) // emit base constructor, if there is one
    {
        // verify
        if( !type->ctor_default->code )
        {
            EM_error3( "(internal error) missing VM code for base constructor '%s'",
                       type->ctor_default->signature(FALSE,FALSE).c_str() );
            return FALSE;
        }

        // emit
        emit->append( new Chuck_Instr_Pre_Constructor( type->ctor_default->code,
            emit->code->frame->curr_offset ) );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_pre_constructor_array()
// desc: emit instruction for pre-constructing an array of Objects
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_pre_constructor_array( Chuck_Emitter * emit, Chuck_Type * type, a_Ctor_Call ctor_info )
{
    // alloc should have put all objects to made in linear list, on stack
    Chuck_Instr_Pre_Ctor_Array_Top * top = NULL;
    Chuck_Instr_Pre_Ctor_Array_Bottom * bottom = NULL;
    // get start index
    t_CKUINT start_index = emit->next_index();
    // append first part of pre ctor
    emit->append( top = new Chuck_Instr_Pre_Ctor_Array_Top( type ) );
    // call pre constructor
    emit_engine_pre_constructor( emit, type, ctor_info );
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
// desc: emit instructions for instantiating object
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_instantiate_object( Chuck_Emitter * emit, Chuck_Type * type,
                                         a_Ctor_Call ctor_info, a_Array_Sub array, t_CKBOOL is_ref,
                                         t_CKBOOL is_array_ref )
{
    // if array
    if( type->array_depth )
    {
        // check if the array was an empty decl, e.g., int foo[]
        if( !is_array_ref )
        {
            // emit indices
            emit_engine_emit_exp( emit, array->exp_list );
            // emit array allocation
            Chuck_Instr * instr = NULL;
            emit->append( instr = new Chuck_Instr_Array_Alloc( emit->env,
                                                               type->array_depth, type->array_type,
                                                               emit->code->frame->curr_offset,
                                                               is_ref, type ) );
            instr->set_linepos( array->line );

            // handle constructor; possible to have an instanced array of references
            // Object @ foo[10], for example
            if( isobj( emit->env, type->array_type ) && !is_ref )
            {
                // call pre constructor for objects in array | TODO verify multi-dim array
                emit_engine_pre_constructor_array( emit, type->array_type, ctor_info );
            }
        }
    }
    else if( !is_ref ) // not array
    {
        // emit object instantiation code, include pre constructor
        emit->append( new Chuck_Instr_Instantiate_Object( type ) );

        // call pre constructor
        emit_engine_pre_constructor( emit, type, ctor_info );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_decl()
// desc: emit instruction for variable declaration expression
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
    t_CKBOOL is_array = FALSE;
    t_CKBOOL needs_global_ctor = FALSE;

    // chuck type (for global emission use)
    t_CKTYPE tglobals = NULL;
    // handling global decls | 1.4.0.1 (jack) added
    te_GlobalType globalType = te_globalTypeNone;
    // declared as global?
    if( decl->is_global )
    {
        // moved into global section | 1.5.1.3
        // NOTE inner classes definition (experimental) is not found this way (but works otherwise)
        tglobals = type_engine_find_type( emit->env, decl->type->xid );
        // check return | 1.5.1.3 (ge)
        if( !tglobals )
        {
            // fail if type unsupported
            EM_error2( decl->type->where, "unsupported type for global keyword: '%s'", decl->ck_type->base_name.c_str() );
            EM_error2( 0, "(supported types: int, float, string, Event, UGen, Object)" );
            return FALSE;
        }

        if( isa( tglobals, emit->env->ckt_int ) )
        {
            globalType = te_globalInt;
        }
        else if( isa( tglobals, emit->env->ckt_float ) )
        {
            globalType = te_globalFloat;
        }
        else if( isa( tglobals, emit->env->ckt_string ) )
        {
            globalType = te_globalString;
        }
        else if( isa( tglobals, emit->env->ckt_event ) )
        {
            // kind-of-event (te_Type for this would be te_user, which is not helpful)
            globalType = te_globalEvent;
            // need to call ctors
            needs_global_ctor = TRUE;
        }
        else if( isa( tglobals, emit->env->ckt_ugen ) )
        {
            // kind-of-ugen (te_Type might not be te_ugen, so we store globalUGen in our own field)
            globalType = te_globalUGen;
            // need to call ctors
            needs_global_ctor = TRUE;
        }
        else if( isa( tglobals, emit->env->ckt_object ) )
        {
            // kind-of-object (te_Type might not be te_object, so we store globalObject in our own field)
            globalType = te_globalObject;
            // need to call ctors
            needs_global_ctor = TRUE;
        }
        else
        {
            // fail if type unsupported
            EM_error2( decl->type->where, (std::string("unsupported type for global keyword: ") + tglobals->base_name).c_str() );
            EM_error2( 0, "(supported types: int, float, string, Event, UGen, Object)" );
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
        is_ref = var_decl->force_ref || decl->type->ref; // 1.5.0.8 (ge) added force_ref for foreach
        // not init
        is_init = FALSE;
        // is array
        is_array = FALSE;

        // if this is an object, do instantiation
        if( is_obj )
        {
            // if this is an array, ...
            if( var_decl->array )
            {
                // mark as true
                is_array = TRUE;
                // ... then check to see if empty []
                t_CKBOOL is_array_ref = ( var_decl->array->exp_list == NULL );
                // ...and only instantiate if NOT empty
                // REFACTOR-2017 TODO: do we want to
                // avoid doing this if the array is global?
                if( !is_array_ref )
                {
                    // only check dependency violations if we are at a context-top-level
                    // or class-top-level scope, i.e., not in a function definition
                    if( !emit->env->func )
                    {
                        // look at the array/actual type
                        Chuck_Type * actual_type = type->actual_type;
                        // dependency tracking: ensure object instantiation (e.g., Foo foo;) is not invoked
                        // before dependencies are met | 1.5.0.8 (ge) added
                        // NOTE if actual_type originates from another file, this should behave correctly and return NULL | 1.5.1.1 (ge) fixed
                        const Chuck_Value_Dependency * unfulfilled = actual_type->depends.locate( decl->type->where );
                        // at least one unfulfilled dependency
                        if( unfulfilled )
                        {
                            EM_error2( decl->type->where,
                                      "'%s' instantiation at this point skips initialization of a needed variable:",
                                      actual_type->c_name() );
                            EM_error2( unfulfilled->where,
                                      "...(note: this skipped variable initialization is needed by class '%s')",
                                      actual_type->c_name() );
                            EM_error2( unfulfilled->use_where,
                                      "...(note: this is where the variable is used within class '%s')",
                                      actual_type->c_name() );
                            EM_error2( 0,
                                      "...(hint: try instantiating '%s' after the variable initialization)", actual_type->c_name() );
                            return FALSE;
                        }
                    }

                    // set
                    is_init = TRUE;
                    // instantiate object, including array
                    if( !emit_engine_instantiate_object( emit, type, &var_decl->ctor, var_decl->array, is_ref, is_array_ref ) )
                        return FALSE;
                }
            }
            else if( !is_ref )
            {
                // only check dependency violations if we are at a context-top-level
                // or class-top-level scope, i.e., not in a function definition
                if( !emit->env->func )
                {
                    // dependency tracking: ensure object instantiation (e.g., Foo foo;) is not invoked
                    // before dependencies are met | 1.5.0.8 (ge) added
                    // NOTE if type originates from another file, this should behave correctly and return NULL | 1.5.1.1 (ge) fixed
                    const Chuck_Value_Dependency * unfulfilled = type->depends.locate( var_decl->where );
                    // at least one unfulfilled
                    if( unfulfilled )
                    {
                        EM_error2( var_decl->where,
                                  "'%s' instantiation at this point skips initialization of needed variable:",
                                  type->c_name() );
                        EM_error2( unfulfilled->where,
                                  "...(note: this skipped variable initialization is needed by class '%s')",
                                  type->c_name() );
                        EM_error2( unfulfilled->use_where,
                                  "...(note: this is where the variable is used within class '%s')",
                                  type->c_name() );
                        EM_error2( 0,
                                  "...(hint: try instantiating '%s' after the variable initialization)", type->c_name() );
                        return FALSE;
                    }
                }

                // REFACTOR-2017: don't emit instructions to instantiate
                // non-array global variables -- they are init/instantiated
                // during emit (see below in this function)
                if( !decl->is_global )
                {
                    // set
                    is_init = TRUE;
                    // instantiate object (not array)
                    if( !emit_engine_instantiate_object( emit, type, &var_decl->ctor, var_decl->array, is_ref, FALSE ) )
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
            else if( type->size == sz_VEC2 ) // ISSUE: 64-bit
                emit->append( new Chuck_Instr_Reg_Push_Imm4( 0.0, 0.0 ) );
            else
            {
                EM_error2( decl->where,
                    "(emit): unhandle decl size of '%i'...",
                    type->size );
                return FALSE;
            }
        }*/

        // done with object instantiation

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
            else if( type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
                emit->append( new Chuck_Instr_Alloc_Member_Word4( value->offset ) ); // 1.5.1.7 (ge) NOTE also handles vec2
            else if( type->size == sz_VEC3 ) // ge: added 1.3.5.3
                emit->append( new Chuck_Instr_Alloc_Member_Vec3( value->offset ) );
            else if( type->size == sz_VEC4 ) // ge: added 1.3.5.3
                emit->append( new Chuck_Instr_Alloc_Member_Vec4( value->offset) );
            else
            {
                EM_error2( decl->where,
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
                    is_ref, is_obj, decl->is_global );
                if( !local )
                {
                    EM_error2( decl->where,
                        "(emit): internal error: cannot allocate local '%s'...",
                        value->name.c_str() );
                    return FALSE;
                }

                // put in the value
                value->offset = local->offset;

                // REFACTOR-2017: global declaration
                if( decl->is_global )
                {
                    Chuck_Instr_Alloc_Word_Global * instr = new Chuck_Instr_Alloc_Word_Global();
                    instr->m_name = value->name;
                    instr->m_type = globalType;
                    instr->set_linepos( decl->line );
                    instr->m_is_array = is_array;

                    // extra fields for objects that need their ctors called
                    if( needs_global_ctor )
                    {
                        instr->m_chuck_type = type;
                        instr->m_stack_offset = local->offset;
                        instr->m_should_execute_ctors = TRUE;
                    }

                    // if it's an event, we need to initialize it and check if the exact type matches
                    if( globalType == te_globalEvent )
                    {
                        // init and construct it now!
                        if( !emit->env->vm()->globals_manager()->init_global_event( value->name, tglobals ) )
                        {
                            // if the type doesn't exactly match (different kinds of Event), then fail.
                            EM_error2( decl->where,
                                "global Event '%s' has different type '%s' than already existing global Event of the same name",
                                value->name.c_str(), tglobals->base_name.c_str() );
                            return FALSE;
                        }
                    }
                    // if it's a ugen, we need to initialize it and check if the exact type matches
                    else if( globalType == te_globalUGen )
                    {
                        // init and construct it now!
                        if( !emit->env->vm()->globals_manager()->init_global_ugen( value->name, tglobals ) )
                        {
                            // if the type doesn't exactly match (different kinds of UGen), then fail.
                            EM_error2( decl->where,
                                "global UGen '%s' has different type '%s' than already existing global UGen of the same name",
                                value->name.c_str(), tglobals->base_name.c_str() );
                            return FALSE;
                        }
                    }
                    // if it's a ugen, we need to initialize it and check if the exact type matches
                    else if( globalType == te_globalObject )
                    {
                        // init and construct it now!
                        if( !emit->env->vm()->globals_manager()->init_global_object( value->name, tglobals ) )
                        {
                            // if the type doesn't exactly match (different types), then fail.
                            EM_error2( decl->where,
                                      "global Object '%s' has different type '%s' than already existing global Object of the same name",
                                      value->name.c_str(), tglobals->base_name.c_str() );
                            return FALSE;
                        }
                    }

                    // add instruction
                    emit->append( instr );
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
                    instr->set_linepos( decl->line );
                    emit->append( instr );
                }
                else if( type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
                {
                    Chuck_Instr_Alloc_Word2 * instr = new Chuck_Instr_Alloc_Word2( local->offset );
                    instr->set_linepos( decl->line );
                    emit->append( instr );
                }
                else if( type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
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
                    EM_error2( decl->where,
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
                // global objects -- they should be instantiated during emit,
                // not during runtime and therefore don't need an assign instr
                if( !decl->is_global )
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
            else if( type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
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
// desc: emit VM instructions for a function definition
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
    //    EM_error2( func_def->where,
    //        "(emit): ambiguous function resolution for %s...",
    //        S_name(func_def->name) );
    //    return FALSE;
    //}

    // make sure code is good to go | 1.5.2.0
    // NOTE now func->code is allocated earlier in scan2_func_def(), this allows
    // the code reference to be available for emission potentially before its
    // contents are emitted
    if( func->code == NULL )
    {
        EM_error2( func_def->where,
            "(emit): internal error: function '%s' missing code shuttle...",
            func->signature(FALSE,FALSE).c_str() );
        return FALSE;
    }

    // make sure the code is empty
    if( func->code->num_instr ) // updated logic 1.5.2.0 was: if( func->code != NULL )
    {
        EM_error2( func_def->where,
            "(emit): internal error: function '%s' already emitted...",
            func->signature(FALSE,FALSE).c_str() );
        return FALSE;
    }

    // make sure we are not in a function already
    if( emit->env->func != NULL )
    {
        EM_error2( func_def->where,
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
    emit->code_stack.push_back( emit->code );
    // make a new one
    emit->code = new Chuck_Code;
    // name the code | 1.5.1.5 use signature()
    emit->code->name += func->signature(FALSE,FALSE);
    // name the code (older code)
    // emit->code->name = emit->env->class_def ? emit->env->class_def->base_name + "." : "";
    // emit->code->name += func->name + "(...)";

    // set whether need this
    emit->code->need_this = func->is_member;
    // if static inside class | 1.4.1.0 (ge) added
    emit->code->is_static = func->is_static;
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
            EM_error2( a->where,
                "(emit): internal error: cannot allocate local 'this'..." );
            return FALSE;
        }
    }
    // if static function inside class def | 1.4.1.0 (ge) added
    else if( func->is_static )
    {
        // get the size (for the TYPE argument)
        emit->code->stack_depth += sizeof(t_CKUINT);
        // create local
        if( !emit->alloc_local( sizeof(t_CKUINT), "@type", TRUE, FALSE, FALSE ) )
        {
            EM_error2( a->where,
                      "(emit): internal error: cannot allocate local '@type'..." );
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
            EM_error2( a->where,
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

    // added by spencer June 2014 (1.3.5.0) | ensure return
    if( func_def->ret_type && func_def->ret_type != emit->env->ckt_void )
    {
        // 1.5.0.0 (ge) | account for differences in return type size
        // push 0 for the width of the return type in question
        emit->append( new Chuck_Instr_Reg_Push_Zero( func_def->ret_type->size ) );
        // previously this pushed 0 as int, regardless of return type size
        // emit->append( new Chuck_Instr_Reg_Push_Imm(0) );

        // 1.5.0.0 (ge) | commented out this goto...
        // (okay as long as the next instruction is Chuck_Instr_Func_Return...)
        // Chuck_Instr_Goto * instr = new Chuck_Instr_Goto( 0 );
        // emit->append( instr );
        // emit->code->stack_return.push_back( instr );
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

    // vm code | 1.5.2.0 (ge) updated to pass in existing func->code
    if( !emit_to_code( emit->code, func->code, emit->dump ) )
        return FALSE;

    // unset the func
    emit->env->func = NULL;
    // delete the code
    CK_SAFE_DELETE( emit->code );
    // pop the code
    assert( emit->code_stack.size() );
    emit->code = emit->code_stack.back();
    emit->code_stack.pop_back();

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
        EM_error2( class_def->where,
            "(emit): class '%s' already emitted...",
            type->base_name.c_str() );
        return FALSE;
    }

    // make sure we are not in a class already
    //if( emit->env->class_def != NULL )
    //{
    //    EM_error2( class_def->where,
    //        "(emit): internal error: nested class definition..." );
    //    return FALSE;
    //}

    // set the class
    emit->env->class_stack.push_back( emit->env->class_def );
    emit->env->class_def = type;
    // push the current code
    emit->code_stack.push_back( emit->code );
    // make a new one
    emit->code = new Chuck_Code;
    // name the code
    emit->code->name = string("class ") + type->base_name;
    // whether code needs this
    emit->code->need_this = TRUE;
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;

    // get the size
    emit->code->stack_depth += sizeof(t_CKUINT);
    // add this
    // ge: added TRUE to the 'is_obj' argument, 2012 april (added 1.3.0.0)
    // TODO: verify this is right, and not over-ref counted / cleaned up?
    if( !emit->alloc_local( sizeof(t_CKUINT), "this", TRUE, TRUE, FALSE ) )
    {
        EM_error2( class_def->where,
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
            //EM_error2( body->section->class_def->where,
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

        // ----------------------
        // *** FYI: previously, as type->info->pre_ctor in both LHS and RHS...
        // type->info->pre_ctor = emit_to_code( emit->code, type->info->pre_ctor, emit->dump );
        // *** ... could result in extra ref count
        // type->info->pre_ctor->add_ref();
        // ----------------------
        // use CK_SAFE_REF_ASSIGN to add_ref RHS then releae LHS | 1.5.1.5
        // maintain refcount integrity whether type->info->pre_ctor==NULL or not
        // ----------------------
        CK_SAFE_REF_ASSIGN( type->info->pre_ctor,
                            emit_to_code( emit->code, type->info->pre_ctor, emit->dump ) );

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
        // release | 1.5.1.5 (ge) changed from DELETE to RELEASE
        CK_SAFE_RELEASE( type->info->pre_ctor );
    }

    // unset the class
    emit->env->class_def = emit->env->class_stack.back();
    emit->env->class_stack.pop_back();
    // delete the code
    CK_SAFE_DELETE( emit->code );
    // pop the code
    assert( emit->code_stack.size() );
    emit->code = emit->code_stack.back();
    emit->code_stack.pop_back();

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
        EM_error2( exp->where,
                  "(emit): internal error in evaluating function call..." );
        return FALSE;
    }

    // push the current code
    emit->code_stack.push_back( emit->code );
    // make a new one (spork~exp shred)
    emit->code = new Chuck_Code;
    // handle need this
    emit->code->need_this = exp->ck_func->is_member;
    // handle is static (inside class def) | 1.4.1.0 (ge) added
    emit->code->is_static = exp->ck_func->is_static;
    // name it: e.g. spork~foo [line 5]
    std::ostringstream name;
    name << "spork~"
         << exp->ck_func->name.substr( 0, exp->ck_func->name.find( "@" ))
         << " [line " << exp->line << "]";
    emit->code->name = name.str();
    // keep track of full path (added 1.3.0.0)
    emit->code->filename = emit->context->full_path;
    // push op
    op = new Chuck_Instr_Mem_Push_Imm( 0 );
    // emit the stack depth - we don't know this yet
    emit->append( op );

    // call the func on sporkee shred
    if( !emit_engine_emit_exp_func_call( emit,
                                         exp->ck_func,
                                         exp->ret_type,
                                         exp->line,
                                         exp->where,
                                         TRUE ) ) return FALSE;

    // 1.5.0.0 (ge) | added to pop/release the returned value, which could be an object
    if( iskindofint(emit->env, exp->ret_type) )
    {
        // is an object?
        if( isobj( emit->env, exp->ret_type) )
        { emit->append( new Chuck_Instr_Release_Object3_Pop_Int ); }
        else // not an object
        { emit->append( new Chuck_Instr_Reg_Pop_Int ); }
    }

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
    assert( emit->code_stack.size() > 0 );
    emit->code = emit->code_stack.back();
    // pop
    emit->code_stack.pop_back();

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
// name: emit_engine_emit_symbol()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_symbol( Chuck_Emitter * emit, S_Symbol symbol,
                                  Chuck_Value * v, t_CKBOOL emit_var,
                                  t_CKUINT line, t_CKUINT where )
{
    // look up the value
    // Chuck_Value * v = emit->env->curr->lookup_value( symbol, TRUE );
    // it should be there
    if( !v )
    {
        // internal error
        EM_error2( where,
            "(emit): internal error: undefined symbol '%s'...",
            S_name(symbol) );
        return FALSE;
    }

    // if global, find what type
    // (due to user classes, this info is only available during emit)
    te_GlobalType global_type = te_globalTypeNone;
    if( v->is_global )
    {
        if( isa( v->type, emit->env->ckt_int ) )
        {
            global_type = te_globalInt;
        }
        else if( isa( v->type, emit->env->ckt_float ) )
        {
            global_type = te_globalFloat;
        }
        else if( isa( v->type, emit->env->ckt_string ) )
        {
            global_type = te_globalString;
        }
        else if( isa( v->type, emit->env->ckt_event ) )
        {
            global_type = te_globalEvent;
        }
        else if( isa( v->type, emit->env->ckt_ugen ) )
        {
            global_type = te_globalUGen;
        }
        else if( isa( v->type, emit->env->ckt_array ) )
        {
            global_type = te_globalArraySymbol;
        }
        else if( isa( v->type, emit->env->ckt_object ) )
        {
            global_type = te_globalObject;
        }
        else
        {
            // internal error
            EM_error2( where,
                "(emit): internal error: unknown global type '%s'...",
                v->type->base_name.c_str() );
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
        // ACTUALLY: this might be ok, since this would result in a S_symbol which allocates
        //           new memory for the xid that would be "this" | 1.5.1.3
        a_Exp base = new_exp_from_id( (char *)"this", (uint32_t)line, (uint32_t)where );
        a_Exp dot = new_exp_from_member_dot( base, (char *)v->name.c_str(), (uint32_t)line, (uint32_t)where, (uint32_t)where ); // TODO: 1.5.0.5 check this last memberPos

        // add information
        base->type = v->owner_class;
        CK_SAFE_ADD_REF( base->type ); // 1.5.1.3
        dot->type = v->type;
        CK_SAFE_ADD_REF( dot->type ); // 1.5.1.3
        dot->dot_member.t_base = v->owner_class;
        CK_SAFE_ADD_REF( dot->dot_member.t_base ); // 1.5.1.3
        dot->emit_var = emit_var;

        // emit it
        if( !emit_engine_emit_exp_dot_member( emit, &dot->dot_member ) )
        {
            // internal error
            EM_error2( where,
                "(emit): internal error: symbol transformation failed..." );
            return FALSE;
        }

        // delete locally-allocated case and dot | 1.5.1.3
        // FYI: dot will implicitly delete base, so ONLY delete_exp( dot )
        // TODO: is this safe? hopefully nothing within exp_dot_member() keeps a reference to either base or dot
        delete_exp( dot );
        // zero out for good measure
        base = NULL; dot = NULL;

        // done
        return TRUE;
    }

    // var or value
    if( emit_var )
    {
        // emit as addr
        if( v->is_global )
        {
            emit->append( new Chuck_Instr_Reg_Push_Global_Addr( v->name, global_type ) );
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
        {
            // 1.4.1.0 (ge) added for new static calling convention of passing
            // the type as the first parameter
            if( v->func_ref->is_static )
            {
                // push the type pointer
                emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)v->owner_class ) );
            }
            // push function pointer
            emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)v->func_ref ) );
        }
        else if( v->is_global )
        {
            Chuck_Instr_Reg_Push_Global * instr =
                new Chuck_Instr_Reg_Push_Global( v->name, global_type );
            instr->set_linepos( line );
            emit->append( instr );
        }
        // check size
        // (added 1.3.1.0: iskindofint -- since in some 64-bit systems, sz_INT == sz_FLOAT)
        else if( v->type->size == sz_INT && iskindofint(emit->env, v->type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem2( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
            emit->append( new Chuck_Instr_Reg_Push_Mem4( v->offset, v->is_context_global ) ); // 1.5.1.7 (ge) NOTE also handles vec2
        else if( v->type->size == sz_VEC3 ) // ge: added 1.3.5.3
            emit->append( new Chuck_Instr_Reg_Push_Mem_Vec3( v->offset, v->is_context_global ) );
        else if( v->type->size == sz_VEC4 ) // ge: added 1.3.5.3
            emit->append( new Chuck_Instr_Reg_Push_Mem_Vec4( v->offset, v->is_context_global ) );
        else
        {
            // internal error
            EM_error2( where,
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
            // REFACTOR-2017: Don't do if local is global
            // Note: I don't think addref_on_scope() is used anywhere,
            // but I am doing this to mirror pop_scope() below, which IS used
            if( !local->is_global )
            {
                // emit instruction to add reference
                this->append( new Chuck_Instr_AddRef_Object2( local->offset ) );
            }
        }
    }
}




//-----------------------------------------------------------------------------
// name: traverse_scope_on_jump() | 1.5.0.0 (ge) added
// desc: traverse the scope and emit instructions on a jump statement:
//       'return', 'break', 'continue'
//       this is done to ensure proper cleanup of objects on scope frames
//-----------------------------------------------------------------------------
void Chuck_Emitter::traverse_scope_on_jump( )
{
    // this gotta be the case
    assert( code != NULL );

    // clear locals
    locals.clear();
    // get locals (localOnly=false to get all locals at this point in 'code')
    code->frame->get_scope( locals, false );

    // loop over
    for( int i = 0; i < locals.size(); i++ )
    {
        // get local
        Chuck_Local * local = locals[i];
        // skip scope boundary (denoted by NULL)
        if( !local ) continue;

        // check to see if it's an object (but not global)
        // (REFACTOR-2017: don't release global objects)
        if( local->is_obj && !local->is_global )
        {
            // emit instruction to release the object
            this->append( new Chuck_Instr_Release_Object2( local->offset ) );
        }
    }

    // clear it
    locals.clear();
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
            // (REFACTOR-2017: don't release global objects)
            if( !local->is_global )
            {
                // emit instruction to release the object
                this->append( new Chuck_Instr_Release_Object2( local->offset ) );
            }
        }

        // reclaim local; null out to be safe
        CK_SAFE_DELETE( local ); locals[i] = NULL;
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
    if( !value || !equals( value->type, env->ckt_dur ) ) return FALSE;
    // copy
    *out = *( (t_CKDUR *)value->addr );

    return TRUE;
}
