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
// desc: initialize the base Object class
//-----------------------------------------------------------------------------
t_CKBOOL init_class_object( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;
    Chuck_Value * value = NULL;

    // log
    EM_log( CK_LOG_HERALD, "class 'Object'" );

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
    func->doc = "output helpful information about a class or an instance thereof.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add getType() // 1.5.0.0
    func = make_new_sfun( "Type", "typeOf", object_typeInfo );
    func->doc = "get the type of this object (or class).";
    if( !type_engine_import_sfun( env, func ) ) goto error;

//    // add dump()
//    func = make_new_mfun( "void", "dump", object_dump );
//    func->doc = "output current state of the object.";
//    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // find the offset for can_wait
    value = type_engine_find_value( type, "toString" );
    assert( value != NULL );
    assert( value->func_ref != NULL );
    // remember it
    Chuck_Event::our_vt_toString = value->func_ref->vt_index;

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

    EM_log( CK_LOG_HERALD, "class 'UGen'" );

    // add ugen info
    type->ugen_info = new Chuck_UGen_Info;
    type->ugen_info->add_ref();
    type->ugen_info->tick = __ugen_tick;
    type->ugen_info->num_ins = 1;
    type->ugen_info->num_outs = 1;
    // documentation text
    const char * doc = "base class for all unit generator (UGen) types.";

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
    func->add_arg( "UGen", "rhs" );
    func->doc = "return true if this UGen's output is connected to the input of rhs; if either this UGen or rhs has more than one channel, this function returns true if any connections exist between the channels; return false if there are no connections.";
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

    EM_log( CK_LOG_HERALD, "class 'UAna'" );

    // add uana info
    type->ugen_info = new Chuck_UGen_Info;
    type->ugen_info->add_ref();
    type->ugen_info->tick = __ugen_tick;
    type->ugen_info->num_ins = 1;
    type->ugen_info->num_outs = 1;

    const char * doc = "base class from which all unit analyzer (UAna) types inherit; UAnae (note plural form) can be interconnected using => (chuck operator for synthesis; all UAnae are also UGens) or using =^ (upchuck operator for analysis) -- the operator used will determine how data is passed. When .upchuck() is invoked on a given UAna, the UAna-chain (i.e., UAnae connected via =^) is traversed upstream from the upchucked UAna, and analysis is performed at each UAna along the chain; the analysis results are returned in UAnaBlobs.";

    // init as base class, type should already know the parent type
    // TODO: ctor/dtor, ugen's sometimes created internally?
    if( !type_engine_import_class_begin( env, type, env->global(), uana_ctor, uana_dtor, doc ) )
        return FALSE;

    // add variables
    uana_offset_blob = type_engine_import_mvar( env, "int", "@m_blobproxy", FALSE );
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
    EM_log( CK_LOG_HERALD, "class 'UAnaBlob'" );

    const char * doc = "a data structure that contains results associated with UAna analysis. There is a UAnaBlob associated with every UAna. As a UAna is upchucked (using .upchuck()), the result is stored in the UAnaBlob's floating point vector and/or complex vector. The interpretation of the results depends on the specific UAna.";

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
    EM_log( CK_LOG_HERALD, "class 'Event'" );

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
    func->doc = "can the event can be waited on? (internal) used by virtual machine for synchronization.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add waiting_on()
    func = make_new_mfun( "void", "waiting_on", event_waiting_on );
    func->doc = "(internal) used by virtual machine to be notified when a shred starts waiting on this Event.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "event/broadcast.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/signal.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/signal4.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/event-extend.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/event-extend2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/event-x-bpm-1.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/event-x-bpm-2.ck" ) ) goto error;

    // if( !type_engine_import_add_ex( env, "hid/joy.ck" ) ) goto error;
    // if( !type_engine_import_add_ex( env, "hid/kb.ck" ) ) goto error;
    // if( !type_engine_import_add_ex( env, "midi/gomidi.ck" ) ) goto error;
    // if( !type_engine_import_add_ex( env, "midi/polyfony.ck" ) ) goto error;
    // if( !type_engine_import_add_ex( env, "midi/polyfony2.ck" ) ) goto error;
    // if( !type_engine_import_add_ex( env, "osc/r.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    // find the offset for can_wait
    value = type_engine_find_value( type, "can_wait" );
    assert( value != NULL );
    assert( value->func_ref != NULL );
    // remember it
    Chuck_Event::our_can_wait = value->func_ref->vt_index;

    // find the offset for waiting_on
    value = type_engine_find_value( type, "waiting_on" );
    assert( value != NULL );
    assert( value->func_ref != NULL );
    // remember it
    Chuck_Event::our_waiting_on = value->func_ref->vt_index;

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
    EM_log( CK_LOG_HERALD, "class 'Shred'" );

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
    // func = make_new_mfun( "void", "clone", shred_clone );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

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
    func->doc = "is the Shred currently actively running in the VM?";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add done()
    func = make_new_mfun( "int", "done", shred_done );
    func->doc = "has the Shred reached the end of its execution?";
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

    // add childMemSize() | 1.5.1.5
    func = make_new_mfun( "int", "childMemSize", shred_ctrl_hintChildMemSize );
    func->add_arg( "int", "sizeInBytes" );
    func->doc = "set size hint of per-shred call stack (\"mem\") for children shreds subsequently sporked from the calling shred (NOTE this size hint does not affect the calling shred--only its descendants); if sizeInBytes <= 0, the size hint is set to the VM default. (FYI This is an arcane functionality that most programmers never need to worry about. Advanced usage: set size hint to small values (e.g., 1K) to support A LOT (e.g., >10000) of simultaneous shreds; set size hint to large values (e.g., >65K) to spork functions with extremely deep recursion, or to support A LOT (>10000) of declared local variables. Use with care.)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add childMemSize() | 1.5.1.5
    func = make_new_mfun( "int", "childMemSize", shred_cget_hintChildMemSize );
    func->doc = "get the memory stack size hint (in bytes) for shreds sporked from this one.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add childRegSize() | 1.5.1.5
    func = make_new_mfun( "int", "childRegSize", shred_ctrl_hintChildRegSize );
    func->add_arg( "int", "sizeInBytes" );
    func->doc = "set size hint of per-shred operand stack (\"reg\") for children shreds subsequently sporked from the calling shred (NOTE this size hint does not affect the calling shred--only its descendants); if sizeInBytes <= 0, the size hint is set to the VM default. (FYI This is an arcane functionality that most programmers never need to worry about. Advanced usage: set size hint to small values (e.g., 256 bytes) to support A LOT (>10000) of simultaneous shreds; set size hint to large values (e.g., >20K) to spork functions with extremely lengthy (>10000) statements, including array initializer lists. Use with care.)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add childRegSize() | 1.5.1.5
    func = make_new_mfun( "int", "childRegSize", shred_cget_hintChildRegSize );
    func->doc = "get the operand stack size hint (in bytes) for shreds sporked from this one.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add parent() | 1.5.2.0 (nshaheed)
    func = make_new_sfun( "Shred", "parent", shred_parent );
    func->doc = "get the calling shred's parent shred (i.e., the shred that sporked the calling shred). Returns null if there is no parent Shred. (Related: see Shred.ancestor())";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add ancestor() | 1.5.2.0 (nshaheed)
    func = make_new_sfun( "Shred", "ancestor", shred_ancestor );
    func->doc = "get the calling shred's \"ancestor\" shred (i.e., the top-level shred). Returns itself if the calling shred is the top-level shred. (Related: see Shred.parent())";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add gc() | 1.5.2.0 (ge) added
    // func = make_new_mfun( "void", "gc", shred_gc );
    // func->doc = "manually trigger a per-shred garbage collection pass; can be used to clean up certain UGens/objects without waiting for the shred to complete; use with care.";
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "shred/spork.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/spork2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/spork2-exit.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/spork2-remove.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/powerup.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/broadcast.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/signal.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "event/signal4.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/parent.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/ancestor.ck" ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_vec2()
// desc: initialize primitive type vec2
//-----------------------------------------------------------------------------
t_CKBOOL init_class_vec2( Chuck_Env * env, Chuck_Type * type )
{
    // init as base class
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_HERALD, "class 'vec2' (primitive)" );

    // document
    const char * doc = "a primitive type for a 2-dimensional vector; potentially useful for 2D and UV coordinates.";

    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;

    // add set(float,float,float)
    func = make_new_mfun( "void", "set", vec2_set );
    func->add_arg( "float", "x" );
    func->add_arg( "float", "y" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add setAll(float)
    func = make_new_mfun( "void", "setAll", vec2_setAll );
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add magnitude()
    func = make_new_mfun( "float", "magnitude", vec2_magnitude );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add normalize()
    func = make_new_mfun( "void", "normalize", vec2_normalize );
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
    EM_log( CK_LOG_HERALD, "class 'vec3' (primitive)" );

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
    EM_log( CK_LOG_HERALD, "class 'vec4' (primitive)" );

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
    EM_log( CK_LOG_HERALD, "class 'string'" );

    const char * doc = "textual data as a sequence of characters, along with functions for manipulating text.";

    // init as base class
    // NOTE: explicitly instantiated in instantiate_and_initialize_object()
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;

    // add contructor
    func = make_new_ctor( string_ctor_str );
    func->add_arg( "string", "str" );
    func->doc = "construct a string as a copy of another string.";
    if( !type_engine_import_ctor( env, func ) ) goto error;

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
    func->doc = "return the reference of calling string.";
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
    func->doc = "replace characters from 'position' with contents of 'str'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add replace()
    func = make_new_mfun( "void", "replace", string_replaceN );
    func->add_arg( "int", "position" );
    func->add_arg( "int", "length" );
    func->add_arg( "string", "str" );
    func->doc = "replace 'length' characters from 'position' with contents of 'str'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // substring replace() | 1.5.1.3 (nshaheed) added
    func = make_new_mfun( "void", "replace", string_replace_str );
    func->add_arg( "string", "from" );
    func->add_arg( "string", "to" );
    func->doc = "replace all instances of 'from' in the string with 'to'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_mfun( "int", "find", string_find );
    func->add_arg( "int", "theChar" );
    func->doc = "get the index of the first occurrence of theChar, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_mfun( "int", "find", string_findStart );
    func->add_arg( "int", "theChar" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the first occurrence of theChar at or after the start position, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_mfun( "int", "find", string_findStr );
    func->add_arg( "string", "str" );
    func->doc = "get the index of the first occurrence of str, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_mfun( "int", "find", string_findStrStart );
    func->add_arg( "string", "str" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the first occurrence of str at or after the start position, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfind );
    func->add_arg( "int", "theChar" );
    func->doc = "get the index of the last occurrence of theChar, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStart );
    func->add_arg( "int", "theChar" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the last occurrence of theChar at or before the start position, or -1 if theChar is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStr );
    func->add_arg( "string", "str" );
    func->doc = "get the index of the last occurrence of str, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add rfind()
    func = make_new_mfun( "int", "rfind", string_rfindStrStart );
    func->add_arg( "string", "str" );
    func->add_arg( "int", "start" );
    func->doc = "get the index of the last occurrence of str at or before the start position, or -1 if str is not found.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add erase()
    func = make_new_mfun( "void", "erase", string_erase );
    func->add_arg( "int", "start" );
    func->add_arg( "int", "length" );
    func->doc = "erase length characters of the string from start position.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toInt()
    func = make_new_mfun( "int", "toInt", string_toInt );
    func->doc = "attempt to convert the contents of the string to an integer and return the result, or 0 if conversion failed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toFloat()
    func = make_new_mfun( "float", "toFloat", string_toFloat );
    func->doc = "Attempt to convert the contents of the string to an float and return the result, or 0 if conversion failed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add parent()
    // disable for now
    // func = make_new_mfun( "string", "parent", string_parent );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toTime()
    // func = make_new_mfun( "float", "toTime", string_toFloat );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add toDur()
    // func = make_new_mfun( "float", "toDur", string_toFloat );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add at()
    // func = make_new_mfun( "int", "ch", string_set_at );
    // func->add_arg( "int", "index" );
    // func->add_arg( "int", "val" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;
    // func = make_new_mfun( "int", "ch", string_get_at );
    // func->add_arg( "int", "index" );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples | 1.5.0.0 (ge) added
    if( !type_engine_import_add_ex( env, "string/strops.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "string/strops2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "string/replace.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "string/token.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "string/escape.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "string/tostr.ck" ) ) goto error;

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
    EM_log( CK_LOG_HERALD, "class 'array'" );

    // init as base class
    // TODO: ctor/dtor?
    string doc = "storage construct for sequential data of the same type; can also be used as an associative map data structure; also can be used as a stack with << operator to append/push and popBack to pop.";
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc.c_str() ) )
        return FALSE;

    // add clear()
    func = make_new_mfun( "void", "clear", array_clear );
    func->doc = "clear the contents of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add zero() | 1.5.0.0 (ge) added
    func = make_new_mfun( "void", "zero", array_zero );
    func->doc = "zero out the contents of the array; size is unchanged.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add reset()
    func = make_new_mfun( "void", "reset", array_reset );
    func->doc = "reset array size to 0, set capacity to (at least) 8.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

//    // add insert()
//    func = make_new_mfun( "void", "insert", array_insert );
//    func->add_arg( "int", "position" );
//    func->add_arg( "auto", "value" );
//    func->doc = "insert value before the specific position.";
//    if( !type_engine_import_mfun( env, func ) ) goto error;

//    // add pushBack()
//    func = make_new_mfun( "void", "pushBack", array_push_back );
//    func->add_arg( "auto", "value" );
//    func->doc = "append value to array.";
//    if( !type_engine_import_mfun( env, func ) ) goto error;

//    // add pushFront()
//    func = make_new_mfun( "void", "pushFront", array_push_front );
//    func->add_arg( "auto", "value" );
//    func->doc = "prepend value to array.";
//    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add popBack()
    func = make_new_mfun( "void", "popBack", array_pop_back );
    func->doc = "remove the last element of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add popFront()
    func = make_new_mfun( "void", "popFront", array_pop_front );
    func->doc = "remove the first element of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add popOut()
    func = make_new_mfun( "void", "popOut", array_erase );
    func->add_arg( "int", "position" );
    func->doc = "remove the element at 'position' from the array (same as erase(int)).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add erase()
    func = make_new_mfun( "int", "erase", array_erase );
    func->add_arg( "int", "position" );
    func->doc = "remove element at 'position' from the array (same as popOut(int)).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add erase()
    func = make_new_mfun( "int", "erase", array_erase2 );
    func->add_arg( "int", "begin" );
    func->add_arg( "int", "end" );
    func->doc = "remove element(s) in the range [begin,end).";
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

    // add getKeys() | (1.4.1.1) nshaheed
    func = make_new_mfun( "void", "getKeys", array_get_keys );
    func->add_arg( "string[]", "keys" );
    func->doc = "return all keys found in associative array in keys";
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

    // add isInMap() | 1.5.0.8 previously was find()
    func = make_new_mfun( "int", "isInMap", array_map_find );
    func->add_arg( "string", "key" );
    func->doc = "(map only) test if 'key' is in the map; (historical) this was renamed from .find() to avoid confusion with the vector part of array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add erase()
    func = make_new_mfun( "int", "erase", array_map_erase );
    func->add_arg( "string", "key" );
    func->doc = "(map only) Erase all elements with the specified key.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add reverse() | (1.5.0.0) azaday
    func = make_new_mfun( "void", "reverse", array_reverse );
    func->doc = "reverses the array in-place";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add shuffle()
    func = make_new_mfun( "void", "shuffle", array_shuffle );
    func->doc = "shuffle the contents of the array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add sort()
    func = make_new_mfun( "void", "sort", array_sort );
    func->doc = "sort the contents of the array in ascending order.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add examples
    if( !type_engine_import_add_ex( env, "array/array_append.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_argument.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_assign.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_associative.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_capacity.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_dynamic.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_erase.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_erase2.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_mdim.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_mmixed.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_negative.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_range.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_resize.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_reverse.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_shuffle.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_sort.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_storage.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_sub_assign.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "array/array_zero.ck" ) ) goto error;

    // end import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: init_class_function()
// desc: initialize the @fuction class | 1.5.0.0 (ge) added
//-----------------------------------------------------------------------------
t_CKBOOL init_class_function( Chuck_Env * env, Chuck_Type * type )
{
    // log
    EM_log( CK_LOG_HERALD, "class '@function'" );
    const char * doc = "the base function Type.";

    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), NULL, NULL, doc ) )
        return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    return TRUE;
}




// global initialization
const t_CKINT TYPE_ATTRIB_OBJECT = 0x1;
const t_CKINT TYPE_ATTRIB_PRIMITIVE = 0x2;
const t_CKINT TYPE_ATTRIB_SPECIAL = 0x4;
const t_CKINT TYPE_ORIGIN_BUILTIN = 0x8;
const t_CKINT TYPE_ORIGIN_CHUGIN = 0x10;
const t_CKINT TYPE_ORIGIN_CKLIB = 0x20;
const t_CKINT TYPE_ORIGIN_USER = 0x40;


//-----------------------------------------------------------------------------
// name: init_class_type()
// desc: initialize the Type class | 1.5.0.0 (ge) added
//-----------------------------------------------------------------------------
t_CKBOOL init_class_type( Chuck_Env * env, Chuck_Type * type )
{
    Chuck_DL_Func * func = NULL;

    // log
    EM_log( CK_LOG_HERALD, "class 'Type'" );
    const char * doc = "a representation of a ChucK type.";

    // init as base class
    if( !type_engine_import_class_begin( env, type, env->global(), type_ctor, type_dtor, doc ) )
        return FALSE;

    // add relevant examples
    if( !type_engine_import_add_ex( env, "type/type_type.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "type/type_query.ck" ) ) goto error;

    // add static variables
    if( !type_engine_import_svar( env, "int", "OBJECT", TRUE, (t_CKUINT)(&TYPE_ATTRIB_OBJECT), "attribute bit for non-primitive Object types.") ) goto error;
    if( !type_engine_import_svar( env, "int", "PRIMITIVE", TRUE, (t_CKUINT)(&TYPE_ATTRIB_PRIMITIVE), "attribute bit for primitive types.") ) goto error;
    if( !type_engine_import_svar( env, "int", "SPECIAL", TRUE, (t_CKUINT)(&TYPE_ATTRIB_SPECIAL), "attribute bit for special types (e.g., @array and @function).") ) goto error;
    if( !type_engine_import_svar( env, "int", "BUILTIN", TRUE, (t_CKUINT)(&TYPE_ORIGIN_BUILTIN), "origin bit for \"builtin\".") ) goto error;
    if( !type_engine_import_svar( env, "int", "CHUGIN", TRUE, (t_CKUINT)(&TYPE_ORIGIN_CHUGIN), "origin bit for \"chugin\".") ) goto error;
    if( !type_engine_import_svar( env, "int", "CKLIB", TRUE, (t_CKUINT)(&TYPE_ORIGIN_BUILTIN), "origin bit for \"cklib\".") ) goto error;
    if( !type_engine_import_svar( env, "int", "USER", TRUE, (t_CKUINT)(&TYPE_ORIGIN_BUILTIN), "origin bit for \"user\".") ) goto error;

    // add equals()
    func = make_new_mfun( "int", "equals", type_equals );
    func->add_arg( "Type", "another" );
    func->doc = "return whether this Type is same as 'another'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isa()
    func = make_new_mfun( "int", "isa", type_isa );
    func->add_arg( "Type", "another" );
    func->doc = "return whether this Type is a kind of 'another'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isa()
    func = make_new_mfun( "int", "isa", type_isa_str );
    func->add_arg( "string", "another" );
    func->doc = "return whether this Type is a kind of 'another'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add baseName()
    func = make_new_mfun( "string", "baseName", type_basename );
    func->doc = "return the base name of this Type. The base of name of an array Type is the type without the array dimensions (e.g., base name of 'int[][]' is 'int')";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add name()
    func = make_new_mfun( "string", "name", type_name );
    func->doc = "return the name of this Type.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add parent()
    func = make_new_mfun( "Type", "parent", type_parent );
    func->doc = "return this Type's parent Type; returns null if this Type is 'Object'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add kids()
    func = make_new_mfun( "Type[]", "children", type_children );
    func->doc = "retrieve this Type's children Types.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isPrimitive()
    func = make_new_mfun( "int", "isPrimitive", type_isPrimitive );
    func->doc = "return whether this is a primitive Type (e.g., 'int' and 'dur' are primitives types; 'Object' and its children Types are not).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add isArray()
    func = make_new_mfun( "int", "isArray", type_isArray );
    func->doc = "return whether this Type is some kind of an array.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add arrayDims()
    func = make_new_mfun( "int", "arrayDepth", type_arrayDims );
    func->doc = "return the number of array dimensions associated with this Type (e.g., 'int[][]' has 2; 'int' has 0).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add origin()
    func = make_new_mfun( "string", "origin", type_origin );
    func->doc = "return a string describing where this Type was defined (e.g., \"builtin\", \"chugin\", \"cklib\", \"user\").";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add find()
    func = make_new_sfun( "Type", "find", type_findString );
    func->add_arg( "string", "typeName" );
    func->doc = "find and return the Type associated with 'typeName'; returns null if no Types currently in the VM with that name.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_obj );
    func->add_arg( "Object", "obj" );
    func->doc = "return the Type of 'obj'";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_int );
    func->add_arg( "int", "val" );
    func->doc = "return the Type associated with 'int'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_float );
    func->add_arg( "float", "val" );
    func->doc = "return the Type associated with 'float'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_time );
    func->add_arg( "time", "val" );
    func->doc = "return the Type associated with 'time'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_dur );
    func->add_arg( "dur", "val" );
    func->doc = "return the Type associated with 'dur'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_complex );
    func->add_arg( "complex", "val" );
    func->doc = "return the Type associated with 'complex'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_polar );
    func->add_arg( "polar", "val" );
    func->doc = "return the Type associated with 'polar'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_vec3 );
    func->add_arg( "vec3", "val" );
    func->doc = "return the Type associated with 'vec3'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add typeOf()
    func = make_new_sfun( "Type", "of", type_typeOf_vec4 );
    func->add_arg( "vec4", "val" );
    func->doc = "return the Type associated with 'vec4'.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add getTypes()
    func = make_new_sfun( "Type[]", "getTypes", type_getTypes );
    func->add_arg( "int", "attributes" );
    func->add_arg( "int", "origins" );
    func->doc = "retrieve all top-level Types in the ChucK runtime type system that fit the attributes and origins flags. Flags that can bitwise-OR'ed for attributes: Type.ATTRIB_OBJECT, Type.ATTRIB_PRIMITIVE,  TYPE_SPECIAL -- and for origins: Type.ORIGIN_BUILTIN, Type.ORIGIN_CHUGIN, Type.ORIGIN_CKLIB, Type.ORIGIN_USER.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // add getTypes()
    // func = make_new_sfun( "Type[]", "getTypes", type_getTypes2 );
    // func->add_arg( "int", "includeObjects" );
    // func->add_arg( "int", "includePrimitives" );
    // func->add_arg( "int", "includeSpecial" );
    // func->add_arg( "int", "includeBuiltin" );
    // func->add_arg( "int", "includeChugins" );
    // func->add_arg( "int", "includeImports" );
    // func->add_arg( "int", "includeUserDefined" );
    // func->doc = "retrieves in the array 'types' all top-level Types currently in the ChucK runtime type system; 'includeObjects'--include all Objects? 'includePrimitives'--include primitive types suchs as 'int' and 'dur'? 'includeSpecial'--include special types such as '@array' and '@function'? The results are further filtered by 'includeBuiltin'--include builtin types? 'includeChugins'--include types imported from chugins? 'includeImports'--include types imported from CK files in library path? 'includeUserDefined'--include types defined in ChucK code?";
    // if( !type_engine_import_sfun( env, func ) ) goto error;

    // add getTypesAll()
    func = make_new_sfun( "Type[]", "getTypes", type_getTypesAll );
    func->doc = "retrieves all top-level Types currently in the type system.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}





//-----------------------------------------------------------------------------
// name: init_primitive_types() | 1.5.0.0 (ge)
// desc: initialize all primitive Type types
//-----------------------------------------------------------------------------
t_CKBOOL init_primitive_types( Chuck_Env * env )
{
    //-----------------------------------------------------------------------------
    // init void
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_void, env->global(), NULL, NULL,
        "the type of nothingness, or the lack of a type.") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init int
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_int, env->global(), NULL, NULL,
        "the primitive integer type.") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init float
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_float, env->global(), NULL, NULL,
        "the primitive floating point type.") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init time
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_time, env->global(), NULL, NULL,
        "the primitive type of a point in time.") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init dur
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_dur, env->global(), NULL, NULL,
        "the primitive type of a length of time.") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init complex
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_complex, env->global(), NULL, NULL,
        "the primitive type of a complex pair #(re,im).") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    //-----------------------------------------------------------------------------
    // init polar
    //-----------------------------------------------------------------------------
    if( !type_engine_import_class_begin( env, env->ckt_polar, env->global(), NULL, NULL,
        "the primitive type of a polar value %(mag,phase)") ) return FALSE;
    // end the class import
    type_engine_import_class_end( env );

    return TRUE;
}

