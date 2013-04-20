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
// file: util_hid.h
// desc: header file for joystick/mouse/keyboard support
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: spring 2006
//-----------------------------------------------------------------------------
#ifndef __UTIL_HID_H__
#define __UTIL_HID_H__




//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
struct HidMsg
{
    t_CKINT device_type; // device type
    t_CKINT device_num;  // device number
    t_CKINT type;        // message type
    t_CKINT eid;         // element id
    t_CKINT idata[4];    // int data
    t_CKFLOAT fdata[4];  // float data
    
#ifdef __cplusplus
    HidMsg()
    { this->clear(); }

    void clear()
    {
        memset( this, 0, sizeof(HidMsg) );
    }
#endif
};




/* device types */
extern const t_CKUINT CK_HID_DEV_NONE;
extern const t_CKUINT CK_HID_DEV_JOYSTICK;
extern const t_CKUINT CK_HID_DEV_MOUSE;
extern const t_CKUINT CK_HID_DEV_KEYBOARD;
extern const t_CKUINT CK_HID_DEV_WIIREMOTE;
extern const t_CKUINT CK_HID_DEV_TILTSENSOR;
extern const t_CKUINT CK_HID_DEV_TABLET;
extern const t_CKUINT CK_HID_DEV_MULTITOUCH;
extern const t_CKUINT CK_HID_DEV_COUNT;

/* message types */
extern const t_CKUINT CK_HID_JOYSTICK_AXIS;
extern const t_CKUINT CK_HID_BUTTON_DOWN;
extern const t_CKUINT CK_HID_BUTTON_UP;
extern const t_CKUINT CK_HID_JOYSTICK_HAT;
extern const t_CKUINT CK_HID_JOYSTICK_BALL;
extern const t_CKUINT CK_HID_MOUSE_MOTION;
extern const t_CKUINT CK_HID_MOUSE_WHEEL;
extern const t_CKUINT CK_HID_DEVICE_CONNECTED;
extern const t_CKUINT CK_HID_DEVICE_DISCONNECTED;
extern const t_CKUINT CK_HID_ACCELEROMETER;
extern const t_CKUINT CK_HID_WIIREMOTE_IR;
extern const t_CKUINT CK_HID_LED;
extern const t_CKUINT CK_HID_FORCE_FEEDBACK;
extern const t_CKUINT CK_HID_SPEAKER;
extern const t_CKUINT CK_HID_TABLET_PRESSURE;
extern const t_CKUINT CK_HID_TABLET_MOTION;
extern const t_CKUINT CK_HID_TABLET_ROTATION;
extern const t_CKUINT CK_HID_MULTITOUCH_TOUCH;
extern const t_CKUINT CK_HID_MSG_COUNT;

/* keys */

/* extension API */

enum HidResult
{
    HID_GENERALERROR = -1,
    HID_NOERROR = 0,
};

struct _Chuck_Hid_Driver
{
    void ( *init )();
    void ( *quit )();
    void ( *poll )();
    void ( *probe )();
    int ( *count )();
    int ( *count_elements )( int, int );
    int ( *open )( int );
    int ( *open_async )( int );
    int ( *close )( int );
    int ( *send )( int, const HidMsg * );
    int ( *read )( int, int, int, HidMsg * );
    const char * ( *name )( int );
    const char * driver_name;
};
typedef struct _Chuck_Hid_Driver Chuck_Hid_Driver;

/* functions */
extern void Hid_init();
extern void Hid_poll();
extern void Hid_quit();

extern void Joystick_init();
extern void Joystick_poll();
extern void Joystick_quit();
extern void Joystick_probe();
extern int Joystick_count();
extern int Joystick_count_elements( int js, int type );
extern int Joystick_open( int js );
extern int Joystick_open_async( int js );
extern int Joystick_open( const char * name );
extern int Joystick_close( int js );
extern int Joystick_send( int js, const HidMsg * msg );
extern const char * Joystick_name( int js );

extern int Joystick_axes( int js );
extern int Joystick_buttons( int js );
extern int Joystick_hats( int js );

extern void Mouse_init();
extern void Mouse_poll();
extern void Mouse_quit();
extern void Mouse_probe();
extern int Mouse_count();
extern int Mouse_count_elements( int js, int type );
extern int Mouse_open( int m );
extern int Mouse_open( const char * name );
extern int Mouse_close( int m );
extern int Mouse_send( int m, const HidMsg * msg );
extern const char * Mouse_name( int m );
extern int Mouse_buttons( int m );
extern int Mouse_start_cursor_track();
extern int Mouse_stop_cursor_track();
    
extern void Keyboard_init();
extern void Keyboard_poll();
extern void Keyboard_quit();
extern void Keyboard_probe();
extern int Keyboard_count();
extern int Keyboard_count_elements( int js, int type );
extern int Keyboard_open( int kb );
extern int Keyboard_open( const char * name );
extern int Keyboard_close( int kb );
extern int Keyboard_send( int kb, const HidMsg * msg );
extern const char * Keyboard_name( int kb );

extern void WiiRemote_init();
extern void WiiRemote_poll();
extern void WiiRemote_quit();
extern void WiiRemote_probe();
extern int WiiRemote_count();
extern int WiiRemote_open( int wr );
extern int WiiRemote_open( const char * name );
extern int WiiRemote_close( int wr );
extern int WiiRemote_send( int wr, const HidMsg * msg );
extern const char * WiiRemote_name( int wr );

extern void TiltSensor_init();
extern void TiltSensor_quit();
extern void TiltSensor_probe();
extern int TiltSensor_count();
extern int TiltSensor_open( int ts );
extern int TiltSensor_close( int ts );
extern int TiltSensor_read( int ts, int type, int num, HidMsg * msg );
extern const char * TiltSensor_name( int ts );
// ge: SMS multi-thread poll rate
t_CKINT TiltSensor_setPollRate( t_CKINT usec );
t_CKINT TiltSensor_getPollRate( );


extern void MultiTouchDevice_init();
extern void MultiTouchDevice_quit();
extern void MultiTouchDevice_probe();
extern int MultiTouchDevice_count();
extern int MultiTouchDevice_open( int ts );
extern int MultiTouchDevice_close( int ts );
extern const char * MultiTouchDevice_name( int ts );


extern void Tablet_init();
extern void Tablet_quit();
extern void Tablet_probe();
extern int Tablet_count();
extern int Tablet_open( int ts );
extern int Tablet_close( int ts );
extern const char * Tablet_name( int ts );




#endif
