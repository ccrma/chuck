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
// file: chuck_main.cpp
// desc: chuck entry point
//
// v1.3.5.3: migration to chuck_system.* (see for more details)
//
// author: Ge Wang (http://www.gewang.com/)
//         (see chuck_system.*)
//-----------------------------------------------------------------------------
#include "chuck_system.h"

#ifndef __ALTER_ENTRY_POINT__
int main( int argc, const char ** argv )
#else
extern "C" int chuck_main( int argc, const char ** argv )
#endif
{
    // chuck system
    Chuck_System chuck;
    // do it
    chuck.go( argc, argv );
    
    return 0;
}
