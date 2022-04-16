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
// name: chuck_lang.cpp
// desc: chuck class library base
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//          Spencer Salazar (spencer@ccrma.stanford.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Andrew Schran (aschran@princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#include "chuck_lang.h"
#include "chuck_type.h"
#include "chuck_instr.h"
#include "chuck_vm.h"
#include "chuck_errmsg.h"
#include "chuck_ugen.h"
#include "util_string.h"

#include <math.h>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// dac tick
CK_DLL_TICK(__ugen_tick) { *out = in; return TRUE; }
// object string offset
static t_CKUINT Object_offset_string = 0;




//-----------------------------------------------------------------------------
// name: init_class_object()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_object( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_SEVERE, "class 'Object'" );
    
    const char * doc = "base class for all class types in ChucK.";
    
    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), object_ctor, object_dtor, doc ) )
        return FALSE;

    // add member to hold string
    Object_offset_string = type_engine_import_mvar( env, "string", "@string", FALSE );
    if( Object_offset_string == CK_INVALID_OFFSET ) goto error;

    // add toString()
    func = make_new_mfun( "string", "toString", object_toString );
    func->doc = "get a textual description of this object.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add help()
    func = make_new_sfun( "void", "help", object_help );
    func->doc = "generate and output helpful information about a class or object.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

//    // add dump()
//    func = make_new_mfun( "void", "dump", object_dump );
//    func->doc = "output current state of the object.";
//    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_ugen()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_ugen( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;

    EM_log( CK_LOG_SEVERE, "class 'UGen'" );

    // add ugen info
    type->ugen_info = new Chuck_UGen_Info;
    type->ugen_info->add_ref();
    type->ugen_info->tick = __ugen_tick;
    type->ugen_info->num_ins = 1;
    type->ugen_info->num_outs = 1;

    const char * doc = "base class for all unit generator (UGen) types in ChucK.";

    // init as base class
    // TODO: ctor/dtor, ugen's sometimes created internally?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;

    // gain
    func = make_new_mfun( "float", "gain", ugen_gain );
    func->add_arg( "float", "val" );
    func->doc = "set the gain of the unit generator.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "gain", ugen_cget_gain );
    func->doc = "get the gain of the unit generator.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

/*    // next
    func = make_new_mfun( "float", "next", ugen_next );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "next", ugen_cget_next );
    if( !type_engine_import_mfun( env, func ) ) goto error;
*/
    // last
    func = make_new_mfun( "float", "last", ugen_last );
    func->doc = "get the last sample value of the unit generator.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // op
    func = make_new_mfun( "int", "op", ugen_op );
    func->add_arg( "int", "val" );
    func->doc = "set the unit generator's operation mode. Accepted values are: 1 (sum inputs), 2 (take difference between first input and subsequent inputs), 3 (multiply inputs), 4 (divide first input by subsequent inputs), 0 (do not synthesize audio, output 0) or -1 (passthrough inputs to output).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "op", ugen_cget_op );
    func->doc = "get the unit generator's operation mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add numChannels
    func = make_new_mfun( "int", "channels", ugen_numChannels );
    func->add_arg( "int", "num" );
    func->doc = "set number of channels. (currently NOT supported)";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "channels", ugen_cget_numChannels );
    func->doc = "get number of channels.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add chan
    func = make_new_mfun( "UGen", "chan", ugen_chan );
    func->add_arg( "int", "num" );
    func->doc = "get channel (as a UGen) at specified index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isConnectedTo
    func = make_new_mfun( "int", "isConnectedTo", ugen_connected );
    func->add_arg( "UGen", "right" );
    func->doc = "return true if this UGen's output is connected to the input of the argument; return false otherwise.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add buffered
    func = make_new_mfun( "int", "buffered", ugen_buffered );
    func->add_arg( "int", "val" );
    func->doc = "set the unit generator's buffered operation mode, typically used externally from hosts that embed ChucK as a component. If true, the UGen stores a buffer of its most recent samples, which can be fetched using global variables in the host language.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "buffered", ugen_cget_buffered );
    func->doc = "get the ugen's buffered operation mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




// virtual table offset
static t_CKINT uana_offset_blob = 0;
//-----------------------------------------------------------------------------
// name: init_class_uana()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_uana( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;

    EM_log( CK_LOG_SEVERE, "class 'UAna'" );

    // add uana info
    type->ugen_info = new Chuck_UGen_Info;
    type->ugen_info->add_ref();
    type->ugen_info->tick = __ugen_tick;
    type->ugen_info->num_ins = 1;
    type->ugen_info->num_outs = 1;

    const char * doc = "base class from which all unit analyzer (UAna) type inherit; UAnae (note plural form) can be interconnected using => (standard chuck operator) or using =^ (upchuck operator), specify the the types of and when data is passed between UAnae and UGens.  When .upchuck() is invoked on a given UAna, the UAna-chain (UAnae connected via =^) is traversed backwards from the upchucked UAna, and analysis is performed at each UAna along the chain; the updated analysis results are stored in UAnaBlobs.";

    // init as base class, type should already know the parent type
    // TODO: ctor/dtor, ugen's sometimes created internally?
    if( !type_engine_import_class_begin( env, type, env->global(), uana_ctor, uana_dtor, doc ) )
        return FALSE;

    // add variables
    uana_offset_blob = type_engine_import_mvar( env, "UAnaBlob", "m_blob", FALSE );
    if( uana_offset_blob == CK_INVALID_OFFSET ) goto error;

    // add upchuck
    func = make_new_mfun( "UAnaBlob", "upchuck", uana_upchuck );
    func->doc = "initiate analysis at the UAna and return result.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add fvals
    func = make_new_mfun( "float[]", "fvals", uana_fvals );
    func->doc = "get blob's float array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cvals
    func = make_new_mfun( "complex[]", "cvals", uana_cvals );
    func->doc = "get blob's complex array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add fval
    func = make_new_mfun( "float", "fval", uana_fval );
    func->add_arg( "int", "index" );
    func->doc = "get blob's float value at index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add cval
    func = make_new_mfun( "complex", "cval", uana_cval );
    func->add_arg( "int", "index" );
    func->doc = "get blob's complex value at index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isUpConnectedTo
    func = make_new_mfun( "int", "isUpConnectedTo", uana_connected );
    func->add_arg( "UAna", "right" );
    func->doc = "is connected to another uana via =^?";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // TODO: add nonchuck
    // func = make_new_mfun( "void", "nonchuck", uana_nonchuck );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // TODO: add event
    // func = make_new_mfun( "Event", "event", uana_event );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // end
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




