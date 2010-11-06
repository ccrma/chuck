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
// file: chuck_emit.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//       Autumn 2003 updated
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
using namespace std;

#include "chuck_emit.h"
#include "chuck_frame.h"
#include "chuck_instr.h"
#include "chuck_bbq.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"




//-----------------------------------------------------------------------------
// name: class Chuck_Code
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Code
{
public:
    vector<Chuck_Instr *> code;
    string name;
    F_Frame frame;
    unsigned int stack_depth;

    Chuck_Code( c_str scope_name )
    {
        name = scope_name;
        frame = F_new_frame( Temp_named_label( scope_name ) );
        stack_depth = 0;
    }

    ~Chuck_Code()
    {
        // TODO: free the frame
    }
};




//-----------------------------------------------------------------------------
// name: struct Dur_Node
// desc: ...
//-----------------------------------------------------------------------------
struct Dur_Node
{
    t_CKDUR dur;

    Dur_Node( t_CKDUR d )
    { dur = d; }
};




//-----------------------------------------------------------------------------
// name: struct Func_Link
// desc: ...
//-----------------------------------------------------------------------------
struct Func_Link
{
    S_Symbol name;
    Chuck_Instr_Reg_Push_Imm * op;
};

// global func table
S_table g_func2code = NULL;
vector<Func_Link *> g_func_links;




//-----------------------------------------------------------------------------
// name: struct Chuck_Emmission
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Emmission
{
    Chuck_Code * global;
    Chuck_Code * local;
    vector<Chuck_Code *> functions;
    S_table var2offset;
    S_table dur2num_samps;
    S_table globals;
    t_Env env;
    unsigned int is_global;
    unsigned int count;
    unsigned int chuck_count;
    vector<Chuck_Instr_Branch_Op *> returns;
    vector<Chuck_Instr_Unary_Op *> addr_map;

    // namespace
    t_Env nspc;
    a_Func_Def nspc_func;
    vector<Chuck_Instr_Unary_Op *> ops;

    Chuck_Emmission( t_Env e )
    {
        global = NULL;
        local = NULL;
        var2offset = S_empty();
        dur2num_samps = S_empty();
        globals = S_empty();
        is_global = TRUE;
        count = 0;
        chuck_count = 0;
        returns.clear();
        env = e;
        nspc = e;
        nspc_func = NULL;
    }
    
    void push_op( Chuck_Instr_Unary_Op * a )
    {
        ops.push_back( a );
    }
    
    Chuck_Instr_Unary_Op * get_op()
    {
        if( ops.size() ) return ops[ops.size()-1];
        else return NULL; 
    }
    
    void pop_op( )
    {
        if( ops.size() ) ops.pop_back();
    }
    
    void append_ops()
    {
        while( get_op() )
        {
            append( get_op() );
            pop_op();
        }
    }
    
    void pop_the_ops()
    {
        ops.clear();
        // while( get_op() ) pop_op();
    }

    uint append( Chuck_Instr * instr )
    {
        if( is_global )
        {
            global->code.push_back( instr );
            return (uint)global->code.size() - 1;
        }
        else
        {
            local->code.push_back( instr );
            return (uint)local->code.size() - 1;
        }
    }
    
    Chuck_Instr * remove_last( )
    {
        if( is_global )
        {
            if( global->code.size() == 0 ) return NULL;
            Chuck_Instr * instr = *global->code.end();
            global->code.pop_back();
            return instr;
        }
        else
        {
            if( local->code.size() == 0 ) return NULL;
            Chuck_Instr * instr = *local->code.end();
            local->code.pop_back();
            return instr;
        }
    }

    uint next_index()
    {
        if( is_global )
            return (uint)global->code.size();
        else
            return (uint)local->code.size();
    }

    void alloc_dur( S_Symbol var, t_CKDUR samp )
    {
        S_enter( dur2num_samps, var, new Dur_Node( samp ) );
    }

    uint find_dur( S_Symbol var, t_CKDUR * dur )
    {
        Dur_Node * n = (Dur_Node *)S_look( dur2num_samps, var );
        if( !n )
            return FALSE;

        *dur = n->dur;
        return TRUE;
    }

    unsigned int alloc_local( S_Symbol var, t_Type type )
    {
        // allocate offset in the frame
        F_Access a;
        if( is_global )
            a = F_alloc_local( global->frame, type->size, is_global );
        else
            a = F_alloc_local( local->frame, type->size, is_global );

        unsigned int offset = F_offset( a );

        // push in the table
        S_enter( var2offset, var, a );

        return offset;
    }

    int find_offset( S_Symbol var, t_CKBOOL * isglobal = NULL, t_CKUINT * size = NULL )
    {
        F_Access a = (F_Access)S_look( var2offset, var );
        if( !a )
            return -1;
        if( isglobal )
            *isglobal = a->global;
        if( size )
            *size = a->size;

        return a->offset;
    }
    
    unsigned int stack_depth()
    {
        if( is_global )
            return F_stack_depth( global->frame );
        else
            return F_stack_depth( local->frame );
    }

    void push( c_str scope_name = NULL )
    {
        S_beginScope( var2offset );
        if( this->is_global )
            F_begin_scope( global->frame );
        else
            F_begin_scope( local->frame );
        count++;
    }

    void pop( )
    {
        if( !count )
        {
            EM_error2( 0, "(emit): internal error - too many pop()!" );
            return;
        }

        S_endScope( var2offset );
        if( is_global )
            F_end_scope( global->frame );
        else
            F_end_scope( local->frame );
        count--;
    }
};




//-----------------------------------------------------------------------------
// name: emit_to_code()
// desc: emit to vm code
//-----------------------------------------------------------------------------
Chuck_VM_Code * emit_to_code( Chuck_Emmission * emit, t_CKBOOL dump )
{
    Chuck_VM_Code * code = new Chuck_VM_Code;
    code->num_instr = emit->global->code.size();
    code->instr = new Chuck_Instr *[code->num_instr];

    // copy
    for( unsigned int i = 0; i < code->num_instr; i++ )
        code->instr[i] = emit->global->code[i];

    // functions
    for( unsigned int j = 0; j < emit->functions.size(); j++ )
    {
        Chuck_VM_Code * code2 = new Chuck_VM_Code;
        Chuck_Code * c = emit->functions[j];
        code2->num_instr = c->code.size();
        code2->instr = new Chuck_Instr *[code2->num_instr];

        // copy
        for( unsigned int i = 0; i < code2->num_instr; i++ )
            code2->instr[i] = c->code[i];

        // set the parent
        code2->stack_depth = c->stack_depth;
        
        if( dump )
        {
            fprintf( stderr, "[chuck]: dumping function %s( ... )\n\n", c->name.c_str() );

            for( unsigned int i = 0; i < code2->num_instr; i++ )
                fprintf( stdout, "'%i' %s( %s )\n", i, 
                    code2->instr[i]->name(), code2->instr[i]->params() );

            fprintf( stdout, "...\n\n" );
        }

        // put in table
        S_enter( g_func2code, insert_symbol((char *)c->name.c_str()), code2 );
    }

    return code;
}




