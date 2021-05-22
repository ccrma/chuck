/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "chuck.y"


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
// file: chuck.tab.c
// desc: chuck parser
//
// author: Ge Wang (gewang@cs.princeton.edu) - generated by yacc
//         Perry R. Cook (prc@cs.princeton.edu)
//
// based in part on the ansi C grammar by Jeff Lee, maintained by Jutta Degener
//
// date: Summer 2002
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chuck_utils.h"
#include "chuck_errmsg.h"
#include "chuck_absyn.h"

// function
int yylex( void );

void yyerror( char *s )
{
    EM_error( EM_tokPos, "%s", s );
}

a_Program g_program = NULL;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 193 of yacc.c.  */
#line 400 "chuck.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 413 "chuck.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  122
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1312

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  112
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  58
/* YYNRULES -- Number of rules.  */
#define YYNRULES  184
/* YYNRULES -- Number of states.  */
#define YYNSTATES  319

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   366

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    21,    29,
      36,    44,    47,    52,    55,    60,    62,    63,    65,    68,
      70,    72,    74,    77,    79,    83,    85,    89,    98,   106,
     115,   123,   125,   127,   128,   130,   132,   134,   136,   138,
     140,   141,   143,   146,   150,   155,   157,   159,   161,   164,
     167,   172,   174,   177,   179,   181,   183,   185,   187,   190,
     194,   197,   200,   206,   214,   220,   228,   235,   243,   249,
     257,   263,   266,   270,   272,   275,   277,   281,   283,   287,
     289,   293,   297,   302,   305,   309,   311,   314,   318,   322,
     326,   329,   333,   335,   339,   341,   344,   347,   351,   355,
     359,   361,   363,   365,   367,   369,   371,   373,   375,   377,
     379,   381,   383,   385,   387,   389,   391,   393,   399,   401,
     405,   407,   411,   413,   417,   419,   423,   425,   429,   431,
     435,   439,   441,   445,   449,   453,   457,   459,   463,   467,
     469,   473,   477,   479,   483,   487,   491,   493,   497,   499,
     503,   505,   508,   511,   514,   517,   520,   523,   527,   529,
     531,   533,   535,   537,   540,   542,   546,   548,   551,   555,
     560,   564,   567,   570,   572,   574,   576,   578,   580,   582,
     584,   586,   588,   592,   596
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     113,     0,    -1,   114,    -1,   113,   114,    -1,   132,    -1,
     123,    -1,   115,    -1,   124,    97,   121,    16,   117,    17,
      -1,   124,    97,   121,   116,    16,   117,    17,    -1,   124,
      98,   121,    16,   117,    17,    -1,   124,    98,   121,   120,
      16,   117,    17,    -1,   100,   121,    -1,   100,   121,    99,
     122,    -1,    99,   122,    -1,    99,   122,   100,   121,    -1,
     118,    -1,    -1,   119,    -1,   119,   118,    -1,   132,    -1,
     123,    -1,   115,    -1,    99,   121,    -1,     3,    -1,     3,
       9,   121,    -1,     3,    -1,     3,    18,   122,    -1,   125,
     126,   130,     3,    12,   131,    13,   137,    -1,   125,   126,
     130,     3,    12,    13,   137,    -1,   125,   126,   130,     3,
      12,   131,    13,    11,    -1,   125,   126,   130,     3,    12,
      13,    11,    -1,   101,    -1,   103,    -1,    -1,    43,    -1,
     101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,    -1,
      -1,     3,    -1,     3,    71,    -1,    26,   122,    28,    -1,
      26,   122,    28,    71,    -1,   127,    -1,   128,    -1,   129,
      -1,   129,   143,    -1,   129,   146,    -1,   129,   146,     9,
     131,    -1,   133,    -1,   132,   133,    -1,   138,    -1,   136,
      -1,   135,    -1,   134,    -1,   137,    -1,    44,    11,    -1,
      44,   139,    11,    -1,    40,    11,    -1,    41,    11,    -1,
      33,    12,   139,    13,   133,    -1,    33,    12,   139,    13,
     133,    35,   133,    -1,    36,    12,   139,    13,   133,    -1,
      38,   133,    36,    12,   139,    13,    11,    -1,    37,    12,
     138,   138,    13,   133,    -1,    37,    12,   138,   138,   139,
      13,   133,    -1,    64,    12,   139,    13,   133,    -1,    38,
     133,    64,    12,   139,    13,    11,    -1,    39,    12,   139,
      13,   133,    -1,    16,    17,    -1,    16,   132,    17,    -1,
      11,    -1,   139,    11,    -1,   140,    -1,   139,     9,   140,
      -1,   141,    -1,   140,   150,   141,    -1,   144,    -1,   141,
     151,   144,    -1,    14,   139,    15,    -1,    14,   139,    15,
     142,    -1,    14,    15,    -1,   143,    14,    15,    -1,   152,
      -1,   129,   145,    -1,    65,   129,   145,    -1,    66,   129,
     145,    -1,   104,   129,   145,    -1,    76,   145,    -1,   104,
      76,   145,    -1,   146,    -1,   146,     9,   145,    -1,     3,
      -1,     3,   142,    -1,     3,   143,    -1,    53,   139,    13,
      -1,    54,   139,    13,    -1,    55,   139,    13,    -1,    90,
      -1,    93,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    84,    -1,    85,    -1,    86,    -1,    95,    -1,    96,
      -1,    81,    -1,    82,    -1,    83,    -1,   109,    -1,   108,
      -1,   153,    -1,   153,    45,   139,    10,   152,    -1,   154,
      -1,   153,    31,   154,    -1,   155,    -1,   154,    30,   155,
      -1,   156,    -1,   155,    47,   156,    -1,   157,    -1,   156,
      49,   157,    -1,   158,    -1,   157,    48,   158,    -1,   159,
      -1,   158,    24,   159,    -1,   158,    25,   159,    -1,   160,
      -1,   159,    26,   160,    -1,   159,    28,   160,    -1,   159,
      27,   160,    -1,   159,    29,   160,    -1,   161,    -1,   160,
      88,   161,    -1,   160,    87,   161,    -1,   162,    -1,   161,
      19,   162,    -1,   161,    20,   162,    -1,   163,    -1,   162,
      21,   163,    -1,   162,    22,   163,    -1,   162,    23,   163,
      -1,   164,    -1,   163,    89,   164,    -1,   165,    -1,   164,
      52,   129,    -1,   167,    -1,    50,   165,    -1,    51,   165,
      -1,   166,   165,    -1,    75,   165,    -1,    74,   165,    -1,
      73,   129,    -1,    73,   129,   142,    -1,    19,    -1,    20,
      -1,    89,    -1,    46,    -1,    21,    -1,   107,    89,    -1,
     168,    -1,   167,    91,   168,    -1,   169,    -1,   168,   142,
      -1,   168,    12,    13,    -1,   168,    12,   139,    13,    -1,
     168,    18,     3,    -1,   168,    50,    -1,   168,    51,    -1,
       3,    -1,     6,    -1,     7,    -1,     4,    -1,     5,    -1,
     142,    -1,   147,    -1,   148,    -1,   149,    -1,   110,   139,
     111,    -1,    12,   139,    13,    -1,    12,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   184,   184,   185,   189,   190,   191,   195,   197,   199,
     201,   206,   207,   208,   209,   213,   214,   218,   219,   224,
     225,   226,   230,   234,   235,   239,   240,   244,   246,   248,
     250,   255,   256,   257,   261,   262,   263,   264,   268,   269,
     270,   274,   275,   279,   280,   289,   290,   295,   296,   300,
     301,   305,   306,   310,   311,   312,   313,   315,   319,   320,
     321,   322,   326,   328,   333,   335,   337,   339,   341,   343,
     345,   350,   351,   355,   356,   360,   361,   365,   366,   371,
     372,   377,   378,   383,   384,   388,   389,   390,   391,   392,
     393,   394,   398,   399,   403,   404,   405,   409,   414,   419,
     424,   425,   426,   427,   428,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   441,   442,   446,   447,   452,   453,
     458,   459,   464,   465,   470,   471,   476,   477,   482,   483,
     485,   490,   491,   493,   495,   497,   502,   503,   505,   510,
     511,   513,   518,   519,   521,   523,   528,   529,   534,   535,
     540,   541,   543,   545,   547,   549,   551,   553,   560,   561,
     562,   563,   564,   565,   570,   571,   576,   577,   579,   581,
     583,   585,   587,   593,   594,   595,   596,   597,   598,   599,
     600,   601,   602,   603,   604
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "STRING_LIT", "CHAR_LIT", "NUM",
  "FLOAT", "POUND", "COMMA", "COLON", "SEMICOLON", "LPAREN", "RPAREN",
  "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS", "MINUS", "TIMES",
  "DIVIDE", "PERCENT", "EQ", "NEQ", "LT", "LE", "GT", "GE", "AND", "OR",
  "ASSIGN", "IF", "THEN", "ELSE", "WHILE", "FOR", "DO", "LOOP", "BREAK",
  "CONTINUE", "NULL_TOK", "FUNCTION", "RETURN", "QUESTION", "EXCLAMATION",
  "S_OR", "S_AND", "S_XOR", "PLUSPLUS", "MINUSMINUS", "DOLLAR",
  "POUNDPAREN", "PERCENTPAREN", "ATPAREN", "SIMULT", "PATTERN", "CODE",
  "TRANSPORT", "HOST", "TIME", "WHENEVER", "NEXT", "UNTIL", "EXTERNAL",
  "GLOBAL", "EVERY", "BEFORE", "AFTER", "AT", "AT_SYM", "ATAT_SYM", "NEW",
  "SIZEOF", "TYPEOF", "SAME", "PLUS_CHUCK", "MINUS_CHUCK", "TIMES_CHUCK",
  "DIVIDE_CHUCK", "S_AND_CHUCK", "S_OR_CHUCK", "S_XOR_CHUCK",
  "SHIFT_RIGHT_CHUCK", "SHIFT_LEFT_CHUCK", "PERCENT_CHUCK", "SHIFT_RIGHT",
  "SHIFT_LEFT", "TILDA", "CHUCK", "COLONCOLON", "S_CHUCK", "AT_CHUCK",
  "LEFT_S_CHUCK", "UNCHUCK", "UPCHUCK", "CLASS", "INTERFACE", "EXTENDS",
  "IMPLEMENTS", "PUBLIC", "PROTECTED", "PRIVATE", "STATIC", "ABSTRACT",
  "CONST", "SPORK", "ARROW_RIGHT", "ARROW_LEFT", "L_HACK", "R_HACK",
  "$accept", "program", "program_section", "class_definition", "class_ext",
  "class_body", "class_body2", "class_section", "iface_ext", "id_list",
  "id_dot", "function_definition", "class_decl", "function_decl",
  "static_decl", "type_decl_a", "type_decl_b", "type_decl", "type_decl2",
  "arg_list", "statement_list", "statement", "jump_statement",
  "selection_statement", "loop_statement", "code_segment",
  "expression_statement", "expression", "chuck_expression",
  "arrow_expression", "array_exp", "array_empty", "decl_expression",
  "var_decl_list", "var_decl", "complex_exp", "polar_exp", "vec_exp",
  "chuck_operator", "arrow_operator", "conditional_expression",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "tilda_expression",
  "cast_expression", "unary_expression", "unary_operator",
  "dur_expression", "postfix_expression", "primary_expression", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   112,   113,   113,   114,   114,   114,   115,   115,   115,
     115,   116,   116,   116,   116,   117,   117,   118,   118,   119,
     119,   119,   120,   121,   121,   122,   122,   123,   123,   123,
     123,   124,   124,   124,   125,   125,   125,   125,   126,   126,
     126,   127,   127,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   133,   133,   133,   133,   133,   134,   134,
     134,   134,   135,   135,   136,   136,   136,   136,   136,   136,
     136,   137,   137,   138,   138,   139,   139,   140,   140,   141,
     141,   142,   142,   143,   143,   144,   144,   144,   144,   144,
     144,   144,   145,   145,   146,   146,   146,   147,   148,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   151,   151,   152,   152,   153,   153,
     154,   154,   155,   155,   156,   156,   157,   157,   158,   158,
     158,   159,   159,   159,   159,   159,   160,   160,   160,   161,
     161,   161,   162,   162,   162,   162,   163,   163,   164,   164,
     165,   165,   165,   165,   165,   165,   165,   165,   166,   166,
     166,   166,   166,   166,   167,   167,   168,   168,   168,   168,
     168,   168,   168,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     6,     7,     6,
       7,     2,     4,     2,     4,     1,     0,     1,     2,     1,
       1,     1,     2,     1,     3,     1,     3,     8,     7,     8,
       7,     1,     1,     0,     1,     1,     1,     1,     1,     1,
       0,     1,     2,     3,     4,     1,     1,     1,     2,     2,
       4,     1,     2,     1,     1,     1,     1,     1,     2,     3,
       2,     2,     5,     7,     5,     7,     6,     7,     5,     7,
       5,     2,     3,     1,     2,     1,     3,     1,     3,     1,
       3,     3,     4,     2,     3,     1,     2,     3,     3,     3,
       2,     3,     1,     3,     1,     2,     2,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     5,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     1,     3,     1,     3,
       1,     2,     2,     2,     2,     2,     2,     3,     1,     1,
       1,     1,     1,     2,     1,     3,     1,     2,     3,     4,
       3,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      33,   173,   176,   177,   174,   175,    73,     0,     0,     0,
     158,   159,   162,     0,     0,     0,     0,     0,     0,     0,
       0,    34,     0,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   160,    35,    36,    37,
       0,     0,     0,    33,     2,     6,     5,     0,    40,    45,
      46,     0,     4,    51,    56,    55,    54,    57,    53,     0,
      75,    77,   178,    79,   179,   180,   181,    85,   116,   118,
     120,   122,   124,   126,   128,   131,   136,   139,   142,   146,
     148,     0,   150,   164,   166,    42,   184,     0,     0,    71,
       0,    25,     0,     0,     0,     0,     0,     0,    60,    61,
      58,     0,   173,   151,   152,     0,     0,     0,     0,    41,
       0,     0,   156,   155,   154,    94,    90,    92,     0,     0,
     163,     0,     1,     3,     0,     0,    38,    39,     0,    86,
      52,     0,    74,   102,   103,   104,   105,   111,   112,   113,
     106,   107,   108,   100,   101,   109,   110,     0,   115,   114,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   153,     0,     0,     0,   171,   172,   167,   183,
      81,    72,     0,    43,     0,     0,     0,     0,     0,     0,
      59,    97,    98,    99,     0,    87,    88,   157,     0,    95,
      96,     0,    91,    89,   182,    23,     0,     0,    47,     0,
      76,    78,    80,   119,     0,   121,   123,   125,   127,   129,
     130,   132,   134,   133,   135,   138,   137,   140,   141,   143,
     144,   145,   147,   149,   165,   168,     0,   170,    82,    26,
      44,     0,     0,     0,     0,     0,     0,     0,    83,     0,
      93,     0,    33,     0,     0,     0,    33,     0,     0,     0,
      48,     0,     0,   169,    62,    64,     0,     0,     0,     0,
      70,    68,    84,    24,    21,     0,    15,    33,    20,    19,
      13,    11,    33,     0,    22,    33,     0,   117,     0,    66,
       0,     0,     0,     7,    18,     0,     0,     0,     9,     0,
       0,     0,     0,    63,    67,    65,    69,    14,    12,     8,
      10,    30,    28,    49,     0,     0,    29,    27,    50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    43,    44,   274,   255,   275,   276,   277,   258,   206,
      92,   278,    47,    48,   128,    49,    50,    51,   209,   302,
     279,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,   200,    63,   116,   117,    64,    65,    66,   147,   150,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -247