// virtual table offset
static t_CKINT uanablob_offset_when = 0;
static t_CKINT uanablob_offset_fvals = 0;
static t_CKINT uanablob_offset_cvals = 0;
//-----------------------------------------------------------------------------
// name: init_class_blob()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_blob( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;
    // Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'UAnaBlob'" );
    
    const char * doc = "contains results associated with UAna analysis. There is a UAnaBlob associated with every UAna. As a UAna is upchucked, the result is stored in the UAnaBlob's floating point vector and/or complex vector. The intended interpretation of the results depends on the specific UAna.";
    
    // init class
    // TODO: ctor/dtor
    if( !type_engine_import_class_begin( env, type, env->global(), uanablob_ctor, uanablob_dtor, doc ) )
        return FALSE;

    // add variables
    uanablob_offset_when = type_engine_import_mvar( env, "time", "m_when", FALSE );
    if( uanablob_offset_when == CK_INVALID_OFFSET ) goto error;
    uanablob_offset_fvals = type_engine_import_mvar( env, "float[]", "m_fvals", FALSE );
    if( uanablob_offset_fvals == CK_INVALID_OFFSET ) goto error;
    uanablob_offset_cvals = type_engine_import_mvar( env, "complex[]", "m_cvals", FALSE );
    if( uanablob_offset_cvals == CK_INVALID_OFFSET ) goto error;

    // add when
    func = make_new_mfun( "time", "when", uanablob_when );
    func->doc = "get the time when blob was last upchucked.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add fvals
    func = make_new_mfun( "float[]", "fvals", uanablob_fvals );
    func->doc = "get blob's float array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cvals
    func = make_new_mfun( "complex[]", "cvals", uanablob_cvals );
    func->doc = "get blob's complex array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add fval
    func = make_new_mfun( "float", "fval", uanablob_fval );
    func->add_arg( "int", "index" );
    func->doc = "get blob's float value at index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cval
    func = make_new_mfun( "complex", "cval", uanablob_cval );
    func->add_arg( "int", "index" );
    func->doc = "get blob's complex value at index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end class import
    type_engine_import_class_end( env );
    
    // done
    return TRUE;

error:

    // end class import
    type_engine_import_class_end( env );
    
    // error
    return FALSE;
}




// static t_CKUINT event_offset_data = 0;
//-----------------------------------------------------------------------------
// name: init_class_event()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_event( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_SEVERE, "class 'Event'" );
    
    const char *doc = "a mechanism for precise synchronization across shreds.";

    // init as base class
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;

    // add signal()
    func = make_new_mfun( "void", "signal", event_signal );
    func->doc = "signal one shred that is waiting on this event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add broadcast()
    func = make_new_mfun( "void", "broadcast", event_broadcast );
    func->doc = "signal all shreds that are waiting on this event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add wait() | commented out 1.4.1.0 (ge) -- it doesn't DO anything!!!
    // func = make_new_mfun( "void", "wait", event_wait );
    // func->add_arg( "Shred", "me" );
    // func->doc = "(currently unsupported; use Event => now to wait on event)";
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add can_wait()
    func = make_new_mfun( "int", "can_wait", event_can_wait );
    func->doc = "can the event can be waited on? Currently always returns true.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // find the offset for can_wait
    value = type_engine_find_value( type, "can_wait" );
    assert( value != NULL );
    assert( value->func_ref != NULL );
    // remember it
    Chuck_Event::our_can_wait = value->func_ref->vt_index;
    
    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




// static t_CKUINT shred_offset_args = 0;
//-----------------------------------------------------------------------------
// name: init_class_shred()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_shred( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_SEVERE, "class 'Shred'" );

    const char *doc = "a strongly-timed ChucK thread of execution.";
    
    // init as base class
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;
    
    // add dtor
    // not

    // add fromId()
    func = make_new_sfun( "Shred", "fromId", shred_fromId );
    func->add_arg( "int", "id" );
    func->doc = "get Shred corresponding to a Shred ID.";
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    // add clone()
    func = make_new_mfun( "void", "clone", shred_clone );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add exit()
    func = make_new_mfun( "void", "exit", shred_exit );
    func->doc = "halt the shred's operation and remove it from the virtual machine.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add id()
    func = make_new_mfun( "int", "id", shred_id );
    func->doc = "get the unique numeric id of the Shred.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add yield()
    func = make_new_mfun( "void", "yield", shred_yield );
    func->doc = "cause the current Shred to temporarily suspend without advancing time, allowing other simultaneously schreduled shreds to run as needed. NOTE: yield() is equivalent to '0::second +=> now;'";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add running()
    func = make_new_mfun( "int", "running", shred_running );
    func->doc = "is the Shred currently running?";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add done()
    func = make_new_mfun( "int", "done", shred_done );
    func->doc = "is the Shred done running?";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add nargs()
    // func = make_new_mfun( "int", "numArgs", shred_numArgs );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add nargs()
    func = make_new_mfun( "int", "args", shred_numArgs );
    func->doc = "get the number of arguments provided to the Shred.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add arg()
    // func = make_new_mfun( "string", "getArg", shred_getArg );
    // func->add_arg( "int", "index" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add arg()
    func = make_new_mfun( "string", "arg", shred_getArg );
    func->add_arg( "int", "index" );
    func->doc = "get the Shred argument at the specified index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add sourcePath() (added 1.3.0.0)
    func = make_new_mfun( "string", "sourcePath", shred_sourcePath );
    func->doc = "get the path of the source file from which this Shred's code is derived (same as .path()).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add path() (added 1.3.2.0)
    func = make_new_mfun( "string", "path", shred_sourcePath );
    func->doc = "get the path of the source file from which this Shred's code is derived (same as .sourcePath()).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add sourceDir() (added 1.3.0.0)
    func = make_new_mfun( "string", "sourceDir", shred_sourceDir );
    func->doc = "get the enclosing directory of the source file from which this Shred's code is derived (same as .dir()).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add dir() (added 1.3.2.0)
    func = make_new_mfun( "string", "dir", shred_sourceDir );
    func->doc = "get the enclosing directory of the source file from which this Shred's code is derived (same as .sourceDir()).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add dir() (added 1.3.2.0, ge)
    func = make_new_mfun( "string", "dir", shred_sourceDir2 );
    func->add_arg( "int", "levelsUp" );
    func->doc = "get the enclosing directory, the specified number of parent directories up.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_vec3()
