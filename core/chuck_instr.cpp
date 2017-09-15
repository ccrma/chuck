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
// file: chuck_instr.cpp
// desc: chuck virtual machine instruction set
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <math.h>
#include <limits.h>

#include "chuck_type.h"
#include "chuck_lang.h"
#include "chuck_instr.h"
#include "chuck_vm.h"
#include "chuck_ugen.h"
#include "chuck_dl.h"
#include "chuck_errmsg.h"

#include "util_string.h"

#include <typeinfo>
using namespace std;




//-----------------------------------------------------------------------------
// name: Chuck_Instr()
// desc: base class constructor
//-----------------------------------------------------------------------------
Chuck_Instr::Chuck_Instr() {
    // set linepos to 0 so we can tell later whether it has been set properly
    m_linepos = 0;
}




//-----------------------------------------------------------------------------
// name: name()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_Instr::name() const
{
     return mini_type( typeid(*this).name() );
}




//-----------------------------------------------------------------------------
// name: set_linepos()
// desc: store line position for error messages
//-----------------------------------------------------------------------------
void Chuck_Instr::set_linepos(t_CKUINT linepos) {
    m_linepos = linepos;
}




//-----------------------------------------------------------------------------
// name: handle_overflow()
// desc: stack overflow
//-----------------------------------------------------------------------------
static void handle_overflow( Chuck_VM_Shred * shred, Chuck_VM * vm )
{
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): Exception StackOverflow in shred[id=%lu:%s], PC=[%lu]\n",
        shred->xid, shred->name.c_str(), shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


#pragma mark === Integer Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_PreInc_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKINT **& reg_sp = (t_CKINT **&)shred->reg->sp;
    t_CKINT *&  the_sp = (t_CKINT *&)shred->reg->sp;

    // pointer
    pop_( reg_sp, 1 );
    // increment value
    (**(reg_sp))++;
    // value on stack
    push_( the_sp, **(reg_sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_PostInc_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKINT **& reg_sp = (t_CKINT **&)shred->reg->sp;
    t_CKINT *&  the_sp = (t_CKINT *&)shred->reg->sp;
    t_CKINT *   ptr;

    // pointer
    pop_( reg_sp, 1 );
    // copy
    ptr = *reg_sp;
    // value on stack
    push_( the_sp, **(reg_sp) );
    // increment value
    (*(ptr))++;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_PreDec_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKINT **& reg_sp = (t_CKINT **&)shred->reg->sp;
    t_CKINT *&  the_sp = (t_CKINT *&)shred->reg->sp;

    // pointer
    pop_( reg_sp, 1 );
    // decrement value
    (**(reg_sp))--;
    // value on stack
    push_( the_sp, **(reg_sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_PostDec_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKINT **& reg_sp = (t_CKINT **&)shred->reg->sp;
    t_CKINT *&  the_sp = (t_CKINT *&)shred->reg->sp;
    t_CKINT *   ptr;

    // pointer
    pop_( reg_sp, 1 );
    // copy
    ptr = *reg_sp;
    // value on stack
    push_( the_sp, **(reg_sp) );
    // decrement value
    (*(ptr))--;
}




//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Dec_int_Addr
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dec_int_Addr::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // decrement value
    (*((t_CKINT *)(m_val)))--;
}




//-----------------------------------------------------------------------------
// name: exexute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Complement_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    (*(sp-1)) = ~(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) % val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_int_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp+1) % val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_int_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp+1) - val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp+1) == 0 ) goto div_zero;
    push_( sp, val_(sp) / val_(sp+1) );
    
    return;
div_zero:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): DivideByZeroException: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str());
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_int_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == 0 ) goto div_zero;
    push_( sp, val_(sp+1) / val_(sp) );
    
    return;
div_zero:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): DivideByZeroException: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str());
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Float Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_double_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp+1) - val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) / val_(sp+1) );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_double_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp+1) / val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, ::fmod( val_(sp), val_(sp+1) ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_double_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, ::fmod( val_(sp+1), val_(sp) ) );
}



#pragma mark === Complex Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im;
    pop_( sp, 2 );
    re = sp->re + (sp+1)->re;
    im = sp->im + (sp+1)->im;
    push_( sp_float, re );
    push_( sp_float, im );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im;
    pop_( sp, 2 );
    re = sp->re - (sp+1)->re;
    im = sp->im - (sp+1)->im;
    push_( sp_float, re );
    push_( sp_float, im );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_complex_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im;
    pop_( sp, 2 );
    re = (sp+1)->re - sp->re;
    im = (sp+1)->im - sp->im;
    push_( sp_float, re );
    push_( sp_float, im );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im;
    pop_( sp, 2 );
    re = sp->re * (sp+1)->re - sp->im * (sp+1)->im;
    im = sp->re * (sp+1)->im + sp->im * (sp+1)->re;
    push_( sp_float, re );
    push_( sp_float, im );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im, denom;

    // pop
    pop_( sp, 2 );
    // complex division -> * complex conjugate of divisor
    denom = (sp+1)->re*(sp+1)->re + (sp+1)->im*(sp+1)->im;
    // go
    re = sp->re*(sp+1)->re + sp->im*(sp+1)->im;
    im = sp->im*(sp+1)->re - sp->re*(sp+1)->im;
    // result
    push_( sp_float, re/denom );
    push_( sp_float, im/denom );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_complex_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT re, im, denom;

    // pop
    pop_( sp, 2 );
    // complex division -> * complex conjugate of divisor
    denom = sp->re*sp->re + sp->im*sp->im;
    // go
    re = sp->re*(sp+1)->re + sp->im*(sp+1)->im;
    im = (sp+1)->im*sp->re - (sp+1)->re*sp->im;
    // result
    push_( sp_float, re/denom );
    push_( sp_float, im/denom );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKCOMPLEX a, b;
    pop_( sp, 2 );
    a.re = sp->modulus * ::cos( sp->phase );
    a.im = sp->modulus * ::sin( sp->phase );
    b.re = (sp+1)->modulus * ::cos( (sp+1)->phase );
    b.im = (sp+1)->modulus * ::sin( (sp+1)->phase );
    a.re += b.re;
    a.im += b.im;
    push_( sp_float, ::hypot( a.re, a.im ) );
    push_( sp_float, ::atan2( a.im, a.re ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKCOMPLEX a, b;
    pop_( sp, 2 );
    a.re = sp->modulus * ::cos( sp->phase );
    a.im = sp->modulus * ::sin( sp->phase );
    b.re = (sp+1)->modulus * ::cos( (sp+1)->phase );
    b.im = (sp+1)->modulus * ::sin( (sp+1)->phase );
    a.re -= b.re;
    a.im -= b.im;
    push_( sp_float, ::hypot( a.re, a.im ) );
    push_( sp_float, ::atan2( a.im, a.re ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_polar_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKCOMPLEX a, b;
    pop_( sp, 2 );
    a.re = sp->modulus * ::cos( sp->phase );
    a.im = sp->modulus * ::sin( sp->phase );
    b.re = (sp+1)->modulus * ::cos( (sp+1)->phase );
    b.im = (sp+1)->modulus * ::sin( (sp+1)->phase );
    a.re = b.re - a.re;
    a.im = b.im - a.im;
    push_( sp_float, ::hypot( a.re, a.im ) );
    push_( sp_float, ::atan2( a.im, a.re ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT mag, phase;
    pop_( sp, 2 );
    mag = sp->modulus * (sp+1)->modulus;
    phase = sp->phase + (sp+1)->phase;
    push_( sp_float, mag );
    push_( sp_float, phase );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT mag, phase;
    pop_( sp, 2 );
    mag = sp->modulus / (sp+1)->modulus;
    phase = sp->phase - (sp+1)->phase;
    push_( sp_float, mag );
    push_( sp_float, phase );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_polar_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR *& sp = (t_CKPOLAR *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    t_CKFLOAT mag, phase;
    pop_( sp, 2 );
    mag = (sp+1)->modulus / (sp)->modulus;
    phase = (sp+1)->phase - sp->phase;
    push_( sp_float, mag );
    push_( sp_float, phase );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add two vec3; ge: added 1.3.5.3 with other vec3 vec4 instructions
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC3 r;
    r.x = sp->x + (sp+1)->x;
    r.y = sp->y + (sp+1)->y;
    r.z = sp->z + (sp+1)->z;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
    push_( sp_float, r.z );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC3 r;
    r.x = sp->x - (sp+1)->x;
    r.y = sp->y - (sp+1)->y;
    r.z = sp->z - (sp+1)->z;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
    push_( sp_float, r.z );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_XProduct_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC3 result;
    result.x = (sp->y * (sp+1)->z) - (sp->z * (sp+1)->y);
    result.y = (sp->z * (sp+1)->x) - (sp->x * (sp+1)->z);
    result.z = (sp->x * (sp+1)->y) - (sp->y * (sp+1)->x);
    push_( sp_float, result.x );
    push_( sp_float, result.y );
    push_( sp_float, result.z );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC3 );
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)sp);
    // the vector
    t_CKVEC3 v = *((t_CKVEC3 *)(sp+sz_FLOAT));
    // result
    t_CKVEC3 r;
    r.x = v.x * f;
    r.y = v.y * f;
    r.z = v.z * f;
    // pointer as vec3
    t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)sp;
    // push on reg stack
    push_( sp_vec3, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec3_Times_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC3 );
    // the vector
    t_CKVEC3 v = *((t_CKVEC3 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC3));
    // result
    t_CKVEC3 r;
    r.x = v.x * f;
    r.y = v.y * f;
    r.z = v.z * f;
    // pointer as vec3
    t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)sp;
    // push on reg stack
    push_( sp_vec3, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec3_Divide_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC3 );
    // the vector
    t_CKVEC3 v = *((t_CKVEC3 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC3));
    // result
    t_CKVEC3 r;
    r.x = v.x / f;
    r.y = v.y / f;
    r.z = v.z / f;
    // pointer as vec3
    t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)sp;
    // push on reg stack
    push_( sp_vec3, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC4 r;
    r.x = sp->x + (sp+1)->x;
    r.y = sp->y + (sp+1)->y;
    r.z = sp->z + (sp+1)->z;
    r.w = sp->w + (sp+1)->w;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
    push_( sp_float, r.z );
    push_( sp_float, r.w );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC4 r;
    r.x = sp->x - (sp+1)->x;
    r.y = sp->y - (sp+1)->y;
    r.z = sp->z - (sp+1)->z;
    r.w = sp->w - (sp+1)->w;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
    push_( sp_float, r.z );
    push_( sp_float, r.w );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_XProduct_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC4 result;
    result.x = (sp->y * (sp+1)->z) - (sp->z * (sp+1)->y);
    result.y = (sp->z * (sp+1)->x) - (sp->x * (sp+1)->z);
    result.z = (sp->x * (sp+1)->y) - (sp->y * (sp+1)->x);
    result.w = 0;
    push_( sp_float, result.x );
    push_( sp_float, result.y );
    push_( sp_float, result.z );
    push_( sp_float, result.w );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC4 );
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)sp);
    // the vector
    t_CKVEC4 v = *((t_CKVEC4 *)(sp+sz_FLOAT));
    // result
    t_CKVEC4 r;
    r.x = v.x * f;
    r.y = v.y * f;
    r.z = v.z * f;
    r.w = v.w * f;
    // pointer as vec4
    t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)sp;
    // push on reg stack
    push_( sp_vec4, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec4_Times_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC4 );
    // the vector
    t_CKVEC4 v = *((t_CKVEC4 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC4));
    // result
    t_CKVEC4 r;
    r.x = v.x * f;
    r.y = v.y * f;
    r.z = v.z * f;
    r.w = v.w * f;
    // pointer as vec4
    t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)sp;
    // push on reg stack
    push_( sp_vec4, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec4_Divide_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC4 );
    // the vector
    t_CKVEC4 v = *((t_CKVEC4 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC4));
    // result
    t_CKVEC4 r;
    r.x = v.x / f;
    r.y = v.y / f;
    r.z = v.z / f;
    r.w = v.w / f;
    // pointer as vec4
    t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)sp;
    // push on reg stack
    push_( sp_vec4, r );
}




#pragma mark === Arithmetic Assignment ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_int_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) += val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_int_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) %= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_int_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) -= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_int_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) *= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_int_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == 0 ) goto div_zero;
    temp = **(t_CKINT **)(sp+1) /= val_(sp);
    push_( sp, temp );
    
    return;