static const yytype_int16 yypact[] =
{
     433,    10,  -247,  -247,  -247,  -247,  -247,   729,  1137,   512,
    -247,  -247,  -247,    46,    41,    68,    75,   664,    84,    88,
      99,  -247,   803,  -247,  1202,  1202,  1137,  1137,  1137,    92,
      44,    44,    44,  1202,  1202,   131,  -247,   -26,  -247,    58,
      19,    56,  1137,   147,  -247,  -247,  -247,    81,    32,  -247,
    -247,   131,   664,  -247,  -247,  -247,  -247,  -247,  -247,    98,
      35,    73,  -247,  -247,  -247,  -247,  -247,  -247,    17,   145,
     125,   128,   141,   200,   177,   142,   215,   193,   103,   144,
    -247,  1202,   108,    14,  -247,  -247,  -247,    69,    37,  -247,
     588,   176,   198,  1137,  1137,   877,     5,  1137,  -247,  -247,
    -247,   115,  -247,  -247,  -247,    79,    80,    93,  1137,   160,
     131,   131,   225,  -247,  -247,   233,  -247,   244,   131,   131,
    -247,     0,  -247,  -247,   252,   252,  -247,  -247,    44,  -247,
    -247,  1137,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,  -247,  -247,  1137,  -247,  -247,
    1137,  1202,  1137,  1202,  1202,  1202,  1202,  1202,  1202,  1202,
    1202,  1202,  1202,  1202,  1202,  1202,  1202,  1202,  1202,  1202,
    1202,    44,  -247,    30,   942,   255,  -247,  -247,  -247,  -247,
     225,  -247,    46,   190,   114,   120,   877,   250,   251,   130,
    -247,  -247,  -247,  -247,   133,  -247,  -247,  -247,  1007,  -247,
     253,   131,  -247,  -247,  -247,   257,     1,    -2,   254,   261,
      35,    73,  -247,   145,   228,   125,   128,   141,   200,   177,
     177,   142,   142,   142,   142,   215,   215,   193,   193,   103,
     103,   103,   144,  -247,    14,  -247,   135,  -247,  -247,  -247,
    -247,   664,   664,  1072,  1137,  1137,   664,   664,  -247,   256,
    -247,   252,   275,    46,   252,   259,   275,   252,   267,   258,
     253,   260,  1202,  -247,   234,  -247,   664,   151,   156,   161,
    -247,  -247,  -247,  -247,  -247,   271,  -247,   354,  -247,   664,
     170,   178,   275,   273,  -247,   275,    60,  -247,   664,  -247,
     664,   282,   286,  -247,  -247,   252,    46,   281,  -247,   283,
      39,   131,   289,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,   290,    40,    44,  -247,  -247,  -247
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -247,  -247,   241,    15,  -247,  -225,    26,  -247,  -247,  -119,
    -174,    16,  -247,  -247,  -247,  -247,  -247,   -30,  -247,   -11,
      18,   -14,  -247,  -247,  -247,  -246,   -83,    -3,   174,   159,
     -72,   101,   157,   -44,     9,  -247,  -247,  -247,  -247,  -247,
      55,  -247,   169,   171,   168,   172,   167,    86,    48,    82,
      94,    50,   162,    -4,  -247,  -247,   158,  -247
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -42
static const yytype_int16 yytable[] =
{
     110,   111,   112,    96,    87,    88,   207,   129,   239,   131,
     119,   178,   186,   -41,   256,    45,    46,   252,    52,   101,
     103,   104,   109,   105,   106,   107,   174,    90,     8,   113,
     114,   283,   175,   102,     2,     3,     4,     5,   130,   121,
     197,   187,     7,   199,     8,    13,   131,   109,   151,    91,
     311,   316,   180,    93,   312,     9,     9,   297,    45,    46,
     299,    52,   152,   109,   176,   177,   195,   196,   317,   188,
      13,   -31,   -31,   300,   202,   203,   130,   172,   131,   280,
      94,    85,   179,    26,    27,    28,    13,    95,   131,   131,
     184,   185,   191,   192,   189,   118,    97,   257,   208,    98,
     253,   254,   131,   243,   108,   194,   193,   131,   238,   132,
      99,   204,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   308,   131,   131,   143,   190,   241,   144,   131,
     145,   146,   273,   242,   115,   281,   126,   127,   284,   131,
      42,   233,   131,   246,   131,   120,   247,   122,   263,   214,
       1,     2,     3,     4,     5,   -32,   -32,   250,     6,     7,
     131,     8,   178,     9,   290,   131,    10,    11,    12,   291,
     131,   236,   154,    13,   292,   153,   307,   155,   124,   125,
      14,   148,   149,    15,    16,    17,    18,    19,    20,   156,
      21,    22,   170,    23,   182,    88,   171,    24,    25,   173,
      26,    27,    28,   159,   160,   161,   162,   221,   222,   223,
     224,    29,    30,    31,   167,   168,   169,   229,   230,   231,
      32,    33,    34,    35,   157,   158,   183,   264,   265,   163,
     164,    85,   270,   271,   165,   166,    36,   131,   262,     8,
     267,   268,   269,   219,   220,   225,   226,   198,    37,    38,
      39,    40,   289,   201,    41,   205,   301,    42,   237,   227,
     228,   240,   244,   245,   261,   130,   251,   249,   259,   288,
     295,   272,   286,   248,   303,   282,   304,   296,     1,     2,
       3,     4,     5,   285,   123,   301,     6,     7,   293,     8,
     298,     9,   -16,   305,    10,    11,    12,   306,   309,   315,
     310,    13,   314,   294,   318,   210,   211,   212,    14,   260,
     313,    15,    16,    17,    18,    19,    20,   287,    21,    22,
     213,    23,   216,   218,   215,    24,    25,   217,    26,    27,
      28,   234,   232,     0,     0,     0,     0,     0,     0,    29,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     1,     2,     3,
       4,     5,     0,     0,    36,     6,     7,     0,     8,     0,
       9,   -17,     0,    10,    11,    12,    37,    38,    39,    40,
      13,     0,    41,     0,     0,    42,     0,    14,     0,     0,
      15,    16,    17,    18,    19,    20,     0,    21,    22,     0,
      23,     0,     0,     0,    24,    25,     0,    26,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
      31,     0,     0,     0,     0,     0,     0,    32,    33,    34,
      35,     0,     0,     0,     0,     0,     1,     2,     3,     4,
       5,     0,     0,    36,     6,     7,     0,     8,     0,     9,
       0,     0,    10,    11,    12,    37,    38,    39,    40,    13,
       0,    41,     0,     0,    42,     0,    14,     0,     0,    15,
      16,    17,    18,    19,    20,     0,    21,    22,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       0,     0,     0,     0,     0,     1,     2,     3,     4,     5,
       0,     0,    36,     6,     7,     0,     8,     0,     9,    89,
       0,    10,    11,    12,    37,    38,    39,    40,    13,     0,
      41,     0,     0,    42,     0,    14,     0,     0,    15,    16,
      17,    18,    19,    20,     0,     0,    22,     0,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,    29,    30,    31,     0,
       0,     0,     0,     0,     0,    32,    33,    34,    35,     0,
       0,     1,     2,     3,     4,     5,     0,     0,     0,     6,
       7,    36,     8,     0,     9,   181,     0,    10,    11,    12,
       0,     0,     0,     0,    13,     0,    40,     0,     0,    41,
       0,    14,    42,     0,    15,    16,    17,    18,    19,    20,
       0,     0,    22,     0,    23,     0,     0,     0,    24,    25,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    30,    31,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,     0,     0,     1,     2,     3,
       4,     5,     0,     0,     0,     6,     7,    36,     8,     0,
       9,     0,     0,    10,    11,    12,     0,     0,     0,     0,
      13,     0,    40,     0,     0,    41,     0,    14,    42,     0,
      15,    16,    17,    18,    19,    20,     0,     0,    22,     0,
      23,     0,     0,     0,    24,    25,     0,    26,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
      31,     0,     1,     2,     3,     4,     5,    32,    33,    34,
      35,     7,    86,     8,     0,     0,     0,     0,    10,    11,
      12,     0,     0,    36,     0,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,     0,
       0,    41,     0,     0,    42,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,    31,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,     1,     2,     3,     4,
       5,     0,     0,     0,   100,     7,     0,     8,    36,     0,
       0,     0,    10,    11,    12,     0,     0,     0,     0,    13,
       0,     0,     0,    40,     0,     0,    41,     0,     0,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       1,     2,     3,     4,     5,     0,     0,     0,     6,     7,
       0,     8,    36,     0,     0,     0,    10,    11,    12,     0,
       0,     0,     0,    13,     0,     0,     0,    40,     0,     0,
      41,     0,     0,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     1,     2,     3,     4,     5,
      32,    33,    34,    35,     7,   235,     8,     0,     0,     0,
       0,    10,    11,    12,     0,     0,    36,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,    41,     0,     0,    42,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       1,     2,     3,     4,     5,    32,    33,    34,    35,     7,
       0,     8,   248,     0,     0,     0,    10,    11,    12,     0,
       0,    36,     0,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
       0,     0,    42,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     1,     2,     3,     4,     5,
      32,    33,    34,    35,     7,   266,     8,     0,     0,     0,
       0,    10,    11,    12,     0,     0,    36,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,    41,     0,     0,    42,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       1,     2,     3,     4,     5,    32,    33,    34,    35,     7,
       0,     8,     0,     0,     0,     0,    10,    11,    12,     0,
       0,    36,     0,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
       0,     0,    42,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,   102,     2,     3,     4,     5,
      32,    33,    34,    35,     7,     0,     8,     0,     0,     0,
       0,    10,    11,    12,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,    41,     0,     0,    42,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,    34,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
       0,     0,    42
};

static const yytype_int16 yycheck[] =
{
      30,    31,    32,    17,     7,     8,   125,    51,   182,     9,
      40,    83,    95,     3,    16,     0,     0,    16,     0,    22,
      24,    25,     3,    26,    27,    28,    12,     9,    14,    33,
      34,   256,    18,     3,     4,     5,     6,     7,    52,    42,
     112,    36,    12,   115,    14,    26,     9,     3,    31,     3,
      11,    11,    15,    12,   300,    16,    16,   282,    43,    43,
     285,    43,    45,     3,    50,    51,   110,   111,   314,    64,
      26,    97,    98,    13,   118,   119,    90,    81,     9,   253,
      12,    71,    13,    53,    54,    55,    26,    12,     9,     9,
      93,    94,    13,    13,    97,    76,    12,    99,   128,    11,
      99,   100,     9,   186,    12,   108,    13,     9,   180,    11,
      11,   111,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,   296,     9,     9,    90,    11,    13,    93,     9,
      95,    96,   251,    13,     3,   254,   104,   105,   257,     9,
     110,   171,     9,    13,     9,    89,    13,     0,    13,   152,
       3,     4,     5,     6,     7,    97,    98,   201,    11,    12,
       9,    14,   234,    16,    13,     9,    19,    20,    21,    13,
       9,   174,    47,    26,    13,    30,   295,    49,    97,    98,
      33,   108,   109,    36,    37,    38,    39,    40,    41,    48,
      43,    44,    89,    46,    18,   198,    52,    50,    51,    91,
      53,    54,    55,    26,    27,    28,    29,   159,   160,   161,
     162,    64,    65,    66,    21,    22,    23,   167,   168,   169,
      73,    74,    75,    76,    24,    25,    28,   241,   242,    87,
      88,    71,   246,   247,    19,    20,    89,     9,    10,    14,
     243,   244,   245,   157,   158,   163,   164,    14,   101,   102,
     103,   104,   266,     9,   107,     3,   286,   110,     3,   165,
     166,    71,    12,    12,     3,   279,     9,    14,    14,    35,
     100,    15,    12,    15,   288,    16,   290,    99,     3,     4,
       5,     6,     7,    16,    43,   315,    11,    12,    17,    14,
      17,    16,    17,    11,    19,    20,    21,    11,    17,     9,
      17,    26,    13,   277,   315,   131,   147,   150,    33,   208,
     301,    36,    37,    38,    39,    40,    41,   262,    43,    44,
     151,    46,   154,   156,   153,    50,    51,   155,    53,    54,
      55,   173,   170,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    89,    11,    12,    -1,    14,    -1,
      16,    17,    -1,    19,    20,    21,   101,   102,   103,   104,
      26,    -1,   107,    -1,    -1,   110,    -1,    33,    -1,    -1,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    89,    11,    12,    -1,    14,    -1,    16,
      -1,    -1,    19,    20,    21,   101,   102,   103,   104,    26,
      -1,   107,    -1,    -1,   110,    -1,    33,    -1,    -1,    36,
      37,    38,    39,    40,    41,    -1,    43,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    89,    11,    12,    -1,    14,    -1,    16,    17,
      -1,    19,    20,    21,   101,   102,   103,   104,    26,    -1,
     107,    -1,    -1,   110,    -1,    33,    -1,    -1,    36,    37,
      38,    39,    40,    41,    -1,    -1,    44,    -1,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,    -1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    89,    14,    -1,    16,    17,    -1,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    -1,   104,    -1,    -1,   107,
      -1,    33,   110,    -1,    36,    37,    38,    39,    40,    41,
      -1,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    -1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    12,    89,    14,    -1,
      16,    -1,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    -1,   104,    -1,    -1,   107,    -1,    33,   110,    -1,
      36,    37,    38,    39,    40,    41,    -1,    -1,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    -1,     3,     4,     5,     6,     7,    73,    74,    75,
      76,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,
      21,    -1,    -1,    89,    -1,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
      -1,   107,    -1,    -1,   110,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    -1,    14,    89,    -1,
      -1,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      -1,    -1,    -1,   104,    -1,    -1,   107,    -1,    -1,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    11,    12,
      -1,    14,    89,    -1,    -1,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,   104,    -1,    -1,
     107,    -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,     3,     4,     5,     6,     7,
      73,    74,    75,    76,    12,    13,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    -1,    -1,    89,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   104,    -1,    -1,   107,    -1,    -1,   110,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,
       3,     4,     5,     6,     7,    73,    74,    75,    76,    12,
      -1,    14,    15,    -1,    -1,    -1,    19,    20,    21,    -1,
      -1,    89,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,    -1,   107,
      -1,    -1,   110,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,     3,     4,     5,     6,     7,
      73,    74,    75,    76,    12,    13,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    -1,    -1,    89,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   104,    -1,    -1,   107,    -1,    -1,   110,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,
       3,     4,     5,     6,     7,    73,    74,    75,    76,    12,
      -1,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    -1,
      -1,    89,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,    -1,   107,
      -1,    -1,   110,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,     3,     4,     5,     6,     7,
      73,    74,    75,    76,    12,    -1,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   104,    -1,    -1,   107,    -1,    -1,   110,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
      -1,    -1,   110
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    11,    12,    14,    16,
      19,    20,    21,    26,    33,    36,    37,    38,    39,    40,
      41,    43,    44,    46,    50,    51,    53,    54,    55,    64,
      65,    66,    73,    74,    75,    76,    89,   101,   102,   103,
     104,   107,   110,   113,   114,   115,   123,   124,   125,   127,
     128,   129,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   144,   147,   148,   149,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,    71,    13,   139,   139,    17,
     132,     3,   122,    12,    12,    12,   133,    12,    11,    11,
      11,   139,     3,   165,   165,   139,   139,   139,    12,     3,
     129,   129,   129,   165,   165,     3,   145,   146,    76,   129,
      89,   139,     0,   114,    97,    98,   104,   105,   126,   145,
     133,     9,    11,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    90,    93,    95,    96,   150,   108,   109,
     151,    31,    45,    30,    47,    49,    48,    24,    25,    26,
      27,    28,    29,    87,    88,    19,    20,    21,    22,    23,
      89,    52,   165,    91,    12,    18,    50,    51,   142,    13,
      15,    17,    18,    28,   139,   139,   138,    36,    64,   139,
      11,    13,    13,    13,   139,   145,   145,   142,    14,   142,
     143,     9,   145,   145,   111,     3,   121,   121,   129,   130,
     140,   141,   144,   154,   139,   155,   156,   157,   158,   159,
     159,   160,   160,   160,   160,   161,   161,   162,   162,   163,
     163,   163,   164,   129,   168,    13,   139,     3,   142,   122,
      71,    13,    13,   138,    12,    12,    13,    13,    15,    14,
     145,     9,    16,    99,   100,   116,    16,    99,   120,    14,
     143,     3,    10,    13,   133,   133,    13,   139,   139,   139,
     133,   133,    15,   121,   115,   117,   118,   119,   123,   132,
     122,   121,    16,   117,   121,    16,    12,   152,    35,   133,
      13,    13,    13,    17,   118,   100,    99,   117,    17,   117,
      13,   129,   131,   133,   133,    11,    11,   121,   122,    17,
      17,    11,   137,   146,    13,     9,    11,   137,   131
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 184 "chuck.y"
    { (yyval.program) = g_program = new_program( (yyvsp[(1) - (1)].program_section), EM_lineNum ); ;}
    break;

  case 3:
#line 185 "chuck.y"
    { (yyval.program) = g_program = append_program( (yyvsp[(1) - (2)].program), (yyvsp[(2) - (2)].program_section), EM_lineNum ); ;}
    break;

  case 4:
#line 189 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), EM_lineNum ); ;}
    break;

  case 5:
#line 190 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), EM_lineNum ); ;}
    break;

  case 6:
#line 191 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), EM_lineNum ); ;}
    break;

  case 7:
#line 196 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), EM_lineNum ); ;}
    break;

  case 8:
#line 198 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), EM_lineNum ); ;}
    break;

  case 9:
#line 200 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), EM_lineNum ); ;}
    break;

  case 10:
#line 202 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), EM_lineNum ); ;}
    break;

  case 11:
#line 206 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), EM_lineNum ); ;}
    break;

  case 12:
#line 207 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(4) - (4)].id_list), (yyvsp[(2) - (4)].id_list), EM_lineNum ); ;}
    break;

  case 13:
#line 208 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (2)].id_list), NULL, EM_lineNum ); ;}
    break;

  case 14:
#line 209 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (4)].id_list), (yyvsp[(4) - (4)].id_list), EM_lineNum ); ;}
    break;

  case 15:
#line 213 "chuck.y"
    { (yyval.class_body) = (yyvsp[(1) - (1)].class_body); ;}
    break;

  case 16:
#line 214 "chuck.y"
    { (yyval.class_body) = NULL; ;}
    break;

  case 17:
#line 218 "chuck.y"
    { (yyval.class_body) = new_class_body( (yyvsp[(1) - (1)].program_section), EM_lineNum ); ;}
    break;

  case 18:
#line 219 "chuck.y"
    { (yyval.class_body) = prepend_class_body( (yyvsp[(1) - (2)].program_section), (yyvsp[(2) - (2)].class_body), EM_lineNum ); ;}
    break;

  case 19:
#line 224 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), EM_lineNum ); ;}
    break;

  case 20:
#line 225 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), EM_lineNum ); ;}
    break;

  case 21:
#line 226 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), EM_lineNum ); ;}
    break;

  case 22:
#line 230 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), EM_lineNum ); ;}
    break;

  case 23:
#line 234 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), EM_lineNum ); ;}
    break;

  case 24:
#line 235 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), EM_lineNum ); ;}
    break;

  case 25:
#line 239 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), EM_lineNum ); ;}
    break;

  case 26:
#line 240 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), EM_lineNum ); ;}
    break;

  case 27:
#line 245 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), (yyvsp[(8) - (8)].stmt), EM_lineNum ); ;}
    break;

  case 28:
#line 247 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, (yyvsp[(7) - (7)].stmt), EM_lineNum ); ;}
    break;

  case 29:
#line 249 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), NULL, EM_lineNum ); ;}
    break;

  case 30:
#line 251 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, NULL, EM_lineNum ); ;}
    break;

  case 31:
#line 255 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 32:
#line 256 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 33:
#line 257 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 34:
#line 261 "chuck.y"
    { (yyval.ival) = ae_key_func; ;}
    break;

  case 35:
#line 262 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 36:
#line 263 "chuck.y"
    { (yyval.ival) = ae_key_protected; ;}
    break;

  case 37:
#line 264 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 38:
#line 268 "chuck.y"
    { (yyval.ival) = ae_key_static; ;}
    break;

  case 39:
#line 269 "chuck.y"
    { (yyval.ival) = ae_key_abstract; ;}
    break;

  case 40:
#line 270 "chuck.y"
    { (yyval.ival) = ae_key_instance; ;}
    break;

  case 41:
#line 274 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (1)].sval), EM_lineNum ), 0, EM_lineNum ); ;}
    break;

  case 42:
#line 275 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (2)].sval), EM_lineNum ), 1, EM_lineNum ); ;}
    break;

  case 43:
#line 279 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (3)].id_list), 0, EM_lineNum ); ;}
    break;

  case 44:
#line 280 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (4)].id_list), 1, EM_lineNum ); ;}
    break;

  case 45:
#line 289 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 46:
#line 290 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 47:
#line 295 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 48:
#line 296 "chuck.y"
    { (yyval.type_decl) = add_type_decl_array( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].array_sub), EM_lineNum ); ;}
    break;

  case 49:
