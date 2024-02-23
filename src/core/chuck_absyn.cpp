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
// file: chuck_absyn.cpp
// desc: chuck abstract syntax
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include "chuck_absyn.h"
#include "chuck_errmsg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string> // 1.5.1.5 for string concat


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

a_Stmt new_stmt_from_foreach( a_Exp iter, a_Exp array, a_Stmt body, uint32_t lineNum, uint32_t posNum )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_foreach;
    a->stmt_foreach.theIter = iter;
    a->stmt_foreach.theArray = array;
    a->stmt_foreach.body = body;
    a->line = lineNum; a->where = posNum;
    a->stmt_foreach.line = lineNum; a->stmt_foreach.where = posNum;
    a->stmt_foreach.self = a;

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
                           int ctor_invoked, a_Exp ctor_args, a_Array_Sub array,
                           uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_unary;
    a->s_meta = ae_meta_value;
    a->unary.op = oper;
    a->unary.type = type;
    a->unary.ctor.invoked = ctor_invoked;
    a->unary.ctor.args = ctor_args;
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

a_Exp new_exp_from_int( t_CKINT num, uint32_t lineNum, uint32_t posNum )
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

a_Exp new_exp_from_float( t_CKFLOAT num, uint32_t lineNum, uint32_t posNum )
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
    a->primary.str = str; // no strdup( str ); <-- str should have been allocated in alloc_str()
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
    a->primary.chr = chr; // no strdup( chr ); <-- chr should have been allocated in alloc_str()
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
    a->primary.array->self = a; // 1.5.2.0 (ge & nshaheed) added
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

a_Exp new_exp_decl_external( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, int is_const, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = new_exp_decl( type, var_decl_list, is_static, is_const, lineNum, posNum );
    a->decl.is_global = 1;
    EM_error2( posNum, "'external' keyword is deprecated. please use 'global'" );

    return a;
}

a_Exp new_exp_decl_global( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, int is_const, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = new_exp_decl( type, var_decl_list, is_static, is_const, lineNum, posNum );
    a->decl.is_global = 1;

    return a;
}

a_Exp new_exp_decl( a_Type_Decl type, a_Var_Decl_List var_decl_list, int is_static, int is_const, uint32_t lineNum, uint32_t posNum )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_decl;
    a->s_meta = ae_meta_var;
    a->decl.type = type;
    a->decl.var_decl_list = var_decl_list;
    a->line = lineNum; a->where = posNum;
    a->decl.is_static = is_static;
    a->decl.is_const = is_const; // 1.5.1.3
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

a_Var_Decl new_var_decl( c_constr xid, int ctor_invoked, a_Exp ctor_args, a_Array_Sub array, uint32_t lineNum, uint32_t posNum )
{
    a_Var_Decl a = (a_Var_Decl)checked_malloc( sizeof( struct a_Var_Decl_ ) );
    a->xid = insert_symbol(xid);
    a->ctor.invoked = ctor_invoked;
    a->ctor.args = ctor_args;
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
                         a_Arg_List arg_list, a_Stmt code,
                         uint32_t is_from_ast, uint32_t lineNum, uint32_t posNum )
{
    a_Func_Def a = (a_Func_Def)checked_malloc(
        sizeof( struct a_Func_Def_ ) );
    a->func_decl = func_decl;
    a->static_decl = static_decl;
    // substitute if NULL | 1.5.2.0 (ge) for constructors
    a->type_decl = type_decl ? type_decl : new_type_decl(new_id_list("void",0,0),0,0,0);
    a->name = insert_symbol( name );
    a->arg_list = arg_list;
    a->s_type = ae_func_user;
    a->code = code;
    a->ast_owned = is_from_ast != 0; // 1.5.0.5 (ge) added
    a->line = lineNum; a->where = posNum;

    return a;
}

