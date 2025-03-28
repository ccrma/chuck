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
// file: chuck_instr.h
// desc: chuck virtual machine instruction set
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_INSTR_H__
#define __CHUCK_INSTR_H__

#include <stdio.h>
#include "chuck_def.h"
#include "chuck_type.h" // REFACTOR-2017
#include <vector>


// forward reference
struct Chuck_VM;
struct Chuck_VM_Shred;
struct Chuck_Type;
struct Chuck_Func;

// 1.4.2.0 (ge) | added for switching from snprintf()
#define CK_PRINT_BUF_LENGTH 256




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr
{
public:
    Chuck_Instr();
    virtual ~Chuck_Instr();

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred ) = 0;

public:
    virtual const char * name() const;
    virtual const char * params() const
    { return ""; }

public:
    // store line position for error messages
    void set_linepos( t_CKUINT linepos );
    t_CKUINT m_linepos;

    // prepend codestr associated with this instruction
    void prepend_codestr( const std::string & str );
    // append codestr associated with this instruction
    void append_codestr( const std::string & str );
    // (used in instruction dump) | 1.5.0.0 (ge) added
    // (1.5.0.8) made this vector to support cases where there
    // may be multiple code str, e.g., top of for-loops
    std::vector<std::string> * m_codestr_pre;
    // pre prints before the instruction; post prints after
    std::vector<std::string> * m_codestr_post;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Op
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Op : public Chuck_Instr
{
public:
    Chuck_Instr_Branch_Op() : m_jmp(0) { }
    inline void set( t_CKUINT jmp ) { m_jmp = jmp; }

public:
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH]; snprintf( buffer, CK_PRINT_BUF_LENGTH, "%ld", (long)m_jmp ); return buffer; }

protected:
    t_CKUINT m_jmp;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Unary_Op
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Unary_Op : public Chuck_Instr
{
public:
    Chuck_Instr_Unary_Op() : m_val(0) { }
    inline void set( t_CKUINT val ) { m_val = val; }
    inline t_CKUINT get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH]; snprintf( buffer, CK_PRINT_BUF_LENGTH, "%ld", (long)m_val ); return buffer; }

protected:
    t_CKUINT m_val;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Unary_Op2
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Unary_Op2 : public Chuck_Instr
{
public:
    Chuck_Instr_Unary_Op2() : m_val(0) { }
    inline void set( t_CKFLOAT val ) { m_val = val; }
    inline t_CKFLOAT get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH]; snprintf( buffer, CK_PRINT_BUF_LENGTH, "%.6f", m_val ); return buffer; }