#line 300 "chuck.y"
    { (yyval.arg_list) = new_arg_list( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl), EM_lineNum ); ;}
    break;

  case 50:
#line 301 "chuck.y"
    { (yyval.arg_list) = prepend_arg_list( (yyvsp[(1) - (4)].type_decl), (yyvsp[(2) - (4)].var_decl), (yyvsp[(4) - (4)].arg_list), EM_lineNum ); ;}
    break;

  case 51:
#line 305 "chuck.y"
    { (yyval.stmt_list) = new_stmt_list( (yyvsp[(1) - (1)].stmt), EM_lineNum ); ;}
    break;

  case 52:
#line 306 "chuck.y"
    { (yyval.stmt_list) = append_stmt_list( (yyvsp[(1) - (2)].stmt_list), (yyvsp[(2) - (2)].stmt), EM_lineNum ); ;}
    break;

  case 53:
#line 310 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 54:
#line 311 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 55:
#line 312 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 56:
#line 313 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 57:
#line 315 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 58:
#line 319 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( NULL, EM_lineNum ); ;}
    break;

  case 59:
#line 320 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 60:
#line 321 "chuck.y"
    { (yyval.stmt) = new_stmt_from_break( EM_lineNum ); ;}
    break;

  case 61:
#line 322 "chuck.y"
    { (yyval.stmt) = new_stmt_from_continue( EM_lineNum ); ;}
    break;

  case 62:
#line 327 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), NULL, EM_lineNum ); ;}
    break;

  case 63:
#line 329 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt), EM_lineNum ); ;}
    break;

  case 64:
#line 334 "chuck.y"
    { (yyval.stmt) = new_stmt_from_while( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), EM_lineNum ); ;}
    break;

  case 65:
#line 336 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_while( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), EM_lineNum ); ;}
    break;

  case 66:
#line 338 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (6)].stmt), (yyvsp[(4) - (6)].stmt), NULL, (yyvsp[(6) - (6)].stmt), EM_lineNum ); ;}
    break;

  case 67:
#line 340 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (7)].stmt), (yyvsp[(4) - (7)].stmt), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), EM_lineNum ); ;}
    break;

  case 68:
#line 342 "chuck.y"
    { (yyval.stmt) = new_stmt_from_until( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), EM_lineNum ); ;}
    break;

  case 69:
#line 344 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_until( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), EM_lineNum ); ;}
    break;

  case 70:
#line 346 "chuck.y"
    { (yyval.stmt) = new_stmt_from_loop( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), EM_lineNum ); ;}
    break;

  case 71:
#line 350 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( NULL, EM_lineNum ); ;}
    break;

  case 72:
#line 351 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( (yyvsp[(2) - (3)].stmt_list), EM_lineNum ); ;}
    break;

  case 73:
#line 355 "chuck.y"
    { (yyval.stmt) = NULL; ;}
    break;

  case 74:
#line 356 "chuck.y"
    { (yyval.stmt) = new_stmt_from_expression( (yyvsp[(1) - (2)].exp), EM_lineNum ); ;}
    break;

  case 75:
