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
#ifdef __PLATFORM_WIN32__
#define ULIB_OPSC_CPP 
#endif

#include "lo/lo.h"

#include "ulib_opsc.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_dl.h"
#include "util_opsc.h"
#include "chuck_instr.h"
#include "util_thread.h"
#include "util_buffers.h"
#include "util_string.h"


#if _MSC_VER
#define snprintf _snprintf
#endif


const std::string CK_OSC_TYPETAG_CHARS = "ifs";

struct OscMsg
{
    std::string path;
    std::string type;
    
    struct OscArg
    {
        OscArg() : i(0), f(0), s() { }
        OscArg(int _i) : i(_i), f(0), s() { }
        OscArg(float _f) : i(0), f(_f), s() { }
        OscArg(const std::string &_s) : i(0), f(0), s(_s) { }
        
        int i;
        float f;
        std::string s;
    };
    
    std::vector<OscArg> args;
};


// TODO: use this to cache ck objects to not leak memory so rampantly
struct CkOscMsg
{
    OscMsg msg;
    
    Chuck_String *path;
    Chuck_String *type;
    std::vector<Chuck_Object> args;
};


class OscIn;

class OscInServer
{
public:
    
    static OscInServer *forPort(int port)
    {
        if(s_oscIn.count(port) == 0)
        {
            s_oscIn[port] = new OscInServer(port);
        }
        
        return s_oscIn[port];
    }
    
    static void shutdownAll()
    {
        
    }
    
    void addMethod(const std::string &method, OscIn * obj)
    {
        OscInMsg msg;
        
        msg.msg_type = OscInMsg::ADD_METHOD;
        methodToPathAndType(method, msg.path, msg.nopath, msg.type, msg.notype);
        msg.obj = obj;
        
        m_inMsgBuffer.put(msg);
    }
    
    void removeMethod(const std::string &method, OscIn * obj)
    {
        OscInMsg msg;
        
        msg.msg_type = OscInMsg::REMOVE_METHOD;
        methodToPathAndType(method, msg.path, msg.nopath, msg.type, msg.notype);
        msg.obj = obj;
        
        m_inMsgBuffer.put(msg);
    }
    
    void removeAllMethods(OscIn * obj)
    {
        OscInMsg msg;
        
        msg.msg_type = OscInMsg::REMOVEALL_METHODS;
        msg.obj = obj;
        
        m_inMsgBuffer.put(msg);
    }
    
    int port()
    {
        return m_assignedPort;
    }
    
private:
    
    static std::map<int, OscInServer *> s_oscIn;
    
#ifdef WIN32
    static unsigned int __stdcall s_server_cb(void *data)
#else
    static void *s_server_cb(void *data)
#endif // WIN32
    {
        OscInServer * _this = (OscInServer *) data;

#ifdef WIN32
        return (int) _this->server_cb();
#else
        return _this->server_cb();
#endif
    }
    
    static void s_err_handler(int num, const char *msg, const char *where)
    {
        if(msg)
            EM_error3("OscIn: error: %s", msg);
        else
            EM_error3("OscIn: unknown error");
    }
    
    static void methodToPathAndType(const std::string &method,
                                    std::string &path, t_CKBOOL &nopath,
                                    std::string &type, t_CKBOOL &notype)
    {
        int comma_pos = method.find(',');
        
        if(comma_pos != method.npos)
        {
            path = ltrim(rtrim(method.substr(0, comma_pos)));
            nopath = FALSE;
            
            std::string dirty_type = method.substr(comma_pos+1);
            type.clear();
            for(int i = 0; i < dirty_type.size(); i++)
                if(CK_OSC_TYPETAG_CHARS.find(dirty_type[i]) != std::string::npos)
                    type.push_back(dirty_type[i]);
            notype = FALSE;
        }
        else
        {
            path = ltrim(rtrim(method));
            nopath = (path.size() == 0);
            notype = TRUE;
        }
    }
    
    struct OscInMsg
    {
        enum Type
        {
            ADD_METHOD,
            REMOVE_METHOD,
            REMOVEALL_METHODS,
        };
        
        Type msg_type;
        std::string path;
        t_CKBOOL nopath;
        std::string type;
        t_CKBOOL notype;
        OscIn * obj;
    };
    
    OscInServer(int port) :
    m_port(port),
    m_inMsgBuffer(CircularBuffer<OscInMsg>(1024)),
    m_thread(XThread()),
    m_quit(false),
    m_assignedPort(-1)
    {
        m_thread.start(s_server_cb, this);
    }
    
    void *server_cb();
    
    XThread m_thread;
    bool m_quit;
    const int m_port;
    int m_assignedPort;
    CircularBuffer<OscInMsg> m_inMsgBuffer;
    
