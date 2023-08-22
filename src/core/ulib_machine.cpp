/*----------------------------------------------------------------------------
  ChucK Concurrent, On-the-fly Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
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
// file: ulib_machine.cpp
// desc: class library for 'Machine'
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_machine.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck.h"
#include "util_string.h"

#include <string>
#include <algorithm>
using namespace std;

// exports
CK_DLL_SFUN( machine_crash_impl );
CK_DLL_SFUN( machine_add_impl );
CK_DLL_SFUN( machine_spork_impl );
CK_DLL_SFUN( machine_remove_impl );
CK_DLL_SFUN( machine_replace_impl );
CK_DLL_SFUN( machine_status_impl );
CK_DLL_SFUN( machine_intsize_impl );
CK_DLL_SFUN( machine_shreds_impl );
CK_DLL_SFUN( machine_realtime_impl );
CK_DLL_SFUN( machine_silent_impl );
CK_DLL_SFUN( machine_eval_impl );
CK_DLL_SFUN( machine_eval2_impl );
CK_DLL_SFUN( machine_eval3_impl );
CK_DLL_SFUN( machine_version_impl );
CK_DLL_SFUN( machine_setloglevel_impl );
CK_DLL_SFUN( machine_getloglevel_impl );
CK_DLL_SFUN( machine_refcount_impl);




//-----------------------------------------------------------------------------
// name: machine_query()
// desc: query entry point for Machine class library
//-----------------------------------------------------------------------------
DLL_QUERY machine_query( Chuck_DL_Query * QUERY )
{
    // get global env instance
    Chuck_Env * env = QUERY->env();
    // set name of this query
    QUERY->setname( QUERY, "Machine" );

    /*! \nameinfo
      ChucK runtime interface to the virtual machine.
      this interface can be used to manage shreds.
      They are similar to the <a href="otfp.html">
      On-the-fly Programming Commands</a>, except these are
      invoked from within a ChucK program, and are accessible
      to the timing mechanism.
    */

    // register deprecate
    type_engine_register_deprecate( env, "machine", "Machine" );

    // class
    QUERY->begin_class( QUERY, "Machine", "Object" );
    // documentation
    QUERY->doc_class( QUERY, "Machine is ChucK runtime interface to the virtual machine. This interface can be used to manage shreds, evalute code, set log levels, etc. Machine's shred commands are similar to the On-the-fly Programming Commands, except these are invoked from within a ChucK program, and are subject to the timing mechanism." );

#ifndef __DISABLE_OTF_SERVER__
    // add add
    //! compile and spork a new shred from file at 'path' into the VM now
    //! returns the shred ID
    //! (see example/machine.ck)
    QUERY->add_sfun( QUERY, machine_add_impl, "int", "add" );
    QUERY->doc_func( QUERY, "add/run a new shred from file at 'path'." );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Compile and spork a new shred from file at 'path' into the VM now, returns the shred ID.");

    // add spork
    //! same as add
    // QUERY->add_sfun( QUERY, machine_add_impl, "int", "spork" );
    // QUERY->add_arg( QUERY, "string", "path" );

    // add remove
    //! remove shred from VM by shred ID (returned by add/spork)
    QUERY->add_sfun( QUERY, machine_remove_impl, "int", "remove" );
    QUERY->doc_func( QUERY, "remove a running shred by 'id'." );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->doc_func( QUERY, "Remove shred from VM by shred ID (returned by Machine.add).");

    // add replace
    //! replace shred with new shred from file
    //! returns shred ID , or 0 on error
    QUERY->add_sfun( QUERY, machine_replace_impl, "int", "replace" );
    QUERY->doc_func( QUERY, "replace running shred by 'id' with new shred at 'path'." );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Replace shred with new shred from file. Returns shred ID , or 0 on error.");

    // add status
    //! display current status of VM
    //! (see example/status.ck)
    QUERY->add_sfun( QUERY, machine_status_impl, "int", "status" );
    QUERY->doc_func( QUERY, "display current status of virtual machine." );
    QUERY->doc_func( QUERY, "Display the current status of VM.");

