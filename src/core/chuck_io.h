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
#include "chuck_dl.h"
#ifndef __DISABLE_SERIAL__
#include "util_thread.h"
#endif
#include "util_buffers.h"
#include <list>




// class initialization
// do not __DISABLE_MIDI__ this one in particular
t_CKBOOL init_class_Midi( Chuck_Env * env );
#ifndef __DISABLE_MIDI__
t_CKBOOL init_class_MidiRW( Chuck_Env * env );
#endif
#ifndef __DISABLE_HID__
t_CKBOOL init_class_HID( Chuck_Env * env );
#endif
t_CKBOOL init_class_io( Chuck_Env * env, Chuck_Type * type );
#ifndef __DISABLE_FILEIO__
t_CKBOOL init_class_fileio( Chuck_Env * env, Chuck_Type * type );
#endif
// added 1.3.0.0 -- moved to be full-fledged class
t_CKBOOL init_class_chout( Chuck_Env * env, Chuck_Type * type );
// added 1.3.0.0 -- moved to be full-fledged class
t_CKBOOL init_class_cherr( Chuck_Env * env, Chuck_Type * type );
#ifndef __DISABLE_SERIAL__
 // added 1.3.1
t_CKBOOL init_class_serialio( Chuck_Env * env );
#endif





//-----------------------------------------------------------------------------
// io API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( io_dummy );
CK_DLL_SFUN( io_newline );
CK_DLL_SFUN( io_openfile );


#ifndef __DISABLE_FILEIO__
//-----------------------------------------------------------------------------
// fileio API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( fileio_ctor );
CK_DLL_DTOR( fileio_dtor );
CK_DLL_MFUN( fileio_open );
CK_DLL_MFUN( fileio_openflags );
CK_DLL_MFUN( fileio_good );
CK_DLL_MFUN( fileio_close );
CK_DLL_MFUN( fileio_flush );
CK_DLL_MFUN( fileio_getmode );
CK_DLL_MFUN( fileio_setmode );
CK_DLL_MFUN( fileio_size );
CK_DLL_MFUN( fileio_seek );
CK_DLL_MFUN( fileio_tell );
CK_DLL_MFUN( fileio_isdir );
CK_DLL_MFUN( fileio_dirlist );
CK_DLL_MFUN( fileio_read );
CK_DLL_MFUN( fileio_readline );
CK_DLL_MFUN( fileio_readint );
CK_DLL_MFUN( fileio_readintflags );
CK_DLL_MFUN( fileio_readfloat );
CK_DLL_MFUN( fileio_eof );
CK_DLL_MFUN( fileio_more );
CK_DLL_MFUN( fileio_writestring );
CK_DLL_MFUN( fileio_writeint );
CK_DLL_MFUN( fileio_writeintflags );
CK_DLL_MFUN( fileio_writefloat );
#endif


//-----------------------------------------------------------------------------
// chout API -- added 1.3.0.0 as full class
//-----------------------------------------------------------------------------
CK_DLL_MFUN( chout_good );
CK_DLL_MFUN( chout_close );
CK_DLL_MFUN( chout_flush );
CK_DLL_MFUN( chout_getmode );
CK_DLL_MFUN( chout_setmode );
CK_DLL_MFUN( chout_read );
CK_DLL_MFUN( chout_readline );
CK_DLL_MFUN( chout_readint );
CK_DLL_MFUN( chout_readintflags );
CK_DLL_MFUN( chout_readfloat );
CK_DLL_MFUN( chout_eof );
CK_DLL_MFUN( chout_more );
CK_DLL_MFUN( chout_writestring );
CK_DLL_MFUN( chout_writeint );
CK_DLL_MFUN( chout_writefloat );


//-----------------------------------------------------------------------------
// cherr API -- added 1.3.0.0 as full class
//-----------------------------------------------------------------------------
CK_DLL_MFUN( cherr_good );
CK_DLL_MFUN( cherr_close );
CK_DLL_MFUN( cherr_flush );
CK_DLL_MFUN( cherr_getmode );
CK_DLL_MFUN( cherr_setmode );
CK_DLL_MFUN( cherr_read );
CK_DLL_MFUN( cherr_readline );
CK_DLL_MFUN( cherr_readint );
CK_DLL_MFUN( cherr_readintflags );
CK_DLL_MFUN( cherr_readfloat );
CK_DLL_MFUN( cherr_eof );
CK_DLL_MFUN( cherr_more );
CK_DLL_MFUN( cherr_writestring );
CK_DLL_MFUN( cherr_writeint );
CK_DLL_MFUN( cherr_writefloat );




//-----------------------------------------------------------------------------
// MidiMsg API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsg_ctor );

extern t_CKUINT MidiMsg_offset_data1;
extern t_CKUINT MidiMsg_offset_data2;
extern t_CKUINT MidiMsg_offset_data3;
extern t_CKUINT MidiMsg_offset_when;

#ifndef __DISABLE_MIDI__
//-----------------------------------------------------------------------------
// MidiRW API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiRW_ctor );
CK_DLL_DTOR( MidiRW_dtor );
CK_DLL_MFUN( MidiRW_open );
CK_DLL_MFUN( MidiRW_close );
CK_DLL_MFUN( MidiRW_read );
CK_DLL_MFUN( MidiRW_write );


//-----------------------------------------------------------------------------
// MidiMsgOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsgOut_ctor );
CK_DLL_DTOR( MidiMsgOut_dtor );
CK_DLL_MFUN( MidiMsgOut_open );
CK_DLL_MFUN( MidiMsgOut_close );
CK_DLL_MFUN( MidiMsgOut_write );