// desc: initialize primitive type vec3
//-----------------------------------------------------------------------------
t_CKBOOL init_class_vec3( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'vec3' (primitive)" );

    // document
    const char *doc = "a primitive type for a 3-dimensional vector; potentially useful for 3D coordinate, RGB color, or as a value/goal/slew interpolator.";
    
    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;
    
    // add set(float,float,float)
    func = make_new_mfun( "void", "set", vec3_set );
    func->add_arg( "float", "x" );
    func->add_arg( "float", "y" );
    func->add_arg( "float", "z" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add setAll(float)
    func = make_new_mfun( "void", "setAll", vec3_setAll );
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add magnitude()
    func = make_new_mfun( "float", "magnitude", vec3_magnitude );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add normalize()
    func = make_new_mfun( "void", "normalize", vec3_normalize );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add interp()
    func = make_new_mfun( "float", "interp", vec3_interp );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add interp( float )
    func = make_new_mfun( "float", "interp", vec3_interp_delta_float );
    func->add_arg( "float", "delta" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add interp( dur )
    func = make_new_mfun( "float", "interp", vec3_interp_delta_dur );
    func->add_arg( "dur", "delta" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add update( float )
    func = make_new_mfun( "void", "update", vec3_update_goal );
    func->add_arg( "float", "goal" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add update( float, float )
    func = make_new_mfun( "void", "update", vec3_update_goal_slew );
    func->add_arg( "float", "goal" );
    func->add_arg( "float", "slew" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add update( float )
    func = make_new_mfun( "void", "updateSet", vec3_updateSet_goalAndValue );
    func->add_arg( "float", "goalAndValue" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add update( float, float )
    func = make_new_mfun( "void", "updateSet", vec3_updateSet_goalAndValue_slew );
    func->add_arg( "float", "goal" );
    func->add_arg( "float", "slew" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;
    
error:
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_vec4()
// desc: initialize primitive type vec4
//-----------------------------------------------------------------------------
t_CKBOOL init_class_vec4( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'vec4' (primitive)" );
    
    // document
    const char *doc = "a primitive type for a 4-dimensional vector; potentially useful for 4D coordinate and RGBA color.";
    
    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;
    
    // add set(float,float,float)
    func = make_new_mfun( "void", "set", vec4_set );
    func->add_arg( "float", "x" );
    func->add_arg( "float", "y" );
    func->add_arg( "float", "z" );
    func->add_arg( "float", "w" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add setAll(float)
    func = make_new_mfun( "void", "setAll", vec4_setAll );
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add magnitude()
    func = make_new_mfun( "float", "magnitude", vec4_magnitude );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add normalize()
    func = make_new_mfun( "void", "normalize", vec4_normalize );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_string()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_string( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_SEVERE, "class 'string'" );

    const char * doc = "textual data as a sequence of characters, along with functions for manipulating text.";

    // init as base class
    // TODO: ctor/dtor
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;

    // add length()
    func = make_new_mfun( "int", "length", string_length );
    func->doc = "get the number of characters of the string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add upper()
    func = make_new_mfun( "string", "upper", string_upper );
    func->doc = "get a new string in which the lowercase characters of the original string have been converted to uppercase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add lower()
    func = make_new_mfun( "string", "lower", string_lower );
    func->doc = "get a new string in which the uppercase characters of the original string have been converted to lowercase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ltrim()
    func = make_new_mfun( "string", "ltrim", string_ltrim );
    func->doc = "get a new string in which leading whitespace has been removed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add rtrim()
    func = make_new_mfun( "string", "rtrim", string_rtrim );
    func->doc = "get a new string in which trailing whitespace has been removed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add trim()
    func = make_new_mfun( "string", "trim", string_trim );
    func->doc = "get a new string in which leading and trailing whitespace has been removed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add toString()
    func = make_new_mfun( "string", "toString", string_toString );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add charAt()
    func = make_new_mfun( "int", "charAt", string_charAt );
    func->add_arg("int", "index");
    func->doc = "get a character at the specified index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add setCharAt()
    func = make_new_mfun( "int", "setCharAt", string_setCharAt );
    func->add_arg("int", "index");
    func->add_arg("int", "theChar");
    func->doc = "set the character at the specified index.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add substring()
    func = make_new_mfun( "string", "substring", string_substring );
    func->add_arg("int", "start");
    func->doc = "get a new string containing the substring from the start index to the end of the string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add substring()
    func = make_new_mfun( "string", "substring", string_substringN );
    func->add_arg("int", "start");
    func->add_arg("int", "length");
    func->doc = "get a new string containing the substring from the start index of the specified length.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add insert()
    func = make_new_mfun( "void", "insert", string_insert );
    func->add_arg("int", "position");
    func->add_arg("string", "str");
    func->doc = "insert a string at the specified position.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add replace()
    func = make_new_mfun( "void", "replace", string_replace );
    func->add_arg( "int", "position" );
    func->add_arg( "string", "str" );
    func->doc = "get characters from the start position to the end of the string with str.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add replace()
    func = make_new_mfun( "void", "replace", string_replaceN );
    func->add_arg( "int", "position" );
    func->add_arg( "int", "length" );
    func->add_arg( "string", "str" );
    func->doc = "get length characters from the start position with str.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add find()
    func = make_new_mfun( "int", "find", string_find );
    func->add_arg( "int", "theChar" );
    func->doc = "get the index of the first occurence of theChar, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add find()
    func = make_new_mfun( "int", "find", string_findStart );
    func->add_arg( "int", "theChar" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the first occurence of theChar at or after the start position, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add find()
    func = make_new_mfun( "int", "find", string_findStr );
    func->add_arg( "string", "str" );
    func->doc = "get the index of the first occurence of str, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add find()
    func = make_new_mfun( "int", "find", string_findStrStart );
    func->add_arg( "string", "str" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the first occurence of str at or after the start position, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfind );
    func->add_arg( "int", "theChar" );
    func->doc = "get the index of the last occurence of theChar, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStart );
    func->add_arg( "int", "theChar" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the last occurence of theChar at or before the start position, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStr );
    func->add_arg( "string", "str" );
    func->doc = "get the index of the last occurence of str, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStrStart );
    func->add_arg( "string", "str" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the last occurence of str at or before the start position, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add erase()
    func = make_new_mfun( "void", "erase", string_erase );
    func->add_arg( "int", "start" );
    func->add_arg( "int", "length" );
    func->doc = "erase length characters of the string from start position.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toInt()
    func = make_new_mfun( "int", "toInt", string_toInt );
    func->doc = "attemp to convert the contents of the string to an integer and return the result, or 0 if conversion failed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toFloat()
    func = make_new_mfun( "float", "toFloat", string_toFloat );
    func->doc = "Attempt to convert the contents of the string to an float and return the result, or 0 if conversion failed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add parent()
    // disable for now
//    func = make_new_mfun( "string", "parent", string_parent );
//    if( !type_engine_import_mfun( env, func ) ) goto error;
    
//    // add toTime()
//    func = make_new_mfun( "float", "toTime", string_toFloat );
//    if( !type_engine_import_mfun( env, func ) ) goto error;
//
//    // add toDur()
//    func = make_new_mfun( "float", "toDur", string_toFloat );
//    if( !type_engine_import_mfun( env, func ) ) goto error;
    
/*    // add at()
    func = make_new_mfun( "int", "ch", string_set_at );
    func->add_arg( "int", "index" );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "ch", string_get_at );
    func->add_arg( "int", "index" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
*/
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_array()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_array( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_SEVERE, "class 'array'" );

    // init as base class
    // TODO: ctor/dtor?
    string doc = "storage construct for sequential data of the same type; can also be used as an associative map data structure; also can be used as a stack with << operator to append/push and popBack to pop.";
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc.c_str() ) )
        return FALSE;

    // add clear()
    func = make_new_mfun( "void", "clear", array_clear );
    func->doc = "clear the contents of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add reset()
    func = make_new_mfun( "void", "reset", array_reset );
    func->doc = "reset array size to 0, set capacity to (at least) 8.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add popBack()
    func = make_new_mfun( "void", "popBack", array_pop_back );
    func->doc = "remove the last item of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add popOut()
    func = make_new_mfun( "void", "popOut", array_pop_out );
    func->add_arg("int", "position");
    func->doc = "Removes the item with position from the array";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add size()
    func = make_new_mfun( "int", "size", array_get_size );
    func->doc = "get the number of elements in the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add size()
    func = make_new_mfun( "int", "size", array_set_size );
    func->add_arg( "int", "newSize" );
    func->doc = "set the size of the array. If the new size is less than the current size, elements will be deleted from the end; if the new size is larger than the current size, 0 or null elements will be added to the end.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // note 1.4.1.0: in the future, should deprecate and encourage programmer
    // to explicitly use .size() OR .capacity(), depending on the context
    // ---------
    // add cap()
    func = make_new_mfun( "int", "cap", array_get_capacity_hack );
    func->doc = "(deprecated) For historical/compatibilty reasons, .cap() is always equal to .size(); instead of using .cap(), it is recommended to explicitly use .size() or .capacity().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add capacity() // 1.4.1.0
    func = make_new_mfun( "int", "capacity", array_set_capacity );
    func->doc = "ensure capacity of the array (number of addressable elements).";
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "capacity", array_get_capacity );
    func->doc = "get current capacity of the array (number of addressable elements).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_mfun( "int", "find", array_find );
    func->add_arg( "string", "key" );
    func->doc = "(map only) Get number of elements with the specified key.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add erase()
    func = make_new_mfun( "int", "erase", array_erase );
    func->add_arg( "string", "key" );
    func->doc = "(map only) Erase all elements with the specified key.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "array/array_argument.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_assign.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_dynamic.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_mdim.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_mmixed.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_resize.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_storage.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_sub_assign.ck" ) ) goto error;
    
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




// Object ctor
CK_DLL_CTOR( object_ctor )
{
    // log
    // EM_log( CK_LOG_FINEST, "Object constructor..." );

    // initialize
    OBJ_MEMBER_UINT(SELF, Object_offset_string) = 0;
}


// Object dtor
CK_DLL_DTOR( object_dtor )
{
    // log
    // EM_log( CK_LOG_FINEST, "Object destructor..." );

    // get the string
    Chuck_String * str = (Chuck_String *)OBJ_MEMBER_UINT(SELF, Object_offset_string);
    // release
    SAFE_RELEASE( str );
}


// toString
CK_DLL_MFUN( object_toString )
{
    // get the string
    Chuck_String * str = (Chuck_String *)OBJ_MEMBER_UINT(SELF, Object_offset_string);
    // allocate
    if( !str )
    {
        // new it
        str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
        // check it
        if( !str )
        {
            // TODO: make this exception
            CK_FPRINTF_STDERR( "[chuck]: Object.toString() out of memory!\n" );
            RETURN->v_object = NULL;
            return;
        }
        // set it
        ostringstream strout( ostringstream::out );
        // get the type
        Chuck_Type * type = SELF->type_ref;
        // write
        strout.setf( ios::hex, ios::basefield );
        strout << ((type != NULL) ? type->c_name() : "[VOID]") << ":" << (t_CKUINT)SELF;
        strout.flush();

        // done
        str->set( strout.str() );
    }

    // set return
    RETURN->v_object = str;
}

// dump
CK_DLL_MFUN( object_dump )
{
    // get object
    Chuck_Object * me = SELF;
    // call dump
    me->dump();
}

// help
CK_DLL_SFUN( object_help )
{
    TYPE->apropos();
    // get object
    // Chuck_Object * me = SELF;
    // call dump
    // me->apropos();
}




// ctor
CK_DLL_CTOR( ugen_ctor )
{
}

CK_DLL_DTOR( ugen_dtor )
{
    // get as ugen
    // Chuck_UGen * ugen = (Chuck_UGen *)SELF;
}

CK_DLL_MFUN( ugen_op )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // get arg
    t_CKINT op = GET_CK_INT( ARGS );
    // set op
    ugen->m_op = op;
    // set return
    RETURN->v_int = ugen->m_op;

    // for multiple channels
    Chuck_DL_Return ret;
    // added 1.3.0.0 -- Chuck_DL_Api::Api::instance()
    for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
        ugen_op( ugen->m_multi_chan[i], ARGS, &ret, VM, SHRED, API );
    // added 1.3.0.2 -- apply op to subgraph outlet
    if( ugen->inlet() )
        ugen->inlet()->m_op = op;
    if( ugen->outlet() )
        ugen->outlet()->m_op = op;
}

CK_DLL_MFUN( ugen_cget_op )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // set return
    RETURN->v_int = ugen->m_op;
}

CK_DLL_MFUN( ugen_last )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // set return
    // added 1.3.0.2 -- ugen outlet() last if subgraph
    if( ugen->outlet() )
        RETURN->v_float = (t_CKFLOAT) ugen->outlet()->m_last;
    else
        RETURN->v_float = (t_CKFLOAT)ugen->m_last;
}

CK_DLL_MFUN( ugen_next )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // get arg
    t_CKFLOAT next = GET_CK_FLOAT( ARGS );
    // set op
    ugen->m_next = (SAMPLE)next;
    ugen->m_use_next = TRUE;
    // set return
    RETURN->v_float = (t_CKFLOAT)ugen->m_next;

    // for multiple channels
    Chuck_DL_Return ret;
    // added 1.3.0.0 -- Chuck_DL_Api::Api::instance()
    for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
        ugen_next( ugen->m_multi_chan[i], ARGS, &ret, VM, SHRED, API );
    // added 1.3.0.2 -- apply op to subgraph outlet
    if( ugen->outlet() )
    {
        ugen->outlet()->m_next = (SAMPLE) next;
        ugen->outlet()->m_use_next = TRUE;
    }
}

CK_DLL_MFUN( ugen_cget_next )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // set return
    RETURN->v_float = ugen->m_use_next ? (t_CKFLOAT)ugen->m_next : 0.0f;
}

CK_DLL_MFUN( ugen_gain )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // get arg
    t_CKFLOAT gain = GET_CK_FLOAT( ARGS );
    // set op
    ugen->m_gain = (SAMPLE)gain;
    // set return
    RETURN->v_float = (t_CKFLOAT)ugen->m_gain;

    // for multiple channels
    Chuck_DL_Return ret;
    // added 1.3.0.0 -- Chuck_DL_Api::Api::instance()
    for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
        ugen_gain( ugen->m_multi_chan[i], ARGS, &ret, VM, SHRED, API );
    // added 1.3.0.2 -- apply gain to subgraph outlet
    if( ugen->outlet() )
        ugen->outlet()->m_gain = (SAMPLE) gain;
}

CK_DLL_MFUN( ugen_cget_gain )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // set return
    RETURN->v_float = (t_CKFLOAT)ugen->m_gain;
}

CK_DLL_CTRL( ugen_numChannels )
{
    // get ugen
    //Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // error
    EM_error3( "setting .numChannels is not yet supported (use the command line)..." );
}

CK_DLL_CGET( ugen_cget_numChannels )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // return
    RETURN->v_int = ugen->m_multi_chan_size == 0 ? 1 : ugen->m_multi_chan_size;
}

CK_DLL_CTRL( ugen_chan )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    t_CKINT num = GET_NEXT_INT(ARGS);
    // check
    if( !ugen->m_multi_chan_size && num == 0 )
        RETURN->v_object = ugen;
    else if( num >= 0 && num < ugen->m_multi_chan_size )
        RETURN->v_object = ugen->m_multi_chan[num];
    else
        RETURN->v_object = NULL;
}