div_zero:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): DivideByZeroException: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str());
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_double_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT temp;
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pop value + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    // assign
    temp = **(t_CKFLOAT **)(sp+sz_FLOAT) += val_((t_CKFLOAT *&)sp);
    // push result
    push_( (t_CKFLOAT *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_double_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT temp;
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pop value + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    // assign
    temp = **(t_CKFLOAT **)(sp+sz_FLOAT) -= val_((t_CKFLOAT *&)sp);
    // push result
    push_( (t_CKFLOAT *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_double_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT temp;
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pop value + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    // assign
    temp = **(t_CKFLOAT **)(sp+sz_FLOAT) *= val_((t_CKFLOAT *&)sp);
    // push result
    push_( (t_CKFLOAT *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_double_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT temp;
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pop value + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    // assign
    temp = **(t_CKFLOAT **)(sp+sz_FLOAT) /= val_((t_CKFLOAT *&)sp);
    // push result
    push_( (t_CKFLOAT *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_double_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT temp;
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pop value + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    // assign
    temp = ::fmod( **(t_CKFLOAT **)(sp+sz_FLOAT), val_((t_CKFLOAT *&)sp) );
    **(t_CKFLOAT **)(sp+sz_FLOAT) = temp;
    // push result
    push_( (t_CKFLOAT *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_complex_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKCOMPLEX temp;
    // pop value + pointer
    pop_( sp, sz_COMPLEX + sz_UINT );

    // assign
    temp.re = (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->re += ((t_CKCOMPLEX *&)sp)->re;
    temp.im = (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->im += ((t_CKCOMPLEX *&)sp)->im;
    // push result
    push_( (t_CKCOMPLEX *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_complex_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKCOMPLEX temp;
    // pop value + pointer
    pop_( sp, sz_COMPLEX + sz_UINT );

    // assign
    temp.re = (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->re -= ((t_CKCOMPLEX *&)sp)->re;
    temp.im = (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->im -= ((t_CKCOMPLEX *&)sp)->im;
    // push result
    push_( (t_CKCOMPLEX *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_complex_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKCOMPLEX temp, a, b;
    // pop value + pointer
    pop_( sp, sz_COMPLEX + sz_UINT );
    // copy
    a = **(t_CKCOMPLEX **)(sp+sz_COMPLEX);
    b = *(t_CKCOMPLEX *&)sp;
    // calculate
    temp.re = a.re * b.re - a.im * b.im;
    temp.im = a.re * b.im + a.im * b.re;
    // assign
    (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->re = temp.re;
    (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->im = temp.im;
    // push result
    push_( (t_CKCOMPLEX *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_complex_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKCOMPLEX temp, a, b;
    t_CKFLOAT denom;
    // pop value + pointer
    pop_( sp, sz_COMPLEX + sz_UINT );
    // copy
    a = **(t_CKCOMPLEX **)(sp+sz_COMPLEX);
    b = *(t_CKCOMPLEX *&)sp;
    // calculate
    temp.re = a.re * b.re + a.im * b.im;
    temp.im = a.im * b.re - a.re * b.im;
    denom = b.re * b.re + b.im * b.im;
    temp.re /= denom;
    temp.im /= denom;
    // assign
    (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->re = temp.re;
    (*(t_CKCOMPLEX **)(sp+sz_COMPLEX))->im = temp.im;
    // push result
    push_( (t_CKCOMPLEX *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_polar_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKPOLAR result, * pa, * pb;
    t_CKCOMPLEX temp, a, b;
    // pop value + pointer
    pop_( sp, sz_POLAR + sz_UINT );
    // pointer copy
    pa = *(t_CKPOLAR **)(sp+sz_POLAR);
    pb = (t_CKPOLAR *&)sp;
    // rectangular
    a.re = pa->modulus * ::cos(pa->phase);
    a.im = pa->modulus * ::sin(pa->phase);
    b.re = pb->modulus * ::cos(pb->phase);
    b.im = pb->modulus * ::sin(pb->phase);
    // calculate
    temp.re = a.re + b.re;
    temp.im = a.im + b.im;
    // assign
    result.modulus = (*(t_CKPOLAR **)(sp+sz_POLAR))->modulus = ::hypot(temp.re,temp.im);
    result.phase = (*(t_CKPOLAR **)(sp+sz_POLAR))->phase = ::atan2(temp.im,temp.re);
    // push result
    push_( (t_CKPOLAR *&)sp, result );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_polar_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKPOLAR result, * pa, * pb;
    t_CKCOMPLEX temp, a, b;
    // pop value + pointer
    pop_( sp, sz_POLAR + sz_UINT );
    // pointer copy
    pa = *(t_CKPOLAR **)(sp+sz_POLAR);
    pb = (t_CKPOLAR *&)sp;
    // rectangular
    a.re = pa->modulus * ::cos(pa->phase);
    a.im = pa->modulus * ::sin(pa->phase);
    b.re = pb->modulus * ::cos(pb->phase);
    b.im = pb->modulus * ::sin(pb->phase);
    // calculate
    temp.re = a.re - b.re;
    temp.im = a.im - b.im;
    // assign
    result.modulus = (*(t_CKPOLAR **)(sp+sz_POLAR))->modulus = ::hypot(temp.re,temp.im);
    result.phase = (*(t_CKPOLAR **)(sp+sz_POLAR))->phase = ::atan2(temp.im,temp.re);
    // push result
    push_( (t_CKPOLAR *&)sp, result );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_polar_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKPOLAR temp;
    // pop value + pointer
    pop_( sp, sz_POLAR + sz_UINT );

    // assign
    temp.modulus = (*(t_CKPOLAR **)(sp+sz_POLAR))->modulus *= ((t_CKPOLAR *&)sp)->modulus;
    temp.phase = (*(t_CKPOLAR **)(sp+sz_POLAR))->phase += ((t_CKPOLAR *&)sp)->phase;
    // push result
    push_( (t_CKPOLAR *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_polar_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKPOLAR temp;
    // pop value + pointer
    pop_( sp, sz_POLAR + sz_UINT );

    // assign
    temp.modulus = (*(t_CKPOLAR **)(sp+sz_POLAR))->modulus /= ((t_CKPOLAR *&)sp)->modulus;
    temp.phase = (*(t_CKPOLAR **)(sp+sz_POLAR))->phase -= ((t_CKPOLAR *&)sp)->phase;
    // push result
    push_( (t_CKPOLAR *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec3_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC3 temp;
    // pop value + pointer
    pop_( sp, sz_VEC3 + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC3 **)(sp+sz_VEC3))->x += ((t_CKVEC3 *&)sp)->x;
    temp.y = (*(t_CKVEC3 **)(sp+sz_VEC3))->y += ((t_CKVEC3 *&)sp)->y;
    temp.z = (*(t_CKVEC3 **)(sp+sz_VEC3))->z += ((t_CKVEC3 *&)sp)->z;
    // push result
    push_( (t_CKVEC3 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec3_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC3 temp;
    // pop value + pointer
    pop_( sp, sz_VEC3 + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC3 **)(sp+sz_VEC3))->x -= ((t_CKVEC3 *&)sp)->x;
    temp.y = (*(t_CKVEC3 **)(sp+sz_VEC3))->y -= ((t_CKVEC3 *&)sp)->y;
    temp.z = (*(t_CKVEC3 **)(sp+sz_VEC3))->z -= ((t_CKVEC3 *&)sp)->z;
    // push result
    push_( (t_CKVEC3 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec3_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC3 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC3 **)(sp+sz_FLOAT))->x *= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC3 **)(sp+sz_FLOAT))->y *= *((t_CKFLOAT *)sp);
    temp.z = (*(t_CKVEC3 **)(sp+sz_FLOAT))->z *= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC3 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec3_Divide_float_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC3 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC3 **)(sp+sz_FLOAT))->x /= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC3 **)(sp+sz_FLOAT))->y /= *((t_CKFLOAT *)sp);
    temp.z = (*(t_CKVEC3 **)(sp+sz_FLOAT))->z /= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC3 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec4_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC4 temp;
    // pop value + pointer
    pop_( sp, sz_VEC4 + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC4 **)(sp+sz_VEC4))->x += ((t_CKVEC4 *&)sp)->x;
    temp.y = (*(t_CKVEC4 **)(sp+sz_VEC4))->y += ((t_CKVEC4 *&)sp)->y;
    temp.z = (*(t_CKVEC4 **)(sp+sz_VEC4))->z += ((t_CKVEC4 *&)sp)->z;
    temp.w = (*(t_CKVEC4 **)(sp+sz_VEC4))->w += ((t_CKVEC4 *&)sp)->w;
    // push result
    push_( (t_CKVEC4 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec4_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC4 temp;
    // pop value + pointer
    pop_( sp, sz_VEC4 + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC4 **)(sp+sz_VEC4))->x -= ((t_CKVEC4 *&)sp)->x;
    temp.y = (*(t_CKVEC4 **)(sp+sz_VEC4))->y -= ((t_CKVEC4 *&)sp)->y;
    temp.z = (*(t_CKVEC4 **)(sp+sz_VEC4))->z -= ((t_CKVEC4 *&)sp)->z;
    temp.w = (*(t_CKVEC4 **)(sp+sz_VEC4))->w -= ((t_CKVEC4 *&)sp)->w;
    // push result
    push_( (t_CKVEC4 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec4_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC4 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC4 **)(sp+sz_FLOAT))->x *= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC4 **)(sp+sz_FLOAT))->y *= *((t_CKFLOAT *)sp);
    temp.z = (*(t_CKVEC4 **)(sp+sz_FLOAT))->z *= *((t_CKFLOAT *)sp);
    temp.w = (*(t_CKVEC4 **)(sp+sz_FLOAT))->w *= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC4 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_vec4_Divide_float_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC4 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );
    
    // assign
    temp.x = (*(t_CKVEC4 **)(sp+sz_FLOAT))->x *= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC4 **)(sp+sz_FLOAT))->y *= *((t_CKFLOAT *)sp);
    temp.z = (*(t_CKVEC4 **)(sp+sz_FLOAT))->z *= *((t_CKFLOAT *)sp);
    temp.w = (*(t_CKVEC4 **)(sp+sz_FLOAT))->w *= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC4 *&)sp, temp );
}



#pragma mark === String Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: string + string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    Chuck_String * rhs = NULL;
    Chuck_String * result = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // left
    lhs = (Chuck_String *)(*(reg_sp));
    // right
    rhs = (Chuck_String *)(*(reg_sp+1));

    // make sure no null
    if( !rhs || !lhs ) goto null_pointer;

    // make new string
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );

    // concat
    // result->str = lhs->str + rhs->str;
    result->set( lhs->str() + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string + string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() ); // , shred->pc );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add assign string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_string_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    Chuck_String ** rhs_ptr = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // the previous reference
    rhs_ptr = (Chuck_String **)(*(reg_sp+1));
    // copy popped value into memory
    lhs = (Chuck_String *)(*(reg_sp));

    // make sure no null
    if( !(*rhs_ptr) ) goto null_pointer;

    // concat
    // (*rhs_ptr)->str += lhs->str;
    (*rhs_ptr)->set( (*rhs_ptr)->str() + lhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string +=> string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: string + int
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_string_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    t_CKINT rhs = 0;
    Chuck_String * result = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // left
    lhs = (Chuck_String *)(*(reg_sp));
    // right
    rhs = (*(t_CKINT *)(reg_sp+1));

    // make sure no null
    if( !lhs ) goto null_pointer;

    // make new string
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );

    // concat
    // result->str = lhs->str + ::itoa(rhs);
    result->set( lhs->str() + ::itoa( rhs ) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string + int) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: string + float
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_string_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    t_CKFLOAT rhs = 0;
    Chuck_String * result = NULL;

    // pop word from reg stack (1.3.1.0: add size check)
    pop_( reg_sp, 1 + (sz_FLOAT / sz_UINT) ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // left
    lhs = (Chuck_String *)(*(reg_sp));
    // right
    rhs = (*(t_CKFLOAT *)(reg_sp+1));

    // make sure no null
    if( !lhs ) goto null_pointer;

    // make new string
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );

    // concat
    // result->str = lhs->str + ::ftoa(rhs, 4);
    result->set( lhs->str() + ::ftoa( rhs, 4 ) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string + float) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: int + string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_int_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKINT lhs = 0;
    Chuck_String * rhs = NULL;
    Chuck_String * result = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // left
    lhs = (*(t_CKINT *)(reg_sp));
    // right
    rhs = (Chuck_String *)(*(reg_sp+1));

    // make sure no null
    if( !rhs ) goto null_pointer;

    // make new string
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );

    // concat
    // result->str = ::itoa(lhs) + rhs->str;
    result->set( ::itoa(lhs) + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (int + string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: float + string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_float_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKFLOAT lhs = 0;
    Chuck_String * rhs = NULL;
    Chuck_String * result = NULL;

    // pop word from reg stack (1.3.1.0: added size check)
    pop_( reg_sp, 1 + (sz_FLOAT / sz_UINT) );  // ISSUE: 64-bit (fixed 1.3.1.0)
    // left (2 word)
    lhs = (*(t_CKFLOAT *)(reg_sp));
    // right (1.3.1.0: added size)
    rhs = (Chuck_String *)(*(reg_sp+(sz_FLOAT/sz_INT))); // ISSUE: 64-bit (fixed 1.3.1.0)

    // make sure no null
    if( !rhs ) goto null_pointer;

    // make new string
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );

    // concat
    // result->str = ::ftoa(lhs, 4) + rhs->str;
    result->set( ::ftoa( lhs, 4 ) + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (int + string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add assign int string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_int_string_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKINT lhs = 0;
    Chuck_String ** rhs_ptr = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // the previous reference
    rhs_ptr = (Chuck_String **)(*(reg_sp+1));
    // copy popped value into memory
    lhs = (*(t_CKINT *)(reg_sp));

    // make sure no null
    if( !(*rhs_ptr) ) goto null_pointer;

    // concat
    // (*rhs_ptr)->str += ::itoa(lhs);
    (*rhs_ptr)->set( (*rhs_ptr)->str() + ::itoa(lhs) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: () on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add assign float string
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_float_string_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKFLOAT lhs = 0;
    Chuck_String ** rhs_ptr = NULL;

    // pop word from reg stack (1.3.1.0: added size check)
    pop_( reg_sp, 1 + (sz_FLOAT / sz_UINT) ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // the previous reference (1.3.1.0: added size check)
    rhs_ptr = (Chuck_String **)(*(reg_sp+(sz_FLOAT/sz_UINT))); // ISSUE: 64-bit (fixed 1.3.1.0)
    // copy popped value into memory
    lhs = (*(t_CKFLOAT *)(reg_sp));

    // make sure no null
    if( !(*rhs_ptr) ) goto null_pointer;

    // concat
    // (*rhs_ptr)->str += ::ftoa(lhs, 4);
    (*rhs_ptr)->set( (*rhs_ptr)->str() + ::ftoa(lhs, 4) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string + string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Stack Operations ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Imm::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Imm2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Imm4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, m_val );
    push_( reg_sp, m_val2 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Dup_Last::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // dup val into reg stack
    push_( reg_sp, *(reg_sp-1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Dup_Last2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // dup val into reg stack
    push_( reg_sp, *(reg_sp-1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Dup_Last_As_Pointer::execute(
     Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKBYTE * where = (t_CKBYTE *)shred->reg->sp;
    
    // push pointer into reg stack
    push_( reg_sp, (t_CKUINT)(where-(m_val*sz_WORD)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Now::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKTIME *& reg_sp = (t_CKTIME *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, shred->now );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Me::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, (t_CKUINT)shred );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_This::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    
    // push val into reg stack
    push_( reg_sp, *(mem_sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Start::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKTIME *& reg_sp = (t_CKTIME *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, shred->start );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Maybe::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& reg_sp = (t_CKINT *&)shred->reg->sp;

    // push val into reg stack
    float num = (float)rand() / (float)RAND_MAX;
    push_( reg_sp, num > .5 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: push the value pointed to by m_val onto register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Deref::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // (added 1.3.1.0: made this integer only)
    // ISSUE: 64-bit (fixed 1.3.1.0)
    push_( reg_sp, *((t_CKUINT *)m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: push the value pointed to by m_val onto register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Deref2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;
    
    // (added 1.3.1.0)
    push_( reg_sp, *((t_CKFLOAT *)m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: push value from memory stack to register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // push mem stack content into reg stack
    push_( reg_sp, *((t_CKUINT *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // push mem stack content into reg stack
    push_( reg_sp, *((t_CKFLOAT *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKCOMPLEX *& reg_sp = (t_CKCOMPLEX *&)shred->reg->sp;

    // push mem stack content into reg stack
    push_( reg_sp, *((t_CKCOMPLEX *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKVEC3 *& reg_sp = (t_CKVEC3 *&)shred->reg->sp;
    
    // push mem stack content into reg stack
    push_( reg_sp, *((t_CKVEC3 *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKVEC4 *& reg_sp = (t_CKVEC4 *&)shred->reg->sp;
    
    // push mem stack content into reg stack
    push_( reg_sp, *((t_CKVEC4 *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: push value from external maps to register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_External::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    
    // get external map content
    switch( m_type ) {
        case te_externalInt:
        {
            // int pointer to registers
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->get_external_int_value( m_name );
            
            // push external map content into int-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_externalFloat:
        {
            // float pointer to registers
            t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;
            t_CKFLOAT val = (t_CKFLOAT) vm->get_external_float_value( m_name );
            
            // push external map content into float-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_externalEvent:
        {
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->get_external_event( m_name );
            
            // push external map content into event-reg stack
            push_( reg_sp, val );
        }
            break;
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem_Addr::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)(base?shred->base_ref->stack:shred->mem->sp);
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // push mem stack addr into reg stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_External_Addr::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // find addr
    t_CKUINT addr;
    switch( m_type ) {
        case te_externalInt:
            addr = (t_CKUINT) vm->get_ptr_to_external_int( m_name );
            break;
        case te_externalFloat:
            addr = (t_CKUINT) vm->get_ptr_to_external_float( m_name );
            break;
        case te_externalEvent:
            addr = (t_CKUINT) vm->get_ptr_to_external_event( m_name );
            break;
            
    }

    // push mem stack addr into reg stack
    push_( reg_sp, addr );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Mem::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *((t_CKUINT *)(mem_sp + *(reg_sp+1) )) = *reg_sp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack 
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // pop word from reg stack 
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX *& reg_sp = (t_CKCOMPLEX *&)shred->reg->sp;
    
    // pop word from reg stack 
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& reg_sp = (t_CKBYTE *&)shred->reg->sp;

    // pop word from reg stack (changed 1.3.1.0 to use sz_WORD)
    pop_( reg_sp, m_val * sz_WORD );
}




#pragma mark === Memory Operations ===



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Set_Imm::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT * mem_sp = (t_CKUINT *)(shred->mem->sp + m_offset);
    
    // set
    *(mem_sp) = m_val;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Set_Imm2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT * mem_sp = (t_CKFLOAT *)(shred->mem->sp + m_offset);
    
    // set
    *(mem_sp) = m_val;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Push_Imm::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    
    // pop word from reg stack 
    push_( mem_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Push_Imm2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& mem_sp = (t_CKFLOAT *&)shred->mem->sp;
    
    // pop word from reg stack 
    push_( mem_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Pop_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    
    // pop word from reg stack 
    pop_( mem_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Pop_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& mem_sp = (t_CKFLOAT *&)shred->mem->sp;
    
    // pop word from reg stack 
    pop_( mem_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Pop_Word3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    
    // pop word from reg stack 
    pop_( mem_sp, m_val );
}



#pragma mark === Branching ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) < val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Gt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) > val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Le_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) <= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Ge_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) >= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Eq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Neq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) != val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Not_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    (*(sp-1)) = !(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    (*(sp-1)) = -(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    (*(sp-1)) = -(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) < val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Gt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) > val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Le_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) <= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Ge_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) >= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Eq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Neq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) != val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Eq_int_IO_good::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    Chuck_IO **& ppIO = (Chuck_IO **&)shred->reg->sp;
    t_CKINT result = 0;
    pop_( sp, 2 );

    if( (*ppIO) != NULL )
    {
        // TODO: verify this logic
        result = (*ppIO)->good() && !(*ppIO)->eof();
    }

    if( result == val_(sp+1) || !(*ppIO) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Neq_int_IO_good::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    Chuck_IO **& ppIO = (Chuck_IO **&)shred->reg->sp;
    t_CKINT result = 0;
    pop_( sp, 2 );

    if( (*ppIO) != NULL )
    {
        // fixed 1.3.0.0 -- removed the t_CKINT
        // TODO: verify this logic?
        result = (*ppIO)->good() && !(*ppIO)->eof();
    }

    if( result != val_(sp+1) || !(ppIO) )
        shred->next_pc = m_jmp;
}



#pragma mark === Bitwise Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_And::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) & val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Or::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) | val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Xor::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) ^ val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Right::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) >> val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Right_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp+1) >> val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Left::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) << val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Left_Reverse::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp+1) << val_(sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_And_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) &= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Or_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) |= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Xor_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) ^= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Right_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) >>= val_(sp);
    push_( sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Left_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT temp, *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    temp = **(t_CKINT **)(sp+1) <<= val_(sp);
    push_( sp, temp );
}



#pragma mark === Comparison ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) >= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) != val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) >= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, val_(sp) != val_(sp+1) );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->re == (sp+1)->re) && (sp->im == (sp+1)->im) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->re != (sp+1)->re) || (sp->im != (sp+1)->im) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x == (sp+1)->x) && (sp->y == (sp+1)->y)
           && (sp->z == (sp+1)->z) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x != (sp+1)->x) || (sp->y != (sp+1)->y)
           || (sp->z != (sp+1)->z) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x == (sp+1)->x) && (sp->y == (sp+1)->y)
           && (sp->z == (sp+1)->z) && (sp->w == (sp+1)->w) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x != (sp+1)->x) || (sp->y != (sp+1)->y)
           || (sp->z != (sp+1)->z) || (sp->w != (sp+1)->w) );
}




#pragma mark === Boolean Arithmetic ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_And::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) && val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Or::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) || val_(sp+1) );
}



#pragma mark === Miscellany ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Goto::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Nop::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // no op
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_EOC::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // end the shred
    shred->is_done = TRUE;
    shred->is_running = FALSE;
}



#pragma mark === Allocation ===




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // zero out the memory stack
    *( (t_CKUINT *)(mem_sp + m_val) ) = 0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // zero out the memory stack
    *( (t_CKFLOAT *)(mem_sp + m_val) ) = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // zero out the memory stack
    ( (t_CKCOMPLEX *)(mem_sp + m_val) )->re = 0.0;
    ( (t_CKCOMPLEX *)(mem_sp + m_val) )->im = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // zero out the memory stack
    ( (t_CKVEC3 *)(mem_sp + m_val) )->x = 0.0;
    ( (t_CKVEC3 *)(mem_sp + m_val) )->y = 0.0;
    ( (t_CKVEC3 *)(mem_sp + m_val) )->z = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // zero out the memory stack
    ( (t_CKVEC4 *)(mem_sp + m_val) )->x = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->y = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->z = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->w = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the memory stack
    *( (t_CKUINT *)(obj->data + m_val) ) = 0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the memory stack
    *( (t_CKFLOAT *)(obj->data + m_val) ) = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the memory stack
    ( (t_CKCOMPLEX *)(obj->data + m_val) )->re = 0.0;
    ( (t_CKCOMPLEX *)(obj->data + m_val) )->im = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the memory stack
    ( (t_CKVEC3 *)(obj->data + m_val) )->x = 0.0;
    ( (t_CKVEC3 *)(obj->data + m_val) )->y = 0.0;
    ( (t_CKVEC3 *)(obj->data + m_val) )->z = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the memory stack
    ( (t_CKVEC4 *)(obj->data + m_val) )->x = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->y = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->z = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->w = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc external
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word_External::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKUINT addr = 0;

    // init in the correct vm map according to the type
    switch( m_type ) {
        case te_externalInt:
            vm->init_external_int( m_name );
            addr = (t_CKUINT) vm->get_ptr_to_external_int( m_name );
            break;
        case te_externalFloat:
            vm->init_external_float( m_name );
            addr = (t_CKUINT) vm->get_ptr_to_external_float( m_name );
            break;
        case te_externalEvent:
            // no need to init, it has already been initted during emit
            addr = (t_CKUINT) vm->get_ptr_to_external_event( m_name );
            break;
    }
    
    // push addr onto operand stack
    push_( reg_sp, addr );
}




#pragma mark === Object Initialization/Construction ===


static Chuck_Instr_Func_Call g_func_call;
static Chuck_Instr_Func_Call_Member g_func_call_member( 0 );
//-----------------------------------------------------------------------------
// name: call_pre_constructor()
// desc: ...
//-----------------------------------------------------------------------------
inline void call_pre_constructor( Chuck_VM * vm, Chuck_VM_Shred * shred, 
                                  Chuck_VM_Code * pre_ctor, t_CKUINT stack_offset )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // sanity
    assert( pre_ctor != NULL );

    // first duplicate the top of the stack, which should be object pointer
    push_( reg_sp, *(reg_sp-1) );
    // push the pre constructor
    push_( reg_sp, (t_CKUINT)pre_ctor );
    // push the stack offset
    push_( reg_sp, stack_offset );

    // call the function
    if( pre_ctor->native_func != 0 )
        g_func_call_member.execute( vm, shred );
    else
        g_func_call.execute( vm, shred );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: object pre construct
//-----------------------------------------------------------------------------
void Chuck_Instr_Pre_Constructor::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    call_pre_constructor( vm, shred, pre_ctor, stack_offset );
}




//-----------------------------------------------------------------------------
// name: instantiate_object()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL initialize_object( Chuck_Object * object, Chuck_Type * type )
{
    // sanity
    assert( type != NULL );
    assert( type->info != NULL );

    // allocate virtual table
    object->vtable = new Chuck_VTable;
    if( !object->vtable ) goto out_of_memory;
    // copy the object's virtual table
    object->vtable->funcs = type->info->obj_v_table.funcs;
    // set the type reference
    // TODO: reference count
    object->type_ref = type;
    object->type_ref->add_ref();
    // get the size
    object->size = type->obj_size;
    // allocate memory
    if( object->size )
    {
        // check to ensure enough memory
        object->data = new t_CKBYTE[object->size];
        if( !object->data ) goto out_of_memory;
        // zero it out
        memset( object->data, 0, object->size );
    }
    else object->data = NULL;

    // special
    if( type->ugen_info )
    {
        // ugen
        Chuck_UGen * ugen = (Chuck_UGen *)object;
        if( type->ugen_info->tick ) ugen->tick = type->ugen_info->tick;
        // added 1.3.0.0 -- tickf for multi-channel tick
        if( type->ugen_info->tickf ) ugen->tickf = type->ugen_info->tickf;
        if( type->ugen_info->pmsg ) ugen->pmsg = type->ugen_info->pmsg;
        // TODO: another hack!
        if( type->ugen_info->tock ) ((Chuck_UAna *)ugen)->tock = type->ugen_info->tock;
        // allocate multi chan
        ugen->alloc_multi_chan( type->ugen_info->num_ins, 
                                type->ugen_info->num_outs );
        // allocate the channels
        for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
        {
            // allocate ugen for each | REFACTOR-2017: added ugen->vm
            Chuck_Object * obj = instantiate_and_initialize_object(
                ugen->vm->env()->t_ugen, ugen->shred, ugen->vm );
            // cast to ugen
            ugen->m_multi_chan[i] = (Chuck_UGen *)obj;
            // additional reference count
            ugen->m_multi_chan[i]->add_ref();
            // owner
            ugen->m_multi_chan[i]->owner = ugen;
            // ref count
            // spencer 2013-5-20: don't add extra ref, to avoid a ref cycle
            //ugen->add_ref();
        }
        // TODO: alloc channels for uana
    }

    return TRUE;

out_of_memory:

    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while instantiating object '%s'\n",
        type->c_name() );

    // delete
    if( object ) SAFE_DELETE( object->vtable );

    // return FALSE
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: instantiate_and_initialize_object()
// desc: call this one if you have a non-null shred
//-----------------------------------------------------------------------------
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM_Shred * shred )
{
    assert( shred != NULL );
    return instantiate_and_initialize_object( type, shred, shred->vm_ref );
}




//-----------------------------------------------------------------------------
// name: instantiate_and_initialize_object()
// desc: call this one if you don't have a shred
//-----------------------------------------------------------------------------
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM * vm )
{
    assert( vm != NULL );
    return instantiate_and_initialize_object( type, NULL, vm );
}




//-----------------------------------------------------------------------------
// name: instantiate_and_initialize_object()
// desc: you probably shouldn't call this version. call the one that takes a
//       shred if you have a non-null shred, otherwise call the one that
//       takes a vm
//-----------------------------------------------------------------------------
Chuck_Object * instantiate_and_initialize_object( Chuck_Type * type, Chuck_VM_Shred * shred, Chuck_VM * vm )
{
    Chuck_Object * object = NULL;
    Chuck_UAna * uana = NULL;

    // sanity
    assert( type != NULL );
    assert( type->info != NULL );

    // allocate the VM object
    if( !type->ugen_info )
    {
        // check type TODO: make this faster
        if( type->allocator )
            object = type->allocator( shred, Chuck_DL_Api::Api::instance() );
        else if( isa( type, vm->env()->t_fileio ) ) object = new Chuck_IO_File( vm );
        else if( isa( type, vm->env()->t_event ) ) object = new Chuck_Event;
        else if( isa( type, vm->env()->t_string ) ) object = new Chuck_String;
        // TODO: is this ok?
        else if( isa( type, vm->env()->t_shred ) )
        {
            object = new Chuck_VM_Shred;
            ((Chuck_VM_Shred * )object)->vm_ref = vm; // REFACTOR-2017
        }
        // TODO: is this ok?
        else object = new Chuck_Object;
    }
    else
    {
        // make ugen
        Chuck_UGen * ugen = NULL;
        // ugen vs. uana
        if( type->ugen_info->tock != NULL )
        {
            // uana
            object = ugen = uana = new Chuck_UAna;
            ugen->alloc_v( vm->shreduler()->m_max_block_size );
        }
        else 
        {
            object = ugen = new Chuck_UGen;
            ugen->alloc_v( vm->shreduler()->m_max_block_size );
        }

        if( shred )
        {
            ugen->shred = shred;
            shred->add( ugen );
        }
        
        // REFACTOR-2017: added
        ugen->vm = vm;
    }
    
    // check to see enough memory
    if( !object ) goto out_of_memory;

    // initialize
    if( !initialize_object( object, type ) ) goto error;

    return object;

out_of_memory:

    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while instantiating object '%s'\n",
        type->c_name() );

error:

    // delete
    SAFE_DELETE( object );

    // return NULL
    return NULL;
}




//-----------------------------------------------------------------------------
// name: instantiate_object()
// desc: ...
//-----------------------------------------------------------------------------
inline void instantiate_object( Chuck_VM * vm, Chuck_VM_Shred * shred,
                                Chuck_Type * type )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // allocate the VM object
    Chuck_Object * object = instantiate_and_initialize_object( type, shred );
    if( !object ) goto error;

    // push the pointer on the operand stack
    push_( reg_sp, (t_CKUINT)object );

    // call preconstructor
    // call_pre_constructor( vm, shred, object, type, stack_offset );

    return;

error:

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: instantiate object
//-----------------------------------------------------------------------------
void Chuck_Instr_Instantiate_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    instantiate_object( vm, shred, this->type  );
}




//-----------------------------------------------------------------------------
// name: params()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Instantiate_Object::params() const
{
    static char buffer[256];
    sprintf( buffer, "%s", this->type->c_name() );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: params()
// desc: text description
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Pre_Ctor_Array_Top::params() const
{
    static char buffer[256];
    sprintf( buffer, "val=%ld, type=\"%s\"", m_val, type ? type->c_name() : "[empty]" );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: object pre construct top
//-----------------------------------------------------------------------------
void Chuck_Instr_Pre_Ctor_Array_Top::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // see if we are done with all elements in the array
    if( *(reg_sp-2) >= *(reg_sp-1) )
        shred->next_pc = m_val;
    else
    {
        // instantiate
        instantiate_object( vm, shred, type );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: object pre construct bottom
//-----------------------------------------------------------------------------
void Chuck_Instr_Pre_Ctor_Array_Bottom::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop the object
    pop_( reg_sp, 1 );
    
    // cast the object
    Chuck_Object * obj = (Chuck_Object *)(*(reg_sp));

    // assign object
    t_CKUINT * array = (t_CKUINT *)(*(reg_sp-3));
    // get the object pointer
    Chuck_Object ** dest = (Chuck_Object **)(array[*(reg_sp-2)]);
    // copy
    *dest = obj;
    // ref count
    obj->add_ref();
    // increment the index
    (*(reg_sp-2))++; //= (*(reg_sp-2)) + 1;

    // goto top
    shred->next_pc = m_val;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: object pre construct post
//-----------------------------------------------------------------------------
void Chuck_Instr_Pre_Ctor_Array_Post::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop the array, index, and size
    pop_( reg_sp, 3 );

    // clean up the array
    t_CKUINT * arr = (t_CKUINT *)*reg_sp;
    SAFE_DELETE_ARRAY( arr );
}



#pragma mark === Assignment ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: assign primitive (word)
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_Primitive::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *((t_CKUINT *)(*(reg_sp+1))) = *reg_sp;

    push_( reg_sp, *reg_sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign primitive (2 word)
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_Primitive2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack // ISSUE: 64-bit (fixed 1.3.1.0)
    pop_( reg_sp, 1 + (sz_FLOAT / sz_UINT) );
    // copy popped value into mem stack // ISSUE: 64-bit (fixed 1.3.1.0)
    *( (t_CKFLOAT *)(*(reg_sp+(sz_FLOAT/sz_UINT))) ) = *(t_CKFLOAT *)reg_sp;

    t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
    push_( sp_double, *sp_double );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign primitive (4 word)
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_Primitive4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 + (sz_COMPLEX / sz_UINT) ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // copy popped value into mem stack 
    *( (t_CKCOMPLEX*)(*(reg_sp+(sz_COMPLEX/sz_UINT))) ) = *(t_CKCOMPLEX *)reg_sp; // ISSUE: 64-bit (fixed 1.3.1.0)

    t_CKCOMPLEX *& sp_complex = (t_CKCOMPLEX *&)reg_sp;
    push_( sp_complex, *sp_complex );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign primitive (vec3), 1.3.5.3
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_PrimitiveVec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // pop word from reg stack
    pop_( reg_sp, 1 + (sz_VEC3 / sz_UINT) );
    // copy popped value into mem stack
    *( (t_CKVEC3*)(*(reg_sp+(sz_VEC3/sz_UINT))) ) = *(t_CKVEC3 *)reg_sp;
    
    t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)reg_sp;
    push_( sp_vec3, *sp_vec3 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign primitive (vec4)
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_PrimitiveVec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // pop word from reg stack
    pop_( reg_sp, 1 + (sz_VEC4 / sz_UINT) );
    // copy popped value into mem stack
    *( (t_CKVEC4*)(*(reg_sp+(sz_VEC4/sz_UINT))) ) = *(t_CKVEC4 *)reg_sp;
    
    t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)reg_sp;
    push_( sp_vec4, *sp_vec4 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign object with reference counting and releasing previous reference
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_VM_Object ** obj = NULL, * done = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // the previous reference
    obj = (Chuck_VM_Object **)(*(reg_sp+1));
    // save the reference (release should come after, in case same object)
    done = *obj;
    // copy popped value into memory
    *obj = (Chuck_VM_Object *)(*(reg_sp));
    // add reference
    if( *obj ) (*obj)->add_ref();
    // release
    if( done ) done->release();

//    CK_FPRINTF_STDERR( "obj: 0x%08x\n", *obj );

    // copy
    // memcpy( (void *)*(reg_sp+1), *obj, sizeof(t_CKUINT) );
    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)*obj );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: assign string
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_String::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    Chuck_String ** rhs_ptr = NULL;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // the previous reference
    rhs_ptr = (Chuck_String **)(*(reg_sp+1));
    // copy popped value into memory
    lhs = (Chuck_String *)(*(reg_sp));
    // release any previous reference
    if( *rhs_ptr )
    {
        if( lhs ) (*rhs_ptr)->set( lhs->str() );
        else
        {
            // release reference
            (*rhs_ptr)->release();
            (*rhs_ptr) = NULL;
        }
    }
    else
    {
        // if left is not null, yes
        if( lhs != NULL )
        {
            (*rhs_ptr) = (Chuck_String *)instantiate_and_initialize_object( vm->env()->t_string, shred );
            // add ref
            (*rhs_ptr)->add_ref();
            (*rhs_ptr)->set( lhs->str() );
        }
        //EM_error2( 0, "internal error: somehow the type checker has allowed NULL strings" );
        //EM_error2( 0, "we are sorry for the inconvenience but..." );
        //EM_error2( 0, "we have to crash now.  Thanks." );
        //assert( FALSE );
    }

    // copy
    // memcpy( (void *)*(reg_sp+1), *obj, sizeof(t_CKUINT) );
    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)*rhs_ptr );
}


#pragma mark === Reference Counting ===



//-----------------------------------------------------------------------------
// name: execute()
// desc: add one reference on object (added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Instr_AddRef_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // ISSUE: 64-bit?
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_VM_Object * obj = NULL;
    
    // pop word from reg stack
    pop_( reg_sp, 1 );
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp + *(reg_sp)) );
    // ge (2012 april): check for NULL (added 1.3.0.0)
    if( obj != NULL )
    {
        // release
        obj->add_ref();
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add one reference on object (ge 2012 april | added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Instr_AddRef_Object2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    Chuck_VM_Object * obj = NULL;
    
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp + m_val) );
    // check for NULL
    if( obj != NULL )
    {
        // add reference
        obj->add_ref();
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: add one reference on object (added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_AddRef_Object3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // ISSUE: 64-bit?
    // NOTE: this pointer is NOT a reference pointer
    t_CKUINT * reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_VM_Object * obj = NULL;
    
    // move word without popping
    reg_sp--;
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(reg_sp) );
    // ge (2012 april): check for NULL (added 1.3.0.0)
    if( obj != NULL )
    {
        // release
        obj->add_ref();
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: release one reference on object
//-----------------------------------------------------------------------------
void Chuck_Instr_Release_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // ISSUE: 64-bit?
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_VM_Object * obj = NULL;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp + *(reg_sp)) );
    // ge (2012 april): check for NULL (added 1.3.0.0)
    if( obj != NULL )
    {
        // release
        obj->release();
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: release one reference on object (added ge 2012 april | added 1.3.0.0)
//-----------------------------------------------------------------------------
void Chuck_Instr_Release_Object2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    Chuck_VM_Object * obj = NULL;
    
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp + m_val) );
    // check for NULL
    if( obj != NULL )
    {
        // release
        obj->release();
    }
}



#pragma mark === Function Calls ===



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_To_Code::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // ISSUE: 64-bit?
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get func
    Chuck_Func * func = (Chuck_Func *)*(reg_sp-1);
    // make sure
    assert( func != NULL );
    // code
    *(reg_sp-1) = (t_CKUINT)func->code;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*reg_sp;
    // get the local stack depth - caller local variables
    t_CKUINT local_depth = *(reg_sp+1);
    // convert to number of int's (was: 4-byte words), extra partial word counts as additional word
    local_depth = ( local_depth / sz_INT ) + ( local_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // get the stack depth of the callee function args
    t_CKUINT stack_depth = ( func->stack_depth / sz_INT ) + ( func->stack_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // get the previous stack depth - caller function args
    t_CKUINT prev_stack = ( *(mem_sp-1) / sz_INT ) + ( *(mem_sp-1) & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // jump the sp
    mem_sp += prev_stack + local_depth;
    // push the prev stack
    push_( mem_sp, prev_stack + local_depth );
    // push the current function
    push_( mem_sp, (t_CKUINT)shred->code );
    // push the pc
    push_( mem_sp, (t_CKUINT)(shred->pc + 1) );
    // push the stack depth
    push_( mem_sp, stack_depth );
    // set the pc to 0
    shred->next_pc = 0;
    // set the code
    shred->code = func;
    // set the instruction to the function instruction
    shred->instr = func->instr;

    // if there are arguments to be passed
    if( stack_depth )
    {
        // pop the arguments, by number of words
        pop_( reg_sp, stack_depth );

        // make copies
        t_CKUINT * mem_sp2 = (t_CKUINT *)mem_sp;
        t_CKUINT * reg_sp2 = (t_CKUINT *)reg_sp;

        // need this
        if( func->need_this )
        {
            // copy this from end of arguments to the front
            *mem_sp2++ = *(reg_sp2 + stack_depth - 1);
            // one less word to copy
            stack_depth--;
        }
        
        // push the arguments
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // detect overflow/underflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    return;

error_overflow:
    
    handle_overflow( shred, vm );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: imported member function call with return
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call_Member::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_DL_Return retval;

    // pop word
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*reg_sp;
    // get the function to be called
    // MOVED TO BELOW: f_mfun f = (f_mfun)func->native_func;
    // get the local stack depth - caller local variables
    t_CKUINT local_depth = *(reg_sp+1);
    // convert to number of int's (was: 4-byte words), extra partial word counts as additional word
    local_depth = ( local_depth / sz_INT ) + ( local_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // get the stack depth of the callee function args
    t_CKUINT stack_depth = ( func->stack_depth / sz_INT ) + ( func->stack_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // UNUSED: get the previous stack depth - caller function args
    // UNUSED: t_CKUINT prev_stack = ( *(mem_sp-1) >> 2 ) + ( *(mem_sp-1) & 0x3 ? 1 : 0 );
    // the amount to push in 4-byte words
    t_CKUINT push = local_depth;
    // push the mem stack passed the current function variables and arguments
    mem_sp += push;

    // pass args
    if( stack_depth )
    {
        // pop the arguments for pass to callee function
        reg_sp -= stack_depth;

        // make copies
        t_CKUINT * reg_sp2 = reg_sp;
        t_CKUINT * mem_sp2 = mem_sp;
        
        // need this
        if( func->need_this )
        {
            // copy this from end of arguments to the front
            *mem_sp2++ = *(reg_sp2 + stack_depth - 1);
            // one less word to copy
            stack_depth--;
        }
        // copy to args
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // detect overflow/underflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // check the type
    if( func->native_func_type == Chuck_VM_Code::NATIVE_CTOR )
    {
        // cast to right type
        f_ctor f = (f_ctor)func->native_func;
        // call (added 1.3.0.0 -- Chuck_DL_Api::Api::instance())
        f( (Chuck_Object *)(*mem_sp), mem_sp + 1, shred, Chuck_DL_Api::Api::instance() );
    }
    else
    {
        // cast to right type
        f_mfun f = (f_mfun)func->native_func;
        // call the function (added 1.3.0.0 -- Chuck_DL_Api::Api::instance())
        f( (Chuck_Object *)(*mem_sp), mem_sp + 1, &retval, shred, Chuck_DL_Api::Api::instance() );
    }
    // pop (TODO: check if this is right)
    mem_sp -= push;
    
    // push the return
    // 1.3.1.0: check type to use kind instead of size
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed: 1.3.1.0)
    {
        // push the return args
        push_( reg_sp, retval.v_uint );
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
        push_( sp_double, retval.v_float );
    }
    else if( m_val == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKCOMPLEX *& sp_complex = (t_CKCOMPLEX *&)reg_sp;
        // TODO: polar same?
        push_( sp_complex, retval.v_complex );
    }
    else if( m_val == kindof_VEC3 ) // 1.3.5.3
    {
        // push the return args
        t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)reg_sp;
        push_( sp_vec3, retval.v_vec3 );
    }
    else if( m_val == kindof_VEC4 ) // 1.3.5.3
    {
        // push the return args
        t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)reg_sp;
        push_( sp_vec4, retval.v_vec4 );
    }
    else if( m_val == kindof_VOID ) { }
    else assert( FALSE );

    return;

error_overflow:
    
    handle_overflow( shred, vm );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: imported static function call with return
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call_Static::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_DL_Return retval;

    // pop word
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*reg_sp;
    // get the function to be called
    f_sfun f = (f_sfun)func->native_func;
    // get the local stack depth - caller local variables
    t_CKUINT local_depth = *(reg_sp+1);
    // convert to number of int's (was: 4-byte words), extra partial word counts as additional word
    local_depth = ( local_depth / sz_INT ) + ( local_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // get the stack depth of the callee function args
    t_CKUINT stack_depth = ( func->stack_depth / sz_INT ) + ( func->stack_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // UNUSED: get the previous stack depth - caller function args
    // UNUSED: t_CKUINT prev_stack = ( *(mem_sp-1) >> 2 ) + ( *(mem_sp-1) & 0x3 ? 1 : 0 );    
    // the amount to push in 4-byte words
    t_CKUINT push = local_depth;
    // push the mem stack passed the current function variables and arguments
    mem_sp += push;

    // pass args
    if( stack_depth )
    {
        // pop the arguments for pass to callee function
        reg_sp -= stack_depth;

        // make copies
        t_CKUINT * reg_sp2 = reg_sp;
        t_CKUINT * mem_sp2 = mem_sp;
        
        // need this
        if( func->need_this )
        {
            // copy this from end of arguments to the front
            *mem_sp2++ = *(reg_sp2 + stack_depth - 1);
            // advance reg pointer
            reg_sp2++;
            // one less word to copy
            stack_depth--;
        }
        // copy to args
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // detect overflow/underflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // call the function (added 1.3.0.0 -- Chuck_DL_Api::Api::instance())
    f( mem_sp, &retval, shred, Chuck_DL_Api::Api::instance() );
    mem_sp -= push;

    // push the return
    // 1.3.1.0: check type to use kind instead of size
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        push_( reg_sp, retval.v_uint );
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
        push_( sp_double, retval.v_float );
    }
    else if( m_val == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKCOMPLEX *& sp_complex = (t_CKCOMPLEX *&)reg_sp;
        // TODO: polar same?
        push_( sp_complex, retval.v_complex );
    }
    else if( m_val == kindof_VEC3 ) // 1.3.5.3
    {
        // push the return args
        t_CKVEC3 *& sp_vec3 = (t_CKVEC3 *&)reg_sp;
        push_( sp_vec3, retval.v_vec3 );
    }
    else if( m_val == kindof_VEC4 ) // 1.3.5.3
    {
        // push the return args
        t_CKVEC4 *& sp_vec4 = (t_CKVEC4 *&)reg_sp;
        push_( sp_vec4, retval.v_vec4 );
    }
    else if( m_val == kindof_VOID ) { }
    else assert( FALSE );

    return;

error_overflow:
    
    handle_overflow( shred, vm );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Return::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    // UNUSED: t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop pc
    pop_( mem_sp, 2 );
    shred->next_pc = *mem_sp;
    // pop the code
    pop_( mem_sp, 1 );
    Chuck_VM_Code * func = (Chuck_VM_Code *)*mem_sp;
    // pop the prev_stack
    pop_( mem_sp, 1 );
    // jump the prev stack
    mem_sp -= *(mem_sp);

    // set the shred 
    shred->code = func;
    shred->instr = func->instr;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Spork::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKUINT this_ptr = 0;
    
    // pop the stack
    pop_( reg_sp, 1 );
    // get the code
    Chuck_VM_Code * code = *(Chuck_VM_Code **)reg_sp;
    // spork it
    Chuck_VM_Shred * sh = vm->spork( code, shred, TRUE );
    // pop the stack
    pop_( reg_sp, 1 );
    // get the func
    Chuck_Func * func = (Chuck_Func *)(*reg_sp);
    // need this?
    if( func->is_member )
    {
        // pop the stack
        pop_( reg_sp, 1 );
        // get this
        this_ptr = *reg_sp;
        // add to shred so it's ref counted, and released when shred done (1.3.1.2)
        sh->add_parent_ref( (Chuck_Object *)this_ptr );
    }
    // copy args
    if( m_val )
    {
        // ISSUE: 64-bit? (1.3.1.0: this should be OK as long as shred->reg->sp is t_CKBYTE *)
        pop_( shred->reg->sp, m_val );
        memcpy( sh->reg->sp, shred->reg->sp, m_val );
        sh->reg->sp += m_val;
    }
    // copy this, if need
    if( func->is_member )
    {
        push_( (t_CKUINT*&)sh->reg->sp, this_ptr );
    }
    // copy func
    push_( (t_CKUINT*&)sh->reg->sp, (t_CKUINT)func );
    // push the stack
    push_( reg_sp, (t_CKUINT)sh );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
//void Chuck_Instr_Spork_Stmt::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
//{
//    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
//    
//    // pop the stack
//    pop_( reg_sp, 1 );
//    // get the code
//    Chuck_VM_Code * code = *(Chuck_VM_Code **)reg_sp;
//    // spork it
//    Chuck_VM_Shred * sh = vm->spork( code, shred );
//    
//    if( code->need_this )
//    {
//        // pop the stack
//        pop_( reg_sp, 1 );
//        // copy this from local stack to top of new shred mem
//        *( ( t_CKUINT * ) sh->mem->sp ) = *reg_sp;
//    }
//        
//    // push the stack
//    push_( reg_sp, (t_CKUINT)sh );
//}



#pragma mark === Time Advance ===

//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Time_Advance::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKTIME *& sp = (t_CKTIME *&)shred->reg->sp;

    // pop word from reg stack
    pop_( sp, 1 );

    if( *sp < shred->now )
    {
        // we have a problem
        CK_FPRINTF_STDERR( 
            "[chuck](VM): DestTimeNegativeException: '%.6f' on line[%lu] in shred[id=%lu:%s]\n",
            *sp, m_linepos, shred->xid, shred->name.c_str() );
        // do something!
        shred->is_running = FALSE;
        shred->is_done = TRUE;

        return;
    }

    // shredule the shred
    vm->shreduler()->shredule( shred, *sp );
    // suspend
    shred->is_running = FALSE;

    // track time advance
    CK_TRACK( Chuck_Stats::instance()->advance_time( shred, *sp ) );

    push_( sp, *sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Event_Wait::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( sp, 1 );

    Chuck_Event * event = (Chuck_Event *)(*sp);

    // check for null
    if( !event ) goto null_pointer;

    // wait
    event->wait( shred, vm );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (null Event wait) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Arrays ===


//-----------------------------------------------------------------------------
// name: Chuck_Instr_Array_Init()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Init::Chuck_Instr_Array_Init( Chuck_Env * env, Chuck_Type * t, t_CKINT length )
{
    // set
    m_length = length;
    // copy
    m_type_ref = t;
    // TODO: do this? remember?
    // m_type_ref->add_ref();
    // type
    m_param_str = new char[64];
    // obj | REFACTOR-2017: added env
    m_is_obj = isobj( env, m_type_ref );
    const char * str = m_type_ref->c_name();
    t_CKUINT len = strlen( str );
    // copy
    if( len < 48 )
        strcpy( m_param_str, str );
    else
    {
        strncpy( m_param_str, str, 48 );
        strcpy( m_param_str + 48, "..." );
    }

    // append length
    char buffer[16];
    sprintf( buffer, "[%ld]", m_length );
    strcat( m_param_str, buffer );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Instr_Array_Init()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Init::~Chuck_Instr_Array_Init()
{
    // delete
    delete [] m_param_str;
    m_param_str = NULL;
    // release
    //m_type_ref->release();
    m_type_ref = NULL;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Init::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // allocate the array
    if( m_type_ref->size == sz_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // TODO: look at size and treat Chuck_Array4 as ChuckArrayInt
        // pop the values
        pop_( reg_sp, m_length );
        // instantiate array
        Chuck_Array4 * array = new Chuck_Array4( m_is_obj, m_length );
        // problem
        if( !array ) goto out_of_memory;
        // initialize object
        initialize_object( array, vm->env()->t_array );
        // set array type
        array->m_array_type = m_type_ref;
        m_type_ref->add_ref();
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(reg_sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else if( m_type_ref->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // pop the values
        pop_( reg_sp, m_length * (sz_FLOAT / sz_INT) ); // 1.3.1.0 added size division
        // instantiate array
        Chuck_Array8 * array = new Chuck_Array8( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKFLOAT * sp = (t_CKFLOAT *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->t_array );
        // set array type
        array->m_array_type = m_type_ref;
        m_type_ref->add_ref();
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else if( m_type_ref->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // pop the values
        pop_( reg_sp, m_length * (sz_COMPLEX / sz_INT) ); // 1.3.1.0 added size division
        // instantiate array
        Chuck_Array16 * array = new Chuck_Array16( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKCOMPLEX * sp = (t_CKCOMPLEX *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->t_array );
        // set array type
        array->m_array_type = m_type_ref;
        m_type_ref->add_ref();
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else if( m_type_ref->size == sz_VEC3 ) // 1.3.5.3
    {
        // pop the values
        pop_( reg_sp, m_length * (sz_VEC3 / sz_INT) );
        // instantiate array
        Chuck_Array24 * array = new Chuck_Array24( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKVEC3 * sp = (t_CKVEC3 *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->t_array );
        // set array type
        array->m_array_type = m_type_ref;
        m_type_ref->add_ref();
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else if( m_type_ref->size == sz_VEC4 ) // 1.3.5.3
    {
        // pop the values
        pop_( reg_sp, m_length * (sz_VEC4 / sz_INT) );
        // instantiate array
        Chuck_Array32 * array = new Chuck_Array32( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKVEC4 * sp = (t_CKVEC4 *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->t_array );
        // set array type
        array->m_array_type = m_type_ref;
        m_type_ref->add_ref();
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else assert( FALSE );

    return;

out_of_memory:

    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while initializing arrays on line[%lu]\n", m_linepos );

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_Instr_Array_Alloc()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Alloc::Chuck_Instr_Array_Alloc( Chuck_Env * env, t_CKUINT depth, Chuck_Type * t,
                                                  t_CKUINT offset, t_CKBOOL is_ref )
{
    // set
    m_depth = depth;
    // copy
    m_type_ref = t;
    // remember
    // m_type_ref->add_ref();
    // type
    m_param_str = new char[64];
    // obj | REFACTOR-2017: added env
    m_is_obj = isobj( env, m_type_ref );
    // offset for pre constructor
    m_stack_offset = offset;
    // is object ref
    m_is_ref = is_ref;
    const char * str = m_type_ref->c_name();
    t_CKUINT len = strlen( str );
    // copy
    if( len < 64 )
        strcpy( m_param_str, str );
    else
    {
        strncpy( m_param_str, str, 60 );
        strcpy( m_param_str + 60, "..." );
    }
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Instr_Array_Alloc()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Alloc::~Chuck_Instr_Array_Alloc()
{
    // delete
    delete [] m_param_str;
    m_param_str = NULL;
    // release
    //m_type_ref->release();
    m_type_ref = NULL;
}




//-----------------------------------------------------------------------------
// name: do_alloc_array()
// desc: 1.3.1.0 -- changed size to kind
//-----------------------------------------------------------------------------
Chuck_Object * do_alloc_array( Chuck_VM * vm, // REFACTOR-2017: added
                               t_CKINT * capacity, const t_CKINT * top,
                               t_CKUINT kind, t_CKBOOL is_obj,
                               t_CKUINT * objs, t_CKINT & index )
{
    // not top level
    Chuck_Array4 * base = NULL;
    Chuck_Object * next = NULL;
    t_CKINT i = 0;

    // capacity
    if( *capacity < 0 ) goto negative_array_size;

    // see if top level
    if( capacity >= top )
    {
        // check size
        // 1.3.1.0: look at type to use kind instead of size
        if( kind == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            Chuck_Array4 * base = new Chuck_Array4( is_obj, *capacity );
            if( !base ) goto out_of_memory;

            // if object
            if( is_obj && objs )
            {
                // loop
                for( i = 0; i < *capacity; i++ )
                {
                    // add to array for later allocation
                    objs[index++] = base->addr( i );
                }
            }

            // initialize object
            initialize_object( base, vm->env()->t_array );
            return base;
        }
        else if( kind == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            Chuck_Array8 * base = new Chuck_Array8( *capacity );
            if( !base ) goto out_of_memory;
            
            // initialize object
            initialize_object( base, vm->env()->t_array );
            return base;
        }
        else if( kind == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            Chuck_Array16 * base = new Chuck_Array16( *capacity );
            if( !base ) goto out_of_memory;
            
            // initialize object
            initialize_object( base, vm->env()->t_array );
            return base;
        }
        else if( kind == kindof_VEC3 ) // 1.3.5.3
        {
            Chuck_Array24 * base = new Chuck_Array24( *capacity );
            if( !base ) goto out_of_memory;
            
            // initialize object
            initialize_object( base, vm->env()->t_array );
            return base;
        }
        else if( kind == kindof_VEC4 ) // 1.3.5.3
        {
            Chuck_Array32 * base = new Chuck_Array32( *capacity );
            if( !base ) goto out_of_memory;
            
            // initialize object
            initialize_object( base, vm->env()->t_array );
            return base;
        }

        // shouldn't get here
        assert( FALSE );
    }

    // not top level
    base = new Chuck_Array4( TRUE, *capacity );
    if( !base ) goto out_of_memory;

    // allocate the next level
    for( i = 0; i < *capacity; i++ )
    {
        // the next | REFACTOR-2017: added vm
        next = do_alloc_array( vm, capacity+1, top, kind, is_obj, objs, index );
        // error if NULL
        if( !next ) goto error;
        // set that, with ref count
        base->set( i, (t_CKUINT)next );
    }

    // initialize object
    initialize_object( base, vm->env()->t_array );
    return base;

out_of_memory:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while allocating arrays...\n" );
    goto error;

negative_array_size:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): NegativeArraySize: while allocating arrays...\n" );
    goto error;

error:
    // base shouldn't have been ref counted
    SAFE_DELETE( base );
    return NULL;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Alloc::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    // ref
    t_CKUINT ref = 0;
    // the total number of objects to allocate
    t_CKUINT num_obj = 0;
    // the index to pass to the arrays
    t_CKINT index = 0;
    // number
    t_CKFLOAT num = 1.0;
    // array
    t_CKUINT * obj_array = NULL;
    // size
    t_CKUINT obj_array_size = 0;

    // if need instantiation
    if( m_is_obj && !m_is_ref )
    {
        t_CKINT * curr = (t_CKINT *)(reg_sp - m_depth);
        t_CKINT * top = (t_CKINT *)(reg_sp - 1);

        num_obj = 1;
        num = 1.0;
        // product of all dims
        while( curr <= top )
        {
            num_obj *= *(curr);

            // overflow
            num *= (t_CKFLOAT)(*(curr));
            if( num > (t_CKFLOAT)INT_MAX ) goto overflow;

            curr++;
        }

        // allocate array to hold elements, this array
        // is pushed on the reg stack, filled, and cleaned
        // during the array_post stage
        // ----
        // TODO: this scheme results in potential leak
        //       if intermediate memory allocations fail
        //       and the array instantiation is aborted
        if( num_obj > 0 )
        {
            obj_array = new t_CKUINT[num_obj];
            if( !obj_array ) goto out_of_memory;
            obj_array_size = num_obj;
        }

        // check to see if we need to allocate
        // if( num_obj > shred->obj_array_size )
        // {
        //     SAFE_DELETE( shred->obj_array );
        //     shred->obj_array_size = 0;
        //     shred->obj_array = new t_CKUINT[num_obj];
        //     if( !shred->obj_array ) goto out_of_memory;
        //     shred->obj_array_size = num_obj;
        // }
    }

    // recursively allocate | REFACTOR-2017: added env
    ref = (t_CKUINT)do_alloc_array( vm,
        (t_CKINT *)(reg_sp - m_depth),
        (t_CKINT *)(reg_sp - 1),
        getkindof(vm->env(), m_type_ref), // 1.3.1.0: changed; was 'm_type_ref->size'
        m_is_obj,
        obj_array, index 
    );

    // pop the indices - this protects the contents of the stack
    // do_alloc_array writes stuff to the stack
    pop_( reg_sp, m_depth );

    // make sure
    assert( index == (t_CKINT)num_obj );

    // problem
    if( !ref ) goto error;

    // push array
    push_( reg_sp, ref );

    // if need to instantiate
    if( m_is_obj && !m_is_ref )
    {
        // push objects to instantiate
        push_( reg_sp, (t_CKUINT)obj_array );
        // push index to use
        push_( reg_sp, 0 );
        // push size
        push_( reg_sp, (t_CKUINT)num_obj );
    }

    return;

overflow:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): OverFlow: requested array size too big...\n" );
    goto error;

out_of_memory:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): OutOfMemory: while allocating arrays...\n" );
    goto error;

error:
    CK_FPRINTF_STDERR( 
        "[chuck](VM):     (note: on line[%lu] in shred[id=%lu:%s])\n",
        m_linepos, shred->xid, shred->name.c_str() );

    // done
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Access::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // UNUSED: t_CKUINT *& reg_sp = sp;
    t_CKINT i = 0;
    t_CKUINT val = 0;
    t_CKFLOAT fval = 0;
    t_CKCOMPLEX cval;
    cval.re = 0;
    cval.im = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // pop
    pop_( sp, 2 );

    // check pointer
    if( !(*sp) ) goto null_pointer;

    // 4 or 8 or 16
    // 1.3.1.0: look at type to use kind instead of size
    if( m_kind == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array4 * arr = (Chuck_Array4 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &val ) )
                goto array_out_of_bound;
            // push the value
            push_( sp, val );
        }
    }
    else if( m_kind == kindof_FLOAT ) // ISSUE: 64-bit (1.3.1.0)
    {
        // get array
        Chuck_Array8 * arr = (Chuck_Array8 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &fval ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKFLOAT *&)sp), fval );
        }
    }
    else if( m_kind == kindof_COMPLEX ) // ISSUE: 64-bit
    {
        // get array
        Chuck_Array16 * arr = (Chuck_Array16 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &cval ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKCOMPLEX *&)sp), cval );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_Array24 * arr = (Chuck_Array24 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &v3 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC3 *&)sp), v3 );
        }
    }
    else if( m_kind == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_Array32 * arr = (Chuck_Array32 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &v4 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC4 *&)sp), v4 );
        }
    }
    else
        assert( FALSE );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (array access) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc removed
    goto done;

array_out_of_bound:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): ArrayOutofBounds: on line[%lu] in shred[id=%lu:%s], index=[%ld]\n",
        m_linepos, shred->xid, shred->name.c_str(), i ); // shred->pc removed
    // go to done
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Map_Access::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * key = NULL;
    t_CKUINT val = 0;
    t_CKFLOAT fval = 0;
    t_CKCOMPLEX cval;
    cval.re = 0;
    cval.im = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // pop
    pop_( sp, 2 );

    // check pointer
    if( !(*sp) ) goto null_pointer;

    // 4 or 8 or 16
    // 1.3.1.0: look at type to use kind instead of size
    if( m_kind == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array4 * arr = (Chuck_Array4 *)(*sp);
        // get index
        key = (Chuck_String *)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( key->str() );
            // exception
            if( !val ) goto error;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( key->str(), &val ) )
                goto error;
            // push the value
            push_( sp, val );
        }
    }
    else if( m_kind == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array8 * arr = (Chuck_Array8 *)(*sp);
        // get index
        key = (Chuck_String *)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( key->str() );
            // exception
            if( !val ) goto error;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( key->str(), &fval ) )
                goto error;
            // push the value
            push_( ((t_CKFLOAT *&)sp), fval );
        }
    }
    else if( m_kind == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array16 * arr = (Chuck_Array16 *)(*sp);
        // get index
        key = (Chuck_String *)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( key->str() );
            // exception
            if( !val ) goto error;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( key->str(), &cval ) )
                goto error;
            // push the value
            push_( ((t_CKCOMPLEX *&)sp), cval );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_Array24 * arr = (Chuck_Array24 *)(*sp);
        // get index
        key = (Chuck_String *)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( key->str() );
            // exception
            if( !val ) goto error;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( key->str(), &v3 ) )
                goto error;
            // push the value
            push_( ((t_CKVEC3 *&)sp), v3 );
        }
    }
    else if( m_kind == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_Array32 * arr = (Chuck_Array32 *)(*sp);
        // get index
        key = (Chuck_String *)(*(sp+1));
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( key->str() );
            // exception
            if( !val ) goto error;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( key->str(), &v4 ) )
                goto error;
            // push the value
            push_( ((t_CKVEC4 *&)sp), v4 );
        }
    }
    else
        assert( FALSE );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (map access) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc
    goto done;

error:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): InternalArrayMapError: on line[%lu] in shred[id=%lu:%s], index=[%s]\n",
        m_linepos, shred->xid, shred->name.c_str(), key->str().c_str() ); // shred->pc
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Access_Multi::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    t_CKINT i = 0;
    t_CKUINT val = 0, j;
    t_CKFLOAT fval = 0;
    t_CKCOMPLEX cval;
    t_CKINT * ptr = NULL;
    t_CKUINT index = 0; cval.re = cval.im = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // pop all indices then array
    pop_( sp, m_depth + 1 );

    // get array
    Chuck_Array4 * base = (Chuck_Array4 *)(*sp);
    ptr = (t_CKINT *)(sp+1);

    // check for null
    if( !base ) goto null_pointer;

    // make sure
    assert( m_depth > 1 );
    // loop through indices
    for( j = 0; j < (m_depth-1); j++ )
    {
        // get index
        i = *ptr++;
        // check if index is string (added 1.3.1.0 -- thanks Robin Haberkorn!)
        if( j < m_indexIsAssociative.size() && m_indexIsAssociative[j] )
        {
            // get index
            Chuck_String * key = (Chuck_String *)(i);
            // get the array
            if( !base->get( key->str(), &val ) )
                goto array_out_of_bound;
        }
        else
        {
            // get the array
            if( !base->get( i, &val ) )
                goto array_out_of_bound;            
        }

        // set the array
        base = (Chuck_Array4 *)val;
        // check for null
        if( !base )
        {
            // error
            index = j + 1;
            goto null_pointer;
        }
    }

    // 4 or 8 or 16
    // 1.3.1.0: look at type and use kind instead of size
    if( m_kind == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get arry
        Chuck_Array4 * arr = base;
        // get index
        i = (t_CKINT)(*ptr);
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &val ) )
                goto array_out_of_bound;
            // push the value
            push_( sp, val );
        }
    }
    else if( m_kind == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array8 * arr = (Chuck_Array8 *)(base);
        // get index
        i = (t_CKINT)(*ptr);
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &fval ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKFLOAT *&)sp), fval );
        }
    }
    else if( m_kind == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array16 * arr = (Chuck_Array16 *)(base);
        // get index
        i = (t_CKINT)(*ptr);
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &cval ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKCOMPLEX *&)sp), cval );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_Array24 * arr = (Chuck_Array24 *)(base);
        // get index
        i = (t_CKINT)(*ptr);
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &v3 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC3 *&)sp), v3 );
        }
    }
    else if( m_kind == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_Array32 * arr = (Chuck_Array32 *)(base);
        // get index
        i = (t_CKINT)(*ptr);
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( i );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( i, &v4 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC4 *&)sp), v4 );
        }
    }
    else
        assert( FALSE );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (array access) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc
    CK_FPRINTF_STDERR( 
        "[chuck](VM): (array dimension where exception occurred: %lu)\n", index );
    goto done;

array_out_of_bound:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): ArrayOutofBounds: on line[%lu] in shred[id=%lu:%s], index=[%ld]\n",
        m_linepos, shred->xid, shred->name.c_str(), i ); // shred->pc
    // go to done
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Prepend::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Append::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // t_CKINT i = 0;
    t_CKUINT val = 0;
    t_CKFLOAT fval = 0;
    t_CKCOMPLEX cval;
    cval.re = 0;
    cval.im = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // how much to pop (added 1.3.1.0)
    t_CKUINT howmuch = 0;
    // check kind
    if( m_val == kindof_INT ) howmuch = 1;
    else if( m_val == kindof_FLOAT ) howmuch = sz_FLOAT / sz_INT;
    else if( m_val == kindof_COMPLEX ) howmuch = sz_COMPLEX / sz_INT;
    else if( m_val == kindof_VEC3 ) howmuch = sz_VEC3 / sz_INT;
    else if( m_val == kindof_VEC4 ) howmuch = sz_VEC4 / sz_INT;
    // pop (1.3.1.0: use howmuch instead of m_val/4)
    pop_( sp, 1 + howmuch ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // check pointer
    if( !(*sp) ) goto null_pointer;

    // 4 or 8 or 16
    // 1.3.1.0: changed to look at type (instead of size)
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array4 * arr = (Chuck_Array4 *)(*sp);
        // get value
        val = (t_CKINT)(*(sp+1));
        // append
        arr->push_back( val );
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array8 * arr = (Chuck_Array8 *)(*sp);
        // get value
        fval = (*(t_CKFLOAT *)(sp+1));
        // append
        arr->push_back( fval );
    }
    else if( m_val == kindof_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_Array16 * arr = (Chuck_Array16 *)(*sp);
        // get value
        cval = (*(t_CKCOMPLEX *)(sp+1));
        // append
        arr->push_back( cval );
    }
    else if( m_val == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_Array24 * arr = (Chuck_Array24 *)(*sp);
        // get value
        v3 = (*(t_CKVEC3 *)(sp+1));
        // append
        arr->push_back( v3 );
    }
    else if( m_val == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_Array32 * arr = (Chuck_Array32 *)(*sp);
        // get value
        v4 = (*(t_CKVEC4 *)(sp+1));
        // append
        arr->push_back( v4 );
    }
    else
        assert( FALSE );

    // push array back on stack
    push_( sp, (*sp) );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (array append) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Dot Access ===

//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Member_Data::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // the pointer
    t_CKUINT data;
    
    // pop the object pointer
    pop_( sp, 1 );
    // get the object pointer
    Chuck_Object * obj = (Chuck_Object *)(*sp);
    // check
    if( !obj ) goto error;
    // calculate the data pointer
    data = (t_CKUINT)(obj->data + m_offset);
    
    // emit addr or value
    if( m_emit_addr )
    {
        // push the address
        push_( sp, data );
    }
    else
    {
        // 4 or 8 or 16
        // 1.3.1.0: check type to use kind instead of size
        if( m_kind == kindof_INT ) { push_( sp, *((t_CKUINT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_FLOAT ) { push_float( sp, *((t_CKFLOAT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_COMPLEX ) { push_complex( sp, *((t_CKCOMPLEX *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0) // TODO: polar same?
        else if( m_kind == kindof_VEC3 ) { push_vec3( sp, *((t_CKVEC3 *)data) ); } // 1.3.5.3
        else if( m_kind == kindof_VEC4 ) { push_vec4( sp, *((t_CKVEC4 *)data) ); } // 1.3.5.3
        else assert( FALSE );
    }

    return;

error:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Member_Func::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // the pointer
    t_CKUINT data;
    
    // pop the object pointer
    pop_( sp, 1 );
    // get the object pointer
    Chuck_Object * obj = (Chuck_Object *)(*sp);
    // check
    if( !obj ) goto error;
    // make sure we are in range
    assert( m_offset < obj->vtable->funcs.size() );
    // calculate the data pointer
    data = (t_CKUINT)(obj->vtable->funcs[m_offset]);
    
    // push the address
    push_( sp, data );

    return;

error:
    // we have a problem
    CK_FPRINTF_STDERR( 
             "[chuck](VM): NullPointerException: on line[%lu] in shred[id=%lu:%s]\n",
            m_linepos, shred->xid, shred->name.c_str() );

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: primitive func, 1.3.5.3
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Primitive_Func::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    
    // pop the primitive pointer
    pop_( sp, 1 );
    
    // push the function address address
    push_( sp, m_native_func );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Static_Data::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // the pointer
    t_CKUINT data;
    
    // pop the type pointer
    pop_( sp, 1 );
    // get the object pointer
    Chuck_Type * t_class = (Chuck_Type *)(*sp);
    // make sure
    assert( (m_offset + m_size) <= t_class->info->class_data_size );
    // calculate the data pointer
    data = (t_CKUINT)(t_class->info->class_data + m_offset);
    
    // emit addr or value
    if( m_emit_addr )
    {
        // push the address
        push_( sp, data );
    }
    else
    {
        // 4 or 8 or 16
        // 1.3.1.0: check type to use kind instead of size
        if( m_kind == kindof_INT ) { push_( sp, *((t_CKUINT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_FLOAT ) { push_float( sp, *((t_CKFLOAT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_COMPLEX ) { push_complex( sp, *((t_CKCOMPLEX *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0) // TODO: polar same?
        else if( m_kind == kindof_VEC3 ) { push_vec3( sp, *((t_CKVEC3 *)data) ); } // 1.3.5.3
        else if( m_kind == kindof_VEC4 ) { push_vec4( sp, *((t_CKVEC4 *)data) ); } // 1.3.5.3
        else assert( FALSE );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Static_Import_Data::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    
    // emit addr or value
    if( m_emit_addr )
    {
        // push the address
        push_( sp, (t_CKUINT)m_addr );
    }
    else
    {
        // 4 or 8 or 16
        // 1.3.1.0: check type to use kind instead of size
        if( m_kind == kindof_INT ) { push_( sp, *((t_CKUINT *)m_addr) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_FLOAT ) { push_float( sp, *((t_CKFLOAT *)m_addr) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_COMPLEX ) { push_complex( sp, *((t_CKCOMPLEX *)m_addr) ); } // ISSUE: 64-bit (fixed 1.3.1.0) // TODO: polar same?
        else if( m_kind == kindof_VEC3 ) { push_vec3( sp, *((t_CKVEC3 *)m_addr) ); } // 1.3.5.3
        else if( m_kind == kindof_VEC4 ) { push_vec4( sp, *((t_CKVEC4 *)m_addr) ); } // 1.3.5.3
        else assert( FALSE );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Static_Func::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // register stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    
    // pop the type pointer
    pop_( sp, 1 );
    
    // push the address
    push_( sp, (t_CKUINT)(m_func) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_First::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // stack
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr on
        if( m_emit_addr ) {
            // t_CKFLOAT a = (*(t_CKCOMPLEX **)sp)->re;
            push_( sp, (t_CKUINT)(&((*(t_CKCOMPLEX **)sp)->re)) );
        } else {
            push_float( sp, (*(t_CKCOMPLEX **)sp)->re );
        }
    }
    else
    {
        // stack
        t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr, um we can't
        if( m_emit_addr ) {
            assert( FALSE );
        } else {
            push_float( sp, sp->re );
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Second::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // stack
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr on
        if( m_emit_addr ) {
            push_( sp, (t_CKUINT)(&((*(t_CKCOMPLEX **)sp)->im)) );
        } else {
            push_float( sp, (*(t_CKCOMPLEX **)sp)->im );
        }
    }
    else
    {
        // stack
        t_CKCOMPLEX *& sp = (t_CKCOMPLEX *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr, um we can't
        if( m_emit_addr ) {
            assert( FALSE );
        } else {
            push_float( sp, sp->im );
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Third::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // stack
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr on
        if( m_emit_addr ) {
            push_( sp, (t_CKUINT)(&((*(t_CKVEC3 **)sp)->z)) );
        } else {
            push_float( sp, (*(t_CKVEC3 **)sp)->z );
        }
    }
    else
    {
        // stack
        t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr, um we can't
        if( m_emit_addr ) {
            assert( FALSE );
        } else {
            push_float( sp, sp->z );
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Fourth::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // stack
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr on
        if( m_emit_addr ) {
            push_( sp, (t_CKUINT)(&((*(t_CKVEC4 **)sp)->w)) );
        } else {
            push_float( sp, (*(t_CKVEC4 **)sp)->w );
        }
    }
    else
    {
        // stack
        t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push the addr, um we can't
        if( m_emit_addr ) {
            assert( FALSE );
        } else {
            push_float( sp, sp->w );
        }
    }
}




#pragma mark === Casting ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_double2int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKINT *& sp_int = (t_CKINT *&)sp;
    pop_( sp, 1 );
    push_( sp_int, (t_CKINT)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_int2double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    t_CKFLOAT *& sp_double = (t_CKFLOAT *&)sp;
    pop_( sp, 1 );
    push_( sp_double, (t_CKFLOAT)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_int2complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 1 );
    // push re and im
    push_( sp_float, (t_CKFLOAT)(*sp) );
    push_( sp_float, (t_CKFLOAT)0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_int2polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 1 );
    // push re and im
    push_( sp_float, (t_CKFLOAT)(*sp) );
    push_( sp_float, (t_CKFLOAT)0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_double2complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    // leave on stack and push 0
    push_( sp_float, (t_CKFLOAT)0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_double2polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    // leave on stack and push 0
    push_( sp_float, (t_CKFLOAT)0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_complex2polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKCOMPLEX * sp = (t_CKCOMPLEX *)shred->reg->sp;
    // find it
    sp--;
    t_CKPOLAR * sp_polar = (t_CKPOLAR *)sp;
    t_CKFLOAT modulus, phase;
    // leave on stack
    modulus = ::sqrt( sp->re*sp->re + sp->im*sp->im );
    phase = ::atan2( sp->im, sp->re );
    sp_polar->modulus = modulus;
    sp_polar->phase = phase;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_polar2complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKPOLAR * sp = (t_CKPOLAR *)shred->reg->sp;
    // find it
    sp--;
    t_CKCOMPLEX * sp_complex = (t_CKCOMPLEX *)sp;
    t_CKFLOAT re, im;
    // leave on stack
    re = sp->modulus * ::cos( sp->phase );
    im = sp->modulus * ::sin( sp->phase );
    sp_complex->re = re;
    sp_complex->im = im;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec3tovec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
    // zero pad it
    push_( sp, 0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec4tovec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
    // remove w component
    pop_( sp, 1 );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_object2string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // pop it
    pop_( sp, 1 );
    // object
    Chuck_Object * obj = (Chuck_Object *)(*sp);
    // return
    Chuck_DL_Return RETURN;
    // get toString from it (added 1.3.0.0 -- Chuck_DL_Api::Api::instance())
    object_toString( obj, NULL, &RETURN, NULL, Chuck_DL_Api::Api::instance() );
    Chuck_String * str = RETURN.v_string;
    // set it
    push_( sp, (t_CKUINT)str );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Op_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_String * lhs = NULL;
    Chuck_String * rhs = NULL;

    // pop
    pop_( sp, 2 );
    // get the string references
    lhs = (Chuck_String *)*sp;
    rhs = (Chuck_String *)*(sp + 1);
    // neither should be null
    if( !lhs || !rhs ) goto null_pointer;

    // look
    switch( m_val )
    {
    case ae_op_eq:
        push_( sp, lhs->str() == rhs->str() );
    break;

    case ae_op_neq:
        push_( sp, lhs->str() != rhs->str() );
    break;

    case ae_op_lt:
        push_( sp, lhs->str() < rhs->str() );
    break;

    case ae_op_le:
        push_( sp, lhs->str() <= rhs->str() );
    break;

    case ae_op_gt:
        push_( sp, lhs->str() > rhs->str() );
    break;

    case ae_op_ge:
        push_( sp, lhs->str() >= rhs->str() );
    break;

    default:
        goto invalid_op;
    break;
    }

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (string op) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

invalid_op:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): InvalidStringOpException: '%lu' on line[%lu] in shred[id=%lu:%s]\n",
        m_val, m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Builtins ===


//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_ADC::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    push_( reg_sp, (t_CKUINT)vm->m_adc );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_DAC::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    push_( reg_sp, (t_CKUINT)vm->m_dac );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Bunghole::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    push_( reg_sp, (t_CKUINT)vm->m_bunghole );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chout::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    push_( reg_sp, (t_CKUINT) vm->chout() );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cherr::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    push_( reg_sp, (t_CKUINT) vm->cherr() );
}


#pragma mark === UGens ===

//-----------------------------------------------------------------------------
// name: Chuck_Instr_UGen_Link()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_UGen_Link::Chuck_Instr_UGen_Link( t_CKBOOL isUpChuck )
{
    m_isUpChuck = isUpChuck;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Link::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_UGen **& sp = (Chuck_UGen **&)shred->reg->sp;
    
    // pop
    pop_( sp, 2 );
    // check for null
    if( !*(sp+1) || !(*sp) ) goto null_pointer;
    // go for it
    (*(sp + 1))->add( *sp, m_isUpChuck );
    // push the second
    push_( sp, *(sp + 1) );
    
    return;
    
null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): NullPointerException: (UGen link) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Array_Link::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_Object **& sp = (Chuck_Object **&)shred->reg->sp;
    Chuck_Object *src_obj, *dst_obj;
    t_CKINT num_in;
    
    // pop
    pop_( sp, 2 );
    // check for null
    if( !*(sp+1) || !(*sp) ) goto null_pointer;
    
    src_obj = *sp;
    dst_obj = (*(sp + 1));
    
    // go for it
    num_in = ugen_generic_num_in(dst_obj, m_dstIsArray);
    for( int i = 0; i < num_in; i++ )
    {
        Chuck_UGen *dst_ugen = ugen_generic_get_dst( dst_obj, i, m_dstIsArray );
        Chuck_UGen *src_ugen = ugen_generic_get_src( src_obj, i, m_srcIsArray );
        if( dst_ugen == NULL || src_ugen == NULL )
            goto null_pointer;
        dst_ugen->add( src_ugen, FALSE);
    }
    
    // push the second
    push_( sp, *(sp + 1) );
    
    return;
    
null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): NullPointerException: (UGen link) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_UnLink::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_UGen **& sp = (Chuck_UGen **&)shred->reg->sp;
    
    pop_( sp, 2 );
    (*(sp+1))->remove( *sp );
    push_( sp, *(sp + 1) );
}



/*
//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Ctrl::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    
    pop_( sp, 4 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp+1);
    f_ctrl ctrl = (f_ctrl)*(sp+2);
    f_cget cget = (f_cget)*(sp+3);
    // set now
    ugen->now = shred->now;
    // call ctrl
    ctrl( ugen, (void *)sp );
    if( cget ) cget( ugen, (void *)sp );
    // push the new value
    push_( sp, *sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    f_cget cget = (f_cget)*(sp+1);
    // set now
    ugen->now = shred->now;
    // call cget
    cget( ugen, (void *)sp );
    // push the new value
    push_( sp, *sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Ctrl2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    
    pop_( sp, 4 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp+1);
    f_ctrl ctrl = (f_ctrl)*(sp+2);
    f_cget cget = (f_cget)*(sp+3);
    // set now
    ugen->now = shred->now;
    // call ctrl
    pop_( sp, 1 );
    ctrl( ugen, (void *)sp );
    if( cget ) cget( ugen, (void *)sp );
    // push the new value
    ((t_CKFLOAT *&)shred->reg->sp)++;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    f_cget cget = (f_cget)*(sp+1);
    // set now
    ugen->now = shred->now;
    // call cget
    cget( ugen, (void *)sp );
    // push the new value
    ((t_CKFLOAT *&)shred->reg->sp)++;
}
*/



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_PMsg::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_UGen **& sp = (Chuck_UGen **&)shred->reg->sp;
    
    pop_( sp, 2 );
    
    // (*(sp + 1))->pmsg( shred->now, *sp );
    
    push_( sp, *(sp + 1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Init_Loop_Counter::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // pop the value
    pop_( sp, 1 );
    
    // allocate counter
    t_CKUINT * p = shred->pushLoopCounter();
    
    // copy it
    *p = (t_CKUINT)(*sp >= 0 ? *sp : -*sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: decrement top loop counter for shred
//-----------------------------------------------------------------------------
void Chuck_Instr_Dec_Loop_Counter::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // get topmost
    t_CKUINT * p = shred->currentLoopCounter();
    
    // check
    if( p == NULL ) goto error;
    
    // decrement
    (*p)--;
    
    // done
    return;
    
error:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): LoopCounterError: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;
    
done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: get top loop counter for shred
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Loop_Counter_Deref::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    
    // get topmost
    t_CKUINT * p = shred->currentLoopCounter();
    
    // check
    if( p == NULL ) goto error;
    
    // push
    push_( reg_sp, *p );
    
    // done
    return;
    
error:
    // we have a problem
    CK_FPRINTF_STDERR(
        "[chuck](VM): LoopCounterError: on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;
    
done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Pop_Loop_Counter::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // pop counter
    shred->popLoopCounter();
}




#pragma mark === IO ===




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_in_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // pop the value
    pop_( sp, 2 );
    
    // ISSUE: 64-bit?
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;
    
    // read into the variable
    **(t_CKINT **)(sp+1) = (*ppIO)->readInt( Chuck_IO::INT32 );
    
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
    
    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO input int) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_in_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // issue: 64-bit
    // pop the value (fixed 1.3.0.0 -- changed from 3 to 2, note it's a float POINTER)
    pop_( sp, 2 );
    
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;
    
    // check it
    if( *(ppIO) == NULL ) goto null_pointer;
    
    // read into the variable
    **(t_CKFLOAT **)(sp+1) = (*ppIO)->readFloat();
    
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
    
    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO input float) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_in_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // pop the value
    pop_( sp, 2 );
    
    // issue: 64-bit
    // the IO
    Chuck_IO ** ppIO = (Chuck_IO **)sp;
    // the string
    Chuck_String ** ppStr = (Chuck_String **)(sp+1);
    std::string s; // = (*ppStr)->str();
    
//    CK_FPRINTF_STDERR( "ppIO: 0x%08x\n", ppIO );
//    CK_FPRINTF_STDERR( "ppStr: 0x%08x\n", ppStr );
//    CK_FPRINTF_STDERR( "*ppStr: 0x%08x\n", *ppStr );
    
    // check it
    if( *(ppIO) == NULL || *(ppStr) == NULL ) goto null_pointer;
    
    // read into the variable
    (*ppIO)->readString( s );
    (*ppStr)->set( s );
    
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
    
    return;
    
null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO input string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
        
    // pop the value
    pop_( sp, 2 );
        
    // issue: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;
        
    // check it
    if( *(ppIO) == NULL ) goto null_pointer;
        
    // write the value
    (*ppIO)->write( *(sp+1) );
        
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
        
    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO output int) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;
        
done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // pop the value
    pop_( sp, 1 + (sz_FLOAT / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)
    
    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;
    
    // check it
    if( *(ppIO) == NULL ) goto null_pointer;
    
    // write the value
    (*ppIO)->write( *((t_CKFLOAT *)(sp+1)) );
    
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
    
    return;
    
null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO output float) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;
    
done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_string::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;
    
    // pop the value
    pop_( sp, 2 );
    
    // ISSUE: 64-bit
    // the IO
    Chuck_IO ** ppIO = (Chuck_IO **)sp;
    // the string
    Chuck_String ** ppStr = (Chuck_String **)(sp+1);
    
    // check it
    if( *(ppIO) == NULL || *(ppStr) == NULL ) goto null_pointer;
    
    // write the variable
    (*ppIO)->write( (*ppStr)->str().c_str() );
    
    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );
    
    return;
    
null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](VM): NullPointerException: (IO output string) on line[%lu] in shred[id=%lu:%s]\n",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;
    
done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




// hack
Chuck_Instr_Hack::Chuck_Instr_Hack( Chuck_Type * type )
{
    this->m_type_ref = type;
    // this->m_type_ref->add_ref();
}

Chuck_Instr_Hack::~Chuck_Instr_Hack()
{
//    this->m_type_ref->release();
}

void Chuck_Instr_Hack::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // look at the type (1.3.1.0: added iskindofint)
    if( m_type_ref->size == sz_INT && iskindofint(vm->env(), m_type_ref) ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        t_CKINT * sp = (t_CKINT *)shred->reg->sp;
        if( !isa( m_type_ref, vm->env()->t_string ) ||  *(sp-1) == 0 )
        {
            // print it
            if( *(sp-1) == 0 && isa( m_type_ref, vm->env()->t_object ) )
                CK_FPRINTF_STDERR( "null :(%s)\n", m_type_ref->c_name() );
            else
                CK_FPRINTF_STDERR( "%ld :(%s)\n", *(sp-1), m_type_ref->c_name() );
        }
        else
        {
            CK_FPRINTF_STDERR( "\"%s\" : (%s)\n", ((Chuck_String *)*(sp-1))->str().c_str(), m_type_ref->c_name() );
        }
    }
    else if( m_type_ref->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
        // print it
        CK_FPRINTF_STDERR( "%f :(%s)\n", *(sp-1), m_type_ref->c_name() );
    }
    else if( m_type_ref->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        if( m_type_ref->xid == te_complex )
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
            // print it
            CK_FPRINTF_STDERR( "#(%.4f,%.4f) :(%s)\n", *(sp-2), *(sp-1), m_type_ref->c_name() );
        }
        else if( m_type_ref->xid == te_polar )
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
            // print it
            CK_FPRINTF_STDERR( "%%(%.4f,%.4f*pi) :(%s)\n", *(sp-2), *(sp-1)/ONE_PI, m_type_ref->c_name() );
        }
        else
        {
            CK_FPRINTF_STDERR( "[chuck]: internal error printing 16-word primitive...\n" );
        }
    }
    else if( m_type_ref->size == sz_VEC3 ) // 1.3.5.3
    {
        if( m_type_ref->xid == te_vec3 )
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
            // print it
            CK_FPRINTF_STDERR( "@(%.4f,%.4f,%.4f) :(%s)\n", *(sp-3), *(sp-2), *(sp-1), m_type_ref->c_name() );
        }
        else
        {
            CK_FPRINTF_STDERR( "[chuck]: internal error printing 24-word primitive...\n" );
        }
    }
    else if( m_type_ref->size == sz_VEC4 ) // 1.3.5.3
    {
        if( m_type_ref->xid == te_vec4 )
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
            // print it
            CK_FPRINTF_STDERR( "@(%.4f,%.4f,%.4f,%.4f) :(%s)\n", *(sp-4), *(sp-3), *(sp-2), *(sp-1), m_type_ref->c_name() );
        }
        else
        {
            CK_FPRINTF_STDERR( "[chuck]: internal error printing 32-word primitive...\n" );
        }
    }
    else if( m_type_ref->size == 0 )
    {
        CK_FPRINTF_STDERR( "... :(%s)\n", m_type_ref->c_name() );
    }
    else
        assert( FALSE );

    // flush
    CK_FFLUSH_STDERR();
}

const char * Chuck_Instr_Hack::params() const
{
    static char buffer[256];
    sprintf( buffer, "(%s)", m_type_ref->c_name() );
    return buffer;
}




// gack
Chuck_Instr_Gack::Chuck_Instr_Gack( const std::vector<Chuck_Type *> & types )
{
    m_type_refs = types;

    // add refs
}

Chuck_Instr_Gack::~Chuck_Instr_Gack()
{
    // release refs
}

void Chuck_Instr_Gack::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE * the_sp = (t_CKBYTE *)shred->reg->sp;

    if( m_type_refs.size() == 1 )
    {
        Chuck_Instr_Hack hack( m_type_refs[0] );
        hack.execute( vm, shred );
        return;
    }

    // loop over types
    t_CKUINT i;

    // find the start of the expression
    for( i = 0; i < m_type_refs.size(); i++ )
    {
        Chuck_Type * type = m_type_refs[i];
        the_sp -= type->size;
    }

    // print
    for( i = 0; i < m_type_refs.size(); i++ )
    {
        Chuck_Type * type = m_type_refs[i];

        // look at the type (1.3.1.0: added is kindofint)
        if( type->size == sz_INT && iskindofint(vm->env(), type) ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            t_CKINT * sp = (t_CKINT *)the_sp;
            if( !isa( type, vm->env()->t_string ) ||  *(sp) == 0 )
            {
                if( isa( type, vm->env()->t_object ) )
                {
                    // print it
                    if( *(sp-1) == 0 )
                        CK_FPRINTF_STDERR( "null " );
                    else
                        CK_FPRINTF_STDERR( "0x%lx ", *(sp) );
                }
                else
                {
                    // print it
                    CK_FPRINTF_STDERR( "%ld ", *(sp) );
                }
            }
            else
            {
                Chuck_String * str = ((Chuck_String *)*(sp));
                CK_FPRINTF_STDERR( "%s ", str->str().c_str() );
            }

            the_sp += sz_INT; // ISSUE: 64-bit (fixed 1.3.1.0)
        }
        else if( type->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)the_sp;
            // print it
            CK_FPRINTF_STDERR( "%f ", *(sp) );

            the_sp += sz_FLOAT; // ISSUE: 64-bit (fixed 1.3.1.0)
        }
        else if( type->size == sz_COMPLEX ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)the_sp;
            if( type->xid == te_complex )
                // print it
                CK_FPRINTF_STDERR( "#(%.4f,%.4f) ", *(sp), *(sp+1) );
            else if( type->xid == te_polar )
                // print it
                CK_FPRINTF_STDERR( "%%(%.4f,%.4f*pi) ", *(sp), *(sp+1)/ONE_PI );

            the_sp += sz_COMPLEX; // ISSUE: 64-bit (fixed 1.3.1.0)
        }
        else if( type->size == sz_VEC3 ) // 1.3.5.3
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)the_sp;
            if( type->xid == te_vec3 )
                // print it
                CK_FPRINTF_STDERR( "#(%.4f,%.4f,%.4f) ", *(sp), *(sp+1), *(sp+2) );
            
            the_sp += sz_VEC3; // ISSUE: 64-bit (fixed 1.3.1.0)
        }
        else if( type->size == sz_VEC4 ) // 1.3.5.3
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)the_sp;
            if( type->xid == te_vec4 )
                // print it
                CK_FPRINTF_STDERR( "#(%.4f,%.4f,%.4f,%.4f) ", *(sp), *(sp+1), *(sp+2), *(sp+3) );
            
            the_sp += sz_VEC4; // ISSUE: 64-bit (fixed 1.3.1.0)
        }
        else if( type->size == 0 )
        {
            CK_FPRINTF_STDERR( "... " );
        }
        else
            assert( FALSE );
    }

    CK_FPRINTF_STDERR( "\n" );
    
    // flush
    CK_FFLUSH_STDERR();
}

const char * Chuck_Instr_Gack::params() const
{
    static char buffer[256];
    sprintf( buffer, "( many types )" );
    return buffer;
}


void throw_exception(Chuck_VM_Shred * shred, const char * name)
{
    // we have a problem
    CK_FPRINTF_STDERR(
            "[chuck](VM): %s: on line[%lu] in shred[id=%lu:%s]\n",
            name, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


void throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKINT desc)
{
    // we have a problem
    CK_FPRINTF_STDERR(
            "[chuck](VM): %s: '%li' on line[%lu] in shred[id=%lu:%s]\n",
            name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


void throw_exception(Chuck_VM_Shred * shred, const char * name, t_CKFLOAT desc)
{
    // we have a problem
    CK_FPRINTF_STDERR(
            "[chuck](VM): %s: '%f' on line[%lu] in shred[id=%lu:%s]\n",
            name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


void throw_exception(Chuck_VM_Shred * shred, const char * name, const char * desc)
{
    // we have a problem
    CK_FPRINTF_STDERR(
            "[chuck](VM): %s: %s on line[%lu] in shred[id=%lu:%s]\n",
            name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


