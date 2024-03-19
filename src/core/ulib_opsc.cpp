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
// file: ulib_opsc.cpp
// desc: class library for open sound control
//
// author: Philip L. Davidson (philipd@alumni.princeton.edu)
//         Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2005
//-----------------------------------------------------------------------------

// lo.h ----> lo_osc_types.h needs to import <cstdint> before running itself,
// but ONLY when compiling this file, and NOT when compiling liblo .c files
// so, set a flag | REFACTOR-2017
#include "chuck_def.h" // to pick up platform macros
#ifdef __PLATFORM_WINDOWS__
#define ULIB_OPSC_CPP
#endif

#ifdef __UNITY_FLAT__
#include "lo.h"
#else
#include "lo/lo.h"
#endif

// on windows, the above must remain above these includes, else winsock errors
#include "ulib_opsc.h"
#include "chuck_compile.h"
#include "chuck_instr.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "util_buffers.h"
#include "util_opsc.h"
#include "util_string.h"
#include "util_thread.h"

#include <algorithm> // 1.5.1.3 | for find

#if _MSC_VER
#define snprintf _snprintf
#endif


// Osc DL implementation
CK_DLL_CTOR( osc_send_ctor );
CK_DLL_DTOR( osc_send_dtor );
CK_DLL_MFUN( osc_send_setHost );
CK_DLL_MFUN( osc_send_startMesg );
CK_DLL_MFUN( osc_send_startMesg_spec );
CK_DLL_MFUN( osc_send_addInt );
CK_DLL_MFUN( osc_send_addFloat );
CK_DLL_MFUN( osc_send_addString );
CK_DLL_MFUN( osc_send_openBundle );
CK_DLL_MFUN( osc_send_closeBundle );
CK_DLL_MFUN( osc_send_holdMesg );
CK_DLL_MFUN( osc_send_kickMesg );

CK_DLL_CTOR( osc_address_ctor );
CK_DLL_DTOR( osc_address_dtor );
CK_DLL_MFUN( osc_address_set );
CK_DLL_MFUN( osc_address_can_wait );
CK_DLL_MFUN( osc_address_has_mesg );
CK_DLL_MFUN( osc_address_next_mesg );
CK_DLL_MFUN( osc_address_next_int );
CK_DLL_MFUN( osc_address_next_float );
CK_DLL_MFUN( osc_address_next_string );

CK_DLL_CTOR( osc_recv_ctor );
CK_DLL_DTOR( osc_recv_dtor );
CK_DLL_MFUN( osc_recv_port );
CK_DLL_MFUN( osc_recv_new_address );
CK_DLL_MFUN( osc_recv_new_address_type );
CK_DLL_MFUN( osc_recv_add_address );
CK_DLL_MFUN( osc_recv_remove_address );
CK_DLL_MFUN( osc_recv_listen );
CK_DLL_MFUN( osc_recv_listen_port );
CK_DLL_MFUN( osc_recv_listen_stop );


// typetag characters
const std::string CK_OSC_TYPETAG_CHARS = "ifs";




//-----------------------------------------------------------------------------
// name: struct OSC message
// desc: internal OSC message struct
//-----------------------------------------------------------------------------
struct OscMsg
{
    std::string path;
    std::string type;

    // an argument, of type int, float, or string
    struct OscArg
    {
        OscArg() : i( 0 ), f( 0 ), s() { }
        OscArg( int _i ) : i( _i ), f( 0 ), s() { }
        OscArg( float _f ) : i( 0 ), f( _f ), s() { }
        OscArg( const std::string & _s ) : i( 0 ), f( 0 ), s( _s ) { }

        int i;
        float f;
        std::string s;
    };

    // vector of args
    std::vector<OscArg> args;
};




//-----------------------------------------------------------------------------
// TODO: use this to cache ck objects to not leak memory so rampantly
//-----------------------------------------------------------------------------
struct CkOscMsg
{
    OscMsg msg;

    Chuck_String * path;
    Chuck_String * type;
    std::vector<Chuck_Object> args;
};




// forward reference
class OscInServer;





//-----------------------------------------------------------------------------
// name: class OscIn
// desc: data structure for OscIn implementation
//-----------------------------------------------------------------------------
class OscIn : public Chuck_VM_Object
{
public:
    // constructor
    OscIn( Chuck_Event * event, Chuck_VM * vm ) :
        m_event( event ),
        m_vm( vm ),
        m_port( -1 ),
        m_oscMsgBuffer( CircularBuffer<OscMsg>( 1024 ) )
    {
        // create event buffer from VM
        m_eventBuffer = m_vm->create_event_buffer();
        // add references | 1.5.1.3
        CK_SAFE_ADD_REF( m_vm );
        CK_SAFE_ADD_REF( m_event );
    }

    // destructor | 1.5.1.3 (ge) reworked in tracking down this years-old issue
    //-------------------------------------------------------------------------
    // https://github.com/ccrma/chuck/issues/93 (filed 2017; resolved 2013)
    // FYI issue was caused by two related problems, especially when multiple
    //     OscIn and shreds are listening to the same address:
    // 1) audio thread deletes OscIn; server thread may still call it afterwards
    // 2) previously ADD_METHOD, REMOVE_METHOD, REMOVEALL_METHODS do not
    //    correctly handle multiple client OscIn listening for same OSC method
    // ---
    // CLEANUP address 1) race condition between audio thread & server thread
    // -> (chuck) OscIn object deletion
    // -> (c++; VM thread) trigger this shutdown_begin()
    //        - which sends a message to unregister all methods from server
    //        - OscIn, an internal data structure no visibile in from chuck
    //          now inherits Chuck_VM_Object to use its reference counting
    //        - also sends a message to release ref count on this client
    //        - OscIn release ref count on this OscIn (from oscin_dtor())
    // -> (c++; server thread) eventually processes the unregistering of methods
    //        - race condition: this thread may be within OscIn's handler with stale data
    //          (previously caused a crash before we moved to reference-counting OscIn)
    //        - now the there is a mechanism to release ref count on this OscIn
    //          (this OscIn should be deleted only when both threads are done with it)
    // ---
    // (SERVER DISPATCH REWORK addresses part 2), throughout OscInServer above)
    //-------------------------------------------------------------------------
    ~OscIn()
    {
        // delete event buffer (see shutdown_begin() for more explanation)
        m_vm->destroy_event_buffer( m_eventBuffer );
        // zero the pointer
        m_eventBuffer = NULL;

        // release VM and even treference
        CK_SAFE_RELEASE( m_vm );
        CK_SAFE_RELEASE( m_event );
    }

    void shutdown_begin()
    {
        // detach event buffer from VM without deleting yet
        // NOTE event buffer may still be in play on server thread and used in OscIn::handler()
        m_vm->detach_event_buffer_without_delete( m_eventBuffer );
        // unregister OSC methods
        removeAllMethods();
        // queue up message to release this client
        releaseThisClient();
    }

public:
    // create server message: add OSC method associated with this OscIn client
    void addMethod( const std::string & method );
    // create server message: remove OSC method associated with this OscIn client
    void removeMethod( const std::string & method );
    // create server message: remove all OSC method associated with this OscIn client
    void removeAllMethods();
    // create server message: release ref count this OscIn client
    void releaseThisClient();

    // get next message from buffer; return false if currently empty
    t_CKBOOL get( OscMsg & msg )
    {
        return m_oscMsgBuffer.get( msg );
    }

    // set port; get port; also primes the port's server
    int port( int port );

    // get port (FYI implemented below since it needs to access OscInServer)
    int port();

