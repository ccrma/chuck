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
// file: hidio_sdl.cpp
// desc: HID io over SDL header
//
// author: Spencer Salazar (ssalazar@cs.princeton.edu)
//         Ge Wang (gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Summer 2005
//-----------------------------------------------------------------------------
#include "hidio_sdl.h"
#include "util_hid.h"
#include "chuck_errmsg.h"
#include "chuck_vm.h"

#ifndef __PLATFORM_WIN32__
#include <unistd.h>
#else
#include "chuck_def.h"
#endif

#include <limits.h>
#include <vector>
#include <map>
using namespace std;

Chuck_Hid_Driver * default_drivers = NULL;

struct PhyHidDevIn
{
public:
    PhyHidDevIn();
    ~PhyHidDevIn();
    t_CKBOOL open( Chuck_VM * vm, t_CKINT type, t_CKUINT number );
    t_CKBOOL read( t_CKINT element_type, t_CKINT element_num, HidMsg * msg );
    t_CKBOOL send( const HidMsg * msg );
    t_CKBOOL close();
    std::string name();
    
    t_CKBOOL query_element( HidMsg * query );

    t_CKBOOL register_client( HidIn * client );
    t_CKBOOL unregister_client( HidIn * client );

public:
    CBufferAdvance * cbuf;

protected:    
    t_CKINT device_type;
    t_CKUINT device_num;
    
    std::vector< HidIn * > clients;
};

struct PhyHidDevOut
{
    // lies
};



//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
// per-physical device buffer size
#define BUFFER_SIZE 1024

std::vector< std::vector<PhyHidDevIn *> > HidInManager::the_matrix;
XThread * HidInManager::the_thread = NULL;
t_CKBOOL HidInManager::thread_going = FALSE;
t_CKBOOL HidInManager::has_init = FALSE;
CBufferSimple * HidInManager::msg_buffer = NULL;
std::vector<PhyHidDevOut *> HidOutManager::the_phouts;
std::map< Chuck_VM *, CBufferSimple * > HidInManager::m_event_buffers;

//-----------------------------------------------------------------------------
// name: PhyHidDevIn()
// desc: constructor
//-----------------------------------------------------------------------------
PhyHidDevIn::PhyHidDevIn()
{
    device_type = CK_HID_DEV_NONE;
    device_num = 0;
    cbuf = NULL;
}


//-----------------------------------------------------------------------------
// name: ~PhyHidDevIn()
// desc: destructor
//-----------------------------------------------------------------------------
PhyHidDevIn::~PhyHidDevIn()
{
    // check
    if( device_type != CK_HID_DEV_NONE )
        this->close();
}


//-----------------------------------------------------------------------------
// name: open()
// desc: opens the device of specified type and id
//-----------------------------------------------------------------------------
t_CKBOOL PhyHidDevIn::open( Chuck_VM * vm, t_CKINT type, t_CKUINT number )
{
    // int temp;

    // check
    if( device_type != CK_HID_DEV_NONE )
    {
        // log
        EM_log( CK_LOG_WARNING, "PhyHidDevIn: open() failed -> already open!" );
        return FALSE;
    }
    
    if( type <= CK_HID_DEV_NONE || type >= CK_HID_DEV_COUNT )
    {
        // log
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: open() failed -> invalid device type %d", 
                type );
        return FALSE;
    }
    
    if( !default_drivers[type].open )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: open() failed -> %s does not support open",
                default_drivers[type].driver_name );
        return FALSE;
    }
    
    if( default_drivers[type].open( ( int ) number ) )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: open() failed -> invalid %s number %d",
                default_drivers[type].driver_name,
                number );
        return FALSE;
    }
    
    // allocate the buffer
    cbuf = new CBufferAdvance;
    if( !cbuf->initialize( BUFFER_SIZE, sizeof(HidMsg), HidInManager::m_event_buffers[vm] ) )
    {
        // log
        EM_log( CK_LOG_WARNING, "PhyHidDevIn: open operation failed: cannot initialize buffer" );
        this->close();
        return FALSE;
    }

    device_type = type;
    device_num = number;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: read()