// Object ctor
CK_DLL_CTOR( object_ctor )
{
    // log
    EM_log( CK_LOG_ALL, "Object constructor..." );

    // initialize
    OBJ_MEMBER_UINT(SELF, Object_offset_string) = 0;
}


// Object dtor
CK_DLL_DTOR( object_dtor )
{
    // log
    EM_log( CK_LOG_ALL, "Object destructor..." );

    // NOTE the garbage collector should take care of
    // string reference at offset Object_offset_string
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
        str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
        // check it
        if( !str )
        {
            // TODO: make this exception
            CK_FPRINTF_STDERR( "[chuck]: Object.toString() out of memory!\n" );
            RETURN->v_object = NULL;
            return;
        }
        // 1.5.0.0 (ge) assign to object
        OBJ_MEMBER_UINT(SELF, Object_offset_string) = (t_CKUINT)str;
        // 1.5.0.0 (ge) | add ref
        str->add_ref();

        // set it
        ostringstream strout( ostringstream::out );
        // get the type
        Chuck_Type * type = SELF->type_ref;
        // write
        strout.setf( ios::hex, ios::basefield );
        strout << ((type != NULL) ? type->c_name() : "[VOID]") << ":" << (t_CKUINT)SELF << " (refcount=" << SELF->m_ref_count << ")";
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

// get the type info
CK_DLL_SFUN( object_typeInfo )
{
    // default is to return the type...
    RETURN->v_object = TYPE;

    // if actual type is indicated...
    if( TYPE->actual_type )
    {
        // what we return depends on if TYPE is an array type or not
        if( TYPE->array_depth == 0 )
        {
            // for cases like X.typeInfo() where X is a class
            RETURN->v_object = TYPE->actual_type;
        }
    }
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
    // added 1.3.0.0 -- Chuck_DL_Api::instance()
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
    // added 1.3.0.0 -- Chuck_DL_Api::instance()
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
    // added 1.3.0.0 -- Chuck_DL_Api::instance()
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
    Chuck_Object * blob = instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_uanablob, SHRED );
    // check
    if( blob == NULL )
    {
        // error message
        EM_error3( "UAna constructor unable to instantiate UAnaBlob; bailing out!" );
        // everything has gone to poop
        assert( FALSE );
        // not even gonna get here
        return;
    }
    // make a blob proxy
    Chuck_UAnaBlobProxy * proxy = new Chuck_UAnaBlobProxy( blob );
    // remember it
    OBJ_MEMBER_INT(SELF, uana_offset_blob) = (t_CKINT)proxy;
    // HACK: DANGER: manually call blob's ctor (added 1.3.0.0 -- Chuck_DL_Api::instance())
    uanablob_ctor( blob, NULL, VM, SHRED, API );
}

