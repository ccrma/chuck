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
// name: miniAudicle_import.cpp
// desc: miniAudicle class library base
//
// authors: Spencer Salazar (ssalazar@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------

#include "miniAudicle_import.h"
#include "miniAudicle_ui_elements.h"
#include "chuck_type.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"

//namespace miniAudicle
//{

t_CKBOOL init_class_mauielement( Chuck_Env * env );
t_CKBOOL init_class_maslider( Chuck_Env * env );
t_CKBOOL init_class_mauiview( Chuck_Env * env );
t_CKBOOL init_class_mauibutton( Chuck_Env * env );
t_CKBOOL init_class_mauiled( Chuck_Env * env );
t_CKBOOL init_class_mauitext( Chuck_Env * env );
t_CKBOOL init_class_mauigauge( Chuck_Env * env );

static t_CKINT mauielement_offset_data = 0;

//-----------------------------------------------------------------------------
// name: init_maui()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_maui( Chuck_Env * env )
{
    // log
    EM_log( CK_LOG_SEVERE, "loading miniAudicle UI (MAUI) API..." );
    // push indent level
    EM_pushlog();

    // make context
    Chuck_Context * context = type_engine_make_context( NULL, "@[MAUI]" );
    // reset env - not needed since we just created the env
    env->reset();
    // load it
    type_engine_load_context( env, context );

    // go
    //EM_log( CK_LOG_SEVERE, "module MAUI_Element..." );    
    if( !init_class_mauielement( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_Slider..." );
    if( !init_class_maslider( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_View..." );
    if( !init_class_mauiview( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_Button..." );
    if( !init_class_mauibutton( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_LED..." );
    if( !init_class_mauiled( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_Text..." );
    if( !init_class_mauitext( env ) ) goto error;
    //EM_log( CK_LOG_SEVERE, "module MAUI_Gauge..." );
    if( !init_class_mauigauge( env ) ) goto error;

    // clear context
    type_engine_unload_context( env );

    // commit what is in the type checker at this point
    env->global()->commit();

    // pop indent level
    EM_poplog();

    return TRUE;

error:

    // probably dangerous: rollback
    env->global()->rollback();

    // clear context
    type_engine_unload_context( env );

    // pop indent level
    EM_poplog();

    return FALSE;
}

// MAUI_Element implementation
#pragma mark --MAUI_Element--
//-----------------------------------------------------------------------------
// name: init_class_mauielement()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauielement( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_Element'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_Element", "Event",
                                         env->global(), mauielement_ctor, 
                                         mauielement_dtor ) )
        return FALSE;
    
    // add member
    mauielement_offset_data = type_engine_import_mvar( env, "int", "@data", FALSE );
    if( mauielement_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // add display()
    func = make_new_mfun( "void", "display", mauielement_display );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add hide()
    func = make_new_mfun( "void", "hide", mauielement_hide );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add destroy()
    func = make_new_mfun( "void", "destroy", mauielement_destroy );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add name()
    func = make_new_mfun( "string", "name", mauielement_name_set );
    func->add_arg( "string", "n" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add name()
    func = make_new_mfun( "string", "name", mauielement_name_get );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add size()
    func = make_new_mfun( "void", "size", mauielement_size );
    func->add_arg( "float", "w" );
    func->add_arg( "float", "h" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    
    // add width()
    func = make_new_mfun( "float", "width", mauielement_width );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add height()
    func = make_new_mfun( "float", "height", mauielement_height );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add position()
    func = make_new_mfun( "void", "position", mauielement_position );
    func->add_arg( "float", "x" );
    func->add_arg( "float", "y" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add x()
    func = make_new_mfun( "float", "x", mauielement_x );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add y()
    func = make_new_mfun( "float", "y", mauielement_y );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add onChange()
    func = make_new_mfun( "Event", "onChange", mauielement_onchange );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        
        // end the class import
        type_engine_import_class_end( env );
    
    return FALSE;
}

// constructor
CK_DLL_CTOR( mauielement_ctor )
{ }

// destructor
CK_DLL_DTOR( mauielement_dtor )
{ }

// display
CK_DLL_MFUN( mauielement_display )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    if( maui )
    {
        maui->display();
    }
}

// hide
CK_DLL_MFUN( mauielement_hide )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    if( maui )
        maui->hide();
}

// destroy
CK_DLL_MFUN( mauielement_destroy )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    if( maui )
        maui->destroy();
}

// name
CK_DLL_MFUN( mauielement_name_set )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    Chuck_String * s = GET_NEXT_STRING(ARGS);
    if( maui && s )
    {
        maui->set_name( s->str );
        RETURN->v_string = s;
    }
    else
    {
        RETURN->v_string = NULL;
    }
}

// name
CK_DLL_MFUN( mauielement_name_get )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_string = new Chuck_String( maui->get_name() );
}

// size
CK_DLL_MFUN( mauielement_size )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKFLOAT w = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT h = GET_NEXT_FLOAT(ARGS);
    maui->set_size( (t_CKDOUBLE)w, (t_CKDOUBLE)h );
}

// height
CK_DLL_MFUN( mauielement_height )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = maui->get_height();
}

// width
CK_DLL_MFUN( mauielement_width )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = maui->get_width();
}

// position
CK_DLL_MFUN( mauielement_position )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    maui->set_position( (t_CKDOUBLE)x, (t_CKDOUBLE)y );
}

// x
CK_DLL_MFUN( mauielement_x )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = maui->get_x();
}

// y
CK_DLL_MFUN( mauielement_y )
{
    UI::Element * maui = (UI::Element *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = maui->get_y();
}

// onChange
CK_DLL_MFUN( mauielement_onchange )
{
//    UI::Element * maui = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_object = SELF;
}

// MAUI_Slider implementation
#pragma mark --MAUI_Slider--
//-----------------------------------------------------------------------------
// name: init_class_maslider()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_maslider( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    //    Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_Slider'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_Slider", "MAUI_Element",
                                         env->global(), maslider_ctor, 
                                         maslider_dtor ) )
        return FALSE;
    
    // add value()
    func = make_new_mfun( "float", "value", maslider_value_set );
    func->add_arg( "float", "v" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add value()
    func = make_new_mfun( "float", "value", maslider_value_get );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add range()
    func = make_new_mfun( "void", "range", maslider_range );
    func->add_arg( "float", "min" );
    func->add_arg( "float", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add frange()
    func = make_new_mfun( "void", "frange", maslider_frange );
    func->add_arg( "float", "min" );
    func->add_arg( "float", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add irange()
    func = make_new_mfun( "void", "irange", maslider_irange );
    func->add_arg( "int", "min" );
    func->add_arg( "int", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add max()
    func = make_new_mfun( "float", "max", maslider_max );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add min()
    func = make_new_mfun( "float", "min", maslider_min );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add get_precision()
    func = make_new_mfun( "int", "precision", maslider_get_precision );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add set_precision()
    func = make_new_mfun( "int", "precision", maslider_set_precision );
    func->add_arg( "int", "p" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add get_display_format()
    func = make_new_mfun( "int", "displayFormat", maslider_get_display_format );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add set_display_format()
    func = make_new_mfun( "int", "displayFormat", maslider_set_display_format );
    func->add_arg( "int", "df" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add get_orientation()
    func = make_new_mfun( "int", "orientation", maslider_get_orientation );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add set_orientation()
    func = make_new_mfun( "int", "orientation", maslider_set_orientation );
    func->add_arg( "int", "o" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add bestFormat
    if( !type_engine_import_svar( env, "int", "bestFormat", TRUE, (t_CKUINT)&UI::Slider::best_format ) )
        goto error;
    
    // add decimalFormat
    if( !type_engine_import_svar( env, "int", "decimalFormat", TRUE, (t_CKUINT)&UI::Slider::decimal_format ) )
        goto error;
    
    // add scientificFormat
    if( !type_engine_import_svar( env, "int", "scientificFormat", TRUE, (t_CKUINT)&UI::Slider::scientific_format ) )
        goto error;
    
    // add integerFormat
    if( !type_engine_import_svar( env, "int", "integerFormat", TRUE, (t_CKUINT)&UI::Slider::integer_format ) )
        goto error;
    
    // add vertical
    if( !type_engine_import_svar( env, "int", "vertical", TRUE, (t_CKUINT)&UI::Slider::vertical ) )
        goto error;
    
    // add horizontal
    if( !type_engine_import_svar( env, "int", "horizontal", TRUE, (t_CKUINT)&UI::Slider::horizontal ) )
        goto error;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        
        // wrap up
        type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( maslider_ctor )
{
    UI::Slider * slider = new UI::Slider;
    
    slider->init();
    slider->set_event( (Chuck_Event *)SELF );

    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)slider;
}

// dtor
CK_DLL_DTOR( maslider_dtor )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    slider->destroy();
    delete slider;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// value
CK_DLL_MFUN( maslider_value_set )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    slider->set_value( v );
    RETURN->v_float = slider->get_value();
}

// value
CK_DLL_MFUN( maslider_value_get )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = slider->get_value();
}

// range
CK_DLL_MFUN( maslider_range )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKFLOAT lo = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT hi = GET_NEXT_FLOAT(ARGS);
    slider->set_range( lo, hi );
}

// frange
CK_DLL_MFUN( maslider_frange )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKFLOAT lo = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT hi = GET_NEXT_FLOAT(ARGS);
    slider->set_range( lo, hi );
}

// irange
CK_DLL_MFUN( maslider_irange )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    t_CKINT lo = GET_NEXT_INT(ARGS);
    t_CKINT hi = GET_NEXT_INT(ARGS);
    slider->set_range( (t_CKFLOAT)lo, (t_CKFLOAT)hi );
}

// max
CK_DLL_MFUN( maslider_max )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = slider->get_max();
}

