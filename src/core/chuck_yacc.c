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
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     STRING_LIT = 259,
     CHAR_LIT = 260,
     INT_VAL = 261,
     FLOAT_VAL = 262,
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
     R_HACK = 366,
     GRUCK_RIGHT = 367,
     GRUCK_LEFT = 368,
     UNGRUCK_RIGHT = 369,
     UNGRUCK_LEFT = 370,
     AT_OP = 371,
     AT_CTOR = 372,
     AT_DTOR = 373
   };
#endif
/* Tokens.  */
#define ID 258
#define STRING_LIT 259
#define CHAR_LIT 260
#define INT_VAL 261
#define FLOAT_VAL 262
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
#define GRUCK_RIGHT 367
#define GRUCK_LEFT 368
#define UNGRUCK_RIGHT 369
#define UNGRUCK_LEFT 370
#define AT_OP 371
#define AT_CTOR 372
#define AT_DTOR 373




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
// initial version created by Ge Wang;
// based on ansi C grammar by Jeff Lee, maintained by Jutta Degener
//
// date: Summer 2002
//-----------------------------------------------------------------------------
#include "chuck_absyn.h"
#include "chuck_errmsg.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// function
int yylex( void );

void yyerror( char * s )
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
#line 77 "chuck.y"
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
#line 415 "chuck.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 440 "chuck.tab.c"

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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYLAST   1993

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  119
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  255
/* YYNRULES -- Number of states.  */
#define YYNSTATES  432

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   373

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
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    21,    29,
      36,    44,    47,    52,    55,    60,    62,    63,    65,    68,
      70,    72,    74,    77,    79,    83,    85,    89,    98,   106,
     114,   121,   128,   134,   140,   147,   156,   164,   174,   184,
     196,   208,   210,   212,   213,   215,   217,   219,   221,   223,
     225,   226,   228,   231,   235,   240,   242,   244,   246,   249,
     252,   257,   259,   262,   264,   266,   268,   270,   272,   275,
     279,   282,   285,   291,   299,   305,   313,   320,   328,   336,
     342,   350,   356,   359,   363,   365,   368,   370,   374,   376,
     380,   382,   386,   390,   395,   400,   406,   409,   413,   415,
     418,   422,   426,   430,   433,   437,   439,   443,   445,   448,
     451,   455,   460,   465,   471,   475,   479,   483,   485,   487,
     489,   491,   493,   495,   497,   499,   501,   503,   505,   507,
     509,   511,   513,   515,   517,   519,   521,   523,   525,   531,
     533,   537,   539,   543,   545,   549,   551,   555,   557,   561,
     563,   567,   571,   573,   577,   581,   585,   589,   591,   595,
     599,   601,   605,   609,   611,   615,   619,   623,   625,   629,
     631,   635,   637,   640,   643,   646,   649,   652,   655,   659,
     664,   670,   676,   683,   685,   687,   689,   691,   693,   696,
     698,   700,   702,   704,   706,   708,   710,   712,   714,   716,
     718,   720,   722,   724,   726,   728,   730,   732,   734,   736,
     738,   740,   742,   744,   746,   748,   750,   752,   754,   756,
     758,   760,   762,   764,   766,   768,   770,   772,   774,   776,
     778,   780,   782,   784,   786,   788,   790,   794,   796,   799,
     803,   808,   812,   815,   818,   820,   822,   824,   826,   828,
     830,   832,   834,   836,   840,   844
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     120,     0,    -1,   121,    -1,   120,   121,    -1,   139,    -1,
     130,    -1,   122,    -1,   131,    97,   128,    16,   124,    17,
      -1,   131,    97,   128,   123,    16,   124,    17,    -1,   131,
      98,   128,    16,   124,    17,    -1,   131,    98,   128,   127,
      16,   124,    17,    -1,   100,   128,    -1,   100,   128,    99,
     129,    -1,    99,   129,    -1,    99,   129,   100,   128,    -1,
     125,    -1,    -1,   126,    -1,   126,   125,    -1,   139,    -1,
     130,    -1,   122,    -1,    99,   128,    -1,     3,    -1,     3,
       9,   128,    -1,     3,    -1,     3,    18,   129,    -1,   132,
     133,   137,     3,    12,   138,    13,   144,    -1,   132,   133,
     137,     3,    12,    13,   144,    -1,   132,   133,     3,    12,
     138,    13,   144,    -1,   132,   133,     3,    12,    13,   144,
      -1,   132,   117,    12,   138,    13,   144,    -1,   132,   117,
      12,    13,   144,    -1,   132,   118,    12,    13,   144,    -1,
     132,   118,    12,   138,    13,   144,    -1,   132,   133,   137,
       3,    12,   138,    13,    11,    -1,   132,   133,   137,     3,
      12,    13,    11,    -1,   132,   133,   137,   116,   174,    12,
     138,    13,   144,    -1,   132,   133,   137,   116,    12,   138,
      13,   174,   144,    -1,   132,   133,   137,   116,    12,   174,
      13,    12,   138,    13,   144,    -1,   132,   133,   137,   116,
      12,   138,    13,    12,   174,    13,   144,    -1,   101,    -1,
     103,    -1,    -1,    43,    -1,   101,    -1,   102,    -1,   103,
      -1,   104,    -1,   105,    -1,    -1,     3,    -1,     3,    71,
      -1,    26,   129,    28,    -1,    26,   129,    28,    71,    -1,
     134,    -1,   135,    -1,   136,    -1,   136,   150,    -1,   136,
     153,    -1,   136,   153,     9,   138,    -1,   140,    -1,   139,
     140,    -1,   145,    -1,   143,    -1,   142,    -1,   141,    -1,
     144,    -1,    44,    11,    -1,    44,   146,    11,    -1,    40,
      11,    -1,    41,    11,    -1,    33,    12,   146,    13,   140,
      -1,    33,    12,   146,    13,   140,    35,   140,    -1,    36,
      12,   146,    13,   140,    -1,    38,   140,    36,    12,   146,
      13,    11,    -1,    37,    12,   145,   145,    13,   140,    -1,
      37,    12,   145,   145,   146,    13,   140,    -1,    37,    12,
     146,    10,   146,    13,   140,    -1,    64,    12,   146,    13,
     140,    -1,    38,   140,    64,    12,   146,    13,    11,    -1,
      39,    12,   146,    13,   140,    -1,    16,    17,    -1,    16,
     139,    17,    -1,    11,    -1,   146,    11,    -1,   147,    -1,
     146,     9,   147,    -1,   148,    -1,   147,   157,   148,    -1,
     151,    -1,   148,   158,   151,    -1,    14,   146,    15,    -1,
      14,   146,     9,    15,    -1,    14,   146,    15,   149,    -1,
      14,   146,     9,    15,   149,    -1,    14,    15,    -1,   150,
      14,    15,    -1,   159,    -1,   136,   152,    -1,    65,   136,
     152,    -1,    66,   136,   152,    -1,   104,   136,   152,    -1,
      76,   152,    -1,   104,    76,   152,    -1,   153,    -1,   153,
       9,   152,    -1,     3,    -1,     3,   149,    -1,     3,   150,
      -1,     3,    12,    13,    -1,     3,    12,   146,    13,    -1,
       3,    12,    13,   149,    -1,     3,    12,   146,    13,   149,
      -1,    53,   146,    13,    -1,    54,   146,    13,    -1,    55,
     146,    13,    -1,    90,    -1,    93,    -1,    77,    -1,    78,
      -1,    79,    -1,    80,    -1,    84,    -1,    85,    -1,    86,
      -1,    95,    -1,    96,    -1,    81,    -1,    82,    -1,    83,
      -1,   109,    -1,   108,    -1,   113,    -1,   112,    -1,   115,
      -1,   114,    -1,   160,    -1,   160,    45,   146,    10,   159,
      -1,   161,    -1,   160,    31,   161,    -1,   162,    -1,   161,
      30,   162,    -1,   163,    -1,   162,    47,   163,    -1,   164,
      -1,   163,    49,   164,    -1,   165,    -1,   164,    48,   165,
      -1,   166,    -1,   165,    24,   166,    -1,   165,    25,   166,
      -1,   167,    -1,   166,    26,   167,    -1,   166,    28,   167,
      -1,   166,    27,   167,    -1,   166,    29,   167,    -1,   168,
      -1,   167,    88,   168,    -1,   167,    87,   168,    -1,   169,
      -1,   168,    19,   169,    -1,   168,    20,   169,    -1,   170,
      -1,   169,    21,   170,    -1,   169,    22,   170,    -1,   169,
      23,   170,    -1,   171,    -1,   170,    89,   171,    -1,   172,
      -1,   171,    52,   136,    -1,   175,    -1,    50,   172,    -1,
      51,   172,    -1,   173,   172,    -1,    75,   172,    -1,    74,
     172,    -1,    73,   136,    -1,    73,   136,   149,    -1,    73,
     136,    12,    13,    -1,    73,   136,    12,   146,    13,    -1,
      73,   136,    12,    13,   149,    -1,    73,   136,    12,   146,
      13,   149,    -1,    19,    -1,    20,    -1,    89,    -1,    46,
      -1,    21,    -1,   107,    89,    -1,    90,    -1,    19,    -1,
      20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,
      25,    -1,    26,    -1,    27,    -1,    28,    -1,    29,    -1,
      30,    -1,    31,    -1,    32,    -1,    46,    -1,    47,    -1,
      48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,    -1,
      72,    -1,    77,    -1,    78,    -1,    79,    -1,    80,    -1,
      81,    -1,    82,    -1,    83,    -1,    84,    -1,    85,    -1,
      86,    -1,    87,    -1,    88,    -1,    89,    -1,    91,    -1,
      93,    -1,    95,    -1,    96,    -1,   108,    -1,   109,    -1,
     112,    -1,   113,    -1,   114,    -1,   115,    -1,   176,    -1,
     175,    91,   176,    -1,   177,    -1,   176,   149,    -1,   176,
      12,    13,    -1,   176,    12,   146,    13,    -1,   176,    18,
       3,    -1,   176,    50,    -1,   176,    51,    -1,     3,    -1,
       6,    -1,     7,    -1,     4,    -1,     5,    -1,   149,    -1,
     154,    -1,   155,    -1,   156,    -1,   110,   146,   111,    -1,
      12,   146,    13,    -1,    12,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   206,   206,   207,   211,   212,   213,   217,   219,   221,
     223,   228,   229,   230,   231,   235,   236,   240,   241,   246,
     247,   248,   252,   256,   257,   261,   262,   266,   268,   270,
     272,   274,   276,   278,   280,   282,   284,   286,   288,   290,
     292,   297,   298,   299,   303,   304,   305,   306,   310,   311,
     312,   316,   317,   321,   322,   331,   332,   337,   338,   342,
     343,   347,   348,   352,   353,   354,   355,   357,   361,   362,
     363,   364,   368,   370,   375,   377,   379,   381,   383,   385,
     387,   389,   394,   395,   399,   400,   404,   405,   409,   410,
     415,   416,   421,   422,   423,   425,   430,   431,   435,   436,
     437,   438,   439,   440,   441,   445,   446,   450,   451,   452,
     453,   454,   455,   456,   460,   465,   470,   475,   476,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   492,   493,   494,   495,   496,   497,   501,   502,   507,
     508,   513,   514,   519,   520,   525,   526,   531,   532,   537,
     538,   540,   545,   546,   548,   550,   552,   557,   558,   560,
     565,   566,   568,   573,   574,   576,   578,   583,   584,   589,
     590,   595,   596,   598,   600,   602,   604,   606,   608,   610,
     612,   614,   616,   623,   624,   625,   626,   627,   628,   634,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   647,   648,   649,   650,   651,   652,   653,   654,
     655,   656,   657,   658,   659,   660,   661,   662,   663,   664,
     665,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,   677,   678,   679,   683,   684,   689,   690,   692,
     694,   696,   698,   700,   706,   707,   708,   709,   710,   711,
     712,   713,   714,   715,   716,   717
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "STRING_LIT", "CHAR_LIT",
  "INT_VAL", "FLOAT_VAL", "POUND", "COMMA", "COLON", "SEMICOLON", "LPAREN",
  "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS", "MINUS",
  "TIMES", "DIVIDE", "PERCENT", "EQ", "NEQ", "LT", "LE", "GT", "GE", "AND",
  "OR", "ASSIGN", "IF", "THEN", "ELSE", "WHILE", "FOR", "DO", "LOOP",
  "BREAK", "CONTINUE", "NULL_TOK", "FUNCTION", "RETURN", "QUESTION",
  "EXCLAMATION", "S_OR", "S_AND", "S_XOR", "PLUSPLUS", "MINUSMINUS",
  "DOLLAR", "POUNDPAREN", "PERCENTPAREN", "ATPAREN", "SIMULT", "PATTERN",
  "CODE", "TRANSPORT", "HOST", "TIME", "WHENEVER", "NEXT", "UNTIL",
  "EXTERNAL", "GLOBAL", "EVERY", "BEFORE", "AFTER", "AT", "AT_SYM",
  "ATAT_SYM", "NEW", "SIZEOF", "TYPEOF", "SAME", "PLUS_CHUCK",
  "MINUS_CHUCK", "TIMES_CHUCK", "DIVIDE_CHUCK", "S_AND_CHUCK",
  "S_OR_CHUCK", "S_XOR_CHUCK", "SHIFT_RIGHT_CHUCK", "SHIFT_LEFT_CHUCK",
  "PERCENT_CHUCK", "SHIFT_RIGHT", "SHIFT_LEFT", "TILDA", "CHUCK",
  "COLONCOLON", "S_CHUCK", "AT_CHUCK", "LEFT_S_CHUCK", "UNCHUCK",
  "UPCHUCK", "CLASS", "INTERFACE", "EXTENDS", "IMPLEMENTS", "PUBLIC",
  "PROTECTED", "PRIVATE", "STATIC", "ABSTRACT", "CONST", "SPORK",
  "ARROW_RIGHT", "ARROW_LEFT", "L_HACK", "R_HACK", "GRUCK_RIGHT",
  "GRUCK_LEFT", "UNGRUCK_RIGHT", "UNGRUCK_LEFT", "AT_OP", "AT_CTOR",
  "AT_DTOR", "$accept", "program", "program_section", "class_definition",
  "class_ext", "class_body", "class_body2", "class_section", "iface_ext",
  "id_list", "id_dot", "function_definition", "class_decl",
  "function_decl", "static_decl", "type_decl_a", "type_decl_b",
  "type_decl", "type_decl2", "arg_list", "statement_list", "statement",
  "jump_statement", "selection_statement", "loop_statement",
  "code_segment", "expression_statement", "expression", "chuck_expression",
  "arrow_expression", "array_exp", "array_empty", "decl_expression",
  "var_decl_list", "var_decl", "complex_exp", "polar_exp", "vec_exp",
  "chuck_operator", "arrow_operator", "conditional_expression",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "tilda_expression",
  "cast_expression", "unary_expression", "unary_operator",
  "overloadable_operator", "dur_expression", "postfix_expression",
  "primary_expression", 0
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
     365,   366,   367,   368,   369,   370,   371,   372,   373
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   119,   120,   120,   121,   121,   121,   122,   122,   122,
     122,   123,   123,   123,   123,   124,   124,   125,   125,   126,
     126,   126,   127,   128,   128,   129,   129,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   131,   131,   131,   132,   132,   132,   132,   133,   133,
     133,   134,   134,   135,   135,   136,   136,   137,   137,   138,
     138,   139,   139,   140,   140,   140,   140,   140,   141,   141,
     141,   141,   142,   142,   143,   143,   143,   143,   143,   143,
     143,   143,   144,   144,   145,   145,   146,   146,   147,   147,
     148,   148,   149,   149,   149,   149,   150,   150,   151,   151,
     151,   151,   151,   151,   151,   152,   152,   153,   153,   153,
     153,   153,   153,   153,   154,   155,   156,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   158,   158,   158,   158,   158,   158,   159,   159,   160,
     160,   161,   161,   162,   162,   163,   163,   164,   164,   165,
     165,   165,   166,   166,   166,   166,   166,   167,   167,   167,
     168,   168,   168,   169,   169,   169,   169,   170,   170,   171,
     171,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   173,   173,   173,   173,   173,   173,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   175,   175,   176,   176,   176,
     176,   176,   176,   176,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     6,     7,     6,
       7,     2,     4,     2,     4,     1,     0,     1,     2,     1,
       1,     1,     2,     1,     3,     1,     3,     8,     7,     7,
       6,     6,     5,     5,     6,     8,     7,     9,     9,    11,
      11,     1,     1,     0,     1,     1,     1,     1,     1,     1,
       0,     1,     2,     3,     4,     1,     1,     1,     2,     2,
       4,     1,     2,     1,     1,     1,     1,     1,     2,     3,
       2,     2,     5,     7,     5,     7,     6,     7,     7,     5,
       7,     5,     2,     3,     1,     2,     1,     3,     1,     3,
       1,     3,     3,     4,     4,     5,     2,     3,     1,     2,
       3,     3,     3,     2,     3,     1,     3,     1,     2,     2,
       3,     4,     4,     5,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     3,     1,
       3,     1,     2,     2,     2,     2,     2,     2,     3,     4,
       5,     5,     6,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     2,     3,
       4,     3,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      43,   244,   247,   248,   245,   246,    84,     0,     0,     0,
     183,   184,   187,     0,     0,     0,     0,     0,     0,     0,
       0,    44,     0,   186,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   185,    45,    46,    47,
       0,     0,     0,    43,     2,     6,     5,     0,    50,    55,
      56,     0,     4,    61,    66,    65,    64,    67,    63,     0,
      86,    88,   249,    90,   250,   251,   252,    98,   137,   139,
     141,   143,   145,   147,   149,   152,   157,   160,   163,   167,
     169,     0,   171,   235,   237,    52,   255,     0,     0,    82,
       0,    25,     0,     0,     0,     0,     0,     0,    70,    71,
      68,     0,   244,   172,   173,     0,     0,     0,     0,    51,
       0,     0,   177,   176,   175,   107,   103,   105,     0,     0,
     188,     0,     1,     3,     0,     0,    48,    49,     0,     0,
       0,    99,    62,     0,    85,   119,   120,   121,   122,   128,
     129,   130,   123,   124,   125,   117,   118,   126,   127,     0,
     132,   131,   134,   133,   136,   135,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   174,     0,
       0,     0,   242,   243,   238,   254,     0,    92,    83,     0,
      53,     0,     0,     0,     0,     0,     0,     0,    69,   114,
     115,   116,     0,   100,   101,     0,   178,     0,     0,   108,
     109,     0,   104,   102,   253,    23,     0,     0,     0,     0,
      51,    57,     0,    87,    89,    91,   140,     0,   142,   144,
     146,   148,   150,   151,   153,   155,   154,   156,   159,   158,
     161,   162,   164,   165,   166,   168,   170,   236,   239,     0,
     241,    93,    94,    26,    54,     0,     0,     0,     0,     0,
       0,     0,     0,   179,     0,   110,     0,    96,     0,   106,
       0,    43,     0,     0,     0,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    58,     0,     0,     0,   240,
      95,    72,    74,     0,     0,     0,     0,     0,    81,    79,
     181,   180,   112,   111,    97,    24,    21,     0,    15,    43,
      20,    19,    13,    11,    43,     0,    22,    43,    32,    59,
       0,    33,     0,     0,     0,     0,     0,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   189,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,     0,   138,     0,    76,     0,     0,     0,
       0,   182,   113,     7,    18,     0,     0,     0,     9,     0,
       0,    31,    34,    30,     0,     0,     0,   197,     0,     0,
       0,    73,    77,    78,    75,    80,    14,    12,     8,    10,
      60,    29,    36,    28,     0,     0,     0,     0,    35,    27,
       0,     0,     0,     0,     0,    38,     0,    37,     0,     0,
      40,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    43,    44,   306,   274,   307,   308,   309,   277,   216,
      92,   310,    47,    48,   130,    49,    50,    51,   222,   280,
     311,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,   210,    63,   116,   117,    64,    65,    66,   149,   156,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,   373,    82,    83,    84
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -306
static const yytype_int16 yypact[] =
{
     648,    14,  -306,  -306,  -306,  -306,  -306,   944,  1547,   727,
    -306,  -306,  -306,    39,    25,   102,   118,   879,   133,    59,
     125,  -306,  1018,  -306,  1612,  1612,  1547,  1547,  1547,   142,
      28,    28,    28,  1612,  1612,   181,  -306,   -68,  -306,   -49,
      20,    68,  1547,   411,  -306,  -306,  -306,   -10,    57,  -306,
    -306,   181,   879,  -306,  -306,  -306,  -306,  -306,  -306,   190,
     130,    26,  -306,  -306,  -306,  -306,  -306,  -306,    58,   141,
     113,   180,   198,   238,   207,   179,   249,    76,   101,   197,
    -306,  1612,   163,    33,  -306,  -306,  -306,   119,    24,  -306,
     803,   242,   237,  1547,  1547,  1092,    -1,  1547,  -306,  -306,
    -306,   210,  -306,  -306,  -306,   154,   156,   157,  1547,   205,
     181,   181,    55,  -306,  -306,   247,  -306,   271,   181,   181,
    -306,     7,  -306,  -306,   278,   278,  -306,  -306,   270,   272,
      79,  -306,  -306,  1547,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  1547,
    -306,  -306,  -306,  -306,  -306,  -306,  1547,  1612,  1547,  1612,
    1612,  1612,  1612,  1612,  1612,  1612,  1612,  1612,  1612,  1612,
    1612,  1612,  1612,  1612,  1612,  1612,  1612,    28,  -306,    54,
    1157,   280,  -306,  -306,  -306,  -306,  1222,   273,  -306,    39,
     214,   159,   168,  1092,   234,   274,   276,   170,  -306,  -306,
    -306,  -306,   173,  -306,  -306,  1287,  -306,  1352,  1417,  -306,
     275,   181,  -306,  -306,  -306,   281,     2,    12,    91,   116,
      15,   277,     6,   130,    26,  -306,   141,   261,   113,   180,
     198,   238,   207,   207,   179,   179,   179,   179,   249,   249,
      76,    76,   101,   101,   101,   197,  -306,    33,  -306,   178,
    -306,   273,  -306,  -306,  -306,   879,   879,  1482,  1547,  1547,
    1547,   879,   879,   273,   184,   273,   187,  -306,   279,  -306,
     278,   490,    39,   278,   283,   490,   278,   284,   285,   181,
     289,   285,   290,   120,   291,   275,   292,  1683,  1612,  -306,
    -306,   257,  -306,   879,   189,   209,   215,   218,  -306,  -306,
    -306,   273,  -306,   273,  -306,  -306,  -306,   288,  -306,   569,
    -306,   879,   193,   208,   490,   293,  -306,   490,  -306,   299,
     285,  -306,   285,   285,   296,   124,   295,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,   300,  -306,   879,  -306,   879,   879,   302,
     317,  -306,  -306,  -306,  -306,   278,    39,   294,  -306,   312,
      28,  -306,  -306,  -306,   285,   115,   318,    39,   319,   320,
      28,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,  -306,
    -306,  -306,  -306,  -306,   132,  1781,   322,   323,  -306,  -306,
    1878,   285,    28,   285,   324,  -306,   325,  -306,   285,   285,
    -306,  -306
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -306,  -306,   287,    19,  -306,  -243,    30,  -306,  -306,  -121,
    -182,    36,  -306,  -306,  -306,  -306,  -306,   -30,  -306,  -205,
      34,   -14,  -306,  -306,  -306,  -270,   -80,    -2,  -111,   186,
     -71,   127,   194,   -38,    61,  -306,  -306,  -306,  -306,  -306,
      63,  -306,   192,   195,   196,   191,   200,   109,    72,   105,
     106,    77,   177,    31,  -306,  -305,  -306,   176,  -306
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -52
static const yytype_int16 yytable[] =
{
     110,   111,   112,    96,   217,    87,    88,   253,   318,   286,
     119,   321,   184,   131,   282,   193,   133,   -51,   271,    45,
     101,   399,   223,   109,   105,   106,   107,   283,   275,   -41,
     -41,   109,   315,   186,    52,   195,    46,    93,   132,   187,
     121,   206,    91,    90,   209,   180,    13,     8,   -42,   -42,
     391,   181,   392,   393,    13,   103,   104,   102,     2,     3,
       4,     5,    45,   196,   113,   114,     7,   205,     8,     8,
      98,   387,   203,   204,   389,   223,   132,    52,   324,    46,
     212,   213,   220,   182,   183,    85,    85,   124,   125,   157,
     312,   191,   192,   194,   109,   197,   118,   173,   174,   175,
     221,   272,   273,   158,   278,    13,   202,    26,    27,    28,
     421,   276,   178,   257,    94,   424,   252,    13,   214,   109,
     396,   398,   287,   109,   411,   413,   412,   109,   133,   281,
      95,     9,   185,   323,   150,   151,    99,   395,   152,   153,
     154,   155,    13,   418,   419,    97,    13,   246,     9,   305,
      13,   425,   313,   427,   108,   316,   227,   120,   430,   431,
     160,   126,   127,   133,    42,   133,   133,   199,   133,   200,
     201,   159,   255,   269,   128,   129,   184,   133,   249,   133,
     290,   256,   133,   261,   115,   410,   262,   133,   279,   279,
     176,   289,   300,   133,   302,   417,   133,   301,   133,   133,
     303,   134,   377,   264,   407,   266,    88,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   426,   133,   133,
     145,   198,   378,   146,   133,   147,   148,   133,   379,   161,
     381,   380,   382,   165,   166,   167,   168,   234,   235,   236,
     237,   291,   292,   133,   258,   134,   162,   298,   299,   177,
     242,   243,   244,   279,   179,   294,   295,   296,   297,   207,
     189,   208,   163,   164,   406,   190,   169,   170,   171,   172,
     133,   288,   232,   233,   238,   239,    85,   240,   241,   376,
     211,   215,   218,   250,   219,   254,   259,     8,   260,   268,
     270,   284,   375,   385,   304,   279,   279,   132,   109,   314,
     317,     9,   320,   322,   325,   383,   267,   386,   390,   394,
     388,   408,   400,   404,   327,   328,   329,   330,   331,   332,
     333,   397,   335,   336,   337,   338,   339,   340,   405,   409,
     123,   414,   415,   416,   422,   224,   423,   428,   429,   384,
     319,   341,   342,   343,   344,   345,   346,   347,   285,   226,
     225,   374,   230,   245,   228,   247,   229,     0,     0,     0,
     279,   401,   231,   402,   403,     0,     0,   348,     0,     0,
     279,     0,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   363,     0,   364,     0,
     365,   366,   279,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   367,   368,     0,     0,   369,   370,   371,
     372,   122,     0,     0,     1,     2,     3,     4,     5,     0,
       0,     0,     6,     7,     0,     8,     0,     9,     0,     0,
      10,    11,    12,     0,     0,     0,     0,    13,     0,     0,
       0,     0,     0,     0,    14,     0,     0,    15,    16,    17,
      18,    19,    20,     0,    21,    22,     0,    23,     0,     0,
       0,    24,    25,     0,    26,    27,    28,     0,     0,     0,
       0,     0,     0,     0,     0,    29,    30,    31,     0,     0,
       0,     0,     0,     0,    32,    33,    34,    35,     0,     0,
       0,     0,     0,     1,     2,     3,     4,     5,     0,     0,
      36,     6,     7,     0,     8,     0,     9,   -16,     0,    10,
      11,    12,    37,    38,    39,    40,    13,     0,    41,     0,
       0,    42,     0,    14,     0,     0,    15,    16,    17,    18,
      19,    20,     0,    21,    22,     0,    23,     0,     0,     0,
      24,    25,     0,    26,    27,    28,     0,     0,     0,     0,
       0,     0,     0,     0,    29,    30,    31,     0,     0,     0,
       0,     0,     0,    32,    33,    34,    35,     0,     0,     0,
       0,     0,     1,     2,     3,     4,     5,     0,     0,    36,
       6,     7,     0,     8,     0,     9,   -17,     0,    10,    11,
      12,    37,    38,    39,    40,    13,     0,    41,     0,     0,
      42,     0,    14,     0,     0,    15,    16,    17,    18,    19,
      20,     0,    21,    22,     0,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,    31,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,     0,     0,     0,     0,
       0,     1,     2,     3,     4,     5,     0,     0,    36,     6,
       7,     0,     8,     0,     9,     0,     0,    10,    11,    12,
      37,    38,    39,    40,    13,     0,    41,     0,     0,    42,
       0,    14,     0,     0,    15,    16,    17,    18,    19,    20,
       0,    21,    22,     0,    23,     0,     0,     0,    24,    25,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    30,    31,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     0,     0,    36,     6,     7,
       0,     8,     0,     9,    89,     0,    10,    11,    12,    37,
      38,    39,    40,    13,     0,    41,     0,     0,    42,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,     0,
       0,    22,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     1,     2,     3,     4,
       5,     0,     0,     0,     6,     7,    36,     8,     0,     9,
     188,     0,    10,    11,    12,     0,     0,     0,     0,    13,
       0,    40,     0,     0,    41,     0,    14,    42,     0,    15,
      16,    17,    18,    19,    20,     0,     0,    22,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       0,     0,     1,     2,     3,     4,     5,     0,     0,     0,
       6,     7,    36,     8,     0,     9,     0,     0,    10,    11,
      12,     0,     0,     0,     0,    13,     0,    40,     0,     0,
      41,     0,    14,    42,     0,    15,    16,    17,    18,    19,
      20,     0,     0,    22,     0,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,    31,     0,     1,     2,     3,
       4,     5,    32,    33,    34,    35,     7,    86,     8,     0,
       0,     0,     0,    10,    11,    12,     0,     0,    36,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,    41,     0,     0,    42,
      23,     0,     0,     0,    24,    25,     0,    26,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
      31,     0,     0,     0,     0,     0,     0,    32,    33,    34,
      35,     1,     2,     3,     4,     5,     0,     0,     0,   100,
       7,     0,     8,    36,     0,     0,     0,    10,    11,    12,
       0,     0,     0,     0,    13,     0,     0,     0,    40,     0,
       0,    41,     0,     0,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,     0,     0,    24,    25,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,    31,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,     1,     2,     3,     4,     5,
       0,     0,     0,     6,     7,     0,     8,    36,     0,     0,
       0,    10,    11,    12,     0,     0,     0,     0,    13,     0,
       0,     0,    40,     0,     0,    41,     0,     0,    42,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       1,     2,     3,     4,     5,    32,    33,    34,    35,     7,
     248,     8,     0,     0,     0,     0,    10,    11,    12,     0,
       0,    36,     0,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
       0,     0,    42,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     1,     2,     3,     4,     5,
      32,    33,    34,    35,     7,     0,     8,   251,     0,     0,
       0,    10,    11,    12,     0,     0,    36,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,    41,     0,     0,    42,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       1,     2,     3,     4,     5,    32,    33,    34,    35,     7,
     263,     8,     0,     0,     0,     0,    10,    11,    12,     0,
       0,    36,     0,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
       0,     0,    42,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     1,     2,     3,     4,     5,
      32,    33,    34,    35,     7,   265,     8,     0,     0,     0,
       0,    10,    11,    12,     0,     0,    36,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,    41,     0,     0,    42,    23,     0,
       0,     0,    24,    25,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    30,    31,     0,
       1,     2,     3,     4,     5,    32,    33,    34,    35,     7,
       0,     8,   267,     0,     0,     0,    10,    11,    12,     0,
       0,    36,     0,    13,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
       0,     0,    42,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     1,     2,     3,     4,     5,
      32,    33,    34,    35,     7,   293,     8,     0,     0,     0,
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
       0,     0,     0,     0,     0,   326,     0,     0,     0,     0,
       0,    36,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,     0,     0,     0,    41,
       0,     0,    42,     0,     0,     0,     0,     0,     0,   341,
     342,   343,   344,   345,   346,   347,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   348,     0,     0,     0,     0,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,     0,   364,     0,   365,   366,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   367,   368,   420,     0,   369,   370,   371,   372,     0,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   341,   342,   343,
     344,   345,   346,   347,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   348,     0,     0,     0,     0,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     361,   362,   363,     0,   364,     0,   365,   366,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   367,
     368,     0,     0,   369,   370,   371,   372,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   341,   342,   343,   344,   345,   346,
     347,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     348,     0,     0,     0,     0,   349,   350,   351,   352,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
       0,   364,     0,   365,   366,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   367,   368,     0,     0,
     369,   370,   371,   372
};

static const yytype_int16 yycheck[] =
{
      30,    31,    32,    17,   125,     7,     8,   189,   278,     3,
      40,   281,    83,    51,   219,    95,     9,     3,    16,     0,
      22,   326,   133,     3,    26,    27,    28,    12,    16,    97,
      98,     3,   275,     9,     0,    36,     0,    12,    52,    15,
      42,   112,     3,     9,   115,    12,    26,    14,    97,    98,
     320,    18,   322,   323,    26,    24,    25,     3,     4,     5,
       6,     7,    43,    64,    33,    34,    12,    12,    14,    14,
      11,   314,   110,   111,   317,   186,    90,    43,   283,    43,
     118,   119,     3,    50,    51,    71,    71,    97,    98,    31,
     272,    93,    94,    95,     3,    97,    76,    21,    22,    23,
     130,    99,   100,    45,    13,    26,   108,    53,    54,    55,
     415,    99,    81,   193,    12,   420,   187,    26,   111,     3,
     325,   326,   116,     3,   394,   395,    11,     3,     9,    13,
      12,    16,    13,    13,   108,   109,    11,    13,   112,   113,
     114,   115,    26,    11,   414,    12,    26,   177,    16,   270,
      26,   421,   273,   423,    12,   276,   158,    89,   428,   429,
      47,   104,   105,     9,   110,     9,     9,    13,     9,    13,
      13,    30,    13,   211,   117,   118,   247,     9,   180,     9,
     251,    13,     9,    13,     3,   390,    13,     9,   218,   219,
      89,    13,   263,     9,   265,   400,     9,    13,     9,     9,
      13,    11,    13,   205,   386,   207,   208,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,   422,     9,     9,
      90,    11,    13,    93,     9,    95,    96,     9,    13,    49,
     301,    13,   303,    26,    27,    28,    29,   165,   166,   167,
     168,   255,   256,     9,    10,    11,    48,   261,   262,    52,
     173,   174,   175,   283,    91,   257,   258,   259,   260,    12,
      18,    14,    24,    25,   385,    28,    87,    88,    19,    20,
       9,    10,   163,   164,   169,   170,    71,   171,   172,   293,
       9,     3,    12,     3,    12,    71,    12,    14,    12,    14,
       9,    14,    35,   100,    15,   325,   326,   311,     3,    16,
      16,    16,    13,    13,    12,    17,    15,    99,     9,    13,
      17,    17,    12,    11,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    11,    17,
      43,    13,    13,    13,    12,   149,    13,    13,    13,   309,
     279,    46,    47,    48,    49,    50,    51,    52,   221,   157,
     156,   288,   161,   176,   159,   179,   160,    -1,    -1,    -1,
     390,   375,   162,   377,   378,    -1,    -1,    72,    -1,    -1,
     400,    -1,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    -1,    93,    -1,
      95,    96,   422,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,    -1,    -1,   112,   113,   114,
     115,     0,    -1,    -1,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    12,    -1,    14,    -1,    16,    -1,    -1,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    -1,    -1,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    65,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      89,    11,    12,    -1,    14,    -1,    16,    17,    -1,    19,
      20,    21,   101,   102,   103,   104,    26,    -1,   107,    -1,
      -1,   110,    -1,    33,    -1,    -1,    36,    37,    38,    39,
      40,    41,    -1,    43,    44,    -1,    46,    -1,    -1,    -1,
      50,    51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    75,    76,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    89,
      11,    12,    -1,    14,    -1,    16,    17,    -1,    19,    20,
      21,   101,   102,   103,   104,    26,    -1,   107,    -1,    -1,
     110,    -1,    33,    -1,    -1,    36,    37,    38,    39,    40,
      41,    -1,    43,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,    -1,    -1,    89,    11,
      12,    -1,    14,    -1,    16,    -1,    -1,    19,    20,    21,
     101,   102,   103,   104,    26,    -1,   107,    -1,    -1,   110,
      -1,    33,    -1,    -1,    36,    37,    38,    39,    40,    41,
      -1,    43,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,    -1,    -1,    89,    11,    12,
      -1,    14,    -1,    16,    17,    -1,    19,    20,    21,   101,
     102,   103,   104,    26,    -1,   107,    -1,    -1,   110,    -1,
      33,    -1,    -1,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    -1,    -1,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    89,    14,    -1,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      -1,   104,    -1,    -1,   107,    -1,    33,   110,    -1,    36,
      37,    38,    39,    40,    41,    -1,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,    89,    14,    -1,    16,    -1,    -1,    19,    20,
      21,    -1,    -1,    -1,    -1,    26,    -1,   104,    -1,    -1,
     107,    -1,    33,   110,    -1,    36,    37,    38,    39,    40,
      41,    -1,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    66,    -1,     3,     4,     5,
       6,     7,    73,    74,    75,    76,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    -1,    -1,    89,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   104,    -1,    -1,   107,    -1,    -1,   110,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,     3,     4,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    -1,    14,    89,    -1,    -1,    -1,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,   104,    -1,
      -1,   107,    -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    12,    -1,    14,    89,    -1,    -1,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    -1,   104,    -1,    -1,   107,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,
       3,     4,     5,     6,     7,    73,    74,    75,    76,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    -1,
      -1,    89,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,    -1,   107,
      -1,    -1,   110,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,     3,     4,     5,     6,     7,
      73,    74,    75,    76,    12,    -1,    14,    15,    -1,    -1,
      -1,    19,    20,    21,    -1,    -1,    89,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   104,    -1,    -1,   107,    -1,    -1,   110,    46,    -1,
      -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,
       3,     4,     5,     6,     7,    73,    74,    75,    76,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    -1,
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
      -1,    -1,    -1,    -1,    -1,    12,    -1,    -1,    -1,    -1,
      -1,    89,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,   107,
      -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    51,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    -1,    93,    -1,    95,    96,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,    12,    -1,   112,   113,   114,   115,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    93,    -1,    95,    96,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,    -1,    -1,   112,   113,   114,   115,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    -1,    -1,    -1,    -1,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      -1,    93,    -1,    95,    96,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,    -1,    -1,
     112,   113,   114,   115
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    11,    12,    14,    16,
      19,    20,    21,    26,    33,    36,    37,    38,    39,    40,
      41,    43,    44,    46,    50,    51,    53,    54,    55,    64,
      65,    66,    73,    74,    75,    76,    89,   101,   102,   103,
     104,   107,   110,   120,   121,   122,   130,   131,   132,   134,
     135,   136,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   151,   154,   155,   156,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   175,   176,   177,    71,    13,   146,   146,    17,
     139,     3,   129,    12,    12,    12,   140,    12,    11,    11,
      11,   146,     3,   172,   172,   146,   146,   146,    12,     3,
     136,   136,   136,   172,   172,     3,   152,   153,    76,   136,
      89,   146,     0,   121,    97,    98,   104,   105,   117,   118,
     133,   152,   140,     9,    11,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    90,    93,    95,    96,   157,
     108,   109,   112,   113,   114,   115,   158,    31,    45,    30,
      47,    49,    48,    24,    25,    26,    27,    28,    29,    87,
      88,    19,    20,    21,    22,    23,    89,    52,   172,    91,
      12,    18,    50,    51,   149,    13,     9,    15,    17,    18,
      28,   146,   146,   145,   146,    36,    64,   146,    11,    13,
      13,    13,   146,   152,   152,    12,   149,    12,    14,   149,
     150,     9,   152,   152,   111,     3,   128,   128,    12,    12,
       3,   136,   137,   147,   148,   151,   161,   146,   162,   163,
     164,   165,   166,   166,   167,   167,   167,   167,   168,   168,
     169,   169,   170,   170,   170,   171,   136,   176,    13,   146,
       3,    15,   149,   129,    71,    13,    13,   145,    10,    12,
      12,    13,    13,    13,   146,    13,   146,    15,    14,   152,
       9,    16,    99,   100,   123,    16,    99,   127,    13,   136,
     138,    13,   138,    12,    14,   150,     3,   116,    10,    13,
     149,   140,   140,    13,   146,   146,   146,   146,   140,   140,
     149,    13,   149,    13,    15,   128,   122,   124,   125,   126,
     130,   139,   129,   128,    16,   124,   128,    16,   144,   153,
      13,   144,    13,    13,   138,    12,    12,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    46,    47,    48,    49,    50,    51,    52,    72,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    93,    95,    96,   108,   109,   112,
     113,   114,   115,   174,   159,    35,   140,    13,    13,    13,
      13,   149,   149,    17,   125,   100,    99,   124,    17,   124,
       9,   144,   144,   144,    13,    13,   138,    26,   138,   174,
      12,   140,   140,   140,    11,    11,   128,   129,    17,    17,
     138,   144,    11,   144,    13,    13,    13,   138,    11,   144,
      12,   174,    12,    13,   174,   144,   138,   144,    13,    13,
     144,   144
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;



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

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

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
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 206 "chuck.y"
    { (yyval.program) = g_program = new_program( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 3:
#line 207 "chuck.y"
    { (yyval.program) = g_program = append_program( (yyvsp[(1) - (2)].program), (yyvsp[(2) - (2)].program_section), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 4:
#line 211 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 5:
#line 212 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 6:
#line 213 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 7:
#line 218 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 8:
#line 220 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 9:
#line 222 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 10:
#line 224 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 11:
#line 228 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 12:
#line 229 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(4) - (4)].id_list), (yyvsp[(2) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 13:
#line 230 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (2)].id_list), NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 14:
#line 231 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (4)].id_list), (yyvsp[(4) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 15:
#line 235 "chuck.y"
    { (yyval.class_body) = (yyvsp[(1) - (1)].class_body); ;}
    break;

  case 16:
#line 236 "chuck.y"
    { (yyval.class_body) = NULL; ;}
    break;

  case 17:
#line 240 "chuck.y"
    { (yyval.class_body) = new_class_body( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 18:
#line 241 "chuck.y"
    { (yyval.class_body) = prepend_class_body( (yyvsp[(1) - (2)].program_section), (yyvsp[(2) - (2)].class_body), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 19:
#line 246 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 20:
#line 247 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 21:
#line 248 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 22:
#line 252 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 23:
#line 256 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1 */ ); ;}
    break;

  case 24:
#line 257 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1 */ ); ;}
    break;

  case 25:
#line 261 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1*/ ); ;}
    break;

  case 26:
#line 262 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1*/ ); ;}
    break;

  case 27:
#line 267 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), (yyvsp[(8) - (8)].stmt), TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 28:
#line 269 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 29:
#line 271 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), NULL, (yyvsp[(3) - (7)].sval), (yyvsp[(5) - (7)].arg_list), (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 30:
#line 273 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), (yyvsp[(2) - (6)].ival), NULL, (yyvsp[(3) - (6)].sval), NULL, (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 31:
#line 275 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@construct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 32:
#line 277 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@construct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 33:
#line 279 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@destruct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 34:
#line 281 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@destruct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 35:
#line 283 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), NULL, TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 36:
#line 285 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, NULL, TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 37:
#line 287 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(5) - (9)].ival), (yyvsp[(7) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, FALSE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(5) - (9)]).first_column ); ;}
    break;

  case 38:
#line 289 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(8) - (9)].ival), (yyvsp[(6) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, TRUE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(8) - (9)]).first_column ); ;}
    break;

  case 39:
#line 291 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(6) - (11)].ival), (yyvsp[(9) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, FALSE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(5) - (11)]).first_column ); ;}
    break;

  case 40:
#line 293 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(9) - (11)].ival), (yyvsp[(6) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, TRUE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(8) - (11)]).first_column ); ;}
    break;

  case 41:
#line 297 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 42:
#line 298 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 43:
#line 299 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 44:
#line 303 "chuck.y"
    { (yyval.ival) = ae_key_func; ;}
    break;

  case 45:
#line 304 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 46:
#line 305 "chuck.y"
    { (yyval.ival) = ae_key_protected; ;}
    break;

  case 47:
#line 306 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 48:
#line 310 "chuck.y"
    { (yyval.ival) = ae_key_static; ;}
    break;

  case 49:
#line 311 "chuck.y"
    { (yyval.ival) = ae_key_abstract; ;}
    break;

  case 50:
#line 312 "chuck.y"
    { (yyval.ival) = ae_key_instance; ;}
    break;

  case 51:
#line 316 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column /*, &@1*/ ), 0, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 52:
#line 317 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (2)].sval), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column /*, &@1*/ ), 1, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 53:
#line 321 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (3)].id_list), 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 54:
#line 322 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (4)].id_list), 1, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 55:
#line 331 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 56:
#line 332 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 57:
#line 337 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 58:
#line 338 "chuck.y"
    { (yyval.type_decl) = add_type_decl_array( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 59:
#line 342 "chuck.y"
    { (yyval.arg_list) = new_arg_list( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 60:
#line 343 "chuck.y"
    { (yyval.arg_list) = prepend_arg_list( (yyvsp[(1) - (4)].type_decl), (yyvsp[(2) - (4)].var_decl), (yyvsp[(4) - (4)].arg_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 61:
#line 347 "chuck.y"
    { (yyval.stmt_list) = new_stmt_list( (yyvsp[(1) - (1)].stmt), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 62:
#line 348 "chuck.y"
    { (yyval.stmt_list) = append_stmt_list( (yyvsp[(1) - (2)].stmt_list), (yyvsp[(2) - (2)].stmt), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 63:
#line 352 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 64:
#line 353 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 65:
#line 354 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 66:
#line 355 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 67:
#line 357 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 68:
#line 361 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 69:
#line 362 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 70:
#line 363 "chuck.y"
    { (yyval.stmt) = new_stmt_from_break( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 71:
#line 364 "chuck.y"
    { (yyval.stmt) = new_stmt_from_continue( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 72:
#line 369 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 73:
#line 371 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 74:
#line 376 "chuck.y"
    { (yyval.stmt) = new_stmt_from_while( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 75:
#line 378 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_while( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 76:
#line 380 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (6)].stmt), (yyvsp[(4) - (6)].stmt), NULL, (yyvsp[(6) - (6)].stmt), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 77:
#line 382 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (7)].stmt), (yyvsp[(4) - (7)].stmt), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 78:
#line 384 "chuck.y"
    { (yyval.stmt) = new_stmt_from_foreach( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 79:
#line 386 "chuck.y"
    { (yyval.stmt) = new_stmt_from_until( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 80:
#line 388 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_until( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 81:
#line 390 "chuck.y"
    { (yyval.stmt) = new_stmt_from_loop( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 82:
#line 394 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 83:
#line 395 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( (yyvsp[(2) - (3)].stmt_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 84:
#line 399 "chuck.y"
    { (yyval.stmt) = NULL; ;}
    break;

  case 85:
#line 400 "chuck.y"
    { (yyval.stmt) = new_stmt_from_expression( (yyvsp[(1) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 86:
#line 404 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 87:
#line 405 "chuck.y"
    { (yyval.exp) = append_expression( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 88:
#line 409 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 89:
#line 411 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 90:
#line 415 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 91:
#line 417 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 92:
#line 421 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 93:
#line 422 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 94:
#line 424 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(4) - (4)].array_sub), (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 95:
#line 426 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(5) - (5)].array_sub), (yyvsp[(2) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 96:
#line 430 "chuck.y"
    { (yyval.array_sub) = new_array_sub( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 97:
#line 431 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(1) - (3)].array_sub), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 98:
#line 435 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 99:
#line 436 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 100:
#line 437 "chuck.y"
    { (yyval.exp) = new_exp_decl_external( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 101:
#line 438 "chuck.y"
    { (yyval.exp) = new_exp_decl_global( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 102:
#line 439 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 103:
#line 440 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 104:
#line 441 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 105:
#line 445 "chuck.y"
    { (yyval.var_decl_list) = new_var_decl_list( (yyvsp[(1) - (1)].var_decl), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 106:
#line 446 "chuck.y"
    { (yyval.var_decl_list) = prepend_var_decl_list( (yyvsp[(1) - (3)].var_decl), (yyvsp[(3) - (3)].var_decl_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 107:
#line 450 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (1)].sval), FALSE, NULL, NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 108:
#line 451 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 109:
#line 452 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 110:
#line 453 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (3)].sval), TRUE, NULL, NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 111:
#line 454 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, (yyvsp[(3) - (4)].exp), NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 112:
#line 455 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, NULL, (yyvsp[(4) - (4)].array_sub), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 113:
#line 456 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (5)].sval), TRUE, (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 114:
#line 461 "chuck.y"
    { (yyval.complex_exp) = new_complex( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 115:
#line 466 "chuck.y"
    { (yyval.polar_exp) = new_polar( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 116:
#line 471 "chuck.y"
    { (yyval.vec_exp) = new_vec( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 117:
#line 475 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 118:
#line 476 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 119:
#line 477 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 120:
#line 478 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 121:
#line 479 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 122:
#line 480 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 123:
#line 481 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 124:
#line 482 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 125:
#line 483 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 126:
#line 484 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 127:
#line 485 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 128:
#line 486 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 129:
#line 487 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 130:
#line 488 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 131:
#line 492 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 132:
#line 493 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 133:
#line 494 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 134:
#line 495 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 135:
#line 496 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 136:
#line 497 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 137:
#line 501 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 138:
#line 503 "chuck.y"
    { (yyval.exp) = new_exp_from_if( (yyvsp[(1) - (5)].exp), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 139:
#line 507 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 140:
#line 509 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 141:
#line 513 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 142:
#line 515 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 143:
#line 519 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 144:
#line 521 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 145:
#line 525 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 146:
#line 527 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_xor, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 147:
#line 531 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 148:
#line 533 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 149:
#line 537 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 150:
#line 539 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_eq, (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 151:
#line 541 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_neq, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 152:
#line 545 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 153:
#line 547 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_lt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 154:
#line 549 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_gt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 155:
#line 551 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_le, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 156:
#line 553 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_ge, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 157:
#line 557 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 158:
#line 559 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_left, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 159:
#line 561 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_right, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 160:
#line 565 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 161:
#line 567 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_plus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 162:
#line 569 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_minus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 163:
#line 573 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 164:
#line 575 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_times, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 165:
#line 577 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_divide, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 166:
#line 579 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_percent, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 167:
#line 583 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 168:
#line 585 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_tilda, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 169:
#line 589 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 170:
#line 591 "chuck.y"
    { (yyval.exp) = new_exp_from_cast( (yyvsp[(3) - (3)].type_decl), (yyvsp[(1) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 171:
#line 595 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 172:
#line 597 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_plusplus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 173:
#line 599 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_minusminus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 174:
#line 601 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( (yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 175:
#line 603 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_typeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 176:
#line 605 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_sizeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 177:
#line 607 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (2)].type_decl), FALSE, NULL, NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 178:
#line 609 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (3)].type_decl), FALSE, NULL, (yyvsp[(3) - (3)].array_sub), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 179:
#line 611 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (4)].type_decl), TRUE, NULL, NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 180:
#line 613 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, (yyvsp[(4) - (5)].exp), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 181:
#line 615 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, NULL, (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 182:
#line 617 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (6)].type_decl), TRUE, (yyvsp[(4) - (6)].exp), (yyvsp[(6) - (6)].array_sub), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 183:
#line 623 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 184:
#line 624 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 185:
#line 625 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 186:
#line 626 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 187:
#line 627 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 188:
#line 628 "chuck.y"
    { (yyval.ival) = ae_op_spork; ;}
    break;

  case 189:
#line 634 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 190:
#line 635 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 191:
#line 636 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 192:
#line 637 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 193:
#line 638 "chuck.y"
    { (yyval.ival) = ae_op_divide; ;}
    break;

  case 194:
#line 639 "chuck.y"
    { (yyval.ival) = ae_op_percent; ;}
    break;

  case 195:
#line 640 "chuck.y"
    { (yyval.ival) = ae_op_eq; ;}
    break;

  case 196:
#line 641 "chuck.y"
    { (yyval.ival) = ae_op_neq; ;}
    break;

  case 197:
#line 642 "chuck.y"
    { (yyval.ival) = ae_op_lt; ;}
    break;

  case 198:
#line 643 "chuck.y"
    { (yyval.ival) = ae_op_le; ;}
    break;

  case 199:
#line 644 "chuck.y"
    { (yyval.ival) = ae_op_gt; ;}
    break;

  case 200:
#line 645 "chuck.y"
    { (yyval.ival) = ae_op_ge; ;}
    break;

  case 201:
#line 646 "chuck.y"
    { (yyval.ival) = ae_op_and; ;}
    break;

  case 202:
#line 647 "chuck.y"
    { (yyval.ival) = ae_op_or; ;}
    break;

  case 203:
#line 648 "chuck.y"
    { (yyval.ival) = ae_op_assign; ;}
    break;

  case 204:
#line 649 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 205:
#line 650 "chuck.y"
    { (yyval.ival) = ae_op_s_or; ;}
    break;

  case 206:
#line 651 "chuck.y"
    { (yyval.ival) = ae_op_s_and; ;}
    break;

  case 207:
#line 652 "chuck.y"
    { (yyval.ival) = ae_op_s_xor; ;}
    break;

  case 208:
#line 653 "chuck.y"
    { (yyval.ival) = ae_op_plusplus; ;}
    break;

  case 209:
#line 654 "chuck.y"
    { (yyval.ival) = ae_op_minusminus; ;}
    break;

  case 210:
#line 655 "chuck.y"
    { (yyval.ival) = ae_op_dollar; ;}
    break;

  case 211:
#line 656 "chuck.y"
    { (yyval.ival) = ae_op_at_at; ;}
    break;

  case 212:
#line 657 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 213:
#line 658 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 214:
#line 659 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 215:
#line 660 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 216:
#line 661 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 217:
#line 662 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 218:
#line 663 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 219:
#line 664 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 220:
#line 665 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 221:
#line 666 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 222:
#line 667 "chuck.y"
    { (yyval.ival) = ae_op_shift_right; ;}
    break;

  case 223:
#line 668 "chuck.y"
    { (yyval.ival) = ae_op_shift_left; ;}
    break;

  case 224:
#line 669 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 225:
#line 670 "chuck.y"
    { (yyval.ival) = ae_op_coloncolon; ;}
    break;

  case 226:
#line 671 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 227:
#line 672 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 228:
#line 673 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 229:
#line 674 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 230:
#line 675 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 231:
#line 676 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 232:
#line 677 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 233:
#line 678 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 234:
#line 679 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 236:
#line 685 "chuck.y"
    { (yyval.exp) = new_exp_from_dur( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 237:
#line 689 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 238:
#line 691 "chuck.y"
    { (yyval.exp) = new_exp_from_array( (yyvsp[(1) - (2)].exp), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 239:
#line 693 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (3)].exp), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 240:
#line 695 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (4)].exp), (yyvsp[(3) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 241:
#line 697 "chuck.y"
    { (yyval.exp) = new_exp_from_member_dot( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].sval), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column ); ;}
    break;

  case 242:
#line 699 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_plusplus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 243:
#line 701 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_minusminus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 244:
#line 706 "chuck.y"
    { (yyval.exp) = new_exp_from_id( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 245:
#line 707 "chuck.y"
    { (yyval.exp) = new_exp_from_int( (yyvsp[(1) - (1)].ival), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 246:
#line 708 "chuck.y"
    { (yyval.exp) = new_exp_from_float( (yyvsp[(1) - (1)].fval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 247:
#line 709 "chuck.y"
    { (yyval.exp) = new_exp_from_str( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 248:
#line 710 "chuck.y"
    { (yyval.exp) = new_exp_from_char( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 249:
#line 711 "chuck.y"
    { (yyval.exp) = new_exp_from_array_lit( (yyvsp[(1) - (1)].array_sub), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 250:
#line 712 "chuck.y"
    { (yyval.exp) = new_exp_from_complex( (yyvsp[(1) - (1)].complex_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 251:
#line 713 "chuck.y"
    { (yyval.exp) = new_exp_from_polar( (yyvsp[(1) - (1)].polar_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 252:
#line 714 "chuck.y"
    { (yyval.exp) = new_exp_from_vec( (yyvsp[(1) - (1)].vec_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 253:
#line 715 "chuck.y"
    { (yyval.exp) = new_exp_from_hack( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 254:
#line 716 "chuck.y"
    { (yyval.exp) = (yyvsp[(2) - (3)].exp); ;}
    break;

  case 255:
#line 717 "chuck.y"
    { (yyval.exp) = new_exp_from_nil( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3686 "chuck.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc);
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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



#line 1 "chuck.yy.c"

#line 3 "chuck.yy.c"

#define  YY_INT_ALIGNED short int

/* A lexical scanner generated by flex */

#define FLEX_SCANNER
#define YY_FLEX_MAJOR_VERSION 2
#define YY_FLEX_MINOR_VERSION 6
#define YY_FLEX_SUBMINOR_VERSION 4
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

#ifndef SIZE_MAX
#define SIZE_MAX               (~(size_t)0)
#endif

#endif /* ! C99 */

#endif /* ! FLEXINT_H */

/* begin standard C++ headers. */

/* TODO: this is always defined, so inline it */
#define yyconst const

#if defined(__GNUC__) && __GNUC__ >= 3
#define yynoreturn __attribute__((__noreturn__))
#else
#define yynoreturn
#endif

/* Returned upon end-of-file. */
#define YY_NULL 0

/* Promotes a possibly negative, possibly signed char to an
 *   integer in range [0..255] for use as an array index.
 */
#define YY_SC_TO_UI(c) ((YY_CHAR) (c))

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
#define YY_NEW_FILE yyrestart( yyin  )
#define YY_END_OF_BUFFER_CHAR 0

/* Size of default input buffer. */
#ifndef YY_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k.
 * Moreover, YY_BUF_SIZE is 2*YY_READ_BUF_SIZE in the general case.
 * Ditto for the __ia64__ case accordingly.
 */
#define YY_BUF_SIZE 32768
#else
#define YY_BUF_SIZE 16384
#endif /* __ia64__ */
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
    
    /* Note: We specifically omit the test for yy_rule_can_match_eol because it requires
     *       access to the local variable yy_act. Since yyless() is a macro, it would break
     *       existing scanners that call yyless() from OUTSIDE yylex.
     *       One obvious solution it to make yy_act a global. I tried that, and saw
     *       a 5% performance hit in a non-yylineno scanner, because yy_act is
     *       normally declared as a register variable-- so it is not worth it.
     */
    #define  YY_LESS_LINENO(n) \
            do { \
                yy_size_t yyl;\
                for ( yyl = n; yyl < yyleng; ++yyl )\
                    if ( yytext[yyl] == '\n' )\
                        --yylineno;\
            }while(0)
    #define YY_LINENO_REWIND_TO(dst) \
            do {\
                const char *p;\
                for ( p = yy_cp-1; p >= (dst); --p)\
                    if ( *p == '\n' )\
                        --yylineno;\
            }while(0)
    
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
	int yy_buf_size;

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
static YY_BUFFER_STATE * yy_buffer_stack = NULL; /**< Stack as an array. */

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
static char *yy_c_buf_p = NULL;
static int yy_init = 0;		/* whether we need to initialize */
static int yy_start = 0;	/* start state number */

/* Flag which is used to allow yywrap()'s to do buffer switches
 * instead of setting up a fresh yyin.  A bit of a hack ...
 */
static int yy_did_buffer_switch_on_eof;

void yyrestart ( FILE *input_file  );
void yy_switch_to_buffer ( YY_BUFFER_STATE new_buffer  );
YY_BUFFER_STATE yy_create_buffer ( FILE *file, int size  );
void yy_delete_buffer ( YY_BUFFER_STATE b  );
void yy_flush_buffer ( YY_BUFFER_STATE b  );
void yypush_buffer_state ( YY_BUFFER_STATE new_buffer  );
void yypop_buffer_state ( void );

static void yyensure_buffer_stack ( void );
static void yy_load_buffer_state ( void );
static void yy_init_buffer ( YY_BUFFER_STATE b, FILE *file  );
#define YY_FLUSH_BUFFER yy_flush_buffer( YY_CURRENT_BUFFER )

YY_BUFFER_STATE yy_scan_buffer ( char *base, yy_size_t size  );
YY_BUFFER_STATE yy_scan_string ( const char *yy_str  );
YY_BUFFER_STATE yy_scan_bytes ( const char *bytes, yy_size_t len  );

void *yyalloc ( yy_size_t  );
void *yyrealloc ( void *, yy_size_t  );
void yyfree ( void *  );

#define yy_new_buffer yy_create_buffer
#define yy_set_interactive(is_interactive) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){ \
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_is_interactive = is_interactive; \
	}
#define yy_set_bol(at_bol) \
	{ \
	if ( ! YY_CURRENT_BUFFER ){\
        yyensure_buffer_stack (); \
		YY_CURRENT_BUFFER_LVALUE =    \
            yy_create_buffer( yyin, YY_BUF_SIZE ); \
	} \
	YY_CURRENT_BUFFER_LVALUE->yy_at_bol = at_bol; \
	}
#define YY_AT_BOL() (YY_CURRENT_BUFFER_LVALUE->yy_at_bol)

/* Begin user sect3 */
typedef flex_uint8_t YY_CHAR;

FILE *yyin = NULL, *yyout = NULL;

typedef int yy_state_type;

extern int yylineno;
int yylineno = 1;

extern char *yytext;
#ifdef yytext_ptr
#undef yytext_ptr
#endif
#define yytext_ptr yytext

static yy_state_type yy_get_previous_state ( void );
static yy_state_type yy_try_NUL_trans ( yy_state_type current_state  );
static int yy_get_next_buffer ( void );
static void yynoreturn yy_fatal_error ( const char* msg  );

/* Done after the current pattern has been matched and before the
 * corresponding action - sets up yytext.
 */
#define YY_DO_BEFORE_ACTION \
	(yytext_ptr) = yy_bp; \
	yyleng = (yy_size_t) (yy_cp - yy_bp); \
	(yy_hold_char) = *yy_cp; \
	*yy_cp = '\0'; \
	(yy_c_buf_p) = yy_cp;
#define YY_NUM_RULES 113
#define YY_END_OF_BUFFER 114
/* This struct is not used in this scanner,
   but its presence is necessary. */
struct yy_trans_info
	{
	flex_int32_t yy_verify;
	flex_int32_t yy_nxt;
	};
static const flex_int16_t yy_accept[343] =
    {   0,
        0,    0,  114,  112,    4,    6,  112,    3,   45,  112,
       20,   21,   19,   31,  112,   37,   38,   17,   15,   12,
       16,   14,   18,  103,  103,   13,   43,   25,   36,   26,
       44,   91,  102,   39,   40,   33,  112,  102,  102,  102,
      102,  102,  102,  102,  102,  102,  102,  102,  102,  102,
      102,   41,   32,   42,   46,    5,   24,    0,  109,    0,
        9,   10,    0,   29,    0,    0,    0,    0,    7,    0,
        8,    0,   96,  107,    2,    1,    0,  107,  103,    0,
        0,  103,    0,   22,   97,   35,   27,   77,   23,   76,
       79,    0,   28,   34,   11,    0,   92,    0,    0,    0,

      102,    0,    0,    0,  110,  102,  102,  102,   55,  102,
      102,  102,  102,  102,   53,  102,  102,  102,  102,  102,
      102,  102,  102,  102,  102,  102,    0,   30,   78,    0,
        0,    0,    0,   90,   85,  111,    0,    0,    0,    0,
       83,   81,  100,   98,   82,    0,  107,   84,  104,    0,
      106,    0,  105,   99,   56,    0,  101,    0,   57,   80,
        0,    0,    0,   87,    0,    0,    0,    0,  102,  102,
      102,  102,  102,   47,   60,  102,  102,  102,   61,  102,
      102,  102,  102,  102,  102,  102,  102,  102,  102,  102,
       86,    0,    0,    0,    0,    0,    0,    0,    0,    0,

      107,  104,  106,    0,    0,  105,    0,   89,   88,    0,
        0,    0,    0,    0,    0,    0,  102,  102,  102,  102,
       54,  102,  102,  102,  102,  102,  102,  102,  102,   70,
      102,  102,  102,  102,  102,  102,  102,    0,    0,    0,
        0,    0,    0,    0,    0,    0,  108,    0,    0,    0,
        0,    0,    0,    0,   52,   62,   71,  102,  102,  102,
      102,  102,  102,  102,  102,  102,  102,  102,  102,   72,
      102,  102,   49,   48,    0,    0,    0,    0,  108,    0,
        0,    0,    0,    0,  102,  102,  102,  102,   75,  102,
      102,  102,  102,   66,   50,   58,   69,   73,    0,    0,

        0,    0,    0,    0,    0,    0,    0,  102,   64,  102,
      102,  102,  102,   68,  102,    0,    0,    0,    0,    0,
        0,   51,   74,   59,  102,  102,  102,    0,    0,    0,
       95,   93,    0,  102,   63,   67,    0,    0,   94,    0,
       65,    0
    } ;

static const YY_CHAR yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    4,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   22,
       22,   22,   22,   22,   22,   23,   23,   24,   25,   26,
       27,   28,   29,   30,   31,   31,   32,   31,   33,   34,
       35,   35,   35,   35,   35,   36,   35,   35,   35,   37,
       35,   35,   35,   35,   38,   35,   35,   39,   35,   35,
       40,   41,   42,   43,   35,   44,   45,   46,   47,   48,

       49,   50,   51,   52,   53,   35,   54,   55,   56,   57,
       58,   59,   35,   60,   61,   62,   63,   64,   65,   66,
       67,   35,   68,   69,   70,   71,    1,    1,    1,    1,
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

static const YY_CHAR yy_meta[72] =
    {   0,
        1,    1,    1,    1,    1,    1,    2,    1,    1,    1,
        1,    3,    1,    1,    1,    1,    1,    1,    4,    1,
        5,    5,    6,    1,    1,    1,    1,    1,    2,    1,
        6,    6,    6,    6,    7,    7,    7,    8,    8,    1,
        2,    1,    1,    1,    5,    5,    6,    6,    6,    5,
        7,    7,    7,    7,    7,    7,    8,    7,    7,    8,
        7,    8,    8,    8,    7,    8,    7,    1,    1,    1,
        1
    } ;

static const flex_int16_t yy_base[388] =
    {   0,
        0,    0,  624,  625,  625,  625,  620,  625,  595,   65,
      608,  625,   60,   63,  579,  625,  625,  592,   59,  625,
       61,   55,   65,   88,   79,  594,  625,   77,   56,   95,
      625,  117,    0,  625,  625,  590,   50,  556,   38,  557,
       42,   56,  559,   75,  564,   73,  563,   79,  544,  553,
      557,  625,   89,  625,  625,  625,  580,   98,  625,  124,
      625,  625,  579,  625,  578,  128,  128,  577,  625,  576,
      127,  575,  625,  149,  625,  625,  574,  174,  192,  135,
      158,  123,  582,  625,  582,  162,  625,  625,  625,  625,
      625,  581,  625,  165,  625,  570,  625,  539,  547,  536,

        0,  566,  159,  180,  625,  544,  547,  534,    0,  529,
      527,  528,  530,  528,    0,  526,  522,  518,  115,  160,
      150,  524,  536,  521,  517,  525,  549,  625,  625,  210,
        0,    0,    0,  625,  625,  625,  215,    0,    0,    0,
      625,  625,  625,  625,  625,  217,  625,  625,  223,  227,
      241,    0,  234,  625,  625,  548,  625,  547,  625,  625,
      517,  512,  523,  625,  244,    0,    0,    0,  526,  509,
      160,  520,  519,    0,  520,  520,  510,  515,    0,  499,
      500,  506,  511,  510,  495,  497,  494,  506,  501,  498,
      625,  260,    0,  295,    0,  262,    0,  334,    0,  277,

      298,  268,  625,  217,  220,  275,  355,  625,  625,  491,
      489,  490,  328,    0,  364,    0,  495,  412,  410,  417,
        0,  230,  401,  417,  411,  396,  410,  404,  380,    0,
      376,  360,  365,  365,  358,  360,  358,  111,    0,    0,
        0,  136,    0,    0,    0,  286,  367,  344,  344,  357,
      251,    0,    0,    0,    0,    0,    0,  343,  351,  337,
      340,  337,  335,  324,  308,  317,  316,  300,  304,    0,
      313,  309,    0,    0,    0,    0,    0,    0,  625,  298,
      291,  290,    0,    0,  288,  286,  294,  279,    0,  286,
      288,  276,  262,    0,    0,    0,    0,    0,    0,  219,

        0,  193,  259,  267,  252,    0,  271,  256,    0,  239,
      223,  216,  222,    0,  219,    0,    0,  211,  190,  182,
        0,    0,    0,    0,  154,  135,  112,    0,    0,   67,
      625,  625,    0,   20,    0,    0,    0,    0,  625,    0,
        0,  625,  422,  430,  434,  442,  449,  456,  461,  466,
      470,  472,  474,  476,  478,  480,  482,  484,  486,  488,
      490,  492,  494,  496,  498,  500,  502,  504,  506,  508,
      510,  512,  514,  516,  518,  520,  522,  524,  526,  528,
      530,  532,  534,  536,  538,  540,  542
    } ;

static const flex_int16_t yy_def[388] =
    {   0,
      342,    1,  342,  342,  342,  342,  342,  342,  342,  343,
      342,  342,  342,  342,  344,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  345,  342,  342,  342,  346,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      345,  342,  342,  342,  342,  342,  342,  343,  342,  347,
      342,  342,  342,  342,  342,  344,  348,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  349,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,

      345,  342,  346,  350,  342,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  342,  342,  342,  343,
      351,  352,  353,  342,  342,  342,  344,  354,  355,  356,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  357,  349,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  346,  358,  359,  360,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      342,  343,  361,  343,  362,  344,  363,  344,  364,  342,

      342,  342,  342,  357,  357,  342,  342,  342,  342,  342,
      342,  342,  346,  365,  346,  366,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  343,  367,  194,
      368,  344,  369,  198,  370,  342,  342,  342,  342,  342,
      346,  371,  215,  372,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  373,  374,  375,  376,  342,  342,
      342,  342,  377,  378,  345,  345,  345,  345,  345,  345,
      345,  345,  345,  345,  345,  345,  345,  345,  379,  343,

      380,  344,  342,  342,  342,  381,  346,  345,  345,  345,
      345,  345,  345,  345,  345,  382,  383,  342,  342,  342,
      384,  345,  345,  345,  345,  345,  345,  385,  386,  342,
      342,  342,  387,  345,  345,  345,  374,  376,  342,  378,
      345,    0,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342
    } ;

static const flex_int16_t yy_nxt[697] =
    {   0,
        4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
       24,   25,   25,   26,   27,   28,   29,   30,   31,   32,
       33,   33,   33,   33,   33,   33,   33,   33,   33,   34,
        4,   35,   36,   37,   33,   38,   39,   40,   41,   42,
       43,   33,   44,   33,   33,   33,   45,   33,   46,   47,
       48,   49,   50,   33,   51,   33,   33,   52,   53,   54,
       55,   59,   62,   64,   69,   74,   74,   74,   71,   75,
      341,   88,   89,   90,   76,   70,   63,   72,   73,   65,
      104,   77,  107,  105,   85,  108,  110,   78,   91,   79,

       79,   79,   86,   87,   59,   60,   78,  111,   79,   79,
       79,   81,   92,  112,   82,  127,   82,   59,  113,   80,
       81,   93,   94,   82,  115,   82,   83,   81,  339,   95,
      116,  117,  119,   82,   80,  120,   81,  122,   60,  136,
      123,   82,   82,   96,  130,  130,   97,  136,  137,  137,
       82,   60,  143,   83,  144,  149,  149,  128,   82,  336,
       82,  131,  132,   98,   99,  138,  139,  180,   67,   74,
       74,   74,  181,  150,  100,  150,   67,   82,  151,  151,
      151,  146,  147,  335,  147,   82,  133,  155,  156,  132,
      140,  158,  159,  139,   74,   74,   74,  146,  147,  104,

      165,  165,  105,  147,  136,  182,  146,  147,  184,  147,
       78,  185,   79,   79,   79,  334,   59,  166,  167,  183,
      219,  220,  146,  147,   81,   59,  136,   82,  147,   82,
      192,  192,  200,   67,  200,  196,  196,  201,  201,  201,
       81,  332,  168,  149,  149,  167,   82,  151,  151,  151,
       60,  331,  205,  207,   82,   67,  207,  330,  202,   60,
      202,  151,  151,  151,  213,  213,   59,  327,  326,  206,
      207,  206,  325,  136,  203,  207,  203,  202,  207,  324,
      238,  238,  242,  242,  104,  202,  259,  105,  206,  260,
      203,  104,  207,  323,  105,  203,  206,  201,  201,  201,

       60,   59,   67,  202,  322,  202,  247,  247,  247,  320,
      206,  104,  206,  319,  105,  240,  240,  240,  201,  201,
      201,  318,  202,  315,  314,  240,  240,  240,  240,  206,
      202,  147,  313,  147,  312,   60,  311,  206,  310,  240,
      240,  240,  240,  240,  240,  136,  309,  147,  251,  251,
      308,  305,  147,  304,  244,  244,  244,  303,  298,  297,
      296,  295,  294,  293,  244,  244,  244,  244,  104,  292,
      246,  105,  246,  291,   67,  247,  247,  247,  244,  244,
      244,  244,  244,  244,  253,  253,  253,  247,  247,  247,
      290,  289,  288,  287,  253,  253,  253,  253,  286,  285,

      279,  282,  279,  281,  104,  280,  274,  105,  253,  253,
      253,  253,  253,  253,  273,  272,  279,  271,  270,  269,
      268,  279,   58,   58,   58,   58,   58,   58,   58,   58,
       66,   66,  267,   66,   66,   66,   66,   66,  101,  101,
      101,  101,  103,  103,  103,  103,  103,  103,  103,  103,
       58,   58,  266,   58,  265,  264,   58,   66,   66,  263,
       66,  262,  261,   66,  153,  153,  153,  103,  103,  258,
      103,  257,  256,  103,  193,  193,  194,  194,  195,  195,
      197,  197,  198,  198,  199,  199,  204,  204,  214,  214,
      215,  215,  216,  216,  239,  239,  241,  241,  243,  243,

      245,  245,  252,  252,  254,  254,  275,  275,  276,  276,
      277,  277,  278,  278,  283,  283,  284,  284,  299,  299,
      300,  300,  301,  301,  302,  302,  306,  306,  307,  307,
      316,  316,  317,  317,  321,  321,  328,  328,  329,  329,
      333,  333,  337,  337,  338,  338,  340,  340,  255,  250,
      249,  248,  237,  236,  235,  234,  233,  232,  231,  230,
      229,  228,  227,  226,  225,  224,  223,  222,  221,  218,
      217,  212,  211,  210,  209,  208,  191,  190,  189,  188,
      187,  186,  179,  178,  177,  176,  175,  174,  173,  172,
      171,  170,  169,  164,  163,  162,  161,  160,  157,  154,

      152,  148,  145,  142,  141,  135,  134,  129,  126,  125,
      124,  121,  118,  114,  109,  106,  102,   84,   68,   67,
       61,   57,   56,  342,    3,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342

    } ;

static const flex_int16_t yy_chk[697] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,   10,   13,   14,   19,   22,   22,   22,   21,   23,
      334,   29,   29,   29,   23,   19,   13,   21,   21,   14,
       37,   23,   39,   37,   28,   39,   41,   25,   29,   25,

       25,   25,   28,   28,   58,   10,   24,   41,   24,   24,
       24,   25,   30,   42,   25,   53,   25,  238,   42,   24,
       24,   30,   30,   24,   44,   24,   24,   25,  330,   32,
       44,   44,   46,   25,   24,   46,   24,   48,   58,   66,
       48,   25,   24,   32,   60,   60,   32,  242,   67,   67,
       24,  238,   71,   24,   71,   80,   80,   53,   82,  327,
       82,   60,   60,   32,   32,   67,   67,  119,   66,   74,
       74,   74,  119,   81,   32,   81,  242,   82,   81,   81,
       81,   74,   74,  326,   74,   82,   60,   86,   86,   60,
       67,   94,   94,   67,   78,   78,   78,   74,   74,  103,

      104,  104,  103,   74,  302,  120,   78,   78,  121,   78,
       79,  121,   79,   79,   79,  325,  130,  104,  104,  120,
      171,  171,   78,   78,   79,  300,  137,   79,   78,   79,
      130,  130,  146,  302,  146,  137,  137,  146,  146,  146,
       79,  320,  104,  149,  149,  104,   79,  150,  150,  150,
      130,  319,  153,  204,   79,  137,  205,  318,  149,  300,
      149,  151,  151,  151,  165,  165,  192,  315,  313,  153,
      153,  153,  312,  196,  151,  204,  151,  149,  205,  311,
      192,  192,  196,  196,  165,  149,  222,  165,  153,  222,
      151,  251,  153,  310,  251,  151,  153,  200,  200,  200,

      192,  194,  196,  202,  308,  202,  246,  246,  246,  305,
      206,  307,  206,  304,  307,  194,  194,  194,  201,  201,
      201,  303,  202,  293,  292,  194,  194,  194,  194,  206,
      202,  201,  291,  201,  290,  194,  288,  206,  287,  194,
      194,  194,  194,  194,  194,  198,  286,  201,  213,  213,
      285,  282,  201,  281,  198,  198,  198,  280,  272,  271,
      269,  268,  267,  266,  198,  198,  198,  198,  213,  265,
      207,  213,  207,  264,  198,  207,  207,  207,  198,  198,
      198,  198,  198,  198,  215,  215,  215,  247,  247,  247,
      263,  262,  261,  260,  215,  215,  215,  215,  259,  258,

      247,  250,  247,  249,  215,  248,  237,  215,  215,  215,
      215,  215,  215,  215,  236,  235,  247,  234,  233,  232,
      231,  247,  343,  343,  343,  343,  343,  343,  343,  343,
      344,  344,  229,  344,  344,  344,  344,  344,  345,  345,
      345,  345,  346,  346,  346,  346,  346,  346,  346,  346,
      347,  347,  228,  347,  227,  226,  347,  348,  348,  225,
      348,  224,  223,  348,  349,  349,  349,  350,  350,  220,
      350,  219,  218,  350,  351,  351,  352,  352,  353,  353,
      354,  354,  355,  355,  356,  356,  357,  357,  358,  358,
      359,  359,  360,  360,  361,  361,  362,  362,  363,  363,

      364,  364,  365,  365,  366,  366,  367,  367,  368,  368,
      369,  369,  370,  370,  371,  371,  372,  372,  373,  373,
      374,  374,  375,  375,  376,  376,  377,  377,  378,  378,
      379,  379,  380,  380,  381,  381,  382,  382,  383,  383,
      384,  384,  385,  385,  386,  386,  387,  387,  217,  212,
      211,  210,  190,  189,  188,  187,  186,  185,  184,  183,
      182,  181,  180,  178,  177,  176,  175,  173,  172,  170,
      169,  163,  162,  161,  158,  156,  127,  126,  125,  124,
      123,  122,  118,  117,  116,  114,  113,  112,  111,  110,
      108,  107,  106,  102,  100,   99,   98,   96,   92,   85,

       83,   77,   72,   70,   68,   65,   63,   57,   51,   50,
       49,   47,   45,   43,   40,   38,   36,   26,   18,   15,
       11,    9,    7,    3,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342,  342,  342,  342,  342,
      342,  342,  342,  342,  342,  342

    } ;

/* Table of booleans, true if rule could match eol. */
static const flex_int32_t yy_rule_can_match_eol[114] =
    {   0,
0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,     };

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
#line 10 "chuck.lex"

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
// author: Ge Wang (gewang@cs.princeton.edu) - generated by lex
//         Perry R. Cook (prc@cs.princeton.edu)
//
// initial version created by Ge Wang;
// based on ansi C grammar by Jeff Lee, maintained by Jutta Degener
//
// date: Summer 2002
//-----------------------------------------------------------------------------
#include "chuck_absyn.h"
#include "chuck_errmsg.h"

#include <stdlib.h>
#include <string.h>

// check platforms
#if !defined(__PLATFORM_WINDOWS__) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__) && !defined(__USE_CHUCK_YACC__)
#include "chuck.tab.h" // use fresh bison/flex-generated header
#else
#include "chuck_yacc.h" // use pre-generated header (no bison/flex needed)
                        // created by `make chuck_yacc.h` and `make chuck_yacc.c` in core/
#endif

// globals
extern YYSTYPE yylval;
// 1.5.0.5 (ge) added for precise location tracking
extern YYLTYPE yylloc;
// 1.5.0.5 (ge) added
t_CKINT yycolumn = 1;

// define error handling
#define YY_FATAL_ERROR(msg) EM_error2( 0, msg )

#if defined(_cplusplus) || defined(__cplusplus)
extern "C" {
#endif

  // function prototypes
  int yywrap(void);
  void adjust();
  c_str strip_lit( c_str str );
  c_str alloc_str( c_str str );
  long htol( c_str str );

  // 1.5.0.5 (ge) added
  void a_newline( void );
  void yycleanup( void );
  void yyinitial( void );
  void yyflush( void );

#if defined(_cplusplus) || defined(__cplusplus)
}
#endif

// yycleanup | 1.5.0.5 (ge)
void yycleanup( void )
{
    // clean up the FILE
    if( yyin )
    {
        fclose( yyin );
        yyin = NULL;
    }
}

// yyflush | 1.5.0.5 (ge)
void yyflush( void )
{
    // flush the buffer
    YY_FLUSH_BUFFER;
}

// yyinitial | 1.5.0.5 (ge)
void yyinitial( void )
{
    // set to initial
    BEGIN(YY_START);
    // reset
    yycolumn = 1;
    EM_tokPos = yycolumn;
    yylloc.first_line   = yylloc.last_line   = 1;
    yylloc.first_column = yylloc.last_column = 0;
}

// yywrap()
int yywrap( void )
{
    yycolumn = 1;
    return 1;
}

// new line
void a_newline()
{
    EM_newline( yylloc.last_column );
}

// manually advance
void advance_m()
{
    yycolumn++;
    yylloc.first_column++;
    yylloc.last_column++;
}

// adjust()
void adjust()
{
    // update tok pos
    EM_tokPos = yylloc.first_column;

    // handy debug print for precise position and bounds of each token
    // fprintf( stderr, "yylloc: %d %ld %d %d ------ %s\n", yylloc.first_line, yycolumn, yylloc.last_line, yylloc.last_column, yytext );
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


// block comment hack
#define block_comment_hack loop: \
    while ((c = input()) != '*' && c != 0 && c != EOF ) { \
        if( c == '\n' ) { a_newline(); } \
        else { advance_m(); adjust(); } \
    } \
    if( c == EOF || c == 0 ) /* EOF; return 0 */ { adjust(); return 0; } \
    if( (c1 = input()) != '/' && c != 0 && c != EOF ) { \
        advance_m(); \
        adjust(); \
        unput(c1); \
        goto loop; \
    } \
    if( c1 == EOF || c1 == 0 ) /* EOF; return 0 */ { adjust(); return 0; } \
    else { advance_m(); advance_m(); adjust(); };


// comment hack
#define comment_hack \
    while( (c = input()) != '\n' && c != '\r' && c != 0 && c != EOF ) ; \
    if( c == EOF || c == 0 ) /* EOF; return 0 */ { adjust(); return 0; } \
    if( c == '\n' ) { a_newline(); }


// 1.5.0.5 added for tracking | (thanks ekeyser + Becca Royal-Gordon)
// https://stackoverflow.com/questions/656703/how-does-flex-support-bison-location-exactly
// https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;


#line 974 "chuck.yy.c"
/* generate line number */
/* 1.5.0.7 (ge) added to remove dependecy on isatty() and unistd.h
 without the above option, flex/bison will check isatty() to determine
 whether it's getting its input from a TTY terminal input OR file/pipe,
 and chooses a corresponding caching scheme. since chuck is passing
 data to the parser either as a file or through a string buffer using
 yy_scan_string(), chuck is never using using the parser for direct
 TTY input. (FYI the opposite of this option is `%option interactive`
 if neither `never-interactive` nor `interactive` is specified, bison
 will test using isatty()) */
/* 1.5.1.5 (ge) reentrant lexer */
/* %option reentrant */
/* float exponent | 1.5.0.5 (ge) */
/* universal character name */
/* thanks O'Reilly book for the above recipes for STRING_LIT / CHAR_LIT / FLOAT_VAL
   https://web.iitd.ac.in/~sumeet/flex__bison.pdf */
#line 991 "chuck.yy.c"

#define INITIAL 0

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE void *
#endif

static int yy_init_globals ( void );

/* Accessor methods to globals.
   These are made visible to non-reentrant scanners for convenience. */

int yylex_destroy ( void );

int yyget_debug ( void );

void yyset_debug ( int debug_flag  );

YY_EXTRA_TYPE yyget_extra ( void );

void yyset_extra ( YY_EXTRA_TYPE user_defined  );

FILE *yyget_in ( void );

void yyset_in  ( FILE * _in_str  );

FILE *yyget_out ( void );

void yyset_out  ( FILE * _out_str  );

			yy_size_t yyget_leng ( void );

char *yyget_text ( void );

int yyget_lineno ( void );

void yyset_lineno ( int _line_number  );

/* Macros after this point can all be overridden by user definitions in
 * section 1.
 */

#ifndef YY_SKIP_YYWRAP
#ifdef __cplusplus
extern "C" int yywrap ( void );
#else
extern int yywrap ( void );
#endif
#endif

#ifndef YY_NO_UNPUT
    
    static void yyunput ( int c, char *buf_ptr  );
    
#endif

#ifndef yytext_ptr
static void yy_flex_strncpy ( char *, const char *, int );
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen ( const char * );
#endif

#ifndef YY_NO_INPUT
#ifdef __cplusplus
static int yyinput ( void );
#else
static int input ( void );
#endif

#endif

/* Amount of stuff to slurp up with each read. */
#ifndef YY_READ_BUF_SIZE
#ifdef __ia64__
/* On IA-64, the buffer size is 16k, not 8k */
#define YY_READ_BUF_SIZE 16384
#else
#define YY_READ_BUF_SIZE 8192
#endif /* __ia64__ */
#endif

/* Copy whatever the last rule matched to the standard output. */
#ifndef ECHO
/* This used to be an fputs(), but since the string might contain NUL's,
 * we now use fwrite().
 */
#define ECHO do { if (fwrite( yytext, (size_t) yyleng, 1, yyout )) {} } while (0)
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
		while ( (result = (int) fread(buf, 1, (yy_size_t) max_size, yyin)) == 0 && ferror(yyin)) \
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
#define YY_BREAK /*LINTED*/break;
#endif

#define YY_RULE_SETUP \
	YY_USER_ACTION

/** The main scanner function which does all the work.
 */
YY_DECL
{
	yy_state_type yy_current_state;
	char *yy_cp, *yy_bp;
	int yy_act;
    
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
				yy_create_buffer( yyin, YY_BUF_SIZE );
		}

		yy_load_buffer_state(  );
		}

	{
#line 260 "chuck.lex"

#line 262 "chuck.lex"
 /* --------------------- RULES SECTION --------------------------
    NOTE in this section, begin comments on a new line AND
         with one or more whitespaces before comment
    --------------------------------------------------------------
    NOTE since . matches anything except a newline,
         .* will gobble up the rest of the line
    (from /Flex & Bison/ by John Levin, published O'Reilly 2009)
    ALTERNATIVE "//".* { char c; adjust(); continue; }
  ---------------------------------------------------------------*/
 /* "<--"               { char c; adjust(); comment_hack; continue; } */
 /* ------------------------------------------------------------ */
#line 1214 "chuck.yy.c"

	while ( /*CONSTCOND*/1 )		/* loops until end-of-file is reached */
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
			YY_CHAR yy_c = yy_ec[YY_SC_TO_UI(*yy_cp)] ;
			if ( yy_accept[yy_current_state] )
				{
				(yy_last_accepting_state) = yy_current_state;
				(yy_last_accepting_cpos) = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 343 )
					yy_c = yy_meta[yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
			++yy_cp;
			}
		while ( yy_current_state != 342 );
		yy_cp = (yy_last_accepting_cpos);
		yy_current_state = (yy_last_accepting_state);

yy_find_action:
		yy_act = yy_accept[yy_current_state];

		YY_DO_BEFORE_ACTION;

		if ( yy_act != YY_END_OF_BUFFER && yy_rule_can_match_eol[yy_act] )
			{
			yy_size_t yyl;
			for ( yyl = 0; yyl < yyleng; ++yyl )
				if ( yytext[yyl] == '\n' )
					
    yylineno++;
;
			}

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
#line 273 "chuck.lex"
{ char c; adjust(); comment_hack; continue; }
	YY_BREAK
case 2:
YY_RULE_SETUP
#line 274 "chuck.lex"
{ char c, c1; adjust(); block_comment_hack; continue; }
	YY_BREAK
case 3:
YY_RULE_SETUP
#line 275 "chuck.lex"
{ adjust(); continue; }
	YY_BREAK
case 4:
YY_RULE_SETUP
#line 276 "chuck.lex"
{ adjust(); continue; }
	YY_BREAK
case 5:
/* rule 5 can match eol */
YY_RULE_SETUP
#line 277 "chuck.lex"
{ adjust(); a_newline(); continue; }
	YY_BREAK
case 6:
/* rule 6 can match eol */
YY_RULE_SETUP
#line 278 "chuck.lex"
{ adjust(); a_newline(); continue; }
	YY_BREAK
case 7:
YY_RULE_SETUP
#line 280 "chuck.lex"
{ adjust(); return PLUSPLUS; }
	YY_BREAK
case 8:
YY_RULE_SETUP
#line 281 "chuck.lex"
{ adjust(); return MINUSMINUS; }
	YY_BREAK
case 9:
YY_RULE_SETUP
#line 282 "chuck.lex"
{ adjust(); return POUNDPAREN; }
	YY_BREAK
case 10:
YY_RULE_SETUP
#line 283 "chuck.lex"
{ adjust(); return PERCENTPAREN; }
	YY_BREAK
case 11:
YY_RULE_SETUP
#line 284 "chuck.lex"
{ adjust(); return ATPAREN; }
	YY_BREAK
case 12:
YY_RULE_SETUP
#line 286 "chuck.lex"
{ adjust(); return COMMA; }
	YY_BREAK
case 13:
YY_RULE_SETUP
#line 287 "chuck.lex"
{ adjust(); return COLON; }
	YY_BREAK
case 14:
YY_RULE_SETUP
#line 288 "chuck.lex"
{ adjust(); return DOT; }
	YY_BREAK
case 15:
YY_RULE_SETUP
#line 289 "chuck.lex"
{ adjust(); return PLUS; }
	YY_BREAK
case 16:
YY_RULE_SETUP
#line 290 "chuck.lex"
{ adjust(); return MINUS; }
	YY_BREAK
case 17:
YY_RULE_SETUP
#line 291 "chuck.lex"
{ adjust(); return TIMES; }
	YY_BREAK
case 18:
YY_RULE_SETUP
#line 292 "chuck.lex"
{ adjust(); return DIVIDE; }
	YY_BREAK
case 19:
YY_RULE_SETUP
#line 293 "chuck.lex"
{ adjust(); return PERCENT; }
	YY_BREAK
case 20:
YY_RULE_SETUP
#line 294 "chuck.lex"
{ adjust(); return POUND; }
	YY_BREAK
case 21:
YY_RULE_SETUP
#line 295 "chuck.lex"
{ adjust(); return DOLLAR; }
	YY_BREAK
case 22:
YY_RULE_SETUP
#line 297 "chuck.lex"
{ adjust(); return COLONCOLON; }
	YY_BREAK
case 23:
YY_RULE_SETUP
#line 298 "chuck.lex"
{ adjust(); return EQ; }
	YY_BREAK
case 24:
YY_RULE_SETUP
#line 299 "chuck.lex"
{ adjust(); return NEQ; }
	YY_BREAK
case 25:
YY_RULE_SETUP
#line 300 "chuck.lex"
{ adjust(); return LT; }
	YY_BREAK
case 26:
YY_RULE_SETUP
#line 301 "chuck.lex"
{ adjust(); return GT; }
	YY_BREAK
case 27:
YY_RULE_SETUP
#line 302 "chuck.lex"
{ adjust(); return LE; }
	YY_BREAK
case 28:
YY_RULE_SETUP
#line 303 "chuck.lex"
{ adjust(); return GE; }
	YY_BREAK
case 29:
YY_RULE_SETUP
#line 304 "chuck.lex"
{ adjust(); return AND; }
	YY_BREAK
case 30:
YY_RULE_SETUP
#line 305 "chuck.lex"
{ adjust(); return OR; }
	YY_BREAK
case 31:
YY_RULE_SETUP
#line 306 "chuck.lex"
{ adjust(); return S_AND; }
	YY_BREAK
case 32:
YY_RULE_SETUP
#line 307 "chuck.lex"
{ adjust(); return S_OR; }
	YY_BREAK
case 33:
YY_RULE_SETUP
#line 308 "chuck.lex"
{ adjust(); return S_XOR; }
	YY_BREAK
case 34:
YY_RULE_SETUP
#line 309 "chuck.lex"
{ adjust(); return SHIFT_RIGHT; }
	YY_BREAK
case 35:
YY_RULE_SETUP
#line 310 "chuck.lex"
{ adjust(); return SHIFT_LEFT; }
	YY_BREAK
case 36:
YY_RULE_SETUP
#line 311 "chuck.lex"
{ adjust(); return ASSIGN; }
	YY_BREAK
case 37:
YY_RULE_SETUP
#line 312 "chuck.lex"
{ adjust(); return LPAREN; }
	YY_BREAK
case 38:
YY_RULE_SETUP
#line 313 "chuck.lex"
{ adjust(); return RPAREN; }
	YY_BREAK
case 39:
YY_RULE_SETUP
#line 314 "chuck.lex"
{ adjust(); return LBRACK; }
	YY_BREAK
case 40:
YY_RULE_SETUP
#line 315 "chuck.lex"
{ adjust(); return RBRACK; }
	YY_BREAK
case 41:
YY_RULE_SETUP
#line 316 "chuck.lex"
{ adjust(); return LBRACE; }
	YY_BREAK
case 42:
YY_RULE_SETUP
#line 317 "chuck.lex"
{ adjust(); return RBRACE; }
	YY_BREAK
case 43:
YY_RULE_SETUP
#line 318 "chuck.lex"
{ adjust(); return SEMICOLON; }
	YY_BREAK
case 44:
YY_RULE_SETUP
#line 319 "chuck.lex"
{ adjust(); return QUESTION; }
	YY_BREAK
case 45:
YY_RULE_SETUP
#line 320 "chuck.lex"
{ adjust(); return EXCLAMATION; }
	YY_BREAK
case 46:
YY_RULE_SETUP
#line 321 "chuck.lex"
{ adjust(); return TILDA; }
	YY_BREAK
case 47:
YY_RULE_SETUP
#line 322 "chuck.lex"
{ adjust(); return FOR; }
	YY_BREAK
case 48:
YY_RULE_SETUP
#line 323 "chuck.lex"
{ adjust(); return WHILE; }
	YY_BREAK
case 49:
YY_RULE_SETUP
#line 324 "chuck.lex"
{ adjust(); return UNTIL; }
	YY_BREAK
case 50:
YY_RULE_SETUP
#line 325 "chuck.lex"
{ adjust(); return LOOP; }
	YY_BREAK
case 51:
YY_RULE_SETUP
#line 326 "chuck.lex"
{ adjust(); return CONTINUE; }
	YY_BREAK
case 52:
YY_RULE_SETUP
#line 327 "chuck.lex"
{ adjust(); return BREAK; }
	YY_BREAK
case 53:
YY_RULE_SETUP
#line 328 "chuck.lex"
{ adjust(); return IF; }
	YY_BREAK
case 54:
YY_RULE_SETUP
#line 329 "chuck.lex"
{ adjust(); return ELSE; }
	YY_BREAK
case 55:
YY_RULE_SETUP
#line 330 "chuck.lex"
{ adjust(); return DO; }
	YY_BREAK
case 56:
YY_RULE_SETUP
#line 331 "chuck.lex"
{ adjust(); return L_HACK; }
	YY_BREAK
case 57:
YY_RULE_SETUP
#line 332 "chuck.lex"
{ adjust(); return R_HACK; }
	YY_BREAK
case 58:
YY_RULE_SETUP
#line 334 "chuck.lex"
{ adjust(); return RETURN; }
	YY_BREAK
case 59:
YY_RULE_SETUP
#line 336 "chuck.lex"
{ adjust(); return FUNCTION; }
	YY_BREAK
case 60:
YY_RULE_SETUP
#line 337 "chuck.lex"
{ adjust(); return FUNCTION; }
	YY_BREAK
case 61:
YY_RULE_SETUP
#line 338 "chuck.lex"
{ adjust(); return NEW; }
	YY_BREAK
case 62:
YY_RULE_SETUP
#line 339 "chuck.lex"
{ adjust(); return CLASS; }
	YY_BREAK
case 63:
YY_RULE_SETUP
#line 340 "chuck.lex"
{ adjust(); return INTERFACE; }
	YY_BREAK
case 64:
YY_RULE_SETUP
#line 341 "chuck.lex"
{ adjust(); return EXTENDS; }
	YY_BREAK
case 65:
YY_RULE_SETUP
#line 342 "chuck.lex"
{ adjust(); return IMPLEMENTS; }
	YY_BREAK
case 66:
YY_RULE_SETUP
#line 343 "chuck.lex"
{ adjust(); return PUBLIC; }
	YY_BREAK
case 67:
YY_RULE_SETUP
#line 344 "chuck.lex"
{ adjust(); return PROTECTED; }
	YY_BREAK
case 68:
YY_RULE_SETUP
#line 345 "chuck.lex"
{ adjust(); return PRIVATE; }
	YY_BREAK
case 69:
YY_RULE_SETUP
#line 346 "chuck.lex"
{ adjust(); return STATIC; }
	YY_BREAK
case 70:
YY_RULE_SETUP
#line 347 "chuck.lex"
{ adjust(); return ABSTRACT; }
	YY_BREAK
case 71:
YY_RULE_SETUP
#line 348 "chuck.lex"
{ adjust(); return CONST; }
	YY_BREAK
case 72:
YY_RULE_SETUP
#line 349 "chuck.lex"
{ adjust(); return SPORK; }
	YY_BREAK
case 73:
YY_RULE_SETUP
#line 350 "chuck.lex"
{ adjust(); return TYPEOF; }
	YY_BREAK
case 74:
YY_RULE_SETUP
#line 351 "chuck.lex"
{ adjust(); return EXTERNAL; }
	YY_BREAK
case 75:
YY_RULE_SETUP
#line 352 "chuck.lex"
{ adjust(); return GLOBAL; }
	YY_BREAK
case 76:
YY_RULE_SETUP
#line 354 "chuck.lex"
{ adjust(); return CHUCK; }
	YY_BREAK
case 77:
YY_RULE_SETUP
#line 355 "chuck.lex"
{ adjust(); return UNCHUCK; }
	YY_BREAK
case 78:
YY_RULE_SETUP
#line 356 "chuck.lex"
{ adjust(); return UNCHUCK; }
	YY_BREAK
case 79:
YY_RULE_SETUP
#line 357 "chuck.lex"
{ adjust(); return UPCHUCK; }
	YY_BREAK
case 80:
YY_RULE_SETUP
#line 358 "chuck.lex"
{ adjust(); return AT_CHUCK; }
	YY_BREAK
case 81:
YY_RULE_SETUP
#line 359 "chuck.lex"
{ adjust(); return PLUS_CHUCK; }
	YY_BREAK
case 82:
YY_RULE_SETUP
#line 360 "chuck.lex"
{ adjust(); return MINUS_CHUCK; }
	YY_BREAK
case 83:
YY_RULE_SETUP
#line 361 "chuck.lex"
{ adjust(); return TIMES_CHUCK; }
	YY_BREAK
case 84:
YY_RULE_SETUP
#line 362 "chuck.lex"
{ adjust(); return DIVIDE_CHUCK; }
	YY_BREAK
case 85:
YY_RULE_SETUP
#line 363 "chuck.lex"
{ adjust(); return S_AND_CHUCK; }
	YY_BREAK
case 86:
YY_RULE_SETUP
#line 364 "chuck.lex"
{ adjust(); return S_OR_CHUCK; }
	YY_BREAK
case 87:
YY_RULE_SETUP
#line 365 "chuck.lex"
{ adjust(); return S_XOR_CHUCK; }
	YY_BREAK
case 88:
YY_RULE_SETUP
#line 366 "chuck.lex"
{ adjust(); return SHIFT_RIGHT_CHUCK; }
	YY_BREAK
case 89:
YY_RULE_SETUP
#line 367 "chuck.lex"
{ adjust(); return SHIFT_LEFT_CHUCK; }
	YY_BREAK
case 90:
YY_RULE_SETUP
#line 368 "chuck.lex"
{ adjust(); return PERCENT_CHUCK; }
	YY_BREAK
case 91:
YY_RULE_SETUP
#line 369 "chuck.lex"
{ adjust(); return AT_SYM; }
	YY_BREAK
case 92:
YY_RULE_SETUP
#line 370 "chuck.lex"
{ adjust(); return ATAT_SYM; }
	YY_BREAK
case 93:
YY_RULE_SETUP
#line 371 "chuck.lex"
{ adjust(); return AT_OP; }
	YY_BREAK
case 94:
YY_RULE_SETUP
#line 372 "chuck.lex"
{ adjust(); return AT_CTOR; }
	YY_BREAK
case 95:
YY_RULE_SETUP
#line 373 "chuck.lex"
{ adjust(); return AT_DTOR; }
	YY_BREAK
case 96:
YY_RULE_SETUP
#line 374 "chuck.lex"
{ adjust(); return ARROW_RIGHT; }
	YY_BREAK
case 97:
YY_RULE_SETUP
#line 375 "chuck.lex"
{ adjust(); return ARROW_LEFT; }
	YY_BREAK
case 98:
YY_RULE_SETUP
#line 376 "chuck.lex"
{ adjust(); return GRUCK_RIGHT; }
	YY_BREAK
case 99:
YY_RULE_SETUP
#line 377 "chuck.lex"
{ adjust(); return GRUCK_LEFT; }
	YY_BREAK
case 100:
YY_RULE_SETUP
#line 378 "chuck.lex"
{ adjust(); return UNGRUCK_RIGHT; }
	YY_BREAK
case 101:
YY_RULE_SETUP
#line 379 "chuck.lex"
{ adjust(); return UNGRUCK_LEFT; }
	YY_BREAK
case 102:
YY_RULE_SETUP
#line 381 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(yytext); return ID; }
	YY_BREAK
case 103:
YY_RULE_SETUP
#line 383 "chuck.lex"
{ adjust(); yylval.ival=atoi(yytext); return INT_VAL; }
	YY_BREAK
case 104:
YY_RULE_SETUP
#line 384 "chuck.lex"
{ adjust(); yylval.ival=atoi(yytext); return INT_VAL; }
	YY_BREAK
case 105:
YY_RULE_SETUP
#line 385 "chuck.lex"
{ adjust(); yylval.ival=htol(yytext); return INT_VAL; }
	YY_BREAK
case 106:
YY_RULE_SETUP
#line 387 "chuck.lex"
{ adjust(); yylval.fval=atof(yytext); return FLOAT_VAL; }
	YY_BREAK
case 107:
YY_RULE_SETUP
#line 388 "chuck.lex"
{ adjust(); yylval.fval=atof(yytext); return FLOAT_VAL; }
	YY_BREAK
case 108:
YY_RULE_SETUP
#line 389 "chuck.lex"
{ adjust(); yylval.fval=atof(yytext); return FLOAT_VAL; }
	YY_BREAK
case 109:
/* rule 109 can match eol */
YY_RULE_SETUP
#line 391 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
	YY_BREAK
case 110:
/* rule 110 can match eol */
YY_RULE_SETUP
#line 392 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
	YY_BREAK
case 111:
/* rule 111 can match eol */
YY_RULE_SETUP
#line 393 "chuck.lex"
{ adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return CHAR_LIT; }
	YY_BREAK
case 112:
YY_RULE_SETUP
#line 395 "chuck.lex"
{ adjust(); EM_error( EM_tokPos, "illegal token" ); }
	YY_BREAK
case 113:
YY_RULE_SETUP
#line 397 "chuck.lex"
ECHO;
	YY_BREAK
#line 1847 "chuck.yy.c"
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
				yy_cp = (yy_last_accepting_cpos);
				yy_current_state = (yy_last_accepting_state);
				goto yy_find_action;
				}
			}

		else switch ( yy_get_next_buffer(  ) )
			{
			case EOB_ACT_END_OF_FILE:
				{
				(yy_did_buffer_switch_on_eof) = 0;

				if ( yywrap(  ) )
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
	} /* end of user's declarations */
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
    	char *dest = YY_CURRENT_BUFFER_LVALUE->yy_ch_buf;
	char *source = (yytext_ptr);
	int number_to_move, i;
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
	number_to_move = (int) ((yy_c_buf_p) - (yytext_ptr) - 1);

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
			YY_BUFFER_STATE b = YY_CURRENT_BUFFER_LVALUE;

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
					yyrealloc( (void *) b->yy_ch_buf,
							 (yy_size_t) (b->yy_buf_size + 2)  );
				}
			else
				/* Can't grow it, we don't own it. */
				b->yy_ch_buf = NULL;

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
			yyrestart( yyin  );
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

	if (((yy_n_chars) + number_to_move) > YY_CURRENT_BUFFER_LVALUE->yy_buf_size) {
		/* Extend the array by 50%, plus the number we really need. */
		yy_size_t new_size = (yy_n_chars) + number_to_move + ((yy_n_chars) >> 1);
		YY_CURRENT_BUFFER_LVALUE->yy_ch_buf = (char *) yyrealloc(
			(void *) YY_CURRENT_BUFFER_LVALUE->yy_ch_buf, (yy_size_t) new_size  );
		if ( ! YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			YY_FATAL_ERROR( "out of dynamic memory in yy_get_next_buffer()" );
		/* "- 2" to take care of EOB's */
		YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
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
	yy_state_type yy_current_state;
	char *yy_cp;
    
	yy_current_state = (yy_start);

	for ( yy_cp = (yytext_ptr) + YY_MORE_ADJ; yy_cp < (yy_c_buf_p); ++yy_cp )
		{
		YY_CHAR yy_c = (*yy_cp ? yy_ec[YY_SC_TO_UI(*yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			(yy_last_accepting_state) = yy_current_state;
			(yy_last_accepting_cpos) = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 343 )
				yy_c = yy_meta[yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
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
	int yy_is_jam;
    	char *yy_cp = (yy_c_buf_p);

	YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		(yy_last_accepting_state) = yy_current_state;
		(yy_last_accepting_cpos) = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 343 )
			yy_c = yy_meta[yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + yy_c];
	yy_is_jam = (yy_current_state == 342);

		return yy_is_jam ? 0 : yy_current_state;
}

#ifndef YY_NO_UNPUT

    static void yyunput (int c, char * yy_bp )
{
	char *yy_cp;
    
    yy_cp = (yy_c_buf_p);

	/* undo effects of setting up yytext */
	*yy_cp = (yy_hold_char);

	if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
		{ /* need to shift things up to make room */
		/* +2 for EOB chars. */
		yy_size_t number_to_move = (yy_n_chars) + 2;
		char *dest = &YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[
					YY_CURRENT_BUFFER_LVALUE->yy_buf_size + 2];
		char *source =
				&YY_CURRENT_BUFFER_LVALUE->yy_ch_buf[number_to_move];

		while ( source > YY_CURRENT_BUFFER_LVALUE->yy_ch_buf )
			*--dest = *--source;

		yy_cp += (int) (dest - source);
		yy_bp += (int) (dest - source);
		YY_CURRENT_BUFFER_LVALUE->yy_n_chars =
			(yy_n_chars) = (int) YY_CURRENT_BUFFER_LVALUE->yy_buf_size;

		if ( yy_cp < YY_CURRENT_BUFFER_LVALUE->yy_ch_buf + 2 )
			YY_FATAL_ERROR( "flex scanner push-back overflow" );
		}

	*--yy_cp = (char) c;

    if ( c == '\n' ){
        --yylineno;
    }

	(yytext_ptr) = yy_bp;
	(yy_hold_char) = *yy_cp;
	(yy_c_buf_p) = yy_cp;
}

#endif

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
					yyrestart( yyin );

					/*FALLTHROUGH*/

				case EOB_ACT_END_OF_FILE:
					{
					if ( yywrap(  ) )
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

	if ( c == '\n' )
		
    yylineno++;
;

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
            yy_create_buffer( yyin, YY_BUF_SIZE );
	}

	yy_init_buffer( YY_CURRENT_BUFFER, input_file );
	yy_load_buffer_state(  );
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
	yy_load_buffer_state(  );

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
    
	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_buf_size = size;

	/* yy_ch_buf has to be 2 characters longer than the size given because
	 * we need to put in 2 end-of-buffer characters.
	 */
	b->yy_ch_buf = (char *) yyalloc( (yy_size_t) (b->yy_buf_size + 2)  );
	if ( ! b->yy_ch_buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

	b->yy_is_our_buffer = 1;

	yy_init_buffer( b, file );

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
		yyfree( (void *) b->yy_ch_buf  );

	yyfree( (void *) b  );
}

/* Initializes or reinitializes a buffer.
 * This function is sometimes called more than once on the same buffer,
 * such as during a yyrestart() or at EOF.
 */
    static void yy_init_buffer  (YY_BUFFER_STATE  b, FILE * file )

{
	int oerrno = errno;
    
	yy_flush_buffer( b );

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

        b->yy_is_interactive = 0;
    
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
		yy_load_buffer_state(  );
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
	yy_load_buffer_state(  );
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
		yy_load_buffer_state(  );
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
      num_to_alloc = 1; /* After all that talk, this was set to 1 anyways... */
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
		yy_size_t grow_size = 8 /* arbitrary grow size */;

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
		return NULL;

	b = (YY_BUFFER_STATE) yyalloc( sizeof( struct yy_buffer_state )  );
	if ( ! b )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_buffer()" );

	b->yy_buf_size = (int) (size - 2);	/* "- 2" to take care of EOB's */
	b->yy_buf_pos = b->yy_ch_buf = base;
	b->yy_is_our_buffer = 0;
	b->yy_input_file = NULL;
	b->yy_n_chars = b->yy_buf_size;
	b->yy_is_interactive = 0;
	b->yy_at_bol = 1;
	b->yy_fill_buffer = 0;
	b->yy_buffer_status = YY_BUFFER_NEW;

	yy_switch_to_buffer( b  );

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
YY_BUFFER_STATE yy_scan_string (const char * yystr )
{
    
	return yy_scan_bytes( yystr, (int) strlen(yystr) );
}

/** Setup the input buffer state to scan the given bytes. The next call to yylex() will
 * scan from a @e copy of @a bytes.
 * @param yybytes the byte buffer to scan
 * @param _yybytes_len the number of bytes in the buffer pointed to by @a bytes.
 * 
 * @return the newly allocated buffer state object.
 */
YY_BUFFER_STATE yy_scan_bytes  (const char * yybytes, yy_size_t  _yybytes_len )
{
	YY_BUFFER_STATE b;
	char *buf;
	yy_size_t n;
	yy_size_t i;
    
	/* Get memory for full buffer, including space for trailing EOB's. */
	n = (yy_size_t) (_yybytes_len + 2);
	buf = (char *) yyalloc( n  );
	if ( ! buf )
		YY_FATAL_ERROR( "out of dynamic memory in yy_scan_bytes()" );

	for ( i = 0; i < _yybytes_len; ++i )
		buf[i] = yybytes[i];

	buf[_yybytes_len] = buf[_yybytes_len+1] = YY_END_OF_BUFFER_CHAR;

	b = yy_scan_buffer( buf, n );
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

static void yynoreturn yy_fatal_error (const char* msg )
{
			fprintf( stderr, "%s\n", msg );
	exit( YY_EXIT_FAILURE );
}

/* Redefine yyless() so it works in section 3 code. */

#undef yyless
#define yyless(n) \
	do \
		{ \
		/* Undo effects of setting up yytext. */ \
        yy_size_t yyless_macro_arg = (n); \
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
 * @param _line_number line number
 * 
 */
void yyset_lineno (int  _line_number )
{
    
    yylineno = _line_number;
}

/** Set the input stream. This does not discard the current
 * input buffer.
 * @param _in_str A readable stream.
 * 
 * @see yy_switch_to_buffer
 */
void yyset_in (FILE *  _in_str )
{
        yyin = _in_str ;
}

void yyset_out (FILE *  _out_str )
{
        yyout = _out_str ;
}

int yyget_debug  (void)
{
        return yy_flex_debug;
}

void yyset_debug (int  _bdebug )
{
        yy_flex_debug = _bdebug ;
}

static int yy_init_globals (void)
{
        /* Initialization is the same as for the non-reentrant scanner.
     * This function is called from yylex_destroy(), so don't allocate here.
     */

    /* We do not touch yylineno unless the option is enabled. */
    yylineno =  1;
    
    (yy_buffer_stack) = NULL;
    (yy_buffer_stack_top) = 0;
    (yy_buffer_stack_max) = 0;
    (yy_c_buf_p) = NULL;
    (yy_init) = 0;
    (yy_start) = 0;

/* Defined in main.c */
#ifdef YY_STDINIT
    yyin = stdin;
    yyout = stdout;
#else
    yyin = NULL;
    yyout = NULL;
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
		yy_delete_buffer( YY_CURRENT_BUFFER  );
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
static void yy_flex_strncpy (char* s1, const char * s2, int n )
{
		
	int i;
	for ( i = 0; i < n; ++i )
		s1[i] = s2[i];
}
#endif

#ifdef YY_NEED_STRLEN
static int yy_flex_strlen (const char * s )
{
	int n;
	for ( n = 0; s[n]; ++n )
		;

	return n;
}
#endif

void *yyalloc (yy_size_t  size )
{
			return malloc(size);
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
	return realloc(ptr, size);
}

void yyfree (void * ptr )
{
			free( (char *) ptr );	/* see yyrealloc() for (char *) cast */
}

#define YYTABLES_NAME "yytables"

#line 397 "chuck.lex"


// older
// ([0-9]+"."[0-9]*)|([0-9]*"."[0-9]+) { adjust(); yylval.fval=atof(yytext); return FLOAT_VAL; }
// \"(\\.|[^\\"])*\"       { adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
// `(\\.|[^\\`])*`         { adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return STRING_LIT; }
// '(\\.|[^\\'])'          { adjust(); yylval.sval=alloc_str(strip_lit(yytext)); return CHAR_LIT; }

