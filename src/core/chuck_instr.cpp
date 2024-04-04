/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
#include "chuck_instr.h"
#include "chuck_absyn.h" // for op2str | 1.5.1.5
#include "chuck_type.h"
#include "chuck_lang.h"
#include "chuck_vm.h"
#include "chuck_ugen.h"
#include "chuck_dl.h"
#include "chuck_io.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h" // added 1.4.1.0
#include "util_math.h"
#include "util_string.h"

#include <math.h>
#include <limits.h>
#include <typeinfo>
using namespace std;




//-----------------------------------------------------------------------------
// name: Chuck_Instr()
// desc: base class constructor
//-----------------------------------------------------------------------------
Chuck_Instr::Chuck_Instr()
{
    // set linepos to 0 so we can tell later whether it has been set properly
    m_linepos = 0;
    // default codestr
    m_codestr_pre = NULL;
    m_codestr_post = NULL;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Instr()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Instr::~Chuck_Instr()
{
    CK_SAFE_DELETE( m_codestr_pre );
    CK_SAFE_DELETE( m_codestr_post );
}




//-----------------------------------------------------------------------------
// name: name()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_Instr::name() const
{
#ifndef __CHUNREAL_ENGINE__
    return mini_type( typeid(*this).name() );
#else
    // 1.5.0.0 (ge) | #chureal
    // Unreal Engine seems to disable C++ run time type information
    return "[chunreal type info disabled]";
#endif // #ifndef __CHUNREAL_ENGINE__
}




//-----------------------------------------------------------------------------
// name: prepend_codestr()
// desc: prepend codestr associated with this instruction
//       only certain instructions (e.g., start of stmts) have this
//-----------------------------------------------------------------------------
void Chuck_Instr::prepend_codestr( const string & str )
{
    // alloc if needed
    if( !m_codestr_pre ) m_codestr_pre = new vector<string>();
    // prepend to pre
    m_codestr_pre->insert( m_codestr_pre->begin(), str );
}




//-----------------------------------------------------------------------------
// name: append_codestr()
// desc: append codestr associated with this instruction
//       only certain instructions (e.g., start of stmts) have this
//-----------------------------------------------------------------------------
void Chuck_Instr::append_codestr( const string & str )
{
    // alloc if needed
    if( !m_codestr_post ) m_codestr_post = new vector<string>();
    // append to post
    m_codestr_post->push_back( str );
}




//-----------------------------------------------------------------------------
// name: set_linepos()
// desc: store line position for error messages
//-----------------------------------------------------------------------------
void Chuck_Instr::set_linepos(t_CKUINT linepos)
{
    m_linepos = linepos;
}




//-----------------------------------------------------------------------------
// name: ck_handle_overflow()
// desc: take evasive action upon detecting stack overflow
//-----------------------------------------------------------------------------
void ck_handle_overflow( Chuck_VM_Shred * shred, Chuck_VM * vm, const string & reason )
{
    // we have a problem
    EM_exception( "StackOverflow in shred[id=%lu:%s] [pc=%lu]",
                  shred->xid, shred->name.c_str(), shred->pc );
    if( reason != "" )
        EM_error3( "...(possible cause: %s)", reason.c_str() );

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
    EM_exception(
        "DivideByZero: on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "DivideByZero: on line[%lu] in shred[id=%lu:%s]",
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
// desc: add two vec2 (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC2 r;
    r.x = sp->x + (sp+1)->x;
    r.y = sp->y + (sp+1)->y;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: subtract two vec2 (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
    t_CKFLOAT *& sp_float = (t_CKFLOAT *&)sp;
    pop_( sp, 2 );
    // result
    t_CKVEC2 r;
    r.x = sp->x - (sp+1)->x;
    r.y = sp->y - (sp+1)->y;
    push_( sp_float, r.x );
    push_( sp_float, r.y );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: float * vec2 (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC2 );
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)sp);
    // the vector
    t_CKVEC2 v = *((t_CKVEC2 *)(sp+sz_FLOAT));
    // result
    t_CKVEC2 r;
    r.x = v.x * f;
    r.y = v.y * f;
    // pointer as vec2
    t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)sp;
    // push on reg stack
    push_( sp_vec2, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec2*float (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
void Chuck_Instr_vec2_Times_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC2 );
    // the vector
    t_CKVEC2 v = *((t_CKVEC2 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC2));
    // result
    t_CKVEC2 r;
    r.x = v.x * f;
    r.y = v.y * f;
    // pointer as vec2
    t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)sp;
    // push on reg stack
    push_( sp_vec2, r );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec2/float (ge) added 1.5.1.7 with ChuGL
//-----------------------------------------------------------------------------
void Chuck_Instr_vec2_Divide_float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    // pointer number of bytes
    pop_( sp, sz_FLOAT+sz_VEC2 );
    // the vector
    t_CKVEC2 v = *((t_CKVEC2 *)sp);
    // the float
    t_CKFLOAT f = *((t_CKFLOAT *)(sp+sz_VEC2));
    // result
    t_CKVEC2 r;
    r.x = v.x / f;
    r.y = v.y / f;
    // pointer as vec2
    t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)sp;
    // push on reg stack
    push_( sp_vec2, r );
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
// desc: subtract two vec3; ge: added 1.3.5.3 with other vec3 vec4 instructions
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
// desc: vector cross-product (ge) added 1.3.5.3 with other vec3 vec4 instructions
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
// desc: multiply two vec3; ge: added 1.3.5.3 with other vec3 vec4 instructions
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
// desc: scaling a vec3; ge: added 1.3.5.3 with other vec3 vec4 instructions
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
// desc: divide by scalar; ge: added 1.3.5.3 with other vec3 vec4 instructions
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
    EM_exception(
        "DivideByZero: on line[%lu] in shred[id=%lu:%s]",
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
// desc: vec2 +=> vec2var
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_vec2_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC2 temp;
    // pop value + pointer
    pop_( sp, sz_VEC2 + sz_UINT );

    // assign
    temp.x = (*(t_CKVEC2 **)(sp+sz_VEC2))->x += ((t_CKVEC2 *&)sp)->x;
    temp.y = (*(t_CKVEC2 **)(sp+sz_VEC2))->y += ((t_CKVEC2 *&)sp)->y;
    // push result
    push_( (t_CKVEC2 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec2 -=> vec2var
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_vec2_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC2 temp;
    // pop value + pointer
    pop_( sp, sz_VEC2 + sz_UINT );

    // assign
    temp.x = (*(t_CKVEC2 **)(sp+sz_VEC2))->x -= ((t_CKVEC2 *&)sp)->x;
    temp.y = (*(t_CKVEC2 **)(sp+sz_VEC2))->y -= ((t_CKVEC2 *&)sp)->y;
    // push result
    push_( (t_CKVEC2 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: float *=> vec2var
//-----------------------------------------------------------------------------
void Chuck_Instr_float_Times_vec2_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC2 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );

    // assign
    temp.x = (*(t_CKVEC2 **)(sp+sz_FLOAT))->x *= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC2 **)(sp+sz_FLOAT))->y *= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC2 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: float /=> vec2var
//-----------------------------------------------------------------------------
void Chuck_Instr_vec2_Divide_float_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& sp = (t_CKBYTE *&)shred->reg->sp;
    t_CKVEC2 temp;
    // pop float + pointer
    pop_( sp, sz_FLOAT + sz_UINT );

    // assign
    temp.x = (*(t_CKVEC2 **)(sp+sz_FLOAT))->x /= *((t_CKFLOAT *)sp);
    temp.y = (*(t_CKVEC2 **)(sp+sz_FLOAT))->y /= *((t_CKFLOAT *)sp);
    // push result
    push_( (t_CKVEC2 *&)sp, temp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec3 +=> vec3var
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
// desc: vec3 -=> vec3var
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
// desc: float *=> vec3var
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
// desc: float /=> vec3var
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
// desc: float +=> vec4var
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
// desc: floa -=> vec4var
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
// desc: float *=> vec4var
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
// desc: float /=> vec4var
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
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );

    // concat
    // result->str = lhs->str + rhs->str;
    result->set( lhs->str() + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (string + string) on line[%lu] in shred[id=%lu:%s]",
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
    if( !lhs || !(*rhs_ptr) ) goto null_pointer;

    // concat
    // (*rhs_ptr)->str += lhs->str;
    (*rhs_ptr)->set( (*rhs_ptr)->str() + lhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (string +=> string) on line[%lu] in shred[id=%lu:%s]",
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
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );

    // concat
    // result->str = lhs->str + ::ck_itoa(rhs);
    result->set( lhs->str() + ::ck_itoa( rhs ) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (string + int) on line[%lu] in shred[id=%lu:%s]",
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
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );

    // concat
    // result->str = lhs->str + ::ck_ftoa(rhs, 4);
    result->set( lhs->str() + ::ck_ftoa( rhs, 4 ) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (string + float) on line[%lu] in shred[id=%lu:%s]",
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
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );

    // concat
    // result->str = ::ck_itoa(lhs) + rhs->str;
    result->set( ::ck_itoa(lhs) + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (int + string) on line[%lu] in shred[id=%lu:%s]",
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
    result = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );

    // concat
    // result->str = ::ck_ftoa(lhs, 4) + rhs->str;
    result->set( ::ck_ftoa( lhs, 4 ) + rhs->str() );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(result) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (float + string) on line[%lu] in shred[id=%lu:%s]",
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
    // (*rhs_ptr)->str += ::ck_itoa(lhs);
    (*rhs_ptr)->set( (*rhs_ptr)->str() + ::ck_itoa(lhs) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (int +=> string) on line[%lu] in shred[id=%lu:%s]",
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
    // (*rhs_ptr)->str += ::ck_ftoa(lhs, 4);
    (*rhs_ptr)->set( (*rhs_ptr)->str() + ::ck_ftoa(lhs, 4) );

    // push the reference value to reg stack
    push_( reg_sp, (t_CKUINT)(*rhs_ptr) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (float +=> string) on line[%lu] in shred[id=%lu:%s]",
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
// desc: push Chuck_VM_Code * onto register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Code::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, (t_CKUINT)m_code );
}




//-----------------------------------------------------------------------------
// name: params()
// desc: params for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Reg_Push_Code::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "'%s'", m_code ? m_code->name.c_str() : "[null]" );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Zero::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& reg_sp = (t_CKBYTE *&)shred->reg->sp;

    // push val into reg stack
    memset( reg_sp, 0, m_val );
    // move stack pointer
    reg_sp += m_val;
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
    t_CKFLOAT num = ck_random_f();
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
// desc: push value from global maps to register stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Global::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // get global map content
    switch( m_type ) {
        case te_globalInt:
        {
            // int pointer to registers
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_int_value( m_name );

            // push global map content into int-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalFloat:
        {
            // float pointer to registers
            t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;
            t_CKFLOAT val = (t_CKFLOAT) vm->globals_manager()->get_global_float_value( m_name );

            // push global map content into float-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalString:
        {
            // pointer to registers
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_string( m_name );

            // push global map content into string-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalEvent:
        {
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_event( m_name );

            // push global map content into event-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalUGen:
        {
            if( !vm->globals_manager()->is_global_ugen_valid( m_name ) )
            {
                // we have a problem
                EM_exception(
                    "UninitializedUGen: on line[%lu] in shred[id=%lu:%s]",
                    m_linepos, shred->xid, shred->name.c_str());
                EM_exception(
                    "...(hint: need to declare global UGen earlier in file)" );
                goto error;
            }

            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_ugen( m_name );

            // push global map content into ugen-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalObject:
        {
            if( !vm->globals_manager()->is_global_object_valid( m_name ) )
            {
                // we have a problem
                EM_exception(
                    "UninitializedObject: on line[%lu] in shred[id=%lu:%s]",
                    m_linepos, shred->xid, shred->name.c_str());
                EM_exception(
                    "...(hint: need to declare global Object earlier in file)" );
                goto error;
            }

            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_object( m_name );

            // push global map content into object-reg stack
            push_( reg_sp, val );
        }
            break;
        case te_globalArraySymbol:
        {
            // all array allocations return a Chuck_Object * casted to an int
            // --> put exactly that on the stack
            t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
            t_CKUINT val = (t_CKUINT) vm->globals_manager()->get_global_array( m_name );

            // push global map content into object-reg stack
            push_( reg_sp, val );

        }
            break;
        default:
            // we have a problem | 1.5.0.1 (ge) added
            EM_exception(
                "UnhandledGlobalType: on line[%lu] in shred[id=%lu:%s] unhandled-type[flag=%d]",
                m_linepos, shred->xid, shred->name.c_str(), m_type );
            goto error;
            break;
    }

    return;

error:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;

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
void Chuck_Instr_Reg_Push_Global_Addr::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // find addr
    t_CKUINT addr = 0;
    switch( m_type ) {
        case te_globalInt:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_int( m_name );
            break;
        case te_globalFloat:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_float( m_name );
            break;
        case te_globalString:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_string( m_name );
            break;
        case te_globalEvent:
            // TODO: should this be a * or a * * ?
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_event( m_name );
            break;
        case te_globalUGen:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_ugen( m_name );
            break;
        case te_globalObject:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_object( m_name );
            break;
        case te_globalArraySymbol:
            addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_array( m_name );
            break;
        default:
            EM_error3( "Chuck_Instr_Reg_Push_Global_Addr: unrecognized type flag %d...", m_type );
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
// desc: pop an int value from reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: pop a float value from reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Float::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& reg_sp = (t_CKFLOAT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: pop a vec2/complex/polar value from reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Vec2ComplexPolar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC2 *& reg_sp = (t_CKVEC2 *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: pop a vec3 value from reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC3 *& reg_sp = (t_CKVEC3 *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: pop a vec4 value from reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKVEC4 *& reg_sp = (t_CKVEC4 *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_WordsMulti::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
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
// desc: vec2 == vec2
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x == (sp+1)->x) && (sp->y == (sp+1)->y) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec2 != vec2
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // also works for polar
    t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
    t_CKUINT *& sp_uint = (t_CKUINT *&)sp;
    pop_( sp, 2 );
    push_( sp_uint, (sp->x != (sp+1)->x) || (sp->y != (sp+1)->y) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: vec3 == vec3
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
// desc: vec3 != vec3
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
// desc: vec4 == vec4
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
// desc: vec4 != vec4
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
// desc: alloc local variable (int)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // overflow detection | 1.5.1.5 (ge) added
    if( would_overflow_( mem_sp+m_val, shred->mem ) ) goto overflow;

    // zero out the memory stack
    *( (t_CKUINT *)(mem_sp + m_val) ) = 0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );

    // done
    return;

overflow:
    // handle overflow
    ck_handle_overflow( shred, vm, "too many local variables" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local variable (float)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // overflow detection | 1.5.1.5 (ge) added
    if( would_overflow_( mem_sp+m_val, shred->mem ) ) goto overflow;

    // zero out the memory stack
    *( (t_CKFLOAT *)(mem_sp + m_val) ) = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );

    // done
    return;

overflow:
    // handle overflow
    ck_handle_overflow( shred, vm, "too many local variables" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local variable (complex or polar)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // overflow detection | 1.5.1.5 (ge) added
    if( would_overflow_( mem_sp+m_val, shred->mem ) ) goto overflow;

    // zero out the memory stack
    ( (t_CKCOMPLEX *)(mem_sp + m_val) )->re = 0.0;
    ( (t_CKCOMPLEX *)(mem_sp + m_val) )->im = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );

    // done
    return;

overflow:
    // handle overflow
    ck_handle_overflow( shred, vm, "too many local variables" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local variable (vec3)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // overflow detection | 1.5.1.5 (ge) added
    if( would_overflow_( mem_sp+m_val, shred->mem ) ) goto overflow;

    // zero out the memory stack
    ( (t_CKVEC3 *)(mem_sp + m_val) )->x = 0.0;
    ( (t_CKVEC3 *)(mem_sp + m_val) )->y = 0.0;
    ( (t_CKVEC3 *)(mem_sp + m_val) )->z = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );

    // done
    return;

overflow:
    // handle overflow
    ck_handle_overflow( shred, vm, "too many local variables" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc local variable (vec4)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKBYTE *& mem_sp = (t_CKBYTE *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // overflow detection | 1.5.1.5 (ge) added
    if( would_overflow_( mem_sp+m_val, shred->mem ) ) goto overflow;

    // zero out the memory stack
    ( (t_CKVEC4 *)(mem_sp + m_val) )->x = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->y = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->z = 0.0;
    ( (t_CKVEC4 *)(mem_sp + m_val) )->w = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(mem_sp + m_val) );

    // done
    return;

overflow:
    // handle overflow
    ck_handle_overflow( shred, vm, "too many local variables" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member variable (int)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the object data block entry
    *( (t_CKUINT *)(obj->data + m_val) ) = 0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member variable (float)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the object data block entry
    *( (t_CKFLOAT *)(obj->data + m_val) ) = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member variable (complex or polar)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Word4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the object data block entry
    ( (t_CKCOMPLEX *)(obj->data + m_val) )->re = 0.0;
    ( (t_CKCOMPLEX *)(obj->data + m_val) )->im = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member variable (vec3)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the object data block entry
    ( (t_CKVEC3 *)(obj->data + m_val) )->x = 0.0;
    ( (t_CKVEC3 *)(obj->data + m_val) )->y = 0.0;
    ( (t_CKVEC3 *)(obj->data + m_val) )->z = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc member variable (vec4)
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Member_Vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // get the object
    Chuck_Object * obj = (Chuck_Object *)*(mem_sp);
    // zero out the object data block entry
    ( (t_CKVEC4 *)(obj->data + m_val) )->x = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->y = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->z = 0.0;
    ( (t_CKVEC4 *)(obj->data + m_val) )->w = 0.0;
    // push addr onto operand stack
    push_( reg_sp, (t_CKUINT)(obj->data + m_val) );
}




// function prototype
void call_pre_constructor( Chuck_VM * vm, Chuck_VM_Shred * shred,
    Chuck_VM_Code * pre_ctor, t_CKUINT stack_offset );
//-----------------------------------------------------------------------------
// name: call_all_parent_pre_constructors()
// desc: traverse up type parent tree calling pre constructors top-down
//-----------------------------------------------------------------------------
void call_all_parent_pre_constructors( Chuck_VM * vm, Chuck_VM_Shred * shred,
    Chuck_Type * type, t_CKUINT stack_offset )
{
    // first, call parent ctor
    if( type->parent != NULL )
    {
        call_all_parent_pre_constructors( vm, shred, type->parent, stack_offset );
    }
    // next, call my pre-ctor
    if( type->has_pre_ctor )
    {
        call_pre_constructor( vm, shred, type->info->pre_ctor, stack_offset );
    }
    // next, call my default ctor | 1.5.2.2 (ge)
    if( type->ctor_default && type->ctor_default->code )
    {
        call_pre_constructor( vm, shred, type->ctor_default->code, stack_offset );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: alloc global
//-----------------------------------------------------------------------------
void Chuck_Instr_Alloc_Word_Global::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKUINT addr = 0;

    // init in the correct vm map according to the type
    if( m_is_array )
    {
        switch( m_type )
        {
            case te_globalInt:
            case te_globalFloat:
                vm->globals_manager()->init_global_array( m_name, m_chuck_type, m_type );
                addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_array( m_name );
                break;
            case te_globalString:
                EM_error2( 0, "global string arrays are currently disabled." );
                goto error;
                break;
            case te_globalEvent:
                EM_error2( 0, "global Event arrays are currently disabled." );
                goto error;
                break;
            case te_globalUGen:
                EM_error2( 0, "global UGen arrays are currently disabled." );
                goto error;
                break;
            case te_globalObject:
                EM_error2( 0, "global Object arrays are currently disabled." );
                goto error;
                break;
            case te_globalArraySymbol:
                EM_error2( 0, "(internal error) symbol-only global type used in allocation" );
                goto error;
            default:
                // we have a problem | 1.5.0.1 (ge) added
                EM_exception(
                    "UnhandledGlobalType: on line[%lu] in shred[id=%lu:%s] unhandled-type[flag=%d]",
                                  m_linepos, shred->xid, shred->name.c_str(), m_type );
                goto error;
                break;
        }

    }
    else
    {
        // not array
        switch( m_type ) {
            case te_globalInt:
                vm->globals_manager()->init_global_int( m_name );
                addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_int( m_name );
                break;
            case te_globalFloat:
                vm->globals_manager()->init_global_float( m_name );
                addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_float( m_name );
                break;
            case te_globalString:
                vm->globals_manager()->init_global_string( m_name );
                addr = (t_CKUINT) vm->globals_manager()->get_ptr_to_global_string( m_name );
                break;
            case te_globalEvent:
                // events are already init in emit
                // but might need to execute ctors (below)
                addr = (t_CKUINT) vm->globals_manager()->get_global_event( m_name );
                break;
            case te_globalUGen:
                // ugens are already init in emit
                // but might need to execute ctors (below)
                addr = (t_CKUINT) vm->globals_manager()->get_global_ugen( m_name );
                break;
            case te_globalObject:
                // object are already init in emit
                // but might need to execute ctors (below)
                addr = (t_CKUINT) vm->globals_manager()->get_global_object( m_name );
                break;
            case te_globalArraySymbol:
                EM_error2( 0, "(internal error) symbol-only global type used in allocation" );
                goto error;
            default:
                // we have a problem | 1.5.0.1 (ge) added
                EM_exception(
                    "UnhandledGlobalType: on line[%lu] in shred[id=%lu:%s] unhandled-type[flag=%d]",
                    m_linepos, shred->xid, shred->name.c_str(), m_type );
                goto error;
                break;
        }
    }

    // push addr onto operand stack
    push_( reg_sp, addr );

    // if we have ctors to execute, do it
    if( m_should_execute_ctors &&
        vm->globals_manager()->should_call_global_ctor( m_name, m_type ) )
    {
        // call ctors (normally done by the pre_constructor instruction)
        call_all_parent_pre_constructors( vm, shred,
            m_chuck_type, m_stack_offset );
        // tell VM we did it so that it will never be done again for m_name
        vm->globals_manager()->global_ctor_was_called( m_name, m_type );
    }

    return;

error:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




#pragma mark === Object Initialization/Construction ===


static Chuck_Instr_Func_Call g_func_call;
static Chuck_Instr_Func_Call_Member g_func_call_member( 0, NULL );
//-----------------------------------------------------------------------------
// name: call_pre_constructor()
// desc: invoke class pre-constructor code
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

    // NOTE (1.5.0.0): pre-constructors by their nature have no arguments...
    // if they did, they would need to clean up argument list, in the built-in/native
    // func case, by setting g_func_call_member.m_func_ref with the appropriate func

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
// name: params()
// desc: params for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Pre_Constructor::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "ctor='%s', offset=%lu", pre_ctor ? pre_ctor->name.c_str() : "[null]", (unsigned long)stack_offset );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: instantiate_object()
// desc: instantiate Object including data and virtual table
//-----------------------------------------------------------------------------
t_CKBOOL initialize_object( Chuck_Object * object, Chuck_Type * type, Chuck_VM_Shred * shred, Chuck_VM * vm, t_CKBOOL setShredOrigin )
{
    // check if already initialized | 1.5.1.5
    if( object->vtable != NULL ) return TRUE;

    // sanity
    assert( type != NULL );
    assert( type->info != NULL );

    // REFACTOR-2017: added | 1.5.1.5 (ge & andrew) moved here from instantiate_...
    object->setOriginVM( vm );
    // set origin shred for non-ugens | 1.5.1.5 (ge & andrew) moved here from instantiate_...
    if( !type->ugen_info && setShredOrigin ) object->setOriginShred( shred );

    // allocate virtual table
    object->vtable = new Chuck_VTable;
    if( !object->vtable ) goto out_of_memory;
    // copy the object's virtual table
    object->vtable->funcs = type->info->obj_v_table.funcs;
    // set the type reference
    object->type_ref = type;
    // reference count
    CK_SAFE_ADD_REF(object->type_ref);
    // get the size
    object->data_size = type->obj_size;
    // allocate memory
    if( object->data_size )
    {
        // check to ensure enough memory
        object->data = new t_CKBYTE[object->data_size];
        if( !object->data ) goto out_of_memory;
        // zero it out
        memset( object->data, 0, object->data_size );
    }
    else object->data = NULL;

    // special
    if( type->ugen_info )
    {
        // ugen
        Chuck_UGen * ugen = (Chuck_UGen *)object;
        // UGens: needs shred for auto-disconnect when shred is removed
        // 1.5.1.5 (ge & andrew) moved from instantiate_and_initialize_object()
        if( shred )
        {
            // add ugen to shred (ref-counted)
            shred->add( ugen );
            // add shred to ugen (ref-counted) | 1.5.1.5 (ge) was: ugen->shred = shred;
            object->setOriginShred( shred );
        }
        // set tick
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
                ugen->originVM()->env()->ckt_ugen, ugen->originShred(), ugen->originVM() );
            // cast to ugen
            ugen->m_multi_chan[i] = (Chuck_UGen *)obj;
            // additional reference count
            CK_SAFE_ADD_REF(obj);
            // owner
            ugen->m_multi_chan[i]->owner_ugen = ugen;
            // ref count
            // spencer 2013-5-20: don't add extra ref, to avoid a ref cycle
            // ugen->add_ref();
        }
        // TODO: alloc channels for uana
    }

    return TRUE;

out_of_memory:

    // we have a problem
    EM_exception(
        "OutOfMemory: while instantiating object '%s'",
        type->c_name() );

    // delete
    if( object ) CK_SAFE_DELETE( object->vtable );

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
    Chuck_UGen * ugen = NULL;
    Chuck_UAna * uana = NULL;
    vector<Chuck_Type::CallbackOnInstantiate> instance_cbs;
    t_CKBOOL setShredOrigin = FALSE;

    // sanity
    assert( type != NULL );
    assert( type->info != NULL );

    // allocate the VM object
    if( !type->ugen_info )
    {
        // check type TODO: make this faster
        if( type->allocator )
            object = type->allocator( vm, shred, Chuck_DL_Api::instance() );
        else if( isa( type, vm->env()->ckt_fileio ) ) object = new Chuck_IO_File( vm );
        else if( isa( type, vm->env()->ckt_event ) ) object = new Chuck_Event;
        else if( isa( type, vm->env()->ckt_string ) ) object = new Chuck_String;
        // TODO: is this ok?
        else if( isa( type, vm->env()->ckt_shred ) )
        {
            // instantiate shred
            Chuck_VM_Shred * newShred = new Chuck_VM_Shred();
            // set vm_ref
            newShred->vm_ref = vm;
            // copy to object reference (in case of error, object will be deleted)
            object = newShred;

            // get stack size hints | 1.5.1.5
            t_CKINT mems = shred ? shred->childGetMemSize() : 0;
            t_CKINT regs = shred ? shred->childGetRegSize() : 0;
            // initialize shred | 1.5.1.5 (ge) added, along with child mem and reg stack size hints
            if( !newShred->initialize( NULL, mems, regs ) ) goto error;
        }
        // 1.5.0.0 (ge) added -- here my feeble brain starts leaking out of my eyeballs
        else if( isa( type, vm->env()->ckt_class ) ) object = new Chuck_Type( vm->env(), te_class, type->base_name, type, type->size );
        // TODO: is this ok?
        else object = new Chuck_Object;
    }
    else
    {
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
    }

    // check to see enough memory
    if( !object ) goto out_of_memory;

    // check for callback
    setShredOrigin = type->cbs_on_instantiate( instance_cbs );

    // initialize
    if( !initialize_object( object, type, shred, vm, setShredOrigin ) ) goto error;

    // check for callback
    if( instance_cbs.size() )
    {
        // loop over callbacks to call
        for( t_CKUINT i = 0; i < instance_cbs.size(); i++ )
        {
            // call it
            instance_cbs[i].callback( object, type, shred, vm );
        }
    }

    // return the instantiated object
    return object;

out_of_memory:

    // we have a problem
    EM_exception(
        "OutOfMemory: while instantiating object '%s'",
        type->c_name() );

error:

    // delete
    CK_SAFE_DELETE( object );

    // return NULL
    return NULL;
}




//-----------------------------------------------------------------------------
// name: instantiate_object()
// desc: instantiate a object, push its pointer on reg stack
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
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "%s", this->type->c_name() );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: params()
// desc: text description
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Pre_Ctor_Array_Top::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "val=%ld, type=\"%s\"", (long)m_val, type ? type->c_name() : "[empty]" );
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
    CK_SAFE_DELETE_ARRAY( arr );
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
// desc: assign primitive (4 word) vec2 complex polar
//-----------------------------------------------------------------------------
void Chuck_Instr_Assign_Primitive4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 + (sz_VEC2 / sz_UINT) ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // copy popped value into mem stack
    *( (t_CKVEC2*)(*(reg_sp+(sz_VEC2/sz_UINT))) ) = *(t_CKVEC2 *)reg_sp; // ISSUE: 64-bit (fixed 1.3.1.0)

    t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)reg_sp;
    push_( sp_vec2, *sp_vec2 );
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

    // hmm if need to debug
    // CK_FPRINTF_STDERR( "obj: 0x%08x 0x%08x\n", *obj, done );

    // add reference
    if( *obj ) (*obj)->add_ref();
    // release
    if( done ) done->release();

    // FYI this instruction expects a variable address in obj
    // but ends up pushing *obj (the value pointed to by obj)
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
            (*rhs_ptr) = (Chuck_String *)instantiate_and_initialize_object( vm->env()->ckt_string, shred );
            // add ref
            (*rhs_ptr)->add_ref();
            (*rhs_ptr)->set( lhs->str() );
        }
        //EM_error2( 0, "(internal error) somehow the type checker has allowed NULL strings" );
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
// desc: ref-count increment the top of the register stack (added 1.3.0.0)
//       1.5.0.0 (ge) additional notes...
//       this is used to keep objects referenced while on stack;
//       relevant for cases like `return obj;` where obj may need to
//       released for the local stack AND need to be kept on the stack
//       to be return; in this case, we need this additional add_ref();
//       releasing obj from stack is responsibility of the caller
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_AddRef_Object3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
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
        // add reference
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




//-----------------------------------------------------------------------------
// name: execute()
// desc: release object reference + pop from reg stack | 1.5.0.0 (ge) added
//       the variant assumes object pointer directly on stack (not offset)
//       used to release returned object pointer after a function call;
//       FYI the return value is conveyed on the reg stack; this undoes
//       the addref before the return; see Chuck_Instr_Reg_AddRef_Object3
//-----------------------------------------------------------------------------
void Chuck_Instr_Release_Object3_Pop_Int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_VM_Object * obj = NULL;

    // pop
    pop_( reg_sp, 1 );
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(reg_sp) );
    // ge (2012 april): check for NULL (added 1.3.0.0)
    if( obj != NULL )
    {
        // release
        obj->release();
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: release object reference from reg stack | 1.5.0.0 (ge) added
//       the variant assumes object pointer directly on stack (not offset)
//       used to release objects on function arguments, specifically for
//       built-in / imported (chugins) functions; code-defined functions
//       do their own argument cleanup
//-----------------------------------------------------------------------------
void Chuck_Instr_Release_Object4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // NOTE: this pointer is NOT a reference pointer
    t_CKBYTE * mem_sp = (t_CKBYTE *&)shred->mem->sp;
    Chuck_VM_Object * obj = NULL;

    // move it
    mem_sp += m_val;
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp) );
    if( obj != NULL )
    {
        // release
        obj->release();
    }
}




#pragma mark === Function Calls ===



//-----------------------------------------------------------------------------
// name: execute()
// desc: converts, in place on operand stack, a Func to its Code (instructions)
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
// name: func_release_args() | 1.5.0.0 (ge) added
// desc: helper function to release objects an function arg list
//       input: VM, args list, mem stack pointer
//       context: this is designed for builtin/imported functions
//                functions defined in chuck code do their own arg list cleanup
//-----------------------------------------------------------------------------
t_CKBOOL func_release_args( Chuck_VM * vm, a_Arg_List args, t_CKBYTE * mem_sp )
{
    // keep track of offset
    t_CKUINT offset = 0;
    // the type in question
    Chuck_Type * type = NULL;
    // object pointer
    Chuck_VM_Object * object = NULL;

    // loop over
    while( args )
    {
        // get the type
        type = args->type;
        // check we have one!
        if( type != NULL )
        {
            // is an object?
            if( isobj( vm->env(), args->type) )
            {
                // get the object pointer
                object = (Chuck_VM_Object *)(*(t_CKUINT *)(mem_sp+offset));
                // check
                if( object != NULL )
                {
                    // release it
                    object->release();
                }
            }

            // advance offset
            offset += args->type->size;
        }

        // go
        args = args->next;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Func_Call::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "convention='%s'", m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK ? "this:back" : "this:front" );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // pop words
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*reg_sp;
    // get the local stack depth - caller local variables
    t_CKUINT local_depth = *(reg_sp+1);
    // convert to number of int's (was: 4-byte words), extra partial word counts as additional word
    local_depth = ( local_depth / sz_INT ) + ( local_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
    // get the stack depth of the callee function args
    t_CKUINT stack_depth_ints = ( func->stack_depth / sz_INT ) + ( func->stack_depth & 0x3 ? 1 : 0 ); // ISSUE: 64-bit (fixed 1.3.1.0)
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
    push_( mem_sp, stack_depth_ints );
    // set the pc to 0
    shred->next_pc = 0;
    // set the code
    shred->code = func;
    // set the instruction to the function instruction
    shred->instr = func->instr;

    // detect overflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // if there are arguments to be passed
    if( stack_depth_ints )
    {
        // pop the arguments, by number of words
        pop_( reg_sp, stack_depth_ints );

        // make copies (but without modifying actual stack pointers)
        t_CKUINT * mem_sp2 = (t_CKUINT *)mem_sp;
        t_CKUINT * reg_sp2 = (t_CKUINT *)reg_sp;

        // detect would-be overflow | 1.5.1.5 (ge) added
        if( would_overflow_( mem_sp2+stack_depth_ints, shred->mem ) ) goto error_overflow;

        // need this
        if( func->need_this )
        {
            // check convention | 1.5.2.0 (ge) added
            if( m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK )
            {
                // copy this from end of arguments to the front
                *mem_sp2++ = *(reg_sp2 + stack_depth_ints - 1);
            } else {
                // copy this from start of arguments
                *mem_sp2++ = *reg_sp2++;
            }
            // one less word to copy
            stack_depth_ints--;
        }
        // static inside class | 1.4.1.0 (ge) added
        else if( func->is_static )
        {
            // check convention | 1.5.2.0 (ge) added
            if( m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK )
            {
                // copy type from end of arguments to the front
                *mem_sp2++ = *(reg_sp2 + stack_depth_ints - 1);
            } else {
                // copy type from start of arguments
                *mem_sp2++ = *reg_sp2++;
            }
            // one less word to copy
            stack_depth_ints--;
        }

        // push the arguments
        for( t_CKUINT i = 0; i < stack_depth_ints; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    return;

error_overflow:

    ck_handle_overflow( shred, vm, "too many nested/recursive function calls" );
}




//-----------------------------------------------------------------------------
// for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Func_Call_Member::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "%s, %s",
              m_func_ref ? m_func_ref->signature(FALSE,FALSE).c_str() : "[null]",
              m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK ? "this:back" : "this:front" );
    return buffer;
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

    // detect overflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // pass args
    if( stack_depth )
    {
        // pop the arguments for pass to callee function
        reg_sp -= stack_depth;

        // make copies (without modifying actual stack pointers)
        t_CKUINT * reg_sp2 = reg_sp;
        t_CKUINT * mem_sp2 = mem_sp;

        // detect would-be overflow | 1.5.1.5 (ge) added
        if( would_overflow_( mem_sp2+stack_depth, shred->mem ) ) goto error_overflow;

        // need this
        if( func->need_this )
        {
            // check convention | 1.5.2.0 (ge) added
            if( m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK )
            {
                // copy this from end of arguments to the front
                *mem_sp2++ = *(reg_sp2 + stack_depth - 1);
            } else {
                // copy this from start of arguments
                *mem_sp2++ = *reg_sp2++;
            }
            // one less word to copy
            stack_depth--;
        }
        // copy to args
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // check the function pointer kind: ctor or mfun?
    if( func->native_func_kind == ae_fp_ctor ) // ctor
    {
        // cast to right type
        f_ctor f = (f_ctor)func->native_func;
        // call (added 1.3.0.0 -- Chuck_DL_Api::instance())
        f( (Chuck_Object *)(*mem_sp), mem_sp + 1, vm, shred, Chuck_DL_Api::instance() );
    }
    else // mfun
    {
        // make sure is mfun
        assert( func->native_func_kind == ae_fp_mfun );
        // cast to right type
        f_mfun f = (f_mfun)func->native_func;
        // call the function (added 1.3.0.0 -- Chuck_DL_Api::instance())
        f( (Chuck_Object *)(*mem_sp), mem_sp + 1, &retval, vm, shred, Chuck_DL_Api::instance() );
    }

    // push the return
    // 1.3.1.0: check type to use kind instead of size
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed: 1.3.1.0)
    {
        // push the return args
        push_( reg_sp, retval.v_uint );

        // 1.5.0.0 (ge) | added -- ensure ref count
        if( m_func_ref && isobj(vm->env(), m_func_ref->def()->ret_type) )
        {
            // get return value as object reference
            Chuck_VM_Object * obj = (Chuck_VM_Object *)retval.v_uint;
            if( obj )
            {
                // always add reference to returned objects (should release outside)
                obj->add_ref();
            }
        }
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
        push_( sp_double, retval.v_float );
    }
    else if( m_val == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0) | 1.5.1.7 (ge) complex -> vec2
    {
        // push the return args
        t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)reg_sp;
        // in this context vec2 = complex = polar
        push_( sp_vec2, retval.v_vec2 );
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

    // if we have a func def | 1.5.0.0 (ge) added
    if( m_func_ref != NULL )
    {
        // cleanup / release objects on the arg list
        // context: this should be done here for builtin/import functions
        //          user-defined functions do their own arg list cleanup
        // note: this is done after pushing the return value, in case the
        //       return value is one of these args being released;
        //          e.g., functions that pass through args;
        //          e.g., string Sndbuf.read(string)
        func_release_args( vm, m_func_ref->def()->arg_list, (t_CKBYTE *)(mem_sp+1) );
    }

    // pop the stack pointer
    mem_sp -= push;

    return;

error_overflow:

    ck_handle_overflow( shred, vm, "too many nested/recursive function calls" );
}




//-----------------------------------------------------------------------------
// for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Func_Call_Static::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "%s, %s",
              m_func_ref ? m_func_ref->signature(FALSE,FALSE).c_str() : "[null]",
              m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK ? "this:back" : "this:front" );
    return buffer;
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

    // pop code and local depth
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*(reg_sp);
    // get the function to be called
    f_sfun f = (f_sfun)func->native_func;
    // verify | 1.5.2.0
    assert( func->native_func_kind == ae_fp_sfun );
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
    // push the mem stack past the current function variables and arguments
    mem_sp += push;

    // detect overflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // pass args
    if( stack_depth )
    {
        // pop the arguments for pass to callee function
        reg_sp -= stack_depth;

        // make copies (without modifying actual stack pointers)
        t_CKUINT * reg_sp2 = reg_sp;
        t_CKUINT * mem_sp2 = mem_sp;

        // detect would-be overflow | 1.5.1.5 (ge) added
        if( would_overflow_( mem_sp2+stack_depth, shred->mem ) ) goto error_overflow;

        // need type
        if( func->is_static )
        {
            // check convention | 1.5.2.0 (ge) added
            if( m_arg_convention == CK_FUNC_CALL_THIS_IN_BACK )
            {
                // copy this from end of arguments to the front
                *mem_sp2++ = *(reg_sp2 + stack_depth - 1);
            } else {
                // copy this from start of arguments
                *mem_sp2++ = *reg_sp2++;
            }
            // one less word to copy
            stack_depth--;
        }
        // copy to args
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // call the function
    // (added 1.3.0.0 -- Chuck_DL_Api::instance())
    // (added 1.4.1.0 -- base_type)
    f( (Chuck_Type *)(*mem_sp), mem_sp+1, &retval, vm, shred, Chuck_DL_Api::instance() );

    // push the return
    // 1.3.1.0: check type to use kind instead of size
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        push_( reg_sp, retval.v_uint );

        // 1.5.0.0 (ge) | added -- ensure ref count
        if( m_func_ref && isobj(vm->env(), m_func_ref->def()->ret_type) )
        {
            // get return value as object reference
            Chuck_VM_Object * obj = (Chuck_VM_Object *)retval.v_uint;
            if( obj )
            {
                // always add reference to returned objects (should release outside)
                obj->add_ref();
            }
        }
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
        push_( sp_double, retval.v_float );
    }
    else if( m_val == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0) | 1.5.1.7 (ge) complex -> vec2
    {
        // push the return args
        t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)reg_sp;
        // in this context vec2 = complex = polar
        push_( sp_vec2, retval.v_vec2 );
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

    // if we have a func def | 1.5.0.0 (ge) added
    if( m_func_ref != NULL )
    {
        // cleanup / release objects on the arg list
        // context: this should be done here for builtin/import functions
        //          user-defined functions do their own arg list cleanup
        // note: this is done after pushing the return value, in case the
        //       return value is one of these args being released;
        //          e.g., functions that pass through args;
        //          e.g., string Sndbuf.read(string)
        func_release_args( vm, m_func_ref->def()->arg_list, (t_CKBYTE *)(mem_sp+1) );
    }

    // pop the stack pointer
    mem_sp -= push;

    return;

error_overflow:

    ck_handle_overflow( shred, vm, "too many nested/recursive function calls" );
}




//-----------------------------------------------------------------------------
// for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Func_Call_Global::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "%s",
              m_func_ref ? m_func_ref->signature(FALSE,FALSE).c_str() : "[null]" );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.5
// desc: imported global function call with return
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call_Global::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& mem_sp = (t_CKUINT *&)shred->mem->sp;
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    Chuck_DL_Return retval;

    // pop code and local depth
    pop_( reg_sp, 2 );
    // get the function to be called as code
    Chuck_VM_Code * func = (Chuck_VM_Code *)*(reg_sp);
    // get the function to be called
    f_gfun f = (f_gfun)func->native_func;
    // verify | 1.5.2.0
    assert( func->native_func_kind == ae_fp_gfun );
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
    // push the mem stack past the current function variables and arguments
    mem_sp += push;

    // detect overflow
    if( overflow_( shred->mem ) ) goto error_overflow;

    // pass args
    if( stack_depth )
    {
        // pop the arguments for pass to callee function
        reg_sp -= stack_depth;

        // make copies (without modifying actual stack pointers)
        t_CKUINT * reg_sp2 = reg_sp;
        t_CKUINT * mem_sp2 = mem_sp;

        // detect would-be overflow | 1.5.1.5 (ge) added
        if( would_overflow_( mem_sp2+stack_depth, shred->mem ) ) goto error_overflow;

        // copy to args
        for( t_CKUINT i = 0; i < stack_depth; i++ )
            *mem_sp2++ = *reg_sp2++;
    }

    // call the function
    f( mem_sp, &retval, vm, shred, Chuck_DL_Api::instance() );

    // push the return
    // 1.3.1.0: check type to use kind instead of size
    if( m_val == kindof_INT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        push_( reg_sp, retval.v_uint );

        // 1.5.0.0 (ge) | added -- ensure ref count
        if( m_func_ref && isobj(vm->env(), m_func_ref->def()->ret_type) )
        {
            // get return value as object reference
            Chuck_VM_Object * obj = (Chuck_VM_Object *)retval.v_uint;
            if( obj )
            {
                // always add reference to returned objects (should release outside)
                obj->add_ref();
            }
        }
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // push the return args
        t_CKFLOAT *& sp_double = (t_CKFLOAT *&)reg_sp;
        push_( sp_double, retval.v_float );
    }
    else if( m_val == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0) | 1.5.1.7 (ge) complex -> vec2
    {
        // push the return args
        t_CKVEC2 *& sp_vec2 = (t_CKVEC2 *&)reg_sp;
        // in this context vec2 = complex = polar
        push_( sp_vec2, retval.v_vec2 );
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

    // if we have a func def | 1.5.0.0 (ge) added
    if( m_func_ref != NULL )
    {
        // cleanup / release objects on the arg list
        // context: this should be done here for builtin/import functions
        //          user-defined functions do their own arg list cleanup
        // note: this is done after pushing the return value, in case the
        //       return value is one of these args being released;
        //          e.g., functions that pass through args;
        //          e.g., string Sndbuf.read(string)
        func_release_args( vm, m_func_ref->def()->arg_list, (t_CKBYTE *)(mem_sp) );
    }

    // pop the stack pointer
    mem_sp -= push;

    return;

error_overflow:

    ck_handle_overflow( shred, vm, "too many nested/recursive function calls" );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: return from a function
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
// name: Chuck_Instr_Stmt_Start()
// desc: constructor
//-----------------------------------------------------------------------------
Chuck_Instr_Stmt_Start::Chuck_Instr_Stmt_Start( t_CKUINT numObjReleases )
{
    m_nextOffset = 0;
    m_numObjReleases = numObjReleases;
    m_stackLevel = 0;
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Instr_Stmt_Start()
// desc: destructor
//-----------------------------------------------------------------------------
Chuck_Instr_Stmt_Start::~Chuck_Instr_Stmt_Start()
{
    // drain stack
    while( m_stack.size() )
    {
        CK_SAFE_DELETE_ARRAY( m_stack.back() );
        m_stack.pop_back();
    }
}




//-----------------------------------------------------------------------------
// name: params()
// desc: for instruction dumps
//-----------------------------------------------------------------------------
// for printing
const char * Chuck_Instr_Stmt_Start::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "numObjReleases=%lu this=%p", (unsigned long)m_numObjReleases, this );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: executed at the start of a statement (see header for details)
//-----------------------------------------------------------------------------
void Chuck_Instr_Stmt_Start::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // if nothing to push, no op
    if( !m_numObjReleases ) return;

    // push level
    m_stackLevel++;

    // see if using base cache
    if( m_stackLevel == 1 && m_stack.size() == 1 ) return;

    // make new region
    t_CKUINT * region = new t_CKUINT[m_numObjReleases];
    // zero out region
    for( t_CKUINT i = 0; i < m_numObjReleases; i++ )
    {
        // zero out
        region[i] = 0;
    }
    // push onto stack
    m_stack.push_back( region );
}




//-----------------------------------------------------------------------------
// name: nextOffset()
// desc: returns next offset on success; 0 if we have exceeded numObjeReleases
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Instr_Stmt_Start::nextOffset( t_CKUINT & offset )
{
    // clear offset
    offset = 0;
    // check
    if( m_nextOffset >= m_numObjReleases )
    {
        EM_exception(
            "(internal error) out of bounds in Stmt_Start.nextIndex(): nextOffset == %lu",
            m_nextOffset );

        // return
        return FALSE;
    }

    // copy into return variable, then increment
    offset = m_nextOffset++;
    // return
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setObject()
// desc: set object into data region of objects to release by stmt's end
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Instr_Stmt_Start::setObject( Chuck_VM_Object * object, t_CKUINT offset )
{
    // check
    if( m_stackLevel == 0 || m_stack.size() == 0 )
    {
        EM_exception(
            "(internal error) region stack inconsistency in Stmt_Start.setObject(): level=%lu size=%lu",
            m_stackLevel, (t_CKUINT)m_stack.size() );
        // return
        return FALSE;
    }

    // check
    if( offset >= m_numObjReleases )
    {
        EM_exception(
            "(internal error) offset out of bounds in Stmt_Start.setObject(): offset == %lu",
            offset );
        // return
        return FALSE;
    }

    // region pointer
    t_CKUINT * region = m_stack.back();
    // pointer arithmetic
    t_CKUINT * pInt = region + offset;

    // release if not NULL; what was previously there is no-longer accessible
    // NOTE this could happen in the case of a loop:
    // e.g., while( foo() ) { ... } // where foo() returns an object
    Chuck_VM_Object * outgoing = (Chuck_VM_Object *)(*pInt);
    CK_SAFE_RELEASE( outgoing );

    // copy incoming object pointer
    *pInt = (t_CKUINT)object;
    // done
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanupRefs()
// desc: clean up references
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Instr_Stmt_Start::cleanupRefs( Chuck_VM_Shred * shred )
{
    // if nothing to push, no op
    if( !m_numObjReleases ) return TRUE;

    // check
    if( m_stackLevel == 0 || m_stack.size() == 0 )
    {
        EM_exception(
            "(internal error) region stack inconsistency in Stmt_Start.cleanupRefs(): level=%lu size=%lu on shred[id=%lu:%s]",
            m_stackLevel, (t_CKUINT)m_stack.size(), shred->xid, shred->name.c_str() );
        // return
        return FALSE;
    }

    // cast pointer to data region as Object pointers
    t_CKUINT * pInt = m_stack.back();

    // make room for all the object references to release
    for( t_CKUINT i = 0; i < m_numObjReleases; i++ )
    {
        // object pointer
        Chuck_VM_Object * object = (Chuck_VM_Object *)(*pInt);
        // release (could be NULL)
        CK_SAFE_RELEASE( object );
        // zero out the region
        *pInt = 0;
        // advance pointer
        pInt++;
    }

    // decrement stack level
    m_stackLevel--;
    // pop stack unless we are level 1
    if( m_stack.size() > 1 )
    {
        // clean up
        CK_SAFE_DELETE_ARRAY( m_stack.back() );
        // pop
        m_stack.pop_back();
    }

    return TRUE;
}





//-----------------------------------------------------------------------------
// name: params()
// desc: for instruction dumps
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Stmt_Remember_Object::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "offset=%lu start=%p", (unsigned long)m_offset, m_stmtStart );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: remember obj ref on reg stack for stmt-related cleanup
//-----------------------------------------------------------------------------
void Chuck_Instr_Stmt_Remember_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // get stack pointer
    t_CKUINT * reg_sp = (t_CKUINT *)shred->reg->sp;
    Chuck_VM_Object * obj = (Chuck_VM_Object *)(*(reg_sp-1));

    // add-ref for certain expressions (e.g., 'new Object;`)
    if( m_addRef ) CK_SAFE_ADD_REF( obj );

    // check
    if( !m_stmtStart )
    {
        EM_exception(
            "(internal error) missing data region information in Stmt_Remember_Object instruction..." );
        goto error;
    }

    // clear the objects returns by function calls in the statement
    if( !m_stmtStart->setObject( obj, m_offset ) )
    {
        EM_exception(
            "(internal error) cannot setObject() in Stmt_Remember_Object instruction..." );
        goto error;
    }

    // done
    return;

error:
    // done
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: params()
// desc: for instruction dumps
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Stmt_Cleanup::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "numObjRelease=%lu start=%p", (unsigned long)(m_stmtStart ? m_stmtStart->m_numObjReleases : 0), m_stmtStart );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: clean up after a statement
//-----------------------------------------------------------------------------
void Chuck_Instr_Stmt_Cleanup::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // check
    if( !m_stmtStart )
    {
        EM_exception(
            "(internal error) missing data region in Stmt_Cleanup instruction..." );
        goto error;
    }

    // clean up references
    if( !m_stmtStart->cleanupRefs( shred ) )
        goto error;

    // done
    return;

error:
    // done
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: spork a shred from code
//-----------------------------------------------------------------------------
void Chuck_Instr_Spork::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;
    t_CKUINT this_ptr = 0;
    t_CKUINT type_ptr = 0;

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
    // need @type (for static functions) | 1.4.1.0 (ge) added
    else if( func->is_static )
    {
        // pop the stack
        pop_( reg_sp, 1 );
        // get type info
        type_ptr = *reg_sp;
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
    // copy type info (for static functions) | 1.4.1.0 (ge) added
    else if( func->is_static )
    {
        push_( (t_CKUINT*&)sh->reg->sp, type_ptr );
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
// desc: advance to particular time (one TIME value on reg stack)
//-----------------------------------------------------------------------------
void Chuck_Instr_Time_Advance::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKTIME *& sp = (t_CKTIME *&)shred->reg->sp;

    // pop time value from reg stack
    pop_( sp, 1 );

    // check for immediate mode exception | 1.5.1.5 (ge)
    if( shred->checkImmediatModeException(m_linepos) )
    {
        // do something!
        shred->is_running = FALSE;
        shred->is_done = TRUE;
        // bail out
        return;
    }

    // check
    if( *sp < shred->now )
    {
        // we have a problem
        EM_exception(
            "DestTimeNegative: '%.6f' on line[%lu] in shred[id=%lu:%s]",
            *sp, m_linepos, shred->xid, shred->name.c_str() );
        // do something!
        shred->is_running = FALSE;
        shred->is_done = TRUE;
        // bail out
        return;
    }

    // shredule the shred
    vm->shreduler()->shredule( shred, *sp );
    // suspend
    shred->is_running = FALSE;
    // increment towards per-shred garbage collection | 1.5.2.0 (ge)
    // NOTE 0-dur advance possible; but inc at least 1::samp
    // shred->gc_inc( ck_max((*sp)-shred->now,1) );

    // track time advance
    CK_TRACK( Chuck_Stats::instance()->advance_time( shred, *sp ) );

    // push time value on stack
    push_( sp, *sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: have current shred wait on event (expects one Event on reg stack)
//-----------------------------------------------------------------------------
void Chuck_Instr_Event_Wait::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
    // pop word from reg stack
    pop_( sp, 1 );
    // cast to event
    Chuck_Event * event = (Chuck_Event *)(*sp);

    // check for null
    if( !event ) goto null_pointer;
    // check for immediate mode exception | 1.5.1.5 (ge) added
    if( shred->checkImmediatModeException(m_linepos) ) goto done;

    // wait
    event->wait( shred, vm );

    // done
    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (null Event wait) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



#pragma mark === Arrays ===


//-----------------------------------------------------------------------------
// name: Chuck_Instr_Array_Init_Literal()
// desc: initialize array literal, e.g., [1,2,3]
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Init_Literal::Chuck_Instr_Array_Init_Literal( Chuck_Env * env, Chuck_Type * t, t_CKINT length )
{
    // set
    m_length = length;
    // copy
    m_type_ref = t;
    // add reference
    m_type_ref->add_ref();
    // type
    m_param_str = new char[72]; // 1.5.0.1 (ge) changed from 64 to 72
    // obj | REFACTOR-2017: added env
    m_is_obj = isobj( env, m_type_ref );
    // float | 1.4.2.0 (ge) added to differentiate between int and float arrays
    // -- in situations where they are the same size
    m_is_float = isa( t, env->ckt_float );

    // copy type name
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
    snprintf( buffer, 16, "[%ld]", (long)m_length );
    strcat( m_param_str, buffer );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Instr_Array_Init_Literal()
// desc: ...
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Init_Literal::~Chuck_Instr_Array_Init_Literal()
{
    // delete
    CK_SAFE_DELETE_ARRAY( m_param_str );
    // release
    CK_SAFE_RELEASE( m_type_ref );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Array_Init_Literal::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg stack pointer
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // allocate the array
    // 1.4.2.0 (ge) | added: check for float explicitly
    if( m_type_ref->size == sz_INT && !m_is_float ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // TODO: look at size and treat Chuck_ArrayInt as ChuckArrayInt
        // pop the values
        pop_( reg_sp, m_length );
        // instantiate array
        Chuck_ArrayInt * array = new Chuck_ArrayInt( m_is_obj, m_length );
        // problem
        if( !array ) goto out_of_memory;
        // initialize object
        // TODO: should it be this??? initialize_object( array, m_type_ref );
        initialize_object( array, vm->env()->ckt_array, shred, vm );
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
        Chuck_ArrayFloat * array = new Chuck_ArrayFloat( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKFLOAT * sp = (t_CKFLOAT *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->ckt_array, shred, vm );
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else if( m_type_ref->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0) | also should work for vec2 (noted 1.5.1.7)
    {
        // pop the values
        pop_( reg_sp, m_length * (sz_VEC2 / sz_INT) ); // 1.3.1.0 added size division
        // instantiate array
        Chuck_ArrayVec2 * array = new Chuck_ArrayVec2( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKVEC2 * sp = (t_CKVEC2 *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->ckt_array, shred, vm );
        // differentiate between complex and polar | 1.5.1.0 (ge) added, used for sorting Array16s
        if( isa(m_type_ref, vm->env()->ckt_polar) ) array->m_isPolarType = TRUE;
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
        Chuck_ArrayVec3 * array = new Chuck_ArrayVec3( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKVEC3 * sp = (t_CKVEC3 *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->ckt_array, shred, vm );
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
        Chuck_ArrayVec4 * array = new Chuck_ArrayVec4( m_length );
        // problem
        if( !array ) goto out_of_memory;
        // fill array
        t_CKVEC4 * sp = (t_CKVEC4 *)reg_sp;
        // intialize object
        initialize_object( array, vm->env()->ckt_array, shred, vm );
        // set size
        array->set_size( m_length );
        // fill array
        for( t_CKINT i = 0; i < m_length; i++ )
            array->set( i, *(sp + i) );
        // push the pointer
        push_( reg_sp, (t_CKUINT)array );
    }
    else
    {
        // we have a problem
        EM_exception(
            "InvalidArrayTypeInfo: while initializing arrays on line[%lu]", m_linepos );
        EM_exception(
            "...(internal consistency compromised; halting..." );
        assert( FALSE );
    }

    return;

out_of_memory:

    // we have a problem
    EM_exception(
        "OutOfMemory: while initializing arrays on line[%lu]", m_linepos );

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: Chuck_Instr_Array_Alloc()
// desc: allocate a chuck array
//-----------------------------------------------------------------------------
Chuck_Instr_Array_Alloc::Chuck_Instr_Array_Alloc( Chuck_Env * env, t_CKUINT depth, Chuck_Type * contentType,
                                                  t_CKUINT offset, t_CKBOOL is_ref, Chuck_Type * arrayType )
{
    // set
    m_depth = depth;

    // 1.5.0.0 (ge) now maintains two Chuck_Types: one for the array content;
    // one for the array itself (which has depth > 0); the latter is so that
    // array object can have a reference to the array type itself

    // copy array content type, e.g., int
    m_type_ref_content = contentType;
    // remember
    CK_SAFE_ADD_REF( m_type_ref_content );
    // remember the type of the array itself, e.g., int[][]
    m_type_ref_array = arrayType;
    CK_SAFE_ADD_REF( m_type_ref_array );

    // parameter string
    m_param_str = new char[64];
    // obj | REFACTOR-2017: added env
    m_is_obj = isobj( env, m_type_ref_content );
    // offset for pre constructor
    m_stack_offset = offset;
    // is object ref
    m_is_ref = is_ref;
    const char * str = m_type_ref_content->c_name();
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
    // delete | 1.5.0.0 (ge) convert to macro
    CK_SAFE_DELETE_ARRAY( m_param_str );
    // release | 1.5.0.0 (ge) added
    CK_SAFE_RELEASE(m_type_ref_content);
    CK_SAFE_RELEASE(m_type_ref_array);
}




//-----------------------------------------------------------------------------
// name: do_alloc_array()
// desc: 1.3.1.0 -- changed size to kind
//-----------------------------------------------------------------------------
Chuck_Object * do_alloc_array(
    Chuck_VM * vm, // REFACTOR-2017: added
    Chuck_VM_Shred * shred, // 1.5.1.5 added
    t_CKINT * capacity,
    const t_CKINT * top,
    t_CKUINT kind,
    t_CKBOOL is_obj,
    t_CKUINT * objs,
    t_CKINT & index,
    Chuck_Type * type)
{
    // not top level
    Chuck_ArrayInt * theBase = NULL;
    Chuck_Object * next = NULL;
    Chuck_Type * typeNext = NULL;
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
            Chuck_ArrayInt * baseX = new Chuck_ArrayInt( is_obj, *capacity );
            if( !baseX ) goto out_of_memory;

            // if object
            if( is_obj && objs )
            {
                // loop
                for( i = 0; i < *capacity; i++ )
                {
                    // add to array for later allocation
                    objs[index++] = baseX->addr(i);
                }
            }

            // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
            // for the object->type_ref to contain more specific information
            initialize_object( baseX, type, shred, vm );

            // initialize_object( baseX, vm->env()->ckt_array );
            return baseX;
        }
        else if( kind == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            Chuck_ArrayFloat * baseX = new Chuck_ArrayFloat( *capacity );
            if( !baseX ) goto out_of_memory;

            // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
            // for the object->type_ref to contain more specific information
            initialize_object( baseX, type, shred, vm );
            // initialize_object( baseX, vm->env()->ckt_array );
            return baseX;
        }
        else if( kind == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0) | 1.5.1.7 (ge) complex -> vec2
        {
            Chuck_ArrayVec2 * baseX = new Chuck_ArrayVec2( *capacity );
            if( !baseX ) goto out_of_memory;

            // check array type
            Chuck_Type * array_type = type->array_type;
            // differentiate between complex and polar | 1.5.1.0 (ge) added, used for sorting Array16s
            if( array_type && isa(array_type, vm->env()->ckt_polar) ) baseX->m_isPolarType = TRUE;

            // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
            // for the object->type_ref to contain more specific information
            initialize_object( baseX, type, shred, vm );
            // initialize_object( baseX, vm->env()->ckt_array );
            return baseX;
        }
        else if( kind == kindof_VEC3 ) // 1.3.5.3
        {
            Chuck_ArrayVec3 * baseX = new Chuck_ArrayVec3( *capacity );
            if( !baseX ) goto out_of_memory;

            // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
            // for the object->type_ref to contain more specific information
            initialize_object( baseX, type, shred, vm );
            // initialize_object( baseX, vm->env()->ckt_array );
            return baseX;
        }
        else if( kind == kindof_VEC4 ) // 1.3.5.3
        {
            Chuck_ArrayVec4* baseX = new Chuck_ArrayVec4( *capacity );
            if( !baseX ) goto out_of_memory;

            // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
            // for the object->type_ref to contain more specific information
            initialize_object( baseX, type, shred, vm );
            // initialize_object( baseX, vm->env()->ckt_array );
            return baseX;
        }

        // shouldn't get here
        assert( FALSE );
    }

    // not top level
    theBase = new Chuck_ArrayInt( TRUE, *capacity );
    if( !theBase ) goto out_of_memory;

    // construct type for next array level | 1.5.0.0 (ge) added
    // TODO: look up in common array type pool before allocating
    // pass in NULL (context) as typeNext is meant to be temporary and
    // shouldn't be associated with context; also by this point,
    // vm->env()->context is likely the global context, and the
    // originating context (e.g., ck file) is already cleaned up | 1.5.1.1
    typeNext = type->copy( vm->env(), NULL );
    // check
    if( typeNext->array_depth == 0 ) goto internal_error_array_depth;
    // minus the depth
    typeNext->array_depth--;
    // add ref | 1.5.1.1
    CK_SAFE_ADD_REF( typeNext );

    // allocate the next level
    for( i = 0; i < *capacity; i++ )
    {
        // the next | REFACTOR-2017: added vm
        next = do_alloc_array( vm, shred, capacity + 1, top, kind, is_obj, objs, index, typeNext );
        // error if NULL
        if( !next ) goto error;
        // set that, with ref count
        theBase->set( i, (t_CKUINT)next );
    }

    // release | 1.5.1.1
    CK_SAFE_RELEASE( typeNext );

    // initialize object | 1.5.0.0 (ge) use array type instead of base t_array
    // for the object->type_ref to contain more specific information
    initialize_object( theBase, type, shred, vm );
    // initialize_object( theBase, vm->env()->ckt_array );
    return theBase;

internal_error_array_depth:
    // we have a big problem
    EM_exception(
        "(internal error) multi-dimensional array depth mismatch while allocating arrays...");
    goto error;

out_of_memory:
    // we have a problem
    EM_exception(
        "OutOfMemory: while allocating arrays...");
    goto error;

negative_array_size:
    // we have a problem
    EM_exception(
        "NegativeArraySize: while allocating arrays...");
    goto error;

error:
    // base shouldn't have been ref counted
    CK_SAFE_DELETE( theBase );
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
        //     CK_SAFE_DELETE( shred->obj_array );
        //     shred->obj_array_size = 0;
        //     shred->obj_array = new t_CKUINT[num_obj];
        //     if( !shred->obj_array ) goto out_of_memory;
        //     shred->obj_array_size = num_obj;
        // }
    }

    // recursively allocate | REFACTOR-2017: added env
    ref = (t_CKUINT)do_alloc_array( vm, shred,
        (t_CKINT *)(reg_sp - m_depth),
        (t_CKINT *)(reg_sp - 1),
        getkindof(vm->env(), m_type_ref_content), // 1.3.1.0: changed; was 'm_type_ref->size'
        m_is_obj, obj_array, index, m_type_ref_array // 1.5.0.0: added m_type_ref_array
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
    EM_exception(
        "OverFlow: requested array size too big..." );
    goto error;

out_of_memory:
    // we have a problem
    EM_exception(
        "OutOfMemory: while allocating arrays..." );
    goto error;

error:
    EM_exception(
        "...on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );

    // done
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: normalize_index()
// desc: normalize index to allow for negative array indexing
//       1.5.0.0 (nshaheed) added
//-----------------------------------------------------------------------------
static t_CKINT normalize_index( t_CKINT i, t_CKINT len )
{
    // a normal array index
    if( i >= 0 ) return i;
    // start with len-1 and keep going down
    return len + i;
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
    t_CKINT ni = 0; // normalized
    t_CKUINT val = 0;
    t_CKFLOAT fval = 0;
    t_CKVEC2 v2;
    v2.x = 0;
    v2.y = 0;
    // vec2 should behave here as complex
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
        Chuck_ArrayInt * arr = (Chuck_ArrayInt *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // normalize index to allow for negative indexing | 1.5.0.0 (nshaheed) added
        ni = normalize_index( i, arr->size() );
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( ni );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( ni, &val ) )
                goto array_out_of_bound;
            // push the value
            push_( sp, val );
        }
    }
    else if( m_kind == kindof_FLOAT ) // ISSUE: 64-bit (1.3.1.0)
    {
        // get array
        Chuck_ArrayFloat * arr = (Chuck_ArrayFloat *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // normalize index to allow for negative indexing
        ni = normalize_index( i, arr->size() );
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( ni );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( ni, &fval ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKFLOAT *&)sp), fval );
        }
    }
    else if( m_kind == kindof_VEC2 ) // ISSUE: 64-bit | 1.5.1.7 (ge) complex -> vec2
    {
        // get array
        Chuck_ArrayVec2 * arr = (Chuck_ArrayVec2 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // normalize index to allow for negative indexing
        ni = normalize_index( i, arr->size() );
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( ni );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( ni, &v2 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC2 *&)sp), v2 );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec3 * arr = (Chuck_ArrayVec3 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp + 1));
        // normalize index to allow for negative indexing
        ni = normalize_index( i, arr->size() );
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( ni );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( ni, &v3 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC3 *&)sp), v3 );
        }
    }
    else if( m_kind == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec4 * arr = (Chuck_ArrayVec4 *)(*sp);
        // get index
        i = (t_CKINT)(*(sp+1));
        // normalize index to allow for negative indexing
        ni = normalize_index( i, arr->size() );
        // check if writing
        if( m_emit_addr ) {
            // get the addr
            val = arr->addr( ni );
            // exception
            if( !val ) goto array_out_of_bound;
            // push the addr
            push_( sp, val );
        } else {
            // get the value
            if( !arr->get( ni, &v4 ) )
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
    EM_exception(
        "NullPointer: (array access) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc removed
    goto done;

array_out_of_bound:
    // we have a problem
    EM_exception(
        "ArrayOutofBounds: on line[%lu] in shred[id=%lu:%s] index[%ld]",
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
    t_CKVEC2 v2; v2.x = v2.y = 0;
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
        Chuck_ArrayInt * arr = (Chuck_ArrayInt *)(*sp);
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
            arr->get( key->str(), &val );
            // push the value
            push_( sp, val );
        }
    }
    else if( m_kind == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_ArrayFloat * arr = (Chuck_ArrayFloat *)(*sp);
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
            arr->get( key->str(), &fval );
            // push the value
            push_( ((t_CKFLOAT *&)sp), fval );
        }
    }
    else if( m_kind == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_ArrayVec2 * arr = (Chuck_ArrayVec2 *)(*sp);
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
            arr->get( key->str(), &v2 );
            // push the value
            push_( ((t_CKVEC2 *&)sp), v2 );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec3 * arr = (Chuck_ArrayVec3 *)(*sp);
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
            arr->get( key->str(), &v3 );
            // push the value
            push_( ((t_CKVEC3 *&)sp), v3 );
        }
    }
    else if( m_kind == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec4 * arr = (Chuck_ArrayVec4 *)(*sp);
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
            arr->get( key->str(), &v4 );
            // push the value
            push_( ((t_CKVEC4 *&)sp), v4 );
        }
    }
    else
        assert( FALSE );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (map access) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc
    goto done;

error:
    // we have a problem
    EM_exception(
        "InternalArrayMapError: on line[%lu] in shred[id=%lu:%s], index=[%s]",
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
    t_CKINT * ptr = NULL;
    t_CKUINT index = 0;
    t_CKVEC2 v2; v2.x = v2.y = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // pop all indices then array
    pop_( sp, m_depth + 1 );

    // get array
    Chuck_ArrayInt * base = (Chuck_ArrayInt *)(*sp);
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
        base = (Chuck_ArrayInt *)val;
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
        Chuck_ArrayInt * arr = base;
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
        Chuck_ArrayFloat * arr = (Chuck_ArrayFloat *)(base);
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
    else if( m_kind == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_ArrayVec2 * arr = (Chuck_ArrayVec2 *)(base);
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
            if( !arr->get( i, &v2 ) )
                goto array_out_of_bound;
            // push the value
            push_( ((t_CKVEC2 *&)sp), v2 );
        }
    }
    else if( m_kind == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec3 * arr = (Chuck_ArrayVec3 *)(base);
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
        Chuck_ArrayVec4 * arr = (Chuck_ArrayVec4 *)(base);
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
    EM_exception(
        "NullPointer: (array access) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() ); // shred->pc
    EM_exception(
        "...(array dimension where exception occurred: %lu of %lu)", index+1, m_depth );
    goto done;

array_out_of_bound:
    // we have a problem
    EM_exception(
        "ArrayOutofBounds: on line[%lu] in shred[id=%lu:%s] index[%ld]",
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
    t_CKVEC2 v2; v2.x = v2.y = 0;
    t_CKVEC3 v3;
    t_CKVEC4 v4;

    // how much to pop (added 1.3.1.0)
    t_CKUINT howmuch = 0;
    // check kind
    if( m_val == kindof_INT ) howmuch = 1;
    else if( m_val == kindof_FLOAT ) howmuch = sz_FLOAT / sz_INT;
    else if( m_val == kindof_VEC2 ) howmuch = sz_VEC2 / sz_INT;
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
        Chuck_ArrayInt * arr = (Chuck_ArrayInt *)(*sp);
        // get value
        val = (t_CKINT)(*(sp+1));
        // append
        arr->push_back( val );
    }
    else if( m_val == kindof_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_ArrayFloat * arr = (Chuck_ArrayFloat *)(*sp);
        // get value
        fval = (*(t_CKFLOAT *)(sp+1));
        // append
        arr->push_back( fval );
    }
    else if( m_val == kindof_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        // get array
        Chuck_ArrayVec2 * arr = (Chuck_ArrayVec2 *)(*sp);
        // get value
        v2 = (*(t_CKVEC2 *)(sp+1));
        // append
        arr->push_back( v2 );
    }
    else if( m_val == kindof_VEC3 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec3 * arr = (Chuck_ArrayVec3 *)(*sp);
        // get value
        v3 = (*(t_CKVEC3 *)(sp+1));
        // append
        arr->push_back( v3 );
    }
    else if( m_val == kindof_VEC4 ) // 1.3.5.3
    {
        // get array
        Chuck_ArrayVec4 * arr = (Chuck_ArrayVec4 *)(*sp);
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
    EM_exception(
        "NullPointer: (array append) on line[%lu] in shred[id=%lu:%s]",
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
        if( m_kind == kindof_INT ) { push_( sp, *((t_CKINT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_FLOAT ) { push_float( sp, *((t_CKFLOAT *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0)
        else if( m_kind == kindof_VEC2 ) { push_vec2( sp, *((t_CKVEC2 *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0) | in this context, vec2 = complex = polar
        else if( m_kind == kindof_VEC3 ) { push_vec3( sp, *((t_CKVEC3 *)data) ); } // 1.3.5.3
        else if( m_kind == kindof_VEC4 ) { push_vec4( sp, *((t_CKVEC4 *)data) ); } // 1.3.5.3
        else assert( FALSE );
    }

    return;

error:
    // we have a problem
    EM_exception(
        "NullPointer: on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "NullPointer: on line[%lu] in shred[id=%lu:%s]",
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
        else if( m_kind == kindof_VEC2 ) { push_vec2( sp, *((t_CKVEC2 *)data) ); } // ISSUE: 64-bit (fixed 1.3.1.0) | in this context, vec2 = complex = polar
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
        else if( m_kind == kindof_VEC2 ) { push_vec2( sp, *((t_CKVEC2 *)m_addr) ); } // ISSUE: 64-bit (fixed 1.3.1.0) | in this context, vec2 = complex = polar
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

    // 1.4.1.0 (ge): leave the base type on the operand stack
    // commented out: pop the type pointer
    // pop_( sp, 1 );

    // push the address
    push_( sp, (t_CKUINT)(m_func) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: access the first field of a complex/polar/vec3/vec4 value
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_First::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // pointer to var (regardless of value type)
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push var addr or value?
        if( m_emit_addr ) {
            // push addr (NOTE this works for complex/polar/vec2/vec3/vec4)
            push_( sp, (t_CKUINT)(&((*(t_CKCOMPLEX **)sp)->re)) );
        } else {
            // push value
            push_float( sp, (*(t_CKCOMPLEX **)sp)->re );
        }
    }
    else
    {
        // make sure
        assert( m_emit_addr == FALSE );

        // which kind of datatype?
        switch( m_kind )
        {
            // get corresponding value, pop that value, push field
            case kindof_VEC2:
            {
                t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->x );
            }
            break;

            case kindof_VEC3:
            {
                t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->x );
            }
            break;

            case kindof_VEC4:
            {
                t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->x );
            }
            break;

            // shouldn't get here
            default: assert( FALSE ); break;
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: access the second field of a complex/polar/vec2/vec3/vec4 value
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Second::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // pointer to var (regardless of value type)
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // emit as var address or as value?
        if( m_emit_addr ) {
            // push addr (works for complex/polar/vec2/vec3/vec4)
            push_( sp, (t_CKUINT)(&((*(t_CKCOMPLEX **)sp)->im)) );
        } else {
            // push value
            push_float( sp, (*(t_CKCOMPLEX **)sp)->im );
        }
    }
    else
    {
        // make sure
        assert( m_emit_addr == FALSE );

        // which kind of datatype?
        switch( m_kind )
        {
            // get corresponding value, pop that value, push field
            case kindof_VEC2:
            {
                t_CKVEC2 *& sp = (t_CKVEC2 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->y );
            }
            break;

            case kindof_VEC3:
            {
                t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->y );
            }
            break;

            case kindof_VEC4:
            {
                t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->y );
            }
            break;

            // shouldn't get here
            default: assert( FALSE ); break;
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: access the third field of a vec3/vec4 value
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Third::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // pointer
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // var addr or value?
        if( m_emit_addr ) {
            // push addr
            push_( sp, (t_CKUINT)(&((*(t_CKVEC3 **)sp)->z)) );
        } else {
            // push value
            push_float( sp, (*(t_CKVEC3 **)sp)->z );
        }
    }
    else
    {
        // make sure
        assert( m_emit_addr == FALSE );

        // which kind of datatype?
        switch( m_kind )
        {
            // get corresponding value, pop that value, push field
            case kindof_VEC3:
            {
                t_CKVEC3 *& sp = (t_CKVEC3 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->z );
            }
            break;

            case kindof_VEC4:
            {
                t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
                pop_( sp, 1 ); push_float( sp, sp->z );
            }
            break;

            // shouldn't get here
            default: assert( FALSE ); break;
        }
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: access the fourth field of a vec4 value
//-----------------------------------------------------------------------------
void Chuck_Instr_Dot_Cmp_Fourth::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // reg contains pointer to complex elsewhere
    if( m_is_mem )
    {
        // pointer
        t_CKUINT *& sp = (t_CKUINT *&)shred->reg->sp;
        // pop
        pop_( sp, 1 );
        // push var addr or value?
        if( m_emit_addr ) {
            // push addr
            push_( sp, (t_CKUINT)(&((*(t_CKVEC4 **)sp)->w)) );
        } else {
            // push value
            push_float( sp, (*(t_CKVEC4 **)sp)->w );
        }
    }
    else
    {
        // make sure...
        assert( m_emit_addr == FALSE );
        assert( m_kind == kindof_VEC4 );

        // get vec4 value, pop that value, push field
        t_CKVEC4 *& sp = (t_CKVEC4 *&)shred->reg->sp;
        pop_( sp, 1 );
        push_float( sp, sp->w );
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
// desc: cast vec2 to vec3 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec2tovec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // zero pad for z
    push_( sp, 0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: cast vec2 to vec4 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec2tovec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // zero pad for z, w
    push_( sp, 0 );
    push_( sp, 0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: cast vec3 to vec4 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec3tovec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // zero pad it
    push_( sp, 0 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: cast vec3 to vec2 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec3tovec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // remove z component
    pop_( sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: cast vec4 to vec2 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec4tovec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // remove z and w components
    pop_( sp, 2 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: cast vec4 to vec3 on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_vec4tovec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKFLOAT *& sp = (t_CKFLOAT *&)shred->reg->sp;
    // remove w component
    pop_( sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: "cast" Object to a string, using the Object's method .toString()
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
    // get toString from it (added 1.3.0.0 -- Chuck_DL_Api::instance())
    object_toString( obj, NULL, &RETURN, vm, shred, Chuck_DL_Api::instance() );
    // get pointer
    Chuck_String * str = RETURN.v_string;
    // set it
    push_( sp, (t_CKUINT)str );
}




//-----------------------------------------------------------------------------
// name: params()
// desc: params for printing
//-----------------------------------------------------------------------------
const char * Chuck_Instr_Cast_Runtime_Verify::params() const
{
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "%s $ %s", m_from->c_name(), m_to->c_name() );
    return buffer;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: type cast runtime verification
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_Runtime_Verify::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    // stack pointer
    t_CKUINT * sp = (t_CKUINT *)shred->reg->sp;
    // cast to object
    Chuck_Object * obj = (Chuck_Object *)(*(sp-1));

    // NULL is ok
    if( !obj ) return;

    // actual type of object
    Chuck_Type * actualType = obj->type_ref;
    // check against to type to cast to
    if( !isa(actualType,m_to) ) goto invalid_cast;

    // got here? ok
    return;

invalid_cast:
    // we have a problem
    EM_exception( "RuntimeCastIncompatible: in shred[id=%lu:%s]...",
                  shred->xid, shred->name.c_str()  );
    // print code context
    if( m_codeWithFormat != "" ) CK_FPRINTF_STDERR( m_codeWithFormat.c_str(), TC::orange(actualType->c_name(),true).c_str(), TC::orange(m_to->c_name(),true).c_str() );
    // done
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
    EM_exception(
        "NullPointer: (string op) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

invalid_op:
    // we have a problem
    EM_exception(
        "InvalidStringOp: '%s' on line[%lu] in shred[id=%lu:%s]",
        op2str((ae_Operator)m_val), m_linepos, shred->xid, shred->name.c_str() );
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
// desc: link one UGen to another; except two UGen pointers on reg stack
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
    EM_exception(
        "NullPointer: (UGen link) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );

    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: linking array of UGens
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Array_Link::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_Object **& sp = (Chuck_Object **&)shred->reg->sp;
    Chuck_Object * src_obj = NULL, * dst_obj = NULL;
    t_CKINT num_out = 1, num_in = 1;

    // pop
    pop_( sp, 2 );
    // check for null
    if( !*(sp+1) || !(*sp) ) goto null_pointer;

    src_obj = *sp;
    dst_obj = (*(sp + 1));

    // go for it
    // 1.5.2.2 (ge) semantic update: default num_out and num_in to 1,
    // only update if actually an array; let ugen->add() sort out channels
    // 1.5.2.3 (pr-lab, ge, nick, eito, terry) un-break case of array => non-mono ugen
    num_out = ugen_generic_num_out( src_obj, m_srcIsArray );
    num_in = ugen_generic_num_in( dst_obj, m_dstIsArray );

    // check for different combos, similar to ugen->add() | 1.5.2.2 (ge)
    if( num_out == 1 && num_in == 1 )
    {
        Chuck_UGen * dst_ugen = ugen_generic_get_dst( dst_obj, 0, m_dstIsArray );
        Chuck_UGen * src_ugen = ugen_generic_get_src( src_obj, 0, m_srcIsArray );
        if( dst_ugen == NULL || src_ugen == NULL ) goto null_pointer;
        dst_ugen->add( src_ugen, m_isUpChuck );
    }
    else if( num_out == 1 && num_in > 1 )
    {
        Chuck_UGen * src_ugen = ugen_generic_get_src( src_obj, 0, m_srcIsArray );
        for( t_CKINT i = 0; i < num_in; i++ )
        {
            Chuck_UGen * dst_ugen = ugen_generic_get_dst( dst_obj, i, m_dstIsArray );
            if( dst_ugen == NULL || src_ugen == NULL ) goto null_pointer;
            dst_ugen->add( src_ugen, m_isUpChuck );
        }
    }
    else if( num_out > 1 && num_in == 1 )
    {
        Chuck_UGen * dst_ugen = ugen_generic_get_dst( dst_obj, 0, m_dstIsArray );
        for( t_CKINT i = 0; i < num_out; i++ )
        {
            Chuck_UGen * src_ugen = ugen_generic_get_src( src_obj, i, m_srcIsArray );
            if( dst_ugen == NULL || src_ugen == NULL ) goto null_pointer;
            dst_ugen->add( src_ugen, m_isUpChuck );
        }
    }
    else if( num_out > 1 && num_in > 1 )
    {
        // find greater
        t_CKINT greater = ck_max( num_out, num_in );
        // map one to one, up to greater (lesser should modulo)
        for( t_CKINT i = 0; i < greater; i++ )
        {
            Chuck_UGen * src_ugen = ugen_generic_get_src( src_obj, i, m_srcIsArray );
            Chuck_UGen * dst_ugen = ugen_generic_get_dst( dst_obj, i, m_dstIsArray );
            if( dst_ugen == NULL || src_ugen == NULL ) goto null_pointer;
            dst_ugen->add( src_ugen, m_isUpChuck );
        }
    }

    // push the second
    push_( sp, *(sp + 1) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (UGen array link) on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "LoopCounterError: on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "LoopCounterError: on line[%lu] in shred[id=%lu:%s]",
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




//-----------------------------------------------------------------------------
// name: struct Chuck_Instr_ForEach_Inc_And_Branch
// desc: for( VAR : ARRAY ) increment VAR, test against ARRAY size; branch
//
// pre-condition: expects three items on reg stack (see below)
// post-condition: leaves no values on reg stack
//-----------------------------------------------------------------------------
void Chuck_Instr_ForEach_Inc_And_Branch::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKUINT *& reg_sp = (t_CKUINT *&)shred->reg->sp;

    // --------
    // output of VAR alloc (e.g., int or float or other) -- but always an UINT
    // ARRAY to iterate over
    // implicit loop counter
    // --------
    // ^ assume three things on the reg stack

    // compute stack offset
    // t_CKUINT offset = (m_dataSize + sz_INT + sz_INT) / sz_INT;
    t_CKUINT offset = (sz_INT + sz_INT + sz_INT) / sz_INT;

    // pop
    pop_( reg_sp, offset );
    // set
    t_CKUINT *& sp = reg_sp;

    // get var address
    t_CKUINT * pVar = (t_CKUINT *)(*sp);
    // get array pointer
    Chuck_Array * array = (Chuck_Array *)(*(sp+1));
    // loop counter pointer
    t_CKINT ** pCounter = (t_CKINT **)(sp+2);
    // counter
    t_CKINT * counter = *(pCounter);

    // branch if either NULL array, or counter reached array size
    if( !array || val_(counter) >= array->size() )
    {
        shred->next_pc = m_jmp;
    }
    else
    {
        // set pVar to current element
        switch( m_dataKind )
        {
            case kindof_INT:
            {
                // cast to specific array type
                Chuck_ArrayInt * arr = (Chuck_ArrayInt *)array;
                // is object RELEASE
                if( arr->contains_objects() && *pVar)
                {
                    // release ref, since for-each loops don't auto-release every iteration
                    // (only at the end or if return encountered)
                    ((Chuck_VM_Object *)(*pVar))->release();
                }
                // get element
                arr->get( *counter, pVar );
                // is object ADD_REF
                if( arr->contains_objects() && *pVar)
                {
                    // add ref, as this will be cleaned up at end of scope, hopefully
                    ((Chuck_VM_Object *)(*pVar))->add_ref(); 
                }
                break;
            }
            case kindof_FLOAT:
            {
                // cast to specific array type
                Chuck_ArrayFloat * arr = (Chuck_ArrayFloat *) array;
                // get element
                arr->get( *counter, (t_CKFLOAT *)pVar );
                break;
            }
            case kindof_VEC2:
            {
                // cast to specific array type
                Chuck_ArrayVec2 * arr = (Chuck_ArrayVec2 *) array;
                // get element
                arr->get( *counter, (t_CKVEC2 *)pVar );
                break;
            }
            case kindof_VEC3:
            {
                // cast to specific array type
                Chuck_ArrayVec3 * arr = (Chuck_ArrayVec3 *) array;
                // get element
                arr->get( *counter, (t_CKVEC3 *)pVar );
                break;
            }
            case kindof_VEC4:
            {
                // cast to specific array type
                Chuck_ArrayVec4 * arr = (Chuck_ArrayVec4 *) array;
                // get element
                arr->get( *counter, (t_CKVEC4 *)pVar );
                break;
            }
            default:
            {
                // shouldn't get here
                assert( FALSE );
                break;
            }
        }

        // increment counter
        (*(*pCounter))++;
    }
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
    EM_exception(
        "NullPointer: (IO input int) on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "NullPointer: (IO input float) on line[%lu] in shred[id=%lu:%s]",
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
    EM_exception(
        "NullPointer: (IO input string) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: insert int value into output stream
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
    EM_exception(
        "NullPointer: (IO output int) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: insert float value into output stream
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
    EM_exception(
        "NullPointer: (IO output float) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.0
// desc: insert complex value into output stream
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_complex::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;

    // pop the value
    pop_( sp, 1 + (sz_COMPLEX / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;

    // write the value
    (*ppIO)->write( *((t_CKCOMPLEX *)(sp+1)) );

    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (IO output complex) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.0
// desc: insert complex value into output stream
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_polar::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;

    // pop the value
    pop_( sp, 1 + (sz_POLAR / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;

    // write the value
    (*ppIO)->write( *((t_CKPOLAR *)(sp+1)) );

    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (IO output polar) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.7
// desc: insert vec2 value into output stream
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_vec2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;

    // pop the value
    pop_( sp, 1 + (sz_VEC2 / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;

    // write the value
    (*ppIO)->write( *((t_CKVEC2 *)(sp+1)) );

    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (IO output vec2) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.0
// desc: insert vec3 value into output stream
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_vec3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;

    // pop the value
    pop_( sp, 1 + (sz_VEC3 / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;

    // write the value
    (*ppIO)->write( *((t_CKVEC3 *)(sp+1)) );

    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (IO output vec3) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute() | 1.5.1.0
// desc: insert vec4 value into output stream
//-----------------------------------------------------------------------------
void Chuck_Instr_IO_out_vec4::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKINT *& sp = (t_CKINT *&)shred->reg->sp;

    // pop the value
    pop_( sp, 1 + (sz_VEC4 / sz_INT) ); // ISSUE: 64-bit (fixed 1.3.1.0)

    // ISSUE: 64-bit
    // the IO
    Chuck_IO **& ppIO = (Chuck_IO **&)sp;

    // check it
    if( *(ppIO) == NULL ) goto null_pointer;

    // write the value
    (*ppIO)->write( *((t_CKVEC4 *)(sp+1)) );

    // push the IO
    push_( sp, (t_CKINT)(*(ppIO)) );

    return;

null_pointer:
    // we have a problem
    EM_exception(
        "NullPointer: (IO output vec4) on line[%lu] in shred[id=%lu:%s]",
        m_linepos, shred->xid, shred->name.c_str() );
    goto done;

done:
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: insert string into output stream
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
    EM_exception(
        "NullPointer: (IO output string) on line[%lu] in shred[id=%lu:%s]",
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
        Chuck_Object * obj = ((Chuck_Object *)*(sp-1));
        if( !isa( m_type_ref, vm->env()->ckt_string ) || *(sp-1) == 0 )
        {
            // print it
            if( isa( m_type_ref, vm->env()->ckt_object ) )
            {
                if( obj == NULL )
                {
                    CK_FPRINTF_STDERR( "null :(%s)\n", m_type_ref->c_name() );
                }
                else
                {
                    CK_FPRINTF_STDERR( "0x%lx :(%s|refcount=%d)\n", *(sp-1), m_type_ref->c_name(), obj->m_ref_count );
                }
            }
            else
                CK_FPRINTF_STDERR( "%d :(%s)\n", *(sp-1), m_type_ref->c_name() );
        }
        else
        {
            CK_FPRINTF_STDERR( "\"%s\" :(%s)\n", ((Chuck_String *)obj)->str().c_str(), m_type_ref->c_name() );
        }
    }
    else if( m_type_ref->size == sz_FLOAT ) // ISSUE: 64-bit (fixed 1.3.1.0)
    {
        t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
        // print it
        CK_FPRINTF_STDERR( "%f :(%s)\n", *(sp-1), m_type_ref->c_name() );
    }
    else if( m_type_ref->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
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
            CK_FPRINTF_STDERR( "%%(%.4f,%.4f*pi) :(%s)\n", *(sp-2), *(sp-1)/CK_ONE_PI, m_type_ref->c_name() );
        }
        else if( m_type_ref->xid == te_vec2 ) // 1.5.1.7
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)shred->reg->sp;
            // print it
            CK_FPRINTF_STDERR( "@(%.4f,%.4f) :(%s)\n", *(sp-2), *(sp-1), m_type_ref->c_name() );
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
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "(%s)", m_type_ref->c_name() );
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
            if( !isa( type, vm->env()->ckt_string ) || *(sp) == 0 )
            {
                if( isa( type, vm->env()->ckt_object ) )
                {
                    Chuck_Object * obj = ((Chuck_Object *)(*sp));
                    // print it
                    if( *(sp) == 0 )
                        CK_FPRINTF_STDERR( "null " );
                    else
                        CK_FPRINTF_STDERR( "0x%lx (refcount=%d) ", *(sp), obj->m_ref_count );
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
        else if( type->size == sz_VEC2 ) // ISSUE: 64-bit (fixed 1.3.1.0)
        {
            t_CKFLOAT * sp = (t_CKFLOAT *)the_sp;
            if( type->xid == te_complex )
                // print it
                CK_FPRINTF_STDERR( "#(%.4f,%.4f) ", *(sp), *(sp+1) );
            else if( type->xid == te_polar )
                // print it
                CK_FPRINTF_STDERR( "%%(%.4f,%.4f*pi) ", *(sp), *(sp+1)/CK_ONE_PI );
            else if( type->xid == te_vec2 ) // 1.5.1.7
                // print it
                CK_FPRINTF_STDERR( "@(%.4f,%.4f) ", *(sp), *(sp+1) );

            the_sp += sz_VEC2; // ISSUE: 64-bit (fixed 1.3.1.0)
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
            CK_FPRINTF_STDERR( "..." );
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
    static char buffer[CK_PRINT_BUF_LENGTH];
    snprintf( buffer, CK_PRINT_BUF_LENGTH, "( many types )" );
    return buffer;
}


// actions to throw exception
void ck_throw_exception( Chuck_VM_Shred * shred, const char * name )
{
    // we have a problem
    EM_exception(
        "%s: on line[%lu] in shred[id=%lu:%s]",
        name, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


// actions to throw exception with int
void ck_throw_exception( Chuck_VM_Shred * shred, const char * name, t_CKINT desc )
{
    // we have a problem
    EM_exception(
        "%s: '%li' on line[%lu] in shred[id=%lu:%s]",
        name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}


// actions to throw exception with float
void ck_throw_exception( Chuck_VM_Shred * shred, const char * name, t_CKFLOAT desc )
{
    // we have a problem
    EM_exception(
        "%s: '%f' on line[%lu] in shred[id=%lu:%s]",
        name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}

// actions to throw exception with string desc
void ck_throw_exception( Chuck_VM_Shred * shred, const char * name, const char * desc )
{
    // we have a problem
    EM_exception(
        "%s: %s on line[%lu] in shred[id=%lu:%s]",
        name, desc, shred->instr[shred->pc]->m_linepos, shred->xid, shred->name.c_str() ); //, shred->pc );
    // do something!
    shred->is_running = FALSE;
    shred->is_done = TRUE;
}
