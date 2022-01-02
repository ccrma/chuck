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
// file: chuck_globals.h
// desc: system to support in-language globals; added 1.4.1.0
//       historically, the old chuck_globals.h/cpp supported implementation
//       globals; these files were removed during REFACTOR-2017, which moved
//       away from a monolithic VM model to support for many VM; implementation
//       globals were refactored to be per-VM; the new chuck_globals.h/cpp,
//       reinstated in 2021, supports in-language globals (using the "global"
//       keyword) and integration with external hosts (such as Chunity).
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
//         Ge Wang (ge@ccrma.stanford.edu)
// date: Spring 2021
//-----------------------------------------------------------------------------
#ifndef __CHUCK_GLOBALS_H__
#define __CHUCK_GLOBALS_H__

#include "chuck_def.h"
#include "chuck_type.h"
#include "chuck_ugen.h"
#include <string>

// forward reference for ChucK VM
struct Chuck_VM;
struct Chuck_VM_Shred;
struct Chuck_Msg;



// global variables & management -- added 1.4.1.0 (jack)
// forward references for global messages
struct Chuck_Set_Global_Int_Request;
struct Chuck_Get_Global_Int_Request;
struct Chuck_Set_Global_Float_Request;
struct Chuck_Get_Global_Float_Request;
struct Chuck_Signal_Global_Event_Request;
struct Chuck_Listen_For_Global_Event_Request;
struct Chuck_Set_Global_String_Request;
struct Chuck_Get_Global_String_Request;
struct Chuck_Set_Global_Int_Array_Request;
struct Chuck_Get_Global_Int_Array_Request;
struct Chuck_Set_Global_Int_Array_Value_Request;
struct Chuck_Get_Global_Int_Array_Value_Request;
struct Chuck_Set_Global_Associative_Int_Array_Value_Request;
struct Chuck_Get_Global_Associative_Int_Array_Value_Request;
struct Chuck_Set_Global_Float_Array_Request;
struct Chuck_Get_Global_Float_Array_Request;
struct Chuck_Set_Global_Float_Array_Value_Request;
struct Chuck_Get_Global_Float_Array_Value_Request;
struct Chuck_Set_Global_Associative_Float_Array_Value_Request;
struct Chuck_Get_Global_Associative_Float_Array_Value_Request;
struct Chuck_Execute_Chuck_Msg_Request;

// forward references for global storage
struct Chuck_Global_Int_Container;
struct Chuck_Global_Float_Container;
struct Chuck_Global_String_Container;
struct Chuck_Global_Event_Container;
struct Chuck_Global_UGen_Container;
struct Chuck_Global_Array_Container;
struct Chuck_Global_Object_Container;




//-----------------------------------------------------------------------------
// name: enum Global_Request_Type
// desc: what kind of global message request is this? (REFACTOR-2017)
//-----------------------------------------------------------------------------
enum Chuck_Global_Request_Type
{
    // primitives
    set_global_int_request,
    get_global_int_request,
    set_global_float_request,
    get_global_float_request,
    set_global_string_request,
    get_global_string_request,
    // events
    signal_global_event_request,
    listen_for_global_event_request,
    // int arrays
    set_global_int_array_request,
    get_global_int_array_request,
    set_global_int_array_value_request,
    get_global_int_array_value_request,
    set_global_associative_int_array_value_request,
    get_global_associative_int_array_value_request,
    // float arrays
    set_global_float_array_request,
    get_global_float_array_request,
    set_global_float_array_value_request,
    get_global_float_array_value_request,
    set_global_associative_float_array_value_request,
    get_global_associative_float_array_value_request,
    // shreds
    spork_shred_request,
    // chuck_msg
    execute_chuck_msg_request
};