CK_DLL_DTOR( uana_dtor )
{
    // get the blob
    Chuck_UAnaBlobProxy * blob = (Chuck_UAnaBlobProxy *)OBJ_MEMBER_INT(SELF, uana_offset_blob);
    // delete the blob proxy | 1.5.0.0 (ge) added
    CK_SAFE_DELETE( blob ); // this should also clean up actual blob reference
    // zero out
    OBJ_MEMBER_INT(SELF, uana_offset_blob) = 0;
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
        // added 1.3.0.0 -- Chuck_DL_Api::instance()
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
    Chuck_ArrayFloat & fvals = blob->fvals();
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
    Chuck_ArrayVec2 & cvals = blob->cvals();
    // check caps
    if( i < 0 || cvals.size() <= i ) RETURN->v_complex.re = RETURN->v_complex.im = 0;
    else
    {
        // get as vec2
        t_CKVEC2 val;
        cvals.get( i, &val );
        RETURN->v_vec2 = val;
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
    CK_SAFE_RELEASE( m_blob );
}

t_CKTIME & Chuck_UAnaBlobProxy::when()
{
    // TODO: DANGER: is this actually returning correct reference?!
    return OBJ_MEMBER_TIME(m_blob, uanablob_offset_when);
}

Chuck_ArrayFloat & Chuck_UAnaBlobProxy::fvals()
{
    // TODO: DANGER: is this actually returning correct reference?!
    Chuck_ArrayFloat * arr8 = (Chuck_ArrayFloat *)OBJ_MEMBER_INT(m_blob, uanablob_offset_fvals);
    assert( arr8 != NULL );
    return *arr8;
}

Chuck_ArrayVec2 & Chuck_UAnaBlobProxy::cvals()
{
    // TODO: DANGER: is this actually returning correct reference?!
    Chuck_ArrayVec2 * arr16 = (Chuck_ArrayVec2 *)OBJ_MEMBER_INT(m_blob, uanablob_offset_cvals);
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
    Chuck_ArrayFloat * arrF = new Chuck_ArrayFloat( 8 );
    initialize_object( arrF, SHRED->vm_ref->env()->ckt_array, SHRED, VM );
    CK_SAFE_ADD_REF( arrF );
    OBJ_MEMBER_INT(SELF, uanablob_offset_fvals) = (t_CKINT)arrF;

    // cvals (complex)
    Chuck_ArrayVec2 * arrC = new Chuck_ArrayVec2( 8 );
    initialize_object( arrC, SHRED->vm_ref->env()->ckt_array, SHRED, VM );
    CK_SAFE_ADD_REF( arrC );
    OBJ_MEMBER_INT(SELF, uanablob_offset_cvals) = (t_CKINT)arrC;
}

// dtor
CK_DLL_DTOR( uanablob_dtor )
{
    // set when to 0 for good measure
    OBJ_MEMBER_TIME(SELF, uanablob_offset_when) = 0;

    // typed object mvars are auto-released | 1.5.2.0
    // include uanablob_offset_fvals and uanablob_offset_cvals
}

CK_DLL_MFUN( uanablob_when )
{
    // set return
    RETURN->v_time = OBJ_MEMBER_TIME(SELF, uanablob_offset_when);
}

CK_DLL_MFUN( uanablob_fvals )
{
    // set return
    RETURN->v_object = (Chuck_ArrayFloat *)OBJ_MEMBER_INT(SELF, uanablob_offset_fvals);
}

CK_DLL_MFUN( uanablob_fval )
{
    // get index
    t_CKINT i = GET_NEXT_INT(ARGS);
    // get the fvals array
    Chuck_ArrayFloat * fvals = (Chuck_ArrayFloat *)OBJ_MEMBER_INT(SELF, uanablob_offset_fvals);
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
    Chuck_ArrayVec2 * cvals = (Chuck_ArrayVec2 *)OBJ_MEMBER_INT(SELF, uanablob_offset_cvals);
    // check caps
    if( i < 0 || cvals->size() <= i ) RETURN->v_complex.re = RETURN->v_complex.im = 0;
    else
    {
        // get
        t_CKVEC2 val;
        cvals->get( i, &val );
        RETURN->v_vec2 = val;
    }
}

CK_DLL_MFUN( uanablob_cvals )
{
    // set return
    RETURN->v_object = (Chuck_ArrayVec2 *)OBJ_MEMBER_INT(SELF, uanablob_offset_cvals);
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

// default implementation
CK_DLL_MFUN( event_can_wait )
{
    RETURN->v_int = TRUE;
}

// 1.5.1.5 (ge/andrew) added; default implementation
CK_DLL_MFUN( event_waiting_on )
{
    // do nothing here; this function could be overridden as needed
    // FYI get the calling shred with 'SHRED' parameter to this function
}




//-----------------------------------------------------------------------------
// vec2 API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( vec2_set )
{
    // HACK: this is a particularly horrid cast from (Chuck_Object *)
    // t_CKVEC3 is neither a super- or sub-class of Chuck_Object...
    t_CKVEC2 * vec2 = (t_CKVEC2 *)SELF;
    // get index
    vec2->x = GET_NEXT_FLOAT(ARGS);
    vec2->y = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec2_setAll )
{
    // get data pointer
    t_CKVEC2 * vec2 = (t_CKVEC2 *)SELF;
    // get index
    vec2->x = vec2->y = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN( vec2_magnitude )
{
    // get data pointer
    t_CKVEC2 * vec2 = (t_CKVEC2 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec2->x*vec2->x + vec2->y*vec2->y );
    // return
    RETURN->v_float = mag;
}

CK_DLL_MFUN( vec2_normalize )
{
    // get data pointer
    t_CKVEC2 * vec2 = (t_CKVEC2 *)SELF;
    // compute magnitude
    t_CKFLOAT mag = ::sqrt( vec2->x*vec2->x + vec2->y*vec2->y );
    // check for zero
    if( mag > 0 )
    {
        vec2->x /= mag;
        vec2->y /= mag;
    }
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
    t_CKVEC4 * vec4 = (t_CKVEC4 *)SELF;
    // get index
    vec4->x = GET_NEXT_FLOAT(ARGS);
    vec4->y = GET_NEXT_FLOAT(ARGS);
    vec4->z = GET_NEXT_FLOAT(ARGS);
    vec4->w = GET_NEXT_FLOAT(ARGS);
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
    // get this shred pointer
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;
    // yield the shred in question | 1.5.0.5 (ge) refactored to call yield()
    derhs->yield();

    // WAS:
    // Chuck_VM * vm = derhs->vm_ref;
    // suspend
    // derhs->is_running = FALSE;
    // reshredule
    // vm->shreduler()->shredule( derhs, derhs->now );
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

    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( ( i < num ? derhs->args[i] : "" ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( shred_sourcePath ) // added 1.3.0.0
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;

    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( derhs->code->filename );
    RETURN->v_string = str;
}

CK_DLL_MFUN( shred_sourceDir ) // added 1.3.0.0
{
    Chuck_VM_Shred * derhs = (Chuck_VM_Shred *)SELF;

    // new chuck string
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // set the content
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
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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


CK_DLL_MFUN( shred_ctrl_hintChildMemSize ) // 1.5.1.5
{
    // get arg
    t_CKINT sizeInBytes = GET_NEXT_INT(ARGS);
    // set
    RETURN->v_int = SHRED->childSetMemSize( sizeInBytes );
}


CK_DLL_MFUN( shred_cget_hintChildMemSize ) // 1.5.1.5
{
    // set
    RETURN->v_int = SHRED->childGetMemSize();
}


CK_DLL_MFUN( shred_ctrl_hintChildRegSize ) // 1.5.1.5
{
    // get arg
    t_CKINT sizeInBytes = GET_NEXT_INT(ARGS);
    // set
    RETURN->v_int = SHRED->childSetRegSize( sizeInBytes );
}


CK_DLL_MFUN( shred_cget_hintChildRegSize ) // 1.5.1.5
{
    // set
    RETURN->v_int = SHRED->childGetRegSize();
}


CK_DLL_SFUN( shred_parent ) // added 1.5.2.0 (nshaheed)
{
    // get the parent
    Chuck_VM_Shred * parent = SHRED->parent;
    // set return value
    RETURN->v_object = parent;
}


CK_DLL_SFUN( shred_ancestor ) // added 1.5.2.0 (nshaheed)
{
    // current shred
    Chuck_VM_Shred * curr = SHRED;

    // iterate up until parent is null; it's possible that
    // ancestor() returns the calling shred, if called on
    // the top-level "ancestor" shred
    while( curr->parent != NULL )
    {
        // set curr as parent
        curr = curr->parent;
    }

    // set return value
    RETURN->v_object = curr;
}


CK_DLL_MFUN( shred_gc ) // added 1.5.2.0 (ge)
{
    // invoke manual per-shred GC pass
    SHRED->gc();
}




//-----------------------------------------------------------------------------
// string API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( string_ctor_str )
{
    // self (the string being constructed)
    Chuck_String * self = (Chuck_String *)SELF;
    // get arg
    Chuck_String * rhs = GET_NEXT_STRING(ARGS);
    // set it
    self->set( rhs ? rhs->str() : "" );
}

CK_DLL_MFUN( string_length )
{
    Chuck_String * s = (Chuck_String *)SELF;
    RETURN->v_int = s->str().length();
}

CK_DLL_MFUN( string_upper )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( toupper( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_lower )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( tolower( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_ltrim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( ltrim( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_rtrim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    str->set( rtrim( s->str() ) );
    RETURN->v_string = str;
}

CK_DLL_MFUN( string_trim )
{
    Chuck_String * s = (Chuck_String *)SELF;
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", index);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", index);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
        RETURN->v_string = NULL;
        return;
    }

    Chuck_String * ss = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->ckt_string, SHRED);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
        RETURN->v_string = NULL;
        return;
    }

    if(length < 0 || start+length > str->str().length())
    {
        ck_throw_exception(SHRED, "IndexOutOfBounds", length);
        RETURN->v_string = NULL;
        return;
    }

    Chuck_String * ss = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->ckt_string, SHRED);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", position);
        return;
    }
    if(str2 == NULL)
    {
        ck_throw_exception(SHRED, "NullPointer");
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", position);
        return;
    }
    if(str2 == NULL)
    {
        ck_throw_exception(SHRED, "NullPointer");
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", position);
        return;
    }

    if(length < 0 || position+length > str->str().length())
    {
        ck_throw_exception(SHRED, "IndexOutOfBounds", length);
        return;
    }

    if(str2 == NULL)
    {
        ck_throw_exception(SHRED, "NullPointer");
        return;
    }

    // str->str.replace(position, length, str2->str);
    std::string s = str->str();
    s.replace( position, length, str2->str() );
    str->set( s );
}

// string.replace(old, new) | 1.5.1.3 (nshaheed) added
CK_DLL_MFUN(string_replace_str)
{
    // this is just a wrapper around std::replace
    Chuck_String * str = (Chuck_String *) SELF;
    std::string from = GET_NEXT_STRING_SAFE(ARGS);
    std::string to = GET_NEXT_STRING_SAFE(ARGS);

    std::string s = str->str();

    // Taken from https://stackoverflow.com/a/3418285/20065423
    if(from.empty())
        return;
    size_t start_pos = 0;

    while((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }

    str->set(s);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
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
        ck_throw_exception(SHRED, "IndexOutOfBounds", start);
        return;
    }

    if(length < 0 || start+length > str->str().length())
    {
        ck_throw_exception(SHRED, "IndexOutOfBounds", length);
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
#ifdef __PLATFORM_WINDOWS__
    // SPENCERTODO: make this legit on windows
    if(i == string::npos)
        i = str->str().rfind('\\', str->str().length()-2);
#endif // __PLATFORM_WINDOWS__

    Chuck_String * parent = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->ckt_string, SHRED);

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

// array.zero() | 1.5.0.0 (ge) added
CK_DLL_MFUN( array_zero )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    array->zero();
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
CK_DLL_MFUN( array_map_find )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    Chuck_String * name = GET_NEXT_STRING( ARGS );
    RETURN->v_int = array->map_find( name->str() );
}

// array.erase()
CK_DLL_MFUN( array_map_erase )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    Chuck_String * name = GET_NEXT_STRING( ARGS );
    RETURN->v_int = array->map_erase( name->str() );
}

// array.shuffle()
CK_DLL_MFUN( array_shuffle )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    array->shuffle();
}

// array.sort()
CK_DLL_MFUN( array_sort )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    array->sort();
}

// array.push_back()
CK_DLL_MFUN( array_push_back )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    // ISSUE: 64-bit (fixed 1.3.1.0 using data kind)
    if( array->data_type_kind() == CHUCK_ARRAYINT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayInt *)array)->push_back( GET_NEXT_UINT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYFLOAT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayFloat *)array)->push_back( GET_NEXT_FLOAT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC2_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec2 *)array)->push_back( GET_NEXT_VEC2( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC3_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec3 *)array)->push_back( GET_NEXT_VEC3( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC4_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec4 *)array)->push_back( GET_NEXT_VEC4( ARGS ) );
    else
        assert( FALSE );
}


