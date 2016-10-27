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
// file: chuck_desugar.h
// desc: chuck desugar system for syntactic sugar
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
// date: Autumn 2016 - original
//-----------------------------------------------------------------------------

#ifndef __CHUCK_DESUGAR_H__
#define __CHUCK_DESUGAR_H__

#include "chuck_absyn.h" // we will be modifying abstract syntax tree
#include "chuck_type.h" // need for Chuck_Context




//-----------------------------------------------------------------------------
// name: desugar_file()
// desc: desugar a program
//-----------------------------------------------------------------------------
t_CKBOOL desugar_file( Chuck_Context * context );




//-----------------------------------------------------------------------------
// name: desugar_class()
// desc: desugar a class. recurse for nested classes.
//-----------------------------------------------------------------------------
t_CKBOOL desugar_class( a_Class_Def class_def );




//-----------------------------------------------------------------------------
// name: desugar_default_func()
// desc: desugar a function with default arguments
//-----------------------------------------------------------------------------
t_CKBOOL desugar_default_func( a_Program parse_tree );




//-----------------------------------------------------------------------------
// name: desugar_default_method()
// desc: desugar a class method with default arguments
//       (classes and programs store sections in different linked lists)
//-----------------------------------------------------------------------------
t_CKBOOL desugar_default_method( a_Class_Body class_parse_tree );




#endif /* defined(__CHUCK__CHUCK_DESUGAR_H__) */