// desc: query specific elements of the hid device
//-----------------------------------------------------------------------------
t_CKBOOL PhyHidDevIn::read( t_CKINT element_type, t_CKINT element_num, HidMsg * msg )
{
    if( !default_drivers[device_type].read )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: read() failed -> %s does not support read",
                default_drivers[device_type].driver_name );
        return FALSE;
    }
    
    if( default_drivers[device_type].read( device_num, element_type, 
                                           element_num, msg ) )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: read() failed for %s %i, element type %i, element number %i",
                default_drivers[device_type].driver_name, device_num, 
                element_type, element_num );
        return FALSE;
    }
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: send()
// desc: send message to a HID device
//-----------------------------------------------------------------------------
t_CKBOOL PhyHidDevIn::send( const HidMsg * msg )
{
    if( !default_drivers[device_type].send )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: send() failed -> %s does not support send",
                default_drivers[device_type].driver_name );
        return FALSE;
    }
    
    if( default_drivers[device_type].send( device_num, msg ) )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: send() failed for %s %i",
                default_drivers[device_type].driver_name, device_num );
        return FALSE;
    }
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: close()
// desc: closes the device, deallocates all associated data
//-----------------------------------------------------------------------------
t_CKBOOL PhyHidDevIn::close()
{
    // check
    if( cbuf != NULL )
    {
        // delete
        SAFE_DELETE( cbuf );
        // TODO: release references from cbuf?
    }

    if( device_type <= CK_HID_DEV_NONE || device_type >= CK_HID_DEV_COUNT )
    {
        // log
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: close() failed -> invalid device type %d", 
                device_type );
        return FALSE;
    }
    
    if( !default_drivers[device_type].close )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: close() failed -> %s does not support close",
                default_drivers[device_type].driver_name );
        return FALSE;
    }
    
    if( default_drivers[device_type].close( ( int ) device_num ) )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: close() failed -> invalid %s number %d",
                default_drivers[device_type].driver_name,
                device_num );
        return FALSE;
    }
    
    device_type = CK_HID_DEV_NONE;
    device_num = 0;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: name()
// desc: retrieve device name
//-----------------------------------------------------------------------------
string PhyHidDevIn::name()
{
    if( device_type == CK_HID_DEV_NONE )
        return " ";
    
    if( device_type <= CK_HID_DEV_NONE || device_type >= CK_HID_DEV_COUNT )
    {
        // log
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: name() failed -> invalid device type %d", 
                device_type );
        return " ";
    }
    
    if( !default_drivers[device_type].name )
        return default_drivers[device_type].driver_name;
    
    const char * _name;
    
    if( !( _name = default_drivers[device_type].name( ( int ) device_num ) ) )
    {
        EM_log( CK_LOG_WARNING, 
                "PhyHidDevIn: name() failed -> invalid %s number %d",
                default_drivers[device_type].driver_name,
                device_num );
        return " ";
    }
    
    return _name;
}

//-----------------------------------------------------------------------------
// name: HidOut()
// desc: constructor
//-----------------------------------------------------------------------------
HidOut::HidOut()
{
    // phout = new PhyHidDevOut;
    m_device_num = 0;
    m_valid = FALSE;
    m_suppress_output = FALSE;
}




//-----------------------------------------------------------------------------
// name: ~HidOut()
// desc: destructor
//-----------------------------------------------------------------------------
HidOut::~HidOut()
{
    if( phout ) this->close();
    SAFE_DELETE( phout );
}




//-----------------------------------------------------------------------------
// name: send()
// desc: send
//-----------------------------------------------------------------------------
t_CKUINT HidOut::send( const HidMsg * msg )
{
    if( !m_valid ) return 0;
    
    return 0;
}




//-----------------------------------------------------------------------------
// name: open
// desc: open HID output
//-----------------------------------------------------------------------------
t_CKBOOL HidOut::open( t_CKUINT device_num )
{
    // close if already opened
    if( m_valid )
        this->close();

    return m_valid = HidOutManager::open( this, (t_CKINT)device_num );
}




