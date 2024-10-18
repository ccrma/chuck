/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
CK_DLL_SFUN( machine_removeLast_impl );
CK_DLL_SFUN( machine_removeAll_impl );
CK_DLL_SFUN( machine_replace_impl );
CK_DLL_SFUN( machine_resetID_impl );
CK_DLL_SFUN( machine_clearVM_impl );
CK_DLL_SFUN( machine_printStatus_impl );
CK_DLL_SFUN( machine_printTimeCheck_impl );
CK_DLL_SFUN( machine_intsize_impl );
CK_DLL_SFUN( machine_shreds_impl );
CK_DLL_SFUN( machine_numshreds_impl );
CK_DLL_SFUN( machine_realtime_impl );
CK_DLL_SFUN( machine_silent_impl );
CK_DLL_SFUN( machine_eval_impl );
CK_DLL_SFUN( machine_eval2_impl );
CK_DLL_SFUN( machine_eval3_impl );
CK_DLL_SFUN( machine_version_impl );
CK_DLL_SFUN( machine_setloglevel_impl );
CK_DLL_SFUN( machine_getloglevel_impl );
CK_DLL_SFUN( machine_refcount_impl );
CK_DLL_SFUN( machine_regstack_impl );
CK_DLL_SFUN( machine_memstack_impl );
CK_DLL_SFUN( machine_gc );
CK_DLL_SFUN( machine_opOverloadReset_impl );
// not used
//CK_DLL_SFUN( machine_opOverloadPush_impl );
//CK_DLL_SFUN( machine_opOverloadPop_impl );
//CK_DLL_SFUN( machine_opOverloadStackLevel_impl );
//CK_DLL_SFUN( machine_testSetup_impl );
//CK_DLL_SFUN( machine_testInvoke_impl );




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
      Machine is the runtime interface to the ChucK Virtual Machine
      (Ck.VM or simply VM). This interface can be used to manage shreds,
      evaluate code, set log levels, etc. Machine's shred commands
      (add, replace, remove, etc.) are similar to the
      <a href="otfp.html">on-the-fly programming commands</a>,
      except these are invoked from within a ChucK program, and benefit
      from ChucK's strongly-timed mechanics.
    */

    // register deprecate
    type_engine_register_deprecate( env, "machine", "Machine" );

    // class
    QUERY->begin_class( QUERY, "Machine", "Object" );
    // documentation
    QUERY->doc_class( QUERY, "Machine is the runtime interface to the ChucK Virtual Machine (Ck.VM or simply VM). This interface can be used to manage shreds, evaluate code, set log levels, etc. Machine's shred commands (add, replace, remove, etc.) are similar to the on-the-fly programming commands, except these are invoked from within a ChucK program, and benefit from ChucK's strongly-timed mechanics." );

    // add add
    //! compile and spork a new shred from file at 'path' into the VM now
    //! returns the shred ID
    //! (see example/machine.ck)
    QUERY->add_sfun( QUERY, machine_add_impl, "int", "add" );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Compile and spork a new shred from file at 'path' into the VM; returns the new shred ID. It is possible to include arguments with the file being added, e.g., `Machine.add( \"foo.ck:bar:3:5.0\" )`." );

    // add replace
    //! replace shred with new shred from file
    //! returns shred ID , or 0 on error
    QUERY->add_sfun( QUERY, machine_replace_impl, "int", "replace" );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->add_arg( QUERY, "string", "path" );
    QUERY->doc_func( QUERY, "Replace shred with new shred from file. Returns new shred ID, or 0 on error. It is possible to include arguments, e.g., `Machine.replace( outID, \"foo.ck:bar:3:5.0\" )`.");

    // add remove
    //! remove shred from VM by shred ID (returned by add/spork)
    QUERY->add_sfun( QUERY, machine_remove_impl, "int", "remove" );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->doc_func( QUERY, "Remove shred from VM by shred ID (returned by Machine.add).");

    // add removeLastShred
    QUERY->add_sfun( QUERY, machine_removeLast_impl, "int", "removeLastShred" );
    QUERY->doc_func( QUERY, "Remove the most recently added shred in the VM (could be the shred calling this function); returns the ID of the removed shred.");

    // add removeAllShreds
    QUERY->add_sfun( QUERY, machine_removeAll_impl, "void", "removeAllShreds" );
    QUERY->doc_func( QUERY, "Remove all shreds in the VM (including the shred calling this function).");

    // add resetShredID
    QUERY->add_sfun( QUERY, machine_resetID_impl, "int", "resetShredID" );
    QUERY->doc_func( QUERY, "Reset shred IDs to 1 + the highest current shred ID in the VM; can be used as shred management to keep shred IDs low, after a lot of sporks; returns what the next shred ID would be.");

    // add clearVM
    QUERY->add_sfun( QUERY, machine_clearVM_impl, "void", "clearVM" );
    QUERY->doc_func( QUERY, "Reset the type system, removing all user-defined types and all global variables; removes all shreds in the VM (including the shred calling this function); use with care.");

    // add resetOperators
    QUERY->add_sfun( QUERY, machine_opOverloadReset_impl, "void", "resetOperators" );
    QUERY->doc_func( QUERY, "Reset operator overloading state to default startup state; removes all public @operator overloads; use with care.");

    // comment out for now -- since local overloads don't persist, push/pop should probably affect public @operator overloads
    // // add operatorsPush
    // QUERY->add_sfun( QUERY, machine_opOverloadPush_impl, "void", "operatorsPush" );
    // QUERY->doc_func( QUERY, "Push the operator overloading stack; use with care.");
    //
    // // add operatorsPop
    // QUERY->add_sfun( QUERY, machine_opOverloadPop_impl, "void", "operatorsPop" );
    // QUERY->doc_func( QUERY, "Pop the operator overloading stack; use with care.");
    //
    // // add operatorsStackLevel
    // QUERY->add_sfun( QUERY, machine_opOverloadStackLevel_impl, "void", "operatorsStackLevel" );
    // QUERY->doc_func( QUERY, "Get the current operator overloading stack level.");