    // handle get port notification from server (for auto-assigned ports)
    void portNotification( int port );

public:
    // called by OscInServer to process a message
    // handles an incoming OSC message; prep data and queue up event broadcasting
    int handler( const char * path, const char * types, lo_arg ** argv, int argc, lo_message _msg )
    {
        OscMsg msg;
        msg.path = path;
        msg.type = types;

        for( int i = 0; i < argc; i++ )
        {
            OscMsg::OscArg arg;

            switch( types[i] )
            {
            case 'i': arg.i = argv[i]->i; break;
            case 'f': arg.f = argv[i]->f; break;
            case 's': arg.s = &(argv[i]->s); break;
            default:
                EM_error3( "OscIn: error: unhandled OSC type '%c'", types[i] );
            }

            msg.args.push_back( arg );
        }

        // put into msg buffer
        m_oscMsgBuffer.put( msg );
        // queue event broadcast; FYI this handler will most likely be called from server thread
        // instead of audio thread, so queue_event() instead of directly manipulating event
        m_vm->queue_event( m_event, 1, m_eventBuffer );

        // why -1? something liblo expects?
        return -1;
    }

private:
    // VM reference
    Chuck_VM * m_vm;
    // event reference
    Chuck_Event * m_event;
    // port number
    int m_port;
    // event buffer
    CBufferSimple * m_eventBuffer;
    // OSC message buffer (stores incoming messages, until chuck calls recv to drain buffer)
    CircularBuffer<OscMsg> m_oscMsgBuffer;
};




//-----------------------------------------------------------------------------
// name: struct OscInServer
// desc: OSC incoming server
//-----------------------------------------------------------------------------
class OscInServer
{
public:
    // get server for a particular port; OscInServer constructor is designated
    // private, so this is the intended and only way to get OscInServer instance
    static OscInServer * forPort( int port )
    {
        // lock
        s_portMapMutex.acquire();
        // if no server on the port
        if( s_oscIn.count( port ) == 0 )
        {
            // make a new server and map to port
            s_oscIn[port] = new OscInServer( port );
        }
        // the thing to return
        OscInServer * server = s_oscIn[port];
        // unlock
        s_portMapMutex.release();

        // return the server
        return server;
    }

    // alias a server pointer into a particular port; useful only for auto-assigned ports
    static t_CKBOOL forPortCreateAlias( int port, OscInServer * server )
    {
        // lock
        s_portMapMutex.acquire();
        // if no server on the port
        if( s_oscIn.count( port ) != 0 )
        {
            // print warning
            EM_error3( "(OscIn) warning: cannot create server alias on port %i...", port );
            // done
            return FALSE;
        }
        // make a new server and map to port
        s_oscIn[port] = server;
        // unlock
        s_portMapMutex.release();

        // return success
        return TRUE;
    }

    // shutdown
    static void shutdownAll()
    {
        // TODO
    }

    // add OSC method, e.g., "/foo/frequency"
    void addMethod( const std::string & method, OscIn * obj )
    {
        // message to server
        OscInMsg msg;
        // message type
        msg.msg_type = OscInMsg::ADD_METHOD;
        // parse from method to path and type
        methodToPathAndType( method, msg.path, msg.nopath, msg.type, msg.notype );
        // copy oscin obj ref
        msg.obj = obj;
        // copy msg into message buffer
        m_inMsgBuffer.put( msg );
    }

    void removeMethod( const std::string & method, OscIn * obj )
    {
        // message to server
        OscInMsg msg;
        // message type
        msg.msg_type = OscInMsg::REMOVE_METHOD;
        // parse from method to path and type
        methodToPathAndType( method, msg.path, msg.nopath, msg.type, msg.notype );
        // copy oscin obj ref
        msg.obj = obj;
        // copy msg into message buffer
        m_inMsgBuffer.put( msg );
    }

    void removeAllMethods( OscIn * obj )
    {
        // message to server
        OscInMsg msg;
        // message type
        msg.msg_type = OscInMsg::REMOVEALL_METHODS;
        // copy oscin obj ref
        msg.obj = obj;
        // copy msg into message buffer
        m_inMsgBuffer.put( msg );
    }

    // release ref for the specified client (OscIn)
    void releaseClient( OscIn * obj )
    {
        // message to server
        OscInMsg msg;
        // message type
        msg.msg_type = OscInMsg::RELEASE_CLIENT;
        // copy oscin obj ref
        msg.obj = obj;
        // copy msg into message buffer
        m_inMsgBuffer.put( msg );
    }

    // return port number (might need to wait slightly for server to spin up)
    int port()
    {
        // return assigned port
        return m_assignedPort;
    }

private:
    // static map of port # to OscInServer
    static std::map<int, OscInServer *> s_oscIn;
    // static mutex to protext s_oscIn, since both audio thread calls forPort()
    // and server thread could call forPortCreateAlias() | added 1.5.1.3
    static XMutex s_portMapMutex;


    //-------------------------------------------------------------------------
    // server thread entry point
    //-------------------------------------------------------------------------
#ifdef __PLATFORM_WINDOWS__
    static unsigned int __stdcall s_server_cb( void * data )
#else
    static void * s_server_cb( void * data )
#endif // __PLATFORM_WINDOWS__
    {
        OscInServer * _this = (OscInServer *)data;

#ifdef __PLATFORM_WINDOWS__
        return (t_CKINT)_this->server_cb();
#else
        return _this->server_cb();
#endif // __PLATFORM_WINDOWS__
    }
    //-------------------------------------------------------------------------

    // static error handler
    static void s_err_handler( int num, const char * msg, const char * where )
    {
        if( msg )
            EM_error3( "OscIn: error: %s", msg );
        else
            EM_error3( "OscIn: unknown error" );
    }

    // parse method into path and type
    static void methodToPathAndType( const std::string & method,
        std::string & path, t_CKBOOL & nopath,
        std::string & type, t_CKBOOL & notype )
    {
        // look for comma
        t_CKINT comma_pos = method.find( ',' );

        // yes comma
        if( comma_pos != method.npos )
        {
            path = ltrim( rtrim( method.substr( 0, comma_pos ) ) );
            nopath = FALSE;

            std::string dirty_type = method.substr( comma_pos+1 );
            type.clear();
            for( int i = 0; i < dirty_type.size(); i++ )
                if( CK_OSC_TYPETAG_CHARS.find( dirty_type[i] ) != std::string::npos )
                    type.push_back( dirty_type[i] );
            notype = FALSE;
        }
        else // no comma
        {
            path = ltrim( rtrim( method ) );
            nopath = (path.size() == 0);
            notype = TRUE;
        }
    }

    // inner struct def for an OSC server request message
    struct OscInMsg
    {
        enum Type
        {
            NO_METHOD,
            ADD_METHOD,
            REMOVE_METHOD,
            REMOVEALL_METHODS,
            RELEASE_CLIENT, // 1.5.1.3 (ge) added
        };

        Type msg_type;
        std::string path;
        t_CKBOOL nopath;
        std::string type;
        t_CKBOOL notype;
        OscIn * obj;

        // 1.4.1.0 (ge) added
        OscInMsg()
            : msg_type( NO_METHOD ), nopath( FALSE ), notype( FALSE ), obj( NULL ) { }
    };

    // constructor
    OscInServer( int port ) :
        m_port( port ),
        m_inMsgBuffer( CircularBuffer<OscInMsg>( 1024 ) ),
        m_thread( XThread() ),
        m_quit( false ),
        m_assignedPort( -1 ),
        m_portNotifyTargets( CircularBuffer<OscIn *>(1024) )
    {
        // start server on thread
        m_thread.start( s_server_cb, this );
    }

    // server callback for a particular server
    void * server_cb();

    // data
    XThread m_thread;
    bool m_quit;
    const int m_port;
    int m_assignedPort;
    CircularBuffer<OscInMsg> m_inMsgBuffer;

    // inner struct for OSC method
    struct Method
    {
        std::string path;
        t_CKBOOL nopath;
        std::string type;
        t_CKBOOL notype;

        // default constructor
        Method() : nopath( FALSE ), notype( FALSE ) { }

        // copy constructor
        Method( const Method & m )
        {
            path = m.path;
            nopath = m.nopath;
            type = m.type;
            notype = m.notype;
        }

        // == operator
        bool operator==( const Method & m ) const
        {
            return path == m.path && nopath == m.nopath && type == m.type && notype == m.notype;
        }

