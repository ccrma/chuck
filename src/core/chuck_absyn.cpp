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
// file: chuck_absyn.cpp
// desc: chuck abstract syntax
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_absyn.h"
#include "chuck_utils.h"
#include "chuck_errmsg.h"
#include <stdlib.h>
#include <stdio.h>


// 1.5.0.5 (ge) option to include in case we need something from flex/bison
// #include "chuck_yacc.h"
// ASSUME: on systems where the lexer/parser is generated using flex/bison,
// that chuck_yacc.h is the same as chuck.tab.h


//-----------------------------------------------------------------------------
// AST construction function (these are mostly called by bison)
//-----------------------------------------------------------------------------
a_Program new_program( a_Section section, uint32_t lineNum, uint32_t posNum )
{
    // NB checked_malloc() zeros the allocated memory
    a_Program a = (a_Program)checked_malloc( sizeof( struct a_Program_ ) );
    a->section = section;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Program append_program( a_Program program, a_Section section, uint32_t lineNum, uint32_t posNum )
{
    a_Program a = new_program( section, lineNum, posNum );
    a_Program current = program->next;

    // initial state
    if( current == NULL )
    {
        program->next = a;
        return program;
    }

    // go to the end
    while( current->next ) current = current->next;
    // append
    current->next = a;

    return program;
}

a_Section new_section_stmt( a_Stmt_List list, uint32_t lineNum, uint32_t posNum )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_ ) );
    a->s_type = ae_section_stmt;
    a->stmt_list = list;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Section new_section_func_def( a_Func_Def func_def, uint32_t lineNum, uint32_t posNum )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_) );
    a->s_type = ae_section_func;
    a->func_def = func_def;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Section new_section_class_def( a_Class_Def class_def, uint32_t lineNum, uint32_t posNum )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_) );
    a->s_type = ae_section_class;
    a->class_def = class_def;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Stmt_List new_stmt_list( a_Stmt stmt, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt_List a = (a_Stmt_List)checked_malloc( sizeof( struct a_Stmt_List_ ) );
    a->stmt = stmt;
    a->next = NULL;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Stmt_List append_stmt_list( a_Stmt_List stmt_list, a_Stmt stmt, uint32_t lineNum, uint32_t posNum )
{
  a_Stmt_List a = new_stmt_list( stmt, lineNum, posNum );
  a_Stmt_List current;

  current = stmt_list->next;
  if (current == NULL) {
    stmt_list->next = a;
    return stmt_list;
  }

  while (1)
    {
      if (current->next == NULL) {
        current->next = a;
        break;
      } else {
        current = current->next;
      }
    }
  return stmt_list;
}

a_Stmt new_stmt_from_expression( a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_exp;
    a->stmt_exp = exp;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Stmt new_stmt_from_code( a_Stmt_List stmt_list, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_code;
    a->stmt_code.stmt_list = stmt_list;
    a->line = lineNum; a->where = posNum;
    a->stmt_code.line = lineNum; a->stmt_code.where = posNum;
    a->stmt_code.self = a;

    return a;
}

a_Stmt new_stmt_from_if( a_Exp cond, a_Stmt if_body, a_Stmt else_body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_if;
    a->stmt_if.cond = cond;
    a->stmt_if.if_body = if_body;
    a->stmt_if.else_body = else_body;
    a->line = lineNum; a->where = posNum;
    a->stmt_if.line = lineNum; a->stmt_if.where = posNum;
    a->stmt_if.self = a;

    return a;
}

a_Stmt new_stmt_from_while( a_Exp cond, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_while;
    a->stmt_while.is_do = 0;
    a->stmt_while.cond = cond;
    a->stmt_while.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_while.line = lineNum; a->stmt_while.where = posNum;
    a->stmt_while.self = a;

    return a;
}

a_Stmt new_stmt_from_do_while( a_Exp cond, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_while;
    a->stmt_while.is_do = 1;
    a->stmt_while.cond = cond;
    a->stmt_while.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_while.line = lineNum; a->stmt_while.where = posNum;
    a->stmt_while.self = a;

    return a;
}

a_Stmt new_stmt_from_until( a_Exp cond, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_until;
    a->stmt_until.is_do = 0;
    a->stmt_until.cond = cond;
    a->stmt_until.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_until.line = lineNum; a->stmt_until.where = posNum;
    a->stmt_until.self = a;

    return a;
}

a_Stmt new_stmt_from_do_until( a_Exp cond, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_until;
    a->stmt_until.is_do = 1;
    a->stmt_until.cond = cond;
    a->stmt_until.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_until.line = lineNum; a->stmt_until.where = posNum;
    a->stmt_until.self = a;

    return a;
}

a_Stmt new_stmt_from_for( a_Stmt c1, a_Stmt c2, a_Exp c3, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_for;
    a->stmt_for.c1 = c1;
    a->stmt_for.c2 = c2;
    a->stmt_for.c3 = c3;
    a->stmt_for.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_for.line = lineNum; a->stmt_for.where = posNum;
    a->stmt_for.self = a;

    return a;
}

a_Stmt new_stmt_from_loop( a_Exp cond, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_loop;
    a->stmt_loop.cond = cond;
    a->stmt_loop.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_loop.line = lineNum; a->stmt_loop.where = posNum;
    a->stmt_loop.self = a;

    return a;
}

a_Stmt new_stmt_from_switch( a_Exp val, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_switch;
    a->stmt_switch.val = val;
    a->line = lineNum; a->where = posNum;
    a->stmt_switch.line = lineNum; a->stmt_switch.where = posNum;
    a->stmt_switch.self = a;

    return a;
}

a_Stmt new_stmt_from_break( uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_break;
    a->line = lineNum; a->where = posNum;
    a->stmt_break.line = lineNum; a->stmt_break.where = posNum;
    a->stmt_break.self = a;

    return a;
}

a_Stmt new_stmt_from_continue( uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_continue;
    a->line = lineNum; a->where = posNum;
    a->stmt_continue.line = lineNum; a->stmt_continue.where = posNum;
    a->stmt_continue.self = a;

    return a;
}

a_Stmt new_stmt_from_return( a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_return;
    a->stmt_return.val = exp;
    a->line = lineNum; a->where = posNum;
    a->stmt_return.line = lineNum; a->stmt_return.where = posNum;
    a->stmt_return.self = a;

    return a;
}

a_Stmt new_stmt_from_label( c_str xid, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_gotolabel;
    a->stmt_gotolabel.name = insert_symbol( xid );
    a->line = lineNum; a->where = posNum;
    a->stmt_gotolabel.line = lineNum; a->stmt_gotolabel.where = posNum;
    a->stmt_gotolabel.self = a;

    return a;
}

a_Stmt new_stmt_from_case( a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_case;
    a->stmt_case.exp = exp;
    a->line = lineNum; a->where = posNum;
    a->stmt_case.line = lineNum; a->stmt_case.where = posNum;
    a->stmt_case.self = a;

    return a;
}

a_Exp append_expression( a_Exp list, a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
  a_Exp current;
  current = list->next;
  if (current == NULL) {
    list->next = exp;
    return list;
  }

  while (1)
    {
      if (current->next == NULL) {
        current->next = exp;
        break;
      } else {
        current = current->next;
      }
    }
    return list;
}

a_Exp new_exp_from_binary( a_Exp lhs, ae_Operator oper, a_Exp rhs, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_binary;
    a->s_meta = ae_meta_value;
    a->binary.lhs = lhs;
    a->binary.op = oper;
    a->binary.rhs = rhs;
    a->line = lineNum; a->where = posNum;
    a->binary.line = lineNum; a->binary.where = posNum;
    a->binary.self = a;

    return a;
}

a_Exp new_exp_from_unary( ae_Operator oper, a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_unary;
    a->s_meta = exp->s_meta;
    a->unary.op = oper;
    a->unary.exp = exp;
    a->line = lineNum; a->where = posNum;
    a->unary.line = lineNum; a->unary.where = posNum;
    a->unary.self = a;

    return a;
}

a_Exp new_exp_from_unary2( ae_Operator oper, a_Type_Decl type,
                           a_Array_Sub array, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_unary;
    a->s_meta = ae_meta_value;
    a->unary.op = oper;
    a->unary.type = type;
    a->unary.array = array;
    a->line = lineNum; a->where = posNum;
    a->unary.line = lineNum; a->unary.where = posNum;
    a->unary.self = a;

    return a;
}

a_Exp new_exp_from_unary3( ae_Operator oper, a_Stmt code, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_unary;
    a->s_meta = ae_meta_value;
    a->unary.op = oper;
    a->unary.code = code;
    a->line = lineNum; a->where = posNum;
    a->unary.line = lineNum; a->unary.where = posNum;
    a->unary.self = a;

    return a;
}

a_Exp new_exp_from_cast( a_Type_Decl type, a_Exp exp, uint32_t lineNum, uint32_t posNum, uint32_t castPos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_cast;
    a->s_meta = ae_meta_value;
    a->cast.type = type;
    a->cast.exp = exp;
    a->line = lineNum; a->where = posNum;
    a->cast.line = lineNum; a->cast.where = castPos;
    a->cast.self = a;

    return a;
}

a_Exp new_exp_from_array( a_Exp base, a_Array_Sub indices, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_array;
    a->s_meta = ae_meta_var;
    a->array.base = base;
    a->array.indices = indices;
    a->line = lineNum; a->where = posNum;
    a->array.line = lineNum; a->array.where = posNum;
    a->array.self = a;

    return a;
}

a_Exp new_exp_from_func_call( a_Exp base, a_Exp args, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_func_call;
    a->s_meta = ae_meta_value;
    a->func_call.func = base;
    a->func_call.args = args;
    a->line = lineNum; a->where = posNum;
    a->func_call.line = lineNum; a->func_call.where = posNum;
    a->func_call.self = a;

    return a;
}

a_Exp new_exp_from_member_dot( a_Exp base, c_str xid, uint32_t lineNum, uint32_t posNum, uint32_t memberPos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_dot_member;
    a->s_meta = ae_meta_var;
    a->dot_member.base = base;
    a->dot_member.xid = insert_symbol( xid );
    a->line = lineNum; a->where = posNum;
    a->dot_member.line = lineNum; a->dot_member.where = memberPos;
    a->dot_member.self = a;

    return a;
}

a_Exp new_exp_from_postfix( a_Exp base, ae_Operator op, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_postfix;
    a->s_meta = ae_meta_var;
    a->postfix.exp = base;
    a->postfix.op = op;
    a->line = lineNum; a->where = posNum;
    a->postfix.line = lineNum; a->postfix.where = posNum;
    a->postfix.self = a;

    return a;
}

a_Exp new_exp_from_dur( a_Exp base, a_Exp unit, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_dur;
    a->s_meta = ae_meta_value;
    a->dur.base = base;
    a->dur.unit = unit;
    a->line = lineNum; a->where = posNum;
    a->dur.line = lineNum; a->dur.where = posNum;
    a->dur.self = a;

    return a;
}

a_Exp new_exp_from_id( c_str xid, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_var;
    a->primary.s_type = ae_primary_var;
    a->primary.var = insert_symbol( xid );
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_int( long num, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_num;
    a->primary.num = num;
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_float( double num, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_float;
    a->primary.fnum = num;
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_str( c_str str, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_str;
    a->primary.str = str;
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_char( c_str chr, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_char;
    a->primary.chr = chr;
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_array_lit( a_Array_Sub exp_list, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_array;
    a->primary.array = exp_list;
    a->line = lineNum; a->where = posNum;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_if( a_Exp cond, a_Exp if_exp, a_Exp else_exp, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_if;
    a->s_meta = ( ( if_exp->s_meta == ae_meta_var &&
        else_exp->s_meta == ae_meta_var ) ? ae_meta_var : ae_meta_value );
    a->exp_if.cond = cond;
    a->exp_if.if_exp = if_exp;
    a->exp_if.else_exp = else_exp;
    a->line = lineNum; a->where = posNum;
    a->exp_if.line = lineNum; a->exp_if.where = posNum;
    a->exp_if.self = a;

    return a;
}

a_Exp new_exp_decl_external( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = new_exp_decl( type, var_decl_list, is_static, lineNum, posNum );
    a->decl.is_global = 1;
    EM_error2( posNum, "'external' keyword is deprecated. please use 'global'" );

    return a;
}

a_Exp new_exp_decl_global( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = new_exp_decl( type, var_decl_list, is_static, lineNum, posNum );
    a->decl.is_global = 1;

    return a;
}

a_Exp new_exp_decl( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_decl;
    a->s_meta = ae_meta_var;
    a->decl.type = type;
    a->decl.var_decl_list = var_decl_list;
    a->line = lineNum; a->where = posNum;
    a->decl.is_static = is_static;
    a->decl.line = lineNum; a->decl.where = posNum;
    a->decl.self = a;

    return a;
}

a_Exp new_exp_from_hack( a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_hack;
    a->primary.exp = exp;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->line = lineNum; a->where = posNum;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_complex( a_Complex exp, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_complex;
    a->primary.complex = exp;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->line = lineNum; a->where = posNum;
    a->primary.complex->self = a;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_polar( a_Polar exp, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_polar;
    a->primary.polar = exp;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->line = lineNum; a->where = posNum;
    a->primary.polar->self = a;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_vec( a_Vec exp, uint32_t lineNum, uint32_t posNum ) // ge: added 1.3.5.3
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_vec;
    a->primary.vec = exp;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->line = lineNum; a->where = posNum;
    a->primary.vec->self = a;
    a->primary.self = a;

    return a;
}

a_Exp new_exp_from_nil( uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_nil;
    a->primary.line = lineNum; a->primary.where = posNum;
    a->line = lineNum; a->where = posNum;
    a->primary.self = a;

    return a;
}

a_Var_Decl new_var_decl( c_constr xid, a_Array_Sub array, uint32_t lineNum, uint32_t posNum )
{
    a_Var_Decl a = (a_Var_Decl)checked_malloc( sizeof( struct a_Var_Decl_ ) );
    a->xid = insert_symbol(xid);
    a->array = array;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Var_Decl_List new_var_decl_list( a_Var_Decl var_decl, uint32_t lineNum, uint32_t posNum )
{
    a_Var_Decl_List a = (a_Var_Decl_List)checked_malloc(
        sizeof( struct a_Var_Decl_List_ ) );
    a->var_decl = var_decl;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Var_Decl_List prepend_var_decl_list( a_Var_Decl var_decl, a_Var_Decl_List list, uint32_t lineNum, uint32_t posNum )
{
    a_Var_Decl_List a = new_var_decl_list( var_decl, lineNum, posNum );
    a->next = list;

    return a;
}

a_Type_Decl new_type_decl( a_Id_List type, int ref, uint32_t lineNum, uint32_t posNum )
{
    a_Type_Decl a = (a_Type_Decl)checked_malloc(
        sizeof( struct a_Type_Decl_ ) );
    a->xid = type;
    a->ref = ref;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Type_Decl add_type_decl_array( a_Type_Decl a, a_Array_Sub array, uint32_t lineNum, uint32_t posNum )
{
    assert( a->array == NULL );
    a->array = array;

    return a;
}

a_Arg_List new_arg_list( a_Type_Decl type_decl, a_Var_Decl var_decl, uint32_t lineNum, uint32_t posNum )
{
    a_Arg_List a = (a_Arg_List)checked_malloc(
        sizeof( struct a_Arg_List_ ) );
    a->type_decl = type_decl;
    a->var_decl = var_decl;
    a->next = NULL;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Arg_List prepend_arg_list( a_Type_Decl type_decl, a_Var_Decl var_decl,
                             a_Arg_List arg_list, uint32_t lineNum, uint32_t posNum )
{
    a_Arg_List a = new_arg_list( type_decl, var_decl, lineNum, posNum );
    a->next = arg_list;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Func_Def new_func_def( ae_Keyword func_decl, ae_Keyword static_decl,
                         a_Type_Decl type_decl, c_str name,
                         a_Arg_List arg_list, a_Stmt code, uint32_t lineNum, uint32_t posNum )
{
    a_Func_Def a = (a_Func_Def)checked_malloc(
        sizeof( struct a_Func_Def_ ) );
    a->func_decl = func_decl;
    a->static_decl = static_decl;
    a->type_decl = type_decl;
    a->name = insert_symbol( name );
    a->arg_list = arg_list;
    a->s_type = ae_func_user;
    a->code = code;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Class_Def new_class_def( ae_Keyword class_decl, a_Id_List name,
                           a_Class_Ext ext, a_Class_Body body, uint32_t lineNum, uint32_t posNum )
{
    a_Class_Def a = (a_Class_Def)checked_malloc( sizeof( struct a_Class_Def_ ) );
    a->decl = class_decl;
    a->name = name;
    a->ext = ext;
    a->body = body;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Class_Def new_iface_def( ae_Keyword class_decl, a_Id_List name,
                           a_Class_Ext ext, a_Class_Body body, uint32_t lineNum, uint32_t posNum )
{
    a_Class_Def a = new_class_def( class_decl, name, ext, body, lineNum, posNum );
    a->iface = 1;

    return a;
}

a_Class_Body new_class_body( a_Section section, uint32_t lineNum, uint32_t posNum )
{
    a_Class_Body a = (a_Class_Body)checked_malloc( sizeof( struct a_Class_Body_ ) );
    a->section = section;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Class_Body prepend_class_body( a_Section section, a_Class_Body body, uint32_t lineNum, uint32_t posNum )
{
    a_Class_Body a = new_class_body( section, lineNum, posNum );
    a->next = body;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Class_Ext new_class_ext( a_Id_List extend_id, a_Id_List impl_list, uint32_t lineNum, uint32_t posNum )
{
    a_Class_Ext a = (a_Class_Ext)checked_malloc( sizeof( struct a_Class_Ext_ ) );
    a->extend_id = extend_id;
    a->impl_list = impl_list;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Id_List new_id_list( c_constr xid, uint32_t lineNum, uint32_t posNum /*, YYLTYPE * loc*/ )
{
    a_Id_List a = (a_Id_List)checked_malloc( sizeof( struct a_Id_List_ ) );
    a->xid = insert_symbol( xid );
    a->next = NULL;
    a->line = lineNum; a->where = posNum;

    // debug print
    // if( loc ) fprintf( stderr, "ID: %s %d %d %d %d | pos = %d\n", xid, loc->first_line, loc->first_column, loc->last_line, loc->last_column, posNum );

    return a;
}

a_Id_List prepend_id_list( c_constr xid, a_Id_List list, uint32_t lineNum, uint32_t posNum /*, YYLTYPE * loc */ )
{
    a_Id_List a = new_id_list( xid, lineNum, posNum /*, loc */ );
    a->next = list;
    a->line = lineNum; a->where = posNum;

    return a;
}

void clean_exp( a_Exp exp )
{
    if( !exp ) return;
    clean_exp( exp->next );
    free( exp );
}

a_Array_Sub new_array_sub( a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    a_Array_Sub a = (a_Array_Sub)checked_malloc( sizeof( struct a_Array_Sub_ ) );
    a->exp_list = exp;
    a->depth = 1;
    a->line = lineNum; a->where = posNum;

    // make sure no multi
    /* if( exp && exp->next )
    {
        // primary exp?
        if( is_primary )
        {
            a->exp_list = NULL;
            a->exp_multi = exp;
        }
        else
        {
            a->err_num = 1;            // multi
            a->err_pos = exp->linepos; // set error for type-checker
            goto error;
        }
    } */

    return a;

/* error:
    clean_exp( exp );
    a->exp_list = NULL;
    return a; */
}

a_Array_Sub prepend_array_sub( a_Array_Sub a, a_Exp exp, uint32_t lineNum, uint32_t posNum )
{
    // if already error
    if( a->err_num ) goto error;

    // make sure no multi
    if( exp && exp->next )
    {
        a->err_num = 1;        // multi
        a->err_pos = exp->where; // set error for type-checker
        goto error;
    }

    // empty or not
    if( (exp && !a->exp_list) || (!exp && a->exp_list) )
    {
        a->err_num = 2;   // partial
        a->err_pos = posNum; // set error for type-checker
        goto error;
    }

    // prepend
    if( exp )
    {
        exp->next = a->exp_list;
        a->exp_list = exp;
    }

    // count
    a->depth++;
    return a;

error:
    clean_exp( exp );
    return a;
}

a_Complex new_complex( a_Exp re, uint32_t lineNum, uint32_t posNum )
{
    a_Complex a = (a_Complex)checked_malloc( sizeof( struct a_Complex_ ) );
    a->re = re;
    if( re ) a->im = re->next;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Polar new_polar( a_Exp mod, uint32_t lineNum, uint32_t posNum )
{
    a_Polar a = (a_Polar)checked_malloc( sizeof( struct a_Polar_ ) );
    a->mod = mod;
    if( mod ) a->phase = mod->next;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Vec new_vec( a_Exp e, uint32_t lineNum, uint32_t posNum ) // ge: added 1.3.5.3
{
    a_Vec a = (a_Vec)checked_malloc( sizeof( struct a_Vec_ ) );
    a->args = e;
    while( e ) // count number of dims
    {
        a->numdims++;
        e = e->next;
    }
    a->line = lineNum; a->where = posNum;

    return a;
}

void delete_id_list( a_Id_List x )
{
    if( !x ) return;

    a_Id_List curr = x, next = x->next, temp;

    while( curr )
    {
        temp = curr;
        curr = next;
        next = temp->next;
        free( temp );
    }
}


static const char * op_str[] = {
  "+",
  "-",
  "*",
  "/",
  "==",
  "!=",
  "<",
  "<=",
  ">",
  ">=",
  "&&",
  "||",
  "|",
  "&",
  "<<",
  ">>",
  "%",
  "^",
  "=>",
  "+=>",
  "-=>",
  "*=>",
  "/=>",
  "&=>",
  "|=>",
  "^=>",
  ">>=>",
  "<<=>",
  "%=>",
  "->",
  "++",
  "--",
  "~",
  "!",
  "@=>",
  "=<",
  "spork ~",
  "typeof",
  "sizeof",
  "new"
};


const char * op2str( ae_Operator op )
{
    t_CKINT index = (t_CKINT)op;
    if( index < 0 || index >= (t_CKINT)(sizeof(op_str)/sizeof(char *)) )
        return "[non-existent operator]";

    return op_str[index];
}