#line 360 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 76:
#line 361 "chuck.y"
    { (yyval.exp) = append_expression( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 77:
#line 365 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 78:
#line 367 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 79:
#line 371 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 80:
#line 373 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 81:
#line 377 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 82:
#line 379 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(4) - (4)].array_sub), (yyvsp[(2) - (4)].exp), EM_lineNum ); ;}
    break;

  case 83:
#line 383 "chuck.y"
    { (yyval.array_sub) = new_array_sub( NULL, EM_lineNum ); ;}
    break;

  case 84:
#line 384 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(1) - (3)].array_sub), NULL, EM_lineNum ); ;}
    break;

  case 85:
#line 388 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 86:
#line 389 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl_list), 0, EM_lineNum ); ;}
    break;

  case 87:
#line 390 "chuck.y"
    { (yyval.exp) = new_exp_decl_external( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, EM_lineNum ); ;}
    break;

  case 88:
#line 391 "chuck.y"
    { (yyval.exp) = new_exp_decl_global( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, EM_lineNum ); ;}
    break;

  case 89:
#line 392 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 1, EM_lineNum ); ;}
    break;

  case 90:
#line 393 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(2) - (2)].var_decl_list), 0, EM_lineNum ); ;}
    break;

  case 91:
#line 394 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(3) - (3)].var_decl_list), 1, EM_lineNum ); ;}
    break;

  case 92:
#line 398 "chuck.y"
    { (yyval.var_decl_list) = new_var_decl_list( (yyvsp[(1) - (1)].var_decl), EM_lineNum ); ;}
    break;

  case 93:
#line 399 "chuck.y"
    { (yyval.var_decl_list) = prepend_var_decl_list( (yyvsp[(1) - (3)].var_decl), (yyvsp[(3) - (3)].var_decl_list), EM_lineNum ); ;}
    break;

  case 94:
#line 403 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (1)].sval), NULL, EM_lineNum ); ;}
    break;

  case 95:
#line 404 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), (yyvsp[(2) - (2)].array_sub), EM_lineNum ); ;}
    break;

  case 96:
#line 405 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), (yyvsp[(2) - (2)].array_sub), EM_lineNum ); ;}
    break;

  case 97:
#line 410 "chuck.y"
    { (yyval.complex_exp) = new_complex( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 98:
#line 415 "chuck.y"
    { (yyval.polar_exp) = new_polar( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 99:
#line 420 "chuck.y"
    { (yyval.vec_exp) = new_vec( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 100:
#line 424 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 101:
#line 425 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 102:
#line 426 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 103:
#line 427 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 104:
#line 428 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 105:
#line 429 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 106:
#line 430 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 107:
#line 431 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 108:
#line 432 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 109:
#line 433 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 110:
#line 434 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 111:
#line 435 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 112:
#line 436 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 113:
#line 437 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 114:
#line 441 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 115:
#line 442 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 116:
#line 446 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 117:
#line 448 "chuck.y"
    { (yyval.exp) = new_exp_from_if( (yyvsp[(1) - (5)].exp), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].exp), EM_lineNum ); ;}
    break;

  case 118:
#line 452 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 119:
#line 454 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_or, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 120:
#line 458 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 121:
#line 460 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_and, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 122:
#line 464 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 123:
#line 466 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_or, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 124:
#line 470 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 125:
#line 472 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_xor, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 126:
#line 476 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 127:
#line 478 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_and, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 128:
#line 482 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 129:
#line 484 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_eq, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 130:
#line 486 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_neq, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 131:
#line 490 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 132:
#line 492 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_lt, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 133:
#line 494 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_gt, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 134:
#line 496 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_le, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 135:
#line 498 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_ge, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 136:
#line 502 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 137:
#line 504 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_left, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 138:
#line 506 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_right, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 139:
#line 510 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 140:
#line 512 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_plus, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 141:
#line 514 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_minus, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 142:
#line 518 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 143:
#line 520 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_times, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 144:
#line 522 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_divide, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 145:
#line 524 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_percent, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 146:
#line 528 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 147:
#line 530 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_tilda, (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 148:
#line 534 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 149:
#line 536 "chuck.y"
    { (yyval.exp) = new_exp_from_cast( (yyvsp[(3) - (3)].type_decl), (yyvsp[(1) - (3)].exp), EM_lineNum ); ;}
    break;

  case 150:
#line 540 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 151:
#line 542 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_plusplus, (yyvsp[(2) - (2)].exp), EM_lineNum ); ;}
    break;

  case 152:
#line 544 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_minusminus, (yyvsp[(2) - (2)].exp), EM_lineNum ); ;}
    break;

  case 153:
#line 546 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( (yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].exp), EM_lineNum ); ;}
    break;

  case 154:
#line 548 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_typeof, (yyvsp[(2) - (2)].exp), EM_lineNum ); ;}
    break;

  case 155:
#line 550 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_sizeof, (yyvsp[(2) - (2)].exp), EM_lineNum ); ;}
    break;

  case 156:
#line 552 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (2)].type_decl), NULL, EM_lineNum ); ;}
    break;

  case 157:
#line 554 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].array_sub), EM_lineNum ); ;}
    break;

  case 158:
#line 560 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 159:
#line 561 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 160:
#line 562 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 161:
#line 563 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 162:
#line 564 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 163:
#line 565 "chuck.y"
    { (yyval.ival) = ae_op_spork; ;}
    break;

  case 165:
#line 572 "chuck.y"
    { (yyval.exp) = new_exp_from_dur( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), EM_lineNum ); ;}
    break;

  case 166:
#line 576 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 167:
#line 578 "chuck.y"
    { (yyval.exp) = new_exp_from_array( (yyvsp[(1) - (2)].exp), (yyvsp[(2) - (2)].array_sub), EM_lineNum ); ;}
    break;

  case 168:
#line 580 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (3)].exp), NULL, EM_lineNum ); ;}
    break;

  case 169:
#line 582 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (4)].exp), (yyvsp[(3) - (4)].exp), EM_lineNum ); ;}
    break;

  case 170:
#line 584 "chuck.y"
    { (yyval.exp) = new_exp_from_member_dot( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].sval), EM_lineNum ); ;}
    break;

  case 171:
#line 586 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_plusplus, EM_lineNum ); ;}
    break;

  case 172:
#line 588 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_minusminus, EM_lineNum ); ;}
    break;

  case 173:
#line 593 "chuck.y"
    { (yyval.exp) = new_exp_from_id( (yyvsp[(1) - (1)].sval), EM_lineNum ); ;}
    break;

  case 174:
#line 594 "chuck.y"
    { (yyval.exp) = new_exp_from_int( (yyvsp[(1) - (1)].ival), EM_lineNum ); ;}
    break;

  case 175:
#line 595 "chuck.y"
    { (yyval.exp) = new_exp_from_float( (yyvsp[(1) - (1)].fval), EM_lineNum ); ;}
    break;

  case 176:
#line 596 "chuck.y"
    { (yyval.exp) = new_exp_from_str( (yyvsp[(1) - (1)].sval), EM_lineNum ); ;}
    break;

  case 177:
#line 597 "chuck.y"
    { (yyval.exp) = new_exp_from_char( (yyvsp[(1) - (1)].sval), EM_lineNum ); ;}
    break;

  case 178:
#line 598 "chuck.y"
    { (yyval.exp) = new_exp_from_array_lit( (yyvsp[(1) - (1)].array_sub), EM_lineNum ); ;}
    break;

  case 179:
#line 599 "chuck.y"
    { (yyval.exp) = new_exp_from_complex( (yyvsp[(1) - (1)].complex_exp), EM_lineNum ); ;}
    break;

  case 180:
#line 600 "chuck.y"
    { (yyval.exp) = new_exp_from_polar( (yyvsp[(1) - (1)].polar_exp), EM_lineNum ); ;}
    break;

  case 181:
#line 601 "chuck.y"
    { (yyval.exp) = new_exp_from_vec( (yyvsp[(1) - (1)].vec_exp), EM_lineNum ); ;}
    break;

  case 182:
#line 602 "chuck.y"
    { (yyval.exp) = new_exp_from_hack( (yyvsp[(2) - (3)].exp), EM_lineNum ); ;}
    break;

  case 183:
#line 603 "chuck.y"
    { (yyval.exp) = (yyvsp[(2) - (3)].exp); ;}
    break;

  case 184:
#line 604 "chuck.y"
    { (yyval.exp) = new_exp_from_nil( EM_lineNum ); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3051 "chuck.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



#line 2 "chuck.yy.c"

#line 4 "chuck.yy.c"

#define  YY_INT_ALIGNED short int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 5
#define YY_FLEX_SUBMINOR_VERSION 35
#if YY_FLEX_SUBMINOR_VERSION > 0
#define FLEX_BETA
#endif

/* First, we deal with  platform-specific or compiler-specific issues. */

/* begin standard C headers. */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* end standard C headers. */

/* flex integer type definitions */

#ifndef FLEXINT_H
#define FLEXINT_H

/* C99 systems have <inttypes.h>. Non-C99 systems may or may not. */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

/* C99 says to define __STDC_LIMIT_MACROS before including stdint.h,
 * if you want the limit (max/min) macros for int types. 
 */
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif

#include <inttypes.h>
typedef int8_t flex_int8_t;
typedef uint8_t flex_uint8_t;
typedef int16_t flex_int16_t;
typedef uint16_t flex_uint16_t;
typedef int32_t flex_int32_t;
typedef uint32_t flex_uint32_t;
typedef uint64_t flex_uint64_t;
#else
typedef signed char flex_int8_t;
typedef short int flex_int16_t;
typedef int flex_int32_t;
typedef unsigned char flex_uint8_t; 
typedef unsigned short int flex_uint16_t;
typedef unsigned int flex_uint32_t;
#endif /* ! C99 */

/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif

#endif /* ! FLEXINT_H */

#ifdef __cplusplus

/* The "const" storage-class-modifier is valid. */
#define YY_USE_CONST

#else	/* ! __cplusplus */

/* C99 requires __STDC__ to be defined as 1. */
#if defined (__STDC__)

#define YY_USE_CONST

#endif	/* defined (__STDC__) */
#endif	/* ! __cplusplus */

#ifdef YY_USE_CONST
#define yyconst const
#else
#define yyconst
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an unsigned
 * integer for use as an array index.  If the signed char is negative,
 * we want to instead treat it as an 8-bit unsigned char, hence the
 * double cast.
 */
#define YY_SC_TO_UI(c) ((unsigned int) (unsigned char) c)

/* Enter a start condition.  This macro really ought to take a parameter,
 * but we do it the disgusting crufty way forced on us by the ()-less
 * definition of BEGIN.
 */
#define BEGIN (yy_start) = 1 + 2 *

/* Translate the current start state into a value that can be later handed
 * to BEGIN to return to the state.  The YYSTATE alias is for lex
 * compatibility.
 */
#define YY_START (((yy_start) - 1) / 2)
#define YYSTATE YY_START

/* Action number for EOF rule of a given start state. */
#define YY_STATE_EOF(state) (YY_END_OF_BUFFER + state + 1)

/* Special action meaning "start processing a new file". */
#define YY_NEW_FILE yyrestart(yyin  )

#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

/* The state buf must be large enough to hold one state per character in the main buffer.
 */
#define YY_STATE_BUF_SIZE   ((YY_BUF_SIZE + 2) * sizeof(yy_state_type))

#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif

#ifndef YY_TYPEDEF_YY_SIZE_T
#define YY_TYPEDEF_YY_SIZE_T
typedef size_t yy_size_t;
#endif

extern yy_size_t yyleng;

extern FILE *yyin, *yyout;

#define EOB_ACT_CONTINUE_SCAN 0
#define EOB_ACT_END_OF_FILE 1
#define EOB_ACT_LAST_MATCH 2

    #define YY_LESS_LINENO(n)
    
/* Return all but the first "n" matched characters back to the input stream. */
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		*yy_cp = (yy_hold_char); \
		YY_RESTORE_YY_MORE_OFFSET \
		(yy_c_buf_p) = yy_cp = yy_bp + yyless_macro_arg - YY_MORE_ADJ; \
		YY_DO_BEFORE_ACTION; /* set up yytext again */ \
		} \
	while ( 0 )

#define unput(c) yyunput( c, (yytext_ptr)  )

#ifndef YY_STRUCT_YY_BUFFER_STATE
#define YY_STRUCT_YY_BUFFER_STATE
struct yy_buffer_state
	{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	yy_size_t yy_n_chars;

	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;

	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;

	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;

    int yy_bs_lineno; /**< The line count. */
    int yy_bs_column; /**< The column count. */
    
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;

	int yy_buffer_status;

#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2

	};
#endif /* !YY_STRUCT_YY_BUFFER_STATE */

/* Stack of input buffers. */
static size_t yy_buffer_stack_top = 0; /**< index of top of stack. */
static size_t yy_buffer_stack_max = 0; /**< capacity of stack. */
static YY_BUFFER_STATE * yy_buffer_stack = 0; /**< Stack as an array. */

/* We provide macros for accessing buffer states in case in the
 * future we want to put the buffer states in a more general
 * "scanner state".
 *
 * Returns the top of the stack, or NULL.
 */
#define YY_CURRENT_BUFFER ( (yy_buffer_stack) \
                          ? (yy_buffer_stack)[(yy_buffer_stack_top)] \
                          : NULL)

/* Same as previous macro, but useful when we know that the buffer stack is not
 * NULL or when we need an lvalue. For internal use only.
 */
#define YY_CURRENT_BUFFER_LVALUE (yy_buffer_stack)[(yy_buffer_stack_top)]

/* yy_hold_char holds the character lost when yytext is formed. */
static char yy_hold_char;
static yy_size_t yy_n_chars;		/* number of characters read into yy_ch_buf */
yy_size_t yyleng;

/* Points to current character in buffer. */
static char *yy_c_buf_p = (char *) 0;
static int yy_init = 0;		/* whether we need to initialize */
static int yy_start = 0;	/* start state number */

/* Flag which is used to allow yywrap()'s to do buffer switches
 * instead of setting up a fresh yyin.  A bit of a hack ...
 */
static int yy_did_buffer_switch_on_eof;

void yyrestart (FILE *input_file  );
void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer  );
YY_BUFFER_STATE yy_create_buffer (FILE *file,int size  );
void yy_delete_buffer (YY_BUFFER_STATE b  );
void yy_flush_buffer (YY_BUFFER_STATE b  );
void yypush_buffer_state (YY_BUFFER_STATE new_buffer  );
void yypop_buffer_state (void );

static void yyensure_buffer_stack (void );
static void yy_load_buffer_state (void );
static void yy_init_buffer (YY_BUFFER_STATE b,FILE *file  );

#define YY_FLUSH_BUFFER yy_flush_buffer(YY_CURRENT_BUFFER )

YY_BUFFER_STATE yy_scan_buffer (char *base,yy_size_t size  );
YY_BUFFER_STATE yy_scan_string (yyconst char *yy_str  );
YY_BUFFER_STATE yy_scan_bytes (yyconst char *bytes,yy_size_t len  );

void *yyalloc (yy_size_t  );
void *yyrealloc (void *,yy_size_t  );
void yyfree (void *  );

#define yy_new_buffer yy_create_buffer

#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){ \
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer(yyin,YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
	}

#define yy_set_bol(at_bol) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){\
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer(yyin,YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_at_bol = at_bol; \
	}

#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yy_at_bol)

/* Begin user sect3 */

typedef unsigned char YY_CHAR;

FILE *yyin = (FILE *) 0, *yyout = (FILE *) 0;

typedef int yy_state_type;

extern int yylineno;

int yylineno = 1;

extern char *yytext;
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state (void );
static yy_state_type yy_try_NUL_trans (yy_state_type current_state  );
static int yy_get_next_buffer (void );
static void yy_fatal_error (yyconst char msg[]  );

/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	(yytext_ptr) = yy_bp; \
	yyleng = (yy_size_t) (yy_cp - yy_bp); \
	(yy_hold_char) = *yy_cp; \
	*yy_cp = '\0'; \
	(yy_c_buf_p) = yy_cp;

#define YY_NUM_RULES 105
#define YY_END_OF_BUFFER 106
/* This struct is not used in this scanner,
   but its presence is necessary. */
struct yy_trans_info
	{
	flex_int32_t yy_verify;
	flex_int32_t yy_nxt;
	};
static yyconst flex_int16_t yy_accept[243] =
    {   0,
        0,    0,  106,  104,    5,    7,  104,    4,   46,  104,
       21,   22,   20,   32,  104,   38,   39,   18,   16,   13,
       17,   15,   19,   98,   98,   14,   44,   26,   37,   27,
       45,   92,  100,   40,   41,   34,  104,  100,  100,  100,
      100,  100,  100,  100,  100,  100,  100,  100,  100,  100,
      100,   42,   33,   43,   47,    6,   25,    0,  101,    0,
       10,   11,    0,   30,    0,    0,    0,    0,    8,    0,
        9,    0,   94,   99,    3,    1,    0,   99,   98,    0,
       98,    0,   23,   95,   36,   28,   78,   24,   77,   80,
       29,   35,   12,    0,   93,  100,    0,    0,    0,  102,

      100,  100,  100,   56,  100,  100,  100,  100,  100,   54,
      100,  100,  100,  100,  100,  100,  100,  100,  100,  100,
      100,    0,   31,   79,   91,   86,  103,   84,   82,   83,
       85,   99,   97,   96,    2,   57,    0,    0,   58,   81,
       88,  100,  100,  100,  100,  100,   48,   61,  100,  100,
      100,   62,  100,  100,  100,  100,  100,  100,  100,  100,
      100,  100,  100,   87,   97,   96,   90,   89,  100,  100,
      100,  100,   55,  100,  100,  100,  100,  100,  100,  100,
      100,   71,  100,  100,  100,  100,  100,  100,  100,   53,
       63,   72,  100,  100,  100,  100,  100,  100,  100,  100,

      100,  100,  100,  100,   73,  100,  100,   50,   49,  100,
      100,  100,  100,   76,  100,  100,  100,  100,   67,   51,
       59,   70,   74,  100,   65,  100,  100,  100,  100,   69,
      100,   52,   75,   60,  100,  100,  100,  100,   64,   68,
       66,    0
    } ;

static yyconst flex_int32_t yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    4,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   22,
       22,   22,   22,   22,   22,   23,   23,   24,   25,   26,
       27,   28,   29,   30,   31,   31,   32,   31,   31,   31,
       33,   33,   33,   33,   33,   34,   33,   33,   33,   33,
       33,   33,   33,   33,   35,   33,   33,   36,   33,   33,
       37,   38,   39,   40,   33,   41,   42,   43,   44,   45,

       46,   47,   48,   49,   50,   33,   51,   52,   53,   54,
       55,   56,   33,   57,   58,   59,   60,   61,   62,   63,
       64,   33,   65,   66,   67,   68,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,

        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;

static yyconst flex_int32_t yy_meta[69] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    1,    1,    1,    1,    1,    1,    1,    1,
        3,    3,    3,    1,    1,    1,    1,    1,    1,    1,
        3,    3,    4,    4,    4,    4,    1,    1,    1,    1,
        1,    3,    3,    3,    3,    3,    3,    4,    4,    4,
        4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
        4,    4,    4,    4,    1,    1,    1,    1
    } ;