// array.insert()
CK_DLL_MFUN( array_insert )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    t_CKINT position = GET_NEXT_INT( ARGS );

    // ISSUE: 64-bit (fixed 1.3.1.0 using data kind)
    if( array->data_type_kind() == CHUCK_ARRAYINT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayInt *)array)->insert( position, GET_NEXT_UINT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYFLOAT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayFloat *)array)->insert( position, GET_NEXT_FLOAT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC2_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec2 *)array)->insert( position, GET_NEXT_VEC2( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC3_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec3 *)array)->insert( position, GET_NEXT_VEC3( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC4_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec4 *)array)->insert( position, GET_NEXT_VEC4( ARGS ) );
    else
        assert( FALSE );
}


// array.pop_back()
CK_DLL_MFUN( array_pop_back )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    RETURN->v_int = array->pop_back();
}

// array.push_front()
CK_DLL_MFUN( array_push_front )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    // ISSUE: 64-bit (fixed 1.3.1.0 using data kind)
    if( array->data_type_kind() == CHUCK_ARRAYINT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayInt *)array)->push_front( GET_NEXT_UINT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYFLOAT_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayFloat *)array)->push_front( GET_NEXT_FLOAT( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC2_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec2 *)array)->push_front( GET_NEXT_VEC2( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC3_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec3 *)array)->push_front( GET_NEXT_VEC3( ARGS ) );
    else if( array->data_type_kind() == CHUCK_ARRAYVEC4_DATAKIND )
        RETURN->v_int = ((Chuck_ArrayVec4 *)array)->push_front( GET_NEXT_VEC4( ARGS ) );
    else
        assert( FALSE );
}

