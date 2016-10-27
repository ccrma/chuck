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
// file: chuck_desugar.cpp
// desc: chuck desugar system for syntactic sugar
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
// date: Autumn 2016 - original
//-----------------------------------------------------------------------------

#include "chuck_desugar.h"




//-----------------------------------------------------------------------------
// name: desugar_file()
// desc: desugar a program
//-----------------------------------------------------------------------------
t_CKBOOL desugar_file( Chuck_Context * context )
{
    a_Program parse_tree = context->parse_tree;
    
    while( parse_tree )
    {
        // check if section is a default function
        if( parse_tree->section->s_type == ae_section_default_func )
        {
            if( !desugar_default_func( parse_tree ) ) { return FALSE; }
        }
        else if( parse_tree->section->s_type == ae_section_class )
        {
            // classes can be nested
            if( !desugar_class( parse_tree->section->class_def ) ) { return FALSE; }
        }
        // ... more syntactic sugar goes here
        
        // continue iterating
        parse_tree = parse_tree->next;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: desugar_class()
// desc: desugar a class. recurse for nested classes.
//-----------------------------------------------------------------------------
t_CKBOOL desugar_class( a_Class_Def class_def )
{
    a_Class_Body body = class_def->body;
    
    while( body )
    {
        if( body->section->s_type == ae_section_default_func )
        {
            // found method with default args
            if( !desugar_default_method( body ) ) { return FALSE; }
        }
        else if( body->section->s_type == ae_section_class ) // if body is class
        {
            // found nested class. desugar it
            if( !desugar_class( body->section->class_def ) ) { return FALSE; }
        }
        
        body = body->next;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: append_arg()
// desc: appends the first arg in default_arg_list to end of arg_list
//       after copying the arg_list
//-----------------------------------------------------------------------------
a_Arg_List append_arg( a_Arg_List arg_list, a_Default_Arg_List default_arg, int linepos )
{
    // NOTE: only appends the first arg in default_arg_list
    if( arg_list )
    {
        return prepend_arg_list( arg_list->type_decl, arg_list->var_decl,
                                 // recurse with rest of arg_list
                                 append_arg(arg_list->next, default_arg, linepos),
                                 linepos );
    }
    else
    {
        // base case: new arg list with the first elem of default_arg at end
        return new_arg_list( default_arg->type_decl, default_arg->var_decl,
                             linepos );
    }
}




//-----------------------------------------------------------------------------
// name: arg_names_with_trailing_expr()
// desc: extracts arg_list into a list of ID exps, with a trailing exp
//       (for default args)
//-----------------------------------------------------------------------------
a_Exp arg_names_with_trailing_expr( a_Arg_List arg_list, a_Exp default_exp, int linepos ) {
    // each arg_list has ->var_decl which has ->xid which is type insert_symbol(c_constr) which is type S_Symbol which has ->name (and ->next, wat?)
    
    if( arg_list )
    {
        return prepend_expression(
            new_exp_from_id(
                S_name( arg_list->var_decl->xid ), arg_list->linepos
            ),
            arg_names_with_trailing_expr( arg_list->next, default_exp, linepos ),
            linepos );
    }
    else
    {
        // other base case. arg_list is null to start with. return nothing
        return default_exp;
    }
}



// TODO: Determine whether this will cause a memory leak because I am just
// discarding old parts of the AST without deleting them (and there's no smart pointers D: )
// func_name = S_name( default_func->name );
//-----------------------------------------------------------------------------
// name: new_return_codeblock()
// desc: makes a codeblock with a return statement calling the function
//       with name func_name, and args as the list of ids in id_names followed
//       by default_expr
//-----------------------------------------------------------------------------
a_Stmt new_return_codeblock( c_str func_name, a_Arg_List id_names, a_Exp default_expr, int linepos )
{
    return new_stmt_from_code(
        new_stmt_list(
            new_stmt_from_return(
                new_exp_from_func_call(
                    new_exp_from_id( func_name, linepos ),
                    arg_names_with_trailing_expr(
                        id_names, default_expr, linepos
                    ),
                linepos ),
            linepos ),
        linepos ),
    linepos );
}




//-----------------------------------------------------------------------------
// name: desugar_default_func_helper()
// desc: returns the head of a_Program linked list that includes all the
//       desugared function definitions, with the tail pointing to the
//       passed in a_Program eventual_next
//-----------------------------------------------------------------------------
a_Program desugar_default_func_helper( a_Arg_List current_args,
                                       a_Default_Arg_List current_default_args,
                                       a_Default_Func_Def default_func,
                                       a_Program eventual_next )
{
    int lp = default_func->linepos;
    if( current_default_args )
    {
        return prepend_program(
            new_section_func_def(
                // function definition: a return statement chaining to next func
                new_func_def( default_func->func_decl,
                              default_func->static_decl,
                              default_func->type_decl,
                              S_name( default_func->name ),
                              current_args,
                              new_return_codeblock(
                                  S_name( default_func->name ),
                                  current_args,
                                  current_default_args->exp,
                                  default_func->linepos
                              ),
                              lp ),
            lp ),
            // recurse to next function
            desugar_default_func_helper(
                append_arg( current_args, current_default_args, default_func->linepos ),
                current_default_args->next, default_func, eventual_next
            ),
        lp );
    }
    else
    {
        // base case: no default args left.
        // return new function with original code block
        // section points to "eventual next" to maintain the program linkedlist
        return prepend_program(
            new_section_func_def(
                new_func_def( default_func->func_decl,
                              default_func->static_decl,
                              default_func->type_decl,
                              S_name( default_func->name ),
                              current_args,
                              default_func->code,
                              lp ),
            lp ),
        eventual_next, lp );
    }
}




//-----------------------------------------------------------------------------
// name: desugar_default_func()
// desc: desugar a function with default arguments
//-----------------------------------------------------------------------------
t_CKBOOL desugar_default_func( a_Program parse_tree )
{
    a_Program new_functions = desugar_default_func_helper(
        parse_tree->section->default_func_def->arg_list,
        parse_tree->section->default_func_def->default_arg_list,
        parse_tree->section->default_func_def,
        parse_tree->next
    );
    // copy over
    parse_tree->section = new_functions->section;
    parse_tree->next = new_functions->next;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: desugar_default_method_helper()
// desc: returns the head of a_Class_Body linked list that includes all the
//       desugared method definitions, with the tail pointing to the
//       passed in a_Class_Body eventual_next
//-----------------------------------------------------------------------------
a_Class_Body desugar_default_method_helper( a_Arg_List current_args,
                                       a_Default_Arg_List current_default_args,
                                       a_Default_Func_Def default_func,
                                       a_Class_Body eventual_next )
{
    int lp = default_func->linepos;
    if( current_default_args )
    {
        return prepend_class_body(
            new_section_func_def(
                // function definition: a return statement chaining to next func
                new_func_def( default_func->func_decl,
                              default_func->static_decl,
                              default_func->type_decl,
                              S_name( default_func->name ),
                              current_args,
                              new_return_codeblock(
                                  S_name( default_func->name ),
                                  current_args,
                                  current_default_args->exp,
                                  default_func->linepos
                              ),
                              lp ),
            lp ),
            // recurse to next function
            desugar_default_method_helper(
                append_arg( current_args, current_default_args, default_func->linepos ),
                current_default_args->next, default_func, eventual_next
            ),
        lp );
    }
    else
    {
        // base case: no default args left.
        // return new function with original code block
        // section points to "eventual next" to maintain the program linkedlist
        return prepend_class_body(
            new_section_func_def(
                new_func_def( default_func->func_decl,
                              default_func->static_decl,
                              default_func->type_decl,
                              S_name( default_func->name ),
                              current_args,
                              default_func->code,
                              lp ),
            lp ),
        eventual_next, lp );
    }
}




//-----------------------------------------------------------------------------
// name: desugar_default_method()
// desc: desugar a class method with default arguments
//       (classes and programs store sections in different linked lists)
//-----------------------------------------------------------------------------
t_CKBOOL desugar_default_method( a_Class_Body class_parse_tree )
{
    a_Class_Body new_functions = desugar_default_method_helper(
        class_parse_tree->section->default_func_def->arg_list,
        class_parse_tree->section->default_func_def->default_arg_list,
        class_parse_tree->section->default_func_def,
        class_parse_tree->next
    );
    // copy over
    class_parse_tree->section = new_functions->section;
    class_parse_tree->next = new_functions->next;

    return TRUE;
}