    struct Method
    {
        std::string path;
        t_CKBOOL nopath;
        std::string type;
        t_CKBOOL notype;
        
        bool operator==(const Method &m) const
        {
            return path == m.path && nopath == m.nopath && type == m.type && notype == m.notype;
        }
    };
    
    std::map<OscIn *, std::list<Method> > m_methods;
};

std::map<int, OscInServer *> OscInServer::s_oscIn;



class OscIn
{
public:
    OscIn(Chuck_Event * event, Chuck_VM * vm) :
    m_event(event),
    m_vm(vm),
    m_port(-1),
    m_oscMsgBuffer(CircularBuffer<OscMsg>(1024))
    {
        m_eventBuffer = m_vm->create_event_buffer();
    }
    
    ~OscIn()
    {
        removeAllMethods();
        
        CBufferSimple * t_eventBuffer = m_eventBuffer;
        m_eventBuffer = NULL;
        m_vm->destroy_event_buffer(t_eventBuffer);
        
        m_vm = NULL;
        m_event = NULL;
    }
    
    
    void addMethod(const std::string &method) { OscInServer::forPort(m_port)->addMethod(method, this); }
    void removeMethod(const std::string &method) { OscInServer::forPort(m_port)->removeMethod(method, this); }
    void removeAllMethods() { OscInServer::forPort(m_port)->removeAllMethods(this); }
    
    t_CKBOOL get(OscMsg &msg)
    {
        return m_oscMsgBuffer.get(msg);
    }
    
    static int s_handler(const char *path, const char *types,
                         lo_arg **argv, int argc,
                         lo_message msg, void *user_data)
    {
        OscIn * _this = (OscIn *) user_data;
        return _this->handler(path, types, argv, argc, msg);
    }
    
    int port(int port) { m_port = port; return m_port; }
    int port() { return OscInServer::forPort(m_port)->port(); }
    
private:
    
    Chuck_VM * m_vm;
    Chuck_Event * m_event;
    int m_port;
    CBufferSimple * m_eventBuffer;
    CircularBuffer<OscMsg> m_oscMsgBuffer;
    
    int handler(const char *path, const char *types,
                lo_arg **argv, int argc, lo_message _msg)
    {
        OscMsg msg;
        msg.path = path;
        msg.type = types;
        
        for(int i = 0; i < argc; i++)
        {
            OscMsg::OscArg arg;
            
            switch(types[i])
            {
                case 'i': arg.i = argv[i]->i; break;
                case 'f': arg.f = argv[i]->f; break;
                case 's': arg.s = &(argv[i]->s); break;
                default:
                    EM_error3("OscIn: error: unhandled OSC type '%c'", types[i]);
            }
            
            msg.args.push_back(arg);
        }
        
        m_oscMsgBuffer.put(msg);
        m_vm->queue_event(m_event, 1, m_eventBuffer);
        
        return -1;
    }
};


void *OscInServer::server_cb()
{
    lo_server m_server = NULL;
    
    if(m_port > 0)
    {
        char portStr[32];
        snprintf(portStr, 32, "%d", m_port);
        m_server = lo_server_new(portStr, s_err_handler);
    }
    else
    {
        m_server = lo_server_new(NULL, s_err_handler);
    }
    
    if(m_server == NULL)
    {
        EM_error3("OscIn: error: unable to create server instance");
        return NULL;
    }
    
    m_assignedPort = lo_server_get_port(m_server);
    
    EM_log(CK_LOG_INFO, "OscIn: starting OSC server at port '%d'", m_port);
    
    while(!m_quit)
    {
        OscInMsg msg;
        while(m_inMsgBuffer.get(msg))
        {
            switch(msg.msg_type)
            {
                case OscInMsg::ADD_METHOD:
                {
                    if(lo_server_add_method(m_server,
                                            msg.nopath ? NULL : msg.path.c_str(),
                                            msg.notype ? NULL : msg.type.c_str(),
                                            OscIn::s_handler, msg.obj))
                    {
                        if(!m_methods.count(msg.obj))
                            m_methods[msg.obj] = std::list<Method>();
                        
                        Method m;
                        m.path = msg.path;
                        m.nopath = msg.nopath;
                        m.type = msg.type;
                        m.notype = msg.notype;
                        m_methods[msg.obj].push_back(m);
                    }
                    else
                    {
                        EM_error3("OscIn: error: add_method failed for %s, %s", msg.path.c_str(), msg.type.c_str());
                    }
                }
                    break;
                    
                case OscInMsg::REMOVE_METHOD:
                {
                    lo_server_del_method(m_server,
                                         msg.nopath ? NULL : msg.path.c_str(),
                                         msg.notype ? NULL : msg.type.c_str());
                    if(m_methods.count(msg.obj))
                    {
                        Method m;
                        m.path = msg.path;
                        m.nopath = msg.nopath;
                        m.type = msg.type;
                        m.notype = msg.notype;
                        m_methods[msg.obj].remove(m);
                    }
                }
                    break;
                    
                case OscInMsg::REMOVEALL_METHODS:
                    if(m_methods.count(msg.obj))
                    {
                        for(std::list<Method>::iterator i = m_methods[msg.obj].begin();
                            i != m_methods[msg.obj].end(); i++)
                        {
                            lo_server_del_method(m_server,
                                                 i->nopath ? NULL : i->path.c_str(),
                                                 i->notype ? NULL : i->type.c_str());
                        }
                        
                        m_methods[msg.obj].clear();
                        m_methods.erase(msg.obj);
                    }
                    break;
            }
        }
        
        lo_server_recv_noblock(m_server, 2);
    }
    
    lo_server_free(m_server);
    
    EM_log(CK_LOG_INFO, "OscIn: shutting down OSC server", m_port);
    
    return NULL;
}