// min
CK_DLL_MFUN( maslider_min )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = slider->get_min();
}

// get_precision
CK_DLL_MFUN( maslider_get_precision )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_int = slider->get_precision();
}

// set_precision
CK_DLL_MFUN( maslider_set_precision )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    slider->set_precision( GET_NEXT_INT( ARGS ) );
    RETURN->v_int = slider->get_precision();
}

// get_display_format
CK_DLL_MFUN( maslider_get_display_format )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_int = slider->get_display_format();
}

// set_display_format
CK_DLL_MFUN( maslider_set_display_format )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    slider->set_display_format( GET_NEXT_INT( ARGS ) );
    RETURN->v_int = slider->get_display_format();
}

// get_orientation
CK_DLL_MFUN( maslider_get_orientation )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_int = slider->get_orientation();
}

// set_orientation
CK_DLL_MFUN( maslider_set_orientation )
{
    UI::Slider * slider = (UI::Slider *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    slider->set_orientation( GET_NEXT_INT( ARGS ) );
    RETURN->v_int = slider->get_orientation();
}

// MAUI_View implementation
#pragma mark --MAUI_View--
//-----------------------------------------------------------------------------
// name: init_class_mauiview()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauiview( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    //    Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_View'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_View", "MAUI_Element",
                                         env->global(), mauiview_ctor, 
                                         mauiview_dtor ) )
        return FALSE;
    
    // add add_element()
    func = make_new_mfun( "void", "addElement", mauiview_add_element );
    func->add_arg( "MAUI_Element", "e" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        
        // wrap up
        type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( mauiview_ctor )
{
    UI::View * view = new UI::View;
    view->init();
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)view;
    view->set_event( (Chuck_Event *)SELF );
}

