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
// file: ugen_osc.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Summer 2004
//-----------------------------------------------------------------------------
#ifndef __UGEN_OSC_H__
#define __UGEN_OSC_H__

#include "chuck_dl.h"


// query
DLL_QUERY osc_query( Chuck_DL_Query * query );

// oscs
UGEN_CTOR osc_ctor( t_CKTIME now );
UGEN_CTOR phasor_ctor( t_CKTIME now );
UGEN_DTOR osc_dtor( t_CKTIME now, void * data );
UGEN_TICK osc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_TICK osc_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL osc_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET osc_cget_freq( t_CKTIME now, void * data, void * out );
// sfreq &  phase controls  - pld
UGEN_CTRL osc_ctrl_sfreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL osc_ctrl_phase( t_CKTIME now, void * data, void * value );
UGEN_CGET osc_cget_phase( t_CKTIME now, void * data, void * out );
UGEN_CTRL osc_ctrl_phase_offset( t_CKTIME now, void * data, void * value );
UGEN_CGET osc_cget_phase_offset( t_CKTIME now, void * data, void * out );
UGEN_CTRL osc_ctrl_sync( t_CKTIME now, void * data, void * value );
UGEN_CGET osc_cget_sync( t_CKTIME now, void * data, void * out );
UGEN_CTRL osc_ctrl_width( t_CKTIME now, void * data, void * value );
UGEN_CGET osc_cget_width( t_CKTIME now, void * data, void * out );

UGEN_TICK sinosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL sinosc_ctrl_phase( t_CKTIME now, void * data, void * value );
UGEN_CGET sinosc_cget_phase( t_CKTIME now, void * data, void * out );

UGEN_TICK triosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_TICK sawosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_TICK pulseosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_TICK sqrosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );


#endif
