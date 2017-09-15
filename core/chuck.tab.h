/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_CHUCK_TAB_H_INCLUDED
# define YY_YY_CHUCK_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    STRING_LIT = 259,
    CHAR_LIT = 260,
    NUM = 261,
    FLOAT = 262,
    POUND = 263,
    COMMA = 264,
    COLON = 265,
    SEMICOLON = 266,
    LPAREN = 267,
    RPAREN = 268,
    LBRACK = 269,
    RBRACK = 270,
    LBRACE = 271,
    RBRACE = 272,
    DOT = 273,
    PLUS = 274,
    MINUS = 275,
    TIMES = 276,
    DIVIDE = 277,
    PERCENT = 278,
    EQ = 279,
    NEQ = 280,
    LT = 281,
    LE = 282,
    GT = 283,
    GE = 284,
    AND = 285,
    OR = 286,
    ASSIGN = 287,
    IF = 288,
    THEN = 289,
    ELSE = 290,
    WHILE = 291,
    FOR = 292,
    DO = 293,
    LOOP = 294,
    BREAK = 295,
    CONTINUE = 296,
    NULL_TOK = 297,
    FUNCTION = 298,
    RETURN = 299,
    QUESTION = 300,
    EXCLAMATION = 301,
    S_OR = 302,
    S_AND = 303,
    S_XOR = 304,
    PLUSPLUS = 305,
    MINUSMINUS = 306,
    DOLLAR = 307,
    POUNDPAREN = 308,
    PERCENTPAREN = 309,
    ATPAREN = 310,
    SIMULT = 311,
    PATTERN = 312,
    CODE = 313,
    TRANSPORT = 314,
    HOST = 315,
    TIME = 316,
    WHENEVER = 317,
    NEXT = 318,
    UNTIL = 319,
    EXTERNAL = 320,
    EVERY = 321,
    BEFORE = 322,
    AFTER = 323,
    AT = 324,
    AT_SYM = 325,
    ATAT_SYM = 326,
    NEW = 327,
    SIZEOF = 328,
    TYPEOF = 329,
    SAME = 330,
    PLUS_CHUCK = 331,
    MINUS_CHUCK = 332,
    TIMES_CHUCK = 333,
    DIVIDE_CHUCK = 334,
    S_AND_CHUCK = 335,
    S_OR_CHUCK = 336,
    S_XOR_CHUCK = 337,
    SHIFT_RIGHT_CHUCK = 338,
    SHIFT_LEFT_CHUCK = 339,
    PERCENT_CHUCK = 340,
    SHIFT_RIGHT = 341,
    SHIFT_LEFT = 342,
    TILDA = 343,
    CHUCK = 344,
    COLONCOLON = 345,
    S_CHUCK = 346,
    AT_CHUCK = 347,
    LEFT_S_CHUCK = 348,
    UNCHUCK = 349,
    UPCHUCK = 350,
    CLASS = 351,
    INTERFACE = 352,
    EXTENDS = 353,
    IMPLEMENTS = 354,
    PUBLIC = 355,
    PROTECTED = 356,
    PRIVATE = 357,
    STATIC = 358,
    ABSTRACT = 359,
    CONST = 360,
    SPORK = 361,
    ARROW_RIGHT = 362,
    ARROW_LEFT = 363,
    L_HACK = 364,
    R_HACK = 365
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 59 "chuck.y" /* yacc.c:1909  */

    int pos;
    int ival;
    double fval;
    c_str sval;
    
    a_Program program;
    a_Section program_section;
    a_Stmt_List stmt_list;
    a_Class_Def class_def;
    a_Class_Ext class_ext;
    a_Class_Body class_body;
    a_Stmt stmt;
    a_Exp exp;
    a_Func_Def func_def;
    a_Var_Decl_List var_decl_list;
    a_Var_Decl var_decl;
    a_Type_Decl type_decl;
    a_Arg_List arg_list;
    a_Id_List id_list;
    a_Array_Sub array_sub;
    a_Complex complex_exp;
    a_Polar polar_exp;
    a_Vec vec_exp; // ge: added 1.3.5.3

#line 191 "chuck.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_CHUCK_TAB_H_INCLUDED  */
