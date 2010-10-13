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
// file: chuck_instr.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#include <math.h>
#include <limits.h>
#include <typeinfo>
using namespace std;

#include "chuck_instr.h"
#include "chuck_vm.h"
#include "chuck_ugen.h"
#include "chuck_bbq.h"
#include "chuck_dl.h"
#include "chuck_type.h"




// define SP offset
#define push_( sp, val )         *(sp) = (val); (sp)++
#define push_uint( sp, val )     *((uint*&)sp) = val; ((uint*&)sp)++
#define pop_( sp, n )            sp -= (n)
#define val_( sp )               *(sp)




//-----------------------------------------------------------------------------
// name: name()
// desc: ...
//-----------------------------------------------------------------------------
const char * Chuck_Instr::name() const
{
     return typeid(*this).name();
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Inc_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    sint *& reg_sp = (sint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // increment value
    (*(reg_sp-1))++;
    // copy value into mem stack
    *( (sint *)(mem_sp + *(reg_sp)) ) = *(reg_sp-1);
}





//-----------------------------------------------------------------------------
// name: class Chuck_Instr_Dec_int
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dec_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    sint *& reg_sp = (sint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // decrement value
    (*(reg_sp-1))--;
    // copy value into mem stack
    *( (sint *)(mem_sp + *(reg_sp)) ) = *(reg_sp-1);
}




//-----------------------------------------------------------------------------
// name: exexute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Complement_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    (*(sp-1)) = ~(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) % val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) / val_(sp+1) );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Inc_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // increment value
    (*(reg_sp-1))++;
    // copy value into mem stack
    *( (uint *)(mem_sp + *(reg_sp)) ) = *(reg_sp-1);
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Dec_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // decrement value
    (*(reg_sp-1))--;
    // copy value into mem stack
    *( (uint *)(mem_sp + *(reg_sp)) ) = *(reg_sp-1);
}




//-----------------------------------------------------------------------------
// name: exexute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Complement_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    (*sp) = ~(*sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) % val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) / val_(sp+1) );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) / val_(sp+1) );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, (float)fmod( val_(sp), val_(sp+1) ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) + val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Minus_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) - val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Times_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) * val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Divide_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) / val_(sp+1) );
}





