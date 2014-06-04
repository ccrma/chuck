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
// file: ugen_xxx.h
// desc: non-specific unit generators
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Dan Trueman (dtrueman@princeton.edu)
//         Matt Hoffman (for Dyno)
// date: Spring 2004
//       Summer 2005 - updated
//-----------------------------------------------------------------------------
#ifndef __UGEN_XXX_H__
#define __UGEN_XXX_H__

#include "chuck_dl.h"


// query
DLL_QUERY xxx_query( Chuck_DL_Query * query );

// global
struct Chuck_Type;
extern Chuck_Type * g_t_dac;
extern Chuck_Type * g_t_adc;

// stereo
CK_DLL_CTOR( stereo_ctor );
CK_DLL_CTRL( stereo_ctrl_pan );
CK_DLL_CGET( stereo_cget_pan );

// multi
CK_DLL_CTOR( multi_ctor );
CK_DLL_CTRL( multi_ctrl_pan );
CK_DLL_CGET( multi_cget_pan );
CK_DLL_CGET( multi_cget_chan );

// bunghole
CK_DLL_TICK( bunghole_tick );

// pan2
CK_DLL_CTOR( pan2_ctor );
CK_DLL_DTOR( pan2_dtor );
CK_DLL_CTRL( pan2_ctrl_value );
CK_DLL_CGET( pan2_cget_value );

// MIX2
CK_DLL_CTOR( mix2_ctor );
CK_DLL_CTOR( mix2_dtor );
CK_DLL_CTRL( mix2_ctrl_value );
CK_DLL_CGET( mix2_cget_value );

// noise
CK_DLL_TICK( noise_tick );

// cnoise
CK_DLL_CTOR( cnoise_ctor );
CK_DLL_DTOR( cnoise_dtor );
CK_DLL_TICK( cnoise_tick );
CK_DLL_CTRL( cnoise_ctrl_mode );
CK_DLL_CTRL( cnoise_ctrl_fprob );

// impulse
CK_DLL_CTOR( impulse_ctor );
CK_DLL_DTOR( impulse_dtor );
CK_DLL_TICK( impulse_tick );
CK_DLL_CTRL( impulse_ctrl_next );
CK_DLL_CGET( impulse_cget_next );

// step
CK_DLL_CTOR( step_ctor );
CK_DLL_DTOR( step_dtor );
CK_DLL_TICK( step_tick );
CK_DLL_CTRL( step_ctrl_next );
CK_DLL_CGET( step_cget_next );

// gain
CK_DLL_CTOR( gain_ctor );
CK_DLL_DTOR( gain_dtor );
CK_DLL_TICK( gain_tick );
CK_DLL_CTRL( gain_ctrl_value );
CK_DLL_CGET( gain_cget_value );

// dyno (Matt Hoffman + Graham Coleman)
// TODO: add soft knee option
CK_DLL_CTOR( dyno_ctor );
CK_DLL_DTOR( dyno_dtor );
CK_DLL_TICK( dyno_tick );
CK_DLL_CTRL( dyno_ctrl_limit );
CK_DLL_CTRL( dyno_ctrl_compress );
CK_DLL_CTRL( dyno_ctrl_expand );
CK_DLL_CTRL( dyno_ctrl_gate );
CK_DLL_CTRL( dyno_ctrl_duck );
CK_DLL_CTRL( dyno_ctrl_thresh );
CK_DLL_CGET( dyno_cget_thresh );
CK_DLL_CTRL( dyno_ctrl_attackTime );
CK_DLL_CGET( dyno_cget_attackTime );
CK_DLL_CTRL( dyno_ctrl_releaseTime );
CK_DLL_CGET( dyno_cget_releaseTime );
CK_DLL_CTRL( dyno_ctrl_ratio );
CK_DLL_CGET( dyno_cget_ratio );
CK_DLL_CTRL( dyno_ctrl_slopeBelow );
CK_DLL_CGET( dyno_cget_slopeBelow );
CK_DLL_CTRL( dyno_ctrl_slopeAbove );
CK_DLL_CGET( dyno_cget_slopeAbove );
CK_DLL_CTRL( dyno_ctrl_externalSideInput );
CK_DLL_CGET( dyno_cget_externalSideInput );
CK_DLL_CTRL( dyno_ctrl_sideInput );
CK_DLL_CGET( dyno_cget_sideInput );