class OscOut
{
public:
    OscOut()
    {
        m_address = NULL;
        m_message = NULL;
    }
    
    ~OscOut()
    {
        if(m_address != NULL)
        {
            lo_address_free(m_address);
            m_address = NULL;
        }
        
        if(m_message != NULL)
        {
            lo_message_free(m_message);
            m_message = NULL;
        }
    }
    
    t_CKBOOL setDestination(const std::string &host, int port)
    {
        if(m_address != NULL)
        {
            lo_address_free(m_address);
            m_address = NULL;
        }
        
        char portStr[32];
        snprintf(portStr, 32, "%d", port);
        
        m_address = lo_address_new(host.c_str(), portStr);
        
        if(m_address == NULL)
        {
            const char *msg = lo_address_errstr(NULL);
            EM_error3("OscOut: error: failed to set destination address '%s:%d'%s%s",
                      host.c_str(), port, msg?": ":"", msg?msg:"");
            return FALSE;
        }
        
        return TRUE;
    }
    
    t_CKBOOL start(const std::string &path, const std::string &arg)
    {
        return start(path + "," + arg);
    }
    
    t_CKBOOL start(const std::string &method)
    {
        std::string path;
        int comma_pos = method.find(',');
        if(comma_pos != method.npos) m_path = method.substr(0, comma_pos);
        else m_path = method;
        
        m_message = lo_message_new();
        
        return TRUE;
    }
    
    t_CKBOOL add(t_CKINT i)
    {
        if(m_message == NULL)
        {
            EM_error3("OscOut: error: attempt to add argument to message with no OSC address");
            return FALSE;
        }
        
        lo_message_add_int32(m_message, (int32_t)i);
        
        return TRUE;
    }
    
    t_CKBOOL add(t_CKFLOAT f)
    {
        if(m_message == NULL)
        {
            EM_error3("OscOut: error: attempt to add argument to message with no OSC address");
            return FALSE;
        }
        
        lo_message_add_float(m_message, (float)f);
        
        return TRUE;
    }
    
    t_CKBOOL add(const std::string &s)
    {
        if(m_message == NULL)
        {
            EM_error3("OscOut: error: attempt to add argument to message with no OSC address");
            return FALSE;
        }
        
        lo_message_add_string(m_message, s.c_str());
        
        return TRUE;
    }
    
    t_CKBOOL send()
    {
        if(m_message == NULL || m_address == NULL || m_path.size() == 0)
        {
            EM_error3("OscOut: error: attempt to send message with no destination or OSC address");
            return FALSE;
        }
        
        int result = lo_send_message(m_address, m_path.c_str(), m_message);
        
        lo_message_free(m_message);
        m_message = NULL;
        
        if(result == -1)
        {
            const char *msg = lo_address_errstr(m_address);
            EM_error3("OscOut: error: sending OSC message%s%s",
                      msg?": ":"", msg?msg:"");
            return FALSE;
        }
        
        return TRUE;
    }
    
private:
    lo_address m_address;
    std::string m_path;
    lo_message m_message;
};


static t_CKUINT oscin_offset_data = 0;

static t_CKUINT oscout_offset_data = 0;

static t_CKUINT oscmsg_offset_address = 0;
static t_CKUINT oscmsg_offset_typetag = 0;
static t_CKUINT oscmsg_offset_args = 0;

static t_CKUINT oscarg_offset_type = 0;
static t_CKUINT oscarg_offset_i = 0;
static t_CKUINT oscarg_offset_f = 0;
static t_CKUINT oscarg_offset_s = 0;

Chuck_Type * g_OscMsgType = NULL;
Chuck_Type * g_OscArgType = NULL;

//-----------------------------------------------------------------------------
// name: OscOut
// desc:
//-----------------------------------------------------------------------------
#pragma mark - OscOut