        // < operator need in order to use as key in map
        bool operator<( const Method & rhs ) const
        {
            // comparing nopath and notype works because t_CKBOOL are really ints (but we will cast anyway)
            return (path < rhs.path || type < rhs.type ||
                (t_CKUINT)nopath < (t_CKUINT)rhs.nopath || (t_CKUINT)notype < (t_CKUINT)rhs.notype);
        }
    };

    // data structure to hold user_data for liblo callback on incoming message
    struct HandlerData
    {
        OscInServer * server;
        Method method;

        HandlerData( OscInServer * s, const Method & m )
            : server( s ), method( m )
        {
            // std::cerr << "HANDLER DATA CREATE: " << (t_CKUINT)this << std::endl;
        }

        ~HandlerData()
        {
            // std::cerr << "HANDLER DATA DESTROY: " << (t_CKUINT)this << std::endl;
        }

    };

    // map from OscIn to its associated list of added OSC methods
    std::map<OscIn *, std::list<Method> > m_methods;
    // handler data map to track the handler data structre for each unique OSC method | 1.5.1.3
    std::map<Method, HandlerData *> m_handlerDataMap;
    // OscIns to notification for auto-assigned ports | 1.5.1.3
    CircularBuffer<OscIn *> m_portNotifyTargets;

    // assess how many TOTAL OscIn clients have added a particular OSC method | 1.5.1.3
    t_CKUINT total_instances_in_use( const Method & method )
    {
        // the count
        t_CKUINT count = 0;
        // get iterator to methods map
        std::map<OscIn *, std::list<Method> >::iterator m;
        // iterate over all clients
        for( m = m_methods.begin(); m != m_methods.end(); m++ )
        {
            // each entry is <OscIn *, list<Method> >
            // if see this entry contains the method in question
            if( std::find( m->second.begin(), m->second.end(), method ) != m->second.end() )
                count++;
        }
        // done
        return count;
    }

    // called by liblo to handle a message | 1.5.1.3
    static int s_handler( const char * path, const char * types,
        lo_arg ** argv, int argc,
        lo_message msg, void * user_data )
    {
        // our user data
        HandlerData * data = (HandlerData *)user_data;
        // call to dispatch message
        data->server->dispatch_to_client( path, types, argv, argc, msg, data->method );

        // not sure what to return, OscIn::handler() returned -1 so...
        return -1;
    }

public:
    // is this server tracking a particular client (OscIn) | 1.5.1.3
    t_CKBOOL is_tracking( OscIn * obj )
    {
        return m_methods.count( obj ) > 0;
    }

    // attach port auto-assign notify target
    void attachPortNotification( OscIn * client )
    {
        // check for NULL
        if( !client ) return;
        // put on thread-safe buffer
        m_portNotifyTargets.put( client );
        // add ref
        CK_SAFE_ADD_REF( client );
    }

    // dispatch an incoming OSC message on this port to all clients listening | 1.5.1.3
    void dispatch_to_client( const char * path, const char * types,
        lo_arg ** argv, int argc,
        lo_message msg, const Method & method )
    {
        // get iterator to methods map
        std::map<OscIn *, std::list<Method> >::iterator m;
        // iterate over all clients
        for( m = m_methods.begin(); m != m_methods.end(); m++ )
        {
            // each entry is <OscIn *, list<Method> >
            // FILTER if see this entry contains the method in question
            if( std::find( m->second.begin(), m->second.end(), method ) != m->second.end() )
            {
                // call this entry/client's handler
                m->first->handler( path, types, argv, argc, msg );
            }
        }
    }
};

// static instantiation: map of port to OscInServer
std::map<int, OscInServer *> OscInServer::s_oscIn;
// static instantiation: mutex to protect map
XMutex OscInServer::s_portMapMutex;




//-----------------------------------------------------------------------------
// create server message: add OSC method associated with this OscIn client
//-----------------------------------------------------------------------------
void OscIn::addMethod( const std::string & method )
{
    OscInServer::forPort( m_port )->addMethod( method, this );
}

//-----------------------------------------------------------------------------
// create server message: remove OSC method associated with this OscIn client
//-----------------------------------------------------------------------------
void OscIn::removeMethod( const std::string & method )
{
    OscInServer::forPort( m_port )->removeMethod( method, this );
}

//-----------------------------------------------------------------------------
// create server message: remove all OSC method associated with this OscIn client
//-----------------------------------------------------------------------------
void OscIn::removeAllMethods()
{
    OscInServer::forPort( m_port )->removeAllMethods( this );
}

//-----------------------------------------------------------------------------
// create server message: release ref count this OscIn client
//-----------------------------------------------------------------------------
void OscIn::releaseThisClient()
{
    OscInServer::forPort( m_port )->releaseClient( this );
}

//-----------------------------------------------------------------------------
// set port; get port (this also begins priming the port)
//-----------------------------------------------------------------------------
int OscIn::port( int port )
{
    // if same, then do nothing
    if( m_port == port ) return port;

    // if we have an existing port, including 0
    if( m_port >= 0 )
    {
        // request to unsubscribe from current server
        this->removeAllMethods();
    }

    // set port
    m_port = port;

    // if port > 0
    if( port > 0 )
    {
        // call forPort() to get that port's server going
        OscInServer::forPort( port );
    }
    else if( port == 0 ) // auto-assign
    {
        // get server on port 0 (the auto-assign slot)
        OscInServer * server = OscInServer::forPort( 0 );
        // if already been assigned, and we are done
        if( server->port() > 0 ) m_port = server->port();
        // else need to be notified
        else server->attachPortNotification( this );
    }
    else // port < 0
    {
        // set back to -1
        m_port = -1;
    }

    return m_port;
}

//-----------------------------------------------------------------------------
// get port (if 0 => port, then return assigned port
//-----------------------------------------------------------------------------
int OscIn::port()
{
    // 1.5.0.0 (ge) simply return the current m_port
    // 1.5.1.3 (ge) if port was set to 0 for auto-assigned port, this variable
    //              will be notified when the port # is obtained in the server
    //              (which is asynchronous relative to the audio thread)
    return m_port;
}

//-----------------------------------------------------------------------------
// handle get port notification from server (for auto-assigned ports)
//-----------------------------------------------------------------------------
void OscIn::portNotification( int port )
{
    // already have port number, ignore notification (was updated already or set to another port #
    if( m_port > 0 ) return;
    // copy
    m_port = port;
}




