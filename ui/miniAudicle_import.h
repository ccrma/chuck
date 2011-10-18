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
// name: miniAudicle_import.h
// desc: miniAudicle class library base
//
// authors: Spencer Salazar (ssalazar@cs.princeton.edu)
//          Ge Wang (gewang@cs.princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#ifndef __MINI_AUDICLE_IMPORT_H__
#define __MINI_AUDICLE_IMPORT_H__

#include "chuck_def.h"
#include "chuck_dl.h"

//namespace miniAudicle
//{

// class initialization
t_CKBOOL init_maui( Chuck_Env * env );

//-----------------------------------------------------------------------------
// MAUIElement
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauielement_ctor );
CK_DLL_DTOR( mauielement_dtor );
CK_DLL_MFUN( mauielement_display );
CK_DLL_MFUN( mauielement_hide );
CK_DLL_MFUN( mauielement_destroy );
CK_DLL_MFUN( mauielement_name_set );
CK_DLL_MFUN( mauielement_name_get );
CK_DLL_MFUN( mauielement_size );
CK_DLL_MFUN( mauielement_height );
CK_DLL_MFUN( mauielement_width );
CK_DLL_MFUN( mauielement_position );
CK_DLL_MFUN( mauielement_x );
CK_DLL_MFUN( mauielement_y );
CK_DLL_MFUN( mauielement_onchange );

//-----------------------------------------------------------------------------
// MAUISlider API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( maslider_ctor );
CK_DLL_DTOR( maslider_dtor );
CK_DLL_MFUN( maslider_value_set );
CK_DLL_MFUN( maslider_value_get );
CK_DLL_MFUN( maslider_range );
CK_DLL_MFUN( maslider_frange );
CK_DLL_MFUN( maslider_irange );
CK_DLL_MFUN( maslider_max );
CK_DLL_MFUN( maslider_min );
CK_DLL_MFUN( maslider_set_precision );
CK_DLL_MFUN( maslider_get_precision );
CK_DLL_MFUN( maslider_set_display_format );
CK_DLL_MFUN( maslider_get_display_format );

//-----------------------------------------------------------------------------
// MAUIView API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauiview_ctor );
CK_DLL_DTOR( mauiview_dtor );
CK_DLL_MFUN( mauiview_add_element );

//-----------------------------------------------------------------------------
// MAUIButton API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauibutton_ctor );
CK_DLL_DTOR( mauibutton_dtor );
CK_DLL_MFUN( mauibutton_set_state );
CK_DLL_MFUN( mauibutton_get_state );
CK_DLL_MFUN( mauibutton_push_type );
CK_DLL_MFUN( mauibutton_toggle_type );

//-----------------------------------------------------------------------------
// MAUILED API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauiled_ctor );
CK_DLL_DTOR( mauiled_dtor );
CK_DLL_MFUN( mauiled_light );
CK_DLL_MFUN( mauiled_unlight );
CK_DLL_MFUN( mauiled_get_color );
CK_DLL_MFUN( mauiled_set_color );

//-----------------------------------------------------------------------------
// MAUI_Text API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauitext_ctor );
CK_DLL_DTOR( mauitext_dtor );

//-----------------------------------------------------------------------------
// MAUI_Gauge API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( mauigauge_ctor );
CK_DLL_DTOR( mauigauge_dtor );
CK_DLL_MFUN( mauigauge_set_value );
CK_DLL_MFUN( mauigauge_get_value );

//}

#endif
