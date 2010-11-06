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
// file: chuck_emit.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Autumn 2002
//-----------------------------------------------------------------------------
#ifndef __CHUCK_EMIT_H__
#define __CHUCK_EMIT_H__

#include "chuck_def.h"
#include "chuck_type.h"



struct Chuck_Emmission;
class Chuck_VM_Code;
class Chuck_VM_Shred;

Chuck_Emmission * emit_engine_init( t_Env env );
t_CKBOOL emit_engine_shutdown( Chuck_Emmission *& emit );
t_CKBOOL emit_engine_emit_prog( Chuck_Emmission * emit, a_Program prog );
Chuck_VM_Code * emit_to_code( Chuck_Emmission * emit, t_CKBOOL dump_funcs = FALSE );
t_CKBOOL emit_engine_addr_map( Chuck_Emmission * emit, Chuck_VM_Shred * shred );
t_CKBOOL emit_engine_resolve( );




#endif
