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
// file: chuck_instr.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_INSTR_H__
#define __CHUCK_INSTR_H__

#include <stdio.h>




// forward reference
class Chuck_VM;
class Chuck_VM_Shred;

// define
#define sint long
#define uint unsigned long




//-----------------------------------------------------------------------------
// name: class Chuck_Instr
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr
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
// name: class Chuck_Instr_Branch_Op
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Op : public Chuck_Instr
{
public:
    inline void set( uint jmp ) { m_jmp = jmp; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%d", m_jmp ); return buffer; }

protected:
    uint m_jmp;
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Unary_Op
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Unary_Op : public Chuck_Instr
{
public:
    inline void set( uint val ) { m_val = val; }
    inline uint get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%d", m_val ); return buffer; }

protected:
    uint m_val;
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Unary_Op2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Unary_Op2 : public Chuck_Instr
{
public:
    inline void set( double val ) { m_val = val; }
    inline double get() { return m_val; }

public:
    virtual const char * params() const
    { static char buffer[256]; sprintf( buffer, "%.6f", m_val ); return buffer; }

protected:
    double m_val;
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_Op
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_Op : public Chuck_Instr
{ };




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Inc_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Inc_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};





//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Dec_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Dec_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Complement_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Complement_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mod_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Mod_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Minus_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Minus_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Times_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Times_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Divide_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Divide_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Inc_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Inc_uint : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};





//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Dec_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Dec_uint : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Complement_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Complement_uint : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mod_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Mod_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Minus_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Minus_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Times_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Times_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Divide_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Divide_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Minus_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Minus_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Times_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Times_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Divide_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Divide_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mod_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Mod_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Minus_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Minus_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Times_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Times_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Divide_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Divide_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mod_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Mod_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_dur
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_dur : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Add_dur_time
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Add_dur_time : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Lt_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Lt_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Gt_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Gt_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Le_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Le_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Ge_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Ge_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Eq_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Eq_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Neq_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Neq_int : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_int( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Lt_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Lt_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Gt_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Gt_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Le_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Le_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Ge_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Ge_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Eq_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Eq_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Neq_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Neq_uint : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_uint( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Lt_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Lt_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Gt_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Gt_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Le_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Le_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Ge_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Ge_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Eq_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Eq_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Neq_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Neq_single : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_single( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Lt_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Lt_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Lt_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Gt_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Gt_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Gt_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Le_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Le_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Le_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Ge_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Ge_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Ge_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Eq_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Eq_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Eq_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Branch_Neq_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Branch_Neq_double : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Branch_Neq_double( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Lt_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Lt_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Gt_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Gt_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Le_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Le_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Ge_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Ge_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Eq_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Eq_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Neq_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Neq_int : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Not_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Not_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Negate_int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Negate_int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Negate_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Negate_uint : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Negate_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Negate_single : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Negate_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Negate_double : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Lt_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Lt_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Gt_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Gt_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Le_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Le_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Ge_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Ge_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Eq_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Eq_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Neq_uint
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Neq_uint : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Lt_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Lt_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Gt_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Gt_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Le_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Le_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Ge_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Ge_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Eq_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Eq_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Neq_single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Neq_single : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Lt_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Lt_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Gt_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Gt_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Le_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Le_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Ge_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Ge_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Eq_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Eq_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Neq_double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Neq_double : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_And
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_And : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_Or
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_Or : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_Xor
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_Xor : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_Shift_Right
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_Shift_Right : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Binary_Shift_Left
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Binary_Shift_Left : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_And
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_And : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Or
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Or : public Chuck_Instr_Binary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Goto
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Goto : public Chuck_Instr_Branch_Op
{
public:
    Chuck_Instr_Goto( uint jmp ) { this->set( jmp ); }
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Pop_Word
// desc: pop word from reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Pop_Word : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Pop_Word2
// desc: pop double word from reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Pop_Word2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Pop_Mem
// desc: pop word, and copy it to the mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Pop_Mem: public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Pop_Mem2
// desc: pop double word, and copy it to the mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Pop_Mem2: public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Imm
// desc: push immediate to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Imm : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Imm( uint val )
    { this->set( val ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Imm2
// desc: push double immediate to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Imm2 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Reg_Push_Imm2( double val )
    { this->set( val ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Now
// desc: push value of now to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Now : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Start
// desc: push value of start to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Start : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Maybe
// desc: push immediate to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Maybe : public Chuck_Instr_Unary_Op
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Mem
// desc: push a variable from mem stack to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Mem : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem( uint src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Mem2
// desc: push a double variable from mem stack to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Mem2 : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Mem2( uint src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Deref
// desc: push a double variable from main memory to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Deref : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Reg_Push_Deref( uint src, uint size )
    { this->set( src ); m_size = size; }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
    uint m_size;
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Reg_Push_Deref
// desc: push a double variable from main memory to reg stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Reg_Push_Deref2 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Reg_Push_Deref2( double src )
    { this->set( src ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mem_Push_Imm
// desc: push a value unto mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Mem_Push_Imm : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Mem_Push_Imm( uint src )
    { this->set( src ); }
    
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};



//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mem_Push_Imm2
// desc: push a double value unto mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Mem_Push_Imm2 : public Chuck_Instr_Unary_Op2
{
public:
    Chuck_Instr_Mem_Push_Imm2( double src )
    { this->set( src ); }
    
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};



//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mem_Pop_Word
// desc: pop a value from mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Mem_Pop_Word : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Mem_Pop_Word2
// desc: pop a value from mem stack
//-----------------------------------------------------------------------------
class Chuck_Instr_Mem_Pop_Word2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Nop
// desc: no op
//-----------------------------------------------------------------------------
class Chuck_Instr_Nop : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_EOC
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_EOC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign_Deref
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign_Deref : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign_Deref2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign_Deref2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign_Object
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign_Object : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign_Object_Deref
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign_Object_Deref : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Assign_Object2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Assign_Object2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Chuck_Release_Object
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Chuck_Release_Object : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Func_Call
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Func_Call : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Func_Call2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Func_Call2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Func_Call3
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Func_Call3 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Func_Call0
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Func_Call0 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Func_Return
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Func_Return : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Spork
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Spork : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Spork( uint v = 0 ) { this->set( v ); }
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: enum CI_PrintTypeEnum
// desc: ...
//-----------------------------------------------------------------------------
enum CI_PrintTypeEnum
{
    cip_int = 1,
    cip_uint,
    cip_float,
    cip_double,
    cip_single,
    cip_string,
    cip_dur,
    cip_time,
    cip_object
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Print_Console
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Print_Console : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Print_Console2
// desc: ..
//-----------------------------------------------------------------------------
class Chuck_Instr_Print_Console2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Time_Advance
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Time_Advance : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Midi_Out
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Midi_Out : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Midi_Out( uint mode )
    { this->set(mode); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Midi_Out_Go
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Midi_Out_Go : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Midi_Out_Go( uint mode )
    { this->set( mode ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Midi_In
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Midi_In : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Midi_In( uint mode )
    { this->set(mode); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Midi_In_Go
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Midi_In_Go : public Chuck_Instr_Unary_Op
{
public:
    Chuck_Instr_Midi_In_Go( uint mode )
    { this->set( mode ); }

public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_ADC
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_ADC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_DAC
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_DAC : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Bunghole
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Bunghole : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Link
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Link : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_UnLink
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_UnLink : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Alloc
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Alloc : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_DeAlloc
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_DeAlloc : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Ctrl
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Ctrl : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_CGet
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_CGet : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Ctrl2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Ctrl2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_CGet2
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_CGet2 : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_PMsg
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_PMsg : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Ctrl_Op
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Ctrl_Op : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_CGet_Op
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_CGet_Op : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_CGet_Gain
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_CGet_Gain : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_Ctrl_Gain
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_Ctrl_Gain : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_UGen_CGet_Last
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_UGen_CGet_Last : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_DLL_Load
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_DLL_Load : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_DLL_Unload
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_DLL_Unload : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_single2int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_single2int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_int2single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_int2single : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_double2int
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_double2int : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_int2double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_int2double : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_single2double
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_single2double : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Cast_double2single
// desc: ...
//-----------------------------------------------------------------------------
class Chuck_Instr_Cast_double2single : public Chuck_Instr
{
public:
    virtual void execute( Chuck_VM * vm, Chuck_VM_Shred * shred );
};




#endif
