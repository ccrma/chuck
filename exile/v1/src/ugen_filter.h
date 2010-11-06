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
// file: ugen_filter.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __UGEN_FILTER_H__
#define __UGEN_FILTER_H__

#include "chuck_dl.h"


// query
DLL_QUERY filter_query( Chuck_DL_Query * query );

// filter
UGEN_CTOR filter_ctor( t_CKTIME now );
UGEN_DTOR filter_dtor( t_CKTIME now, void * data );
UGEN_TICK filter_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL filter_ctrl_coefs( t_CKTIME now, void * data, void * value );
UGEN_CGET filter_cget_coefs( t_CKTIME now, void * data, void * out );
UGEN_PMSG filter_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// biquad
UGEN_CTOR biquad_ctor( t_CKTIME now );
UGEN_DTOR biquad_dtor( t_CKTIME now, void * data );
UGEN_TICK biquad_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );

UGEN_CTRL biquad_ctrl_pfreq( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_pfreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_prad( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_prad( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_zfreq( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_zfreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_zrad( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_zrad( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_norm( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_norm( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_pregain( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_pregain( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_eqzs( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_eqzs( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_b1( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_b1( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_b2( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_b2( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_a0( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_a0( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_a1( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_a1( t_CKTIME now, void * data, void * value );
UGEN_CTRL biquad_ctrl_a2( t_CKTIME now, void * data, void * value );
UGEN_CGET biquad_cget_a2( t_CKTIME now, void * data, void * value );

// onepole
UGEN_CTOR onepole_ctor( t_CKTIME now );
UGEN_DTOR onepole_dtor( t_CKTIME now, void * data );
UGEN_TICK onepole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL onepole_ctrl_pole( t_CKTIME now, void * data, void * value );

// onezero
UGEN_CTOR onezero_ctor( t_CKTIME now );
UGEN_DTOR onezero_dtor( t_CKTIME now, void * data );
UGEN_TICK onezero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL onezero_ctrl_zero( t_CKTIME now, void * data, void * value );

// twopole
UGEN_CTOR twopole_ctor( t_CKTIME now );
UGEN_DTOR twopole_dtor( t_CKTIME now, void * data );
UGEN_TICK twopole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL twopole_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL twopole_ctrl_rad( t_CKTIME now, void * data, void * value );
UGEN_CTRL twopole_ctrl_norm( t_CKTIME now, void * data, void * value );

// twozero
UGEN_CTOR twozero_ctor( t_CKTIME now );
UGEN_DTOR twozero_dtor( t_CKTIME now, void * data );
UGEN_TICK twozero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL twozero_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL twozero_ctrl_rad( t_CKTIME now, void * data, void * value );

// gQ
UGEN_CTOR gQ_ctor( t_CKTIME now );
UGEN_DTOR gQ_dtor( t_CKTIME now, void * data );
UGEN_TICK gQ_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL gQ_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL gQ_ctrl_rad( t_CKTIME now, void * data, void * value );

// allpass
UGEN_CTOR allpass_ctor( t_CKTIME now );
UGEN_DTOR allpass_dtor( t_CKTIME now, void * data );
UGEN_TICK allpass_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG allpass_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// delay
UGEN_CTOR delay_ctor( t_CKTIME now );
UGEN_DTOR delay_dtor( t_CKTIME now, void * data );
UGEN_TICK delay_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL delay_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL delay_ctrl_max( t_CKTIME now, void * data, void * value );
UGEN_CTRL delay_ctrl_tap( t_CKTIME now, void * data, void * value );
UGEN_CTRL delay_ctrl_energy( t_CKTIME now, void * data, void * value );

// delayA
UGEN_CTOR delayA_ctor( t_CKTIME now );
UGEN_DTOR delayA_dtor( t_CKTIME now, void * data );
UGEN_TICK delayA_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL delayA_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL delayA_ctrl_max( t_CKTIME now, void * data, void * value );

// delayL
UGEN_CTOR delayL_ctor( t_CKTIME now );
UGEN_DTOR delayL_dtor( t_CKTIME now, void * data );
UGEN_TICK delayL_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_CTRL delayL_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL delayL_ctrl_max( t_CKTIME now, void * data, void * value );




#endif