//-----------------------------------------------------------------------------
// name: server_cb()
// desc: instanced (one per port) server processing loop
//-----------------------------------------------------------------------------
void * OscInServer::server_cb()
{
    // liblo server handle
    lo_server m_server = NULL;

    // check the port request
    if( m_port > 0 )
    {
        char portStr[32];
        snprintf( portStr, 32, "%d", m_port );
        // create server instance
        m_server = lo_server_new( portStr, s_err_handler );
    }
    else
    {
        // create server instance with NULL port; system will choose unused port
        // (internally, liblo tries up to ~16 random in-range numbers; there is
        // a small chance this will fail)
        m_server = lo_server_new( NULL, s_err_handler );
    }

    // check
    if( m_server == NULL )
    {
        EM_error3( "OscIn: error: unable to create server instance" );
        return NULL;
    }

    // get the actual port in use
    m_assignedPort = lo_server_get_port( m_server );
    // if port was 0, then need to also put this server into the auto-assigned port slot
    if( m_port == 0 && m_assignedPort > 0 )
    {
        // create alias
        OscInServer::forPortCreateAlias( m_assignedPort, this );
    }

    // log
    EM_log( CK_LOG_INFO, "OscIn: starting OSC server at port '%d'", m_assignedPort );

    // loop
    while( !m_quit )
    {
        OscIn * client = NULL;
        // drain thread-safe buffer of clients to notify
        while( m_portNotifyTargets.get( client ) )
        {
            // notify
            client->portNotification( m_assignedPort );
            // release
            CK_SAFE_RELEASE( client );
        }

        OscInMsg msg;
        // get the next message
        while( m_inMsgBuffer.get( msg ) )
        {
            switch( msg.msg_type )
            {
            case OscInMsg::NO_METHOD: // 1.4.1.0 (ge) added
            {
                // just skip
                continue;
            }
            break;

            case OscInMsg::ADD_METHOD:
            {
                Method m;
                m.path = msg.path;
                m.nopath = msg.nopath;
                m.type = msg.type;
                m.notype = msg.notype;

                // check how many different OscIn care about this OSC method on this server | 1.5.1.3
                if( total_instances_in_use( m ) == 0 )
                {
                    // instantiate handler data
                    HandlerData * data = new HandlerData( this, m );
                    // add to map so we can delete later
                    m_handlerDataMap[m] = data;
                    // add method
                    if( !lo_server_add_method( m_server,
                        msg.nopath ? NULL : msg.path.c_str(),
                        msg.notype ? NULL : msg.type.c_str(),
                        OscInServer::s_handler, (void *)data ) )
                    {
                        EM_error3( "OscIn: error: add_method failed for %s, %s", msg.path.c_str(), msg.type.c_str() );
                    }
                }

                // if no entry for this client OscIn, add one
                if( !m_methods.count( msg.obj ) )
                {
                    // add ref for the first
                    CK_SAFE_ADD_REF( msg.obj );
                    // create entry with empty list
                    m_methods[msg.obj] = std::list<Method>();
                }

                // check whether already added for this client
                if( std::find( m_methods[msg.obj].begin(), m_methods[msg.obj].end(), m ) == m_methods[msg.obj].end() )
                {
                    // if not already added, add OSC method to list
                    m_methods[msg.obj].push_back( m );
                }
            }
            break;

            case OscInMsg::REMOVE_METHOD:
            {
                if( m_methods.count( msg.obj ) )
                {
                    Method m;
                    m.path = msg.path;
                    m.nopath = msg.nopath;
                    m.type = msg.type;
                    m.notype = msg.notype;

                    // check this client cares
                    if( std::find( m_methods[msg.obj].begin(), m_methods[msg.obj].end(), m ) != m_methods[msg.obj].end() )
                    {
                        // remove from ourselves
                        m_methods[msg.obj].remove( m );
                    }

                    // check how many different OscIn still care about this OSC method on this server
                    t_CKUINT count = total_instances_in_use( m );
                    // no one cares; no one is thinking about you (on the positive side, as one grows older, this realization can be quite liberating)
                    if( count == 0 )
                    {
                        // remove from liblo
                        lo_server_del_method( m_server,
                            msg.nopath ? NULL : msg.path.c_str(),
                            msg.notype ? NULL : msg.type.c_str() );
                        // free memory
                        CK_SAFE_DELETE( m_handlerDataMap[m] );
                        // erase the entry
                        m_handlerDataMap.erase( m );
                    }
                }
            }
            break;

            case OscInMsg::REMOVEALL_METHODS:
            {
                // if there is an entry for the client OscIn
                if( m_methods.count( msg.obj ) )
                {
                    // iterate
                    std::list<Method>::iterator i;
                    for( i = m_methods[msg.obj].begin(); i != m_methods[msg.obj].end(); i++ )
                    {
                        // check how many different OscIn care about this OSC method on this server
                        t_CKUINT count = total_instances_in_use( *i );
                        // count == 1 means we are the only one
                        // count > 1 means others care as well so we should leave it be)
                        if( count == 1 )
                        {
                            // delete OSC method from liblo
                            lo_server_del_method( m_server,
                                i->nopath ? NULL : i->path.c_str(),
                                i->notype ? NULL : i->type.c_str() );
                            // free memory for handler data
                            CK_SAFE_DELETE( m_handlerDataMap[*i] );
                            // erase the entry from the handler data map
                            m_handlerDataMap.erase( *i );
                        }
                    }

                    // clear the contents of the list
                    m_methods[msg.obj].clear();
                    // erase the entry in the methods map
                    m_methods.erase( msg.obj );
                }
            }
            break;

            // release ref count for client | 1.5.1.3
            case OscInMsg::RELEASE_CLIENT:
            {
                // release ref | 1.5.1.3
                CK_SAFE_RELEASE( msg.obj );
            }
            break;
            }
        }

        // FYI this should be the server thread (not audio thread)
        // check for new message with a 2ms timeout; there are are incoming message,
        // a mechanism within this call will call us back on s_handler()
        lo_server_recv_noblock( m_server, 2 );
    }

    // done with this OscInServer
    lo_server_free( m_server );

    // local
    EM_log( CK_LOG_INFO, "OscIn: shutting down OSC server on port %i", m_assignedPort );

    return NULL;
}




//-----------------------------------------------------------------------------
// name: class OscOut
// desc: OSC sender, internal data structure
//-----------------------------------------------------------------------------
class OscOut
{
public:
    // constructor
    OscOut()
    {
        m_address = NULL;
        m_message = NULL;
        // 1.4.1.1 (ge) added
        m_port = 0;
    }

    // destructor
    ~OscOut()
    {
        // if we have one
        if( m_address != NULL )
        {
            // free address
            lo_address_free( m_address );
            // set to NULL
            m_address = NULL;
        }

        // if we have unsent message
        if( m_message != NULL )
        {
            // free message
            lo_message_free( m_message );
            // set to NULL
            m_message = NULL;
        }
    }

    // where to aim the transmission
    t_CKBOOL setDestination( const std::string & host, int port )
    {
        if( m_address != NULL )
        {
            lo_address_free( m_address );
            m_address = NULL;
        }

        char portStr[32];
        snprintf( portStr, 32, "%d", port );

        // 1.4.1.1 (ge) remember current hostname and port
        m_hostname = host;
        m_port = port;

        // allocate address
        m_address = lo_address_new( host.c_str(), portStr );

        // check it
        if( m_address == NULL )
        {
            const char * msg = ""; // lo_address_errstr(NULL); // 1.4.1.0 commented out
            EM_error3( "OscOut: error: failed to set destination address '%s:%d'%s%s",
                host.c_str(), port, msg ? ": " : "", msg ? msg : "" );
            return FALSE;
        }

        return TRUE;
    }

    // start, with OSC method
    t_CKBOOL start( const std::string & path, const std::string & arg )
    {
        return start( path + "," + arg );
    }

    // start, with OSC method
    t_CKBOOL start( const std::string & method )
    {
        std::string path;
        t_CKINT comma_pos = method.find( ',' );
        if( comma_pos != method.npos ) m_path = method.substr( 0, comma_pos );
        else m_path = method;

        // clean up if there is an unsent message | 1.5.1.3
        if( m_message != NULL ) lo_message_free( m_message );

        // allocate new messaage
        m_message = lo_message_new();

        // return whether msg is NULL | 1.5.1.3
        return m_message != NULL;
    }

    // add an integer argument
    t_CKBOOL add( t_CKINT i )
    {
        if( m_message == NULL )
        {
            EM_error3( "OscOut: error: attempt to add argument to message with no OSC address" );
            return FALSE;
        }

        lo_message_add_int32( m_message, (int32_t)i );

        return TRUE;
    }

    // add a floating point argument
    t_CKBOOL add( t_CKFLOAT f )
    {
        if( m_message == NULL )
        {
            EM_error3( "OscOut: error: attempt to add argument to message with no OSC address" );
            return FALSE;
        }

        lo_message_add_float( m_message, (float)f );

        return TRUE;
    }

    // add a string argument
    t_CKBOOL add( const std::string & s )
    {
        if( m_message == NULL )
        {
            EM_error3( "OscOut: error: attempt to add argument to message with no OSC address" );
            return FALSE;
        }

        lo_message_add_string( m_message, s.c_str() );

        return TRUE;
    }

    // send the message
    t_CKBOOL send()
    {
        // check if we have necessary info
        if( m_message == NULL || m_address == NULL || m_path.size() == 0 )
        {
            EM_error3( "OscOut: error: attempt to send message with no destination or OSC address" );
            return FALSE;
        }

        // send message
        int result = lo_send_message( m_address, m_path.c_str(), m_message );

        // free message
        lo_message_free( m_message );
        // set to NULL
        m_message = NULL;

        // check result
        if( result == -1 )
        {
            const char * msg = lo_address_errstr( m_address );
            // 1.4.1.1 (ge) updated to also print out hostname:port
            EM_error3( "OscOut: error: sending OSC message%s%s (%s:%d)",
                msg ? ": " : "", msg ? msg : "", m_hostname.c_str(), m_port );
            return FALSE;
        }

        return TRUE;
    }

private:
    lo_address m_address;
    std::string m_path;
    lo_message m_message;
    // 1.4.1.1 (ge) added to print hostname:port in error reporting
    std::string m_hostname;
    int m_port;
};

