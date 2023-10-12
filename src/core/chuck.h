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
// desc: interface for ChucK core: compiler + virtual machine + synthesis
//       |- possible to instantiate multiple ChucK instances
//       |- each ChucK instance contains a fully functioning ChucK runtime
//       |- the ChucK class interface is primary point of integration between
//          ChucK core and ChucK hosts (e.g., command-line ChucK,
//          miniAudicle, WebChucK, Chunity, Chunreal, and any C++ program
//          using ChucK core as a component).
//       |- see src/host-examples/ for sample integration code for ChucK core
//       |- see chuck, miniAudicle, WebChucK, Chunity etc. for their
//          respective integration of ChucK core
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
//   date: fall 2017
//
// additional authors:
//       Jack Atherton (lja@ccrma.stanford.edu)
//       Spencer Salazar (spencer@ccrma.stanford.edu)
//
// (part of The Big Refactor of 2017; codename `numchucks`; #REFACTOR-2017)
//-----------------------------------------------------------------------------
#ifndef __CHUCK_H__
#define __CHUCK_H__

#include "chuck_compile.h"
#include "chuck_dl.h"
#include "chuck_vm.h"
#include "chuck_carrier.h"
#include "util_math.h"
#include "util_string.h"
#include <string>
#include <map>

#ifndef __DISABLE_SHELL__
#include "chuck_shell.h"
#endif

#ifndef __DISABLE_OTF_SERVER
#include "ulib_machine.h"
#endif

#ifndef __DISABLE_HID__
#include "hidio_sdl.h"
#endif

#ifndef __DISABLE_MIDI__
#include "midiio_rtmidi.h"
#endif

// ChucK version string -- retrieve using ChucK::version()
// 1.5.0.0 (ge) | moved here for at-a-glance visibility (e.g., for chugins)
#define CHUCK_VERSION_STRING                    "1.5.1.7-dev (chai)"

// ChucK param names -- used in setParam(...) and getParam*(...)
#define CHUCK_PARAM_VERSION                     "VERSION"
#define CHUCK_PARAM_SAMPLE_RATE                 "SAMPLE_RATE"
#define CHUCK_PARAM_INPUT_CHANNELS              "INPUT_CHANNELS"
#define CHUCK_PARAM_OUTPUT_CHANNELS             "OUTPUT_CHANNELS"
#define CHUCK_PARAM_VM_ADAPTIVE                 "VM_ADAPTIVE"
#define CHUCK_PARAM_VM_HALT                     "VM_HALT"
#define CHUCK_PARAM_OTF_ENABLE                  "OTF_ENABLE"
#define CHUCK_PARAM_OTF_PORT                    "OTF_PORT"
#define CHUCK_PARAM_OTF_PRINT_WARNINGS          "OTF_PRINT_WARNINGS"
#define CHUCK_PARAM_DUMP_INSTRUCTIONS           "DUMP_INSTRUCTIONS"
#define CHUCK_PARAM_AUTO_DEPEND                 "AUTO_DEPEND"
#define CHUCK_PARAM_DEPRECATE_LEVEL             "DEPRECATE_LEVEL"
#define CHUCK_PARAM_WORKING_DIRECTORY           "WORKING_DIRECTORY"
#define CHUCK_PARAM_CHUGIN_ENABLE               "CHUGIN_ENABLE"
#define CHUCK_PARAM_CHUGIN_DIRECTORY            "CHUGIN_DIRECTORY"
#define CHUCK_PARAM_USER_CHUGINS                "USER_CHUGINS"
#define CHUCK_PARAM_USER_CHUGIN_DIRECTORIES     "USER_CHUGIN_DIRECTORIES"
#define CHUCK_PARAM_IS_REALTIME_AUDIO_HINT      "IS_REALTIME_AUDIO_HINT"
#define CHUCK_PARAM_COMPILER_HIGHLIGHT_ON_ERROR "COMPILER_HIGHLIGHT_ON_ERROR"
#define CHUCK_PARAM_TTY_COLOR                   "TTY_COLOR"
#define CHUCK_PARAM_TTY_WIDTH_HINT              "TTY_WIDTH_HINT"

