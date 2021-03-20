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
//       REFACTOR-2017
//
// author: Ge Wang (http://www.gewang.com/)
// date: fall 2017
//
// additional authors:
//       Jack Atherton (lja@ccrma.stanford.edu)
//       Spencer Salazar (spencer@ccrma.stanford.edu)
//-----------------------------------------------------------------------------
#ifndef __CHUCK_H__
#define __CHUCK_H__

#include "chuck_compile.h"
#include "chuck_dl.h"
#include "chuck_vm.h"
#include "chuck_shell.h"
#include "chuck_carrier.h"
#include "ulib_machine.h"
#include "util_math.h"
#include "util_string.h"
#include "hidio_sdl.h"
#include "midiio_rtmidi.h"
#include <string>
#include <map>




// ChucK param names -- used in setParam(...) and getParam*(...)
#define CHUCK_PARAM_SAMPLE_RATE             "SAMPLE_RATE"
#define CHUCK_PARAM_INPUT_CHANNELS          "INPUT_CHANNELS"
#define CHUCK_PARAM_OUTPUT_CHANNELS         "OUTPUT_CHANNELS"
#define CHUCK_PARAM_VM_ADAPTIVE             "VM_ADAPTIVE"
#define CHUCK_PARAM_VM_HALT                 "VM_HALT"
#define CHUCK_PARAM_OTF_ENABLE              "OTF_ENABLE"
#define CHUCK_PARAM_OTF_PORT                "OTF_PORT"
#define CHUCK_PARAM_DUMP_INSTRUCTIONS       "DUMP_INSTRUCTIONS"
#define CHUCK_PARAM_AUTO_DEPEND             "AUTO_DEPEND"
#define CHUCK_PARAM_DEPRECATE_LEVEL         "DEPRECATE_LEVEL"
#define CHUCK_PARAM_WORKING_DIRECTORY       "WORKING_DIRECTORY"
#define CHUCK_PARAM_CHUGIN_ENABLE           "CHUGIN_ENABLE"
#define CHUCK_PARAM_CHUGIN_DIRECTORY        "CHUGIN_DIRECTORY"
#define CHUCK_PARAM_USER_CHUGINS            "USER_CHUGINS"
#define CHUCK_PARAM_USER_CHUGIN_DIRECTORIES "USER_CHUGIN_DIRECTORIES"
#define CHUCK_PARAM_HINT_IS_REALTIME_AUDIO  "HINT_IS_REALTIME_AUDIO"




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
    bool setParamFloat( const std::string & name, t_CKFLOAT value );
    bool setParam( const std::string & name, const std::string & value );
    bool setParam( const std::string & name, const std::list< std::string > & value );
    // get params
    t_CKINT getParamInt( const std::string & key );
    t_CKFLOAT getParamFloat( const std::string & key );
    std::string getParamString( const std::string & key );
    std::list< std::string > getParamStringList( const std::string & key );

public:
    // compile a file (can be called anytime)
    bool compileFile( const std::string & path, const std::string & argsTogether, int count = 1 );
    // compile code directly
    bool compileCode( const std::string & code, const std::string & argsTogether, int count = 1 );

public:
    // initialize ChucK (using params)
    bool init();
    // explicit start (optionally -- done as neede from run())
    bool start();

public:
    // run engine (call from callback)
    void run( SAMPLE * input, SAMPLE * output, int numFrames );

public:
    // is initialized
    bool running() { return m_init; }
    
public:
    // additional native chuck bindings/types (use with extra caution)
    bool bind( f_ck_query queryFunc, const std::string & name );
    // set a function pointer to call from the main thread loop
    bool setMainThreadHook( Chuck_DL_MainThreadHook * hook );
    // get pointer to current function to be called from main loop
    Chuck_DL_MainThreadHook * getMainThreadHook();

public:
    // get VM (dangerous)
    Chuck_VM * vm() { return m_carrier->vm; }
    bool vmrunning() { return m_carrier->vm && m_carrier->vm->running(); }
    // get compiler (dangerous)
    Chuck_Compiler * compiler() { return m_carrier->compiler; }

public:
    // global variables - set and get
    t_CKBOOL setGlobalInt( const char * name, t_CKINT val );
    t_CKBOOL getGlobalInt( const char * name, void (* callback)(t_CKINT) );
    t_CKBOOL setGlobalFloat( const char * name, t_CKFLOAT val );
    t_CKBOOL getGlobalFloat( const char * name, void (* callback)(t_CKFLOAT) );
    t_CKBOOL signalGlobalEvent( const char * name );
    t_CKBOOL broadcastGlobalEvent( const char * name );
    
public:
    // global callback functions
    t_CKBOOL setChoutCallback( void (* callback)(const char *) );
    t_CKBOOL setCherrCallback( void (* callback)(const char *) );
    static t_CKBOOL setStdoutCallback( void (* callback)(const char *) );
    static t_CKBOOL setStderrCallback( void (* callback)(const char *) );

public:
    // global initialization for all instances (called once at beginning)
    static t_CKBOOL globalInit();
    // global cleanup for all instances (called once at end)
    static void globalCleanup();
    // flag for global init
    static t_CKBOOL o_isGlobalInit;

protected:
    // shutdown
    bool shutdown();
    
public: // static functions
    // chuck version
    static const char * version();
    // chuck int size (in bits)
    static t_CKUINT intSize();
    // number of ChucK's
    static t_CKUINT numVMs() { return o_numVMs; };
    // --poop compatibilty
    static void poop();
    // set log level -- this should eventually be per-VM?
    static void setLogLevel( t_CKINT level );
    // get log level -- also per-VM?
    static t_CKINT getLogLevel();
    // use with care: if true, this enables system() calls from code
    static t_CKBOOL enableSystemCall;

protected:
    // chuck version
    static const char VERSION[];
    // number of VMs -- managed from VM constructor/destructors
    static t_CKUINT o_numVMs;
    
protected:
    // initialize default params
    void initDefaultParams();
    // init VM
    bool initVM();
    // init compiler
    bool initCompiler();
    // init chugin system
    bool initChugins();
    // init OTF programming system
    bool initOTF();
    
protected:
    // core elements: compiler, VM, etc.
    Chuck_Carrier * m_carrier;
    // chuck params
    std::map<std::string, std::string> m_params;
    // chuck list params
    std::map< std::string, std::list<std::string> > m_listParams;
    // did user init?
    t_CKBOOL m_init;
    // main thread "hook" (if there is a main thread)
    Chuck_DL_MainThreadHook * m_hook;
};




#endif