CK_DLL_CTRL( ugen_connected )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    Chuck_UGen * right = (Chuck_UGen *)GET_NEXT_OBJECT(ARGS);

    // sanity
    t_CKINT ret = FALSE;
    if( !right )
    {
        ret = FALSE;
    }
    else
    {
        ret = right->is_connected_from( ugen );
    }

    RETURN->v_int = ret;
}

// 1.4.1.0 (jack): "buffered" flag -- necessary for getUGenSamples()
// for global UGens
CK_DLL_MFUN( ugen_buffered )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // get arg
    t_CKINT buffered = GET_CK_INT( ARGS );
    // set op
    ugen->set_is_buffered( buffered );
    // set return
    RETURN->v_int = ugen->m_is_buffered;

    // for multiple channels
    Chuck_DL_Return ret;
    for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
        ugen_buffered( ugen->m_multi_chan[i], ARGS, &ret, VM, SHRED, API );
}

CK_DLL_MFUN( ugen_cget_buffered )
{
    // get as ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // set return
    RETURN->v_int = ugen->m_is_buffered;
}


// ctor
CK_DLL_CTOR( uana_ctor )
{
    // make an actual blob
    Chuck_Object * blob = instantiate_and_initialize_object( SHRED->vm_ref->env()->t_uanablob, SHRED );
    // TODO: check out of memory
    assert( blob != NULL );
    // make a blob proxy
    Chuck_UAnaBlobProxy * proxy = new Chuck_UAnaBlobProxy( blob );
    // remember it
    OBJ_MEMBER_INT(SELF, uana_offset_blob) = (t_CKINT)proxy;
    // HACK: DANGER: manually call blob's ctor (added 1.3.0.0 -- Chuck_DL_Api::Api::instance())
    uanablob_ctor( blob, NULL, VM, SHRED, API );
}