protected:
    t_CKFLOAT m_val;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Op
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Op : public Chuck_Instr
{ };




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_PreInc_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_PreInc_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_PostInc_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_PostInc_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_PreDec_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_PreDec_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_PostDec_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_PostDec_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dec_int_Addr
// desc: decrement int value at addr
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dec_int_Addr : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Dec_int_Addr( t_CKUINT src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Complement_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Complement_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_int_Reverse
// desc: same as mod_int, operands reversed, for %=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_int_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_int_Reverse
// desc: same as minus_int, operands reversed, for -=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_int_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_int_Reverse
// desc: same as divide_int, operands reversed, for /=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_int_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_double_Reverse
// desc: same as minus_double, operands reversed, for -=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_double_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_double_Reverse
// desc: same as divide_double, operands reversed, for /=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_double_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_double_Reverse
// desc: same as mod_double, operands reversed, for %=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_double_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_complex_Reverse
// desc: same as minus_complex, operands reversed, for -=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_complex_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_complex_Reverse
// desc: same as divide_complex, operands reversed, for /=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_complex_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_polar : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_polar : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_polar_Reverse
// desc: same as minus_polar, operands reversed, for -=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_polar_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_polar : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_polar : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_polar_Reverse
// desc: same as divide_complex, operands reversed, for /=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_polar_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec2
// desc: add two vec2 (ge) added 1.5.1.7 with the rise of the planet of ChuGL
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec2
// desc: subtract two vec2 (ge) added 1.5.1.7 with the rise of the planet of ChuGL
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec2
// desc: float * vec2 (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec2_Times_float
// desc: vec2*float (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec2_Times_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec2_Divide_float
// desc: divide two vec2 (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec2_Divide_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec3
// desc: add two vec3, 1.3.5.3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec3
// desc: subtract two vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_XProduct_vec3
// desc: cross product of two vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_XProduct_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec4
// desc: add two vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec4
// desc: subtract two vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_XProduct_vec4
// desc: cross product of two vec4 (same as 3D cross product, discard w)
//-----------------------------------------------------------------------------
struct Chuck_Instr_XProduct_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec3
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec3_Times_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec3_Times_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec3_Divide_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec3_Divide_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec4
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec4_Times_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec4_Times_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec4_Divide_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec4_Divide_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_int_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_int_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_int_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_int_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_int_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_int_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_int_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_int_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_int_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_int_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_double_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_double_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_double_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_double_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_double_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_double_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_double_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_double_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mod_double_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mod_double_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_complex_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_complex_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_complex_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_complex_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_complex_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_complex_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_complex_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_complex_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_polar_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_polar_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_polar_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_polar_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Times_polar_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Times_polar_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Divide_polar_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Divide_polar_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec2_Assign
// desc: add assign two vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec2_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec2_Assign
// desc: minus assign two vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec2_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec3_Assign
// desc: add assign two vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec3_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec3_Assign
// desc: minus assign two vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec3_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_XProduct_vec3_Assign
// desc: cross product assign two vec3
//-----------------------------------------------------------------------------
//struct Chuck_Instr_XProduct_vec3_Assign : public Chuck_Instr_Binary_Op
//{
//public:
//    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
//};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_vec4_Assign
// desc: add assign two vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_vec4_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Minus_vec4_Assign
// desc: minus assign two vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Minus_vec4_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_XProduct_vec4_Assign
// desc: cross product assign two vec4
//-----------------------------------------------------------------------------
//struct Chuck_Instr_XProduct_vec4_Assign : public Chuck_Instr_Binary_Op
//{
//public:
//    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
//};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec2_Assign
// desc: float *=> vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec2_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec3_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec3_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_float_Times_vec4_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_float_Times_vec4_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec2_Divide_float_Assign
// desc: float /=> vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec2_Divide_float_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};



//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec3_Divide_float_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec3_Divide_float_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_vec4_Divide_float_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_vec4_Divide_float_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};





//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_string
// desc: string + string
//       (no longer used; string concat now handled by op overloading)
//-----------------------------------------------------------------------------
//struct Chuck_Instr_Add_string : public Chuck_Instr_Binary_Op
//{
//public:
//    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
//};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_string_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_string_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_string_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_string_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_string_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_string_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_int_string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_int_string : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_float_string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_float_string : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_int_string_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_int_string_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Add_float_string_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_float_string_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Lt_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Lt_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Gt_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Gt_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Le_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Le_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Ge_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Ge_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Eq_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Eq_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Neq_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Neq_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_int( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Lt_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Lt_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Gt_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Gt_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Le_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Le_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Ge_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Ge_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Eq_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Eq_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Neq_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Neq_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_double( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Eq_int_IO_good
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Eq_int_IO_good : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_int_IO_good( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Neq_int_IO_good
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Neq_int_IO_good : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_int_IO_good( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Lt_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Lt_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Gt_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Gt_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Le_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Le_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Ge_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Ge_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Not_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Not_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Negate_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Negate_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Negate_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Negate_double : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Lt_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Lt_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Gt_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Gt_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Le_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Le_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Ge_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Ge_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_vec2
// desc: vec2 == vec2 | 1.5.1.7 (ge) added
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_vec2
// desc: vec2 != vec2 | 1.5.1.7 (ge) added
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_vec3
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_vec3
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Eq_vec4
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Eq_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Neq_vec4
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Neq_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_And
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_And : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Or
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Or : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Xor
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Xor : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Right
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Right : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Right_Reverse
// desc: same as shift_right, operands reversed, for >>=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Right_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Left
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Left : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Left_Reverse
// desc: same as shift_left, operands reversed, for <<=>
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Left_Reverse : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_And_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_And_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Or_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Or_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Xor_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Xor_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Right_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Right_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Binary_Shift_Left_Assign
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Binary_Shift_Left_Assign : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_And
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_And : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Or
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Or : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Goto
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Goto : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Goto( t_CKUINT jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Int
// desc: pop an int from reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Float
// desc: pop a float value from reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Float : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Vec2ComplexPolar
// desc: pop a complex/polar/vec2 value from reg stack (change 1.3.1.0)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Vec2ComplexPolar : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Vec2
// desc: pop a vec2 value from reg stack | 1.5.1.7
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Vec2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Vec3
// desc: pop a vec3 value from reg stack | 1.5.1.5
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Vec3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Vec4
// desc: pop a vec4 value from reg stack | 1.5.1.5
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Vec4 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_WordsMulti
// desc: pop arbitrary num of word from reg stack (added 1.3.1.0)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_WordsMulti : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Pop_WordsMulti( t_CKUINT num ) { this->set( num ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Mem
// desc: pop word, and copy it to the mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Mem: public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Mem2
// desc: pop t_CKFLOAT word, and copy it to the mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Mem2: public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Imm
// desc: push immediate to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Imm : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Imm( t_CKUINT val )
    { this->set( val ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Imm2
// desc: push t_CKFLOAT immediate to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Imm2 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Reg_Push_Imm2( t_CKFLOAT val )
    { this->set( val ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Imm4
// desc: push t_CKCOMPLEX immediate to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Imm4 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Reg_Push_Imm4( t_CKFLOAT x, t_CKFLOAT y )
    { this->set( x ); m_val2 = y; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );

protected:
    t_CKFLOAT m_val2;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Code | 1.5.2.0 (ge) added
// desc: push Chuck_VM_Code * onto register stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Code : public Chuck_Instr
{
public:
    // for carrying out instruction
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    const char * params() const;

public:
    // constructor
    Chuck_Instr_Reg_Push_Code( Chuck_VM_Code * code ) : m_code(code) { }

public:
    // code to push
    Chuck_VM_Code * m_code;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Zero
// desc: push immediate value 0 to reg stack with specific width
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Zero : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Zero( t_CKUINT sizeInBytes )
    { this->set( sizeInBytes ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Dup_Last
// desc: duplicate last value on reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Dup_Last : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Dup_Last2
// desc: duplicate last float on reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Dup_Last2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Transmute_Value_To_Pointer
// desc: duplicate last value on stack as pointer; 1.3.5.3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Transmute_Value_To_Pointer : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Transmute_Value_To_Pointer( t_CKUINT sizeInBytes )
    { this->set( sizeInBytes ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Now
// desc: push value of now to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Now : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Me
// desc: push value of me to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Me : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_This
// desc: push value of now to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_This : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Start
// desc: push value of start to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Start : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Maybe
// desc: push immediate to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Maybe : public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem
// desc: push a variable from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem( t_CKUINT src, t_CKBOOL use_base = FALSE )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem2
// desc: push a t_CKFLOAT variable from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem2( t_CKUINT src, t_CKBOOL use_base = FALSE )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem4
// desc: push a t_CKCOMPLEX variable from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem4( t_CKUINT src, t_CKBOOL use_base = FALSE )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem_Vec3
// desc: push a t_CKVEC3 variable from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem_Vec3 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem_Vec3( t_CKUINT src, t_CKBOOL use_base = FALSE )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
        snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
        return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem_Vec4
// desc: push a t_CKVEC4 variable from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem_Vec4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem_Vec4( t_CKUINT src, t_CKBOOL use_base = FALSE )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
        snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
        return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Global
// desc: push a variable from global map to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Global : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Global( std::string name, te_GlobalType type )
    { this->set( 0 ); m_name = name; m_type = type; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "name='%s'", m_name.c_str() );
      return buffer; }

public:
    std::string m_name;
    te_GlobalType m_type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Mem_Addr
// desc: push addr from mem stack to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Mem_Addr : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem_Addr( t_CKUINT src, t_CKBOOL use_base )
    { this->set( src ); base = use_base; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "src=%ld, base=%ld", (long)m_val, (long)base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Global_Addr
// desc: push addr from global storage to reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Global_Addr : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Global_Addr( std::string name, te_GlobalType type )
    { this->set( 0 ); m_name = name; m_type = type; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "name='%s'", m_name.c_str() );
      return buffer; }

protected:
    std::string m_name;
    te_GlobalType m_type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Deref
// desc: push value from pointer
//       changed 1.3.1.0 to not use a separate size param; now int-only
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Deref : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Deref( t_CKUINT src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Deref2
// desc: push a t_CKFLOAT value from pointer
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Deref2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Deref2( t_CKUINT src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Set_Imm
// desc: set a value unto mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Set_Imm : public Chuck_Instr
{
public:
    Chuck_Instr_Mem_Set_Imm( t_CKUINT offset, t_CKUINT val )
    { m_offset = offset; m_val = val; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%ld, value=%ld", (long)m_offset, (long)m_val );
      return buffer; }

protected:
    t_CKUINT m_offset;
    t_CKUINT m_val;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Set_Imm2
// desc: set a value unto mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Set_Imm2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Mem_Set_Imm2( t_CKUINT offset, t_CKFLOAT val )
    { m_offset = offset; m_val = val; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%ld, value=%f", (long)m_offset, m_val );
      return buffer; }

protected:
    t_CKUINT m_offset;
    t_CKFLOAT m_val;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Push_Imm
// desc: push a value unto mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Push_Imm : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Mem_Push_Imm( t_CKUINT src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};



//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Push_Imm2
// desc: push a t_CKFLOAT value unto mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Push_Imm2 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Mem_Push_Imm2( t_CKFLOAT src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};



//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Pop_Word
// desc: pop a value from mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Pop_Word : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Pop_Word2
// desc: pop a value from mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Pop_Word2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Mem_Pop_Word3
// desc: pop a value from mem stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Mem_Pop_Word3 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Mem_Pop_Word3( t_CKUINT num ) { this->set( num ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Nop
// desc: no op
//-----------------------------------------------------------------------------
struct Chuck_Instr_Nop : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_EOC
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_EOC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Word
// desc: alloc local - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Word : public Chuck_Instr_Unary_Op
{
public:
    // (added 1.3.0.0 -- is_object)
    Chuck_Instr_Alloc_Word( t_CKUINT offset, t_CKBOOL is_object )
    { this->set( offset ); m_is_object = is_object; }

    // was this object reference? (added 1.3.0.0)
    t_CKBOOL m_is_object;

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Word2
// desc: alloc local - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Word2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Word2( t_CKUINT offset )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Word4
// desc: alloc local - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Word4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Word4( t_CKUINT offset )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Vec3
// desc: alloc local - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Vec3 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Vec3( t_CKUINT offset )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Vec4
// desc: alloc local - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Vec4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Vec4( t_CKUINT offset )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Member_Word
// desc: alloc member - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Member_Word : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Member_Word( t_CKUINT offset  )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Member_Word2
// desc: alloc member - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Member_Word2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Member_Word2( t_CKUINT offset  )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Member_Word4
// desc: alloc member - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Member_Word4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Member_Word4( t_CKUINT offset  )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Member_Vec3
// desc: alloc member - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Member_Vec3 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Member_Vec3( t_CKUINT offset  )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Member_Vec4
// desc: alloc member - leaves addr on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Member_Vec4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Alloc_Member_Vec4( t_CKUINT offset  )
    { this->set( offset ); }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Alloc_Word_Global
// desc: alloc in vm global maps
//-----------------------------------------------------------------------------
struct Chuck_Instr_Alloc_Word_Global : public Chuck_Instr_Unary_Op
{
public:
    // (added 1.3.0.0 -- is_object)
    Chuck_Instr_Alloc_Word_Global()
        : m_type(te_globalTypeNone), m_is_array(FALSE),
        m_should_execute_ctors(FALSE),
        m_stack_offset(0), m_chuck_type(NULL)
    { this->set( 0 ); }

    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "name='%s'", m_name.c_str() );
      return buffer; }

    // global name and type
    std::string m_name;
    te_GlobalType m_type;
    t_CKBOOL m_is_array;

    // for objects
    t_CKBOOL m_should_execute_ctors;
    t_CKUINT m_stack_offset;
    Chuck_Type * m_chuck_type;

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Instantiate_Object_Start
// desc: instantiate object (starting step); leaves reference value on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Instantiate_Object_Start : public Chuck_Instr
{
public:
    Chuck_Instr_Instantiate_Object_Start( Chuck_Type * t )
    { this->type = t; }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

public:
    Chuck_Type * type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Instantiate_Object_Complete | 1.5.4.3 (ge) added
// desc: instantiate object (completing step); leaves reference value on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Instantiate_Object_Complete : public Chuck_Instr
{
public:
    Chuck_Instr_Instantiate_Object_Complete( Chuck_Type * t )
    { this->type = t; }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

public:
    Chuck_Type * type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Pre_Constructor
// desc: preconstruct object
//-----------------------------------------------------------------------------
struct Chuck_Instr_Pre_Constructor : public Chuck_Instr
{
public:
    Chuck_Instr_Pre_Constructor( Chuck_VM_Code * pre, t_CKUINT offset )
    { pre_ctor = pre; this->stack_offset = offset; }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

public:
    Chuck_VM_Code * pre_ctor;
    t_CKUINT stack_offset;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Pre_Ctor_Array_Top
// desc: preconstruct object in array (top)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Pre_Ctor_Array_Top : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Pre_Ctor_Array_Top( Chuck_Type * t )
    { this->type = t; }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

public:
    Chuck_Type * type;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Pre_Ctor_Array_Bottom
// desc: preconstruct object in array (bottom)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Pre_Ctor_Array_Bottom : public Chuck_Instr_Unary_Op
{
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // virtual const char * params() const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Pre_Ctor_Array_Post
// desc: preconstruct object in array (clean up)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Pre_Ctor_Array_Post : public Chuck_Instr
{
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // virtual const char * params() const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Prepend
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Prepend : public Chuck_Instr_Unary_Op
{
    Chuck_Instr_Array_Prepend( t_CKUINT size ) { set( size ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // virtual const char * params() const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Append
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Append : public Chuck_Instr_Unary_Op
{
    Chuck_Instr_Array_Append( t_CKUINT size ) { set( size ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // virtual const char * params() const;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_String
// desc: assign primitive (string)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_String : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_Primitive
// desc: assign primitive (word)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_Primitive : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_Primitive2
// desc: assign primitive (2 word)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_Primitive2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_Primitive4
// desc: assign primitive (4 word)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_Primitive4 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_PrimitiveVec3
// desc: assign primitive (vec3)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_PrimitiveVec3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_PrimitiveVec4
// desc: assign primitive (vec4)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_PrimitiveVec4 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_Object
// desc: assign object with reference counting and releasing previous reference
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_Object : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Assign_Object
// desc: map object with reference counting and releasing previous reference
//-----------------------------------------------------------------------------
struct Chuck_Instr_Assign_Object_To_Map : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Assign_Object_To_Map( t_CKUINT size )
    { this->set( size ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_AddRef_Object
// desc: added 1.3.0.0
//-----------------------------------------------------------------------------
struct Chuck_Instr_AddRef_Object : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_AddRef_Object2
// desc: added 1.3.0.0 -- instead of getting offset from reg stack,
//       this variant is given an offset, e.g., from emitter
//-----------------------------------------------------------------------------
struct Chuck_Instr_AddRef_Object2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_AddRef_Object2( t_CKUINT offset )
    { this->set( offset ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_AddRef_Object3
// desc: added 1.3.0.0 -- does the ref add in-place
//       operates on top of the register stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_AddRef_Object3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Release_Object
// desc: release object on stack by its offset
//-----------------------------------------------------------------------------
struct Chuck_Instr_Release_Object : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Release_Object2
// desc: added 1.3.0.0 -- instead of getting offset from reg stack,
//       this variant is given an offset, e.g., from emitter
//-----------------------------------------------------------------------------
struct Chuck_Instr_Release_Object2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Release_Object2( t_CKUINT offset )
    { this->set( offset ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Release_Object3_Pop_Int
// desc: release object reference + pop from reg stack | 1.5.0.0 (ge) added
//       the variant assumes object pointer directly on stack (not offset)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Release_Object3_Pop_Int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Release_Object4
// desc: release object reference from reg stack (no pop) | 1.5.0.0 (ge) added
//       the variant assumes object pointer directly on stack (not offset)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Release_Object4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Release_Object4( t_CKUINT offset )
    { this->set( offset ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_To_Code
// desc: Chuck_Func * to Chuck_VM_Code *
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_To_Code : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: enum ck_Func_Call_Arg_Convention | 1.5.2.0
// desc: where to find this/type pointers in argument block
//-----------------------------------------------------------------------------
enum ck_Func_Call_Arg_Convention
{
    // 'this' (member) or 'type' (static) found in the back of arguments block
    CK_FUNC_CALL_THIS_IN_BACK,
    // 'this' (member) or 'type' (static) found in the back of arguments block
    CK_FUNC_CALL_THIS_IN_FRONT
};
//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call
// desc: user-defined function call
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call : public Chuck_Instr
{
public:
    // for carrying out instruction
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

public:
    // constructor
    Chuck_Instr_Func_Call( ck_Func_Call_Arg_Convention arg_convention = CK_FUNC_CALL_THIS_IN_BACK )
    : m_arg_convention(arg_convention) { }

    // when applicable, this flag indicates whether this/type is at the
    // beginning or at the end of the argument block on the reg stack
    ck_Func_Call_Arg_Convention m_arg_convention;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call_Member
// desc: imported member function call with return
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call_Member : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Func_Call_Member( t_CKUINT ret_size, Chuck_Func * func_ref,
                                  ck_Func_Call_Arg_Convention arg_convention = CK_FUNC_CALL_THIS_IN_BACK,
                                  t_CKBOOL special_primitive_cleanup_this = FALSE )
    { this->set( ret_size ); m_func_ref = func_ref; m_arg_convention = arg_convention;
      m_special_primitive_cleanup_this = special_primitive_cleanup_this; }

public:
    // for carrying out instruction
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

public:
    // 1.5.0.0 (ge) added for arg list cleanup
    Chuck_Func * m_func_ref;
    // when applicable, this flag indicates whether this/type is at the
    // beginning or at the end of the argument block on the reg stack
    ck_Func_Call_Arg_Convention m_arg_convention;
    // 1.5.4.2 (ge) added only for special primitives that have "member" functions (vec2/3/4)
    // #special-primitive-member-func-from-literal
    t_CKBOOL m_special_primitive_cleanup_this;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call_Static
// desc: imported static function call with return
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call_Static : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Func_Call_Static( t_CKUINT ret_size, Chuck_Func * func_ref,
                                  ck_Func_Call_Arg_Convention arg_convention = CK_FUNC_CALL_THIS_IN_BACK )
    { this->set( ret_size ); m_func_ref = func_ref; m_arg_convention = arg_convention; }

public:
    // for carrying out instruction
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

public:
    // 1.5.0.0 (ge) | added for arg list cleanup
    Chuck_Func * m_func_ref;
    // when applicable, this flag indicates whether this/type is at the
    // beginning or at the end of the argument block on the reg stack
    ck_Func_Call_Arg_Convention m_arg_convention;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call_Global
// desc: imported global function call with return
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call_Global : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Func_Call_Global( t_CKUINT ret_size, Chuck_Func * func_ref )
    { this->set( ret_size ); m_func_ref = func_ref; }

public:
    // for carrying out instruction
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

public:
    // 1.5.0.0 (ge) | added for arg list cleanup
    Chuck_Func * m_func_ref;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Return
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Return : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Stmt_Start
// desc: executed at the start of a statement:
//       1) during type-checking, each stmt notes how many objects needs
//          releasing by stmt end, including a) func-calls that return Objects
//          b) `new` expressions which may or may not be assigned to vars
//          (verify these auto-add_refs to make the math work out)
//       2) during emit, if a stmt has any objects to release, one of these
//          instruction will be emitted to begin a statement, purpose:
//          make room on reg stack for objects to release at end of stmt
//       3) operations that return Objects (func calls and `new; not variables
//          since those references are accounted for) should be given the means
//          to add its return value to the list of objects refs to release
//          in the reg stack section allocated by the Stmt_Start instr
//       4) at stmt's end, a Stmt_Cleanup instr will be issued that cleans up
//          the reg stack, including all objects references
//-----------------------------------------------------------------------------
struct Chuck_Instr_Stmt_Start : public Chuck_Instr
{
public:
    // constructor
    Chuck_Instr_Stmt_Start( t_CKUINT numObjReleases );
    // destructor
    virtual ~Chuck_Instr_Stmt_Start();
    // execute
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

public:
    // get next index; returns offset on success; 0 if we have exceeded numObjeReleases
    t_CKBOOL nextOffset( t_CKUINT & offset );
    // set object reference by offset
    t_CKBOOL setObject( Chuck_VM_Object * object, t_CKUINT offset );
    // clean up object references stored in this
    t_CKBOOL cleanupRefs( Chuck_VM_Shred * shred );

public:
    // next index
    t_CKUINT m_nextOffset;
    // number of objects to release at the end of statement
    t_CKUINT m_numObjReleases;
    // stack of regions in case of recursion
    std::vector<t_CKUINT *> m_stack;
    // stack level number (used to cache base stack level to avoid realloc)
    t_CKUINT m_stackLevel;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Stmt_Remember_Object
// desc: called to remember obj ref on reg stack for stmt-related cleanup
//       does not alter contents of stack (in-place)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Stmt_Remember_Object : public Chuck_Instr
{
public:
    // constructor
    Chuck_Instr_Stmt_Remember_Object( Chuck_Instr_Stmt_Start * start, t_CKUINT offset, t_CKUINT addRef = FALSE )
    { m_stmtStart = start; m_offset = offset; m_addRef = addRef; }
    // execute
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    virtual const char * params() const;

protected:
    // pointer to corresponding Stmt_Start
    Chuck_Instr_Stmt_Start * m_stmtStart;
    // data offset
    t_CKUINT m_offset;
    // whether to add ref (FYI func calls internal add ref; 'new' would need this additional add-ref)
    t_CKBOOL m_addRef;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Stmt_Cleanup
// desc: called at the end of each statement for cleanup
//-----------------------------------------------------------------------------
struct Chuck_Instr_Stmt_Cleanup : public Chuck_Instr
{
public:
    // constructor
    Chuck_Instr_Stmt_Cleanup( Chuck_Instr_Stmt_Start * start = NULL )
    { m_stmtStart = start; }
    // execute
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );

public:
    // for printing
    virtual const char * params() const;

protected:
    // pointer to corresponding Stmt_Start
    Chuck_Instr_Stmt_Start * m_stmtStart;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Spork
// desc: spork instruction
//-----------------------------------------------------------------------------
struct Chuck_Instr_Spork : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Spork( t_CKUINT v = 0 ) { this->set( v ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Spork_Stmt
// desc: added 1.3.0.0 -- disable until further notice
//-----------------------------------------------------------------------------
//struct Chuck_Instr_Spork_Stmt : public Chuck_Instr_Unary_Op
//{
//public:
//    Chuck_Instr_Spork_Stmt( t_CKUINT v = 0 ) { this->set( v ); }
//public:
//    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
//};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Time_Advance
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Time_Advance : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Event_Wait
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Event_Wait : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Init_Literal
// desc: for [ ... ] array literal values
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Init_Literal : public Chuck_Instr
{
public: // REFACTOR-2017: added env
    Chuck_Instr_Array_Init_Literal( Chuck_Env * env, Chuck_Type * the_type, t_CKINT length );
    virtual ~Chuck_Instr_Array_Init_Literal();

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const { return m_param_str; }

protected:
    Chuck_Type * m_type_ref;
    t_CKINT m_length;
    t_CKBOOL m_is_obj;
    t_CKBOOL m_is_float;
    char * m_param_str;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Alloc
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Alloc : public Chuck_Instr
{
public: // REFACTOR-2017: added env
    Chuck_Instr_Array_Alloc( Chuck_Env * env, t_CKUINT depth,
        Chuck_Type * contentType, t_CKUINT offset, t_CKBOOL ref,
        Chuck_Type * arrayType );
    virtual ~Chuck_Instr_Array_Alloc();

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const { return m_param_str; }

protected:
    t_CKUINT m_depth;
    Chuck_Type * m_type_ref_content;
    Chuck_Type * m_type_ref_array; // 1.5.0.0 (ge) added
    t_CKBOOL m_is_obj;
    char * m_param_str;
    t_CKUINT m_stack_offset;
    t_CKBOOL m_is_ref;
};




//-----------------------------------------------------------------------------
// do alloc array
//-----------------------------------------------------------------------------
Chuck_Object * do_alloc_array( Chuck_VM * vm, Chuck_VM_Shred * shred, t_CKINT * capacity,
                               const t_CKINT * top, t_CKUINT kind, t_CKBOOL is_obj,
                               t_CKUINT * objs, t_CKINT & index, Chuck_Type * type);




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Access
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Access : public Chuck_Instr
{
public:
    Chuck_Instr_Array_Access( t_CKUINT kind, t_CKUINT emit_addr,
        t_CKUINT istr = FALSE )
    { m_kind = kind; m_emit_addr = emit_addr; m_istr = istr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "kind=%ld, emit_addr=%p istr=%ld",
          (long)m_kind, (void *)m_emit_addr, (long)m_istr );
      return buffer; }

protected:
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;
    t_CKUINT m_istr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Map_Access
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Map_Access : public Chuck_Instr
{
public:
    Chuck_Instr_Array_Map_Access( t_CKUINT kind, t_CKUINT emit_addr )
    { m_kind = kind; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "kind=%ld, emit_addr=%p", (long)m_kind, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Access_Multi
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Access_Multi : public Chuck_Instr
{
public:
    Chuck_Instr_Array_Access_Multi( t_CKUINT depth, t_CKUINT kind, t_CKUINT emit_addr )
    { m_kind = kind; m_depth = depth; m_emit_addr = emit_addr; }
    // get the indice is associative map
    std::vector<t_CKBOOL> & indexIsAssociative() { return m_indexIsAssociative; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "depth=%ld, kind=%ld, emit_addr=%p", (long)m_depth, (long)m_kind, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_depth;
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;

    // 1.3.1.0 list of types of indices
    std::vector<t_CKBOOL> m_indexIsAssociative;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Member_Data
// desc: access the member data of object by offset
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Member_Data : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Member_Data( t_CKUINT offset, t_CKUINT kind, t_CKUINT emit_addr )
    { m_offset = offset; m_kind = kind; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%ld, kind=%ld, emit_addr=%p", (long)m_offset, (long)m_kind, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_offset;
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Member_Func
// desc: access the member function of object by offset
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Member_Func : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Member_Func( t_CKUINT offset )
    { m_offset = offset; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%ld", (long)m_offset );
      return buffer; }

protected:
    t_CKUINT m_offset;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Primitive_Func
// desc: access the member function of primitive type 1.3.5.3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Primitive_Func : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Primitive_Func( t_CKUINT native_func )
    { m_native_func = native_func; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
        snprintf( buffer, CK_PRINT_BUF_LENGTH, "native_func=%p", (void *)m_native_func );
        return buffer; }

protected:
    t_CKUINT m_native_func;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Static_Data
// desc: access the static data of object by offset
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Static_Data : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Static_Data( t_CKUINT offset, t_CKUINT size, t_CKUINT kind, t_CKUINT emit_addr )
    { m_offset = offset; m_size = size; m_kind = kind; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%ld, size=%ld, kind=%ld, emit_addr=%p",
                (long)m_offset, (long)m_size, (long)m_kind, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_offset;
    t_CKUINT m_size;
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Static_Import_Data
// desc: access the static data of object by pointer
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Static_Import_Data : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Static_Import_Data( void * addr, t_CKUINT kind, t_CKUINT emit_addr )
    { m_addr = addr; m_kind = kind; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "addr=%p, kind=%ld, emit_addr=%p",
                (void *)m_addr, (long)m_kind, (void *)m_emit_addr );
      return buffer; }

protected:
    void * m_addr;
    t_CKUINT m_kind;
    t_CKUINT m_emit_addr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Static_Func
// desc: access the static function of object
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Static_Func : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Static_Func( Chuck_Func * func )
    { m_func = func; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "func=%p", (void *)m_func );
      return buffer; }

protected:
    Chuck_Func * m_func;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Cmp_First
// desc: access the complex value's first component
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Cmp_First : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Cmp_First( t_CKUINT is_mem, t_CKUINT emit_addr, te_KindOf kind )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; m_kind = kind; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "is_mem=%ld, emit_addr=%p", (long)m_is_mem, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
    te_KindOf m_kind;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Cmp_Second
// desc: access the complex value's second component
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Cmp_Second : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Cmp_Second( t_CKUINT is_mem, t_CKUINT emit_addr, te_KindOf kind )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; m_kind = kind; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
      snprintf( buffer, CK_PRINT_BUF_LENGTH, "is_mem=%ld, emit_addr=%p", (long)m_is_mem, (void *)m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
    te_KindOf m_kind;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Cmp_Third
// desc: access the complex value's third component
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Cmp_Third : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Cmp_Third( t_CKUINT is_mem, t_CKUINT emit_addr, te_KindOf kind )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; m_kind = kind; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
        snprintf( buffer, CK_PRINT_BUF_LENGTH, "is_mem=%ld, emit_addr=%p", (long)m_is_mem, (void *)m_emit_addr );
        return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
    te_KindOf m_kind;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Cmp_Fourth
// desc: access the complex value's second component
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Cmp_Fourth : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Cmp_Fourth( t_CKUINT is_mem, t_CKUINT emit_addr, te_KindOf kind )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; m_kind = kind; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[CK_PRINT_BUF_LENGTH];
        snprintf( buffer, CK_PRINT_BUF_LENGTH, "is_mem=%ld, emit_addr=%p", (long)m_is_mem, (void *)m_emit_addr );
        return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
    te_KindOf m_kind;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_ADC
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_ADC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_DAC
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_DAC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Bunghole
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Bunghole : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Chout
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Chout : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cherr
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cherr : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_Link
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_Link : public Chuck_Instr
{
public:
    Chuck_Instr_UGen_Link( t_CKBOOL isUpChuck = FALSE );

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );

protected:
    t_CKBOOL m_isUpChuck;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_Array_Link
// desc: link ugens where one or both operands are a ugen array
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_Array_Link : public Chuck_Instr
{
public:
    Chuck_Instr_UGen_Array_Link( t_CKBOOL srcIsArray, t_CKBOOL dstIsArray, t_CKBOOL isUpChuck = FALSE ) :
    m_srcIsArray(srcIsArray), m_dstIsArray(dstIsArray), m_isUpChuck(isUpChuck)
    { }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );

protected:
    t_CKBOOL m_srcIsArray, m_dstIsArray, m_isUpChuck;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_UnLink
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_UnLink : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_Ctrl
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_Ctrl : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_CGet
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_CGet : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_Ctrl2
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_Ctrl2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_CGet2
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_CGet2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_UGen_PMsg
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_UGen_PMsg : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_double2int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_double2int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_int2double
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_int2double : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_int2complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_int2complex : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_int2polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_int2polar : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_double2complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_double2complex : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_double2polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_double2polar : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_complex2polar
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_complex2polar : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_polar2complex
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_polar2complex : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec2tovec3
// desc: cast frmo vec2 to vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec2tovec3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec2tovec4
// desc: cast frmo vec2 to vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec2tovec4 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec3tovec2
// desc: cast frmo vec3 to vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec3tovec2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec4tovec2
// desc: cast frmo vec4 to vec2
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec4tovec2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec3tovec4
// desc: cast frmo vec3 to vec4
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec3tovec4 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_vec4tovec3
// desc: cast frmo vec4 to vec3
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_vec4tovec3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_object2string
// desc: "cast" Object to a string, using the Object's method .toString()
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_object2string : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Cast_Runtime_Verify
// desc: type cast runtime verification  1.5.2.0 (ge) added
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_Runtime_Verify : public Chuck_Instr
{
public:
    // execute
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    // for printing
    const char * params() const;
    // set code snippet (with printf-format style) in case of exception
    void set_codeformat4exception( const std::string & codeWithFormat )
    { m_codeWithFormat = codeWithFormat; }

public:
    Chuck_Instr_Cast_Runtime_Verify( Chuck_Type * from, Chuck_Type * to )
    {
        m_from = from;
        m_to = to;
    }

protected:
    Chuck_Type * m_from;
    Chuck_Type * m_to;
    std::string m_codeWithFormat;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Op_string
// desc: // desc: various string ops
//       (no longer used; string ops now handled more properly by op overloads)
//-----------------------------------------------------------------------------
//struct Chuck_Instr_Op_string : public Chuck_Instr_Unary_Op
//{
//public:
//    Chuck_Instr_Op_string( t_CKUINT v ) { this->set( v ); }
//
//public:
//    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
//};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Init_Loop_Counter
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Init_Loop_Counter : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Push_Loop_Counter_Deref
// desc: push topmost loop counter (1.3.5.3)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Push_Loop_Counter_Deref : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dec_Loop_Counter
// desc: push topmost loop counter (1.3.5.3)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dec_Loop_Counter : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Pop_Loop_Counter
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Pop_Loop_Counter : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_ForEach_Inc_And_Branch
// desc: for( VAR: ARRAY ) increment VAR, test against ARRAY size; branch
//-----------------------------------------------------------------------------
struct Chuck_Instr_ForEach_Inc_And_Branch : public Chuck_Instr_Branch_Op
{
public:
    // constructor
    Chuck_Instr_ForEach_Inc_And_Branch( te_KindOf kind, t_CKUINT size )
    { m_dataKind = kind; m_dataSize = size; this->set( 0 ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );

protected:
    // type of VAR (will determine which array to operate on):
    // kindof_INT
    // kindof_FLOAT
    // kindof_VEC2
    // kindof_VEC3
    // kindof_VEC4
    te_KindOf m_dataKind;
    // size of VAR
    t_CKUINT m_dataSize;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_in_int
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_in_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_in_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_in_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_in_string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_in_string : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_int
// desc: insert int value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_float
// desc: insert float value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_complex
// desc: insert complex value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_complex : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_polar
// desc: insert polar value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_polar : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_vec2
// desc: insert vec2 value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_vec2 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_vec3
// desc: insert vec3 value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_vec3 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_vec4
// desc: insert vec4 value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_vec4 : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_string
// desc: insert string value into output stream
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_string : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Hack
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Hack : public Chuck_Instr
{
public:
    Chuck_Instr_Hack( Chuck_Type * type );
    virtual ~Chuck_Instr_Hack();
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

protected:
    Chuck_Type * m_type_ref;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Gack
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Gack : public Chuck_Instr
{
public:
    Chuck_Instr_Gack( const std::vector<Chuck_Type *> & types );
    virtual ~Chuck_Instr_Gack();
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const;

protected:
    std::vector<Chuck_Type *> m_type_refs;
};




// runtime functions (REFACTOR-2017: added overloads)
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM_Shred * shred );
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM * vm );
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM_Shred * shred, Chuck_VM * vm );
// initialize object using Type | 1.5.1.5 (ge) added setShredOrigin flag
t_CKBOOL initialize_object( Chuck_Object * obj, Chuck_Type * type, Chuck_VM_Shred * shred, Chuck_VM * vm, t_CKBOOL setShredOrigin = FALSE );

// "throw exception" (halt current shred, print message)
void ck_throw_exception(Chuck_VM_Shred * shred, const char * name);
void ck_throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKINT desc);
void ck_throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKFLOAT desc);
void ck_throw_exception(Chuck_VM_Shred * shred, const char * name, const char * desc);
// handle overflow (halt current shred, print message + possible reason)
void ck_handle_overflow( Chuck_VM_Shred * shred, Chuck_VM * vm, const std::string & reason = "" );

// define SP offset
#define push_( sp, val )         do { *(sp) = (val); (sp)++; } while(0)
#define push_float( sp, val )    do { *((t_CKFLOAT *&)sp) = (val); ((t_CKFLOAT *&)sp)++; } while(0)
#define push_complex( sp, val )  do { *((t_CKCOMPLEX *&)sp) = (val); ((t_CKCOMPLEX *&)sp)++; } while(0)
#define push_vec2( sp, val )     do { *((t_CKVEC2 *&)sp) = (val); ((t_CKVEC2 *&)sp)++; } while(0)
#define push_vec3( sp, val )     do { *((t_CKVEC3 *&)sp) = (val); ((t_CKVEC3 *&)sp)++; } while(0)
#define push_vec4( sp, val )     do { *((t_CKVEC4 *&)sp) = (val); ((t_CKVEC4 *&)sp)++; } while(0)
#define pop_( sp, n )            do { sp -= (n); } while(0)
#define val_( sp )               *(sp)

// stack overflow detection
#define overflow_( stack )       ( stack->sp > stack->sp_max )
#define underflow_( stack )      ( stack->sp < stack->stack )
// test if a particular sp would overflow the stack
#define would_overflow_( sp, stack )  ( (t_CKBYTE *)(sp) > stack->sp_max )
#define would_underflow_( sp, stack ) ( (t_CKBYTE *)(sp) < stack->stack )




#endif