#endif // __DISABLE_OTF_SERVER__

    // add crash
    //! explicitly crash the virtual machine
    QUERY->add_sfun( QUERY, machine_crash_impl, "void", "crash" );
    QUERY->doc_func( QUERY, "explicitly crash the virtual machine. The very last resort; or an emphatic gesture. Use with care." );

    // add shreds
    //! get list of active shreds by id
    QUERY->add_sfun( QUERY, machine_shreds_impl, "int[]", "shreds" );
    QUERY->doc_func( QUERY, "Retrieve an array of active shred ids." );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, compile it and immediately spork it as a new independent shred, and automatically yield the current shred to give the new shred a chance to run, without advancing time." );
    QUERY->add_arg( QUERY, "string", "code" );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval2_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, with arguments (bundled in 'args' as \"ARG1:ARG2:...\", compile it and immediately spork it as a new independent shred, and automatically yield the current shred to give the new shred a chance to run, without advancing time." );
    QUERY->add_arg( QUERY, "string", "code" );
    QUERY->add_arg( QUERY, "string", "args" );

    // add eval
    //! evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
    QUERY->add_sfun( QUERY, machine_eval3_impl, "int", "eval" );
    QUERY->doc_func( QUERY, "evaluate a string as ChucK code, with optional arguments (bundled in 'args' as \"ARG1:ARG2:...\", compile it and immediately spork 'count' independent shreds; and automatically yield the current shred to give all new shreds a chance to run, without advancing time." );
    QUERY->add_arg( QUERY, "string", "code" );
    QUERY->add_arg( QUERY, "string", "args" );
    QUERY->add_arg( QUERY, "int", "count" );

    // add get intsize (width)
    //! get the intsize in bits (e.g., 32 or 64)
    QUERY->add_sfun( QUERY, machine_intsize_impl, "int", "intsize" );
    QUERY->doc_func( QUERY, "Return the bit size of an integer.");

    // add check for silent (or realtime audio) | adapted from nshaheed PR #230
    QUERY->add_sfun( QUERY, machine_silent_impl, "int", "silent" );
    QUERY->doc_func( QUERY, "return false if the shred is in realtime mode, true if it's in silent mode (i.e. --silent is enabled)" );

    // add check for realtime (always opposite of silent()) | adapted from nshaheed PR #230
    QUERY->add_sfun( QUERY, machine_realtime_impl, "int", "realtime" );
    QUERY->doc_func( QUERY, "return true if the shred is in realtime mode, false if it's in silent mode (i.e. --silent is enabled)" );

    // get version string
    QUERY->add_sfun( QUERY, machine_version_impl, "string", "version" );
    QUERY->doc_func( QUERY, "return version string" );

    // get set loglevel
    QUERY->add_sfun( QUERY, machine_setloglevel_impl, "int", "loglevel" );
    QUERY->add_arg( QUERY, "int", "level" );
    QUERY->doc_func( QUERY, "set log level\n"
                     "       |- 0: NONE\n"
                     "       |- 1: CORE\n"
                     "       |- 2: SYSTEM\n"
                     "       |- 3: SEVERE\n"
                     "       |- 4: WARNING\n"
                     "       |- 5: INFO\n"
                     "       |- 6: CONFIG\n"
                     "       |- 7: FINE\n"
                     "       |- 8: FINER\n"
                     "       |- 9: FINEST\n"
                     "       |- 10: ALL" );

    // get log level
    QUERY->add_sfun( QUERY, machine_getloglevel_impl, "int", "loglevel" );
    QUERY->doc_func( QUERY, "get log level." );

    // get reference count for any object
    QUERY->add_sfun( QUERY, machine_refcount_impl, "int", "refcount" );
    QUERY->add_arg( QUERY, "Object", "obj" );
    QUERY->doc_func( QUERY, "get an object's current internal reference count; this is intended for testing or curiosity; NOTE: this function intentionally does not take into account any reference counting related to the calling of this function (normally all functions increments the reference count for objects passed as arguments and decrements upon returning)" );

    // add examples
    QUERY->add_ex( QUERY, "machine/eval.ck" );
    QUERY->add_ex( QUERY, "machine/eval-global.ck" );
    QUERY->add_ex( QUERY, "machine/intsize.ck" );
    QUERY->add_ex( QUERY, "machine/is-realtime.ck" );
    QUERY->add_ex( QUERY, "machine/machine-shred.ck" );
    QUERY->add_ex( QUERY, "machine/version.ck" );
    QUERY->add_ex( QUERY, "book/digital-artists/chapter9/DrumMachine" );
    QUERY->add_ex( QUERY, "book/digital-artists/chapter9/SmartMandolin/initialize.ck" );

    // end class
    QUERY->end_class( QUERY );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: machine_intsize()
// desc: integer size in bits
//-----------------------------------------------------------------------------
t_CKUINT machine_intsize()
{
    return sizeof(t_CKINT) * 8;
}




//-----------------------------------------------------------------------------
// name: machine_realtime()
// desc: return whether chuck is in realtime mode, or silent mode (--silent is enabled)
//-----------------------------------------------------------------------------
t_CKUINT machine_realtime( Chuck_VM * vm )
{
    return vm->carrier()->hintIsRealtimeAudio();
}




//-----------------------------------------------------------------------------
// name: machine_eval()
// desc: evaluate a string as ChucK code, compiling it and adding it to the the virtual machine
//-----------------------------------------------------------------------------
t_CKUINT machine_eval( Chuck_String * codeStr, Chuck_String * argsTogether,
                       t_CKINT count, Chuck_VM_Shred * evaluator )
{
    // check if null
    if( codeStr == NULL ) return FALSE;

    // get VM
    Chuck_VM * vm = evaluator->vm_ref;
    // get ChucK instance
    ChucK * chuck = vm->carrier()->chuck;
    // get code as string
    string code = codeStr->str();
    // get args, if there
    string args = argsTogether ? argsTogether->str() : "";

    // compile and spork | 1.5.0.5 (ge) immediate=TRUE
    t_CKUINT retval = chuck->compileCode( code, args, count, TRUE );
    // automatically yield current shred to let new code run | 1.5.0.5 (ge)
    evaluator->yield();

    // return
    return retval;
}



