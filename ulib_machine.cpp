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
// file: ulib_machine.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_machine.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_globals.h"




//-----------------------------------------------------------------------------
// name: machine_query()
// desc: query entry point
//-----------------------------------------------------------------------------
DLL_QUERY machine_query( Chuck_DL_Query * QUERY )
{
    // get global env instance
    Chuck_Env * env = Chuck_Env::instance();
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

    // end class
    QUERY->end_class( QUERY );

    return TRUE;
}




static Chuck_VM * the_vm = NULL;
static Chuck_Compiler * the_compiler = NULL;
static proc_msg_func the_func = NULL;
//-----------------------------------------------------------------------------
// name: machine_init()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL machine_init( Chuck_Compiler * compiler, proc_msg_func proc_msg )
{
    the_vm = g_vm;
    the_compiler = compiler;
    the_func = proc_msg;

    return TRUE;
}


// add
CK_DLL_SFUN( machine_crash_impl )
{
    fprintf( stderr, "[chuck]: crashing...\n" );
    *(int *)0 = 0;
}


// add
CK_DLL_SFUN( machine_add_impl )
{
    const char * v = GET_CK_STRING(ARGS)->str.c_str();
    Net_Msg msg;

    msg.type = MSG_ADD;
    strcpy( msg.buffer, v );
    RETURN->v_int = (int)the_func( the_vm, the_compiler, &msg, TRUE, NULL );
}

// remove
CK_DLL_SFUN( machine_remove_impl )
{
    t_CKINT v = GET_CK_INT(ARGS);
    Net_Msg msg;
    
    msg.type = MSG_REMOVE;
    msg.param = v;
    RETURN->v_int = (int)the_func( the_vm, the_compiler, &msg, TRUE, NULL );
}

// replace
CK_DLL_SFUN( machine_replace_impl )
{
    t_CKINT v = GET_NEXT_INT(ARGS);
    const char * v2 = GET_NEXT_STRING(ARGS)->str.c_str();
    Net_Msg msg;
    
    msg.type = MSG_REPLACE;
    msg.param = v;
    strcpy( msg.buffer, v2 );
    RETURN->v_int = (int)the_func( the_vm, the_compiler, &msg, TRUE, NULL );
}

// status
CK_DLL_SFUN( machine_status_impl )
{
    Net_Msg msg;
    
    msg.type = MSG_STATUS;
    RETURN->v_int = (int)the_func( the_vm, the_compiler, &msg, TRUE, NULL );
}