static yyconst flex_int16_t yy_base[248] =
    {   0,
        0,    0,  326,  327,  327,  327,  322,  327,  297,   62,
      310,  327,   57,   60,  284,  327,  327,  294,   56,  327,
       58,   52,   62,   82,   72,  296,  327,   70,   52,   71,
      327,   95,    0,  327,  327,  292,   71,  261,   58,  262,
       67,   60,  264,   74,  269,   76,  268,   79,  249,  258,
      262,  327,   63,  327,  327,  327,  282,  116,  327,  306,
      327,  327,  280,  327,  279,  294,   99,  277,  327,  276,
      327,  275,  327,  118,  327,  327,  274,  125,  130,  122,
      121,    0,  327,  283,  131,  327,  327,  327,  327,  327,
      327,  132,  327,  272,  327,    0,  271,  125,  295,  327,

      251,  254,  241,    0,  236,  234,  235,  237,  235,    0,
      233,  229,  225,  112,  118,  112,  231,  243,  228,  224,
      232,  253,  327,  327,  327,  327,  327,  327,  327,  327,
      327,  155,  158,  135,  327,  327,  252,  251,  327,  327,
      327,  236,  219,  125,  230,  229,    0,  230,  230,  220,
      225,    0,  209,  210,  216,  221,  220,  205,  207,  204,
      216,  211,  208,  327,  151,  154,  327,  327,  208,  200,
      198,  206,    0,  137,  196,  212,  207,  195,  209,  204,
      199,    0,  206,  190,  195,  195,  189,  191,  196,    0,
        0,    0,  187,  195,  185,  188,  185,  183,  188,  175,

      189,  180,  158,  162,    0,  171,  166,    0,    0,  152,
      151,  166,  152,    0,  159,  162,  156,  142,    0,    0,
        0,    0,    0,  154,    0,  147,  144,  143,  152,    0,
      128,    0,    0,    0,  113,  104,   92,   73,    0,    0,
        0,  327,  218,  222,  224,  228,   78
    } ;

static yyconst flex_int16_t yy_def[248] =
    {   0,
      242,    1,  242,  242,  242,  242,  242,  242,  242,  243,
      242,  242,  242,  242,  244,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  245,  242,  242,  242,  246,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  242,  242,  242,  242,  242,  242,  243,  242,  243,
      242,  242,  242,  242,  242,  242,  244,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  247,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  245,  242,  246,  246,  242,

      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  247,  242,  242,  242,  242,  242,  242,
      242,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  242,  242,  242,  242,  242,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,

      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,  245,  245,  245,  245,  245,  245,  245,  245,  245,
      245,    0,  242,  242,  242,  242,  242
    } ;

static yyconst flex_int16_t yy_nxt[396] =
    {   0,
        4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
       24,   25,   25,   26,   27,   28,   29,   30,   31,   32,
       33,   33,   33,   33,   33,   33,   34,    4,   35,   36,
       37,   33,   38,   39,   40,   41,   42,   43,   33,   44,
       33,   33,   33,   45,   33,   46,   47,   48,   49,   50,
       33,   51,   33,   33,   52,   53,   54,   55,   59,   62,
       64,   69,   74,   74,   74,   71,   75,   87,   88,   89,
      134,   76,   70,   63,   72,   73,   65,   84,   77,  122,
       78,   90,   79,   79,   79,   85,   86,   91,   92,   60,

       78,  242,   79,   79,   79,   81,   81,   93,   99,  102,
       66,  100,  103,   80,  107,   81,   81,   82,  105,  108,
      110,   94,   59,   81,   95,   80,  111,  112,  123,  106,
      241,   81,  114,   81,  117,  115,  240,  118,   74,   74,
       74,   81,  133,  133,   82,  132,  132,  132,   78,  239,
       79,   79,   79,   60,   81,   81,  136,  137,  138,  139,
      155,  153,   99,   81,   81,  100,  154,  157,  166,  166,
      158,  238,   81,  237,  156,  132,  132,  132,  133,  133,
       81,   81,  171,  172,  165,  165,  166,  166,  166,   81,
      194,  165,  165,  195,  166,  236,  235,  234,  233,  232,

      231,  230,  165,  229,  228,  166,  227,  226,  225,  165,
      165,  224,  223,  166,  222,  221,  220,  165,   58,   58,
       58,   58,   66,  219,   66,   66,   96,   96,   98,   98,
       98,   98,  218,  217,  216,  215,  214,  213,  212,  211,
      210,  209,  208,  207,  206,  205,  204,  203,  202,  201,
      200,  199,  198,  197,  196,  193,  192,  191,  190,  189,
      188,  187,  186,  185,  184,  183,  182,  181,  180,  179,
      178,  177,  176,  175,  174,  173,  170,  169,  168,  167,
      164,  163,  162,  161,  160,  159,  152,  151,  150,  149,
      148,  147,  146,  145,  144,  143,  142,  242,  141,  140,

      135,  131,  130,  129,  128,  127,  126,  125,  242,  124,
      121,  120,  119,  116,  113,  109,  104,  101,   97,   83,
       68,   67,   61,   57,   56,  242,    3,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242

    } ;

static yyconst flex_int16_t yy_chk[396] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,   10,   13,
       14,   19,   22,   22,   22,   21,   23,   29,   29,   29,
      247,   23,   19,   13,   21,   21,   14,   28,   23,   53,
       25,   29,   25,   25,   25,   28,   28,   30,   30,   10,

       24,   67,   24,   24,   24,   25,   25,   32,   37,   39,
       67,   37,   39,   24,   42,   24,   24,   24,   41,   42,
       44,   32,   58,   25,   32,   24,   44,   44,   53,   41,
      238,   25,   46,   24,   48,   46,  237,   48,   74,   74,
       74,   24,   80,   80,   24,   78,   78,   78,   79,  236,
       79,   79,   79,   58,   81,   81,   85,   85,   92,   92,
      115,  114,   98,   79,   79,   98,  114,  116,  134,  134,
      116,  235,   81,  231,  115,  132,  132,  132,  133,  133,
       81,   79,  144,  144,  165,  165,  134,  166,  166,   79,
      174,  133,  133,  174,  134,  229,  228,  227,  226,  224,

      218,  217,  165,  216,  215,  166,  213,  212,  211,  133,
      165,  210,  207,  166,  206,  204,  203,  133,  243,  243,
      243,  243,  244,  202,  244,  244,  245,  245,  246,  246,
      246,  246,  201,  200,  199,  198,  197,  196,  195,  194,
      193,  189,  188,  187,  186,  185,  184,  183,  181,  180,
      179,  178,  177,  176,  175,  172,  171,  170,  169,  163,
      162,  161,  160,  159,  158,  157,  156,  155,  154,  153,
      151,  150,  149,  148,  146,  145,  143,  142,  138,  137,
      122,  121,  120,  119,  118,  117,  113,  112,  111,  109,
      108,  107,  106,  105,  103,  102,  101,   99,   97,   94,

       84,   77,   72,   70,   68,   66,   65,   63,   60,   57,
       51,   50,   49,   47,   45,   43,   40,   38,   36,   26,
       18,   15,   11,    9,    7,    3,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242,  242,  242,  242,  242,  242,
      242,  242,  242,  242,  242

    } ;

static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;

extern int yy_flex_debug;
int yy_flex_debug = 0;

/* The intent behind this definition is that it'll catch
 * any uses of REJECT which flex missed.
 */
#define REJECT reject_used_but_not_detected
#define yymore() yymore_used_but_not_detected
#define YY_MORE_ADJ 0
#define YY_RESTORE_YY_MORE_OFFSET
char *yytext;
#line 1 "chuck.lex"
#line 11 "chuck.lex"
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
// file: chuck.yy.c
// desc: chuck lexer
//
// author: Ge Wang (gewang.cs.princeton.edu) - generated by lex
//         Perry R. Cook (prc@cs.princeton.edu)
//
// based on the ansi C grammar by Jeff Lee, maintained by Jutta Degener 
// 
// date: Summer 2002
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include "chuck_utils.h"
#include "chuck_absyn.h"
#include "chuck_errmsg.h"

#if !defined(__PLATFORM_WIN32__) && !defined(__ANDROID__)
  #include "chuck.tab.h"
#else
  #include "chuck_win32.h"
#endif



// globals
extern YYSTYPE yylval;
int char_pos = 1;

// define error handling
#define YY_FATAL_ERROR(msg) EM_error2( 0, msg )

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

  int yywrap(void);
  void adjust();
  c_str strip_lit( c_str str );
  c_str alloc_str( c_str str );
  long htol( c_str str );
  int comment();
  int block_comment();

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

// yywrap()
int yywrap(void )
{
    char_pos = 1;
    return 1;
}

// adjust()
void adjust()
{
    EM_tokPos = char_pos;
    char_pos += yyleng;
}

// strip
c_str strip_lit( c_str str )
{
    str[strlen(str)-1] = '\0';
    return str+1;
}

// alloc_str()
c_str alloc_str( c_str str )
{
    c_str s = (c_str)malloc( strlen(str) + 1 );
    strcpy( s, str );

    return s;
}

// to long
long htol( c_str str )
{
    char * c = str;
    unsigned long n = 0;

    // skip over 0x
    c += 2;
    while( *c )
    {
        n <<= 4; 
        switch( *c )
        {
        case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '0':
            n += *c - '0';
            break;

        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            n += *c - 'a' + 10;
            break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            n += *c - 'A' + 10;
            break;
        }    
        c++;
    }

    return n;
}


    
// block comment hack (thanks to unput/yytext_ptr inconsistency)
#define block_comment_hack loop: \
    while ((c = input()) != '*' && c != 0 && c != EOF ) \
        if( c == '\n' ) EM_newline(); \
    if( c == EOF ) adjust(); \
    else if( (c1 = input()) != '/' && c != 0 ) \
    { \
        unput(c1); \
        goto loop; \
    } \
    if( c != 0 ) adjust();

// comment hack
#define comment_hack \
    while ((c = input()) != '\n' && c != '\r' && c != 0 && c != EOF ); \
    if (c != 0) { \
       adjust(); \
       if (c == '\n') EM_newline(); \
    }

// .*\-\->                 { adjust(); continue; }

#line 781 "chuck.yy.c"

#define INITIAL 0

#ifndef YY_NO_UNISTD_H
/* Special case for "unistd.h", since it is non-ANSI. We include it way
 * down here because we want the user's section 1 to have been scanned first.
 * The user has a chance to override it with an option.
 */
//#include <unistd.h>
#endif

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

static int yy_init_globals (void );

/* Accessor methods to globals.
   These are made visible to non-reentrant scanners for convenience. */

int yylex_destroy (void );

int yyget_debug (void );

void yyset_debug (int debug_flag  );

YY_EXTRA_TYPE yyget_extra (void );

void yyset_extra (YY_EXTRA_TYPE user_defined  );

FILE *yyget_in (void );

void yyset_in  (FILE * in_str  );

FILE *yyget_out (void );

void yyset_out  (FILE * out_str  );

yy_size_t yyget_leng (void );

char *yyget_text (void );

int yyget_lineno (void );

void yyset_lineno (int line_number  );

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap (void );
#else
extern int yywrap (void );
#endif
#endif

    static void yyunput (int c,char *buf_ptr  );
    
#ifndef yytext_ptr
static void yy_flex_strncpy (char *,yyconst char *,int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * );
#endif

#ifndef YY_NO_INPUT

#ifdef __cplusplus
static int yyinput (void );
#else
static int input (void );
#endif

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#define YY_READ_BUF_SIZE 8192
#endif

/* Copy whatever the last rule matched to the standard output. */
#ifndef ECHO
/* This used to be an fputs(), but since the string might contain NUL's,
 * we now use fwrite().
 */
#define ECHO fwrite( yytext, yyleng, 1, yyout )
#endif

/* Gets input and stuffs it into "buf".  number of characters read, or YY_NULL,
 * is returned in "result".
 */