// @operator overload | 1.5.1.5
a_Func_Def new_op_overload( ae_Keyword func_decl, ae_Keyword static_decl,
                            a_Type_Decl type_decl, ae_Operator oper,
                            a_Arg_List arg_list, a_Stmt code,
                            uint32_t is_from_ast, uint32_t overload_post,
                            uint32_t lineNum, uint32_t posNum, uint32_t operPos )
{
    a_Func_Def a = (a_Func_Def)checked_malloc(
        sizeof( struct a_Func_Def_ ) );
    a->func_decl = func_decl;
    a->static_decl = static_decl;
    a->type_decl = type_decl;
    // construct op overload function name
    std::string s( "@operator" ); s += op2str(oper);
    a->name = insert_symbol( s.c_str() );
    a->op2overload = oper;
    a->arg_list = arg_list;
    a->s_type = ae_func_user;
    a->code = code;
    a->ast_owned = is_from_ast != 0; // 1.5.0.5 (ge) added
    a->overload_post = overload_post; // 1.5.1.5 (ge) added
    a->line = lineNum; a->where = posNum; a->operWhere = operPos;

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
    delete_exp( exp );
    return a;
}

a_Complex new_complex( a_Exp re, uint32_t lineNum, uint32_t posNum )
{
    a_Complex a = (a_Complex)checked_malloc( sizeof( struct a_Complex_ ) );
    a->re = re;
    // NOTE: if this ever changes, make sure to also update delete_complex | 1.5.1.0
    if( re ) a->im = re->next;
    a->line = lineNum; a->where = posNum;

    return a;
}