// legacy compatibiity (new code should use newer version of these, on the right)
#define CHUCK_PARAM_HINT_IS_REALTIME_AUDIO      CHUCK_PARAM_IS_REALTIME_AUDIO_HINT




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
    //   |- (in general, parameters have reasonable defaults)
    t_CKBOOL setParam( const std::string & name, t_CKINT value );
    t_CKBOOL setParamFloat( const std::string & name, t_CKFLOAT value );
    t_CKBOOL setParam( const std::string & name, const std::string & value );
    t_CKBOOL setParam( const std::string & name, const std::list< std::string > & value );
    // get parameter by type
    t_CKINT getParamInt( const std::string & key );
    t_CKFLOAT getParamFloat( const std::string & key );
    std::string getParamString( const std::string & key );
    // if a parameter is actually a list of value
    std::list< std::string > getParamStringList( const std::string & key );

public:
    // initialize ChucK (using params)
    t_CKBOOL init();
    // explicit start (optionally -- implicitly called as needed from run())
    t_CKBOOL start();

public:
    // compile a file -> generate chuck bytecode -> spork as new shred(s)
    // `argsTogether` is appended to `path` as additional arguments, separated by ':' e.g., "1:foo:bar"
    // `count` specifies how many instances of the new shred to spork
    // if `immediate` == TRUE, the new shred(s) is shreduled immediately (on the calling thread)
    // if `immediate` == FALSE, the new shreds(s) is queued and shreduled on the next time step (on the VM compute/audio thread)
    // if `shredIDS` is not NULL, it will be filled with the ID(s) of the new resulting shreds
    // returns TRUE if compilation successful (even if count == 0)
    // returns FALSE if compilation unsuccessful
    t_CKBOOL compileFile( const std::string & path, const std::string & argsTogether = "",
                          t_CKUINT count = 1, t_CKBOOL immediate = FALSE, std::vector<t_CKUINT> * shredIDs = NULL );
    // compile code/text -> genereate chuck bytecode -> spork as new shred(s)
    // `argsTogether` is appended to `path` as arguments, separated by ':' e.g., "1:foo:bar"
    // `count` specifies how many instances of the new shred to spork
    // if `immediate` == TRUE, the new shred(s) is shreduled immediately (on the calling thread)
    // if `immediate` == FALSE, the new shreds(s) is queued and shreduled on the next time step (on the VM compute/audio thread)
    // if `shredIDS` is not NULL, it will be filled with the ID(s) of the new resulting shreds
    // returns TRUE if compilation successful (even if count == 0)
    // returns FALSE if compilation unsuccessful
    t_CKBOOL compileCode( const std::string & code, const std::string & argsTogether = "",
                          t_CKUINT count = 1, t_CKBOOL immediate = FALSE, std::vector<t_CKUINT> * shredIDs = NULL );

public:
    // run ChucK and synthesize audio for `numFrames`...
    //   |- (NOTE this function is often called from audio callback)
    // `input`: the incoming input array of audio samples
    //   |- input array size expected to == `numFrames` * number-of-input-chanels, as initialized in setParam(CHUCK_PARAM_INPUT_CHANNELS, ...)
    // `output`: the outgoing output array of audio samples
    //   |- output array size expected to == `numFrames` * number-of-output-chanels, as initialized in setParam(CHUCK_PARAM_OUTPUT_CHANNELS, ...)
    // `numFrames` : the number of audio frames to run
    //   |- each audio frame corresponds to one point in time, and contains values for every audio channel
    void run( const SAMPLE * input, SAMPLE * output, t_CKINT numFrames );

public:
    // get globals (needed to access Globals Manager)
    //   |- useful for communication between C++ and ChucK using chuck global variables
    Chuck_Globals_Manager * globals();