CK_DLL_CTOR(oscout_ctor)
{
    OBJ_MEMBER_INT(SELF, oscout_offset_data) = (t_CKINT) new OscOut;
}

CK_DLL_DTOR(oscout_dtor)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    SAFE_DELETE(out);
    OBJ_MEMBER_INT(SELF, oscout_offset_data) = 0;
}

CK_DLL_MFUN(oscout_dest)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    Chuck_String *host = GET_NEXT_STRING(ARGS);
    t_CKINT port = GET_NEXT_INT(ARGS);
    
    if(host == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscOut.start: argument 'host' is null");
        goto error;
    }
    
    if(!out->setDestination(host->str(), port))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_start)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    Chuck_String *method = GET_NEXT_STRING(ARGS);
    
    if(method == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscOut.start: argument 'method' is null");
        goto error;
    }
    
    if(!out->start(method->str()))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_startDest)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    Chuck_String *method = GET_NEXT_STRING(ARGS);
    Chuck_String *host = GET_NEXT_STRING(ARGS);
    t_CKINT port = GET_NEXT_INT(ARGS);
    
    if(method == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscOut.start: argument 'method' is null");
        goto error;
    }
    
    if(host == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscOut.start: argument 'host' is null");
        goto error;
    }
    
    if(!out->setDestination(host->str(), port))
        goto error;
    
    if(!out->start(method->str()))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_addInt)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    t_CKINT i = GET_NEXT_INT(ARGS);
    
    if(!out->add(i))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_addFloat)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    
    if(!out->add(f))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_addString)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    Chuck_String *s = GET_NEXT_STRING(ARGS);
    
    if(!out->add(s->str()))
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}

CK_DLL_MFUN(oscout_send)
{
    OscOut * out = (OscOut *) OBJ_MEMBER_INT(SELF, oscout_offset_data);
    
    if(!out->send())
        goto error;
    
    RETURN->v_object = SELF;
    
    return;
    
error:
    RETURN->v_object = NULL;
}


//-----------------------------------------------------------------------------
// name: OscArg
// desc:
//-----------------------------------------------------------------------------
#pragma mark - OscArg

CK_DLL_CTOR(oscarg_ctor)
{
    Chuck_String *type = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    SAFE_ADD_REF(type);
    OBJ_MEMBER_STRING(SELF, oscarg_offset_type) = type;
    
    OBJ_MEMBER_INT(SELF, oscarg_offset_i) = 0;
    
    OBJ_MEMBER_FLOAT(SELF, oscarg_offset_f) = 0.0;
    
    Chuck_String *s = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    SAFE_ADD_REF(s);
    OBJ_MEMBER_STRING(SELF, oscarg_offset_s) = s;
}

CK_DLL_DTOR(oscarg_dtor)
{
    SAFE_RELEASE(OBJ_MEMBER_STRING(SELF, oscarg_offset_type));
    OBJ_MEMBER_STRING(SELF, oscarg_offset_type) = NULL;
    
    SAFE_RELEASE(OBJ_MEMBER_STRING(SELF, oscarg_offset_s));
    OBJ_MEMBER_STRING(SELF, oscarg_offset_s) = NULL;
}



//-----------------------------------------------------------------------------
// name: OscIn
// desc:
//-----------------------------------------------------------------------------
#pragma mark - OscIn

CK_DLL_CTOR(oscin_ctor)
{
    OBJ_MEMBER_INT(SELF, oscin_offset_data) = (t_CKINT) new OscIn((Chuck_Event *) SELF, SHRED->vm_ref);
}

CK_DLL_DTOR(oscin_dtor)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    SAFE_DELETE(in);
    OBJ_MEMBER_INT(SELF, oscin_offset_data) = 0;
}

CK_DLL_MFUN(oscin_setport)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    t_CKINT port = GET_NEXT_INT(ARGS);
    
    RETURN->v_int = in->port(port);
}

CK_DLL_MFUN(oscin_getport)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    RETURN->v_int = in->port();
}

CK_DLL_MFUN(oscin_addAddress)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    Chuck_String *address = GET_NEXT_STRING(ARGS);
    
    if(address == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscIn.address: argument 'address' is null");
        goto error;
    }

    in->addMethod(address->str());
    
error:
    return;
}

CK_DLL_MFUN(oscin_removeAddress)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    Chuck_String *address = GET_NEXT_STRING(ARGS);
    
    if(address == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscIn.address: argument 'address' is null");
        goto error;
    }
    
    in->removeMethod(address->str());
    
error:
    return;
}

CK_DLL_MFUN(oscin_removeAllAddresses)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    in->removeAllMethods();
    
error:
    return;
}

CK_DLL_MFUN(oscin_listenAll)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    in->addMethod("");
    
error:
    return;
}

