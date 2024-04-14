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
// file: chuck_parse.cpp
// desc: chuck parser interface (using flex and bison)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original in chuck_main.cpp
//       Autumn 2005 - this file
//-----------------------------------------------------------------------------
#include "chuck_parse.h"
#include "chuck_errmsg.h"
#include "util_string.h"
#include "util_platforms.h"

#include <string.h>
#include <sys/stat.h>
#if defined(__PLATFORM_WINDOWS__)
  #include "dirent_win32.h"
#endif

#include <string>
using namespace std;

// global
static std::string g_filename;
// file to parse
static FILE * g_fd2parse = NULL;
// whether to auto close
static t_CKBOOL g_fd2autoClose = FALSE;
// separate file to close
static FILE * g_fd2close = NULL;
// clean up function
static void fd2parse_cleanup();

// external from lexer
extern "C" {
    extern FILE * yyin;
}




//-----------------------------------------------------------------------------
// name: cleanup_AST()
// desc: clean up the abstract syntax tree
//-----------------------------------------------------------------------------
static void cleanup_AST()
{
    // check
    if( !g_program ) return;

    // log
    EM_log( CK_LOG_FINE, "cleaning up abstract syntax tree..." );
    // do it
    delete_program( g_program );
    // reset to NULL
    g_program = NULL;
}




//-----------------------------------------------------------------------------
// name: chuck_parse()
// desc: INPUT: chuck code (either from file or actual code) to be parsed
//       OUTPUT: abstract syntax tree representation of the code
//-----------------------------------------------------------------------------
t_CKBOOL chuck_parse( const std::string & fname, const std::string & codeLiteral )
{
    // return value
    t_CKBOOL ret = FALSE;
    // file descriptor | normally NULL unless g_fd2parse is set
    FILE * fd = g_fd2parse;
    // our own lexer/parser buffer
    YY_BUFFER_STATE yyCodeBuffer = NULL;
    // where the code is coming from
    CompileFileSource source;

    // check for conflict
    if( fd && codeLiteral != "" )
    {
        CK_FPRINTF_STDERR( "[chuck](parser): (internal) code and FILE descriptor both present!\n" );
        CK_FPRINTF_STDERR( "[chuck](parser):  |- ignoring FILE descriptor...\n" );
    }

    // copy it
    g_filename = fname;

    // if actual code was passed in
    if( codeLiteral != "" )
    {
        // clean lexer
        yyrestart( NULL );
        // set to initial condition | 1.5.2.4 (ge) added
        yyinitial();
        // load string (yy_scan_string will copy the C string)
        yyCodeBuffer = yy_scan_string( codeLiteral.c_str() );
        // if could load
        if( !yyCodeBuffer ) goto cleanup;
        // set source
        source.setCode( codeLiteral.c_str() );
    }
    else
    {
        // test it
        if( !fd )
        {
            // open, could potentially change g_filename
            fd = ck_openFileAutoExt( g_filename, ".ck" );
            // check file open result
            if( !fd ) // if couldn't open
            { g_filename = fname; } // revert filename
            else if( ck_isdir(g_filename) ) // check for directory; if so, clean up
            { EM_error2( 0, "cannot parse file: '%s' is a directory", mini(g_filename.c_str()) ); goto cleanup; }
        }

        // if still none
        if( !fd ) { EM_error2( 0, "no such file: '%s'", mini(fname.c_str()) ); goto cleanup; }
        // set to beginning0
        else fseek( fd, 0, SEEK_SET );

        // reset yyin to fd
        yyrestart( fd );
        // set to initial condition | 1.5.0.5 (ge) added
        yyinitial();

        // check
        if( yyin == NULL ) goto cleanup;

        // set source
        source.setFile( fd );
    }

    // start error message for new filename
    EM_start_filename( g_filename.c_str() );
    // set current input source
    EM_setCurrentFileSource( source );

    // ensure abstract syntax tree is clean | 1.5.0.5 (ge) added, finally
    cleanup_AST();

    // parse
    if( !(yyparse() == 0) ) goto cleanup;

    // flag success
    ret = TRUE;

cleanup:

    // defer some clean up to reset_parse(), since later passes
    // could use current file source / file descriptor; copy
    g_fd2close = fd;

    // flush
    // yyflush();

    // set to NULL | 1.5.0.5 (ge) added to reset yyin
    yyin = NULL;

    // clean up lexer buffer, if we used one
    if( yyCodeBuffer )
    { yy_delete_buffer( yyCodeBuffer ); yyCodeBuffer = NULL; }

    return ret;
}