#ifndef __DISABLE_OTF_SERVER__

// OTF server stuff
static Chuck_Compiler * the_compiler = NULL;
static proc_msg_func the_func = NULL;
//-----------------------------------------------------------------------------
// name: machine_init()
// desc: initialize Machine class library
//-----------------------------------------------------------------------------
t_CKBOOL machine_init( Chuck_Compiler * compiler, proc_msg_func proc_msg )
{
    the_compiler = compiler;
    the_func = proc_msg;

    return TRUE;
}

// add
CK_DLL_SFUN( machine_add_impl )
{
    const char * v = GET_CK_STRING(ARGS)->str().c_str();
    OTF_Net_Msg msg;

    msg.type = CK_MSG_ADD;
    strcpy( msg.buffer, v );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// remove
CK_DLL_SFUN( machine_remove_impl )
{
    t_CKINT v = GET_CK_INT(ARGS);
    OTF_Net_Msg msg;

    msg.type = CK_MSG_REMOVE;
    msg.param = v;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// replace
CK_DLL_SFUN( machine_replace_impl )
{
    t_CKINT v = GET_NEXT_INT(ARGS);
    const char * v2 = GET_NEXT_STRING(ARGS)->str().c_str();
    OTF_Net_Msg msg;

    msg.type = CK_MSG_REPLACE;
    msg.param = v;
    strcpy( msg.buffer, v2 );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// status
CK_DLL_SFUN( machine_status_impl )
{
    OTF_Net_Msg msg;

    msg.type = CK_MSG_STATUS;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

#endif // __DISABLE_OTF_SERVER__

// intsize
CK_DLL_SFUN( machine_intsize_impl )
{
    RETURN->v_int = machine_intsize();
}

// realtime
CK_DLL_SFUN( machine_realtime_impl )
{
    RETURN->v_int = machine_realtime( SHRED->vm_ref );
}

// silent
CK_DLL_SFUN( machine_silent_impl )
{
    RETURN->v_int = !machine_realtime( SHRED->vm_ref );
}

CK_DLL_SFUN( machine_shreds_impl )
{
    Chuck_Array4 *array = new Chuck_Array4(FALSE);
    initialize_object(array, SHRED->vm_ref->env()->ckt_array);
    array->clear();

    Chuck_VM_Status status;
    SHRED->vm_ref->shreduler()->status(&status);

    for(int i = 0; i < status.list.size(); i++)
        array->push_back(status.list[i]->xid);

    status.clear();

    RETURN->v_object = array;
}

CK_DLL_SFUN( machine_eval_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);

    RETURN->v_int = machine_eval( code, NULL, 1, SHRED );
}

CK_DLL_SFUN( machine_eval2_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);
    Chuck_String * argsTogether = GET_NEXT_STRING(ARGS);

    RETURN->v_int = machine_eval( code, argsTogether, 1, SHRED );
}

CK_DLL_SFUN( machine_eval3_impl )
{
    // get arguments
    Chuck_String * code = GET_NEXT_STRING(ARGS);
    Chuck_String * argsTogether = GET_NEXT_STRING(ARGS);
    t_CKINT count = GET_NEXT_INT(ARGS);

    RETURN->v_int = machine_eval( code, argsTogether, count, SHRED );
}

CK_DLL_SFUN( machine_version_impl )
{
    // get version string
    string vs = VM->carrier()->chuck->version();
    // make chuck string
    Chuck_String * s = new Chuck_String( vs );
    // initialize
    initialize_object(s, VM->carrier()->env->ckt_string );
    // return
    RETURN->v_string = s;
}

CK_DLL_SFUN( machine_setloglevel_impl )
{
    // arg
    t_CKINT level = GET_NEXT_INT(ARGS);
    // get current log
    t_CKINT current = VM->carrier()->chuck->getLogLevel();
    // clamp compare
    if( level < CK_LOG_NONE ) level = CK_LOG_NONE;
    else if( level > CK_LOG_ALL ) level = CK_LOG_ALL;
    // compare
    if( level != current )
    {
        EM_log( CK_LOG_NONE, "updating log level from %d to %d...", current, level );
        VM->carrier()->chuck->setLogLevel( level );
    }
    // return
    RETURN->v_int = level;
}

CK_DLL_SFUN( machine_getloglevel_impl )
{
    // return
    RETURN->v_int = VM->carrier()->chuck->getLogLevel();
}

CK_DLL_SFUN( machine_refcount_impl )
{
    // get the argument
    Chuck_Object * obj = (Chuck_Object *)GET_NEXT_OBJECT(ARGS);
    // return (-1 to account for the extra refcount as part of calling this function!)
    RETURN->v_int = obj != NULL ? obj->m_ref_count-1 : 0;
}

// add
CK_DLL_SFUN( machine_crash_impl )
{
    CK_FPRINTF_STDERR( "[chuck]: crashing...\n" );
    *(volatile int *)0 = 0;
}
