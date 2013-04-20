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
// file: chuck_absyn.h
// desc: chuck abstract syntax
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_ABSYN_H__
#define __CHUCK_ABSYN_H__

#include "chuck_symbol.h"

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

// pos
typedef int a_Pos;

// enum oper
typedef enum {
    ae_op_plus, ae_op_minus, ae_op_times, ae_op_divide,
    ae_op_eq, ae_op_neq, ae_op_lt, ae_op_le, ae_op_gt,
    ae_op_ge, ae_op_and, ae_op_or, ae_op_s_or, ae_op_s_and,
    ae_op_shift_left, ae_op_shift_right, ae_op_percent,
    ae_op_s_xor, ae_op_chuck, ae_op_plus_chuck, ae_op_minus_chuck,
    ae_op_times_chuck, ae_op_divide_chuck, ae_op_s_and_chuck,
    ae_op_s_or_chuck, ae_op_s_xor_chuck, ae_op_shift_right_chuck,
    ae_op_shift_left_chuck, ae_op_percent_chuck, ae_op_s_chuck,
    ae_op_plusplus, ae_op_minusminus, ae_op_tilda, ae_op_exclamation,
    ae_op_at_chuck, ae_op_unchuck, ae_op_upchuck, ae_op_spork, 
    ae_op_typeof, ae_op_sizeof, ae_op_new, ae_op_arrow_left, ae_op_arrow_right
} ae_Operator;

const char * op2str( ae_Operator op );

// enum key words
typedef enum {
    ae_key_this, ae_key_me, ae_key_func, ae_key_public, ae_key_protected,
    ae_key_private, ae_key_static, ae_key_instance, ae_key_abstract
} ae_Keyword;




//-----------------------------------------------------------------------------
// pointer types
//-----------------------------------------------------------------------------
typedef struct a_Program_ * a_Program;
typedef struct a_Section_ * a_Section;
typedef struct a_Stmt_List_ * a_Stmt_List;
typedef struct a_Class_Def_ * a_Class_Def;
typedef struct a_Func_Def_ * a_Func_Def;
typedef struct a_Code_Segment_ * a_Code_Segment;
typedef struct a_Stmt_ * a_Stmt;
typedef struct a_Exp_ * a_Exp;
typedef struct a_Exp_Chuck_ * a_Exp_Chuck;
typedef struct a_Exp_Binary_ * a_Exp_Binary;
typedef struct a_Exp_Cast_ * a_Exp_Cast;
typedef struct a_Exp_Unary_ * a_Exp_Unary;
typedef struct a_Exp_Postfix_ * a_Exp_Postfix;
typedef struct a_Exp_Primary_ * a_Exp_Primary;
typedef struct a_Exp_Dur_ * a_Exp_Dur;
typedef struct a_Exp_Array_ * a_Exp_Array;
typedef struct a_Exp_Func_Call_ * a_Exp_Func_Call;
typedef struct a_Exp_Dot_Member_ * a_Exp_Dot_Member;
typedef struct a_Exp_If_ * a_Exp_If;
typedef struct a_Exp_Decl_ * a_Exp_Decl;
typedef struct a_Exp_Hack_ * a_Exp_Hack;
typedef struct a_Stmt_Code_ * a_Stmt_Code;
typedef struct a_Stmt_If_ * a_Stmt_If;
typedef struct a_Stmt_While_ * a_Stmt_While;
typedef struct a_Stmt_Until_ * a_Stmt_Until;
typedef struct a_Stmt_For_ * a_Stmt_For;
typedef struct a_Stmt_Loop_ * a_Stmt_Loop;
typedef struct a_Stmt_Switch_ * a_Stmt_Switch;
typedef struct a_Stmt_Break_ * a_Stmt_Break;
typedef struct a_Stmt_Continue_ * a_Stmt_Continue;
typedef struct a_Stmt_Return_ * a_Stmt_Return;
typedef struct a_Stmt_Case_ * a_Stmt_Case;
typedef struct a_Stmt_GotoLabel_ * a_Stmt_GotoLabel;
typedef struct a_Decl_ * a_Decl;
typedef struct a_Var_Decl_ * a_Var_Decl;
typedef struct a_Var_Decl_List_ * a_Var_Decl_List;
typedef struct a_Type_Decl_ * a_Type_Decl;
typedef struct a_Arg_List_ * a_Arg_List;
typedef struct a_Id_List_ * a_Id_List;
typedef struct a_Class_Ext_ * a_Class_Ext;
typedef struct a_Class_Body_ * a_Class_Body;
typedef struct a_Array_Sub_ * a_Array_Sub;
typedef struct a_Complex_ * a_Complex;
typedef struct a_Polar_ * a_Polar;