//------------------------------------------------------------------------------
// name: fd2parse_cleanup()
// desc: clean up fd2parse
//------------------------------------------------------------------------------
void fd2parse_cleanup()
{
    // check
    if( g_fd2parse && g_fd2autoClose )
    {
        // log
        EM_log( CK_LOG_FINEST, "fd2parse_clean() closing FILE descriptor [0x%x]...", (t_CKUINT)g_fd2parse );
        fclose( g_fd2parse );
    }

    // reset
    g_fd2parse = NULL;
    g_fd2autoClose = FALSE;
}




//------------------------------------------------------------------------------
// name: fd2parse_set()
// desc: special FILE input mode | added 1.5.0.5 (ge)
// set an already open FILE descriptor `fd` for one time use
// by the next call to go(), which will use `fd` as the input
// to the parser (NOTE in any invocation of go(), `codeLiteral`
// and `fd` should not both be non-empty, otherwise a warning
// will be output and the `codeLiteral` will take precedence
// and the `fd` will be cleaned up and skipped
// MEMORY: if `autoClose` is true, the compiler will automatically
// call fclose() on `fd` on the next call to go(), regardless of
// the aforementioned conflict with `codeLiteral`
//------------------------------------------------------------------------------
void fd2parse_set( FILE * fd, t_CKBOOL autoClose )
{
    // clean up
    fd2parse_cleanup();

    // copy
    g_fd2parse = fd;
    g_fd2autoClose = autoClose;
}




//------------------------------------------------------------------------------
// name: reset_parse()
// desc: reset parse after a parse
//------------------------------------------------------------------------------
void reset_parse( )
{
    // reset the current input source
    EM_cleanupCurrentFileSource();

    // clean up the file descriptor
    if( g_fd2close )
    {
        // check fd2parse (could be the same)
        if( g_fd2parse ) { fd2parse_cleanup(); }
        else { fclose( g_fd2close ); }
        // zero out either way
        g_fd2close = NULL;
    }

    // empty file name
    EM_reset_filename();
    // clear current ChucK pointer; no dangling references
    EM_set_current_chuck( NULL );

    // clean up the AST
    cleanup_AST();
}




//-----------------------------------------------------------------------------
// name: parseLine()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL SyntaxQuery::parseLine( const std::string & line, SyntaxTokenList & tokens )
{
    // clear the token list
    tokens.howmany = 0;

    return TRUE;
}





//-----------------------------------------------------------------------------
// absyn to string functions
//-----------------------------------------------------------------------------
#include "chuck_type.h"

// expression to string conversion
string absyn_exp2str( a_Exp exp, t_CKBOOL iterate = true );
string absyn_binary2str( a_Exp_Binary binary );
string absyn_unary2str( a_Exp_Unary unary );
string absyn_op2str( ae_Operator op );
string absyn_cast2str( a_Exp_Cast cast );
string absyn_postfix2str( a_Exp_Postfix postfix );
string absyn_dur2str( a_Exp_Dur dur );
string absyn_primary2str( a_Exp_Primary primary );
string absyn_array2str( a_Exp_Array array );
string absyn_func_call2str( a_Exp_Func_Call func_call );
string absyn_dot_member2str( a_Exp_Dot_Member dot_member );
string absyn_exp_if2str( a_Exp_If exp_if );
string absyn_decl2str( a_Exp_Decl decl );

