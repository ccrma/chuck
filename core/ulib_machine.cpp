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




//-----------------------------------------------------------------------------
// name: machine_query()
// desc: query entry point
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

    // add add
    //! compile and spork a new shred from file at 'path' into the VM now
    //! returns the shred ID
    //! (see example/machine.ck)
    QUERY->add_sfun( QUERY, machine_add_impl, "int", "add" );
    QUERY->add_arg( QUERY, "string", "path" );
    
    // add spork
    //! same as add
    // QUERY->add_sfun( QUERY, machine_add_impl, "int", "spork" );
    // QUERY->add_arg( QUERY, "string", "path" );
    
    // add remove
    //! remove shred from VM by shred ID (returned by add/spork)
    QUERY->add_sfun( QUERY, machine_remove_impl, "int", "remove" );
    QUERY->add_arg( QUERY, "int", "id" );

    // add replace
    //! replace shred with new shred from file
    //! returns shred ID , or 0 on error 
    QUERY->add_sfun( QUERY, machine_replace_impl, "int", "replace" );
    QUERY->add_arg( QUERY, "int", "id" );
    QUERY->add_arg( QUERY, "string", "path" );

    // add replace
    //! replace shred with new shred from file
    //! returns shred ID , or 0 on error 
    QUERY->add_sfun( QUERY, machine_crash_impl, "void", "crash" );

    // add status
    //! display current status of VM
    //! (see example/status.ck)
    QUERY->add_sfun( QUERY, machine_status_impl, "int", "status" );
    
    // add get intsize (width)
    //! get the intsize in bits (e.g., 32 or 64)
    QUERY->add_sfun( QUERY, machine_intsize_impl, "int", "intsize" );
    
    // add shreds
    //! get list of active shreds by id
    QUERY->add_sfun( QUERY, machine_shreds_impl, "int[]", "shreds" );

    // end class
    QUERY->end_class( QUERY );

    return TRUE;
}




static Chuck_Compiler * the_compiler = NULL;
static proc_msg_func the_func = NULL;
//-----------------------------------------------------------------------------
// name: machine_init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL machine_init( Chuck_Compiler * compiler, proc_msg_func proc_msg )
{
    the_compiler = compiler;
    the_func = proc_msg;

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




// add
CK_DLL_SFUN( machine_crash_impl )
{
    CK_FPRINTF_STDERR( "[chuck]: crashing...\n" );
    *(volatile int *)0 = 0;
}

// add
CK_DLL_SFUN( machine_add_impl )
{
    const char * v = GET_CK_STRING(ARGS)->str().c_str();
    Net_Msg msg;

    msg.type = MSG_ADD;
    strcpy( msg.buffer, v );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// remove
CK_DLL_SFUN( machine_remove_impl )
{
    t_CKINT v = GET_CK_INT(ARGS);
    Net_Msg msg;
    
    msg.type = MSG_REMOVE;
    msg.param = v;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// replace
CK_DLL_SFUN( machine_replace_impl )
{
    t_CKINT v = GET_NEXT_INT(ARGS);
    const char * v2 = GET_NEXT_STRING(ARGS)->str().c_str();
    Net_Msg msg;
    
    msg.type = MSG_REPLACE;
    msg.param = v;
    strcpy( msg.buffer, v2 );
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// status
CK_DLL_SFUN( machine_status_impl )
{
    Net_Msg msg;
    
    msg.type = MSG_STATUS;
    RETURN->v_int = (int)the_func( SHRED->vm_ref, the_compiler, &msg, TRUE, NULL );
}

// intsize
CK_DLL_SFUN( machine_intsize_impl )
{
    RETURN->v_int = machine_intsize();
}

CK_DLL_SFUN( machine_shreds_impl )
{
    Chuck_Array4 *array = new Chuck_Array4(FALSE);
    initialize_object(array, SHRED->vm_ref->env()->t_array);
    array->clear();
    
    Chuck_VM_Status status;
    SHRED->vm_ref->shreduler()->status(&status);
    
    for(int i = 0; i < status.list.size(); i++)
        array->push_back(status.list[i]->xid);
    
    status.clear();
    
    RETURN->v_object = array;
}
