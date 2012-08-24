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
// name: chuck_io.cpp
// desc: chuck i/o
//
// authors: Spencer Salazar (spencer@ccrma.stanford.edu)
//    date: Summer 2012
//-----------------------------------------------------------------------------


#include "chuck_io.h"
#include "chuck_errmsg.h"
#include "util_serial.h"
#include "chuck_instr.h"
#include "chuck_type.h"

#include <poll.h>
#include <fcntl.h>
#include <termios.h>

#include <ext/stdio_filebuf.h>
#include <iostream>
#include <fstream>

using namespace std;


Chuck_IO_Serial::Chuck_IO_Serial()
{
    m_fd = 0;
    m_cfd = NULL;
    
    m_buf_size = 256;
    m_buf = new char[m_buf_size];
    m_buf_end = m_buf_begin = 0;
}

Chuck_IO_Serial::~Chuck_IO_Serial()
{
    fclose(m_cfd);
}

t_CKBOOL Chuck_IO_Serial::ready()
{
    struct pollfd pollfds;
    pollfds.fd = m_fd;
    pollfds.events = POLLIN;
    pollfds.revents = 0;
    
    int result = poll(&pollfds, 1, 0);
    
    if( result > 0 && (pollfds.revents & POLLIN) ) return TRUE;
    else return FALSE; // TODO: error handling
}

t_CKBOOL Chuck_IO_Serial::open( const t_CKUINT i, t_CKINT flags )
{
    vector<string> ports = SerialIOManager::availableSerialDevices();
    
    if( i < ports.size() )
    {
        const string &path = ports[i];
        int fd = ::open( path.c_str(), O_NONBLOCK | O_RDWR );
        if( fd >= 0 ) m_fd = fd;
        
        // set serial IO baud rate
        struct termios tios;
        tcgetattr(m_fd, &tios);
        cfsetispeed(&tios, B9600);
        cfsetospeed(&tios, B9600);
        tcsetattr(m_fd, TCSAFLUSH, &tios);
    }
    
    return TRUE;
}


t_CKBOOL Chuck_IO_Serial::open( const std::string & path, t_CKINT flags )
{
    return TRUE;
}


Chuck_String * Chuck_IO_Serial::readLine()
{
    Chuck_String * ckstr = new Chuck_String;
    initialize_object(ckstr, &t_string);
    
    // found newline?
    bool found_it = false;

    while(!found_it)
    {
        // consume existing data in buffer, up to newline
        if(m_buf_end != m_buf_begin)
        {
            // scan for newline
            int i;
            for(i = m_buf_begin; i < m_buf_end; i++)
            {
                // found it
                if(m_buf[i] == '\n')
                {
                    found_it = true;
                    break;
                }
            }
            
            // append characters to string
            if(found_it)
            {
                // dont append newline
                ckstr->str.append(m_buf + m_buf_begin, i - m_buf_begin);
                // advance buffer start index
                m_buf_begin = i+1;
                
                // restart buffer at zero if all data has been consumed
                if(m_buf_begin == m_buf_end)
                {
                    m_buf_begin = m_buf_end = 0;
                    // debug: clear buffer
                    memset(m_buf, 0, m_buf_size*sizeof(char));
                }
            }
            else
            {
                // append existing data to string; need more read(s) to find newline
                ckstr->str.append(m_buf + m_buf_begin, m_buf_end - m_buf_begin);
                // all data in buffer has been consumed
                m_buf_begin = m_buf_end = 0;
                // debug: clear buffer
                memset(m_buf, 0, m_buf_size*sizeof(char));
            }
            
        }
        
        if(!found_it)
        {
            // dont read into a buffer that still has data
            assert(m_buf_begin == m_buf_end && m_buf_begin == 0);
            // read data in
            int result = ::read(m_fd, m_buf, m_buf_size);
            // no data or error
            // TODO: save string for next readLine call in case of 0-length read
            if(result <= 0) goto error;
            // advance buffer, if possible
            m_buf_end = result;
        }
    }
    
    return ckstr;
    
error:
    ckstr->str = "";
    return ckstr;
}



//-----------------------------------------------------------------------------
// serialio API
//-----------------------------------------------------------------------------
CK_DLL_SFUN( serialio_list );
CK_DLL_CTOR( serialio_ctor );
CK_DLL_DTOR( serialio_dtor );
CK_DLL_MFUN( serialio_open );
CK_DLL_MFUN( serialio_ready );
CK_DLL_MFUN( serialio_readLine );
static t_CKUINT serialio_data_offset = CK_INVALID_OFFSET;


//-----------------------------------------------------------------------------
// name: init_class_serialio()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_serialio( Chuck_Env * env )
{
    // init as base class
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_SEVERE, "class 'SerialIO'" );
    
    // TODO: ctor/dtor?
    if( !type_engine_import_class_begin( env, "SerialIO", "IO",
                                        env->global(), serialio_ctor, NULL ) )
        return FALSE;
    
    // add list()
    func = make_new_sfun( "string[]", "list", serialio_list );
    if( !type_engine_import_sfun( env, func ) ) goto error;
    
    serialio_data_offset = type_engine_import_mvar( env, "int", "@SerialIO_data", false );
    if( serialio_data_offset == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun("int", "open", serialio_open);
    func->add_arg("int", "i");
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun("int", "ready", serialio_ready);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun("string", "readLine", serialio_readLine);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    
    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}



CK_DLL_SFUN( serialio_list )
{
    vector<string> devices = SerialIOManager::availableSerialDevices();
    
    // ISSUE: 64-bit
    Chuck_Array4 * array = new Chuck_Array4(TRUE, 0);
    initialize_object( array, &t_array );
    
    for(vector<string>::iterator i = devices.begin(); i != devices.end(); i++)
    {
        Chuck_String * name = new Chuck_String(*i);
        initialize_object(name, &t_string);
        array->push_back((t_CKUINT) name);
    }
    
    RETURN->v_object = array;
}


CK_DLL_CTOR( serialio_ctor )
{
    Chuck_IO_Serial * cereal = new Chuck_IO_Serial;
    OBJ_MEMBER_INT(SELF, serialio_data_offset) = (t_CKINT) cereal;
}

CK_DLL_MFUN( serialio_open )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) OBJ_MEMBER_INT(SELF, serialio_data_offset);
    t_CKINT num = GET_NEXT_INT(ARGS);
    
    RETURN->v_int = cereal->open(num, 0);
}

CK_DLL_MFUN( serialio_ready )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) OBJ_MEMBER_INT(SELF, serialio_data_offset);
    
    RETURN->v_int = cereal->ready();
}

CK_DLL_MFUN( serialio_readLine )
{
    Chuck_IO_Serial * cereal = (Chuck_IO_Serial *) OBJ_MEMBER_INT(SELF, serialio_data_offset);
    
    RETURN->v_string = cereal->readLine();
}