a_Polar new_polar( a_Exp mod, uint32_t lineNum, uint32_t posNum )
{
    a_Polar a = (a_Polar)checked_malloc( sizeof( struct a_Polar_ ) );
    a->mod = mod;
    // NOTE: if this ever changes, make sure to also update delete_polar | 1.5.1.0
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




// operator strings
static const char * op_str[] = {
  "[no_op]",
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
  "++",
  "--",
  "~",
  "!",
  "@=>",
  "=<",
  "=^",
  "=v",
  "=",
  "$",
  "@@",
  "::",
  "spork ~",
  "typeof",
  "sizeof",
  "new",
  "<-",
  "->",
  "<--",
  "-->",
  ">--",
  "--<"
};
//-----------------------------------------------------------------------------
// name: op2str()
// desc: operator enumeration to string
//-----------------------------------------------------------------------------
const char * op2str( ae_Operator op )
{
    t_CKINT index = (t_CKINT)op;
    if( index < 0 || index >= ae_op_count )
        return "[non-existent operator]";

    return op_str[index];
}




//-----------------------------------------------------------------------------
// name: str2op()
// desc: string to operator enumeration
//-----------------------------------------------------------------------------
ae_Operator str2op( const char * str )
{
    std::string s(str);
    for( t_CKUINT i = 1; i < ae_op_count; i++ )
        if( s == op_str[i] ) return (ae_Operator)i;

    return ae_op_none;
}




// function pointer kind names | 1.5.2.0
static const char * fpkind_str[] = {
    "[unknown]",
    "ctor",
    "dtor",
    "mfun",
    "sfun",
    "gfun",
    "addr"
};
//-----------------------------------------------------------------------------
// name: fpkind2str() | 1.5.2.0
// desc: convert fp kind by enum to str
//-----------------------------------------------------------------------------
const char * fpkind2str( ae_FuncPointerKind kind )
{
    t_CKINT index = (t_CKINT)kind;
    if( index < 0 || index >= ae_fp_count )
        return "[non-existent func kind]";

    return fpkind_str[index];
}




//-----------------------------------------------------------------------------
// AST cleanup function (these are mostly called by chuck compiler)
//-----------------------------------------------------------------------------
void delete_program( a_Program program )
{
    // pointer
    a_Program next = NULL;

    // log
    EM_log( CK_LOG_FINEST, "deleting AST root PROGRAM [%p]...", (void *)program );

    // iterate linearly instead of recursing to avoid stack overflow
    while( program )
    {
        // delete the section in this program node
        delete_section( program->section );
        // get the next node before we delete this one
        next = program->next;
        // delete this one
        CK_SAFE_FREE( program );
        // move to the next one
        program = next;
    }
}

void delete_section( a_Section section )
{
    if( !section ) return;
    EM_log( CK_LOG_FINEST, "deleting section [%p]...", (void *)section );

    // check section type
    switch( section->s_type )
    {
    case ae_section_stmt: delete_stmt_list( section->stmt_list ); break;
    case ae_section_class: delete_class_def( section->class_def ); break;
    case ae_section_func: delete_func_def( section->func_def ); break;
    }
    CK_SAFE_FREE( section );
}

// delete stmt list
void delete_stmt_list( a_Stmt_List list )
{
    // log
    EM_log( CK_LOG_FINEST, "deleting stmt list [%p]...", (void *)list );

    // pointers
    a_Stmt_List next = NULL;

    // iterate without recursion to avoid stack overflow
    while( list )
    {
        // delete the stmt at this node
        delete_stmt( list->stmt );
        // get the next node before we delete this one
        next = list->next;
        // delete this one
        CK_SAFE_FREE( list );
        // move to the next
        list = next;
    }
}

// delete class_def
void delete_class_def( a_Class_Def def )
{
    if( !def ) return;

    // TODO: release reference count for def->type
    // TODO: release reference count for def->home
    // TODO: what to do about iface?
    EM_log( CK_LOG_FINEST, "deleting class def [%p]...", (void *)def );

    delete_id_list( def->name );
    delete_class_ext( def->ext );
    delete_class_body( def->body );
    CK_SAFE_FREE( def );
}

void delete_class_body( a_Class_Body body )
{
    if( !body ) return;
    EM_log( CK_LOG_FINEST, "deleting class body [%p]...", (void *)body );

    delete_class_body( body->next );
    delete_section( body->section );
    CK_SAFE_FREE( body );
}

void delete_class_ext( a_Class_Ext ext )
{
    if( !ext ) return;
    EM_log( CK_LOG_FINEST, "deleting class ext [%p]...", (void *)ext );

    delete_id_list( ext->extend_id );
    delete_id_list( ext->impl_list );
    CK_SAFE_FREE( ext );
}

// delete func def
void delete_func_def( a_Func_Def def )
{
    if( !def ) return;

    // safety check
    if( def->ast_owned && def->vm_refs > 0 )
    {
        EM_error2( 0, "(internal error): cannot delete Func_Def %s with %ld VM references...", S_name(def->name), def->vm_refs );
        return;
    }

    EM_log( CK_LOG_FINEST, "deleting func def [%p]...", (void *)def );

    // TODO: release reference def->ret_type
    // TODO: do something with Symbol def->name?
    // TODO: release reference def->ck_func;

    delete_type_decl( def->type_decl );
    delete_arg_list( def->arg_list );
    delete_stmt( def->code );
    CK_SAFE_FREE( def );
}

void delete_iface_def( a_Class_Def def )
{
    delete_class_def( def );
}

// delete stmt
void delete_stmt( a_Stmt stmt )
{
    if( !stmt ) return;
    EM_log( CK_LOG_FINEST, "deleting stmt [%p]...", (void *)stmt );

    // unlike a_Exp, stmt are part of a list and don't have a ->next

    // check statement type
    switch( stmt->s_type )
    {
    case ae_stmt_exp:
        delete_exp( stmt->stmt_exp );
        break;
    case ae_stmt_while:
        delete_stmt_from_while( stmt );
        break;
    case ae_stmt_until:
        delete_stmt_from_until( stmt );
        break;
    case ae_stmt_for:
        delete_stmt_from_for( stmt );
        break;
    case ae_stmt_foreach:
        delete_stmt_from_foreach( stmt );
        break;
    case ae_stmt_loop:
        delete_stmt_from_loop( stmt );
        break;
    case ae_stmt_if:
        delete_stmt_from_if( stmt );
        break;
    case ae_stmt_code:
        delete_stmt_from_code( stmt );
        break;
    case ae_stmt_switch:
        delete_stmt_from_switch( stmt );
        break;
    case ae_stmt_break:
        delete_stmt_from_break( stmt );
        break;
    case ae_stmt_continue:
        delete_stmt_from_continue( stmt );
        break;
    case ae_stmt_return:
        delete_stmt_from_return( stmt );
        break;
    case ae_stmt_case:
        // TODO?
        break;
    case ae_stmt_gotolabel:
        delete_stmt_from_label( stmt );
        break;
    }

    CK_SAFE_FREE( stmt );
}

void delete_stmt_from_code( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (code)...", (void *)stmt );
    delete_stmt_list( stmt->stmt_code.stmt_list );
}

void delete_stmt_from_while( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (while)...", (void *)stmt );
    delete_exp( stmt->stmt_while.cond );
    delete_stmt( stmt->stmt_while.body );
}

void delete_stmt_from_until( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (until)...", (void *)stmt );
    delete_stmt( stmt->stmt_until.body );
}

void delete_stmt_from_for( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (for)...", (void *)stmt );
    delete_stmt( stmt->stmt_for.c1 );
    delete_stmt( stmt->stmt_for.c2 );
    delete_exp( stmt->stmt_for.c3 );
    delete_stmt( stmt->stmt_for.body );
}

void delete_stmt_from_foreach( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (foreach)...", (void *)stmt );
    delete_exp( stmt->stmt_foreach.theIter );
    delete_exp( stmt->stmt_foreach.theArray );
    delete_stmt( stmt->stmt_foreach.body );
}

void delete_stmt_from_loop( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (loop)...", (void *)stmt );
    delete_exp( stmt->stmt_loop.cond );
    delete_stmt( stmt->stmt_loop.body );
}

void delete_stmt_from_if( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (if)...", (void *)stmt );
    delete_exp( stmt->stmt_if.cond );
    delete_stmt( stmt->stmt_if.if_body );
    delete_stmt( stmt->stmt_if.else_body );
}

void delete_stmt_from_return( a_Stmt stmt )
{
    EM_log( CK_LOG_FINEST, "deleting stmt %p (return)...", (void *)stmt );
    delete_exp( stmt->stmt_return.val );
}

void delete_stmt_from_switch( a_Stmt stmt )
{
    // TODO
}

void delete_stmt_from_break( a_Stmt stmt )
{
    // nothing to do
}

void delete_stmt_from_continue( a_Stmt stmt )
{
    // nothing to do
}

void delete_stmt_from_label( a_Stmt stmt )
{
    // TODO: someting with S_Symbol stmt->gotolabel.name
}

void delete_exp_from_primary( a_Exp_Primary_ & p )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary)..." );

    switch( p.s_type )
    {
    case ae_primary_str:
        delete_exp_from_str( &p );
        break;
    case ae_primary_char:
        delete_exp_from_char( &p );
        break;
    case ae_primary_var:
        delete_exp_from_id( &p );
        break;
    case ae_primary_array:
        delete_exp_from_array_lit( &p );
        break;
    case ae_primary_exp:
        delete_exp( p.exp );
        break;
    case ae_primary_complex:
        delete_exp_from_complex( &p );
        break;
    case ae_primary_polar:
        delete_exp_from_polar( &p );
        break;
    case ae_primary_vec:
        delete_exp_from_vec( &p );
        break;
    case ae_primary_hack:
        delete_exp_from_hack( &p );
        break;

    // have nothing to delete
    case ae_primary_num:
        EM_log( CK_LOG_FINEST, "deleting exp (primary NUM) [%d]...", (long)p.num );
        break;
    case ae_primary_float:
        EM_log( CK_LOG_FINEST, "deleting exp (primary FLOAT) [%f]...", p.fnum );
        break;
    case ae_primary_nil:
        EM_log( CK_LOG_FINEST, "deleting exp (primary NIL)..." );
        break;
    }
}