// dtor
CK_DLL_DTOR( mauiview_dtor )
{
    UI::View * view = (UI::View *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    view->destroy();
    delete view;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// add_element
CK_DLL_MFUN( mauiview_add_element )
{
    UI::View * view = (UI::View *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    UI::Element * e = ( UI::Element * )OBJ_MEMBER_INT(GET_NEXT_OBJECT(ARGS),
                                                      mauielement_offset_data );
    view->add_element( e );
}

// MAUI_Button implementation
#pragma mark --MAUI_Button--
//-----------------------------------------------------------------------------
// name: init_class_mauibutton()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauibutton( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    //    Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_Button'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_Button", "MAUI_Element",
                                         env->global(), mauibutton_ctor, 
                                         mauibutton_dtor ) )
        return FALSE;
    
    // add get_state()
    func = make_new_mfun( "int", "state", mauibutton_get_state );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add set_state()
    func = make_new_mfun( "int", "state", mauibutton_set_state );
    func->add_arg( "int", "s" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add pushType()
    func = make_new_mfun( "void", "pushType", mauibutton_push_type );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add toggleType()
    func = make_new_mfun( "void", "toggleType", mauibutton_toggle_type );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add unsetImage()
    func = make_new_mfun( "void", "unsetImage", mauibutton_unset_image );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add setImage()
    func = make_new_mfun( "int", "setImage", mauibutton_set_image );
    func->add_arg( "string", "filepath" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        // wrap up
        type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( mauibutton_ctor )
{
    UI::Button * button = new UI::Button;
    button->init();
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)button;
    button->set_event( (Chuck_Event *)SELF );
}

// dtor
CK_DLL_DTOR( mauibutton_dtor )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    button->destroy();
    delete button;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// state
CK_DLL_MFUN( mauibutton_get_state )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_int = button->get_state();
}

// state
CK_DLL_MFUN( mauibutton_set_state )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    button->set_state( GET_NEXT_INT(ARGS) );
    RETURN->v_int = button->get_state();
}

// push_type
CK_DLL_MFUN( mauibutton_push_type )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    button->set_action_type( UI::Button::push_type );
}

// toggle_type
CK_DLL_MFUN( mauibutton_toggle_type )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    button->set_action_type( UI::Button::toggle_type );
}

CK_DLL_MFUN( mauibutton_set_image )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    Chuck_String * s = GET_NEXT_STRING(ARGS);
    RETURN->v_int = button->set_image(s->str);
}