CK_DLL_MFUN(oscin_msg)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    in->removeAllMethods();
    
error:
    return;
}

CK_DLL_MFUN(oscin_recv)
{
    OscIn * in = (OscIn *) OBJ_MEMBER_INT(SELF, oscin_offset_data);
    
    Chuck_Object * msg_obj = GET_NEXT_OBJECT(ARGS);
    OscMsg msg;
    
    Chuck_Array4 *args_obj = NULL;
    Chuck_Type * oscarg_type = NULL;

    int i;
    
    if(msg_obj == NULL)
    {
        throw_exception(SHRED, "NullPtrException", "OscIn.recv: argument 'msg' is null");
        goto error;
    }
    
    RETURN->v_int = in->get(msg);
    
    OBJ_MEMBER_STRING(msg_obj, oscmsg_offset_address)->set( msg.path );
    OBJ_MEMBER_STRING(msg_obj, oscmsg_offset_typetag)->set( msg.type );
    
    args_obj = (Chuck_Array4 *) OBJ_MEMBER_OBJECT(msg_obj, oscmsg_offset_args);
    args_obj->clear();
    
    oscarg_type = type_engine_find_type( SHRED->vm_ref->env(), str2list("OscArg") );
    
    for(i = 0; i < msg.args.size(); i++)
    {
        Chuck_Object * arg_obj = instantiate_and_initialize_object(oscarg_type, SHRED);
        // HACK: manually call osc_arg ctor
        oscarg_ctor(arg_obj, NULL, SHRED, API);
        
        switch(msg.type[i])
        {
            case 'i':
                OBJ_MEMBER_STRING(arg_obj, oscarg_offset_type)->set( "i" );
                OBJ_MEMBER_INT(arg_obj, oscarg_offset_i) = msg.args[i].i;
                break;
            case 'f':
                OBJ_MEMBER_STRING(arg_obj, oscarg_offset_type)->set( "f" );
                OBJ_MEMBER_FLOAT(arg_obj, oscarg_offset_f) = msg.args[i].f;
                break;
            case 's':
                OBJ_MEMBER_STRING(arg_obj, oscarg_offset_type)->set( "s" );
                OBJ_MEMBER_STRING(arg_obj, oscarg_offset_s)->set( msg.args[i].s );
                break;
        }
        
        args_obj->push_back((t_CKINT) arg_obj);
    }
    
    return;
    
error:
    RETURN->v_int = 0;
}


//-----------------------------------------------------------------------------
// name: OscMsg
// desc:
//-----------------------------------------------------------------------------
#pragma mark - OscMsg

CK_DLL_CTOR(oscmsg_ctor)
{
    Chuck_String *address = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    SAFE_ADD_REF(address);
    OBJ_MEMBER_STRING(SELF, oscmsg_offset_address) = address;
    
    Chuck_String *typetag = (Chuck_String *) instantiate_and_initialize_object(SHRED->vm_ref->env()->t_string, SHRED);
    SAFE_ADD_REF(typetag);
    OBJ_MEMBER_STRING(SELF, oscmsg_offset_typetag) = typetag;
    
    Chuck_Array4 *args = new Chuck_Array4(TRUE);
    initialize_object(args, SHRED->vm_ref->env()->t_array);
    args->clear();
    SAFE_ADD_REF(args);
    OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args) = args;
}

CK_DLL_DTOR(oscmsg_dtor)
{
    SAFE_RELEASE(OBJ_MEMBER_STRING(SELF, oscmsg_offset_address));
    OBJ_MEMBER_STRING(SELF, oscmsg_offset_address) = NULL;
    
    SAFE_RELEASE(OBJ_MEMBER_STRING(SELF, oscmsg_offset_typetag));
    OBJ_MEMBER_STRING(SELF, oscmsg_offset_typetag) = NULL;
    
    SAFE_RELEASE(OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args));
    OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args) = NULL;
}

CK_DLL_MFUN(oscmsg_numArgs)
{
    Chuck_Array4 *args_obj = (Chuck_Array4 *) OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args);
    RETURN->v_int = args_obj->size();
}

CK_DLL_MFUN(oscmsg_getInt)
{
    int i = GET_NEXT_INT(ARGS);
    Chuck_Array4 *args_obj = (Chuck_Array4 *) OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args);
    Chuck_Object *arg_obj;
    
    // check bounds
    if(i >= 0 && i < args_obj->size())
    {
        args_obj->get(i, (t_CKUINT *) &arg_obj);
        RETURN->v_int = OBJ_MEMBER_INT(arg_obj, oscarg_offset_i);
    }
    else
    {
        RETURN->v_int = 0;
    }
}