// array.pop_front()
CK_DLL_MFUN( array_pop_front )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    RETURN->v_int = array->pop_front();
}

// array.erase
CK_DLL_MFUN( array_erase )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    t_CKINT position = GET_NEXT_INT(ARGS);
    RETURN->v_int = array->erase( position );
}

// array.erase
CK_DLL_MFUN( array_erase2 )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    t_CKINT begin = GET_NEXT_INT(ARGS);
    t_CKINT end = GET_NEXT_INT(ARGS);
    RETURN->v_int = array->erase( begin, end );
}

// 1.4.1.1 nshaheed (added) array.getKeys()
CK_DLL_MFUN( array_get_keys )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    Chuck_ArrayInt * returned_keys = (Chuck_ArrayInt *) GET_NEXT_OBJECT(ARGS);

    // clear return array
    returned_keys->set_size(0);
    // local keys array
    std::vector<std::string> array_keys;
    // get the keys
    array->get_keys( array_keys );

    // copy array keys into the provided string array
    for (t_CKUINT i = 0; i < array_keys.size(); i++ ) {
        Chuck_String * key = (Chuck_String *)instantiate_and_initialize_object(SHRED->vm_ref->env()->ckt_string, SHRED);
        key->set(array_keys[i]);
        returned_keys->push_back((t_CKUINT) key);
    }
}