//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mod_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp; 
    pop_( sp, 2 );
    push_( sp, fmod( val_(sp), val_(sp+1) ) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Imm::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Imm2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& reg_sp = (double *&)shred->reg->sp;

    // push val into reg stack
    push_( reg_sp, m_val );
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
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Deref::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    if( m_size == 4 )
    {
        uint *& reg_sp = (uint *&)shred->reg->sp;
        push_( reg_sp, *((uint *)m_val) );
    }
    else
    {
        double *& reg_sp = (double *&)shred->reg->sp;
        push_( reg_sp, *((double *)m_val) );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // push mem stack content into reg stack
    push_( reg_sp, *((uint *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Push_Mem2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    double *& reg_sp = (double *&)shred->reg->sp;

    // push mem stack content into reg stack
    push_( reg_sp, *((double *)(mem_sp + m_val)) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Mem::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *((uint *)(mem_sp + *(reg_sp+1) )) = *reg_sp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack 
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Reg_Pop_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& reg_sp = (double *&)shred->reg->sp;

    // pop word from reg stack 
    pop_( reg_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Push_Imm::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    
    // pop word from reg stack 
    push_( mem_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Push_Imm2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& mem_sp = (double *&)shred->mem->sp;
    
    // pop word from reg stack 
    push_( mem_sp, m_val );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Pop_Word::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    
    // pop word from reg stack 
    pop_( mem_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Mem_Pop_Word2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& mem_sp = (double *&)shred->mem->sp;
    
    // pop word from reg stack 
    pop_( mem_sp, 1 );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
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
    sint *& sp = (sint *&)shred->reg->sp;
    (*(sp-1)) = !(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    (*(sp-1)) = -(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    sint *& sp_int = (sint *&)shred->reg->sp;
    (*(sp_int-1)) = -((sint)(*(sp-1)));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    (*(sp-1)) = -(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Negate_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    (*(sp-1)) = -(*(sp-1));
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) < val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Gt_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) > val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Le_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) <= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Ge_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) >= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Eq_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Neq_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) != val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) < val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Gt_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) > val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Le_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) <= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Ge_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) >= val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Eq_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) == val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Neq_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) != val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Branch_Lt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
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
    double *& sp = (double *&)shred->reg->sp;
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
    double *& sp = (double *&)shred->reg->sp;
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
    double *& sp = (double *&)shred->reg->sp;
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
    double *& sp = (double *&)shred->reg->sp;
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
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    if( val_(sp) != val_(sp+1) )
        shred->next_pc = m_jmp;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_And::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) & val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Or::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) | val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Xor::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) ^ val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Right::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) >> val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Binary_Shift_Left::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) << val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) >= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) != val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) >= val_(sp+1) );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_uint::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) != val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) >= val_(sp+1) );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) != val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Lt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) < val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Gt_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) > val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Le_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) <= val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Ge_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) >= val_(sp+1) );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Eq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) == val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Neq_double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    pop_( sp, 2 );
    push_uint( sp, val_(sp) != val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_And::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) && val_(sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Or::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    pop_( sp, 2 );
    push_( sp, val_(sp) || val_(sp+1) );
}




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




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *( (uint *)(mem_sp + *(reg_sp+1)) ) = *reg_sp;

    push_( reg_sp, *reg_sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 3 );
    // copy popped value into mem stack
    *( (double *)(mem_sp + *(reg_sp+2)) ) = *(double *)reg_sp;

    double *& sp_double = (double *&)reg_sp;
    push_( sp_double, *sp_double );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign_Deref::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *( (uint *)(*(reg_sp+1)) ) = *reg_sp;

    push_( reg_sp, *reg_sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign_Deref2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 3 );
    // copy popped value into mem stack
    *( (double *)(*(reg_sp+2)) ) = *(double *)reg_sp;

    double *& sp_double = (double *&)reg_sp;
    push_( sp_double, *sp_double );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // release any previous reference
    Chuck_VM_Object ** obj = (Chuck_VM_Object **)(mem_sp + *(reg_sp+1));
    // if( *obj ) (*obj)->release();
    // copy popped value into mem stack
    *obj = *(Chuck_VM_Object **)reg_sp;
    // add reference
    (*(Chuck_VM_Object **)reg_sp)->add_ref();

    push_( reg_sp, *reg_sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Release_Object::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;
    Chuck_VM_Object * obj = NULL;

    // pop word from reg stack
    pop_( reg_sp, 1 );
    // copy popped value into mem stack
    obj = *( (Chuck_VM_Object **)(mem_sp + *(reg_sp)) );
    // release
    obj->release();
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign_Object_Deref::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *( (uint *)(*(reg_sp+1)) ) = *reg_sp;
    // add reference
    ( *((Chuck_VM_Object **)reg_sp) )->add_ref();

    push_( reg_sp, *reg_sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Chuck_Assign_Object2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    BYTE__ *& mem_sp = (BYTE__ *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;
    
    // pop word from reg stack
    pop_( reg_sp, 2 );
    // copy popped value into mem stack
    *( (uint *)(mem_sp + *reg_sp) ) = *(reg_sp+1);
    // add reference
    ( *((Chuck_VM_Object **)(reg_sp+1)) )->add_ref();
    
    push_( reg_sp, *(reg_sp+1) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop word
    pop_( reg_sp, 2 );
    Chuck_VM_Code * func = (Chuck_VM_Code *)*reg_sp;
    uint local_depth = *(reg_sp+1);
    local_depth = ( local_depth >> 2 ) + ( local_depth & 0x3 ? 1 : 0 );
    uint stack_depth = ( func->stack_depth >> 2 ) + ( func->stack_depth & 0x3 ? 1 : 0 );
    uint prev_stack = ( *(mem_sp-1) >> 2 ) + ( *(mem_sp-1) & 0x3 ? 1 : 0 );

    // jump the sp
    mem_sp += prev_stack + local_depth;
    // push the prev stack
    push_( mem_sp, prev_stack + local_depth );
    // push the current function
    push_( mem_sp, (uint)shred->code );
    // push the pc
    push_( mem_sp, (uint)(shred->pc + 1) );
    // push the stack depth
    push_( mem_sp, stack_depth );
    // set the pc to 0
    shred->next_pc = 0;
    // set the code
    shred->code = func;
    shred->instr = func->instr;

    if( stack_depth )
    {
        BYTE__ *& mem_sp2 = (BYTE__ *&)mem_sp;
        BYTE__ *& reg_sp2 = (BYTE__ *&)reg_sp;

        // pop the arguments
        pop_( reg_sp2, stack_depth << 2 );
        // push the arguments
        memcpy( mem_sp2, reg_sp2, stack_depth << 2 );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;
    Chuck_DL_Return retval;

    pop_( reg_sp, 3 );
    f_ck_func f = (f_ck_func)(*(reg_sp+1));
    uint stack_size = ((*reg_sp) >> 2) + ( *reg_sp & 0x3 ? 1 : 0 );
    uint local_depth = *(reg_sp+2) + *(mem_sp-1);
    uint push = ((local_depth) >> 2) + ( local_depth & 0x3 ? 1 : 0 );
    reg_sp -= stack_size;
    mem_sp += push;
    uint * sp = reg_sp;
    uint * mem = mem_sp;
    // copy to args
    for( uint i = 0; i < stack_size; i++ )
        *mem++ = *sp++;
    // call the function
    f( mem_sp, &retval );
    mem_sp -= push;
    // push the return args
    push_( reg_sp, retval.v_uint );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call3::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;
    Chuck_DL_Return retval;

    pop_( reg_sp, 3 );
    f_ck_func f = (f_ck_func)(*(reg_sp+1));
    uint stack_size = ((*reg_sp) >> 2) + ( *reg_sp & 0x3 ? 1 : 0 );
    uint local_depth = *(reg_sp+2) + *(mem_sp-1);
    uint push = (local_depth >> 2) + ( local_depth & 0x3 ? 1 : 0 );
    reg_sp -= stack_size;
    mem_sp += push;
    uint * sp = reg_sp;
    uint * mem = mem_sp;
    // copy to args
    for( uint i = 0; i < stack_size; i++ )
        *mem++ = *sp++;
    // call the function
    f( mem_sp, &retval );
    mem_sp -= push;
    // push the return args
    double *& sp_double = (double *&)reg_sp;
    push_( sp_double, retval.v_float );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Call0::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;
    Chuck_DL_Return retval;

    pop_( reg_sp, 3 );
    f_ck_func f = (f_ck_func)(*(reg_sp+1));
    uint stack_size = ((*reg_sp) >> 2) + ( *reg_sp & 0x3 ? 1 : 0 );
    uint local_depth = *(reg_sp+2) + *(mem_sp-1);
    uint push = (local_depth >> 2) + ( local_depth & 0x3 ? 1 : 0 );
    reg_sp -= stack_size;
    mem_sp += push;
    uint * sp = reg_sp;
    uint * mem = mem_sp;
    // copy to args
    for( uint i = 0; i < stack_size; i++ )
        *mem++ = *sp++;
    // call the function
    f( mem_sp, &retval );
    mem_sp -= push;
    // push the return args
    // push_( reg_sp, retval.v_uint );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Func_Return::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& mem_sp = (uint *&)shred->mem->sp;
    uint *& reg_sp = (uint *&)shred->reg->sp;

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
    uint *& reg_sp = (uint *&)shred->reg->sp;

    // pop the stack
    pop_( reg_sp, 1 );
    // get the code
    Chuck_VM_Code * code = *(Chuck_VM_Code **)reg_sp;
    // spork it
    Chuck_VM_Shred * sh = vm->spork( code, shred );
    // copy args
    if( m_val )
    {
        pop_( shred->reg->sp, m_val );
        memcpy( sh->reg->sp, shred->reg->sp, m_val );
        sh->reg->sp += m_val;
    }
    // push the stack
    push_( reg_sp, sh->id );
}




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
        fprintf( stderr, 
            "[chuck](VM): Exception DestTimeNegative: '%.6f'\n", *sp );
        // do something!
        shred->is_running = FALSE;

        return;
    }

    // shredule the shred
    vm->shreduler()->shredule( shred, *sp );
    // suspend
    shred->is_running = FALSE;

    push_( sp, *sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Add_dur_time::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    t_CKTIME *& p = (t_CKTIME *&)shred->reg->sp;
    pop_( p, 1 );
    t_CKTIME t = *p;

    t_CKDUR *& sp = (t_CKDUR *&)shred->reg->sp;
    pop_( sp, 1 );
    t_CKDUR d = *sp;

    push_( sp, (t_CKTIME)( d + (t_CKDUR)t ) );
}



//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Midi_Out::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    // pop word
    pop_( sp, m_val ? 1 : 0 );

    // send the word as midi msg
    MidiOut * out = vm->bbq()->midi_out( m_val ? *(sp) : 0 );
    if( !out && vm->bbq()->out_count( m_val ? *(sp) : 0 ) < 2 )
        fprintf( stderr, "[chuck](VM): cannot open MIDI output device # '%i'...\n",
                 m_val ? *(sp) : 0 );
                 
    // push anything?
    push_( sp, (uint)out );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Midi_Out_Go::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    MidiMsg msg;
    uint *& sp = (uint *&)shred->reg->sp;

    // pop word
    pop_( sp, 2 );

    uint v = *( m_val ? sp+1 : sp );
    msg.data[0] = (v >> 16) & 0xff;
    msg.data[1] = (v >> 8) & 0xff;
    msg.data[2] = (v) & 0xff;
    
    // send the word as midi msg
    MidiOut * out = *(MidiOut **)( m_val ? (sp) : sp+1 );
    if( out ) out->send( &msg );

    // push anything?
    push_( sp, (uint)out );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Midi_In::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    if( m_val ) pop_( sp, 1 );
    MidiIn * in = vm->bbq()->midi_in( m_val ? *(sp) : 0 );
    if( !in && vm->bbq()->in_count( m_val ? *(sp) : 0 ) < 2 )
    {
        fprintf( stderr, "[chuck](VM): cannot open MIDI input device '%i'...\n",
                 m_val ? *(sp) : 0 );
    }
    
    push_( sp, (uint)in );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Midi_In_Go::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    MidiMsg msg;
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 1 );
    MidiIn * in = *(MidiIn **)sp;
    if( !in )
    {
        push_( sp, 0 );
        return;
    }

    if( in->recv( &msg ) )
    {
        // msg2.data[0] = msg.data[3];
        // msg2.data[1] = msg.data[2];
        // msg2.data[2] = msg.data[1];
        // msg2.data[3] = msg.data[0];
        push_( sp, *((uint *)&msg) );
    }
    else
    {
        push_( sp, 0 );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_ADC::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;
    push_( reg_sp, (uint)vm->m_adc );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_DAC::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;
    push_( reg_sp, (uint)vm->m_dac );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Bunghole::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& reg_sp = (uint *&)shred->reg->sp;
    push_( reg_sp, (uint)vm->m_bunghole );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Link::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    Chuck_UGen **& sp = (Chuck_UGen **&)shred->reg->sp;

    pop_( sp, 2 );
    (*(sp + 1))->add( *sp );
    push_( sp, *(sp + 1) );
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




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Alloc::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    Chuck_UGen_Info * info = NULL;
    Chuck_UGen * ugen = NULL;

    pop_( sp, 1 );
    info = (Chuck_UGen_Info *)*(sp);
    ugen = new Chuck_UGen;
    // copy the info over
    ugen->ctor = info->ctor;
    ugen->dtor = info->dtor;
    ugen->tick = info->tick;
    ugen->pmsg = info->pmsg;
    ugen->m_max_src = info->max_src;
    // call the constructor
    ugen->state = ugen->ctor ? ugen->ctor( shred->now ) : NULL ;

    // setup the reference with the shred
    ugen->shred = shred;
    shred->add( ugen );
    push_( sp, (uint)ugen );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_DeAlloc::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Ctrl::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    
    pop_( sp, 4 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp+1);
    f_ctrl ctrl = (f_ctrl)*(sp+2);
    f_cget cget = (f_cget)*(sp+3);
    // call ctrl
    ctrl( shred->now, ugen->state, (void *)sp );
    if( cget ) cget( shred->now, ugen->state, (void *)sp );
    // push the new value
    push_( sp, *sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    f_cget cget = (f_cget)*(sp+1);
    // call cget
    cget( shred->now, ugen->state, (void *)sp );
    // push the new value
    push_( sp, *sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Ctrl2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    
    pop_( sp, 4 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp+1);
    f_ctrl ctrl = (f_ctrl)*(sp+2);
    f_cget cget = (f_cget)*(sp+3);
    // call ctrl
    pop_( sp, 1 );
    ctrl( shred->now, ugen->state, (void *)sp );
    if( cget ) cget( shred->now, ugen->state, (void *)sp );
    // push the new value
    ((double *&)shred->reg->sp)++;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    f_cget cget = (f_cget)*(sp+1);
    // call cget
    cget( shred->now, ugen->state, (void *)sp );
    // push the new value
    ((double *&)shred->reg->sp)++;
}




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
void Chuck_Instr_UGen_Ctrl_Op::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    
    pop_( sp, 4 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp+1);
    ugen->m_op = *(sint *)sp;
    // push the new value
    push_( sp, *sp);
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet_Op::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    // push the new value
    push_( sp, ugen->m_op );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_Ctrl_Gain::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    
    // HACK: this won't work for 64-bit long
    ((Chuck_UGen *)*(sp-3))->m_gain = (float)*(double *)(sp-5);
    pop_( sp, 5 );

    // push the new value
    ((double *&)shred->reg->sp)++;
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet_Gain::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    // push the new value
    double *& sp_double = (double *&)shred->reg->sp;
    push_( sp_double, (double)ugen->m_gain );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_UGen_CGet_Last::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;

    pop_( sp, 2 );
    Chuck_UGen * ugen = (Chuck_UGen *)*(sp);
    // push the new value
    double *& sp_double = (double *&)shred->reg->sp;
    push_( sp_double, (double)ugen->m_current );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_DLL_Load::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    sint retval = FALSE;
    Chuck_DLL * dll = NULL;
    pop_( sp, 2 );
    
    // load the DLL into the vm
    dll = vm->dll_load( (const char *)(*sp) );
    // load the DLL into the namespace
    if( dll ) retval = type_engine_add_dll( (t_Env)vm->get_env(), dll,
                                            (const char *)(*(sp+1)) );

    // push the result
    push_( sp, (uint)dll );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_DLL_Unload::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    sint retval = FALSE;
    Chuck_DLL * dll = NULL;
    pop_( sp, 1 );

    // unload the dll
    dll = (Chuck_DLL *)(*sp);
    if( dll ) retval = vm->dll_unload( dll );

    // push the result
    push_( sp, retval );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_single2int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    pop_( sp, 1 );
    push_( sp, (sint)*(float *)sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_int2single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    pop_( sp, 1 );
    push_( sp, (float)*(sint *)sp );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_double2int::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    sint *& sp_int = (sint *&)sp;
    pop_( sp, 1 );
    push_( sp_int, (sint)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_int2double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    sint *& sp = (sint *&)shred->reg->sp;
    double *& sp_double = (double *&)sp;
    pop_( sp, 1 );
    push_( sp_double, (double)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_double2single::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    double *& sp = (double *&)shred->reg->sp;
    float *& sp_float = (float *&)sp;
    pop_( sp, 1 );
    push_( sp_float, (float)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Cast_single2double::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    float *& sp = (float *&)shred->reg->sp;
    double *& sp_double = (double *&)sp;
    pop_( sp, 1 );
    push_( sp_double, (double)(*sp) );
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Print_Console::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    uint type;
    uint w = 0;
    
    // pop word from reg stack
    pop_( sp, 1 );
    type = *sp;
    
    switch( type )
    {
        case cip_int:
        case cip_uint:
        case cip_string:
            pop_( sp, 2 );
            break;
            
        case cip_double:
        case cip_float:
        case cip_dur:
        case cip_time:
            w = 1;
            pop_( sp, 3 );
            break;
            
        default:
            fprintf( stderr, 
                     "[chuck virtual machine]: Exception UnknownTypeToStdout '%u'\n",
                     type );
            return;
    }
    
    switch( type )
    {
        case cip_int:
            fprintf( stdout, "%i\n", *((sint*)sp) );
            break;
        case cip_uint:
            fprintf( stdout, "%u\n", *((uint *)sp) );
            break;
        case cip_float:
            fprintf( stdout, "%.6f\n", *((double *)sp) );
            break;
        case cip_double:
            fprintf( stdout, "%.6f\n", *((double *)sp) );
        break;
        case cip_string:
            fprintf( stdout, "%s\n", *((char **)sp) );
            break;
        case cip_dur:
            fprintf( stdout, "dur=%.4fs\n", *((t_CKDUR *)sp) / (double)Digitalio::sampling_rate() );
            break;
        case cip_time:
            fprintf( stdout, "time=%.4fs\n", *((t_CKTIME *)sp) / (double)Digitalio::sampling_rate() );
            break;
    }
    
    // push the data back
    double *& sp_double = (double *&)sp;
    if( w == 0 )
    {
        push_( sp, *sp );
    }
    else
    {
        push_( sp_double, *sp_double );
    }
}




//-----------------------------------------------------------------------------
// name: execute()
// desc: ...
//-----------------------------------------------------------------------------
void Chuck_Instr_Print_Console2::execute( Chuck_VM * vm, Chuck_VM_Shred * shred )
{
    uint *& sp = (uint *&)shred->reg->sp;
    uint type;
    uint w = 0;

    // pop word from reg stack
    pop_( sp, 1 );
    type = *sp;

    switch( type )
    {
    case cip_int:
    case cip_uint:
    case cip_string:
        pop_( sp, 2 );
    break;

    case cip_double:
    case cip_float:
    case cip_dur:
    case cip_time:
        w = 1;
        pop_( sp, 3 );
    break;

    default:
        fprintf( stderr, 
                 "[chuck virtual machine]: Exception UnknownTypeToStdout '%u'\n",
                 type );
        return;
    }

    switch( type )
    {
    case cip_int:
        fprintf( stderr, "%i", *((sint*)(sp+1)) );
        break;
    case cip_uint:
        fprintf( stderr, "%u", *((uint *)(sp+1)) );
        break;
    case cip_float:
        fprintf( stderr, "%.6f", *((double *)(sp+1)) );
    break;

    case cip_double:
        fprintf( stderr, "%.6f", *((double *)(sp+1)) );
        break;
    case cip_string:
        fprintf( stderr, "%s", *((char **)(sp+1)) );
        break;
    case cip_dur:
        fprintf( stderr, "%.4fs", *((t_CKDUR *)(sp+1)) / (double)Digitalio::sampling_rate() );
        break;
    case cip_time:
        fprintf( stderr, "%.4fs", *((t_CKTIME *)(sp+1)) / (double)Digitalio::sampling_rate() );
    break;
    }

    // push
    push_( sp, *sp );
}