// delete an exp
void delete_exp_contents( a_Exp e )
{
    // chekc
    if( !e ) return;
    // log
    EM_log( CK_LOG_FINEST, "deleting exp [%p] [next: %p]...", (void *)e, (void *)(e->next) );

    // check expression type
    switch( e->s_type )
    {
    case ae_exp_binary:
        delete_exp_from_binary( e );
        break;
    case ae_exp_unary:
        delete_exp_from_unary( e );
        break;
    case ae_exp_cast:
        delete_exp_from_cast( e );
        break;
    case ae_exp_postfix:
        delete_exp_from_postfix( e );
        break;
    case ae_exp_dur:
        delete_exp_from_dur( e );
        break;
    case ae_exp_primary:
        delete_exp_from_primary( e->primary );
        break;
    case ae_exp_array:
        delete_exp_from_array( e );
        break;
    case ae_exp_func_call:
        delete_exp_from_func_call( e );
        break;
    case ae_exp_dot_member:
        delete_exp_from_member_dot( e );
        break;
    case ae_exp_if:
        delete_exp_from_if( e );
        break;
    case ae_exp_decl:
        delete_exp_decl( e );
        break;
    }
}

// delete an exp
void delete_exp( a_Exp e )
{
    // pointer
    a_Exp next = NULL;

    // iterate instead of recurse to avoid stack overflow
    while( e )
    {
        // TODO: release reference type
        // TODO: release reference owner

        // delete content in this exp
        delete_exp_contents( e );
        // get next exp before we delete this one
        next = e->next;
        // delete this one
        CK_SAFE_FREE( e );
        // move to the next one
        e = next;
    }
}