// 1.5.0.0 azaday (added) array.reverse()
CK_DLL_MFUN( array_reverse )
{
    Chuck_Array * array = (Chuck_Array *)SELF;
    array->reverse();
}



//-----------------------------------------------------------------------------
// Type implementation
// 1.5.0.0 (ge) added
//-----------------------------------------------------------------------------
static void typeGetTypes(
    Chuck_VM * vm,
    Chuck_ArrayInt * ret,
    t_CKBOOL isObj,
    t_CKBOOL isPrim,
    t_CKBOOL isSpecial,
    t_CKBOOL isBuiltin,
    t_CKBOOL isChug,
    t_CKBOOL isImport,
    t_CKBOOL isUser )
{
    // results
    vector<Chuck_Type *> types;
    // get types
    vm->env()->nspc_top()->get_types( types );
    // clear
    ret->m_vector.clear();
    // iterate
    for( t_CKINT i = 0; i < types.size(); i++ )
    {
        // check special
        t_CKBOOL special = (types[i]->base_name.length()>0 && types[i]->base_name[0] == '@');
        // if not requesting special types
        if( !isSpecial && (special == TRUE) ) continue;
        // check origin
        ckte_Origin origin = types[i]->originHint;

        // filter level 1
        if( (isObj && isobj(vm->env(), types[i])) ||
            (isPrim && isprim(vm->env(), types[i])) )
        {
            // filter level 2
            if( (isBuiltin && (origin == ckte_origin_BUILTIN)) ||
                (isChug && (origin == ckte_origin_CHUGIN)) ||
                (isImport && (origin == ckte_origin_IMPORT)) ||
                (isUser && (origin == ckte_origin_USERDEFINED)) )
            {
                // copy
                ret->m_vector.push_back( (t_CKINT)types[i] );
                // add reference
                CK_SAFE_ADD_REF( types[i] );
            }
        }
    }
}

