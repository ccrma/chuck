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
// file: chuck_parse.cpp
// desc: chuck parser interface (using flex and bison)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original in chuck_main.cpp
//       Autumn 2005 - this file
//-----------------------------------------------------------------------------
#include "chuck_parse.h"
#include "chuck_errmsg.h"
#include <string.h>

#ifdef __ANDROID__
#include "util_platforms.h"
#endif

#ifdef __PLATFORM_WIN32__
#ifndef __CHUNREAL_ENGINE__
  #include <windows.h> // for win32_tmpfile()
#else
  // 1.5.0.0 (ge) | #chunreal
  // unreal engine on windows disallows including windows.h
  #include "Windows/MinWindows.h"
#endif // #ifndef __CHUNREAL_ENGINE__
#endif // #ifdef __PLATFORM_WIN32__

using namespace std;

// max path len
static const t_CKUINT MAX_FILENAME_LEN = 2048;
// global
static char g_filename[MAX_FILENAME_LEN+1] = "";

// external
extern "C" {
    extern FILE *yyin;
}




//-----------------------------------------------------------------------------
// name: open_cat_ck()
// desc: ...
//-----------------------------------------------------------------------------
FILE * open_cat_ck( c_str fname )
{
#ifdef __ANDROID__
    if( strncmp(fname, "jar:", strlen("jar:")) == 0 )
    {
        int fd = 0;
        if( !ChuckAndroid::copyJARURLFileToTemporary(fname, &fd) )
        {
            EM_error2( 0, "unable to download from JAR URL: %s", fname );
            return NULL;
        }
        return fdopen(fd, "rb");
    }
#endif // __ANDROID__

    FILE * fd = fopen(fname, "rb");
    if( !fd )
    {
        if( !strstr( fname, ".ck" ) && !strstr( fname, ".CK" ) )
        {
            strcat( fname, ".ck" );
            fd = fopen( fname, "rb" );
        }
    }
    return fd;
}




//-----------------------------------------------------------------------------
// name: win32_tmpfile()
// desc: replacement for broken tmpfile() on Windows Vista + 7
//-----------------------------------------------------------------------------
#ifdef __PLATFORM_WIN32__
FILE *win32_tmpfile()
{
    char tmp_path[MAX_PATH];
    char file_path[MAX_PATH];
    FILE * file = NULL;

#ifndef __CHUNREAL_ENGINE__
    if( GetTempPath(256, tmp_path) == 0 )
#else
    // 1.5.0.0 (ge) | #chunreal explicit call ASCII version
    if( GetTempPathA(256, tmp_path) == 0 )
#endif
        return NULL;

#ifndef __CHUNREAL_ENGINE__
    if( GetTempFileName(tmp_path, "mA", 0, file_path) == 0 )
#else
    // 1.5.0.0 (ge) | #chunreal explicit call ASCII version
    if( GetTempFileNameA(tmp_path, "mA", 0, file_path) == 0 )
#endif
        return NULL;

    file = fopen( file_path, "wb+D" );

    return file;
}
#endif // #ifdef __PLATFORM_WIN32__




//-----------------------------------------------------------------------------
// name: android_tmpfile()
// desc: replacement for broken tmpfile() on Android
//-----------------------------------------------------------------------------
#ifdef __ANDROID__

FILE * android_tmpfile()
{
    return ChuckAndroid::getTemporaryFile();
}

#endif




//-----------------------------------------------------------------------------
// name: chuck_parse()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL chuck_parse( c_constr fname, FILE * fd, c_constr code )
{
    t_CKBOOL clo = FALSE;
    t_CKBOOL ret = FALSE;

    // sanity check
    if( fd && code )
    {
        CK_FPRINTF_STDERR( "[chuck](via parser): (internal) both fd and code specified!\n" );
        return FALSE;
    }

    // prepare code
    if( code )
    {
        // !
        assert( fd == NULL );
        // generate temp file
#ifdef __PLATFORM_WIN32__
        fd = win32_tmpfile();
#elif defined (__ANDROID__)
        fd = android_tmpfile();
#else
        fd = tmpfile();
#endif
        // on some systems, tmpfile() can return NULL
        if( !fd ) { EM_error2( 0, "unable to create temp file" ); return FALSE; }
        // flag it to close
        clo = TRUE;
        // write
        fwrite( code, sizeof(char), strlen(code), fd );
    }

    /*
    // use code from memory buffer if its available
    if( code )
    {
        // copy name
        strcpy( g_filename, fname );
        // reset
        if( EM_reset( g_filename, NULL ) == FALSE ) goto cleanup;

        // TODO: clean g_program
        g_program = NULL;

        // clean
        yyrestart( NULL );

        // load string (yy_scan_string will copy the C string)
        YY_BUFFER_STATE ybs = yy_scan_string( code );
        if( !ybs ) goto cleanup;

        // parse
        if( !( yyparse() == 0 ) ) goto cleanup;

        // delete the lexer buffer
        yy_delete_buffer( ybs );

    }
    */

    // check length | 1.4.1.0 (ge) added
    t_CKUINT len = strlen( fname );
    if( len > MAX_FILENAME_LEN )
    {
        EM_error2( 0, "filename length (%d) exceeds max (%d) set by compiler...",
                   len, MAX_FILENAME_LEN );
        EM_error2( 0, "filename in question: '%s'", fname );
        EM_error2( 0, "compiler bailing out..." );
        return FALSE;
    }
    // remember filename
    strcpy( g_filename, fname );

    // test it
    if( !fd ) {
        fd = open_cat_ck( g_filename );
        if( !fd ) strcpy( g_filename, fname );
        else clo = TRUE;
    }

    // reset
    if( EM_reset( g_filename, fd ) == FALSE ) goto cleanup;

    // lexer/parser
    // TODO: if( yyin ) { fclose( yyin ); yyin = NULL; }

    // if no fd, open
    if( !fd ) { fd = fopen( g_filename, "r" ); if( fd ) clo = TRUE; }
    // if still none
    if( !fd ) { EM_error2( 0, "no such file or directory" ); goto cleanup; }
    // set to beginning
    else fseek( fd, 0, SEEK_SET );

    // reset yyin to fd
    yyrestart( fd );

    // check
    if( yyin == NULL ) goto cleanup;

    // TODO: clean g_program
    g_program = NULL;

    // parse
    if( !(yyparse( ) == 0) ) goto cleanup;

    // flag success
    ret = TRUE;

cleanup:

    // done
    if( clo ) fclose( fd );

    return ret;
}