void delete_exp_from_binary( a_Exp e )
{
    // TODO: release reference ck_func
    EM_log( CK_LOG_FINEST, "deleting exp (binary) [%p]...", (void *)e );

    delete_exp( e->binary.lhs );
    delete_exp( e->binary.rhs );
}

void delete_exp_from_unary( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (unary) [%p]...", (void *)e );

    delete_exp( e->unary.exp );
    delete_type_decl( e->unary.type );
    delete_array_sub( e->unary.array );
}

void delete_exp_from_cast( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (cast) [%p]...", (void *)e );

    delete_type_decl( e->cast.type );
    delete_exp( e->cast.exp );
}

void delete_exp_from_array( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (arry) [%p]...", (void *)e );

    delete_exp( e->array.base );
    delete_array_sub( e->array.indices );
}

void delete_exp_from_func_call( a_Exp e )
{
    // TODO: release reference ret_type
    // TODO: release reference ck_func
    // TODO: release reference ck_vm_code
    EM_log( CK_LOG_FINEST, "deleting exp (func call) [%p]...", (void *)e );

    delete_exp( e->func_call.func );
    delete_exp( e->func_call.args );
}

void delete_exp_from_member_dot( a_Exp e )
{
    // TODO: release reference t_base
    // TODO: do something about Symbol xid?
    EM_log( CK_LOG_FINEST, "deleting exp (member access) [%p]...", (void *)e );

    delete_exp( e->dot_member.base );
}

void delete_exp_from_postfix( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (postfix) [%p]...", (void *)e );
    delete_exp( e->postfix.exp );
}

void delete_exp_from_dur( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (dur) [%p]...", (void *)e );

    delete_exp( e->dur.base );
    delete_exp( e->dur.unit );
}

void delete_exp_from_if( a_Exp e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (if) [%p]...", (void *)e );

    delete_exp( e->exp_if.cond );
    delete_exp( e->exp_if.if_exp );
    delete_exp( e->exp_if.else_exp );
}

void delete_exp_decl( a_Exp e )
{
    // TODO: release reference ck_type
    EM_log( CK_LOG_FINEST, "deleting exp (decl) [%p]...", (void *)e );

    delete_type_decl( e->decl.type );
    delete_var_decl_list( e->decl.var_decl_list );
}

void delete_exp_from_id( a_Exp_Primary e )
{
    // TODO: do we need to anything with the Symbol?
    EM_log( CK_LOG_FINEST, "deleting exp (primary ID '%s') [%p]...", S_name(e->var), (void *)e );
}