CK_DLL_DTOR( uana_dtor )
{
    // TODO: GC should release the blob!
}

CK_DLL_MFUN( uana_upchuck )
{
    // get as uana
    Chuck_UAna * uana = (Chuck_UAna *)SELF;
    // get shred
    Chuck_VM_Shred * derhs = SHRED;
    // make sure it's not NULL
    if( !derhs )
    {
        EM_error3( "UAna.upchuck() encountered NULL shred; operation aborting!" );
        return;
    }
    
    // get VM
    Chuck_VM * vm = derhs->vm_ref;
    // ensure
    if( !vm )
    {
        EM_error3( "UAna.upchuck() encountered NULL VM ref; operation aborting!" );
        return;
    }

    // check if time
    if( uana->m_uana_time < vm->shreduler()->now_system )
    {
        // for multiple channels
        Chuck_DL_Return ret;
        // added 1.3.0.0 -- Chuck_DL_Api::Api::instance()
        for( t_CKUINT i = 0; i < uana->m_multi_chan_size; i++ )
            uana_upchuck( uana->m_multi_chan[i], ARGS, &ret, VM, SHRED, API );

        // tock it (TODO: order relative to multiple channels?)
        uana->system_tock( vm->shreduler()->now_system );
    }

    // return
    RETURN->v_object = uana->blobProxy()->realblob();
}

/* CK_DLL_MFUN( uana_blob )
{
    // get as uana
    Chuck_UAna * uana = (Chuck_UAna *)SELF;
    
    // TODO: return
    RETURN->v_object = NULL;
} */