CK_DLL_CTOR( type_ctor )
{
}

CK_DLL_DTOR( type_dtor )
{
}

CK_DLL_MFUN( type_equals )
{
    // get self as type
    Chuck_Type * lhs = (Chuck_Type *)SELF;
    // get arg
    Chuck_Type * rhs = (Chuck_Type *)GET_NEXT_OBJECT(ARGS);
    // check
    if( rhs == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }
    // query
    RETURN->v_int = equals( lhs, rhs );
}

CK_DLL_MFUN( type_isa )
{
    // get self as type
    Chuck_Type * lhs = (Chuck_Type *)SELF;
    // get arg
    Chuck_Type * rhs = (Chuck_Type *)GET_NEXT_OBJECT(ARGS);
    // check
    if( rhs == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }
    // query
    RETURN->v_int = isa( lhs, rhs );
}

CK_DLL_MFUN( type_isa_str )
{
    // get self as type
    Chuck_Type * lhs = (Chuck_Type *)SELF;
    // get arg
    Chuck_String * another = GET_NEXT_STRING(ARGS);
    // default
    RETURN->v_int = FALSE;

    // check
    if( another == NULL ) return;
    // get the type
    Chuck_Type * rhs = type_engine_find_type( VM->env(), another->str() );
    // check
    if( rhs == NULL ) return;
    // check
    RETURN->v_int = isa( lhs, rhs );
}

CK_DLL_MFUN( type_basename )
{
    // get self as type
    Chuck_Type * type = (Chuck_Type *)SELF;
    // new it
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // set
    str->set( type->base_name );
    // return
    RETURN->v_object = str;
}

CK_DLL_MFUN( type_name )
{
    // get self as type
    Chuck_Type * type = (Chuck_Type *)SELF;
    // new it
    Chuck_String * str = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    // construct the full type name (arrays included)
    string name = type->base_name;
    // append array depth
    for( t_CKINT i = 0; i < type->array_depth; i++ ) name += "[]";
    // set
    str->set( name );
    // return
    RETURN->v_object = str;
}

CK_DLL_MFUN( type_parent )
{
    // get self as type
    Chuck_Type * type = (Chuck_Type *)SELF;
    // get parent type
    RETURN->v_object = type->parent;
}