// data offset for OscIn
static t_CKUINT oscin_offset_data = 0;
// data offset for OscOut
static t_CKUINT oscout_offset_data = 0;
// data offsets for OscMsg
static t_CKUINT oscmsg_offset_address = 0;
static t_CKUINT oscmsg_offset_typetag = 0;
static t_CKUINT oscmsg_offset_args = 0;
// data offsets for OscArg
static t_CKUINT oscarg_offset_type = 0;
static t_CKUINT oscarg_offset_i = 0;
static t_CKUINT oscarg_offset_f = 0;
static t_CKUINT oscarg_offset_s = 0;




#pragma mark - OscOut
//-----------------------------------------------------------------------------
// name: OscOut
//-----------------------------------------------------------------------------
CK_DLL_CTOR( oscout_ctor )
{
    OBJ_MEMBER_INT( SELF, oscout_offset_data ) = (t_CKINT)new OscOut;
}

CK_DLL_DTOR( oscout_dtor )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );
    CK_SAFE_DELETE( out );
    OBJ_MEMBER_INT( SELF, oscout_offset_data ) = 0;
}

CK_DLL_MFUN( oscout_dest )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    Chuck_String * host = GET_NEXT_STRING( ARGS );
    t_CKINT port = GET_NEXT_INT( ARGS );

    if( host == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscOut.start: argument 'host' is null" );
        goto error;
    }

    if( !out->setDestination( host->str(), port ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_start )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    Chuck_String * method = GET_NEXT_STRING( ARGS );

    if( method == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscOut.start: argument 'method' is null" );
        goto error;
    }

    if( !out->start( method->str() ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_startDest )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    Chuck_String * method = GET_NEXT_STRING( ARGS );
    Chuck_String * host = GET_NEXT_STRING( ARGS );
    t_CKINT port = GET_NEXT_INT( ARGS );

    if( method == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscOut.start: argument 'method' is null" );
        goto error;
    }

    if( host == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscOut.start: argument 'host' is null" );
        goto error;
    }

    if( !out->setDestination( host->str(), port ) )
        goto error;

    if( !out->start( method->str() ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_addInt )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    t_CKINT i = GET_NEXT_INT( ARGS );

    if( !out->add( i ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_addFloat )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    t_CKFLOAT f = GET_NEXT_FLOAT( ARGS );

    if( !out->add( f ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_addString )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    Chuck_String * s = GET_NEXT_STRING( ARGS );

    if( !out->add( s->str() ) )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN( oscout_send )
{
    OscOut * out = (OscOut *)OBJ_MEMBER_INT( SELF, oscout_offset_data );

    if( !out->send() )
        goto error;

    RETURN->v_object = SELF;

    return;

error:
    RETURN->v_object = NULL;
}




#pragma mark - OscArg
//-----------------------------------------------------------------------------
// name: OscArg
//-----------------------------------------------------------------------------
CK_DLL_CTOR( oscarg_ctor )
{
    Chuck_String * type = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    CK_SAFE_ADD_REF( type );
    OBJ_MEMBER_STRING( SELF, oscarg_offset_type ) = type;

    OBJ_MEMBER_INT( SELF, oscarg_offset_i ) = 0;

    OBJ_MEMBER_FLOAT( SELF, oscarg_offset_f ) = 0.0;

    Chuck_String * s = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    CK_SAFE_ADD_REF( s );
    OBJ_MEMBER_STRING( SELF, oscarg_offset_s ) = s;
}

CK_DLL_DTOR( oscarg_dtor )
{
    CK_SAFE_RELEASE( OBJ_MEMBER_STRING( SELF, oscarg_offset_type ) );
    OBJ_MEMBER_STRING( SELF, oscarg_offset_type ) = NULL;

    CK_SAFE_RELEASE( OBJ_MEMBER_STRING( SELF, oscarg_offset_s ) );
    OBJ_MEMBER_STRING( SELF, oscarg_offset_s ) = NULL;
}




#pragma mark - OscIn
//-----------------------------------------------------------------------------
// name: OscIn
//-----------------------------------------------------------------------------
CK_DLL_CTOR( oscin_ctor )
{
    // create internal OscIn data structure; pass in SELF (OscIn is defined as a subclass of Event)
    OscIn * oscin = new OscIn( (Chuck_Event *)SELF, SHRED->vm_ref );
    // add reference for cross-thread memory managment
    CK_SAFE_ADD_REF( oscin );
    // set into chuck object's data field
    OBJ_MEMBER_INT( SELF, oscin_offset_data ) = (t_CKINT)oscin;
}

CK_DLL_DTOR( oscin_dtor )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // begin shutdown between audio thread (likely what called this) and server thread
    in->shutdown_begin();
    // release on this side
    CK_SAFE_RELEASE( in );
    // zero out
    OBJ_MEMBER_INT( SELF, oscin_offset_data ) = 0;
}

CK_DLL_MFUN( oscin_setport )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // get the argument
    t_CKINT port = GET_NEXT_INT( ARGS );
    // set the port
    RETURN->v_int = in->port( port );
}

CK_DLL_MFUN( oscin_getport )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // get the port
    RETURN->v_int = in->port();
}

CK_DLL_MFUN( oscin_addAddress )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // get the arg
    Chuck_String * address = GET_NEXT_STRING( ARGS );
    // check the arg
    if( address == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscIn.address: argument 'address' is null" );
        goto error;
    }
    // add OSC method associated with this OscIn
    in->addMethod( address->str() );

error:
    // nothing to do
    return;
}

CK_DLL_MFUN( oscin_removeAddress )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // get the arg
    Chuck_String * address = GET_NEXT_STRING( ARGS );
    // check the arg
    if( address == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscIn.address: argument 'address' is null" );
        goto error;
    }
    // remove OSC method associated with this OscIn
    in->removeMethod( address->str() );

error:
    // nothing to do
    return;
}

CK_DLL_MFUN( oscin_removeAllAddresses )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // remove all OSC methods associated with this OscIn
    in->removeAllMethods();
}

CK_DLL_MFUN( oscin_listenAll )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // add "ALL" OSC method string
    in->addMethod( "" );
}

CK_DLL_MFUN( oscin_recv )
{
    // get internal data structure
    OscIn * in = (OscIn *)OBJ_MEMBER_INT( SELF, oscin_offset_data );
    // get the OscMsg arg
    Chuck_Object * msg_obj = GET_NEXT_OBJECT( ARGS );
    // internal representation
    OscMsg msg;

    // some local for later
    Chuck_ArrayInt * args_obj = NULL;
    Chuck_Type * oscarg_type = NULL;
    int i;

    // check for NULL
    if( msg_obj == NULL )
    {
        ck_throw_exception( SHRED, "NullPointer", "OscIn.recv: argument 'msg' is null" );
        goto error;
    }

    // get the message
    RETURN->v_int = in->get( msg );

    // set it in the chuck-side OscMsg
    OBJ_MEMBER_STRING( msg_obj, oscmsg_offset_address )->set( msg.path );
    OBJ_MEMBER_STRING( msg_obj, oscmsg_offset_typetag )->set( msg.type );
    // get the array at OscMsg.args
    args_obj = (Chuck_ArrayInt *)OBJ_MEMBER_OBJECT( msg_obj, oscmsg_offset_args );
    // clear it (which will also memory-manage existing contents)
    args_obj->clear();
    // find the type corresponding to chuck-side OscArg
    oscarg_type = type_engine_find_type( SHRED->vm_ref->env(), str2list( "OscArg" ) );

    // go through args
    for( i = 0; i < msg.args.size(); i++ )
    {
        // instantiate a OscArg
        Chuck_Object * arg_obj = instantiate_and_initialize_object( oscarg_type, SHRED );
        // HACK: manually call osc_arg ctor
        oscarg_ctor( arg_obj, NULL, VM, SHRED, API );
        // check message type tag and set
        switch( msg.type[i] )
        {
        case 'i':
            OBJ_MEMBER_STRING( arg_obj, oscarg_offset_type )->set( "i" );
            OBJ_MEMBER_INT( arg_obj, oscarg_offset_i ) = msg.args[i].i;
            break;
        case 'f':
            OBJ_MEMBER_STRING( arg_obj, oscarg_offset_type )->set( "f" );
            OBJ_MEMBER_FLOAT( arg_obj, oscarg_offset_f ) = msg.args[i].f;
            break;
        case 's':
            OBJ_MEMBER_STRING( arg_obj, oscarg_offset_type )->set( "s" );
            OBJ_MEMBER_STRING( arg_obj, oscarg_offset_s )->set( msg.args[i].s );
            break;
        }
        // push back into chuck-side array
        args_obj->push_back( (t_CKINT)arg_obj );
    }

    // done
    return;

error:
    // return failure code
    RETURN->v_int = 0;
}




