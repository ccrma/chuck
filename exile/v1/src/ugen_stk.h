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
// file: ugen_stk.h
// desc: ChucK import for Synthesis ToolKit (STK)
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//   also: Ari Lazier (alazier@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __UGEN_STK_H__
#define __UGEN_STK_H__

#include "chuck_dl.h"


// query
DLL_QUERY stk_query( Chuck_DL_Query * QUERY );
t_CKBOOL  stk_detach( t_CKUINT type, void * data );

// ADSR
UGEN_CTOR ADSR_ctor( t_CKTIME now );
UGEN_DTOR ADSR_dtor( t_CKTIME now, void * data );
UGEN_TICK ADSR_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG ADSR_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL ADSR_ctrl_attackTime( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_attackRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_decayTime( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_decayRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_sustainLevel( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_releaseTime( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_releaseRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_target( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_value( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_keyOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL ADSR_ctrl_keyOff( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_attackRate( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_decayRate( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_sustainLevel( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_releaseRate( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_target( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_value( t_CKTIME now, void * data, void * value );
UGEN_CGET ADSR_cget_state( t_CKTIME now, void * data, void * value );

// BiQuad
UGEN_CTOR BiQuad_ctor( t_CKTIME now );
UGEN_DTOR BiQuad_dtor( t_CKTIME now, void * data );
UGEN_TICK BiQuad_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BiQuad_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BiQuad_ctrl_b2( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_b1( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_a2( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_a1( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_pfreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_prad( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_zfreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_zrad( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_norm( t_CKTIME now, void * data, void * value );
UGEN_CTRL BiQuad_ctrl_eqzs( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_b2( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_b1( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_a2( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_a1( t_CKTIME now, void * data, void * value );
UGEN_CGET BiQuad_cget_a0( t_CKTIME now, void * data, void * value );

// Chorus
UGEN_CTOR Chorus_ctor( t_CKTIME now );
UGEN_DTOR Chorus_dtor( t_CKTIME now, void * data );
UGEN_TICK Chorus_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Chorus_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Chorus_ctrl_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CTRL Chorus_ctrl_modFreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Chorus_ctrl_mix( t_CKTIME now, void * data, void * value );
UGEN_CGET Chorus_cget_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CGET Chorus_cget_modFreq( t_CKTIME now, void * data, void * value );
UGEN_CGET Chorus_cget_mix( t_CKTIME now, void * data, void * value );

// Delay
UGEN_CTOR Delay_ctor( t_CKTIME now );
UGEN_DTOR Delay_dtor( t_CKTIME now, void * data );
UGEN_TICK Delay_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Delay_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Delay_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL Delay_ctrl_max( t_CKTIME now, void * data, void * value );
UGEN_CGET Delay_cget_delay( t_CKTIME now, void * data, void * value );
UGEN_CGET Delay_cget_max( t_CKTIME now, void * data, void * value );

// DelayA
UGEN_CTOR DelayA_ctor( t_CKTIME now );
UGEN_DTOR DelayA_dtor( t_CKTIME now, void * data );
UGEN_TICK DelayA_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG DelayA_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL DelayA_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL DelayA_ctrl_max( t_CKTIME now, void * data, void * value );
UGEN_CGET DelayA_cget_delay( t_CKTIME now, void * data, void * value );
UGEN_CGET DelayA_cget_max( t_CKTIME now, void * data, void * value );

// DelayL
UGEN_CTOR DelayL_ctor( t_CKTIME now );
UGEN_DTOR DelayL_dtor( t_CKTIME now, void * data );
UGEN_TICK DelayL_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG DelayL_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL DelayL_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL DelayL_ctrl_max( t_CKTIME now, void * data, void * value );
UGEN_CGET DelayL_cget_delay( t_CKTIME now, void * data, void * value );
UGEN_CGET DelayL_cget_max( t_CKTIME now, void * data, void * value );

// Echo
UGEN_CTOR Echo_ctor( t_CKTIME now );
UGEN_DTOR Echo_dtor( t_CKTIME now, void * data );
UGEN_TICK Echo_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Echo_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Echo_ctrl_delay( t_CKTIME now, void * data, void * value );
UGEN_CTRL Echo_ctrl_max( t_CKTIME now, void * data, void * value );
UGEN_CTRL Echo_ctrl_mix( t_CKTIME now, void * data, void * value );
UGEN_CGET Echo_cget_delay( t_CKTIME now, void * data, void * value );
UGEN_CGET Echo_cget_max( t_CKTIME now, void * data, void * value );
UGEN_CGET Echo_cget_mix( t_CKTIME now, void * data, void * value );

// Envelope
UGEN_CTOR Envelope_ctor( t_CKTIME now );
UGEN_DTOR Envelope_dtor( t_CKTIME now, void * data );
UGEN_TICK Envelope_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Envelope_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Envelope_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_ctrl_target( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_cget_target( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_ctrl_time( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_ctrl_value( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_cget_value( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_ctrl_keyOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Envelope_ctrl_keyOff( t_CKTIME now, void * data, void * value );
UGEN_CGET Envelope_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Envelope_cget_target( t_CKTIME now, void * data, void * value );
UGEN_CGET Envelope_cget_time( t_CKTIME now, void * data, void * value );
UGEN_CGET Envelope_cget_value( t_CKTIME now, void * data, void * value );

// Filter
UGEN_CTOR Filter_ctor( t_CKTIME now );
UGEN_DTOR Filter_dtor( t_CKTIME now, void * data );
UGEN_TICK Filter_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Filter_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Filter_ctrl_coefs( t_CKTIME now, void * data, void * value );
UGEN_CGET Filter_cget_coefs( t_CKTIME now, void * data, void * value );

// OnePole
UGEN_CTOR OnePole_ctor( t_CKTIME now );
UGEN_DTOR OnePole_dtor( t_CKTIME now, void * data );
UGEN_TICK OnePole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG OnePole_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL OnePole_ctrl_a1( t_CKTIME now, void * data, void * value );
UGEN_CTRL OnePole_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL OnePole_ctrl_pole( t_CKTIME now, void * data, void * value );
UGEN_CGET OnePole_cget_a1( t_CKTIME now, void * data, void * value );
UGEN_CGET OnePole_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET OnePole_cget_pole( t_CKTIME now, void * data, void * value );

// TwoPole
UGEN_CTOR TwoPole_ctor( t_CKTIME now );
UGEN_DTOR TwoPole_dtor( t_CKTIME now, void * data );
UGEN_TICK TwoPole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG TwoPole_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL TwoPole_ctrl_a1( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoPole_ctrl_a2( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoPole_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoPole_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoPole_ctrl_radius( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoPole_ctrl_norm( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_a1( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_a2( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_radius( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoPole_cget_norm( t_CKTIME now, void * data, void * value );


// OneZero
UGEN_CTOR OneZero_ctor( t_CKTIME now );
UGEN_DTOR OneZero_dtor( t_CKTIME now, void * data );
UGEN_TICK OneZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG OneZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL OneZero_ctrl_zero( t_CKTIME now, void * data, void * value );
UGEN_CTRL OneZero_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL OneZero_ctrl_b1( t_CKTIME now, void * data, void * value );
UGEN_CGET OneZero_cget_zero( t_CKTIME now, void * data, void * value );
UGEN_CGET OneZero_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET OneZero_cget_b1( t_CKTIME now, void * data, void * value );

// TwoZero
UGEN_CTOR TwoZero_ctor( t_CKTIME now);
UGEN_DTOR TwoZero_dtor( t_CKTIME now, void * data );
UGEN_TICK TwoZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG TwoZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL TwoZero_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoZero_ctrl_b1( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoZero_ctrl_b2( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoZero_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL TwoZero_ctrl_radius( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoZero_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoZero_cget_b1( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoZero_cget_b2( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoZero_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET TwoZero_cget_radius( t_CKTIME now, void * data, void * value );

// PoleZero
UGEN_CTOR PoleZero_ctor( t_CKTIME now );
UGEN_DTOR PoleZero_dtor( t_CKTIME now, void * data );
UGEN_TICK PoleZero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG PoleZero_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL PoleZero_ctrl_a1( t_CKTIME now, void * data, void * value );
UGEN_CTRL PoleZero_ctrl_b0( t_CKTIME now, void * data, void * value );
UGEN_CTRL PoleZero_ctrl_b1( t_CKTIME now, void * data, void * value );
UGEN_CTRL PoleZero_ctrl_blockZero( t_CKTIME now, void * data, void * value );
UGEN_CTRL PoleZero_ctrl_allpass( t_CKTIME now, void * data, void * value );
UGEN_CGET PoleZero_cget_a1( t_CKTIME now, void * data, void * value );
UGEN_CGET PoleZero_cget_b0( t_CKTIME now, void * data, void * value );
UGEN_CGET PoleZero_cget_b1( t_CKTIME now, void * data, void * value );
UGEN_CGET PoleZero_cget_blockZero( t_CKTIME now, void * data, void * value );
UGEN_CGET PoleZero_cget_allpass( t_CKTIME now, void * data, void * value );

// Noise
UGEN_CTOR Noise_ctor( t_CKTIME now );
UGEN_DTOR Noise_dtor( t_CKTIME now, void * data );
UGEN_TICK Noise_tick( t_CKTIME now, void * data, SAMPLE * in, SAMPLE * out );
UGEN_PMSG Noise_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Noise_ctrl_seed( t_CKTIME now, void * data, void * value );
UGEN_CGET Noise_cget_seed( t_CKTIME now, void * data, void * value );

// SubNoise
UGEN_CTOR SubNoise_ctor( t_CKTIME now );
UGEN_DTOR SubNoise_dtor( t_CKTIME now, void * data );
UGEN_TICK SubNoise_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG SubNoise_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL SubNoise_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET SubNoise_cget_rate( t_CKTIME now, void * data, void * value );

// JCRev 
UGEN_CTOR JCRev_ctor( t_CKTIME now );
UGEN_DTOR JCRev_dtor( t_CKTIME now, void * data );
UGEN_TICK JCRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG JCRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL JCRev_ctrl_mix( t_CKTIME now, void * data, void * value );
UGEN_CGET JCRev_cget_mix( t_CKTIME now, void * data, void * value );

// NRev
UGEN_CTOR NRev_ctor( t_CKTIME now );
UGEN_DTOR NRev_dtor( t_CKTIME now, void * data );
UGEN_TICK NRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG NRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL NRev_ctrl_mix( t_CKTIME now, void * data, void * value );
UGEN_CGET NRev_cget_mix( t_CKTIME now, void * data, void * value );

// PRCRev
UGEN_CTOR PRCRev_ctor( t_CKTIME now );
UGEN_DTOR PRCRev_dtor( t_CKTIME now, void * data );
UGEN_TICK PRCRev_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG PRCRev_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL PRCRev_ctrl_mix( t_CKTIME now, void * data, void * value );
UGEN_CGET PRCRev_cget_mix( t_CKTIME now, void * data, void * value );

// WaveLoop
UGEN_CTOR WaveLoop_ctor( t_CKTIME now );
UGEN_DTOR WaveLoop_dtor( t_CKTIME now, void * data );
UGEN_TICK WaveLoop_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG WaveLoop_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL WaveLoop_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL WaveLoop_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL WaveLoop_ctrl_phase( t_CKTIME now, void * data, void * value );
UGEN_CTRL WaveLoop_ctrl_phaseOffset( t_CKTIME now, void * data, void * value );
UGEN_CTRL WaveLoop_ctrl_path( t_CKTIME now, void * data, void * value );
UGEN_CGET WaveLoop_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET WaveLoop_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET WaveLoop_cget_phase( t_CKTIME now, void * data, void * value );
UGEN_CGET WaveLoop_cget_phaseOffset( t_CKTIME now, void * data, void * value );
UGEN_CGET WaveLoop_cget_path( t_CKTIME now, void * data, void * value );

// WvIn
UGEN_CTOR WvIn_ctor( t_CKTIME now);
UGEN_DTOR WvIn_dtor( t_CKTIME now, void * data );
UGEN_TICK WvIn_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG WvIn_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL WvIn_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvIn_ctrl_path( t_CKTIME now, void * data, void * value );
UGEN_CGET WvIn_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET WvIn_cget_path( t_CKTIME now, void * data, void * value );

// WvOut
UGEN_CTOR WvOut_ctor( t_CKTIME now );
UGEN_DTOR WvOut_dtor( t_CKTIME now, void * data );
UGEN_TICK WvOut_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG WvOut_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL WvOut_ctrl_filename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_matFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_sndFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_wavFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_rawFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_aifFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_closeFile( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_record( t_CKTIME now, void * data, void * value );
UGEN_CTRL WvOut_ctrl_autoPrefix( t_CKTIME now, void * data, void * value );
UGEN_CGET WvOut_cget_filename( t_CKTIME now, void * data, void * value );
UGEN_CGET WvOut_cget_record( t_CKTIME now, void * data, void * value );
UGEN_CGET WvOut_cget_autoPrefix( t_CKTIME now, void * data, void * value );

// FM
UGEN_CTOR FM_ctor( t_CKTIME now );
UGEN_DTOR FM_dtor( t_CKTIME now, void * data );
UGEN_TICK FM_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG FM_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL FM_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_modSpeed( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_control1( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_control2( t_CKTIME now, void * data, void * value );
UGEN_CTRL FM_ctrl_afterTouch( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_modSpeed( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_control1( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_control2( t_CKTIME now, void * data, void * value );
UGEN_CGET FM_cget_afterTouch( t_CKTIME now, void * data, void * value );

// FormSwep
UGEN_CTOR FormSwep_ctor( t_CKTIME now);
UGEN_DTOR FormSwep_dtor( t_CKTIME now, void * data );
UGEN_TICK FormSwep_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG FormSwep_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL FormSwep_ctrl_frequency( t_CKTIME now, void * data, void * value );
UGEN_CGET FormSwep_cget_frequency( t_CKTIME now, void * data, void * value );
UGEN_CTRL FormSwep_ctrl_radius( t_CKTIME now, void * data, void * value );
UGEN_CGET FormSwep_cget_radius( t_CKTIME now, void * data, void * value );
UGEN_CTRL FormSwep_ctrl_gain( t_CKTIME now, void * data, void * value );
UGEN_CGET FormSwep_cget_gain( t_CKTIME now, void * data, void * value );
UGEN_CTRL FormSwep_ctrl_sweepRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL FormSwep_ctrl_sweepTime( t_CKTIME now, void * data, void * value );
UGEN_CGET FormSwep_cget_sweepRate( t_CKTIME now, void * data, void * value );
UGEN_CGET FormSwep_cget_sweepTime( t_CKTIME now, void * data, void * value );

// Modulate
UGEN_CTOR Modulate_ctor( t_CKTIME now );
UGEN_DTOR Modulate_dtor( t_CKTIME now, void * data );
UGEN_TICK Modulate_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Modulate_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Modulate_ctrl_vibratoGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modulate_ctrl_vibratoRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modulate_ctrl_randomGain( t_CKTIME now, void * data, void * value );
UGEN_CGET Modulate_cget_vibratoGain( t_CKTIME now, void * data, void * value );
UGEN_CGET Modulate_cget_vibratoRate( t_CKTIME now, void * data, void * value );
UGEN_CGET Modulate_cget_randomGain( t_CKTIME now, void * data, void * value );

// PitShift
UGEN_CTOR PitShift_ctor( t_CKTIME now );
UGEN_DTOR PitShift_dtor( t_CKTIME now, void * data );
UGEN_TICK PitShift_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG PitShift_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL PitShift_ctrl_shift( t_CKTIME now, void * data, void * value );
UGEN_CTRL PitShift_ctrl_effectMix( t_CKTIME now, void * data, void * value );
UGEN_CGET PitShift_cget_shift( t_CKTIME now, void * data, void * value );
UGEN_CGET PitShift_cget_effectMix( t_CKTIME now, void * data, void * value );

// Sampler
UGEN_CTOR Sampler_ctor( t_CKTIME now );
UGEN_DTOR Sampler_dtor( t_CKTIME now, void * data );
UGEN_TICK Sampler_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Sampler_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// Drummer
UGEN_CTOR Drummer_ctor( t_CKTIME now );
UGEN_DTOR Drummer_dtor( t_CKTIME now, void * data );
UGEN_TICK Drummer_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Drummer_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// BandedWG
UGEN_CTOR BandedWG_ctor( t_CKTIME now );
UGEN_DTOR BandedWG_dtor( t_CKTIME now, void * data );
UGEN_TICK BandedWG_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BandedWG_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BandedWG_ctrl_strikePosition( t_CKTIME now, void * data, void * value );
UGEN_CGET BandedWG_cget_strikePosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET BandedWG_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_preset( t_CKTIME now, void * data, void * value );
UGEN_CGET BandedWG_cget_preset( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_bowRate( t_CKTIME now, void * data, void * value );
UGEN_CGET BandedWG_cget_bowRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_bowPressure( t_CKTIME now, void * data, void * value );
UGEN_CGET BandedWG_cget_bowPressure( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_startBowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_stopBowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_aftertouch( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_bowTarget( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_modWheel( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_modFreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_sustain( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_trackVelocity( t_CKTIME now, void * data, void * value );
UGEN_CTRL BandedWG_ctrl_portamento( t_CKTIME now, void * data, void * value );


// BeeThree
UGEN_CTOR BeeThree_ctor( t_CKTIME now );
UGEN_DTOR BeeThree_dtor( t_CKTIME now, void * data );
UGEN_TICK BeeThree_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BeeThree_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BeeThree_ctrl_noteOn( t_CKTIME now, void * data, void * value );

// BlowBotl
UGEN_CTOR BlowBotl_ctor( t_CKTIME now );
UGEN_DTOR BlowBotl_dtor( t_CKTIME now, void * data );
UGEN_TICK BlowBotl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BlowBotl_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BlowBotl_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowBotl_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowBotl_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowBotl_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowBotl_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowBotl_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowBotl_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowBotl_cget_rate( t_CKTIME now, void * data, void * value );

// BlowHole
UGEN_CTOR BlowHole_ctor( t_CKTIME now );
UGEN_DTOR BlowHole_dtor( t_CKTIME now, void * data );
UGEN_TICK BlowHole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BlowHole_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BlowHole_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_tonehole( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_vent( t_CKTIME now, void * data, void * value );
UGEN_CTRL BlowHole_ctrl_reed( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowHole_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowHole_cget_tonehole( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowHole_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowHole_cget_vent( t_CKTIME now, void * data, void * value );
UGEN_CGET BlowHole_cget_reed( t_CKTIME now, void * data, void * value );

// Bowed
UGEN_CTOR Bowed_ctor( t_CKTIME now );
UGEN_DTOR Bowed_dtor( t_CKTIME now, void * data );
UGEN_TICK Bowed_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Bowed_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Bowed_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_startBowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_stopBowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Bowed_ctrl_vibrato( t_CKTIME now, void * data, void * value );
UGEN_CGET Bowed_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Bowed_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Bowed_cget_vibrato( t_CKTIME now, void * data, void * value );

// BowTabl
UGEN_CTOR BowTabl_ctor( t_CKTIME now );
UGEN_DTOR BowTabl_dtor( t_CKTIME now, void * data );
UGEN_TICK BowTabl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG BowTabl_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL BowTabl_ctrl_offset( t_CKTIME now, void * data, void * value );
UGEN_CTRL BowTabl_ctrl_slope( t_CKTIME now, void * data, void * value );

// Brass
UGEN_CTOR Brass_ctor( t_CKTIME now );
UGEN_DTOR Brass_dtor( t_CKTIME now, void * data );
UGEN_TICK Brass_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Brass_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Brass_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Brass_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Brass_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Brass_ctrl_lip( t_CKTIME now, void * data, void * value );
UGEN_CGET Brass_cget_lip( t_CKTIME now, void * data, void * value );

// Clarinet
UGEN_CTOR Clarinet_ctor( t_CKTIME now );
UGEN_DTOR Clarinet_dtor( t_CKTIME now, void * data );
UGEN_TICK Clarinet_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Clarinet_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Clarinet_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Clarinet_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Clarinet_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Clarinet_cget_rate( t_CKTIME now, void * data, void * value );

// Flute
UGEN_CTOR Flute_ctor( t_CKTIME now );
UGEN_DTOR Flute_dtor( t_CKTIME now, void * data );
UGEN_TICK Flute_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Flute_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Flute_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Flute_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Flute_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_jetReflection( t_CKTIME now, void * data, void * value );
UGEN_CGET Flute_cget_jetReflection( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_jetDelay( t_CKTIME now, void * data, void * value );
UGEN_CGET Flute_cget_jetDelay( t_CKTIME now, void * data, void * value );
UGEN_CTRL Flute_ctrl_endReflection( t_CKTIME now, void * data, void * value );
UGEN_CGET Flute_cget_endReflection( t_CKTIME now, void * data, void * value );


// FMVoices
UGEN_CTOR FMVoices_ctor( t_CKTIME now );
UGEN_DTOR FMVoices_dtor( t_CKTIME now, void * data );
UGEN_TICK FMVoices_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG FMVoices_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL FMVoices_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_vowel( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_spectralTilt( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_lfoSpeed( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_lfoDepth( t_CKTIME now, void * data, void * value );
UGEN_CTRL FMVoices_ctrl_adsrTarget( t_CKTIME now, void * data, void * value );
UGEN_CGET FMVoices_cget_freq( t_CKTIME now, void * data, void * value );

// HevyMetl
UGEN_CTOR HevyMetl_ctor( t_CKTIME now );
UGEN_DTOR HevyMetl_dtor( t_CKTIME now, void * data );
UGEN_TICK HevyMetl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG HevyMetl_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL HevyMetl_ctrl_noteOn( t_CKTIME now, void * data, void * value );

// JetTabl
UGEN_CTOR JetTabl_ctor( t_CKTIME now );
UGEN_DTOR JetTabl_dtor( t_CKTIME now, void * data );
UGEN_TICK JetTabl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG JetTabl_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// Mandolin
UGEN_CTOR Mandolin_ctor( t_CKTIME now );
UGEN_DTOR Mandolin_dtor( t_CKTIME now, void * data );
UGEN_TICK Mandolin_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Mandolin_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Mandolin_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_pluckPos( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_bodySize( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_stringDamping( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_stringDetune( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mandolin_ctrl_afterTouch( t_CKTIME now, void * data, void * value );

UGEN_CGET Mandolin_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Mandolin_cget_pluckPos( t_CKTIME now, void * data, void * value );
UGEN_CGET Mandolin_cget_bodySize( t_CKTIME now, void * data, void * value );
UGEN_CGET Mandolin_cget_stringDamping( t_CKTIME now, void * data, void * value );
UGEN_CGET Mandolin_cget_stringDetune( t_CKTIME now, void * data, void * value );



// Modal
UGEN_CTOR Modal_ctor( t_CKTIME now );
UGEN_DTOR Modal_dtor( t_CKTIME now, void * data );
UGEN_TICK Modal_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Modal_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Modal_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_masterGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_directGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_mode( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_modeRatio( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_modeRadius( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_modeGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_strike( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_damp( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Modal_ctrl_noteOff( t_CKTIME now, void * data, void * value );

// ModalBar
UGEN_CTOR ModalBar_ctor( t_CKTIME now );
UGEN_DTOR ModalBar_dtor( t_CKTIME now, void * data );
UGEN_TICK ModalBar_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG ModalBar_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL ModalBar_ctrl_strike( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_damp( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_preset( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_strikePosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_stickHardness( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_masterGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_directGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_mode( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeRatio( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeRadius( t_CKTIME now, void * data, void * value );


UGEN_CTRL ModalBar_ctrl_preset( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_strikePosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_stickHardness( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_masterGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_directGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_mode( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeRatio( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeRadius( t_CKTIME now, void * data, void * value );
UGEN_CTRL ModalBar_ctrl_modeGain( t_CKTIME now, void * data, void * value );

UGEN_CGET ModalBar_cget_preset( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_strikePosition( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_stickHardness( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_masterGain( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_directGain( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_mode( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_modeRatio( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_modeRadius( t_CKTIME now, void * data, void * value );
UGEN_CGET ModalBar_cget_modeGain( t_CKTIME now, void * data, void * value );


// Moog
UGEN_CTOR Moog_ctor( t_CKTIME now );
UGEN_DTOR Moog_dtor( t_CKTIME now, void *data );
UGEN_TICK Moog_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Moog_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Moog_ctrl_freq( t_CKTIME now, void * data, void *value );
UGEN_CTRL Moog_ctrl_noteOn( t_CKTIME now, void * data, void *value );
UGEN_CTRL Moog_ctrl_modSpeed( t_CKTIME now, void * data, void * value );
UGEN_CTRL Moog_ctrl_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CTRL Moog_ctrl_filterQ( t_CKTIME now, void * data, void * value );
UGEN_CTRL Moog_ctrl_filterSweepRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Moog_ctrl_afterTouch( t_CKTIME now, void * data, void * value );

UGEN_CGET Moog_cget_freq( t_CKTIME now, void * data, void *value );
UGEN_CGET Moog_cget_modSpeed( t_CKTIME now, void * data, void * value );
UGEN_CGET Moog_cget_modDepth( t_CKTIME now, void * data, void * value );
UGEN_CGET Moog_cget_filterQ( t_CKTIME now, void * data, void * value );
UGEN_CGET Moog_cget_filterSweepRate( t_CKTIME now, void * data, void * value );


// PercFlut
UGEN_CTOR PercFlut_ctor( t_CKTIME now );
UGEN_DTOR PercFlut_dtor( t_CKTIME now, void * data );
UGEN_TICK PercFlut_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG PercFlut_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL PercFlut_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL PercFlut_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET PercFlut_cget_freq( t_CKTIME now, void * data, void * value );

// Plucked
UGEN_CTOR Plucked_ctor( t_CKTIME now );
UGEN_DTOR Plucked_dtor( t_CKTIME now, void * data );
UGEN_TICK Plucked_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Plucked_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Plucked_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Plucked_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL Plucked_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Plucked_ctrl_noteOff( t_CKTIME now, void * data, void * value );

// PluckTwo
UGEN_CTOR PluckTwo_ctor( t_CKTIME now );
UGEN_DTOR PluckTwo_dtor( t_CKTIME now, void * data );
UGEN_TICK PluckTwo_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG PluckTwo_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL PluckTwo_ctrl_detune( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_pluckPosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_baseLoopGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL PluckTwo_ctrl_noteOff( t_CKTIME now, void * data, void * value );

// ReedTabl
UGEN_CTOR ReedTabl_ctor( t_CKTIME now );
UGEN_DTOR ReedTabl_dtor( t_CKTIME now, void * data );
UGEN_TICK ReedTabl_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG ReedTabl_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL ReedTabl_ctrl_offset( t_CKTIME now, void * data, void * value );
UGEN_CTRL ReedTabl_ctrl_slope( t_CKTIME now, void * data, void * value );

// Resonate
UGEN_CTOR Resonate_ctor( t_CKTIME now );
UGEN_DTOR Resonate_dtor( t_CKTIME now, void * data );
UGEN_TICK Resonate_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Resonate_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// Rhodey
UGEN_CTOR Rhodey_ctor( t_CKTIME now );
UGEN_DTOR Rhodey_dtor( t_CKTIME now, void * data );
UGEN_TICK Rhodey_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Rhodey_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Rhodey_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Rhodey_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Rhodey_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Rhodey_cget_freq( t_CKTIME now, void * data, void * value );

// Saxofony
UGEN_CTOR Saxofony_ctor( t_CKTIME now );
UGEN_DTOR Saxofony_dtor( t_CKTIME now, void * data );
UGEN_TICK Saxofony_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Saxofony_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Saxofony_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_rate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_blowPosition( t_CKTIME now, void * data, void * value );
UGEN_CGET Saxofony_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET Saxofony_cget_rate( t_CKTIME now, void * data, void * value );
UGEN_CGET Saxofony_cget_blowPosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Saxofony_ctrl_noteOff( t_CKTIME now, void * data, void * value );

// Shakers
UGEN_CTOR Shakers_ctor( t_CKTIME now );
UGEN_DTOR Shakers_dtor( t_CKTIME now, void * data );
UGEN_TICK Shakers_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Shakers_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Shakers_ctrl_which( t_CKTIME now, void * data, void * value );
UGEN_CTRL Shakers_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Shakers_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Shakers_ctrl_freq( t_CKTIME now, void * data, void * value );

UGEN_CGET Shakers_cget_which( t_CKTIME now, void * data, void * value );
UGEN_CGET Shakers_cget_freq( t_CKTIME now, void * data, void * value );

// Simple
UGEN_CTOR Simple_ctor( t_CKTIME now );
UGEN_DTOR Simple_dtor( t_CKTIME now, void * data );
UGEN_PMSG Simple_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Simple_ctrl_freq( t_CKTIME now, void * data, void * value );

// SingWave
UGEN_CTOR SingWave_ctor( t_CKTIME now );
UGEN_DTOR SingWave_dtor( t_CKTIME now, void * data );
UGEN_TICK SingWave_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG SingWave_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL SingWave_ctrl_wavFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_rawFilename( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_norm( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_sweepRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_vibratoRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_vibratoGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_randomGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_gainTarget( t_CKTIME now, void * data, void * value );
UGEN_CTRL SingWave_ctrl_gainRate( t_CKTIME now, void * data, void * value );

// Sitar
UGEN_CTOR Sitar_ctor( t_CKTIME now );
UGEN_DTOR Sitar_dtor( t_CKTIME now, void * data );
UGEN_TICK Sitar_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Sitar_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Sitar_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sitar_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sitar_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sitar_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sitar_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sitar_cget_freq( t_CKTIME now, void * data, void * value );

// Sphere
UGEN_CTOR Sphere_ctor( t_CKTIME now );
UGEN_DTOR Sphere_dtor( t_CKTIME now, void * data );
UGEN_TICK Sphere_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Sphere_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Sphere_ctrl_radius( t_CKTIME now, void * data, void * value );
UGEN_CTRL Sphere_ctrl_mass( t_CKTIME now, void * data, void * value );

// StifKarp
UGEN_CTOR StifKarp_ctor( t_CKTIME now );
UGEN_DTOR StifKarp_dtor( t_CKTIME now, void * data );
UGEN_TICK StifKarp_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG StifKarp_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL StifKarp_ctrl_pluck( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_clear( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_noteOff( t_CKTIME now, void * data, void * value );

UGEN_CTRL StifKarp_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET StifKarp_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_pickupPosition( t_CKTIME now, void * data, void * value );
UGEN_CGET StifKarp_cget_pickupPosition( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_stretch( t_CKTIME now, void * data, void * value );
UGEN_CGET StifKarp_cget_stretch( t_CKTIME now, void * data, void * value );
UGEN_CTRL StifKarp_ctrl_baseLoopGain( t_CKTIME now, void * data, void * value );
UGEN_CGET StifKarp_cget_baseLoopGain( t_CKTIME now, void * data, void * value );

// TubeBell
UGEN_CTOR TubeBell_ctor( t_CKTIME now );
UGEN_DTOR TubeBell_dtor( t_CKTIME now, void * data );
UGEN_TICK TubeBell_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG TubeBell_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL TubeBell_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL TubeBell_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET TubeBell_cget_freq( t_CKTIME now, void * data, void * value );

// Voicer
UGEN_CTOR Voicer_ctor( t_CKTIME now );
UGEN_DTOR Voicer_dtor( t_CKTIME now, void * data );
UGEN_TICK Voicer_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Voicer_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// VoicForm
UGEN_CTOR VoicForm_ctor( t_CKTIME now );
UGEN_DTOR VoicForm_dtor( t_CKTIME now, void * data );
UGEN_TICK VoicForm_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG VoicForm_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL VoicForm_ctrl_phoneme( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_speak( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_quiet( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_voiced( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_unVoiced( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_pitchSweepRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_voiceMix( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_selPhoneme( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_vibratoFreq( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_vibratoGain( t_CKTIME now, void * data, void * value );
UGEN_CTRL VoicForm_ctrl_loudness( t_CKTIME now, void * data, void * value );

UGEN_CGET VoicForm_cget_phoneme( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_freq( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_voiced( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_unVoiced( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_pitchSweepRate( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_voiceMix( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_selPhoneme( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_vibratoFreq( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_vibratoGain( t_CKTIME now, void * data, void * value );
UGEN_CGET VoicForm_cget_loudness( t_CKTIME now, void * data, void * value );



// Whistle
UGEN_CTOR Whistle_ctor( t_CKTIME now );
UGEN_DTOR Whistle_dtor( t_CKTIME now, void * data );
UGEN_TICK Whistle_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Whistle_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Whistle_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Whistle_ctrl_startBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Whistle_ctrl_stopBlowing( t_CKTIME now, void * data, void * value );
UGEN_CTRL Whistle_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Whistle_ctrl_noteOff( t_CKTIME now, void * data, void * value );

// Wurley
UGEN_CTOR Wurley_ctor( t_CKTIME now );
UGEN_DTOR Wurley_dtor( t_CKTIME now, void * data );
UGEN_TICK Wurley_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Wurley_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Wurley_ctrl_freq( t_CKTIME now, void * data, void * value );
UGEN_CTRL Wurley_ctrl_noteOn( t_CKTIME now, void * data, void * value );
UGEN_CTRL Wurley_ctrl_noteOff( t_CKTIME now, void * data, void * value );
UGEN_CGET Wurley_cget_freq( t_CKTIME now, void * data, void * value );



// Instrmnt
UGEN_CTOR Instrmnt_ctor( t_CKTIME now);
UGEN_DTOR Instrmnt_dtor( t_CKTIME now, void * data );
UGEN_TICK Instrmnt_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Instrmnt_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Instrmnt_ctrl_freq( t_CKTIME now, void * data, void * value );

// Mesh2D
UGEN_CTOR Mesh2D_ctor( t_CKTIME now );
UGEN_DTOR Mesh2D_dtor( t_CKTIME now, void * data );
UGEN_TICK Mesh2D_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Mesh2D_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
//UGEN_CTRL Mesh2D_ctrl_NY( t_CKTIME now, void * data, void * value );
//UGEN_CTRL Mesh2D_ctrl_NY( t_CKTIME now, void * data, void * value );
//UGEN_CTRL Mesh2D_ctrl_NY( t_CKTIME now, void * data, void * value );
//UGEN_CTRL Mesh2D_ctrl_NX( t_CKTIME now, void * data, void * value );
//UGEN_CTRL Mesh2D_ctrl_NX( t_CKTIME now, void * data, void * value );
//UGEN_CTRL Mesh2D_ctrl_NX( t_CKTIME now, void * data, void * value );
UGEN_CTRL Mesh2D_ctrl_decay( t_CKTIME now, void * data, void * value );

// Reverb
UGEN_CTOR Reverb_ctor( t_CKTIME now );
UGEN_DTOR Reverb_dtor( t_CKTIME now, void * data );
UGEN_TICK Reverb_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Reverb_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Reverb_ctrl_effectMix( t_CKTIME now, void * data, void * value );

// Socket
UGEN_CTOR Socket_ctor( t_CKTIME now );
UGEN_DTOR Socket_dtor( t_CKTIME now, void * data );
UGEN_TICK Socket_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Socket_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// Stk
UGEN_CTOR Stk_ctor( t_CKTIME now );
UGEN_DTOR Stk_dtor( t_CKTIME now, void * data );
UGEN_TICK Stk_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Stk_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Stk_ctrl_sampleRate( t_CKTIME now, void * data, void * value );
UGEN_CTRL Stk_ctrl_rawwavePath( t_CKTIME now, void * data, void * value );

// Table
UGEN_CTOR Table_ctor( t_CKTIME now );
UGEN_DTOR Table_dtor( t_CKTIME now, void * data );
UGEN_TICK Table_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Table_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// TcpWvIn
UGEN_CTOR TcpWvIn_ctor( t_CKTIME now );
UGEN_DTOR TcpWvIn_dtor( t_CKTIME now, void * data );
UGEN_TICK TcpWvIn_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG TcpWvIn_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// TcpWvOut
UGEN_CTOR TcpWvOut_ctor( t_CKTIME now );
UGEN_DTOR TcpWvOut_dtor( t_CKTIME now, void * data );
UGEN_TICK TcpWvOut_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG TcpWvOut_pmsg( t_CKTIME now, void * data, const char * msg, void * value );

// Vector3D
UGEN_CTOR Vector3D_ctor( t_CKTIME now );
UGEN_DTOR Vector3D_dtor( t_CKTIME now, void * data );
UGEN_TICK Vector3D_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out );
UGEN_PMSG Vector3D_pmsg( t_CKTIME now, void * data, const char * msg, void * value );
UGEN_CTRL Vector3D_ctrl_x( t_CKTIME now, void * data, void * value );
UGEN_CTRL Vector3D_ctrl_y( t_CKTIME now, void * data, void * value );
UGEN_CTRL Vector3D_ctrl_z( t_CKTIME now, void * data, void * value );




#endif
