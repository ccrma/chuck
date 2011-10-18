/*----------------------------------------------------------------------------
  miniAudicle
  Graphical ChucK audio programming environment

  Copyright (c) 2005 Spencer Salazar.  All rights reserved.
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
// file: miniAudicle_ui_elements.h
// desc: for supporting UI controls defined in ChucK code
//
// author: Spencer Salazar (ssalazar@princeton.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __MINIAUDICLE_UI_ELEMENTS_H__
#define __MINIAUDICLE_UI_ELEMENTS_H__

#include "chuck_def.h"
#include "chuck_oo.h"

#include <string>

//namespace miniAudicle
//{
namespace UI
{

// forward declaration
class Element;
class View;

// structures for maintaining platform dependent GUI data structures
#if defined( __MACOSX_CORE__ ) && __OBJC__
} /* UI */

@class mAUIElement;
@class mAUISlider;
@class mAUIButton;
@class mAUIView;
@class mAUILED;
@class mAUIText;
@class mAUIGauge;
@class mAUIPopupMenu;
@class mAUITextEditor;
@class mAUIImage;
@class mAUICanvas;

namespace UI
{
#else
struct mAUIElement;
struct mAUISlider;
struct mAUIButton;
struct mAUIView;
struct mAUILED;
struct mAUIText;
struct mAUIGauge;
struct mAUIPopupMenu;
struct mAUITextEditor;
struct mAUIImage;
struct mAUICanvas;
#endif

//-----------------------------------------------------------------------------
// name: Element
// desc: UI element super class
//-----------------------------------------------------------------------------
class Element
{
public:
    Element();
    virtual ~Element();
    virtual t_CKBOOL init() = 0;
    virtual t_CKBOOL display();
    virtual t_CKBOOL hide();
    virtual t_CKBOOL destroy();
    
    virtual t_CKBOOL set_name( std::string & name );
    virtual std::string get_name();
    
    virtual t_CKBOOL set_parent( View * p );
    virtual t_CKBOOL remove_parent( View * p );
    virtual View * get_parent();
    
    virtual t_CKBOOL set_size( t_CKFLOAT w, t_CKFLOAT h );
    virtual t_CKFLOAT get_width();
    virtual t_CKFLOAT get_height();
    
    virtual t_CKBOOL set_position( t_CKFLOAT x, t_CKFLOAT y );
    virtual t_CKFLOAT get_x();
    virtual t_CKFLOAT get_y();
    
    virtual t_CKBOOL set_margins( t_CKFLOAT l, t_CKFLOAT r, 
                                  t_CKFLOAT t, t_CKFLOAT b );
    virtual t_CKFLOAT get_left_margin();
    virtual t_CKFLOAT get_right_margin();
    virtual t_CKFLOAT get_top_margin();
    virtual t_CKFLOAT get_bottom_margin();
    
    virtual t_CKBOOL set_event( Chuck_Event * e );
    virtual Chuck_Event * get_event();
    
    // functions for implementing platform independent Element functionality
    virtual t_CKBOOL pi_display();
    virtual t_CKBOOL pi_hide();
    virtual t_CKBOOL pi_destroy();
    virtual t_CKBOOL pi_set_name( std::string & name );    
    virtual t_CKBOOL pi_set_parent( View * p );
    virtual t_CKBOOL pi_remove_parent( View * p );    
    virtual t_CKBOOL pi_set_size( t_CKFLOAT w, t_CKFLOAT h );
    virtual t_CKBOOL pi_set_position( t_CKFLOAT x, t_CKFLOAT y );
    
protected:
    std::string name;
    t_CKFLOAT x, y, w, h;
    t_CKFLOAT lmargin, rmargin, tmargin, bmargin;
    t_CKBOOL hidden;
    Chuck_Event * e;
    View * parent;
    
    mAUIElement * native_element;
    
private:
    t_CKBOOL default_parent;
};      

//------------------------------------------------------------------------------
// name: View
// desc: generic view element
//------------------------------------------------------------------------------
class View : public Element
{
public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL display();
    virtual t_CKBOOL hide();
    virtual t_CKBOOL destroy();
    