// halfrect
CK_DLL_TICK( halfrect_tick );

// fullrect
CK_DLL_TICK( fullrect_tick );

// zerox
// CK_DLL_CTOR( zerox_ctor );
// CK_DLL_DTOR( zerox_dtor );
// CK_DLL_TICK( zerox_tick );

// delayp
CK_DLL_CTOR( delayp_ctor );
CK_DLL_DTOR( delayp_dtor );
CK_DLL_PMSG( delayp_pmsg );
CK_DLL_TICK( delayp_tick );
CK_DLL_CTRL( delayp_ctrl_delay );
CK_DLL_CGET( delayp_cget_delay );
CK_DLL_CTRL( delayp_ctrl_window );
CK_DLL_CGET( delayp_cget_window );
CK_DLL_CTRL( delayp_ctrl_max );
CK_DLL_CGET( delayp_cget_max );

// sndbuf
CK_DLL_CTOR( sndbuf_ctor );
CK_DLL_DTOR( sndbuf_dtor );
CK_DLL_TICK( sndbuf_tick );
CK_DLL_TICKF( sndbuf_tickf );
CK_DLL_CTRL( sndbuf_ctrl_read );
CK_DLL_CGET( sndbuf_cget_read );
CK_DLL_CTRL( sndbuf_ctrl_write );
CK_DLL_CGET( sndbuf_cget_write );
CK_DLL_CTRL( sndbuf_ctrl_pos );
CK_DLL_CGET( sndbuf_cget_pos );
CK_DLL_CTRL( sndbuf_ctrl_loop );
CK_DLL_CGET( sndbuf_cget_loop );
CK_DLL_CTRL( sndbuf_ctrl_interp );
CK_DLL_CGET( sndbuf_cget_interp );
CK_DLL_CTRL( sndbuf_ctrl_rate );
CK_DLL_CGET( sndbuf_cget_rate );
CK_DLL_CTRL( sndbuf_ctrl_play );
CK_DLL_CGET( sndbuf_cget_play );
CK_DLL_CTRL( sndbuf_ctrl_freq );
CK_DLL_CGET( sndbuf_cget_freq );
CK_DLL_CTRL( sndbuf_ctrl_phase );
CK_DLL_CGET( sndbuf_cget_phase );
CK_DLL_CTRL( sndbuf_ctrl_channel );
CK_DLL_CGET( sndbuf_cget_channel );
CK_DLL_CTRL( sndbuf_ctrl_chunks );
CK_DLL_CGET( sndbuf_cget_chunks );
CK_DLL_CTRL( sndbuf_ctrl_phase_offset );
CK_DLL_CGET( sndbuf_cget_samples );
CK_DLL_CGET( sndbuf_cget_length );
CK_DLL_CGET( sndbuf_cget_channels );
CK_DLL_CGET( sndbuf_cget_valueAt );