//-----------------------------------------------------------------------------
// name: struct Global_Request
// desc: a global request (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Global_Request
{
    Chuck_Global_Request_Type type;
    t_CKUINT retries;
    union {
        // primitives
        Chuck_Set_Global_Int_Request* setIntRequest;
        Chuck_Get_Global_Int_Request* getIntRequest;
        Chuck_Set_Global_Float_Request* setFloatRequest;
        Chuck_Get_Global_Float_Request* getFloatRequest;
        Chuck_Set_Global_String_Request* setStringRequest;
        Chuck_Get_Global_String_Request* getStringRequest;
        // events
        Chuck_Signal_Global_Event_Request* signalEventRequest;
        Chuck_Listen_For_Global_Event_Request* listenForEventRequest;
        // int arrays
        Chuck_Set_Global_Int_Array_Request* setIntArrayRequest;
        Chuck_Get_Global_Int_Array_Request* getIntArrayRequest;
        Chuck_Set_Global_Int_Array_Value_Request* setIntArrayValueRequest;
        Chuck_Get_Global_Int_Array_Value_Request* getIntArrayValueRequest;
        Chuck_Set_Global_Associative_Int_Array_Value_Request* setAssociativeIntArrayValueRequest;
        Chuck_Get_Global_Associative_Int_Array_Value_Request* getAssociativeIntArrayValueRequest;
        // float arrays
        Chuck_Set_Global_Float_Array_Request* setFloatArrayRequest;
        Chuck_Get_Global_Float_Array_Request* getFloatArrayRequest;
        Chuck_Set_Global_Float_Array_Value_Request* setFloatArrayValueRequest;
        Chuck_Get_Global_Float_Array_Value_Request* getFloatArrayValueRequest;
        Chuck_Set_Global_Associative_Float_Array_Value_Request* setAssociativeFloatArrayValueRequest;
        Chuck_Get_Global_Associative_Float_Array_Value_Request* getAssociativeFloatArrayValueRequest;
        // shreds
        Chuck_VM_Shred* shred;
        // chuck_msg
        Chuck_Execute_Chuck_Msg_Request* executeChuckMsgRequest;
    };
    
    Chuck_Global_Request()
    {
        retries = 0;
    }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Globals_Manager
// desc: manager for globals storage | added 1.4.1.0 (jack)
//-----------------------------------------------------------------------------
struct Chuck_Globals_Manager
{
public:
    Chuck_Globals_Manager( Chuck_VM * vm );
    ~Chuck_Globals_Manager();
    
public:
    // REFACTOR-2017: externally accessible + global variables.
    // use these getters and setters from outside the audio thread
    t_CKBOOL getGlobalInt( const char * name, void (*callback)(t_CKINT) );
    t_CKBOOL getGlobalInt( const char * name, void (*callback)(const char*, t_CKINT) );
    t_CKBOOL getGlobalInt( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKINT) );
    t_CKBOOL setGlobalInt( const char * name, t_CKINT val );
    
    t_CKBOOL getGlobalFloat( const char * name, void (*callback)(t_CKFLOAT) );
    t_CKBOOL getGlobalFloat( const char * name, void (*callback)(const char*, t_CKFLOAT) );
    t_CKBOOL getGlobalFloat( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKFLOAT) );
    t_CKBOOL setGlobalFloat( const char * name, t_CKFLOAT val );
    
    t_CKBOOL getGlobalString( const char * name, void (*callback)(const char*) );
    t_CKBOOL getGlobalString( const char * name, void (*callback)(const char*, const char*) );
    t_CKBOOL getGlobalString( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, const char*) );
    t_CKBOOL setGlobalString( const char * name, const char * val );
    
    t_CKBOOL signalGlobalEvent( const char * name );
    t_CKBOOL broadcastGlobalEvent( const char * name );
    t_CKBOOL listenForGlobalEvent( const char * name, void (*callback)(void), t_CKBOOL listen_forever );
    t_CKBOOL listenForGlobalEvent( const char * name, void (*callback)(const char*), t_CKBOOL listen_forever );
    t_CKBOOL listenForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT), t_CKBOOL listen_forever );
    t_CKBOOL stopListeningForGlobalEvent( const char * name, void (*callback)(void) );
    t_CKBOOL stopListeningForGlobalEvent( const char * name, void (*callback)(const char*) );
    t_CKBOOL stopListeningForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT) );
    
    t_CKBOOL getGlobalUGenSamples( const char * name, SAMPLE* buffer, int numFrames );
    
    t_CKBOOL setGlobalIntArray( const char * name, t_CKINT arrayValues[], t_CKUINT numValues );
    t_CKBOOL getGlobalIntArray( const char * name, void (*callback)(t_CKINT[], t_CKUINT) );
    t_CKBOOL getGlobalIntArray( const char * name, void (*callback)(const char*, t_CKINT[], t_CKUINT) );
    t_CKBOOL getGlobalIntArray( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKINT[], t_CKUINT) );
    t_CKBOOL setGlobalIntArrayValue( const char * name, t_CKUINT index, t_CKINT value );
    t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKUINT index, void (*callback)(t_CKINT) );
    t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKUINT index, void (*callback)(const char*, t_CKINT) );
    t_CKBOOL getGlobalIntArrayValue( const char * name, t_CKINT callbackID, t_CKUINT index, void (*callback)(t_CKINT, t_CKINT) );
    t_CKBOOL setGlobalAssociativeIntArrayValue( const char * name, const char * key, t_CKINT value );
    t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, const char * key, void (*callback)(t_CKINT) );
    t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, const char * key, void (*callback)(const char*, t_CKINT) );
    t_CKBOOL getGlobalAssociativeIntArrayValue( const char * name, t_CKINT callbackID, const char * key, void (*callback)(t_CKINT, t_CKINT) );
    
    t_CKBOOL setGlobalFloatArray( const char * name, t_CKFLOAT arrayValues[], t_CKUINT numValues );
    t_CKBOOL getGlobalFloatArray( const char * name, void (*callback)(t_CKFLOAT[], t_CKUINT) );
    t_CKBOOL getGlobalFloatArray( const char * name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT) );
    t_CKBOOL getGlobalFloatArray( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT, t_CKFLOAT[], t_CKUINT) );
    t_CKBOOL setGlobalFloatArrayValue( const char * name, t_CKUINT index, t_CKFLOAT value );
    t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKUINT index, void (*callback)(t_CKFLOAT) );
    t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKUINT index, void (*callback)(const char*, t_CKFLOAT) );
    t_CKBOOL getGlobalFloatArrayValue( const char * name, t_CKINT callbackID, t_CKUINT index, void (*callback)(t_CKINT, t_CKFLOAT) );
    t_CKBOOL setGlobalAssociativeFloatArrayValue( const char * name, const char * key, t_CKFLOAT value );
    t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, const char * key, void (*callback)(t_CKFLOAT) );
    t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, const char * key, void (*callback)(const char*, t_CKFLOAT) );
    t_CKBOOL getGlobalAssociativeFloatArrayValue( const char * name, t_CKINT callbackID, const char * key, void (*callback)(t_CKINT, t_CKFLOAT) );
    
