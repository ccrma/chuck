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
// name: chuck_lang.h
// desc: chuck class library base
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//          Spencer Salazar (spencer@ccrma.stanford.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Andrew Schran (aschran@princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#ifndef __CHUCK_LANG_H__
#define __CHUCK_LANG_H__

#include "chuck_def.h"
#include "chuck_oo.h"
#include "chuck_dl.h"
#include <queue>


// forward reference
struct Chuck_UAna;

// query
DLL_QUERY lang_query( Chuck_DL_Query * QUERY );

// base class initialization
t_CKBOOL init_class_object( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_ugen( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_uana( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_blob( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_event( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_shred( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_io( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_fileio( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_chout( Chuck_Env * env, Chuck_Type * type ); // added 1.3.0.0 -- moved to be full-fledged class
t_CKBOOL init_class_cherr( Chuck_Env * env, Chuck_Type * type ); // added 1.3.0.0 -- moved to be full-fledged class
t_CKBOOL init_class_string( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_array( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_Midi( Chuck_Env * env );
t_CKBOOL init_class_MidiRW( Chuck_Env * env );
t_CKBOOL init_class_HID( Chuck_Env * env );




//-----------------------------------------------------------------------------
// object API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( object_ctor );
CK_DLL_DTOR( object_dtor );
CK_DLL_MFUN( object_equals );
CK_DLL_MFUN( object_getType );
CK_DLL_MFUN( object_hashCode );
CK_DLL_MFUN( object_toString );


//-----------------------------------------------------------------------------
// ugen API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( ugen_ctor );
CK_DLL_DTOR( ugen_dtor );
CK_DLL_MFUN( ugen_op );
CK_DLL_MFUN( ugen_cget_op );
CK_DLL_MFUN( ugen_last );
CK_DLL_MFUN( ugen_cget_last );
CK_DLL_MFUN( ugen_next );
CK_DLL_MFUN( ugen_cget_next );
CK_DLL_MFUN( ugen_gain );
CK_DLL_MFUN( ugen_cget_gain );
CK_DLL_MFUN( ugen_numChannels );
CK_DLL_MFUN( ugen_cget_numChannels );
CK_DLL_MFUN( ugen_chan );
CK_DLL_MFUN( ugen_connected );


//-----------------------------------------------------------------------------
// uana API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( uana_ctor );
CK_DLL_DTOR( uana_dtor );
CK_DLL_MFUN( uana_upchuck );
CK_DLL_MFUN( uana_blob );
CK_DLL_MFUN( uana_fvals );
CK_DLL_MFUN( uana_cvals );
CK_DLL_MFUN( uana_fval );
CK_DLL_MFUN( uana_cval );
CK_DLL_MFUN( uana_connected );


//-----------------------------------------------------------------------------
// uanablob API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( uanablob_ctor );
CK_DLL_DTOR( uanablob_dtor );
CK_DLL_MFUN( uanablob_fvals );
CK_DLL_MFUN( uanablob_cvals );
CK_DLL_MFUN( uanablob_fval );
CK_DLL_MFUN( uanablob_cval );
CK_DLL_MFUN( uanablob_when );


//-----------------------------------------------------------------------------
// name: Chuck_UAnaBlobProxy
// desc: proxy for interfacing with UAnaBlob, which is a Chuck class
//-----------------------------------------------------------------------------
struct Chuck_UAnaBlobProxy
{
public:
    Chuck_UAnaBlobProxy( Chuck_Object * blob );
    virtual ~Chuck_UAnaBlobProxy();

public:
    t_CKTIME & when();
    Chuck_Array8 & fvals();
    Chuck_Array16 & cvals();

public:
    Chuck_Object * realblob() { return m_blob; }

protected:
    Chuck_Object * m_blob;
};

// get proxy
Chuck_UAnaBlobProxy * getBlobProxy( const Chuck_UAna * uana );


//-----------------------------------------------------------------------------
// shred API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( shred_ctor );
CK_DLL_DTOR( shred_dtor );
CK_DLL_MFUN( shred_exit );
CK_DLL_MFUN( shred_clone );
CK_DLL_MFUN( shred_id );
CK_DLL_MFUN( shred_yield );
CK_DLL_MFUN( shred_running );
CK_DLL_MFUN( shred_done );
CK_DLL_MFUN( shred_numArgs );
CK_DLL_MFUN( shred_getArg );
CK_DLL_MFUN( shred_sourcePath ); // added 1.3.0.0
CK_DLL_MFUN( shred_sourceDir ); // added 1.3.0.0
CK_DLL_MFUN( shred_sourceDir2 ); // added 1.3.2.0
CK_DLL_SFUN( shred_fromId ); // added 1.3.2.0


//-----------------------------------------------------------------------------
// array API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( array_ctor );
CK_DLL_DTOR( array_dtor );
CK_DLL_MFUN( array_set_capacity );
CK_DLL_MFUN( array_get_capacity );
CK_DLL_MFUN( array_get_capacity_hack );
CK_DLL_MFUN( array_set_size );
CK_DLL_MFUN( array_get_size );
CK_DLL_MFUN( array_push_back );
CK_DLL_MFUN( array_pop_back );
CK_DLL_MFUN( array_push_front );
CK_DLL_MFUN( array_pop_front );
CK_DLL_MFUN( array_find );
CK_DLL_MFUN( array_erase );
CK_DLL_MFUN( array_clear );
CK_DLL_MFUN( array_reset );


//-----------------------------------------------------------------------------
// event API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( event_ctor );
CK_DLL_DTOR( event_dtor );
CK_DLL_MFUN( event_signal );
CK_DLL_MFUN( event_broadcast );
CK_DLL_MFUN( event_wait );
CK_DLL_MFUN( event_can_wait );


//-----------------------------------------------------------------------------
// io API
//-----------------------------------------------------------------------------
CK_DLL_MFUN( io_dummy );
CK_DLL_SFUN( io_newline );
CK_DLL_SFUN( io_openfile );


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
// string API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( string_ctor );
CK_DLL_DTOR( string_dtor );
CK_DLL_MFUN( string_length );
CK_DLL_MFUN( string_upper );
CK_DLL_MFUN( string_lower );
CK_DLL_MFUN( string_ltrim );
CK_DLL_MFUN( string_rtrim );
CK_DLL_MFUN( string_trim );
CK_DLL_MFUN( string_toString );
CK_DLL_MFUN( string_set_at );
CK_DLL_MFUN( string_get_at );
CK_DLL_MFUN(string_charAt);
CK_DLL_MFUN(string_setCharAt);
CK_DLL_MFUN(string_substring);
CK_DLL_MFUN(string_substringN);
CK_DLL_MFUN(string_insert);
CK_DLL_MFUN(string_replace);
CK_DLL_MFUN(string_replaceN);
CK_DLL_MFUN(string_find);
CK_DLL_MFUN(string_findStart);
CK_DLL_MFUN(string_findStr);
CK_DLL_MFUN(string_findStrStart);
CK_DLL_MFUN(string_rfind);
CK_DLL_MFUN(string_rfindStart);
CK_DLL_MFUN(string_rfindStr);
CK_DLL_MFUN(string_rfindStrStart);
CK_DLL_MFUN(string_erase);
CK_DLL_MFUN(string_toInt);
CK_DLL_MFUN(string_toFloat);
CK_DLL_MFUN(string_parent);




//-----------------------------------------------------------------------------
// name: Data_Exception
// desc: data for base Chuck Exception class
//-----------------------------------------------------------------------------
struct Data_Exception
{
public:
    Data_Exception();
    ~Data_Exception();

public:
};
//-----------------------------------------------------------------------------
// exception API
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// MidiMsg API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MidiMsg_ctor );

extern t_CKUINT MidiMsg_offset_data1;
extern t_CKUINT MidiMsg_offset_data2;
extern t_CKUINT MidiMsg_offset_data3;
extern t_CKUINT MidiMsg_offset_when;

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




#endif