//    // add testSetup
//    QUERY->add_sfun( QUERY, machine_testSetup_impl, "void", "testSetup" );
//    QUERY->add_arg( QUERY, "Object", "obj" );
//
//    // add testInvoke
//    QUERY->add_sfun( QUERY, machine_testInvoke_impl, "void", "testInvoke" );
//    QUERY->add_arg( QUERY, "Object", "obj" );
//    QUERY->add_arg( QUERY, "float", "arg" );

    // add status (legacy version of printStatus; has return value)
    QUERY->add_sfun( QUERY, machine_printStatus_impl, "int", "status" );
    QUERY->doc_func( QUERY, "Print the current status of the VM; legacy version of printStatus().");

    // add printStatus
    QUERY->add_sfun( QUERY, machine_printStatus_impl, "void", "printStatus" );
    QUERY->doc_func( QUERY, "Print (to terminal or console) the current status of the VM.");

    // add printTimeCheck
    QUERY->add_sfun( QUERY, machine_printTimeCheck_impl, "void", "printTimeCheck" );
    QUERY->doc_func( QUERY, "Print (to terminal or console) the current time information in the VM.");

    // add crash
    //! explicitly crash the virtual machine
    QUERY->add_sfun( QUERY, machine_crash_impl, "void", "crash" );
    QUERY->doc_func( QUERY, "Explicitly crash the virtual machine. The very last resort; or an emphatic gesture. Use with care." );

    // add spork
    //! same as add
    // QUERY->add_sfun( QUERY, machine_add_impl, "int", "spork" );
    // QUERY->add_arg( QUERY, "string", "path" );

    // add shreds
    //! get list of active shreds by id
    QUERY->add_sfun( QUERY, machine_shreds_impl, "int[]", "shreds" );
    QUERY->doc_func( QUERY, "Retrieve an array of active shred ids." );

    // add numShreds
    QUERY->add_sfun( QUERY, machine_numshreds_impl, "int", "numShreds" );
    QUERY->doc_func( QUERY, "Get the number of shreds currently in the VM." );

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
    QUERY->doc_func( QUERY, "return language and VM version string." );

    // get set loglevel
    QUERY->add_sfun( QUERY, machine_setloglevel_impl, "int", "loglevel" );
    QUERY->add_arg( QUERY, "int", "level" );
    QUERY->doc_func( QUERY, "set log level\n"
                     "       |- 0: NONE\n"
                     "       |- 1: CORE\n"
                     "       |- 2: SYSTEM\n"
                     "       |- 3: HERALD\n"
                     "       |- 4: WARNING\n"
                     "       |- 5: INFO\n"
                     "       |- 6: DEBUG\n"
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

    // explicitly trigger garbage collection pass | 1.5.2.0 (ge) added
    // QUERY->add_sfun( QUERY, machine_gc, "void", "gc" );
    // QUERY->doc_func( QUERY, "explicitly trigger a manual VM-level garbage collection pass; this will preemptively clean up eligible objects before automated garbage collection system does. Use with care." );

    // get reg stack pointer
    QUERY->add_sfun( QUERY, machine_regstack_impl, "int", "sp_reg" );
    QUERY->doc_func( QUERY, "get the calling shred's operand (aka \"reg\"/register) stack pointer; intended for either debugging or curiosity." );

    // get mem stack pointer
    QUERY->add_sfun( QUERY, machine_memstack_impl, "int", "sp_mem" );
    QUERY->doc_func( QUERY, "get the calling shred's memory (aka \"mem\") stack pointer; intended for either debugging or curiosity." );

    // add examples
    QUERY->add_ex( QUERY, "machine/eval.ck" );
    QUERY->add_ex( QUERY, "machine/eval-global.ck" );
    QUERY->add_ex( QUERY, "machine/intsize.ck" );
    QUERY->add_ex( QUERY, "machine/is-realtime.ck" );
    QUERY->add_ex( QUERY, "machine/machine-help.ck" );
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

// add
CK_DLL_SFUN( machine_add_impl )
{
    // get arg
    Chuck_String * ckstr = GET_CK_STRING(ARGS);
    // check arg; return (0 by default)
    if( !ckstr ) return;

    // get string
    string path = ckstr->str();
    // filename
    string filename;
    // arguments
    string args;
    // extract args FILE:arg1:arg2:arg3
    extract_args( path, filename, args );

    // compile but don't run yet (instance == 0)
    if( !VM->carrier()->chuck->compileFile( filename, args, 0 ) ) return;

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_ADD;
    // set code
    msg->code = VM->carrier()->compiler->output();
    // create args array
    msg->args = new vector<string>;
    // extract args again but this time into vector
    extract_args( path, filename, *(msg->args) );
    // process ADD message, return new shred ID
    RETURN->v_int = VM->process_msg( msg );
}

// remove
CK_DLL_SFUN( machine_remove_impl )
{
    // get the shred ID to remove
    t_CKINT v = GET_CK_INT(ARGS);

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REMOVE;
    // set id to remove
    msg->param = v;
    // process the message
    RETURN->v_int = VM->process_msg( msg );
}

// removeLast
CK_DLL_SFUN( machine_removeLast_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REMOVE;
    // set special value to signify remove last shred
    msg->param = CK_NO_VALUE;
    // process the message
    RETURN->v_int = VM->process_msg( msg );
}

// removeLast
CK_DLL_SFUN( machine_removeAll_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REMOVEALL;
    // process the message
    VM->process_msg( msg );
}

// replace
CK_DLL_SFUN( machine_replace_impl )
{
    // shred ID to replace
    t_CKINT v = GET_NEXT_INT(ARGS);
    // incoming file
    Chuck_String * ckstr = GET_NEXT_STRING(ARGS);
    // check and if NULL return (0 by default)
    if( !ckstr ) return;

    // get string
    string path = ckstr->str();
    // filename
    string filename;
    // arguments
    string args;
    // extract args FILE:arg1:arg2:arg3
    extract_args( path, filename, args );

    // compile but don't run yet (instance == 0)
    if( !VM->carrier()->chuck->compileFile( filename, args, 0 ) ) return;

    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_REPLACE;
    // set shred id to replace
    msg->param = v;
    // set code for incoming shred
    msg->code = VM->carrier()->compiler->output();
    // create args array
    msg->args = new vector<string>;
    // extract args again but this time into vector
    extract_args( path, filename, *(msg->args) );
    // process ADD message, return new shred ID
    RETURN->v_int = VM->process_msg( msg );
}

// printStatus (and also for status)
CK_DLL_SFUN( machine_printStatus_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_STATUS;
    // process the message; set return for status() which returns an int
    RETURN->v_int = (VM->process_msg( msg ) != 0);
}

// printTimeCheck
CK_DLL_SFUN( machine_printTimeCheck_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_TIME;
    // process the message
    VM->process_msg( msg );
}

// resetID
CK_DLL_SFUN( machine_resetID_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_RESET_ID;
    // process the message
    RETURN->v_int = VM->process_msg( msg );
}

// removeLast
CK_DLL_SFUN( machine_clearVM_impl )
{
    // construct chuck msg (must allocate on heap, as VM will clean up)
    Chuck_Msg * msg = new Chuck_Msg();
    // set type
    msg->type = CK_MSG_CLEARVM;
    // process the message
    VM->process_msg( msg );
}

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
    Chuck_ArrayInt *array = new Chuck_ArrayInt(FALSE);
    initialize_object(array, SHRED->vm_ref->env()->ckt_array, SHRED, VM);
    array->clear();

    Chuck_VM_Status status;
    SHRED->vm_ref->shreduler()->status(&status);

    for(int i = 0; i < status.list.size(); i++)
        array->push_back(status.list[i]->xid);

    status.clear();

    RETURN->v_object = array;
}