CK_DLL_MFUN( uana_fvals )
{
    // get the fvals array
    Chuck_UAnaBlobProxy * blob = (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(SELF, uana_offset_blob);
    RETURN->v_object = &blob->fvals();
}

CK_DLL_MFUN( uana_cvals )
{
    // get the fvals array
    Chuck_UAnaBlobProxy * blob = (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(SELF, uana_offset_blob);
    RETURN->v_object = &blob->cvals();
}

CK_DLL_MFUN( uana_fval )
{
    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // get the fvals array
    Chuck_UAnaBlobProxy * blob = (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(SELF, uana_offset_blob);
    Chuck_Array8 & fvals = blob->fvals();
    // check caps
    if( i < 0 || fvals.size() <= i ) RETURN->v_float = 0;
    else
    {
        // get
        t_CKFLOAT val;
        fvals.get( i, &val );
        RETURN->v_float = val;
    }
}

CK_DLL_MFUN( uana_cval )
{
    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // get the fvals array
    Chuck_UAnaBlobProxy * blob = (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(SELF, uana_offset_blob);
    Chuck_Array16 & cvals = blob->cvals();
    // check caps
    if( i < 0 || cvals.size() <= i ) RETURN->v_complex.re = RETURN->v_complex.im = 0;
    else
    {
        // get
        t_CKCOMPLEX val;
        cvals.get( i, &val );
        RETURN->v_complex = val;
    }
}

CK_DLL_MFUN( uana_connected )
{
    // get ugen
    Chuck_UAna * uana = (Chuck_UAna *)SELF;
    Chuck_UAna * right = (Chuck_UAna *)GET_NEXT_OBJECT(ARGS);

    // sanity
    t_CKINT ret = FALSE;
    if( !right )
    {
        ret = FALSE;
    }
    else
    {
        ret = right->is_up_connected_from( uana );
    }

    RETURN->v_int = ret;
}


// blob proxy implementation
Chuck_UAnaBlobProxy::Chuck_UAnaBlobProxy( Chuck_Object * blob )
{
    m_blob = blob;
    assert( m_blob != NULL );
    // add reference
    m_blob->add_ref();
}

Chuck_UAnaBlobProxy::~Chuck_UAnaBlobProxy()
{
    // release
    SAFE_RELEASE( m_blob );
}

t_CKTIME & Chuck_UAnaBlobProxy::when()
{
    // TODO: DANGER: is this actually returning correct reference?!
    return OBJ_MEMBER_TIME(m_blob, uanablob_offset_when);
}

Chuck_Array8 & Chuck_UAnaBlobProxy::fvals()
{
    // TODO: DANGER: is this actually returning correct reference?!
    Chuck_Array8 * arr8 = (Chuck_Array8 *)OBJ_MEMBER_INT(m_blob, uanablob_offset_fvals);
    assert( arr8 != NULL );
    return *arr8;
}

Chuck_Array16 & Chuck_UAnaBlobProxy::cvals()
{
    // TODO: DANGER: is this actually returning correct reference?!
    Chuck_Array16 * arr16 = (Chuck_Array16 *)OBJ_MEMBER_INT(m_blob, uanablob_offset_cvals);
    assert( arr16 != NULL );
    return *arr16;
}

// get proxy
Chuck_UAnaBlobProxy * getBlobProxy( const Chuck_UAna * uana )
{
    return (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(uana, uana_offset_blob);
}

// ctor
CK_DLL_CTOR( uanablob_ctor )
{
    // when
    OBJ_MEMBER_TIME(SELF, uanablob_offset_when) = 0;
    // fvals
    Chuck_Array8 * arr8 = new Chuck_Array8( 8 );
    initialize_object( arr8, SHRED->vm_ref->env()->t_array );
    // TODO: check out of memory
    arr8->add_ref();
    OBJ_MEMBER_INT(SELF, uanablob_offset_fvals) = (t_CKINT)arr8;
    // cvals
    Chuck_Array16 * arr16 = new Chuck_Array16( 8 );
    initialize_object( arr16, SHRED->vm_ref->env()->t_array );
    // TODO: check out of memory
    arr16->add_ref();
    OBJ_MEMBER_INT(SELF, uanablob_offset_cvals) = (t_CKINT)arr16;
}

// dtor
CK_DLL_DTOR( uanablob_dtor )
{
    // get array
    Chuck_Array8 * arr8 = (Chuck_Array8 *)OBJ_MEMBER_INT(SELF, uanablob_offset_fvals);
    // release it
    arr8->release();
    OBJ_MEMBER_INT(SELF, uanablob_offset_fvals) = 0;
    
    // get array
    Chuck_Array16 * arr16 = (Chuck_Array16 *)OBJ_MEMBER_INT(SELF, uanablob_offset_cvals);
    // release it
    arr16->release();
    OBJ_MEMBER_INT(SELF, uanablob_offset_cvals) = 0;
    
    OBJ_MEMBER_TIME(SELF, uanablob_offset_when) = 0;
}

CK_DLL_MFUN( uanablob_when )
{
    // set return
    RETURN->v_time = OBJ_MEMBER_TIME(SELF, uanablob_offset_when);
}

CK_DLL_MFUN( uanablob_fvals )
{
    // set return
    RETURN->v_object = (Chuck_Array8 *)OBJ_MEMBER_INT(SELF, uanablob_offset_fvals);
}

CK_DLL_MFUN( uanablob_fval )
{
    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // get the fvals array
    Chuck_Array8 * fvals = (Chuck_Array8 *)OBJ_MEMBER_INT(SELF, uanablob_offset_fvals);
    // check caps
    if( i < 0 || fvals->size() <= i ) RETURN->v_float = 0;
    else
    {
        // get
        t_CKFLOAT val;
        fvals->get( i, &val );
        RETURN->v_float = val;
    }
}

CK_DLL_MFUN( uanablob_cval )
{
    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // get the fvals array
    Chuck_Array16 * cvals = (Chuck_Array16 *)OBJ_MEMBER_INT(SELF, uanablob_offset_cvals);
    // check caps
    if( i < 0 || cvals->size() <= i ) RETURN->v_complex.re = RETURN->v_complex.im = 0;
    else
    {
        // get
        t_CKCOMPLEX val;
        cvals->get( i, &val );
        RETURN->v_complex = val;
    }
}

CK_DLL_MFUN( uanablob_cvals )
{
    // set return
    RETURN->v_object = (Chuck_Array16 *)OBJ_MEMBER_INT(SELF, uanablob_offset_cvals);
}

// ctor
CK_DLL_CTOR( event_ctor )
{
//  OBJ_MEMBER_INT(SELF, event_offset_data) = (t_CKUINT)new Data_Event;
//  Chuck_Event * event = (Chuck_Event *)SELF;
}


CK_DLL_DTOR( event_dtor )
{
//  delete (Data_Event *)OBJ_MEMBER_INT(SELF, event_offset_data);
}

CK_DLL_MFUN( event_signal )
{
    Chuck_Event * d = (Chuck_Event *)SELF;
    d->signal_local();
    d->signal_global();
}

CK_DLL_MFUN( event_broadcast )
{
    Chuck_Event * d = (Chuck_Event *)SELF;
    d->broadcast_local();
    d->broadcast_global();
}

CK_DLL_MFUN( event_wait )
{
    // Chuck_Event * d = (Chuck_Event *)SELF;
    assert( FALSE );
}

CK_DLL_MFUN( event_can_wait )
{
    RETURN->v_int = TRUE;
}


//-----------------------------------------------------------------------------
// vec3 API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( vec3_set )
{
    // HACK: this is a particularly horrid cast from (Chuck_Object *)
    // t_CKVEC3 is neither a super- or sub-class of Chuck_Object...
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get index
    vec3->x = GET_NEXT_FLOAT(ARGS);
    vec3->y = GET_NEXT_FLOAT(ARGS);
    vec3->z = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec3_setAll )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get index
    vec3->x = vec3->y = vec3->z = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec3_magnitude )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec3->x*vec3->x + vec3->y*vec3->y + vec3->z*vec3->z );
    // return
    RETURN->v_float = mag;
}

CK_DLL_MFUN( vec3_normalize )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec3->x*vec3->x + vec3->y*vec3->y + vec3->z*vec3->z );
    // check for zero
    if( mag > 0 )
    {
        vec3->x /= mag;
        vec3->y /= mag;
        vec3->z /= mag;
    }
}

CK_DLL_MFUN( vec3_interp )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // update value based on goal and slew
    vec3->x = (vec3->y - vec3->x) * vec3->z + vec3->x;
    // set return to value so it can used right after if desired
    RETURN->v_float = vec3->x;
}

CK_DLL_MFUN( vec3_interp_delta_float )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get index
    t_CKFLOAT delta = GET_NEXT_FLOAT(ARGS);
    // update value based on goal and slew
    vec3->x = (vec3->y - vec3->x) * vec3->z * delta + vec3->x;
    // set return to value so it can used right after if desired
    RETURN->v_float = vec3->x;
}

CK_DLL_MFUN( vec3_interp_delta_dur )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get index
    t_CKDUR delta = GET_NEXT_DUR(ARGS);
    // get srate
    t_CKFLOAT srate = SHRED->vm_ref->srate();
    // update value based on goal and slew
    vec3->x = (vec3->y - vec3->x) * vec3->z * (delta / srate) + vec3->x;
    // set return to value so it can used right after if desired
    RETURN->v_float = vec3->x;
}

CK_DLL_MFUN( vec3_update_goal )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get index
    t_CKDUR goal = GET_NEXT_DUR(ARGS);
    // set goal
    vec3->y = goal;
}

CK_DLL_MFUN( vec3_update_goal_slew )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get goal
    t_CKDUR goal = GET_NEXT_DUR(ARGS);
    // get slew
    t_CKDUR slew = GET_NEXT_DUR(ARGS);
    // set goal
    vec3->y = goal;
    // set slew
    vec3->z = slew;
}

CK_DLL_MFUN( vec3_updateSet_goalAndValue )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get goal
    t_CKDUR goalAndValue = GET_NEXT_DUR(ARGS);
    // set goal and value
    vec3->y = vec3->x = goalAndValue;
}

CK_DLL_MFUN( vec3_updateSet_goalAndValue_slew )
{
    // get data pointer
    t_CKVEC3 * vec3 = (t_CKVEC3 *)SELF;
    // get goal
    t_CKDUR goalAndValue = GET_NEXT_DUR(ARGS);
    // get slew
    t_CKDUR slew = GET_NEXT_DUR(ARGS);
    // set goal and value
    vec3->y = vec3->x = goalAndValue;
    // set slew
    vec3->z = slew;
}


//-----------------------------------------------------------------------------
// vec4 API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( vec4_set )
{
    // HACK: this is a particularly horrid cast from (Chuck_Object *)
    // t_CKVEC3 is neither a super- or sub-class of Chuck_Object...
    t_CKVEC4 * vec3 = (t_CKVEC4 *)SELF;
    // get index
    vec3->x = GET_NEXT_FLOAT(ARGS);
    vec3->y = GET_NEXT_FLOAT(ARGS);
    vec3->z = GET_NEXT_FLOAT(ARGS);
    vec3->w = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec4_setAll )
{
    // get data pointer
    t_CKVEC4 * vec4 = (t_CKVEC4 *)SELF;
    // get index
    vec4->x = vec4->y = vec4->z = vec4->w = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec4_magnitude )
{
    // get data pointer
    t_CKVEC4 * vec4 = (t_CKVEC4 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec4->x*vec4->x + vec4->y*vec4->y
                            + vec4->z*vec4->z + vec4->w*vec4->w );
    // return
    RETURN->v_float = mag;
}