//------------------------------------------------------------------------------
// name: reset_parse()
// desc: ...
//------------------------------------------------------------------------------
void reset_parse( )
{
    // empty file name
    EM_change_file( NULL );
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




//-----------------------------------------------------------------------------
// name: absyn2str()
// desc: convert abstract syntax exp to string
//-----------------------------------------------------------------------------
string absyn2str( a_Exp exp )
{
    return absyn_exp2str( exp ) + ";";
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
    string str;
    // get operation string
    switch( op )
    {
        case ae_op_plus: str = "+"; break;
        case ae_op_plus_chuck: str = "+=>"; break;
        case ae_op_minus: str = "-"; break;
        case ae_op_minus_chuck: str = "-=>"; break;
        case ae_op_times: str = "*"; break;
        case ae_op_times_chuck: str = "*=>"; break;
        case ae_op_divide: str = "/"; break;
        case ae_op_divide_chuck: str = "/=>"; break;
        case ae_op_s_or: str = "|"; break;
        case ae_op_s_or_chuck: str = "|=>"; break;
        case ae_op_s_and: str = "&"; break;
        case ae_op_s_and_chuck: str = "&=>"; break;
        case ae_op_shift_left: str = "<<"; break;
        case ae_op_shift_left_chuck: str = "<<=>"; break;
        case ae_op_shift_right: str = ">>"; break;
        case ae_op_shift_right_chuck: str = ">>=>"; break;
        case ae_op_percent: str = "%"; break;
        case ae_op_percent_chuck: str = "%=>"; break;
        case ae_op_s_xor: str = "^"; break;
        case ae_op_s_xor_chuck: str = "^=>"; break;
        case ae_op_chuck: str = "=>"; break;
        case ae_op_unchuck: str = "=<"; break;
        case ae_op_upchuck: str = "=^"; break;
        case ae_op_at_chuck: str = "@=>"; break;
        case ae_op_s_chuck: str = ""; break;
        case ae_op_eq: str = "=="; break;
        case ae_op_neq: str = "!="; break;
        case ae_op_lt: str = "<"; break;
        case ae_op_le: str = "<="; break;
        case ae_op_gt: str = ">"; break;
        case ae_op_ge: str = ">="; break;
        case ae_op_and: str = "&&"; break;
        case ae_op_or: str = "||"; break;
        case ae_op_arrow_left: str = "<-"; break;
        case ae_op_arrow_right: str = "->"; break;

        // unary
        case ae_op_plusplus: str = "++"; break;
        case ae_op_minusminus: str = "--"; break;
        case ae_op_tilda: str = "~"; break;
        case ae_op_exclamation: str = "!"; break;
        case ae_op_spork: str = "spork ~"; break;
        case ae_op_new: str = "new"; break;

        // not used
        case ae_op_typeof: str = "typeof"; break;
        case ae_op_sizeof: str = "sizeof"; break;
        default: str = "[OP]"; break;
    }
    return str;
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
        case ae_op_lt:
        case ae_op_gt:
        case ae_op_le:
        case ae_op_ge:
        case ae_op_at_chuck:
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
    return absyn_op2str(unary->op) + " " + absyn_exp2str(unary->exp);
}




//-----------------------------------------------------------------------------
// name: absyn_cast2str()
// desc: convert abstract syntax cast to string
//-----------------------------------------------------------------------------
string absyn_cast2str( a_Exp_Cast cast )
{
    Chuck_Type * to = cast->self->type;
    return absyn_exp2str(cast->exp) + "$" + to->name;
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
        case ae_primary_complex: str = "#("+absyn_exp2str(primary->complex->re)+")"; break;
        case ae_primary_polar: str = "%("+absyn_exp2str(primary->polar->mod)+")"; break;
        case ae_primary_vec: str = "@("+absyn_exp2str(primary->vec->args)+")"; break;
        case ae_primary_exp: str = absyn_exp2str(primary->exp); break;
        case ae_primary_hack: str = "<<< " + absyn_exp2str(primary->exp) + " >>>"; break;
        case ae_primary_nil: str = "null"; break;
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
        str += list->var_decl->value->type->name + " " + (decl->type->ref && !list->var_decl->array ? "@ " : "") + list->var_decl->value->name;
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