#pragma mark - OscMsg
//-----------------------------------------------------------------------------
// name: OscMsg
//-----------------------------------------------------------------------------
CK_DLL_CTOR( oscmsg_ctor )
{
    Chuck_String * address = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    CK_SAFE_ADD_REF( address );
    OBJ_MEMBER_STRING( SELF, oscmsg_offset_address ) = address;

    Chuck_String * typetag = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->ckt_string, SHRED );
    CK_SAFE_ADD_REF( typetag );
    OBJ_MEMBER_STRING( SELF, oscmsg_offset_typetag ) = typetag;

    Chuck_ArrayInt * args = new Chuck_ArrayInt( TRUE );
    initialize_object( args, SHRED->vm_ref->env()->ckt_array, SHRED, VM );
    args->clear();
    CK_SAFE_ADD_REF( args );
    OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args ) = args;
}

CK_DLL_DTOR( oscmsg_dtor )
{
    CK_SAFE_RELEASE( OBJ_MEMBER_STRING( SELF, oscmsg_offset_address ) );
    OBJ_MEMBER_STRING( SELF, oscmsg_offset_address ) = NULL;

    CK_SAFE_RELEASE( OBJ_MEMBER_STRING( SELF, oscmsg_offset_typetag ) );
    OBJ_MEMBER_STRING( SELF, oscmsg_offset_typetag ) = NULL;

    CK_SAFE_RELEASE( OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args ) );
    OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args ) = NULL;
}

CK_DLL_MFUN( oscmsg_numArgs )
{
    Chuck_ArrayInt * args_obj = (Chuck_ArrayInt *)OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args );
    RETURN->v_int = args_obj->size();
}

CK_DLL_MFUN( oscmsg_getInt )
{
    t_CKINT i = GET_NEXT_INT( ARGS );
    Chuck_ArrayInt * args_obj = (Chuck_ArrayInt *)OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args );
    Chuck_Object * arg_obj;

    // check bounds
    if( i >= 0 && i < args_obj->size() )
    {
        args_obj->get( i, (t_CKUINT *)&arg_obj );
        RETURN->v_int = OBJ_MEMBER_INT( arg_obj, oscarg_offset_i );
    }
    else
    {
        RETURN->v_int = 0;
    }
}

CK_DLL_MFUN( oscmsg_getFloat )
{
    t_CKINT i = GET_NEXT_INT( ARGS );
    Chuck_ArrayInt * args_obj = (Chuck_ArrayInt *)OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args );
    Chuck_Object * arg_obj;
    if( i >= 0 && i < args_obj->size() )
    {
        args_obj->get( i, (t_CKUINT *)&arg_obj );
        RETURN->v_float = OBJ_MEMBER_FLOAT( arg_obj, oscarg_offset_f );
    }
    else
        RETURN->v_float = 0;
}

CK_DLL_MFUN( oscmsg_getString )
{
    t_CKINT i = GET_NEXT_INT( ARGS );
    Chuck_ArrayInt * args_obj = (Chuck_ArrayInt *)OBJ_MEMBER_OBJECT( SELF, oscmsg_offset_args );
    Chuck_Object * arg_obj;

    if( i >= 0 && i < args_obj->size() )
    {
        args_obj->get( i, (t_CKUINT *)&arg_obj );
        RETURN->v_string = OBJ_MEMBER_STRING( arg_obj, oscarg_offset_s );
    }
    else
        RETURN->v_string = NULL;
}

static t_CKUINT osc_send_offset_data = 0;
static t_CKUINT osc_recv_offset_data = 0;
static t_CKUINT osc_address_offset_data = 0;
static Chuck_Type * osc_addr_type_ptr = 0;




