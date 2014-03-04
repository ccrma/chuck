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
#include <vector>


// forward reference
struct Chuck_VM;
struct Chuck_VM_Shred;
struct Chuck_Type;
struct Chuck_Func;




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr
{
public:
    virtual ~Chuck_Instr() { }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred ) = 0;

public:
    virtual const char * name() const;
    virtual const char * params() const 
    { return ""; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Branch_Op
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Branch_Op : public Chuck_Instr
{
public:
    inline void set( t_CKUINT jmp ) { m_jmp = jmp; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%ld", m_jmp ); return buffer; }

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
    inline void set( t_CKUINT val ) { m_val = val; }
    inline t_CKUINT get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%ld", m_val ); return buffer; }

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
    inline void set( t_CKFLOAT val ) { m_val = val; }
    inline t_CKFLOAT get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%.6f", m_val ); return buffer; }

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
// name: struct Chuck_Instr_Add_string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Add_string : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




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
// name: struct Chuck_Instr_Reg_Pop_Word
// desc: pop word from reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Word : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Word2
// desc: pop t_CKFLOAT word from reg stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Word2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Word3
// desc: pop a complex value from reg stack (change 1.3.1.0)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Word3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Reg_Pop_Word4
// desc: pop arbitrary num of word from reg stack (added 1.3.1.0)
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_Pop_Word4 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Pop_Word4( t_CKUINT num ) { this->set( num ); }
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
    { static char buffer[256];
      sprintf( buffer, "src=%ld, base=%ld", m_val, base );
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
    { static char buffer[256];
      sprintf( buffer, "src=%ld, base=%ld", m_val, base );
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
    { static char buffer[256];
      sprintf( buffer, "src=%ld, base=%ld", m_val, base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
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
    { static char buffer[256];
      sprintf( buffer, "src=%ld, base=%ld", m_val, base );
      return buffer; }

protected:
    // use global stack base
    t_CKBOOL base;
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
    { static char buffer[256];
      sprintf( buffer, "offset=%ld, value=%ld", m_offset, m_val );
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
    { static char buffer[256];
      sprintf( buffer, "offset=%ld, value=%f", m_offset, m_val );
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
// name: struct Chuck_Instr_Instantiate_Object
// desc: instantiate object - leaves reference value on operand stack
//-----------------------------------------------------------------------------
struct Chuck_Instr_Instantiate_Object : public Chuck_Instr
{
public:
    Chuck_Instr_Instantiate_Object( Chuck_Type * t )
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
    // virtual const char * params() const;

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
//-----------------------------------------------------------------------------
struct Chuck_Instr_Reg_AddRef_Object3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Release_Object
// desc: ...
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
// name: struct Chuck_Instr_Func_To_Code
// desc: Chuck_Func * to Chuck_VM_Code *
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_To_Code : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call
// desc: user-defined function call
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call_Member
// desc: imported member function call with return
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call_Member : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Func_Call_Member( t_CKUINT ret_size )
    { this->set( ret_size ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Func_Call_Static
// desc: imported static function call with return
//-----------------------------------------------------------------------------
struct Chuck_Instr_Func_Call_Static : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Func_Call_Static( t_CKUINT ret_size )
    { this->set( ret_size ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
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
// name: struct Chuck_Instr_Spork
// desc: ...
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
// name: struct Chuck_Instr_Array_Init
// desc: for [ ... ] values
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Init : public Chuck_Instr
{
public:
    Chuck_Instr_Array_Init( Chuck_Type * the_type, t_CKINT length );
    virtual ~Chuck_Instr_Array_Init();

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const { return m_param_str; }

protected:
    Chuck_Type * m_type_ref;
    t_CKINT m_length;
    t_CKBOOL m_is_obj;
    char * m_param_str;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Array_Alloc
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Array_Alloc : public Chuck_Instr
{
public:
    Chuck_Instr_Array_Alloc( t_CKUINT depth, Chuck_Type * the_type,
        t_CKUINT offset, t_CKBOOL ref );
    virtual ~Chuck_Instr_Array_Alloc();

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const { return m_param_str; }

protected:
    t_CKUINT m_depth;
    Chuck_Type * m_type_ref;
    t_CKBOOL m_is_obj;
    char * m_param_str;
    t_CKUINT m_stack_offset;
    t_CKBOOL m_is_ref;
};




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
    { static char buffer[256];
      sprintf( buffer, "kind=%ld, emit_addr=%ld istr=%ld", 
               m_kind, m_emit_addr, m_istr );
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
    { static char buffer[256];
      sprintf( buffer, "kind=%ld, emit_addr=%ld", m_kind, m_emit_addr );
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
    { static char buffer[256];
      sprintf( buffer, "depth=%ld, kind=%ld, emit_addr=%ld", m_depth, m_kind, m_emit_addr );
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
    { static char buffer[256];
      sprintf( buffer, "offset=%ld, kind=%ld, emit_addr=%ld", m_offset, m_kind, m_emit_addr );
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
    { static char buffer[256];
      sprintf( buffer, "offset=%ld", m_offset );
      return buffer; }

protected:
    t_CKUINT m_offset;
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
    { static char buffer[256];
      sprintf( buffer, "offset=%ld, size=%ld, kind=%ld, emit_addr=%ld", m_offset, m_size, m_kind, m_emit_addr );
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
    { static char buffer[256];
      sprintf( buffer, "addr=%ld, kind=%ld, emit_addr=%ld", (t_CKUINT)m_addr, m_kind, m_emit_addr );
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
    { static char buffer[256];
      sprintf( buffer, "func=%ld", (t_CKUINT)m_func );
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
    Chuck_Instr_Dot_Cmp_First( t_CKUINT is_mem, t_CKUINT emit_addr )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[256];
      sprintf( buffer, "is_mem=%ld, emit_addr=%ld", m_is_mem, m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Dot_Cmp_Second
// desc: access the complex value's second component
//-----------------------------------------------------------------------------
struct Chuck_Instr_Dot_Cmp_Second : public Chuck_Instr
{
public:
    Chuck_Instr_Dot_Cmp_Second( t_CKUINT is_mem, t_CKUINT emit_addr )
    { m_is_mem = is_mem; m_emit_addr = emit_addr; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    virtual const char * params() const
    { static char buffer[256];
      sprintf( buffer, "is_mem=%ld, emit_addr=%ld", m_is_mem, m_emit_addr );
      return buffer; }

protected:
    t_CKUINT m_is_mem;
    t_CKUINT m_emit_addr;
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
    Chuck_Instr_UGen_Array_Link( t_CKBOOL srcIsArray, t_CKBOOL dstIsArray ) :
    m_srcIsArray(srcIsArray), m_dstIsArray(dstIsArray)
    { }

    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    
protected:
    t_CKBOOL m_srcIsArray, m_dstIsArray;
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
// name: struct Chuck_Instr_Cast_object2string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Cast_object2string : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Op_string
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Op_string : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Op_string( t_CKUINT v ) { this->set( v ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_Init_Loop_Counter
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_Init_Loop_Counter : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Init_Loop_Counter( t_CKUINT v )
    { this->set( v ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
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
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_float
// desc: ...
//-----------------------------------------------------------------------------
struct Chuck_Instr_IO_out_float : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_IO_out_string
// desc: ...
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




// runtime functions
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM_Shred * shred );
// initialize object using Type
t_CKBOOL initialize_object( Chuck_Object * obj, Chuck_Type * type );
// "throw exception" (halt current shred, print message)
void throw_exception(Chuck_VM_Shred * shred, const char * name);
void throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKINT desc);
void throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKFLOAT desc);
void throw_exception(Chuck_VM_Shred * shred, const char * name, const char * desc);


// define SP offset
#define push_( sp, val )         *(sp) = (val); (sp)++
#define push_float( sp, val )    *((t_CKFLOAT *&)sp) = (val); ((t_CKFLOAT *&)sp)++
#define push_complex( sp, val )    *((t_CKCOMPLEX *&)sp) = (val); ((t_CKCOMPLEX *&)sp)++
#define pop_( sp, n )            sp -= (n)
#define val_( sp )               *(sp)

// stack overflow detection
#define overflow_( stack )       ( stack->sp > stack->sp_max )
#define underflow_( stack )      ( stack->sp < stack->stack )




#endif