#ifndef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) \
		{ \
		int c = '*'; \
		yy_size_t n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else \
		{ \
		errno=0; \
		while ( (result = fread(buf, 1, max_size, yyin))==0 && ferror(yyin)) \
			{ \
			if( errno != EINTR) \
				{ \
				YY_FATAL_ERROR( "input in flex scanner failed" ); \
				break; \
				} \
			errno=0; \
			clearerr(yyin); \
			} \
		}\
\

#endif

/* No semi-colon after return; correct usage is to write "yyterminate();" -
 * we don't want an extra ';' after the "return" because that will cause
 * some compilers to complain about unreachable statements.
 */
#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif

/* Number of entries by which start-condition stack grows. */
#ifndef YY_START_STACK_INCR
#define YY_START_STACK_INCR 25
#endif

/* Report a fatal error. */
#ifndef YY_FATAL_ERROR
#define YY_FATAL_ERROR(msg) yy_fatal_error( msg )
#endif

/* end tables serialization structures and prototypes */

/* Default declaration of generated scanner - a define so the user can
 * easily add parameters.
 */
#ifndef YY_DECL
#define YY_DECL_IS_OURS 1

extern int yylex (void);

#define YY_DECL int yylex (void)
#endif /* !YY_DECL */

/* Code executed at the beginning of each rule, after yytext and yyleng
 * have been set up.
 */
#ifndef YY_USER_ACTION
#define YY_USER_ACTION
#endif

/* Code executed at the end of each rule. */
#ifndef YY_BREAK
#define YY_BREAK break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION

/** The main scanner function which does all the work.
 */
YY_DECL
{
	register yy_state_type yy_current_state;
	register char *yy_cp, *yy_bp;
	register int yy_act;
    
#line 172 "chuck.lex"


#line 966 "chuck.yy.c"

	if ( !(yy_init) )
		{
		(yy_init) = 1;

#ifdef YY_USER_INIT
		YY_USER_INIT;
#endif

		if ( ! (yy_start) )
			(yy_start) = 1;	/* first start state */

		if ( ! yyin )
			yyin = stdin;

		if ( ! yyout )
			yyout = stdout;

		if ( ! YY_CURRENT_BUFFER ) {
			yyensure_buffer_stack ();
			YY_CURRENT_BUFFER_LVALUE =
				yy_create_buffer(yyin,YY_BUF_SIZE );
		}

		yy_load_buffer_state( );
		}

	while ( 1 )		/* loops until end-of-file is reached */
		{
		yy_cp = (yy_c_buf_p);

		/* Support of yytext. */
		*yy_cp = (yy_hold_char);

		/* yy_bp points to the position in yy_ch_buf of the start of
		 * the current run.
		 */
		yy_bp = yy_cp;

		yy_current_state = (yy_start);
yy_match:
		do
			{
			register YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)];
			if ( yy_accept[yy_current_state] )
				{
				(yy_last_accepting_state) = yy_current_state;
				(yy_last_accepting_cpos) = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 243 )
					yy_c = yy_meta[(unsigned int) yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 327 );

yy_find_action:
		yy_act = yy_accept[yy_current_state];
		if ( yy_act == 0 )
			{ /* have to back up */
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			yy_act = yy_accept[yy_current_state];
			}

		YY_DO_BEFORE_ACTION;

do_action:	/* This label is used only to access EOF actions. */

		switch ( yy_act )
	{ /* beginning of action switch */
			case 0: /* must back up */
			/* undo the effects of YY_DO_BEFORE_ACTION */
			*yy_cp = (yy_hold_char);
			yy_cp = (yy_last_accepting_cpos);
			yy_current_state = (yy_last_accepting_state);
			goto yy_find_action;

case 1:
YY_RULE_SETUP
#line 174 "chuck.lex"
{ char c; adjust(); comment_hack; continue; }
	YY_BREAK
case 2:
YY_RULE_SETUP
#line 175 "chuck.lex"
{ char c; adjust(); comment_hack; continue; }
	YY_BREAK
case 3:
YY_RULE_SETUP
#line 176 "chuck.lex"
{ char c, c1; adjust(); block_comment_hack; continue; }
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 177 "chuck.lex"
{ adjust(); continue; }
	YY_BREAK
case 5:
YY_RULE_SETUP
#line 178 "chuck.lex"
{ adjust(); continue; }
	YY_BREAK
case 6:
/* rule 6 can match eol */
YY_RULE_SETUP
#line 179 "chuck.lex"
{ adjust(); EM_newline(); continue; }
	YY_BREAK
case 7:
/* rule 7 can match eol */
YY_RULE_SETUP
#line 180 "chuck.lex"
{ adjust(); EM_newline(); continue; }
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 182 "chuck.lex"
{ adjust(); return PLUSPLUS; }
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 183 "chuck.lex"
{ adjust(); return MINUSMINUS; }
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 184 "chuck.lex"
{ adjust(); return POUNDPAREN; }
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 185 "chuck.lex"
{ adjust(); return PERCENTPAREN; }
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 186 "chuck.lex"
{ adjust(); return ATPAREN; }
	YY_BREAK
case 13:
YY_RULE_SETUP
#line 188 "chuck.lex"
{ adjust(); return COMMA; }
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 189 "chuck.lex"
{ adjust(); return COLON; }
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 190 "chuck.lex"
{ adjust(); return DOT; }
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 191 "chuck.lex"
{ adjust(); return PLUS; }
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 192 "chuck.lex"
{ adjust(); return MINUS; }
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 193 "chuck.lex"
{ adjust(); return TIMES; }
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 194 "chuck.lex"
{ adjust(); return DIVIDE; }
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 195 "chuck.lex"
{ adjust(); return PERCENT; }
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 196 "chuck.lex"
{ adjust(); return POUND; }
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 197 "chuck.lex"
{ adjust(); return DOLLAR; }
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 199 "chuck.lex"
{ adjust(); return COLONCOLON; }
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 200 "chuck.lex"
{ adjust(); return EQ; }
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 201 "chuck.lex"
{ adjust(); return NEQ; }
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 202 "chuck.lex"
{ adjust(); return LT; }
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 203 "chuck.lex"
{ adjust(); return GT; }
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 204 "chuck.lex"
{ adjust(); return LE; }
	YY_BREAK
case 29:
YY_RULE_SETUP
#line 205 "chuck.lex"
{ adjust(); return GE; }
	YY_BREAK
case 30:
YY_RULE_SETUP
#line 206 "chuck.lex"
{ adjust(); return AND; }
	YY_BREAK
case 31:
YY_RULE_SETUP
#line 207 "chuck.lex"
{ adjust(); return OR; }
	YY_BREAK
case 32:
YY_RULE_SETUP
#line 208 "chuck.lex"
{ adjust(); return S_AND; }
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 209 "chuck.lex"
{ adjust(); return S_OR; }
	YY_BREAK
case 34:
YY_RULE_SETUP
#line 210 "chuck.lex"
{ adjust(); return S_XOR; }
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 211 "chuck.lex"
{ adjust(); return SHIFT_RIGHT; }
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 212 "chuck.lex"
{ adjust(); return SHIFT_LEFT; }
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 213 "chuck.lex"
{ adjust(); return ASSIGN; }
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 214 "chuck.lex"
{ adjust(); return LPAREN; }
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 215 "chuck.lex"
{ adjust(); return RPAREN; }
	YY_BREAK
case 40:
YY_RULE_SETUP
#line 216 "chuck.lex"
{ adjust(); return LBRACK; }
	YY_BREAK
case 41:
YY_RULE_SETUP
#line 217 "chuck.lex"
{ adjust(); return RBRACK; }
	YY_BREAK
case 42:
YY_RULE_SETUP
#line 218 "chuck.lex"
{ adjust(); return LBRACE; }
	YY_BREAK
case 43:
YY_RULE_SETUP
#line 219 "chuck.lex"
{ adjust(); return RBRACE; }
	YY_BREAK
case 44:
YY_RULE_SETUP
#line 220 "chuck.lex"
{ adjust(); return SEMICOLON; }
	YY_BREAK
case 45:
YY_RULE_SETUP
#line 221 "chuck.lex"
{ adjust(); return QUESTION; }
	YY_BREAK
case 46:
YY_RULE_SETUP
#line 222 "chuck.lex"
{ adjust(); return EXCLAMATION; }
	YY_BREAK
case 47:
YY_RULE_SETUP
#line 223 "chuck.lex"
{ adjust(); return TILDA; }
	YY_BREAK
case 48:
YY_RULE_SETUP
#line 224 "chuck.lex"
{ adjust(); return FOR; }
	YY_BREAK
case 49:
YY_RULE_SETUP
#line 225 "chuck.lex"
{ adjust(); return WHILE; }
	YY_BREAK
case 50:
YY_RULE_SETUP
#line 226 "chuck.lex"
{ adjust(); return UNTIL; }
	YY_BREAK
case 51:
YY_RULE_SETUP
#line 227 "chuck.lex"
{ adjust(); return LOOP; }
	YY_BREAK
case 52:
YY_RULE_SETUP
#line 228 "chuck.lex"
{ adjust(); return CONTINUE; }
	YY_BREAK
case 53:
YY_RULE_SETUP
#line 229 "chuck.lex"
{ adjust(); return BREAK; }
	YY_BREAK
case 54:
YY_RULE_SETUP
#line 230 "chuck.lex"
{ adjust(); return IF; }
	YY_BREAK
case 55:
YY_RULE_SETUP
#line 231 "chuck.lex"
{ adjust(); return ELSE; }
	YY_BREAK
case 56:
YY_RULE_SETUP
#line 232 "chuck.lex"
{ adjust(); return DO; }
	YY_BREAK
case 57:
YY_RULE_SETUP
#line 233 "chuck.lex"
{ adjust(); return L_HACK; }
	YY_BREAK
case 58:
YY_RULE_SETUP
#line 234 "chuck.lex"
{ adjust(); return R_HACK; }
	YY_BREAK
case 59:
YY_RULE_SETUP
#line 236 "chuck.lex"
{ adjust(); return RETURN; }
	YY_BREAK
case 60:
YY_RULE_SETUP
#line 238 "chuck.lex"
{ adjust(); return FUNCTION; }
	YY_BREAK
case 61:
YY_RULE_SETUP
#line 239 "chuck.lex"
{ adjust(); return FUNCTION; }
	YY_BREAK
case 62:
YY_RULE_SETUP
#line 240 "chuck.lex"
{ adjust(); return NEW; }
	YY_BREAK
case 63:
YY_RULE_SETUP
#line 241 "chuck.lex"
{ adjust(); return CLASS; }
	YY_BREAK
case 64:
YY_RULE_SETUP
#line 242 "chuck.lex"
{ adjust(); return INTERFACE; }
	YY_BREAK
case 65:
YY_RULE_SETUP
#line 243 "chuck.lex"
{ adjust(); return EXTENDS; }
	YY_BREAK
case 66:
YY_RULE_SETUP
#line 244 "chuck.lex"
{ adjust(); return IMPLEMENTS; }
	YY_BREAK
case 67:
YY_RULE_SETUP
#line 245 "chuck.lex"
{ adjust(); return PUBLIC; }
	YY_BREAK
case 68:
YY_RULE_SETUP
#line 246 "chuck.lex"
{ adjust(); return PROTECTED; }
	YY_BREAK
case 69:
YY_RULE_SETUP
#line 247 "chuck.lex"
{ adjust(); return PRIVATE; }
	YY_BREAK
case 70:
YY_RULE_SETUP
#line 248 "chuck.lex"
{ adjust(); return STATIC; }
	YY_BREAK
case 71:
YY_RULE_SETUP
#line 249 "chuck.lex"
{ adjust(); return ABSTRACT; }
	YY_BREAK
case 72:
YY_RULE_SETUP
#line 250 "chuck.lex"
{ adjust(); return CONST; }
	YY_BREAK
case 73:
YY_RULE_SETUP
#line 251 "chuck.lex"
{ adjust(); return SPORK; }
	YY_BREAK
case 74:
YY_RULE_SETUP
#line 252 "chuck.lex"
{ adjust(); return TYPEOF; }
	YY_BREAK
case 75:
YY_RULE_SETUP
#line 253 "chuck.lex"
{ adjust(); return EXTERNAL; }
	YY_BREAK
case 76:
YY_RULE_SETUP
#line 254 "chuck.lex"
{ adjust(); return GLOBAL; }
	YY_BREAK
case 77:
YY_RULE_SETUP
#line 256 "chuck.lex"
{ adjust(); return CHUCK; }
	YY_BREAK
case 78:
YY_RULE_SETUP
#line 257 "chuck.lex"
{ adjust(); return UNCHUCK; }
	YY_BREAK
case 79:
YY_RULE_SETUP
#line 258 "chuck.lex"
{ adjust(); return UNCHUCK; }
	YY_BREAK
case 80:
YY_RULE_SETUP
#line 259 "chuck.lex"
{ adjust(); return UPCHUCK; }
	YY_BREAK
case 81:
YY_RULE_SETUP
#line 260 "chuck.lex"
{ adjust(); return AT_CHUCK; }
	YY_BREAK
case 82:
YY_RULE_SETUP
#line 261 "chuck.lex"
{ adjust(); return PLUS_CHUCK; }
	YY_BREAK
case 83:
YY_RULE_SETUP
#line 262 "chuck.lex"
{ adjust(); return MINUS_CHUCK; }
	YY_BREAK
case 84:
YY_RULE_SETUP
#line 263 "chuck.lex"
{ adjust(); return TIMES_CHUCK; }
	YY_BREAK
case 85:
YY_RULE_SETUP
#line 264 "chuck.lex"
{ adjust(); return DIVIDE_CHUCK; }
	YY_BREAK
case 86:
YY_RULE_SETUP
#line 265 "chuck.lex"
{ adjust(); return S_AND_CHUCK; }
	YY_BREAK
case 87:
YY_RULE_SETUP
#line 266 "chuck.lex"
{ adjust(); return S_OR_CHUCK; }
	YY_BREAK
case 88:
YY_RULE_SETUP
#line 267 "chuck.lex"
{ adjust(); return S_XOR_CHUCK; }
	YY_BREAK
case 89:
YY_RULE_SETUP
#line 268 "chuck.lex"
{ adjust(); return SHIFT_RIGHT_CHUCK; }
	YY_BREAK
case 90:
YY_RULE_SETUP
#line 269 "chuck.lex"
{ adjust(); return SHIFT_LEFT_CHUCK; }
	YY_BREAK
case 91:
YY_RULE_SETUP
#line 270 "chuck.lex"
{ adjust(); return PERCENT_CHUCK; }
	YY_BREAK
case 92:
YY_RULE_SETUP
#line 271 "chuck.lex"
{ adjust(); return AT_SYM; }
	YY_BREAK
case 93:
YY_RULE_SETUP
#line 272 "chuck.lex"
{ adjust(); return ATAT_SYM; }
	YY_BREAK
case 94:
YY_RULE_SETUP
#line 273 "chuck.lex"
{ adjust(); return ARROW_RIGHT; }
	YY_BREAK
case 95:
YY_RULE_SETUP
#line 274 "chuck.lex"
{ adjust(); return ARROW_LEFT; }
	YY_BREAK
case 96:
YY_RULE_SETUP
#line 276 "chuck.lex"
{ adjust(); yylval.ival=htol(yytext); return NUM; }
	YY_BREAK
case 97:
YY_RULE_SETUP
#line 277 "chuck.lex"
{ adjust(); yylval.ival=atoi(yytext); return NUM; }
	YY_BREAK
case 98:
YY_RULE_SETUP
#line 278 "chuck.lex"
{ adjust(); yylval.ival=atoi(yytext); return NUM; }
	YY_BREAK
case 99:
YY_RULE_SETUP
#line 279 "chuck.lex"
{ adjust(); yylval.fval=atof(yytext); return FLOAT; }
	YY_BREAK
case 100:
YY_RULE_SETUP
#line 280 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(yytext); return ID; }
	YY_BREAK
case 101:
/* rule 101 can match eol */
YY_RULE_SETUP
#line 281 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
	YY_BREAK
case 102:
/* rule 102 can match eol */
YY_RULE_SETUP
#line 282 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
	YY_BREAK
case 103:
/* rule 103 can match eol */
YY_RULE_SETUP
#line 283 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return CHAR_LIT; }
	YY_BREAK
case 104:
YY_RULE_SETUP
#line 285 "chuck.lex"
{ adjust(); EM_error( EM_tokPos, "illegal token" ); }
	YY_BREAK
case 105:
YY_RULE_SETUP
#line 287 "chuck.lex"
ECHO;
	YY_BREAK
#line 1579 "chuck.yy.c"
case YY_STATE_EOF(INITIAL):
	yyterminate();

	case YY_END_OF_BUFFER:
		{
		/* Amount of text matched not including the EOB char. */
		int yy_amount_of_matched_text = (int) (yy_cp - (yytext_ptr)) - 1;

		/* Undo the effects of YY_DO_BEFORE_ACTION. */
		*yy_cp = (yy_hold_char);
		YY_RESTORE_YY_MORE_OFFSET

		if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_NEW )
			{
			/* We're scanning a new file or input source.  It's
			 * possible that this happened because the user
			 * just pointed yyin at a new source and called
			 * yylex().  If so, then we have to assure
			 * consistency between YY_CURRENT_BUFFER and our
			 * globals.  Here is the right place to do so, because
			 * this is the first action (other than possibly a
			 * back-up) that will match for the new input source.
			 */
			(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
			YY_CURRENT_BUFFER_LVALUE->yy_input_file = yyin;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status = YY_BUFFER_NORMAL;
			}

		/* Note that here we test for yy_c_buf_p "<=" to the position
		 * of the first EOB in the buffer, since yy_c_buf_p will
		 * already have been incremented past the NUL character
		 * (since all states make transitions on EOB to the
		 * end-of-buffer state).  Contrast this with the test
		 * in input().
		 */
		if ( (yy_c_buf_p) <= &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			{ /* This was really a NUL. */
			yy_state_type yy_next_state;

			(yy_c_buf_p) = (yytext_ptr) + yy_amount_of_matched_text;

			yy_current_state = yy_get_previous_state(  );

			/* Okay, we're now positioned to make the NUL
			 * transition.  We couldn't have
			 * yy_get_previous_state() go ahead and do it
			 * for us because it doesn't know how to deal
			 * with the possibility of jamming (and we don't
			 * want to build jamming into it because then it
			 * will run more slowly).
			 */

			yy_next_state = yy_try_NUL_trans( yy_current_state );

			yy_bp = (yytext_ptr) + YY_MORE_ADJ;

			if ( yy_next_state )
				{
				/* Consume the NUL. */
				yy_cp = ++(yy_c_buf_p);
				yy_current_state = yy_next_state;
				goto yy_match;
				}

			else
				{
				yy_cp = (yy_c_buf_p);
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer(  ) )
			{
			case EOB_ACT_END_OF_FILE:
				{
				(yy_did_buffer_switch_on_eof) = 0;

				if ( yywrap( ) )
					{
					/* Note: because we've taken care in
					 * yy_get_next_buffer() to have set up
					 * yytext, we can now set up
					 * yy_c_buf_p so that if some total
					 * hoser (like flex itself) wants to
					 * call the scanner after we return the
					 * YY_NULL, it'll still work - another
					 * YY_NULL will get returned.
					 */
					(yy_c_buf_p) = (yytext_ptr) + YY_MORE_ADJ;

					yy_act = YY_STATE_EOF(YY_START);
					goto do_action;
					}

				else
					{
					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
					}
				break;
				}

			case EOB_ACT_CONTINUE_SCAN:
				(yy_c_buf_p) =
					(yytext_ptr) + yy_amount_of_matched_text;

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_match;

			case EOB_ACT_LAST_MATCH:
				(yy_c_buf_p) =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)];

				yy_current_state = yy_get_previous_state(  );

				yy_cp = (yy_c_buf_p);
				yy_bp = (yytext_ptr) + YY_MORE_ADJ;
				goto yy_find_action;
			}
		break;
		}

	default:
		YY_FATAL_ERROR(
			"fatal flex scanner internal error--no action found" );
	} /* end of action switch */
		} /* end of scanning one token */
} /* end of yylex */

