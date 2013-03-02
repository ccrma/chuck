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
// file: chuck_cogen.h
// desc: Compiler Optimized Unit Generators
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Spring 2013
//-----------------------------------------------------------------------------

#ifndef __CHUCK_COGEN_H__
#define __CHUCK_COGEN_H__

#include "chuck_def.h"
#include "chuck_ugen.h"

class Chuck_CogenGraph
{
public:
    
    t_CKINT virtual tick(SAMPLE input, SAMPLE &output) = 0;
};

class Chuck_Cogen
{
public:
    static Chuck_Cogen * defaultCogenerator();
    
    virtual Chuck_CogenGraph * cogenerate(Chuck_UGen * root) = 0;
    
private:
};

#endif // __CHUCK_COGEN_H__