// forward reference for type
typedef struct Chuck_Type * t_CKTYPE;
typedef struct Chuck_Value * t_CKVALUE; 
typedef struct Chuck_Func * t_CKFUNC;
typedef struct Chuck_Namespace * t_CKNSPC;
typedef struct Chuck_VM_Code * t_CKVMCODE;




//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
a_Program new_program( a_Section section, int pos );
a_Program prepend_program( a_Section section, a_Program program, int pos );
a_Section new_section_stmt( a_Stmt_List stmt_list, int pos );
a_Section new_section_func_def( a_Func_Def func_def, int pos );
a_Section new_section_class_def( a_Class_Def class_def, int pos );
a_Stmt_List new_stmt_list( a_Stmt stmt, int pos );
a_Stmt_List prepend_stmt_list( a_Stmt stmt, a_Stmt_List stmt_list, int pos );
a_Stmt new_stmt_from_expression( a_Exp exp, int pos );
a_Stmt new_stmt_from_code( a_Stmt_List code, int pos );
a_Stmt new_stmt_from_while( a_Exp cond, a_Stmt body, int pos );
a_Stmt new_stmt_from_do_while( a_Exp cond, a_Stmt body, int pos );
a_Stmt new_stmt_from_until( a_Exp cond, a_Stmt body, int pos );
a_Stmt new_stmt_from_do_until( a_Exp cond, a_Stmt body, int pos );
a_Stmt new_stmt_from_for( a_Stmt c1, a_Stmt c2, a_Exp c3, a_Stmt body, int pos );
a_Stmt new_stmt_from_loop( a_Exp cond, a_Stmt body, int pos );
a_Stmt new_stmt_from_if( a_Exp cond, a_Stmt if_body, a_Stmt else_body, int pos );
a_Stmt new_stmt_from_switch( a_Exp exp, int pos );
a_Stmt new_stmt_from_break( int pos );
a_Stmt new_stmt_from_continue( int pos );
a_Stmt new_stmt_from_return( a_Exp exp, int pos );
a_Stmt new_stmt_from_label( c_str xid, int pos );
a_Stmt new_stmt_from_case( a_Exp exp, int pos );
a_Exp prepend_expression( a_Exp exp, a_Exp list, int pos );
a_Exp new_exp_from_binary( a_Exp lhs, ae_Operator oper, a_Exp rhs, int pos );
a_Exp new_exp_from_unary( ae_Operator oper, a_Exp exp, int pos );
a_Exp new_exp_from_unary2( ae_Operator oper, a_Type_Decl type, a_Array_Sub array, int pos );
a_Exp new_exp_from_unary3( ae_Operator oper, a_Stmt code, int pos );
a_Exp new_exp_from_cast( a_Type_Decl type, a_Exp exp, int pos );
a_Exp new_exp_from_array( a_Exp base, a_Array_Sub indices, int pos );
a_Exp new_exp_from_array_lit( a_Array_Sub exp_list, int pos );
a_Exp new_exp_from_func_call( a_Exp base, a_Exp args, int pos );
a_Exp new_exp_from_member_dot( a_Exp base, c_str member, int pos );
a_Exp new_exp_from_postfix( a_Exp base, ae_Operator op, int pos );
a_Exp new_exp_from_dur( a_Exp base, a_Exp unit, int pos );
a_Exp new_exp_from_id( c_str xid, int pos );
a_Exp new_exp_from_int( long num, int pos );
a_Exp new_exp_from_uint( unsigned long num, int pos );
a_Exp new_exp_from_float( double num, int pos );
a_Exp new_exp_from_str( c_str str, int pos );
a_Exp new_exp_from_char( c_str chr, int pos );
a_Exp new_exp_from_if( a_Exp cond, a_Exp lhs, a_Exp rhs, int pos );
a_Exp new_exp_from_complex( a_Complex, int pos );
a_Exp new_exp_from_polar( a_Polar, int pos );
a_Exp new_exp_decl( a_Type_Decl type_decl, a_Var_Decl_List var_decl_list, int is_static, int pos );
a_Exp new_exp_from_hack( a_Exp exp, int pos );
a_Exp new_exp_from_nil( int pos );
a_Var_Decl_List new_var_decl_list( a_Var_Decl var_decl, int pos );
a_Var_Decl_List prepend_var_decl_list( a_Var_Decl var_decl, a_Var_Decl_List list, int pos );
a_Var_Decl new_var_decl( c_constr xid, a_Array_Sub array, int pos );
a_Type_Decl new_type_decl( a_Id_List xid, int ref, int pos );
a_Type_Decl add_type_decl_array( a_Type_Decl type_decl, a_Array_Sub array, int pos );
a_Arg_List new_arg_list( a_Type_Decl type_decl, a_Var_Decl var_decl, int pos );
a_Arg_List prepend_arg_list( a_Type_Decl type_decl, a_Var_Decl var_decl, a_Arg_List arg_list, int pos );
a_Array_Sub new_array_sub( a_Exp exp, int pos );
a_Array_Sub prepend_array_sub( a_Array_Sub array, a_Exp exp, int pos );
a_Complex new_complex( a_Exp re, int pos );
a_Polar new_polar( a_Exp mod, int pos );
a_Class_Def new_class_def( ae_Keyword class_decl, a_Id_List xid, a_Class_Ext ext, a_Class_Body body, int pos );
a_Class_Body new_class_body( a_Section section, int pos );
a_Class_Body prepend_class_body( a_Section section, a_Class_Body body, int pos );
a_Class_Ext new_class_ext( a_Id_List extend_id, a_Id_List impl_list, int pos );
a_Class_Def new_iface_def( ae_Keyword class_decl, a_Id_List xid, a_Class_Ext ext, a_Class_Body body, int pos );
a_Id_List new_id_list( c_constr xid, int pos );
a_Id_List prepend_id_list( c_constr xid, a_Id_List list, int pos );
void clean_exp( a_Exp exp );
a_Func_Def new_func_def( ae_Keyword func_decl, ae_Keyword static_decl,
                         a_Type_Decl type_decl, c_str name,
                         a_Arg_List arg_list, a_Stmt code, int pos );