/* yy_get_next_buffer - try to read in a new buffer
 *
 * Returns a code representing an action:
 *	EOB_ACT_LAST_MATCH -
 *	EOB_ACT_CONTINUE_SCAN - continue scanning from current position
 *	EOB_ACT_END_OF_FILE - end of file
 */
static int yy_get_next_buffer (void)
{
    	register char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
	register char *source = (yytext_ptr);
	register int number_to_move, i;
	int ret_val;

	if ( (yy_c_buf_p) > &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] )
		YY_FATAL_ERROR(
		"fatal flex scanner internal error--end of buffer missed" );

	if ( YY_CURRENT_BUFFER_LVALUE->yy_fill_buffer == 0 )
		{ /* Don't try to fill the buffer, so this is an EOF. */
		if ( (yy_c_buf_p) - (yytext_ptr) - YY_MORE_ADJ == 1 )
			{
			/* We matched a single character, the EOB, so
			 * treat this as a final EOF.
			 */
			return EOB_ACT_END_OF_FILE;
			}

		else
			{
			/* We matched some text prior to the EOB, first
			 * process it.
			 */
			return EOB_ACT_LAST_MATCH;
			}
		}

	/* Try to read more data. */

	/* First move last chars to start of buffer. */
	number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr)) - 1;

	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);

	if ( YY_CURRENT_BUFFER_LVALUE->yy_buffer_status == YY_BUFFER_EOF_PENDING )
		/* don't do the read, it's not guaranteed to return an EOF,
		 * just force an EOF
		 */
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars) = 0;

	else
		{
			yy_size_t num_to_read =
			YY_CURRENT_BUFFER_LVALUE->yy_buf_size - number_to_move - 1;

		while ( num_to_read <= 0 )
			{ /* Not enough room in the buffer - grow it. */

			/* just a shorter name for the current buffer */
			YY_BUFFER_STATE b = YY_CURRENT_BUFFER;

			int yy_c_buf_p_offset =
				(int) ((yy_c_buf_p) - b->yy_ch_buf);

			if ( b->yy_is_our_buffer )
				{
				yy_size_t new_size = b->yy_buf_size * 2;

				if ( new_size <= 0 )
					b->yy_buf_size += b->yy_buf_size / 8;
				else
					b->yy_buf_size *= 2;

				b->yy_ch_buf = (char *)
					/* Include room in for 2 EOB chars. */
					yyrealloc((void *) b->yy_ch_buf,b->yy_buf_size + 2  );
				}
			else
				/* Can't grow it, we don't own it. */
				b->yy_ch_buf = 0;

			if ( ! b->yy_ch_buf )
				YY_FATAL_ERROR(
				"fatal error - scanner input buffer overflow" );

			(yy_c_buf_p) = &b->yy_ch_buf[yy_c_buf_p_offset];

			num_to_read = YY_CURRENT_BUFFER_LVALUE->yy_buf_size -
						number_to_move - 1;

			}

		if ( num_to_read > YY_READ_BUF_SIZE )
			num_to_read = YY_READ_BUF_SIZE;

		/* Read in more data. */
		YY_INPUT( (&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move]),
			(yy_n_chars), num_to_read );

		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	if ( (yy_n_chars) == 0 )
		{
		if ( number_to_move == YY_MORE_ADJ )
			{
			ret_val = EOB_ACT_END_OF_FILE;
			yyrestart(yyin  );
			}

		else
			{
			ret_val = EOB_ACT_LAST_MATCH;
			YY_CURRENT_BUFFER_LVALUE->yy_buffer_status =
				YY_BUFFER_EOF_PENDING;
			}
		}

	else
		ret_val = EOB_ACT_CONTINUE_SCAN;

	if ((yy_size_t) ((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
		/* Extend the array by 50%, plus the number we really need. */
		yy_size_t new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1);
		YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) yyrealloc((void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf,new_size  );
		if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
	}

	(yy_n_chars) += number_to_move;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] = YY_END_OF_BUFFER_CHAR;
	YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars) + 1] = YY_END_OF_BUFFER_CHAR;

	(yytext_ptr) = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[0];

	return ret_val;
}

/* yy_get_previous_state - get the state just before the EOB char was reached */

    static yy_state_type yy_get_previous_state (void)
{
	register yy_state_type yy_current_state;
	register char *yy_cp;
    
	yy_current_state = (yy_start);

	for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp )
		{
		register YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			(yy_last_accepting_state) = yy_current_state;
			(yy_last_accepting_cpos) = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 243 )
				yy_c = yy_meta[(unsigned int) yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
		}

	return yy_current_state;
}

/* yy_try_NUL_trans - try to make a transition on the NUL character
 *
 * synopsis
 *	next_state = yy_try_NUL_trans( current_state );
 */
    static yy_state_type yy_try_NUL_trans  (yy_state_type yy_current_state )
{
	register int yy_is_jam;
    	register char *yy_cp = (yy_c_buf_p);

	register YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		(yy_last_accepting_state) = yy_current_state;
		(yy_last_accepting_cpos) = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 243 )
			yy_c = yy_meta[(unsigned int) yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
	yy_is_jam = (yy_current_state == 242);

	return yy_is_jam ? 0 : yy_current_state;
}

    static void yyunput (int c, register char * yy_bp )
{
	register char *yy_cp;
    
    yy_cp = (yy_c_buf_p);

	/* undo effects of setting up yytext */
	*yy_cp = (yy_hold_char);

	if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
		{ /* need to shift things up to make room */
		/* +2 for EOB chars. */
		register yy_size_t number_to_move = (yy_n_chars) + 2;
		register char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
					YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
		register char *source =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];

		while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
			(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_buf_size;

		if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;

	(yytext_ptr) = yy_bp;
	(yy_hold_char) = *yy_cp;
	(yy_c_buf_p) = yy_cp;
}

#ifndef YY_NO_INPUT
#ifdef __cplusplus
    static int yyinput (void)
#else
    static int input  (void)
#endif

{
	int c;
    
	*(yy_c_buf_p) = (yy_hold_char);

	if ( *(yy_c_buf_p) == YY_END_OF_BUFFER_CHAR )
		{
		/* yy_c_buf_p now points to the character we want to return.
		 * If this occurs *before* the EOB characters, then it's a
		 * valid NUL; if not, then we've hit the end of the buffer.
		 */
		if ( (yy_c_buf_p) < &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[(yy_n_chars)] )
			/* This was really a NUL. */
			*(yy_c_buf_p) = '\0';

		else
			{ /* need more input */
			yy_size_t offset = (yy_c_buf_p) - (yytext_ptr);
			++(yy_c_buf_p);

			switch ( yy_get_next_buffer(  ) )
				{
				case EOB_ACT_LAST_MATCH:
					/* This happens because yy_g_n_b()
					 * sees that we've accumulated a
					 * token and flags that we need to
					 * try matching the token before
					 * proceeding.  But for input(),
					 * there's no matching to consider.
					 * So convert the EOB_ACT_LAST_MATCH
					 * to EOB_ACT_END_OF_FILE.
					 */

					/* Reset buffer status. */
					yyrestart(yyin );

					/*FALLTHROUGH*/

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap( ) )
						return 0;

					if ( ! (yy_did_buffer_switch_on_eof) )
						YY_NEW_FILE;
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
					}

				case EOB_ACT_CONTINUE_SCAN:
					(yy_c_buf_p) = (yytext_ptr) + offset;
					break;
				}
			}
		}

	c = *(unsigned char *) (yy_c_buf_p);	/* cast for 8-bit char's */
	*(yy_c_buf_p) = '\0';	/* preserve yytext */
	(yy_hold_char) = *++(yy_c_buf_p);

	return c;
}
#endif	/* ifndef YY_NO_INPUT */

/** Immediately switch to a different input stream.
 * @param input_file A readable stream.
 * 
 * @note This function does not reset the start condition to @c INITIAL .
 */
    void yyrestart  (FILE * input_file )
{
    
	if ( ! YY_CURRENT_BUFFER ){
        yyensure_buffer_stack ();
		YY_CURRENT_BUFFER_LVALUE =
            yy_create_buffer(yyin,YY_BUF_SIZE );
	}

	yy_init_buffer(YY_CURRENT_BUFFER,input_file );
	yy_load_buffer_state( );
}