//-----------------------------------------------------------------------------
// name: close( )
// desc: close HID output
//-----------------------------------------------------------------------------
t_CKBOOL HidOut::close( )
{
    if( !m_valid )
        return FALSE;

    // close
    // HidOutManager::close( this );

    m_valid = FALSE;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: HidIn()
// desc: constructor
//-----------------------------------------------------------------------------
HidIn::HidIn()
{
    phin = NULL;
    m_device_num = 0;
    m_valid = FALSE;
    m_read_index = UINT_MAX;
    m_buffer = NULL;
    m_suppress_output = FALSE;
    SELF = NULL;
}




//-----------------------------------------------------------------------------
// name: ~HidIn()
// desc: destructor
//-----------------------------------------------------------------------------
HidIn::~HidIn( )
{
    this->close();
    // SAFE_DELETE( min );
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::open( Chuck_VM * vm, t_CKINT device_type, t_CKINT device_num )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    // open
    return m_valid = HidInManager::open( this, vm, device_type, device_num );
}




//-----------------------------------------------------------------------------
// name: open()
// desc: open
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::open( Chuck_VM * vm, std::string & name, t_CKUINT device_type )
{
    // close if already opened
    if( m_valid )
        this->close();
    
    // open
    return m_valid = HidInManager::open( this, vm, device_type, name );
}




void HidInManager::init()
{
    if( has_init == FALSE )
    {
        // log
        EM_log( CK_LOG_INFO, "initializing HID..." );
        EM_pushlog();

        init_default_drivers();
        
        // allocate the matrix
        the_matrix.resize( CK_HID_DEV_COUNT );
        // resize each vector
        for( vector<vector<PhyHidDevIn *> >::size_type i = 0; i < the_matrix.size(); i++ )
        {
            // allocate
            the_matrix[i].resize( CK_MAX_HID_DEVICES );
        }
        
        msg_buffer = new CBufferSimple;
        msg_buffer->initialize( 1000, sizeof( HidMsg ) );
        
#ifndef __MACOSX_CORE__
        Hid_init();
#endif // __MACOSX_CORE__
        
        for( size_t j = 0; j < CK_HID_DEV_COUNT; j++ )
        {
            if( default_drivers[j].init )
                default_drivers[j].init();
        }
        
#ifdef __MACOSX_CORE__
        // start thread
        if( the_thread == NULL )
        {
            // allocate
            the_thread = new XThread;
            // flag
            thread_going = TRUE;
            // start
            the_thread->start( cb_hid_input, NULL );
        }
#endif
        
        has_init = TRUE;
        
        EM_poplog();
    }
}

void HidInManager::init_default_drivers()
{
    default_drivers = new Chuck_Hid_Driver[CK_HID_DEV_COUNT];
    
    memset( default_drivers, 0, CK_HID_DEV_COUNT * sizeof( Chuck_Hid_Driver ) );
    
    default_drivers[CK_HID_DEV_JOYSTICK].init = Joystick_init;
    default_drivers[CK_HID_DEV_JOYSTICK].quit = Joystick_quit;
    default_drivers[CK_HID_DEV_JOYSTICK].count = Joystick_count;
    default_drivers[CK_HID_DEV_JOYSTICK].count_elements = Joystick_count_elements;
    default_drivers[CK_HID_DEV_JOYSTICK].open = Joystick_open;
    default_drivers[CK_HID_DEV_JOYSTICK].close = Joystick_close;
    default_drivers[CK_HID_DEV_JOYSTICK].name = Joystick_name;
    default_drivers[CK_HID_DEV_JOYSTICK].driver_name = "joystick";
    
    default_drivers[CK_HID_DEV_MOUSE].init = Mouse_init;
    default_drivers[CK_HID_DEV_MOUSE].quit = Mouse_quit;
    default_drivers[CK_HID_DEV_MOUSE].count = Mouse_count;
    default_drivers[CK_HID_DEV_MOUSE].count_elements = Mouse_count_elements;
    default_drivers[CK_HID_DEV_MOUSE].open = Mouse_open;
    default_drivers[CK_HID_DEV_MOUSE].close = Mouse_close;
    default_drivers[CK_HID_DEV_MOUSE].name = Mouse_name;
    default_drivers[CK_HID_DEV_MOUSE].driver_name = "mouse";
    
    default_drivers[CK_HID_DEV_KEYBOARD].init = Keyboard_init;
    default_drivers[CK_HID_DEV_KEYBOARD].quit = Keyboard_quit;
    default_drivers[CK_HID_DEV_KEYBOARD].count = Keyboard_count;
    default_drivers[CK_HID_DEV_KEYBOARD].count_elements = Keyboard_count_elements;
    default_drivers[CK_HID_DEV_KEYBOARD].open = Keyboard_open;
    default_drivers[CK_HID_DEV_KEYBOARD].close = Keyboard_close;
    default_drivers[CK_HID_DEV_KEYBOARD].send = Keyboard_send;
    default_drivers[CK_HID_DEV_KEYBOARD].name = Keyboard_name;
    default_drivers[CK_HID_DEV_KEYBOARD].driver_name = "keyboard";
    
    default_drivers[CK_HID_DEV_WIIREMOTE].init = WiiRemote_init;
    default_drivers[CK_HID_DEV_WIIREMOTE].quit = WiiRemote_quit;
    default_drivers[CK_HID_DEV_WIIREMOTE].count = WiiRemote_count;
    default_drivers[CK_HID_DEV_WIIREMOTE].open = WiiRemote_open;
    default_drivers[CK_HID_DEV_WIIREMOTE].close = WiiRemote_close;
    default_drivers[CK_HID_DEV_WIIREMOTE].send = WiiRemote_send;
    default_drivers[CK_HID_DEV_WIIREMOTE].name = WiiRemote_name;
    default_drivers[CK_HID_DEV_WIIREMOTE].driver_name = "wii remote";
    
    default_drivers[CK_HID_DEV_TILTSENSOR].init = TiltSensor_init;
    default_drivers[CK_HID_DEV_TILTSENSOR].quit = TiltSensor_quit;
    default_drivers[CK_HID_DEV_TILTSENSOR].count = TiltSensor_count;
    default_drivers[CK_HID_DEV_TILTSENSOR].open = TiltSensor_open;
    default_drivers[CK_HID_DEV_TILTSENSOR].close = TiltSensor_close;
    default_drivers[CK_HID_DEV_TILTSENSOR].read = TiltSensor_read;
    default_drivers[CK_HID_DEV_TILTSENSOR].name = TiltSensor_name;
    default_drivers[CK_HID_DEV_TILTSENSOR].driver_name = "tilt sensor";
    
    default_drivers[CK_HID_DEV_MULTITOUCH].init = MultiTouchDevice_init;
    default_drivers[CK_HID_DEV_MULTITOUCH].quit = MultiTouchDevice_quit;
    default_drivers[CK_HID_DEV_MULTITOUCH].count = MultiTouchDevice_count;
    default_drivers[CK_HID_DEV_MULTITOUCH].open = MultiTouchDevice_open;
    default_drivers[CK_HID_DEV_MULTITOUCH].close = MultiTouchDevice_close;
    default_drivers[CK_HID_DEV_MULTITOUCH].name = MultiTouchDevice_name;
    default_drivers[CK_HID_DEV_MULTITOUCH].driver_name = "multitouch";
}

void HidInManager::cleanup()
{
    // log
    EM_log( CK_LOG_INFO, "shutting down HID..." );
    
    if( has_init )
    {    
        // loop
        for( vector<vector<PhyHidDevIn *> >::size_type i = 0; i < the_matrix.size(); i++ )
        {
            // loop
            for( vector<PhyHidDevIn *>::size_type j = 0; j < the_matrix[i].size(); j++ )
            {
                // deallocate devices
                SAFE_DELETE( the_matrix[i][j] );
            }
        }    
        
        // flag
        thread_going = FALSE;
        
        // break Hid_poll();
        Hid_quit();
        
        // clean up
        if( the_thread != NULL )
            SAFE_DELETE( the_thread );
        
        // clean up subsystems
        for( size_t j = 0; j < CK_HID_DEV_COUNT; j++ )
        {
            if( default_drivers[j].quit )
                default_drivers[j].quit();
        }
        
        if( msg_buffer )
        {
            msg_buffer->cleanup();
            SAFE_DELETE( msg_buffer );
        }
        
        for( std::map< Chuck_VM *, CBufferSimple * >::iterator it =
             m_event_buffers.begin(); it != m_event_buffers.end(); it++ )
        {
            Chuck_VM * vm = it->first;
            CBufferSimple * event_buffer = it->second;
            vm->destroy_event_buffer( event_buffer );
        }
        m_event_buffers.clear();
        
        // init
        has_init = FALSE;
        //*/
    }
}


t_CKBOOL HidInManager::open( HidIn * hin, Chuck_VM * vm, t_CKINT device_type, t_CKINT device_num )
{
    // init?
    if( has_init == FALSE )
    {
        init();
    }
    
    if( m_event_buffers.count( vm ) == 0 )
    {
        m_event_buffers[vm] = vm->create_event_buffer();
    }

    // check type
    if( device_type < 1 || device_type >= CK_HID_DEV_COUNT )
    {
        // log
        EM_log( CK_LOG_WARNING, "HidInManager: open() failed -> invalid type '%d'...", 
            device_type );
        return FALSE;
    }
    
    // start thread
    if( the_thread == NULL )
    {
        // allocate
        the_thread = new XThread;
        // flag
        thread_going = TRUE;
        // start
        the_thread->start( cb_hid_input, NULL );
    }
    
    // get the vector
    vector<PhyHidDevIn *> & v = the_matrix[device_type];

    // see if port not already open
    if( device_num >= (t_CKINT)v.capacity() || !v[device_num] )
    {
        // allocate
        PhyHidDevIn * phin = new PhyHidDevIn;
        // open
        if( !phin->open( vm, device_type, device_num ) )
        {
            // log
            // should this use EM_log instead, with a higher log level?
            EM_error2( 0, "HidIn: couldn't open %s %d...", 
                       default_drivers[device_type].driver_name, device_num );
            SAFE_DELETE( phin );
            return FALSE;
        }

        // resize?
        if( device_num >= (t_CKINT)v.capacity() )
        {
            t_CKINT size = v.capacity() * 2;
            if( device_num >= size ) size = device_num + 1;
            v.resize( size );
        }

        // put cbuf and rtmin in vector for future generations
        v[device_num] = phin;
    }

    // set min
    hin->phin = v[device_num];
    // found
    hin->m_buffer = v[device_num]->cbuf;
    // get an index into your (you are min here) own buffer, 
    // and a free ticket to your own workshop
    hin->m_read_index = hin->m_buffer->join( (Chuck_Event *)hin->SELF );
    hin->m_device_num = (t_CKUINT)device_num;

    // done
    return TRUE;
}



t_CKBOOL HidInManager::open( HidIn * hin, Chuck_VM * vm, t_CKINT device_type, std::string & device_name )
{
    // init?
    if( has_init == FALSE )
    {
        init();
    }
    
    if( m_event_buffers.count( vm ) == 0 )
    {
        m_event_buffers[vm] = vm->create_event_buffer();
    }
    
    t_CKINT device_type_start = 1;
    t_CKINT device_type_finish = CK_HID_DEV_COUNT;
    
    if(device_type != CK_HID_DEV_COUNT)
    {
        // check type
        if( device_type < 1 || device_type >= CK_HID_DEV_COUNT )
        {
            // log
            EM_log( CK_LOG_WARNING, "HidInManager: open() failed -> invalid type '%d'...", 
                    device_type );
            return FALSE;
        }
        
        device_type_start = device_type;
        device_type_finish = device_type + 1;
    }
    
    for(t_CKINT i = device_type_start; i < device_type_finish; i++)
    {
        if(default_drivers[i].count == NULL)
            continue;
        
        t_CKINT max_devices = default_drivers[i].count();
        
        for(t_CKINT j = 0; j < max_devices; j++)
        {
            const char * _name = NULL;
            
            if( !default_drivers[i].name )
                _name = default_drivers[i].driver_name;
            else
            {
                _name = default_drivers[i].name( ( int ) j );
            }
            
            if(!_name)
            {
                continue;
            }
            
            std::string name = _name;
            
//            PhyHidDevIn * phyHid = devices[j];
//            if(!phyHid)
//                continue;
//            
//            std::string name = phyHid->name();
            
            if(name == device_name)
            {
                return open( hin, vm, i, j );
            }
        }
    }
    
    EM_log( CK_LOG_WARNING, "HidInManager: open() failed -> no device named '%s'...", 
            device_name.c_str() );
    
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: close()
// desc: close
//-----------------------------------------------------------------------------
t_CKBOOL HidInManager::close( HidIn * hin )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanupBuffer()
// desc: cleanup buffer
//-----------------------------------------------------------------------------
void HidInManager::cleanup_buffer( Chuck_VM *vm )
{
    if( m_event_buffers.count( vm ) > 0 )
    {
        vm->destroy_event_buffer( m_event_buffers[vm] );
        m_event_buffers.erase( vm );
    }
}



//-----------------------------------------------------------------------------
// name: close()
// desc: close
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::close()
{
    if( !m_valid )
        return FALSE;

    if(m_read_index != UINT_MAX && m_buffer)
    {
        m_buffer->resign(m_read_index);
        m_read_index = UINT_MAX;
        m_buffer = NULL;
    }
    
    // close
    HidInManager::close( this );

    m_valid = FALSE;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: empty()
// desc: is empty?
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::empty()
{
    if( !m_valid ) return TRUE;
    return m_buffer->empty( m_read_index );
}




//-----------------------------------------------------------------------------
// name: recv()
// desc: receive message
//-----------------------------------------------------------------------------
t_CKUINT HidIn::recv( HidMsg * msg )
{
    if( !m_valid ) return FALSE;
    return m_buffer->get( msg, 1, m_read_index );
}

//-----------------------------------------------------------------------------
// name: read()
// desc: read
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::read( t_CKINT type, t_CKINT num, HidMsg * msg )
{
    if( !m_valid || !phin )
        return FALSE;
    
    // do read
    return phin->read( type, num, msg );
}

//-----------------------------------------------------------------------------
// name: send()
// desc: send
//-----------------------------------------------------------------------------
t_CKBOOL HidIn::send( const HidMsg * msg )
{
    if( !m_valid || !phin )
        return FALSE;
    
    // do send
    return phin->send( msg );
}

//-----------------------------------------------------------------------------
// name: name()
// desc: get device name
//-----------------------------------------------------------------------------
std::string HidIn::name()
{
    if( m_valid && phin )
        return phin->name();
    return " ";
}

//-----------------------------------------------------------------------------
// name: cb_hid_input
// desc: called by device implementations to push a message onto the buffer
//-----------------------------------------------------------------------------
void HidInManager::push_message( HidMsg & msg )
{
    // find the queue
    if( the_matrix[msg.device_type][msg.device_num] != NULL )
    {
        CBufferAdvance * cbuf = the_matrix[msg.device_type][msg.device_num]->cbuf;
        if( cbuf != NULL )
            // queue the thing
            cbuf->put( &msg, 1 );
    }
}

extern "C" void push_message( HidMsg msg )
{
    HidInManager::push_message( msg );
}

//-----------------------------------------------------------------------------
// name: cb_hid_input
// desc: call back
//-----------------------------------------------------------------------------
#ifndef __PLATFORM_WIN32__
void * HidInManager::cb_hid_input( void * stuff )
#else
unsigned __stdcall HidInManager::cb_hid_input( void * stuff )
#endif 
{
#ifdef __MACOSX_CORE__
    Hid_init();
#endif

    // keep going
    while( thread_going )
    {
        Hid_poll();
    }
    
    // log
    EM_log( CK_LOG_INFO, "HID thread exiting..." );

    return 0;
}

//-----------------------------------------------------------------------------
// name: probeHidIn()
// desc: ...
//-----------------------------------------------------------------------------
void HidInManager::probeHidIn()
{
    t_CKBOOL do_cleanup = !has_init;
    
    init();
    
    for( size_t i = 0; i < CK_HID_DEV_COUNT; i++ )
    {
        if( !default_drivers[i].count )
            continue;
        
        int count = default_drivers[i].count();
        if( count == 0 )
            continue;
        
        EM_error2b( 0, "------( chuck -- %i %s device%s )------", 
                    count, default_drivers[i].driver_name,
                    count > 1 ? "s" : "" );
        
        for( int j = 0; j < count; j++ )
        {
            const char * name;
            if( default_drivers[i].name )
                name = default_drivers[i].name( j );
            if( !name )
                name = "(no name)";
            
            EM_error2b( 0, "    [%i] : \"%s\"", j, name );
        }
        
        EM_error2b( 0, "" );
    }
    
    if( do_cleanup )
    {
        cleanup();
    }
}

//-----------------------------------------------------------------------------
// name: probeHidOut()
// desc: ...
//-----------------------------------------------------------------------------
void HidInManager::probeHidOut()
{
    
}

HidOutManager::HidOutManager()
{
    the_phouts.resize( 1024 );
}


HidOutManager::~HidOutManager()
{
    // yeah right
}


t_CKBOOL HidOutManager::open( HidOut * hout, t_CKINT device_num )
{
    // see if port not already open
    if( device_num >= (t_CKINT)the_phouts.capacity() || !the_phouts[device_num] )
    {
        // allocate
        PhyHidDevOut * phout = new PhyHidDevOut;

        // resize?
        if( device_num >= (t_CKINT)the_phouts.capacity() )
        {
            t_CKINT size = the_phouts.capacity() * 2;
            if( device_num >= size ) size = device_num + 1;
            the_phouts.resize( size );
        }

        // put rtmout in vector for future generations
        the_phouts[device_num] = phout;
    }

    // found (always) (except when it doesn't get here)
    hout->phout = the_phouts[device_num];
    hout->m_device_num = (t_CKUINT)device_num;

    // done
    return TRUE;
}