//-----------------------------------------------------------------------------
// name: opensoundcontrol_query()
// desc: OSC import into chuck type-system
//-----------------------------------------------------------------------------
DLL_QUERY opensoundcontrol_query( Chuck_DL_Query * query )
{
    /*** OscOut ***/
    query->begin_class( query, "OscOut", "Object" );
    query->doc_class( query, "Class for sending Open Sound Control (OSC) messages. See examples for usage." );
    oscout_offset_data = query->add_mvar( query, "int", "@OscOut_data", FALSE );

    query->add_ctor( query, oscout_ctor );
    query->add_dtor( query, oscout_dtor );

    query->add_mfun( query, oscout_dest, "OscOut", "dest" );
    query->add_arg( query, "string", "hostname" );
    query->add_arg( query, "int", "port" );
    query->doc_func( query, "Set the destination hostname and port for sending OSC message." );

    query->add_mfun( query, oscout_start, "OscOut", "start" );
    query->add_arg( query, "string", "address" );
    query->doc_func( query, "Start an OSC message with a particular address." );

    query->add_mfun( query, oscout_startDest, "OscOut", "start" );
    query->add_arg( query, "string", "address" );
    query->add_arg( query, "string", "host" );
    query->add_arg( query, "int", "port" );
    query->doc_func( query, "Start an OSC message with a particular address, aimed at a destination host and port." );

    query->add_mfun( query, oscout_addInt, "OscOut", "add" );
    query->add_arg( query, "int", "i" );
    query->doc_func( query, "Add an integer value to an OSC message." );

    query->add_mfun( query, oscout_addFloat, "OscOut", "add" );
    query->add_arg( query, "float", "f" );
    query->doc_func( query, "Add a floating-point value to an OSC message." );

    query->add_mfun( query, oscout_addString, "OscOut", "add" );
    query->add_arg( query, "string", "s" );
    query->doc_func( query, "Add an string value to an OSC message." );

    query->add_mfun( query, oscout_send, "OscOut", "send" );
    query->doc_func( query, "Send the current OSC message." );

    // add examples | 1.5.0.0 (ge) added
    query->add_ex( query, "osc/s.ck" );
    query->add_ex( query, "osc/r.ck" );
    query->add_ex( query, "osc/osc-dump.ck" );

    query->end_class( query );


    /*** OscArg ***/

    query->begin_class( query, "OscArg", "Object" );

    oscarg_offset_type = query->add_mvar( query, "string", "type", FALSE );
    oscarg_offset_i = query->add_mvar( query, "int", "i", FALSE );
    oscarg_offset_f = query->add_mvar( query, "float", "f", FALSE );
    oscarg_offset_s = query->add_mvar( query, "string", "s", FALSE );

    query->add_ctor( query, oscarg_ctor );
    query->add_dtor( query, oscarg_dtor );

    query->end_class( query );


    /*** OscMsg ***/
    query->begin_class( query, "OscMsg", "Object" );
    query->doc_class( query, "Helper class for receiving the contents of an OSC message." );

    oscmsg_offset_address = query->add_mvar( query, "string", "address", FALSE );
    query->doc_var( query, "The OSC address string (e.g., \"/foo/param\")." );
    oscmsg_offset_typetag = query->add_mvar( query, "string", "typetag", FALSE );
    query->doc_var( query, "The OSC type tag string (e.g., \"iif\" for int, int, float)." );
    oscmsg_offset_args = query->add_mvar( query, "OscArg[]", "args", FALSE );
    query->doc_var( query, "Array of OscArgs contained in this message." );

    query->add_mfun( query, oscmsg_numArgs, "int", "numArgs" );
    query->doc_func( query, "Get the number of arguments contained in this OscMsg." );

    query->add_mfun( query, oscmsg_getInt, "int", "getInt" );
    query->add_arg( query, "int", "i" );
    query->doc_func( query, "Get argument (at index 'i') as an integer." );

    query->add_mfun( query, oscmsg_getFloat, "float", "getFloat" );
    query->add_arg( query, "int", "i" );
    query->doc_func( query, "Get argument (at index 'i') as a float." );

    query->add_mfun( query, oscmsg_getString, "string", "getString" );
    query->add_arg( query, "int", "i" );
    query->doc_func( query, "Get argument (at index 'i') as a string." );

    query->add_ctor( query, oscmsg_ctor );
    query->add_dtor( query, oscmsg_dtor );

    // add examples | 1.5.0.0 (ge) added
    query->add_ex( query, "osc/r.ck" );
    query->add_ex( query, "osc/s.ck" );
    query->add_ex( query, "osc/osc-dump.ck" );
    query->add_ex( query, "osc/multi-msg/r-multi-msg.ck" );

    query->end_class( query );


    /*** OscIn ***/
    query->begin_class( query, "OscIn", "Event" );
    query->doc_class( query, "Class for receiving Open Sound Control (OSC) messages. See examples for usage." );

    // add member reference
    oscin_offset_data = query->add_mvar( query, "int", "@OscIn_data", FALSE );

    query->add_ctor( query, oscin_ctor );
    query->add_dtor( query, oscin_dtor );

    // get port
    query->add_mfun( query, oscin_getport, "int", "port" );
    query->doc_func( query, "Get which port to listen on." );
    // set port
    query->add_mfun( query, oscin_setport, "int", "port" );
    query->add_arg( query, "int", "p" );
    query->doc_func( query, "Set which port to listen on; this will begin priming the background OSC listener "
        "on the named port. If port is set to 0, a usable port would be automatically assigned; the auto-assigned "
        "port number can be retrieved by calling .port() but may initially take some time to acquire (e.g., hundreds of milliseconds); "
        "if there is more than one OscIn client on port 0, they all will eventually share the same auto-assigned port." );

    // add address
    query->add_mfun( query, oscin_addAddress, "void", "addAddress" );
    query->add_arg( query, "string", "address" );
    query->doc_func( query, "Add an OSC address to receive messages from." );
    // remove address
    query->add_mfun( query, oscin_removeAddress, "void", "removeAddress" );
    query->add_arg( query, "string", "address" );
    query->doc_func( query, "Stop listening on a particular OSC address." );
    // remove all addresses
    query->add_mfun( query, oscin_removeAllAddresses, "void", "removeAllAddresses" );
    query->doc_func( query, "Stop listening on all OSC addresses." );

    // listenAll()
    query->add_mfun( query, oscin_listenAll, "void", "listenAll" );
    query->doc_func( query, "Set OscIn to receive messages of any OSC address." );
    // recv
    query->add_mfun( query, oscin_recv, "int", "recv" );
    query->add_arg( query, "OscMsg", "msg" );
    query->doc_func( query, "Receive the next queued incoming OSC message, returning its contents in `msg`." );

    // add examples | 1.5.0.0 (ge) added
    query->add_ex( query, "osc/r.ck" );
    query->add_ex( query, "osc/s.ck" );
    query->add_ex( query, "osc/osc-dump.ck" );
    query->add_ex( query, "osc/multi-msg/r-multi-msg.ck" );

    // end class definition
    query->end_class( query );


    // get the env
    Chuck_Env * env = query->env();
    Chuck_DL_Func * func = NULL;

    // init base class
    if( !type_engine_import_class_begin( env, "OscSend", "Object",
        env->global(), osc_send_ctor,
        osc_send_dtor ) )
        return FALSE;

    // add member variable  - OSCTransmitter object
    osc_send_offset_data = type_engine_import_mvar( env, "int", "@OscSend_data", FALSE );
    if( osc_send_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "int", "setHost", osc_send_setHost );
    func->add_arg( "string", "address" );
    func->add_arg( "int", "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "startMsg", osc_send_startMesg );
    func->add_arg( "string", "address" );
    func->add_arg( "string", "args" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "startMsg", osc_send_startMesg_spec );
    func->add_arg( "string", "spec" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "addInt", osc_send_addInt );
    func->add_arg( "int", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "addFloat", osc_send_addFloat );
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "addString", osc_send_addString );
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "openBundle", osc_send_openBundle );
    func->add_arg( "time", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "closeBundle", osc_send_closeBundle );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "hold", osc_send_holdMesg );
    func->add_arg( "int", "on" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "kick", osc_send_kickMesg );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    type_engine_import_class_end( env );

    // init base class
    if( !type_engine_import_class_begin( env, "OscEvent", "Event",
        env->global(), osc_address_ctor,
        osc_address_dtor ) )
        return FALSE;

    // add member variable  - OSCAddress object

    osc_address_offset_data = type_engine_import_mvar( env, "int", "@OscEvent_Data", FALSE );
    if( osc_recv_offset_data == CK_INVALID_OFFSET ) goto error;

    // keep type around for initialization ( so other classes can return it )

    osc_addr_type_ptr = env->class_def;

    func = make_new_mfun( "int", "set", osc_address_set );
    func->add_arg( "string", "addr" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "hasMsg", osc_address_has_mesg );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "nextMsg", osc_address_next_mesg );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "getInt", osc_address_next_int );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "getFloat", osc_address_next_float );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "getString", osc_address_next_string );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "can_wait", osc_address_can_wait );
    func->doc = "(internal) used by virtual machine for synthronization.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    type_engine_import_class_end( env );

    // init base class
    if( !type_engine_import_class_begin( env, "OscRecv", "Object",
        env->global(), osc_recv_ctor,
        osc_recv_dtor ) )
        return FALSE;

    // add member variable  - OSCReceiver object
    osc_recv_offset_data = type_engine_import_mvar( env, "int", "@OscRecv_data", FALSE );
    if( osc_recv_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "int", "port", osc_recv_port );
    func->add_arg( "int", "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "listen", osc_recv_listen_port );
    func->add_arg( "int", "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "listen", osc_recv_listen );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "stop", osc_recv_listen_stop );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "add_address", osc_recv_add_address );
    func->add_arg( "OscEvent", "addr" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "remove_address", osc_recv_remove_address );
    func->add_arg( "OscEvent", "addr" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "event", osc_recv_new_address );
    func->add_arg( "string", "spec" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "event", osc_recv_new_address_type );
    func->add_arg( "string", "address" );
    func->add_arg( "string", "type" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "address", osc_recv_new_address );
    func->add_arg( "string", "spec" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "address", osc_recv_new_address_type );
    func->add_arg( "string", "address" );
    func->add_arg( "string", "type" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    type_engine_import_class_end( env );
    return TRUE;

error:
    CK_FPRINTF_STDERR( "class import error!\n" );
    // end the class import
    type_engine_import_class_end( env );
    return FALSE;
}

//----------------------------------------------
// name : osc_send_ctor
// desc : CTOR function
//-----------------------------------------------
CK_DLL_CTOR( osc_send_ctor ) {
    OSC_Transmitter * xmit = new OSC_Transmitter();
    OBJ_MEMBER_INT( SELF, osc_send_offset_data ) = (t_CKINT)xmit;
}

//----------------------------------------------
// name :  osc_send_dtor
// desc : DTOR function
//-----------------------------------------------
CK_DLL_DTOR( osc_send_dtor ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    CK_SAFE_DELETE( xmit );
    OBJ_MEMBER_UINT( SELF, osc_send_offset_data ) = 0;
}

//----------------------------------------------
// name :  osc_send_setHost
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_setHost ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    Chuck_String * host = GET_NEXT_STRING( ARGS );
    t_CKINT port = GET_NEXT_INT( ARGS );
    xmit->setHost( (char *)host->str().c_str(), port );
}

//----------------------------------------------
// name :  osc_send_startMesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_startMesg ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    Chuck_String * address = GET_NEXT_STRING( ARGS );
    Chuck_String * args = GET_NEXT_STRING( ARGS );
    xmit->startMessage( (char *)address->str().c_str(), (char *)args->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_startMesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_startMesg_spec ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    Chuck_String * spec = GET_NEXT_STRING( ARGS );
    xmit->startMessage( (char *)spec->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_addInt
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addInt ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->addInt( GET_NEXT_INT( ARGS ) );
}

//----------------------------------------------
// name :  osc_send_addFloat
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addFloat ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->addFloat( (float)(GET_NEXT_FLOAT( ARGS )) );
}

//----------------------------------------------
// name :  osc_send_addString
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addString ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->addString( (char *)(GET_NEXT_STRING( ARGS ))->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_openBundle
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_openBundle ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    // we should add in an option to translate from chuck-time to current time to timetag.
    // but for now let's just spec. immediately.
    xmit->openBundle( OSCTT_Immediately() );
}

//----------------------------------------------
// name :  osc_send_closeBundle
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_closeBundle ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->closeBundle();
}

