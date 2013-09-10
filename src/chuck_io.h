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
// name: chuck_io.h
// desc: chuck i/o
//
// authors: Spencer Salazar (spencer@ccrma.stanford.edu)
//    date: Summer 2012
//-----------------------------------------------------------------------------
#ifndef __CHUCK_IO_H__
#define __CHUCK_IO_H__

#include "chuck_oo.h"
#include "util_thread.h"
#include "util_buffers.h"
#include <list>




//-----------------------------------------------------------------------------
// name: struct Chuck_IO_Serial
// desc: chuck serial I/O
//-----------------------------------------------------------------------------
struct Chuck_IO_Serial : public Chuck_IO
{
public:
    Chuck_IO_Serial();
    virtual ~Chuck_IO_Serial();
    
    static void shutdown();

public:
    // meta
    virtual t_CKBOOL open( const t_CKUINT i, t_CKINT flags, t_CKUINT baud = CK_BAUD_9600 );
    virtual t_CKBOOL open( const std::string & path, t_CKINT flags, t_CKUINT baud = CK_BAUD_9600 );
    
    virtual t_CKBOOL good();
    virtual void close();
    virtual void flush();
    virtual t_CKINT mode();
    virtual void mode( t_CKINT flag );
    
    // reading
    virtual Chuck_String * readLine();
    virtual t_CKINT readInt( t_CKINT flags );
    virtual t_CKFLOAT readFloat();
    virtual t_CKBOOL readString( std::string & str );
    virtual t_CKBOOL eof();
    
    // writing
    virtual void write( const std::string & val );
    virtual void write( t_CKINT val );
    virtual void write( t_CKINT val, t_CKINT size );
    virtual void writeBytes( Chuck_Array4 * arr );
    virtual void write( t_CKFLOAT val );

    // serial stuff
    virtual t_CKBOOL setBaudRate( t_CKUINT rate );
    virtual t_CKUINT getBaudRate();
    
    // async reading
    virtual t_CKBOOL readAsync( t_CKUINT type, t_CKUINT num );
    virtual Chuck_String * getLine();
    virtual t_CKINT getByte();
    virtual Chuck_Array * getBytes();
    virtual Chuck_Array * getInts();
    virtual Chuck_Array * getFloats();
    virtual Chuck_String * getString();
    
    static const t_CKUINT TYPE_NONE; 
    static const t_CKUINT TYPE_BYTE;
    static const t_CKUINT TYPE_WORD;
    static const t_CKUINT TYPE_INT;
    static const t_CKUINT TYPE_FLOAT;
    static const t_CKUINT TYPE_STRING;
    static const t_CKUINT TYPE_LINE;
    static const t_CKUINT TYPE_WRITE;
    
    struct Request
    {
        t_CKUINT m_type; // type
        t_CKUINT m_num; // how many
        t_CKUINT m_status;
        t_CKUINT m_val; // ISSUE: 64-bit
        
        enum Status
        {
            RQ_STATUS_PENDING,
            RQ_STATUS_SUCCESS,
            RQ_STATUS_FAILURE,
            RQ_STATUS_INVALID,
            RQ_STATUS_EOF,
        };
    };
    
    virtual t_CKBOOL ready();
    
    // available baud rates
    static const t_CKUINT CK_BAUD_2400;
    static const t_CKUINT CK_BAUD_4800;
    static const t_CKUINT CK_BAUD_9600;
    static const t_CKUINT CK_BAUD_19200;
    static const t_CKUINT CK_BAUD_38400;
    static const t_CKUINT CK_BAUD_7200;
    static const t_CKUINT CK_BAUD_14400;
    static const t_CKUINT CK_BAUD_28800;
    static const t_CKUINT CK_BAUD_57600;
    static const t_CKUINT CK_BAUD_76800;
    static const t_CKUINT CK_BAUD_115200;
    static const t_CKUINT CK_BAUD_230400;

protected:
    
    void start_read_thread();
    XThread * m_read_thread;
    static void *shell_read_cb(void *);
    void read_cb();

    
    void close_int();

    t_CKBOOL get_buffer(t_CKINT timeout_ms = 1);
    t_CKINT peek_buffer();
    t_CKINT pull_buffer();
    t_CKINT buffer_bytes_to_tmp(t_CKINT num_bytes);
    
    t_CKBOOL handle_line(Request &);
    t_CKBOOL handle_string(Request &);
    t_CKBOOL handle_float_ascii(Request &);
    t_CKBOOL handle_int_ascii(Request &);
    t_CKBOOL handle_byte(Request &);
    t_CKBOOL handle_float_binary(Request &);
    t_CKBOOL handle_int_binary(Request &);
    
    bool m_do_read_thread;
    
    CircularBuffer<Request> m_asyncRequests;
    CircularBuffer<Request> m_asyncResponses;
    CircularBuffer<char> m_writeBuffer;
    CBufferSimple * m_event_buffer;
    
    int m_fd;
    FILE * m_cfd;
    
    std::string m_path;
    
    char * m_io_buf;
    t_CKUINT m_io_buf_max;
    t_CKUINT m_io_buf_available;
    t_CKUINT m_io_buf_pos;
    
    char * m_tmp_buf;
    t_CKUINT m_tmp_buf_max;
    
    t_CKINT m_flags;
    t_CKINT m_iomode; // SYNC or ASYNC
    t_CKBOOL m_eof;
    
    t_CKBOOL m_do_exit;
    
    static std::list<Chuck_IO_Serial *> s_serials;
};


// initialize
t_CKBOOL init_class_serialio( Chuck_Env * env ); // added 1.3.1




#endif // __CHUCK_IO_H__