CK_DLL_MFUN( vec4_normalize )
{
    // get data pointer
    t_CKVEC4 * vec4 = (t_CKVEC4 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec4->x*vec4->x + vec4->y*vec4->y
                            + vec4->z*vec4->z + vec4->w*vec4->w );
    // check for zero
    if( mag > 0 )
    {
        vec4->x /= mag;
        vec4->y /= mag;
        vec4->z /= mag;
        vec4->w /= mag;
    }
}




//-----------------------------------------------------------------------------
// Shred API
//-----------------------------------------------------------------------------

//CK_DLL_CTOR( shred_ctor );
//CK_DLL_DTOR( shred_dtor );

CK_DLL_MFUN( shred_exit )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    // end the shred
    derhs->is_done = TRUE;
    derhs->is_running = FALSE;
}

CK_DLL_MFUN( shred_running )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    RETURN->v_int = derhs->is_running;
}

CK_DLL_MFUN( shred_done )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    RETURN->v_int = derhs->is_done;
}

CK_DLL_MFUN( shred_clone )
{
}

CK_DLL_MFUN( shred_id )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    // return the id
    RETURN->v_int = derhs->xid;
}

CK_DLL_MFUN( shred_yield )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    Chuck_VM * vm = derhs->vm_ref;

    // suspend
    derhs->is_running = FALSE;
    // reshredule
    vm->shreduler()->shredule( derhs, derhs->now );
}

CK_DLL_MFUN( shred_numArgs )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;

    // get the number of arguments
    RETURN->v_int = derhs->args.size();
}

CK_DLL_MFUN( shred_getArg )
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;

    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // total
    t_CKINT num = derhs->args.size();

    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( ( i < num ? derhs->args[i] : "" ) );
    RETURN->v_string = str; 
}

CK_DLL_MFUN( shred_sourcePath ) // added 1.3.0.0
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( derhs->code->filename );
    RETURN->v_string = str;
}

CK_DLL_MFUN( shred_sourceDir ) // added 1.3.0.0
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    
    str->set( extract_filepath_dir(derhs->code->filename) );
    
    RETURN->v_string = str;
}

CK_DLL_MFUN( shred_sourceDir2 ) // added 1.3.2.0
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    // get num up
    t_CKINT i = GET_NEXT_INT(ARGS);
    // abs
    if( i < 0 ) i = -i;

    // new chuck string
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    // set the content
    str->set( extract_filepath_dir(derhs->code->filename) );
    // up
    str->set( dir_go_up( str->str(), i ) );
    
    RETURN->v_string = str;
}


CK_DLL_SFUN( shred_fromId ) // added 1.3.2.0
{
    t_CKINT shred_id = GET_NEXT_INT(ARGS);
    
    Chuck_VM_Shred * derhs = SHRED->vm_ref->shreduler()->lookup(shred_id);
    
    RETURN->v_object = derhs;
}


//-----------------------------------------------------------------------------
// string API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( string_length )
{
    Chuck_String * s = (Chuck_String *)SELF;
    RETURN->v_int = s->str().length();
}

CK_DLL_MFUN( string_upper )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( toupper( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_lower )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( tolower( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_ltrim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( ltrim( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_rtrim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( rtrim( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_trim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    str->set( trim( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_toString )
{
    Chuck_String * s = (Chuck_String *)SELF;
    RETURN->v_string = s;
}

CK_DLL_MFUN(string_charAt)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT index = GET_NEXT_INT(ARGS);
    
    if(index < 0 || index >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", index);
        RETURN->v_int = -1;
        return;
    }
    
    RETURN->v_int = str->str().at(index);
}

CK_DLL_MFUN(string_setCharAt)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT index = GET_NEXT_INT(ARGS);
    t_CKINT the_char = GET_NEXT_INT(ARGS);
    
    if(index < 0 || index >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", index);
        RETURN->v_int = -1;
        return;
    }

    // str->str.at(index) = the_char;
    std::string s = str->str();
    s.at(index) = the_char;
    str->set( s );
    RETURN->v_int = str->str().at(index);
}

CK_DLL_MFUN(string_substring)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT start = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_string = NULL;
        return;
    }

    Chuck_String * ss = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    ss->set( str->str().substr(start) );
    
    RETURN->v_string = ss;
}

CK_DLL_MFUN(string_substringN)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT start = GET_NEXT_INT(ARGS);
    t_CKINT length = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_string = NULL;
        return;
    }

    if(length < 0 || start+length > str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", length);
        RETURN->v_string = NULL;
        return;
    }
    
    Chuck_String * ss = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    ss->set( str->str().substr(start, length) );
    
    RETURN->v_string = ss;
}

CK_DLL_MFUN(string_insert)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT position = GET_NEXT_INT(ARGS);
    Chuck_String * str2 = GET_NEXT_STRING(ARGS);
    
    if(position < 0 || position >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", position);
        return;
    }
    if(str2 == NULL)
    {
        throw_exception(SHRED, "NullPointerException");
        return;
    }

    // str->str.insert(position, str2->str);
    std::string s = str->str();
    s.insert( position, str2->str() );
    str->set( s );
}

CK_DLL_MFUN(string_replace)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT position = GET_NEXT_INT(ARGS);
    Chuck_String * str2 = GET_NEXT_STRING(ARGS);
    
    if(position < 0 || position >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", position);
        return;
    }
    if(str2 == NULL)
    {
        throw_exception(SHRED, "NullPointerException");
        return;
    }

    string::size_type length;
    if(position + str2->str().length() > str->str().length())
        length = str->str().length() - position;
    else
        length = str2->str().length();
    
    // str->str.replace(position, length, str2->str);
    std::string s = str->str();
    s.replace( position, length, str2->str() );
    str->set( s );
}

CK_DLL_MFUN(string_replaceN)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT position = GET_NEXT_INT(ARGS);
    t_CKINT length = GET_NEXT_INT(ARGS);
    Chuck_String * str2 = GET_NEXT_STRING(ARGS);

    if(position < 0 || position >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", position);
        return;
    }

    if(length < 0 || position+length > str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", length);
        return;
    }

    if(str2 == NULL)
    {
        throw_exception(SHRED, "NullPointerException");
        return;
    }

    // str->str.replace(position, length, str2->str);
    std::string s = str->str();
    s.replace( position, length, str2->str() );
    str->set( s );
}