void delete_id_list( a_Id_List x );




//------------------------------------------------------------------------------
// structs
//------------------------------------------------------------------------------
struct a_Exp_Binary_ { a_Exp lhs; ae_Operator op; a_Exp rhs; t_CKFUNC ck_func; int linepos; a_Exp self; };
struct a_Exp_Cast_ { a_Type_Decl type; a_Exp exp; int linepos; a_Exp self; };
struct a_Exp_Unary_ { ae_Operator op; a_Exp exp; a_Type_Decl type; a_Array_Sub array;
                      a_Stmt code; int linepos; a_Exp self; };
struct a_Exp_Postfix_ { a_Exp exp; ae_Operator op; int linepos; a_Exp self; };
struct a_Exp_Dur_ { a_Exp base; a_Exp unit; int linepos; a_Exp self; };
struct a_Exp_Array_ { a_Exp base; a_Array_Sub indices; int linepos; a_Exp self; };
struct a_Exp_Func_Call_ { a_Exp func; a_Exp args; t_CKTYPE ret_type;
                          t_CKFUNC ck_func; t_CKVMCODE ck_vm_code; int linepos; a_Exp self; };
struct a_Exp_Dot_Member_ { a_Exp base; t_CKTYPE t_base; S_Symbol xid; int linepos; a_Exp self; };
struct a_Exp_If_ { a_Exp cond; a_Exp if_exp; a_Exp else_exp; int linepos; a_Exp self; };
struct a_Exp_Decl_ { a_Type_Decl type; a_Var_Decl_List var_decl_list; int num_var_decls; int is_static; 
                     t_CKTYPE ck_type; int linepos; a_Exp self; };