CK_DLL_MFUN(oscmsg_getFloat)
{
    int i = GET_NEXT_INT(ARGS);
    Chuck_Array4 *args_obj = (Chuck_Array4 *) OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args);
    Chuck_Object *arg_obj;
    if(i >= 0 && i < args_obj->size())
    {
        args_obj->get(i, (t_CKUINT *) &arg_obj);
        RETURN->v_float = OBJ_MEMBER_FLOAT(arg_obj, oscarg_offset_f);
    }
    else
        RETURN->v_float = 0;
}

CK_DLL_MFUN(oscmsg_getString)
{
    int i = GET_NEXT_INT(ARGS);
    Chuck_Array4 *args_obj = (Chuck_Array4 *) OBJ_MEMBER_OBJECT(SELF, oscmsg_offset_args);
    Chuck_Object *arg_obj;
    
    if(i >= 0 && i < args_obj->size())
    {
        args_obj->get(i, (t_CKUINT *) &arg_obj);
        RETURN->v_string = OBJ_MEMBER_STRING(arg_obj, oscarg_offset_s);
    }
    else
        RETURN->v_string = NULL;
}

static t_CKUINT osc_send_offset_data = 0;
static t_CKUINT osc_recv_offset_data = 0;
static t_CKUINT osc_address_offset_data = 0;
static Chuck_Type * osc_addr_type_ptr = 0;


DLL_QUERY opensoundcontrol_query ( Chuck_DL_Query * query ) { 

    /*** OscOut ***/
    
    query->begin_class(query, "OscOut", "Object");
    query->doc_class(query, "Class for sending OSC messages. ");
    
    oscout_offset_data = query->add_mvar(query, "int", "@OscOut_data", FALSE);
    
    query->add_ctor(query, oscout_ctor);
    query->add_dtor(query, oscout_dtor);
    
    query->add_mfun(query, oscout_dest, "OscOut", "dest");
    query->add_arg(query, "string", "host");
    query->add_arg(query, "int", "port");
    
    query->add_mfun(query, oscout_start, "OscOut", "start");
    query->add_arg(query, "string", "method");
    
    query->add_mfun(query, oscout_startDest, "OscOut", "start");
    query->add_arg(query, "string", "method");
    query->add_arg(query, "string", "host");
    query->add_arg(query, "int", "port");

    query->add_mfun(query, oscout_addInt, "OscOut", "add");
    query->add_arg(query, "int", "i");
    
    query->add_mfun(query, oscout_addFloat, "OscOut", "add");
    query->add_arg(query, "float", "f");
    
    query->add_mfun(query, oscout_addString, "OscOut", "add");
    query->add_arg(query, "string", "s");
    
    query->add_mfun(query, oscout_send, "OscOut", "send");
    
    query->end_class(query);
    
    
    /*** OscArg ***/
    
    query->begin_class(query, "OscArg", "Object");
    
    oscarg_offset_type = query->add_mvar(query, "string", "type", FALSE);
    oscarg_offset_i = query->add_mvar(query, "int", "i", FALSE);
    oscarg_offset_f = query->add_mvar(query, "float", "f", FALSE);
    oscarg_offset_s = query->add_mvar(query, "string", "s", FALSE);
    
    query->add_ctor(query, oscarg_ctor);
    query->add_dtor(query, oscarg_dtor);
    
    query->end_class(query);
    
    
    /*** OscMsg ***/
    
    query->begin_class(query, "OscMsg", "Object");
    
    oscmsg_offset_address = query->add_mvar(query, "string", "address", FALSE);
    oscmsg_offset_typetag = query->add_mvar(query, "string", "typetag", FALSE);
    oscmsg_offset_args = query->add_mvar(query, "OscArg[]", "args", FALSE);
    
    query->add_mfun(query, oscmsg_numArgs, "int", "numArgs");
    
    query->add_mfun(query, oscmsg_getInt, "int", "getInt");
    query->add_arg(query, "int", "i");

    query->add_mfun(query, oscmsg_getFloat, "float", "getFloat");
    query->add_arg(query, "int", "i");
    
    query->add_mfun(query, oscmsg_getString, "string", "getString");
    query->add_arg(query, "int", "i");
    
    query->add_ctor(query, oscmsg_ctor);
    query->add_dtor(query, oscmsg_dtor);
    
    query->end_class(query);
    
    
    /*** OscIn ***/
    
    query->begin_class(query, "OscIn", "Event");
    
    oscin_offset_data = query->add_mvar(query, "int", "@OscOut_data", FALSE);

    query->add_ctor(query, oscin_ctor);
    query->add_dtor(query, oscin_dtor);
    
    query->add_mfun(query, oscin_getport, "int", "port");
    
    query->add_mfun(query, oscin_setport, "int", "port");
    query->add_arg(query, "int", "p");
    
    query->add_mfun(query, oscin_addAddress, "void", "addAddress");
    query->add_arg(query, "string", "address");
    
    query->add_mfun(query, oscin_removeAddress, "void", "removeAddress");
    query->add_arg(query, "string", "address");
    
    query->add_mfun(query, oscin_removeAllAddresses, "void", "removeAllAddresses");
    
    query->add_mfun(query, oscin_listenAll, "void", "listenAll");

    query->add_mfun(query, oscin_recv, "int", "recv");
    query->add_arg(query, "OscMsg", "msg");

    query->end_class(query);
    
    
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
    func->add_arg( "string" , "addr" );
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
    func->add_arg( "int" , "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "listen", osc_recv_listen_port );
    func->add_arg( "int" , "port" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "listen", osc_recv_listen );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "stop", osc_recv_listen_stop );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "add_address", osc_recv_add_address );
    func->add_arg( "OscEvent" , "addr" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "remove_address", osc_recv_remove_address );
    func->add_arg( "OscEvent" , "addr" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "event", osc_recv_new_address );
    func->add_arg( "string" , "spec" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "event", osc_recv_new_address_type );
    func->add_arg( "string" , "address" );
    func->add_arg( "string" , "type" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "address", osc_recv_new_address );
    func->add_arg( "string" , "spec" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "OscEvent", "address", osc_recv_new_address_type );
    func->add_arg( "string" , "address" );
    func->add_arg( "string" , "type" );
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
    OSC_Transmitter* xmit = new OSC_Transmitter();
    OBJ_MEMBER_INT( SELF, osc_send_offset_data ) = (t_CKINT)xmit;
}
 
