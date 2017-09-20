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
// file: ugen_osc.h
// desc: oscilliator unit generators
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Dan Trueman (dtrueman@princeton.edu)
// date: Summer 2004
//-----------------------------------------------------------------------------
#ifndef __UGEN_OSC_H__
#define __UGEN_OSC_H__

#include "chuck_dl.h"


// query
DLL_QUERY osc_query( Chuck_DL_Query * query );

// osc - base
CK_DLL_CTOR( osc_ctor );
CK_DLL_DTOR( osc_dtor );
CK_DLL_TICK( osc_tick );
CK_DLL_PMSG( osc_pmsg );
CK_DLL_CTRL( osc_ctrl_freq );
CK_DLL_CGET( osc_cget_freq );
CK_DLL_CTRL( osc_ctrl_period );
CK_DLL_CGET( osc_cget_period );
CK_DLL_CTRL( osc_ctrl_phase );
CK_DLL_CGET( osc_cget_phase );
CK_DLL_CTRL( osc_ctrl_width );
CK_DLL_CGET( osc_cget_width );
CK_DLL_CTRL( osc_ctrl_sync );
CK_DLL_CGET( osc_cget_sync );

// sinosc
CK_DLL_TICK( sinosc_tick );

// pulseosc
CK_DLL_TICK( pulseosc_tick );

// triosc
CK_DLL_TICK( triosc_tick );

// sawosc 
CK_DLL_CTOR( sawosc_ctor );
CK_DLL_CTRL( sawosc_ctrl_width );

// sqrosc 
CK_DLL_CTOR( sqrosc_ctor );
CK_DLL_CTRL( sqrosc_ctrl_width );


//-----------------------------------------------------------------------------
// file: ugen_genX
// desc: thought it would be a good way to learn the fascinating innards of
//       ChucK by porting some of the classic lookup table functions and adding
//       a few new ones that might be of use.
//       mostly ported from RTcmix (all by WarpTable)
//
// author: Dan Trueman (dtrueman.princeton.edu)
// date: Winter 2007
//-----------------------------------------------------------------------------
// called by xxx_query
DLL_QUERY genX_query( Chuck_DL_Query * query );

// genX - base
CK_DLL_CTOR( genX_ctor );
CK_DLL_DTOR( genX_dtor );
CK_DLL_TICK( genX_tick );
CK_DLL_PMSG( genX_pmsg );
CK_DLL_CTRL( genX_lookup );
CK_DLL_CTRL( genX_coeffs );
CK_DLL_CTRL( gen5_coeffs );
CK_DLL_CTRL( gen7_coeffs );
CK_DLL_CTRL( gen9_coeffs );
CK_DLL_CTRL( gen10_coeffs );
CK_DLL_CTRL( gen17_coeffs );
CK_DLL_CTRL( curve_coeffs );
CK_DLL_CTRL( warp_coeffs );

t_CKDOUBLE _asymwarp( t_CKDOUBLE inval, t_CKDOUBLE k );
t_CKDOUBLE _symwarp( t_CKDOUBLE inval, t_CKDOUBLE k );




#endif