    virtual t_CKBOOL set_parent( View * p );
    
    virtual t_CKBOOL add_element( Element * e );
    virtual t_CKBOOL remove_element( Element * e );
    
protected:
        std::vector< Element * > elements;
    
public:
        mAUIView * native_view;
};

//-----------------------------------------------------------------------------
// name: Slider
// desc: UI slider element
//-----------------------------------------------------------------------------
class Slider : public Element
{
public:
    virtual t_CKBOOL init();
    
    virtual t_CKDOUBLE get_value();
    virtual t_CKBOOL set_value( t_CKDOUBLE v );
    
    virtual t_CKBOOL set_range( t_CKDOUBLE low, t_CKDOUBLE high );
    virtual t_CKDOUBLE get_max();
    virtual t_CKDOUBLE get_min();
    
    virtual t_CKBOOL set_precision( t_CKUINT precision );
    virtual t_CKUINT get_precision();
    
    typedef t_CKUINT display_format;
    const static display_format integer_format;
    const static display_format decimal_format;
    const static display_format scientific_format;
    const static display_format best_format;
    
    virtual t_CKBOOL set_display_format( display_format f );
    virtual display_format get_display_format();
     
     /*
     virtual t_CKBOOL set_ticks( t_CKINT t );
     virtual t_CKINT get_ticks();
     
     virtual t_CKBOOL set_ticks_only( t_CKBOOL to );
     virtual t_CKBOOL get_ticks_only( t_CKBOOL to );
     */
    
    virtual void slider_changed( t_CKDOUBLE v );
    
    const static t_CKUINT default_margin;
    const static t_CKUINT default_width;
    const static t_CKUINT default_slider_height;
    const static t_CKUINT default_text_height;
    const static t_CKUINT default_inner_margin;
    
protected:
    mAUISlider * native_slider;
    display_format df;
    t_CKUINT precision;
    t_CKDOUBLE value, max, min;
    
};


//-----------------------------------------------------------------------------
// name: Button
// desc: button with multiple appearances
//-----------------------------------------------------------------------------
class Button : public Element
{
public:
    virtual t_CKBOOL init();
    
    virtual t_CKBOOL get_state();
    virtual t_CKBOOL set_state( t_CKBOOL s );
    
    /*
     enum button_type
     {
         rect_type,
         circle_type,
         ui_type
     };
     virtual t_CKBOOL set_button_type( button_type t );
     virtual button_type get_button_type();
     */
    enum action_type
    {
        push_type,
        toggle_type
    };
    virtual t_CKBOOL set_action_type( action_type t );
    virtual action_type get_action_type();
     
    /* enum text_position
     {
         text_none,
         text_inside,
         text_bottom,
         text_top,
         text_left,
         text_right
     };
     virtual t_CKBOOL set_text_position( text_position p );
     virtual text_position get_text_position();
     */
    virtual void button_changed();
    
    static const t_CKUINT default_margin;
    static const t_CKUINT default_button_width;
    static const t_CKUINT default_button_height;
    
protected:
    mAUIButton * native_button;
    t_CKBOOL pushed;
    action_type at;
};

//------------------------------------------------------------------------------
// name: LED
// desc: LED type output
//------------------------------------------------------------------------------
class LED : public Element
{
public:
    virtual t_CKBOOL init();

    //virtual t_CKBOOL flash();
    virtual t_CKBOOL light();
    virtual t_CKBOOL unlight();
    /*
    virtual t_CKBOOL set_flash_time( t_CKFLOAT s );
    virtual t_CKDOUBLE get_flash_time();
    */
    
    // we need actual data members, not enums, for ChucK class static variables
    typedef t_CKUINT color;
    const static color red;
    const static color green;
    const static color blue;
    
    virtual t_CKBOOL set_color( color c );
    virtual color get_color();
    
