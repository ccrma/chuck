/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     GLOBAL = 321,
     EVERY = 322,
     BEFORE = 323,
     AFTER = 324,
     AT = 325,
     AT_SYM = 326,
     ATAT_SYM = 327,
     NEW = 328,
     SIZEOF = 329,
     TYPEOF = 330,
     SAME = 331,
     PLUS_CHUCK = 332,
     MINUS_CHUCK = 333,
     TIMES_CHUCK = 334,
     DIVIDE_CHUCK = 335,
     S_AND_CHUCK = 336,
     S_OR_CHUCK = 337,
     S_XOR_CHUCK = 338,
     SHIFT_RIGHT_CHUCK = 339,
     SHIFT_LEFT_CHUCK = 340,
     PERCENT_CHUCK = 341,
     SHIFT_RIGHT = 342,
     SHIFT_LEFT = 343,
     TILDA = 344,
     CHUCK = 345,
     COLONCOLON = 346,
     S_CHUCK = 347,
     AT_CHUCK = 348,
     LEFT_S_CHUCK = 349,
     UNCHUCK = 350,
     UPCHUCK = 351,
     CLASS = 352,
     INTERFACE = 353,
     EXTENDS = 354,
     IMPLEMENTS = 355,
     PUBLIC = 356,
     PROTECTED = 357,
     PRIVATE = 358,
     STATIC = 359,
     ABSTRACT = 360,
     CONST = 361,
     SPORK = 362,
     ARROW_RIGHT = 363,
     ARROW_LEFT = 364,
     L_HACK = 365,
     R_HACK = 366
   };
#endif
/* Tokens.  */
#define ID 258
#define STRING_LIT 259
#define CHAR_LIT 260
#define NUM 261
#define FLOAT 262
#define POUND 263
#define COMMA 264
#define COLON 265
#define SEMICOLON 266
#define LPAREN 267
#define RPAREN 268
#define LBRACK 269
#define RBRACK 270
#define LBRACE 271
#define RBRACE 272
#define DOT 273
#define PLUS 274
#define MINUS 275
#define TIMES 276
#define DIVIDE 277
#define PERCENT 278
#define EQ 279
#define NEQ 280
#define LT 281
#define LE 282
#define GT 283
#define GE 284
#define AND 285
#define OR 286
#define ASSIGN 287
#define IF 288
#define THEN 289
#define ELSE 290
#define WHILE 291
#define FOR 292
#define DO 293
#define LOOP 294
#define BREAK 295
#define CONTINUE 296
#define NULL_TOK 297
#define FUNCTION 298
#define RETURN 299
#define QUESTION 300
#define EXCLAMATION 301
#define S_OR 302
#define S_AND 303
#define S_XOR 304
#define PLUSPLUS 305
#define MINUSMINUS 306
#define DOLLAR 307
#define POUNDPAREN 308
#define PERCENTPAREN 309
#define ATPAREN 310
#define SIMULT 311
#define PATTERN 312
#define CODE 313
#define TRANSPORT 314
#define HOST 315
#define TIME 316
#define WHENEVER 317
#define NEXT 318
#define UNTIL 319
#define EXTERNAL 320
#define GLOBAL 321
#define EVERY 322
#define BEFORE 323
#define AFTER 324
#define AT 325
#define AT_SYM 326
#define ATAT_SYM 327
#define NEW 328
#define SIZEOF 329
#define TYPEOF 330
#define SAME 331
#define PLUS_CHUCK 332
#define MINUS_CHUCK 333
#define TIMES_CHUCK 334
#define DIVIDE_CHUCK 335
#define S_AND_CHUCK 336
#define S_OR_CHUCK 337
#define S_XOR_CHUCK 338
#define SHIFT_RIGHT_CHUCK 339
#define SHIFT_LEFT_CHUCK 340
#define PERCENT_CHUCK 341
#define SHIFT_RIGHT 342
#define SHIFT_LEFT 343
#define TILDA 344
#define CHUCK 345
#define COLONCOLON 346
#define S_CHUCK 347
#define AT_CHUCK 348
#define LEFT_S_CHUCK 349
#define UNCHUCK 350
#define UPCHUCK 351
#define CLASS 352
#define INTERFACE 353
#define EXTENDS 354
#define IMPLEMENTS 355
#define PUBLIC 356
#define PROTECTED 357
#define PRIVATE 358
#define STATIC 359
#define ABSTRACT 360
#define CONST 361
#define SPORK 362
#define ARROW_RIGHT 363
#define ARROW_LEFT 364
#define L_HACK 365
#define R_HACK 366




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 59 "chuck.y"
{
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
}
/* Line 1529 of yacc.c.  */
#line 297 "chuck.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