public:
    // run Chuck_Msg in the globals order
    t_CKBOOL execute_chuck_msg_with_globals( Chuck_Msg* msg );
    
public:
    // REFACTOR-2017: externally accessible + global variables.
    // WARNING: these internal functions are to be used only by other
    // chuck code in the audio thread.
    t_CKBOOL init_global_int( const std::string & name );
    t_CKINT get_global_int_value( const std::string & name );
    t_CKINT* get_ptr_to_global_int( const std::string & name );
    
    t_CKBOOL init_global_float( const std::string & name );
    t_CKFLOAT get_global_float_value( const std::string & name );
    t_CKFLOAT* get_ptr_to_global_float( const std::string & name );
    
    t_CKBOOL init_global_string( const std::string & name );
    Chuck_String* get_global_string( const std::string & name );
    Chuck_String** get_ptr_to_global_string( const std::string & name );
    
    t_CKBOOL init_global_event( const std::string & name, Chuck_Type* type );
    Chuck_Event* get_global_event( const std::string & name );
    Chuck_Event** get_ptr_to_global_event( const std::string & name );
    
    t_CKBOOL init_global_ugen( const std::string & name, Chuck_Type* type );
    t_CKBOOL is_global_ugen_init( const std::string & name );
    t_CKBOOL is_global_ugen_valid( const std::string & name );
    Chuck_UGen* get_global_ugen( const std::string & name );
    Chuck_UGen** get_ptr_to_global_ugen( const std::string & name );
    
    t_CKBOOL init_global_array( const std::string & name, Chuck_Type* type, te_GlobalType arr_type );
    Chuck_Object* get_global_array( const std::string & name );
    Chuck_Object** get_ptr_to_global_array( const std::string & name );
    t_CKINT get_global_int_array_value( const std::string & name, t_CKUINT index );
    t_CKINT get_global_associative_int_array_value( const std::string & name, const std::string & key );
    t_CKFLOAT get_global_float_array_value( const std::string & name, t_CKUINT index );
    t_CKFLOAT get_global_associative_float_array_value( const std::string & name, const std::string & key );

    t_CKBOOL init_global_object( const std::string & name, Chuck_Type* type );
    t_CKBOOL is_global_object_init( const std::string & name );
    t_CKBOOL is_global_object_valid( const std::string & name );
    Chuck_Object* get_global_object( const std::string & name );
    Chuck_Object** get_ptr_to_global_object( const std::string & name );
    
    t_CKBOOL should_call_global_ctor( const std::string & name, te_GlobalType type );
    void global_ctor_was_called( const std::string & name, te_GlobalType type );
    
    // global variables -- clean up
    void cleanup_global_variables();
    
public: // these should ever ONLY be called from within the VM
    // request queue: add, query for size
    t_CKBOOL add_request( Chuck_Global_Request request );
    t_CKBOOL more_requests();
    // REFACTOR-2017: execute the messages from the global queue
    void handle_global_queue_messages();
    
private:
    // ptr to my vm
    Chuck_VM * m_vm;
    
    // global variables -- storage
    std::map< std::string, Chuck_Global_Int_Container* > m_global_ints;
    std::map< std::string, Chuck_Global_Float_Container* > m_global_floats;
    std::map< std::string, Chuck_Global_String_Container* > m_global_strings;
    std::map< std::string, Chuck_Global_Event_Container* > m_global_events;
    std::map< std::string, Chuck_Global_UGen_Container* > m_global_ugens;
    std::map< std::string, Chuck_Global_Array_Container* > m_global_arrays;
    std::map< std::string, Chuck_Global_Object_Container* > m_global_objects;

    // global variables -- messages for set, get, etc.
    XCircleBuffer< Chuck_Global_Request > m_global_request_queue;
    
    // retry queue: sometimes a request comes through exactly 1 sample
    // before the relevant global variable is constructed
    // (e.g. signalEvent before the compileCode that would construct the event)
    // in these cases, retrying 1 sample later usually works.
    // this is ok because the external host has no guarantee of sample-level
    // determinism, like we have within the ChucK VM
    XCircleBuffer< Chuck_Global_Request > m_global_request_retry_queue;
};




#endif
