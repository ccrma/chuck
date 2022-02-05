/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_CHUCK_TAB_H_INCLUDED
# define YY_YY_CHUCK_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    STRING_LIT = 259,              /* STRING_LIT  */
    CHAR_LIT = 260,                /* CHAR_LIT  */
    NUM = 261,                     /* NUM  */
    FLOAT = 262,                   /* FLOAT  */
    POUND = 263,                   /* POUND  */
    COMMA = 264,                   /* COMMA  */
    COLON = 265,                   /* COLON  */
    SEMICOLON = 266,               /* SEMICOLON  */
    LPAREN = 267,                  /* LPAREN  */
    RPAREN = 268,                  /* RPAREN  */
    LBRACK = 269,                  /* LBRACK  */
    RBRACK = 270,                  /* RBRACK  */
    LBRACE = 271,                  /* LBRACE  */
    RBRACE = 272,                  /* RBRACE  */
    DOT = 273,                     /* DOT  */
    PLUS = 274,                    /* PLUS  */
    MINUS = 275,                   /* MINUS  */
    TIMES = 276,                   /* TIMES  */
    DIVIDE = 277,                  /* DIVIDE  */
    PERCENT = 278,                 /* PERCENT  */
    EQ = 279,                      /* EQ  */
    NEQ = 280,                     /* NEQ  */
    LT = 281,                      /* LT  */
    LE = 282,                      /* LE  */
    GT = 283,                      /* GT  */
    GE = 284,                      /* GE  */
    AND = 285,                     /* AND  */
    OR = 286,                      /* OR  */
    ASSIGN = 287,                  /* ASSIGN  */
    IF = 288,                      /* IF  */
    THEN = 289,                    /* THEN  */
    ELSE = 290,                    /* ELSE  */
    WHILE = 291,                   /* WHILE  */
    FOR = 292,                     /* FOR  */
    DO = 293,                      /* DO  */
    LOOP = 294,                    /* LOOP  */
    BREAK = 295,                   /* BREAK  */
    CONTINUE = 296,                /* CONTINUE  */
    NULL_TOK = 297,                /* NULL_TOK  */
    FUNCTION = 298,                /* FUNCTION  */
    RETURN = 299,                  /* RETURN  */
    QUESTION = 300,                /* QUESTION  */
    EXCLAMATION = 301,             /* EXCLAMATION  */
    S_OR = 302,                    /* S_OR  */
    S_AND = 303,                   /* S_AND  */
    S_XOR = 304,                   /* S_XOR  */
    PLUSPLUS = 305,                /* PLUSPLUS  */
    MINUSMINUS = 306,              /* MINUSMINUS  */
    DOLLAR = 307,                  /* DOLLAR  */
    POUNDPAREN = 308,              /* POUNDPAREN  */
    PERCENTPAREN = 309,            /* PERCENTPAREN  */
    ATPAREN = 310,                 /* ATPAREN  */
    SIMULT = 311,                  /* SIMULT  */
    PATTERN = 312,                 /* PATTERN  */
    CODE = 313,                    /* CODE  */
    TRANSPORT = 314,               /* TRANSPORT  */
    HOST = 315,                    /* HOST  */
    TIME = 316,                    /* TIME  */
    WHENEVER = 317,                /* WHENEVER  */
    NEXT = 318,                    /* NEXT  */
    UNTIL = 319,                   /* UNTIL  */
    EXTERNAL = 320,                /* EXTERNAL  */
    GLOBAL = 321,                  /* GLOBAL  */
    EVERY = 322,                   /* EVERY  */
    BEFORE = 323,                  /* BEFORE  */
    AFTER = 324,                   /* AFTER  */
    AT = 325,                      /* AT  */
    AT_SYM = 326,                  /* AT_SYM  */
    ATAT_SYM = 327,                /* ATAT_SYM  */
    NEW = 328,                     /* NEW  */
    SIZEOF = 329,                  /* SIZEOF  */
    TYPEOF = 330,                  /* TYPEOF  */
    SAME = 331,                    /* SAME  */
    PLUS_CHUCK = 332,              /* PLUS_CHUCK  */
    MINUS_CHUCK = 333,             /* MINUS_CHUCK  */
    TIMES_CHUCK = 334,             /* TIMES_CHUCK  */
    DIVIDE_CHUCK = 335,            /* DIVIDE_CHUCK  */
    S_AND_CHUCK = 336,             /* S_AND_CHUCK  */
    S_OR_CHUCK = 337,              /* S_OR_CHUCK  */
    S_XOR_CHUCK = 338,             /* S_XOR_CHUCK  */
    SHIFT_RIGHT_CHUCK = 339,       /* SHIFT_RIGHT_CHUCK  */
    SHIFT_LEFT_CHUCK = 340,        /* SHIFT_LEFT_CHUCK  */
    PERCENT_CHUCK = 341,           /* PERCENT_CHUCK  */
    SHIFT_RIGHT = 342,             /* SHIFT_RIGHT  */
    SHIFT_LEFT = 343,              /* SHIFT_LEFT  */
    TILDA = 344,                   /* TILDA  */
    CHUCK = 345,                   /* CHUCK  */
    COLONCOLON = 346,              /* COLONCOLON  */
    S_CHUCK = 347,                 /* S_CHUCK  */
    AT_CHUCK = 348,                /* AT_CHUCK  */
    LEFT_S_CHUCK = 349,            /* LEFT_S_CHUCK  */
    UNCHUCK = 350,                 /* UNCHUCK  */
    UPCHUCK = 351,                 /* UPCHUCK  */
    CLASS = 352,                   /* CLASS  */
    INTERFACE = 353,               /* INTERFACE  */
    EXTENDS = 354,                 /* EXTENDS  */
    IMPLEMENTS = 355,              /* IMPLEMENTS  */
    PUBLIC = 356,                  /* PUBLIC  */
    PROTECTED = 357,               /* PROTECTED  */
    PRIVATE = 358,                 /* PRIVATE  */
    STATIC = 359,                  /* STATIC  */
    ABSTRACT = 360,                /* ABSTRACT  */
    CONST = 361,                   /* CONST  */
    SPORK = 362,                   /* SPORK  */
    ARROW_RIGHT = 363,             /* ARROW_RIGHT  */
    ARROW_LEFT = 364,              /* ARROW_LEFT  */
    L_HACK = 365,                  /* L_HACK  */
    R_HACK = 366                   /* R_HACK  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 65 "chuck.y"

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

#line 201 "chuck.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_CHUCK_TAB_H_INCLUDED  */