CK_DLL_MFUN(string_find)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT the_char = GET_NEXT_INT(ARGS);
    
    string::size_type index = str->str().find(the_char);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_findStart)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT the_char = GET_NEXT_INT(ARGS);
    t_CKINT start = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_int = -1;
        return;
    }

    string::size_type index = str->str().find(the_char, start);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_findStr)
{
    Chuck_String * str = (Chuck_String *) SELF;
    Chuck_String * the_str = GET_NEXT_STRING(ARGS);
    
    string::size_type index = str->str().find(the_str->str());
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_findStrStart)
{
    Chuck_String * str = (Chuck_String *) SELF;
    Chuck_String * the_str = GET_NEXT_STRING(ARGS);
    t_CKINT start = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_int = -1;
        return;
    }
    
    string::size_type index = str->str().find(the_str->str(), start);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_rfind)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT the_char = GET_NEXT_INT(ARGS);
    
    string::size_type index = str->str().rfind(the_char);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_rfindStart)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT the_char = GET_NEXT_INT(ARGS);
    t_CKINT start = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_int = -1;
        return;
    }
    
    string::size_type index = str->str().rfind(the_char, start);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_rfindStr)
{
    Chuck_String * str = (Chuck_String *) SELF;
    Chuck_String * the_str = GET_NEXT_STRING(ARGS);
    
    string::size_type index = str->str().rfind(the_str->str());
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_rfindStrStart)
{
    Chuck_String * str = (Chuck_String *) SELF;
    Chuck_String * the_str = GET_NEXT_STRING(ARGS);
    t_CKINT start = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        RETURN->v_int = -1;
        return;
    }
    
    string::size_type index = str->str().rfind(the_str->str(), start);
    
    if(index == string::npos)
        RETURN->v_int = -1;
    else
        RETURN->v_int = index;
}

CK_DLL_MFUN(string_erase)
{
    Chuck_String * str = (Chuck_String *) SELF;
    t_CKINT start = GET_NEXT_INT(ARGS);
    t_CKINT length = GET_NEXT_INT(ARGS);
    
    if(start < 0 || start >= str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", start);
        return;
    }
    
    if(length < 0 || start+length > str->str().length())
    {
        throw_exception(SHRED, "IndexOutOfBoundsException", length);
        return;
    }
    
    // str->str.erase(start, length);
    std::string s = str->str();
    s.erase( start, length );
    str->set( s );
}

CK_DLL_MFUN( string_toInt )
{
    // get pointer to self
    Chuck_String * str = (Chuck_String *)SELF;
    // convert to int
    RETURN->v_int = ::atoi( str->str().c_str() );
}

CK_DLL_MFUN( string_toFloat )
{
    // get pointer to self
    Chuck_String * str = (Chuck_String *)SELF;
    // convert to int
    RETURN->v_float = (t_CKFLOAT)::atof( str->str().c_str() );
}

CK_DLL_MFUN( string_parent )
{
    Chuck_String * str = (Chuck_String *) SELF;
    
    string::size_type i = str->str().rfind('/', str->str().length()-2);
#ifdef WIN32
    // SPENCERTODO: make this legit on windows
    if(i == string::npos)
        i = str->str().rfind('\\', str->str().length()-2);
#endif // WIN32
    
    Chuck_String * parent = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    
    if(i != string::npos)
    {
        if(i == 0)
            parent->set( "/" );
        else
            parent->set( str->str().substr(0, i) );
    }
    
    RETURN->v_string = parent;
}

/*
CK_DLL_MFUN( string_set_at )
{
    Chuck_String * s = (Chuck_String *)SELF;
    t_CKINT c = GET_CK_INT(
    RETURN->v_int = s->str.length();
}

CK_DLL_MFUN( string_get_at )
{
    Chuck_String * s = (Chuck_String *)SELF;
    RETURN->v_int = s->str.length();
}
*/


//-----------------------------------------------------------------------------
// array API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( array_get_size )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    RETURN->v_int = array->size();
}

// array.size()
CK_DLL_MFUN( array_set_size )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    t_CKINT size = GET_NEXT_INT(ARGS);
    if( size < 0 )
    {
        // TODO: make this exception
        CK_FPRINTF_STDERR( "[chuck](via array): attempt to set negative array size!\n" );
        RETURN->v_int = 0;
    }
    else
    {
        array->set_size( size );
        RETURN->v_int = array->size();
    }
    RETURN->v_int = array->size();
}

// array.clear()
CK_DLL_MFUN( array_clear )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    array->clear();
}

// array.reset()
CK_DLL_MFUN( array_reset )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    // default capacity
    array->set_size( 8 );
    // clear the array
    array->clear();
}

// array.cap()
CK_DLL_MFUN( array_set_capacity )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    t_CKINT capacity = GET_NEXT_INT(ARGS);
    if( capacity < 0 )
    {
        // TODO: make this exception
        CK_FPRINTF_STDERR( "[chuck](via array): attempt to set negative array capacity!\n" );
        RETURN->v_int = 0;
    }
    else
    {
        array->set_capacity( capacity );
        RETURN->v_int = array->capacity();
    }
}

// array.cap()
CK_DLL_MFUN( array_get_capacity_hack )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    RETURN->v_int = array->size();
}

// array.capacity()
CK_DLL_MFUN( array_get_capacity )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    RETURN->v_int = array->capacity();
}

// array.find()
CK_DLL_MFUN( array_find )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    Chuck_String * name = GET_NEXT_STRING( ARGS );
    RETURN->v_int = array->find( name->str() );
}

// array.erase()
CK_DLL_MFUN( array_erase )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    Chuck_String * name = GET_NEXT_STRING( ARGS );
    RETURN->v_int = array->erase( name->str() );
}

// array.push_back()
CK_DLL_MFUN( array_push_back )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    // ISSUE: 64-bit (fixed 1.3.1.0 using data kind)
    if( array->data_type_kind() == CHUCK_ARRAY4_DATAKIND )
        RETURN->v_int = ((Chuck_Array4 *)array)->push_back( GET_NEXT_UINT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAY8_DATAKIND )
        RETURN->v_int = ((Chuck_Array8 *)array)->push_back( GET_NEXT_FLOAT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAY16_DATAKIND )
        RETURN->v_int = ((Chuck_Array16 *)array)->push_back( GET_NEXT_COMPLEX( ARGS ) );
    else
        assert( FALSE );
}

// array.pop_back()
CK_DLL_MFUN( array_pop_back )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    // ISSUE: 64-bit (fixed 1.3.1.0 using data kind)
    if( array->data_type_kind() == CHUCK_ARRAY4_DATAKIND )
        RETURN->v_int = ((Chuck_Array4 *)array)->pop_back( );
    else if( array->data_type_kind() == CHUCK_ARRAY8_DATAKIND )
        RETURN->v_int = ((Chuck_Array8 *)array)->pop_back( );
    else if( array->data_type_kind() == CHUCK_ARRAY16_DATAKIND )
        RETURN->v_int = ((Chuck_Array16 *)array)->pop_back( );
    else
        assert( FALSE );
}

// array.pop_out
CK_DLL_MFUN( array_pop_out )
{
	Chuck_Array * array = (Chuck_Array *)SELF;
	t_CKINT position = GET_NEXT_INT(ARGS);
	if( array->data_type_kind() == CHUCK_ARRAY4_DATAKIND)
		RETURN->v_int = ((Chuck_Array4 *)array)->pop_out(position);
	else if( array->data_type_kind() == CHUCK_ARRAY8_DATAKIND )
		RETURN->v_int = ((Chuck_Array8 *)array)->pop_out(position);
	else if( array->data_type_kind() == CHUCK_ARRAY16_DATAKIND )
		RETURN->v_int = ((Chuck_Array16 *)array)->pop_out(position);
	else
		assert( FALSE );

}