    const static t_CKUINT default_margin;
    const static t_CKUINT default_led_width;
    const static t_CKUINT default_led_height;

private:
    t_CKFLOAT flash_time;
    color c;
    mAUILED * native_led;
};

//------------------------------------------------------------------------------
// name: Text
// desc: Text label element
//------------------------------------------------------------------------------
class Text : public Element
{
public:
    virtual t_CKBOOL init();
    
    const static t_CKUINT default_margin;
    const static t_CKUINT default_text_width;
    const static t_CKUINT default_text_height;
    
private:
    mAUIText * native_text;
};

//------------------------------------------------------------------------------
// name: Gauge
// desc: Gauge/progress bar element
//------------------------------------------------------------------------------
class Gauge : public Element
{
public:
    virtual t_CKBOOL init();
    
    virtual t_CKBOOL set_value( t_CKFLOAT _value ); // between 0 and 1
    virtual t_CKFLOAT get_value();
    
    const static t_CKUINT default_margin;
    const static t_CKUINT default_gauge_width;
    const static t_CKUINT default_gauge_height;

private:
    mAUIGauge * native_gauge;
    t_CKFLOAT value;
};

//------------------------------------------------------------------------------
// name: PopupMenu
// desc: popup menu element
//------------------------------------------------------------------------------
class PopupMenu : public Element
{
public:
    virtual t_CKBOOL init();
    
    virtual t_CKBOOL add_item( std::string & title, t_CKINT eid = -1 );
    virtual t_CKBOOL add_separator( t_CKINT eid = -1 );
    virtual t_CKBOOL add_menu( std::string & title, PopupMenu * menu, 
                               t_CKINT eid = -1 );
    virtual t_CKBOOL remove_item( t_CKINT eid );
    virtual t_CKBOOL remove_item( std::string & title );
    
    virtual t_CKINT get_selected_item();
    virtual std::string get_selected_string();
    
    virtual t_CKBOOL set_selected_item( t_CKINT eid );
    virtual t_CKBOOL set_selected_string( std::string & title );
    
    virtual std::string get_string_for_item( t_CKINT eid );
    virtual t_CKINT get_item_for_string( std::string & title );
    
    virtual t_CKINT get_count();
    
private:
    mAUIPopupMenu * native_popupmenu;
};

//------------------------------------------------------------------------------
// name: TextEditor
// desc: text editor element
//------------------------------------------------------------------------------
class TextEditor : public Element
{
public:
    virtual t_CKBOOL init();

    virtual std::string get_value();
    virtual t_CKBOOL set_value( std::string & s );
    
private:
    mAUITextEditor * native_text;
    std::string value;
};

class Image : public Element
{
public:
    virtual t_CKBOOL init();
 
    virtual t_CKBOOL set_image( std::string & filename );
    virtual std::string get_image();
        
private:
    mAUIImage * native_image;
    std::string filename;
};

class Pen
{
public:
    virtual t_CKBOOL init();
    virtual t_CKBOOL destroy();
    
    virtual t_CKBOOL set_width( t_CKFLOAT w );
    virtual t_CKINT get_width();
    
    typedef t_CKUINT color;
    
    const static color red;
    const static color green;
    const static color blue;
    const static color yellow;
    const static color purple;
    const static color orange;
    const static color black;
    const static color white;
    const static color grey;
    
    virtual t_CKBOOL set_color( color c );
    virtual color get_color();
    
    virtual void pen_down();
    virtual void pen_up();
    virtual t_CKBOOL move_to( t_CKFLOAT x, t_CKFLOAT y );
    virtual t_CKBOOL arc_to( t_CKFLOAT x, t_CKFLOAT y );

    virtual void commit();
    virtual void clear();
};

class Canvas : public Element
{
public:
    virtual t_CKBOOL init();
 
private:
    mAUICanvas * native_canvas;
};

} /* namespace UI */
//} /* namespace miniAudicle */
        
#endif // __MINIAUDICLE_UI_ELEMENTS_H__
