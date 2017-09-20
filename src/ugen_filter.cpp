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
// file: ugen_filter.cpp
// desc: filter related unit generators
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ugen_filter.h"
#include "chuck_type.h"
#include "chuck_compile.h"
#include <math.h>
#include <stdlib.h>


static t_CKUINT g_srate = 0;
static t_CKFLOAT g_radians_per_sample = 0;
// filter member data offset
static t_CKUINT FilterBasic_offset_data = 0;
static t_CKUINT Teabox_offset_data = 0;
static t_CKUINT biquad_offset_data = 0;




//-----------------------------------------------------------------------------
// name: filter_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY filter_query( Chuck_DL_Query * QUERY )
{
    // set srate
    g_srate = QUERY->srate;
    // set radians per sample
    g_radians_per_sample = TWO_PI / (t_CKFLOAT)g_srate;
    
    std::string doc;
    
    Chuck_Env * env = QUERY->env();

    Chuck_DL_Func * func = NULL;

    // deprecate
    type_engine_register_deprecate( env, "filter", "Filter" );
    type_engine_register_deprecate( env, "biquad", "BiQuad" );
    type_engine_register_deprecate( env, "onepole", "OnePole" );
    type_engine_register_deprecate( env, "onezero", "OneZero" );
    type_engine_register_deprecate( env, "twopole", "TwoPole" );
    type_engine_register_deprecate( env, "twozero", "TwoZero" );
    type_engine_register_deprecate( env, "delay", "Delay" );
    type_engine_register_deprecate( env, "delayl", "DelayL" );
    type_engine_register_deprecate( env, "delaya", "DelayA" );

    //---------------------------------------------------------------------
    // init as base class: FilterBasic
    //---------------------------------------------------------------------
    doc = "Filter base class.";
    if( !type_engine_import_ugen_begin( env, "FilterBasic", "UGen", env->global(), 
                                        FilterBasic_ctor, FilterBasic_dtor,
                                        FilterBasic_tick, FilterBasic_pmsg,
                                        doc.c_str() ) )
        return FALSE;

    // member variable
    FilterBasic_offset_data = type_engine_import_mvar( env, "int", "@FilterBasic_data", FALSE );
    if( FilterBasic_offset_data == CK_INVALID_OFFSET ) goto error;

    // freq
    func = make_new_mfun( "float", "freq", FilterBasic_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter cutoff/center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", FilterBasic_cget_freq );
    func->doc = "Get filter cutoff/center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // Q
    func = make_new_mfun( "float", "Q", FilterBasic_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", FilterBasic_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", FilterBasic_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    /*
    //---------------------------------------------------------------------
    // init class: LPF
    //---------------------------------------------------------------------
    if( !type_engine_import_ugen_begin( env, "LPF", "FilterBasic", env->global(),
                                        LPF_ctor, LPF_tick, LPF_pmsg ) )
        return FALSE;

    // freq
    func = make_new_mfun( "float", "freq", LPF_ctrl_freq );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", LPF_cget_freq );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init class: HPF
    //---------------------------------------------------------------------
    if( !type_engine_import_ugen_begin( env, "HPF", "FilterBasic", env->global(),
                                        HPF_ctor, HPF_tick, HPF_pmsg ) )
        return FALSE;

    // freq
    func = make_new_mfun( "float", "freq", HPF_ctrl_freq );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", HPF_cget_freq );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    */


    //---------------------------------------------------------------------
    // init class: BPF
    //---------------------------------------------------------------------
    doc = "Bandpass filter (2nd order Butterworth).";
    if( !type_engine_import_ugen_begin( env, "BPF", "FilterBasic", env->global(),
                                        BPF_ctor, NULL, BPF_tick, BPF_pmsg, doc.c_str() ) )
        return FALSE;

    type_engine_import_add_ex(env, "filter/bp.ck");
    
    // freq
    func = make_new_mfun( "float", "freq", BPF_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", BPF_cget_freq );
    func->doc = "Set filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // Q
    func = make_new_mfun( "float", "Q", BPF_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", BPF_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", BPF_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init class: BRF
    //---------------------------------------------------------------------
    doc = "Band-reject filter (2nd order Butterworth).";
    if( !type_engine_import_ugen_begin( env, "BRF", "FilterBasic", env->global(),
                                        BRF_ctor, NULL, BRF_tick, BRF_pmsg, doc.c_str() ) )
        return FALSE;
    
    type_engine_import_add_ex(env, "filter/br.ck");

    // freq
    func = make_new_mfun( "float", "freq", BRF_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", BRF_cget_freq );
    func->doc = "Get filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // Q
    func = make_new_mfun( "float", "Q", BRF_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", BRF_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", BRF_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    

    //---------------------------------------------------------------------
    // init class: RLPF
    //---------------------------------------------------------------------
    doc = "Resonant low-pass filter (2nd order Butterworth).";
    if( !type_engine_import_ugen_begin( env, "LPF", "FilterBasic", env->global(),
                                        RLPF_ctor, NULL, RLPF_tick, RLPF_pmsg, doc.c_str() ) )
        return FALSE;
    
    type_engine_import_add_ex(env, "filter/lp.ck");

    // freq
    func = make_new_mfun( "float", "freq", RLPF_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter cutoff frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", RLPF_cget_freq );
    func->doc = "Get filter cutoff frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // Q
    func = make_new_mfun( "float", "Q", RLPF_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", RLPF_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", RLPF_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init class: RHPF
    //---------------------------------------------------------------------
    doc = "Resonant high-pass filter (2nd order Butterworth).";
    if( !type_engine_import_ugen_begin( env, "HPF", "FilterBasic", env->global(),
                                        RHPF_ctor, NULL, RHPF_tick, RHPF_pmsg, doc.c_str() ) )
        return FALSE;
    
    type_engine_import_add_ex(env, "filter/hp.ck");

    // freq
    func = make_new_mfun( "float", "freq", RHPF_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter cutoff frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", RHPF_cget_freq );
    func->doc = "Get filter cutoff frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // Q
    func = make_new_mfun( "float", "Q", RHPF_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", RHPF_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", RHPF_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init class: ResonZ
    //---------------------------------------------------------------------
    doc = "Resonant filter. BiQuad with equal-gain zeros, keeps gain under control independent of frequency.";
    if( !type_engine_import_ugen_begin( env, "ResonZ", "FilterBasic", env->global(),
                                        ResonZ_ctor, NULL, ResonZ_tick, ResonZ_pmsg, doc.c_str() ) )
        return FALSE;

    type_engine_import_add_ex(env, "filter/resonz.ck");
    
    // freq
    func = make_new_mfun( "float", "freq", ResonZ_ctrl_freq );
    func->add_arg( "float", "val" );
    func->doc = "Set filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", ResonZ_cget_freq );
    func->doc = "Get filter center frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // reson
    func = make_new_mfun( "float", "Q", ResonZ_ctrl_Q );
    func->add_arg( "float", "val" );
    func->doc = "Set filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "Q", ResonZ_cget_Q );
    func->doc = "Get filter resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // set
    func = make_new_mfun( "void", "set", ResonZ_ctrl_set );
    func->add_arg( "float", "freq" );
    func->add_arg( "float", "Q" );
    func->doc = "Set filter frequency and resonance at the same time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init as base class: BiQuad
    //---------------------------------------------------------------------
    doc = "STK biquad (two-pole, two-zero) filter class. A method is provided for creating a resonance in the frequency response while maintaining a constant filter gain.";
    if( !type_engine_import_ugen_begin( env, "BiQuad", "UGen", env->global(), 
                                        biquad_ctor, biquad_dtor, biquad_tick, NULL, doc.c_str() ) )
        return FALSE;

    // member variable
    biquad_offset_data = type_engine_import_mvar ( env, "int", "@biquad_data", FALSE );
    if ( biquad_offset_data == CK_INVALID_OFFSET ) goto error;

    // pfreq
    func = make_new_mfun ( "float", "pfreq", biquad_ctrl_pfreq );
    func->add_arg ( "float", "freq" );
    func->doc = "Set resonance frequency (poles).";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "float", "pfreq", biquad_cget_pfreq );
    func->doc = "Get resonance frequency (poles).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // prad
    func = make_new_mfun ( "float", "prad", biquad_ctrl_prad );
    func->add_arg ( "float", "value" );
    func->doc = "Set Pole radius (values greater than 1 will be unstable).";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "float", "prad", biquad_cget_prad );
    func->doc = "Get pole radius.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // zfreq
    func = make_new_mfun ( "float", "zfreq", biquad_ctrl_zfreq );
    func->add_arg ( "float", "freq" );
    func->doc = "Set notch frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "float", "zfreq", biquad_cget_zfreq );
    func->doc = "Get notch frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // zrad
    func = make_new_mfun ( "float", "zrad", biquad_ctrl_zrad );
    func->add_arg ( "float", "value" );
    func->doc = "Set zero radius.";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "float", "zrad", biquad_cget_zrad );
    func->doc = "Get zero radius.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // norm
    func = make_new_mfun ( "int", "norm", biquad_ctrl_norm );
    func->add_arg ( "int", "value" );
    func->doc = "Toggle normalization.";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "int", "norm", biquad_cget_norm );
    func->doc = "Get normalization.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // pregain
    func = make_new_mfun ( "float", "pregain", biquad_ctrl_pregain );
    func->add_arg ( "float", "level" );
    func->doc = "Set pregain.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "pregain", biquad_cget_pregain );
    func->doc = "Get pregain.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // eqzs
    func = make_new_mfun ( "int", "eqzs", biquad_ctrl_eqzs );
    func->add_arg ( "int", "value" );
    func->doc = "Toggle equal-gain zeroes. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // b0
    func = make_new_mfun ( "float", "b0", biquad_ctrl_b0 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;    
    func = make_new_mfun ( "float", "b0", biquad_cget_b0 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // b1
    func = make_new_mfun ( "float", "b1", biquad_ctrl_b1 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "b1", biquad_cget_b1 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // b2
    func = make_new_mfun ( "float", "b2", biquad_ctrl_b2 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "b2", biquad_cget_b2 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // a0
    func = make_new_mfun ( "float", "a0", biquad_ctrl_a0 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "a0", biquad_cget_a0 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // a1
    func = make_new_mfun ( "float", "a1", biquad_ctrl_a1 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "a1", biquad_cget_a1 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // a2
    func = make_new_mfun ( "float", "a2", biquad_ctrl_a2 );
    func->add_arg ( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun ( "float", "a2", biquad_cget_a2 );
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
	
	 //---------------------------------------------------------------------
    // init class: Teabox
    //---------------------------------------------------------------------
    if( !type_engine_import_ugen_begin( env, "Teabox", "FilterBasic", env->global(),
                                        teabox_ctor, NULL, teabox_tick, teabox_pmsg ) )
        return FALSE;

    // analog
    func = make_new_mfun( "float", "analog", teabox_cget_analog );
	func->add_arg( "int", "which" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// digital
    func = make_new_mfun( "float", "digital", teabox_cget_digital );
	func->add_arg( "int", "which" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
	
    /*
    //----------------------------------
    // begin onepole ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "OnePole", "biquad", env->global(),
                                         NULL, onepole_tick, NULL ) ) return FALSE;
    // ctrl func
    func = make_new_mfun ( "float", "pole", onepole_ctrl_pole );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin onezero ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "onezero", "biquad", env->global(),
                                         NULL, onezero_tick, NULL ) ) return FALSE;
    // ctrl func
    func = make_new_mfun ( "float", "zero", onezero_ctrl_zero );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    


    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin twopole ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "twopole", "biquad", env->global(),
                                         NULL, twopole_tick, NULL ) ) return FALSE;
    // ctrl func
    func = make_new_mfun ( "float", "freq", twopole_ctrl_freq );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "float", "rad", twopole_ctrl_rad );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "int", "norm", twopole_ctrl_norm );
    func->add_arg ( "int", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin twozero ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "twozero", "biquad", env->global(),
                                         NULL, twozero_tick, NULL ) ) return FALSE;
    // ctrl func
    func = make_new_mfun ( "float", "freq", twozero_ctrl_freq );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "float", "rad",  twozero_ctrl_rad );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin delay ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "delay", "UGen", env->global(),
                                         delay_ctor, delay_tick, NULL ) ) return FALSE;

    // ctrl func
    func = make_new_mfun ( "float", "delay", delay_ctrl_delay );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "float", "max", delay_ctrl_max );
    func->add_arg ( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    


    // QUERY->ugen_ctrl( QUERY, delay_ctrl_energy, NULL,"int", "energy" );
    // QUERY->ugen_ctrl( QUERY, delay_ctrl_tap, NULL, "int", "tap" );
    // QUERY->ugen_ctrl( QUERY, delay_ctrl_ftap, NULL, "float", "ftap" );

    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin delayA ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "delayA", "UGen", env->global(),
                                         delayA_ctor, delayA_tick, NULL ) ) return FALSE;

    // ctrl func
    func = make_new_mfun ( "float", "delay", delayA_ctrl_delay ); 
    func->add_arg ( "float", "length" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "float", "max", delayA_ctrl_max );
    func->add_arg ( "float", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    


    // end the class import
    type_engine_import_class_end( env );


    //----------------------------------
    // begin one ugen
    //----------------------------------
    if ( !type_engine_import_ugen_begin( env, "delayL", "UGen", env->global(),
                                         delayL_ctor, delayL_tick, NULL ) ) return FALSE;
    // ctrl func
    func = make_new_mfun ( "float", "delay", delayL_ctrl_delay );
    func->add_arg ( "float", "length" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun ( "float", "max", delayL_ctrl_max );
    func->add_arg ( "float", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    // end the class import
    type_engine_import_class_end( env );
    */

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: Filter_data
// desc: ...
//-----------------------------------------------------------------------------
struct Filter_data
{
    t_CKUINT nB;
    t_CKUINT nA;
    SAMPLE * b;
    SAMPLE * a;
    SAMPLE * input;
    SAMPLE * output;
    
    Filter_data()
    {
        a = b = NULL;
        alloc( 1, 1 );
        a[0] = b[0] = 1.0f;
    }
    
    void alloc( t_CKUINT _b, t_CKUINT _a )
    {
        nB = _b; nA = _a;
        if( b ) delete b;
        if( a ) delete a;
        if( input ) delete input;
        if( output ) delete output;
        b = new SAMPLE[nB];
        a = new SAMPLE[nA];
        input = new SAMPLE[nB];
        output = new SAMPLE[nA];
    }
    
    inline SAMPLE tick( SAMPLE in )
    {
        t_CKUINT i;

        output[0] = 0.0;
        input[0] = in;
        for( i = nB-1; i > 0; i-- )
        {
            output[0] += b[i] * input[i];
            input[i] = input[i-1];
        }
        output[0] += b[0] * input[0];

        for ( i = nA-1; i>0; i--)
        {
            output[0] += -a[i] * output[i];
            output[i] = output[i-1];
        }

        return output[0];
    }
};




//-----------------------------------------------------------------------------
// name: FilterBasic_data
// desc: ...
//-----------------------------------------------------------------------------
struct FilterBasic_data
{
    // much of this implementation is adapted or copied outright from SC3
    SAMPLE m_y1;
    SAMPLE m_y2;
    SAMPLE m_a0;
    SAMPLE m_b1;
    SAMPLE m_b2;
    t_CKFLOAT m_freq;
    t_CKFLOAT m_Q;
    t_CKFLOAT m_db;

    // tick_lpf
    inline SAMPLE tick_lpf( SAMPLE in )
    {
        SAMPLE y0, result;
        
        // go: adapted from SC3's LPF
        y0 = in + m_b1 * m_y1 + m_b2 * m_y2;
        result = m_a0 * (y0 + 2 * m_y1 + m_y2);
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // tick_hpf
    inline SAMPLE tick_hpf( SAMPLE in )
    {
        SAMPLE y0, result;
        
        // go: adapted from SC3's HPF
        y0 = in + m_b1 * m_y1 + m_b2 * m_y2;
        result = m_a0 * (y0 - 2 * m_y1 + m_y2);
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // set_bpf
    inline void set_bpf( t_CKFLOAT freq, t_CKFLOAT Q )
    {
        t_CKFLOAT pfreq = freq * g_radians_per_sample;
        t_CKFLOAT pbw = 1.0 / Q * pfreq * .5;

        t_CKFLOAT C = 1.0 / ::tan(pbw);
        t_CKFLOAT D = 2.0 * ::cos(pfreq);
        t_CKFLOAT next_a0 = 1.0 / (1.0 + C);
        t_CKFLOAT next_b1 = C * D * next_a0 ;
        t_CKFLOAT next_b2 = (1.0 - C) * next_a0;

        m_freq = freq;
        m_Q = Q;
        m_a0 = (SAMPLE)next_a0;
        m_b1 = (SAMPLE)next_b1;
        m_b2 = (SAMPLE)next_b2;
    }

    // tick_bpf
    inline SAMPLE tick_bpf( SAMPLE in )
    {
        SAMPLE y0, result;
        
        // go: adapted from SC3's LPF
        y0 = in + m_b1 * m_y1 + m_b2 * m_y2;
        result = m_a0 * (y0 - m_y2);
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // set_brf
    inline void set_brf( t_CKFLOAT freq, t_CKFLOAT Q )
    {
        t_CKFLOAT pfreq = freq * g_radians_per_sample;
        t_CKFLOAT pbw = 1.0 / Q * pfreq * .5;

        t_CKFLOAT C = ::tan(pbw);
        t_CKFLOAT D = 2.0 * ::cos(pfreq);
        t_CKFLOAT next_a0 = 1.0 / (1.0 + C);
        t_CKFLOAT next_b1 = -D * next_a0 ;
        t_CKFLOAT next_b2 = (1.f - C) * next_a0;

        m_freq = freq;
        m_Q = Q;
        m_a0 = (SAMPLE)next_a0;
        m_b1 = (SAMPLE)next_b1;
        m_b2 = (SAMPLE)next_b2;
    }

    // tick_brf
    inline SAMPLE tick_brf( SAMPLE in )
    {
        SAMPLE y0, result;
        
        // go: adapted from SC3's HPF
        // b1 is actually a1
        y0 = in - m_b1 * m_y1 - m_b2 * m_y2;
        result = m_a0 * (y0 + m_y2) + m_b1 * m_y1;
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // set_rlpf
    inline void set_rlpf( t_CKFLOAT freq, t_CKFLOAT Q )
    {
        t_CKFLOAT qres = ck_max( .001, 1.0/Q );
        t_CKFLOAT pfreq = freq * g_radians_per_sample;

        t_CKFLOAT D = ::tan(pfreq * qres * 0.5);
        t_CKFLOAT C = (1.0 - D) / (1.0 + D);
        t_CKFLOAT cosf = ::cos(pfreq);
        t_CKFLOAT next_b1 = (1.0 + C) * cosf;
        t_CKFLOAT next_b2 = -C;
        t_CKFLOAT next_a0 = (1.0 + C - next_b1) * 0.25;

        m_freq = freq;
        m_Q = 1.0 / qres;
        m_a0 = (SAMPLE)next_a0;
        m_b1 = (SAMPLE)next_b1;
        m_b2 = (SAMPLE)next_b2;
    }

    // tick_rlpf
    inline SAMPLE tick_rlpf( SAMPLE in )
    {
        SAMPLE y0, result;

        // go: adapated from SC3's RLPF
        y0 = m_a0 * in + m_b1 * m_y1 + m_b2 * m_y2;
        result = y0 + 2 * m_y1 + m_y2;
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // set_rhpf
    inline void set_rhpf( t_CKFLOAT freq, t_CKFLOAT Q )
    {
        t_CKFLOAT qres = ck_max( .001, 1.0/Q );
        t_CKFLOAT pfreq = freq * g_radians_per_sample;

        t_CKFLOAT D = ::tan(pfreq * qres * 0.5);
        t_CKFLOAT C = (1.0 - D) / (1.0 + D);
        t_CKFLOAT cosf = ::cos(pfreq);
        t_CKFLOAT next_b1 = (1.0 + C) * cosf;
        t_CKFLOAT next_b2 = -C;
        t_CKFLOAT next_a0 = (1.0 + C + next_b1) * 0.25;

        m_freq = freq;
        m_Q = 1.0 / qres;
        m_a0 = (SAMPLE)next_a0;
        m_b1 = (SAMPLE)next_b1;
        m_b2 = (SAMPLE)next_b2;
    }

    // tick_rhpf
    inline SAMPLE tick_rhpf( SAMPLE in )
    {
        SAMPLE y0, result;

        // go: adapted from SC3's RHPF
        y0 = m_a0 * in + m_b1 * m_y1 + m_b2 * m_y2;
        result = y0 - 2 * m_y1 + m_y2;
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

    // set_resonz
    inline void set_resonz( t_CKFLOAT freq, t_CKFLOAT Q )
    {
        t_CKFLOAT pfreq = freq * g_radians_per_sample;
        t_CKFLOAT B = pfreq / Q;
        t_CKFLOAT R = 1.0 - B * 0.5;
        t_CKFLOAT R2 = 2.0 * R;
        t_CKFLOAT R22 = R * R;
        t_CKFLOAT cost = (R2 * ::cos(pfreq)) / (1.0 + R22);
        t_CKFLOAT next_b1 = R2 * cost;
        t_CKFLOAT next_b2 = -R22;
        t_CKFLOAT next_a0 = (1.0 - R22) * 0.5;

        m_freq = freq;
        m_Q = Q;
        m_a0 = (SAMPLE)next_a0;
        m_b1 = (SAMPLE)next_b1;
        m_b2 = (SAMPLE)next_b2;
    }

    // tick_resonz
    inline SAMPLE tick_resonz( SAMPLE in )
    {
        SAMPLE y0, result;

        // go: adapted from SC3's ResonZ
        y0 = in + m_b1 * m_y1 + m_b2 * m_y2;
        result = m_a0 * (y0 - m_y2);
        m_y2 = m_y1;
        m_y1 = y0;

        // be normal
        CK_DDN(m_y1);
        CK_DDN(m_y2);

        return result;
    }

};

struct Teabox_data
{
	//Teabox sensor interface read
	t_CKINT		teabox_counter;
	t_CKFLOAT	teabox_data[9];		// one container for the data from each sensor
	t_CKFLOAT	teabox_hw_version;	// version number (major.minor) of the connected Teabox
	t_CKFLOAT	teabox_last_value;	
	t_CKINT		teabox_bitmask;
	//tick for teabox
	inline SAMPLE teabox_tick( SAMPLE in )
	{
	
		if(in < 0.0 || teabox_counter > 9){			// If the sample is the start flag...
			if(teabox_last_value < 0.0)					// Actually - if all 16 toggles on the Teabox digital inputs
				teabox_data[8] = teabox_last_value;			//	are high, it will look identical to the start flag - so
													//  so we compensate for that here.
			teabox_counter = 0;
		}
		else if(teabox_counter == 0){					// if the sample is hardware version number...
			teabox_hw_version = in * 8.0;							
			teabox_counter++;
		}	
		else{
			teabox_data[teabox_counter - 1] = in * 8.0;	// Normalize the range
			teabox_counter++;
		}
		
		// POST-PROCESS TOGGLE INPUT BITMASK
		if(teabox_data[8] < 0){
			teabox_bitmask = (t_CKINT)(teabox_data[8] * 32768);			// 4096 = 32768 / 8 (we already multiplied by 8)
			teabox_bitmask ^= -32768;
			teabox_bitmask = 32768 + (teabox_bitmask);			// 2^3
		}
		else
			teabox_bitmask = (t_CKINT)(teabox_data[8] * 4096);			// 4096 = 32768 / 8 (we already multiplied by 8)
		
		teabox_last_value = in;						// store the input value for the next time around

		return in;
		
	}
	
};

//-----------------------------------------------------------------------------
// name: FilterBasic_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FilterBasic_ctor )
{
    // set to zero
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: FilterBasic_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FilterBasic_dtor )
{
    // get data
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    // delete
    SAFE_DELETE(d);
    // set
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: FilterBasic_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( FilterBasic_tick )
{
    CK_FPRINTF_STDERR( "FilterBasic.tick() --> FitlerBasic is virtual!\n" );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FilterBasic_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FilterBasic_ctrl_freq )
{
}


//-----------------------------------------------------------------------------
// name: FilterBasic_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( FilterBasic_cget_freq )
{
}


//-----------------------------------------------------------------------------
// name: FilterBasic_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FilterBasic_ctrl_Q )
{
}


//-----------------------------------------------------------------------------
// name: FilterBasic_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( FilterBasic_cget_Q )
{
}


//-----------------------------------------------------------------------------
// name: FilterBasic_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FilterBasic_ctrl_set )
{
}


//-----------------------------------------------------------------------------
// name: FilterBasic_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FilterBasic_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: LPF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( LPF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: LPF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( LPF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_lpf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: LPF_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LPF_ctrl_freq )
{
    // implementation: adapted from SC3's LPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
	t_CKFLOAT pfreq = freq * g_radians_per_sample * 0.5;
	
    t_CKFLOAT C = 1.0 / ::tan(pfreq);
	t_CKFLOAT C2 = C * C;
	t_CKFLOAT sqrt2C = C * SQRT2;
	t_CKFLOAT next_a0 = 1.0 / (1.0 + sqrt2C + C2);
	t_CKFLOAT next_b1 = -2.0 * (1.0 - C2) * next_a0 ;
	t_CKFLOAT next_b2 = -(1.f - sqrt2C + C2) * next_a0;

    d->m_freq = freq;
    d->m_a0 = (SAMPLE)next_a0;
    d->m_b1 = (SAMPLE)next_b1;
    d->m_b2 = (SAMPLE)next_b2;

    RETURN->v_float = freq;
}


//-----------------------------------------------------------------------------
// name: LPF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LPF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: LPF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( LPF_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: HPF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HPF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: HPF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( HPF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_hpf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: HPF_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( HPF_ctrl_freq )
{
    // implementation: adapted from SC3's HPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
	t_CKFLOAT pfreq = freq * g_radians_per_sample * 0.5;
	
    t_CKFLOAT C = ::tan(pfreq);
	t_CKFLOAT C2 = C * C;
	t_CKFLOAT sqrt2C = C * SQRT2;
	t_CKFLOAT next_a0 = 1.0 / (1.0 + sqrt2C + C2);
	t_CKFLOAT next_b1 = 2.0 * (1.0 - C2) * next_a0 ;
	t_CKFLOAT next_b2 = -(1.0 - sqrt2C + C2) * next_a0;

    d->m_freq = freq;
    d->m_a0 = (SAMPLE)next_a0;
    d->m_b1 = (SAMPLE)next_b1;
    d->m_b2 = (SAMPLE)next_b2;

    RETURN->v_float = freq;
}


//-----------------------------------------------------------------------------
// name: HPF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( HPF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: HPF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( HPF_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: BPF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BPF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: BPF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BPF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_bpf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BPF_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BPF_ctrl_freq )
{
    // implementation: adapted from SC3's BPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_bpf( freq, d->m_Q );

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: BPF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( BPF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: BPF_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BPF_ctrl_Q )
{
    // implementation: adapted from SC3's BPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_bpf( d->m_freq, Q );


    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: BPF_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( BPF_cget_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: BPF_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BPF_ctrl_set )
{
    // implementation: adapted from SC3's BPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_bpf( freq, Q );
}


//-----------------------------------------------------------------------------
// name: BPF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BPF_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: BRF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BRF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: BRF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BRF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_brf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BRF_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BRF_ctrl_freq )
{
    // implementation: adapted from SC3's BRF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_brf( freq, d->m_Q );

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: BRF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( BRF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: BRF_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BRF_ctrl_Q )
{
    // implementation: adapted from SC3's BRF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_brf( d->m_freq, Q );

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: BRF_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( BRF_cget_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: BRF_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BRF_ctrl_set )
{
    // implementation: adapted from SC3's BRF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_brf( freq, Q );
}


//-----------------------------------------------------------------------------
// name: BRF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BRF_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: RLPF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( RLPF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    // default
    f->m_Q = 1.0;
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: RLPF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( RLPF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_rlpf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: RLPF_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RLPF_ctrl_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rlpf( freq, d->m_Q );

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: RLPF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( RLPF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: RLPF_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RLPF_ctrl_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rlpf( d->m_freq, Q );

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: RLPF_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( RLPF_cget_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: RLPF_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RLPF_ctrl_set )
{
    // implementation: adapted from SC3's RLPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rlpf( freq, Q );

    RETURN->v_float = freq;
}


//-----------------------------------------------------------------------------
// name: RLPF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( RLPF_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: ResonZ_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( ResonZ_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    // default
    f->set_resonz( 220, 1 );
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: ResonZ_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( ResonZ_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_resonz( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ResonZ_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ResonZ_ctrl_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_resonz( freq, d->m_Q );

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: ResonZ_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( ResonZ_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: ResonZ_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ResonZ_ctrl_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_resonz( d->m_freq, Q );

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: ResonZ_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( ResonZ_cget_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: ResonZ_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ResonZ_ctrl_set )
{
    // implementation: adapted from SC3's RLPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_resonz( freq, Q );

    RETURN->v_float = freq;
}


//-----------------------------------------------------------------------------
// name: ResonZ_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( ResonZ_pmsg )
{
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: RHPF_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( RHPF_ctor )
{
    FilterBasic_data * f =  new FilterBasic_data;
    memset( f, 0, sizeof(FilterBasic_data) );
    // default
    f->m_Q = 1.0;
    OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data) = (t_CKUINT)f;
}


//-----------------------------------------------------------------------------
// name: RHPF_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( RHPF_tick )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    *out = d->tick_rhpf( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: RHPF_ctrl_freq()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RHPF_ctrl_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rhpf( freq, d->m_Q );

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: RHPF_cget_freq()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( RHPF_cget_freq )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_freq;
}


//-----------------------------------------------------------------------------
// name: RHPF_ctrl_Q()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RHPF_ctrl_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rhpf( d->m_freq, Q );

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: RHPF_cget_Q()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( RHPF_cget_Q )
{
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);

    // return
    RETURN->v_float = d->m_Q;
}


//-----------------------------------------------------------------------------
// name: RHPF_ctrl_set()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( RHPF_ctrl_set )
{
    // implementation: adapted from SC3's RLPF
    FilterBasic_data * d = (FilterBasic_data *)OBJ_MEMBER_UINT(SELF, FilterBasic_offset_data);
    t_CKFLOAT freq = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT Q = GET_NEXT_FLOAT(ARGS);

    // set
    d->set_rlpf( freq, Q );
}


//-----------------------------------------------------------------------------
// name: RHPF_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( RHPF_pmsg )
{
    return FALSE;
}



//-----------------------------------------------------------------------------
// name: teabox_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( teabox_ctor )
{
    Teabox_data * f =  new Teabox_data;
    memset( f, 0, sizeof(Teabox_data) );
	f->teabox_hw_version = 0.;		// version number (major.minor) of the connected Teabox
	f->teabox_last_value = 0.;	
    OBJ_MEMBER_UINT(SELF, Teabox_offset_data) = (t_CKUINT)f;
}

//-----------------------------------------------------------------------------
// name: teabox_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( teabox_tick )
{
    Teabox_data * d = (Teabox_data *)OBJ_MEMBER_UINT(SELF, Teabox_offset_data);
    *out = d->teabox_tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: teabox_cget_analog()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( teabox_cget_analog )
{
	t_CKINT which_in;
    Teabox_data * d = (Teabox_data *)OBJ_MEMBER_UINT(SELF, Teabox_offset_data);
	
	which_in = GET_CK_INT(ARGS);
	
	//constrain
	if(which_in < 0) which_in = 0;
	if(which_in > 7) which_in = 7;
	
    // return
    RETURN->v_float = d->teabox_data[which_in];
}

CK_DLL_CGET( teabox_cget_digital )
{
	t_CKINT which_in;
	t_CKFLOAT out_val;
	t_CKINT which_pow;
	
    Teabox_data * d = (Teabox_data *)OBJ_MEMBER_UINT(SELF, Teabox_offset_data);
	
	which_in = GET_CK_INT(ARGS);
	
	//constrain
	if(which_in < 0) which_in = 0;
	if(which_in > 15) which_in = 15;

	// grab the needed bit from bitmask
    // which_pow = (t_CKINT)(::pow( 2, which_in ));
    which_pow = 1 >> which_in;
	out_val = ( d->teabox_bitmask & which_pow ) > 0;
	
    // return
    RETURN->v_float = out_val;
}

//-----------------------------------------------------------------------------
// name: teabox_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( teabox_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: biquad
// desc: biquad filter
//-----------------------------------------------------------------------------
struct biquad_data
{
    SAMPLE m_a0, m_a1, m_a2;
    SAMPLE m_b0, m_b1, m_b2;
    
    SAMPLE m_input0, m_input1, m_input2;
    SAMPLE m_output0, m_output1, m_output2;

    t_CKFLOAT pfreq, zfreq;
    t_CKFLOAT prad, zrad;
    t_CKBOOL norm;
    t_CKUINT srate;

    biquad_data()
    {
        m_a0 = m_b0 = 1.0f;
        m_a1 = m_a2 = 0.0f;
        m_b1 = 0.0f; m_b2 = 0.0f;

        m_input0 = m_input1 = m_input2 = 0.0f;
        m_output0 = m_output1 = m_output2 = 0.0f;

        pfreq = zfreq = 0.0f;
        prad = zrad = 0.0f;
        norm = FALSE;
        srate = g_srate;
    }
};




//-----------------------------------------------------------------------------
// name: biquad_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( biquad_ctor )
{    
    biquad_data* d = new biquad_data;
    OBJ_MEMBER_UINT( SELF, biquad_offset_data ) = (t_CKUINT)d;
}

//-----------------------------------------------------------------------------
// name: biquad_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( biquad_dtor )
{    
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    SAFE_DELETE( d );
    OBJ_MEMBER_UINT(SELF, biquad_offset_data) = 0;
}

//-----------------------------------------------------------------------------
// name: biquad_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( biquad_tick )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    d->m_input0 = d->m_a0 * in;
    d->m_output0 = d->m_b0 * d->m_input0 + d->m_b1 * d->m_input1 + d->m_b2 * d->m_input2;
    d->m_output0 -= d->m_a2 * d->m_output2 + d->m_a1 * d->m_output1;
    d->m_input2 = d->m_input1;
    d->m_input1 = d->m_input0;
    d->m_output2 = d->m_output1;
    d->m_output1 = d->m_output0;

    // be normal
    CK_DDN(d->m_output1);
    CK_DDN(d->m_output2);

    *out = (SAMPLE)d->m_output0;

    return TRUE;
}

void biquad_set_reson( biquad_data * d )
{
    d->m_a2 = (SAMPLE)(d->prad * d->prad);
    d->m_a1 = (SAMPLE)(-2.0 * d->prad * cos(2.0 * ONE_PI * d->pfreq / (double)d->srate));

    if ( d->norm ) {
        // Use zeros at +- 1 and normalize the filter peak gain.
        d->m_b0= 0.5f - 0.5f * d->m_a2;
        d->m_b1 = -1.0f;
        d->m_b2 = -d->m_b0;
    }    
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_pfreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_pfreq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->pfreq = GET_CK_FLOAT(ARGS);
    biquad_set_reson( d ); 
    RETURN->v_float = d->pfreq;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_pfreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_pfreq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->pfreq;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_prad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_prad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->prad = GET_CK_FLOAT(ARGS);
    biquad_set_reson( d );
    RETURN->v_float = d->prad;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_prad()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_prad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->prad;
}

void biquad_set_notch( biquad_data * d )
{
    d->m_b2 = (SAMPLE)(d->zrad * d->zrad);
    d->m_b1 = (SAMPLE)(-2.0 * d->zrad * cos(2.0 * ONE_PI * d->zfreq / (double)d->srate));
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_zfreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_zfreq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->zfreq = GET_CK_FLOAT(ARGS);
    biquad_set_notch( d );
    RETURN->v_float = d->zfreq;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_zfreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_zfreq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->zfreq;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_zrad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_zrad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->zrad = GET_CK_FLOAT(ARGS);
    biquad_set_notch( d );
    RETURN->v_float = d->zrad;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_zrad()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_zrad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->zrad;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_norm()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_norm )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->norm = *(t_CKBOOL *)ARGS;
    biquad_set_reson( d );
    RETURN->v_int = d->norm;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_norm()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_norm )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_int = d->norm;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_pregain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_pregain )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_a0 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_a0;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_pregain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_pregain )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_a0;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_eqzs()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_eqzs )
{
    if( *(t_CKUINT *)ARGS )
    {
        biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
        d->m_b0 = 1.0f;
        d->m_b1 = 0.0f;
        d->m_b2 = -1.0f;
    }
    RETURN->v_int = *(t_CKUINT *)ARGS;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_b0 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_b0 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_b0;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_b0 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_b0;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_b1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_b1 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_b1 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_b1;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_b1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_b1 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_b1;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_b2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_b2 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_b2 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_b2;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_b2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_b2 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_b2;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_a0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_a0 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_a0 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_a0;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_a0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_a0 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_a0;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_a1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_a1 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_a1 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_a1;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_a1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_a1 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_a1;
}

//-----------------------------------------------------------------------------
// name: biquad_ctrl_a2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_ctrl_a2 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->m_a2 = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = d->m_a2;
}

//-----------------------------------------------------------------------------
// name: biquad_cget_a2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( biquad_cget_a2 )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    RETURN->v_float = d->m_a2;
}


//-----------------------------------------------------------------------------
// name: onepole
// desc: onepole filter
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// name: onepole_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( onepole_tick )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );

    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 - d->m_a1 * d->m_output1;
    d->m_output1 = d->m_output0;

    *out = d->m_output0;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: onepole_ctrl_pole()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( onepole_ctrl_pole )
{
    float f = (float)GET_CK_FLOAT(ARGS);
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    if( f > 0.0f )
        d->m_b0 = 1.0f - f;
    else
        d->m_b0 = 1.0f + f;

    d->m_a0 = -f;
}




//-----------------------------------------------------------------------------
// name: onezero
// desc: onezero filter
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: onezero_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( onezero_tick )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    d->m_input0 = in;
    d->m_output0 = d->m_b1 * d->m_input1 + d->m_b0 * d->m_input0;
    d->m_input1 = d->m_input0;

    *out = d->m_output0;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: onezero_ctrl_zero()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( onezero_ctrl_zero )
{
    float f = (float)GET_CK_FLOAT(ARGS);
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    if( f > 0.0f )
        d->m_b0 = 1.0f / ( 1.0f + f );
    else
        d->m_b0 = 1.0f / ( 1.0f - f );
        
    d->m_b1 = -f * d->m_b0;
}




//-----------------------------------------------------------------------------
// name: twopole
// desc: twopole filter
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: twopole_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( twopole_tick )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 - d->m_a2 * d->m_output2 - d->m_a1 * d->m_output1;
    d->m_output2 = d->m_output1;
    d->m_output1 = d->m_output0;

    *out = d->m_output0;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: twopole_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( twopole_ctrl_freq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->pfreq = (float)GET_CK_FLOAT(ARGS);
    biquad_set_reson( d );
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terribly efficient.
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * ONE_PI * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * ONE_PI * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}

//-----------------------------------------------------------------------------
// name: twopole_ctrl_rad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( twopole_ctrl_rad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->prad = (float)GET_CK_FLOAT(ARGS);
    biquad_set_reson( d );
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terrbly efficient
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * ONE_PI * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * ONE_PI * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}

//-----------------------------------------------------------------------------
// name: twopole_ctrl_norm()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( twopole_ctrl_norm )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->norm = *(t_CKBOOL *)ARGS;
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terribly efficient
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * ONE_PI * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * ONE_PI * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}




//-----------------------------------------------------------------------------
// name: twozero
// desc: twozero filter
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: twozero_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( twozero_tick )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    
    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 + d->m_b1 * d->m_input1 + d->m_b2 * d->m_input2;
    d->m_input2 = d->m_input1;
    d->m_input1 = d->m_input0;

    *out = d->m_output0;
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: twozero_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( twozero_ctrl_freq )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->zfreq = (float)GET_CK_FLOAT(ARGS);
    biquad_set_notch( d );
    
    // normalize the filter gain
    if( d->m_b1 > 0.0f )
        d->m_b0 = 1.0f / (1.0f+d->m_b1+d->m_b2);
    else
        d->m_b0 = 1.0f / (1.0f-d->m_b1+d->m_b2);
    d->m_b1 *= d->m_b0;
    d->m_b2 *= d->m_b0;
}

//-----------------------------------------------------------------------------
// name: twozero_ctrl_rad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( twozero_ctrl_rad )
{
    biquad_data * d = (biquad_data *)OBJ_MEMBER_UINT(SELF, biquad_offset_data );
    d->zrad = (float)GET_CK_FLOAT(ARGS);
    biquad_set_notch( d );

    // normalize the filter gain
    if( d->m_b1 > 0.0f )
        d->m_b0 = 1.0f / (1.0f+d->m_b1+d->m_b2);
    else
        d->m_b0 = 1.0f / (1.0f-d->m_b1+d->m_b2);
    d->m_b1 *= d->m_b0;
    d->m_b2 *= d->m_b0;    
}




//-----------------------------------------------------------------------------
// name: gQ
// desc: gQ filter - a la Dan Trueman
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: gQ_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( gQ_tick )
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: gQ_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gQ_ctrl_freq )
{
}

//-----------------------------------------------------------------------------
// name: gQ_ctrl_rad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gQ_ctrl_rad )
{
}




/*

//-----------------------------------------------------------------------------
// name: allpass
// desc: allpass filter
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: allpass_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( allpass_tick )
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: allpass_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( allpass_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: delay
// desc: ...
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: delay_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( delay_ctor )
{
}


//-----------------------------------------------------------------------------
// name: delay_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( delay_tick )
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: delay_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delay_ctrl_delay )
{
}

//-----------------------------------------------------------------------------
// name: delay_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delay_ctrl_max )
{
}

//-----------------------------------------------------------------------------
// name: delay_ctrl_tap()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delay_ctrl_tap )
{
}

//-----------------------------------------------------------------------------
// name: delay_ctrl_energy()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delay_ctrl_energy )
{
}




//-----------------------------------------------------------------------------
// name: delayA
// desc: delay - allpass interpolation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: delayA_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( delayA_ctor )
{
}

//-----------------------------------------------------------------------------
// name: delayA_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( delayA_tick )
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: delayA_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delayA_ctrl_delay )
{
}

//-----------------------------------------------------------------------------
// name: delayA_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delayA_ctrl_max )
{
}


//-----------------------------------------------------------------------------
// name: delayL
// desc: delay - linear interpolation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// name: delayL_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( delayL_ctor )
{
}

//-----------------------------------------------------------------------------
// name: delayL_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( delayL_tick )
{
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: delayL_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delayL_ctrl_delay )
{
}

//-----------------------------------------------------------------------------
// name: delayL_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( delayL_ctrl_max )
{
}

*/