void delete_exp_from_str( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary str) [%p]...", (void *)e );

    CK_SAFE_FREE( e->str );
}

void delete_exp_from_char( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary char) [%p]...", (void *)e );

    CK_SAFE_FREE( e->chr );
}

void delete_exp_from_array_lit( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary array lit) [%p]...", (void *)e );

    delete_array_sub( e->array );
}

void delete_exp_from_complex( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary complex) [%p]...", (void *)e );

    delete_complex( e->complex );
}

void delete_exp_from_polar( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary polar) [%p]...", (void *)e );

    delete_polar( e->polar );
}

void delete_exp_from_vec( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary vec) [%p]...", (void *)e );

    delete_exp( e->vec->args );
}

void delete_exp_from_hack( a_Exp_Primary e )
{
    EM_log( CK_LOG_FINEST, "deleting exp (primary <<< hack >>>) [%p]...", (void *)e );

    delete_exp( e->exp );
}

void delete_var_decl_list( a_Var_Decl_List list )
{
    if( !list ) return;
    delete_var_decl_list( list->next );
    EM_log( CK_LOG_FINEST, "deleting var decl list [%p] [next: %p]...", (void *)list, (void *)(list->next) );
    delete_var_decl( list->var_decl );
    CK_SAFE_FREE( list );
}

void delete_var_decl( a_Var_Decl decl )
{
    if( !decl ) return;
    EM_log( CK_LOG_FINEST, "deleting var decl [%p]...", (void *)decl );

    // TODO: do anything with Symbol xid?
    // TODO: release reference value
    // TODO: release reference ck_type

    delete_array_sub( decl->array );
    CK_SAFE_FREE( decl );
}

void delete_type_decl( a_Type_Decl decl )
{
    if( !decl ) return;
    EM_log( CK_LOG_FINEST, "deleting type decl [%p]...", (void *)decl );

    delete_id_list( decl->xid );
    delete_array_sub( decl->array );
    CK_SAFE_FREE( decl );
}

void delete_arg_list( a_Arg_List list )
{
    // TODO: release reference type
    if( !list ) return;
    delete_arg_list( list->next );
    EM_log( CK_LOG_FINEST, "deleting arg list [%p] [next: %p]...", (void *)list, (void *)(list->next) );
    delete_type_decl( list->type_decl );
    CK_SAFE_FREE( list );
}

void delete_array_sub( a_Array_Sub sub )
{
    if( !sub ) return;
    EM_log( CK_LOG_FINEST, "deleting type decl [%p]...", (void *)sub );
    delete_exp( sub->exp_list );
    CK_SAFE_FREE( sub );
}

void delete_complex( a_Complex c )
{
    EM_log( CK_LOG_FINEST, "deleting complex [%p]...", (void *)c );
    delete_exp( c->re );
    // do not delete c->im, since it's just c->re->next
    // delete_exp( c->im );
    CK_SAFE_FREE( c );
}

void delete_polar( a_Polar p )
{
    EM_log( CK_LOG_FINEST, "deleting polar [%p]...", (void *)p );
    delete_exp( p->mod );
    // do not delete p->phase, since it's just c->re->next
    // delete_exp( p->phase );
    CK_SAFE_FREE( p );
}

void delete_vec( a_Vec v )
{
    EM_log( CK_LOG_FINEST, "deleting vec [%p]...", (void *)v );
    delete_exp( v->args );
    CK_SAFE_FREE( v );
}

void delete_id_list( a_Id_List list )
{
    // pointer
    a_Id_List next = NULL;

    // iterate instead of recursive to avoid stack overflow
    while( list )
    {
        // TODO: what to do about Symbol xid?
        // log
        EM_log( CK_LOG_FINEST, "deleting id list [%p] [next: %p]...", (void *)list );
        // get next before we delete this
        next = list->next;
        // delete this one
        CK_SAFE_FREE( list );
        // move to the next one
        list = next;
    }
}