struct a_Exp_Hack_ { a_Exp exp; int linepos; a_Exp self; };
struct a_Var_Decl_List_ { a_Var_Decl var_decl; a_Var_Decl_List next; int linepos; a_Exp self; };
struct a_Var_Decl_ { S_Symbol xid; a_Var_Decl var_decl; a_Array_Sub array; t_CKVALUE value;
                     void * addr; int linepos; a_Exp self; };
struct a_Type_Decl_ { a_Id_List xid; a_Array_Sub array; int ref; int linepos; /*a_Exp self;*/ };
struct a_Array_Sub_ { t_CKUINT depth; a_Exp exp_list; int linepos; a_Exp self; 
                      int err_num; int err_pos; };
struct a_Arg_List_ { a_Type_Decl type_decl; a_Var_Decl var_decl; t_CKTYPE type;
                     a_Arg_List next; int linepos; a_Exp self; };
struct a_Complex_ { a_Exp re; a_Exp im; int linepos; a_Exp self; };
struct a_Polar_ { a_Exp mod; a_Exp phase; int linepos; a_Exp self; };

// enum primary exp type
typedef enum { ae_primary_var, ae_primary_num, ae_primary_float, 
               ae_primary_str, ae_primary_array, ae_primary_exp,
               ae_primary_hack, ae_primary_complex, ae_primary_polar,
               ae_primary_char, ae_primary_nil
             } ae_Exp_Primary_Type;

struct a_Exp_Primary_
{
    ae_Exp_Primary_Type s_type;
    t_CKVALUE value;
    
    union
    {
        S_Symbol var;
        t_CKINT num;
        t_CKFLOAT fnum;
        c_str str;
        c_str chr;
        a_Array_Sub array;
        a_Exp exp;
        a_Complex complex;
        a_Polar polar;
    };

    int linepos;
    a_Exp self;
};

// enum exp type
typedef enum { ae_exp_binary = 0, ae_exp_unary, ae_exp_cast, ae_exp_postfix,
               ae_exp_dur, ae_exp_primary, ae_exp_array, ae_exp_func_call,
               ae_exp_dot_member, ae_exp_if, ae_exp_decl
             } ae_Exp_Type;
// type meta data
typedef enum { ae_meta_value = 0, ae_meta_var } ae_Exp_Meta;

struct a_Exp_
{
    ae_Exp_Type s_type;
    ae_Exp_Meta s_meta;
    t_CKTYPE type;
    t_CKNSPC owner;
    a_Exp next;

    t_CKUINT group_size;
    t_CKTYPE cast_to;
    t_CKUINT emit_var; // 1 = emit var, 2 = emit var and value
    
    union
    {
        struct a_Exp_Binary_ binary;
        struct a_Exp_Unary_ unary;
        struct a_Exp_Cast_ cast;
        struct a_Exp_Postfix_ postfix;
        struct a_Exp_Dur_ dur;
        struct a_Exp_Primary_ primary;
        struct a_Exp_Array_ array;
        struct a_Exp_Func_Call_ func_call;
        struct a_Exp_Dot_Member_ dot_member;
        struct a_Exp_If_ exp_if;
        struct a_Exp_Decl_ decl;
    };

    int linepos;
};