CK_DLL_MFUN( mauibutton_unset_image )
{
    UI::Button * button = (UI::Button *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    button->unset_image();
}

// MAUI_LED implementation
#pragma mark --MAUI_LED--
//-----------------------------------------------------------------------------
// name: init_class_mauiled()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauiled( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    //    Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_LED'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_LED", "MAUI_Element",
                                         env->global(), mauiled_ctor,
                                         mauiled_dtor ) )
        return FALSE;
    
    // add light()
    func = make_new_mfun( "void", "light", mauiled_light );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add unlight()
    func = make_new_mfun( "void", "unlight", mauiled_unlight );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add color()
    func = make_new_mfun( "int", "color", mauiled_get_color );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add color()
    func = make_new_mfun( "int", "color", mauiled_set_color );
    func->add_arg( "int", "c" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add red
    if( !type_engine_import_svar( env, "int", "red", TRUE, (t_CKUINT)&UI::LED::red ) )
        goto error;
    
    // add green
    if( !type_engine_import_svar( env, "int", "green", TRUE, (t_CKUINT)&UI::LED::green ) )
        goto error;
    
    // add blue
    if( !type_engine_import_svar( env, "int", "blue", TRUE, (t_CKUINT)&UI::LED::blue ) )
        goto error;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        // wrap up
        type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( mauiled_ctor )
{
    UI::LED * led = new UI::LED;
    led->init();
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)led;
    led->set_event( (Chuck_Event *)SELF );
}

// dtor
CK_DLL_DTOR( mauiled_dtor )
{
    UI::LED * led = (UI::LED *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    led->destroy();
    delete led;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// light
CK_DLL_MFUN( mauiled_light )
{
    UI::LED * led = (UI::LED *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    led->light();
}

// unlight
CK_DLL_MFUN( mauiled_unlight )
{
    UI::LED * led = (UI::LED *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    led->unlight();
}

// get_color
CK_DLL_MFUN( mauiled_get_color )
{
    UI::LED * led = (UI::LED *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_int = led->get_color();
}

// set_color
CK_DLL_MFUN( mauiled_set_color )
{
    UI::LED * led = (UI::LED *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    led->set_color( ( UI::LED::color ) GET_NEXT_INT( ARGS ) );
    RETURN->v_int = led->get_color();
}

// MAUI_Text implementation
#pragma mark --MAUI_Text--
//-----------------------------------------------------------------------------
// name: init_class_mauitext()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauitext( Chuck_Env * env )
{
    //    Chuck_DL_Func * func = NULL;
    //    Chuck_Value * value = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_Text'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_Text", "MAUI_Element",
                                         env->global(), mauitext_ctor,
                                         mauitext_dtor ) )
        return FALSE;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
        // wrap up
        type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( mauitext_ctor )
{
    UI::Text * text = new UI::Text;
    text->init();
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)text;
    text->set_event( (Chuck_Event *)SELF );
}

// dtor
CK_DLL_DTOR( mauitext_dtor )
{
    UI::Text * text = (UI::Text *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    text->destroy();
    delete text;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// MAUI_Gauge implementation
#pragma mark --MAUI_Gauge--
//-----------------------------------------------------------------------------
// name: init_class_mauigauge()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL init_class_mauigauge( Chuck_Env * env )
{
    Chuck_DL_Func * func = NULL;
    
    // log
    EM_log( CK_LOG_INFO, "class 'MAUI_Gauge'" );
    
    // import
    if( !type_engine_import_class_begin( env, "MAUI_Gauge", "MAUI_Element",
                                         env->global(), mauigauge_ctor,
                                         mauigauge_dtor ) )
        return FALSE;
    
    // add (set) value()
    func = make_new_mfun( "void", "value", mauigauge_set_value );
    func->add_arg( "float", "v" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add (get) value()
    func = make_new_mfun( "void", "value", mauigauge_get_value );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // wrap up
    type_engine_import_class_end( env );
    
    return TRUE;
    
error:
    // wrap up
    type_engine_import_class_end( env );
    
    return FALSE;
}

// ctor
CK_DLL_CTOR( mauigauge_ctor )
{
    UI::Gauge * gauge = new UI::Gauge;
    gauge->init();
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = (t_CKINT)gauge;
    gauge->set_event( (Chuck_Event *)SELF );
}

// dtor
CK_DLL_DTOR( mauigauge_dtor )
{
    UI::Gauge * gauge = (UI::Gauge *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    gauge->destroy();
    delete gauge;
    OBJ_MEMBER_INT(SELF, mauielement_offset_data) = 0;
}

// set_value
CK_DLL_MFUN( mauigauge_set_value )
{
    UI::Gauge * gauge = (UI::Gauge *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    gauge->set_value( GET_NEXT_FLOAT( ARGS ) );
    RETURN->v_float = gauge->get_value();
}

// get_value
CK_DLL_MFUN( mauigauge_get_value )
{
    UI::Gauge * gauge = (UI::Gauge *)OBJ_MEMBER_INT(SELF, mauielement_offset_data);
    RETURN->v_float = gauge->get_value();
}


//}