CK_DLL_SFUN( machine_numshreds_impl )
{
    // vector of shreds
    vector<Chuck_VM_Shred *> shreds;
    // get all shreds in the VM
    SHRED->vm_ref->shreduler()->get_all_shreds( shreds );
    // return the size
    RETURN->v_int = shreds.size();
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
    initialize_object(s, VM->carrier()->env->ckt_string, SHRED, VM );
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

CK_DLL_SFUN( machine_regstack_impl )
{
    // return reg stack pointer
    RETURN->v_int = (t_CKINT)SHRED->reg->sp;
}

CK_DLL_SFUN( machine_memstack_impl )
{
    // return mem stack pointer
    RETURN->v_int = (t_CKINT)SHRED->mem->sp;
}

// gc() | 1.5.2.0 (ge)
CK_DLL_SFUN( machine_gc )
{
    // invoke
    VM->gc();
}

CK_DLL_SFUN( machine_opOverloadPush_impl)
{
    VM->env()->op_registry.push();
}

CK_DLL_SFUN( machine_opOverloadPop_impl)
{
    VM->env()->op_registry.pop();
}

CK_DLL_SFUN( machine_opOverloadReset_impl)
{
    VM->env()->op_registry.reset2public();
}

CK_DLL_SFUN( machine_opOverloadStackLevel_impl)
{
    RETURN->v_int = VM->env()->op_registry.stackLevel();
}

// add
CK_DLL_SFUN( machine_crash_impl )
{
    CK_FPRINTF_STDERR( "[chuck]: crashing...\n" );
    *(volatile int *)0 = 0;
}


// TEST
//Chuck_VM_MFunInvoker * g_testInvoker;
//
//CK_DLL_SFUN( machine_testSetup_impl )
//{
//    Chuck_Object * obj = GET_NEXT_OBJECT(ARGS);
//
//    g_testInvoker = new Chuck_VM_MFunInvoker;
//
//    t_CKINT func_vt_offset = -1;
//    Chuck_Func * func = NULL;
//
//    for(t_CKINT i = 0; i < obj->vtable->funcs.size(); i++)
//    {
//        func = obj->vtable->funcs[i];
//        if(func->name.find("update") == 0 &&
//           // ensure has one argument
//           func->def()->arg_list != NULL &&
//           // ensure first argument is float
//           func->def()->arg_list->type == SHRED->vm_ref->env()->ckt_float &&
//           // ensure has only one argument
//           func->def()->arg_list->next == NULL &&
//           // ensure returns float
//           func->def()->ret_type == SHRED->vm_ref->env()->ckt_void )
//        {
//            func_vt_offset = i;
//            break;
//        }
//    }
//
//    // not found
//    if( func_vt_offset < 0 )
//    {
//        EM_error3( "update() not found yo" );
//        return;
//    }
//
//    // set up invoker
//    g_testInvoker->setup( func, func_vt_offset, VM, SHRED );
//}
//
//CK_DLL_SFUN( machine_testInvoke_impl )
//{
//    Chuck_Object * obj = GET_NEXT_OBJECT(ARGS);
//    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
//
//    Chuck_DL_Arg arg;
//    arg.kind = kindof_FLOAT;
//    arg.value.v_float = v;
//    vector<Chuck_DL_Arg> args;
//    args.push_back(arg);
//
//    g_testInvoker->invoke(obj, args );
//
//    CK_SAFE_DELETE( g_testInvoker );
//}
