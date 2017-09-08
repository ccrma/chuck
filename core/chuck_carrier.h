/*----------------------------------------------------------------------------
 ChucK Concurrent, On-the-fly Audio Programming Language
 Compiler and Virtual Machine
 
 Copyright (c) 2003 Ge Wang and Perry R. Cook.  All rights reserved.
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
// file: chuck_carrier.h
// desc: carrier of things associated with each ChucK instance
//       REFACTOR-2017
//
// author: Ge Wang (http://www.gewang.com/)
//         Jack Atherton (lja@ccrma.stanford.edu)
// date: fall 2017
//
// additional authors:
//         Spencer Salazar (spencer@ccrma.stanford.edu)
//-----------------------------------------------------------------------------
#ifndef __CHUCK_CARRIER_H__
#define __CHUCK_CARRIER_H__




// forward references
class Chuck_Compiler;
class Chuck_VM;
class Chuck_Env;
class Chuck_IO_Chout;
class Chuck_IO_Cherr;




//-----------------------------------------------------------------------------
// name: struct Chuck_Carrier
// desc: carrier of per-ChucK-instance things; each
//-----------------------------------------------------------------------------
struct Chuck_Carrier
{
    Chuck_Compiler * compiler;
    Chuck_Env * env;
    Chuck_VM * vm;
    Chuck_IO_Chout * chout;
    Chuck_IO_Cherr * cherr;
    
    // constructor
    Chuck_Carrier() :
        compiler( NULL ),
        env( NULL ),
        vm( NULL ),
        chout( NULL ),
        cherr( NULL )
        { }
};




#endif