//----------------------------------------------
// name :  osc_send_dtor 
// desc : DTOR function 
//-----------------------------------------------
CK_DLL_DTOR( osc_send_dtor ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    SAFE_DELETE(xmit);
    OBJ_MEMBER_UINT(SELF, osc_send_offset_data) = 0;
}

//----------------------------------------------
// name :  osc_send_setHost 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_setHost ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    Chuck_String* host = GET_NEXT_STRING(ARGS);
    t_CKINT port = GET_NEXT_INT(ARGS);
    xmit->setHost( (char*) host->str().c_str(), port );
}

//----------------------------------------------
// name :  osc_send_startMesg 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_startMesg ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    Chuck_String* address = GET_NEXT_STRING(ARGS);
    Chuck_String* args = GET_NEXT_STRING(ARGS);
    xmit->startMessage( (char*) address->str().c_str(), (char*) args->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_startMesg 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_startMesg_spec ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    Chuck_String* spec = GET_NEXT_STRING(ARGS);
    xmit->startMessage( (char*) spec->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_addInt 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addInt ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    xmit->addInt( GET_NEXT_INT(ARGS) );
}

//----------------------------------------------
// name :  osc_send_addFloat 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addFloat ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    xmit->addFloat( (float)(GET_NEXT_FLOAT(ARGS)) );
}

//----------------------------------------------
// name :  osc_send_addString 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_addString ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    xmit->addString( (char*)(GET_NEXT_STRING(ARGS))->str().c_str() );
}

//----------------------------------------------
// name :  osc_send_openBundle 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_openBundle ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    // we should add in an option to translate from chuck-time to current time to timetag.
    // but for now let's just spec. immediately.
    xmit->openBundle(OSCTT_Immediately());
}

//----------------------------------------------
// name :  osc_send_closeBundle 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_closeBundle ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    xmit->closeBundle();
}

//----------------------------------------------
// name :  osc_send_holdMesg 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_holdMesg ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
    xmit->holdMessage( GET_NEXT_INT(ARGS) != 0 );
}

//----------------------------------------------
// name :  osc_send_kickMesg 
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_send_kickMesg ) { 
    OSC_Transmitter* xmit = (OSC_Transmitter *)OBJ_MEMBER_INT(SELF, osc_send_offset_data);
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
    OBJ_MEMBER_INT(SELF, osc_address_offset_data) = (t_CKINT)addr;
}

CK_DLL_DTOR( osc_address_dtor ) {
    delete (OSC_Address_Space *)OBJ_MEMBER_UINT(SELF, osc_address_offset_data);
    OBJ_MEMBER_UINT(SELF, osc_address_offset_data) = 0;
}

CK_DLL_MFUN( osc_address_set ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    addr->setSpec ( (char*)(GET_NEXT_STRING(ARGS))->str().c_str() );
    RETURN->v_int = 0;
}

//----------------------------------------------
// name : osc_address_can_wait   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_can_wait  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = ( addr->has_mesg() ) ? 0 : 1;
}
   
//----------------------------------------------
// name : osc_address_has_mesg   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_has_mesg  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = ( addr->has_mesg() ) ? 1 : 0 ;
}

//----------------------------------------------
// name : osc_address_next_mesg   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_mesg  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = ( addr->next_mesg() ) ? 1 : 0 ;
}

