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
// file: miniAudicle_ui_elements.cpp
// desc: OS-agnostic UI elements infrastructure
//
// author: Spencer Salazar (ssalazar@princeton.edu)
// date: Summer 2006
//-----------------------------------------------------------------------------

#include "miniAudicle_ui_elements.h"
#include "chuck_errmsg.h"

using namespace std;

//namespace miniAudicle
//{
namespace UI
{

Element::Element()
{
    parent = NULL;
    default_parent = FALSE;
    e = NULL;
    hidden = FALSE;
    x = y = 0;
    w = h = 0;
    lmargin = rmargin = tmargin = bmargin = 0;
}

Element::~Element()
{
}

t_CKBOOL Element::pi_display()
{
    if( parent == NULL )
    {
        default_parent = TRUE;
        parent = new View();
        parent->init();
        parent->set_size( w, h );
        parent->set_position( x, y );
        parent->add_element( this );
        parent->display();
    }
    
    if( hidden )
    {
        if( parent && default_parent )
        {
            parent->display();
        }
        
        hidden = FALSE;
    }
    
    return TRUE;
}

t_CKBOOL Element::pi_hide()
{
    if( !hidden )
    {
        if( parent && default_parent )
        {
            parent->hide();
        }
        
        hidden = TRUE;
    }
    
    return TRUE;
}

t_CKBOOL Element::pi_destroy()
{
    if( default_parent && parent != NULL )
    {
        parent->destroy();
        delete parent;
        parent = NULL;
        default_parent = FALSE;
    }
    
    if( parent )
    {
        parent->remove_element( this );
        parent = NULL;
    }
    
    return TRUE;
}

t_CKBOOL Element::pi_set_name( string & name )
{
    this->name = name;
    return true;
}

string Element::get_name()
{
    return name;
}

t_CKBOOL Element::pi_set_size( t_CKFLOAT w, t_CKFLOAT h )
{
    if( w >= 0 )
        this->w = w;
    
    if( h >= 0 )
        this->h = h;
    
    if( default_parent && parent != NULL )
        // resize the default parent to match
        parent->set_size( w, h );
    
    return TRUE;
}

t_CKFLOAT Element::get_width()
{
    return w;
}

t_CKFLOAT Element::get_height()
{
    return h;
}

t_CKBOOL Element::pi_set_position( t_CKFLOAT x, t_CKFLOAT y )
{
    if( default_parent && parent != NULL )
        // no need to move the actual element, just move its private containing view
        parent->set_position( x, y );
    
    else
    {
        if( x >= 0 )
            this->x = x;
        
        if( y >= 0 )
            this->y = y;
    }
    
    return TRUE;
}

t_CKFLOAT Element::get_x()
{
    return x;
}

t_CKFLOAT Element::get_y()
{
    return y;
}

t_CKBOOL Element::set_margins( t_CKFLOAT l, t_CKFLOAT r, 
                               t_CKFLOAT t, t_CKFLOAT b )
{
    return TRUE;
}

t_CKFLOAT Element::get_left_margin()
{
    return lmargin;
}

t_CKFLOAT Element::get_right_margin()
{
    return rmargin;
}

t_CKFLOAT Element::get_top_margin()
{
    return tmargin;
}

t_CKFLOAT Element::get_bottom_margin()
{
    return bmargin;
}

t_CKBOOL Element::pi_set_parent( View * p )
{
    if( parent != p )
    {
        if( parent != NULL )
            remove_parent( parent );
        
        parent = p;
    }
    
    return TRUE;
}

t_CKBOOL Element::pi_remove_parent( View * p )
{
    if( default_parent && parent != NULL )
    {
        default_parent = FALSE;
        /* WARNING: HUGE HACKS FOLLOW */
        /* set this now! so that we dont get infinite recursion when 
        parent->destroy() calls remove_parent on this
        */
        View * _parent = parent;
        parent = NULL;
        /* If we don't create a temporary variable to hold parent, we will be 
        unable to delete parent.  When we call parent->destroy, it will call 
        remove_parent on this.  that remove_parent will set parent = NULL, so we
        need to have a temporary variable to hold on to the parent until we can
        delete it.  
        */
        _parent->destroy();
        delete _parent;
    }

    parent = NULL;
    
    return TRUE;
}

View * Element::get_parent()
{
    return parent;
}

t_CKBOOL Element::set_event( Chuck_Event * e)
{
    this->e = e;
    return TRUE;
}

Chuck_Event * Element::get_event()
{
    return e;
}

const Slider::display_format Slider::integer_format = 0;
const Slider::display_format Slider::decimal_format = 1;
const Slider::display_format Slider::scientific_format = 2;
const Slider::display_format Slider::best_format = 3;

const LED::color LED::red = 0;
const LED::color LED::green = 1;
const LED::color LED::blue = 2;

}
//}