// LiSa (Dan Trueman)
CK_DLL_CTOR( LiSaMulti_ctor );
CK_DLL_DTOR( LiSaMulti_dtor );
CK_DLL_TICK( LiSaMulti_tick );
CK_DLL_TICKF( LiSaMulti_tickf );
CK_DLL_PMSG( LiSaMulti_pmsg );
CK_DLL_CTRL( LiSaMulti_size );
CK_DLL_CTRL( LiSaMulti_cget_size );
CK_DLL_CTRL( LiSaMulti_start_record );
CK_DLL_CTRL( LiSaMulti_start_play );
CK_DLL_CTRL( LiSaMulti_start_play0 );
CK_DLL_CTRL( LiSaMulti_ctrl_rate );
CK_DLL_CTRL( LiSaMulti_ctrl_rate0 );
CK_DLL_CGET( LiSaMulti_cget_rate );
CK_DLL_CGET( LiSaMulti_cget_rate0 );
CK_DLL_CTRL( LiSaMulti_ctrl_pindex );
CK_DLL_CGET( LiSaMulti_cget_pindex );
CK_DLL_CTRL( LiSaMulti_ctrl_pindex0 );
CK_DLL_CGET( LiSaMulti_cget_pindex0 );
CK_DLL_CTRL( LiSaMulti_ctrl_rindex );
CK_DLL_CGET( LiSaMulti_cget_rindex );
CK_DLL_CTRL( LiSaMulti_ctrl_lstart );
CK_DLL_CGET( LiSaMulti_cget_lstart );
CK_DLL_CTRL( LiSaMulti_ctrl_lend );
CK_DLL_CGET( LiSaMulti_cget_lend );
CK_DLL_CTRL( LiSaMulti_ctrl_lstart0 );
CK_DLL_CGET( LiSaMulti_cget_lstart0 );
CK_DLL_CTRL( LiSaMulti_ctrl_lend0 );
CK_DLL_CGET( LiSaMulti_cget_lend0 );
CK_DLL_CTRL( LiSaMulti_ctrl_loop );
CK_DLL_CGET( LiSaMulti_cget_loop );
CK_DLL_CTRL( LiSaMulti_ctrl_bi );
CK_DLL_CGET( LiSaMulti_cget_bi );
CK_DLL_CTRL( LiSaMulti_ctrl_loop0 );
CK_DLL_CGET( LiSaMulti_cget_loop0 );
CK_DLL_CTRL( LiSaMulti_ctrl_bi0 );
CK_DLL_CGET( LiSaMulti_cget_bi0 );
CK_DLL_CTRL( LiSaMulti_ctrl_loop_end_rec );
CK_DLL_CGET( LiSaMulti_cget_loop_end_rec );
CK_DLL_CTRL( LiSaMulti_ctrl_loop_rec );
CK_DLL_CGET( LiSaMulti_cget_loop_rec );
CK_DLL_CGET( LiSaMulti_ctrl_sample );
CK_DLL_CGET( LiSaMulti_cget_sample );
CK_DLL_CTRL( LiSaMulti_ctrl_voicegain );
CK_DLL_CGET( LiSaMulti_cget_voicegain );
CK_DLL_CTRL( LiSaMulti_ctrl_voicepan );
CK_DLL_CGET( LiSaMulti_cget_voicepan );
CK_DLL_CTRL( LiSaMulti_ctrl_voicepan0 );
CK_DLL_CGET( LiSaMulti_cget_voicepan0 );
CK_DLL_CTRL( LiSaMulti_ctrl_coeff );
CK_DLL_CGET( LiSaMulti_cget_coeff );
CK_DLL_CTRL( LiSaMulti_ctrl_clear );
CK_DLL_CGET( LiSaMulti_cget_voice );
CK_DLL_CTRL( LiSaMulti_ctrl_maxvoices );
CK_DLL_CGET( LiSaMulti_cget_maxvoices );
CK_DLL_CGET( LiSaMulti_cget_value );
CK_DLL_CGET( LiSaMulti_cget_value0 );
CK_DLL_CTRL( LiSaMulti_ctrl_track );
CK_DLL_CGET( LiSaMulti_cget_track );
CK_DLL_CGET( LiSaMulti_cget_playing );
// ramp stuff
CK_DLL_CTRL( LiSaMulti_ctrl_rampup );
CK_DLL_CTRL( LiSaMulti_ctrl_rampdown );
CK_DLL_CTRL( LiSaMulti_ctrl_rampup0 );
CK_DLL_CTRL( LiSaMulti_ctrl_rampdown0 );
CK_DLL_CTRL( LiSaMulti_ctrl_rec_ramplen );




#endif