//-----------------------------------------------------------------------------
// MidiMsgIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsgIn_ctor );
CK_DLL_DTOR( MidiMsgIn_dtor );
CK_DLL_MFUN( MidiMsgIn_open );
CK_DLL_MFUN( MidiMsgIn_close );
CK_DLL_MFUN( MidiMsgIn_read );


//-----------------------------------------------------------------------------
// MidiIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiIn_ctor );
CK_DLL_DTOR( MidiIn_dtor );
CK_DLL_MFUN( MidiIn_open );
CK_DLL_MFUN( MidiIn_open_named ); // added 1.3.0.0
CK_DLL_MFUN( MidiIn_open_named_i ); // added 1.3.0.0
CK_DLL_MFUN( MidiIn_good );
CK_DLL_MFUN( MidiIn_num );
CK_DLL_MFUN( MidiIn_name );
CK_DLL_MFUN( MidiIn_printerr );
CK_DLL_MFUN( MidiIn_recv );
CK_DLL_MFUN( MidiIn_can_wait );


//-----------------------------------------------------------------------------
// MidiOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiOut_ctor );
CK_DLL_DTOR( MidiOut_dtor );
CK_DLL_MFUN( MidiOut_open );
CK_DLL_MFUN( MidiOut_open_named ); // added 1.3.0.0
CK_DLL_MFUN( MidiOut_open_named_i); // added 1.3.0.0
CK_DLL_MFUN( MidiOut_good );
CK_DLL_MFUN( MidiOut_num );
CK_DLL_MFUN( MidiOut_name );
CK_DLL_MFUN( MidiOut_printerr );
CK_DLL_MFUN( MidiOut_send );
#endif // __DISABLE_MIDI__



#ifndef __DISABLE_HID__
//-----------------------------------------------------------------------------
// HidMsg API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( HidMsg_is_axis_motion );
CK_DLL_MFUN( HidMsg_is_button_down );
CK_DLL_MFUN( HidMsg_is_button_up );
CK_DLL_MFUN( HidMsg_is_mouse_motion );
CK_DLL_MFUN( HidMsg_is_hat_motion );
CK_DLL_MFUN( HidMsg_is_wheel_motion );

//-----------------------------------------------------------------------------
// HidIn API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HidIn_ctor );
CK_DLL_DTOR( HidIn_dtor );
CK_DLL_MFUN( HidIn_open );
CK_DLL_MFUN( HidIn_open_named );
CK_DLL_MFUN( HidIn_open_named_i ); // added 1.3.0.0
CK_DLL_MFUN( HidIn_open_joystick );
CK_DLL_MFUN( HidIn_open_mouse );
CK_DLL_MFUN( HidIn_open_keyboard );
CK_DLL_MFUN( HidIn_open_tiltsensor );
CK_DLL_MFUN( HidIn_good );
CK_DLL_MFUN( HidIn_num );
CK_DLL_MFUN( HidIn_name );
CK_DLL_MFUN( HidIn_printerr );
CK_DLL_MFUN( HidIn_recv );
CK_DLL_MFUN( HidIn_read );
CK_DLL_MFUN( HidIn_send );
CK_DLL_MFUN( HidIn_can_wait );
CK_DLL_SFUN( HidIn_read_tilt_sensor );
CK_DLL_SFUN( HidIn_ctrl_tiltPollRate );
CK_DLL_SFUN( HidIn_cget_tiltPollRate );
CK_DLL_SFUN( HidIn_start_cursor_track );
CK_DLL_SFUN( HidIn_stop_cursor_track );


//-----------------------------------------------------------------------------
// HidOut API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HidOut_ctor );
CK_DLL_DTOR( HidOut_dtor );
CK_DLL_MFUN( HidOut_open );
CK_DLL_MFUN( HidOut_good );
CK_DLL_MFUN( HidOut_num );
CK_DLL_MFUN( HidOut_name );
CK_DLL_MFUN( HidOut_printerr );
CK_DLL_MFUN( HidOut_send );
#endif // __DISABLE_HID__



#ifndef __DISABLE_SERIAL__
//-----------------------------------------------------------------------------
// name: struct Chuck_IO_Serial
// desc: chuck serial I/O
//-----------------------------------------------------------------------------
struct Chuck_IO_Serial : public Chuck_IO
{
public:
    Chuck_IO_Serial( Chuck_VM * vm );
    virtual ~Chuck_IO_Serial();
    
    static void shutdown();

public:
    // meta
    virtual t_CKBOOL open( const t_CKUINT i, t_CKINT flags, t_CKUINT baud = CK_BAUD_9600 );
    virtual t_CKBOOL open( const std::string & path, t_CKINT flags, t_CKUINT baud = CK_BAUD_9600 );
    
    virtual t_CKBOOL can_wait();
    
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

    XThread * m_write_thread;
    static void *shell_write_cb(void *);
    void write_cb();
    
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
    bool m_do_write_thread;
    
    CircularBuffer<Request> m_asyncRequests;
    CircularBuffer<Request> m_asyncResponses;
    CBufferSimple * m_event_buffer;

    int m_fd;
    FILE * m_cfd;
    
    std::string m_path;
    
    unsigned char * m_io_buf;
    t_CKUINT m_io_buf_max;
    t_CKUINT m_io_buf_available;
    t_CKUINT m_io_buf_pos;
    
    unsigned char * m_tmp_buf;
    t_CKUINT m_tmp_buf_max;
    
    CircularBuffer<Request> m_asyncWriteRequests;
    CircularBuffer<char> m_writeBuffer;
    
    t_CKINT m_flags;
    t_CKINT m_iomode; // SYNC or ASYNC
    t_CKBOOL m_eof;
    
    t_CKBOOL m_do_exit;
    
    static std::list<Chuck_IO_Serial *> s_serials;
    
    Chuck_VM * m_vmRef;
};
#endif // __DISABLE_SERIAL__





#endif // __CHUCK_IO_H__