//----------------------------------------------
// name : osc_address_next_int   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_int  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_int = addr->next_int();
}

//----------------------------------------------
// name : osc_address_next_float   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_float  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    RETURN->v_float = addr->next_float();
}

//----------------------------------------------
// name : osc_address_next_string   
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_address_next_string  ) { 
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( SELF, osc_address_offset_data );
    char * cs = addr->next_string();
    Chuck_String * ckstr = ( cs ) ? new Chuck_String( cs ) : new Chuck_String("");
    initialize_object( ckstr, SHRED->vm_ref->env()->t_string );
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
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    SAFE_DELETE(recv);
    OBJ_MEMBER_INT(SELF, osc_recv_offset_data) = 0;
}

//----------------------------------------------
// name : osc_recv_port  
// desc : specify port to listen on
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_port )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    recv->bind_to_port( (int)GET_NEXT_INT(ARGS) );
}

// need to add a listen function in Receiver which opens up a recv loop on another thread.
// address then subscribe to a receiver to take in events. 

//----------------------------------------------
// name : osc_recv_listen  
// desc : start listening
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    recv->listen();
}

//----------------------------------------------
// name : osc_recv_listen_port  
// desc : listen to a given port ( disconnects from current )
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen_port )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    recv->listen((int)GET_NEXT_INT(ARGS));
}

//----------------------------------------------
// name : osc_recv_listen_port  
// desc : listen to a given port ( disconnects from current )
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_listen_stop )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
	recv->stopListening();
}

//----------------------------------------------
// name : osc_recv_add_listener  
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_add_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    Chuck_Object* addr_obj = GET_NEXT_OBJECT(ARGS); //address object class...
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( addr_obj, osc_address_offset_data ); 
    recv->add_address( addr );
}

//----------------------------------------------
// name : osc_recv_remove_listener  
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_remove_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    Chuck_Object* addr_obj = GET_NEXT_OBJECT(ARGS); //listener object class...
    OSC_Address_Space * addr = (OSC_Address_Space *)OBJ_MEMBER_INT( addr_obj, osc_address_offset_data ); 
    recv->remove_address( addr );
}

//----------------------------------------------
// name : osc_recv_new_address  
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_new_address )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    Chuck_String * spec_obj = (Chuck_String*)GET_NEXT_STRING(ARGS); // listener object class...
    
    // added 1.3.1.1: fix potential race condition
    //OSC_Address_Space * new_addr_obj = recv->new_event( (char*)spec_obj->str.c_str() );
    OSC_Address_Space * new_addr_obj = new OSC_Address_Space( (char*)spec_obj->str().c_str() );

    /* wolf in sheep's clothing
    initialize_object( new_addr_obj , osc_addr_type_ptr ); //initialize in vm
    new_addr_obj->SELF = new_addr_obj;
    OBJ_MEMBER_INT( new_addr_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;
    */

    // more correct...?
    Chuck_Event* new_event_obj = new Chuck_Event();
    initialize_object( new_event_obj, osc_addr_type_ptr );
    new_addr_obj->SELF = new_event_obj;
    OBJ_MEMBER_INT( new_event_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;
    
    recv->add_address(new_addr_obj);

    RETURN->v_object = new_event_obj;
}



//----------------------------------------------
// name : osc_recv_new_address  
// desc : MFUN function 
//-----------------------------------------------
CK_DLL_MFUN( osc_recv_new_address_type )
{
    OSC_Receiver * recv = (OSC_Receiver *)OBJ_MEMBER_INT(SELF, osc_recv_offset_data);
    Chuck_String * addr_obj = (Chuck_String*)GET_NEXT_STRING(ARGS); //listener object class...
    Chuck_String * type_obj = (Chuck_String*)GET_NEXT_STRING(ARGS); //listener object class...
    OSC_Address_Space * new_addr_obj = recv->new_event( (char*)addr_obj->str().c_str(), (char*)type_obj->str().c_str() );

    /* wolf in sheep's clothing
    initialize_object( new_addr_obj , osc_addr_type_ptr ); //initialize in vm
    new_addr_obj->SELF = new_addr_obj;
    OBJ_MEMBER_INT( new_addr_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;
    */

    // more correct...?
    Chuck_Event* new_event_obj = new Chuck_Event();
    initialize_object( new_event_obj, osc_addr_type_ptr );
    new_addr_obj->SELF = new_event_obj;
    OBJ_MEMBER_INT( new_event_obj, osc_address_offset_data ) = (t_CKINT)new_addr_obj;

    RETURN->v_object = new_event_obj;
}

// No longer compiling ulib_opsc.cpp | REFACTOR-2017
#ifdef __PLATFORM_WIN32__
#undef ULIB_OPSC_CPP
#endif