//------------------------------------------------------------------------------
// name: emit_engine_addr_map()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_addr_map( Chuck_Emmission * emit, Chuck_VM_Shred * shred )
{
    // memory stack
    uint sp = (uint)shred->mem->sp;
    
    // offset the initial region
    sp += sizeof(t_CKINT);
    
    // map the offset to pointers
    for( unsigned int i = 0; i < emit->addr_map.size(); i++ )
        emit->addr_map[i]->set( sp + emit->addr_map[i]->get() );

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: emit_engine_resolve()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_resolve()
{
    Func_Link * f = NULL;
    Chuck_VM_Code * code = NULL;

    for( unsigned int i = 0; i < g_func_links.size(); i++ )
    {
        f = g_func_links[i];
        code = (Chuck_VM_Code *)S_look( g_func2code, f->name );
        if( code == NULL )
        {
            EM_error2( 0, "error: cannot resolve function '%s'",
                       S_name(f->name) );
            return FALSE;
        }

        f->op->set( (uint)code );
    }

    g_func_links.clear();

    return TRUE;
}




//-----------------------------------------------------------------------------
// prototypes
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_prog( Chuck_Emmission * emit, a_Program prog );
t_CKBOOL emit_engine_emit_stmt_list( Chuck_Emmission * emit, a_Stmt_List list );
t_CKBOOL emit_engine_emit_stmt( Chuck_Emmission * emit, a_Stmt stmt, t_CKBOOL pop = TRUE );
t_CKBOOL emit_engine_emit_code_segment( Chuck_Emmission * emit, a_Stmt_Code stmt );
t_CKBOOL emit_engine_emit_switch( Chuck_Emmission * emit, a_Stmt_Switch stmt );
t_CKBOOL emit_engine_emit_while( Chuck_Emmission * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_do_while( Chuck_Emmission * emit, a_Stmt_While stmt );
t_CKBOOL emit_engine_emit_until( Chuck_Emmission * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_do_until( Chuck_Emmission * emit, a_Stmt_Until stmt );
t_CKBOOL emit_engine_emit_for( Chuck_Emmission * emit, a_Stmt_For stmt );
t_CKBOOL emit_engine_emit_if( Chuck_Emmission * emit, a_Stmt_If stmt );
t_CKBOOL emit_engine_emit_return( Chuck_Emmission * emit, a_Stmt_Return stmt );
t_CKBOOL emit_engine_emit_exp( Chuck_Emmission * emit, a_Exp exp );
t_CKBOOL emit_engine_emit_exp_binary( Chuck_Emmission * emit, a_Exp_Binary exp );
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emmission * emit, a_Exp_Unary exp );
t_CKBOOL emit_engine_emit_exp_cast( Chuck_Emmission * emit, a_Exp_Cast exp );
t_CKBOOL emit_engine_emit_exp_postfix( Chuck_Emmission * emit, a_Exp_Postfix exp );
t_CKBOOL emit_engine_emit_exp_dur( Chuck_Emmission * emit, a_Exp_Dur exp );
t_CKBOOL emit_engine_emit_exp_primary( Chuck_Emmission * emit, a_Exp_Primary exp );
t_CKBOOL emit_engine_emit_exp_array( Chuck_Emmission * emit, a_Exp_Array exp );
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emmission * emit, a_Exp_Func_Call exp, t_CKBOOL spork = FALSE );
t_CKBOOL emit_engine_emit_exp_dot_member( Chuck_Emmission * emit, a_Exp_Dot_Member exp, t_Type t );
t_CKBOOL emit_engine_emit_exp_if( Chuck_Emmission * emit, a_Exp_If exp );
t_CKBOOL emit_engine_emit_exp_decl( Chuck_Emmission * emit, a_Exp_Decl exp );
t_CKBOOL emit_engine_emit_exp_namespace( Chuck_Emmission * emit, a_Exp_Namespace nspc );
t_CKBOOL emit_engine_emit_op( Chuck_Emmission * emit, ae_Operator op, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_chuck( Chuck_Emmission * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_unchuck( Chuck_Emmission * emit, a_Exp lhs, a_Exp rhs );
t_CKBOOL emit_engine_emit_func_def( Chuck_Emmission * emit, a_Func_Def func_def );
t_CKBOOL emit_engine_emit_spork( Chuck_Emmission * emit, a_Exp_Func_Call exp );

t_CKBOOL emit_engine_emit_exp_mem( Chuck_Emmission * emit, a_Exp exp );
t_CKBOOL emit_engine_emit_exp_primary_mem( Chuck_Emmission * emit, a_Exp_Primary exp );
t_CKBOOL emit_engine_emit_exp_dot_member_mem( Chuck_Emmission * emit, a_Exp_Dot_Member exp );
t_CKBOOL emit_engine_emit_exp_array_mem( Chuck_Emmission * emit, a_Exp_Array exp );
t_CKBOOL emit_engine_emit_exp_func_call_mem( Chuck_Emmission * emit, a_Exp_Func_Call exp );

t_CKBOOL emit_engine_emit_symbol( Chuck_Emmission * emit, S_Symbol symbol, 
                                  t_CKBOOL offset, int linepos );




//-----------------------------------------------------------------------------
// name: emit_engine_init()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Emmission * emit_engine_init( t_Env env )
{
    // allocate the emit
    Chuck_Emmission * emit = new Chuck_Emmission( env );
    if( !emit )
    {
        EM_error2( 0, "(emit): out of memory!" );
        return NULL;
    }

    if( g_func2code == NULL )
        g_func2code = S_empty();

    // set the env
    emit->global = new Chuck_Code( "__system_scope__" );
    emit->push( "__system_scope__" );

    // push special
    //emit->alloc_local( insert_symbol("now"), lookup_value(env, insert_symbol("now") ) );
    //emit->alloc_local( insert_symbol("samp"), lookup_value(env, insert_symbol("samp") ) );
    //emit->alloc_local( insert_symbol("ms"), lookup_value(env, insert_symbol("ms") ) );
    //emit->alloc_local( insert_symbol("second"), lookup_value(env, insert_symbol("second") ) );
    //emit->alloc_local( insert_symbol("minute"), lookup_value(env, insert_symbol("minute") ) );
    //emit->alloc_local( insert_symbol("hour"), lookup_value(env, insert_symbol("hour") ) );
    //emit->alloc_local( insert_symbol("day"), lookup_value(env, insert_symbol("day") ) );
    //emit->alloc_local( insert_symbol("week"), lookup_value(env, insert_symbol("week") ) );
    emit->alloc_local( insert_symbol("stdout"), lookup_value(env, insert_symbol("stdout") ) );
    emit->alloc_local( insert_symbol("stderr"), lookup_value(env, insert_symbol("stderr") ) );
    emit->alloc_local( insert_symbol("chout"), lookup_value(env, insert_symbol("chout") ) );
    emit->alloc_local( insert_symbol("cherr"), lookup_value(env, insert_symbol("cherr") ) );
    emit->alloc_local( insert_symbol("midiin"), lookup_value(env, insert_symbol("midiin") ) );
    emit->alloc_local( insert_symbol("midiout"), lookup_value(env, insert_symbol("midiout") ) );
    //emit->alloc_local( insert_symbol("adc"), lookup_value(env, insert_symbol("adc") ) );
    //emit->alloc_local( insert_symbol("dac"), lookup_value(env, insert_symbol("dac") ) );

    t_CKDUR samp = 1.0;
    t_CKDUR second = Digitalio::sampling_rate() * samp;
    t_CKDUR ms = second / 1000.0;
    t_CKDUR minute = second * 60.0;
    t_CKDUR hour = minute * 60.0;
    t_CKDUR day = hour * 24.0;
    t_CKDUR week = day * 7.0;

    // push dur
    emit->alloc_dur( insert_symbol("samp"), samp );
    emit->alloc_dur( insert_symbol("ms"), ms );
    emit->alloc_dur( insert_symbol("second"), second );
    emit->alloc_dur( insert_symbol("minute"), minute );
    emit->alloc_dur( insert_symbol("hour"), hour );
    emit->alloc_dur( insert_symbol("day"), day );
    emit->alloc_dur( insert_symbol("week"), week );

    emit->push( "__base_scope__" );

    return emit;
}




//-----------------------------------------------------------------------------
// name: emit_engine_shutdown()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_shutdown( Chuck_Emmission *& emit )
{
    // delete and set pointer to NULL
    delete emit;
    emit = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_prog()
// desc: emit a program
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_prog( Chuck_Emmission * emit, a_Program prog )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Mem_Push_Imm * op = new Chuck_Instr_Mem_Push_Imm( 0 );

    if( !prog )
        return FALSE;
    
    // emit the stack depth - we don't know this yet
    emit->append( op );

    while( prog && ret )
    {
        switch( prog->section->s_type )
        {
        case ae_section_stmt:
            ret = emit_engine_emit_stmt_list( emit, prog->section->stmt_list );
            break;
        
        case ae_section_func:
            ret = emit_engine_emit_func_def( emit, prog->section->func_def );
            break;

        case ae_section_class:
            ret = FALSE;
            break;
        }

        prog = prog->next;
    }

    // done
    emit->append( new Chuck_Instr_EOC );
    // set the stack depth now that we know
    op->set( emit->stack_depth() );

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_stmt_list()
// desc: emit stmt list
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt_list( Chuck_Emmission * emit, a_Stmt_List list )
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
// desc: emit a stmt
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_stmt( Chuck_Emmission * emit, a_Stmt stmt, t_CKBOOL pop )
{
    t_CKBOOL ret = TRUE;

    if( !stmt )
        return TRUE;

    switch( stmt->s_type )
    {
        case ae_stmt_exp:
            // env->print( "stmt_exp" );
            ret = emit_engine_emit_exp( emit, stmt->stmt_exp );
            if( pop && stmt->stmt_exp->type->size > 0 )
            {
                if( stmt->stmt_exp->type->size == 4 || stmt->stmt_exp->s_type == ae_exp_decl )
                    emit->append( new Chuck_Instr_Reg_Pop_Word );
                else if( stmt->stmt_exp->type->size == 8 )
                    emit->append( new Chuck_Instr_Reg_Pop_Word2 );
                else
                {
                    EM_error2( stmt->stmt_exp->linepos,
                               "(emit): internal error: %i byte stack item not unhandled",
                               stmt->stmt_exp->type->size );
                    return FALSE;
                }
            }
            break;

        case ae_stmt_if:
            // env->print( "stmt_if" );
            emit->push();
            ret = emit_engine_emit_if( emit, &stmt->stmt_if );
            emit->pop();
            break;

        case ae_stmt_for:
            // env->print( "stmt_for" );
            emit->push();
            ret = emit_engine_emit_for( emit, &stmt->stmt_for );
            emit->pop();
            break;

        case ae_stmt_while:
            // env->print( "stmt_while" );
            emit->push();
            if( stmt->stmt_while.is_do )
                ret = emit_engine_emit_do_while( emit, &stmt->stmt_while );
            else
                ret = emit_engine_emit_while( emit, &stmt->stmt_while );
            emit->pop();
            break;
        
        case ae_stmt_until:
            // env->print( "stmt_until" );
            emit->push();
            if( stmt->stmt_until.is_do )
                ret = emit_engine_emit_do_until( emit, &stmt->stmt_until );
            else
                ret = emit_engine_emit_until( emit, &stmt->stmt_until );
            emit->pop();
            break;

        case ae_stmt_switch:
            // env->print( "stmt_switch" );
            // not implemented
            ret = FALSE;
            break;

        case ae_stmt_break:
            // env->print( "break? not implemented" );
            // ok here
            break;

        case ae_stmt_code:
            // env->print( "code segment" );
            emit->push();
            ret = emit_engine_emit_code_segment( emit, &stmt->stmt_code );
            emit->pop();
            break;
            
        case ae_stmt_continue:
            // env->print( "continue" );
            // ret = emit_engine_emit_continue( emit, &stmt->stmt_continue );
            break;
            
        case ae_stmt_return:
            // env->print( "return" );
            ret = emit_engine_emit_return( emit, &stmt->stmt_return );
            break;
            
        case ae_stmt_case:
            // env->print( "case" );
            // ret = emit_engine_emit_case( emit, &stmt->stmt_case );
            break;
       
        case ae_stmt_gotolabel:
            // env->print( "gotolabel" );
            // ret = emit_engine_emit_gotolabel( emit, &stmt->stmt_case );
            break;
        
        default:
            EM_error2( stmt->linepos, 
                       "(emit): internal error: unhandled statement '%i' during emission",
                       stmt->s_type );
            break;
    }

    return ret;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp()
// desc: emit an exp
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp( Chuck_Emmission * emit, a_Exp exp )
{
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
            if( !emit_engine_emit_exp_dot_member( emit, &exp->dot_member, exp->type ) )
                return FALSE;
            break;

        case ae_exp_if:
            if( !emit_engine_emit_exp_if( emit, &exp->exp_if ) )
                return FALSE;
            break;

        case ae_exp_decl:
            if( !emit_engine_emit_exp_decl( emit, &exp->decl ) )
                return FALSE;
            break;

        case ae_exp_namespace:
            if( !emit_engine_emit_exp_namespace( emit, &exp->name_space ) )
                return FALSE;
            break;

        default:
            EM_error2( exp->linepos, 
                       "(emit): internal error: unhandled expression '%i' in emission!",
                       exp->s_type );
            return FALSE;
        }

        exp = exp->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_binary
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_binary( Chuck_Emmission * emit, a_Exp_Binary exp )
{
    t_CKBOOL left = FALSE;
    t_CKBOOL right = FALSE;

    // hack for determining if a dot member comes first in chuck chain
    if( exp->op == ae_op_chuck && ( exp->rhs->s_type == ae_exp_dot_member ) )
    {
        // set the flag
        exp->rhs->dot_member.flag = TRUE;
    }

    // ^ op should be reversed for midi
    if( exp->op == ae_op_s_xor && ( exp->lhs->type->type == te_midiin || exp->lhs->type->type == te_midiout ) )
    {
        right = emit_engine_emit_exp( emit, exp->rhs );
        left = emit_engine_emit_exp( emit, exp->lhs );
    }
    else
    {
        left = emit_engine_emit_exp( emit, exp->lhs );
        right = emit_engine_emit_exp( emit, exp->rhs );
    }


    // check
    if( !left || !right )
        return FALSE;

    // emit the op
    if( !emit_engine_emit_op( emit, exp->op, exp->lhs, exp->rhs ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_unary
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_unary( Chuck_Emmission * emit, a_Exp_Unary exp )
{
    if( exp->op != ae_op_spork && !emit_engine_emit_exp( emit, exp->exp ) )
        return FALSE;

    switch( exp->op )
    {
    case ae_op_plusplus:
        if( !emit_engine_emit_exp_mem( emit, exp->exp ) )
            return FALSE;

        if( exp->exp->type->type == te_int )
            emit->append( new Chuck_Instr_Inc_int );
        else if( exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Inc_uint );   
        else
        {
            EM_error2( exp->linepos, 
                "(emit): internal error: unhandled type '%s' for pre '++'' operator",
                exp->exp->type->name );
            return FALSE;
        }
        break;

    case ae_op_minusminus:
        if( !emit_engine_emit_exp_mem( emit, exp->exp ) )
            return FALSE;

        if( exp->exp->type->type == te_int )
            emit->append( new Chuck_Instr_Dec_int );
        else if( exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Dec_uint );
        else
        {
            EM_error2( exp->linepos, 
                "(emit): internal error: unhandled type '%s' for pre '--' operator",
                exp->exp->type->name );
            return FALSE;
        }
        break;

    case ae_op_tilda:
        if( exp->exp->type->type == te_int )
            emit->append( new Chuck_Instr_Complement_int );
        else if( exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Complement_uint );
        else
        {
            EM_error2( exp->linepos, 
                "(emit): internal error: unhandled type '%s' for '~' operator",
                exp->exp->type->name );
            return FALSE;
        }
        break;

    case ae_op_exclamation:
        if( exp->exp->type->type == te_int || exp->exp->type->type == te_uint)
            emit->append( new Chuck_Instr_Not_int );
        else
        {
            EM_error2( exp->linepos, 
                "(emit): internal error: unhandled type '%s' for '!' operator",
                exp->exp->type->name );
            return FALSE;
        }
        break;
        
    case ae_op_minus:
        if( exp->exp->type->type == te_int || exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Negate_int );
        else if( exp->exp->type->type == te_float )
            emit->append( new Chuck_Instr_Negate_double );
        else
        {
            EM_error2( exp->linepos, 
                "(emit): internal error: unhandled type '%s' for unary '-' operator",
                exp->exp->type->name );
            return FALSE;
        }
        break;

    case ae_op_spork:
        if( exp->exp->s_type == ae_exp_func_call )
        {
            if( !emit_engine_emit_spork( emit, &exp->exp->func_call ) )
                return FALSE;
        }
        else
        {
            EM_error2( exp->linepos,
                       "(emit): internal error: sporking non-function call..." );
            return FALSE;
        }
        break;
    default:
        EM_error2( exp->linepos, 
            "(emit): internal error: unhandled unary op '%i",
            exp->op );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_spork
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_spork( Chuck_Emmission * emit, a_Exp_Func_Call exp )
{
    // emit the function call
    if( !emit_engine_emit_exp_func_call( emit, exp, true ) )
        return FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_spork
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_spork( Chuck_Emmission * emit, a_Exp exp )
{
    Chuck_Emmission * emit2 = emit_engine_init( emit->env );

    Chuck_Instr_Mem_Push_Imm * op = new Chuck_Instr_Mem_Push_Imm( 0 );

    // emit the stack depth - we don't know this yet
    emit2->append( op );

    // emit the function call
    if( !emit_engine_emit_exp( emit2, exp ) )
        return FALSE;

    // done
    emit2->append( new Chuck_Instr_EOC );
    // set the stack depth now that we know
    op->set( emit2->stack_depth() );

    // emit it
    Chuck_VM_Code * code = emit_to_code( emit2 );
    code->name = string("spork ~ exp");
    emit->append( new Chuck_Instr_Reg_Push_Imm( (uint)code ) );
    emit->append( new Chuck_Instr_Spork );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_get_cast_instr()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_get_cast_instr( Chuck_Emmission * emit, t_Type to, t_Type from )
{
    if( to->type == from->type ) return TRUE;

    if( to->type == te_int && from->type == te_float )
        emit->append( new Chuck_Instr_Cast_double2int );
    else if( to->type == te_float && from->type == te_int )
        emit->append( new Chuck_Instr_Cast_int2double );
    else
    {
        EM_error2( 0, "(emit): internal error: no instruction for casting type '%s' to '%s'",
                   from->name, to->name );
        return FALSE;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_cast
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_cast( Chuck_Emmission * emit, a_Exp_Cast exp )
{
    if( !emit_engine_emit_exp( emit, exp->exp ) )
        return FALSE;
    if( !emit_get_cast_instr( emit, lookup_type( emit->env, exp->type, TRUE ),
                              exp->exp->type ) )
        return FALSE;
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_postfix
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_postfix( Chuck_Emmission * emit, a_Exp_Postfix exp )
{
    // emit the exp inside
    if( !emit_engine_emit_exp( emit, exp->exp ) )
        return FALSE;

    switch( exp->op )
    {
    case ae_op_plusplus:
        if( !emit_engine_emit_exp_mem( emit, exp->exp ) )
        {
            EM_error2( exp->linepos, 
                       "(emit): internal error: emitting [mem] exp from postfix..." );
            return FALSE;
        }
    
        if( exp->exp->type->type == te_int )
            emit->append( new Chuck_Instr_Inc_int );
        else if( exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Inc_uint );           
        else
        {
            EM_error2( exp->linepos,
                "(emit): internal error: unhandled type '%s' for post '++' operator",
                exp->exp->type->name );
            return FALSE;
        }
    break;

    case ae_op_minusminus:
        if( !emit_engine_emit_exp_mem( emit, exp->exp ) )
        {
            EM_error2( exp->linepos,
                       "(emit): internal error: emitting [mem] exp from postfix..." );
            return FALSE;
        }
        
        if( exp->exp->type->type == te_int )
            emit->append( new Chuck_Instr_Dec_int );
        else if( exp->exp->type->type == te_uint )
            emit->append( new Chuck_Instr_Dec_uint );           
        else
        {
            EM_error2( exp->linepos,
                "(emit): internal error: unhandled type '%s' for post '--' operator",
                exp->exp->type->name );
            return FALSE;
        }
    break;

    default:
        EM_error2( exp->linepos,
                   "(emit): internal error: unhandled postfix operator '%i'",
                   exp->op );
        return FALSE;
    }        
     
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_dur
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dur( Chuck_Emmission * emit, a_Exp_Dur exp )
{
    if( !emit_engine_emit_exp( emit, exp->base ) )
        return FALSE;

    // cast
    if( exp->base->type->type == te_int )
        emit->append( new Chuck_Instr_Cast_int2double );

    if( !emit_engine_emit_exp( emit, exp->unit ) )
        return FALSE;
        
    // multiply
    emit->append( new Chuck_Instr_Times_double );
        
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_symbol
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_symbol( Chuck_Emmission * emit, S_Symbol id,
                                  t_CKBOOL off, int linepos )
{
    t_CKUINT is_global = FALSE;
    t_CKUINT size = 0;
    int offset = off ? emit->find_offset( id, &is_global, &size ) : -1;
    if( offset < 0 )
    {
        // could be a function
        t_Type t = lookup_value( emit->nspc, id );
        if( t && t->type == te_function )
        {
            a_Func_Def func = lookup_func( emit->nspc, id, FALSE );
            if( !func || ( func && func->s_type == ae_func_user ) )
            {
                Func_Link * f = (Func_Link *)checked_malloc( sizeof( Func_Link ) );
                f->name = id;
                emit->append( f->op = new Chuck_Instr_Reg_Push_Imm( 0 ) );
                g_func_links.push_back( f );
            }
            else // func && func->s_type == ae_func_builtin
            {
                emit->nspc_func = func;
                emit->append( new Chuck_Instr_Reg_Push_Imm( func->stack_depth ) );
                emit->append( new Chuck_Instr_Reg_Push_Imm( 
                    (uint)func->builtin ) );
            }
        }
        else if( t && t->type == __te_system_namespace__ )
        {
            // find the namespace
            emit->nspc = lookup_namespace( emit->env, id, FALSE );
            if( !emit->nspc )
            {
                EM_error2( linepos,
                    "(emit): internal error: cannot find namespace '%s'",
                    S_name(id) );
                return FALSE;
            }
        }
        else if( t && t->type == __te_system_class__ )
        {
            // error
            EM_error2( linepos,
                       "(emit): internal error: class/lookup not impl" );
            return FALSE;
        }
        else if( emit->nspc && ( t = lookup_value( emit->nspc, id, FALSE ) ) )
        {
            void * addr = lookup_addr( emit->nspc, id, FALSE );
            if( !addr )
            {
                EM_error2( linepos,
                           "(emit): internal error looking up addr for '%s.%s'",
                           "[namespace]", S_name(id) );
                return FALSE;
            }

            // push the addr
            if( t->size == 4 || t->size == 8 )
                emit->append( new Chuck_Instr_Reg_Push_Deref( (uint)addr, t->size ) );
            else
            {
                EM_error2( linepos,
                           "(emit): internal error resolving '%s.%s'",
                           "[namespace]", S_name(id) );
                return FALSE;
            }
        }
        else
        {
            // error
            EM_error2( linepos,
                       "(emit): internal error resolving var '%s'",
                       S_name(id) );
            return FALSE;
        }
    }
    else
    {
        if( emit->is_global || !is_global )
        {
            if( size == 4 )
                emit->append( new Chuck_Instr_Reg_Push_Mem( offset ) );
            else if( size == 8 )
                emit->append( new Chuck_Instr_Reg_Push_Mem2( offset ) );
            else
            {
                EM_error2( linepos,
                           "(emit): internal error with reg push %i",
                           size );
                return FALSE;
            }
        }
        else // local and global
        {
            Chuck_Instr_Unary_Op * op = NULL;
            // the offset is not set yet
            emit->append( op = new Chuck_Instr_Reg_Push_Deref( offset, size ) );
            // queue for addr translation later
            emit->addr_map.push_back( op );
        }
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_primary
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_primary( Chuck_Emmission * emit, a_Exp_Primary exp )
{
    uint temp;
    t_CKDUR dur;
    Chuck_Instr_Unary_Op * op = NULL, * op2 = NULL;

    switch( exp->s_type )
    {
    case ae_primary_var:
        if( exp->var == insert_symbol( "now" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Now );
        }
        else if( exp->var == insert_symbol( "start" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Start );
        }
        else if( exp->var == insert_symbol( "midiin" ) )
        {
            if( emit->get_op() )
            {
                EM_error2( exp->linepos,
                           "MIDI in should be drained by using => before invoking again" );
                return FALSE;
            }

            emit->append( op = new Chuck_Instr_Midi_In(0) );
            emit->append( op2 = new Chuck_Instr_Midi_In_Go(0) );
            emit->push_op( op2 );
            emit->push_op( op );
        }
        else if( exp->var == insert_symbol( "midiout" ) )
        {
            if( emit->get_op() )
            {
                EM_error2( exp->linepos,
                           "MIDI out should be drained by using => before invoking again" );
                return FALSE;
            }

            emit->append( op = new Chuck_Instr_Midi_Out(0) );
            emit->append( op2 = new Chuck_Instr_Midi_Out_Go(0) );
            emit->push_op( op2 );
            emit->push_op( op );
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
        else if( exp->var == insert_symbol( "pi" ) )
        {
            double pi = 3.14159265358979323846;
            emit->append( new Chuck_Instr_Reg_Push_Imm2( pi ) );
        }
        else if( exp->var == insert_symbol( "endl" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm( (uint)"\n" ) );
        }
        else if( exp->var == insert_symbol( "stdout" ) || 
                 exp->var == insert_symbol( "chout" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        }
        else if( emit->find_dur( exp->var, &dur ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Imm2( dur ) );
        }
        else
        {
            // emit the symbol
            return emit_engine_emit_symbol( emit, exp->var, TRUE, exp->linepos );
        }
        break;
    
    case ae_primary_num:
        memcpy( &temp, &exp->num, sizeof(temp) );
        emit->append( new Chuck_Instr_Reg_Push_Imm( temp ) );
        break;
        
    case ae_primary_uint:
        emit->append( new Chuck_Instr_Reg_Push_Imm( exp->num2 ) );
        break;
        
    case ae_primary_float:
        emit->append( new Chuck_Instr_Reg_Push_Imm2( exp->fnum ) );
        break;
        
    case ae_primary_str:
        temp = (uint)exp->str;
        emit->append( new Chuck_Instr_Reg_Push_Imm( temp ) );
        break;
        
    case ae_primary_exp:
        emit_engine_emit_exp( emit, exp->exp );
        break;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_array
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_array( Chuck_Emmission * emit, a_Exp_Array exp )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_func_call
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_func_call( Chuck_Emmission * emit, a_Exp_Func_Call exp,
                                         t_CKBOOL spork )
{
    if( exp->args )
    {
        // emit the args
        if( !emit_engine_emit_exp( emit, exp->args ) )
        {
            EM_error2( exp->linepos,
                       "(emit): internal error in emitting function call arguments..." );
            return FALSE;
        }
    }

    // spork
    Chuck_Emmission * emit_save = emit;
    Chuck_Instr_Mem_Push_Imm * op = NULL;
    if( spork )
    {
        emit = emit_engine_init( emit->env );
        op = new Chuck_Instr_Mem_Push_Imm( 0 );
        // emit the stack depth - we don't know this yet
        emit->append( op );
    }

    // emit func
    if( !emit_engine_emit_exp( emit, exp->func ) )
    {
        EM_error2( exp->linepos,
                   "(emit): internal error in evaluating function call..." );
        return FALSE;
    }

    // push the local stack depth
    emit->append( new Chuck_Instr_Reg_Push_Imm( emit->stack_depth() ) );
    
    // call the function
    if( emit->nspc_func && emit->nspc_func->s_type == ae_func_builtin )
    {
        if( exp->ret_type->size == 0 )
            emit->append( new Chuck_Instr_Func_Call0 );
        else if( exp->ret_type->size == 4 )
            emit->append( new Chuck_Instr_Func_Call2 );
        else if( exp->ret_type->size == 8 )
            emit->append( new Chuck_Instr_Func_Call3 );
        else
        {
            EM_error2( exp->linepos,
                       "(emit): internal error: %i func call not handled",
                       exp->ret_type->size );
            return FALSE;
        }
    }
    else
    {
        Chuck_Instr * op = new Chuck_Instr_Func_Call;
        emit->append( op );
    }

    // spork
    if( spork )
    {
        // done
        emit->append( new Chuck_Instr_EOC );
        // set the stack depth now that we know
        op->set( emit->stack_depth() );

        // emit it
        Chuck_VM_Code * code = emit_to_code( emit );
        code->name = string("spork ~ exp");
        emit = emit_save;

        a_Exp e = exp->args;
        t_CKUINT size = 0;
        while( e ) { size += e->type->size; e=e->next; }        
        emit->append( new Chuck_Instr_Reg_Push_Imm( (uint)code ) );
        emit->append( new Chuck_Instr_Spork( size ) );
    }

    // reset the emit
    emit->nspc = emit->env;
    emit->nspc_func = NULL;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_dot_member
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dot_member( Chuck_Emmission * emit, a_Exp_Dot_Member exp, t_Type t )
{
    t_Type t_base = exp->base->type;
    t_Env e = NULL;
    
    // namespace or class
    if( t_base->type == __te_system_namespace__ )
    {
        // emit the base
        emit_engine_emit_exp( emit, exp->base );
        // emit the member
        emit_engine_emit_symbol( emit, exp->id, FALSE, exp->linepos );
    }
    else if( isa( t_base, lookup_type( emit->env, insert_symbol( "ugen" ) ) ) )
    {
        // emit the base
        emit_engine_emit_exp( emit, exp->base );
        // emit the member
        //if( !exp->data )
        //{
        //    EM_error2( exp->linepos,
        //        "(emit): internal error: no ctrl for '%s.%s'",
        //        t_base->name, S_name(exp->id) );
        //    return FALSE;
        //}
        // push the addr
        //emit->append( new Chuck_Instr_Reg_Push_Imm( exp->data ) );

        // cget
        if( exp->flag == 0 )
        {
            if( exp->data2 == 0 )
            {
                EM_error2( exp->linepos,
                           "(emit): internal error: cannot read from ugen parameter '%s.%s', it is write-only",
                           t_base->name, S_name(exp->id) );
                return FALSE;
            }

            // the cget function addr
            emit->append( new Chuck_Instr_Reg_Push_Imm( exp->data2 ) );

            // the ugen
            if( !strcmp( S_name(exp->id), "op" ) )
                emit->append( new Chuck_Instr_UGen_CGet_Op );
            else if( !strcmp( S_name(exp->id), "gain" ) )
                emit->append( new Chuck_Instr_UGen_CGet_Gain );
            else if( !strcmp( S_name(exp->id), "last" ) )
                emit->append( new Chuck_Instr_UGen_CGet_Last );
            else
            {
                // cget passing to ugen
                if( t->size == 4 )
                    emit->append( new Chuck_Instr_UGen_CGet );
                else if( t->size == 8 )
                    emit->append( new Chuck_Instr_UGen_CGet2 );
                else
                {
                    EM_error2( exp->linepos,
                               "(emit): internal error: %i ugen cget not handled",
                               t->size );
                    return FALSE;
                }
            }
        }
    }
    else
    {
        EM_error2( exp->linepos,
                   "(emit): internal error: class.member not impl" );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_if
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_if( Chuck_Emmission * emit, a_Exp_If exp )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_decl
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_decl( Chuck_Emmission * emit, a_Exp_Decl exp )
{
    a_Var_Decl_List list = exp->var_decl_list;
    a_Var_Decl var_decl = NULL;

    while( list )
    {
        var_decl = list->var_decl;
        t_Type type = lookup_type( emit->env, exp->type );

        if( var_decl->isarray )
        {
            EM_error2( exp->linepos,
                       "(emit): internal error: array not impl" );
            return FALSE;
        }
        else
        {
            // offset
            emit->append( new Chuck_Instr_Reg_Push_Imm(
                emit->alloc_local( var_decl->id, type ) ) );

            // ugen
            if( isa( type, lookup_type(emit->env, insert_symbol("ugen")) ) )
            {
                Chuck_UGen_Info * info = lookup_ugen( emit->nspc, exp->type );
                if( !info )
                {
                    EM_error2( exp->linepos,
                        "(emit): internal error: undefined ugen type '%s'",
                        S_name(var_decl->id) );
                    return FALSE;
                }
                emit->append( new Chuck_Instr_Reg_Push_Imm( (uint)info ) );
                emit->append( new Chuck_Instr_UGen_Alloc() );
                emit->append( new Chuck_Instr_Chuck_Assign_Object2 );
            }
        }
        
        list = list->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_namespace()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_namespace( Chuck_Emmission * emit, a_Exp_Namespace exp )
{
    // push the name
    emit->append( new Chuck_Instr_Reg_Push_Imm( (t_CKUINT)S_name(exp->name) ) );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_op()
// desc: emit op
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_op( Chuck_Emmission * emit,
                              ae_Operator op, a_Exp lhs, a_Exp rhs )
{
    te_Type left = lhs->type->type;
    te_Type right = rhs->type->type;
    
    switch( op )
    {
    
    // ----------------------------- num --------------------------------------
    
    case ae_op_plus:
        if( ( left == te_dur && right == te_time ) ||
            ( left == te_time && right == te_dur ) )
            emit->append( new Chuck_Instr_Add_double );
        else
        {
            switch( left )
            {
            case te_int:
                emit->append( new Chuck_Instr_Add_int );
                break;
            case te_uint:
                emit->append( new Chuck_Instr_Add_uint );
                break;
            case te_float:
                emit->append( new Chuck_Instr_Add_double );
                break;
            case te_dur:
                emit->append( new Chuck_Instr_Add_double );
                break;
                 
            default:
                EM_error2( lhs->linepos, 
                           "(emit): internal error: unhandled type '%i' in binary op '+'",
                           left );
                return FALSE;
            }
        }
        break;
    
    case ae_op_minus:
        if( ( left == te_time && right == te_dur ) )
            emit->append( new Chuck_Instr_Minus_double );
        else if( ( left == te_time && right == te_time ) ) // XXX time - time = dur
            emit->append( new Chuck_Instr_Minus_double );
        else
        {
            switch( left )
            {
            case te_int:
                emit->append( new Chuck_Instr_Minus_int );
                break;
            case te_uint:
                emit->append( new Chuck_Instr_Minus_uint );
                break;
            case te_float:
                emit->append( new Chuck_Instr_Minus_double );
                break;
            case te_dur:
                emit->append( new Chuck_Instr_Minus_double );
                break;
                 
            default:
                EM_error2( lhs->linepos, 
                           "(emit): internal error: unhandled '%i' in binary op '-'",
                           left );
                return FALSE;
            }
        }
        break;
    
    case ae_op_times:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Times_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Times_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Times_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Times_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '*'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_divide:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Divide_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Divide_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Divide_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Divide_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '/'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_s_or:
        switch( left )
        {
        case te_int:
        case te_uint:
            emit->append( new Chuck_Instr_Binary_Or );
            break;
        
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '|'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_s_and:
        switch( left )
        {
        case te_int:
        case te_uint:
            emit->append( new Chuck_Instr_Binary_And );
            break;
        
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '&'",
                       left );
            return FALSE;
        }
        break;

    case ae_op_shift_left:
        switch( left )
        {
        case te_int:
        case te_uint:
            emit->append( new Chuck_Instr_Binary_Shift_Left );
            break;
        
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '<<'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_shift_right:
        switch( left )
        {
        case te_int:
        case te_uint:
            emit->append( new Chuck_Instr_Binary_Shift_Right );
            break;
        
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '>>'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_percent:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Mod_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Mod_uint );
            break;
        case te_float:
        case te_time:
        case te_dur:
            emit->append( new Chuck_Instr_Mod_double );
            break;
        
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '%%'",
                       left );
            return FALSE;
        }
        break;

    case ae_op_s_xor:
        switch( left )
        {
        case te_int:
        case te_uint:
            emit->append( new Chuck_Instr_Binary_Xor );
            break;
        case te_midiin:
        {
            Chuck_Instr_Unary_Op * op = emit->get_op();
            if( !op )
            {
                EM_error2( lhs->linepos,
                           "(emit): internal error: in binary op '%i'", left );
                return FALSE;
            }
            op->set( op->get() | 1 );
            // emit->append_ops();
        }
            break;
        case te_midiout:
        {
            Chuck_Instr_Unary_Op * op = emit->get_op();
            if( !op )
            {
                EM_error2( lhs->linepos,
                           "(emit): internal error: in binary op '%i'", left );
                return FALSE;
            }
            op->set( op->get() | 1 );
            // emit->append_ops();
        }
            break;
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '^'",
                       left );
            return FALSE;
        }
        break;
    
    // ----------------------------- chuck -------------------------------------
    
    case ae_op_chuck:
    {
        a_Exp cl = lhs, cr = rhs;
        
        while( cr )
        {
            cl = lhs;
            while( cl )
            {
                if( !emit_engine_emit_chuck( emit, cl, cr ) )
                    return FALSE;
                cl = cl->next;
            }
            
            cr = cr->next;
        }
        break;
    }
    
    case ae_op_unchuck:
    {
        a_Exp cl = lhs, cr = rhs;
        
        while( cr )
        {
            cl = lhs;
            while( cl )
            {
                if( !emit_engine_emit_unchuck( emit, cl, cr ) )
                    return FALSE;
                cl = cl->next;
            }
            
            cr = cr->next;
        }
        break;
    }
    
    case ae_op_at_chuck:
    break;
    
    case ae_op_s_chuck:
    break;
    
    case ae_op_plus_chuck:
    break;
    
    case ae_op_minus_chuck:
    break;
    
    case ae_op_times_chuck:
    break;
    
    case ae_op_divide_chuck:
    break;
    
    case ae_op_s_and_chuck:
    break;
    
    case ae_op_s_or_chuck:
    break;
    
    case ae_op_s_xor_chuck:
    break;
    
    case ae_op_shift_right_chuck:
    break;
    
    case ae_op_shift_left_chuck:
    break;
    
    case ae_op_percent_chuck:
    break;

    // -------------------------------- bool -----------------------------------
        
    case ae_op_eq:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Eq_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Eq_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Eq_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Eq_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Eq_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '=='",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_neq:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Neq_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Neq_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Neq_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Neq_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Neq_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '!='",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_lt:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Lt_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Lt_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Lt_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Lt_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Lt_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '<'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_le:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Le_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Le_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Le_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Le_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Le_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '<='",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_gt:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Gt_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Gt_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Gt_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Gt_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Gt_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '>='",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_ge:
        switch( left )
        {
        case te_int:
            emit->append( new Chuck_Instr_Ge_int );
            break;
        case te_uint:
            emit->append( new Chuck_Instr_Ge_uint );
            break;
        case te_float:
            emit->append( new Chuck_Instr_Ge_double );
            break;
        case te_dur:
            emit->append( new Chuck_Instr_Ge_double );
            break;
        case te_time:
            emit->append( new Chuck_Instr_Ge_double );
            break;

        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '>='",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_and:
        switch( left )
        {
        case te_int:
        case te_uint:
        //case te_float:
        //case te_dur:
        //case te_time:
            emit->append( new Chuck_Instr_And );
            break;
 
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '&&'",
                       left );
            return FALSE;
        }
        break;
    
    case ae_op_or:
        switch( left )
        {
        case te_int:
        case te_uint:
        //case te_float:
        //case te_dur:
        //case te_time:
            emit->append( new Chuck_Instr_Or );
            break;
 
        default:
            EM_error2( lhs->linepos,
                       "(emit): internal error: unhandled type '%i' in binary op '||'",
                       left );
            return FALSE;
        }
        break;

    default:
        EM_error2( lhs->linepos,
                   "(emit): internal error: op '%i' not handled",
                   op );
        return FALSE;
    }
        
    return TRUE;
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_while()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_while( Chuck_Emmission * emit, a_Stmt_While stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    uint start_index = emit->next_index();
    
    // emit the cond
    emit_engine_emit_exp( emit, stmt->cond );
    
    switch( stmt->cond->type->type )
    {
    case te_int:
    case te_uint:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_uint( 0 );
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
                   "(emit): internal error: unhandled type '%s' in while conditional",
                   stmt->cond->type->name );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );
    
    // set the op's target
    op->set( emit->next_index() );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_do_while()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_do_while( Chuck_Emmission * emit, a_Stmt_While stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    uint start_index = emit->next_index();
    
    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // emit the cond
    emit_engine_emit_exp( emit, stmt->cond );
    
    switch( stmt->cond->type->type )
    {
    case te_int:
    case te_uint:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Neq_uint( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Neq_double( 0 );
        break;
        
    default:
        EM_error2( stmt->cond->linepos,
                   "(emit): internal error: unhandled type '%s' in do/while conditional",
                   stmt->cond->type->name );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // set the op's target
    op->set( start_index );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_until()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_until( Chuck_Emmission * emit, a_Stmt_Until stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    uint start_index = emit->next_index();
    
    // emit the cond
    emit_engine_emit_exp( emit, stmt->cond );
    
    switch( stmt->cond->type->type )
    {
    case te_int:
    case te_uint:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Neq_uint( 0 );
        break;
    case te_float:
    case te_dur:
    case te_time:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
        op = new Chuck_Instr_Branch_Neq_double( 0 );
        break;
        
    default:
        EM_error2( stmt->cond->linepos,
                   "(emit): internal error: unhandled type '%s' in until conditional",
                   stmt->cond->type->name );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );
    
    // set the op's target
    op->set( emit->next_index() );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_do_until()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_do_until( Chuck_Emmission * emit, a_Stmt_Until stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    uint start_index = emit->next_index();
    
    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // emit the cond
    emit_engine_emit_exp( emit, stmt->cond );
    
    switch( stmt->cond->type->type )
    {
    case te_int:
    case te_uint:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_uint( 0 );
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
                   stmt->cond->type->name );
        return FALSE;
    }
    
    // append the op
    emit->append( op );

    // set the op's target
    op->set( start_index );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_for()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_for( Chuck_Emmission * emit, a_Stmt_For stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL;
    
    // emit the cond
    emit_engine_emit_stmt( emit, stmt->c1 );
    
    uint start_index = emit->next_index();
    
    // emit the cond
    emit_engine_emit_stmt( emit, stmt->c2, FALSE );
    
    // could be NULL
    if( stmt->c2 )
    {
        switch( stmt->c2->stmt_exp->type->type )
        {
        case te_int:
        case te_uint:
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
            op = new Chuck_Instr_Branch_Eq_uint( 0 );
            break;
        case te_float:
        case te_dur:
        case te_time:
            // push 0
            emit->append( new Chuck_Instr_Reg_Push_Imm2( 0.0 ) );
            op = new Chuck_Instr_Branch_Eq_double( 0 );
            break;
        
        default:
            EM_error2( stmt->c2->stmt_exp->linepos,
                       "(emit): internal error: unhandled type '%s' in for conditional",
                       stmt->c2->stmt_exp->type->name );
            return FALSE;
        }
        // append the op
        emit->append( op );
    }
    
    // emit the body
    emit_engine_emit_stmt( emit, stmt->body );
    
    // emit the action
    emit_engine_emit_exp( emit, stmt->c3 );
    if( stmt->c3 )
        if( stmt->c3->type->size == 8 )
            emit->append( new Chuck_Instr_Reg_Pop_Word2 );
        else if( stmt->c3->type->size == 4 )
            emit->append( new Chuck_Instr_Reg_Pop_Word );
        else if( stmt->c3->type->size != 0 )
        {
            EM_error2( stmt->c3->linepos,
                       "(emit): internal error: non-void type size %i not handled",
                       stmt->c3->type->size );
            return FALSE;
        }

    // go back to do check the condition
    emit->append( new Chuck_Instr_Goto( start_index ) );    

    // could be NULL
    if( stmt->c2 )
        // set the op's target
        op->set( emit->next_index() );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_if()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_if( Chuck_Emmission * emit, a_Stmt_If stmt )
{
    t_CKBOOL ret = TRUE;
    Chuck_Instr_Branch_Op * op = NULL, * op2 = NULL;
    uint start_index = emit->next_index();
    
    // emit the cond
    emit_engine_emit_exp( emit, stmt->cond );
    
    switch( stmt->cond->type->type )
    {
    case te_int:
    case te_uint:
        // push 0
        emit->append( new Chuck_Instr_Reg_Push_Imm( 0 ) );
        op = new Chuck_Instr_Branch_Eq_uint( 0 );
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
                   "(emit): internal erorr: unhandled type '%s' in if conditional",
                   stmt->cond->type->name );
        break;
    }
    
    // append the op
    emit->append( op );

    // emit the body
    emit_engine_emit_stmt( emit, stmt->if_body );
    
    // emit the skip to the end
    emit->append( op2 = new Chuck_Instr_Goto(0) );
    
    // set the op's target
    op->set( emit->next_index() );

    // emit the body
    emit_engine_emit_stmt( emit, stmt->else_body );

    // set the op2's target
    op2->set( emit->next_index() );

    return ret;    
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_return()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_return( Chuck_Emmission * emit, a_Stmt_Return ret )
{
    if( !emit_engine_emit_exp( emit, ret->val ) )
        return FALSE;

    // determine where later
    Chuck_Instr_Goto * instr = new Chuck_Instr_Goto( 0 );
    emit->append( instr );
    emit->returns.push_back( instr );

    return TRUE;
}




//------------------------------------------------------------------------------
// name: emit_engine_emit_code_segment()
// desc: ...
//------------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_code_segment( Chuck_Emmission * emit, a_Stmt_Code stmt )
{
    a_Stmt_List list = stmt->stmt_list;
    
    while( list )
    {
        if( !emit_engine_emit_stmt( emit, list->stmt ) )
            return FALSE;
            
        list = list->next;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_chuck()
// desc: emit chuck operator
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_chuck( Chuck_Emmission * emit, a_Exp lhs, a_Exp rhs )
{
    if( rhs->type->type == te_midiout || lhs->type->type == te_midiin )
        emit->pop_the_ops();
    
    if( lhs->type->parent && rhs->type->parent &&
        lhs->type->parent->type == te_ugen &&
        rhs->type->parent->type == te_ugen )
    {
        emit->append( new Chuck_Instr_UGen_Link );
        
        return TRUE;
    }
    else if( lhs->type->type == __te_system_out__ )
    {
        switch( rhs->type->type )
        {
            case te_int:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_int ) );
                break;
            case te_uint:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_uint ) );
                break;
            case te_float:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_float ) );
                break;
            case te_double:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_double ) );
                break;
            case te_time:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_time ) );
                break;
            case te_dur:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_dur ) );
                break;
            case te_string:
                emit->append( new Chuck_Instr_Reg_Push_Imm( cip_string ) );
                break;
            default:
                EM_error2( rhs->linepos,
                           "(emit): internal error: unsupported type '%s' to print to console",
                           rhs->type->name );
                return FALSE;
        }
        
        emit->append( new Chuck_Instr_Print_Console2 );
    }
    else if( rhs->s_type != ae_exp_decl )
    {
        if( rhs->s_type == ae_exp_primary && 
            rhs->primary.s_type == ae_primary_var )
        {
            a_Exp_Primary primary = &(rhs->primary);
            if( !strcmp( S_name(primary->var), "now" ) )
            {
                // special case: time advance
                if( lhs->type->type == te_dur )
                    emit->append( new Chuck_Instr_Add_dur_time );
                else if( lhs->type->type == te_time )
                    // pop the now - no need it anymore
                    emit->append( new Chuck_Instr_Reg_Pop_Word2() );
                else
                {
                    EM_error2( lhs->linepos,
                               "(emit): internal error: bad type '%s' chucked to now",
                               lhs->type->name );
                    return FALSE;
                }

                // advance
                emit->append( new Chuck_Instr_Time_Advance );

                return TRUE;
            }
            else if( !strcmp( S_name(primary->var), "midiin" ) )
            {
                EM_error2( lhs->linepos,
                           "(emit): internal error: value chucked to midiin" );
                return FALSE;
            }
            else if( !strcmp( S_name(primary->var), "midiout" ) )
            {
                emit->append_ops();
                // emit->append( new Chuck_Instr_Midi_Out_Go(0) );
                return TRUE; 
            }
            else if( !strcmp( S_name(primary->var), "true" ) )
            {
                EM_error2( primary->linepos,
                           "cannot assign value to 'true'" );
                return FALSE;
            }
            else if( !strcmp( S_name(primary->var), "false" ) )
            {
                EM_error2( primary->linepos,
                           "cannot assign value to 'false'" );
                return FALSE;
            }
            else if( !strcmp( S_name(primary->var), "maybe" ) )
            {
                EM_error2( primary->linepos,
                           "cannot assign value to 'maybe'" );
                return FALSE;
            }
            else if( !strcmp( S_name(primary->var), "pi" ) )
            {
                EM_error2( primary->linepos,
                           "cannot assign value to 'pi'" );
                return FALSE;
            }
            else if( !strcmp( S_name(primary->var), "stdout" ) ||
                     !strcmp( S_name(primary->var), "chout" ) )
            {
                switch( lhs->type->type )
                {
                    case te_int:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_int ) );
                        break;
                    case te_uint:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_uint ) );
                        break;
                    case te_float:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_double ) );
                        break;
                    case te_double:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_double ) );
                        break;
                    case te_time:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_time ) );
                        break;
                    case te_dur:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_dur ) );
                        break;
                    case te_string:
                        emit->append( new Chuck_Instr_Reg_Push_Imm( cip_string ) );
                        break;
                    default:
                        EM_error2( lhs->linepos,
                                   "(emit): internal error: unsupported type '%s' to print to console",
                                   lhs->type->name );
                        return FALSE;
                }
                
                emit->append( new Chuck_Instr_Print_Console );
            }
            else
            {
                t_CKUINT is_global = FALSE;
                t_CKUINT size = 0;
                int offset = emit->find_offset( primary->var, &is_global, &size );
                Chuck_Instr_Unary_Op * op = NULL;
                
                if( offset < 0 )
                {
                    // should be a function
                    EM_error2( rhs->linepos,
                               "(emit): internal error: chuck to function not impl" );
                    return FALSE;
                }
                else
                {
                    // TODO: fix this hack
                    if( size == 4 )
                        emit->append( new Chuck_Instr_Reg_Pop_Word );
                    else if( size == 8 )
                        emit->append( new Chuck_Instr_Reg_Pop_Word2 );
                    else
                    {
                        EM_error2( rhs->linepos,
                                   "(emit): %i byte not handled in chuck operator : decl",
                                   size );
                        return FALSE;
                    }

                    emit->append( op = new Chuck_Instr_Reg_Push_Imm( offset ) );
                    if( emit->is_global || !is_global )
                    {
                        // special case: assignment
                        if( rhs->type->parent == NULL )
                        {
                            if( rhs->type->size == 4 )
                                emit->append( new Chuck_Instr_Chuck_Assign );
                            else if( rhs->type->size == 8 )
                                emit->append( new Chuck_Instr_Chuck_Assign2 );
                            else
                            {
                                EM_error2( rhs->linepos,
                                           "(emit): %i byte not handled in chuck operator assign",
                                           rhs->type->size );
                                return FALSE;
                            }
                        }
                        else
                            emit->append( new Chuck_Instr_Chuck_Assign_Object );
                    }
                    else
                    {
                        emit->addr_map.push_back( op );
                        // special case: deref assignment
                        if( rhs->type->parent == NULL )
                        {
                            if( rhs->type->size == 4 )
                                emit->append( new Chuck_Instr_Chuck_Assign_Deref );
                            else
                                emit->append( new Chuck_Instr_Chuck_Assign_Deref2 );
                        }
                        else
                            emit->append( new Chuck_Instr_Chuck_Assign_Object_Deref );
                    }
                }
            }
        }
        else if( rhs->s_type == ae_exp_namespace )
        {
            if( lhs->type->type == te_string )
            {
                // emit instruction to load the DLL
                emit->append( new Chuck_Instr_DLL_Load );
            }
            else
            {
                EM_error2( lhs->linepos,
                           "(emit): internal error: unhandled type '%s' in namespace expression",
                           lhs->type->name );
                return FALSE;
            }
        }
        else if( rhs->s_type == ae_exp_dot_member && rhs->dot_member.data )
        {
            // the function addr
            emit->append( new Chuck_Instr_Reg_Push_Imm( rhs->dot_member.data ) );
            emit->append( new Chuck_Instr_Reg_Push_Imm( rhs->dot_member.data2 ) );

            // the ugen
            if( !strcmp( S_name(rhs->dot_member.id), "op" ) )
                emit->append( new Chuck_Instr_UGen_Ctrl_Op );
            else if( !strcmp( S_name(rhs->dot_member.id), "gain" ) )
                emit->append( new Chuck_Instr_UGen_Ctrl_Gain );
            else if( !strcmp( S_name(rhs->dot_member.id), "last" ) )
            {
                EM_error2( rhs->linepos,
                           "(emit): internal error: cannot chuck to 'ugen.last'" );
                return FALSE;
            }
            else
            {
                // ctrl passing to ugen
                if( rhs->type->size == 4 )
                    emit->append( new Chuck_Instr_UGen_Ctrl );
                else if( rhs->type->size == 8 )
                    emit->append( new Chuck_Instr_UGen_Ctrl2 );
                else
                {
                    EM_error2( rhs->linepos,
                               "(emit): internal error: %i ugen ctrl not handled",
                               rhs->type->size );
                    return FALSE;
                }
            }
        }
        else if( rhs->s_type == ae_exp_dot_member )
        {
            EM_error2( rhs->linepos,
                       "(emit): cannot assign values to namespace constants" );
            return FALSE;
        }
        else
        {
            EM_error2( rhs->linepos,
                       "(emit): internal error: illegal expression" );
            return FALSE;
        }
    }
    else // if( rhs->s_type == ae_exp_decl )
    {
        // special case: assignment
        if( rhs->type->parent == NULL )
        {
            if( rhs->type->size == 4 )
                emit->append( new Chuck_Instr_Chuck_Assign );
            else if( rhs->type->size == 8 )
                emit->append( new Chuck_Instr_Chuck_Assign2 );
            else
            {
                EM_error2( rhs->linepos,
                           "(emit): %i size not handled in chuck assigg",
                           rhs->type->size );
                return FALSE;
            }
        }
        else
            emit->append( new Chuck_Instr_Chuck_Assign_Object );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_unchuck()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_unchuck( Chuck_Emmission * emit, a_Exp lhs, a_Exp rhs )
{
    if( lhs->type->parent && rhs->type->parent && 
        lhs->type->parent->type == te_ugen && rhs->type->parent->type == te_ugen )
    {
        emit->append( new Chuck_Instr_UGen_UnLink );
    }
    else
    {
        EM_error2( lhs->linepos,
            "(emit): internal error: no suitable un-chuck resolution on types '%s' and '%s'",
            lhs->type->name, rhs->type->name );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_func_def()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_func_def( Chuck_Emmission * emit, a_Func_Def func_def )
{
    if( func_def->s_type == ae_func_user )
    {
        emit->is_global = FALSE;
        emit->local = new Chuck_Code( S_name(func_def->name) );
        emit->push( "__func_def_scope__" );
        emit->functions.push_back( emit->local );
        emit->local->stack_depth = func_def->stack_depth;
        emit->returns.clear();

        // args
        a_Arg_List args = func_def->arg_list;

        while( args )
        {
            emit->alloc_local( args->id, args->type );
            args = args->next;
        }

        // emit the function body
        emit_engine_emit_stmt( emit, func_def->code );

        // return index to the end of the function
        for( unsigned int i = 0; i < emit->returns.size(); i++ )
            emit->returns[i]->set( emit->next_index() );

        // emit the return
        emit->append( new Chuck_Instr_Func_Return );

        emit->pop();
        emit->is_global = TRUE;
        emit->local = NULL;
    }
    else
    {
        EM_error2( func_def->linepos,
                   "(emit): internal error: builtin functions not impl" );
        return FALSE;
    }

    return TRUE;
}



//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_mem()
// desc: emit an exp as memory address only
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_mem( Chuck_Emmission * emit, a_Exp exp )
{
    while( exp )
    {
        switch( exp->s_type )
        {
        case ae_exp_primary:
            if( !emit_engine_emit_exp_primary_mem( emit, &exp->primary ) )
                return FALSE;
            break;

        case ae_exp_dot_member:
            if( !emit_engine_emit_exp_dot_member_mem( emit, &exp->dot_member ) )
                return FALSE;
            break;

        case ae_exp_array:
            if( !emit_engine_emit_exp_array_mem( emit, &exp->array ) )
                return FALSE;
            break;

        case ae_exp_func_call:
            if( !emit_engine_emit_exp_func_call_mem( emit, &exp->func_call ) )
                return FALSE;
            break;

        default:
            EM_error2( exp->linepos,
                       "(emit): internal error: unhandled expression in [mem] emission" );
            return FALSE;
        }

        exp = exp->next;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_primary_mem()
// desc: emit a primary exp as memory address only
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_primary_mem( Chuck_Emmission * emit, a_Exp_Primary exp )
{
    switch( exp->s_type )
    {
    case ae_primary_var:
        /*if( exp->var == insert_symbol( "now" ) )
        {
            emit->append( new Chuck_Instr_Reg_Push_Now );
        }
        else */
        {
            int offset = emit->find_offset( exp->var );
            if( offset < 0 )
            {
                // could be a function
                EM_error2( exp->linepos,
                           "(emit): internal error: cannot resolve [mem] var '%s'",
                           S_name(exp->var) );                
                return FALSE;
            }

            emit->append( new Chuck_Instr_Reg_Push_Imm( offset ) );
        }
        break;

    case ae_primary_exp:        
        return emit_engine_emit_exp_mem( emit, exp->exp );
        break;
        
    default:
        EM_error2( exp->linepos,
                   "(emit): internal error: cannot emit [mem] primary exp" );
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_dot_member_mem()
// desc: emit a dot exp as memory address only
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_dot_member_mem( Chuck_Emmission * emit, a_Exp_Dot_Member exp )
{
    EM_error2( exp->linepos,
               "(emit): internal erorr: . address not impl" );
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_array_mem()
// desc: emit array exp as memory address only
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_array_mem( Chuck_Emmission * emit, a_Exp_Array exp )
{
    EM_error2( exp->linepos,
               "(emit): internal error: array address not impl" );
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: emit_engine_emit_exp_func_call_mem()
// desc: emit a func call exp as memory address only
//-----------------------------------------------------------------------------
t_CKBOOL emit_engine_emit_exp_func_call_mem( Chuck_Emmission * emit, a_Exp_Func_Call exp )
{
    EM_error2( exp->linepos,
               "(emit): internal error: func call address no impl" );
    return FALSE;
}