public:
    // get VM (use with care)
    Chuck_VM * vm() { return m_carrier->vm; }
    // get compiler (use with care)
    Chuck_Compiler * compiler() { return m_carrier->compiler; }
    // is the VM running
    t_CKBOOL vm_running() const { return m_carrier->vm && m_carrier->vm->running(); }
    // is this ChucK instance initialized
    t_CKBOOL isInit() const { return m_init; }
    // get current chuck time
    t_CKTIME now() const { return m_carrier->vm ? m_carrier->vm->now() : 0; }

public:
    // set whether chuck will generate color output for its messages
    // this will set the corresponding parameter as well
    void toggleGlobalColorTextoutput( t_CKBOOL onOff );

public:
    // probe chugins (print info on all chugins as seen under current config)
    void probeChugins(); // 1.5.0.4 (ge) added

public:
    // additional native chuck bindings/types (use with extra caution)
    t_CKBOOL bind( f_ck_query queryFunc, const std::string & name );
    // set a function pointer to call from the main thread loop
    t_CKBOOL setMainThreadHook( Chuck_DL_MainThreadHook * hook );
    // get pointer to current function to be called from main loop
    Chuck_DL_MainThreadHook * getMainThreadHook();

public:
    // global callback functions: replace printing to command line with a callback function
    t_CKBOOL setChoutCallback( void (* callback)(const char *) );
    t_CKBOOL setCherrCallback( void (* callback)(const char *) );
    // static I/O redirects, for alternate printing of chuck oputput and system message
    // e.g., miniAudicle console, Unity console, etc.
    static t_CKBOOL setStdoutCallback( void (* callback)(const char *) );
    static t_CKBOOL setStderrCallback( void (* callback)(const char *) );

public: // static functions
    // chuck version
    static const char * version();
    // chuck int size (in bits)
    // (this depends on machine, which depends on OTF, so disable it if we don't have OTF)
    // 1.5.0.0 (ge) | reinstated for non-OTF, along with parts of machine
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

public:
    // global cleanup for all instances
    //   |- (called once at end when all instances done, usually by host)
    static void globalCleanup();

protected:
    // global initialization for all instances
    //   |- (called once at beginning internally by ChucK)
    static t_CKBOOL globalInit();
    // flag for global init
    static t_CKBOOL o_isGlobalInit;

protected:
    // chuck version
    static const char VERSION[];
    // number of VMs -- managed from VM constructor/destructors
    static t_CKUINT o_numVMs;

protected:
    // core elements: compiler, VM, etc.
    Chuck_Carrier * m_carrier;
    // chuck params
    std::map<std::string, std::string> m_params;
    // chuck list params
    std::map< std::string, std::list<std::string> > m_listParams;
    // read-only
    std::map< std::string, t_CKBOOL > m_readOnly;
    // enum for type of param (int, float, string)
    enum ck_param_type
    { ck_param_int, ck_param_float, ck_param_string, ck_param_string_list };
    // param types
    std::map< std::string, ck_param_type> m_param_types;
    // did user init?
    t_CKBOOL m_init;
    // did user start?
    t_CKBOOL m_started;
    // main thread "hook" (if there is a main thread)
    Chuck_DL_MainThreadHook * m_hook;

protected:
    // init VM
    t_CKBOOL initVM();
    // init compiler
    t_CKBOOL initCompiler();
    // init chugin system
    t_CKBOOL initChugins();
    // init OTF programming system
    t_CKBOOL initOTF();
    // shutdown
    t_CKBOOL shutdown();

    // initialize default params
    void initDefaultParams();
    // check read-only
    t_CKBOOL readOnlyParam( const std::string & name );
    // init param helper
    void initParam( const std::string & name, const std::string & value,
                    ck_param_type typeEnum, t_CKBOOL isReadyOnly = FALSE );
    // match param (case-insensitive)
    t_CKBOOL matchParam( const std::string & lhs, const std::string & rhs );
    // enact processing, as needed, for certain params
    void enactParam( const std::string & name, t_CKINT value );
};




#endif // #ifndef __CHUCK_H__
