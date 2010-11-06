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
// file: chuck_absyn.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "chuck_absyn.h"
#include "chuck_utils.h"

a_Program new_program( a_Section section, int pos )
{
    a_Program a = (a_Program)checked_malloc( sizeof( struct a_Program_ ) );
    a->section = section;
    a->linepos = pos;

    return a;
}

a_Program prepend_program( a_Section section, a_Program program, int pos )
{
    a_Program a = new_program( section, pos );
    a->next = program;
    a->linepos = pos;
    
    return a;
}

a_Section new_section_stmt( a_Stmt_List list, int pos )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_ ) );
    a->s_type = ae_section_stmt;
    a->stmt_list = list;
    a->linepos = pos;

    return a;
}

a_Section new_section_func_def( a_Func_Def func_def, int pos )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_) );
    a->s_type = ae_section_func;
    a->func_def = func_def;
    a->linepos = pos;
    
    return a;
}

a_Section new_section_class_def( a_Class_Def class_def, int pos )
{
    a_Section a = (a_Section)checked_malloc( sizeof( struct a_Section_) );
    a->s_type = ae_section_class;
    a->class_def = class_def;
    a->linepos = pos;
    
    return a;
}

a_Stmt_List new_stmt_list( a_Stmt stmt, int pos )
{
    a_Stmt_List a = (a_Stmt_List)checked_malloc( sizeof( struct a_Stmt_List_ ) );
    a->stmt = stmt;
    a->next = NULL;
    a->linepos = pos;

    return a;
}

a_Stmt_List prepend_stmt_list( a_Stmt stmt, a_Stmt_List stmt_list, int pos )
{
    a_Stmt_List a = new_stmt_list( stmt, pos );
    a->next = stmt_list;
    a->linepos = pos;

    return a;
}