/** Switch to a different input buffer.
 * @param new_buffer The new input buffer.
 * 
 */
    void yy_switch_to_buffer  (YY_BUFFER_STATE  new_buffer )
{
    
	/* TODO. We should be able to replace this entire function body
	 * with
	 *		yypop_buffer_state();
	 *		yypush_buffer_state(new_buffer);
     */
	yyensure_buffer_stack ();
	if ( YY_CURRENT_BUFFER == new_buffer )
		return;

	if ( YY_CURRENT_BUFFER )
		{
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	YY_CURRENT_BUFFER_LVALUE = new_buffer;
	yy_load_buffer_state( );

	/* We don't actually know whether we did this switch during
	 * EOF (yywrap()) processing, but the only time this flag
	 * is looked at is after yywrap() is called, so it's safe
	 * to go ahead and always set it.
	 */
	(yy_did_buffer_switch_on_eof) = 1;
}

static void yy_load_buffer_state  (void)
{
    	(yy_n_chars) = YY_CURRENT_BUFFER_LVALUE->yy_n_chars;
	(yytext_ptr) = (yy_c_buf_p) = YY_CURRENT_BUFFER_LVALUE->yy_buf_pos;
	yyin = YY_CURRENT_BUFFER_LVALUE->yy_input_file;
	(yy_hold_char) = *(yy_c_buf_p);
}

/** Allocate and initialize an input buffer state.
 * @param file A readable stream.
 * @param size The character buffer size in bytes. When in doubt, use @c YY_BUF_SIZE.
 * 
 * @return the allocated buffer state.
 */
    YY_BUFFER_STATE yy_create_buffer  (FILE * file, int  size )
{
	YY_BUFFER_STATE b;
    
	b = (YY_BUFFER_STATE) yyalloc(sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yyalloc(b->yy_buf_size + 2  );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer(b,file );

	return b;
}

/** Destroy the buffer.
 * @param b a buffer created with yy_create_buffer()
 * 
 */
    void yy_delete_buffer (YY_BUFFER_STATE  b )
{
    
	if ( ! b )
		return;

	if ( b == YY_CURRENT_BUFFER ) /* Not sure if we should pop here. */
		YY_CURRENT_BUFFER_LVALUE = (YY_BUFFER_STATE) 0;

	if ( b->yy_is_our_buffer )
		yyfree((void *) b->yy_ch_buf  );

	yyfree((void *) b  );
}

#ifndef __cplusplus
extern int isatty (int );
#endif /* __cplusplus */
    
/* Initializes or reinitializes a buffer.
 * This function is sometimes called more than once on the same buffer,
 * such as during a yyrestart() or at EOF.
 */
    static void yy_init_buffer  (YY_BUFFER_STATE  b, FILE * file )

{
	int oerrno = errno;
    
	yy_flush_buffer(b );

	b->yy_input_file = file;
	b->yy_fill_buffer = 1;

    /* If b is the current buffer, then yy_init_buffer was _probably_
     * called from yyrestart() or through yy_get_next_buffer.
     * In that case, we don't want to reset the lineno or column.
     */
    if (b != YY_CURRENT_BUFFER){
        b->yy_bs_lineno = 1;
        b->yy_bs_column = 0;
    }

        b->yy_is_interactive = file ? (isatty( fileno(file) ) > 0) : 0;
    
	errno = oerrno;
}

/** Discard all buffered characters. On the next scan, YY_INPUT will be called.
 * @param b the buffer state to be flushed, usually @c YY_CURRENT_BUFFER.
 * 
 */
    void yy_flush_buffer (YY_BUFFER_STATE  b )
{
    	if ( ! b )
		return;

	b->yy_n_chars = 0;

	/* We always need two end-of-buffer characters.  The first causes
	 * a transition to the end-of-buffer state.  The second causes
	 * a jam in that state.
	 */
	b->yy_ch_buf[0] = YY_END_OF_BUFFER_CHAR;
	b->yy_ch_buf[1] = YY_END_OF_BUFFER_CHAR;

	b->yy_buf_pos = &b->yy_ch_buf[0];

	b->yy_at_bol = 1;
	b->yy_buffer_status = YY_BUFFER_NEW;

	if ( b == YY_CURRENT_BUFFER )
		yy_load_buffer_state( );
}

/** Pushes the new state onto the stack. The new state becomes
 *  the current state. This function will allocate the stack
 *  if necessary.
 *  @param new_buffer The new state.
 *  
 */
void yypush_buffer_state (YY_BUFFER_STATE new_buffer )
{
    	if (new_buffer == NULL)
		return;

	yyensure_buffer_stack();

	/* This block is copied from yy_switch_to_buffer. */
	if ( YY_CURRENT_BUFFER )
		{
		/* Flush out information for old buffer. */
		*(yy_c_buf_p) = (yy_hold_char);
		YY_CURRENT_BUFFER_LVALUE->yy_buf_pos = (yy_c_buf_p);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars = (yy_n_chars);
		}

	/* Only push if top exists. Otherwise, replace top. */
	if (YY_CURRENT_BUFFER)
		(yy_buffer_stack_top)++;
	YY_CURRENT_BUFFER_LVALUE = new_buffer;

	/* copied from yy_switch_to_buffer. */
	yy_load_buffer_state( );
	(yy_did_buffer_switch_on_eof) = 1;
}

/** Removes and deletes the top of the stack, if present.
 *  The next element becomes the new top.
 *  
 */
void yypop_buffer_state (void)
{
    	if (!YY_CURRENT_BUFFER)
		return;

	yy_delete_buffer(YY_CURRENT_BUFFER );
	YY_CURRENT_BUFFER_LVALUE = NULL;
	if ((yy_buffer_stack_top) > 0)
		--(yy_buffer_stack_top);

	if (YY_CURRENT_BUFFER) {
		yy_load_buffer_state( );
		(yy_did_buffer_switch_on_eof) = 1;
	}
}

/* Allocates the stack if it does not exist.
 *  Guarantees space for at least one push.
 */
static void yyensure_buffer_stack (void)
{
	yy_size_t num_to_alloc;
    
	if (!(yy_buffer_stack)) {

		/* First allocation is just for 2 elements, since we don't know if this
		 * scanner will even need a stack. We use 2 instead of 1 to avoid an
		 * immediate realloc on the next call.
         */
		num_to_alloc = 1;
		(yy_buffer_stack) = (struct yy_buffer_state**)yyalloc
								(num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );
								  
		memset((yy_buffer_stack), 0, num_to_alloc * sizeof(struct yy_buffer_state*));
				
		(yy_buffer_stack_max) = num_to_alloc;
		(yy_buffer_stack_top) = 0;
		return;
	}

	if ((yy_buffer_stack_top) >= ((yy_buffer_stack_max)) - 1){

		/* Increase the buffer to prepare for a possible push. */
		int grow_size = 8 /* arbitrary grow size */;

		num_to_alloc = (yy_buffer_stack_max) + grow_size;
		(yy_buffer_stack) = (struct yy_buffer_state**)yyrealloc
								((yy_buffer_stack),
								num_to_alloc * sizeof(struct yy_buffer_state*)
								);
		if ( ! (yy_buffer_stack) )
			YY_FATAL_ERROR( "out of dynamic memory in yyensure_buffer_stack()" );

		/* zero only the new slots.*/
		memset((yy_buffer_stack) + (yy_buffer_stack_max), 0, grow_size * sizeof(struct yy_buffer_state*));
		(yy_buffer_stack_max) = num_to_alloc;
	}
}

/** Setup the input buffer state to scan directly from a user-specified character buffer.
 * @param base the character buffer
 * @param size the size in bytes of the character buffer
 * 
 * @return the newly allocated buffer state object. 
 */
YY_BUFFER_STATE yy_scan_buffer  (char * base, yy_size_t  size )
{
	YY_BUFFER_STATE b;
    
	if ( size < 2 ||
	     base[size-2] != YY_END_OF_BUFFER_CHAR ||
	     base[size-1] != YY_END_OF_BUFFER_CHAR )
		/* They forgot to leave room for the EOB's. */
		return 0;

	b = (YY_BUFFER_STATE) yyalloc(sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = size - 2;	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = 0;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer(b  );

	return b;
}

/** Setup the input buffer state to scan a string. The next call to yylex() will
 * scan from a @e copy of @a str.
 * @param yystr a NUL-terminated string to scan
 * 
 * @return the newly allocated buffer state object.
 * @note If you want to scan bytes that may contain NUL values, then use
 *       yy_scan_bytes() instead.
 */
YY_BUFFER_STATE yy_scan_string (yyconst char * yystr )
{
    
	return yy_scan_bytes(yystr,strlen(yystr) );
}

/** Setup the input buffer state to scan the given bytes. The next call to yylex() will
 * scan from a @e copy of @a bytes.
 * @param bytes the byte buffer to scan
 * @param len the number of bytes in the buffer pointed to by @a bytes.
 * 
 * @return the newly allocated buffer state object.
 */
YY_BUFFER_STATE yy_scan_bytes  (yyconst char * yybytes, yy_size_t  _yybytes_len )
{
	YY_BUFFER_STATE b;
	char *buf;
	yy_size_t n, i;
    
	/* Get memory for full buffer, including space for trailing EOB's. */
	n = _yybytes_len + 2;
	buf = (char *) yyalloc(n  );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < _yybytes_len; ++i )
		buf[i] = yybytes[i];

	buf[_yybytes_len] = buf[_yybytes_len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer(buf,n );
	if ( ! b )
		YY_FATAL_ERROR( "bad buffer in yy_scan_bytes()" );

	/* It's okay to grow etc. this buffer, and we should throw it
	 * away when we're done.
	 */
	b->yy_is_our_buffer = 1;

	return b;
}

#ifndef YY_EXIT_FAILURE
#define YY_EXIT_FAILURE 2
#endif

static void yy_fatal_error (yyconst char* msg )
{
    	(void) fprintf( stderr, "%s\n", msg );
	exit( YY_EXIT_FAILURE );
}

/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        int yyless_macro_arg = (n); \
        YY_LESS_LINENO(yyless_macro_arg);\
		yytext[yyleng] = (yy_hold_char); \
		(yy_c_buf_p) = yytext + yyless_macro_arg; \
		(yy_hold_char) = *(yy_c_buf_p); \
		*(yy_c_buf_p) = '\0'; \
		yyleng = yyless_macro_arg; \
		} \
	while ( 0 )

/* Accessor  methods (get/set functions) to struct members. */

/** Get the current line number.
 * 
 */
int yyget_lineno  (void)
{
        
    return yylineno;
}

/** Get the input stream.
 * 
 */
FILE *yyget_in  (void)
{
        return yyin;
}

/** Get the output stream.
 * 
 */
FILE *yyget_out  (void)
{
        return yyout;
}

/** Get the length of the current token.
 * 
 */
yy_size_t yyget_leng  (void)
{
        return yyleng;
}

/** Get the current token.
 * 
 */

char *yyget_text  (void)
{
        return yytext;
}

/** Set the current line number.
 * @param line_number
 * 
 */
void yyset_lineno (int  line_number )
{
    
    yylineno = line_number;
}

/** Set the input stream. This does not discard the current
 * input buffer.
 * @param in_str A readable stream.
 * 
 * @see yy_switch_to_buffer
 */
void yyset_in (FILE *  in_str )
{
        yyin = in_str ;
}

void yyset_out (FILE *  out_str )
{
        yyout = out_str ;
}

int yyget_debug  (void)
{
        return yy_flex_debug;
}

void yyset_debug (int  bdebug )
{
        yy_flex_debug = bdebug ;
}

static int yy_init_globals (void)
{
        /* Initialization is the same as for the non-reentrant scanner.
     * This function is called from yylex_destroy(), so don't allocate here.
     */

    (yy_buffer_stack) = 0;
    (yy_buffer_stack_top) = 0;
    (yy_buffer_stack_max) = 0;
    (yy_c_buf_p) = (char *) 0;
    (yy_init) = 0;
    (yy_start) = 0;

/* Defined in main.c */
#ifdef YY_STDINIT
    yyin = stdin;
    yyout = stdout;
#else
    yyin = (FILE *) 0;
    yyout = (FILE *) 0;
#endif

    /* For future reference: Set errno on error, since we are called by
     * yylex_init()
     */
    return 0;
}

/* yylex_destroy is for both reentrant and non-reentrant scanners. */
int yylex_destroy  (void)
{
    
    /* Pop the buffer stack, destroying each element. */
	while(YY_CURRENT_BUFFER){
		yy_delete_buffer(YY_CURRENT_BUFFER  );
		YY_CURRENT_BUFFER_LVALUE = NULL;
		yypop_buffer_state();
	}

	/* Destroy the stack itself. */
	yyfree((yy_buffer_stack) );
	(yy_buffer_stack) = NULL;

    /* Reset the globals. This is important in a non-reentrant scanner so the next time
     * yylex() is called, initialization will occur. */
    yy_init_globals( );

    return 0;
}

/*
 * Internal utility routines.
 */

#ifndef yytext_ptr
static void yy_flex_strncpy (char* s1, yyconst char * s2, int n )
{
	register int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
}
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (yyconst char * s )
{
	register int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
}
#endif

void *yyalloc (yy_size_t  size )
{
	return (void *) malloc( size );
}

void *yyrealloc  (void * ptr, yy_size_t  size )
{
	/* The cast to (char *) in the following accommodates both
	 * implementations that use char* generic pointers, and those
	 * that use void* generic pointers.  It works with the latter
	 * because both ANSI C and C++ allow castless assignment from
	 * any pointer type to void*, and deal with argument conversions
	 * as though doing an assignment.
	 */
	return (void *) realloc( (char *) ptr, size );
}

void yyfree (void * ptr )
{
	free( (char *) ptr );	/* see yyrealloc() for (char *) cast */
}

#define YYTABLES_NAME "yytables"

#line 287 "chuck.lex"



/*
// comment
int comment()
{
    char c;

    while ((c = yyinput()) != '\n' && c != '\r' && c != 0 && c != EOF );

    if (c != 0) { 
       adjust(); 
       if (c == '\n') EM_newline();
    }
    
    return 0;
}

// block comment
int block_comment()
{
    char c, c1;

loop:
    while ((c = yyinput()) != '*' && c != 0 && c != EOF )
        if( c == '\n' ) EM_newline();

    if( c == EOF )
    {
        adjust();
        return 1;
    }
    
    if( (c1 = yyinput()) != '/' && c != 0 )
    {
        unput(c1);
        goto loop;
    }
    
    if( c != 0 ) adjust();

    return 0;
}
*/