//----------------------------------------------
// name :  osc_send_holdMesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_holdMesg ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->holdMessage( GET_NEXT_INT( ARGS ) != 0 );
}

//----------------------------------------------
// name :  osc_send_kickMesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_send_kickMesg ) {
    OSC_Transmitter * xmit = (OSC_Transmitter *)OBJ_MEMBER_INT( SELF, osc_send_offset_data );
    xmit->kickMessage();
}

//----------------------------------------------
// name :  osc_address_ctor
// desc : CTOR function
//-----------------------------------------------
CK_DLL_CTOR( osc_address_ctor ) {
    OSC_Address_Space * addr = new OSC_Address_Space();
    addr->SELF = SELF;
    //    CK_FPRINTF_STDERR("address:ptr %x\n", (uint)addr );
    //    CK_FPRINTF_STDERR("self:ptr %x\n", (uint)SELF );
    OBJ_MEMBER_INT( SELF, osc_address_offset_data ) = (t_CKINT)addr;
}

CK_DLL_DTOR( osc_address_dtor ) {
    delete (OSC_Address_Space *)OBJ_MEMBER_UINT( SELF, osc_address_offset_data );
    OBJ_MEMBER_UINT( SELF, osc_address_offset_data ) = 0;
}

CK_DLL_MFUN( osc_address_set ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    addr->setSpec( (char *)(GET_NEXT_STRING( ARGS ))->str().c_str() );
    RETURN->v_int = 0;
}

//----------------------------------------------
// name : osc_address_can_wait
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_can_wait ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = (addr->has_mesg()) ? 0 : 1;
}

//----------------------------------------------
// name : osc_address_has_mesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_has_mesg ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = (addr->has_mesg()) ? 1 : 0;
}

//----------------------------------------------
// name : osc_address_next_mesg
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_mesg ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = (addr->next_mesg()) ? 1 : 0;
}

//----------------------------------------------
// name : osc_address_next_int
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_int ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = addr->next_int();
}

//----------------------------------------------
// name : osc_address_next_float
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_float ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_float = addr->next_float();
}

//----------------------------------------------
// name : osc_address_next_string
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_string ) {
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    char * cs = addr->next_string();
    Chuck_String * ckstr = (cs) ? new Chuck_String( cs ) : new Chuck_String( "" );
    initialize_object( ckstr, SHRED->vm_ref->env()->ckt_string, SHRED, VM );
    RETURN->v_string = ckstr;
}




// OscRecv functions
//----------------------------------------------
// name : osc_recv_ctor
// desc : CTOR function
//-----------------------------------------------
CK_DLL_CTOR( osc_recv_ctor )
{
    OSC_Receiver * recv = new OSC_Receiver( SHRED->vm_ref );
    OBJ_MEMBER_INT( SELF, osc_send_offset_data ) = (t_CKINT)recv;
}

//----------------------------------------------
// name : osc_recv_dtor
// desc : DTOR function
//-----------------------------------------------
CK_DLL_DTOR( osc_recv_dtor )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    CK_SAFE_DELETE( recv );
    OBJ_MEMBER_INT( SELF, osc_recv_offset_data ) = 0;
}

//----------------------------------------------
// name : osc_recv_port
// desc : specify port to listen on
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_port )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    recv->bind_to_port( (int)GET_NEXT_INT( ARGS ) );
}

// need to add a listen function in Receiver which opens up a recv loop on another thread.
// address then subscribe to a receiver to take in events.

//----------------------------------------------
// name : osc_recv_listen
// desc : start listening
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    recv->listen();
}

//----------------------------------------------
// name : osc_recv_listen_port
// desc : listen to a given port ( disconnects from current )
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen_port )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    recv->listen( (int)GET_NEXT_INT( ARGS ) );
}

//----------------------------------------------
// name : osc_recv_listen_port
// desc : listen to a given port ( disconnects from current )
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen_stop )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    recv->stopListening();
}

//----------------------------------------------
// name : osc_recv_add_listener
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_add_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    Chuck_Object * addr_obj = GET_NEXT_OBJECT( ARGS ); //address object class...
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( addr_obj, osc_address_offset_data );
    recv->add_address( addr );
}

//----------------------------------------------
// name : osc_recv_remove_listener
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_remove_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    Chuck_Object * addr_obj = GET_NEXT_OBJECT( ARGS ); //listener object class...
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( addr_obj, osc_address_offset_data );
    recv->remove_address( addr );
}

//----------------------------------------------
// name : osc_recv_new_address
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_new_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    Chuck_String * spec_obj = (Chuck_String *)GET_NEXT_STRING( ARGS ); // listener object class...

    // added 1.3.1.1: fix potential race condition
    //OSC_Address_Space * new_addr_obj = recv->new_event( (char*)spec_obj->str.c_str() );
    OSC_Address_Space * new_addr_obj = new OSC_Address_Space( (char *)spec_obj->str().c_str() );

    /* wolf in sheep's clothing
    initialize_object( new_addr_obj , osc_addr_type_ptr ); //initialize in vm
    new_addr_obj->SELF = new_addr_obj;
    OBJ_MEMBER_INT( new_addr_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;
    */

    // more correct...?
    Chuck_Event * new_event_obj = new Chuck_Event();
    initialize_object( new_event_obj, osc_addr_type_ptr, SHRED, VM );
    new_addr_obj->SELF = new_event_obj;
    OBJ_MEMBER_INT( new_event_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;

    recv->add_address( new_addr_obj );

    RETURN->v_object = new_event_obj;
}



//----------------------------------------------
// name : osc_recv_new_address
// desc : MFUN function
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_new_address_type )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT( SELF, osc_recv_offset_data );
    Chuck_String * addr_obj = (Chuck_String *)GET_NEXT_STRING( ARGS ); //listener object class...
    Chuck_String * type_obj = (Chuck_String *)GET_NEXT_STRING( ARGS ); //listener object class...
    OSC_Address_Space * new_addr_obj = recv->new_event( (char *)addr_obj->str().c_str(), (char *)type_obj->str().c_str() );

    /* wolf in sheep's clothing
    initialize_object( new_addr_obj , osc_addr_type_ptr ); //initialize in vm
    new_addr_obj->SELF = new_addr_obj;
    OBJ_MEMBER_INT( new_addr_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;
    */

    // more correct...?
    Chuck_Event * new_event_obj = new Chuck_Event();
    initialize_object( new_event_obj, osc_addr_type_ptr, SHRED, VM );
    new_addr_obj->SELF = new_event_obj;
    OBJ_MEMBER_INT( new_event_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;

    RETURN->v_object = new_event_obj;
}

// No longer compiling ulib_opsc.cpp | REFACTOR-2017
#ifdef __PLATFORM_WINDOWS__
#undef ULIB_OPSC_CPP
#endif
