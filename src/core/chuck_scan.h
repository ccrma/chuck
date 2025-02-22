/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_scan.h
// desc: chuck type-system / type-checker pre-scan
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Summer 2005 - original
//-----------------------------------------------------------------------------
#ifndef __CHUCK_SCAN_H__
#define __CHUCK_SCAN_H__

#include "chuck_type.h"




// scan a program into the env - type discovery scan
t_CKBOOL type_engine_scan0_prog( Chuck_Env * env, a_Program prog,
                                 te_HowMuch val = te_do_all );
// scan a class definition (used by type_engine_add_dll())
t_CKBOOL type_engine_scan0_class_def( Chuck_Env * env, a_Class_Def def );

// scan a program into the env - type resolution scan
t_CKBOOL type_engine_scan1_prog( Chuck_Env * env, a_Program prog,
                                 te_HowMuch val = te_do_all );
// scan a class definition (used by type_engine_add_dll())
t_CKBOOL type_engine_scan1_class_def( Chuck_Env * env, a_Class_Def def );
// scan a function definition (used by type_engine_add_dll())
t_CKBOOL type_engine_scan1_func_def( Chuck_Env * env, a_Func_Def def );
// scan an exp decl
t_CKBOOL type_engine_scan1_exp_decl( Chuck_Env * env, a_Exp_Decl decl );

// scan a program into the env - function/member/static scan
t_CKBOOL type_engine_scan2_prog( Chuck_Env * env, a_Program prog,
                                 te_HowMuch val = te_do_all );
// scan a class definition (used by type_engine_add_dll())
t_CKBOOL type_engine_scan2_class_def( Chuck_Env * env, a_Class_Def def );
// scan a function definition (used by type_engine_add_dll())
t_CKBOOL type_engine_scan2_func_def( Chuck_Env * env, a_Func_Def def );
// scan an exp decl
t_CKBOOL type_engine_scan2_exp_decl( Chuck_Env * env, a_Exp_Decl decl );

// create the decl value
// (can be called from chuck_scan or chuck_type, depending on 'auto')
t_CKBOOL type_engine_scan2_exp_decl_create( Chuck_Env * env, a_Exp_Decl decl );

// the rest is done in chuck_type




#endif