CK_DLL_MFUN( type_children )
{
    // get me
    Chuck_Type * me = (Chuck_Type *)SELF;
    // name
    string name = me->base_name;

    // instantiate
    Chuck_ArrayInt * ret = new Chuck_ArrayInt( TRUE );
    initialize_object( ret, VM->env()->ckt_array, SHRED, VM );

    // results
    vector<Chuck_Type *> types;
    // get types
    VM->env()->nspc_top()->get_types( types );
    // clear
    ret->m_vector.clear();
    // iterate
    for( t_CKINT i = 0; i < types.size(); i++ )
    {
        // skip me
        if( equals( types[i], me ) ) continue;
        // skip if not a subclass of me
        if( !isa( types[i], me ) ) continue;
        // copy
        ret->m_vector.push_back( (t_CKINT)types[i] );
        // add reference
        CK_SAFE_ADD_REF( types[i] );
    }

    // return
    RETURN->v_object = ret;
}

CK_DLL_MFUN( type_origin )
{
    // me, the type
    Chuck_Type * type = (Chuck_Type *)SELF;
    // return type
    Chuck_String * origin = (Chuck_String *)instantiate_and_initialize_object( VM->env()->ckt_string, VM );
    // string
    string s = "";
    // check origin hint
    switch( type->originHint )
    {
    case ckte_origin_BUILTIN:
        s = "builtin";
        break;
    case ckte_origin_CHUGIN:
        s = "chugin";
        break;
    case ckte_origin_IMPORT:
        s = "cklib";
        break;
    case ckte_origin_USERDEFINED:
        s = "user";
        break;
    case ckte_origin_GENERATED:
        s = "generated";
        break;

    case ckte_origin_UNKNOWN:
    default:
        s = "[unknown origin]";
        break;
    }

    // set
    origin->set( s );
    // return
    RETURN->v_object = origin;
}

CK_DLL_MFUN( type_isPrimitive )
{
    Chuck_Type * type = (Chuck_Type *)SELF;
    RETURN->v_int = isprim( VM->env(), type );
}

CK_DLL_MFUN( type_isObject )
{
    Chuck_Type * type = (Chuck_Type *)SELF;
    RETURN->v_int = isobj( VM->env(), type );
}

CK_DLL_MFUN( type_isArray )
{
    Chuck_Type * type = (Chuck_Type *)SELF;
    // test for arrayhood -- either array depth > 0 or type could be "@array" | 1.5.2.0
    RETURN->v_int = type->array_depth > 0 || isa(type, type->env()->ckt_array);
}

CK_DLL_MFUN( type_arrayDims )
{
    Chuck_Type * type = (Chuck_Type *)SELF;
    RETURN->v_int = type->array_depth;
}

CK_DLL_SFUN( type_findString )
{
    // get arg
    Chuck_String * another = GET_NEXT_STRING(ARGS);
    // check
    if( another == NULL )
    {
        RETURN->v_object = NULL;
        return;
    }
    // get the type
    RETURN->v_object = type_engine_find_type( VM->env(), another->str() );
}

CK_DLL_SFUN( type_typeOf_obj )
{
    // get arg
    Chuck_Object * obj = GET_NEXT_OBJECT(ARGS);
    // check
    if( obj == NULL )
    {
        RETURN->v_object = NULL;
        return;
    }

    // get type
    // NOTE: Chuck_Object->type_ref may have different semantics for typeOf() and Type.of()
    // [1.0] @=> float array[];
    // <<< array.typeOf().isArray() >>>;
    // <<< Type.of(array).isArray() >>>;
    // TODO: do these need to be unified?
    RETURN->v_object = obj->type_ref;
}

CK_DLL_SFUN( type_typeOf_int )
{
    RETURN->v_object = VM->env()->ckt_int;
}

CK_DLL_SFUN( type_typeOf_float )
{
    RETURN->v_object = VM->env()->ckt_float;
}

CK_DLL_SFUN( type_typeOf_time )
{
    RETURN->v_object = VM->env()->ckt_time;
}

CK_DLL_SFUN( type_typeOf_dur )
{
    RETURN->v_object = VM->env()->ckt_dur;
}

CK_DLL_SFUN( type_typeOf_complex )
{
    RETURN->v_object = VM->env()->ckt_complex;
}

CK_DLL_SFUN( type_typeOf_polar )
{
    RETURN->v_object = VM->env()->ckt_polar;
}

CK_DLL_SFUN( type_typeOf_vec3 )
{
    RETURN->v_object = VM->env()->ckt_vec3;
}

CK_DLL_SFUN( type_typeOf_vec4 )
{
    RETURN->v_object = VM->env()->ckt_vec4;
}

CK_DLL_SFUN( type_getTypes )
{
    // instantiate
    Chuck_ArrayInt * array = new Chuck_ArrayInt( TRUE );
    initialize_object( array, VM->env()->ckt_array, SHRED, VM );

    // get args
    t_CKINT attributes = GET_NEXT_INT(ARGS);
    t_CKINT origins = GET_NEXT_INT(ARGS);

    // get args
    t_CKBOOL objs = (attributes & TYPE_ATTRIB_OBJECT) != 0;
    t_CKBOOL prim = (attributes & TYPE_ATTRIB_PRIMITIVE) != 0;
    t_CKBOOL special = (attributes & TYPE_ATTRIB_SPECIAL) != 0;
    t_CKBOOL builtin = (origins & TYPE_ORIGIN_BUILTIN) != 0;
    t_CKBOOL chugins = (origins & TYPE_ORIGIN_CHUGIN) != 0;
    t_CKBOOL imports = (origins & TYPE_ORIGIN_CKLIB) != 0;
    t_CKBOOL user = (origins & TYPE_ORIGIN_USER) != 0;

    // get types with the flags
    typeGetTypes( VM, array, objs, prim, special, builtin, chugins, imports, user );
    // return
    RETURN->v_object = array;
}

CK_DLL_SFUN( type_getTypes2 )
{
    // instantiate
    Chuck_ArrayInt * array = new Chuck_ArrayInt( TRUE );
    initialize_object( array, VM->env()->ckt_array, SHRED, VM );

    // get args
    t_CKBOOL objs = GET_NEXT_INT(ARGS);
    t_CKBOOL prim = GET_NEXT_INT(ARGS);
    t_CKBOOL special = GET_NEXT_INT(ARGS);
    t_CKBOOL builtin = GET_NEXT_INT(ARGS);
    t_CKBOOL chugins = GET_NEXT_INT(ARGS);
    t_CKBOOL imports = GET_NEXT_INT(ARGS);
    t_CKBOOL user = GET_NEXT_INT(ARGS);

    // get types with the flags
    typeGetTypes( VM, array, objs, prim, special, builtin, chugins, imports, user );
    // return
    RETURN->v_object = array;
}

CK_DLL_SFUN( type_getTypesAll )
{
    // instantiate
    Chuck_ArrayInt * array = new Chuck_ArrayInt( TRUE );
    initialize_object( array, VM->env()->ckt_array, SHRED, VM );

    // get all types
    typeGetTypes( VM, array, true, true, true, true, true, true, true );
    // return
    RETURN->v_object = array;
}
