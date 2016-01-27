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
// file: chuck_system.h
// desc: chuck system encapsulation
//
// author: Ge Wang (http://www.gewang.com/)
// date: fall 2015
//
// migrated from chuck_main.cpp: easier integration wwith 3rd-party frameworks
//
// additional contributors:
//         Ananya Misra (amisra@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: version 1.1.x.x - Autumn 2002
//       version 1.2.x.x - Autumn 2004
//       version 1.3.x.x - Spring 2012
//-----------------------------------------------------------------------------
#ifndef __CHUCK_SYSTEM_H__
#define __CHUCK_SYSTEM_H__

#include "chuck_compile.h"
#include "chuck_dl.h"
#include "chuck_vm.h"
#include <string>




//-----------------------------------------------------------------------------
// name: class ChuckSystem
// desc: ChucK system encapsulation
//-----------------------------------------------------------------------------
class Chuck_System
{
public:
    // constructor
    Chuck_System();
    // desctructor
    virtual ~Chuck_System();

public:
    // monolithic mode (use this only if you are in ChucK)
    bool go( int argc, const char ** argv, t_CKBOOL clientMode = FALSE );

    // OR...
    
public:
    // initialize for client mode (e.g., as plugin)
    bool clientInitialize( int srate, int bufferSize, int channelsIn,
                           int channelsOut, int argc, const char ** argv );
    // shutdown in client mode
    bool clientShutdown();
    
public:
    // additional native chuck bindings/types (use with extra caution)
    bool bind( f_ck_query queryFunc, const std::string & name );

public:
    // get VM (dangerous)
    Chuck_VM * vm() { return m_vmRef; }
    // get compiler (dangerous)
    Chuck_Compiler * compiler() { return m_compilerRef; }

public:
    // compile a file
    bool compileFile( const std::string & path, const std::string & args, int count = 1 );
    // compile code directly
    bool compileCode( const std::string & code, const std::string & args, int count = 1 );
    
public:
    // run engine
    void run( SAMPLE * input, SAMPLE * output, int numFrames );

protected:
    // virtual machine reference
    Chuck_VM * m_vmRef;
    // compiler reference
    Chuck_Compiler * m_compilerRef;
};




#endif