// statement to string conversion
string absyn_stmt2str( a_Stmt stmt );
string absyn_if2str( a_Stmt_If stmt_if );
string absyn_for2str( a_Stmt_For stmt_for );
string absyn_foreach2str( a_Stmt_ForEach stmt_foreach );
string absyn_while2str( a_Stmt_While stmt_while );
string absyn_until2str( a_Stmt_Until stmt_until );
string absyn_loop2str( a_Stmt_Loop stmt_loop );
string absyn_break2str( a_Stmt_Break stmt_break );
string absyn_continue2str( a_Stmt_Continue stmt_continue );
string absyn_return2str( a_Stmt_Return stmt_return );
string absyn_code2str( a_Stmt_Code stmt_code );




//-----------------------------------------------------------------------------
// name: absyn_stmt2str()
// desc: convert abstract syntax stmt to string
//-----------------------------------------------------------------------------
std::string absyn2str( a_Stmt stmt )
{
    return absyn_stmt2str( stmt );
}




//-----------------------------------------------------------------------------
// name: absyn_stmt2str()
// desc: convert abstract syntax stmt to string
//-----------------------------------------------------------------------------
string absyn_stmt2str( a_Stmt stmt )
{
    // check
    if( !stmt ) return "";

    // str to be constructed
    string str = "";

    // loop over statements, check by type
    switch( stmt->s_type )
    {
        case ae_stmt_exp:  // expression statement
            str = absyn_exp2str( stmt->stmt_exp );
            break;

        case ae_stmt_if:  // if statement
            str = absyn_if2str( &stmt->stmt_if );
            break;

        case ae_stmt_for:  // for statement
            str = absyn_for2str( &stmt->stmt_for );
            break;

        case ae_stmt_foreach:  // for statement
            str = absyn_foreach2str( &stmt->stmt_foreach );
            break;

        case ae_stmt_while:  // while statement
            str = absyn_while2str( &stmt->stmt_while );
            break;

        case ae_stmt_until:  // until statement
            str = absyn_until2str( &stmt->stmt_until );
            break;

        case ae_stmt_loop:  // loop statement
            str = absyn_loop2str( &stmt->stmt_loop );
            break;

        case ae_stmt_break:  // break statement
            str = absyn_break2str( &stmt->stmt_break );
            break;

        case ae_stmt_continue:  // continue statement
            str = absyn_continue2str( &stmt->stmt_continue );
            break;

        case ae_stmt_return:  // return statement
            str = absyn_return2str( &stmt->stmt_return );
            break;

        case ae_stmt_code:  // code segment
            str = absyn_code2str( &stmt->stmt_code );
            break;

        // not implemented
        case ae_stmt_switch: // switch statement
        case ae_stmt_case: // case statement
        case ae_stmt_gotolabel: // goto label statement
        default: // bad
            break;
    }

    // return the constructed str
    return str;
}




//-----------------------------------------------------------------------------
// name: absyn2str()
// desc: convert abstract syntax exp to string
//-----------------------------------------------------------------------------
string absyn2str( a_Exp exp, t_CKBOOL appendSemicolon )
{
    return absyn_exp2str( exp ) + (appendSemicolon ? ";" : "" );
}




//-----------------------------------------------------------------------------
// name: name_demangle()
// desc: demangle chuck function names
//       e.g., input: "version@0@Machine" output: "version"
//-----------------------------------------------------------------------------
string name_demangle( const string & name )
{
    // find the first @
    size_t pos = name.find_first_of('@');
    // if found, substring
    return ( pos != string::npos ) ? name.substr(0,pos) : name;
}




