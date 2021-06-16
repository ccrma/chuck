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
// file: chuck_globals.cpp
// desc: system to support in-language globals; added 1.4.1.0
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
//         Ge Wang (ge@ccrma.stanford.edu)
// date: Spring 2021
//-----------------------------------------------------------------------------
#include "chuck_globals.h"
#include "chuck_vm.h"
#include "chuck_instr.h"






//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Int_Request
// desc: container for messages to set global ints (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Int_Request
{
    std::string name;
    t_CKINT val;
    // constructor
    Chuck_Set_Global_Int_Request() : val(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Int_Request
// desc: container for messages to get global ints (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Int_Request
{
    std::string name;
    union {
        void (* cb_with_name)(const char *, t_CKINT);
        void (* cb_with_id)(t_CKINT, t_CKINT);
        void (* cb)(t_CKINT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Int_Request() : cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Float_Request
// desc: container for messages to set global floats (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Float_Request
{
    std::string name;
    t_CKFLOAT val;
    // constructor
    Chuck_Set_Global_Float_Request() : val(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Float_Request
// desc: container for messages to get global floats (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Float_Request
{
    std::string name;
    union {
        void (* cb_with_name)(const char *, t_CKFLOAT);
        void (* cb_with_id)(t_CKINT, t_CKFLOAT);
        void (* cb)(t_CKFLOAT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Float_Request() : cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Signal_Global_Event_Request
// desc: container for messages to signal global events (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Signal_Global_Event_Request
{
    std::string name;
    t_CKBOOL is_broadcast;
    // constructor
    Chuck_Signal_Global_Event_Request() : is_broadcast(TRUE) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Listen_For_Global_Event_Request
// desc: container for messages to wait on global events (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Listen_For_Global_Event_Request
{
    std::string name;
    t_CKBOOL listen_forever;
    t_CKBOOL deregister;
    union {
        void (* cb_with_name)(const char *);
        void (* cb_with_id)(t_CKINT);
        void (* cb)(void);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Listen_For_Global_Event_Request() : listen_forever(FALSE),
    deregister(FALSE), cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_String_Request
// desc: container for messages to set global strings (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_String_Request
{
    std::string name;
    std::string val;
    // constructor
    Chuck_Set_Global_String_Request() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_String_Request
// desc: container for messages to get global strings (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_String_Request
{
    std::string name;
    union {
        void (* cb_with_name)(const char *, const char *);
        void (* cb_with_id)(t_CKINT, const char *);
        void (* cb)(const char *);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_String_Request() : cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Int_Array_Request
// desc: container for messages to set global int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Int_Array_Request
{
    std::string name;
    std::vector< t_CKINT > arrayValues;
    // constructor
    Chuck_Set_Global_Int_Array_Request() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Int_Array_Request
// desc: container for messages to get global int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Int_Array_Request
{
    std::string name;
    union {
        void (* cb_with_name)(const char *, t_CKINT[], t_CKUINT);
        void (* cb_with_id)(t_CKINT, t_CKINT[], t_CKUINT);
        void (* cb)(t_CKINT[], t_CKUINT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Int_Array_Request() : cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Int_Array_Value_Request
// desc: container for messages to set individual elements of int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Int_Array_Value_Request
{
    std::string name;
    t_CKUINT index;
    t_CKINT value;
    // constructor
    Chuck_Set_Global_Int_Array_Value_Request() : index(-1), value(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Int_Array_Value_Request
// desc: container for messages to get individual elements of int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Int_Array_Value_Request
{
    std::string name;
    t_CKUINT index;
    union {
        void (* cb_with_name)(const char *, t_CKINT);
        void (* cb_with_id)(t_CKINT, t_CKINT);
        void (* cb)(t_CKINT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Int_Array_Value_Request() : index(-1), cb(NULL),
    cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Associative_Int_Array_Value_Request
// desc: container for messages to set elements of associative int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Associative_Int_Array_Value_Request
{
    std::string name;
    std::string key;
    t_CKINT value;
    // constructor
    Chuck_Set_Global_Associative_Int_Array_Value_Request() : value(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Associative_Int_Array_Value_Request
// desc: container for messages to get elements of associative int arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Associative_Int_Array_Value_Request
{
    std::string name;
    std::string key;
    union {
        void (* cb_with_name)(const char *, t_CKINT);
        void (* cb_with_id)(t_CKINT, t_CKINT);
        void (* cb)(t_CKINT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Associative_Int_Array_Value_Request() : cb(NULL),
    cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Float_Array_Request
// desc: container for messages to set global float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Float_Array_Request
{
    std::string name;
    std::vector< t_CKFLOAT > arrayValues;
    // constructor
    Chuck_Set_Global_Float_Array_Request() { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Float_Array_Request
// desc: container for messages to get global float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Float_Array_Request
{
    std::string name;
    union {
        void (* cb_with_name)(const char *, t_CKFLOAT[], t_CKUINT);
        void (* cb_with_id)(t_CKINT, t_CKFLOAT[], t_CKUINT);
        void (* cb)(t_CKFLOAT[], t_CKUINT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Float_Array_Request() : cb(NULL), cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Float_Array_Value_Request
// desc: container for messages to set individual elements of float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Float_Array_Value_Request
{
    std::string name;
    t_CKUINT index;
    t_CKFLOAT value;
    // constructor
    Chuck_Set_Global_Float_Array_Value_Request() : index(-1), value(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Float_Array_Value_Request
// desc: container for messages to get individual elements of float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Float_Array_Value_Request
{
    std::string name;
    t_CKUINT index;
    union {
        void (* cb_with_name)(const char *, t_CKFLOAT);
        void (* cb_with_id)(t_CKINT, t_CKFLOAT);
        void (* cb)(t_CKFLOAT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Float_Array_Value_Request() : index(-1), cb(NULL),
    cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Set_Global_Associative_Float_Array_Value_Request
// desc: container for messages to set elements of associative float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Set_Global_Associative_Float_Array_Value_Request
{
    std::string name;
    std::string key;
    t_CKFLOAT value;
    // constructor
    Chuck_Set_Global_Associative_Float_Array_Value_Request() : value(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Get_Global_Associative_Float_Array_Value_Request
// desc: container for messages to get elements of associative float arrays (REFACTOR-2017)
//-----------------------------------------------------------------------------
struct Chuck_Get_Global_Associative_Float_Array_Value_Request
{
    std::string name;
    std::string key;
    union {
        void (* cb_with_name)(const char *, t_CKFLOAT);
        void (* cb_with_id)(t_CKINT, t_CKFLOAT);
        void (* cb)(t_CKFLOAT);
    };
    Chuck_Global_Get_Callback_Type cb_type;
    t_CKINT id;
    // constructor
    Chuck_Get_Global_Associative_Float_Array_Value_Request() : cb(NULL),
    cb_type(ck_get_plain), id(0) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Execute_Chuck_Msg_Request
// desc: container for Chuck_Msg (but to be called from the globals callback
//       rather than the Chuck_Msg callback)
//-----------------------------------------------------------------------------
struct Chuck_Execute_Chuck_Msg_Request
{
    Chuck_Msg * msg;
    // constructor
    Chuck_Execute_Chuck_Msg_Request() : msg(NULL) { }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_Int_Container
// desc: container for global ints
//-----------------------------------------------------------------------------
struct Chuck_Global_Int_Container
{
    t_CKINT val;
    
    Chuck_Global_Int_Container() { val = 0; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_Float_Container
// desc: container for global floats
//-----------------------------------------------------------------------------
struct Chuck_Global_Float_Container
{
    t_CKFLOAT val;
    
    Chuck_Global_Float_Container() { val = 0; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_String_Container
// desc: container for global ints
//-----------------------------------------------------------------------------
struct Chuck_Global_String_Container
{
    Chuck_String * val;
    
    Chuck_Global_String_Container() { val = NULL; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_Event_Container
// desc: container for global events
//-----------------------------------------------------------------------------
struct Chuck_Global_Event_Container
{
    Chuck_Event * val;
    Chuck_Type * type;
    t_CKBOOL ctor_needs_to_be_called;
    
    Chuck_Global_Event_Container() { val = NULL; type = NULL;
        ctor_needs_to_be_called = TRUE; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_UGen_Container
// desc: container for global ugens
//-----------------------------------------------------------------------------
struct Chuck_Global_UGen_Container
{
    Chuck_UGen * val;
    Chuck_Type * type;
    t_CKBOOL ctor_needs_to_be_called;
    
    Chuck_Global_UGen_Container() { val = NULL; type = NULL;
        ctor_needs_to_be_called = TRUE; }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_Array_Container
// desc: container for global arrays
//-----------------------------------------------------------------------------
struct Chuck_Global_Array_Container
{
    Chuck_Object * array;
    te_GlobalType array_type;
    t_CKBOOL ctor_needs_to_be_called;
    
    Chuck_Global_Array_Container( te_GlobalType arr_type )
    {
        array = NULL;
        ctor_needs_to_be_called = FALSE;
        array_type = arr_type;
    }
};




//-----------------------------------------------------------------------------
// name: struct Chuck_Global_Object_Container
// desc: container for global ChucK objects
//-----------------------------------------------------------------------------
struct Chuck_Global_Object_Container
{
    Chuck_Object * val;
    Chuck_Type * type;
    t_CKBOOL ctor_needs_to_be_called;
    
    Chuck_Global_Object_Container() { val = NULL; type = NULL;
        ctor_needs_to_be_called = TRUE; }
};




//-----------------------------------------------------------------------------
// name: Chuck_Globals_Manager()
// desc: constructor: size queues appropriately
//-----------------------------------------------------------------------------
Chuck_Globals_Manager::Chuck_Globals_Manager( Chuck_VM * vm )
{
    // store
    m_vm = vm;
    
    // REFACTOR-2017: TODO might want to dynamically grow queue?
    m_global_request_queue.init( 16384 );
    m_global_request_retry_queue.init( 16384 );
}




//-----------------------------------------------------------------------------
// name: ~Chuck_Globals_Manager()
// desc: destructor: clean up
//-----------------------------------------------------------------------------
Chuck_Globals_Manager::~Chuck_Globals_Manager()
{
    cleanup_global_variables();
}




//-----------------------------------------------------------------------------
// name: add_request
// desc: add a request to the queue
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::add_request( Chuck_Global_Request request )
{
    m_global_request_queue.put( request );
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: more_requests
// desc: are there more requests left?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::more_requests()
{
    return m_global_request_queue.more();
}




//-----------------------------------------------------------------------------
// name: getGlobalInt()
// desc: get a global int by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalInt( const char * name,
                                              void (* callback)(t_CKINT) )
{
    Chuck_Get_Global_Int_Request * get_int_message =
        new Chuck_Get_Global_Int_Request;
    get_int_message->name = name;
    get_int_message->cb = callback;
    get_int_message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_int_request;
    r.getIntRequest = get_int_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalInt()
// desc: get a global int by name, with a by name callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalInt( const char * name,
                                              void (* callback)(const char *, t_CKINT) )
{
    Chuck_Get_Global_Int_Request * get_int_message =
        new Chuck_Get_Global_Int_Request;
    get_int_message->name = name;
    get_int_message->cb_with_name = callback;
    get_int_message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_int_request;
    r.getIntRequest = get_int_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalInt()
// desc: get a global int by name, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalInt( const char * name, t_CKINT id,
                                              void (* callback)(t_CKINT, t_CKINT) )
{
    Chuck_Get_Global_Int_Request * get_int_message =
        new Chuck_Get_Global_Int_Request;
    get_int_message->name = name;
    get_int_message->cb_with_id = callback;
    get_int_message->id = id;
    get_int_message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_int_request;
    r.getIntRequest = get_int_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}





//-----------------------------------------------------------------------------
// name: setGlobalInt()
// desc: set a global int by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalInt( const char * name, t_CKINT val )
{
    Chuck_Set_Global_Int_Request * set_int_message =
        new Chuck_Set_Global_Int_Request;
    set_int_message->name = name;
    set_int_message->val = val;
    
    Chuck_Global_Request r;
    r.type = set_global_int_request;
    r.setIntRequest = set_int_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_int()
// desc: tell the vm that a global int is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_int( const std::string & name )
{
    if( m_global_ints.count( name ) == 0 )
    {
        m_global_ints[name] = new Chuck_Global_Int_Container;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_global_int_value()
// desc: get a value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Globals_Manager::get_global_int_value( const std::string & name )
{
    // ensure exists
    init_global_int( name );
    
    return m_global_ints[name]->val;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_int()
// desc: get a pointer directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT * Chuck_Globals_Manager::get_ptr_to_global_int( const std::string & name )
{
    return &( m_global_ints[name]->val );
}




//-----------------------------------------------------------------------------
// name: getGlobalFloat()
// desc: get a global float by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloat( const char * name,
                                                void (* callback)(t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Request * get_float_message =
        new Chuck_Get_Global_Float_Request;
    get_float_message->name = name;
    get_float_message->cb = callback;
    get_float_message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_float_request;
    r.getFloatRequest = get_float_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloat()
// desc: get a global float by name, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloat( const char * name,
                                                void (* callback)(const char *, t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Request * get_float_message =
        new Chuck_Get_Global_Float_Request;
    get_float_message->name = name;
    get_float_message->cb_with_name = callback;
    get_float_message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_float_request;
    r.getFloatRequest = get_float_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloat()
// desc: get a global float by name, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloat( const char * name, t_CKINT id,
                                                void (* callback)(t_CKINT, t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Request * get_float_message =
        new Chuck_Get_Global_Float_Request;
    get_float_message->name = name;
    get_float_message->cb_with_id = callback;
    get_float_message->id = id;
    get_float_message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_float_request;
    r.getFloatRequest = get_float_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalFloat()
// desc: set a global float by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalFloat( const char * name,
                                                t_CKFLOAT val )
{
    Chuck_Set_Global_Float_Request * set_float_message =
        new Chuck_Set_Global_Float_Request;
    set_float_message->name = name;
    set_float_message->val = val;
    
    Chuck_Global_Request r;
    r.type = set_global_float_request;
    r.setFloatRequest = set_float_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_float()
// desc: tell the vm that a global float is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_float( const std::string & name )
{
    if( m_global_floats.count( name ) == 0 )
    {
        m_global_floats[name] = new Chuck_Global_Float_Container;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_global_float_value()
// desc: get a value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_Globals_Manager::get_global_float_value( const std::string & name )
{
    // ensure exists
    init_global_float( name );
    
    return m_global_floats[name]->val;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_float()
// desc: get a pointer directly to the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT * Chuck_Globals_Manager::get_ptr_to_global_float( const std::string & name )
{
    return &( m_global_floats[name]->val );
}




//-----------------------------------------------------------------------------
// name: getGlobalString()
// desc: get a global string by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalString( const char * name,
                                                 void (* callback)(const char *) )
{
    Chuck_Get_Global_String_Request * get_string_message =
        new Chuck_Get_Global_String_Request;
    get_string_message->name = name;
    get_string_message->cb = callback;
    get_string_message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_string_request;
    r.getStringRequest = get_string_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalString()
// desc: get a global string by name, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalString( const char * name,
                                                 void (* callback)(const char *, const char *) )
{
    Chuck_Get_Global_String_Request * get_string_message =
        new Chuck_Get_Global_String_Request;
    get_string_message->name = name;
    get_string_message->cb_with_name = callback;
    get_string_message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_string_request;
    r.getStringRequest = get_string_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalString()
// desc: get a global string by name, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalString( const char * name, t_CKINT id,
                                                 void (* callback)(t_CKINT, const char *) )
{
    Chuck_Get_Global_String_Request * get_string_message =
        new Chuck_Get_Global_String_Request;
    get_string_message->name = name;
    get_string_message->cb_with_id = callback;
    get_string_message->id = id;
    get_string_message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_string_request;
    r.getStringRequest = get_string_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalString()
// desc: set a global string by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalString( const char * name,
                                                 const char * val )
{
    Chuck_Set_Global_String_Request * set_string_message =
        new Chuck_Set_Global_String_Request;
    set_string_message->name = name;
    set_string_message->val = val;
    
    Chuck_Global_Request r;
    r.type = set_global_string_request;
    r.setStringRequest = set_string_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_string()
// desc: tell the vm that a global string is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_string( const std::string & name )
{
    if( m_global_strings.count( name ) == 0 )
    {
        // make container
        m_global_strings[name] = new Chuck_Global_String_Container;
        // init
        m_global_strings[name]->val = (Chuck_String *)
        instantiate_and_initialize_object( m_vm->env()->t_string, m_vm );
        
        // add reference to prevent deletion
        m_global_strings[name]->val->add_ref();
        
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_global_string_value()
// desc: get a value directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_String * Chuck_Globals_Manager::get_global_string( const std::string & name )
{
    // ensure exists
    init_global_string( name );
    
    return m_global_strings[name]->val;
}




//-----------------------------------------------------------------------------
// name: set_global_string_value()
// desc: set a value directly to the vm (internal)
//-----------------------------------------------------------------------------
Chuck_String * * Chuck_Globals_Manager::get_ptr_to_global_string( const std::string & name )
{
    return &( m_global_strings[name]->val );
}




//-----------------------------------------------------------------------------
// name: signalGlobalEvent()
// desc: signal() an Event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::signalGlobalEvent( const char * name )
{
    Chuck_Signal_Global_Event_Request * signal_event_message =
        new Chuck_Signal_Global_Event_Request;
    signal_event_message->name = name;
    signal_event_message->is_broadcast = FALSE;
    
    Chuck_Global_Request r;
    r.type = signal_global_event_request;
    r.signalEventRequest = signal_event_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: broadcastGlobalEvent()
// desc: broadcast() an Event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::broadcastGlobalEvent( const char * name )
{
    Chuck_Signal_Global_Event_Request * signal_event_message =
        new Chuck_Signal_Global_Event_Request;
    signal_event_message->name = name;
    signal_event_message->is_broadcast = TRUE;
    
    Chuck_Global_Request r;
    r.type = signal_global_event_request;
    r.signalEventRequest = signal_event_message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: listenForGlobalEvent()
// desc: listen for an Event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::listenForGlobalEvent( const char * name,
                                                      void (* callback)(void),
                                                      t_CKBOOL listen_forever )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb = callback;
    listen_event_message->cb_type = ck_get_plain;
    listen_event_message->name = name;
    listen_event_message->listen_forever = listen_forever;
    listen_event_message->deregister = FALSE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: listenForGlobalEvent()
// desc: listen for an Event by name, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::listenForGlobalEvent( const char * name,
                                                      void (* callback)(const char *),
                                                      t_CKBOOL listen_forever )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb_with_name = callback;
    listen_event_message->cb_type = ck_get_name;
    listen_event_message->name = name;
    listen_event_message->listen_forever = listen_forever;
    listen_event_message->deregister = FALSE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: listenForGlobalEvent()
// desc: listen for an Event by name, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::listenForGlobalEvent( const char * name,
                                                      t_CKINT id,
                                                      void (* callback)(t_CKINT),
                                                      t_CKBOOL listen_forever )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb_with_id = callback;
    listen_event_message->id = id;
    listen_event_message->cb_type = ck_get_id;
    listen_event_message->name = name;
    listen_event_message->listen_forever = listen_forever;
    listen_event_message->deregister = FALSE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stopListeningForGlobalEvent()
// desc: stop listening for an event by name
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::stopListeningForGlobalEvent( const char * name,
                                                             void (* callback)(void) )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb = callback;
    listen_event_message->cb_type = ck_get_plain;
    listen_event_message->name = name;
    listen_event_message->listen_forever = FALSE;
    listen_event_message->deregister = TRUE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stopListeningForGlobalEvent()
// desc: stop listening for an Event by name, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::stopListeningForGlobalEvent( const char * name,
                                                             void (* callback)(const char *) )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb_with_name = callback;
    listen_event_message->cb_type = ck_get_name;
    listen_event_message->name = name;
    listen_event_message->listen_forever = FALSE;
    listen_event_message->deregister = TRUE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: stopListeningForGlobalEvent()
// desc: stop listening for an Event by name, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::stopListeningForGlobalEvent( const char * name,
    t_CKINT id, void (* callback)(t_CKINT) )
{
    Chuck_Listen_For_Global_Event_Request * listen_event_message =
        new Chuck_Listen_For_Global_Event_Request;
    listen_event_message->cb_with_id = callback;
    listen_event_message->id = id;
    listen_event_message->cb_type = ck_get_id;
    listen_event_message->name = name;
    listen_event_message->listen_forever = FALSE;
    listen_event_message->deregister = TRUE;
    
    Chuck_Global_Request r;
    r.type = listen_for_global_event_request;
    r.listenForEventRequest = listen_event_message;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_event()
// desc: tell the vm that a global event is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_event( const std::string & name,
                                                   Chuck_Type * type )
{
    // if it hasn't been initted yet
    if( m_global_events.count( name ) == 0 )
    {
        // create a new storage container
        m_global_events[name] = new Chuck_Global_Event_Container;
        // create the chuck object
        m_global_events[name]->val =
        (Chuck_Event *) instantiate_and_initialize_object( type, m_vm );
        // add a reference to it so it won't be deleted until we're done
        // cleaning up the VM
        m_global_events[name]->val->add_ref();
        // store its type in the container, too (is it a user-defined class?)
        m_global_events[name]->type = type;
    }
    // already exists. check if there's a type mismatch.
    else if( type->name != m_global_events[name]->type->name )
    {
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_global_event()
// desc: get a pointer directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Event * Chuck_Globals_Manager::get_global_event( const std::string & name )
{
    return m_global_events[name]->val;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_event()
// desc: get a pointer pointer directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Event * * Chuck_Globals_Manager::get_ptr_to_global_event( const std::string & name )
{
    return &( m_global_events[name]->val );
}




//-----------------------------------------------------------------------------
// name: getGlobalUGenSamples()
// desc: get buffer samples
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalUGenSamples( const char * name,
                                                      SAMPLE * buffer, int numFrames )
{
    // if hasn't been init, or it has been init and hasn't been constructed,
    if( m_global_ugens.count( name ) == 0 ||
       should_call_global_ctor( name, te_globalUGen ) )
    {
        // fail without doing anything
        return FALSE;
    }
    
    // else, fill (if the ugen isn't buffered, then it will fill with zeroes)
    m_global_ugens[name]->val->get_buffer( buffer, numFrames );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_ugen()
// desc: tell the vm that a global ugen is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_ugen( const std::string & name,
                                                  Chuck_Type * type )
{
    
    // if it hasn't been initted yet
    if( m_global_ugens.count( name ) == 0 )
    {
        // create a new storage container
        m_global_ugens[name] = new Chuck_Global_UGen_Container;
        // create the chuck object
        m_global_ugens[name]->val =
        (Chuck_UGen *) instantiate_and_initialize_object( type, m_vm );
        // add a reference to it so it won't be deleted until we're done
        // cleaning up the VM
        m_global_ugens[name]->val->add_ref();
        // store its type in the container, too (is it a user-defined class?)
        m_global_ugens[name]->type = type;
    }
    // already exists. check if there's a type mismatch.
    else if( type->name != m_global_ugens[name]->type->name )
    {
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: is_global_ugen_init()
// desc: has a global ugen been initialized during emit?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::is_global_ugen_init( const std::string & name )
{
    return m_global_ugens.count( name ) > 0;
}




//-----------------------------------------------------------------------------
// name: is_global_ugen_valid()
// desc: has a global ugen been initialized during emit
//       and constructed during runtime?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::is_global_ugen_valid( const std::string & name )
{
    return is_global_ugen_init( name ) &&
    !should_call_global_ctor( name, te_globalUGen );
}




//-----------------------------------------------------------------------------
// name: get_global_ugen()
// desc: get directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_UGen * Chuck_Globals_Manager::get_global_ugen( const std::string & name )
{
    return m_global_ugens[name]->val;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_ugen()
// desc: get ptr directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_UGen * * Chuck_Globals_Manager::get_ptr_to_global_ugen( const std::string & name )
{
    return &( m_global_ugens[name]->val );
}




//-----------------------------------------------------------------------------
// name: setGlobalIntArray()
// desc: tell the vm to set an entire int array
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalIntArray( const char * name,
                                                   t_CKINT arrayValues[],
                                                   t_CKUINT numValues )
{
    Chuck_Set_Global_Int_Array_Request * message =
        new Chuck_Set_Global_Int_Array_Request;
    message->name = name;
    message->arrayValues.resize( numValues );
    for( int i = 0; i < numValues; i++ )
    {
        message->arrayValues[i] = arrayValues[i];
    }
    
    Chuck_Global_Request r;
    r.type = set_global_int_array_request;
    r.setIntArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArray()
// desc: tell the vm to get an entire int array
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArray( const char * name,
                                                   void (* callback)(t_CKINT[], t_CKUINT) )
{
    Chuck_Get_Global_Int_Array_Request * message =
        new Chuck_Get_Global_Int_Array_Request;
    message->name = name;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_request;
    r.getIntArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArray()
// desc: tell the vm to get an entire int array, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArray( const char * name,
    void (* callback)(const char *, t_CKINT[], t_CKUINT) )
{
    Chuck_Get_Global_Int_Array_Request * message =
        new Chuck_Get_Global_Int_Array_Request;
    message->name = name;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_request;
    r.getIntArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArray()
// desc: tell the vm to get an entire int array, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArray( const char * name,
    t_CKINT id, void (* callback)(t_CKINT, t_CKINT[], t_CKUINT) )
{
    Chuck_Get_Global_Int_Array_Request * message =
        new Chuck_Get_Global_Int_Array_Request;
    message->name = name;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_request;
    r.getIntArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalIntArrayValue()
// desc: tell the vm to set one value of an int array by index
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalIntArrayValue( const char * name,
                                                        t_CKUINT index, t_CKINT value )
{
    Chuck_Set_Global_Int_Array_Value_Request * message =
        new Chuck_Set_Global_Int_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->value = value;
    
    Chuck_Global_Request r;
    r.type = set_global_int_array_value_request;
    r.setIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArrayValue()
// desc: tell the vm to get one value of an int array by index
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArrayValue( const char * name,
    t_CKUINT index, void (* callback)(t_CKINT) )
{
    Chuck_Get_Global_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Int_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_value_request;
    r.getIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArrayValue()
// desc: tell the vm to get one value of an int array by index, with named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArrayValue( const char * name,
    t_CKUINT index, void (* callback)(const char *, t_CKINT) )
{
    Chuck_Get_Global_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Int_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_value_request;
    r.getIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalIntArrayValue()
// desc: tell the vm to get one value of an int array by index, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalIntArrayValue( const char * name,
    t_CKINT id, t_CKUINT index, void (* callback)(t_CKINT, t_CKINT) )
{
    Chuck_Get_Global_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Int_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_int_array_value_request;
    r.getIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalAssociativeIntArrayValue()
// desc: tell the vm to set one value of an associative array by string key
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalAssociativeIntArrayValue(
    const char * name, const char * key, t_CKINT value )
{
    Chuck_Set_Global_Associative_Int_Array_Value_Request * message =
        new Chuck_Set_Global_Associative_Int_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->value = value;
    
    Chuck_Global_Request r;
    r.type = set_global_associative_int_array_value_request;
    r.setAssociativeIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeIntArrayValue()
// desc: tell the vm to get one value of an associative array by string key
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeIntArrayValue(
    const char * name, const char * key, void (* callback)(t_CKINT) )
{
    Chuck_Get_Global_Associative_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Int_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_int_array_value_request;
    r.getAssociativeIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeIntArrayValue()
// desc: tell the vm to get one value of an associative array by string key, with named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeIntArrayValue( const char * name,
    const char * key, void (* callback)(const char *, t_CKINT) )
{
    Chuck_Get_Global_Associative_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Int_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_int_array_value_request;
    r.getAssociativeIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeIntArrayValue()
// desc: tell the vm to get one value of an associative array by string key, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeIntArrayValue( const char * name,
    t_CKINT id, const char * key, void (* callback)(t_CKINT, t_CKINT) )
{
    Chuck_Get_Global_Associative_Int_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Int_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_int_array_value_request;
    r.getAssociativeIntArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalFloatArray()
// desc: tell the vm to set an entire float array
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalFloatArray( const char * name,
    t_CKFLOAT arrayValues[], t_CKUINT numValues )
{
    Chuck_Set_Global_Float_Array_Request * message =
        new Chuck_Set_Global_Float_Array_Request;
    message->name = name;
    message->arrayValues.resize( numValues );
    for( int i = 0; i < numValues; i++ )
    {
        message->arrayValues[i] = arrayValues[i];
    }
    
    Chuck_Global_Request r;
    r.type = set_global_float_array_request;
    r.setFloatArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArray()
// desc: tell the vm to get an entire float array
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArray( const char * name,
    void (* callback)(t_CKFLOAT[], t_CKUINT))
{
    Chuck_Get_Global_Float_Array_Request * message =
        new Chuck_Get_Global_Float_Array_Request;
    message->name = name;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_request;
    r.getFloatArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArray()
// desc: tell the vm to get an entire float array, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArray( const char * name,
    void (* callback)(const char *, t_CKFLOAT[], t_CKUINT) )
{
    Chuck_Get_Global_Float_Array_Request * message =
        new Chuck_Get_Global_Float_Array_Request;
    message->name = name;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_request;
    r.getFloatArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArray()
// desc: tell the vm to get an entire float array, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArray( const char * name,
    t_CKINT id, void (* callback)(t_CKINT, t_CKFLOAT[], t_CKUINT))
{
    Chuck_Get_Global_Float_Array_Request * message =
        new Chuck_Get_Global_Float_Array_Request;
    message->name = name;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_request;
    r.getFloatArrayRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalFloatArrayValue()
// desc: tell the vm to set one value of an float array by index
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalFloatArrayValue( const char * name,
                                                          t_CKUINT index, t_CKFLOAT value )
{
    Chuck_Set_Global_Float_Array_Value_Request * message =
        new Chuck_Set_Global_Float_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->value = value;
    
    Chuck_Global_Request r;
    r.type = set_global_float_array_value_request;
    r.setFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArrayValue()
// desc: tell the vm to get one value of an float array by index
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArrayValue( const char * name,
    t_CKUINT index, void (* callback)(t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Float_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_value_request;
    r.getFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArrayValue()
// desc: tell the vm to get one value of an float array by index, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArrayValue( const char * name,
    t_CKUINT index, void (* callback)(const char *, t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Float_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_value_request;
    r.getFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalFloatArrayValue()
// desc: tell the vm to get one value of an float array by index, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalFloatArrayValue( const char * name,
    t_CKINT id, t_CKUINT index, void (* callback)(t_CKINT, t_CKFLOAT) )
{
    Chuck_Get_Global_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Float_Array_Value_Request;
    message->name = name;
    message->index = index;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_float_array_value_request;
    r.getFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: setGlobalAssociativeFloatArrayValue()
// desc: tell the vm to set one value of an associative array by string key
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::setGlobalAssociativeFloatArrayValue( const char * name,
    const char * key, t_CKFLOAT value )
{
    Chuck_Set_Global_Associative_Float_Array_Value_Request * message =
        new Chuck_Set_Global_Associative_Float_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->value = value;
    
    Chuck_Global_Request r;
    r.type = set_global_associative_float_array_value_request;
    r.setAssociativeFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeFloatArrayValue()
// desc: tell the vm to get one value of an associative array by string key
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeFloatArrayValue( const char * name,
    const char * key, void (* callback)(t_CKFLOAT) )
{
    Chuck_Get_Global_Associative_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Float_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb = callback;
    message->cb_type = ck_get_plain;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_float_array_value_request;
    r.getAssociativeFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeFloatArrayValue()
// desc: tell the vm to get one value of an associative array by string key, with a named callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeFloatArrayValue( const char * name,
    const char * key, void (* callback)(const char *, t_CKFLOAT) )
{
    Chuck_Get_Global_Associative_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Float_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb_with_name = callback;
    message->cb_type = ck_get_name;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_float_array_value_request;
    r.getAssociativeFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: getGlobalAssociativeFloatArrayValue()
// desc: tell the vm to get one value of an associative array by string key, with id in callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::getGlobalAssociativeFloatArrayValue(
    const char * name, t_CKINT id, const char * key,
    void (* callback)(t_CKINT, t_CKFLOAT) )
{
    Chuck_Get_Global_Associative_Float_Array_Value_Request * message =
        new Chuck_Get_Global_Associative_Float_Array_Value_Request;
    message->name = name;
    message->key = key;
    message->cb_with_id = callback;
    message->id = id;
    message->cb_type = ck_get_id;
    
    Chuck_Global_Request r;
    r.type = get_global_associative_float_array_value_request;
    r.getAssociativeFloatArrayValueRequest = message;
    // chuck object might not be constructed on time. retry only once
    r.retries = 1;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: execute_chuck_msg_with_globals()
// desc: execute a Chuck_Msg in the globals callback
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::execute_chuck_msg_with_globals( Chuck_Msg * msg )
{
    Chuck_Execute_Chuck_Msg_Request * message =
        new Chuck_Execute_Chuck_Msg_Request;
    message->msg = msg;
    
    Chuck_Global_Request r;
    r.type = execute_chuck_msg_request;
    r.executeChuckMsgRequest = message;
    r.retries = 0;
    
    m_global_request_queue.put( r );
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: init_global_array()
// desc: tell the vm that a global string is now available
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_array( const std::string & name, Chuck_Type * type, te_GlobalType arr_type )
{
    if( m_global_arrays.count( name ) == 0 )
    {
        // make container
        m_global_arrays[name] = new Chuck_Global_Array_Container( arr_type );
        // do not init
        m_global_arrays[name]->array = NULL;
        // global variable type
        m_global_arrays[name]->array_type = arr_type;
        // note: Chuck_Type * is currently unused, but may be necessary later
        // TODO: how to init if user sets it before any script makes it?
        // TODO: how to keep reference to prevent from being deleted if
        //  a script ends and takes the array with it?
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: get_global_array()
// desc: get value directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Object * Chuck_Globals_Manager::get_global_array( const std::string & name )
{
    return m_global_arrays[name]->array;
}




//-----------------------------------------------------------------------------
// name: _get_global_int_array_value()
// desc: get value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Globals_Manager::get_global_int_array_value( const std::string & name,
                                                           t_CKUINT index )
{
    t_CKUINT result = 0;
    Chuck_Object * array = m_global_arrays[name]->array;
    if( array != NULL &&
       m_global_arrays[name]->array_type == te_globalInt )
    {
        Chuck_Array4 * intArray = (Chuck_Array4 *) array;
        // TODO why is it unsigned int storage?
        intArray->get( index, &result );
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: _get_global_associative_int_array_value()
// desc: get value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKINT Chuck_Globals_Manager::get_global_associative_int_array_value(
    const std::string & name, const std::string & key )
{
    t_CKUINT result = 0;
    Chuck_Object * array = m_global_arrays[name]->array;
    if( array != NULL &&
       m_global_arrays[name]->array_type == te_globalInt )
    {
        Chuck_Array4 * intArray = (Chuck_Array4 *) array;
        // TODO why is it unsigned int storage?
        intArray->get( key, &result );
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: _get_global_float_array_value()
// desc: get value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_Globals_Manager::get_global_float_array_value( const std::string & name,
                                                               t_CKUINT index )
{
    t_CKFLOAT result = 0;
    Chuck_Object * array = m_global_arrays[name]->array;
    if( array != NULL &&
       m_global_arrays[name]->array_type == te_globalFloat )
    {
        Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
        floatArray->get( index, &result );
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: _get_global_associative_float_array_value()
// desc: get value directly from the vm (internal)
//-----------------------------------------------------------------------------
t_CKFLOAT Chuck_Globals_Manager::get_global_associative_float_array_value(
    const std::string & name, const std::string & key )
{
    t_CKFLOAT result = 0;
    Chuck_Object * array = m_global_arrays[name]->array;
    if( array != NULL &&
       m_global_arrays[name]->array_type == te_globalFloat )
    {
        Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
        floatArray->get( key, &result );
    }
    return result;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_array()
// desc: get ptr directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Object * * Chuck_Globals_Manager::get_ptr_to_global_array( const std::string & name )
{
    return &( m_global_arrays[name]->array );
}




//-----------------------------------------------------------------------------
// name: init_global_object()
// desc: init a global object
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::init_global_object( const std::string & name,
                                                    Chuck_Type * type )
{
    // if it hasn't been initted yet
    if( m_global_objects.count( name ) == 0 ) {
        // create a new storage container
        m_global_objects[name] = new Chuck_Global_Object_Container;
        // create the chuck object
        m_global_objects[name]->val =
        (Chuck_Object *) instantiate_and_initialize_object( type, m_vm );
        // add a reference to it so it won't be deleted until we're done
        // cleaning up the VM
        m_global_objects[name]->val->add_ref();
        // store its type in the container, too (is it a user-defined class?)
        m_global_objects[name]->type = type;
    }
    // already exists. check if there's a type mismatch.
    else if( type->name != m_global_objects[name]->type->name )
    {
        return FALSE;
    }
    
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: is_global_object_init()
// desc: has a global object been initialized during emit?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::is_global_object_init( const std::string & name )
{
    return m_global_objects.count( name ) > 0;
}




//-----------------------------------------------------------------------------
// name: is_global_object_valid()
// desc: has a global object been initialized during emit
//       and constructed during runtime?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::is_global_object_valid( const std::string & name )
{
    return is_global_object_init( name ) &&
    !should_call_global_ctor( name, te_globalObject );
}




//-----------------------------------------------------------------------------
// name: get_global_object()
// desc: get directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Object * Chuck_Globals_Manager::get_global_object( const std::string & name )
{
    return m_global_objects[name]->val;
}




//-----------------------------------------------------------------------------
// name: get_ptr_to_global_object()
// desc: get ptr directly from the vm (internal)
//-----------------------------------------------------------------------------
Chuck_Object ** Chuck_Globals_Manager::get_ptr_to_global_object( const std::string & name )
{
    return &( m_global_objects[name]->val );
}




//-----------------------------------------------------------------------------
// name: should_call_global_ctor()
// desc: ask the vm if a global needs to be constructed after init
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Globals_Manager::should_call_global_ctor( const std::string & name,
                                                         te_GlobalType type )
{
    switch( type )
    {
        case te_globalInt:
        case te_globalFloat:
        case te_globalString:
            // these basic types don't have ctors
            return FALSE;
            break;
        case te_globalEvent:
            return m_global_events.count( name ) > 0 &&
            m_global_events[name]->ctor_needs_to_be_called;
            break;
        case te_globalUGen:
            return m_global_ugens.count( name ) > 0 &&
            m_global_ugens[name]->ctor_needs_to_be_called;
            break;
        case te_globalArraySymbol:
            // this case is only used for array-as-symbol, not array-as-decl.
            // if arrays need their ctors called, we need a rearchitecture!
            return FALSE;
            break;
        case te_globalObject:
            return m_global_objects.count(name) > 0 &&
            m_global_objects[name]->ctor_needs_to_be_called;
            break;
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: global_ctor_was_called()
// desc: tell the vm that a global has been constructed after init
//-----------------------------------------------------------------------------
void Chuck_Globals_Manager::global_ctor_was_called( const std::string & name,
                                                    te_GlobalType type )
{
    switch( type )
    {
        case te_globalInt:
        case te_globalFloat:
        case te_globalString:
            // do nothing for these basic types
            break;
        case te_globalEvent:
            if( m_global_events.count( name ) > 0 )
            {
                m_global_events[name]->ctor_needs_to_be_called = FALSE;
            }
            break;
        case te_globalUGen:
            if( m_global_ugens.count( name ) > 0 )
            {
                m_global_ugens[name]->ctor_needs_to_be_called = FALSE;
            }
            break;
        case te_globalArraySymbol:
            // do nothing
            break;
        case te_globalObject:
            if( m_global_objects.count( name ) > 0 )
            {
                m_global_objects[name]->ctor_needs_to_be_called = FALSE;
            }
            break;
    }
}




//-----------------------------------------------------------------------------
// name: cleanup_global_variables()
// desc: set a pointer directly on the vm (internal)
//-----------------------------------------------------------------------------
void Chuck_Globals_Manager::cleanup_global_variables()
{
    // ints: delete containers and clear map
    for( std::map< std::string, Chuck_Global_Int_Container * >::iterator it=
        m_global_ints.begin(); it!=m_global_ints.end(); it++ )
    {
        delete (it->second);
    }
    m_global_ints.clear();
    
    // floats: delete containers and clear map
    for( std::map< std::string, Chuck_Global_Float_Container * >::iterator it=
        m_global_floats.begin(); it!=m_global_floats.end(); it++ )
    {
        delete (it->second);
    }
    m_global_floats.clear();
    
    // strings: release strings, delete containers, and clear map
    for( std::map< std::string, Chuck_Global_String_Container * >::iterator it=
        m_global_strings.begin(); it!=m_global_strings.end(); it++ )
    {
        SAFE_RELEASE( it->second->val );
        delete (it->second);
    }
    m_global_strings.clear();
    
    // events: release events, delete containers, and clear map
    for( std::map< std::string, Chuck_Global_Event_Container * >::iterator it=
        m_global_events.begin(); it!=m_global_events.end(); it++ )
    {
        SAFE_RELEASE( it->second->val );
        delete (it->second);
    }
    m_global_events.clear();
    
    // ugens: release ugens, delete containers, and clear map
    for( std::map< std::string, Chuck_Global_UGen_Container * >::iterator it=
        m_global_ugens.begin(); it!=m_global_ugens.end(); it++ )
    {
        SAFE_RELEASE( it->second->val );
        delete (it->second);
    }
    m_global_ugens.clear();
    
    // arrays: release arrays, delete containers, and clear map
    for( std::map< std::string, Chuck_Global_Array_Container * >::iterator it=
        m_global_arrays.begin(); it!=m_global_arrays.end(); it++ )
    {
        // release. array initialization adds a reference,
        // but the release instruction is prevented from being
        // used on all global objects (including arrays)
        SAFE_RELEASE( it->second->array );
        delete (it->second);
    }
    m_global_arrays.clear();
    
    // ugens: release objects, delete containers, and clear map
    for( std::map< std::string, Chuck_Global_Object_Container * >::iterator it=
        m_global_objects.begin(); it!=m_global_objects.end(); it++ )
    {
        SAFE_RELEASE( it->second->val );
        delete (it->second);
    }
    m_global_objects.clear();
}




//-----------------------------------------------------------------------------
// name: handle_global_queue_messages()
// desc: update vm with set, get, listen, spork, etc. messages
//-----------------------------------------------------------------------------
void Chuck_Globals_Manager::handle_global_queue_messages()
{
    bool should_retry_this_request = false;
    
    while( m_global_request_queue.more() )
    {
        should_retry_this_request = false;
        
        Chuck_Global_Request message;
        if( m_global_request_queue.get( & message ) )
        {
            switch( message.type )
            {
                case execute_chuck_msg_request:
                    if( message.executeChuckMsgRequest->msg != NULL )
                    {
                        // this will automatically delete msg if applicable
                        m_vm->process_msg( message.executeChuckMsgRequest->msg );
                    }
                    // clean up request storage
                    SAFE_DELETE( message.executeChuckMsgRequest );
                    break;
                    
                case spork_shred_request:
                    // spork the shred!
                    m_vm->spork( message.shred );
                    break;
                    
                case set_global_int_request:
                    // ensure the container exists
                    init_global_int( message.setIntRequest->name );
                    // set int
                    m_global_ints[message.setIntRequest->name]->val = message.setIntRequest->val;
                    // clean up request storage
                    SAFE_DELETE( message.setIntRequest );
                    break;
                    
                case get_global_int_request:
                    // ensure one of the callbacks is not null (union)
                    if( message.getIntRequest->cb != NULL )
                    {
                        // ensure the value exists
                        init_global_int( message.getIntRequest->name );
                        // call the callback with the value
                        switch( message.getIntRequest->cb_type )
                        {
                            case ck_get_plain:
                                message.getIntRequest->cb( m_global_ints[message.getIntRequest->name]->val );
                                break;
                            case ck_get_name:
                                message.getIntRequest->cb_with_name( message.getIntRequest->name.c_str(), m_global_ints[message.getIntRequest->name]->val );
                                break;
                            case ck_get_id:
                                message.getIntRequest->cb_with_id( message.getIntRequest->id, m_global_ints[message.getIntRequest->name]->val );
                                break;
                        }
                    }
                    // clean up request storage
                    SAFE_DELETE( message.getIntRequest );
                    break;
                    
                case set_global_float_request:
                    // ensure the container exists
                    init_global_float( message.setFloatRequest->name );
                    // set float
                    m_global_floats[message.setFloatRequest->name]->val = message.setFloatRequest->val;
                    // clean up request storage
                    SAFE_DELETE( message.setFloatRequest );
                    break;
                    
                case get_global_float_request:
                    // ensure one cb is not null (union)
                    if( message.getFloatRequest->cb != NULL )
                    {
                        // ensure value exists
                        init_global_float( message.getFloatRequest->name );
                        // call callback with float
                        switch( message.getFloatRequest->cb_type )
                        {
                            case ck_get_plain:
                                message.getFloatRequest->cb( m_global_floats[message.getFloatRequest->name]->val );
                                break;
                            case ck_get_name:
                                message.getFloatRequest->cb_with_name( message.getFloatRequest->name.c_str(), m_global_floats[message.getFloatRequest->name]->val );
                                break;
                            case ck_get_id:
                                message.getFloatRequest->cb_with_id( message.getFloatRequest->id, m_global_floats[message.getFloatRequest->name]->val );
                                break;
                        }
                    }
                    // clean up request storage
                    SAFE_DELETE( message.getFloatRequest );
                    break;
                    
                case set_global_string_request:
                    // ensure the container exists
                    init_global_string( message.setStringRequest->name );
                    // set string
                    m_global_strings[message.setStringRequest->name]->val->set( message.setStringRequest->val );
                    // clean up request storage
                    SAFE_DELETE( message.setStringRequest );
                    break;
                    
                case get_global_string_request:
                    // ensure one callback is not null (union)
                    if( message.getStringRequest->cb != NULL )
                    {
                        // ensure value exists
                        init_global_string( message.getStringRequest->name );
                        // call callback with string
                        switch( message.getStringRequest->cb_type )
                        {
                            case ck_get_plain:
                                message.getStringRequest->cb( m_global_strings[message.getStringRequest->name]->val->c_str() );
                                break;
                            case ck_get_name:
                                message.getStringRequest->cb_with_name( message.getStringRequest->name.c_str(), m_global_strings[message.getStringRequest->name]->val->c_str() );
                                break;
                            case ck_get_id:
                                message.getStringRequest->cb_with_id( message.getStringRequest->id, m_global_strings[message.getStringRequest->name]->val->c_str() );
                                break;
                        }
                    }
                    // clean up request storage
                    SAFE_DELETE( message.getStringRequest );
                    break;
                    
                case signal_global_event_request:
                    // ensure it exists and it doesn't need its ctor called
                    if( m_global_events.count( message.signalEventRequest->name ) > 0 &&
                       !should_call_global_ctor( message.signalEventRequest->name, te_globalEvent ) )
                    {
                        // get the event
                        Chuck_Event * event = get_global_event( message.signalEventRequest->name );
                        // signal it, or broadcast it
                        if( message.signalEventRequest->is_broadcast )
                        {
                            event->broadcast_local();
                            event->broadcast_global();
                        }
                        else
                        {
                            event->signal_local();
                            event->signal_global();
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.signalEventRequest );
                    }
                    break;
                    
                case listen_for_global_event_request:
                    // ensure a callback is not null (union)
                    if( message.listenForEventRequest->cb != NULL )
                    {
                        // ensure it exists
                        if( m_global_events.count( message.listenForEventRequest->name ) > 0 )
                        {
                            // get event
                            Chuck_Event * event = get_global_event( message.listenForEventRequest->name );
                            if( message.listenForEventRequest->deregister )
                            {
                                // deregister
                                switch( message.listenForEventRequest->cb_type )
                                {
                                    case ck_get_plain:
                                        event->remove_listen( message.listenForEventRequest->cb );
                                        break;
                                    case ck_get_name:
                                        event->remove_listen( message.listenForEventRequest->name,
                                                             message.listenForEventRequest->cb_with_name );
                                        break;
                                    case ck_get_id:
                                        event->remove_listen( message.listenForEventRequest->id,
                                                             message.listenForEventRequest->cb_with_id );
                                        break;
                                }
                                
                            }
                            else
                            {
                                // register
                                switch( message.listenForEventRequest->cb_type )
                                {
                                    case ck_get_plain:
                                        event->global_listen( message.listenForEventRequest->cb,
                                                             message.listenForEventRequest->listen_forever );
                                        break;
                                    case ck_get_name:
                                        event->global_listen( message.listenForEventRequest->name,
                                                             message.listenForEventRequest->cb_with_name,
                                                             message.listenForEventRequest->listen_forever );
                                        break;
                                    case ck_get_id:
                                        event->global_listen( message.listenForEventRequest->id,
                                                             message.listenForEventRequest->cb_with_id,
                                                             message.listenForEventRequest->listen_forever );
                                        break;
                                }
                                
                            }
                        }
                        else
                        {
                            // name does not exist. possible that object just doesn't exist yet.
                            if( message.retries > 0 )
                            {
                                should_retry_this_request = true;
                            }
                        }
                    }
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.listenForEventRequest );
                    }
                    break;
                    
                case set_global_int_array_request:
                {
                    // replace an entire array, if it exists
                    Chuck_Set_Global_Int_Array_Request * request =
                    message.setIntArrayRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it an int array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt )
                        {
                            // it exists! get existing array, new size
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            t_CKUINT newSize = request->arrayValues.size();
                            
                            // resize and copy in
                            intArray->set_size( newSize );
                            for( int i = 0; i < newSize; i++ )
                            {
                                intArray->set( i, request->arrayValues[i] );
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setIntArrayRequest );
                    }
                    break;
                    
                case get_global_int_array_request:
                {
                    // fetch an entire array, if it exists
                    Chuck_Get_Global_Int_Array_Request * request =
                    message.getIntArrayRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt &&
                           request->cb != NULL )
                        {
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            // TODO: why is m_vector a vector of unsigned ints...??
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb(
                                                (t_CKINT *) &(intArray->m_vector[0]),
                                                intArray->size()
                                                );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name(
                                                          request->name.c_str(),
                                                          (t_CKINT *) &(intArray->m_vector[0]),
                                                          intArray->size()
                                                          );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id(
                                                        request->id,
                                                        (t_CKINT *) &(intArray->m_vector[0]),
                                                        intArray->size()
                                                        );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getIntArrayRequest );
                    }
                    break;
                    
                case set_global_int_array_value_request:
                {
                    // set a single value, if array exists and index in range
                    Chuck_Set_Global_Int_Array_Value_Request * request =
                    message.setIntArrayValueRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it an int array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt )
                        {
                            // it exists! get existing array
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            // set! (if index within range)
                            if( intArray->size() > request->index )
                            {
                                intArray->set( request->index, request->value );
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setIntArrayValueRequest );
                    }
                    break;
                    
                case get_global_int_array_value_request:
                {
                    // get a single value, if array exists and index in range
                    Chuck_Get_Global_Int_Array_Value_Request * request =
                    message.getIntArrayValueRequest;
                    // fetch an entire array, if it exists
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback? (union)
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt &&
                           request->cb != NULL )
                        {
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            // TODO why is it unsigned int storage?
                            t_CKUINT result = 0;
                            intArray->get( request->index, &result );
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb( result );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name( request->name.c_str(), result );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id( request->id, result );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getIntArrayValueRequest );
                    }
                    break;
                    
                case set_global_associative_int_array_value_request:
                {
                    // set a single value by key, if array exists
                    Chuck_Set_Global_Associative_Int_Array_Value_Request * request =
                    message.setAssociativeIntArrayValueRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it an int array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt )
                        {
                            // it exists! get existing array
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            // set!
                            intArray->set( request->key, request->value );
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setAssociativeIntArrayValueRequest );
                    }
                    break;
                    
                case get_global_associative_int_array_value_request:
                {
                    // get a single value by key, if array exists and key in map
                    Chuck_Get_Global_Associative_Int_Array_Value_Request * request =
                    message.getAssociativeIntArrayValueRequest;
                    // fetch an entire array, if it exists
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback? (union)
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalInt &&
                           request->cb != NULL )
                        {
                            Chuck_Array4 * intArray = (Chuck_Array4 *) array;
                            // TODO why is it unsigned int storage?
                            t_CKUINT result = 0;
                            intArray->get( request->key, &result );
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb( result );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name( request->name.c_str(), result );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id( request->id, result );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getAssociativeIntArrayValueRequest );
                    }
                    break;
                    
                case set_global_float_array_request:
                {
                    // replace an entire array, if it exists
                    Chuck_Set_Global_Float_Array_Request * request =
                    message.setFloatArrayRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it a float array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat )
                        {
                            // it exists! get existing array, new size
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            t_CKUINT newSize = request->arrayValues.size();
                            
                            // resize and copy in
                            floatArray->set_size( newSize );
                            for( int i = 0; i < newSize; i++ )
                            {
                                floatArray->set( i, request->arrayValues[i] );
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setFloatArrayRequest );
                    }
                    break;
                    
                case get_global_float_array_request:
                {
                    // fetch an entire array, if it exists
                    Chuck_Get_Global_Float_Array_Request * request =
                    message.getFloatArrayRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback? (union)
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat &&
                           request->cb != NULL )
                        {
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb(
                                                &(floatArray->m_vector[0]),
                                                floatArray->size()
                                                );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name(
                                                          request->name.c_str(),
                                                          &(floatArray->m_vector[0]),
                                                          floatArray->size()
                                                          );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id(
                                                        request->id,
                                                        &(floatArray->m_vector[0]),
                                                        floatArray->size()
                                                        );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getFloatArrayRequest );
                    }
                    break;
                    
                case set_global_float_array_value_request:
                {
                    // set a single value, if array exists and index in range
                    Chuck_Set_Global_Float_Array_Value_Request * request =
                    message.setFloatArrayValueRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it a float array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat )
                        {
                            // it exists! get existing array
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            // set! (if index within range)
                            if( floatArray->size() > request->index )
                            {
                                floatArray->set( request->index, request->value );
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setFloatArrayValueRequest );
                    }
                    break;
                    
                case get_global_float_array_value_request:
                {
                    // get a single value, if array exists and index in range
                    Chuck_Get_Global_Float_Array_Value_Request * request =
                    message.getFloatArrayValueRequest;
                    // fetch an entire array, if it exists
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback? (union)
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat &&
                           request->cb != NULL )
                        {
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            t_CKFLOAT result = 0;
                            floatArray->get( request->index, &result );
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb( result );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name( request->name.c_str(), result );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id( request->id, result );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getFloatArrayValueRequest );
                    }
                    break;
                    
                case set_global_associative_float_array_value_request:
                {
                    // set a single value by key, if array exists
                    Chuck_Set_Global_Associative_Float_Array_Value_Request * request =
                    message.setAssociativeFloatArrayValueRequest;
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet? is it a float array?
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat )
                        {
                            // it exists! get existing array
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            // set!
                            floatArray->set( request->key, request->value );
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.setAssociativeFloatArrayValueRequest );
                    }
                    break;
                    
                case get_global_associative_float_array_value_request:
                {
                    // get a single value by key, if array exists and key in map
                    Chuck_Get_Global_Associative_Float_Array_Value_Request * request =
                    message.getAssociativeFloatArrayValueRequest;
                    // fetch an entire array, if it exists
                    if( m_global_arrays.count( request->name ) > 0 )
                    {
                        // we know that array's name. does it exist yet; do we have a callback? (union)
                        Chuck_Object * array = m_global_arrays[request->name]->array;
                        if( array != NULL &&
                           m_global_arrays[request->name]->array_type == te_globalFloat &&
                           request->cb != NULL )
                        {
                            Chuck_Array8 * floatArray = (Chuck_Array8 *) array;
                            t_CKFLOAT result = 0;
                            floatArray->get( request->key, &result );
                            switch( request->cb_type )
                            {
                                case ck_get_plain:
                                    request->cb( result );
                                    break;
                                case ck_get_name:
                                    request->cb_with_name( request->name.c_str(), result );
                                    break;
                                case ck_get_id:
                                    request->cb_with_id( request->id, result );
                                    break;
                            }
                        }
                    }
                    else
                    {
                        // name does not exist. possible that object just doesn't exist yet.
                        if( message.retries > 0 )
                        {
                            should_retry_this_request = true;
                        }
                    }
                }
                    
                    // clean up request storage
                    if( should_retry_this_request )
                    {
                        m_global_request_retry_queue.put( message );
                    }
                    else
                    {
                        SAFE_DELETE( message.getAssociativeFloatArrayValueRequest );
                    }
                    break;
            }
        }
        else
        {
            // get failed. problem?
            break;
        }
    }
    
    // process retries
    while( m_global_request_retry_queue.more() )
    {
        // get the request
        Chuck_Global_Request message;
        if( m_global_request_retry_queue.get( &message ) )
        {
            // make sure it should be retried
            if( message.retries == 0 )
            {
                // trying to retry something that shouldn't be.
                continue;
            }
            
            // decrement the number of retries
            message.retries--;
            // add it back to the queue for next time
            m_global_request_queue.put( message );
        }
        else
        {
            // get failed. problem?
            break;
        }
    }
}
