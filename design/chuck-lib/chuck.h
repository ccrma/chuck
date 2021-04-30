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
// file: chuck.h
// desc: chuck engine header; VM + compiler + state; no audio I/O
//
// author: Ge Wang (http://www.gewang.com/)
// date: fall 2017
//
// additional authors:
//         Jack Atherton (lja@ccrma.stanford.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
//-----------------------------------------------------------------------------
#ifndef __CHUCK_H__
#define __CHUCK_H__

#include "chuck_compile.h"
#include "chuck_dl.h"
#include "chuck_vm.h"
#include <string>




// ChucK param names -- used in setParam(...) and getParam*(...)
#define CHUCK_PARAM_SAMPLE_RATE         "SAMPLE_RATE"
#define CHUCK_PARAM_LOG_LEVEL           "LOG_LEVEL"
#define CHUCK_PARAM_DUMP_INSTRUCTIONS   "DUMP_INSTRUCTIONS"
#define CHUCK_PARAM_DEPRECATE_LEVEL     "DEPRECATE_LEVEL"
#define CHUCK_PARAM_WORKING_DIRECTORY   "WORKING_DIRECTORY"




//-----------------------------------------------------------------------------
// name: class ChucK
// desc: ChucK system encapsulation
//-----------------------------------------------------------------------------
class ChucK
{
public:
    // constructor
    ChucK();
    // desctructor
    virtual ~ChucK();
    
public:
    // set parameter by name
    // -- all params should have reasonable defaults
    bool setParam( const std::string & name, t_CKINT value );
    bool setParam( const std::string & name, t_CKFLOAT value );
    bool setParam( const std::string & name, const std::string & value );
    // get params
    t_CKINT getParamInt( const std::string & key ) const;
    t_CKFLOAT getParamFloat( const std::string & key ) const;
    std::string getParamString( const std::string & key ) const;

public:
    // compile a file (can be called anytime)
    bool compileFile( const std::string & path, const std::string & args, int count = 1 );
    // compile code directly
    bool compileCode( const std::string & code, const std::string & args, int count = 1 );

public:
    // explicit start (optionally -- done as neede from run())
    bool start();

    // run engine (call from callback)
    void run( SAMPLE * input, SAMPLE * output, int numFrames );

public:
    // additional native chuck bindings/types (use with extra caution)
    bool bind( f_ck_query queryFunc, const std::string & name );

public:
    // get VM (dangerous)
    Chuck_VM * vm() { return m_vmRef; }
    // get compiler (dangerous)
    Chuck_Compiler * compiler() { return m_compilerRef; }

protected:
    // initialize
    bool init();
    // shutdown
    bool shutdown();
    
protected:
    // virtual machine reference
    Chuck_VM * m_vmRef;
    // compiler reference
    Chuck_Compiler * m_compilerRef;
};




#endif