//-----------------------------------------------------------------------------
// name: absyn_exp2str()
// desc: convert abstract syntax exp to string
//-----------------------------------------------------------------------------
string absyn_exp2str( a_Exp exp, t_CKBOOL iterate )
{
    // check
    if( !exp ) return "";

    // the string to be constructed
    string str = "";

    // loop over
    while( exp )
    {
        switch( exp->s_type )
        {
        case ae_exp_binary:
            str += absyn_binary2str( &exp->binary );
            break;

        case ae_exp_unary:
            str += absyn_unary2str( &exp->unary );
            break;

        case ae_exp_cast:
            str += absyn_cast2str( &exp->cast );
            break;

        case ae_exp_postfix:
            str += absyn_postfix2str( &exp->postfix );
            break;

        case ae_exp_dur:
            str += absyn_dur2str( &exp->dur );
            break;

        case ae_exp_primary:
                str += absyn_primary2str( &exp->primary );
            break;

        case ae_exp_array:
                str += absyn_array2str( &exp->array );
            break;

        case ae_exp_func_call:
                str += absyn_func_call2str( &exp->func_call );
            break;

        case ae_exp_dot_member:
                str += absyn_dot_member2str( &exp->dot_member );
            break;

        case ae_exp_if:
                str += absyn_exp_if2str( &exp->exp_if );
            break;

        case ae_exp_decl:
                str += absyn_decl2str( &exp->decl );
            break;

        default:
            return "";
        }

        // implicit cast
        if( exp->cast_to != NULL )
            break;

        // break
        if( !iterate ) break;

        // next
        exp = exp->next;
        // if more
        if( exp ) str += ",";
    }

    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_op2str()
// desc: convert abstract syntax operator to string
//-----------------------------------------------------------------------------
string absyn_op2str( ae_Operator op )
{
    return op2str( op );
}




//-----------------------------------------------------------------------------
// name: absyn_binary2str()
// desc: convert abstract syntax binary to string
//-----------------------------------------------------------------------------
string absyn_binary2str( a_Exp_Binary binary )
{
    string lhs = absyn_exp2str(binary->lhs);
    string op = absyn_op2str(binary->op);
    string rhs = absyn_exp2str(binary->rhs);

    // use paren?
    bool paren = false;
    // spacing?
    bool spacing = false;

    switch(binary->op)
    {
        case ae_op_plus_chuck:
        case ae_op_minus_chuck:
        case ae_op_times_chuck:
        case ae_op_divide_chuck:
        case ae_op_s_or_chuck:
        case ae_op_s_and_chuck:
        case ae_op_shift_left_chuck:
        case ae_op_shift_right_chuck:
        case ae_op_percent_chuck:
        case ae_op_s_xor_chuck:
        case ae_op_chuck:
        case ae_op_unchuck:
        case ae_op_upchuck:
        case ae_op_downchuck:
        case ae_op_at_chuck:
        case ae_op_eq:
        case ae_op_neq:
        case ae_op_lt:
        case ae_op_gt:
        case ae_op_le:
        case ae_op_ge:
        case ae_op_and:
        case ae_op_or:
            paren = false;
            spacing = true;
            break;
        default:
            paren = true;
            spacing = false;
            break;
    }

    // (lhs OP rhs)
    string str = string(paren?"(":"") + lhs + (spacing?" ":"") + op + (spacing?" ":"") + rhs + (paren?")":"");
    // return
    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_unary2str()
// desc: convert abstract syntax unary to string
//-----------------------------------------------------------------------------
string absyn_unary2str( a_Exp_Unary unary )
{
    string s;

    // check
    switch( unary->op )
    {
        case ae_op_new:
            s = absyn_op2str(unary->op) + " " + unary->self->type->name();
            if( unary->ctor.invoked ) s += "(" + absyn_exp2str(unary->ctor.args) + ")";
            break;
        default:
            s = absyn_op2str(unary->op) + " " + absyn_exp2str(unary->exp);
            break;
    }

    return s;
}




//-----------------------------------------------------------------------------
// name: absyn_cast2str()
// desc: convert abstract syntax cast to string
//-----------------------------------------------------------------------------
string absyn_cast2str( a_Exp_Cast cast )
{
    Chuck_Type * to = cast->self->type;
    return absyn_exp2str(cast->exp) + "$" + to->base_name;
}




//-----------------------------------------------------------------------------
// name: absyn_cast2str()
// desc: convert abstract syntax cast to string
//-----------------------------------------------------------------------------
string absyn_postfix2str( a_Exp_Postfix postfix )
{
    return absyn_exp2str( postfix->exp ) + absyn_op2str( postfix->op );
}




//-----------------------------------------------------------------------------
// name: absyn_dur2str()
// desc: convert abstract syntax dur to string
//-----------------------------------------------------------------------------
string absyn_dur2str( a_Exp_Dur dur )
{
    return absyn_exp2str(dur->base) + "::" + absyn_exp2str(dur->unit);
}




//-----------------------------------------------------------------------------
// name: absyn_primary2str()
// desc: convert abstract syntax primary to string
//-----------------------------------------------------------------------------
string absyn_primary2str( a_Exp_Primary primary )
{
    string str;

    // find out exp
    switch( primary->s_type )
    {
        case ae_primary_var: str = S_name(primary->var); break;
        case ae_primary_num: str = to_string(primary->num); break;
        case ae_primary_float: str = to_string(primary->fnum); break;
        case ae_primary_str: str = string("\"") + primary->str + "\""; break;
        case ae_primary_char: str = string("'") + primary->chr + "'"; break;
        case ae_primary_array: str = "["+absyn_exp2str(primary->array->exp_list)+"]"; break;
        case ae_primary_complex: str = "#("+absyn_exp2str(primary->complex->re)+","+absyn_exp2str(primary->complex->im)+")"; break;
        case ae_primary_polar: str = "%("+absyn_exp2str(primary->polar->mod)+","+absyn_exp2str(primary->polar->phase)+")"; break;
        case ae_primary_vec: str = "@("+absyn_exp2str(primary->vec->args,TRUE)+")"; break;
        case ae_primary_exp: str = absyn_exp2str(primary->exp); break;
        case ae_primary_hack: str = "<<< " + absyn_exp2str(primary->exp) + " >>>"; break;
        case ae_primary_nil: str = "(void)"; break;
        default: str ="[PRIMARY]"; break;
    }

    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_array2str()
// desc: convert abstract syntax array to string
//-----------------------------------------------------------------------------
string absyn_array2str( a_Exp_Array array )
{
    string str;
    a_Array_Sub sub = NULL;

    // get the sub
    sub = array->indices;
    // expressions
    a_Exp e = sub->exp_list;

    // base of the array
    str = absyn_exp2str(array->base);

    // loop over
    while( e )
    {
        // each index
        str += "[" + absyn_exp2str(e,false) + "]";
        // advance
        e = e->next;
    }

    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_func_call2str()
// desc: convert abstract syntax func_call to string
//-----------------------------------------------------------------------------
string absyn_func_call2str( a_Exp_Func_Call func_call )
{
    string str;
    // function name
    str = name_demangle( absyn_exp2str( func_call->func ) );
    // args
    str += "(" + absyn_exp2str( func_call->args ) + ")";
    // done
    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_dot_member2str()
// desc: convert abstract syntax dot_member to string
//-----------------------------------------------------------------------------
string absyn_dot_member2str( a_Exp_Dot_Member dot_member )
{
    // return
    return absyn_exp2str(dot_member->base) + "." + name_demangle(S_name(dot_member->xid));
}




//-----------------------------------------------------------------------------
// name: absyn_exp_if2str()
// desc: convert abstract syntax exp_if to string
//-----------------------------------------------------------------------------
string absyn_exp_if2str( a_Exp_If exp_if )
{
    return "("+absyn_exp2str(exp_if->cond) + "?" + absyn_exp2str(exp_if->if_exp)
           + ":" + absyn_exp2str(exp_if->else_exp)+")";
}




//-----------------------------------------------------------------------------
// name: absyn_decl2str()
// desc: convert abstract syntax decl to string
//-----------------------------------------------------------------------------
string absyn_decl2str( a_Exp_Decl decl )
{
    a_Var_Decl_List list = decl->var_decl_list;
    string str;
    a_Exp e = NULL;

    if( decl->is_global ) str += "global ";
    else if( decl->is_static ) str += "static ";

    while( list )
    {
        // type
        str += list->var_decl->value->type->base_name + " " + (decl->type->ref ? "@ " : "") + list->var_decl->value->name;
        // constructor?
        if( list->var_decl->ctor.invoked )
        {
            str += "(" + absyn_exp2str(list->var_decl->ctor.args) + ")";
        }
        // array?
        if( list->var_decl->array )
        {
            e = list->var_decl->array->exp_list;
            if( e )
            {
                // loop over
                while( e )
                {
                    // each index
                    str += "[" + absyn_exp2str(e,false) + "]";
                    // advance
                    e = e->next;
                }
            }
            else
            {
                for( t_CKINT i = 0; i < list->var_decl->value->type->array_depth; i++ )
                {
                    str += "[]";
                }
            }
        }

        list = list->next;
        if( list ) str += ",";
    }

    return str;
}




//-----------------------------------------------------------------------------
// name: absyn_if2str()
// desc: convert abstract syntax if to string
//-----------------------------------------------------------------------------
string absyn_if2str( a_Stmt_If stmt_if )
{
    return "if( " + absyn_exp2str( stmt_if->cond ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_for2str()
// desc: convert abstract syntax for to string
//-----------------------------------------------------------------------------
string absyn_for2str( a_Stmt_For stmt_for )
{
    return "for( " + absyn_stmt2str( stmt_for->c1 ) + "; "
                   + absyn_stmt2str( stmt_for->c2 ) + "; "
                   + absyn_exp2str( stmt_for->c3 ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_foreach2str()
// desc: convert abstract syntax for-each to string
//-----------------------------------------------------------------------------
string absyn_foreach2str( a_Stmt_ForEach stmt_foreach )
{
    return "for( " + absyn_exp2str( stmt_foreach->theIter ) + " : "
                   + absyn_exp2str( stmt_foreach->theArray ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_while2str()
// desc: convert abstract syntax while to string
//-----------------------------------------------------------------------------
string absyn_while2str( a_Stmt_While stmt_while )
{
    return "while( " + absyn_exp2str( stmt_while->cond ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_until2str()
// desc: convert abstract syntax until to string
//-----------------------------------------------------------------------------
string absyn_until2str( a_Stmt_Until stmt_until )
{
    return "until( " + absyn_exp2str( stmt_until->cond ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_loop2str()
// desc: convert abstract syntax loop to string
//-----------------------------------------------------------------------------
string absyn_loop2str( a_Stmt_Loop stmt_loop )
{
    return "repeat( " + absyn_exp2str( stmt_loop->cond ) + " )";
}




//-----------------------------------------------------------------------------
// name: absyn_break2str()
// desc: convert abstract syntax break to string
//-----------------------------------------------------------------------------
string absyn_break2str( a_Stmt_Break stmt_break )
{
    return "break;";
}




//-----------------------------------------------------------------------------
// name: absyn_continue2str()
// desc: convert abstract syntax continue to string
//-----------------------------------------------------------------------------
string absyn_continue2str( a_Stmt_Continue stmt_continue )
{
    return "continue;";
}




//-----------------------------------------------------------------------------
// name: absyn_return2str()
// desc: convert abstract syntax return to string
//-----------------------------------------------------------------------------
string absyn_return2str( a_Stmt_Return stmt_return )
{
    return "return " + absyn_exp2str( stmt_return->val ) + ";";
}




//-----------------------------------------------------------------------------
// name: absyn_stmt2str()
// desc: convert abstract syntax decl to string
//-----------------------------------------------------------------------------
string absyn_code2str( a_Stmt_Code stmt_code )
{
    return "";
}