a_Stmt new_stmt_from_expression( a_Exp exp, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_exp;
    a->stmt_exp = exp;
    a->linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_code( a_Stmt_List stmt_list, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_code;
    a->stmt_code.stmt_list = stmt_list;
    a->linepos = pos;
    a->stmt_code.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_if( a_Exp cond, a_Stmt if_body, a_Stmt else_body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_if;
    a->stmt_if.cond = cond;
    a->stmt_if.if_body = if_body;
    a->stmt_if.else_body = else_body;
    a->linepos = pos;
    a->stmt_if.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_while( a_Exp cond, a_Stmt body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_while;
    a->stmt_while.is_do = 0;
    a->stmt_while.cond = cond;
    a->stmt_while.body = body;
    a->linepos = pos;
    a->stmt_while.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_do_while( a_Exp cond, a_Stmt body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_while;
    a->stmt_while.is_do = 1;
    a->stmt_while.cond = cond;
    a->stmt_while.body = body;
    a->linepos = pos;
    a->stmt_while.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_until( a_Exp cond, a_Stmt body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_until;
    a->stmt_until.is_do = 0;
    a->stmt_until.cond = cond;
    a->stmt_until.body = body;
    a->linepos = pos;
    a->stmt_until.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_do_until( a_Exp cond, a_Stmt body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_until;
    a->stmt_until.is_do = 1;
    a->stmt_until.cond = cond;
    a->stmt_until.body = body;
    a->linepos = pos;
    a->stmt_until.linepos = pos;

    return a;
}

a_Stmt new_stmt_from_for( a_Stmt c1, a_Stmt c2, a_Exp c3, a_Stmt body, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_for;
    a->stmt_for.c1 = c1;
    a->stmt_for.c2 = c2;
    a->stmt_for.c3 = c3;
    a->stmt_for.body = body;
    a->linepos = pos;
    a->stmt_for.linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_switch( a_Exp val, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_switch;
    a->stmt_switch.val = val;
    a->linepos = pos;
    a->stmt_switch.linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_break( int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_break;
    a->linepos = pos;

    return a;
}

a_Stmt new_stmt_from_continue( int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_continue;
    a->linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_return( a_Exp exp, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_return;
    a->stmt_return.val = exp;
    a->linepos = pos;
    a->stmt_return.linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_label( c_str id, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_gotolabel;
    a->stmt_gotolabel.name = insert_symbol( id );
    a->linepos = pos;
    a->stmt_gotolabel.linepos = pos;
    
    return a;
}

a_Stmt new_stmt_from_case( a_Exp exp, int pos )
{
    a_Stmt a = (a_Stmt)checked_malloc( sizeof( struct a_Stmt_ ) );
    a->s_type = ae_stmt_case;
    a->stmt_case.exp = exp;
    a->linepos = pos;
    a->stmt_case.linepos = pos;
    
    return a;    
}

a_Exp prepend_expression( a_Exp exp, a_Exp list, int pos )
{
    exp->next = list;
    return exp;
}

a_Exp new_exp_from_binary( a_Exp lhs, ae_Operator oper, a_Exp rhs, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_binary;
    a->s_meta = ae_meta_value;
    a->binary.lhs = lhs;
    a->binary.op = oper;
    a->binary.rhs = rhs;
    a->linepos = pos;
    a->binary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_unary( ae_Operator oper, a_Exp exp, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_unary;
    a->s_meta = exp->s_meta;
    a->unary.op = oper;
    a->unary.exp = exp;
    a->linepos = pos;
    a->unary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_cast( c_str type, a_Exp exp, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_cast;
    a->s_meta = ae_meta_value;
    a->cast.type = insert_symbol( type );
    a->cast.exp = exp;
    a->linepos = pos;
    a->cast.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_array( a_Exp base, a_Exp index, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_array;
    a->s_meta = ae_meta_var;
    a->array.base = base;
    a->array.index = index;
    a->linepos = pos;
    a->array.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_func_call( a_Exp base, a_Exp args, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_func_call;
    a->s_meta = ae_meta_value;
    a->func_call.func = base;
    a->func_call.args = args;
    a->linepos = pos;
    a->func_call.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_member_dot( a_Exp base, c_str id, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_dot_member;
    a->s_meta = ae_meta_var;
    a->dot_member.base = base;
    a->dot_member.id = insert_symbol( id );
    a->linepos = pos;
    a->dot_member.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_postfix( a_Exp base, ae_Operator op, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_postfix;
    a->s_meta = ae_meta_var;
    a->postfix.exp = base;
    a->postfix.op = op;
    a->linepos = pos;
    a->postfix.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_dur( a_Exp base, a_Exp unit, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_dur;
    a->s_meta = ae_meta_value;
    a->dur.base = base;
    a->dur.unit = unit;
    a->linepos = pos;
    a->dur.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_id( c_str id, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_var;
    a->primary.s_type = ae_primary_var;
    a->primary.var = insert_symbol( id );
    a->linepos = pos;
    a->primary.linepos = pos;
    
    return a;    
}

a_Exp new_exp_from_int( long num, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_num;
    a->primary.num = num;
    a->linepos = pos;
    a->primary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_uint( unsigned long num, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_uint;
    a->primary.num2 = num;
    a->linepos = pos;
    a->primary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_float( double num, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_float;
    a->primary.fnum = num;
    a->linepos = pos;
    a->primary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_str( c_str str, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_primary;
    a->s_meta = ae_meta_value;
    a->primary.s_type = ae_primary_str;
    a->primary.str = str;
    a->linepos = pos;
    a->primary.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_if( a_Exp cond, a_Exp if_exp, a_Exp else_exp, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_if;
    a->s_meta = ( ( if_exp->s_meta == ae_meta_var && 
        else_exp->s_meta == ae_meta_var ) ? ae_meta_var : ae_meta_value );
    a->exp_if.cond = cond;
    a->exp_if.if_exp = if_exp;
    a->exp_if.else_exp = else_exp;
    a->linepos = pos;
    a->exp_if.linepos = pos;
    
    return a;
}

a_Exp new_exp_decl( c_str type, a_Var_Decl_List var_decl_list, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_decl;
    a->s_meta = ae_meta_var;
    a->decl.type = insert_symbol( type );
    a->decl.var_decl_list = var_decl_list;
    a->linepos = pos;
    a->decl.linepos = pos;
    
    return a;
}

a_Exp new_exp_from_namespace( c_str name, int pos )
{
    a_Exp a = (a_Exp)checked_malloc( sizeof( struct a_Exp_ ) );
    a->s_type = ae_exp_namespace;
    a->s_meta = ae_meta_value;
    a->name_space.name = insert_symbol( name );
    a->name_space.linepos = pos;
    a->linepos = pos;

    return a;
}

a_Var_Decl new_var_decl( c_str name, int pos )
{
    a_Var_Decl a = (a_Var_Decl)checked_malloc( sizeof( struct a_Var_Decl_ ) );
    a->id = insert_symbol( name );
    a->linepos = pos;
    
    return a;
}

a_Var_Decl new_var_decl2( a_Var_Decl var_decl, int isarray, a_Exp exp, int pos )
{
    a_Var_Decl a = (a_Var_Decl)checked_malloc( sizeof( struct a_Var_Decl_ ) );
    a->var_decl = var_decl;
    a->isarray = isarray;
    a->exp = exp;
    a->linepos = pos;
    
    return a;
}

a_Var_Decl_List new_var_decl_list( a_Var_Decl var_decl, int pos )
{
    a_Var_Decl_List a = (a_Var_Decl_List)checked_malloc( 
        sizeof( struct a_Var_Decl_List_ ) );
    a->var_decl = var_decl;
    a->linepos = pos;
    
    return a;
}

a_Var_Decl_List prepend_var_decl_list( a_Var_Decl var_decl, a_Var_Decl_List list, int pos )
{
    a_Var_Decl_List a = new_var_decl_list( var_decl, pos );
    a->next = list;
    
    return a;
}

a_Type_Decl new_type_decl( c_str type, int pos )
{
    a_Type_Decl a = (a_Type_Decl)checked_malloc(
        sizeof( struct a_Type_Decl_ ) );
    a->id = insert_symbol( type );
    a->array = 0;
    a->linepos = pos;

    return a;
}

a_Type_Decl new_type_decl_array( a_Type_Decl decl, int pos )
{
    decl->array++;
    return decl;
}

a_Arg_List new_arg_list( a_Type_Decl type_decl, c_str name, int pos )
{
    a_Arg_List a = (a_Arg_List)checked_malloc(
        sizeof( struct a_Arg_List_ ) );
    a->type_decl = type_decl;
    a->id = insert_symbol( name );
    a->next = NULL;
    a->linepos = pos;

    return a;
}

a_Arg_List prepend_arg_list( a_Type_Decl type_decl, c_str name, a_Arg_List arg_list, int pos )
{
    a_Arg_List a = new_arg_list( type_decl, name, pos );
    a->next = arg_list;
    a->linepos = pos;

    return a;
}

a_Func_Def new_func_def( ae_Keyword func_decl, a_Type_Decl type_decl, c_str name,
                         a_Arg_List arg_list, a_Stmt code, int pos )
{
    a_Func_Def a = (a_Func_Def)checked_malloc(
        sizeof( struct a_Func_Def_ ) );
    a->func_decl = func_decl;
    a->type_decl = type_decl;
    a->name = insert_symbol( name );
    a->arg_list = arg_list;
    a->s_type = ae_func_user;
    a->code = code;
    a->linepos = pos;

    return a;
}

a_Class_Def new_class_def( c_str name, a_Class_Ext ext, a_Class_Body body, int pos )
{
    a_Class_Def a = (a_Class_Def)checked_malloc( sizeof( struct a_Class_Def_ ) );
    a->name = insert_symbol( name );
    a->ext = ext;
    a->body = body;
    a->linepos = pos;

    return a;
}

a_Class_Body new_class_body( a_Section section, int pos )
{
    a_Class_Body a = (a_Class_Body)checked_malloc( sizeof( struct a_Class_Body_ ) );
    a->section = section;
    a->linepos = pos;

    return a;
}

a_Class_Body prepend_class_body( a_Section section, a_Class_Body body, int pos )
{
    a_Class_Body a = new_class_body( section, pos );
    a->next = body;
    a->linepos = pos;

    return a;
}

a_Class_Ext new_class_ext( c_str extend_id, a_Id_List impl_list, int pos )
{
    a_Class_Ext a = (a_Class_Ext)checked_malloc( sizeof( struct a_Class_Ext_ ) );
    a->extend_id = insert_symbol( extend_id );
    a->impl_list = impl_list;
    a->linepos = pos;

    return a;
}

a_Id_List new_id_list( c_str id, int pos )
{
    a_Id_List a = (a_Id_List)checked_malloc( sizeof( struct a_Id_List_ ) );
    a->id = insert_symbol( id );
    a->next = NULL;
    a->linepos = pos;

    return a;
}

a_Id_List prepend_id_list( c_str id, a_Id_List list, int pos )
{
    a_Id_List a = new_id_list( id, pos );
    a->next = list;
    a->linepos = pos;

    return a;
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
  "=<"
};


const char * op2str( ae_Operator op )
{
    return op_str[op];
}