struct a_Stmt_While_ { int is_do; a_Exp cond; a_Stmt body; int linepos; a_Stmt self; };
struct a_Stmt_Until_ { int is_do; a_Exp cond; a_Stmt body; int linepos; a_Stmt self; };
struct a_Stmt_For_ { a_Stmt c1; a_Stmt c2; a_Exp c3; a_Stmt body; int linepos; a_Stmt self; };
struct a_Stmt_Loop_ { a_Exp cond; a_Stmt body; int linepos; a_Stmt self; };
struct a_Stmt_Code_ { a_Stmt_List stmt_list; int linepos; a_Stmt self; };
struct a_Stmt_If_ { a_Exp cond; a_Stmt if_body; a_Stmt else_body; int linepos; a_Stmt self; };
struct a_Stmt_Switch_ { a_Exp val; int linepos; a_Stmt self; };
struct a_Stmt_Break_ { int linepos; a_Stmt self; };
struct a_Stmt_Continue_ { int linepos; a_Stmt self; };
struct a_Stmt_Return_ { a_Exp val; int linepos; a_Stmt self; };
struct a_Stmt_Case_ { a_Exp exp; int linepos; a_Stmt self; };
struct a_Stmt_GotoLabel_ { S_Symbol name; int linepos; a_Stmt self; };

// enum values for stmt type
typedef enum { ae_stmt_exp, ae_stmt_while, ae_stmt_until, ae_stmt_for, ae_stmt_loop,
               ae_stmt_if, ae_stmt_code, ae_stmt_switch, ae_stmt_break, 
               ae_stmt_continue, ae_stmt_return, ae_stmt_case, ae_stmt_gotolabel
             } ae_Stmt_Type;
 
struct a_Stmt_
{
    ae_Stmt_Type s_type;
    int skip;
    
    union
    {
        a_Exp stmt_exp;
        struct a_Stmt_Code_ stmt_code;
        struct a_Stmt_While_ stmt_while;
        struct a_Stmt_Until_ stmt_until;
        struct a_Stmt_Loop_ stmt_loop;
        struct a_Stmt_For_ stmt_for;
        struct a_Stmt_If_ stmt_if;
        struct a_Stmt_Switch_ stmt_switch;
        struct a_Stmt_Break_ stmt_break;
        struct a_Stmt_Continue_ stmt_continue;
        struct a_Stmt_Return_ stmt_return;
        struct a_Stmt_Case_ stmt_case;
        struct a_Stmt_GotoLabel_ stmt_gotolabel;
    };

    int linepos;
};

struct a_Stmt_List_ { a_Stmt stmt; a_Stmt_List next; int linepos; };
struct a_Class_Def_ { ae_Keyword decl; a_Id_List name; a_Class_Ext ext; 
                      a_Class_Body body; t_CKTYPE type; int iface; t_CKNSPC home;
                      int linepos; };
struct a_Class_Ext_ { a_Id_List extend_id; a_Id_List impl_list; int linepos; };
struct a_Class_Body_ { a_Section section; a_Class_Body next; int linepos; };
struct a_Id_List_ { S_Symbol xid; a_Id_List next; int linepos; };

typedef enum { ae_func_user, ae_func_builtin } ae_Func_Type;
// struct t_Func_User_ { int linepos; };
// typedef unsigned int (* builtin_func_ptr)( unsigned int arg );
// struct t_Func_BuiltIn_ { builtin_func_ptr func_ptr; int linepos; };
struct a_Func_Def_ {
    ae_Keyword func_decl;
    ae_Keyword static_decl; 
    a_Type_Decl type_decl;
    t_CKTYPE ret_type;
    S_Symbol name; 
    a_Arg_List arg_list;
    a_Stmt code;
    t_CKFUNC ck_func;
    unsigned int global;
    unsigned int s_type;
    unsigned int stack_depth;
    void * dl_func_ptr;  // should be not NULL iff s_type == ae_func_builtin
    int linepos;
};

// enum values for section types
typedef enum { ae_section_stmt, ae_section_func, ae_section_class
             } ae_Section_Type;

struct a_Section_
{
    ae_Section_Type s_type;
    
    union
    {
        a_Stmt_List stmt_list;
        a_Class_Def class_def;
        a_Func_Def func_def;
    };

    int linepos;
};

struct a_Program_ { a_Section section; a_Program next; int linepos; };


#if defined(_cplusplus) || defined(__cplusplus)
}
#endif



#endif
