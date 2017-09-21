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
// file: ugen_stk.cpp
// desc: ChucK import for Synthesis ToolKit (STK)
//                        by Perry Cook and Gary Scavone
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
//         Ari Lazier (alazier@cs.princeton.edu)
//         Philip L. Davidson (philipd@cs.princeton.edu)
//         Mark Daly (mdaly@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ugen_stk.h"
#include "chuck_type.h"
#include "util_math.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_lang.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <limits.h>




//------------------------------------------------------------------------------
// name: union what
// desc: the litmus
//------------------------------------------------------------------------------
union what { long x; char y[sizeof(long)]; };
t_CKBOOL little_endian = FALSE;

// static t_CKUINT g_srate = 0;
// filter member data offset

static t_CKUINT Instrmnt_offset_data = 0;
//static t_CKUINT BandedWG_offset_data = 0;
//static t_CKUINT BlowBotl_offset_data = 0;
//static t_CKUINT BlowHole_offset_data = 0;
//static t_CKUINT Bowed_offset_data = 0;
//static t_CKUINT Brass_offset_data = 0;
//static t_CKUINT Clarinet_offset_data = 0;
//static t_CKUINT Flute_offset_data = 0;
//static t_CKUINT Mandolin_offset_data = 0;
//static t_CKUINT ModalBar_offset_data = 0;
//static t_CKUINT Moog_offset_data = 0;
//static t_CKUINT Saxofony_offset_data = 0;
//static t_CKUINT Shakers_offset_data = 0;
//static t_CKUINT Sitar_offset_data = 0;
//static t_CKUINT StifKarp_offset_data = 0;
//static t_CKUINT VoicForm_offset_data = 0;
static t_CKUINT FM_offset_data = 0;
//static t_CKUINT BeeThree_offset_data = 0;
//static t_CKUINT FMVoices_offset_data = 0;
//static t_CKUINT HevyMetl_offset_data = 0;
//static t_CKUINT PercFlut_offset_data = 0;
//static t_CKUINT Rhodey_offset_data = 0;
//static t_CKUINT TubeBell_offset_data = 0;
//static t_CKUINT Wurley_offset_data = 0;
static t_CKUINT FormSwep_offset_data = 0;
static t_CKUINT Delay_offset_data = 0;
static t_CKUINT DelayA_offset_data = 0;
static t_CKUINT DelayL_offset_data = 0;
static t_CKUINT Echo_offset_data = 0;
static t_CKUINT Envelope_offset_data = 0;
// static t_CKUINT Envelope_offset_data = 0;
static t_CKUINT BiQuad_offset_data = 0;
static t_CKUINT FilterStk_offset_data = 0;
static t_CKUINT OnePole_offset_data = 0;
static t_CKUINT TwoPole_offset_data = 0;
static t_CKUINT OneZero_offset_data = 0;
static t_CKUINT TwoZero_offset_data = 0;
static t_CKUINT PoleZero_offset_data = 0;
static t_CKUINT JCRev_offset_data = 0;
static t_CKUINT NRev_offset_data = 0;
static t_CKUINT PRCRev_offset_data = 0;
static t_CKUINT Chorus_offset_data = 0;
static t_CKUINT Modulate_offset_data = 0;
static t_CKUINT SubNoise_offset_data = 0;
static t_CKUINT WvIn_offset_data = 0;
//static t_CKUINT WaveLoop_offset_data = 0;
static t_CKUINT WvOut_offset_data = 0;
static t_CKUINT PitShift_offset_data = 0;
static t_CKUINT BLT_offset_data = 0;
static t_CKUINT JetTabl_offset_data = 0;

static t_CKUINT Mesh2D_offset_data = 0;

static t_CKUINT MidiFileIn_offset_data = 0;

static t_CKINT ADSR_state_ATTACK = ADSR::ATTACK;
static t_CKINT ADSR_state_DECAY = ADSR::DECAY;
static t_CKINT ADSR_state_SUSTAIN = ADSR::SUSTAIN;
static t_CKINT ADSR_state_RELEASE = ADSR::RELEASE;
static t_CKINT ADSR_state_DONE = ADSR::DONE;

// SKINI
/*
SkiniIn SkiniMessage => while(skin.getnextmessage(msg))
msg - type, data parameters, time stamp
SkiniOut.send( midi message )
horn ok please -> written at the back of trucks, always, makes no sense.
*/


// ADSR
CK_DLL_CTOR( ADSR_ctor );
CK_DLL_DTOR( ADSR_dtor );
CK_DLL_TICK( ADSR_tick );
CK_DLL_PMSG( ADSR_pmsg );
CK_DLL_CTRL( ADSR_ctrl_attackTime );
CK_DLL_CTRL( ADSR_ctrl_attackRate );
CK_DLL_CTRL( ADSR_ctrl_decayTime );
CK_DLL_CTRL( ADSR_ctrl_decayRate );
CK_DLL_CTRL( ADSR_ctrl_sustainLevel );
CK_DLL_CTRL( ADSR_ctrl_releaseTime );
CK_DLL_CTRL( ADSR_ctrl_releaseRate );
CK_DLL_CTRL( ADSR_ctrl_set );
CK_DLL_CTRL( ADSR_ctrl_set2 );
CK_DLL_CGET( ADSR_cget_attackTime );
CK_DLL_CGET( ADSR_cget_attackRate );
CK_DLL_CGET( ADSR_cget_decayTime );
CK_DLL_CGET( ADSR_cget_decayRate );
CK_DLL_CGET( ADSR_cget_sustainLevel );
CK_DLL_CGET( ADSR_cget_releaseTime );
CK_DLL_CGET( ADSR_cget_releaseRate );
CK_DLL_CGET( ADSR_cget_state );

// BiQuad
CK_DLL_CTOR( BiQuad_ctor );
CK_DLL_DTOR( BiQuad_dtor );
CK_DLL_TICK( BiQuad_tick );
CK_DLL_PMSG( BiQuad_pmsg );
CK_DLL_CTRL( BiQuad_ctrl_b2 );
CK_DLL_CTRL( BiQuad_ctrl_b1 );
CK_DLL_CTRL( BiQuad_ctrl_b0 );
CK_DLL_CTRL( BiQuad_ctrl_a2 );
CK_DLL_CTRL( BiQuad_ctrl_a1 );
CK_DLL_CGET( BiQuad_ctrl_a0 );
CK_DLL_CTRL( BiQuad_ctrl_pfreq );
CK_DLL_CTRL( BiQuad_ctrl_prad );
CK_DLL_CTRL( BiQuad_ctrl_zfreq );
CK_DLL_CTRL( BiQuad_ctrl_zrad );
CK_DLL_CTRL( BiQuad_ctrl_norm );
CK_DLL_CTRL( BiQuad_ctrl_eqzs );
CK_DLL_CGET( BiQuad_cget_b2 );
CK_DLL_CGET( BiQuad_cget_b1 );
CK_DLL_CGET( BiQuad_cget_b0 );
CK_DLL_CGET( BiQuad_cget_a2 );
CK_DLL_CGET( BiQuad_cget_a1 );
CK_DLL_CGET( BiQuad_cget_a0 );
CK_DLL_CTRL( BiQuad_cget_pfreq );
CK_DLL_CTRL( BiQuad_cget_prad );
CK_DLL_CTRL( BiQuad_cget_zfreq );
CK_DLL_CTRL( BiQuad_cget_zrad );

// Chorus
CK_DLL_CTOR( Chorus_ctor );
CK_DLL_DTOR( Chorus_dtor );
CK_DLL_TICK( Chorus_tick );
CK_DLL_PMSG( Chorus_pmsg );
CK_DLL_CTRL( Chorus_ctrl_modDepth );
CK_DLL_CTRL( Chorus_ctrl_modFreq );
CK_DLL_CTRL( Chorus_ctrl_baseDelay );
CK_DLL_CTRL( Chorus_ctrl_set );
CK_DLL_CTRL( Chorus_ctrl_mix );
CK_DLL_CGET( Chorus_cget_modDepth );
CK_DLL_CGET( Chorus_cget_modFreq );
CK_DLL_CGET( Chorus_cget_baseDelay );
CK_DLL_CGET( Chorus_cget_mix );

// Delay
CK_DLL_CTOR( Delay_ctor );
CK_DLL_DTOR( Delay_dtor );
CK_DLL_TICK( Delay_tick );
CK_DLL_PMSG( Delay_pmsg );
CK_DLL_CTRL( Delay_ctrl_delay );
CK_DLL_CTRL( Delay_ctrl_max );
CK_DLL_CGET( Delay_cget_delay );
CK_DLL_CGET( Delay_cget_max );
CK_DLL_CGET( Delay_clear );

// DelayA
CK_DLL_CTOR( DelayA_ctor );
CK_DLL_DTOR( DelayA_dtor );
CK_DLL_TICK( DelayA_tick );
CK_DLL_PMSG( DelayA_pmsg );
CK_DLL_CTRL( DelayA_ctrl_delay );
CK_DLL_CTRL( DelayA_ctrl_max );
CK_DLL_CGET( DelayA_cget_delay );
CK_DLL_CGET( DelayA_cget_max );
CK_DLL_CGET( DelayA_clear );

// DelayL
CK_DLL_CTOR( DelayL_ctor );
CK_DLL_DTOR( DelayL_dtor );
CK_DLL_TICK( DelayL_tick );
CK_DLL_PMSG( DelayL_pmsg );
CK_DLL_CTRL( DelayL_ctrl_delay );
CK_DLL_CTRL( DelayL_ctrl_max );
CK_DLL_CGET( DelayL_cget_delay );
CK_DLL_CGET( DelayL_cget_max );
CK_DLL_CGET( DelayL_clear );

// Echo
CK_DLL_CTOR( Echo_ctor );
CK_DLL_DTOR( Echo_dtor );
CK_DLL_TICK( Echo_tick );
CK_DLL_PMSG( Echo_pmsg );
CK_DLL_CTRL( Echo_ctrl_delay );
CK_DLL_CTRL( Echo_ctrl_max );
CK_DLL_CTRL( Echo_ctrl_mix );
CK_DLL_CGET( Echo_cget_delay );
CK_DLL_CGET( Echo_cget_max );
CK_DLL_CGET( Echo_cget_mix );

// Envelope
CK_DLL_CTOR( Envelope_ctor );
CK_DLL_DTOR( Envelope_dtor );
CK_DLL_TICK( Envelope_tick );
CK_DLL_PMSG( Envelope_pmsg );
CK_DLL_CTRL( Envelope_ctrl_rate );
CK_DLL_CTRL( Envelope_ctrl_target );
CK_DLL_CTRL( Envelope_cget_target );
CK_DLL_CTRL( Envelope_ctrl_time );
CK_DLL_CTRL( Envelope_ctrl_duration );
CK_DLL_CTRL( Envelope_ctrl_value );
CK_DLL_CTRL( Envelope_cget_value );
CK_DLL_CTRL( Envelope_ctrl_keyOn0 );
CK_DLL_CTRL( Envelope_ctrl_keyOn );
CK_DLL_CTRL( Envelope_ctrl_keyOff0 );
CK_DLL_CTRL( Envelope_ctrl_keyOff );
CK_DLL_CGET( Envelope_cget_rate );
CK_DLL_CGET( Envelope_cget_target );
CK_DLL_CGET( Envelope_cget_time );
CK_DLL_CGET( Envelope_cget_duration );
CK_DLL_CGET( Envelope_cget_value );

// FilterStk (originally Filter)
CK_DLL_CTOR( FilterStk_ctor );
CK_DLL_DTOR( FilterStk_dtor );
CK_DLL_TICK( FilterStk_tick );
CK_DLL_PMSG( FilterStk_pmsg );
CK_DLL_CTRL( FilterStk_ctrl_coefs );
CK_DLL_CGET( FilterStk_cget_coefs );

// OnePole
CK_DLL_CTOR( OnePole_ctor );
CK_DLL_DTOR( OnePole_dtor );
CK_DLL_TICK( OnePole_tick );
CK_DLL_PMSG( OnePole_pmsg );
CK_DLL_CTRL( OnePole_ctrl_a1 );
CK_DLL_CTRL( OnePole_ctrl_b0 );
CK_DLL_CTRL( OnePole_ctrl_pole );
CK_DLL_CGET( OnePole_cget_a1 );
CK_DLL_CGET( OnePole_cget_b0 );
CK_DLL_CGET( OnePole_cget_pole );

// TwoPole
CK_DLL_CTOR( TwoPole_ctor );
CK_DLL_DTOR( TwoPole_dtor );
CK_DLL_TICK( TwoPole_tick );
CK_DLL_PMSG( TwoPole_pmsg );
CK_DLL_CTRL( TwoPole_ctrl_a1 );
CK_DLL_CTRL( TwoPole_ctrl_a2 );
CK_DLL_CTRL( TwoPole_ctrl_b0 );
CK_DLL_CTRL( TwoPole_ctrl_freq );
CK_DLL_CTRL( TwoPole_ctrl_radius );
CK_DLL_CTRL( TwoPole_ctrl_norm );
CK_DLL_CGET( TwoPole_cget_a1 );
CK_DLL_CGET( TwoPole_cget_a2 );
CK_DLL_CGET( TwoPole_cget_b0 );
CK_DLL_CGET( TwoPole_cget_freq );
CK_DLL_CGET( TwoPole_cget_radius );
CK_DLL_CGET( TwoPole_cget_norm );


// OneZero
CK_DLL_CTOR( OneZero_ctor );
CK_DLL_DTOR( OneZero_dtor );
CK_DLL_TICK( OneZero_tick );
CK_DLL_PMSG( OneZero_pmsg );
CK_DLL_CTRL( OneZero_ctrl_zero );
CK_DLL_CTRL( OneZero_ctrl_b0 );
CK_DLL_CTRL( OneZero_ctrl_b1 );
CK_DLL_CGET( OneZero_cget_zero );
CK_DLL_CGET( OneZero_cget_b0 );
CK_DLL_CGET( OneZero_cget_b1 );

// TwoZero
CK_DLL_CTOR( TwoZero_ctor );
CK_DLL_DTOR( TwoZero_dtor );
CK_DLL_TICK( TwoZero_tick );
CK_DLL_PMSG( TwoZero_pmsg );
CK_DLL_CTRL( TwoZero_ctrl_b0 );
CK_DLL_CTRL( TwoZero_ctrl_b1 );
CK_DLL_CTRL( TwoZero_ctrl_b2 );
CK_DLL_CTRL( TwoZero_ctrl_freq );
CK_DLL_CTRL( TwoZero_ctrl_radius );
CK_DLL_CGET( TwoZero_cget_b0 );
CK_DLL_CGET( TwoZero_cget_b1 );
CK_DLL_CGET( TwoZero_cget_b2 );
CK_DLL_CGET( TwoZero_cget_freq );
CK_DLL_CGET( TwoZero_cget_radius );

// PoleZero
CK_DLL_CTOR( PoleZero_ctor );
CK_DLL_DTOR( PoleZero_dtor );
CK_DLL_TICK( PoleZero_tick );
CK_DLL_PMSG( PoleZero_pmsg );
CK_DLL_CTRL( PoleZero_ctrl_a1 );
CK_DLL_CTRL( PoleZero_ctrl_b0 );
CK_DLL_CTRL( PoleZero_ctrl_b1 );
CK_DLL_CTRL( PoleZero_ctrl_blockZero );
CK_DLL_CTRL( PoleZero_ctrl_allpass );
CK_DLL_CGET( PoleZero_cget_a1 );
CK_DLL_CGET( PoleZero_cget_b0 );
CK_DLL_CGET( PoleZero_cget_b1 );
CK_DLL_CGET( PoleZero_cget_blockZero );
CK_DLL_CGET( PoleZero_cget_allpass );

// Noise
CK_DLL_CTOR( Noise_ctor );
CK_DLL_DTOR( Noise_dtor );
CK_DLL_TICK( Noise_tick );
CK_DLL_PMSG( Noise_pmsg );
CK_DLL_CTRL( Noise_ctrl_seed );
CK_DLL_CGET( Noise_cget_seed );

// SubNoise
CK_DLL_CTOR( SubNoise_ctor );
CK_DLL_DTOR( SubNoise_dtor );
CK_DLL_TICK( SubNoise_tick );
CK_DLL_PMSG( SubNoise_pmsg );
CK_DLL_CTRL( SubNoise_ctrl_rate );
CK_DLL_CGET( SubNoise_cget_rate );

// BLT
CK_DLL_CTOR( BLT_ctor );
CK_DLL_DTOR( BLT_dtor );
CK_DLL_TICK( BLT_tick );
CK_DLL_PMSG( BLT_pmsg );
CK_DLL_CTRL( BLT_ctrl_phase );
CK_DLL_CGET( BLT_cget_phase );
CK_DLL_CTRL( BLT_ctrl_freq );
CK_DLL_CGET( BLT_cget_freq );
CK_DLL_CTRL( BLT_ctrl_harmonics );
CK_DLL_CGET( BLT_cget_harmonics );

// Blit
CK_DLL_CTOR( Blit_ctor );
CK_DLL_DTOR( Blit_dtor );
CK_DLL_TICK( Blit_tick );
CK_DLL_PMSG( Blit_pmsg );

// BlitSaw
CK_DLL_CTOR( BlitSaw_ctor );
CK_DLL_DTOR( BlitSaw_dtor );
CK_DLL_TICK( BlitSaw_tick );
CK_DLL_PMSG( BlitSaw_pmsg );

// BlitSquare
CK_DLL_CTOR( BlitSquare_ctor );
CK_DLL_DTOR( BlitSquare_dtor );
CK_DLL_TICK( BlitSquare_tick );
CK_DLL_PMSG( BlitSquare_pmsg );

// JCRev
CK_DLL_CTOR( JCRev_ctor );
CK_DLL_DTOR( JCRev_dtor );
CK_DLL_TICK( JCRev_tick );
CK_DLL_PMSG( JCRev_pmsg );
CK_DLL_CTRL( JCRev_ctrl_mix );
CK_DLL_CGET( JCRev_cget_mix );

// NRev
CK_DLL_CTOR( NRev_ctor );
CK_DLL_DTOR( NRev_dtor );
CK_DLL_TICK( NRev_tick );
CK_DLL_PMSG( NRev_pmsg );
CK_DLL_CTRL( NRev_ctrl_mix );
CK_DLL_CGET( NRev_cget_mix );

// PRCRev
CK_DLL_CTOR( PRCRev_ctor );
CK_DLL_DTOR( PRCRev_dtor );
CK_DLL_TICK( PRCRev_tick );
CK_DLL_PMSG( PRCRev_pmsg );
CK_DLL_CTRL( PRCRev_ctrl_mix );
CK_DLL_CGET( PRCRev_cget_mix );

// WaveLoop
CK_DLL_CTOR( WaveLoop_ctor );
CK_DLL_DTOR( WaveLoop_dtor );
CK_DLL_TICK( WaveLoop_tick );
CK_DLL_PMSG( WaveLoop_pmsg );
CK_DLL_CTRL( WaveLoop_ctrl_freq );
CK_DLL_CTRL( WaveLoop_ctrl_rate );
CK_DLL_CTRL( WaveLoop_ctrl_phase );
CK_DLL_CTRL( WaveLoop_ctrl_phaseOffset );
CK_DLL_CTRL( WaveLoop_ctrl_path );
CK_DLL_CGET( WaveLoop_cget_freq );
CK_DLL_CGET( WaveLoop_cget_rate );
CK_DLL_CGET( WaveLoop_cget_phase );
CK_DLL_CGET( WaveLoop_cget_phaseOffset );
CK_DLL_CGET( WaveLoop_cget_path );

// WvIn
CK_DLL_CTOR( WvIn_ctor );
CK_DLL_DTOR( WvIn_dtor );
CK_DLL_TICK( WvIn_tick );
CK_DLL_PMSG( WvIn_pmsg );
CK_DLL_CTRL( WvIn_ctrl_rate );
CK_DLL_CTRL( WvIn_ctrl_path );
CK_DLL_CGET( WvIn_cget_rate );
CK_DLL_CGET( WvIn_cget_path );

// WvOut
CK_DLL_CTOR( WvOut_ctor );
CK_DLL_DTOR( WvOut_dtor );
CK_DLL_TICK( WvOut_tick );
CK_DLL_TICKF( WvOut2_tickf );
CK_DLL_PMSG( WvOut_pmsg );
CK_DLL_CTRL( WvOut_ctrl_filename );
CK_DLL_CTRL( WvOut_ctrl_matFilename );
CK_DLL_CTRL( WvOut2_ctrl_matFilename );
CK_DLL_CTRL( WvOut_ctrl_sndFilename );
CK_DLL_CTRL( WvOut2_ctrl_sndFilename );
CK_DLL_CTRL( WvOut_ctrl_wavFilename );
CK_DLL_CTRL( WvOut2_ctrl_wavFilename );
CK_DLL_CTRL( WvOut_ctrl_rawFilename );
CK_DLL_CTRL( WvOut2_ctrl_rawFilename );
CK_DLL_CTRL( WvOut_ctrl_aifFilename );
CK_DLL_CTRL( WvOut2_ctrl_aifFilename );
CK_DLL_CTRL( WvOut_ctrl_closeFile );
CK_DLL_CTRL( WvOut_ctrl_record );
CK_DLL_CTRL( WvOut_ctrl_autoPrefix );
CK_DLL_CGET( WvOut_cget_filename );
CK_DLL_CGET( WvOut_cget_record );
CK_DLL_CGET( WvOut_cget_autoPrefix );
CK_DLL_CTRL( WvOut_ctrl_fileGain );
CK_DLL_CGET( WvOut_cget_fileGain );


// FM
CK_DLL_CTOR( FM_ctor );
CK_DLL_DTOR( FM_dtor );
CK_DLL_TICK( FM_tick );
CK_DLL_PMSG( FM_pmsg );
CK_DLL_CTRL( FM_ctrl_freq );
CK_DLL_CTRL( FM_ctrl_noteOn );
CK_DLL_CTRL( FM_ctrl_noteOff );
CK_DLL_CTRL( FM_ctrl_modDepth );
CK_DLL_CTRL( FM_ctrl_modSpeed );
CK_DLL_CTRL( FM_ctrl_control1 );
CK_DLL_CTRL( FM_ctrl_control2 );
CK_DLL_CTRL( FM_ctrl_controlChange );
CK_DLL_CTRL( FM_ctrl_afterTouch );
CK_DLL_CGET( FM_cget_freq );
CK_DLL_CGET( FM_cget_modDepth );
CK_DLL_CGET( FM_cget_modSpeed );
CK_DLL_CGET( FM_cget_control1 );
CK_DLL_CGET( FM_cget_control2 );
CK_DLL_CGET( FM_cget_afterTouch );

// FormSwep
CK_DLL_CTOR( FormSwep_ctor );
CK_DLL_DTOR( FormSwep_dtor );
CK_DLL_TICK( FormSwep_tick );
CK_DLL_PMSG( FormSwep_pmsg );
CK_DLL_CTRL( FormSwep_ctrl_frequency );
CK_DLL_CGET( FormSwep_cget_frequency );
CK_DLL_CTRL( FormSwep_ctrl_radius );
CK_DLL_CGET( FormSwep_cget_radius );
CK_DLL_CTRL( FormSwep_ctrl_gain );
CK_DLL_CGET( FormSwep_cget_gain );
CK_DLL_CTRL( FormSwep_ctrl_sweepRate );
CK_DLL_CTRL( FormSwep_ctrl_sweepTime );
CK_DLL_CGET( FormSwep_cget_sweepRate );
CK_DLL_CGET( FormSwep_cget_sweepTime );

// Modulate
CK_DLL_CTOR( Modulate_ctor );
CK_DLL_DTOR( Modulate_dtor );
CK_DLL_TICK( Modulate_tick );
CK_DLL_PMSG( Modulate_pmsg );
CK_DLL_CTRL( Modulate_ctrl_vibratoGain );
CK_DLL_CTRL( Modulate_ctrl_vibratoRate );
CK_DLL_CTRL( Modulate_ctrl_randomGain );
CK_DLL_CGET( Modulate_cget_vibratoGain );
CK_DLL_CGET( Modulate_cget_vibratoRate );
CK_DLL_CGET( Modulate_cget_randomGain );

// PitShift
CK_DLL_CTOR( PitShift_ctor );
CK_DLL_DTOR( PitShift_dtor );
CK_DLL_TICK( PitShift_tick );
CK_DLL_PMSG( PitShift_pmsg );
CK_DLL_CTRL( PitShift_ctrl_shift );
CK_DLL_CTRL( PitShift_ctrl_effectMix );
CK_DLL_CGET( PitShift_cget_shift );
CK_DLL_CGET( PitShift_cget_effectMix );

// Sampler
CK_DLL_CTOR( Sampler_ctor );
CK_DLL_DTOR( Sampler_dtor );
CK_DLL_TICK( Sampler_tick );
CK_DLL_PMSG( Sampler_pmsg );

// Drummer
CK_DLL_CTOR( Drummer_ctor );
CK_DLL_DTOR( Drummer_dtor );
CK_DLL_TICK( Drummer_tick );
CK_DLL_PMSG( Drummer_pmsg );

// Instrmnt
CK_DLL_CTOR( Instrmnt_ctor );
CK_DLL_DTOR( Instrmnt_dtor );
CK_DLL_TICK( Instrmnt_tick );
CK_DLL_PMSG( Instrmnt_pmsg );
CK_DLL_CTRL( Instrmnt_ctrl_freq );
CK_DLL_CGET( Instrmnt_cget_freq );
CK_DLL_CTRL( Instrmnt_ctrl_noteOn );
CK_DLL_CTRL( Instrmnt_ctrl_noteOff );
CK_DLL_CTRL( Instrmnt_ctrl_controlChange );

// BandedWG
CK_DLL_CTOR( BandedWG_ctor );
CK_DLL_DTOR( BandedWG_dtor );
CK_DLL_TICK( BandedWG_tick );
CK_DLL_PMSG( BandedWG_pmsg );
CK_DLL_CTRL( BandedWG_ctrl_bowPressure );
CK_DLL_CGET( BandedWG_cget_bowPressure );
CK_DLL_CTRL( BandedWG_ctrl_bowMotion );
CK_DLL_CGET( BandedWG_cget_bowMotion );
CK_DLL_CTRL( BandedWG_ctrl_strikePosition );
CK_DLL_CGET( BandedWG_cget_strikePosition );
CK_DLL_CTRL( BandedWG_ctrl_vibratoFreq );
CK_DLL_CGET( BandedWG_cget_vibratoFreq );
CK_DLL_CTRL( BandedWG_ctrl_modesGain );
CK_DLL_CGET( BandedWG_cget_modesGain );
CK_DLL_CTRL( BandedWG_ctrl_bowRate );
CK_DLL_CGET( BandedWG_cget_bowRate );
CK_DLL_CTRL( BandedWG_ctrl_preset );
CK_DLL_CGET( BandedWG_cget_preset );
CK_DLL_CTRL( BandedWG_ctrl_startBowing );
CK_DLL_CTRL( BandedWG_ctrl_stopBowing );
CK_DLL_CTRL( BandedWG_ctrl_pluck );
CK_DLL_CTRL( BandedWG_ctrl_noteOn );
CK_DLL_CTRL( BandedWG_ctrl_noteOff );
CK_DLL_CTRL( BandedWG_ctrl_freq );
CK_DLL_CGET( BandedWG_cget_freq );
CK_DLL_CTRL( BandedWG_ctrl_controlChange );
/*
CK_DLL_CTRL( BandedWG_ctrl_aftertouch );
CK_DLL_CTRL( BandedWG_ctrl_bowTarget );
CK_DLL_CTRL( BandedWG_ctrl_sustain );
CK_DLL_CTRL( BandedWG_ctrl_trackVelocity );
CK_DLL_CTRL( BandedWG_ctrl_portamento );
*/

// BeeThree
CK_DLL_CTOR( BeeThree_ctor );
CK_DLL_DTOR( BeeThree_dtor );
CK_DLL_TICK( BeeThree_tick );
CK_DLL_PMSG( BeeThree_pmsg );
CK_DLL_CTRL( BeeThree_ctrl_noteOn );

// BlowBotl
CK_DLL_CTOR( BlowBotl_ctor );
CK_DLL_DTOR( BlowBotl_dtor );
CK_DLL_TICK( BlowBotl_tick );
CK_DLL_PMSG( BlowBotl_pmsg );
CK_DLL_CTRL( BlowBotl_ctrl_freq );
CK_DLL_CGET( BlowBotl_cget_freq );
CK_DLL_CTRL( BlowBotl_ctrl_noiseGain );
CK_DLL_CGET( BlowBotl_cget_noiseGain );
CK_DLL_CTRL( BlowBotl_ctrl_vibratoFreq );
CK_DLL_CGET( BlowBotl_cget_vibratoFreq );
CK_DLL_CTRL( BlowBotl_ctrl_vibratoGain );
CK_DLL_CGET( BlowBotl_cget_vibratoGain );
CK_DLL_CTRL( BlowBotl_ctrl_volume );
CK_DLL_CGET( BlowBotl_cget_volume );
CK_DLL_CTRL( BlowBotl_ctrl_noteOn );
CK_DLL_CTRL( BlowBotl_ctrl_noteOff );
CK_DLL_CTRL( BlowBotl_ctrl_controlChange );
CK_DLL_CTRL( BlowBotl_ctrl_startBlowing );
CK_DLL_CTRL( BlowBotl_ctrl_stopBlowing );
CK_DLL_CTRL( BlowBotl_ctrl_rate );
CK_DLL_CGET( BlowBotl_cget_rate );

// BlowHole
CK_DLL_CTOR( BlowHole_ctor );
CK_DLL_DTOR( BlowHole_dtor );
CK_DLL_TICK( BlowHole_tick );
CK_DLL_PMSG( BlowHole_pmsg );
CK_DLL_CTRL( BlowHole_ctrl_freq );
CK_DLL_CGET( BlowHole_cget_freq );
CK_DLL_CTRL( BlowHole_ctrl_reed );
CK_DLL_CGET( BlowHole_cget_reed );
CK_DLL_CTRL( BlowHole_ctrl_noiseGain );
CK_DLL_CGET( BlowHole_cget_noiseGain );
CK_DLL_CTRL( BlowHole_ctrl_tonehole );
CK_DLL_CGET( BlowHole_cget_tonehole );
CK_DLL_CTRL( BlowHole_ctrl_vent );
CK_DLL_CGET( BlowHole_cget_vent );
CK_DLL_CTRL( BlowHole_ctrl_pressure );
CK_DLL_CGET( BlowHole_cget_pressure );
CK_DLL_CTRL( BlowHole_ctrl_noteOn );
CK_DLL_CTRL( BlowHole_ctrl_noteOff );
CK_DLL_CTRL( BlowHole_ctrl_controlChange );
CK_DLL_CTRL( BlowHole_ctrl_startBlowing );
CK_DLL_CTRL( BlowHole_ctrl_stopBlowing );
CK_DLL_CTRL( BlowHole_ctrl_rate );
CK_DLL_CGET( BlowHole_cget_rate );

// Bowed
CK_DLL_CTOR( Bowed_ctor );
CK_DLL_DTOR( Bowed_dtor );
CK_DLL_TICK( Bowed_tick );
CK_DLL_PMSG( Bowed_pmsg );
CK_DLL_CTRL( Bowed_ctrl_freq );
CK_DLL_CGET( Bowed_cget_freq );
CK_DLL_CTRL( Bowed_ctrl_bowPressure );
CK_DLL_CGET( Bowed_cget_bowPressure );
CK_DLL_CTRL( Bowed_ctrl_bowPos );
CK_DLL_CGET( Bowed_cget_bowPos );
CK_DLL_CTRL( Bowed_ctrl_vibratoFreq );
CK_DLL_CGET( Bowed_cget_vibratoFreq );
CK_DLL_CTRL( Bowed_ctrl_vibratoGain );
CK_DLL_CGET( Bowed_cget_vibratoGain );
CK_DLL_CTRL( Bowed_ctrl_volume );
CK_DLL_CGET( Bowed_cget_volume );
CK_DLL_CTRL( Bowed_ctrl_noteOn );
CK_DLL_CTRL( Bowed_ctrl_noteOff );
CK_DLL_CTRL( Bowed_ctrl_controlChange );
CK_DLL_CTRL( Bowed_ctrl_startBowing );
CK_DLL_CTRL( Bowed_ctrl_stopBowing );
CK_DLL_CTRL( Bowed_ctrl_rate );
CK_DLL_CGET( Bowed_cget_rate );

// BowTabl
CK_DLL_CTOR( BowTabl_ctor );
CK_DLL_DTOR( BowTabl_dtor );
CK_DLL_TICK( BowTabl_tick );
CK_DLL_PMSG( BowTabl_pmsg );
CK_DLL_CTRL( BowTabl_ctrl_offset );
CK_DLL_CTRL( BowTabl_ctrl_slope );

// Brass
CK_DLL_CTOR( Brass_ctor );
CK_DLL_DTOR( Brass_dtor );
CK_DLL_TICK( Brass_tick );
CK_DLL_PMSG( Brass_pmsg );
CK_DLL_CTRL( Brass_ctrl_freq );
CK_DLL_CGET( Brass_cget_freq );
CK_DLL_CTRL( Brass_ctrl_lip );
CK_DLL_CGET( Brass_cget_lip );
CK_DLL_CTRL( Brass_ctrl_slide );
CK_DLL_CGET( Brass_cget_slide );
CK_DLL_CTRL( Brass_ctrl_vibratoFreq );
CK_DLL_CGET( Brass_cget_vibratoFreq );
CK_DLL_CTRL( Brass_ctrl_vibratoGain );
CK_DLL_CGET( Brass_cget_vibratoGain );
CK_DLL_CTRL( Brass_ctrl_volume );
CK_DLL_CGET( Brass_cget_volume );
CK_DLL_CTRL( Brass_ctrl_clear );
CK_DLL_CTRL( Brass_ctrl_noteOn );
CK_DLL_CTRL( Brass_ctrl_noteOff );
CK_DLL_CTRL( Brass_ctrl_controlChange );
CK_DLL_CTRL( Brass_ctrl_startBlowing );
CK_DLL_CTRL( Brass_ctrl_stopBlowing );
CK_DLL_CTRL( Brass_ctrl_rate );
CK_DLL_CGET( Brass_cget_rate );

// Clarinet
CK_DLL_CTOR( Clarinet_ctor );
CK_DLL_DTOR( Clarinet_dtor );
CK_DLL_TICK( Clarinet_tick );
CK_DLL_PMSG( Clarinet_pmsg );
CK_DLL_CTRL( Clarinet_ctrl_freq );
CK_DLL_CGET( Clarinet_cget_freq );
CK_DLL_CTRL( Clarinet_ctrl_reed );
CK_DLL_CGET( Clarinet_cget_reed );
CK_DLL_CTRL( Clarinet_ctrl_noiseGain );
CK_DLL_CGET( Clarinet_cget_noiseGain );
CK_DLL_CTRL( Clarinet_ctrl_vibratoFreq );
CK_DLL_CGET( Clarinet_cget_vibratoFreq );
CK_DLL_CTRL( Clarinet_ctrl_vibratoGain );
CK_DLL_CGET( Clarinet_cget_vibratoGain );
CK_DLL_CTRL( Clarinet_ctrl_pressure );
CK_DLL_CGET( Clarinet_cget_pressure );
CK_DLL_CTRL( Clarinet_ctrl_clear );
CK_DLL_CTRL( Clarinet_ctrl_noteOn );
CK_DLL_CTRL( Clarinet_ctrl_noteOff );
CK_DLL_CTRL( Clarinet_ctrl_startBlowing );
CK_DLL_CTRL( Clarinet_ctrl_stopBlowing );
CK_DLL_CTRL( Clarinet_ctrl_rate );
CK_DLL_CGET( Clarinet_cget_rate );
CK_DLL_CTRL( Clarinet_ctrl_controlChange );

// Flute
CK_DLL_CTOR( Flute_ctor );
CK_DLL_DTOR( Flute_dtor );
CK_DLL_TICK( Flute_tick );
CK_DLL_PMSG( Flute_pmsg );
CK_DLL_CTRL( Flute_ctrl_freq );
CK_DLL_CGET( Flute_cget_freq );
CK_DLL_CTRL( Flute_ctrl_jetDelay );
CK_DLL_CGET( Flute_cget_jetDelay );
CK_DLL_CTRL( Flute_ctrl_jetReflection );
CK_DLL_CGET( Flute_cget_jetReflection );
CK_DLL_CTRL( Flute_ctrl_endReflection );
CK_DLL_CGET( Flute_cget_endReflection );
CK_DLL_CTRL( Flute_ctrl_noiseGain );
CK_DLL_CGET( Flute_cget_noiseGain );
CK_DLL_CTRL( Flute_ctrl_vibratoFreq );
CK_DLL_CGET( Flute_cget_vibratoFreq );
CK_DLL_CTRL( Flute_ctrl_vibratoGain );
CK_DLL_CGET( Flute_cget_vibratoGain );
CK_DLL_CTRL( Flute_ctrl_pressure );
CK_DLL_CGET( Flute_cget_pressure );
CK_DLL_CTRL( Flute_ctrl_clear );
CK_DLL_CTRL( Flute_ctrl_noteOn );
CK_DLL_CTRL( Flute_ctrl_noteOff );
CK_DLL_CTRL( Flute_ctrl_controlChange );
CK_DLL_CTRL( Flute_ctrl_startBlowing );
CK_DLL_CTRL( Flute_ctrl_stopBlowing );
CK_DLL_CTRL( Flute_ctrl_rate );
CK_DLL_CGET( Flute_cget_rate );

// FMVoices
CK_DLL_CTOR( FMVoices_ctor );
CK_DLL_DTOR( FMVoices_dtor );
CK_DLL_TICK( FMVoices_tick );
CK_DLL_PMSG( FMVoices_pmsg );
CK_DLL_CTRL( FMVoices_ctrl_vowel );
CK_DLL_CTRL( FMVoices_cget_vowel );
CK_DLL_CTRL( FMVoices_ctrl_spectralTilt );
CK_DLL_CTRL( FMVoices_cget_spectralTilt );
CK_DLL_CTRL( FMVoices_ctrl_adsrTarget );
CK_DLL_CTRL( FMVoices_cget_adsrTarget );

// HevyMetl
CK_DLL_CTOR( HevyMetl_ctor );
CK_DLL_DTOR( HevyMetl_dtor );
CK_DLL_TICK( HevyMetl_tick );
CK_DLL_PMSG( HevyMetl_pmsg );

// JetTabl
CK_DLL_CTOR( JetTabl_ctor );
CK_DLL_DTOR( JetTabl_dtor );
CK_DLL_TICK( JetTabl_tick );
CK_DLL_PMSG( JetTabl_pmsg );

// Mandolin
CK_DLL_CTOR( Mandolin_ctor );
CK_DLL_DTOR( Mandolin_dtor );
CK_DLL_TICK( Mandolin_tick );
CK_DLL_PMSG( Mandolin_pmsg );
CK_DLL_CTRL( Mandolin_ctrl_freq );
CK_DLL_CGET( Mandolin_cget_freq );
CK_DLL_CTRL( Mandolin_ctrl_pluckPos );
CK_DLL_CGET( Mandolin_cget_pluckPos );
CK_DLL_CTRL( Mandolin_ctrl_bodySize );
CK_DLL_CGET( Mandolin_cget_bodySize );
CK_DLL_CTRL( Mandolin_ctrl_stringDamping );
CK_DLL_CGET( Mandolin_cget_stringDamping );
CK_DLL_CTRL( Mandolin_ctrl_stringDetune );
CK_DLL_CGET( Mandolin_cget_stringDetune );
CK_DLL_CTRL( Mandolin_ctrl_controlChange );
CK_DLL_CTRL( Mandolin_ctrl_afterTouch );
CK_DLL_CTRL( Mandolin_ctrl_pluck );
CK_DLL_CTRL( Mandolin_ctrl_noteOn );
CK_DLL_CTRL( Mandolin_ctrl_noteOff );
CK_DLL_CTRL( Mandolin_ctrl_bodyIR );
CK_DLL_CGET( Mandolin_cget_bodyIR );

// Modal
CK_DLL_CTOR( Modal_ctor );
CK_DLL_DTOR( Modal_dtor );
CK_DLL_TICK( Modal_tick );
CK_DLL_PMSG( Modal_pmsg );
CK_DLL_CTRL( Modal_ctrl_freq );
CK_DLL_CTRL( Modal_ctrl_masterGain );
CK_DLL_CTRL( Modal_ctrl_directGain );
CK_DLL_CTRL( Modal_ctrl_mode );
CK_DLL_CTRL( Modal_ctrl_modeRatio );
CK_DLL_CTRL( Modal_ctrl_modeRadius );
CK_DLL_CTRL( Modal_ctrl_modeGain );
CK_DLL_CTRL( Modal_ctrl_strike );
CK_DLL_CTRL( Modal_ctrl_damp );
CK_DLL_CTRL( Modal_ctrl_noteOn );
CK_DLL_CTRL( Modal_ctrl_noteOff );

// ModalBar
CK_DLL_CTOR( ModalBar_ctor );
CK_DLL_DTOR( ModalBar_dtor );
CK_DLL_TICK( ModalBar_tick );
CK_DLL_PMSG( ModalBar_pmsg );
CK_DLL_CTRL( ModalBar_ctrl_strike );
CK_DLL_CTRL( ModalBar_ctrl_damp );
CK_DLL_CTRL( ModalBar_ctrl_clear );
CK_DLL_CTRL( ModalBar_ctrl_noteOn );
CK_DLL_CTRL( ModalBar_ctrl_noteOff );
CK_DLL_CTRL( ModalBar_ctrl_stickHardness );
CK_DLL_CGET( ModalBar_cget_stickHardness );
CK_DLL_CTRL( ModalBar_ctrl_strikePosition );
CK_DLL_CGET( ModalBar_cget_strikePosition );
CK_DLL_CTRL( ModalBar_ctrl_vibratoGain );
CK_DLL_CGET( ModalBar_cget_vibratoGain );
CK_DLL_CTRL( ModalBar_ctrl_vibratoFreq );
CK_DLL_CGET( ModalBar_cget_vibratoFreq );
CK_DLL_CTRL( ModalBar_ctrl_preset );
CK_DLL_CGET( ModalBar_cget_preset );
CK_DLL_CTRL( ModalBar_ctrl_freq );
CK_DLL_CGET( ModalBar_cget_freq );
CK_DLL_CTRL( ModalBar_ctrl_directGain );
CK_DLL_CGET( ModalBar_cget_directGain );
CK_DLL_CTRL( ModalBar_ctrl_masterGain );
CK_DLL_CGET( ModalBar_cget_masterGain );
CK_DLL_CTRL( ModalBar_ctrl_mode );
CK_DLL_CGET( ModalBar_cget_mode );
CK_DLL_CTRL( ModalBar_ctrl_modeRatio );
CK_DLL_CGET( ModalBar_cget_modeRatio );
CK_DLL_CTRL( ModalBar_ctrl_modeRadius );
CK_DLL_CGET( ModalBar_cget_modeRadius );
CK_DLL_CTRL( ModalBar_ctrl_modeGain );
CK_DLL_CGET( ModalBar_cget_modeGain );
CK_DLL_CTRL( ModalBar_ctrl_volume );
CK_DLL_CGET( ModalBar_cget_volume );
CK_DLL_CTRL( ModalBar_ctrl_controlChange );


// Moog
CK_DLL_CTOR( Moog_ctor );
CK_DLL_DTOR( Moog_dtor );
CK_DLL_TICK( Moog_tick );
CK_DLL_PMSG( Moog_pmsg );
CK_DLL_CTRL( Moog_ctrl_freq );
CK_DLL_CTRL( Moog_ctrl_noteOn );
CK_DLL_CTRL( Moog_ctrl_modSpeed );
CK_DLL_CTRL( Moog_ctrl_modDepth );
CK_DLL_CTRL( Moog_ctrl_filterQ );
CK_DLL_CTRL( Moog_ctrl_filterSweepRate );
CK_DLL_CTRL( Moog_ctrl_afterTouch );
CK_DLL_CTRL( Moog_ctrl_vibratoFreq );
CK_DLL_CTRL( Moog_ctrl_vibratoGain );
CK_DLL_CTRL( Moog_ctrl_volume );

CK_DLL_CGET( Moog_cget_freq );
CK_DLL_CGET( Moog_cget_modSpeed );
CK_DLL_CGET( Moog_cget_modDepth );
CK_DLL_CGET( Moog_cget_filterQ );
CK_DLL_CGET( Moog_cget_filterSweepRate );
CK_DLL_CGET( Moog_cget_vibratoFreq );
CK_DLL_CGET( Moog_cget_vibratoGain );
CK_DLL_CGET( Moog_cget_volume );

CK_DLL_CTRL( Moog_ctrl_controlChange );

// PercFlut
CK_DLL_CTOR( PercFlut_ctor );
CK_DLL_DTOR( PercFlut_dtor );
CK_DLL_TICK( PercFlut_tick );
CK_DLL_PMSG( PercFlut_pmsg );
CK_DLL_CTRL( PercFlut_ctrl_noteOn );
CK_DLL_CTRL( PercFlut_ctrl_freq );
CK_DLL_CGET( PercFlut_cget_freq );

// Plucked
CK_DLL_CTOR( Plucked_ctor );
CK_DLL_DTOR( Plucked_dtor );
CK_DLL_TICK( Plucked_tick );
CK_DLL_PMSG( Plucked_pmsg );
CK_DLL_CTRL( Plucked_ctrl_freq );
CK_DLL_CTRL( Plucked_ctrl_pluck );
CK_DLL_CTRL( Plucked_ctrl_noteOn );
CK_DLL_CTRL( Plucked_ctrl_noteOff );

// PluckTwo
CK_DLL_CTOR( PluckTwo_ctor );
CK_DLL_DTOR( PluckTwo_dtor );
CK_DLL_TICK( PluckTwo_tick );
CK_DLL_PMSG( PluckTwo_pmsg );
CK_DLL_CTRL( PluckTwo_ctrl_detune );
CK_DLL_CTRL( PluckTwo_ctrl_freq );
CK_DLL_CTRL( PluckTwo_ctrl_pluckPosition );
CK_DLL_CTRL( PluckTwo_ctrl_baseLoopGain );
CK_DLL_CTRL( PluckTwo_ctrl_pluck );
CK_DLL_CTRL( PluckTwo_ctrl_noteOn );
CK_DLL_CTRL( PluckTwo_ctrl_noteOff );

// ReedTabl
CK_DLL_CTOR( ReedTabl_ctor );
CK_DLL_DTOR( ReedTabl_dtor );
CK_DLL_TICK( ReedTabl_tick );
CK_DLL_PMSG( ReedTabl_pmsg );
CK_DLL_CTRL( ReedTabl_ctrl_offset );
CK_DLL_CTRL( ReedTabl_ctrl_slope );

// Resonate
CK_DLL_CTOR( Resonate_ctor );
CK_DLL_DTOR( Resonate_dtor );
CK_DLL_TICK( Resonate_tick );
CK_DLL_PMSG( Resonate_pmsg );

// Rhodey
CK_DLL_CTOR( Rhodey_ctor );
CK_DLL_DTOR( Rhodey_dtor );
CK_DLL_TICK( Rhodey_tick );
CK_DLL_PMSG( Rhodey_pmsg );
CK_DLL_CTRL( Rhodey_ctrl_freq );
CK_DLL_CTRL( Rhodey_ctrl_noteOn );
CK_DLL_CTRL( Rhodey_ctrl_noteOff );
CK_DLL_CTRL( Rhodey_cget_freq );

// Saxofony
CK_DLL_CTOR( Saxofony_ctor );
CK_DLL_DTOR( Saxofony_dtor );
CK_DLL_TICK( Saxofony_tick );
CK_DLL_PMSG( Saxofony_pmsg );
CK_DLL_CTRL( Saxofony_ctrl_freq );
CK_DLL_CGET( Saxofony_cget_freq );
CK_DLL_CTRL( Saxofony_ctrl_reed );
CK_DLL_CGET( Saxofony_cget_reed );
CK_DLL_CTRL( Saxofony_ctrl_aperture );
CK_DLL_CTRL( Saxofony_cget_aperture );
CK_DLL_CTRL( Saxofony_ctrl_noiseGain );
CK_DLL_CGET( Saxofony_cget_noiseGain );
CK_DLL_CTRL( Saxofony_ctrl_vibratoGain );
CK_DLL_CGET( Saxofony_cget_vibratoGain );
CK_DLL_CTRL( Saxofony_ctrl_vibratoFreq );
CK_DLL_CTRL( Saxofony_cget_vibratoFreq );
CK_DLL_CTRL( Saxofony_ctrl_blowPosition );
CK_DLL_CGET( Saxofony_cget_blowPosition );
CK_DLL_CTRL( Saxofony_ctrl_pressure );
CK_DLL_CGET( Saxofony_cget_pressure );
CK_DLL_CTRL( Saxofony_ctrl_controlChange );
CK_DLL_CTRL( Saxofony_ctrl_startBlowing );
CK_DLL_CTRL( Saxofony_ctrl_stopBlowing );
CK_DLL_CTRL( Saxofony_ctrl_clear );
CK_DLL_CTRL( Saxofony_ctrl_rate );
CK_DLL_CGET( Saxofony_cget_rate );
CK_DLL_CTRL( Saxofony_ctrl_noteOn );
CK_DLL_CTRL( Saxofony_ctrl_noteOff );

// Shakers
CK_DLL_CTOR( Shakers_ctor );
CK_DLL_DTOR( Shakers_dtor );
CK_DLL_TICK( Shakers_tick );
CK_DLL_PMSG( Shakers_pmsg );
CK_DLL_CTRL( Shakers_ctrl_energy );
CK_DLL_CGET( Shakers_cget_energy );
CK_DLL_CTRL( Shakers_ctrl_decay );
CK_DLL_CGET( Shakers_cget_decay );
CK_DLL_CTRL( Shakers_ctrl_objects );
CK_DLL_CGET( Shakers_cget_objects );
CK_DLL_CTRL( Shakers_ctrl_which );
CK_DLL_CGET( Shakers_cget_which );
CK_DLL_CTRL( Shakers_ctrl_noteOn );
CK_DLL_CTRL( Shakers_ctrl_noteOff );
CK_DLL_CTRL( Shakers_ctrl_freq );
CK_DLL_CGET( Shakers_cget_freq );
CK_DLL_CTRL( Shakers_ctrl_controlChange );

// Simple
CK_DLL_CTOR( Simple_ctor );
CK_DLL_DTOR( Simple_dtor );
CK_DLL_PMSG( Simple_pmsg );
CK_DLL_CTRL( Simple_ctrl_freq );

// SingWave
CK_DLL_CTOR( SingWave_ctor );
CK_DLL_DTOR( SingWave_dtor );
CK_DLL_TICK( SingWave_tick );
CK_DLL_PMSG( SingWave_pmsg );
CK_DLL_CTRL( SingWave_ctrl_wavFilename );
CK_DLL_CTRL( SingWave_ctrl_rawFilename );
CK_DLL_CTRL( SingWave_ctrl_norm );
CK_DLL_CTRL( SingWave_ctrl_sweepRate );
CK_DLL_CTRL( SingWave_ctrl_freq );
CK_DLL_CTRL( SingWave_ctrl_vibratoRate );
CK_DLL_CTRL( SingWave_ctrl_vibratoGain );
CK_DLL_CTRL( SingWave_ctrl_randomGain );
CK_DLL_CTRL( SingWave_ctrl_gainTarget );
CK_DLL_CTRL( SingWave_ctrl_gainRate );

// Sitar
CK_DLL_CTOR( Sitar_ctor );
CK_DLL_DTOR( Sitar_dtor );
CK_DLL_TICK( Sitar_tick );
CK_DLL_PMSG( Sitar_pmsg );
CK_DLL_CTRL( Sitar_ctrl_noteOn );
CK_DLL_CTRL( Sitar_ctrl_noteOff );
CK_DLL_CTRL( Sitar_ctrl_pluck );
CK_DLL_CTRL( Sitar_ctrl_clear );
CK_DLL_CTRL( Sitar_ctrl_freq );
CK_DLL_CTRL( Sitar_cget_freq );

// Sphere
CK_DLL_CTOR( Sphere_ctor );
CK_DLL_DTOR( Sphere_dtor );
CK_DLL_TICK( Sphere_tick );
CK_DLL_PMSG( Sphere_pmsg );
CK_DLL_CTRL( Sphere_ctrl_radius );
CK_DLL_CTRL( Sphere_ctrl_mass );

// StifKarp
CK_DLL_CTOR( StifKarp_ctor );
CK_DLL_DTOR( StifKarp_dtor );
CK_DLL_TICK( StifKarp_tick );
CK_DLL_PMSG( StifKarp_pmsg );
CK_DLL_CTRL( StifKarp_ctrl_pluck );
CK_DLL_CTRL( StifKarp_ctrl_clear );
CK_DLL_CTRL( StifKarp_ctrl_noteOn );
CK_DLL_CTRL( StifKarp_ctrl_noteOff );

CK_DLL_CTRL( StifKarp_ctrl_freq );
CK_DLL_CGET( StifKarp_cget_freq );
CK_DLL_CTRL( StifKarp_ctrl_pickupPosition );
CK_DLL_CGET( StifKarp_cget_pickupPosition );
CK_DLL_CTRL( StifKarp_ctrl_stretch );
CK_DLL_CGET( StifKarp_cget_stretch );
CK_DLL_CTRL( StifKarp_ctrl_sustain );
CK_DLL_CGET( StifKarp_cget_sustain );
CK_DLL_CTRL( StifKarp_ctrl_baseLoopGain );
CK_DLL_CGET( StifKarp_cget_baseLoopGain );

// TubeBell
CK_DLL_CTOR( TubeBell_ctor );
CK_DLL_DTOR( TubeBell_dtor );
CK_DLL_TICK( TubeBell_tick );
CK_DLL_PMSG( TubeBell_pmsg );
CK_DLL_CTRL( TubeBell_ctrl_noteOn );
CK_DLL_CTRL( TubeBell_ctrl_freq );
CK_DLL_CGET( TubeBell_cget_freq );

// Voicer
CK_DLL_CTOR( Voicer_ctor );
CK_DLL_DTOR( Voicer_dtor );
CK_DLL_TICK( Voicer_tick );
CK_DLL_PMSG( Voicer_pmsg );

// VoicForm
CK_DLL_CTOR( VoicForm_ctor );
CK_DLL_DTOR( VoicForm_dtor );
CK_DLL_TICK( VoicForm_tick );
CK_DLL_PMSG( VoicForm_pmsg );
CK_DLL_CTRL( VoicForm_ctrl_voiceMix );
CK_DLL_CGET( VoicForm_cget_voiceMix );
CK_DLL_CTRL( VoicForm_ctrl_phoneme );
CK_DLL_CGET( VoicForm_cget_phoneme );
CK_DLL_CTRL( VoicForm_ctrl_vibratoFreq );
CK_DLL_CGET( VoicForm_cget_vibratoFreq );
CK_DLL_CTRL( VoicForm_ctrl_vibratoGain );
CK_DLL_CGET( VoicForm_cget_vibratoGain );
CK_DLL_CTRL( VoicForm_ctrl_loudness );
CK_DLL_CGET( VoicForm_cget_loudness );
CK_DLL_CTRL( VoicForm_ctrl_freq );
CK_DLL_CGET( VoicForm_cget_freq );
CK_DLL_CTRL( VoicForm_ctrl_noteOn );
CK_DLL_CTRL( VoicForm_ctrl_noteOff );
CK_DLL_CTRL( VoicForm_ctrl_speak );
CK_DLL_CTRL( VoicForm_ctrl_quiet );
CK_DLL_CTRL( VoicForm_ctrl_voiced );
CK_DLL_CGET( VoicForm_cget_voiced );
CK_DLL_CTRL( VoicForm_ctrl_unVoiced );
CK_DLL_CGET( VoicForm_cget_unVoiced );
CK_DLL_CTRL( VoicForm_ctrl_pitchSweepRate );
CK_DLL_CGET( VoicForm_cget_pitchSweepRate );
CK_DLL_CTRL( VoicForm_ctrl_selPhoneme );
CK_DLL_CGET( VoicForm_cget_selPhoneme );

// Whistle
CK_DLL_CTOR( Whistle_ctor );
CK_DLL_DTOR( Whistle_dtor );
CK_DLL_TICK( Whistle_tick );
CK_DLL_PMSG( Whistle_pmsg );
CK_DLL_CTRL( Whistle_ctrl_freq );
CK_DLL_CTRL( Whistle_ctrl_startBlowing );
CK_DLL_CTRL( Whistle_ctrl_stopBlowing );
CK_DLL_CTRL( Whistle_ctrl_noteOn );
CK_DLL_CTRL( Whistle_ctrl_noteOff );

// Wurley
CK_DLL_CTOR( Wurley_ctor );
CK_DLL_DTOR( Wurley_dtor );
CK_DLL_TICK( Wurley_tick );
CK_DLL_PMSG( Wurley_pmsg );
CK_DLL_CTRL( Wurley_ctrl_freq );
CK_DLL_CTRL( Wurley_ctrl_noteOn );
CK_DLL_CTRL( Wurley_ctrl_noteOff );
CK_DLL_CGET( Wurley_cget_freq );



// Instrmnt
CK_DLL_CTOR( Instrmnt_ctor );
CK_DLL_DTOR( Instrmnt_dtor );
CK_DLL_TICK( Instrmnt_tick );
CK_DLL_PMSG( Instrmnt_pmsg );
CK_DLL_CTRL( Instrmnt_ctrl_freq );

// Mesh2D
CK_DLL_CTOR( Mesh2D_ctor );
CK_DLL_DTOR( Mesh2D_dtor );
CK_DLL_TICK( Mesh2D_tick );
CK_DLL_PMSG( Mesh2D_pmsg );
//CK_DLL_CTRL( Mesh2D_ctrl_NY );
//CK_DLL_CTRL( Mesh2D_ctrl_NY );
//CK_DLL_CTRL( Mesh2D_ctrl_NY );
//CK_DLL_CTRL( Mesh2D_ctrl_NX );
//CK_DLL_CTRL( Mesh2D_ctrl_NX );
//CK_DLL_CTRL( Mesh2D_ctrl_NX );
CK_DLL_CTRL( Mesh2D_ctrl_decay );

// Reverb
CK_DLL_CTOR( Reverb_ctor );
CK_DLL_DTOR( Reverb_dtor );
CK_DLL_TICK( Reverb_tick );
CK_DLL_PMSG( Reverb_pmsg );
CK_DLL_CTRL( Reverb_ctrl_effectMix );

// Socket
CK_DLL_CTOR( Socket_ctor );
CK_DLL_DTOR( Socket_dtor );
CK_DLL_TICK( Socket_tick );
CK_DLL_PMSG( Socket_pmsg );

// Stk
CK_DLL_CTOR( Stk_ctor );
CK_DLL_DTOR( Stk_dtor );
CK_DLL_TICK( Stk_tick );
CK_DLL_PMSG( Stk_pmsg );
CK_DLL_CTRL( Stk_ctrl_sampleRate );
CK_DLL_CTRL( Stk_ctrl_rawwavePath );

// Table
CK_DLL_CTOR( Table_ctor );
CK_DLL_DTOR( Table_dtor );
CK_DLL_TICK( Table_tick );
CK_DLL_PMSG( Table_pmsg );

// TcpWvIn
CK_DLL_CTOR( TcpWvIn_ctor );
CK_DLL_DTOR( TcpWvIn_dtor );
CK_DLL_TICK( TcpWvIn_tick );
CK_DLL_PMSG( TcpWvIn_pmsg );

// TcpWvOut
CK_DLL_CTOR( TcpWvOut_ctor );
CK_DLL_DTOR( TcpWvOut_dtor );
CK_DLL_TICK( TcpWvOut_tick );
CK_DLL_PMSG( TcpWvOut_pmsg );

// Vector3D
CK_DLL_CTOR( Vector3D_ctor );
CK_DLL_DTOR( Vector3D_dtor );
CK_DLL_TICK( Vector3D_tick );
CK_DLL_PMSG( Vector3D_pmsg );
CK_DLL_CTRL( Vector3D_ctrl_x );
CK_DLL_CTRL( Vector3D_ctrl_y );
CK_DLL_CTRL( Vector3D_ctrl_z );  

// JetTable
CK_DLL_CTOR( JetTabl_ctor );
CK_DLL_DTOR( JetTabl_dtor );
CK_DLL_TICK( JetTabl_tick );
CK_DLL_PMSG( JetTabl_pmsg );

// Mesh2D
CK_DLL_CTOR( Mesh2D_ctor );
CK_DLL_DTOR( Mesh2D_dtor );
CK_DLL_TICK( Mesh2D_tick );
CK_DLL_PMSG( Mesh2D_pmsg );
CK_DLL_CTRL( Mesh2D_ctrl_nx );
CK_DLL_CTRL( Mesh2D_ctrl_ny );
CK_DLL_CTRL( Mesh2D_cget_nx );
CK_DLL_CTRL( Mesh2D_cget_ny );
CK_DLL_CTRL( Mesh2D_ctrl_input_position );
CK_DLL_CTRL( Mesh2D_cget_input_position );
CK_DLL_CTRL( Mesh2D_ctrl_decay );
CK_DLL_CTRL( Mesh2D_cget_decay );
CK_DLL_CTRL( Mesh2D_ctrl_note_on );
CK_DLL_CTRL( Mesh2D_ctrl_note_off );
CK_DLL_CTRL( Mesh2D_cget_energy );
CK_DLL_CTRL( Mesh2D_ctrl_control_change );


// MidiFileIn
CK_DLL_CTOR( MidiFileIn_ctor );
CK_DLL_DTOR( MidiFileIn_dtor );
CK_DLL_MFUN( MidiFileIn_open );
CK_DLL_MFUN( MidiFileIn_close );
CK_DLL_MFUN( MidiFileIn_numTracks );
CK_DLL_MFUN( MidiFileIn_read );
CK_DLL_MFUN( MidiFileIn_readTrack );
CK_DLL_MFUN( MidiFileIn_rewind );




//-----------------------------------------------------------------------------
// name: stk_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY stk_query( Chuck_DL_Query * QUERY )
{

    Chuck_Env * env = QUERY->env();
    Chuck_DL_Func * func = NULL;

    std::string doc;
    
    // set srate
    Stk::setSampleRate( QUERY->srate );
    // test for endian

    what w; w.x = 1;
    little_endian = (t_CKBOOL)w.y[0];

    //! \sectionMain STK

    //! \section stk - instruments

    //------------------------------------------------------------------------
    // begin Instrmnt ugen
    //------------------------------------------------------------------------

    doc = "Super-class for STK instruments.";
    
    if( !type_engine_import_ugen_begin( env, "StkInstrument", "UGen", env->global(),
                                       Instrmnt_ctor, NULL, Instrmnt_tick, Instrmnt_pmsg,
                                       doc.c_str()) ) return FALSE;
    // member variable
    Instrmnt_offset_data = type_engine_import_mvar ( env, "int", "@Instrmnt_data", FALSE );
    if( Instrmnt_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "noteOn", Instrmnt_ctrl_noteOn ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Trigger note on.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noteOff", Instrmnt_ctrl_noteOff ); //! note off
    func->add_arg( "float", "value" );
    func->doc = "Trigger note off.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", Instrmnt_ctrl_freq ); //! frequency
    func->add_arg( "float", "value" );
    func->doc = "Set frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", Instrmnt_cget_freq ); //! frequency
    func->doc = "Get frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "controlChange", Instrmnt_ctrl_controlChange ); //! control change
    func->add_arg( "int", "ctrl" );
    func->add_arg( "float", "value" );
    func->doc = "Assert control change; numbers are instrument specific, value range:  [0.0 - 128.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin BandedWG ugen
    //------------------------------------------------------------------------

    doc = "This class uses banded waveguide techniques to model a variety of sounds, including bowed bars, glasses, and bowls.  For more information, see Essl, G. and Cook, P. 'Banded Waveguides: Towards Physical Modelling of Bar Percussion Instruments', Proceedings of the 1999 International Computer Music Conference.\n\
\n\
Control Change Numbers:\n\
\n\
- Bow Pressure = 2\n\
- Bow Motion = 4\n\
- Strike Position = 8 (not implemented)\n\
- Vibrato Frequency = 11\n\
- Gain = 1\n\
- Bow Velocity = 128\n\
- Instrument Presets = 16\n\
- Uniform Bar = 0\n\
- Tuned Bar = 1\n\
- Glass Harmonica = 2\n\
- Tibetan Bowl = 3\n\
\n\
by Georg Essl, 1999 - 2002.\n\
Modified for Stk 4.0 by Gary Scavone.";
    
    if( !type_engine_import_ugen_begin( env, "BandedWG", "StkInstrument", env->global(),
                        BandedWG_ctor, BandedWG_dtor,
                        BandedWG_tick, BandedWG_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/band-o-matic.ck");
    type_engine_import_add_ex(env, "stk/bandedwg.ck");
    type_engine_import_add_ex(env, "stk/bandedwg2.ck");

    // member variable
    // BandedWG_offset_data = type_engine_import_mvar ( env, "int", "@BandedWG_data", FALSE );
    // if( BandedWG_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "pluck", BandedWG_ctrl_pluck ); //! pluck waveguide
    func->add_arg( "float", "value" );
    func->doc = "Pluck instrument, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "startBowing", BandedWG_ctrl_startBowing ); //! startBowing
    func->add_arg( "float", "value" );
    func->doc = "Start bowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBowing", BandedWG_ctrl_stopBowing ); //! stopBowing
    func->add_arg( "float", "value" );
    func->doc = "Stop bowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowRate", BandedWG_ctrl_bowRate ); //! strike bowRate
    func->add_arg( "float", "value" );
    func->doc = "Set bowing rate (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowRate", BandedWG_cget_bowRate ); //! strike bowRate
    func->doc = "Get bowing rate (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPressure", BandedWG_ctrl_bowPressure ); //! bowPressure
    func->add_arg( "float", "value" );
    func->doc = "Set bow pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPressure", BandedWG_cget_bowPressure ); //! bowPressure
    func->doc = "Get bow pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowMotion", BandedWG_ctrl_bowMotion ); //! bowMotion
    func->add_arg( "float", "value" );
    func->doc = "Set bow motion, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowMotion", BandedWG_cget_bowMotion ); //! bowMotion
    func->doc = "Get bow motion, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "integrationConstant", BandedWG_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set integration constant, vibrato frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "integrationConstant", BandedWG_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get integration constant, vibrato frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modesGain", BandedWG_ctrl_modesGain ); //! modesGain
    func->add_arg( "float", "value" );
    func->doc = "Set amplitude for modes.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modesGain", BandedWG_cget_modesGain ); //! modesGain
    func->doc = "Get amplitude for modes.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", BandedWG_ctrl_preset ); //! preset
    func->add_arg( "int", "value" );
    func->doc = "Set instrument preset, (0-3, see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", BandedWG_cget_preset ); //! preset
    func->doc = "Get instrument preset, (0-3, see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "strikePosition", BandedWG_ctrl_strikePosition ); //! strike Position
    func->add_arg( "float", "value" );
    func->doc = "Set strike position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "strikePosition", BandedWG_cget_strikePosition ); //! strike Position
    func->doc = "Get strike position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin BlowBotl ugen
    //------------------------------------------------------------------------

    doc = "This class implements a helmholtz resonator (biquad filter) with a polynomial jet excitation (a la Cook).\n\
\n\
Control Change Numbers:\n\
\n\
- Noise Gain = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Volume = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002";

    if( !type_engine_import_ugen_begin( env, "BlowBotl", "StkInstrument", env->global(), 
                        BlowBotl_ctor, BlowBotl_dtor,
                        BlowBotl_tick, BlowBotl_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/blowbotl.ck");
    type_engine_import_add_ex(env, "stk/blowbotl2.ck");

    // member variable
    // BlowBotl_offset_data = type_engine_import_mvar ( env, "int", "@BlowBotl_data", FALSE );
    // if( BlowBotl_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "startBlowing", BlowBotl_ctrl_startBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", BlowBotl_ctrl_stopBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", BlowBotl_ctrl_rate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", BlowBotl_cget_rate ); //! attack rate
    func->doc = "Get rate of attack";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", BlowBotl_ctrl_noiseGain ); //! noiseGain
    func->add_arg( "float", "value" );
    func->doc = "Set noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", BlowBotl_cget_noiseGain ); //! noiseGain
    func->doc = "Get noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", BlowBotl_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", BlowBotl_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", BlowBotl_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", BlowBotl_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", BlowBotl_ctrl_volume ); //! volume
    func->add_arg( "float", "value" );
    func->doc = "Set volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", BlowBotl_cget_volume ); //! volume
    func->doc = "Get volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );



    //------------------------------------------------------------------------
    // begin BlowHole ugen
    //------------------------------------------------------------------------

    doc = "This class is based on the clarinet model, with the addition of a two-port register hole and a three-port dynamic tonehole implementation, as discussed by Scavone and Cook (1998).\n\
\n\
In this implementation, the distances between the reed/register hole and tonehole/bell are fixed.  As a result, both the tonehole and register hole will have variable influence on the playing frequency, which is dependent on the length of the air column.  In addition, the highest playing freqeuency is limited by these fixed lengths. This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Reed Stiffness = 2\n\
- Noise Gain = 4\n\
- Tonehole State = 11\n\
- Register State = 1\n\
- Breath Pressure = 128\n\
\n\
Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "BlowHole", "StkInstrument", env->global(), 
                        BlowHole_ctor, BlowHole_dtor,
                        BlowHole_tick, BlowHole_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/blowhole.ck");
    type_engine_import_add_ex(env, "stk/blowhole2.ck");

    // member variable
    // BlowHole_offset_data = type_engine_import_mvar ( env, "int", "@BlowHole_data", FALSE );
    // if( BlowHole_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "startBlowing", BlowHole_ctrl_startBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", BlowHole_ctrl_stopBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vent", BlowHole_ctrl_vent ); //! vent frequency
    func->add_arg( "float", "value" );
    func->doc = "Set vent frequency, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vent", BlowHole_cget_vent ); //! vent frequency
    func->doc = "Get vent frequency, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "tonehole", BlowHole_ctrl_tonehole ); //! tonehole size
    func->add_arg( "float", "value" );
    func->doc = "Set tonehole size, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "tonehole", BlowHole_cget_tonehole ); //! tonehole size
    func->doc = "Get tonehole size, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "reed", BlowHole_ctrl_reed ); //! reed stiffness
    func->add_arg( "float", "value" );
    func->doc = "Set reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "reed", BlowHole_cget_reed ); //! reed stiffness
    func->doc = "Get reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", BlowHole_ctrl_rate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", BlowHole_cget_rate ); //! attack rate
    func->doc = "Get rate of attack, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", BlowHole_ctrl_noiseGain ); //! noise gain
    func->add_arg( "float", "value" );
    func->doc = "Set noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", BlowHole_cget_noiseGain ); //! noise gain
    func->doc = "Get noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", BlowHole_ctrl_pressure ); //! breath pressure
    func->add_arg( "float", "value" );
    func->doc = "Set pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", BlowHole_cget_pressure ); //! breath pressure
    func->doc = "Get pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Bowed ugen
    //------------------------------------------------------------------------

    doc = "This class implements a bowed string model, a la Smith (1986), after McIntyre, Schumacher, Woodhouse (1983).\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Bow Pressure = 2\n\
- Bow Position = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Volume = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Bowed", "StkInstrument", env->global(), 
                        Bowed_ctor, Bowed_dtor,
                        Bowed_tick, Bowed_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/bowed.ck");
    type_engine_import_add_ex(env, "stk/bowed2.ck");

    // member variable
    // Bowed_offset_data = type_engine_import_mvar ( env, "int", "@Bowed_data", FALSE );
    // if( Bowed_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "startBowing", Bowed_ctrl_startBowing ); //! begin bowing instrument
    func->add_arg( "float", "value" );
    func->doc = "Start bowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBowing", Bowed_ctrl_stopBowing ); //! stop bowing
    func->add_arg( "float", "value" );
    func->doc = "Stop bowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPressure", Bowed_ctrl_bowPressure ); //! bowPressure
    func->add_arg( "float", "value" );
    func->doc = "Set bow pressure, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPressure", Bowed_cget_bowPressure ); //! bowPressure
    func->doc = "Get bow pressure, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPosition", Bowed_ctrl_bowPos ); //! bowPos
    func->add_arg( "float", "value" );
    func->doc = "Set bow position, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bowPosition", Bowed_cget_bowPos ); //! bowPos
    func->doc = "Get bow position, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Bowed_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Bowed_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Bowed_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Bowed_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Bowed_ctrl_volume ); //! volume
    func->add_arg( "float", "value" );
    func->doc = "Set volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Bowed_cget_volume ); //! volume
    func->doc = "Get volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Brass ugen
    //------------------------------------------------------------------------

    doc = "This class implements a simple brass instrument waveguide model, a la Cook (TBone, HosePlayer).\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Lip Tension = 2\n\
- Slide Length = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Volume = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Brass", "StkInstrument", env->global(), 
                        Brass_ctor, Brass_dtor,
                        Brass_tick, Brass_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/brass.ck");
    type_engine_import_add_ex(env, "stk/brass2.ck");

    // member variable
    // Brass_offset_data = type_engine_import_mvar ( env, "int", "@Brass_data", FALSE );
    // if( Brass_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "clear", Brass_ctrl_clear ); //! clear instrument
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "startBlowing", Brass_ctrl_startBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", Brass_ctrl_stopBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lip", Brass_ctrl_lip ); //! lip stiffness
    func->add_arg( "float", "value" );
    func->doc = "Set lip tension, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lip", Brass_cget_lip ); //! lip stiffness
    func->doc = "Get lip tension, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "slide", Brass_ctrl_slide ); //! slide
    func->add_arg( "float", "value" );
    func->doc = "Set slide length, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "slide", Brass_cget_slide ); //! slide
    func->doc = "Get slide length, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Brass_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Brass_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Brass_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Brass_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Brass_ctrl_volume ); //! volume
    func->add_arg( "float", "value" );
    func->doc = "Set volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Brass_cget_volume ); //! volume
    func->doc = "Get volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Brass_ctrl_rate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Brass_cget_rate ); //! attack rate
    func->doc = "Get rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Clarinet ugen
    //------------------------------------------------------------------------

    doc = "This class implements a simple clarinet physical model, as discussed by Smith (1986), McIntyre, Schumacher, Woodhouse (1983), and others.\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Reed Stiffness = 2\n\
- Noise Gain = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Breath Pressure = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Clarinet", "StkInstrument", env->global(), 
                        Clarinet_ctor, Clarinet_dtor,
                        Clarinet_tick, Clarinet_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/clarinet.ck");
    type_engine_import_add_ex(env, "stk/clarinet2.ck");

    // member variable
    // Clarinet_offset_data = type_engine_import_mvar ( env, "int", "@Clarinet_data", FALSE );
    // if( Clarinet_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "clear", Clarinet_ctrl_clear ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "startBlowing", Clarinet_ctrl_startBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", Clarinet_ctrl_stopBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Clarinet_ctrl_rate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Clarinet_cget_rate ); //! attack rate
    func->doc = "Get rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "reed", Clarinet_ctrl_reed); //! reed
    func->add_arg( "float", "value" );
    func->doc = "Set reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "reed", Clarinet_cget_reed); //! reed
    func->doc = "Get reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Clarinet_ctrl_noiseGain ); //! noiseGain
    func->add_arg( "float", "value" );
    func->doc = "Set noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Clarinet_cget_noiseGain ); //! noiseGain
    func->doc = "Get noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Clarinet_ctrl_vibratoFreq ); //! rvibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Clarinet_cget_vibratoFreq ); //! rvibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Clarinet_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Clarinet_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Clarinet_ctrl_pressure ); //! pressure
    func->add_arg( "float", "value" );
    func->doc = "Set pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Clarinet_cget_pressure ); //! pressure
    func->doc = "Get pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Flute ugen
    //------------------------------------------------------------------------

    doc = "This class implements a simple flute physical model, as discussed by Karjalainen, Smith, Waryznyk, etc. The jet model uses a polynomial, a la Cook.\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Jet Delay = 2\n\
- Noise Gain = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Breath Pressure = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Flute", "StkInstrument", env->global(), 
                        Flute_ctor, Flute_dtor,
                        Flute_tick, Flute_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/flute.ck");

    // member variable
    // Flute_offset_data = type_engine_import_mvar ( env, "int", "@Flute_data", FALSE );
    // if( Flute_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "clear", Flute_ctrl_clear ); //! clear
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "startBlowing", Flute_ctrl_startBlowing ); //! startBlowing
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", Flute_ctrl_stopBlowing ); //! stopBlowing
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Flute_ctrl_rate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Flute_cget_rate ); //! attack rate
    func->doc = "Get rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "jetReflection", Flute_ctrl_jetReflection ); //! jet reflection
    func->add_arg( "float", "value" );
    func-> doc = "Set jet reflection.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "jetReflection", Flute_cget_jetReflection ); //! jet reflection
    func-> doc = "Get jet reflection.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "jetDelay", Flute_ctrl_jetDelay ); //! jet delay
    func->add_arg( "float", "value" );
    func->doc = "Set jet delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "jetDelay", Flute_cget_jetDelay ); //! jet delay
    func->doc = "Get jet delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "endReflection", Flute_ctrl_endReflection ); //! end reflection
    func->add_arg( "float", "value" );
    func->doc = "Set end reflection.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "endReflection", Flute_cget_endReflection ); //! end reflection
    func->doc = "Get end reflection.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Flute_ctrl_noiseGain ); //! noiseGain
    func->add_arg( "float", "value" );
    func->doc = "Set noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Flute_cget_noiseGain ); //! noiseGain
    func->doc = "Get noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Flute_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Flute_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Flute_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Flute_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Flute_ctrl_pressure ); //! pressure
    func->add_arg( "float", "value" );
    func->doc = "Set pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Flute_cget_pressure ); //! pressure
    func->doc = "Get pressure, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Mandolin ugen
    //------------------------------------------------------------------------

    doc = "This class inherits from PluckTwo and uses 'commuted synthesis' techniques to model a mandolin instrument.\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others. Commuted Synthesis, in particular, is covered by patents, granted, pending, and/or applied-for. All are assigned to the Board of Trustees, Stanford University.  For information, contact the Office of Technology Licensing, Stanford University.\n\
\n\
Control Change Numbers:\n\
\n\
- Body Size = 2\n\
- Pluck Position = 4\n\
- String Sustain = 11\n\
- String Detuning = 1\n\
- Microphone Position = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example mand-o-matic.ck
    if( !type_engine_import_ugen_begin( env, "Mandolin", "StkInstrument", env->global(), 
                        Mandolin_ctor, Mandolin_dtor,
                        Mandolin_tick, Mandolin_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/mandolin.ck");
    type_engine_import_add_ex(env, "stk/mand-o-matic.ck");
    type_engine_import_add_ex(env, "stk/mand-o-matic-simple.ck");

    // member variable
    // Mandolin_offset_data = type_engine_import_mvar ( env, "int", "@Mandolin_data", FALSE );
    // if( Mandolin_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "float", "pluck", Mandolin_ctrl_pluck );  //! pluck string with given amplitude 
    func->add_arg( "float", "value" );
    func->doc = "Pluck instrument, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pluckPos", Mandolin_ctrl_pluckPos ); //! set pluck position ( 0-1) along string
    func->add_arg( "float", "value" );
    func->doc = "Set pluck position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pluckPos", Mandolin_cget_pluckPos ); //! get pluck position ( 0-1) along string
    func->doc = "Get pluck position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bodySize", Mandolin_ctrl_bodySize ); //! modify instrument size
    func->add_arg( "float", "value" );
    func->doc = "Set body size (percentage).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "bodySize", Mandolin_cget_bodySize ); //! modify instrument size
    func->doc = "Get body size (percentage).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stringDamping", Mandolin_ctrl_stringDamping ); //! control string damping
    func->add_arg( "float", "value" );
    func->doc = "Set string damping, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stringDamping", Mandolin_cget_stringDamping ); //! control string damping
    func->doc = "Get string damping, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stringDetune", Mandolin_ctrl_stringDetune ); //! control detuning of string pair
    func->add_arg( "float", "value" );
    func->doc = "Set detuning of string pair, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stringDetune", Mandolin_cget_stringDetune ); //! control detuning of string pair
    func->doc = "Get detuning of string pair, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "afterTouch", Mandolin_ctrl_afterTouch ); //! aftertouch
    func->add_arg( "float", "value" );
    func->doc = "Set aftertouch (currently unsupported).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "bodyIR", Mandolin_ctrl_bodyIR ); //! read body impulse response
    func->add_arg( "string", "path" );
    func->doc = "Set body impulse response.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "bodyIR", Mandolin_cget_bodyIR ); //! get path
    func->doc = "Get body impulse response.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin ModalBar ugen
    //------------------------------------------------------------------------

    doc = "This class implements a number of different struck bar instruments. It inherits from the Modal class.\n\
\n\
Control Change Numbers:\n\
\n\
- Stick Hardness = 2\n\
- Stick Position = 4\n\
- Vibrato Gain = 11\n\
- Vibrato Frequency = 7\n\
- Direct Stick Mix = 1\n\
- Volume = 128\n\
- Modal Presets = 16\n\
- Marimba = 0\n\
- Vibraphone = 1\n\
- Agogo = 2\n\
- Wood1 = 3\n\
- Reso = 4\n\
- Wood2 = 5\n\
- Beats = 6\n\
- Two Fixed = 7\n\
- Clump = 8\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example modalbot.ck
    if( !type_engine_import_ugen_begin( env, "ModalBar", "StkInstrument", env->global(),
                        ModalBar_ctor, ModalBar_dtor,
                        ModalBar_tick, ModalBar_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/modalbar.ck");
    type_engine_import_add_ex(env, "stk/modalbar2.ck");
    
    // member variable
    // ModalBar_offset_data = type_engine_import_mvar ( env, "int", "@ModalBar_data", FALSE );
    // if( ModalBar_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "strike", ModalBar_ctrl_strike ); //! strike bar
    func->add_arg( "float", "value" );
    func->doc = "Strike bar, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "damp", ModalBar_ctrl_damp ); //! damp bar
    func->add_arg( "float", "value" );
    func->doc = "Damp bar, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "clear", ModalBar_ctrl_clear ); //! clear
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", ModalBar_ctrl_preset ); //! choose preset
    func->add_arg( "int", "value" );
    func->doc = "Set preset (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", ModalBar_cget_preset ); //! choose preset
    func->doc = "Get preset.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stickHardness", ModalBar_ctrl_stickHardness ); //! set stickHardness
    func->add_arg( "float", "value" );
    func->doc = "Set stick hardness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stickHardness", ModalBar_cget_stickHardness ); //! set stickHardness
    func->doc = "Get stick hardness, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "strikePosition", ModalBar_ctrl_strikePosition ); //! set strikePosition
    func->add_arg( "float", "value" );
    func->doc = "Set strike position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "strikePosition", ModalBar_cget_strikePosition ); //! set strikePosition
    func->doc = "Get strike position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", ModalBar_ctrl_vibratoGain ); //! set vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", ModalBar_cget_vibratoGain ); //! set vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", ModalBar_ctrl_vibratoFreq ); //! set vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", ModalBar_cget_vibratoFreq ); //! set vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "directGain", ModalBar_ctrl_directGain ); //! set directGain
    func->add_arg( "float", "value" );
    func->doc = "Set direct gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "directGain", ModalBar_cget_directGain ); //! set directGain
    func->doc = "Get direct gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "masterGain", ModalBar_ctrl_masterGain ); //! set masterGain
    func->add_arg( "float", "value" );
    func->doc = "Set master gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "masterGain", ModalBar_cget_masterGain ); //! set masterGain
    func->doc = "Get master gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "mode", ModalBar_ctrl_mode ); //! choose mode
    func->add_arg( "int", "value" );
    func->doc = "Set mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "mode", ModalBar_cget_mode ); //! choose mode
    func->doc = "Get mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeRatio", ModalBar_ctrl_modeRatio ); //! mode edit (modeRatio)
    func->add_arg( "float", "value" );
    func->doc = "Set ratio for selected mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeRatio", ModalBar_cget_modeRatio ); //! mode edit (modeRatio)
    func->doc = "Get ratio for selected mode.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeRadius", ModalBar_ctrl_modeRadius ); //! mode edit (modeRadius)
    func->add_arg( "float", "value" );
    func->doc = "Set radius for selected mode, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeRadius", ModalBar_cget_modeRadius ); //! mode edit (modeRadius)
    func->doc = "Get radius for selected mode, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeGain", ModalBar_ctrl_modeGain ); //! mode edit (modeGain)
    func->add_arg( "float", "value" );
    func->doc = "Set gain for selected mode, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modeGain", ModalBar_cget_modeGain ); //! mode edit (modeGain)
    func->doc = "Get gain for selected mode, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", ModalBar_ctrl_volume ); //! volume
    func->add_arg( "float", "value" );
    func->doc = "Set volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", ModalBar_cget_volume ); //! volume
    func->doc = "Get volume, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Moog ugen
    //------------------------------------------------------------------------

    doc = "This instrument uses one attack wave, one looped wave, and an ADSR envelope (inherited from the Sampler class) and adds two sweepable formant (FormSwep) filters.\n\
\n\
Control Change Numbers:\n\
\n\
- Filter Q = 2\n\
- Filter Sweep Rate = 4\n\
- Vibrato Frequency = 11\n\
- Vibrato Gain = 1\n\
- Gain = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example moogie.ck
    if( !type_engine_import_ugen_begin( env, "Moog", "StkInstrument", env->global(), 
                        Moog_ctor, Moog_dtor,
                        Moog_tick, Moog_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/moog.ck");
    type_engine_import_add_ex(env, "stk/moog2.ck");
    type_engine_import_add_ex(env, "stk/moogie.ck");

    // member variable
    // Moog_offset_data = type_engine_import_mvar ( env, "int", "@Moog_data", FALSE );
    // if( Moog_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "lfoSpeed", Moog_ctrl_modSpeed ); //! modulation speed
    func->add_arg( "float", "value" );
    func->doc = "Set LFO modulation speed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoSpeed", Moog_cget_modSpeed ); //! modulation speed
    func->doc = "Get LFO modulation speed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoDepth", Moog_ctrl_modDepth ); //! modulation depth
    func->add_arg( "float", "value" );
    func->doc = "Set LFO modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoDepth", Moog_cget_modDepth ); //! modulation depth
    func->doc = "Get LFO modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modSpeed", Moog_ctrl_modSpeed ); //! modulation speed
    func->add_arg( "float", "value" );
    func->doc = "Set modulation speed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modSpeed", Moog_cget_modSpeed ); //! modulation speed
    func->doc = "Get modulation speed.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modDepth", Moog_ctrl_modDepth ); //! modulation depth
    func->add_arg( "float", "value" );
    func->doc = "Set modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modDepth", Moog_cget_modDepth ); //! modulation depth
    func->doc = "Get modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "filterQ", Moog_ctrl_filterQ ); //! filter Q value
    func->add_arg( "float", "value" );
    func->doc = "Set filter Q value, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "filterQ", Moog_cget_filterQ ); //! filter Q value
    func->doc = "Get filter Q value, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "filterSweepRate", Moog_ctrl_filterSweepRate ); //! filter sweep rate
    func->add_arg( "float", "value" );
    func->doc = "Set filter sweep rate, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "filterSweepRate", Moog_cget_filterSweepRate ); //! filter sweep rate
    func->doc = "Get filter sweep rate, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "afterTouch", Moog_ctrl_afterTouch ); // aftertouch
    func->add_arg( "float", "value" );
    func->doc = "Set aftertouch, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Moog_ctrl_volume ); //! volume
    func->add_arg( "float", "value" );
    func->doc = "Set volume.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "volume", Moog_cget_volume ); //! volume
    func->doc = "Get volume.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Moog_ctrl_vibratoFreq ); //! vibrato frequency
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Moog_cget_vibratoFreq ); //! vibrato frequency
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Moog_ctrl_vibratoGain ); //! vibrato gain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Moog_cget_vibratoGain ); //! vibrato gain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Saxofony ugen
    //------------------------------------------------------------------------

    doc = "This class implements a 'hybrid' digital waveguide instrument that can generate a variety of wind-like sounds.  It has also been referred to as the 'blowed string' model. The waveguide section is essentially that of a string, with one rigid and one lossy termination.  The non-linear function is a reed table.  The string can be 'blown' at any point between the terminations, though just as with strings, it is impossible to excite the system at either end.  If the excitation is placed at the string mid-point, the sound is that of a clarinet.  At points closer to the 'bridge', the sound is closer to that of a saxophone.  See Scavone (2002) for more details.\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Reed Stiffness = 2\n\
- Reed Aperture = 26\n\
- Noise Gain = 4\n\
- Blow Position = 11\n\
- Vibrato Frequency = 29\n\
- Vibrato Gain = 1\n\
- Breath Pressure = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Saxofony", "StkInstrument", env->global(), 
                        Saxofony_ctor, Saxofony_dtor,
                        Saxofony_tick, Saxofony_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/saxofony.ck");

    // member variable
    // Saxofony_offset_data = type_engine_import_mvar ( env, "int", "@Saxofony_data", FALSE );
    // if( Saxofony_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "clear", Saxofony_ctrl_clear ); //! clear instrument
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "startBlowing", Saxofony_ctrl_startBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Start blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stopBlowing", Saxofony_ctrl_stopBlowing ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Stop blowing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Saxofony_ctrl_rate ); //! rate
    func->add_arg( "float", "value" );
    func->doc = "Set rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Saxofony_cget_rate ); //! rate
    func->doc = "Get rate of attack (seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stiffness", Saxofony_ctrl_reed ); //! stiffness
    func->add_arg( "float", "value" );
    func->doc = "Set reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stiffness", Saxofony_cget_reed ); //! stiffness
    func->doc = "Get reed stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "aperture", Saxofony_ctrl_aperture ); //! aperture
    func->add_arg( "float", "value" );
    func->doc = "Set reed aperture, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "aperture", Saxofony_cget_aperture ); //! aperture
    func->doc = "Get reed aperture, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Saxofony_ctrl_noiseGain ); //! noiseGain
    func->add_arg( "float", "value" );
    func->doc = "Set noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noiseGain", Saxofony_cget_noiseGain ); //! noiseGain
    func->doc = "Get noise component gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Saxofony_ctrl_vibratoGain ); //! vibratoGain
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Saxofony_cget_vibratoGain ); //! vibratoGain
    func->doc = "Get vibrato gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Saxofony_ctrl_vibratoFreq ); //! vibratoFreq
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", Saxofony_cget_vibratoFreq ); //! vibratoFreq
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "blowPosition", Saxofony_ctrl_blowPosition ); //! blowPosition
    func->add_arg( "float", "value" );
    func->doc = "Set lip stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "blowPosition", Saxofony_cget_blowPosition ); //! blowPosition
    func->doc = "Get lip stiffness, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Saxofony_ctrl_pressure ); //! pressure
    func->add_arg( "float", "value" );
    func->doc = "Set pressure, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pressure", Saxofony_cget_pressure ); //! pressure
    func->doc = "Get pressure, [0.0-1.0]";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Shakers ugen
    //------------------------------------------------------------------------

    doc = "PhISEM (Physically Informed Stochastic Event Modeling) is an algorithmic approach for simulating collisions of multiple independent sound producing objects. This class is a meta-model that can simulate a Maraca, Sekere, Cabasa, Bamboo Wind Chimes, Water Drops, Tambourine, Sleighbells, and a Guiro.\n\
\n\
PhOLIES (Physically-Oriented Library of Imitated Environmental Sounds) is a similar approach for the synthesis of environmental sounds. This class implements simulations of breaking sticks, crunchy snow (or not), a wrench, sandpaper, and more.\n\
\n\
Control Change Numbers:\n\
\n\
- Shake Energy = 2\n\
- System Decay = 4\n\
- Number Of Objects = 11\n\
- Resonance Frequency = 1\n\
- Shake Energy = 128\n\
- Instrument Selection = 1071\n\
- Maraca = 0\n\
- Cabasa = 1\n\
- Sekere = 2\n\
- Guiro = 3\n\
- Water Drops = 4\n\
- Bamboo Chimes = 5\n\
- Tambourine = 6\n\
- Sleigh Bells = 7\n\
- Sticks = 8\n\
- Crunch = 9\n\
- Wrench = 10\n\
- Sand Paper = 11\n\
- Coke Can = 12\n\
- Next Mug = 13\n\
- Penny + Mug = 14\n\
- Nickle + Mug = 15\n\
- Dime + Mug = 16\n\
- Quarter + Mug = 17\n\
- Franc + Mug = 18\n\
- Peso + Mug = 19\n\
- Big Rocks = 20\n\
- Little Rocks = 21\n\
- Tuned Bamboo Chimes = 22\n\
\n\
by Perry R. Cook, 1996 - 1999.";
    
    //! see \example shake-o-matic.ck
    if( !type_engine_import_ugen_begin( env, "Shakers", "StkInstrument", env->global(), 
                        Shakers_ctor, Shakers_dtor,
                        Shakers_tick, Shakers_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/shake-cycle.ck");
    type_engine_import_add_ex(env, "stk/shake-o-matic.ck");
    
    // member variable
    // Shakers_offset_data = type_engine_import_mvar ( env, "int", "@Shakers_data", FALSE );
    // if( Shakers_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "int", "which", Shakers_ctrl_which ); //! select instrument
    func->add_arg( "int", "value" );
    func->doc = "Set instrument, [0-22] (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "which", Shakers_cget_which ); //! select instrument
    func->doc = "Get instrument, [0-22] (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", Shakers_ctrl_which ); //! select instrument
    func->add_arg( "int", "value" );
    func->doc = "Set instrument, [0-22] (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "preset", Shakers_cget_which ); //! select instrument
    func->doc = "Get instrument, [0-22] (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "energy", Shakers_ctrl_energy ); //! energy
    func->add_arg( "float", "value" );
    func->doc = "Set shake energy, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "energy", Shakers_cget_energy ); //! energy
    func->doc = "Get shake energy, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "decay", Shakers_ctrl_decay ); //! decay
    func->add_arg( "float", "value" );
    func->doc = "Set system decay, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "decay", Shakers_cget_decay ); //! decay
    func->doc = "Get system decay, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "objects", Shakers_ctrl_objects ); //! objects
    func->add_arg( "float", "value" );
    func->doc = "Set number of objects, [0.0-128.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "objects", Shakers_cget_objects ); //! objects
    func->doc = "Get number of objects, [0.0-128.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", Shakers_ctrl_freq ); //! frequency
    func->add_arg( "float", "value" );
    func->doc = "Set frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", Shakers_cget_freq ); //! frequency
    func->doc = "Get frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noteOn", Shakers_ctrl_noteOn ); //! note on
    func->add_arg( "float", "value" );
    func->doc = "Trigger note on.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "noteOff", Shakers_ctrl_noteOff ); //! note off
    func->add_arg( "float", "value" );
    func->doc = "Trigger note off.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Sitar ugen
    //------------------------------------------------------------------------

    doc = "This class implements a sitar plucked string physical model based on the Karplus-Strong algorithm.\n\
    \n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others. There exist at least two patents, assigned to Stanford, bearing the names of Karplus and/or Strong.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Sitar", "StkInstrument", env->global(), 
                        Sitar_ctor, Sitar_dtor,
                        Sitar_tick, Sitar_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/sitar.ck");

    // member variable
    // Sitar_offset_data = type_engine_import_mvar ( env, "int", "@Sitar_data", FALSE );
    // if( Sitar_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "float", "pluck", Sitar_ctrl_pluck ); //! pluck
    func->add_arg( "float", "value" );
    func->doc = "Pluck string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "clear", Sitar_ctrl_clear ); 
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin StifKarp ugen
    //------------------------------------------------------------------------

    doc = "This class implements a simple plucked string algorithm (Karplus Strong) with enhancements (Jaffe-Smith, Smith, and others), including string stiffness and pluck position controls. The stiffness is modeled with allpass filters.\n\
\n\
This is a digital waveguide model, making its use possibly subject to patents held by Stanford University, Yamaha, and others.\n\
\n\
Control Change Numbers:\n\
\n\
- Pickup Position = 4\n\
- String Sustain = 11\n\
- String Stretch = 1\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example stifkarp.ck
    if( !type_engine_import_ugen_begin( env, "StifKarp", "StkInstrument", env->global(), 
                        StifKarp_ctor, StifKarp_dtor,
                        StifKarp_tick, StifKarp_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/stifkarp.ck");
    type_engine_import_add_ex(env, "stk/stif-o-karp.ck");
    
    // member variable
    // StifKarp_offset_data = type_engine_import_mvar ( env, "int", "@StifKarp_data", FALSE );
    // if( StifKarp_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "pluck", StifKarp_ctrl_pluck ); //! pluck
    func->add_arg( "float", "value" );
    func->doc = "Pluck string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "clear", StifKarp_ctrl_clear ); 
    func->add_arg( "float", "value" );
    func->doc = "Clear instrument.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pickupPosition", StifKarp_ctrl_pickupPosition ); 
    func->add_arg( "float", "value" );
    func->doc = "Set pickup position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pickupPosition", StifKarp_cget_pickupPosition );
    func->doc = "Get pickup position, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stretch", StifKarp_ctrl_stretch ); 
    func->add_arg( "float", "value" );
    func->doc = "Set string strech, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "stretch", StifKarp_cget_stretch );
    func->doc = "Get string strech, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "sustain", StifKarp_ctrl_sustain ); 
    func->add_arg( "float", "value" );
    func->doc = "Set string sustain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "sustain", StifKarp_cget_sustain );
    func->doc = "Get string sustain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "baseLoopGain", StifKarp_ctrl_baseLoopGain ); 
    func->add_arg( "float", "value" );
    func->doc = "Set base loop gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "baseLoopGain", StifKarp_cget_baseLoopGain );
    func->doc = "Get base loop gain, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin VoicForm ugen
    //------------------------------------------------------------------------

    doc = "This instrument contains an excitation singing wavetable (looping wave with random and periodic vibrato, smoothing on frequency, etc.), excitation noise, and four sweepable complex resonances.\n\
\n\
Measured formant data is included, and enough data is there to support either parallel or cascade synthesis. In the floating point case cascade synthesis is the most natural so that's what you'll find here.\n\
\n\
Control Change Numbers:\n\
Voiced/Unvoiced Mix = 2\n\
Vowel/Phoneme Selection = 4\n\
Vibrato Frequency = 11\n\
Vibrato Gain = 1\n\
Loudness (Spectral Tilt) = 128\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.\n\
\n\
Phoneme Names:\n\
\n\
    'eee'  'ihh'  'ehh'  'aaa'\n\
    'ahh'  'aww'  'ohh'  'uhh'\n\
    'uuu'  'ooo'  'rrr'  'lll'\n\
    'mmm'  'nnn'  'nng'  'ngg'\n\
    'fff'  'sss'  'thh'  'shh'\n\
    'xxx'  'hee'  'hoo'  'hah'\n\
    'bbb'  'ddd'  'jjj'  'ggg'\n\
    'vvv'  'zzz'  'thz'  'zhh'";
    
    //! see \example voic-o-form.ck
    if( !type_engine_import_ugen_begin( env, "VoicForm", "StkInstrument", env->global(), 
                        VoicForm_ctor, VoicForm_dtor,
                        VoicForm_tick, VoicForm_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/voic-o-form.ck");

    // member variable
    // VoicForm_offset_data = type_engine_import_mvar ( env, "int", "@VoicForm_data", FALSE );
    // if( VoicForm_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "string", "phoneme", VoicForm_ctrl_phoneme ); //! select phoneme  ( above ) 
    func->add_arg( "string", "value" );
    func->doc = "Set phoneme (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "phoneme", VoicForm_cget_phoneme ); //! select phoneme  ( above )
    func->doc = "Get phoneme (see above).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "speak", VoicForm_ctrl_speak ); //! start singing
    func->add_arg( "float", "value" );
    func->doc = "Start singing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "quiet", VoicForm_ctrl_quiet ); //! stop singing
    func->add_arg( "float", "value" );
    func->doc = "Stop singing, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "voiced", VoicForm_ctrl_voiced ); //! set mix for voiced component
    func->add_arg( "float", "value" );
    func->doc = "Set mix for voiced component, [0.0 - 1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "voiced", VoicForm_cget_voiced ); //! set mix for voiced component
    func->doc = "Get mix for voiced component, [0.0 - 1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "unVoiced", VoicForm_ctrl_unVoiced ); //! set mix for unvoiced component
    func->add_arg( "float", "value" );
    func->doc = "Set mix for unvoiced component, [0.0 - 1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "unVoiced", VoicForm_cget_unVoiced ); //! set mix for unvoiced component
    func->doc = "Get mix for unvoiced component, [0.0 - 1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pitchSweepRate", VoicForm_ctrl_pitchSweepRate ); //! pitch sweep
    func->add_arg( "float", "value" );
    func->doc = "Set rate of pitch sweep, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pitchSweepRate", VoicForm_cget_pitchSweepRate ); //! pitch sweep
    func->doc = "Get rate of pitch sweep, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "voiceMix", VoicForm_ctrl_voiceMix ); //! voiced/unvoiced mix
    func->add_arg( "float", "value" );
    func->doc = "Set voiced/unvoiced mix, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "voiceMix", VoicForm_cget_voiceMix ); //! voiced/unvoiced mix
    func->doc = "Get voiced/unvoiced mix, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "phonemeNum", VoicForm_ctrl_selPhoneme ); //! select phoneme by number
    func->add_arg( "int", "value" );
    func->doc = "Set phoneme by number, [0.0-128.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "phonemeNum", VoicForm_cget_selPhoneme ); //! select phoneme by number
    func->doc = "Get phoneme by number, [0.0-128.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", VoicForm_ctrl_vibratoFreq );//! vibrato
    func->add_arg( "float", "value" );
    func->doc = "Set vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoFreq", VoicForm_cget_vibratoFreq );//! vibrato
    func->doc = "Get vibrato frequency (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", VoicForm_ctrl_vibratoGain );//! vibrato depth
    func->doc = "Set vibrato gain (Hz), [0.0-1.0].";
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", VoicForm_cget_vibratoGain );//! vibrato depth
    func->doc = "Get vibrato gain (Hz), [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "loudness", VoicForm_ctrl_loudness ); //! 'loudness' of voicee
    func->add_arg( "float", "value" );
    func->doc = "Set 'loudness' of voice, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "loudness", VoicForm_cget_loudness ); //! 'loudness' of voicee
    func->doc = "Get 'loudness' of voice, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //! \section2 stk - fm synths


    //------------------------------------------------------------------------
    // begin FM ugen
    //------------------------------------------------------------------------

    doc = "This class controls an arbitrary number of waves and envelopes, determined via a constructor argument.\n\
\n\
Control Change Numbers:\n\
\n\
- Control One = 2\n\
- Control Two = 4\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "FM", "StkInstrument", env->global(), 
                                        FM_ctor, FM_dtor,
                                        FM_tick, FM_pmsg, doc.c_str() ) ) return FALSE;

    // member variable
    // all subclasses of FM must use this offset, as this is where the inherited 
    // functions will look for the object
    // the other option would be to keep SubClass_offset_data, but assign
    // the value to FM_offset_data.  
    // FM_offset_data = type_engine_import_mvar ( env, "int", "@FM_data", FALSE );
    // if( FM_offset_data == CK_INVALID_OFFSET ) goto error;
    FM_offset_data = Instrmnt_offset_data;

    func = make_new_mfun( "float", "lfoDepth", FM_ctrl_modDepth ); //!modulation Depth
    func->add_arg( "float", "value" );
    func->doc = "Set modulation depth, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoDepth", FM_cget_modDepth ); //!modulation Depth
    func->doc = "Get modulation depth, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoSpeed", FM_ctrl_modSpeed ); //!modulation Speed
    func->add_arg( "float", "value" );
    func->doc = "Set modulation speed (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "lfoSpeed", FM_cget_modSpeed ); //!modulation Speed
    func->doc = "Get modulation speed (Hz).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "afterTouch", FM_ctrl_afterTouch ); //!aftertouch
    func->add_arg( "float", "value" );
    func->doc = "Set aftertouch, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "afterTouch", FM_cget_afterTouch ); //!aftertouch
    func->doc = "Get aftertouch, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "controlOne", FM_ctrl_control1 ); //! FM control 1
    func->add_arg( "float", "value" );
    func->doc = "Set control one [instrument specific].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "controlOne", FM_cget_control1 ); //! FM control 1
    func->doc = "Get control one [instrument specific].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "controlTwo", FM_ctrl_control2 ); //! FM control 2
    func->add_arg( "float", "value" );
    func->doc = "Set control two [instrument specific].";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "controlTwo", FM_cget_control2 ); //! FM control 2
    func->doc = "Get control two [instrument specific].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin BeeThree ugen
    //------------------------------------------------------------------------

    doc = " This class implements a simple 4 operator topology, also referred to as algorithm 8 of the TX81Z.\n\
\n\
Algorithm 8 is :\n\
    \n\
    1 --.\n\
    2 -\\|\n\
    +-> Out\n\
    3 -/|\n\
    4 --.\n\
    \n\
Control Change Numbers:\n\
\n\
- Operator 4 (feedback) Gain = 2 (.controlOne)\n\
- Operator 3 Gain = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "BeeThree", "FM", env->global(), 
                        BeeThree_ctor, BeeThree_dtor,
                        BeeThree_tick, BeeThree_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin FMVoices ugen
    //------------------------------------------------------------------------

    doc = "This class implements 3 carriers and a common modulator, also referred to as algorithm 6 of the TX81Z.\n\
\n\
Algorithm 6 is :\n\
    \n\
    ___//->1 -\\\n\
    4-||-->2 - +-> Out\n\
    ___\\->3 -//\n\
    \n\
Control Change Numbers:\n\
\n\
- Vowel = 2 (.controlOne)\n\
- Spectral Tilt = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "FMVoices", "FM", env->global(), 
                        FMVoices_ctor, FMVoices_dtor,
                        FMVoices_tick, FMVoices_pmsg, doc.c_str() ) ) return FALSE;

    func = make_new_mfun( "float", "vowel", FMVoices_ctrl_vowel ); //!select vowel
    func->add_arg( "float", "value" );
    func->doc = "Set vowel, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vowel", FMVoices_cget_vowel ); //!select vowel
    func->doc = "Get vowel, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "spectralTilt", FMVoices_ctrl_spectralTilt ); //! spectral tilt
    func->add_arg( "float", "value" );
    func->doc = "Set spectral tilt, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "spectralTilt", FMVoices_cget_spectralTilt ); //! spectral tilt
    func->doc = "Get spectral tilt, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "adsrTarget", FMVoices_ctrl_adsrTarget ); //!adsr targets
    func->add_arg( "float", "value" );
    func->doc = "Set ADSR targets, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "adsrTarget", FMVoices_cget_adsrTarget ); //!adsr targets
    func->doc = "Get ADSR targets, [0.0-1.0].";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin HevyMetl ugen
    //------------------------------------------------------------------------

    doc = "This class implements 3 cascade operators with feedback modulation, also referred to as algorithm 3 of the TX81Z.\n\
\n\
Algorithm 3 is :\n\
    \n\
    ____4--\\\n\
    3-->2-- + -->1-->Out\n\
    \n\
Control Change Numbers:\n\
\n\
- Total Modulator Index = 2 (.controlOne)\n\
- Modulator Crossfade = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "HevyMetl", "FM", env->global(), 
                        HevyMetl_ctor, HevyMetl_dtor,
                        HevyMetl_tick, HevyMetl_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin PercFlut ugen
    //------------------------------------------------------------------------

    doc = "This class implements algorithm 4 of the TX81Z.\n\
\n\
Algorithm 4 is :\n\
    \n\
    4->3--\\\n\
    ___2-- + -->1-->Out\n\
    \n\
Control Change Numbers:\n\
\n\
- Total Modulator Index = 2 (.controlOne)\n\
- Modulator Crossfade = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "PercFlut", "FM", env->global(), 
                        PercFlut_ctor, PercFlut_dtor,
                        PercFlut_tick, PercFlut_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Rhodey ugen
    //------------------------------------------------------------------------

    doc = " This class implements two simple FM Pairs summed together, also referred to as algorithm 5 of the TX81Z.\n\
\n\
Algorithm 5 is :\n\
    \n\
    4->3--\n\
    + --> Out\n\
    2->1--\n\
    \n\
Control Change Numbers:\n\
\n\
- Modulator Index One = 2 (.controlOne)\n\
- Crossfade of Outputs = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \examples rhodey.ck
    if( !type_engine_import_ugen_begin( env, "Rhodey", "FM", env->global(), 
                        Rhodey_ctor, Rhodey_dtor,
                        Rhodey_tick, Rhodey_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "stk/rhodey.ck");

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin TubeBell ugen
    //------------------------------------------------------------------------
    
    doc = "This class implements two simple FM Pairs summed together, also referred to as algorithm 5 of the TX81Z.\n\
\n\
Algorithm 5 is :\n\
    \n\
    4->3--\n\
    + --> Out\n\
    2->1--\n\
    \n\
Control Change Numbers:\n\
\n\
- Modulator Index One = 2 (.controlOne)\n\
- Crossfade of Outputs = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "TubeBell", "FM", env->global(), 
                        TubeBell_ctor, TubeBell_dtor,
                        TubeBell_tick, TubeBell_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Wurley ugen
    //------------------------------------------------------------------------

    doc = "This class implements two simple FM Pairs summed together, also referred to as algorithm 5 of the TX81Z.\n\
\n\
Algorithm 5 is :\n\
\n\
4->3--\n\
+ --> Out\n\
2->1--\n\
\n\
Control Change Numbers:\n\
\n\
- Modulator Index One = 2 (.controlOne)\n\
- Crossfade of Outputs = 4 (.controlTwo)\n\
- LFO Speed = 11\n\
- LFO Depth = 1\n\
- ADSR 2 & 4 Target = 128\n\
\n\
The basic Chowning/Stanford FM patent expired in 1995, but there exist follow-on patents, mostly assigned to Yamaha. If you are of the type who should worry about this (making money) worry away.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \examples wurley.ck
    if( !type_engine_import_ugen_begin( env, "Wurley", "FM", env->global(), 
                        Wurley_ctor, Wurley_dtor,
                        Wurley_tick, Wurley_pmsg, doc.c_str() ) ) return FALSE;

    type_engine_import_add_ex(env, "stk/wurley.ck");
    type_engine_import_add_ex(env, "stk/wurley2.ck");
    type_engine_import_add_ex(env, "stk/wurley3.ck");

    // end the class import
    type_engine_import_class_end( env );

    //end FM
    
    //! \section stk - delay

    //------------------------------------------------------------------------
    // begin Delay ugen
    //------------------------------------------------------------------------

    doc = "This protected Filter subclass implements a non-interpolating digital delay-line. A fixed maximum length of 4095 and a delay of zero is set using the default constructor. Alternatively, the delay and maximum length can be set during instantiation with an overloaded constructor.\n\
\n\
A non-interpolating delay line is typically used in fixed delay-length applications, such as for reverberation.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example net_relay.ck
    if( !type_engine_import_ugen_begin( env, "Delay", "UGen", env->global(), 
                        Delay_ctor, Delay_dtor,
                        Delay_tick, Delay_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    Delay_offset_data = type_engine_import_mvar ( env, "int", "@Delay_data", FALSE );
    if( Delay_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "dur", "delay", Delay_ctrl_delay ); //! length of delay
    func->add_arg( "dur", "value" );
    func->doc = "Set length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "delay", Delay_cget_delay ); //! length of delay
    func->doc = "Get length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", Delay_ctrl_max ); //! max delay (buffer size) 
    func->add_arg( "dur", "value" );
    func->doc = "Set max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", Delay_cget_max ); //! max delay (buffer size)
    func->doc = "Get max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "clear", Delay_clear ); //! clear delay
    func->doc = "Clear delay line.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // delay 'subs' not actually extending .
    
    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin DelayA ugen
    //------------------------------------------------------------------------

    doc = "This Delay subclass implements a fractional-length digital delay-line using a first-order allpass filter. A fixed maximum length of 4095 and a delay of 0.5 is set using the default constructor.  Alternatively, the delay and maximum length can be set during instantiation with an overloaded constructor.\n\
\n\
An allpass filter has unity magnitude gain but variable phase delay properties, making it useful in achieving fractional delays without affecting a signal's frequency magnitude response. In order to achieve a maximally flat phase delay response, the minimum delay possible in this implementation is limited to a value of 0.5.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "DelayA", "UGen", env->global(), 
                        DelayA_ctor, DelayA_dtor,
                        DelayA_tick, DelayA_pmsg, doc.c_str() ) ) return FALSE;
    //member variable
    DelayA_offset_data = type_engine_import_mvar ( env, "int", "@DelayA_data", FALSE );
    if( DelayA_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "dur", "delay", DelayA_ctrl_delay ); //! length of delay
    func->add_arg( "dur", "value" );
    func->doc = "Set length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "delay", DelayA_cget_delay ); //! length of delay
    func->doc = "Get length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", DelayA_ctrl_max ); //! max delay ( buffer size ) 
    func->add_arg( "dur", "value" );
    func->doc = "Set max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", DelayA_cget_max ); //! max delay ( buffer size )
    func->doc = "Get max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "clear", DelayA_clear ); //! clear delay
    func->doc = "Clear delay line.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin DelayL ugen
    //------------------------------------------------------------------------

    doc = "This Delay subclass implements a fractional-length digital delay-line using first-order linear interpolation. A fixed maximum length of 4095 and a delay of zero is set using the default constructor. Alternatively, the delay and maximum length can be set during instantiation with an overloaded constructor.\n\
\n\
Linear interpolation is an efficient technique for achieving fractional delay lengths, though it does introduce high-frequency signal attenuation to varying degrees depending on the fractional delay setting. The use of higher order Lagrange interpolators can typically improve (minimize) this attenuation characteristic.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example i-robot.ck
    if( !type_engine_import_ugen_begin( env, "DelayL", "UGen", env->global(), 
                        DelayL_ctor, DelayL_dtor,
                        DelayL_tick, DelayL_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "basic/delay.ck");
    type_engine_import_add_ex(env, "basic/i-robot.ck");
    
    //member variable
    DelayL_offset_data = type_engine_import_mvar ( env, "int", "@DelayL_data", FALSE );
    if( DelayL_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "dur", "delay", DelayL_ctrl_delay ); //! length of delay
    func->add_arg( "dur", "value" );
    func->doc = "Set length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "delay", DelayL_cget_delay ); //! length of delay
    func->doc = "Get length of delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", DelayL_ctrl_max ); //! max delay ( buffer size ) 
    func->add_arg( "dur", "value" );
    func->doc = "Set max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", DelayL_cget_max ); //! max delay ( buffer size )
    func->doc = "Get max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "clear", DelayL_clear ); //! clear delay
    func->doc = "Clear delay line.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Echo ugen
    //------------------------------------------------------------------------

    doc = "This class implements a echo effect.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Echo", "UGen", env->global(), 
                        Echo_ctor, Echo_dtor,
                        Echo_tick, Echo_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "basic/echo.ck");

    //member variable
    Echo_offset_data = type_engine_import_mvar ( env, "int", "@Echo_data", FALSE );
    if( Echo_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "dur", "delay", Echo_ctrl_delay ); //! length of echo
    func->add_arg( "dur", "value" );
    func->doc = "Set length of echo.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "delay", Echo_cget_delay ); //! length of echo
    func->doc = "Get length of echo.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", Echo_ctrl_max ); //! max delay
    func->add_arg( "dur", "value" );
    func->doc = "Set max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "max", Echo_cget_max ); //! max delay
    func->doc = "Get max delay (buffer size).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", Echo_ctrl_mix ); //! mix level ( wet/dry ) 
    func->add_arg( "float", "value" );
    func->doc = "Set mix level (wet/dry).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", Echo_cget_mix ); //! mix level ( wet/dry )
    func->doc = "Get mix level (wet/dry).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //! \section stk - envelopes



    //------------------------------------------------------------------------
    // begin Envelope ugen
    //------------------------------------------------------------------------
    doc = "This class implements a simple envelope generator which is capable of ramping to a target value by a specified rate. It also responds to simple `keyOn` and `keyOff` messages, ramping to 1.0 on `keyOn` and to 0.0 on `keyOff`.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example sixty.ck
    if( !type_engine_import_ugen_begin( env, "Envelope", "UGen", env->global(), 
                        Envelope_ctor, Envelope_dtor,
                        Envelope_tick, Envelope_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "basic/envelope.ck");
    
    //member variable
    Envelope_offset_data = type_engine_import_mvar ( env, "int", "@Envelope_data", FALSE );
    if( Envelope_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "int", "keyOn", Envelope_ctrl_keyOn0 ); //! ramp to 1.0
    func->doc = "Get keyOn state.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "keyOn", Envelope_ctrl_keyOn ); //! ramp to 1.0
    func->add_arg( "int", "value" );
    func->doc = "Ramp to 1.0.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "keyOff", Envelope_ctrl_keyOff0 ); //! ramp to 0.0
    func->doc = "Get keyOff state.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "keyOff", Envelope_ctrl_keyOff ); //! ramp to 0.0
    func->add_arg( "int", "value" );
    func->doc = "Ramp to 0.0.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "target", Envelope_ctrl_target ); //! ramp to arbitrary value.
    func->add_arg( "float", "value" );
    func->doc = "Set value to ramp to.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "target", Envelope_cget_target ); //! ramp to arbitrary value.
    func->doc = "Get value to ramp to.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "time", Envelope_ctrl_time ); //! time to reach target
    func->add_arg( "float", "value" );
    func->doc = "Set time to reach target (in seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "time", Envelope_cget_time ); //! time to reach target
    func->doc = "Get time to reach target (in seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "duration", Envelope_ctrl_duration ); //! time to reach target
    func->add_arg( "dur", "value" );
    func->doc = "Set duration to reach target.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "duration", Envelope_cget_duration ); //! time to reach target
    func->doc = "Get duration to reach target.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Envelope_ctrl_rate ); //! attack rate 
    func->add_arg( "float", "value" );
    func->doc = "Set rate of change.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", Envelope_cget_rate ); //! attack rate
    func->doc = "Get rate of change.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "value", Envelope_ctrl_value ); //! set immediate value
    func->add_arg( "float", "value" );
    func->doc = "Set immediate value.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "value", Envelope_cget_value ); //! set immediate value
    func->doc = "Get immediate value.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin ADSR ugen
    //------------------------------------------------------------------------
   
    doc = "This Envelope subclass implements a traditional ADSR (Attack, Decay, Sustain, Release) envelope.  It responds to simple `keyOn` and `keyOff` messages, keeping track of its state.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example adsr.ck
    if( !type_engine_import_ugen_begin( env, "ADSR", "Envelope", env->global(), 
                                        ADSR_ctor, ADSR_dtor,
                                        ADSR_tick, ADSR_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "basic/adsr.ck");

    func = make_new_mfun( "dur", "attackTime", ADSR_ctrl_attackTime ); //! attack time
    func->add_arg( "dur", "value" );
    func->doc = "Set attack time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "attackTime", ADSR_cget_attackTime ); //! attack time
    func->doc = "Get attack time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "attackRate", ADSR_ctrl_attackRate ); //! attack rate
    func->add_arg( "float", "value" );
    func->doc = "Set attack rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "attackRate", ADSR_cget_attackRate ); //! attack rate
    func->doc = "Get attack rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "decayTime", ADSR_ctrl_decayTime ); //! decay time 
    func->add_arg( "dur", "value" );
    func->doc = "Set decay time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "decayTime", ADSR_cget_decayTime ); //! decay time
    func->doc = "Get decay time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "decayRate", ADSR_ctrl_decayRate ); //! decay rate
    func->add_arg( "float", "value" );
    func->doc = "Set decay rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "decayRate", ADSR_cget_decayRate ); //! decay rate
    func->doc = "Get decay rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "sustainLevel", ADSR_ctrl_sustainLevel ); //! sustain level
    func->add_arg( "float", "value" );
    func->doc = "Set sustain level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "sustainLevel", ADSR_cget_sustainLevel ); //! sustain level
    func->doc = "Get sustain level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "releaseTime", ADSR_ctrl_releaseTime ); //! release time 
    func->add_arg( "dur", "value" );
    func->doc = "Set release time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "releaseTime", ADSR_cget_releaseTime ); //! release time
    func->doc = "Get release time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "releaseRate", ADSR_ctrl_releaseRate ); //! release rate
    func->add_arg( "float", "value" );
    func->doc = "Set release rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "releaseRate", ADSR_cget_releaseRate ); //! release rate
    func->doc = "Get release rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "set", ADSR_ctrl_set ); //! set
    func->add_arg( "float", "attackTime" );
    func->add_arg( "float", "decayTime" );
    func->add_arg( "float", "sustainLevel" );
    func->add_arg( "float", "releaseTime" );
    func->doc = "Set attack, decay, sustain, and release all at once (in seconds).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "set", ADSR_ctrl_set2 ); //! set
    func->add_arg( "dur", "attackDuration" );
    func->add_arg( "dur", "decayDuration" );
    func->add_arg( "float", "sustainLevel" );
    func->add_arg( "dur", "releaseDuration" );
    func->doc = "Set attack, decay, sustain, and release all at once.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "state", ADSR_cget_state ); //! attack=0, decay=1 , sustain=2, release=3, done=4
    func->doc = "Get state; attack=0, decay=1, sustain=2, release=3, done=4";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    if( !type_engine_import_svar( env, "int", "ATTACK", TRUE, (t_CKUINT) &ADSR_state_ATTACK) ) goto error;
    if( !type_engine_import_svar( env, "int", "DECAY", TRUE, (t_CKUINT) &ADSR_state_DECAY) ) goto error;
    if( !type_engine_import_svar( env, "int", "SUSTAIN", TRUE, (t_CKUINT) &ADSR_state_SUSTAIN) ) goto error;
    if( !type_engine_import_svar( env, "int", "RELEASE", TRUE, (t_CKUINT) &ADSR_state_RELEASE) ) goto error;
    if( !type_engine_import_svar( env, "int", "DONE", TRUE, (t_CKUINT) &ADSR_state_DONE) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );

    //uhhh we are supposed to have target and value here as well..  d'oh

    //! \section stk - filters
    


    /* -- using native BiQuad --
    //------------------------------------------------------------------------
    // begin BiQuad ugen
    //------------------------------------------------------------------------

    if( !type_engine_import_ugen_begin( env, "BiQuadStk", "UGen", env->global(), 
                        BiQuad_ctor, BiQuad_tick, BiQuad_pmsg ) ) return FALSE;
    //member variable
    BiQuad_offset_data = type_engine_import_mvar ( env, "int", "@BiQuad_data", FALSE );
    if( BiQuad_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "b2", BiQuad_ctrl_b2 ); //! b2 coefficient
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b2", BiQuad_cget_b2 ); //! b2 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", BiQuad_ctrl_b1 ); //! b1 coefficient
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", BiQuad_cget_b1 ); //! b1 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", BiQuad_ctrl_b0 ); //! b0 coefficient
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", BiQuad_cget_b0 ); //! b0 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a2", BiQuad_ctrl_a2 ); //! a2 coefficient
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a2", BiQuad_cget_a2 ); //! a2 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a1", BiQuad_ctrl_a1 ); //! a1 coefficient
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a1", BiQuad_cget_a1 ); //! a1 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a0", BiQuad_cget_a0 ); //! a0 coefficient
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pfreq", BiQuad_ctrl_pfreq );  //! set resonance frequency (poles)
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pfreq", BiQuad_cget_pfreq );  //! get resonance frequency (poles)
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "prad", BiQuad_ctrl_prad ); //! pole radius (less than 1 to be stable)
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "prad", BiQuad_cget_prad ); //! pole radius (less than 1 to be stable)
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "zfreq", BiQuad_ctrl_zfreq ); //! notch frequency
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "zfreq", BiQuad_cget_zfreq ); //! notch frequency
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "zrad", BiQuad_ctrl_zrad ); //! zero radius
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "zrad", BiQuad_cget_zrad ); //! zero radius
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "norm", BiQuad_ctrl_norm ); //! normalization
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "eqzs", BiQuad_ctrl_eqzs ); //! equal gain zeroes
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );
    */


    //------------------------------------------------------------------------
    // begin FilterStk ugen (orginally Filter)
    //------------------------------------------------------------------------

    if( !type_engine_import_ugen_begin( env, "FilterStk", "UGen", env->global(), 
                        FilterStk_ctor, FilterStk_dtor,
                        FilterStk_tick, FilterStk_pmsg ) ) return FALSE;
    // member variable
    FilterStk_offset_data = type_engine_import_mvar ( env, "int", "@FilterStk_data", FALSE );
    if( FilterStk_offset_data == CK_INVALID_OFFSET ) goto error;

    // coefs
    func = make_new_mfun( "string", "coefs", FilterStk_ctrl_coefs );
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin OnePole ugen
    //------------------------------------------------------------------------

    doc = "This protected Filter subclass implements a one-pole digital filter.  A method is provided for setting the pole position along the real axis of the z-plane while maintaining a constant peak filter gain.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "OnePole", "UGen", env->global(),
                        OnePole_ctor, OnePole_dtor,
                        OnePole_tick, OnePole_pmsg, doc.c_str() ) ) return FALSE;
    
    // member variable
    OnePole_offset_data = type_engine_import_mvar ( env, "int", "@OnePole_data", FALSE );
    if( OnePole_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "float", "a1", OnePole_ctrl_a1 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a1", OnePole_cget_a1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", OnePole_ctrl_b0 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", OnePole_cget_b0 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pole", OnePole_ctrl_pole ); //! set pole position along real axis of z-plane
    func->add_arg( "float", "value" );
    func->doc = "Set pole position along real axis of z-plane.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "pole", OnePole_cget_pole ); //! set pole position along real axis of z-plane
    func->doc = "Get pole position along real axis of z-plane.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin TwoPole ugen
    //------------------------------------------------------------------------
    doc = "This protected Filter subclass implements a two-pole digital filter.  A method is provided for creating a resonance in the frequency response while maintaining a nearly constant filter gain.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "TwoPole", "UGen", env->global(), 
                        TwoPole_ctor, TwoPole_dtor,
                        TwoPole_tick, TwoPole_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "shred/powerup.ck");
    
    //member variable
    TwoPole_offset_data = type_engine_import_mvar ( env, "int", "@TwoPole_data", FALSE );
    if( TwoPole_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "a1", TwoPole_ctrl_a1 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a1", TwoPole_cget_a1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a2", TwoPole_ctrl_a2 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a2", TwoPole_cget_a2 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", TwoPole_ctrl_b0 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", TwoPole_cget_b0 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", TwoPole_ctrl_freq ); //! filter resonance frequency
    func->add_arg( "float", "value" );
    func->doc = "Set filter resonance frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", TwoPole_cget_freq ); //! filter resonance frequency
    func->doc = "Get filter resonance frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "radius", TwoPole_ctrl_radius ); //! filter resonance radius
    func->add_arg( "float", "value" );
    func->doc = "Set filter radius resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "radius", TwoPole_cget_radius ); //! filter resonance radius
    func->doc = "Get filter radius resonance.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "norm", TwoPole_ctrl_norm ); //! toggle filter normalization 
    func->add_arg( "int", "value" );
    func->doc = "Set filter normalization";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "norm", TwoPole_cget_norm ); //! toggle filter normalization
    func->doc = "Get filter normalization";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin OneZero ugen
    //------------------------------------------------------------------------
    doc = " This protected Filter subclass implements a one-zero digital filter.  A method is provided for setting the zero position along the real axis of the z-plane while maintaining a constant filter gain.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "OneZero", "UGen", env->global(), 
                        OneZero_ctor, OneZero_dtor,
                        OneZero_tick, OneZero_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    OneZero_offset_data = type_engine_import_mvar ( env, "int", "@OneZero_data", FALSE );
    if( OneZero_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "zero", OneZero_ctrl_zero ); //! set zero position
    func->add_arg( "float", "value" );
    func->doc = "Set zero position";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "zero", OneZero_cget_zero ); //! set zero position
    func->doc = "Get zero position";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", OneZero_ctrl_b0 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", OneZero_cget_b0 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", OneZero_ctrl_b1 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", OneZero_cget_b1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin TwoZero ugen
    //------------------------------------------------------------------------

    doc =  "This protected Filter subclass implements a two-zero digital filter.  A method is provided for creating a 'notch' in the frequency response while maintaining a constant filter gain.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "TwoZero", "UGen", env->global(), 
                        TwoZero_ctor, TwoZero_dtor,
                        TwoZero_tick, TwoZero_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    TwoZero_offset_data = type_engine_import_mvar ( env, "int", "@TwoZero_data", FALSE );
    if( TwoZero_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "b0", TwoZero_ctrl_b0 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", TwoZero_cget_b0 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", TwoZero_ctrl_b1 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", TwoZero_cget_b1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b2", TwoZero_ctrl_b2 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b2", TwoZero_cget_b2 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", TwoZero_ctrl_freq ); //! filter notch frequency
    func->add_arg( "float", "value" );
    func->doc = "Set filter notch frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", TwoZero_cget_freq ); //! filter notch frequency
    func->doc = "Get filter notch frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "radius", TwoZero_ctrl_radius ); //! filter notch radius
    func->add_arg( "float", "value" );
    func->doc = "Set filter notch radius.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "radius", TwoZero_cget_radius ); //! filter notch radius
    func->doc = "Get filter notch radius.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin PoleZero ugen
    //------------------------------------------------------------------------

    doc = "This protected Filter subclass implements a one-pole, one-zero digital filter. A method is provided for creating an allpass filter with a given coefficient. Another method is provided to create a DC blocking filter.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "PoleZero", "UGen", env->global(), 
                        PoleZero_ctor, PoleZero_dtor,
                        PoleZero_tick, PoleZero_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    PoleZero_offset_data = type_engine_import_mvar ( env, "int", "@PoleZero_data", FALSE );
    if( PoleZero_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "a1", PoleZero_ctrl_a1 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "a1", PoleZero_cget_a1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", PoleZero_ctrl_b0 ); //! filter coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b0", PoleZero_cget_b0 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", PoleZero_ctrl_b1 ); //! filter coefficient 
    func->add_arg( "float", "value" );
    func->doc = "Set filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "b1", PoleZero_cget_b1 ); //! filter coefficient
    func->doc = "Get filter coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "blockZero", PoleZero_ctrl_blockZero ); //! DC blocking filter with given pole position
    func->add_arg( "float", "value" );
    func->doc = "Set DC blocking filter with given pole position.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "blockZero", PoleZero_cget_blockZero ); //! DC blocking filter with given pole position
    func->doc = "Get DC blocking filter with given pole position.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "allpass", PoleZero_ctrl_allpass ); //!allpass filter with given coefficient
    func->add_arg( "float", "value" );
    func->doc = "Set allpass filter with given coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "allpass", PoleZero_cget_allpass ); //!allpass filter with given coefficient
    func->doc = "Get allpass filter with given coefficient.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //end Filters

    //! \section stk-reverbs


    //------------------------------------------------------------------------
    // begin JCRev ugen
    //------------------------------------------------------------------------

    doc = "John Chowning's reverberator class. This class is derived from the CLM NRev function, which is based on the use of networks of simple allpass and comb delay filters.  This particular arrangement consists of 6 comb filters in parallel, followed by 3 allpass filters, a lowpass filter, and another allpass in series, followed by two allpass filters in parallel with corresponding right and left outputs.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "JCRev", "UGen", env->global(), 
                        JCRev_ctor, JCRev_dtor,
                        JCRev_tick, JCRev_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    JCRev_offset_data = type_engine_import_mvar ( env, "int", "@JCRev_data", FALSE );
    if( JCRev_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "mix", JCRev_ctrl_mix ); //! mix level
    func->add_arg( "float", "value" );
    func->doc = "Set mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", JCRev_cget_mix ); //! mix level
    func->doc = "Get mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin NRev ugen
    //------------------------------------------------------------------------

    doc =  "CCRMA's NRev reverberator class. This class is derived from the CLM NRev function, which is based on the use of networks of simple allpass and comb delay filters.  This particular arrangement consists of 6 comb filters in parallel, followed by 3 allpass filters, a lowpass filter, and another allpass in series, followed by two allpass filters in parallel with corresponding rightand left outputs.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "NRev", "UGen", env->global(), 
                        NRev_ctor, NRev_dtor,
                        NRev_tick, NRev_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    NRev_offset_data = type_engine_import_mvar ( env, "int", "@NRev_data", FALSE );
    if( NRev_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "mix", NRev_ctrl_mix ); // set effect mix
    func->add_arg( "float", "value" );
    func->doc = "Set mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", NRev_cget_mix ); // set effect mix
    func->doc = "Get mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin PRCRev ugen
    //------------------------------------------------------------------------

    doc =  "Perry's simple reverberator class. This class is based on some of the famous Stanford/CCRMA reverbs (NRev, KipRev), which were based on the Chowning/Moorer/Schroeder reverberators using networks of simple allpass and comb delay filters.  This class implements two series allpass units and two parallel comb filters.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "PRCRev", "UGen", env->global(), 
                        PRCRev_ctor, PRCRev_dtor,
                        PRCRev_tick, PRCRev_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    PRCRev_offset_data = type_engine_import_mvar ( env, "int", "@PRCRev_data", FALSE );
    if( PRCRev_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "mix", PRCRev_ctrl_mix ); //! mix level
    func->add_arg( "float", "value" );
    func->doc = "Set mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", PRCRev_cget_mix ); //! mix level
    func->doc = "Get mix level.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //! \section components

    //------------------------------------------------------------------------
    // begin Chorus ugen
    //------------------------------------------------------------------------

    doc = "Implements a chorus effect.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Chorus", "UGen", env->global(), 
                        Chorus_ctor, Chorus_dtor,
                        Chorus_tick, Chorus_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    Chorus_offset_data = type_engine_import_mvar ( env, "int", "@Chorus_data", FALSE );
    if( Chorus_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "modFreq", Chorus_ctrl_modFreq ); //! modulation frequency
    func->add_arg( "float", "value" );
    func->doc = "Set modulation frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modFreq", Chorus_cget_modFreq ); //! modulation frequency
    func->doc = "Get modulation frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modDepth", Chorus_ctrl_modDepth ); //! modulation depth
    func->add_arg( "float", "value" );
    func->doc = "Set modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "modDepth", Chorus_cget_modDepth ); //! modulation depth
    func->doc = "Get modulation depth.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "baseDelay", Chorus_ctrl_baseDelay ); //! base delay
    func->add_arg( "dur", "value" );
    func->doc = "Set base delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "dur", "baseDelay", Chorus_cget_baseDelay ); //! base delay
    func->doc = "Get base delay.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", Chorus_ctrl_mix ); //! effect mix
    func->add_arg( "float", "value" );
    func->doc = "Set effect mix.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", Chorus_cget_mix ); //! effect mix
    func->doc = "Get effect mix.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "void", "max", Chorus_ctrl_set );
    func->add_arg( "dur", "baseDelay" );
    func->add_arg( "float", "modDepth" );
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin Modulate ugen
    //------------------------------------------------------------------------

    doc = "This class combines random and periodic modulations to give a nice, natural human modulation function.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "Modulate", "UGen", env->global(), 
                        Modulate_ctor, Modulate_dtor,
                        Modulate_tick, Modulate_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    Modulate_offset_data = type_engine_import_mvar ( env, "int", "@Modulate_data", FALSE );
    if( Modulate_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "vibratoRate", Modulate_ctrl_vibratoRate );  //! set rate of vibrato
    func->add_arg( "float", "value" );
    func->doc = "Set rate for vibrato.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoRate", Modulate_cget_vibratoRate );  //! set rate of vibrato
    func->doc = "Get rate for vibrato.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Modulate_ctrl_vibratoGain ); //! gain for vibrato
    func->add_arg( "float", "value" );
    func->doc = "Set gain for vibrato.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "vibratoGain", Modulate_cget_vibratoGain ); //! gain for vibrato
    func->doc = "Get gain for vibrato.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "randomGain", Modulate_ctrl_randomGain ); //!  gain for random contribution
    func->add_arg( "float", "value" );
    func->doc = "Set gain for random contribution";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "randomGain", Modulate_cget_randomGain ); //!  gain for random contribution
    func->doc = "Get gain for random contribution";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin PitShift ugen
    //------------------------------------------------------------------------

    doc = "This class implements a simple pitch shifter using delay lines.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "PitShift", "UGen", env->global(), 
                        PitShift_ctor, PitShift_dtor,
                        PitShift_tick, PitShift_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    PitShift_offset_data = type_engine_import_mvar ( env, "int", "@PitShift_data", FALSE );
    if( PitShift_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "shift", PitShift_ctrl_shift ); //! degree of pitch shifting
    func->add_arg( "float", "value" );
    func->doc = "Set degree of pitch shifting";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "shift", PitShift_cget_shift ); //! degree of pitch shifting
    func->doc = "Get degree of pitch shifting";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", PitShift_ctrl_effectMix ); //! mix level
    func->add_arg( "float", "value" );
    func->doc = "Set mix level";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "mix", PitShift_cget_effectMix ); //! mix level
    func->doc = "Get mix level";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "effectMix", PitShift_ctrl_effectMix ); //! mix level
    func->doc = "Set effect mix level";
    func->add_arg( "float", "value" );
    
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "effectMix", PitShift_cget_effectMix ); //! mix level
    func->doc = "Get effect mix level";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin SubNoise ugen
    //------------------------------------------------------------------------

    doc = "Generates a new random number every 'rate' ticks using the C `rand()` function. The quality of the r`and()` function varies from one OS to another.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "SubNoise", "UGen", env->global(),
                        SubNoise_ctor, SubNoise_dtor,
                        SubNoise_tick, SubNoise_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    SubNoise_offset_data = type_engine_import_mvar ( env, "int", "@SubNoise_data", FALSE );
    if( SubNoise_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "int", "rate", SubNoise_ctrl_rate ); //! subsampling rate
    func->add_arg( "int", "value" );
    func->doc = "Set subsampling rate";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "rate", SubNoise_cget_rate ); //! subsampling rate
    func->doc = "Get subsampling rate";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //! \section stk - file i/o
    

    //------------------------------------------------------------------------
    // begin WvIn ugen
    //------------------------------------------------------------------------

    doc = "This class provides input support for various audio file formats. It also serves as a base class for 'realtime' streaming subclasses.\n\
\n\
WvIn loads the contents of an audio file for subsequent output. Linear interpolation is used for fractional 'read rates'.\n\
\n\
WvIn supports multi-channel data in interleaved format. It is important to distinguish the `tick()` methods, which return samples produced by averaging across sample frames, from the `tickFrame()` methods, which return pointers to multi-channel sample frames. For single-channel data, these methods return equivalent values. Small files are completely read into local memory during instantiation. Large files are read incrementally from disk. The file size threshold and the increment size values are defined in WvIn.h.\n\
\n\
WvIn currently supports WAV, AIFF, SND (AU), MAT-file (Matlab), and STK RAW file formats. Signed integer (8-, 16-, and 32-bit) and floating-point (32- and 64-bit) data types are supported. Uncompressed data types are not supported. If using MAT-files, data should be saved in an array with each data channel filling a matrix row.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "WvIn", "UGen", env->global(), 
                        WvIn_ctor, WvIn_dtor,
                        WvIn_tick, WvIn_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    WvIn_offset_data = type_engine_import_mvar ( env, "int", "@WvIn_data", FALSE );
    if( WvIn_offset_data == CK_INVALID_OFFSET ) goto error;
    func = make_new_mfun( "float", "rate", WvIn_ctrl_rate ); //! playback rate
    func->add_arg( "float", "value" );
    func->doc = "Set playback rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "rate", WvIn_cget_rate ); //! playback rate
    func->doc = "Get playback rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "path", WvIn_ctrl_path ); //! specifies file to be played
    func->add_arg( "string", "value" );
    func->doc = "Set file to be played.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "path", WvIn_cget_path ); //! specifies file to be played
    func->doc = "Get file to be played.";
    if( !type_engine_import_mfun( env, func ) ) goto error;


    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin WaveLoop ugen
    //------------------------------------------------------------------------

    doc = "This class inherits from WvIn and provides audio file looping functionality.\n\
\n\
WaveLoop supports multi-channel data in interleaved format. It is important to distinguish the `tick()` methods, which return samples produced by averaging across sample frames, from the `tickFrame()` methods, which return pointers to multi-channel sample frames. For single-channel data, these methods return equivalent values.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    //! see \example dope.ck
    if( !type_engine_import_ugen_begin( env, "WaveLoop", "WvIn", env->global(), 
                        WaveLoop_ctor, WaveLoop_dtor,
                        WaveLoop_tick, WaveLoop_pmsg, doc.c_str() ) ) return FALSE;
    
    func = make_new_mfun( "float", "freq", WaveLoop_ctrl_freq ); //! set frequency of playback ( loops / second )
    func->add_arg( "float", "value" );
    func->doc = "Set frequency of playback (loops/second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", WaveLoop_cget_freq ); //! set frequency of playback ( loops / second )
    func->doc = "Get frequency of playback (loops/second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "addPhase", WaveLoop_ctrl_phase ); //! offset by phase
    func->add_arg( "float", "value" );
    func->doc = "Set offset by phase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "addPhase", WaveLoop_cget_phase ); //! offset by phase
    func->doc = "Get offset by phase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "addPhaseOffset", WaveLoop_ctrl_phaseOffset ); //! set phase offset
    func->add_arg( "float", "value" );
    func->doc = "Set phase offset.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "addPhaseOffset", WaveLoop_cget_phaseOffset ); //! set phase offset
    func->doc = "Get phase offset.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin WvOut ugen
    //------------------------------------------------------------------------
 
    doc = "This class provides output support for various audio file formats. It also serves as a base class for 'realtime' streaming subclasses.\n\
\n\
WvOut writes samples to an audio file. It supports multi-channel data in interleaved format.  It is important to distinguish the `tick()` methods, which output single samples to all channels in a sample frame, from the `tickFrame()` method, which takes a pointer to multi-channel sample frame data.\n\
\n\
WvOut currently supports WAV, AIFF, AIFC, SND (AU), MAT-file (Matlab), and STK RAW file formats.  Signed integer (8-, 16-, and 32-bit) and floating- point (32- and 64-bit) data types are supported.  STK RAW files use 16-bit integers by definition.  MAT-files will always be written as 64-bit floats.  If a data type specification does not match the specified file type, the data type will automatically be modified.  Uncompressed data types are not supported.\n\
\n\
Currently, WvOut is non-interpolating and the output rate is always `Stk::sampleRate()`.\n\
\n\
by Perry R. Cook and Gary P. Scavone, 1995 - 2002.";
    
    if( !type_engine_import_ugen_begin( env, "WvOut", "UGen", env->global(), 
                        WvOut_ctor, WvOut_dtor,
                        WvOut_tick, WvOut_pmsg, doc.c_str() ) ) return FALSE;
    
    //member variable
    WvOut_offset_data = type_engine_import_mvar ( env, "int", "@WvOut_data", FALSE );
    if( WvOut_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "string", "matFilename", WvOut_ctrl_matFilename ); //!open matlab file for writing
    func->add_arg( "string", "value" );
    func->doc = "Open MatLab file for writing.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "sndFilename", WvOut_ctrl_sndFilename ); //!open snd file for writing
    func->add_arg( "string", "value" );
    func->doc = "Open SND file for writing.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "wavFilename", WvOut_ctrl_wavFilename ); //!open WAVE file for writing
    func->add_arg( "string", "value" );
    func->doc = "Open WAVE file for writing.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "rawFilename", WvOut_ctrl_rawFilename ); //!open raw file for writing
    func->add_arg( "string", "value" );
    func->doc = "Open a raw file for writing.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "aifFilename", WvOut_ctrl_aifFilename ); //!open AIFF file for writing
    func->add_arg( "string", "value" );
    func->doc = "Open an AIFF file for writing.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "filename", WvOut_cget_filename ); //!get filename
    func->doc = "Get filename.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "record", WvOut_ctrl_record ); // !start/stop output
    func->add_arg( "int", "value" );
    func->doc = "Start/stop output.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "record", WvOut_cget_record ); // !start/stop output
    func->doc = "Start/stop output.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "closeFile", WvOut_ctrl_closeFile ); //! close file properly
    func->add_arg( "string", "value" );
    func->doc = "Close the file properly.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "closeFile", WvOut_ctrl_closeFile ); //! close file properly
    func->doc = "Close the file properly.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "autoPrefix", WvOut_ctrl_autoPrefix ); //! set/get auto prefix string
    func->add_arg( "string", "value" );
    func->doc = "Set auto prefix string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "string", "autoPrefix", WvOut_cget_autoPrefix ); //! set/get auto prefix string
    func->doc = "Get auto prefix string.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "fileGain", WvOut_ctrl_fileGain ); //! set/get auto prefix string
    func->add_arg( "float", "value" );
    func->doc = "Set file gain.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "fileGain", WvOut_cget_fileGain ); //! set/get auto prefix string
    func->doc = "Get file gain.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    
    
    if( !type_engine_import_ugen_begin( env, "WvOut2", "WvOut", env->global(), 
                                        NULL, NULL,
                                        NULL, WvOut2_tickf, WvOut_pmsg, 2, 2 ) ) return FALSE;

    func = make_new_mfun( "string", "matFilename", WvOut2_ctrl_matFilename ); //!open matlab file for writing
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "string", "sndFilename", WvOut2_ctrl_sndFilename ); //!open snd file for writing
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "string", "wavFilename", WvOut2_ctrl_wavFilename ); //!open WAVE file for writing
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "string", "rawFilename", WvOut2_ctrl_rawFilename ); //!open raw file for writing
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "string", "aifFilename", WvOut2_ctrl_aifFilename ); //!open AIFF file for writing
    func->add_arg( "string", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );
    

    //------------------------------------------------------------------------
    // begin BLT (BandLtd)
    //------------------------------------------------------------------------

    doc = "Super-class for band-limited oscillators.";
    
    if( !type_engine_import_ugen_begin( env, "BLT", "UGen", env->global(), 
         BLT_ctor, BLT_dtor,
         BLT_tick, BLT_pmsg, doc.c_str()) ) return FALSE;

    // member variable
    // all subclasses of BLT must use this offset, as this is where the 
    // inherited functions will look for the object
    // the other option would be to keep SubClass_offset_data, but assign
    // the value to BLT_offset_data.  
    BLT_offset_data = type_engine_import_mvar( env, "int", "@BLT_data", FALSE );
    if( BLT_offset_data == CK_INVALID_OFFSET ) goto error;

    func = make_new_mfun( "float", "phase", BLT_ctrl_phase ); //!set phase
    func->add_arg( "float", "value" );
    func->doc = "Set phase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "phase", BLT_cget_phase ); //!get phase
    func->doc = "Get phase.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", BLT_ctrl_freq ); //!set freq
    func->add_arg( "float", "value" );
    func->doc = "Set frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "freq", BLT_cget_freq ); //!get freq
    func->doc = "Set frequency.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "harmonics", BLT_ctrl_harmonics ); //!set harmonics
    func->add_arg( "int", "value" );
    func->doc = "Set harmonics.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "harmonics", BLT_cget_harmonics ); //!get harmonics
    func->doc = "Get harmonics.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    

    //------------------------------------------------------------------------
    // begin Blit
    //------------------------------------------------------------------------

    doc = "This class generates a band-limited impulse train using a closed-form algorithm reported by Stilson and Smith in 'Alias-Free Digital Synthesis of Classic Analog Waveforms', 1996. The user can specify both the fundamental frequency of the impulse train and the number of harmonics contained in the resulting signal.\n\
\n\
The signal is normalized so that the peak value is +/-1.0.\n\
\n\
If `nHarmonics` is 0, then the signal will contain all harmonics up to half the sample rate. Note, however, that this setting may produce aliasing in the signal when the frequency is changing (no automatic modification of the number of harmonics is performed by the `setFrequency()` function).\n\
\n\
Original code by Robin Davies, 2005.\n\
Revisions by Gary Scavone for STK, 2005.";
    
    if( !type_engine_import_ugen_begin( env, "Blit", "BLT", env->global(), 
         Blit_ctor, Blit_dtor, Blit_tick, Blit_pmsg, doc.c_str() ) ) return FALSE;
    
    type_engine_import_add_ex(env, "basic/blit.ck");

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin BlitSaw
    //------------------------------------------------------------------------

    doc = "This class generates a band-limited sawtooth waveform using a closed-form algorithm reported by Stilson and Smith in 'Alias-Free Digital Synthesis of Classic Analog Waveforms', 1996. The user can specify both the fundamental frequency of the sawtooth and the number of harmonics contained in the resulting signal.\n\
\n\
If `nHarmonics` is 0, then the signal will contain all harmonics up to half the sample rate. Note, however, that this setting may produce aliasing in the signal when the frequency is changing (no automatic modification of the number of harmonics is performed by the `setFrequency()` function).\n\
\n\
Based on initial code of Robin Davies, 2005.\n\
Modified algorithm code by Gary Scavone, 2005.";
    
    if( !type_engine_import_ugen_begin( env, "BlitSaw", "BLT", env->global(), 
         BlitSaw_ctor, BlitSaw_dtor, BlitSaw_tick, BlitSaw_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin BlitSquare
    //------------------------------------------------------------------------

    doc = "This class generates a band-limited square wave signal. It is derived in part from the approach reported by Stilson and Smith in 'Alias-Free Digital Synthesis of Classic Analog Waveforms', 1996. The algorithm implemented in this class uses a SincM function with an even M value to achieve a bipolar bandlimited impulse train. This signal is then integrated to achieve a square waveform. The integration process has an associated DC offset but that is subtracted off the output signal.\n\
\n\
The user can specify both the fundamental frequency of the waveform and the number of harmonics contained in the resulting signal.\n\
\n\
If `nHarmonics` is 0, then the signal will contain all harmonics up to half the sample rate. Note, however, that this setting may produce aliasing in the signal when the frequency is changing (no automatic modification of the number of harmonics is performed by the `setFrequency()` function).\n\
\n\
Based on initial code of Robin Davies, 2005.\n\
Modified algorithm code by Gary Scavone, 2005.";
    
    if( !type_engine_import_ugen_begin( env, "BlitSquare", "BLT", env->global(), 
         BlitSquare_ctor, BlitSquare_dtor,
         BlitSquare_tick, BlitSquare_pmsg, doc.c_str() ) ) return FALSE;

    // end the class import
    type_engine_import_class_end( env );


    //------------------------------------------------------------------------
    // begin JetTable
    //------------------------------------------------------------------------

    if( !type_engine_import_ugen_begin( env, "JetTabl", "UGen", env->global(), 
         JetTabl_ctor, JetTabl_dtor, JetTabl_tick, JetTabl_pmsg ) ) return FALSE;

    // member variable
    JetTabl_offset_data = type_engine_import_mvar( env, "int", "@JetTabl_data", FALSE );
    if( JetTabl_offset_data == CK_INVALID_OFFSET ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    
    
    // Mesh2D 
    if( !type_engine_import_ugen_begin( env, "Mesh2D", "UGen", env->global(), 
                                        Mesh2D_ctor, Mesh2D_dtor,
                                        Mesh2D_tick, Mesh2D_pmsg ) ) return FALSE;
    //member variable
    Mesh2D_offset_data = type_engine_import_mvar ( env, "int", "@Mesh2D_data", FALSE );
    if( Mesh2D_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "int", "nx", Mesh2D_ctrl_nx ); //! nx
    func->add_arg( "int", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "int", "nx", Mesh2D_cget_nx ); //! nx
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "ny", Mesh2D_ctrl_ny ); //! nx
    func->add_arg( "int", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "ny", Mesh2D_cget_ny ); //! nx
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "inputPosition", Mesh2D_ctrl_input_position ); //! nx
    func->add_arg( "float", "xval" );
    func->add_arg( "float", "yval" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "decay", Mesh2D_ctrl_decay ); //! nx
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;   

    func = make_new_mfun( "float", "energy", Mesh2D_cget_energy ); //! nx
    if( !type_engine_import_mfun( env, func ) ) goto error;   
    
    func = make_new_mfun( "float", "noteOn", Mesh2D_ctrl_note_on ); //! start 
    func->add_arg( "float", "note" );
    func->add_arg( "float", "vel" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "float", "noteOff", Mesh2D_ctrl_note_off ); //! stop
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "controlChange", Mesh2D_ctrl_control_change ); //! select instrument
    func->add_arg( "int", "ctrl" );
    func->add_arg( "float", "value" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
        
    // end the class import
    type_engine_import_class_end( env );
    
    
    if(!type_engine_import_class_begin( env, "MidiFileIn", "Object", env->global(), MidiFileIn_ctor, MidiFileIn_dtor ))
        return FALSE;
    
    MidiFileIn_offset_data = type_engine_import_mvar ( env, "int", "@MidiFileIn_data", FALSE );
    if( MidiFileIn_offset_data == CK_INVALID_OFFSET ) goto error;
    
    func = make_new_mfun( "int", "open", MidiFileIn_open );
    func->add_arg( "string", "path" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "void", "close", MidiFileIn_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "read", MidiFileIn_read );
    func->add_arg( "MidiMsg", "msg" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "read", MidiFileIn_readTrack );
    func->add_arg( "MidiMsg", "msg" );
    func->add_arg( "int", "track" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "int", "numTracks", MidiFileIn_numTracks );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "void", "rewind", MidiFileIn_rewind );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // end the class import
    type_engine_import_class_end( env );

    
    return TRUE;

//error recovery - end class and throw flag...
error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;

}




struct SKINISpec { char messageString[32];
                   long  type;
                   long data2;
                   long data3;
                 };

/*   SEE COMMENT BLOCK AT BOTTOM FOR FIELDS AND USES   */
/* MessageString     ,type,  ch?,        data2        ,             data3 */ 

struct SKINISpec skini_msgs[__SK_MaxMsgTypes_] = 
{
 {"NoteOff"          ,        __SK_NoteOff_,               SK_DBL,  SK_DBL},
 {"NoteOn"           ,         __SK_NoteOn_,               SK_DBL,  SK_DBL},
 {"PolyPressure"     ,   __SK_PolyPressure_,               SK_DBL,  SK_DBL},
 {"ControlChange"    ,  __SK_ControlChange_,               SK_INT,  SK_DBL},
 {"ProgramChange"    ,  __SK_ProgramChange_,               SK_DBL,  SK_DBL},
 {"AfterTouch"       ,     __SK_AfterTouch_,               SK_DBL,    NOPE},
 {"ChannelPressure"  ,__SK_ChannelPressure_,               SK_DBL,    NOPE},
 {"PitchWheel"       ,     __SK_PitchWheel_,               SK_DBL,    NOPE},
 {"PitchBend"        ,      __SK_PitchBend_,               SK_DBL,    NOPE},
 {"PitchChange"      ,    __SK_PitchChange_,               SK_DBL,    NOPE},
                                                                 
 {"Clock"            ,          __SK_Clock_,                 NOPE,    NOPE},
 {"Undefined"        ,                  249,                 NOPE,    NOPE},
 {"SongStart"        ,      __SK_SongStart_,                 NOPE,    NOPE},
 {"Continue"         ,       __SK_Continue_,                 NOPE,    NOPE},
 {"SongStop"         ,       __SK_SongStop_,                 NOPE,    NOPE},
 {"Undefined"        ,                  253,                 NOPE,    NOPE},
 {"ActiveSensing"    ,  __SK_ActiveSensing_,                 NOPE,    NOPE},
 {"SystemReset"      ,    __SK_SystemReset_,                 NOPE,    NOPE},
    
 {"Volume"           ,  __SK_ControlChange_, __SK_Volume_        ,  SK_DBL},
 {"ModWheel"         ,  __SK_ControlChange_, __SK_ModWheel_      ,  SK_DBL},
 {"Modulation"       ,  __SK_ControlChange_, __SK_Modulation_    ,  SK_DBL},
 {"Breath"           ,  __SK_ControlChange_, __SK_Breath_        ,  SK_DBL},
 {"FootControl"      ,  __SK_ControlChange_, __SK_FootControl_   ,  SK_DBL},
 {"Portamento"       ,  __SK_ControlChange_, __SK_Portamento_    ,  SK_DBL},
 {"Balance"          ,  __SK_ControlChange_, __SK_Balance_       ,  SK_DBL},
 {"Pan"              ,  __SK_ControlChange_, __SK_Pan_           ,  SK_DBL},
 {"Sustain"          ,  __SK_ControlChange_, __SK_Sustain_       ,  SK_DBL},
 {"Damper"           ,  __SK_ControlChange_, __SK_Damper_        ,  SK_DBL},
 {"Expression"       ,  __SK_ControlChange_, __SK_Expression_    ,  SK_DBL},
                                  
 {"NoiseLevel"       ,  __SK_ControlChange_, __SK_NoiseLevel_    ,  SK_DBL},
 {"PickPosition"     ,  __SK_ControlChange_, __SK_PickPosition_  ,  SK_DBL},
 {"StringDamping"    ,  __SK_ControlChange_, __SK_StringDamping_ ,  SK_DBL},
 {"StringDetune"     ,  __SK_ControlChange_, __SK_StringDetune_  ,  SK_DBL},
 {"BodySize"         ,  __SK_ControlChange_, __SK_BodySize_      ,  SK_DBL},
 {"BowPressure"      ,  __SK_ControlChange_, __SK_BowPressure_   ,  SK_DBL},
 {"BowPosition"      ,  __SK_ControlChange_, __SK_BowPosition_   ,  SK_DBL},
 {"BowBeta"          ,  __SK_ControlChange_, __SK_BowBeta_       ,  SK_DBL},
 
 {"ReedStiffness"    ,  __SK_ControlChange_, __SK_ReedStiffness_ ,  SK_DBL},
 {"ReedRestPos"      ,  __SK_ControlChange_, __SK_ReedRestPos_   ,  SK_DBL},
 {"FluteEmbouchure"  ,  __SK_ControlChange_, __SK_FluteEmbouchure_, SK_DBL},
 {"LipTension"       ,  __SK_ControlChange_, __SK_LipTension_    ,  SK_DBL},
 {"StrikePosition"   ,  __SK_ControlChange_, __SK_StrikePosition_,  SK_DBL},
 {"StickHardness"    ,  __SK_ControlChange_, __SK_StickHardness_ ,  SK_DBL},

 {"TrillDepth"       ,  __SK_ControlChange_, __SK_TrillDepth_    ,  SK_DBL}, 
 {"TrillSpeed"       ,  __SK_ControlChange_, __SK_TrillSpeed_    ,  SK_DBL},
                                             
 {"Strumming"        ,  __SK_ControlChange_, __SK_Strumming_     ,  127   }, 
 {"NotStrumming"     ,  __SK_ControlChange_, __SK_Strumming_     ,  0     },
                                             
 {"PlayerSkill"      ,  __SK_ControlChange_, __SK_PlayerSkill_   ,  SK_DBL}, 

 {"Chord"            ,  __SK_Chord_    ,        SK_DBL   , SK_STR }, 
 {"ChordOff"         ,  __SK_ChordOff_     ,        SK_DBL   ,  NOPE  }, 

 {"ShakerInst"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,  SK_DBL},
 {"Maraca"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,   0    },
 {"Sekere"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,   1    },
 {"Cabasa"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,   2    },
 {"Bamboo"       ,  __SK_ControlChange_, __SK_ShakerInst_   ,   3    },
 {"Waterdrp"         ,  __SK_ControlChange_, __SK_ShakerInst_   ,   4    },
 {"Tambourn"         ,  __SK_ControlChange_, __SK_ShakerInst_   ,   5    },
 {"Sleighbl"         ,  __SK_ControlChange_, __SK_ShakerInst_   ,   6    },
 {"Guiro"        ,  __SK_ControlChange_, __SK_ShakerInst_   ,   7    }, 

 {"OpenFile"         ,                  256,         SK_STR      ,    NOPE},
 {"SetPath"          ,                  257,         SK_STR      ,    NOPE},

 {"FilePath"         ,  __SK_SINGER_FilePath_,           SK_STR   ,    NOPE},
 {"Frequency"        ,  __SK_SINGER_Frequency_,          SK_STR   ,    NOPE},
 {"NoteName"         ,  __SK_SINGER_NoteName_,           SK_STR   ,    NOPE},
 {"VocalShape"       ,  __SK_SINGER_Shape_   ,           SK_STR   ,    NOPE},
 {"Glottis"          ,  __SK_SINGER_Glot_    ,           SK_STR   ,    NOPE},
 {"VoicedUnVoiced"   ,  __SK_SINGER_VoicedUnVoiced_,     SK_DBL   ,  SK_STR},
 {"Synthesize"       ,  __SK_SINGER_Synthesize_,         SK_STR   ,    NOPE},
 {"Silence"          ,  __SK_SINGER_Silence_,            SK_STR   ,    NOPE},
 {"VibratoAmt"       ,  __SK_ControlChange_  ,__SK_SINGER_VibratoAmt_,SK_DBL},
 {"RndVibAmt"        ,  __SK_SINGER_RndVibAmt_          ,SK_STR,       NOPE},
 {"VibFreq"          ,  __SK_ControlChange_  ,__SK_SINGER_VibFreq_   ,SK_DBL}
};


/**  FORMAT: *************************************************************/
/*                                                                       */
/* MessageStr$      ,type, data2, data3,                                 */
/*                                                                       */
/*     type is the message type sent back from the SKINI line parser.    */
/*     data<n> is either                                                 */
/*          NOPE    : field not used, specifically, there aren't going   */                                           
/*                    to be any more fields on this line.  So if there   */
/*                    is is NOPE in data2, data3 won't even be checked   */
/*          SK_INT  : byte (actually scanned as 32 bit signed integer)   */
/*                      If it's a MIDI data field which is required to   */
/*                      be an integer, like a controller number, it's    */
/*                      0-127.  Otherwise) get creative with SK_INTs     */
/*          SK_DBL  : double precision floating point.  SKINI uses these */
/*                    in the MIDI context for note numbers with micro    */
/*                    tuning, velocities, controller values, etc.        */
/*          SK_STR  : only valid in final field.  This allows (nearly)   */
/*                    arbitrary message types to be supported by simply  */
/*                    scanning the string to EndOfLine and then passing  */
/*                    it to a more intellegent handler.  For example,    */
/*                    MIDI SYSEX (system exclusive) messages of up to    */
/*                    256bytes can be read as space-delimited integers   */
/*                    into the 1K SK_STR buffer.  Longer bulk dumps,     */
/*                    soundfiles, etc. should be handled as a new        */
/*                    message type pointing to a FileName stored in the  */
/*                    SK_STR field, or as a new type of multi-line       */                                      
/*                    message.                                           */
/*                                                                       */
/*************************************************************************/


/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/
#include <stdio.h>
#include "ugen_stk.h"

ADSR :: ADSR() : Envelope()
{
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  attackRate = (MY_FLOAT) 0.001;
  decayRate = (MY_FLOAT) 0.001;
  sustainLevel = (MY_FLOAT) 0.5;
  releaseRate = (MY_FLOAT) 0.01;
  m_decayTime = (MY_FLOAT) -1.0; // not used
  m_releaseTime = (MY_FLOAT) -1.0; // not used
  state = DONE;
}

ADSR :: ~ADSR()
{
}

void ADSR :: keyOn()
{
  target = (MY_FLOAT) 1.0;
  rate = attackRate;
  state = ATTACK;
}

void ADSR :: keyOff()
{
  // chuck
  if( m_releaseTime > 0 )
  {
      // in case release triggered before sustain
      rate = value / (m_releaseTime * Stk::sampleRate());
  }
  else
  {
      // rate was set
      rate = releaseRate;
  }

  target = (MY_FLOAT) 0.0;
  state = RELEASE;
}

void ADSR :: setAttackRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    attackRate = -aRate;
  }
  else attackRate = aRate;
}

void ADSR :: setDecayRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    decayRate = -aRate;
  }
  else decayRate = aRate;

  // chuck
  m_decayTime = -1.0;
}

void ADSR :: setSustainLevel(MY_FLOAT aLevel)
{
  if (aLevel < 0.0 ) {
    printf("[chuck](via ADSR): sustain level out of range ... correcting!\n");
    sustainLevel = (MY_FLOAT) 0.0;
  }
  else sustainLevel = aLevel;

  // chuck: need to recompute decay and release rates
  if( m_decayTime > 0.0 ) setDecayTime( m_decayTime );
  if( m_releaseTime > 0.0 ) setReleaseTime( m_releaseTime );
}

void ADSR :: setReleaseRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    releaseRate = -aRate;
  }
  else releaseRate = aRate;

  // chuck
  m_releaseTime = -1.0;
}

void ADSR :: setAttackTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative rates not allowed ... correcting!\n");
    attackRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else attackRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setDecayTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative times not allowed ... correcting!\n");
    // chuck: compute rate for 1.0 to sustain
    decayRate = (1.0 - sustainLevel) / ( -aTime * Stk::sampleRate() );
  }
  else if( aTime == 0.0 ) {
    // printf("[chuck](via ADSR): zero decay time not allowed ... correcting!\n");
    decayRate = FLT_MAX; // a big number
  }
  else decayRate = (1.0 - sustainLevel) / ( aTime * Stk::sampleRate() );

  // chuck
  m_decayTime = aTime;
}

void ADSR :: setReleaseTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via ADSR): negative times not allowed ... correcting!\n");
    releaseRate = sustainLevel / ( -aTime * Stk::sampleRate() );
  }
  else releaseRate = sustainLevel / ( aTime * Stk::sampleRate() );

  // chuck
  m_releaseTime = aTime;
}

// chuck
MY_FLOAT ADSR :: getAttackTime() { return 1.0 / (attackRate*Stk::sampleRate()); }
MY_FLOAT ADSR :: getDecayTime()
{ return (1.0 - sustainLevel) / (decayRate*Stk::sampleRate()); }
MY_FLOAT ADSR :: getReleaseTime()
{ return sustainLevel / (releaseRate*Stk::sampleRate()); }

void ADSR :: setAllTimes(MY_FLOAT aTime, MY_FLOAT dTime, MY_FLOAT sLevel, MY_FLOAT rTime)
{
  this->setAttackTime(aTime);
  this->setDecayTime(dTime);
  this->setSustainLevel(sLevel);
  this->setReleaseTime(rTime);
}

void ADSR :: setTarget(MY_FLOAT aTarget)
{
  target = aTarget;
  if (value < target) {
    state = ATTACK;
    this->setSustainLevel(target);
    rate = attackRate;
  }
  if (value > target) {
    this->setSustainLevel(target);
    state = DECAY;
    rate = decayRate;
  }
}

void ADSR :: setValue(MY_FLOAT aValue)
{
  state = SUSTAIN;
  target = aValue;
  value = aValue;
  this->setSustainLevel(aValue);
  rate = (MY_FLOAT)  0.0;
}

int ADSR :: getState(void) const
{
  return state;
}

MY_FLOAT ADSR :: tick()
{
  switch(state)
  {
  case ATTACK:
    value += rate;
    if (value >= target)
    {
      value = target;
      rate = decayRate;
      target = sustainLevel;
      state = DECAY;

      // TODO: check this
      if( decayRate >= FLT_MAX ) // big number
      {
          // go directly to sustain;
          state = SUSTAIN;
          value = sustainLevel;
          rate = 0.0;
      }
    }
    break;

  case DECAY:
    value -= decayRate;
    if (value <= sustainLevel)
    {
      value = sustainLevel;
      rate = (MY_FLOAT) 0.0;
      state = SUSTAIN;
    }
    break;

  case RELEASE:
    // WAS:
    // value -= releaseRate;

    // chuck
    value -= rate;

    if (value <= 0.0)
    {
      value = (MY_FLOAT) 0.0;
      state = DONE;
    }
  }

  return value;
}

MY_FLOAT *ADSR :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}


/***************************************************/
/*! \class BandedWG
    \brief Banded waveguide modeling class.

    This class uses banded waveguide techniques to
    model a variety of sounds, including bowed
    bars, glasses, and bowls.  For more
    information, see Essl, G. and Cook, P. "Banded
    Waveguides: Towards Physical Modelling of Bar
    Percussion Instruments", Proceedings of the
    1999 International Computer Music Conference.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

    by Georg Essl, 1999 - 2002.
    Modified for Stk 4.0 by Gary Scavone.
*/
/***************************************************/

#include <math.h>

BandedWG :: BandedWG()
{
  doPluck = true;

  delay = new DelayL[MAX_BANDED_MODES];
  bandpass = new BiQuad[MAX_BANDED_MODES];
  
  bowTabl = new BowTabl;
  bowTabl->setSlope( 3.0 );

  adsr = new ADSR;
  adsr->setAllTimes( 0.02, 0.005, 0.9, 0.01 );

  freakency = 220.0;
  setPreset(0);

  bowPosition = 0;
  baseGain = (MY_FLOAT) 0.999;
  
  integrationConstant = 0.0;
  trackVelocity = false;

  bowVelocity = 0.0;
  bowTarget = 0.0;

  strikeAmp = 0.0;

  // chuck
  m_frequency = freakency;
  // rate
  m_rate = .02;
  // reverse: nothing (set directly from norm in controlChange)
  m_bowPressure = 0.0;
  // reverse: bowPosition
  m_bowMotion = bowPosition;
  // reverse: baseGain = 0.8999999999999999 + (0.1 * norm);
  m_modesGain = (baseGain - 0.8999999999999999) / .1;
  // reverse: nothing (set directly from preset in setPreset)
  m_preset = 0;
  // reverse: not used
  m_strikePosition = 0;
}

BandedWG :: ~BandedWG()
{
  delete bowTabl;
  delete adsr;
  delete [] bandpass;
  delete [] delay;
}

void BandedWG :: clear()
{
  for (int i=0; i<nModes; i++) {
    delay[i].clear();
    bandpass[i].clear();
  }
}

void BandedWG :: setPreset(int preset)
{
  int i;
  m_preset = preset;
  switch (preset){

  case 1: // Tuned Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 4.0198391420;
    modes[2] = (MY_FLOAT) 10.7184986595;
    modes[3] = (MY_FLOAT) 18.0697050938;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.999,(double) i+1);
      excitation[i] = 1.0;
    }

    break;

  case 2: // Glass Harmonica
    presetModes = 5;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.32;
    modes[2] = (MY_FLOAT) 4.25;
    modes[3] = (MY_FLOAT) 6.63;
    modes[4] = (MY_FLOAT) 9.38;
    // modes[5] = (MY_FLOAT) 12.22;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.999,(double) i+1);
      excitation[i] = 1.0;
    }
    /*
      baseGain = (MY_FLOAT) 0.99999;
      for (i=0; i<presetModes; i++) 
      gains[i]= (MY_FLOAT) pow(baseGain, delay[i].getDelay()+i);
    */

    break;
   
  case 3: // Tibetan Prayer Bowl (ICMC'02)
    presetModes = 12;
    modes[0]=0.996108344;
    basegains[0]=0.999925960128219;
    excitation[0]=11.900357/10.0;
    modes[1]=1.0038916562;
    basegains[1]=0.999925960128219;
    excitation[1]=11.900357/10.;
    modes[2]=2.979178;
    basegains[2]=0.999982774366897;
    excitation[2]=10.914886/10.;
    modes[3]=2.99329767;
    basegains[3]=0.999982774366897;
    excitation[3]=10.914886/10.;
    modes[4]=5.704452;
    basegains[4]=1.0; //0.999999999999999999987356406352;
    excitation[4]=42.995041/10.;
    modes[5]=5.704452;
    basegains[5]=1.0; //0.999999999999999999987356406352;
    excitation[5]=42.995041/10.;
    modes[6]=8.9982;
    basegains[6]=1.0; //0.999999999999999999996995497558225;
    excitation[6]=40.063034/10.;
    modes[7]=9.01549726;
    basegains[7]=1.0; //0.999999999999999999996995497558225;
    excitation[7]=40.063034/10.;
    modes[8]=12.83303;
    basegains[8]=0.999965497558225;
    excitation[8]=7.063034/10.;
    modes[9]=12.807382;
    basegains[9]=0.999965497558225;
    excitation[9]=7.063034/10.;
    modes[10]=17.2808219;
    basegains[10]=0.9999999999999999999965497558225;
    excitation[10]=57.063034/10.;
    modes[11]=21.97602739726;
    basegains[11]=0.999999999999999965497558225;
    excitation[11]=57.063034/10.;

    break;  

  default: // Uniform Bar
    presetModes = 4;
    modes[0] = (MY_FLOAT) 1.0;
    modes[1] = (MY_FLOAT) 2.756;
    modes[2] = (MY_FLOAT) 5.404;
    modes[3] = (MY_FLOAT) 8.933;

    for (i=0; i<presetModes; i++) {
      basegains[i] = (MY_FLOAT) pow(0.9,(double) i+1);
      excitation[i] = 1.0;
    }

    break;
  }

  nModes = presetModes;
  setFrequency( freakency );
}

void BandedWG :: setFrequency(MY_FLOAT frequency)
{
  freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): BandedWG: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }
  if (freakency > 1568.0) freakency = 1568.0;

  MY_FLOAT radius;
  MY_FLOAT base = Stk::sampleRate() / freakency;
  MY_FLOAT length;
  for (int i=0; i<presetModes; i++) {
    // Calculate the delay line lengths for each mode.
    length = (int)(base / modes[i]);
    if ( length > 2.0) {
      delay[i].setDelay( length );
      gains[i]=basegains[i];
      //      gains[i]=(MY_FLOAT) pow(basegains[i], 1/((MY_FLOAT)delay[i].getDelay()));
      //      CK_STDCERR << gains[i];
    }
    else    {
      nModes = i;
      break;
    }
    //  CK_STDCERR << CK_STDENDL;

    // Set the bandpass filter resonances
    radius = 1.0 - ONE_PI * 32 / Stk::sampleRate(); //freakency * modes[i] / Stk::sampleRate()/32;
    if ( radius < 0.0 ) radius = 0.0;
    bandpass[i].setResonance(freakency * modes[i], radius, true);

    delay[i].clear();
    bandpass[i].clear();
  }

  //int olen = (int)(delay[0].getDelay());
  //strikePosition = (int)(strikePosition*(length/modes[0])/olen);

  // chuck
  m_frequency = freakency;
}

void BandedWG :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = (int)(delay[0].getDelay() * position / 2.0);
  m_strikePosition = position;
}

void BandedWG :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = 0.03 + (0.1 * amplitude); 
}

void BandedWG :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void BandedWG :: pluck(MY_FLOAT amplitude)
{
  int j;
  MY_FLOAT min_len = delay[nModes-1].getDelay();
  for (int i=0; i<nModes; i++)
    for(j=0; j<(int)(delay[i].getDelay()/min_len); j++)
      delay[i].tick( excitation[i]*amplitude / nModes /*/ (delay[i].getDelay()/min_len)*/);

  /*    strikeAmp += amplitude;*/
}

void BandedWG :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);

  if ( doPluck )
    this->pluck(amplitude);
  else
    this->startBowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BandedWG: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void BandedWG :: noteOff(MY_FLOAT amplitude)
{
  if ( !doPluck )
    this->stopBowing((1.0 - amplitude) * 0.005);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BandedWG: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT BandedWG :: tick()
{
  int k;

  MY_FLOAT input = 0.0;
  if ( doPluck ) {
    input = 0.0;
    //  input = strikeAmp/nModes;
    //  strikeAmp = 0.0;
  }
  else {
    if (integrationConstant == 0.0)
      velocityInput = 0.0;
    else
      velocityInput = integrationConstant * velocityInput;

    for (k=0; k<nModes; k++)
      velocityInput += baseGain * delay[k].lastOut();
      
    if ( trackVelocity )  {
      bowVelocity *= 0.9995;
      bowVelocity += bowTarget;
      bowTarget *= 0.995;
    }
    else
      bowVelocity = adsr->tick() * maxVelocity;

    input = bowVelocity - velocityInput;
    input = input * bowTabl->tick(input);
    input = input/(MY_FLOAT)nModes;
  }

  MY_FLOAT data = 0.0;  
  for (k=0; k<nModes; k++) {
    bandpass[k].tick(input + gains[k] * delay[k].lastOut());
    delay[k].tick(bandpass[k].lastOut());
    data += bandpass[k].lastOut();
  }
  
  //lastOutput = data * nModes;
  lastOutput = data * 4;
  return lastOutput;
}

void BandedWG :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): BandedWG: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): BandedWG: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_BowPressure_) { // 2
    m_bowPressure = norm;
    if ( norm == 0.0 )
      doPluck = true;
    else {
      doPluck = false;
      bowTabl->setSlope( 10.0 - (9.0 * norm));
    }
  }
  else if (number == 4) { // 4
    if ( !trackVelocity ) trackVelocity = true;
    bowTarget += 0.005 * (norm - bowPosition);
    bowPosition = norm;
    m_bowMotion = norm;
    //adsr->setTarget(bowPosition);
  }
  else if (number == 8) // 8
    this->setStrikePosition( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    //bowTarget += 0.02 * (norm - bowPosition);
    //bowPosition = norm;
    if ( trackVelocity ) trackVelocity = false;
    maxVelocity = 0.13 * norm;
    adsr->setTarget(norm);
  }      
  else if (number == __SK_ModWheel_) { // 1
    //    baseGain = 0.9989999999 + (0.001 * norm );
    baseGain = 0.8999999999999999 + (0.1 * norm);
    //  CK_STDCERR << "[chuck](via STK): Yuck!" << CK_STDENDL;
    for (int i=0; i<nModes; i++)
      gains[i]=(MY_FLOAT) basegains[i]*baseGain;
    //      gains[i]=(MY_FLOAT) pow(baseGain, (int)((MY_FLOAT)delay[i].getDelay()+i));
    m_modesGain = norm;
  }
  else if (number == __SK_ModFrequency_) // 11
    integrationConstant = norm;
  else if (number == __SK_Sustain_) { // 64
    if (value < 65) doPluck = true;
    else doPluck = false;
  }
  else if (number == __SK_Portamento_)  { // 65
    if (value < 65) trackVelocity = false;
    else trackVelocity = true;
  }
  else if (number == __SK_ProphesyRibbon_) // 16
    this->setPreset((int) value);  
  else
    CK_STDCERR << "[chuck](via STK): BandedWG: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BandedWG: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


BeeThree :: BeeThree()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( (Stk::rawwavePath() + "special:sinewave").c_str(), TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 0.999);
  this->setRatio(1, 1.997);
  this->setRatio(2, 3.006);
  this->setRatio(3, 6.009);

  gains[0] = __FM_gains[95];
  gains[1] = __FM_gains[95];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[95];

  adsr[0]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[2]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[3]->setAllTimes( 0.005, 0.001, 0.4, 0.03);

  twozero->setGain( 0.1 );
}  

BeeThree :: ~BeeThree()
{
}

void BeeThree :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[95];
  gains[1] = amplitude * __FM_gains[95];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[95];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "BeeThree: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT BeeThree :: tick()
{
  register MY_FLOAT temp;

  if (modDepth > 0.0)   {
    temp = 1.0 + (modDepth * vibrato->tick() * 0.1);
    waves[0]->setFrequency(baseFrequency * temp * ratios[0]);
    waves[1]->setFrequency(baseFrequency * temp * ratios[1]);
    waves[2]->setFrequency(baseFrequency * temp * ratios[2]);
    waves[3]->setFrequency(baseFrequency * temp * ratios[3]);
  }

  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = control1 * 2.0 * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  temp += control2 * 2.0 * gains[2] * adsr[2]->tick() * waves[2]->tick();
  temp += gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp += gains[0] * adsr[0]->tick() * waves[0]->tick();

  lastOutput = temp * 0.125;
  return lastOutput;
}


/***************************************************/
/*! \class BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This protected Filter subclass implements a
    two-pole, two-zero digital filter.  A method
    is provided for creating a resonance in the
    frequency response while maintaining a constant
    filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BiQuad :: BiQuad() : FilterStk()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  FilterStk::setCoefficients( 3, B, 3, A );
}

BiQuad :: ~BiQuad()
{
}

void BiQuad :: clear(void)
{
  FilterStk::clear();
}

void BiQuad :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void BiQuad :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void BiQuad :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void BiQuad :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void BiQuad :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void BiQuad :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Use zeros at +- 1 and normalize the filter peak gain.
    b[0] = 0.5 - 0.5 * a[2];
    b[1] = 0.0;
    b[2] = -b[0];
  }
}

void BiQuad :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  // This method does not attempt to normalize the filter gain.
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());
}

void BiQuad :: setEqualGainZeroes()
{
  b[0] = 1.0;
  b[1] = 0.0;
  b[2] = -1.0;
}

void BiQuad :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT BiQuad :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT BiQuad :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT BiQuad :: tick(MY_FLOAT sample)
{
    inputs[0] = gain * sample;
    outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] + b[2] * inputs[2];
    outputs[0] -= a[2] * outputs[2] + a[1] * outputs[1];
    inputs[2] = inputs[1];
    inputs[1] = inputs[0];
    outputs[2] = outputs[1];
    outputs[1] = outputs[0];

    // gewang: dedenormal
    CK_STK_DDN(outputs[1]);
    CK_STK_DDN(outputs[2]);

    return outputs[0];
}

MY_FLOAT *BiQuad :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}


/***************************************************/
/*! \class Blit
    \brief STK band-limited impulse train class.

    This class generates a band-limited impulse train using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the impulse train
    and the number of harmonics contained in the resulting signal.

    The signal is normalized so that the peak value is +/-1.0.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Original code by Robin Davies, 2005.
    Revisions by Gary Scavone for STK, 2005.
*/
/***************************************************/

#include <cmath>
#include <limits>
 
Blit:: Blit( MY_FLOAT frequency )
{
  nHarmonics_ = 0;
  this->setFrequency( frequency );
  this->reset();
}

Blit :: ~Blit()
{
}

void Blit :: reset()
{
  phase_ = 0.0;
  // lastOutput_ = 0;
}

void Blit :: setFrequency( MY_FLOAT frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "Blit::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  p_ = Stk::sampleRate() / frequency;
  rate_ = ONE_PI / p_;
  this->updateHarmonics();
}

void Blit :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();
}

void Blit :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

#if defined(_STK_DEBUG_)
  errorString_ << "Blit::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

MY_FLOAT Blit :: tick( void )
{
  // The code below implements the SincM algorithm of Stilson and
  // Smith with an additional scale factor of P / M applied to
  // normalize the output.

  // A fully optimized version of this code would replace the two sin
  // calls with a pair of fast sin oscillators, for which stable fast
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has not
  // been made here.

  MY_FLOAT output;

  // Avoid a divide by zero at the sinc peak, which has a limiting
  // value of 1.0.
  MY_FLOAT denominator = sin( phase_ );
  if ( denominator <= std::numeric_limits<MY_FLOAT>::epsilon() ) {
    output = 1.0;
  } else {
    output =  sin( m_ * phase_ );
    output /= m_ * denominator;
  }

  phase_ += rate_;
  if ( phase_ >= ONE_PI ) phase_ -= ONE_PI;

  return output;
}


/***************************************************/
/*! \class BlitSaw
    \brief STK band-limited sawtooth wave class.

    This class generates a band-limited sawtooth waveform using a
    closed-form algorithm reported by Stilson and Smith in "Alias-Free
    Digital Synthesis of Classic Analog Waveforms", 1996.  The user
    can specify both the fundamental frequency of the sawtooth and the
    number of harmonics contained in the resulting signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

BlitSaw:: BlitSaw( MY_FLOAT frequency )
{
  nHarmonics_ = 0;
  this->reset();
  this->setFrequency( frequency );
}

BlitSaw :: ~BlitSaw()
{
}

void BlitSaw :: reset()
{
  phase_ = 0.0f;
  state_ = 0.0;
  // lastOutput_ = 0;
}

void BlitSaw :: setFrequency( MY_FLOAT frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "BlitSaw::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  p_ = Stk::sampleRate() / frequency;
  C2_ = 1 / p_;
  rate_ = ONE_PI * C2_;
  this->updateHarmonics();
}

void BlitSaw :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();

  // I found that the initial DC offset could be minimized with an
  // initial state setting as given below.  This initialization should
  // only happen before starting the oscillator for the first time
  // (but after setting the frequency and number of harmonics).  I
  // struggled a bit to decide where best to put this and finally
  // settled on here.  In general, the user shouldn't be messing with
  // the number of harmonics once the oscillator is running because
  // this is automatically taken care of in the setFrequency()
  // function.  (GPS - 1 October 2005)
  state_ = -0.5 * a_;
}

void BlitSaw :: updateHarmonics( void )
{
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * maxHarmonics + 1;
  }
  else
    m_ = 2 * nHarmonics_ + 1;

  a_ = m_ / p_;

#if defined(_STK_DEBUG_)
  errorString_ << "BlitSaw::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

MY_FLOAT BlitSaw :: tick( void )
{
  // The code below implements the BLIT algorithm of Stilson and
  // Smith, followed by a summation and filtering operation to produce
  // a sawtooth waveform.  After experimenting with various approaches
  // to calculate the average value of the BLIT over one period, I
  // found that an estimate of C2_ = 1.0 / period (in samples) worked
  // most consistently.  A "leaky integrator" is then applied to the
  // difference of the BLIT output and C2_. (GPS - 1 October 2005)

  // A fully  optimized version of this code would replace the two sin 
  // calls with a pair of fast sin oscillators, for which stable fast 
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has 
  // not been made here.

  MY_FLOAT output;

  // Avoid a divide by zero, or use of a denormalized divisor 
  // at the sinc peak, which has a limiting value of m_ / p_.
  MY_FLOAT denominator = sin( phase_ );
  if ( fabs(denominator) <= std::numeric_limits<MY_FLOAT>::epsilon() )
    output = a_;
  else {
    output =  sin( m_ * phase_ );
    output /= p_ * denominator;
  }

  output += state_ - C2_;
  state_ = output * 0.995;

  phase_ += rate_;
  if ( phase_ >= ONE_PI ) phase_ -= ONE_PI;

  return output;
}


/***************************************************/
/*! \class BlitSquare
    \brief STK band-limited square wave class.

    This class generates a band-limited square wave signal.  It is
    derived in part from the approach reported by Stilson and Smith in
    "Alias-Free Digital Synthesis of Classic Analog Waveforms", 1996.
    The algorithm implemented in this class uses a SincM function with
    an even M value to achieve a bipolar bandlimited impulse train.
    This signal is then integrated to achieve a square waveform.  The
    integration process has an associated DC offset but that is
    subtracted off the output signal.

    The user can specify both the fundamental frequency of the
    waveform and the number of harmonics contained in the resulting
    signal.

    If nHarmonics is 0, then the signal will contain all harmonics up
    to half the sample rate.  Note, however, that this setting may
    produce aliasing in the signal when the frequency is changing (no
    automatic modification of the number of harmonics is performed by
    the setFrequency() function).

    Based on initial code of Robin Davies, 2005.
    Modified algorithm code by Gary Scavone, 2005.
*/
/***************************************************/

BlitSquare:: BlitSquare( MY_FLOAT frequency )
{
  nHarmonics_ = 0;
  this->setFrequency( frequency );
  this->reset();
}

BlitSquare :: ~BlitSquare()
{
}

void BlitSquare :: reset()
{
  phase_ = 0.0;
  m_output = 0;
  dcbState_ = 0.0;
  m_boutput = 0.0;
}

void BlitSquare :: setFrequency( MY_FLOAT frequency )
{
#if defined(_STK_DEBUG_)
  errorString_ << "BlitSquare::setFrequency: frequency = " << frequency << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  // By using an even value of the parameter M, we get a bipolar blit
  // waveform at half the blit frequency.  Thus, we need to scale the
  // frequency value here by 0.5. (GPS, 2006).
  p_ = 0.5 * Stk::sampleRate() / frequency;
  rate_ = ONE_PI / p_;
  this->updateHarmonics();
}

void BlitSquare :: setHarmonics( unsigned int nHarmonics )
{
  nHarmonics_ = nHarmonics;
  this->updateHarmonics();
}

void BlitSquare :: updateHarmonics( void )
{
  // Make sure we end up with an even value of the parameter M here.
  if ( nHarmonics_ <= 0 ) {
    unsigned int maxHarmonics = (unsigned int) floor( 0.5 * p_ );
    m_ = 2 * ( maxHarmonics );
  }
  else
    m_ = 2 * ( nHarmonics_ );

  // This offset value was derived empirically. (GPS, 2005)
  // offset_ = 1.0 - 0.5 * m_ / p_;

  a_ = m_ / p_;

#if defined(_STK_DEBUG_)
  errorString_ << "BlitSquare::updateHarmonics: nHarmonics_ = " << nHarmonics_ << ", m_ = " << m_ << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

MY_FLOAT BlitSquare :: tick( void )
{
  MY_FLOAT temp = m_boutput;

  // A fully  optimized version of this would replace the two sin calls
  // with a pair of fast sin oscillators, for which stable fast 
  // two-multiply algorithms are well known. In the spirit of STK,
  // which favors clarity over performance, the optimization has 
  // not been made here.

  // Avoid a divide by zero, or use of a denomralized divisor
  // at the sinc peak, which has a limiting value of 1.0.
  MY_FLOAT denominator = sin( phase_ );
  if ( fabs( denominator )  < std::numeric_limits<MY_FLOAT>::epsilon() ) {
    // Inexact comparison safely distinguishes betwen *close to zero*, and *close to PI*.
    if ( phase_ < 0.1f || phase_ > TWO_PI - 0.1f )
      m_boutput = a_;
    else
      m_boutput = -a_;
  }
  else {
    m_boutput =  sin( m_ * phase_ );
    m_boutput /= p_ * denominator;
  }

  m_boutput += temp;

  // Now apply DC blocker.
  m_output = m_boutput - dcbState_ + 0.999 * m_output;
  dcbState_ = m_boutput;

  phase_ += rate_;
  if ( phase_ >= TWO_PI ) phase_ -= TWO_PI;

  return m_output;
}


/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#define __BOTTLE_RADIUS_ 0.999

BlowBotl :: BlowBotl()
{
  jetTable = new JetTabl();

  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 5.925 );
  vibratoGain = 0.0;

  resonator = new BiQuad();
  resonator->setResonance(500.0, __BOTTLE_RADIUS_, true);

  adsr = new ADSR();
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010 );

  noise = new Noise();
  noiseGain = 20.0;

  maxPressure = (MY_FLOAT)0.0;

  // chuck added later
  outputGain = 1.0;

  // chuck
  //reverse: nothing (set from directly from setFrequency)
  m_frequency = 500;
  //reverse: nothing (set in BlowBotl_ctrl_rate only)
  m_rate = .02;
  //reverse: norm * 30.0 (from controlChange)
  m_noiseGain = noiseGain / 30.0;
  //reverse: vibratoFreq * 12.0 (from controlChange) 
  m_vibratoFreq = vibrato->m_freq / 12.0;
  //reverse: vibratoGain * 0.4 (from controlChange)
  m_vibratoGain = vibratoGain / 0.4;
  //reverse: nothing (set in controlChange)
  m_volume = 1.0;
}

BlowBotl :: ~BlowBotl()
{
  delete jetTable;
  delete resonator;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void BlowBotl :: clear()
{
  resonator->clear();
}

void BlowBotl :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): BlowBotl: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  resonator->setResonance( freakency, __BOTTLE_RADIUS_, true );

  // chuck
  m_frequency = freakency;
}

void BlowBotl :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void BlowBotl :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void BlowBotl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowBotl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void BlowBotl :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowBotl: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT BlowBotl :: tick()
{
  MY_FLOAT breathPressure;
  MY_FLOAT randPressure;
  MY_FLOAT pressureDiff;

  // Calculate the breath pressure (envelope + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  pressureDiff = breathPressure - resonator->lastOut();

  randPressure = noiseGain * noise->tick();
  randPressure *= breathPressure;
  randPressure *= (1.0 + pressureDiff);

  resonator->tick( breathPressure + randPressure - ( jetTable->tick( pressureDiff ) * pressureDiff ) );
  lastOutput = 0.2 * outputGain * dcBlock->tick( pressureDiff );

  return lastOutput;
}

void BlowBotl :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): BlowBotl: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): BlowBotl: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if( number == __SK_NoiseLevel_ ) { // 4
    noiseGain = norm * 30.0;
    m_noiseGain = norm; // chuck
  }
  else if( number == __SK_ModFrequency_ ) // 11
    setVibratoFreq( norm * 12.0 );
  else if( number == __SK_ModWheel_ ) { // 1
    vibratoGain = norm * 0.4;
    m_vibratoGain = norm;
  }
  else if( number == __SK_AfterTouch_Cont_ ) { // 128
    adsr->setTarget( norm );
    m_volume = norm;
  }
  else
    CK_STDCERR << "[chuck](via STK): BlowBotl: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowBotl: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.
    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BlowHole :: BlowHole(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // delays[0] is the delay line between the reed and the register vent.
  delays[0] = (DelayL *) new DelayL( 5.0 * Stk::sampleRate() / 22050.0, 100 );
  // delays[1] is the delay line between the register vent and the tonehole.
  delays[1] = (DelayL *) new DelayL( length >> 1, length );
  // delays[2] is the delay line between the tonehole and the end of the bore.
  delays[2] = (DelayL *) new DelayL( 4.0 * Stk::sampleRate() / 22050.0, 100 );
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Calculate the initial tonehole three-port scattering coefficient
  double r_b = 0.0075;    // main bore radius
  r_th = 0.003;           // tonehole radius
  scatter = -pow(r_th,2) / ( pow(r_th,2) + 2*pow(r_b,2) );

  // Calculate tonehole coefficients
  MY_FLOAT te = 1.4 * r_th;    // effective length of the open hole
  th_coeff = (te*2*Stk::sampleRate() - 347.23) / (te*2*Stk::sampleRate() + 347.23);
  tonehole = new PoleZero;
  // Start with tonehole open
  tonehole->setA1(-th_coeff);
  tonehole->setB0(th_coeff);
  tonehole->setB1(-1.0);

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    // register vent radius
  te = 1.4 * r_rh;         // effective length of the open hole
  double xi = 0.0;         // series resistance term
  double zeta = 347.23 + 2*ONE_PI*pow(r_b,2)*xi/1.1769;
  double psi = 2*ONE_PI*pow(r_b,2)*te / (ONE_PI*pow(r_rh,2));
  rh_coeff = (zeta - 2 * Stk::sampleRate() * psi) / (zeta + 2 * Stk::sampleRate() * psi);
  rh_gain = -347.23 / (zeta + 2 * Stk::sampleRate() * psi);
  vent = new PoleZero;
  vent->setA1(rh_coeff);
  vent->setB0(1.0);
  vent->setB1(1.0);
  // Start with register vent closed
  vent->setGain(0.0);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.01;

  // set
  setFrequency( 220.0 );

  // chuck data

  //m_reed = 0.5;  // DONE: check default value  <- old value
  //reverse: slope =  -0.44 + (0.26 * norm)
  m_reed = (reedTable->slope + 0.44) / 0.26;
  //reverse: noiseGain = norm * 0.4
  m_noiseGain = noiseGain / .4;
  //reverse: nothing  
  m_tonehole = 1.0;
  //reverse: nothing  
  m_vent = 0.0;
  //reverse: nothing  
  m_pressure = 1.0;
  //reverse: nothing 
  m_rate = envelope->rate;
}

BlowHole :: ~BlowHole()
{
  delete delays[0];
  delete delays[1];
  delete delays[2];
  delete reedTable;
  delete filter;
  delete tonehole;
  delete vent;
  delete envelope;
  delete noise;
  delete vibrato;
}

void BlowHole :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  delays[2]->clear();
  filter->tick((MY_FLOAT) 0.0);
  tonehole->tick((MY_FLOAT) 0.0);
  vent->tick((MY_FLOAT) 0.0);
}

void BlowHole :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): BlowHole: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 3.5;
  delay -= delays[0]->getDelay() + delays[2]->getDelay();

  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delays[1]->setDelay(delay);

  // chuck
  m_frequency = freakency;
}

void BlowHole :: setVent(MY_FLOAT newValue)
{
  // This method allows setting of the register vent "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).

  MY_FLOAT gain;
  if (newValue <= 0.0) gain = 0.0;
  else if (newValue >= 1.0) gain = rh_gain;
  else gain = newValue * rh_gain;
  m_vent = newValue;  
  vent->setGain(gain);
}

void BlowHole :: setTonehole(MY_FLOAT newValue)
{
  // This method allows setting of the tonehole "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).
  MY_FLOAT new_coeff;

  if (newValue <= 0.0) new_coeff = 0.9995;
  else if (newValue >= 1.0) new_coeff = th_coeff;
  else new_coeff = (newValue * (th_coeff - 0.9995)) + 0.9995;
  m_tonehole = newValue;
  tonehole->setA1(-new_coeff);
  tonehole->setB0(new_coeff);
}

void BlowHole :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void BlowHole :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void BlowHole :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowHole: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void BlowHole :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowHole: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT BlowHole :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Calculate the differential pressure = reflected - mouthpiece pressures
  pressureDiff = delays[0]->lastOut() - breathPressure;

  // Do two-port junction scattering for register vent
  MY_FLOAT pa = breathPressure + pressureDiff * reedTable->tick(pressureDiff);
  MY_FLOAT pb = delays[1]->lastOut();
  vent->tick(pa+pb);

  lastOutput = delays[0]->tick(vent->lastOut()+pb);
  lastOutput *= outputGain;

  // Do three-port junction scattering (under tonehole)
  pa += vent->lastOut();
  pb = delays[2]->lastOut();
  MY_FLOAT pth = tonehole->lastOut();
  temp = scatter * (pa + pb - 2 * pth);

  delays[2]->tick(filter->tick(pa + temp) * -0.95);
  delays[1]->tick(pb + temp);
  tonehole->tick(pa + pb - pth + temp);

  return lastOutput;
}

void BlowHole :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): BlowHole: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): BlowHole: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_ReedStiffness_) { // 2 
    m_reed = norm;
    reedTable->setSlope( -0.44 + (0.26 * norm) ); 
  }
  else if (number == __SK_NoiseLevel_) { // 4
    m_noiseGain = norm;
    noiseGain = ( norm * 0.4 );
  }
  else if (number == __SK_ModFrequency_) // 11
    this->setTonehole( norm );
  else if (number == __SK_ModWheel_) // 1
    this->setVent( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    m_pressure = norm;
    envelope->setValue( norm );
  }
  else
    CK_STDCERR << "[chuck](via STK): BlowHole: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): BlowHole: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class BowTabl
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

BowTabl :: BowTabl()
{
  offSet = (MY_FLOAT) 0.0;
  slope = (MY_FLOAT) 0.1;
}

BowTabl :: ~BowTabl()
{
}

void BowTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;
}

void BowTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;
}

MY_FLOAT BowTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT BowTabl :: tick(MY_FLOAT input)
{
  // The input represents differential string vs. bow velocity.
  MY_FLOAT sample;
  sample = input + offSet;  // add bias to input
  sample *= slope;          // then scale it
  lastOutput = (MY_FLOAT)  fabs((double) sample) + (MY_FLOAT) 0.75;
  lastOutput = (MY_FLOAT)  pow( lastOutput,(MY_FLOAT) -4.0 );

  // Set minimum friction to 0.0
  //if (lastOutput < 0.0 ) lastOutput = 0.0;
  // Set maximum friction to 1.0.
  if (lastOutput > 1.0 ) lastOutput = (MY_FLOAT) 1.0;

  return lastOutput;
}

MY_FLOAT *BowTabl :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}


/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Bowed :: Bowed(MY_FLOAT lowestFrequency)
{
  long length;
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  neckDelay = new DelayL(100.0, length);
  length >>= 1;
  bridgeDelay = new DelayL(29.0, length);

  bowTable = new BowTabl;
  bowTable->setSlope((MY_FLOAT) 3.0);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 6.12723);
  vibratoGain = (MY_FLOAT) 0.0;

  stringFilter = new OnePole;
  stringFilter->setPole((MY_FLOAT) (0.6 - (0.1 * 22050.0 / Stk::sampleRate() ) ) );
  stringFilter->setGain((MY_FLOAT) 0.95);

  bodyFilter = new BiQuad;
  bodyFilter->setResonance( 500.0, 0.85, TRUE );
  bodyFilter->setGain((MY_FLOAT) 0.2);

  adsr = new ADSR;
  adsr->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.005,(MY_FLOAT) 0.9,(MY_FLOAT) 0.01);
    
  betaRatio = (MY_FLOAT)0.127236;

  // Necessary to initialize internal variables.
  setFrequency( 220.0 );

  // CHUCK
  //reverse: bowTable->setSlope( 5.0 - (4.0 * norm) );
  m_bowPressure = (bowTable->slope - 5.0) / -4.0;
  //reverse: betaRatio = 0.027236 + (0.2 * norm);
  m_bowPosition = (betaRatio - .027236) / .2;
  //reverse: setVibratoFreq( norm * 12.0 );  
  m_vibratoFreq = vibrato->m_freq / 12.0;
  //reverse: vibratoGain = ( norm * 0.4 );
  m_vibratoGain = 0.0; // vibratoGain / 0.4
  //reverse: nothing
  m_volume = 1.0;
}

Bowed :: ~Bowed()
{
  delete neckDelay;
  delete bridgeDelay;
  delete bowTable;
  delete stringFilter;
  delete bodyFilter;
  delete vibrato;
  delete adsr;
}

void Bowed :: clear()
{
  neckDelay->clear();
  bridgeDelay->clear();
}

void Bowed :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Bowed: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  baseDelay = Stk::sampleRate() / freakency - (MY_FLOAT) 4.0;
  if ( baseDelay <= 0.0 ) baseDelay = 0.3;
  bridgeDelay->setDelay(baseDelay * betaRatio);                    // bow to bridge length
  neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio)); // bow to nut (finger) length

  // chuck
  m_frequency = freakency;
}

void Bowed :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = (MY_FLOAT) 0.03 + ((MY_FLOAT) 0.2 * amplitude); 
}

void Bowed :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void Bowed :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->startBowing(amplitude, amplitude * 0.001);
  this->setFrequency(frequency);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Bowed: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Bowed :: noteOff(MY_FLOAT amplitude)
{
  this->stopBowing(((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.005);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Bowed: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Bowed :: setVibrato(MY_FLOAT gain)
{
  vibratoGain = gain;
}

MY_FLOAT Bowed :: tick()
{
  MY_FLOAT bowVelocity;
  MY_FLOAT bridgeRefl;
  MY_FLOAT nutRefl;
  MY_FLOAT newVel;
  MY_FLOAT velDiff;
  MY_FLOAT stringVel;
    
  bowVelocity = maxVelocity * adsr->tick();

  bridgeRefl = -stringFilter->tick( bridgeDelay->lastOut() );
  nutRefl = -neckDelay->lastOut();
  stringVel = bridgeRefl + nutRefl;               // Sum is String Velocity
  velDiff = bowVelocity - stringVel;              // Differential Velocity
  newVel = velDiff * bowTable->tick( velDiff );   // Non-Linear Bow Function
  neckDelay->tick(bridgeRefl + newVel);           // Do string propagations
  bridgeDelay->tick(nutRefl + newVel);
    
  if (vibratoGain > 0.0)  {
    neckDelay->setDelay((baseDelay * ((MY_FLOAT) 1.0 - betaRatio)) + 
                        (baseDelay * vibratoGain * vibrato->tick()));
  }

  lastOutput = bodyFilter->tick(bridgeDelay->lastOut());                 

  return lastOutput;
}

void Bowed :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Bowed: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Bowed: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_BowPressure_) { // 2
    m_bowPressure = norm;
    bowTable->setSlope( 5.0 - (4.0 * norm) );
  }
  else if (number == __SK_BowPosition_) { // 4
    m_bowPosition = norm;
    betaRatio = 0.027236 + (0.2 * norm);
    bridgeDelay->setDelay(baseDelay * betaRatio);
    neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio));
  }
  else if (number == __SK_ModFrequency_) { // 11
    setVibratoFreq( norm * 12.0 );
    m_vibratoFreq = norm;
    }
  else if (number == __SK_ModWheel_) { // 1
    m_vibratoGain = norm;
    vibratoGain = ( norm * 0.4 );
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    m_volume = norm;
    adsr->setTarget(norm);
  }
  else
    CK_STDCERR << "[chuck](via STK): Bowed: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Bowed: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Brass :: Brass(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA( 0.5 * length, length );

  lipFilter = new BiQuad();
  lipFilter->setGain( 0.03 );
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  adsr = new ADSR;
  adsr->setAllTimes( 0.005, 0.001, 1.0, 0.010);

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 6.137 );
  vibratoGain = 0.0;

  this->clear();
    maxPressure = (MY_FLOAT) 0.0;
  lipTarget = 0.0;

  // Necessary to initialize variables.
  setFrequency( 220.0 );

  // CHUCK
  //reverse: nothing
  m_rate = .005;
  //reverse: I give up!
  m_lip = 0.1; 
  //reverse: this is a special variable... we'll just leave this be for now
  m_slide = length;
  //reverse: setVibratoFreq( norm * 12.0 );
  m_vibratoFreq = vibrato->m_freq / 12.0;
  //reverse: setVibratoGain(norm * 0.4)
  m_vibratoGain = 0.0;
  //reverse: nothing
  m_volume = 1.0;
}

Brass :: ~Brass()
{
  delete delayLine;
  delete lipFilter;
  delete dcBlock;
  delete adsr;
  delete vibrato;
}

void Brass :: clear()
{
  delayLine->clear();
  lipFilter->clear();
  dcBlock->clear();
}

void Brass :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Brass: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // Fudge correction for filter delays.
  slideTarget = (Stk::sampleRate() / freakency * 2.0) + 3.0;
  delayLine->setDelay(slideTarget); // play a harmonic

  lipTarget = freakency;
  lipFilter->setResonance( freakency, 0.997 );

  // chuck
  m_frequency = freakency;
}

void Brass :: setLip(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Brass: setLip parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  lipFilter->setResonance( freakency, 0.997 );
}

void Brass :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void Brass :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Brass :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  this->startBlowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Brass: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Brass :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.005);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Brass: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Brass :: tick()
{
  MY_FLOAT breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  MY_FLOAT mouthPressure = 0.3 * breathPressure;
  MY_FLOAT borePressure = 0.85 * delayLine->lastOut();
  MY_FLOAT deltaPressure = mouthPressure - borePressure; // Differential pressure.
  deltaPressure = lipFilter->tick( deltaPressure );      // Force - > position.
  deltaPressure *= deltaPressure;                        // Basic position to area mapping.
  if ( deltaPressure > 1.0 ) deltaPressure = 1.0;         // Non-linear saturation.
  // The following input scattering assumes the mouthPressure = area.
  lastOutput = deltaPressure * mouthPressure + ( 1.0 - deltaPressure) * borePressure;
  lastOutput = delayLine->tick( dcBlock->tick( lastOutput ) );

  return lastOutput;
}

void Brass :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Brass: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Brass: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_LipTension_) { // 2
    MY_FLOAT temp = lipTarget * pow( 4.0, (2.0 * norm) - 1.0 );
    m_lip = norm;
    this->setLip(temp);
  }
  else if (number == __SK_SlideLength_) { // 4
    m_slide = norm;
    delayLine->setDelay( slideTarget * (0.5 + norm) );
  }
  else if (number == __SK_ModFrequency_) // 11
    setVibratoFreq( norm * 12.0 );
  else if (number == __SK_ModWheel_ ) { // 1
    m_vibratoGain = norm;
    vibratoGain = norm * 0.4;
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    m_volume = norm;
    adsr->setTarget( norm );
  }
  else
    CK_STDCERR << "[chuck](via STK): Brass: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Brass: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Chorus :: Chorus(MY_FLOAT baseDelay)
{
  delayLine[0] = delayLine[1] = NULL;
  mods[0] = new WaveLoop( "special:sinewave", TRUE );
  mods[1] = NULL;
  set(baseDelay, 4);
  setDelay( baseDelay );
  setModDepth( .5 );
  setModFrequency( .25 );

  // Concatenate the STK rawwave path to the rawwave file
  // mods[0] = new WaveLoop( "special:sinewave", TRUE );
  // mods[1] = new WaveLoop( "special:sinewave", TRUE );
  // mods[0]->setFrequency(0.2);
  // mods[1]->setFrequency(0.222222);
  effectMix = 0.5;
}

Chorus :: ~Chorus()
{
  SAFE_DELETE( delayLine[0] );
  SAFE_DELETE( delayLine[1] );
  SAFE_DELETE( mods[0] );
  SAFE_DELETE( mods[1] );
}

// chuck
void Chorus :: set(MY_FLOAT baseDelay, MY_FLOAT depth)
{
  SAFE_DELETE( delayLine[0] );
  SAFE_DELETE( delayLine[1] );

  delayLine[0] = new DelayL((long) baseDelay, (long) (baseDelay + baseDelay * depth) + 2);
  // delayLine[0] = new DelayL((long) baseDelay, (long) (baseDelay + baseDelay * 1.414 * depth) + 2);
  // delayLine[1] = new DelayL((long) baseDelay, (long) (baseDelay + baseDelay * depth) + 2);

  this->clear();
}

void Chorus :: clear()
{
  delayLine[0]->clear();
  // delayLine[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

void Chorus :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Chorus: setEffectMix parameter is less than zero!" << CK_STDENDL;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Chorus: setEffectMix parameter is greater than 1.0!" << CK_STDENDL;
    effectMix = 1.0;
  }
}

void Chorus :: setModDepth(MY_FLOAT depth)
{
  modDepth = depth;
}

void Chorus :: setDelay(MY_FLOAT baseDelay)
{
  baseLength = baseDelay;
}

void Chorus :: setModFrequency(MY_FLOAT frequency)
{
  mods[0]->setFrequency(frequency);
  // mods[1]->setFrequency(frequency * 1.1111);
}

MY_FLOAT Chorus :: lastOut() const
{
//  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
    return lastOutput[0];
}

MY_FLOAT Chorus :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Chorus :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT Chorus :: tick(MY_FLOAT input)
{
  delayLine[0]->setDelay(baseLength * modDepth * .5 * (1.0 + mods[0]->tick()));
  // delayLine[0]->setDelay(baseLength * 0.707 * modDepth * (1.0 + mods[0]->tick()));
  // delayLine[1]->setDelay(baseLength  * 0.5 * modDepth * (1.0 + mods[1]->tick()));
  lastOutput[0] = input * (1.0 - effectMix);
  lastOutput[0] += effectMix * delayLine[0]->tick(input);
  // lastOutput[1] = input * (1.0 - effectMix);
  // lastOutput[1] += effectMix * delayLine[1]->tick(input);
  // return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
  return lastOutput[0];
}

MY_FLOAT *Chorus :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Clarinet :: Clarinet(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayL( (MY_FLOAT)(length / 2.0), length);
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;

  // set
  setFrequency( 220.0 );

  // CHUCK
  //reverse: reedTable->setSlope((MY_FLOAT) -0.44 + ( (MY_FLOAT) 0.26 * norm ))
  m_reed = (reedTable->slope + 0.44) / 0.26;
  //reverse: noiseGain = (norm * (MY_FLOAT) 0.4);
  m_noiseGain = noiseGain / .4;
  //reverse: setVibratoFreq( norm * 12.0 );
  m_vibratoFreq = vibrato->m_freq / 12.0;
  //reverse: vibratoGain = (norm * (MY_FLOAT) 0.5);
  m_vibratoGain = .1 / 0.5;
  //reverse: nothing
  m_volume = 1.0;
  //reverse: nothing
  m_rate = .005;
}

Clarinet :: ~Clarinet()
{
  delete delayLine;
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Clarinet :: clear()
{
  delayLine->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Clarinet :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Clarinet: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 1.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);

  // chuck
  m_frequency = freakency;
}

void Clarinet :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Clarinet :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void Clarinet :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->startBlowing((MY_FLOAT) 0.55 + (amplitude * (MY_FLOAT) 0.30), amplitude * (MY_FLOAT) 0.005);
  outputGain = amplitude + (MY_FLOAT) 0.001;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Clarinet: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Clarinet :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * (MY_FLOAT) 0.01);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Clarinet: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Clarinet :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Perform commuted loss filtering.
  pressureDiff = -0.95 * filter->tick(delayLine->lastOut());

  // Calculate pressure difference of reflected and mouthpiece pressures.
  pressureDiff = pressureDiff - breathPressure;

  // Perform non-linear scattering using pressure difference in reed function.
  lastOutput = delayLine->tick(breathPressure + pressureDiff * reedTable->tick(pressureDiff));

  // Apply output gain.
  lastOutput *= outputGain;

  return lastOutput;
}

void Clarinet :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Clarinet: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Clarinet: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_ReedStiffness_) { // 2
    m_reed = norm;
    reedTable->setSlope((MY_FLOAT) -0.44 + ( (MY_FLOAT) 0.26 * norm ));
  }
  else if (number == __SK_NoiseLevel_) { // 4
    m_noiseGain = norm;
    noiseGain = (norm * (MY_FLOAT) 0.4);
  }
  else if (number == __SK_ModFrequency_) { // 11
    m_vibratoFreq = norm;
    setVibratoFreq( norm * 12.0 );
    }
  else if (number == __SK_ModWheel_) { // 1
    m_vibratoGain = norm;
    vibratoGain = (norm * (MY_FLOAT) 0.5);
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    m_volume = norm;
    envelope->setValue(norm);
  }
  else
    CK_STDCERR << "[chuck](via STK): Clarinet: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Clarinet: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This protected Filter subclass implements
    a non-interpolating digital delay-line.
    A fixed maximum length of 4095 and a delay
    of zero is set using the default constructor.
    Alternatively, the delay and maximum length
    can be set during instantiation with an
    overloaded constructor.
    
    A non-interpolating delay line is typically
    used in fixed delay-length applications, such
    as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Delay :: Delay()
{
    this->set( 0, 4096 );
}

Delay :: Delay(long theDelay, long maxDelay)
{
    this->set( theDelay, maxDelay );
}

void Delay :: set( long delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
}

Delay :: ~Delay()
{
}

void Delay :: clear(void)
{
  long i;
  for (i=0;i<length;i++) inputs[i] = 0.0;
  outputs[0] = 0.0;
}

void Delay :: setDelay(long theDelay)
{
  if (theDelay > length-1) { // The value is too big.
    CK_STDCERR << "[chuck](via STK): Delay: setDelay(" << theDelay << ") too big!" << CK_STDENDL;
    // Force delay to maxLength.
    outPoint = inPoint + 1;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    CK_STDCERR << "[chuck](via STK): Delay: setDelay(" << theDelay << ") less than zero!" << CK_STDENDL;
    outPoint = inPoint;
    delay = 0;
  }
  else {
    outPoint = inPoint - (long) theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPoint < 0) outPoint += length;  // modulo maximum length
}

MY_FLOAT Delay :: getDelay(void) const
{
  return delay;
}

MY_FLOAT Delay :: energy(void) const
{
  int i;
  register MY_FLOAT e = 0;
  if (inPoint >= outPoint) {
    for (i=outPoint; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  } else {
    for (i=outPoint; i<length; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
    for (i=0; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  }
  return e;
}

MY_FLOAT Delay :: contentsAt(unsigned long tapDelay) const
{
  long i = tapDelay;
  if (i < 1) {
    CK_STDCERR << "[chuck](via STK): Delay: contentsAt(" << tapDelay << ") too small!" << CK_STDENDL;
    i = 1;
  }
  else if (i > delay) {
    CK_STDCERR << "[chuck](via STK): Delay: contentsAt(" << tapDelay << ") too big!" << CK_STDENDL;
    i = (long) delay;
  }

  long tap = inPoint - i;
  if (tap < 0) // Check for wraparound.
    tap += length;

  return inputs[tap];
}

MY_FLOAT Delay :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT Delay :: nextOut(void) const
{
  return inputs[outPoint];
}

MY_FLOAT Delay :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Check for end condition
  if (inPoint == length)
    inPoint -= length;

  // Read out next value
  outputs[0] = inputs[outPoint++];

  if (outPoint>=length)
    outPoint -= length;

  return outputs[0];
}

MY_FLOAT *Delay :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using a first-order
    allpass filter.  A fixed maximum length
    of 4095 and a delay of 0.5 is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    An allpass filter has unity magnitude gain but
    variable phase delay properties, making it useful
    in achieving fractional delays without affecting
    a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay
    response, the minimum delay possible in this
    implementation is limited to a value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

DelayA :: DelayA()
{
  this->setDelay( 0.5 );
  apInput = 0.0;
  doNextOut = true;
}

DelayA :: DelayA(MY_FLOAT theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  length = maxDelay+1;

  if ( length > 4096 ) {
    // We need to delete the previously allocated inputs.
    delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();
  }

  inPoint = 0;
  this->setDelay(theDelay);
  apInput = 0.0;
  doNextOut = true;
}

void DelayA :: set( MY_FLOAT delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
    apInput = 0.0;
    doNextOut = true;
}

DelayA :: ~DelayA()
{
}

void DelayA :: clear()
{
  Delay::clear();
  apInput = 0.0;
}

void DelayA :: setDelay(MY_FLOAT theDelay)  
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    CK_STDCERR << "[chuck](via STK): DelayA: setDelay(" << theDelay << ") too big!" << CK_STDENDL;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0.5) {
    CK_STDCERR << "[chuck](via STK): DelayA: setDelay(" << theDelay << ") less than 0.5 not possible!" << CK_STDENDL;
    outPointer = inPoint + 0.4999999999;
    delay = 0.5;
  }
  else {
    outPointer = inPoint - theDelay + 1.0;     // outPoint chases inpoint
    delay = theDelay;
  }

  if (outPointer < 0)
    outPointer += length;  // modulo maximum length

  outPoint = (long) outPointer;        // integer part
  alpha = 1.0 + outPoint - outPointer; // fractional part

  if (alpha < 0.5) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint += 1;
    if (outPoint >= length) outPoint -= length;
    alpha += (MY_FLOAT) 1.0;
  }

  coeff = ((MY_FLOAT) 1.0 - alpha) / 
    ((MY_FLOAT) 1.0 + alpha);         // coefficient for all pass
}

MY_FLOAT DelayA :: nextOut(void)
{
  if ( doNextOut ) {
    // Do allpass interpolation delay.
    nextOutput = -coeff * outputs[0];
    nextOutput += apInput + (coeff * inputs[outPoint]);
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayA :: tick(MY_FLOAT sample)
{
    inputs[inPoint++] = sample;

    // Increment input pointer modulo length.
    if (inPoint == length)
    inPoint -= length;

    outputs[0] = nextOut();
    doNextOut = true;

    // Save the allpass input and increment modulo length.
    apInput = inputs[outPoint++];
    if (outPoint == length)
    outPoint -= length;

    return outputs[0];
}


/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using first-order
    linear interpolation.  A fixed maximum length
    of 4095 and a delay of zero is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    Linear interpolation is an efficient technique
    for achieving fractional delay lengths, though
    it does introduce high-frequency signal
    attenuation to varying degrees depending on the
    fractional delay setting.  The use of higher
    order Lagrange interpolators can typically
    improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

DelayL :: DelayL()
{
    doNextOut = true;
}

DelayL :: DelayL(MY_FLOAT theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  length = maxDelay+1;

  if ( length > 4096 ) {
    // We need to delete the previously allocated inputs.
    delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();
  }

  inPoint = 0;
  this->setDelay(theDelay);
  doNextOut = true;
}

DelayL :: ~DelayL()
{
}

void DelayL :: set( MY_FLOAT delay, long max )
{
    // Writing before reading allows delays from 0 to length-1.
    // If we want to allow a delay of maxDelay, we need a
    // delay-line of length = maxDelay+1.
    length = max+1;

    // We need to delete the previously allocated inputs.
    if( inputs ) delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();

    inPoint = 0;
    this->setDelay(delay);
    doNextOut = true;
}

void DelayL :: setDelay(MY_FLOAT theDelay)
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    CK_STDCERR << "[chuck](via STK): DelayL: setDelay(" << theDelay << ") too big!" << CK_STDENDL;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    CK_STDCERR << "[chuck](via STK): DelayL: setDelay(" << theDelay << ") less than zero!" << CK_STDENDL;
    outPointer = inPoint;
    delay = 0;
  }
  else {
    outPointer = inPoint - theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPointer < 0)
    outPointer += length; // modulo maximum length

  outPoint = (long) outPointer;  // integer part
  alpha = outPointer - outPoint; // fractional part
  omAlpha = (MY_FLOAT) 1.0 - alpha;
}

MY_FLOAT DelayL :: nextOut(void)
{
  if ( doNextOut ) {
    // First 1/2 of interpolation
    nextOutput = inputs[outPoint] * omAlpha;
    // Second 1/2 of interpolation
    if (outPoint+1 < length)
      nextOutput += inputs[outPoint+1] * alpha;
    else
      nextOutput += inputs[0] * alpha;
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayL :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Increment input pointer modulo length.
  if (inPoint == length)
    inPoint -= length;

  outputs[0] = nextOut();
  doNextOut = true;

  // Increment output pointer modulo length.
  if (++outPoint >= length)
    outPoint -= length;

  return outputs[0];
}
/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

// Not really General MIDI yet.  Coming soon.
unsigned char genMIDIMap[128] =
  { 0,0,0,0,0,0,0,0,        // 0-7
    0,0,0,0,0,0,0,0,        // 8-15
    0,0,0,0,0,0,0,0,        // 16-23
    0,0,0,0,0,0,0,0,        // 24-31
    0,0,0,0,1,0,2,0,        // 32-39
    2,3,6,3,6,4,7,4,        // 40-47
    5,8,5,0,0,0,10,0,       // 48-55
    9,0,0,0,0,0,0,0,        // 56-63
    0,0,0,0,0,0,0,0,        // 64-71
    0,0,0,0,0,0,0,0,        // 72-79
    0,0,0,0,0,0,0,0,        // 80-87
    0,0,0,0,0,0,0,0,        // 88-95
    0,0,0,0,0,0,0,0,        // 96-103
    0,0,0,0,0,0,0,0,        // 104-111
    0,0,0,0,0,0,0,0,        // 112-119
    0,0,0,0,0,0,0,0     // 120-127
  };
 
//XXX changed this from 16 to 32 for the 'special' convention..also, we do not have these linked
//in the headers              
char waveNames[DRUM_NUMWAVES][32] =
  { 
    "special:dope",
    "special:bassdrum",
    "special:snardrum",
    "special:tomlowdr",
    "special:tommiddr",
    "special:tomhidrm",
    "special:hihatcym",
    "special:ridecymb",
    "special:crashcym", 
    "special:cowbell1", 
    "special:tambourn"
  };

Drummer :: Drummer() : Instrmnt()
{
  for (int i=0; i<DRUM_POLYPHONY; i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding = 0;
}

Drummer :: ~Drummer()
{
  int i;
  for ( i=0; i<nSounding-1; i++ ) delete waves[i];
  for ( i=0; i<DRUM_POLYPHONY; i++ ) delete filters[i];
}

void Drummer :: noteOn(MY_FLOAT instrument, MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Drummer: NoteOn instrument = " << instrument << ", amplitude = " << amplitude << CK_STDENDL;
#endif

  MY_FLOAT gain = amplitude;
  if ( amplitude > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Drummer: noteOn amplitude parameter is greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Drummer: noteOn amplitude parameter is less than 0.0!" << CK_STDENDL;
    return;
  }

  // Yes, this is tres kludgey.
  int noteNum = (int) ((12*log(instrument/220.0)/log(2.0)) + 57.01);

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for (i=0; i<DRUM_POLYPHONY; i++) {
    if (sounding[i] == noteNum) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves[waveIndex]->reset();
    filters[waveIndex]->setPole((MY_FLOAT) 0.999 - (gain * 0.6));
    filters[waveIndex]->setGain(gain);
  }
  else {
    if (nSounding == DRUM_POLYPHONY) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves[0];
      filters[0]->clear();
      WvIn *tempWv = waves[0];
      OnePole *tempFilt = filters[0];
      // Re-order the list.
      for (i=0; i<DRUM_POLYPHONY-1; i++) {
        waves[i] = waves[i+1];
        filters[i] = filters[i+1];
      }
      waves[DRUM_POLYPHONY-1] = tempWv;
      filters[DRUM_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding += 1;

    sounding[nSounding-1] = noteNum;
    // Concatenate the STK rawwave path to the rawwave file
    waves[nSounding-1] = new WvIn( (Stk::rawwavePath() + waveNames[genMIDIMap[noteNum]]).c_str(), TRUE );
    if (Stk::sampleRate() != 22050.0)
      waves[nSounding-1]->setRate( 22050.0 / Stk::sampleRate() );
    filters[nSounding-1]->setPole((MY_FLOAT) 0.999 - (gain * 0.6) );
    filters[nSounding-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Number Sounding = " << nSounding << CK_STDENDL;
  for (i=0; i<nSounding; i++) CK_STDCERR << sounding[i] << "  ";
  CK_STDCERR << "[chuck](via STK): \n";
#endif
}

void Drummer :: noteOff(MY_FLOAT amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while(i < nSounding) {
    filters[i++]->setGain( amplitude * 0.01 );
  }
}

MY_FLOAT Drummer :: tick()
{
  MY_FLOAT output = 0.0;
  OnePole *tempFilt;

  int j, i = 0;
  while (i < nSounding) {
    if ( waves[i]->isFinished() ) {
      delete waves[i];
        tempFilt = filters[i];
      // Re-order the list.
      for (j=i; j<nSounding-1; j++) {
        sounding[j] = sounding[j+1];
        waves[j] = waves[j+1];
        filters[j] = filters[j+1];
      }
      filters[j] = tempFilt;
      filters[j]->clear();
      sounding[j] = -1;
      nSounding -= 1;
      i -= 1;
    }
    else
      output += filters[i]->tick( waves[i]->tick() );
    i++;
  }

  return output;
}
/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements a echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

Echo :: Echo(MY_FLOAT longestDelay)
{
    delayLine = NULL;
    this->set( longestDelay );
    effectMix = 0.5;
}

Echo :: ~Echo()
{
    delete delayLine;
}

void Echo :: set( MY_FLOAT max )
{
    length = (long)max + 2;
    MY_FLOAT delay = delayLine ? delayLine->getDelay() : length>>1;
    if( delayLine ) delete delayLine;
    if( delay >= max ) delay = max;
    delayLine = new Delay(length>>1, length);
    this->clear();
    this->setDelay(delay+.5);
}

MY_FLOAT Echo :: getDelay()
{
    return delayLine->getDelay();
}

void Echo :: clear()
{
  delayLine->clear();
  lastOutput = 0.0;
}

void Echo :: setDelay(MY_FLOAT delay)
{
  MY_FLOAT size = delay;
  if ( delay < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Echo: setDelay parameter is less than zero!" << CK_STDENDL;
    size = 0.0;
  }
  else if ( delay > length ) {
    CK_STDCERR << "[chuck](via STK): Echo: setDelay parameter is greater than delay length!" << CK_STDENDL;
    size = length;
  }

  delayLine->setDelay((long)size);
}

void Echo :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Echo: setEffectMix parameter is less than zero!" << CK_STDENDL;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Echo: setEffectMix parameter is greater than 1.0!" << CK_STDENDL;
    effectMix = 1.0;
  }
}

MY_FLOAT Echo :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Echo :: tick(MY_FLOAT input)
{
  lastOutput = effectMix * delayLine->tick(input);
  lastOutput += input * (1.0 - effectMix);
  return lastOutput;
}

MY_FLOAT *Echo :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>

Envelope :: Envelope(void) : Stk()
{
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 0.001;
  m_target = 1.0;
  m_time = m_target / (rate * Stk::sampleRate());
  state = 0;
}

Envelope :: ~Envelope(void)
{    
}

void Envelope :: keyOn(void)
{
  target = (MY_FLOAT) m_target;
  if (value != target) state = 1;
  setTime( m_time );
}

void Envelope :: keyOff(void)
{
  target = (MY_FLOAT) 0.0;
  if (value != target) state = 1;
  setTime( m_time );
}

void Envelope :: setRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("[chuck](via Envelope): negative rates not allowed ... correcting!\n");
    rate = -aRate;
  }
  else
    rate = aRate;
    
  m_time = (target - value) / (rate * Stk::sampleRate());
  if( m_time < 0.0 ) m_time = -m_time;
}

void Envelope :: setTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("[chuck](via Envelope): negative times not allowed ... correcting!\n");
    aTime = -aTime;
  }
  
  if( aTime == 0.0 )
    rate = FLT_MAX;
  else
    rate = (target - value) / (aTime * Stk::sampleRate());

  // rate
  if( rate < 0 ) rate = -rate;

  // should >= 0
  m_time = aTime;
}

void Envelope :: setTarget(MY_FLOAT aTarget)
{
  target = m_target = aTarget;
  if (value != target) state = 1;
  
  // set time
  setTime( m_time );
}

void Envelope :: setValue(MY_FLOAT aValue)
{
  state = 0;
  target = aValue;
  value = aValue;
}

int Envelope :: getState(void) const
{
  return state;
}

MY_FLOAT Envelope :: tick(void)
{
  if (state) {
    if (target > value) {
      value += rate;
      if (value >= target) {
        value = target;
        state = 0;
      }
    }
    else {
      value -= rate;
      if (value <= target) {
        value = target;
        state = 0;
      }
    }
  }
  return value;
}

MY_FLOAT *Envelope :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}

MY_FLOAT Envelope :: lastOut(void) const
{
  return value;
}

/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>

FM :: FM(int operators)
  : nOperators(operators)
{
  if ( nOperators <= 0 ) {
    char msg[256];
    sprintf(msg, "[chuck](via FM): Invalid number of operators (%d) argument to constructor!", operators);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  twozero = new TwoZero();
  twozero->setB2( -1.0 );
  twozero->setGain( 0.0 );

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency(6.0);

  int i;
  ratios = (MY_FLOAT *) new MY_FLOAT[nOperators];
  gains = (MY_FLOAT *) new MY_FLOAT[nOperators];
  adsr = (ADSR **) calloc( nOperators, sizeof(ADSR *) );
  waves = (WaveLoop **) calloc( nOperators, sizeof(WaveLoop *) );
  for (i=0; i<nOperators; i++ ) {
    ratios[i] = 1.0;
    gains[i] = 1.0;
    adsr[i] = new ADSR();
  }

  modDepth = (MY_FLOAT) 0.0;
  control1 = (MY_FLOAT) 1.0;
  control2 = (MY_FLOAT) 1.0;
  baseFrequency = (MY_FLOAT) 440.0;
  
  // chuck
  m_frequency = baseFrequency;

  MY_FLOAT temp = 1.0;
  for (i=99; i>=0; i--) {
    __FM_gains[i] = temp;
    temp *= 0.933033;
  }

  temp = 1.0;
  for (i=15; i>=0; i--) {
    __FM_susLevels[i] = temp;
    temp *= 0.707101;
  }

  temp = 8.498186;
  for (i=0; i<32; i++) {
    __FM_attTimes[i] = temp;
    temp *= 0.707101;
  }
}

FM :: ~FM()
{
  delete vibrato;
  delete twozero;

  delete [] ratios;
  delete [] gains;
  for (int i=0; i<nOperators; i++ ) {
    delete adsr[i];
    delete waves[i];
  }

  free(adsr);
  free(waves);
}

void FM :: loadWaves(const char **filenames )
{
  for (int i=0; i<nOperators; i++ )
    waves[i] = new WaveLoop( filenames[i], TRUE );
}

void FM :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;

  for (int i=0; i<nOperators; i++ )
    waves[i]->setFrequency( baseFrequency * ratios[i] );

  // chuck
  m_frequency = baseFrequency;
}

void FM :: setRatio(int waveIndex, MY_FLOAT ratio)
{
  if ( waveIndex < 0 ) {
    CK_STDCERR << "[chuck](via STK): FM: setRatio waveIndex parameter is less than zero!" << CK_STDENDL;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    CK_STDCERR << "[chuck](via STK): FM: setRatio waveIndex parameter is greater than the number of operators!" << CK_STDENDL;
    return;
  }

  ratios[waveIndex] = ratio;
  if (ratio > 0.0) 
    waves[waveIndex]->setFrequency(baseFrequency * ratio);
  else
    waves[waveIndex]->setFrequency(ratio);
}

void FM :: setGain(int waveIndex, MY_FLOAT gain)
{
  if ( waveIndex < 0 ) {
    CK_STDCERR << "[chuck](via STK): FM: setGain waveIndex parameter is less than zero!" << CK_STDENDL;
    return;
  }
  else if ( waveIndex >= nOperators ) {
    CK_STDCERR << "[chuck](via STK): FM: setGain waveIndex parameter is greater than the number of operators!" << CK_STDENDL;
    return;
  }

  gains[waveIndex] = gain;
}

void FM :: setModulationSpeed(MY_FLOAT mSpeed)
{
  vibrato->setFrequency(mSpeed);
}

void FM :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth;
}

void FM :: setControl1(MY_FLOAT cVal)
{
  control1 = cVal * (MY_FLOAT) 2.0;
}

void FM :: setControl2(MY_FLOAT cVal)
{
  control2 = cVal * (MY_FLOAT) 2.0;
}

void FM :: keyOn()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOn();
}

void FM :: keyOff()
{
  for (int i=0; i<nOperators; i++ )
    adsr[i]->keyOff();
}

void FM :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): FM: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void FM :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): FM: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): FM: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_Breath_) // 2
    setControl1( norm );
  else if (number == __SK_FootControl_) // 4
    setControl2( norm );
  else if (number == __SK_ModFrequency_) // 11
    setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    //adsr[0]->setTarget( norm );
    adsr[1]->setTarget( norm );
    //adsr[2]->setTarget( norm );
    adsr[3]->setTarget( norm );
  }
  else
    CK_STDCERR << "[chuck](via STK): FM: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): FM: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


FMVoices :: FMVoices()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 2.00);
  this->setRatio(1, 4.00);
  this->setRatio(2, 12.0);
  this->setRatio(3, 1.00);

  gains[3] = __FM_gains[80];

  adsr[0]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[1]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[2]->setAllTimes( 0.05, 0.05, __FM_susLevels[15], 0.05);
  adsr[3]->setAllTimes( 0.01, 0.01, __FM_susLevels[15], 0.5);

  twozero->setGain( 0.0 );
  modDepth = (MY_FLOAT) 0.005;
  currentVowel = 0;
  tilt[0] = 1.0;
  tilt[1] = 0.5;
  tilt[2] = 0.2;    
  mods[0] = 1.0;
  mods[1] = 1.1;
  mods[2] = 1.1;
  baseFrequency = 110.0;
  setFrequency( 110.0 );    
}  

FMVoices :: ~FMVoices()
{
}

void FMVoices :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT temp, temp2 = 0.0;
  int tempi = 0;
  unsigned int i = 0;

  if (currentVowel < 32)    {
    i = currentVowel;
    temp2 = (MY_FLOAT) 0.9;
  }
  else if (currentVowel < 64)   {
    i = currentVowel - 32;
    temp2 = (MY_FLOAT) 1.0;
  }
  else if (currentVowel < 96)   {
    i = currentVowel - 64;
    temp2 = (MY_FLOAT) 1.1;
  }
  else if (currentVowel <= 128) {
    i = currentVowel - 96;
    temp2 = (MY_FLOAT) 1.2;
  }

  baseFrequency = frequency;
  temp = (temp2 * Phonemes::formantFrequency(i, 0) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(0,(MY_FLOAT) tempi);
  temp = (temp2 * Phonemes::formantFrequency(i, 1) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(1,(MY_FLOAT) tempi);
  temp = (temp2 * Phonemes::formantFrequency(i, 2) / baseFrequency) + 0.5;
  tempi = (int) temp;
  this->setRatio(2, (MY_FLOAT) tempi);    
  gains[0] = 1.0;
  gains[1] = 1.0;
  gains[2] = 1.0;

  // chuck
  m_frequency = baseFrequency;
}

void FMVoices :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  tilt[0] = amplitude;
  tilt[1] = amplitude * amplitude;
  tilt[2] = tilt[1] * amplitude;
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): FMVoices: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT FMVoices :: tick()
{
  register MY_FLOAT temp, temp2;

  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  temp2 = vibrato->tick() * modDepth * (MY_FLOAT) 0.1;

  waves[0]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * (1.0 + temp2) * ratios[3]);

  waves[0]->addPhaseOffset(temp * mods[0]);
  waves[1]->addPhaseOffset(temp * mods[1]);
  waves[2]->addPhaseOffset(temp * mods[2]);
  waves[3]->addPhaseOffset(twozero->lastOut());
  twozero->tick(temp);
  temp =  gains[0] * tilt[0] * adsr[0]->tick() * waves[0]->tick();
  temp += gains[1] * tilt[1] * adsr[1]->tick() * waves[1]->tick();
  temp += gains[2] * tilt[2] * adsr[2]->tick() * waves[2]->tick();
    
  return temp * 0.33;
}

void FMVoices :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): FMVoices: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): FMVoices: Control value exceeds nominal range!" << CK_STDENDL;
  }


  if (number == __SK_Breath_) // 2
    gains[3] = __FM_gains[(int) ( norm * 99.9 )];
  else if (number == __SK_FootControl_) { // 4
    currentVowel = (int) (norm * 128.0);
    this->setFrequency(baseFrequency);
  }
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_) { // 128
    tilt[0] = norm;
    tilt[1] = norm * norm;
    tilt[2] = tilt[1] * norm;
  }
  else
    CK_STDCERR << "[chuck](via STK): FMVoices: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): FMVoices: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class Filter
    \brief STK filter class.

    This class implements a generic structure which
    can be used to create a wide range of filters.
    It can function independently or be subclassed
    to provide more specific controls based on a
    particular filter type.

    In particular, this class implements the standard
    difference equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>

FilterStk :: FilterStk()
{
  // The default constructor should setup for pass-through.
  gain = 1.0;
  nB = 1;
  nA = 1;
  b = new MY_FLOAT[nB];
  b[0] = 1.0;
  a = new MY_FLOAT[nA];
  a[0] = 1.0;

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();
}

FilterStk :: FilterStk(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  gain = 1.0;
  nB = nb;
  nA = na;
  b = new MY_FLOAT[nB];
  a = new MY_FLOAT[nA];

  inputs = new MY_FLOAT[nB];
  outputs = new MY_FLOAT[nA];
  this->clear();

  this->setCoefficients(nB, bCoefficients, nA, aCoefficients);
}

FilterStk :: ~FilterStk()
{
  delete [] b;
  delete [] a;
  delete [] inputs;
  delete [] outputs;
}

void FilterStk :: clear(void)
{
  int i;
  for (i=0; i<nB; i++)
    inputs[i] = 0.0;
  for (i=0; i<nA; i++)
    outputs[i] = 0.0;
}

void FilterStk :: setCoefficients(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 || na < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) and na (%d) must be >= 1!", nb, na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary
  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void FilterStk :: setNumerator(int nb, MY_FLOAT *bCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( nb < 1 ) {
    sprintf(message, "[chuck](via Filter): nb (%d) must be >= 1!", nb);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (nb != nB) {
    delete [] b;
    delete [] inputs;
    nB = nb;
    b = new MY_FLOAT[nB];
    inputs = new MY_FLOAT[nB];
    for (i=0; i<nB; i++) inputs[i] = 0.0;
  }

  for (i=0; i<nB; i++)
    b[i] = bCoefficients[i];
}

void FilterStk :: setDenominator(int na, MY_FLOAT *aCoefficients)
{
  int i;
  char message[256];

  // Check the arguments.
  if ( na < 1 ) {
    sprintf(message, "[chuck](via Filter): na (%d) must be >= 1!", na);
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if ( aCoefficients[0] == 0.0 ) {
    sprintf(message, "[chuck](via Filter): a[0] coefficient cannot == 0!");
    handleError( message, StkError::FUNCTION_ARGUMENT );
  }

  if (na != nA) {
    delete [] a;
    delete [] outputs;
    nA = na;
    a = new MY_FLOAT[nA];
    outputs = new MY_FLOAT[nA];
    for (i=0; i<nA; i++) outputs[i] = 0.0;
  }

  for (i=0; i<nA; i++)
    a[i] = aCoefficients[i];

  // scale coefficients by a[0] if necessary

  if (a[0] != 1.0) {
    for (i=0; i<nB; i++)
      b[i] /= a[0];
    for (i=0; i<nA; i++)
      a[i] /= a[0];
  }
}

void FilterStk :: setGain(MY_FLOAT theGain)
{
  gain = theGain;
}

MY_FLOAT FilterStk :: getGain(void) const
{
  return gain;
}

MY_FLOAT FilterStk :: lastOut(void) const
{
  return outputs[0];
}

MY_FLOAT FilterStk :: tick(MY_FLOAT sample)
{
  int i;

  outputs[0] = 0.0;
  inputs[0] = gain * sample;
  for (i=nB-1; i>0; i--) {
    outputs[0] += b[i] * inputs[i];
    inputs[i] = inputs[i-1];
  }
  outputs[0] += b[0] * inputs[0];

  for (i=nA-1; i>0; i--) {
    outputs[0] += -a[i] * outputs[i];
    outputs[i] = outputs[i-1];
  }

  return outputs[0];
}

MY_FLOAT *FilterStk :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}


/***************************************************/
/*! \class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Flute :: Flute(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  boreDelay = new DelayL( 100.0, length );
  length >>= 1;
  jetDelay = new DelayL( 49.0, length );
  jetTable = new JetTabl();
  filter = new OnePole();
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();
  noise = new Noise();
  adsr = new ADSR();

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 5.925 );

  this->clear();

  filter->setPole( 0.7 - ((MY_FLOAT) 0.1 * 22050.0 / Stk::sampleRate() ) );
  filter->setGain( -1.0 );
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010);
  endReflection = (MY_FLOAT) 0.5;
  jetReflection = (MY_FLOAT) 0.5;
  noiseGain =  0.15;             // Breath pressure random component.
  vibratoGain = (MY_FLOAT) 0.05; // Breath periodic vibrato component.
  jetRatio = (MY_FLOAT) 0.32;

  maxPressure = (MY_FLOAT) 0.0;
  lastFrequency = 220.0;

  // CHUCK
  //reverse: nothing
  m_frequency = lastFrequency;
  //reverse: nothing (see setJetDelay)
  m_jetDelay = jetRatio;
  //reverse: nothing
  m_jetReflection = jetReflection;
  //reverse: nothing
  m_endReflection = endReflection;
  //reverse: noiseGain = norm * 0.4
  m_noiseGain = noiseGain / .4;
  //reverse: same * 12.0
  m_vibratoFreq = vibrato->m_freq / 12.0;
  //reverse: same * 0.4
  m_vibratoGain = .15 / 0.4;
  //reverse: nothing
  m_pressure = 1.0;
  //reverse: nothing
  m_rate = .005;
}

Flute :: ~Flute()
{
  delete jetDelay;
  delete boreDelay;
  delete jetTable;
  delete filter;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void Flute :: clear()
{
  jetDelay->clear();
  boreDelay->clear();
  filter->clear();
  dcBlock->clear();
}

void Flute :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Flute: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    lastFrequency = 220.0;
  }

  // We're overblowing here.
  lastFrequency *= 0.66666;
  // Delay = length - approximate filter delay.
  MY_FLOAT delay = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  boreDelay->setDelay(delay);
  jetDelay->setDelay(delay * jetRatio);

  // chuck
  m_frequency = frequency;
}

void Flute :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Flute: startblowing " << amplitude << " " << rate << CK_STDENDL;
#endif
  adsr->setAttackRate(rate);
  maxPressure = amplitude / (MY_FLOAT) 0.8;
  adsr->keyOn();
}

void Flute :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Flute :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Flute: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Flute :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Flute: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Flute :: setJetReflection(MY_FLOAT coefficient)
{
  jetReflection = coefficient;
  m_jetReflection = coefficient;
}

void Flute :: setEndReflection(MY_FLOAT coefficient)
{
  endReflection = coefficient;
  m_endReflection = coefficient;
}

void Flute :: setJetDelay(MY_FLOAT aRatio)
{
  // Delay = length - approximate filter delay.
  MY_FLOAT temp = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  jetRatio = aRatio;
  jetDelay->setDelay(temp * aRatio); // Scaled by ratio.
  m_jetDelay = aRatio;
}

MY_FLOAT Flute :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  MY_FLOAT temp = filter->tick( boreDelay->lastOut() );
  temp = dcBlock->tick(temp); // Block DC on reflection.

  pressureDiff = breathPressure - (jetReflection * temp);
  pressureDiff = jetDelay->tick( pressureDiff );
  pressureDiff = jetTable->tick( pressureDiff ) + (endReflection * temp);
  lastOutput = (MY_FLOAT) 0.3 * boreDelay->tick( pressureDiff );

  lastOutput *= outputGain;
  return lastOutput;
}

void Flute :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Flute: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Flute: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_JetDelay_) // 2
    this->setJetDelay( (MY_FLOAT) (0.08 + (0.48 * norm)) );
  else if (number == __SK_NoiseLevel_) { // 4
    m_noiseGain = norm;
    noiseGain = ( norm * 0.4);
  }
  else if (number == __SK_ModFrequency_) // 11
    setVibratoFreq( norm * 12.0 );
  else if (number == __SK_ModWheel_) { // 1
    m_vibratoGain = norm;
    vibratoGain = ( norm * 0.4 );
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    m_pressure = norm;
    adsr->setTarget( norm );
  }
  else
    CK_STDCERR << "[chuck](via STK): Flute: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Flute: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


FormSwep :: FormSwep() : BiQuad()
{
  frequency = (MY_FLOAT) 0.0;
  radius = (MY_FLOAT) 0.0;
  targetGain = (MY_FLOAT) 1.0;
  targetFrequency = (MY_FLOAT) 0.0;
  targetRadius = (MY_FLOAT) 0.0;
  deltaGain = (MY_FLOAT) 0.0;
  deltaFrequency = (MY_FLOAT) 0.0;
  deltaRadius = (MY_FLOAT) 0.0;
  sweepState = (MY_FLOAT)  0.0;
  sweepRate = (MY_FLOAT) 0.002;
  dirty = false;
  this->clear();
}

FormSwep :: ~FormSwep()
{
}

void FormSwep :: setResonance(MY_FLOAT aFrequency, MY_FLOAT aRadius)
{
  dirty = false;
  radius = aRadius;
  frequency = aFrequency;

  BiQuad::setResonance( frequency, radius, true );
}

void FormSwep :: setStates(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = false;

  if ( frequency != aFrequency || radius != aRadius )
    BiQuad::setResonance( aFrequency, aRadius, true );

  frequency = aFrequency;
  radius = aRadius;
  gain = aGain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
}

void FormSwep :: setTargets(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = true;
  startFrequency = frequency;
  startRadius = radius;
  startGain = gain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
  deltaFrequency = aFrequency - frequency;
  deltaRadius = aRadius - radius;
  deltaGain = aGain - gain;
  sweepState = (MY_FLOAT) 0.0;
}

void FormSwep :: setSweepRate(MY_FLOAT aRate)
{
  sweepRate = aRate;
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

void FormSwep :: setSweepTime(MY_FLOAT aTime)
{
  sweepRate = 1.0 / ( aTime * Stk::sampleRate() );
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

MY_FLOAT FormSwep :: tick(MY_FLOAT sample)
{                                     
  if (dirty)  {
    sweepState += sweepRate;
    if ( sweepState >= 1.0 )   {
      sweepState = (MY_FLOAT) 1.0;
      dirty = false;
      radius = targetRadius;
      frequency = targetFrequency;
      gain = targetGain;
    }
    else  {
      radius = startRadius + (deltaRadius * sweepState);
      frequency = startFrequency + (deltaFrequency * sweepState);
      gain = startGain + (deltaGain * sweepState);
    }
    BiQuad::setResonance( frequency, radius, true );
  }

  return BiQuad::tick( sample );
}

MY_FLOAT *FormSwep :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    Algorithm 3 is :     4--\
                    3-->2-- + -->1-->Out

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


HevyMetl :: HevyMetl()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0 * 1.000);
  this->setRatio(1, 4.0 * 0.999);
  this->setRatio(2, 3.0 * 1.001);
  this->setRatio(3, 0.5 * 1.002);

  gains[0] = __FM_gains[92];
  gains[1] = __FM_gains[76];
  gains[2] = __FM_gains[91];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 0.001, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.001, 0.010, 1.0, 0.50);
  adsr[2]->setAllTimes( 0.010, 0.005, 1.0, 0.20);
  adsr[3]->setAllTimes( 0.030, 0.010, 0.2, 0.20);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 5.5 );
  modDepth = 0.0;
}  

HevyMetl :: ~HevyMetl()
{
}

void HevyMetl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[92];
  gains[1] = amplitude * __FM_gains[76];
  gains[2] = amplitude * __FM_gains[91];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "HevyMetl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT HevyMetl :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * 0.2;    
  waves[0]->setFrequency(baseFrequency * (1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * (1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * (1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * (1.0 + temp) * ratios[3]);
    
  temp = gains[2] * adsr[2]->tick() * waves[2]->tick();
  waves[1]->addPhaseOffset(temp);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = (1.0 - (control2 * 0.5)) * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);
    
  temp += control2 * (MY_FLOAT) 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;
    
  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Instrmnt :: Instrmnt()
{
    m_frequency = 0;
}

Instrmnt :: ~Instrmnt()
{
}

void Instrmnt :: setFrequency(MY_FLOAT frequency)
{
  CK_STDCERR << "[chuck](via STK): Instrmnt: virtual setFrequency function call!" << CK_STDENDL;
  // m_frequency = frequency;
}

MY_FLOAT Instrmnt :: lastOut() const
{
  return lastOutput;
}

// Support for stereo output:
MY_FLOAT Instrmnt :: lastOutLeft(void) const
{
  return 0.5 * lastOutput;
}
                                                                                
MY_FLOAT Instrmnt :: lastOutRight(void) const
{
  return 0.5 * lastOutput;
}

MY_FLOAT *Instrmnt :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}

/*
TODO :  let's add this as two function in Chuck.
     :  one version is a ( int , float ) for the midi messages
     :  the second is a ( string, float ) that does a quick binary search
     :  into the skini table for the __SK_ value, and dispatches the proper function
     :  hoohoo!

     - and then everyone can inherit from Instrmnt like the good Lord intended. 
     - pld
*/

void Instrmnt :: controlChange(int number, MY_FLOAT value)
{
}

/***************************************************/
/*! \class JCRev
    \brief John Chowning's reverberator class.

    This class is derived from the CLM JCRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This class implements three series
    allpass units, followed by four parallel comb
    filters, and two decorrelation delay lines in
    parallel at the output.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

JCRev :: JCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[9] = {1777, 1847, 1993, 2137, 389, 127, 43, 211, 179};
  double scaler = Stk::sampleRate() / 44100.0;

  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<9; i++) {
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<3; i++)
      allpassDelays[i] = new Delay(lengths[i+4], lengths[i+4]);

  for (i=0; i<4; i++)   {
    combDelays[i] = new Delay(lengths[i], lengths[i]);
    combCoefficient[i] = pow(10.0,(-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  outLeftDelay = new Delay(lengths[7], lengths[7]);
  outRightDelay = new Delay(lengths[8], lengths[8]);
  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

JCRev :: ~JCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete allpassDelays[2];
  delete combDelays[0];
  delete combDelays[1];
  delete combDelays[2];
  delete combDelays[3];
  delete outLeftDelay;
  delete outRightDelay;
}

void JCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  allpassDelays[2]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  combDelays[2]->clear();
  combDelays[3]->clear();
  outRightDelay->clear();
  outLeftDelay->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT JCRev :: tick(MY_FLOAT input)
{
    MY_FLOAT temp, temp0, temp1, temp2, temp3, temp4, temp5, temp6;
    MY_FLOAT filtout;

    // gewang: dedenormal
    CK_STK_DDN(input);

    temp = allpassDelays[0]->lastOut();
    temp0 = allpassCoefficient * temp;
    temp0 += input;
    // gewang: dedenormal
    CK_STK_DDN(temp0);
    allpassDelays[0]->tick(temp0);
    temp0 = -(allpassCoefficient * temp0) + temp;

    temp = allpassDelays[1]->lastOut();
    temp1 = allpassCoefficient * temp;
    temp1 += temp0;
    // gewang: dedenormal
    CK_STK_DDN(temp1);
    allpassDelays[1]->tick(temp1);
    temp1 = -(allpassCoefficient * temp1) + temp;

    temp = allpassDelays[2]->lastOut();
    temp2 = allpassCoefficient * temp;
    temp2 += temp1;
    // gewang: dedenormal
    CK_STK_DDN(temp2);
    allpassDelays[2]->tick(temp2);
    temp2 = -(allpassCoefficient * temp2) + temp;

    temp3 = temp2 + (combCoefficient[0] * combDelays[0]->lastOut());
    temp4 = temp2 + (combCoefficient[1] * combDelays[1]->lastOut());
    temp5 = temp2 + (combCoefficient[2] * combDelays[2]->lastOut());
    temp6 = temp2 + (combCoefficient[3] * combDelays[3]->lastOut());

    // gewang: dedenormal
    CK_STK_DDN(temp3);
    CK_STK_DDN(temp4);
    CK_STK_DDN(temp5);
    CK_STK_DDN(temp6);

    combDelays[0]->tick(temp3);
    combDelays[1]->tick(temp4);
    combDelays[2]->tick(temp5);
    combDelays[3]->tick(temp6);

    filtout = temp3 + temp4 + temp5 + temp6;

    // gewang: dedenormal
    CK_STK_DDN(filtout);

    lastOutput[0] = effectMix * (outLeftDelay->tick(filtout));
    lastOutput[1] = effectMix * (outRightDelay->tick(filtout));
    temp = (1.0 - effectMix) * input;
    lastOutput[0] += temp;
    lastOutput[1] += temp;

    return (lastOutput[0] + lastOutput[1]) * 0.5;
}


/***************************************************/
/*! \class JetTabl
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


JetTabl :: JetTabl()
{
  lastOutput = (MY_FLOAT) 0.0;
}

JetTabl :: ~JetTabl()
{
}

MY_FLOAT JetTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT JetTabl :: tick( MY_FLOAT input )
{
  // Perform "table lookup" using a polynomial
  // calculation (x^3 - x), which approximates
  // the jet sigmoid behavior.
  lastOutput = input * (input * input - (MY_FLOAT)  1.0);

  // Saturate at +/- 1.0.
  if (lastOutput > 1.0) 
    lastOutput = (MY_FLOAT) 1.0;
  if (lastOutput < -1.0)
    lastOutput = (MY_FLOAT) -1.0; 
  return lastOutput;
}

MY_FLOAT *JetTabl :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}

/***************************************************/
/*! \class Mandolin
    \brief STK mandolin instrument model class.

    This class inherits from PluckTwo and uses
    "commuted synthesis" techniques to model a
    mandolin instrument.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    Commuted Synthesis, in particular, is covered
    by patents, granted, pending, and/or
    applied-for.  All are assigned to the Board of
    Trustees, Stanford University.  For
    information, contact the Office of Technology
    Licensing, Stanford University.

    Control Change Numbers: 
       - Body Size = 2
       - Pluck Position = 4
       - String Sustain = 11
       - String Detuning = 1
       - Microphone Position = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Mandolin :: Mandolin(MY_FLOAT lowestFrequency)
  : PluckTwo(lowestFrequency)
{
  // Concatenate the STK rawwave path to the rawwave files
  soundfile[0] = new WvIn( "special:mand1", TRUE );
  /*
  soundfile[1] = new WvIn( "special:mand1", TRUE );
  soundfile[2] = new WvIn( "special:mand1", TRUE );
  soundfile[3] = new WvIn( "special:mand1", TRUE );
  soundfile[4] = new WvIn( "special:mand1", TRUE );
  soundfile[5] = new WvIn( "special:mand1", TRUE );
  soundfile[6] = new WvIn( "special:mand1", TRUE );
  soundfile[7] = new WvIn( "special:mand1", TRUE );
  soundfile[8] = new WvIn( "special:mand1", TRUE );
  soundfile[9] = new WvIn( "special:mand1", TRUE );
  soundfile[10] = new WvIn( "special:mand1", TRUE );
  soundfile[11] = new WvIn( "special:mand1", TRUE );
  */
  for( int i = 1; i <= 11; i ++ )
      soundfile[i] = soundfile[0];

  directBody = 1.0;
  mic = 0;
  dampTime = 0;
  // chuck: don't play
  waveDone = true;
  //waveDone = soundfile[mic]->isFinished();
  //reverse: nothing
  m_bodySize = 1.0;
}

bool Mandolin :: setBodyIR( const char * path, bool isRaw )
{
    soundfile[0]->openFile( path, isRaw );
    return true;
}

Mandolin :: ~Mandolin()
{
//    for( int i=0; i<12; i++ )
//        SAFE_DELETE( soundfile[i] );
    
    // chuck: all the soundfiles are the same object, only delete one of them 
    SAFE_DELETE(soundfile[0]);
    for( int i=1; i<12; i++ )
        soundfile[i] = NULL;
}

void Mandolin :: pluck(MY_FLOAT amplitude)
{
  // This function gets interesting, because pluck
  // may be longer than string length, so we just
  // reset the soundfile and add in the pluck in
  // the tick method.
  soundfile[mic]->reset();
  waveDone = false;
  pluckAmplitude = amplitude;
  if ( amplitude < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Mandolin: pluck amplitude parameter less than zero!" << CK_STDENDL;
    pluckAmplitude = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Mandolin: pluck amplitude parameter greater than 1.0!" << CK_STDENDL;
    pluckAmplitude = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay((MY_FLOAT) 0.5 * pluckPosition * lastLength); 
  dampTime = (long) lastLength;   // See tick method below.
}

void Mandolin :: pluck(MY_FLOAT amplitude, MY_FLOAT position)
{
  // Pluck position puts zeroes at position * length.
  pluckPosition = position;
  if ( position < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Mandolin: pluck position parameter less than zero!" << CK_STDENDL;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Mandolin: pluck position parameter greater than 1.0!" << CK_STDENDL;
    pluckPosition = 1.0;
  }

  this->pluck(amplitude);
}

void Mandolin :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Mandolin: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Mandolin :: setBodySize(MY_FLOAT size)
{
  m_bodySize = size;
  // Scale the commuted body response by its sample rate (22050).
  MY_FLOAT rate = size * soundfile[mic]->sampleRate() / Stk::sampleRate();
  for ( int i=0; i<12; i++ )
    soundfile[i]->setRate(rate);
}

MY_FLOAT Mandolin :: tick()
{
  MY_FLOAT temp = 0.0;
  if ( !waveDone ) {
    // Scale the pluck excitation with comb
    // filtering for the duration of the file.
    temp = soundfile[mic]->tick() * pluckAmplitude;
    temp = temp - combDelay->tick(temp);
    waveDone = soundfile[mic]->isFinished();
  }

  // Damping hack to help avoid overflow on re-plucking.
  if ( dampTime >=0 ) {
    dampTime -= 1;
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * (MY_FLOAT) 0.7) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * (MY_FLOAT) 0.7) ) );
  }
  else { // No damping hack after 1 period.
    // Calculate 1st delay filtered reflection plus pluck excitation.
    lastOutput = delayLine->tick( filter->tick( temp + (delayLine->lastOut() * loopGain) ) );
    // Calculate 2nd delay just like the 1st.
    lastOutput += delayLine2->tick( filter2->tick( temp + (delayLine2->lastOut() * loopGain) ) );
  }

  lastOutput *= (MY_FLOAT) 0.3;
  return lastOutput;
}

void Mandolin :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Mandolin: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Mandolin: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_BodySize_) // 2
    this->setBodySize( norm * 2.0 );
  else if (number == __SK_PickPosition_) // 4
    this->setPluckPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    this->setBaseLoopGain((MY_FLOAT) 0.97 + (norm * (MY_FLOAT) 0.03));
  else if (number == __SK_StringDetune_) // 1
    this->setDetune((MY_FLOAT) 1.0 - (norm * (MY_FLOAT) 0.1));
  else if (number == __SK_AfterTouch_Cont_) // 128
    mic = (int) (norm * 11.0);
  else
    CK_STDCERR << "[chuck](via STK): Mandolin: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Mandolin: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class Mesh2D
    \brief Two-dimensional rectilinear waveguide mesh class.

    This class implements a rectilinear,
    two-dimensional digital waveguide mesh
    structure.  For details, see Van Duyne and
    Smith, "Physical Modeling with the 2-D Digital
    Waveguide Mesh", Proceedings of the 1993
    International Computer Music Conference.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - X Dimension = 2
       - Y Dimension = 4
       - Mesh Decay = 11
       - X-Y Input Position = 1

    by Julius Smith, 2000 - 2002.
    Revised by Gary Scavone for STK, 2002.
*/
/***************************************************/

#include <stdlib.h>

Mesh2D :: Mesh2D(short nX, short nY)
{
  this->setNX(nX);
  this->setNY(nY);

  MY_FLOAT pole = 0.05;

  short i;
  for (i=0; i<NYMAX; i++) {
    filterY[i] = new OnePole(pole);
    filterY[i]->setGain(0.99);
  }

  for (i=0; i<NXMAX; i++) {
    filterX[i] = new OnePole(pole);
    filterX[i]->setGain(0.99);
  }

  this->clearMesh();

  counter=0;
  xInput = 0;
  yInput = 0;
}

Mesh2D :: ~Mesh2D()
{
  short i;
  for (i=0; i<NYMAX; i++)
    delete filterY[i];

  for (i=0; i<NXMAX; i++)
    delete filterX[i];
}

void Mesh2D :: clear()
{
  this->clearMesh();

  short i;
  for (i=0; i<NY; i++)
    filterY[i]->clear();

  for (i=0; i<NX; i++)
    filterX[i]->clear();

  counter=0;
}

void Mesh2D :: clearMesh()
{
  int x, y;
  for (x=0; x<NXMAX-1; x++) {
    for (y=0; y<NYMAX-1; y++) {
      v[x][y] = 0;
    }
  }
  for (x=0; x<NXMAX; x++) {
    for (y=0; y<NYMAX; y++) {

      vxp[x][y] = 0;
      vxm[x][y] = 0;
      vyp[x][y] = 0;
      vym[x][y] = 0;

      vxp1[x][y] = 0;
      vxm1[x][y] = 0;
      vyp1[x][y] = 0;
      vym1[x][y] = 0;
    }
  }
}

MY_FLOAT Mesh2D :: energy()
{
  // Return total energy contained in wave variables Note that some
  // energy is also contained in any filter delay elements.

  int x, y;
  MY_FLOAT t;
  MY_FLOAT e = 0;
  if ( counter & 1 ) { // Ready for Mesh2D::tick1() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp1[x][y];
        e += t*t;
        t = vxm1[x][y];
        e += t*t;
        t = vyp1[x][y];
        e += t*t;
        t = vym1[x][y];
        e += t*t;
      }
    }
  }
  else { // Ready for Mesh2D::tick0() to be called.
    for (x=0; x<NX; x++) {
      for (y=0; y<NY; y++) {
        t = vxp[x][y];
        e += t*t;
        t = vxm[x][y];
        e += t*t;
        t = vyp[x][y];
        e += t*t;
        t = vym[x][y];
        e += t*t;
      }
    }
  }

  return(e);
}

void Mesh2D :: setNX(short lenX)
{
  NX = lenX;
  if ( lenX < 2 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setNX(" << lenX << "): Minimum length is 2!" << CK_STDENDL;
    NX = 2;
  }
  else if ( lenX > NXMAX ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setNX(" << lenX << "): Maximum length is " << NXMAX << "!" << CK_STDENDL;
    NX = NXMAX;
  }
}

void Mesh2D :: setNY(short lenY)
{
  NY = lenY;
  if ( lenY < 2 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setNY(" << lenY << "): Minimum length is 2!" << CK_STDENDL;
    NY = 2;
  }
  else if ( lenY > NYMAX ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setNY(" << lenY << "): Maximum length is " << NYMAX << "!" << CK_STDENDL;
    NY = NYMAX;
  }
}

void Mesh2D :: setDecay(MY_FLOAT decayFactor)
{
  MY_FLOAT gain = decayFactor;
  if ( decayFactor < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setDecay decayFactor value is less than 0.0!" << CK_STDENDL;
    gain = 0.0;
  }
  else if ( decayFactor > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setDecay decayFactor value is greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }

  int i;
  for (i=0; i<NYMAX; i++)
    filterY[i]->setGain(gain);

  for (i=0; i<NXMAX; i++)
    filterX[i]->setGain(gain);
}

void Mesh2D :: setInputPosition(MY_FLOAT xFactor, MY_FLOAT yFactor)
{
  if ( xFactor < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setInputPosition xFactor value is less than 0.0!" << CK_STDENDL;
    xInput = 0;
  }
  else if ( xFactor > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setInputPosition xFactor value is greater than 1.0!" << CK_STDENDL;
    xInput = NX - 1;
  }
  else
    xInput = (short) (xFactor * (NX - 1));

  if ( yFactor < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setInputPosition yFactor value is less than 0.0!" << CK_STDENDL;
    yInput = 0;
  }
  else if ( yFactor > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Mesh2D::setInputPosition yFactor value is greater than 1.0!" << CK_STDENDL;
    yInput = NY - 1;
  }
  else
    yInput = (short) (yFactor * (NY - 1));
}

void Mesh2D :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Input at corner.
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += amplitude;
    vyp1[xInput][yInput] += amplitude;
  }
  else {
    vxp[xInput][yInput] += amplitude;
    vyp[xInput][yInput] += amplitude;
  }

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Mesh2D: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Mesh2D :: noteOff(MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Mesh2D: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Mesh2D :: tick(MY_FLOAT input)
{
  if ( counter & 1 ) {
    vxp1[xInput][yInput] += input;
    vyp1[xInput][yInput] += input;
    lastOutput = tick1();
  }
  else {
    vxp[xInput][yInput] += input;
    vyp[xInput][yInput] += input;
    lastOutput = tick0();
  }

  counter++;
  return lastOutput;
}

MY_FLOAT Mesh2D :: tick()
{
  lastOutput = ((counter & 1) ? this->tick1() : this->tick0());
  counter++;
  return lastOutput;
}

#define VSCALE ((MY_FLOAT) (0.5))

MY_FLOAT Mesh2D :: tick0()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp[x][y] + vxm[x+1][y] + 
          vyp[x][y] + vym[x][y+1] ) * VSCALE;
    }
  }    

  // Update junction outgoing waves, using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];
      // Update positive-going waves.
      vxp1[x+1][y] = vxy - vxm[x+1][y];
      vyp1[x][y+1] = vxy - vym[x][y+1];
      // Update minus-going waves.
      vxm1[x][y] = vxy - vxp[x][y];
      vym1[x][y] = vxy - vyp[x][y];
    }
  }    

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp1[0][y] = filterY[y]->tick(vxm[0][y]);
    vxm1[NX-1][y] = vxp[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp1[x][0] = filterX[x]->tick(vym[x][0]);
    vym1[x][NY-1] = vyp[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.  Note that the last
  // index in each coordinate direction is used only with the other
  // coordinate indices at their next-to-last values.  This is because
  // the "unit strings" attached to each velocity node to terminate
  // the mesh are not themselves connected together.
  outsamp = vxp[NX-1][NY-2] + vyp[NX-2][NY-1];

  return outsamp;
}

MY_FLOAT Mesh2D :: tick1()
{
  int x, y;
  MY_FLOAT outsamp = 0;

  // Update junction velocities.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      v[x][y] = ( vxp1[x][y] + vxm1[x+1][y] + 
          vyp1[x][y] + vym1[x][y+1] ) * VSCALE;
    }
  }

  // Update junction outgoing waves, 
  // using alternate wave-variable buffers.
  for (x=0; x<NX-1; x++) {
    for (y=0; y<NY-1; y++) {
      MY_FLOAT vxy = v[x][y];

      // Update positive-going waves.
      vxp[x+1][y] = vxy - vxm1[x+1][y];
      vyp[x][y+1] = vxy - vym1[x][y+1];

      // Update minus-going waves.
      vxm[x][y] = vxy - vxp1[x][y];
      vym[x][y] = vxy - vyp1[x][y];
    }
  }

  // Loop over velocity-junction boundary faces, update edge
  // reflections, with filtering.  We're only filtering on one x and y
  // edge here and even this could be made much sparser.
  for (y=0; y<NY-1; y++) {
    vxp[0][y] = filterY[y]->tick(vxm1[0][y]);
    vxm[NX-1][y] = vxp1[NX-1][y];
  }
  for (x=0; x<NX-1; x++) {
    vyp[x][0] = filterX[x]->tick(vym1[x][0]);
    vym[x][NY-1] = vyp1[x][NY-1];
  }

  // Output = sum of outgoing waves at far corner.
  outsamp = vxp1[NX-1][NY-2] + vyp1[NX-2][NY-1];

  return outsamp;
}

void Mesh2D :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Mesh2D: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Mesh2D: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == 2) // 2
    setNX( (short) (norm * (NXMAX-2) + 2) );
  else if (number == 4) // 4
    setNY( (short) (norm * (NYMAX-2) + 2) );
  else if (number == 11) // 11
    setDecay( 0.9 + (norm * 0.1) );
  else if (number == __SK_ModWheel_) // 1
    setInputPosition(norm, norm);
  else if (number == __SK_AfterTouch_Cont_) // 128
   ;
  else
    CK_STDCERR << "[chuck](via STK): Mesh2D: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Mesh2D: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}
/***************************************************/
/*! \class Modal
    \brief STK resonance model instrument.

    This class contains an excitation wavetable,
    an envelope, an oscillator, and N resonances
    (non-sweeping BiQuad filters), where N is set
    during instantiation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>

Modal :: Modal(int modes)
  : nModes(modes)
{
  if ( nModes <= 0 ) {
    char msg[256];
    sprintf(msg, "[chuck](via Modal): Invalid number of modes (%d) argument to constructor!", modes);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  // We don't make the excitation wave here yet, because we don't know
  // what it's going to be.

  ratios = (MY_FLOAT *) new MY_FLOAT[nModes];
  radii = (MY_FLOAT *) new MY_FLOAT[nModes];
  filters = (BiQuad **) calloc( nModes, sizeof(BiQuad *) );
  for (int i=0; i<nModes; i++ ) {
    filters[i] = new BiQuad;
    filters[i]->setEqualGainZeroes();
  }

  envelope = new Envelope;
  onepole = new OnePole;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );

  // Set some default values.
  vibrato->setFrequency( 6.0 );
  vibratoGain = 0.0;
  directGain = 0.0;
  masterGain = 1.0;
  baseFrequency = 440.0;

  this->clear();

  stickHardness =  0.5;
  strikePosition = 0.561;

  // chuck
  m_frequency = baseFrequency;
  m_vibratoGain = vibratoGain;
  m_vibratoFreq = vibrato->m_freq;
  m_volume = 1.0;
}  

Modal :: ~Modal()
{
  delete envelope; 
  delete onepole;
  delete vibrato;

  delete [] ratios;
  delete [] radii;
  for (int i=0; i<nModes; i++ ) {
    delete filters[i];
  }
  free(filters);
}

void Modal :: clear()
{    
  onepole->clear();
  for (int i=0; i<nModes; i++ )
    filters[i]->clear();
}

void Modal :: setFrequency(MY_FLOAT frequency)
{
  baseFrequency = frequency;
  for (int i=0; i<nModes; i++ )
    this->setRatioAndRadius(i, ratios[i], radii[i]);

  // chuck
  m_frequency = baseFrequency;
}

void Modal :: setRatioAndRadius(int modeIndex, MY_FLOAT ratio, MY_FLOAT radius)
{
  if ( modeIndex < 0 ) {
    CK_STDCERR << "[chuck](via STK): Modal: setRatioAndRadius modeIndex parameter is less than zero!" << CK_STDENDL;
    return;
  }
  else if ( modeIndex >= nModes ) {
    CK_STDCERR << "[chuck](via STK): Modal: setRatioAndRadius modeIndex parameter is greater than the number of operators!" << CK_STDENDL;
    return;
  }

  MY_FLOAT nyquist = Stk::sampleRate() / 2.0;
  MY_FLOAT temp;

  if (ratio * baseFrequency < nyquist) {
    ratios[modeIndex] = ratio;
  }
  else {
    temp = ratio;
    while (temp * baseFrequency > nyquist) temp *= (MY_FLOAT) 0.5;
    ratios[modeIndex] = temp;
#if defined(_STK_DEBUG_)
    CK_STDCERR << "[chuck](via STK): Modal : Aliasing would occur here ... correcting." << CK_STDENDL;
#endif
  }
  radii[modeIndex] = radius;
  if (ratio < 0) 
    temp = -ratio;
  else
    temp = ratio*baseFrequency;

  filters[modeIndex]->setResonance(temp, radius);
}

void Modal :: setMasterGain(MY_FLOAT aGain)
{
  masterGain = aGain;
}

void Modal :: setDirectGain(MY_FLOAT aGain)
{
  directGain = aGain;
}

void Modal :: setModeGain(int modeIndex, MY_FLOAT gain)
{
  if ( modeIndex < 0 ) {
    CK_STDCERR << "[chuck](via STK): Modal: setModeGain modeIndex parameter is less than zero!" << CK_STDENDL;
    return;
  }
  else if ( modeIndex >= nModes ) {
    CK_STDCERR << "[chuck](via STK): Modal: setModeGain modeIndex parameter is greater than the number of operators!" << CK_STDENDL;
    return;
  }

  filters[modeIndex]->setGain(gain);
}

void Modal :: strike(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( amplitude < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Modal: strike amplitude is less than zero!" << CK_STDENDL;
    gain = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Modal: strike amplitude is greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }

  envelope->setRate(1.0);
  envelope->setTarget(gain);
  onepole->setPole(1.0 - gain);
  envelope->tick();
  wave->reset();

  MY_FLOAT temp;
  for (int i=0; i<nModes; i++) {
    if (ratios[i] < 0)
      temp = -ratios[i];
    else
      temp = ratios[i] * baseFrequency;
    filters[i]->setResonance(temp, radii[i]);
  }
}

void Modal :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->strike(amplitude);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Modal: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Modal :: noteOff(MY_FLOAT amplitude)
{
  // This calls damp, but inverts the meaning of amplitude (high
  // amplitude means fast damping).
  this->damp(1.0 - (amplitude * 0.03));

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Modal: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Modal :: damp(MY_FLOAT amplitude)
{
  MY_FLOAT temp;
  for (int i=0; i<nModes; i++) {
    if (ratios[i] < 0)
      temp = -ratios[i];
    else
      temp = ratios[i] * baseFrequency;
    filters[i]->setResonance(temp, radii[i]*amplitude);
  }
}

MY_FLOAT Modal :: tick()
{
  MY_FLOAT temp = masterGain * onepole->tick(wave->tick() * envelope->tick());

  MY_FLOAT temp2 = 0.0;
  for (int i=0; i<nModes; i++)
    temp2 += filters[i]->tick(temp);

  temp2  -= temp2 * directGain;
  temp2 += directGain * temp;

  if (vibratoGain != 0.0)   {
    // Calculate AM and apply to master out
    temp = 1.0 + (vibrato->tick() * vibratoGain);
    temp2 = temp * temp2;
  }
    
  lastOutput = temp2;
  return lastOutput;
}


/***************************************************/
/*! \class ModalBar
    \brief STK resonant bar instrument class.

    This class implements a number of different
    struck bar instruments.  It inherits from the
    Modal class.

    Control Change Numbers: 
       - Stick Hardness = 2
       - Stick Position = 4
       - Vibrato Gain = 11
       - Vibrato Frequency = 7
       - Direct Stick Mix = 1
       - Volume = 128
       - Modal Presets = 16
         - Marimba = 0
         - Vibraphone = 1
         - Agogo = 2
         - Wood1 = 3
         - Reso = 4
         - Wood2 = 5
         - Beats = 6
         - Two Fixed = 7
         - Clump = 8

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

ModalBar :: ModalBar()
  : Modal()
{
  // Concatenate the STK rawwave path to the rawwave file
  wave = new WvIn( "special:marmstk1", TRUE );
  wave->setRate((MY_FLOAT) 0.5 * 22050.0 / Stk::sampleRate() );

  // Set the resonances for preset 0 (marimba).
  setPreset( 0 );

  // chuck
  m_frequency = baseFrequency;
  //reverse: same as before
  m_vibratoGain = vibratoGain / 0.3;
  //reverse: same as before
  m_vibratoFreq = vibrato->m_freq / 12.0;
  m_volume = 1.0;

}

ModalBar :: ~ModalBar()
{
  delete wave;
}

void ModalBar :: setStickHardness(MY_FLOAT hardness)
{
  stickHardness = hardness;
  if ( hardness < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): ModalBar: setStickHardness parameter is less than zero!" << CK_STDENDL;
    stickHardness = 0.0;
  }
  else if ( hardness > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): ModalBar: setStickHarness parameter is greater than 1.0!" << CK_STDENDL;
    stickHardness = 1.0;
  }

  wave->setRate( (0.25 * (MY_FLOAT)pow(4.0, stickHardness)) );
  masterGain = 0.1 + (1.8 * stickHardness);
}

void ModalBar :: setStrikePosition(MY_FLOAT position)
{
  strikePosition = position;
  if ( position < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): ModalBar: setStrikePositions parameter is less than zero!" << CK_STDENDL;
    strikePosition = 0.0;
  }
  else if ( position > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): ModalBar: setStrikePosition parameter is greater than 1.0!" << CK_STDENDL;
    strikePosition = 1.0;
  }

  // Hack only first three modes.
  MY_FLOAT temp2 = position * ONE_PI;
  MY_FLOAT temp = sin(temp2);                                       
  this->setModeGain(0, 0.12 * temp);

  temp = sin(0.05 + (3.9 * temp2));
  this->setModeGain(1,(MY_FLOAT) -0.03 * temp);

  temp = (MY_FLOAT)  sin(-0.05 + (11 * temp2));
  this->setModeGain(2,(MY_FLOAT) 0.11 * temp);
}

void ModalBar :: setPreset(int preset)
{
  // Presets:
  //     First line:  relative modal frequencies (negative number is
  //                  a fixed mode that doesn't scale with frequency
  //     Second line: resonances of the modes
  //     Third line:  mode volumes
  //     Fourth line: stickHardness, strikePosition, and direct stick
  //                  gain (mixed directly into the output
  static MY_FLOAT presets[9][4][4] = { 
    {{1.0, 3.99, 10.65, -2443},     // Marimba
     {0.9996, 0.9994, 0.9994, 0.999},
     {0.04, 0.01, 0.01, 0.008},
     {0.429688, 0.445312, 0.093750}},
    {{1.0, 2.01, 3.9, 14.37},       // Vibraphone
     {0.99995, 0.99991, 0.99992, 0.9999},   
     {0.025, 0.015, 0.015, 0.015 },
     {0.390625,0.570312,0.078125}},
    {{1.0, 4.08, 6.669, -3725.0},       // Agogo 
     {0.999, 0.999, 0.999, 0.999},  
     {0.06, 0.05, 0.03, 0.02},
     {0.609375,0.359375,0.140625}},
    {{1.0, 2.777, 7.378, 15.377},       // Wood1
     {0.996, 0.994, 0.994, 0.99},   
     {0.04, 0.01, 0.01, 0.008},
     {0.460938,0.375000,0.046875}},
    {{1.0, 2.777, 7.378, 15.377},       // Reso
     {0.99996, 0.99994, 0.99994, 0.9999},   
     {0.02, 0.005, 0.005, 0.004},
     {0.453125,0.250000,0.101562}},
    {{1.0, 1.777, 2.378, 3.377},        // Wood2
     {0.996, 0.994, 0.994, 0.99},   
     {0.04, 0.01, 0.01, 0.008},
     {0.312500,0.445312,0.109375}},
    {{1.0, 1.004, 1.013, 2.377},        // Beats
     {0.9999, 0.9999, 0.9999, 0.999},   
     {0.02, 0.005, 0.005, 0.004},
     {0.398438,0.296875,0.070312}},
    {{1.0, 4.0, -1320.0, -3960.0},      // 2Fix
     {0.9996, 0.999, 0.9994, 0.999},    
     {0.04, 0.01, 0.01, 0.008},
     {0.453125,0.453125,0.070312}},
    {{1.0, 1.217, 1.475, 1.729},        // Clump
     {0.999, 0.999, 0.999, 0.999},  
     {0.03, 0.03, 0.03, 0.03 },
     {0.390625,0.570312,0.078125}},
  };

  int temp = (preset % 9);
  for (int i=0; i<nModes; i++) {
    this->setRatioAndRadius(i, presets[temp][0][i], presets[temp][1][i]);
    this->setModeGain(i, presets[temp][2][i]);
  }

  this->setStickHardness(presets[temp][3][0]);
  this->setStrikePosition(presets[temp][3][1]);
  directGain = presets[temp][3][2];

  if (temp == 1) // vibraphone
    vibratoGain = 0.2;
  else
    vibratoGain = 0.0;
}

void ModalBar :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): ModalBar: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): ModalBar: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_StickHardness_) // 2
    this->setStickHardness( norm );
  else if (number == __SK_StrikePosition_) // 4
    this->setStrikePosition( norm );
  else if (number == __SK_ProphesyRibbon_) // 16
        this->setPreset((int) value);
  else if (number == __SK_ModWheel_) // 1
    directGain = norm;
  else if (number == 11) { // 11
    vibratoGain = norm * 0.3;
    m_vibratoGain = norm;
  }
  else if (number == 7) { // 7
    vibrato->setFrequency( norm * 12.0 );
    m_vibratoFreq = vibrato->m_freq;
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    envelope->setTarget( norm );
    m_volume = norm;
  } else
    CK_STDCERR << "[chuck](via STK): ModalBar: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): ModalBar: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Modulate :: Modulate()
{
  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency( 6.0 );
  vibratoGain = 0.04;

  noise = new SubNoise(330);
  randomGain = 0.05;

  filter = new OnePole( 0.999 );
  filter->setGain( randomGain );
}

Modulate :: ~Modulate()
{
  delete vibrato;
  delete noise;
  delete filter;
}

void Modulate :: reset()
{
  lastOutput = (MY_FLOAT)  0.0;
}

void Modulate :: setVibratoRate(MY_FLOAT aRate)
{
  vibrato->setFrequency( aRate );
}

void Modulate :: setVibratoGain(MY_FLOAT aGain)
{
  vibratoGain = aGain;
}

void Modulate :: setRandomGain(MY_FLOAT aGain)
{
  randomGain = aGain;
  filter->setGain( randomGain );
}

MY_FLOAT Modulate :: tick()
{
  // Compute periodic and random modulations.
  lastOutput = vibratoGain * vibrato->tick();
  lastOutput += filter->tick( noise->tick() );
  return lastOutput;                        
}

MY_FLOAT *Modulate :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}

MY_FLOAT Modulate :: lastOut() const
{
  return lastOutput;
}


/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Moog :: Moog()
{
  // Concatenate the STK rawwave path to the rawwave file
  attacks[0] = new WvIn( "special:mandpluk", TRUE );
  loops[0] = new WaveLoop( "special:impuls20", TRUE );
  loops[1] = new WaveLoop( "special:sinewave", TRUE ); // vibrato
  loops[1]->setFrequency((MY_FLOAT) 6.122);

  filters[0] = new FormSwep();
  filters[0]->setTargets( 0.0, 0.7 );

  filters[1] = new FormSwep();
  filters[1]->setTargets( 0.0, 0.7 );

  adsr->setAllTimes((MY_FLOAT) 0.001,(MY_FLOAT) 1.5,(MY_FLOAT) 0.6,(MY_FLOAT) 0.250);
  filterQ = (MY_FLOAT) 0.85;
  filterRate = (MY_FLOAT) 0.0001;
  modDepth = (MY_FLOAT) 0.0;

  // chuck
  //reverse: loops[1]->setFrequency(mSpeed);
  m_vibratoFreq = loops[1]->m_freq;
  //reverse: modDepth = mDepth * (MY_FLOAT) 0.5;
  m_vibratoGain = modDepth / 0.5;
  //reverse: nothing
  m_volume = 1.0;
}  

Moog :: ~Moog()
{
  delete attacks[0];
  delete loops[0];
  delete loops[1];
  delete filters[0];
  delete filters[1];
}

void Moog :: setFrequency(MY_FLOAT frequency)
{
  baseFrequency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Moog: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    baseFrequency = 220.0;
  }

  MY_FLOAT rate = attacks[0]->getSize() * 0.01 * baseFrequency / sampleRate();
  attacks[0]->setRate( rate );
  loops[0]->setFrequency(baseFrequency);

  // chuck
  m_frequency = baseFrequency;
}

//CHUCK wrapper
void Moog :: noteOn(MY_FLOAT amplitude ) { 
  noteOn ( baseFrequency, amplitude );
}

void Moog :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  MY_FLOAT temp;
    
  this->setFrequency( frequency );
  this->keyOn();
  attackGain = amplitude * (MY_FLOAT) 0.5;
  loopGain = amplitude;

  temp = filterQ + (MY_FLOAT) 0.05;
  filters[0]->setStates( 2000.0, temp );
  filters[1]->setStates( 2000.0, temp );

  temp = filterQ + (MY_FLOAT) 0.099;
  filters[0]->setTargets( frequency, temp );
  filters[1]->setTargets( frequency, temp );

  filters[0]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );
  filters[1]->setSweepRate( filterRate * 22050.0 / Stk::sampleRate() );

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Moog: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Moog :: setModulationSpeed(MY_FLOAT mSpeed)
{
  loops[1]->setFrequency(mSpeed);
  m_vibratoFreq = loops[1]->m_freq;
}

void Moog :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth * (MY_FLOAT) 0.5;
  m_vibratoGain = mDepth;
}

MY_FLOAT Moog :: tick()
{
  MY_FLOAT temp;

  if ( modDepth != 0.0 ) {
    temp = loops[1]->tick() * modDepth;    
    loops[0]->setFrequency( baseFrequency * (1.0 + temp) );
  }
  
  temp = Sampler::tick();
  temp = filters[0]->tick( temp );
  lastOutput = filters[1]->tick( temp );
  return lastOutput * 3.0;
}

void Moog :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Moog: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Moog: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_FilterQ_) // 2
    filterQ = 0.80 + ( 0.1 * norm );
  else if (number == __SK_FilterSweepRate_) // 4
    filterRate = norm * 0.0002;
  else if (number == __SK_ModFrequency_) { // 11
     this->setModulationSpeed( norm * 12.0 );
     }
  else if (number == __SK_ModWheel_) { // 1
    this->setModulationDepth( norm );
    }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    adsr->setTarget( norm );
    m_volume = norm;
    }
  else
    CK_STDCERR << "[chuck](via STK): Moog: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Moog: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}



/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class is derived from the CLM NRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This particular arrangement consists
    of 6 comb filters in parallel, followed by 3
    allpass filters, a lowpass filter, and another
    allpass in series, followed by two allpass
    filters in parallel with corresponding right
    and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

NRev :: NRev(MY_FLOAT T60)
{
  int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19};
  double scaler = Stk::sampleRate() / 25641.0;

  int delay, i;
  for (i=0; i<15; i++) {
    delay = (int) floor(scaler * lengths[i]);
    if ( (delay & 1) == 0) delay++;
    while ( !this->isPrime(delay) ) delay += 2;
    lengths[i] = delay;
  }

  for (i=0; i<6; i++) {
    combDelays[i] = new Delay( lengths[i], lengths[i]);
    combCoefficient[i] = pow(10.0, (-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  for (i=0; i<8; i++)
    allpassDelays[i] = new Delay(lengths[i+6], lengths[i+6]);

  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

NRev :: ~NRev()
{
  int i;
  for (i=0; i<6; i++)  delete combDelays[i];
  for (i=0; i<8; i++)  delete allpassDelays[i];
}

void NRev :: clear()
{
  int i;
  for (i=0; i<6; i++) combDelays[i]->clear();
  for (i=0; i<8; i++) allpassDelays[i]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
  lowpassState = 0.0;
}

MY_FLOAT NRev :: tick(MY_FLOAT input)
{
    MY_FLOAT temp, temp0, temp1, temp2, temp3;
    int i;

    // gewang: dedenormal
    CK_STK_DDN(input);

    temp0 = 0.0;
    for (i=0; i<6; i++) {
        temp = input + (combCoefficient[i] * combDelays[i]->lastOut());
        // gewang: dedenormal
        CK_STK_DDN(temp);
        temp0 += combDelays[i]->tick(temp);
    }

    for (i=0; i<3; i++) {
        temp = allpassDelays[i]->lastOut();
        temp1 = allpassCoefficient * temp;
        temp1 += temp0;
        // gewang: dedenormal
        CK_STK_DDN(temp1);
        allpassDelays[i]->tick(temp1);
        temp0 = -(allpassCoefficient * temp1) + temp;
    }

    // One-pole lowpass filter.
    lowpassState = 0.7*lowpassState + 0.3*temp0;
    // gewang: dedenormal
    CK_STK_DDN(lowpassState);
    temp = allpassDelays[3]->lastOut();
    temp1 = allpassCoefficient * temp;
    temp1 += lowpassState;
    // gewang: dedenormal
    CK_STK_DDN(temp1);
    allpassDelays[3]->tick(temp1);
    temp1 = -(allpassCoefficient * temp1) + temp;

    temp = allpassDelays[4]->lastOut();
    temp2 = allpassCoefficient * temp;
    temp2 += temp1;
    // gewang: dedenormal
    CK_STK_DDN(temp2);
    allpassDelays[4]->tick(temp2);
    lastOutput[0] = effectMix*(-(allpassCoefficient * temp2) + temp);

    temp = allpassDelays[5]->lastOut();
    temp3 = allpassCoefficient * temp;
    temp3 += temp1;
    // gewang: dedenormal
    CK_STK_DDN(temp3);
    allpassDelays[5]->tick(temp3);
    lastOutput[1] = effectMix*(-(allpassCoefficient * temp3) + temp);

    temp = (1.0 - effectMix) * input;
    lastOutput[0] += temp;
    lastOutput[1] += temp;

    return (lastOutput[0] + lastOutput[1]) * 0.5;
}


/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <time.h>

Noise :: Noise() : Stk()
{
  // Seed the random number generator with system time.
  this->setSeed( 0 );
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: Noise( unsigned int seed ) : Stk()
{
  // Seed the random number generator
  this->setSeed( seed );
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: ~Noise()
{
}

void Noise :: setSeed( unsigned int seed )
{
  if ( seed == 0 )
    srand( (unsigned int) time(NULL) );
  else
    srand( seed );
}

MY_FLOAT Noise :: tick()
{
  lastOutput = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  lastOutput -= 1.0;
  return lastOutput;
}

MY_FLOAT *Noise :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}

MY_FLOAT Noise :: lastOut() const
{
  return lastOutput;
}

/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This protected Filter subclass implements
    a one-pole digital filter.  A method is
    provided for setting the pole position along
    the real axis of the z-plane while maintaining
    a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


OnePole :: OnePole() : FilterStk()
{
  MY_FLOAT B = 0.1;
  MY_FLOAT A[2] = {1.0, -0.9};
  FilterStk::setCoefficients( 1, &B, 2, A );
}

OnePole :: OnePole(MY_FLOAT thePole) : FilterStk()
{
  MY_FLOAT B;
  MY_FLOAT A[2] = {1.0, -0.9};

  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    B = (MY_FLOAT) (1.0 - thePole);
  else
    B = (MY_FLOAT) (1.0 + thePole);

  A[1] = -thePole;
  FilterStk::setCoefficients( 1, &B, 2,  A );
}

OnePole :: ~OnePole()    
{
}

void OnePole :: clear(void)
{
  FilterStk::clear();
}

void OnePole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OnePole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void OnePole :: setPole(MY_FLOAT thePole)
{
  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    b[0] = (MY_FLOAT) (1.0 - thePole);
  else
    b[0] = (MY_FLOAT) (1.0 + thePole);

  a[1] = -thePole;
}

void OnePole :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT OnePole :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT OnePole :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT OnePole :: tick(MY_FLOAT sample)
{
    inputs[0] = gain * sample;
    outputs[0] = b[0] * inputs[0] - a[1] * outputs[1];
    outputs[1] = outputs[0];

    // gewang: dedenormal
    CK_STK_DDN(outputs[1]);

    return outputs[0];
}

MY_FLOAT *OnePole :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This protected Filter subclass implements
    a one-zero digital filter.  A method is
    provided for setting the zero position
    along the real axis of the z-plane while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


OneZero :: OneZero() : FilterStk()
{
  MY_FLOAT B[2] = {0.5, 0.5};
  MY_FLOAT A = 1.0;
  FilterStk::setCoefficients( 2, B, 1, &A );
}

OneZero :: OneZero(MY_FLOAT theZero) : FilterStk()
{
  MY_FLOAT B[2];
  MY_FLOAT A = 1.0;

  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    B[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    B[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  B[1] = -theZero * B[0];
  FilterStk::setCoefficients( 2, B, 1,  &A );
}

OneZero :: ~OneZero(void)
{
}

void OneZero :: clear(void)
{
  FilterStk::clear();
}

void OneZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OneZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void OneZero :: setZero(MY_FLOAT theZero)
{
  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    b[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    b[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  b[1] = -theZero * b[0];
}

void OneZero :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT OneZero :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT OneZero :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT OneZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[1] * inputs[1] + b[0] * inputs[0];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *OneZero :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

PRCRev :: PRCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[4]= {353, 1097, 1777, 2137};
  double scaler = Stk::sampleRate() / 44100.0;

  // Scale the delay lengths if necessary.
  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<4; i++) {
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<2; i++)   {
    allpassDelays[i] = new Delay( lengths[i], lengths[i] );
    combDelays[i] = new Delay( lengths[i+2], lengths[i+2] );
    combCoefficient[i] = pow(10.0,(-3 * lengths[i+2] / (T60 * Stk::sampleRate())));
  }

  allpassCoefficient = 0.7;
  effectMix = 0.5;
  this->clear();
}

PRCRev :: ~PRCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete combDelays[0];
  delete combDelays[1];
}

void PRCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT PRCRev :: tick(MY_FLOAT input)
{
    MY_FLOAT temp, temp0, temp1, temp2, temp3;

    // gewang: dedenormal
    CK_STK_DDN(input);

    temp = allpassDelays[0]->lastOut();
    temp0 = allpassCoefficient * temp;
    temp0 += input;
    // gewang: dedenormal
    CK_STK_DDN(temp0);
    allpassDelays[0]->tick(temp0);
    temp0 = -(allpassCoefficient * temp0) + temp;

    temp = allpassDelays[1]->lastOut();
    temp1 = allpassCoefficient * temp;
    temp1 += temp0;
    // gewang: dedenormal
    CK_STK_DDN(temp1);
    allpassDelays[1]->tick(temp1);
    temp1 = -(allpassCoefficient * temp1) + temp;

    temp2 = temp1 + (combCoefficient[0] * combDelays[0]->lastOut());
    temp3 = temp1 + (combCoefficient[1] * combDelays[1]->lastOut());

    // gewang: dedenormal
    CK_STK_DDN(temp2);
    CK_STK_DDN(temp3);

    lastOutput[0] = effectMix * (combDelays[0]->tick(temp2));
    lastOutput[1] = effectMix * (combDelays[1]->tick(temp3));
    temp = (MY_FLOAT) (1.0 - effectMix) * input;
    lastOutput[0] += temp;
    lastOutput[1] += temp;

    return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
}


/***************************************************/
/*! \class PercFlut
    \brief STK percussive flute FM synthesis instrument.

    This class implements algorithm 4 of the TX81Z.

    \code
    Algorithm 4 is :   4->3--\
                          2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PercFlut :: PercFlut()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.50 * 1.000);
  this->setRatio(1, 3.00 * 0.995);
  this->setRatio(2, 2.99 * 1.005);
  this->setRatio(3, 6.00 * 0.997);
  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[71];
  gains[2] = __FM_gains[93];
  gains[3] = __FM_gains[85];

  adsr[0]->setAllTimes( 0.05, 0.05, __FM_susLevels[14], 0.05);
  adsr[1]->setAllTimes( 0.02, 0.50, __FM_susLevels[13], 0.5);
  adsr[2]->setAllTimes( 0.02, 0.30, __FM_susLevels[11], 0.05);
  adsr[3]->setAllTimes( 0.02, 0.05, __FM_susLevels[13], 0.01);

  twozero->setGain( 0.0 );
  modDepth = 0.005;
}  

PercFlut :: ~PercFlut()
{
}

void PercFlut :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;

  // chuck
  m_frequency = baseFrequency;
}

void PercFlut :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99] * 0.5;
  gains[1] = amplitude * __FM_gains[71] * 0.5;
  gains[2] = amplitude * __FM_gains[93] * 0.5;
  gains[3] = amplitude * __FM_gains[85] * 0.5;
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "PercFlut: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT PercFlut :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * (MY_FLOAT) 0.2;    
  waves[0]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[3]);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();

  twozero->tick(temp);
  waves[2]->addPhaseOffset(temp);
  temp = (1.0 - (control2 * 0.5)) * gains[2] * adsr[2]->tick() * waves[2]->tick();

  temp += control2 * 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * (MY_FLOAT) 0.5;
  return lastOutput;
}
/***************************************************/
/*! \class Phonemes
    \brief STK phonemes table.

    This class does nothing other than declare a
    set of 32 static phoneme formant parameters
    and provide access to those values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>

const char Phonemes :: phonemeNames[32][4] = 
  {"eee", "ihh", "ehh", "aaa",
   "ahh", "aww", "ohh", "uhh",
   "uuu", "ooo", "rrr", "lll",
   "mmm", "nnn", "nng", "ngg",
   "fff", "sss", "thh", "shh",
   "xxx", "hee", "hoo", "hah",
   "bbb", "ddd", "jjj", "ggg",
   "vvv", "zzz", "thz", "zhh"
  };

const MY_FLOAT Phonemes :: phonemeGains[32][2] =
  {{1.0, 0.0},    // eee
   {1.0, 0.0},    // ihh
   {1.0, 0.0},    // ehh
   {1.0, 0.0},    // aaa

   {1.0, 0.0},    // ahh
   {1.0, 0.0},    // aww
   {1.0, 0.0},    // ohh
   {1.0, 0.0},    // uhh

   {1.0, 0.0},    // uuu
   {1.0, 0.0},    // ooo
   {1.0, 0.0},    // rrr
   {1.0, 0.0},    // lll

   {1.0, 0.0},    // mmm
   {1.0, 0.0},    // nnn
   {1.0, 0.0},    // nng
   {1.0, 0.0},    // ngg

   {0.0, 0.7},    // fff
   {0.0, 0.7},    // sss
   {0.0, 0.7},    // thh
   {0.0, 0.7},    // shh

   {0.0, 0.7},    // xxx
   {0.0, 0.1},    // hee
   {0.0, 0.1},    // hoo
   {0.0, 0.1},    // hah

   {1.0, 0.1},    // bbb
   {1.0, 0.1},    // ddd
   {1.0, 0.1},    // jjj
   {1.0, 0.1},    // ggg

   {1.0, 1.0},    // vvv
   {1.0, 1.0},    // zzz
   {1.0, 1.0},    // thz
   {1.0, 1.0}     // zhh
  };

const MY_FLOAT Phonemes :: phonemeParameters[32][4][3] =
  {{  { 273, 0.996,  10},       // eee (beet)
      {2086, 0.945, -16}, 
      {2754, 0.979, -12}, 
      {3270, 0.440, -17}},
   {  { 385, 0.987,  10},       // ihh (bit)
      {2056, 0.930, -20},
      {2587, 0.890, -20}, 
      {3150, 0.400, -20}},
   {  { 515, 0.977,  10},       // ehh (bet)
      {1805, 0.810, -10}, 
      {2526, 0.875, -10}, 
      {3103, 0.400, -13}},
   {  { 773, 0.950,  10},       // aaa (bat)
      {1676, 0.830,  -6},
      {2380, 0.880, -20}, 
      {3027, 0.600, -20}},
     
   {  { 770, 0.950,   0},       // ahh (father)
      {1153, 0.970,  -9},
      {2450, 0.780, -29},
      {3140, 0.800, -39}},
   {  { 637, 0.910,   0},       // aww (bought)
      { 895, 0.900,  -3},
      {2556, 0.950, -17},
      {3070, 0.910, -20}},
   {  { 637, 0.910,   0},       // ohh (bone)  NOTE::  same as aww (bought)
      { 895, 0.900,  -3},
      {2556, 0.950, -17},
      {3070, 0.910, -20}},
   {  { 561, 0.965,   0},       // uhh (but)
      {1084, 0.930, -10}, 
      {2541, 0.930, -15}, 
      {3345, 0.900, -20}},
    
   {  { 515, 0.976,   0},       // uuu (foot)
      {1031, 0.950,  -3},
      {2572, 0.960, -11},
      {3345, 0.960, -20}},
   {  { 349, 0.986, -10},       // ooo (boot)
      { 918, 0.940, -20},
      {2350, 0.960, -27},
      {2731, 0.950, -33}},
   {  { 394, 0.959, -10},       // rrr (bird)
      {1297, 0.780, -16},
      {1441, 0.980, -16},
      {2754, 0.950, -40}},
   {  { 462, 0.990,  +5},       // lll (lull)
      {1200, 0.640, -10},
      {2500, 0.200, -20},
      {3000, 0.100, -30}},
     
   {  { 265, 0.987, -10},       // mmm (mom)
      {1176, 0.940, -22},
      {2352, 0.970, -20},
      {3277, 0.940, -31}},
   {  { 204, 0.980, -10},       // nnn (nun)
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
   {  { 204, 0.980, -10},       // nng (sang)    NOTE:: same as nnn
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
   {  { 204, 0.980, -10},       // ngg (bong)    NOTE:: same as nnn
      {1570, 0.940, -15},
      {2481, 0.980, -12},
      {3133, 0.800, -30}},
     
   {  {1000, 0.300,   0},       // fff
      {2800, 0.860, -10},
      {7425, 0.740,   0},
      {8140, 0.860,   0}},
   {  {0,    0.000,   0},       // sss
      {2000, 0.700, -15},
      {5257, 0.750,  -3}, 
      {7171, 0.840,   0}},
   {  { 100, 0.900,   0},       // thh
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // shh
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},

   {  {1000, 0.300, -10},       // xxx           NOTE:: Not Really Done Yet
      {2800, 0.860, -10},
      {7425, 0.740,   0},
      {8140, 0.860,   0}},
   {  { 273, 0.996, -40},       // hee (beet)    (noisy eee)
      {2086, 0.945, -16}, 
      {2754, 0.979, -12}, 
      {3270, 0.440, -17}},
   {  { 349, 0.986, -40},       // hoo (boot)    (noisy ooo)
      { 918, 0.940, -10},
      {2350, 0.960, -17},
      {2731, 0.950, -23}},
   {  { 770, 0.950, -40},       // hah (father)  (noisy ahh)
      {1153, 0.970,  -3},
      {2450, 0.780, -20},
      {3140, 0.800, -32}},
     
   {  {2000, 0.700, -20},       // bbb           NOTE:: Not Really Done Yet
      {5257, 0.750, -15},
      {7171, 0.840,  -3}, 
      {9000, 0.900,   0}},
   {  { 100, 0.900,   0},       // ddd           NOTE:: Not Really Done Yet
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // jjj           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
   {  {2693, 0.940,   0},       // ggg           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
     
   {  {2000, 0.700, -20},       // vvv           NOTE:: Not Really Done Yet
      {5257, 0.750, -15},
      {7171, 0.840,  -3}, 
      {9000, 0.900,   0}},
   {  { 100, 0.900,   0},       // zzz           NOTE:: Not Really Done Yet
      {4000, 0.500, -20},
      {5500, 0.500, -15},
      {8000, 0.400, -20}},
   {  {2693, 0.940,   0},       // thz           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}},
   {  {2693, 0.940,   0},       // zhh           NOTE:: Not Really Done Yet
      {4000, 0.720, -10},
      {6123, 0.870, -10},
      {7755, 0.750, -18}}
  };

Phonemes :: Phonemes(void)
{
}

Phonemes :: ~Phonemes(void)
{
}

const char *Phonemes :: name( unsigned int index )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: name index is greater than 31!" << CK_STDENDL;
    return 0;
  }
  return phonemeNames[index];
}

MY_FLOAT Phonemes :: voiceGain( unsigned int index )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: voiceGain index is greater than 31!" << CK_STDENDL;
    return 0.0;
  }
  return phonemeGains[index][0];
}

MY_FLOAT Phonemes :: noiseGain( unsigned int index )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: noiseGain index is greater than 31!" << CK_STDENDL;
    return 0.0;
  }
  return phonemeGains[index][1];
}

MY_FLOAT Phonemes :: formantFrequency( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantFrequency index is greater than 31!" << CK_STDENDL;
    return 0.0;
  }
  if ( partial > 3 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantFrequency partial is greater than 3!" << CK_STDENDL;
    return 0.0;
  }
  return phonemeParameters[index][partial][0];
}

MY_FLOAT Phonemes :: formantRadius( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantRadius index is greater than 31!" << CK_STDENDL;
    return 0.0;
  }
  if ( partial > 3 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantRadius partial is greater than 3!" << CK_STDENDL;
    return 0.0;
  }
  return phonemeParameters[index][partial][1];
}

MY_FLOAT Phonemes :: formantGain( unsigned int index, unsigned int partial )
{
  if ( index > 31 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantGain index is greater than 31!" << CK_STDENDL;
    return 0.0;
  }
  if ( partial > 3 ) {
    CK_STDCERR << "[chuck](via STK): Phonemes: formantGain partial is greater than 3!" << CK_STDENDL;
    return 0.0;
  }
  return phonemeParameters[index][partial][2];
}
/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <iostream>
#include <math.h>

PitShift :: PitShift()
{
  delay[0] = 12;
  delay[1] = 512;
  delayLine[0] = new DelayL(delay[0], (long) 1024);
  delayLine[1] = new DelayL(delay[1], (long) 1024);
  effectMix = (MY_FLOAT) 0.5;
  rate = 1.0;
}

PitShift :: ~PitShift()
{
  delete delayLine[0];
  delete delayLine[1];
}

void PitShift :: clear()
{
  delayLine[0]->clear();
  delayLine[1]->clear();
  lastOutput = 0.0;
}

void PitShift :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): PitShift: setEffectMix parameter is less than zero!" << CK_STDENDL;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): PitShift: setEffectMix parameter is greater than 1.0!" << CK_STDENDL;
    effectMix = 1.0;
  }
}

void PitShift :: setShift(MY_FLOAT shift)
{
  if (shift < 1.0)    {
    rate = 1.0 - shift; 
  }
  else if (shift > 1.0)       {
    rate = 1.0 - shift;
  }
  else {
    rate = 0.0;
    delay[0] = 512;
  }
}

MY_FLOAT PitShift :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT PitShift :: tick(MY_FLOAT input)
{
  delay[0] = delay[0] + rate;
  while (delay[0] > 1012) delay[0] -= 1000;
  while (delay[0] < 12) delay[0] += 1000;
  delay[1] = delay[0] + 500;
  while (delay[1] > 1012) delay[1] -= 1000;
  while (delay[1] < 12) delay[1] += 1000;
  delayLine[0]->setDelay((long)delay[0]);
  delayLine[1]->setDelay((long)delay[1]);
  env[1] = fabs(delay[0] - 512) * 0.002;
  env[0] = 1.0 - env[1];
  lastOutput =  env[0] * delayLine[0]->tick(input);
  lastOutput += env[1] * delayLine[1]->tick(input);
  lastOutput *= effectMix;
  lastOutput += (1.0 - effectMix) * input;
  return lastOutput;
}

MY_FLOAT *PitShift :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}

/***************************************************/
/*! \class PluckTwo
    \brief STK enhanced plucked string model class.

    This class implements an enhanced two-string,
    plucked physical model, a la Jaffe-Smith,
    Smith, and others.

    PluckTwo is an abstract class, with no excitation
    specified.  Therefore, it can't be directly
    instantiated.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PluckTwo :: PluckTwo(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  baseLoopGain = (MY_FLOAT) 0.995;
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA((MY_FLOAT)(length / 2.0), length);
  delayLine2 = new DelayA((MY_FLOAT)(length / 2.0), length);
  combDelay = new DelayL((MY_FLOAT)(length / 2.0), length);
  filter = new OneZero;
  filter2 = new OneZero;
  pluckAmplitude = (MY_FLOAT) 0.3;
  pluckPosition = (MY_FLOAT) 0.4;
  detuning = (MY_FLOAT) 0.995;
  lastFrequency = lowestFrequency * (MY_FLOAT) 2.0;
  lastLength = length * (MY_FLOAT) 0.5;

  // chuck
  m_frequency = lastFrequency;
}

PluckTwo :: ~PluckTwo()
{
  delete delayLine;
  delete delayLine2;
  delete combDelay;
  delete filter;
  delete filter2;
}

void PluckTwo :: clear()
{
  delayLine->clear();
  delayLine2->clear();
  combDelay->clear();
  filter->clear();
  filter2->clear();
}

void PluckTwo :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( lastFrequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): PluckTwo: setFrequency parameter less than or equal to zero!" << CK_STDENDL;
    lastFrequency = 220.0;
  }

  // Delay = length - approximate filter delay.
  lastLength = ( Stk::sampleRate() / lastFrequency);
  MY_FLOAT delay = (lastLength / detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine->setDelay( delay );

  delay = (lastLength * detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine2->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 1.0 ) loopGain = (MY_FLOAT) 0.99999;

  // chuck
  m_frequency = lastFrequency;
}

void PluckTwo :: setDetune(MY_FLOAT detune)
{
  detuning = detune;
  if ( detuning <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): PluckTwo: setDetune parameter less than or equal to zero!" << CK_STDENDL;
    detuning = 0.1;
  }
  delayLine->setDelay(( lastLength / detuning) - (MY_FLOAT) 0.5);
  delayLine2->setDelay( (lastLength * detuning) - (MY_FLOAT) 0.5);
}

void PluckTwo :: setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune)
{
  detuning = detune;
  this->setFrequency(frequency);
}

void PluckTwo :: setPluckPosition(MY_FLOAT position)
{
  pluckPosition = position;
  if ( position < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): PluckTwo: setPluckPosition parameter is less than zero!" << CK_STDENDL;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): PluckTwo: setPluckPosition parameter is greater than 1.0!" << CK_STDENDL;
    pluckPosition = 1.0;
  }
}

void PluckTwo :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void PluckTwo :: noteOff(MY_FLOAT amplitude)
{
  loopGain =  ((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.5;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): PluckTwo: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class Plucked
    \brief STK plucked string model class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Plucked :: Plucked(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  loopFilter = new OneZero;
  pickFilter = new OnePole;
  noise = new Noise;
  this->clear();
}

Plucked :: ~Plucked()
{
  delete delayLine;
  delete loopFilter;
  delete pickFilter;
  delete noise;
}

void Plucked :: clear()
{
  delayLine->clear();
  loopFilter->clear();
  pickFilter->clear();
}

void Plucked :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.000005);
  if ( loopGain >= 1.0 ) loopGain = (MY_FLOAT) 0.99999;

  // chuck
  m_frequency = freakency;
}

void Plucked :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: pluck amplitude greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: pluck amplitude less than zero!" << CK_STDENDL;
    gain = 0.0;
  }

  pickFilter->setPole((MY_FLOAT) 0.999 - (gain * (MY_FLOAT) 0.15));
  pickFilter->setGain(gain * (MY_FLOAT) 0.5);
  for (long i=0; i<length; i++)
    // Fill delay with noise additively with current contents.
    delayLine->tick( 0.6 * delayLine->lastOut() + pickFilter->tick( noise->tick() ) );
}

void Plucked :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Plucked: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Plucked :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: noteOff amplitude greater than 1.0!" << CK_STDENDL;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: noteOff amplitude less than or zero!" << CK_STDENDL;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Plucked: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Plucked :: tick()
{
  // Here's the whole inner loop of the instrument!!
  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) ); 
  lastOutput *= (MY_FLOAT) 3.0;
  return lastOutput;
}
/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This protected Filter subclass implements
    a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass
    filter with a given coefficient.  Another
    method is provided to create a DC blocking filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


PoleZero :: PoleZero() : FilterStk()
{
  // Default setting for pass-through.
  MY_FLOAT B[2] = {1.0, 0.0};
  MY_FLOAT A[2] = {1.0, 0.0};
  FilterStk::setCoefficients( 2, B, 2, A );
}

PoleZero :: ~PoleZero()
{
}

void PoleZero :: clear(void)
{
  FilterStk::clear();
}

void PoleZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void PoleZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void PoleZero :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void PoleZero :: setAllpass(MY_FLOAT coefficient)
{
  b[0] = coefficient;
  b[1] = 1.0;
  a[0] = 1.0; // just in case
  a[1] = coefficient;
}

void PoleZero :: setBlockZero(MY_FLOAT thePole)
{
  b[0] = 1.0;
  b[1] = -1.0;
  a[0] = 1.0; // just in case
  a[1] = -thePole;
}

void PoleZero :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT PoleZero :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT PoleZero :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT PoleZero :: tick(MY_FLOAT sample)
{
    inputs[0] = gain * sample;
    outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] - a[1] * outputs[1];
    inputs[1] = inputs[0];
    outputs[1] = outputs[0];

    // gewang: dedenormal
    CK_STK_DDN(outputs[1]);

    return outputs[0];
}

MY_FLOAT *PoleZero :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}

/***************************************************/
/*! \class ReedTabl
    \brief STK reed table class.

    This class implements a simple one breakpoint,
    non-linear reed function, as described by
    Smith (1986).  This function is based on a
    memoryless non-linear spring model of the reed
    (the reed mass is ignored) which saturates when
    the reed collides with the mouthpiece facing.

    See McIntyre, Schumacher, & Woodhouse (1983),
    Smith (1986), Hirschman, Cook, Scavone, and
    others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


ReedTabl :: ReedTabl()
{
  offSet = (MY_FLOAT) 0.6;  // Offset is a bias, related to reed rest position.
  slope = (MY_FLOAT) -0.8;  // Slope corresponds loosely to reed stiffness.
}

ReedTabl :: ~ReedTabl()
{

}

void ReedTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;
}

void ReedTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;
}

MY_FLOAT ReedTabl :: lastOut() const
{
    return lastOutput;
}

MY_FLOAT ReedTabl :: tick(MY_FLOAT input)    
{
  // The input is differential pressure across the reed.
  lastOutput = offSet + (slope * input);

  // If output is > 1, the reed has slammed shut and the
  // reflection function value saturates at 1.0.
  if (lastOutput > 1.0) lastOutput = (MY_FLOAT) 1.0;

  // This is nearly impossible in a physical system, but
  // a reflection function value of -1.0 corresponds to
  // an open end (and no discontinuity in bore profile).
  if (lastOutput < -1.0) lastOutput = (MY_FLOAT) -1.0;
  return lastOutput;
}

MY_FLOAT *ReedTabl :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}

/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Resonate :: Resonate()
{
  adsr = new ADSR;
  noise = new Noise;

  filter = new BiQuad;
  poleFrequency = 4000.0;
  poleRadius = 0.95;
  // Set the filter parameters.
  filter->setResonance( poleFrequency, poleRadius, TRUE );
  zeroFrequency = 0.0;
  zeroRadius = 0.0;
}  

Resonate :: ~Resonate()
{
  delete adsr;
  delete filter;
  delete noise;
}

void Resonate :: keyOn()
{
  adsr->keyOn();
}

void Resonate :: keyOff()
{
  adsr->keyOff();
}

void Resonate :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  adsr->setTarget( amplitude );
  this->keyOn();
  this->setResonance(frequency, poleRadius);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Resonate: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}
void Resonate :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Resonate: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Resonate :: setResonance(MY_FLOAT frequency, MY_FLOAT radius)
{
  poleFrequency = frequency;
  if ( frequency < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Resonate: setResonance frequency parameter is less than zero!" << CK_STDENDL;
    poleFrequency = 0.0;
  }

  poleRadius = radius;
  if ( radius < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Resonate: setResonance radius parameter is less than 0.0!" << CK_STDENDL;
    poleRadius = 0.0;
  }
  else if ( radius >= 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Resonate: setResonance radius parameter is greater than or equal to 1.0, which is unstable!" << CK_STDENDL;
    poleRadius = 0.9999;
  }
  filter->setResonance( poleFrequency, poleRadius, TRUE );
}

void Resonate :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  zeroFrequency = frequency;
  if ( frequency < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Resonate: setNotch frequency parameter is less than zero!" << CK_STDENDL;
    zeroFrequency = 0.0;
  }

  zeroRadius = radius;
  if ( radius < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Resonate: setNotch radius parameter is less than 0.0!" << CK_STDENDL;
    zeroRadius = 0.0;
  }
  
  filter->setNotch( zeroFrequency, zeroRadius );
}

void Resonate :: setEqualGainZeroes()
{
  filter->setEqualGainZeroes();
}

MY_FLOAT Resonate :: tick()
{
  lastOutput = filter->tick(noise->tick());
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Resonate :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Resonate: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Resonate: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == 2) // 2
    setResonance( norm * Stk::sampleRate() * 0.5, poleRadius );
  else if (number == 4) // 4
    setResonance( poleFrequency, norm*0.9999 );
  else if (number == 11) // 11
    this->setNotch( norm * Stk::sampleRate() * 0.5, zeroRadius );
  else if (number == 1)
    this->setNotch( zeroFrequency, norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    CK_STDCERR << "[chuck](via STK): Resonate: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Resonate: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}
/***************************************************/
/*! \class Reverb
    \brief STK abstract reverberator parent class.

    This class provides common functionality for
    STK reverberator subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Reverb :: Reverb()
{
}

Reverb :: ~Reverb()
{
}

void Reverb :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT Reverb :: lastOut() const
{
  return (lastOutput[0] + lastOutput[1]) * 0.5;
}

MY_FLOAT Reverb :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Reverb :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT *Reverb :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}

bool Reverb :: isPrime(int number)
{
  if (number == 2) return true;
  if (number & 1)   {
      for (int i=3; i<(int)sqrt((double)number)+1; i+=2)
          if ( (number % i) == 0) return false;
      return true; /* prime */
    }
  else return false; /* even */
}
/***************************************************/
/*! \class Rhodey
    \brief STK Fender Rhodes-like electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Rhodey :: Rhodey()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0);
  this->setRatio(1, 0.5);
  this->setRatio(2, 1.0);
  this->setRatio(3, 15.0);

  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[90];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[67];

  adsr[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 1.00, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.001, 0.25, 0.0, 0.04);

  twozero->setGain((MY_FLOAT) 1.0);
}  

Rhodey :: ~Rhodey()
{
}

void Rhodey :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency * (MY_FLOAT) 2.0;

  for (int i=0; i<nOperators; i++ )
    waves[i]->setFrequency( baseFrequency * ratios[i] );

  // chuck
  m_frequency = baseFrequency * .5;
}

void Rhodey :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99];
  gains[1] = amplitude * __FM_gains[90];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[67];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "Rhodey: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Rhodey :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}

/***************************************************/
/*! \class SKINI
    \brief STK SKINI parsing class

    This class parses SKINI formatted text
    messages.  It can be used to parse individual
    messages or it can be passed an entire file.
    The file specification is Perry's and his
    alone, but it's all text so it shouldn't be to
    hard to figure out.

    SKINI (Synthesis toolKit Instrument Network
    Interface) is like MIDI, but allows for
    floating-point control changes, note numbers,
    etc.  The following example causes a sharp
    middle C to be played with a velocity of 111.132:

    noteOn  60.01  111.13

    See also SKINI.txt.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <string.h>
#include <stdlib.h>

// Constructor for use when parsing SKINI strings (coming over socket
// for example.  Use parseThis() method with string pointer.
SKINI :: SKINI()
{
}

//  Constructor for reading SKINI files ... use nextMessage() method.
SKINI :: SKINI(char *fileName)
{
  char msg[256];

  myFile = fopen(fileName,"r");
  if ((long) myFile < 0) {
    sprintf(msg, "[chuck](via SKINI): Could not open or find file (%s).", fileName);
    handleError(msg, StkError::FILE_NOT_FOUND);
  }

  this->nextMessage();
}

SKINI :: ~SKINI()
{
}

/*****************  SOME HANDY ROUTINES   *******************/


#define __SK_MAX_FIELDS_ 5
#define __SK_MAX_SIZE_ 32

short ignore(char aChar)
{
  short ignoreIt = 0;
  if (aChar == 0)   ignoreIt = 1;        //  Null String Termination
  if (aChar == '\n')  ignoreIt = 1;      //  Carraige Return???
  if (aChar == '/') ignoreIt = 2;        //  Comment Line
  return ignoreIt;
}

short delimit(char aChar)
{
  if (aChar == ' ' ||           // Space
      aChar == ','  ||          // Or Comma
      aChar == '\t')            // Or Tab
    return 1;
  else
    return 0;
}

short nextChar(char* aString)
{
  int i;

  for (i=0;i<__SK_MAX_SIZE_;i++)  {
    if (        aString[i] != ' ' &&             // Space
                aString[i] != ','  &&            // Or Comma
                aString[i] != '\t'     )         // Or Tab
        return i;
  }
  return 1024;
}

int subStrings(char *aString, 
       char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_], 
       int  somePointrs[__SK_MAX_FIELDS_],
       char *remainderString)
{
  int notDone,howMany,point,temp;
  notDone = 1;
  howMany = 0;
  point = 0;
  temp = nextChar(aString);
  if (temp >= __SK_MAX_SIZE_) {
    notDone = 0;
    // printf("Confusion here: Ignoring this line\n");
    // printf("%s\n",aString);
    return howMany;
  }
  point = temp;
  somePointrs[howMany] = point;
  temp = 0;
  while (notDone)    {
    if (aString[point] == '\n') {
      notDone = 0;
    }
    else        {
      someStrings[howMany][temp++] = aString[point++];
      if (temp >= __SK_MAX_SIZE_)      {
        howMany = 0;
        return howMany;
      }
      if (delimit(aString[point]) || aString[point] == '\n') {
        someStrings[howMany][temp] = 0;
        howMany += 1;
        if (howMany < __SK_MAX_FIELDS_)       {
          temp = nextChar(&aString[point]);
          point += temp;
          somePointrs[howMany-1] = point;
          temp = 0;
        }
        else   {
          temp = 0;
          somePointrs[howMany-1] = point;
          while(aString[point] != '\n')
            remainderString[temp++] = aString[point++];
          remainderString[temp] = aString[point];
        }
      }
    }   
  }
  //     printf("Got: %i Strings:\n",howMany);
  //     for (temp=0;temp<howMany;temp++) 
  //         printf("%s\n",someStrings[temp]);
  return howMany;
     
}

/****************  THE ENCHILLADA !!!!  **********************/
/***   This function parses a single string (if it can)   ****/
/***   of SKINI message, setting the appropriate variables ***/
/*************************************************************/

long SKINI :: parseThis(char* aString)
{
  int which,aField;
  int temp,temp2;
  char someStrings[__SK_MAX_FIELDS_][__SK_MAX_SIZE_];
  int  somePointrs[__SK_MAX_FIELDS_];
     
  temp = nextChar(aString);
  if ((which = ignore(aString[temp]))) {
    if (which == 2) printf("// CommentLine: %s\n",aString);
    messageType = 0;
    return messageType;
  }
  else        {
    temp = subStrings(aString,someStrings,somePointrs,remainderString);
    if (temp > 0)    
      which = 0;
    aField = 0;
    strcpy(msgTypeString,someStrings[aField]);
    while ((which < __SK_MaxMsgTypes_) && 
           (strcmp(msgTypeString,
                   skini_msgs[which].messageString)))  {
        which += 1;  
    }
    if (which >= __SK_MaxMsgTypes_)  {
        messageType = 0;
        printf("Couldn't parse this message field: =%s\n %s\n",
             msgTypeString,aString);
        return messageType;
    }
    else  {
        messageType = skini_msgs[which].type;
      // printf("Message Token = %s type = %i\n", msgTypeString,messageType);
    }
    aField += 1;

    if (someStrings[aField][0] == '=') {
        deltaTime = (MY_FLOAT) atof(&someStrings[aField][1]);
        deltaTime = -deltaTime;
    }
    else {
        deltaTime = (MY_FLOAT) atof(someStrings[aField]);
    }
    // printf("DeltaTime = %f\n",deltaTime);
    aField += 1;
    
    channel = atoi(someStrings[aField]);    
    // printf("Channel = %i\n",channel);
    aField += 1;
    
    if (skini_msgs[which].data2 != NOPE)    {
        if (skini_msgs[which].data2 == SK_INT)       {
        byteTwoInt = atoi(someStrings[aField]);    
        byteTwo = (MY_FLOAT) byteTwoInt;
        }
        else if (skini_msgs[which].data2 == SK_DBL)       {
        byteTwo = (MY_FLOAT) atof(someStrings[aField]);    
        byteTwoInt = (long) byteTwo;
        }
        else if (skini_msgs[which].data2 == SK_STR)       {
        temp = somePointrs[aField-1];    /*  Hack Danger Here, Why -1??? */
        temp2 = 0;
        while (aString[temp] != '\n')   { 
          remainderString[temp2++] = aString[temp++];
        }
        remainderString[temp2] = 0;
      }
        else {
        byteTwoInt = skini_msgs[which].data2;
        byteTwo = (MY_FLOAT) byteTwoInt;
        aField -= 1;
        }
        
        aField += 1;
        if (skini_msgs[which].data3 != NOPE)    {
        if (skini_msgs[which].data3 == SK_INT)        {
          byteThreeInt = atoi(someStrings[aField]);    
          byteThree = (MY_FLOAT) byteThreeInt;
        }
        else if (skini_msgs[which].data3 == SK_DBL)   {
          byteThree = (MY_FLOAT) atof(someStrings[aField]);    
          byteThreeInt = (long) byteThree;
        }
        else if (skini_msgs[which].data3 == SK_STR)   {
          temp = somePointrs[aField-1]; /*  Hack Danger Here, Why -1??? */
          temp2 = 0;
          while (aString[temp] != '\n')   { 
            remainderString[temp2++] = aString[temp++];
          }
          remainderString[temp2] = 0;
        }
        else {
          byteThreeInt = skini_msgs[which].data3;
          byteThree = (MY_FLOAT) byteThreeInt;
        }
        }
        else {
        byteThreeInt = byteTwoInt;
        byteThree = byteTwo;
        }
    }
  }
  return messageType;
}

long SKINI ::  nextMessage()
{
  int notDone;
  char inputString[1024];

  notDone = 1;
  while (notDone)     {
    notDone = 0;
    if (!fgets(inputString,1024,myFile)) {    
        printf("// End of Score. Thanks for using SKINI!!\n");
        messageType = -1;
        return messageType;
    }
    else if (parseThis(inputString) == 0)   {
        notDone = 1;
    }
  }
  return messageType;
}

long SKINI ::  getType() const
{
  return messageType;
}
 
long SKINI ::  getChannel() const
{
  return channel;
}

MY_FLOAT SKINI :: getDelta() const
{
  return deltaTime;
}

MY_FLOAT SKINI :: getByteTwo() const
{
  return byteTwo;
}

long SKINI :: getByteTwoInt() const
{
  return byteTwoInt;
}

MY_FLOAT SKINI :: getByteThree() const
{
  return byteThree;
}

long SKINI :: getByteThreeInt() const
{
  return byteThreeInt;
}

const char* SKINI :: getRemainderString()
{
  return remainderString;
}

const char* SKINI :: getMessageTypeString()
{
  return msgTypeString;
}

const char* SKINI :: whatsThisType(long type)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++ ) {
    if ( type == skini_msgs[i].type ) {
        strcat(whatString, skini_msgs[i].messageString);
        strcat(whatString, ",");
    }
  }
  return whatString;            
}

const char* SKINI :: whatsThisController(long contNum)
{
  int i = 0;
  whatString[0] = 0;
  for ( i=0; i<__SK_MaxMsgTypes_; i++) {
    if ( skini_msgs[i].type == __SK_ControlChange_
        && contNum == skini_msgs[i].data2) {
        strcat(whatString, skini_msgs[i].messageString);
        strcat(whatString, ",");
    }
  }
  return whatString;
}


/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument contains up to 5 attack waves,
    5 looped waves, and an ADSR envelope.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Sampler :: Sampler()
{
  // We don't make the waves here yet, because
  // we don't know what they will be.
  adsr = new ADSR;
  baseFrequency = 440.0;
  filter = new OnePole;
  attackGain = 0.25;
  loopGain = 0.25;
  whichOne = 0;

  // chuck
  m_frequency = baseFrequency;
}  

Sampler :: ~Sampler()
{
  delete adsr;
  delete filter;
}

void Sampler :: keyOn()
{
  adsr->keyOn();
  attacks[0]->reset();
}

void Sampler :: keyOff()
{
  adsr->keyOff();
}

void Sampler :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "Sampler: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Sampler :: tick()
{
  lastOutput = attackGain * attacks[whichOne]->tick();
  lastOutput += loopGain * loops[whichOne]->tick();
  lastOutput = filter->tick(lastOutput);
  lastOutput *= adsr->tick();
  return lastOutput;
}
/***************************************************/
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Saxofony :: Saxofony(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // Initialize blowing position to 0.2 of length / 2.
  position = 0.2;
  delays[0] = (DelayL *) new DelayL( (1.0-position) * (length >> 1), length );
  delays[1] = (DelayL *) new DelayL( position * (length >> 1), length );

  reedTable = new ReedTabl;
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) 0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( "special:sinewave", TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);

  outputGain = (MY_FLOAT) 0.3;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;

  // TODO: what is default?
  m_frequency = length >> 1;
  // assuming 1.0 velocity
  m_rate = .005;
  // reverse: reedTable->setSlope( 0.1 + (0.4 * norm) )
  m_stiffness = (reedTable->slope - .1) / .4;
  // reverse: reedTable->setOffset(0.4 + ( norm * 0.6));
  m_aperture =  (reedTable->offSet - .4) / .6;
  // reverse: noiseGain = ( norm * 0.4 );
  m_noiseGain = noiseGain / .4;
  // reverse: vibratoGain = ( norm * 0.5 );
  m_vibratoGain = vibratoGain / .5;
  // pressure from envelope
  m_pressure = envelope->value;
}

Saxofony :: ~Saxofony()
{
  delete delays[0];
  delete delays[1];
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Saxofony :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Saxofony :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Saxofony: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  // chuck
  m_frequency = freakency;

  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 3.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  delays[0]->setDelay((1.0-position) * delay);
  delays[1]->setDelay(position * delay);
}

void Saxofony :: setBlowPosition(MY_FLOAT aPosition)
{
  if (position == aPosition) return;

  if (aPosition < 0.0) position = 0.0;
  else if (aPosition > 1.0) position = 1.0;
  else position = aPosition;

  MY_FLOAT total_delay = delays[0]->getDelay();
  total_delay += delays[1]->getDelay();

  delays[0]->setDelay((1.0-position) * total_delay);
  delays[1]->setDelay(position * total_delay);
}

void Saxofony :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Saxofony :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0);
}

void Saxofony :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Saxofony: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Saxofony :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Saxofony: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Saxofony :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  temp = -0.95 * filter->tick( delays[0]->lastOut() );
  lastOutput = temp - delays[1]->lastOut();
  pressureDiff = breathPressure - lastOutput;
  delays[1]->tick(temp);
  delays[0]->tick(breathPressure - (pressureDiff * reedTable->tick(pressureDiff)) - temp);

  lastOutput *= outputGain;
  return lastOutput;
}

void Saxofony :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Saxofony: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Saxofony: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_ReedStiffness_) { // 2
    reedTable->setSlope( 0.1 + (0.4 * norm) );
    m_stiffness = norm;
  }
  else if (number == __SK_NoiseLevel_) { // 4
    noiseGain = ( norm * 0.4 );
    m_noiseGain = norm;
  }
  else if (number == 29) // 29
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) { // 1
    vibratoGain = ( norm * 0.5 );
    m_vibratoGain = norm;
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    envelope->setValue( norm );
    m_pressure = norm;
  }
  else if (number == 11) // 11
    this->setBlowPosition( norm );
  else if (number == 26) { // reed table offset
    reedTable->setOffset(0.4 + ( norm * 0.6));
    m_aperture = norm;
  }
  else
    CK_STDCERR << "[chuck](via STK): Saxofony: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Saxofony: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif

}


/***************************************************/
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event
    Modeling) is an algorithmic approach for
    simulating collisions of multiple independent
    sound producing objects.  This class is a
    meta-model that can simulate a Maraca, Sekere,
    Cabasa, Bamboo Wind Chimes, Water Drops,
    Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of
    Imitated Environmental Sounds) is a similar
    approach for the synthesis of environmental
    sounds.  This class implements simulations of
    breaking sticks, crunchy snow (or not), a
    wrench, sandpaper, and more.

    Control Change Numbers: 
       - Shake Energy = 2
       - System Decay = 4
       - Number Of Objects = 11
       - Resonance Frequency = 1
       - Shake Energy = 128
       - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Guiro = 3
        - Water Drops = 4
        - Bamboo Chimes = 5
        - Tambourine = 6
        - Sleigh Bells = 7
        - Sticks = 8
        - Crunch = 9
        - Wrench = 10
        - Sand Paper = 11
        - Coke Can = 12
        - Next Mug = 13
        - Penny + Mug = 14
        - Nickle + Mug = 15
        - Dime + Mug = 16
        - Quarter + Mug = 17
        - Franc + Mug = 18
        - Peso + Mug = 19
        - Big Rocks = 20
        - Little Rocks = 21
        - Tuned Bamboo Chimes = 22

    by Perry R. Cook, 1996 - 1999.
*/
/***************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>

int my_random(int max) //  Return Random Int Between 0 and max
{
  int temp = (int) ((float)max * rand() / (RAND_MAX + 1.0) );
  return temp;
}

MY_FLOAT float_random(MY_FLOAT max) // Return random float between 0.0 and max
{   
  MY_FLOAT temp = (MY_FLOAT) (max * rand() / (RAND_MAX + 1.0) );
  return temp;  
}

MY_FLOAT noise_tick() //  Return random MY_FLOAT float between -1.0 and 1.0
{
  MY_FLOAT temp = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  temp -= 1.0;
  return temp;
}

// Maraca
#define MARA_SOUND_DECAY 0.95
#define MARA_SYSTEM_DECAY 0.999
#define MARA_GAIN 20.0
#define MARA_NUM_BEANS 25
#define MARA_CENTER_FREQ 3200.0
#define MARA_RESON 0.96

// Sekere
#define SEKE_SOUND_DECAY 0.96
#define SEKE_SYSTEM_DECAY 0.999
#define SEKE_GAIN 20.0
#define SEKE_NUM_BEANS 64
#define SEKE_CENTER_FREQ 5500.0
#define SEKE_RESON 0.6

// Sandpaper
#define SANDPAPR_SOUND_DECAY 0.999
#define SANDPAPR_SYSTEM_DECAY 0.999
#define SANDPAPR_GAIN 0.5
#define SANDPAPR_NUM_GRAINS 128
#define SANDPAPR_CENTER_FREQ 4500.0
#define SANDPAPR_RESON 0.6

// Cabasa
#define CABA_SOUND_DECAY 0.96
#define CABA_SYSTEM_DECAY 0.997
#define CABA_GAIN 40.0
#define CABA_NUM_BEADS 512
#define CABA_CENTER_FREQ 3000.0
#define CABA_RESON 0.7

// Bamboo Wind Chimes
#define BAMB_SOUND_DECAY 0.95
#define BAMB_SYSTEM_DECAY 0.9999
#define BAMB_GAIN 2.0
#define BAMB_NUM_TUBES 1.25
#define BAMB_CENTER_FREQ0  2800.0
#define BAMB_CENTER_FREQ1  0.8 * 2800.0
#define BAMB_CENTER_FREQ2  1.2 * 2800.0
#define BAMB_RESON     0.995

// Tuned Bamboo Wind Chimes (Anklung)
#define TBAMB_SOUND_DECAY 0.95
#define TBAMB_SYSTEM_DECAY 0.9999
#define TBAMB_GAIN 1.0
#define TBAMB_NUM_TUBES 1.25
#define TBAMB_CENTER_FREQ0 1046.6
#define TBAMB_CENTER_FREQ1  1174.8
#define TBAMB_CENTER_FREQ2  1397.0
#define TBAMB_CENTER_FREQ3  1568.0
#define TBAMB_CENTER_FREQ4  1760.0
#define TBAMB_CENTER_FREQ5  2093.3
#define TBAMB_CENTER_FREQ6  2350.0
#define TBAMB_RESON    0.996

// Water Drops
#define WUTR_SOUND_DECAY 0.95
#define WUTR_SYSTEM_DECAY 0.996
#define WUTR_GAIN 1.0
#define WUTR_NUM_SOURCES 10
#define WUTR_CENTER_FREQ0  450.0
#define WUTR_CENTER_FREQ1  600.0
#define WUTR_CENTER_FREQ2  750.0 
#define WUTR_RESON   0.9985
#define WUTR_FREQ_SWEEP  1.0001

// Tambourine
#define TAMB_SOUND_DECAY 0.95
#define TAMB_SYSTEM_DECAY 0.9985
#define TAMB_GAIN 5.0
#define TAMB_NUM_TIMBRELS 32
#define TAMB_SHELL_FREQ 2300
#define TAMB_SHELL_GAIN 0.1
#define TAMB_SHELL_RESON 0.96
#define TAMB_CYMB_FREQ1  5600
#define TAMB_CYMB_FREQ2 8100
#define TAMB_CYMB_RESON 0.99

// Sleighbells
#define SLEI_SOUND_DECAY 0.97
#define SLEI_SYSTEM_DECAY 0.9994
#define SLEI_GAIN 1.0
#define SLEI_NUM_BELLS 32
#define SLEI_CYMB_FREQ0 2500
#define SLEI_CYMB_FREQ1 5300
#define SLEI_CYMB_FREQ2 6500
#define SLEI_CYMB_FREQ3 8300
#define SLEI_CYMB_FREQ4 9800
#define SLEI_CYMB_RESON 0.99 

// Guiro
#define GUIR_SOUND_DECAY 0.95
#define GUIR_GAIN 10.0
#define GUIR_NUM_PARTS 128
#define GUIR_GOURD_FREQ  2500.0
#define GUIR_GOURD_RESON 0.97
#define GUIR_GOURD_FREQ2  4000.0
#define GUIR_GOURD_RESON2 0.97

// Wrench
#define WRENCH_SOUND_DECAY 0.95
#define WRENCH_GAIN 5
#define WRENCH_NUM_PARTS 128
#define WRENCH_FREQ  3200.0
#define WRENCH_RESON 0.99
#define WRENCH_FREQ2  8000.0
#define WRENCH_RESON2 0.992

// Cokecan
#define COKECAN_SOUND_DECAY 0.97
#define COKECAN_SYSTEM_DECAY 0.999
#define COKECAN_GAIN 0.8
#define COKECAN_NUM_PARTS 48
#define COKECAN_HELMFREQ 370
#define COKECAN_HELM_RES  0.99
#define COKECAN_METLFREQ0 1025
#define COKECAN_METLFREQ1 1424
#define COKECAN_METLFREQ2 2149
#define COKECAN_METLFREQ3 3596
#define COKECAN_METL_RES 0.992 

// PhOLIES (Physically-Oriented Library of Imitated Environmental
// Sounds), Perry Cook, 1997-8

// Stix1
#define STIX1_SOUND_DECAY 0.96
#define STIX1_SYSTEM_DECAY 0.998
#define STIX1_GAIN 30.0
#define STIX1_NUM_BEANS 2
#define STIX1_CENTER_FREQ 5500.0
#define STIX1_RESON 0.6

// Crunch1
#define CRUNCH1_SOUND_DECAY 0.95
#define CRUNCH1_SYSTEM_DECAY 0.99806
#define CRUNCH1_GAIN 20.0
#define CRUNCH1_NUM_BEADS 7
#define CRUNCH1_CENTER_FREQ 800.0
#define CRUNCH1_RESON 0.95

// Nextmug
#define NEXTMUG_SOUND_DECAY 0.97
#define NEXTMUG_SYSTEM_DECAY 0.9995
#define NEXTMUG_GAIN 0.8
#define NEXTMUG_NUM_PARTS 3
#define NEXTMUG_FREQ0 2123
#define NEXTMUG_FREQ1 4518
#define NEXTMUG_FREQ2 8856
#define NEXTMUG_FREQ3 10753
#define NEXTMUG_RES 0.997 

#define PENNY_FREQ0 11000
#define PENNY_FREQ1 5200
#define PENNY_FREQ2 3835
#define PENNY_RES   0.999

#define NICKEL_FREQ0 5583
#define NICKEL_FREQ1 9255
#define NICKEL_FREQ2 9805
#define NICKEL_RES   0.9992

#define DIME_FREQ0 4450
#define DIME_FREQ1 4974
#define DIME_FREQ2 9945
#define DIME_RES   0.9993

#define QUARTER_FREQ0 1708
#define QUARTER_FREQ1 8863
#define QUARTER_FREQ2 9045
#define QUARTER_RES   0.9995

#define FRANC_FREQ0 5583
#define FRANC_FREQ1 11010
#define FRANC_FREQ2 1917
#define FRANC_RES   0.9995

#define PESO_FREQ0 7250
#define PESO_FREQ1 8150
#define PESO_FREQ2 10060
#define PESO_RES   0.9996

// Big Gravel
#define BIGROCKS_SOUND_DECAY 0.98
#define BIGROCKS_SYSTEM_DECAY 0.9965
#define BIGROCKS_GAIN 20.0
#define BIGROCKS_NUM_PARTS 23
#define BIGROCKS_FREQ 6460
#define BIGROCKS_RES 0.932 

// Little Gravel
#define LITLROCKS_SOUND_DECAY 0.98
#define LITLROCKS_SYSTEM_DECAY 0.99586
#define LITLROCKS_GAIN 20.0
#define LITLROCKS_NUM_PARTS 1600
#define LITLROCKS_FREQ 9000
#define LITLROCKS_RES 0.843 

// Finally ... the class code!


Shakers :: Shakers()
{
  int i;

  instType = 0;
  shakeEnergy = 0.0;
  nFreqs = 0;
  sndLevel = 0.0;

  for ( i=0; i<MAX_FREQS; i++ ) {
    inputs[i] = 0.0;
    outputs[i][0] = 0.0;
    outputs[i][1] = 0.0;
    coeffs[i][0] = 0.0;
    coeffs[i][1] = 0.0;
    gains[i] = 0.0;
    center_freqs[i] = 0.0;
    resons[i] =  0.0;
    freq_rand[i] = 0.0;
    freqalloc[i] = 0;
  }

  soundDecay = 0.0;
  systemDecay = 0.0;
  nObjects = 0.0;
  collLikely = 0.0;
  totalEnergy = 0.0;
  ratchet = 0.0;
  ratchetDelta = 0.0005;
  lastRatchetPos = 0;
  finalZ[0] = 0.0;
  finalZ[1] = 0.0;
  finalZ[2] = 0.0;
  finalZCoeffs[0] = 1.0;
  finalZCoeffs[1] = 0.0;
  finalZCoeffs[2] = 0.0;
  freq = 220.0;

  this->setupNum(instType);

  // chuck
  m_energy = totalEnergy;
  m_decay = systemDecay;
  m_objects = nObjects;
}

Shakers :: ~Shakers()
{
}

#define MAX_SHAKE 2000.0

char instrs[NUM_INSTR][10] = {
  "Maraca", "Cabasa", "Sekere", "Guiro",
  "Waterdrp", "Bamboo", "Tambourn", "Sleighbl", 
  "Stix1", "Crunch1", "Wrench", "SandPapr",
  "CokeCan", "NextMug", "PennyMug", "NicklMug",
  "DimeMug", "QuartMug", "FrancMug", "PesoMug",
  "BigRocks", "LitlRoks", "TBamboo"
};

int Shakers :: setupName(char* instr)
{
  int which = 0;

  for (int i=0;i<NUM_INSTR;i++) {
    if ( !strcmp(instr,instrs[i]) )
        which = i;
  }

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Shakers: Setting instrument to " << instrs[which] << CK_STDENDL;
#endif

  return this->setupNum(which);
}

void Shakers :: setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2)    {
  finalZCoeffs[0] = z0;
  finalZCoeffs[1] = z1;
  finalZCoeffs[2] = z2;
}

void Shakers :: setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay) {
  soundDecay = sndDecay;
  systemDecay = sysDecay;
}

int Shakers :: setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson) {
  if (which < MAX_FREQS)    {
    resons[which] = reson;
    center_freqs[which] = freq;
    t_center_freqs[which] = freq;
    coeffs[which][1] = reson * reson;
    coeffs[which][0] = -reson * 2.0 * cos(freq * TWO_PI / Stk::sampleRate());
    return 1;
  }
  else return 0;
}

int Shakers :: setupNum(int inst)
{
  int i, rv = 0;
  MY_FLOAT temp;

  inst %= 23; // chuck hack
  if (inst == 1) { // Cabasa
    rv = inst;
    nObjects = CABA_NUM_BEADS;
    defObjs[inst] = CABA_NUM_BEADS;
    setDecays(CABA_SOUND_DECAY, CABA_SYSTEM_DECAY);
    defDecays[inst] = CABA_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = CABA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CABA_CENTER_FREQ,CABA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 2) { // Sekere
    rv = inst;
    nObjects = SEKE_NUM_BEANS;
    defObjs[inst] = SEKE_NUM_BEANS;
    this->setDecays(SEKE_SOUND_DECAY,SEKE_SYSTEM_DECAY);
    defDecays[inst] = SEKE_SYSTEM_DECAY;
    decayScale[inst] = 0.94;
    nFreqs = 1;
    baseGain = SEKE_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SEKE_CENTER_FREQ,SEKE_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 3) { //  Guiro
    rv = inst;
    nObjects = GUIR_NUM_PARTS;
    defObjs[inst] = GUIR_NUM_PARTS;
    setDecays(GUIR_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 1.0;
    nFreqs = 2;
    baseGain = GUIR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,GUIR_GOURD_FREQ,GUIR_GOURD_RESON);
    setFreqAndReson(1,GUIR_GOURD_FREQ2,GUIR_GOURD_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 4) { //  Water Drops
    rv = inst;
    nObjects = WUTR_NUM_SOURCES;
    defObjs[inst] = WUTR_NUM_SOURCES;
    setDecays(WUTR_SOUND_DECAY,WUTR_SYSTEM_DECAY);
    defDecays[inst] = WUTR_SYSTEM_DECAY;
    decayScale[inst] = 0.8;
    nFreqs = 3;
    baseGain = WUTR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(1,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(2,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 5) { // Bamboo
    rv = inst;
    nObjects = BAMB_NUM_TUBES;
    defObjs[inst] = BAMB_NUM_TUBES;
    setDecays(BAMB_SOUND_DECAY, BAMB_SYSTEM_DECAY);
    defDecays[inst] = BAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 3;
    baseGain = BAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,BAMB_CENTER_FREQ0,BAMB_RESON);
    setFreqAndReson(1,BAMB_CENTER_FREQ1,BAMB_RESON);
    setFreqAndReson(2,BAMB_CENTER_FREQ2,BAMB_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 6) { // Tambourine
    rv = inst;
    nObjects = TAMB_NUM_TIMBRELS;
    defObjs[inst] = TAMB_NUM_TIMBRELS;
    setDecays(TAMB_SOUND_DECAY,TAMB_SYSTEM_DECAY);
    defDecays[inst] = TAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 3;
    baseGain = TAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp*TAMB_SHELL_GAIN;
    gains[1]=temp*0.8;
    gains[2]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.05;
    freq_rand[2] = 0.05;
    setFreqAndReson(0,TAMB_SHELL_FREQ,TAMB_SHELL_RESON);
    setFreqAndReson(1,TAMB_CYMB_FREQ1,TAMB_CYMB_RESON);
    setFreqAndReson(2,TAMB_CYMB_FREQ2,TAMB_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 7) { // Sleighbell
    rv = inst;
    nObjects = SLEI_NUM_BELLS;
    defObjs[inst] = SLEI_NUM_BELLS;
    setDecays(SLEI_SOUND_DECAY,SLEI_SYSTEM_DECAY);
    defDecays[inst] = SLEI_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 5;
    baseGain = SLEI_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp*0.5;
    gains[4]=temp*0.3;
    for (i=0;i<nFreqs;i++)  {
        freqalloc[i] = 1;
        freq_rand[i] = 0.03;
    }
    setFreqAndReson(0,SLEI_CYMB_FREQ0,SLEI_CYMB_RESON);
    setFreqAndReson(1,SLEI_CYMB_FREQ1,SLEI_CYMB_RESON);
    setFreqAndReson(2,SLEI_CYMB_FREQ2,SLEI_CYMB_RESON);
    setFreqAndReson(3,SLEI_CYMB_FREQ3,SLEI_CYMB_RESON);
    setFreqAndReson(4,SLEI_CYMB_FREQ4,SLEI_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 8) { // Stix1
    rv = inst;
    nObjects = STIX1_NUM_BEANS;
    defObjs[inst] = STIX1_NUM_BEANS;
    setDecays(STIX1_SOUND_DECAY,STIX1_SYSTEM_DECAY);
    defDecays[inst] = STIX1_SYSTEM_DECAY;

    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = STIX1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,STIX1_CENTER_FREQ,STIX1_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 9) { // Crunch1
    rv = inst;
    nObjects = CRUNCH1_NUM_BEADS;
    defObjs[inst] = CRUNCH1_NUM_BEADS;
    setDecays(CRUNCH1_SOUND_DECAY,CRUNCH1_SYSTEM_DECAY);
    defDecays[inst] = CRUNCH1_SYSTEM_DECAY;
    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = CRUNCH1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CRUNCH1_CENTER_FREQ,CRUNCH1_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 10) { // Wrench
    rv = inst;
    nObjects = WRENCH_NUM_PARTS;
    defObjs[inst] = WRENCH_NUM_PARTS;
    setDecays(WRENCH_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 0.98;
    nFreqs = 2;
    baseGain = WRENCH_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,WRENCH_FREQ,WRENCH_RESON);
    setFreqAndReson(1,WRENCH_FREQ2,WRENCH_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 11) { // Sandpapr
    rv = inst;
    nObjects = SANDPAPR_NUM_GRAINS;
    defObjs[inst] = SANDPAPR_NUM_GRAINS;
    this->setDecays(SANDPAPR_SOUND_DECAY,SANDPAPR_SYSTEM_DECAY);
    defDecays[inst] = SANDPAPR_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = SANDPAPR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SANDPAPR_CENTER_FREQ,SANDPAPR_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 12) { // Cokecan
    rv = inst;
    nObjects = COKECAN_NUM_PARTS;
    defObjs[inst] = COKECAN_NUM_PARTS;
    setDecays(COKECAN_SOUND_DECAY,COKECAN_SYSTEM_DECAY);
    defDecays[inst] = COKECAN_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 5;
    baseGain = COKECAN_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*1.8;
    gains[2]=temp*1.8;
    gains[3]=temp*1.8;
    gains[4]=temp*1.8;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    setFreqAndReson(0,COKECAN_HELMFREQ,COKECAN_HELM_RES);
    setFreqAndReson(1,COKECAN_METLFREQ0,COKECAN_METL_RES);
    setFreqAndReson(2,COKECAN_METLFREQ1,COKECAN_METL_RES);
    setFreqAndReson(3,COKECAN_METLFREQ2,COKECAN_METL_RES);
    setFreqAndReson(4,COKECAN_METLFREQ3,COKECAN_METL_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst>12 && inst<20) { // Nextmug
    rv = inst;
    nObjects = NEXTMUG_NUM_PARTS;
    defObjs[inst] = NEXTMUG_NUM_PARTS;
    setDecays(NEXTMUG_SOUND_DECAY,NEXTMUG_SYSTEM_DECAY);
    defDecays[inst] = NEXTMUG_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 4;
    baseGain = NEXTMUG_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*0.8;
    gains[2]=temp*0.6;
    gains[3]=temp*0.4;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    setFreqAndReson(0,NEXTMUG_FREQ0,NEXTMUG_RES);
    setFreqAndReson(1,NEXTMUG_FREQ1,NEXTMUG_RES);
    setFreqAndReson(2,NEXTMUG_FREQ2,NEXTMUG_RES);
    setFreqAndReson(3,NEXTMUG_FREQ3,NEXTMUG_RES);
    setFinalZs(1.0,0.0,-1.0);

    if (inst == 14) { // Mug + Penny
      nFreqs = 7;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,PENNY_FREQ0,PENNY_RES);
      setFreqAndReson(5,PENNY_FREQ1,PENNY_RES);
      setFreqAndReson(6,PENNY_FREQ2,PENNY_RES);
    }
    else if (inst == 15) { // Mug + Nickel
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,NICKEL_FREQ0,NICKEL_RES);
      setFreqAndReson(5,NICKEL_FREQ1,NICKEL_RES);
      setFreqAndReson(6,NICKEL_FREQ2,NICKEL_RES);
    }
    else if (inst == 16) { // Mug + Dime
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,DIME_FREQ0,DIME_RES);
      setFreqAndReson(5,DIME_FREQ1,DIME_RES);
      setFreqAndReson(6,DIME_FREQ2,DIME_RES);
    }
    else if (inst == 17) { // Mug + Quarter
      nFreqs = 6;
      gains[4] = temp*1.3;
      gains[5] = temp*1.0;
      gains[6] = temp*0.8;
      setFreqAndReson(4,QUARTER_FREQ0,QUARTER_RES);
      setFreqAndReson(5,QUARTER_FREQ1,QUARTER_RES);
      setFreqAndReson(6,QUARTER_FREQ2,QUARTER_RES);
    }
    else if (inst == 18) { // Mug + Franc
      nFreqs = 6;
      gains[4] = temp*0.7;
      gains[5] = temp*0.4;
      gains[6] = temp*0.3;
      setFreqAndReson(4,FRANC_FREQ0,FRANC_RES);
      setFreqAndReson(5,FRANC_FREQ1,FRANC_RES);
      setFreqAndReson(6,FRANC_FREQ2,FRANC_RES);
    }
    else if (inst == 19) { // Mug + Peso
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*1.2;
      gains[6] = temp*0.7;
      setFreqAndReson(4,PESO_FREQ0,PESO_RES);
      setFreqAndReson(5,PESO_FREQ1,PESO_RES);
      setFreqAndReson(6,PESO_FREQ2,PESO_RES);
    }
  }
  else if (inst == 20) { // Big Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = BIGROCKS_NUM_PARTS;
    defObjs[inst] = BIGROCKS_NUM_PARTS;
    setDecays(BIGROCKS_SOUND_DECAY,BIGROCKS_SYSTEM_DECAY);
    defDecays[inst] = BIGROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = BIGROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.11;
    setFreqAndReson(0,BIGROCKS_FREQ,BIGROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 21) { // Little Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = LITLROCKS_NUM_PARTS;
    defObjs[inst] = LITLROCKS_NUM_PARTS;
    setDecays(LITLROCKS_SOUND_DECAY,LITLROCKS_SYSTEM_DECAY);
    defDecays[inst] = LITLROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = LITLROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.18;
    setFreqAndReson(0,LITLROCKS_FREQ,LITLROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 22) { // Tuned Bamboo
    rv = inst;
    nObjects = TBAMB_NUM_TUBES;
    defObjs[inst] = TBAMB_NUM_TUBES;
    setDecays(TBAMB_SOUND_DECAY, TBAMB_SYSTEM_DECAY);
    defDecays[inst] = TBAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 7;
    baseGain = TBAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp;
    gains[4]=temp;
    gains[5]=temp;
    gains[6]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    freqalloc[6] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    freq_rand[2] = 0.0;
    freq_rand[3] = 0.0;
    freq_rand[4] = 0.0;
    freq_rand[5] = 0.0;
    freq_rand[6] = 0.0;
    setFreqAndReson(0,TBAMB_CENTER_FREQ0,TBAMB_RESON);
    setFreqAndReson(1,TBAMB_CENTER_FREQ1,TBAMB_RESON);
    setFreqAndReson(2,TBAMB_CENTER_FREQ2,TBAMB_RESON);
    setFreqAndReson(3,TBAMB_CENTER_FREQ3,TBAMB_RESON);
    setFreqAndReson(4,TBAMB_CENTER_FREQ4,TBAMB_RESON);
    setFreqAndReson(5,TBAMB_CENTER_FREQ5,TBAMB_RESON);
    setFreqAndReson(6,TBAMB_CENTER_FREQ6,TBAMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else { // Maraca (inst == 0) or default
    rv = 0;
    nObjects = MARA_NUM_BEANS;
    defObjs[0] = MARA_NUM_BEANS;
    setDecays(MARA_SOUND_DECAY,MARA_SYSTEM_DECAY);
    defDecays[0] = MARA_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 1;
    baseGain = MARA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,MARA_CENTER_FREQ,MARA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }

  // chuck
  m_noteNum = inst;
  m_energy = totalEnergy;
  m_decay = systemDecay;
  m_objects = nObjects;

  return rv;
}

// chuck function!

void Shakers :: ck_noteOn(MY_FLOAT amplitude ) { 
  if (instType !=  m_noteNum) instType = this->setupNum(m_noteNum);
  shakeEnergy += amplitude * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;
}

void Shakers :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Yep ... pretty kludgey, but it works!
  int noteNum = (int) ((12*log(frequency/220.0)/log(2.0)) + 57.01) % 32;
  m_noteNum = noteNum;
  if (instType !=  noteNum) instType = this->setupNum(noteNum);
  shakeEnergy += amplitude * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Shakers: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Shakers :: noteOff(MY_FLOAT amplitude)
{
  shakeEnergy = 0.0;
  if (instType==10 || instType==3) ratchetPos = 0;
}

#define MIN_ENERGY 0.3

MY_FLOAT Shakers :: tick()
{
  MY_FLOAT data;
  MY_FLOAT temp_rand;
  int i;

  if (instType == 4) {
    if (shakeEnergy > MIN_ENERGY)   {
      lastOutput = wuter_tick();
      lastOutput *= 0.0001;
    }
    else {
      lastOutput = 0.0;
    }
  }
  else if (instType == 22) {
    lastOutput = tbamb_tick();
  }
  else if (instType == 10 || instType == 3) {
    if (ratchetPos > 0) {
      ratchet -= (ratchetDelta + (0.002*totalEnergy));
      if (ratchet < 0.0) {
        ratchet = 1.0;
        ratchetPos -= 1;
        }
      totalEnergy = ratchet;
      lastOutput = ratchet_tick();
      lastOutput *= 0.0001;
    }
    else lastOutput = 0.0;
  }
  else  {
    //  MY_FLOAT generic_tick() {
    if (shakeEnergy > MIN_ENERGY) {
      shakeEnergy *= systemDecay;               // Exponential system decay
      if (float_random(1024.0) < nObjects) {
        sndLevel += shakeEnergy;   
        for (i=0;i<nFreqs;i++) {
          if (freqalloc[i]) {
            temp_rand = t_center_freqs[i] * (1.0 + (freq_rand[i] * noise_tick()));
            coeffs[i][0] = -resons[i] * 2.0 * cos(temp_rand * TWO_PI / Stk::sampleRate());
          }
        }
        }
      inputs[0] = sndLevel * noise_tick();      // Actual Sound is Random
      for (i=1; i<nFreqs; i++)  {
        inputs[i] = inputs[0];
      }
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)    {
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)   data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      lastOutput = data * 0.0001;
    }
    else lastOutput = 0.0;
  }

  return lastOutput;
}

void Shakers :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Shakers: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Shakers: Control value exceeds nominal range!" << CK_STDENDL;
  }

  MY_FLOAT temp;
  int i;

  if (number == __SK_Breath_) { // 2 ... energy
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3) {
        ratchetPos = (int) fabs(value - lastRatchetPos);
        ratchetDelta = 0.0002 * ratchetPos;
        lastRatchetPos = (int) value;
    }
  }
  // else if (number == __SK_ModFrequency_) { // 4 ... decay
  // CHUCK HACK: (actually this fixes things?)
  else if (number == __SK_FootControl_) { // 4 ... decay
    if (instType != 3 && instType != 10) {
      systemDecay = defDecays[instType] + ((value - 64.0) *
                                           decayScale[instType] *
                                           (1.0 - defDecays[instType]) / 64.0 );
      gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
      for (i=1;i<nFreqs;i++) gains[i] = gains[0];
      if (instType == 6) { // tambourine
        gains[0] *= TAMB_SHELL_GAIN;
        gains[1] *= 0.8;
      }
      else if (instType == 7) { // sleighbell
        gains[3] *= 0.5;
        gains[4] *= 0.3;
      }
      else if (instType == 12) { // cokecan
        for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
      }
      for (i=0;i<nFreqs;i++) gains[i] *= ((128-value)/100.0 + 0.36);
    }
  }
  // else if (number == __SK_FootControl_) { // 11 ... number of objects
  // CHUCK HACK: (actually this fixes things?)
  else if (number == __SK_Expression_) { // 11 ... number of objects
    if (instType == 5) // bamboo
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 0.3;
    else
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 1.1;
    gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    for (i=1;i<nFreqs;i++) gains[i] = gains[0];
    if (instType == 6) { // tambourine
      gains[0] *= TAMB_SHELL_GAIN;
      gains[1] *= 0.8;
    }
    else if (instType == 7) { // sleighbell
      gains[3] *= 0.5;
      gains[4] *= 0.3;
    }
    else if (instType == 12) { // cokecan
      for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
    }
    if (instType != 3 && instType != 10) {
        // reverse calculate decay setting
        double temp = (double) (64.0 * (systemDecay-defDecays[instType])/(decayScale[instType]*(1-defDecays[instType])) + 64.0);
        // scale gains by decay setting
        for (i=0;i<nFreqs;i++) gains[i] *= ((128-temp)/100.0 + 0.36);
    }
  }
  else if (number == __SK_ModWheel_) { // 1 ... resonance frequency
    for (i=0; i<nFreqs; i++)    {
      if (instType == 6 || instType == 2 || instType == 7) // limit range a bit for tambourine
        temp = center_freqs[i] * pow (1.008,value-64);
      else
        temp = center_freqs[i] * pow (1.015,value-64);
      t_center_freqs[i] = temp;

      coeffs[i][0] = -resons[i] * 2.0 * cos(temp * TWO_PI / Stk::sampleRate());
      coeffs[i][1] = resons[i]*resons[i];
    }
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3)    {
        ratchetPos = (int) fabs(value - lastRatchetPos);
        ratchetDelta = 0.0002 * ratchetPos;
        lastRatchetPos = (int) value;
    }
  }
  else  if (number == __SK_ShakerInst_) { // 1071
    instType = (int) (value + 0.5); //  Just to be safe
    this->setupNum(instType);
  }                                       
  else
    CK_STDCERR << "[chuck](via STK): Shakers: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Shakers: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

// KLUDGE-O-MATIC-O-RAMA

MY_FLOAT Shakers :: wuter_tick() {
  MY_FLOAT data;
  int j;
  shakeEnergy *= systemDecay;               // Exponential system decay
  if (my_random(32767) < nObjects) {     
    sndLevel = shakeEnergy;   
    j = my_random(3);
      if (j == 0)   {
      center_freqs[0] = WUTR_CENTER_FREQ1 * (0.75 + (0.25 * noise_tick()));
        gains[0] = fabs(noise_tick());
      }
      else if (j == 1)      {
      center_freqs[1] = WUTR_CENTER_FREQ1 * (1.0 + (0.25 * noise_tick()));
        gains[1] = fabs(noise_tick());
      }
      else  {
      center_freqs[2] = WUTR_CENTER_FREQ1 * (1.25 + (0.25 * noise_tick()));
        gains[2] = fabs(noise_tick());
      }
    }
    
  gains[0] *= resons[0];
  if (gains[0] >  0.001) {
    center_freqs[0]  *= WUTR_FREQ_SWEEP;
    coeffs[0][0] = -resons[0] * 2.0 * 
      cos(center_freqs[0] * TWO_PI / Stk::sampleRate());
  }
  gains[1] *= resons[1];
  if (gains[1] > 0.001) {
    center_freqs[1] *= WUTR_FREQ_SWEEP;
    coeffs[1][0] = -resons[1] * 2.0 * 
      cos(center_freqs[1] * TWO_PI / Stk::sampleRate());
  }
  gains[2] *= resons[2];
  if (gains[2] > 0.001) {
    center_freqs[2] *= WUTR_FREQ_SWEEP;
    coeffs[2][0] = -resons[2] * 2.0 * 
      cos(center_freqs[2] * TWO_PI / Stk::sampleRate());
  }
    
  sndLevel *= soundDecay;        // Each (all) event(s) 
                                 // decay(s) exponentially 
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick();     // Actual Sound is Random
  inputs[1] = inputs[0] * gains[1];
  inputs[2] = inputs[0] * gains[2];
  inputs[0] *= gains[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  data = gains[0]*outputs[0][0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
  data += gains[1]*outputs[1][0];
  inputs[2] -= outputs[2][0]*coeffs[2][0];
  inputs[2] -= outputs[2][1]*coeffs[2][1];
  outputs[2][1] = outputs[2][0];
  outputs[2][0] = inputs[2];
  data += gains[2]*outputs[2][0];
 
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data * 4;

  data = finalZ[2] - finalZ[0];
  return data;
}

MY_FLOAT Shakers :: ratchet_tick() {
  MY_FLOAT data;
  if (my_random(1024) < nObjects) {
    sndLevel += 512 * ratchet * totalEnergy;
  }
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick() * ratchet;
  sndLevel *= soundDecay;
         
  inputs[1] = inputs[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
     
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = gains[0]*outputs[0][1] + gains[1]*outputs[1][1];
  data = finalZ[0] - finalZ[2];
  return data;
}

MY_FLOAT Shakers :: tbamb_tick() {
  MY_FLOAT data, temp;
  static int which = 0;
  int i;

  if (shakeEnergy > MIN_ENERGY) {
      shakeEnergy *= systemDecay;    // Exponential system decay
      if (float_random(1024.0) < nObjects) {
        sndLevel += shakeEnergy;
        which = my_random(7);
      }  
      temp = sndLevel * noise_tick();      // Actual Sound is Random
      for (i=0;i<nFreqs;i++)    inputs[i] = 0;
      inputs[which] = temp;
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)    {
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)   data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      data = data * 0.0001;
  }
  else data = 0.0;
  return data;
}
/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Simple :: Simple()
{
  adsr = new ADSR;
  baseFrequency = (MY_FLOAT) 440.0;

  // Concatenate the STK rawwave path to the rawwave file
  loop = new WaveLoop( "special:impuls10", TRUE );

  filter = new OnePole(0.5);
  noise = new Noise;
  biquad = new BiQuad();

  setFrequency(baseFrequency);
  loopGain = 0.5;
}  

Simple :: ~Simple()
{
  delete adsr;
  delete loop;
  delete filter;
  delete biquad;
}

void Simple :: keyOn()
{
  adsr->keyOn();
}

void Simple :: keyOff()
{
  adsr->keyOff();
}

void Simple :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  keyOn();
  setFrequency(frequency);
  filter->setGain(amplitude); 

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Simple: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}
void Simple :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Simple: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Simple :: setFrequency(MY_FLOAT frequency)
{
  biquad->setResonance( frequency, 0.98, true );
  loop->setFrequency(frequency);

  // chuck
  m_frequency = frequency;
}

MY_FLOAT Simple :: tick()
{
  lastOutput = loopGain * loop->tick();
  biquad->tick( noise->tick() );
  lastOutput += (1.0 - loopGain) * biquad->lastOut();
  lastOutput = filter->tick( lastOutput );
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Simple :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Clarinet: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Clarinet: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_Breath_) // 2
    filter->setPole( 0.99 * (1.0 - (norm * 2.0)) );
  else if (number == __SK_NoiseLevel_) // 4
    loopGain = norm;
  else if (number == __SK_ModFrequency_) { // 11
    norm /= 0.2 * Stk::sampleRate();
    adsr->setAttackRate( norm );
    adsr->setDecayRate( norm );
    adsr->setReleaseRate( norm );
  }
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    CK_STDCERR << "[chuck](via STK): Simple: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Simple: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}


/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

 
SingWave :: SingWave(const char *fileName, bool raw)
{
  // An exception could be thrown here.
  wave = new WaveLoop( fileName, raw );

    rate = 1.0;
    sweepRate = 0.001;
    modulator = new Modulate();
    modulator->setVibratoRate( 6.0 );
    modulator->setVibratoGain( 0.04 );
    modulator->setRandomGain( 0.005 );
    envelope = new Envelope;
    pitchEnvelope = new Envelope;
    setFrequency( 75.0 );
    pitchEnvelope->setRate( 1.0 );
    this->tick();
    this->tick();
    pitchEnvelope->setRate( sweepRate * rate );
}

SingWave :: ~SingWave()
{
  delete wave;
    delete modulator;
    delete envelope;
    delete pitchEnvelope;
}

void SingWave :: reset()
{
  wave->reset();
    lastOutput = 0.0;
}

void SingWave :: normalize()
{
  wave->normalize();
}

void SingWave :: normalize(MY_FLOAT newPeak)
{
  wave->normalize( newPeak );
}

void SingWave :: setFrequency(MY_FLOAT frequency)
{
    m_freq = frequency;
    MY_FLOAT temp = rate;
    rate = wave->getSize() * frequency / Stk::sampleRate();
    temp -= rate;
    if ( temp < 0) temp = -temp;
    pitchEnvelope->setTarget( rate );
    pitchEnvelope->setRate( sweepRate * temp );
}

void SingWave :: setVibratoRate(MY_FLOAT aRate)
{
    modulator->setVibratoRate( aRate );
}

void SingWave :: setVibratoGain(MY_FLOAT gain)
{
    modulator->setVibratoGain(gain);
}

void SingWave :: setRandomGain(MY_FLOAT gain)
{
    modulator->setRandomGain(gain);
}

void SingWave :: setSweepRate(MY_FLOAT aRate)
{
    sweepRate = aRate;
}

void SingWave :: setGainRate(MY_FLOAT aRate)
{
    envelope->setRate(aRate);
}

void SingWave :: setGainTarget(MY_FLOAT target)
{
    envelope->setTarget(target);
}

void SingWave :: noteOn()
{
    envelope->keyOn();
}

void SingWave :: noteOff()
{
    envelope->keyOff();
}

MY_FLOAT SingWave :: tick()
{
  // Set the wave rate.
  MY_FLOAT newRate = pitchEnvelope->tick();
  newRate += newRate * modulator->tick();
  wave->setRate( newRate );

  lastOutput = wave->tick();
    lastOutput *= envelope->tick();
    
    return lastOutput;             
}

MY_FLOAT SingWave :: lastOut()
{
    return lastOutput;
}
/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Sitar :: Sitar(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  delay = length / 2.0;
  targetDelay = delay;

  loopFilter = new OneZero;
  loopFilter->setZero(0.01);

  envelope = new ADSR();
  envelope->setAllTimes(0.001, 0.04, 0.0, 0.5);

  noise = new Noise;
  this->clear();
}

Sitar :: ~Sitar()
{
  delete delayLine;
  delete loopFilter;
  delete noise;
  delete envelope;
}

void Sitar :: clear()
{
  delayLine->clear();
  loopFilter->clear();
}

void Sitar :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Sitar: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  targetDelay = (Stk::sampleRate() / freakency);
  delay = targetDelay * (1.0 + (0.05 * noise->tick()));
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.0000005);
  if (loopGain > 0.9995) loopGain = 0.9995;

  // chuck
  m_frequency = freakency;
}

void Sitar :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void Sitar :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  pluck(amplitude);
  amGain = 0.1 * amplitude;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Sitar: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Sitar :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: noteOff amplitude greater than 1.0!" << CK_STDENDL;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): Plucked: noteOff amplitude less than or zero!" << CK_STDENDL;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Plucked: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Sitar :: tick()
{
  if ( fabs(targetDelay - delay) > 0.001 ) {
    if (targetDelay < delay)
      delay *= 0.99999;
    else
      delay *= 1.00001;
    delayLine->setDelay(delay);
  }

  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) + 
                                (amGain * envelope->tick() * noise->tick()));
  
  return lastOutput;
}
/***************************************************/
/*! \class Sphere
    \brief STK sphere class.

    This class implements a spherical ball with
    radius, mass, position, and velocity parameters.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <math.h>

Sphere::Sphere(double initRadius)
{
  myRadius = initRadius;
  myMass = 1.0;
  myPosition = new Vector3D(0, 0, 0);
  myVelocity = new Vector3D(0, 0, 0);
};

Sphere::~Sphere()
{
  delete myPosition;
  delete myVelocity;
}

void Sphere::setPosition(double anX, double aY, double aZ)
{
  myPosition->setXYZ(anX, aY, aZ);
};

void Sphere::setVelocity(double anX, double aY, double aZ)
{
  myVelocity->setXYZ(anX, aY, aZ);
};

void Sphere::setRadius(double aRadius)
{
  myRadius = aRadius;
};

void Sphere::setMass(double aMass)
{
  myMass = aMass;
};

Vector3D* Sphere::getPosition()
{
  return myPosition;
};

Vector3D* Sphere::getRelativePosition(Vector3D* aPosition)
{
  workingVector.setXYZ(aPosition->getX() - myPosition->getX(),
                       aPosition->getY() - myPosition->getY(),  
                       aPosition->getZ() - myPosition->getZ());
  return &workingVector;
};

double Sphere::getVelocity(Vector3D* aVelocity)
{
  aVelocity->setXYZ(myVelocity->getX(), myVelocity->getY(), myVelocity->getZ());
  return myVelocity->getLength();
};

double Sphere::isInside(Vector3D *aPosition)
{
  // Return directed distance from aPosition to spherical boundary ( <
  // 0 if inside).
  double distance;
  Vector3D *tempVector;

  tempVector = this->getRelativePosition(aPosition);
  distance = tempVector->getLength();
  return distance - myRadius;
};

double Sphere::getRadius()
{
  return myRadius;
};

double Sphere::getMass()
{
  return myMass;
};

void Sphere::addVelocity(double anX, double aY, double aZ)
{
  myVelocity->setX(myVelocity->getX() + anX);
  myVelocity->setY(myVelocity->getY() + aY);
  myVelocity->setZ(myVelocity->getZ() + aZ);
}

void Sphere::tick(double timeIncrement)
{
  myPosition->setX(myPosition->getX() + (timeIncrement * myVelocity->getX()));
  myPosition->setY(myPosition->getY() + (timeIncrement * myVelocity->getY()));
  myPosition->setZ(myPosition->getZ() + (timeIncrement * myVelocity->getZ()));
};

/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <string.h>
#include <math.h>

StifKarp :: StifKarp(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA(0.5 * length, length);
  combDelay = new DelayL( 0.2 * length, length);

  filter = new OneZero();
  noise = new Noise();
  biQuad[0] = new BiQuad();
  biQuad[1] = new BiQuad();
  biQuad[2] = new BiQuad();
  biQuad[3] = new BiQuad();

  pluckAmplitude = 0.3;
  pickupPosition = (MY_FLOAT) 0.4;
  lastFrequency = lowestFrequency * 2.0;
  lastLength = length * 0.5;
  stretching = 0.9999;
  baseLoopGain = 0.995;
  loopGain = 0.999;

  clear();
}

StifKarp :: ~StifKarp()
{
  delete delayLine;
  delete combDelay;
  delete filter;
  delete noise;
  delete biQuad[0];
  delete biQuad[1];
  delete biQuad[2];
  delete biQuad[3];
}

void StifKarp :: clear()
{
  delayLine->clear();
  combDelay->clear();
  filter->clear();
}

void StifKarp :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency; 
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    lastFrequency = 220.0;
  }

  lastLength = Stk::sampleRate() / lastFrequency;
  MY_FLOAT delay = lastLength - 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if (loopGain >= 1.0) loopGain = (MY_FLOAT) 0.99999;

  setStretch(stretching);

  combDelay->setDelay((MY_FLOAT) 0.5 * pickupPosition * lastLength); 

  // chuck
  m_frequency = lastFrequency;
}

void StifKarp :: setStretch(MY_FLOAT stretch)
{
  stretching = stretch;
  MY_FLOAT coefficient;
  MY_FLOAT freq = lastFrequency * 2.0;
  MY_FLOAT dFreq = ( (0.5 * Stk::sampleRate()) - freq ) * 0.25;
  MY_FLOAT temp = 0.5 + (stretch * 0.5);
  if (temp > 0.9999) temp = 0.9999;
  for (int i=0; i<4; i++)   {
    coefficient = temp * temp;
    biQuad[i]->setA2( coefficient );
    biQuad[i]->setB0( coefficient );
    biQuad[i]->setB2( 1.0 );

    coefficient = -2.0 * temp * cos(TWO_PI * freq / Stk::sampleRate());
    biQuad[i]->setA1( coefficient );
    biQuad[i]->setB1( coefficient );

    freq += dFreq;
  }
}

void StifKarp :: setPickupPosition(MY_FLOAT position) {
  pickupPosition = position;
  if ( position < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: setPickupPosition parameter is less than zero!" << CK_STDENDL;
    pickupPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: setPickupPosition parameter is greater than 1.0!" << CK_STDENDL;
    pickupPosition = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay(0.5 * pickupPosition * lastLength); 
}

void StifKarp :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void StifKarp :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: pluck amplitude greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: pluck amplitude less than zero!" << CK_STDENDL;
    gain = 0.0;
  }

  pluckAmplitude = amplitude;
  for (long i=0; i<length; i++)  {
    // Fill delay with noise additively with current contents.
    delayLine->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude);
    //delayLine->tick( combDelay->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude));
  }
}

void StifKarp :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): StifKarp: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void StifKarp :: noteOff(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: noteOff amplitude greater than 1.0!" << CK_STDENDL;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    CK_STDCERR << "[chuck](via STK): StifKarp: noteOff amplitude less than zero!" << CK_STDENDL;
    gain = 0.0;
  }
  loopGain =  (1.0 - gain) * 0.5;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): StifPluck: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT StifKarp :: tick()
{
  MY_FLOAT temp = delayLine->lastOut() * loopGain;

  // Calculate allpass stretching.
  for (int i=0; i<4; i++)
    temp = biQuad[i]->tick(temp);

  // Moving average filter.
  temp = filter->tick(temp);

  lastOutput = delayLine->tick(temp);
  lastOutput = lastOutput - combDelay->tick(lastOutput);
  return lastOutput;
}

void StifKarp :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): StifKarp: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): StifKarp: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_PickPosition_) // 4
    setPickupPosition( norm );
  else if (number == __SK_StringDamping_) { // 11
    setBaseLoopGain( 0.97 + (norm * 0.03) );
    m_sustain = norm;
  }
  else if (number == __SK_StringDetune_) // 1
    setStretch( 0.9 + (0.1 * (1.0 - norm)) );
  else
    CK_STDCERR << "[chuck](via STK): StifKarp: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): StifKarp: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate can be queried and
    modified via Stk.  In addition, this class
    provides error handling and byte-swapping
    functions.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <string.h>

MY_FLOAT Stk :: srate = (MY_FLOAT) SRATE;
std::string Stk :: rawwavepath = RAWWAVE_PATH;
const Stk::STK_FORMAT Stk :: STK_SINT8 = 1;
const Stk::STK_FORMAT Stk :: STK_SINT16 = 2;
const Stk::STK_FORMAT Stk :: STK_SINT32 = 8;
const Stk::STK_FORMAT Stk :: MY_FLOAT32 = 16;
const Stk::STK_FORMAT Stk :: MY_FLOAT64 = 32;

Stk :: Stk(void)
{
}

Stk :: ~Stk(void)
{
}

MY_FLOAT Stk :: sampleRate(void)
{
  return srate;
}

void Stk :: setSampleRate(MY_FLOAT newRate)
{
  if (newRate > 0)
    srate = newRate;
}

std::string Stk :: rawwavePath(void)
{
  return rawwavepath;
}

void Stk :: setRawwavePath(std::string newPath)
{
  if ( !newPath.empty() )
    rawwavepath = newPath;

  // Make sure the path includes a "/"
  if ( rawwavepath[rawwavepath.length()-1] != '/' )
    rawwavepath += "/";
}

void Stk :: swap16(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 2nd bytes
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap32(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 4th bytes
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  //Swap 2nd and 3rd bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

void Stk :: swap64(unsigned char *ptr)
{
  register unsigned char val;

  // Swap 1st and 8th bytes
  val = *(ptr);
  *(ptr) = *(ptr+7);
  *(ptr+7) = val;

  // Swap 2nd and 7th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+5);
  *(ptr+5) = val;

  // Swap 3rd and 6th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+3);
  *(ptr+3) = val;

  // Swap 4th and 5th bytes
  ptr += 1;
  val = *(ptr);
  *(ptr) = *(ptr+1);
  *(ptr+1) = val;
}

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__) || defined(__OS_WINDOWS_CYGWIN__))
  #include <unistd.h>
#elif defined(__OS_WINDOWS__)
  #include <windows.h>
#endif

void Stk :: sleep(unsigned long milliseconds)
{
#if defined(__OS_WINDOWS__)
  Sleep((DWORD) milliseconds);
#elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__) || defined(__OS_WINDOWS_CYGWIN__))
  usleep( (unsigned long) (milliseconds * 1000.0) );
#endif
}

void Stk :: handleError( const char *message, StkError::TYPE type )
{
    if (type == StkError::WARNING) {
        CK_FPRINTF_STDERR( "%s\n", message );
    } else if (type == StkError::DEBUG_WARNING) {
#if defined(_STK_DEBUG_)
        CK_FPRINTF_STDERR( "%s\n", message );
#endif
    } else {
        // print error message before throwing.
        CK_FPRINTF_STDERR( "%s\n", message );
        throw StkError(message, type);
    }
}

StkError :: StkError(const char *p, TYPE tipe)
  : type(tipe)
{
  strncpy(message, p, 256);
}

StkError :: ~StkError(void)
{
}

void StkError :: printMessage(void)
{
  printf("%s\n", message);
}
/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


SubNoise :: SubNoise(int subRate) : Noise()
{    
  rate = subRate;
  counter = rate;
}
SubNoise :: ~SubNoise()
{
}

int SubNoise :: subRate(void) const
{
  return rate;
}

void SubNoise :: setRate(int subRate)
{
  if (subRate > 0)
    rate = subRate;
}

MY_FLOAT SubNoise :: tick()
{
  if ( ++counter > rate ) {
    Noise::tick();
    counter = 1;
  }

  return lastOutput;
}
/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

Table :: Table(char *fileName)
{
  char message[256];

  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(message, "[chuck](via Table): Couldn't stat or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }
  length = (long) filestat.st_size / 8;  // length in 8-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(message, "[chuck](via Table): Couldn't open or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }

  data = (MY_FLOAT *) new MY_FLOAT[length];

  // Read samples into data[]
  long i = 0;
  double temp;
  while ( fread(&temp, 8, 1, fd) ) {
if( little_endian )
    swap64((unsigned char *)&temp);

    data[i++] = (MY_FLOAT) temp;
  }
  fclose(fd);

  lastOutput = 0.0;
}

Table :: ~Table()
{
  delete [ ] data;
}

long Table :: getLength() const
{
  return length;
}

MY_FLOAT Table :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Table :: tick(MY_FLOAT index)
{
  MY_FLOAT alpha;
  long temp;

  if (index > length-1) {
    CK_STDCERR << "[chuck](via STK): Table: Index (" << index << ") exceeds table length ... sticking at end!" << CK_STDENDL;
    index = length-1;
  }
  else if (index < 0.0) {
    CK_STDCERR << "[chuck](via STK): Table: Index (" << index << ") is less than zero ... setting to zero!" << CK_STDENDL;
    index = 0.0;
  }

  // Index in range 0 to length-1
  temp = (long) index;                   // Integer part of index
  alpha = index - (MY_FLOAT) temp;      // Fractional part of index
  if (alpha > 0.0) {                    // Do linear interpolation
    lastOutput = data[temp];
    lastOutput += (alpha*(data[temp+1] - lastOutput));
  }
  else lastOutput = data[temp];

  return lastOutput;
}

MY_FLOAT *Table :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class TubeBell
    \brief STK tubular bell (orchestral chime) FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


TubeBell :: TubeBell()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0   * 0.995);
  this->setRatio(1, 1.414 * 0.995);
  this->setRatio(2, 1.0   * 1.005);
  this->setRatio(3, 1.414 * 1.000);

  gains[0] = __FM_gains[94];
  gains[1] = __FM_gains[76];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[71];

  adsr[0]->setAllTimes( 0.005, 4.0, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.005, 4.0, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 2.0, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.004, 4.0, 0.0, 0.04);

  twozero->setGain( 0.5 );
  vibrato->setFrequency( 2.0 );
}  

TubeBell :: ~TubeBell()
{
}

void TubeBell :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[94];
  gains[1] = amplitude * __FM_gains[76];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[71];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "TubeBell: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT TubeBell :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}


/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

TwoPole :: TwoPole() : FilterStk()
{
  MY_FLOAT B = 1.0;
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  m_resFreq = 440.0;
  m_resRad = 0.0;
  m_resNorm = false;
  FilterStk::setCoefficients( 1, &B, 3, A );
}

TwoPole :: ~TwoPole()
{
}

void TwoPole :: clear(void)
{
  FilterStk::clear();
}

void TwoPole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoPole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void TwoPole :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void TwoPole :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Normalize the filter gain ... not terribly efficient.
    MY_FLOAT real = 1 - radius + (a[2] - radius) * cos(TWO_PI * 2 * frequency / Stk::sampleRate());
    MY_FLOAT imag = (a[2] - radius) * sin(TWO_PI * 2 * frequency / Stk::sampleRate());
    b[0] = sqrt( pow(real, 2) + pow(imag, 2) );
  }
}

void TwoPole :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT TwoPole :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT TwoPole :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT TwoPole :: tick(MY_FLOAT sample)
{
    inputs[0] = gain * sample;
    outputs[0] = b[0] * inputs[0] - a[2] * outputs[2] - a[1] * outputs[1];
    outputs[2] = outputs[1];
    outputs[1] = outputs[0];

    // gewang: dedenormal
    CK_STK_DDN(outputs[1]);
    CK_STK_DDN(outputs[2]);

    return outputs[0];
}

MY_FLOAT *TwoPole :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

TwoZero :: TwoZero() : FilterStk()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A = 1.0;
  m_notchFreq = 440.0;
  m_notchRad = 0.0;
  FilterStk::setCoefficients( 3, B, 1, &A );
}

TwoZero :: ~TwoZero()
{
}

void TwoZero :: clear(void)
{
  FilterStk::clear();
}

void TwoZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void TwoZero :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void TwoZero :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());

  // Normalize the filter gain.
  if (b[1] > 0.0) // Maximum at z = 0.
    b[0] = 1.0 / (1.0+b[1]+b[2]);
  else            // Maximum at z = -1.
    b[0] = 1.0 / (1.0-b[1]+b[2]);
  b[1] *= b[0];
  b[2] *= b[0];
}

void TwoZero :: setGain(MY_FLOAT theGain)
{
  FilterStk::setGain(theGain);
}

MY_FLOAT TwoZero :: getGain(void) const
{
  return FilterStk::getGain();
}

MY_FLOAT TwoZero :: lastOut(void) const
{
  return FilterStk::lastOut();
}

MY_FLOAT TwoZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[2] * inputs[2] + b[1] * inputs[1] + b[0] * inputs[0];
  inputs[2] = inputs[1];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *TwoZero :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick(vec[i]);

  return vec;
}
/***************************************************/
/*! \class Vector3D
    \brief STK 3D vector class.

    This class implements a three-dimensional vector.

    by Perry R. Cook, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

Vector3D :: Vector3D(double initX, double initY, double initZ)
{
  myX = initX;
  myY = initY;
  myZ = initZ;
}

Vector3D :: ~Vector3D()
{
}

double Vector3D :: getX()
{
  return myX;
}

double Vector3D :: getY()
{
  return myY;
}

double Vector3D :: getZ()
{
  return myZ;
}

double Vector3D :: getLength()
{
  double temp;
  temp = myX * myX;
  temp += myY * myY;
  temp += myZ * myZ;
  temp = sqrt(temp);
  return temp;
}

void Vector3D :: setXYZ(double anX, double aY, double aZ)
{
  myX = anX;
  myY = aY;
  myZ = aZ;
};

void Vector3D :: setX(double aval)
{
  myX = aval;
}

void Vector3D :: setY(double aval)
{
  myY = aval;
}

void Vector3D :: setZ(double aval)
{
  myZ = aval;
}


/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


#include <math.h>

VoicForm :: VoicForm() : Instrmnt()
{
    // Concatenate the STK rawwave path to the rawwave file
    voiced = new SingWave( "special:impuls20", TRUE );
    voiced->setGainRate( 0.001 );
    voiced->setGainTarget( 0.0 );
    noise = new Noise;

    for ( int i=0; i<4; i++ ) {
        filters[i] = new FormSwep;
        filters[i]->setSweepRate( 0.001 );
    }
    
    onezero = new OneZero;
    onezero->setZero( -0.9 );
    onepole = new OnePole;
    onepole->setPole( 0.9 );
    
    noiseEnv = new Envelope;
    noiseEnv->setRate( 0.001 );
    noiseEnv->setTarget( 0.0 );
    
    m_phonemeNum = 0;
    this->setPhoneme( "eee" );
    this->clear();

    // chuck
    setFrequency( 440 );
}  

VoicForm :: ~VoicForm()
{
    delete voiced;
    delete noise;
    delete onezero;
    delete onepole;
    delete noiseEnv;
  for ( int i=0; i<4; i++ ) {
    delete filters[i];
  }
}

void VoicForm :: clear()
{
    onezero->clear();
    onepole->clear();
  for ( int i=0; i<4; i++ ) {
    filters[i]->clear();
  }
}

void VoicForm :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): VoicForm: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  voiced->setFrequency( freakency );

  // chuck
  m_frequency = freakency;
}

bool VoicForm :: setPhoneme( const char *phoneme )
{
    bool found = false;
    unsigned int i = 0;
    while( i < 32 && !found )
    {
        if( !strcmp( Phonemes::name(i), phoneme ) )
        {
            found = true;
            filters[0]->setTargets( Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
            filters[1]->setTargets( Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
            filters[2]->setTargets( Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
            filters[3]->setTargets( Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
            setVoiced( Phonemes::voiceGain( i ) );
            setUnVoiced( Phonemes::noiseGain( i ) );
            m_phonemeNum = i;
#if defined(_STK_DEBUG_)
            CK_STDCERR << "[chuck](via STK): VoicForm: found formant " << phoneme << " (number " << i << ")" << CK_STDENDL;
#endif
        }
        i++;
    }

    if( !found )
        CK_STDCERR << "[chuck](via STK): VoicForm: phoneme " << phoneme << " not found!" << CK_STDENDL;
    else
        str_phoneme.set( Phonemes::name( m_phonemeNum ) );

    return found;
}

void VoicForm :: setVoiced(MY_FLOAT vGain)
{
    voiced->setGainTarget(vGain);
}

void VoicForm :: setUnVoiced(MY_FLOAT nGain)
{
    noiseEnv->setTarget(nGain * 0.01);
}

void VoicForm :: setFilterSweepRate(int whichOne, MY_FLOAT rate)
{
  if ( whichOne < 0 || whichOne > 3 ) {
    CK_STDCERR << "[chuck](via STK): VoicForm: setFilterSweepRate filter argument outside range 0-3!" << CK_STDENDL;
    return;
  }

    filters[whichOne]->setSweepRate(rate);
}

void VoicForm :: setPitchSweepRate(MY_FLOAT rate)
{
    voiced->setSweepRate(rate);
}

void VoicForm :: speak()
{
    voiced->noteOn();
}

void VoicForm :: quiet()
{
    voiced->noteOff();
    noiseEnv->setTarget( 0.0 );
}

void VoicForm :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
    // chuck
    if( frequency != m_frequency )
        setFrequency(frequency);

    voiced->setGainTarget(amplitude);
    onepole->setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOn( MY_FLOAT amplitude )
{
    voiced->setGainTarget(amplitude);
    onepole->setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOff(MY_FLOAT amplitude)
{
    this->quiet();
}

MY_FLOAT VoicForm :: tick()
{
    MY_FLOAT temp;
    temp = onepole->tick( onezero->tick( voiced->tick() ) );
    temp += noiseEnv->tick() * noise->tick();
    lastOutput = filters[0]->tick(temp);
    lastOutput += filters[1]->tick(temp);
    lastOutput += filters[2]->tick(temp);
    lastOutput += filters[3]->tick(temp);
  /*
    temp  += noiseEnv->tick() * noise->tick();
    lastOutput  = filters[0]->tick(temp);
    lastOutput  = filters[1]->tick(lastOutput);
    lastOutput  = filters[2]->tick(lastOutput);
    lastOutput  = filters[3]->tick(lastOutput);
  */
    return lastOutput;
}
 
void VoicForm :: controlChange(int number, MY_FLOAT value)
{
    MY_FLOAT norm = value * ONE_OVER_128;
    if ( norm < 0 ) {
        norm = 0.0;
        CK_STDCERR << "[chuck](via STK): VoicForm: Control value less than zero!" << CK_STDENDL;
    }
    else if ( norm > 1.0 ) {
        norm = 1.0;
        CK_STDCERR << "[chuck](via STK): VoicForm: Control value exceeds nominal range!" << CK_STDENDL;
    }

    if (number == __SK_Breath_) { // 2
        this->setVoiced( 1.0 - norm );
        this->setUnVoiced( norm );
    }
    else if (number == __SK_FootControl_) { // 4
      MY_FLOAT temp = 0.0;
      unsigned int i = (int) value;
      if (i < 32) {
        temp = 0.9;
      }
      else if (i < 64) {
        i -= 32;
        temp = 1.0;
      }
      else if (i < 96) {
        i -= 64;
        temp = 1.1;
      }
      else if (i < 128) {
        i -= 96;
        temp = 1.2;
      }
      else if (i == 128) {
        i = 0;
        temp = 1.4;
      }
      filters[0]->setTargets( temp * Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
      filters[1]->setTargets( temp * Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
      filters[2]->setTargets( temp * Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
      filters[3]->setTargets( temp * Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
      setVoiced( Phonemes::voiceGain( i ) );
      setUnVoiced( Phonemes::noiseGain( i ) );
    }
    else if (number == __SK_ModFrequency_) // 11
        voiced->setVibratoRate( norm * 12.0);  // 0 to 12 Hz
    else if (number == __SK_ModWheel_) // 1
        voiced->setVibratoGain( norm * 0.2);
    else if (number == __SK_AfterTouch_Cont_)   { // 128
        setVoiced( norm );
        onepole->setPole( 0.97 - ( norm * 0.2) );
  }
  else
    CK_STDCERR << "[chuck](via STK): VoicForm: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): VoicForm: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of
    STK instrument classes.  Individual voices can
    be controlled via unique note tags.
    Instrument groups can be controlled by channel
    number.

    A previously constructed STK instrument class
    is linked with a voice manager using the
    addInstrument() function.  An optional channel
    number argument can be specified to the
    addInstrument() function as well (default
    channel = 0).  The voice manager does not
    delete any instrument instances ... it is the
    responsibility of the user to allocate and
    deallocate all instruments.

    The tick() function returns the mix of all
    sounding voices.  Each noteOn returns a unique
    tag (credits to the NeXT MusicKit), so you can
    send control changes to specific voices within
    an ensemble.  Alternately, control changes can
    be sent to all voices on a given channel.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <math.h>

Voicer :: Voicer( int maxInstruments, MY_FLOAT decayTime )
{
  nVoices = 0;
  maxVoices = maxInstruments;
  voices = (Voice *) new Voice[maxVoices];
  tags = 23456;
  muteTime = (int) ( decayTime * Stk::sampleRate() );
}

Voicer :: ~Voicer()
{
  delete [] voices;
}

void Voicer :: addInstrument( Instrmnt *instrument, int channel )
{
  //voices = (Voice *) realloc( (void *) voices, nVoices+1 * sizeof( Voice ) );
  if ( nVoices == maxVoices ) {
    CK_STDCERR << "[chuck](via STK): Voicer: Maximum number of voices already added!!" << CK_STDENDL;
    return;
  }

  voices[nVoices].instrument = instrument;
  voices[nVoices].tag = 0;
  voices[nVoices].channel = channel;
  voices[nVoices].noteNumber = -1;
  voices[nVoices].frequency = 0.0;
  voices[nVoices].sounding = 0;
  nVoices++;
}

void Voicer :: removeInstrument( Instrmnt *instrument )
{
  bool found = false;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].instrument == instrument ) found = true;
    if ( found && i+1 < nVoices ) {
      voices[i].instrument = voices[i+1].instrument;
      voices[i].tag = voices[i+1].tag;
      voices[i].noteNumber = voices[i+1].noteNumber;
      voices[i].frequency = voices[i+1].frequency;
      voices[i].sounding = voices[i+1].sounding;
      voices[i].channel = voices[i+1].channel;
    }
  }

  if ( found )
    nVoices--;
    //voices = (Voice *) realloc( voices, --nVoices * sizeof( Voice ) );

}

long Voicer :: noteOn(MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  int i;
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( i=0; i<nVoices; i++ ) {
    if (voices[i].noteNumber < 0 && voices[i].channel == channel) {
        voices[i].tag = tags++;
      voices[i].channel = channel;
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
        voices[i].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
      voices[i].sounding = 1;
      return voices[i].tag;
    }
  }

  // All voices are sounding, so interrupt the oldest voice.
  int voice = -1;
  for ( i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      if ( voice == -1 ) voice = i;
      else if ( voices[i].tag < voices[voice].tag ) voice = i;
    }
  }

  if ( voice >= 0 ) {
    voices[voice].tag = tags++;
    voices[voice].channel = channel;
    voices[voice].noteNumber = noteNumber;
    voices[voice].frequency = frequency;
    voices[voice].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
    voices[voice].sounding = 1;
    return voices[voice].tag;
  }

  return -1;
}

void Voicer :: noteOff( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].noteNumber == noteNumber && voices[i].channel == channel ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
    }
  }
}

void Voicer :: noteOff( long tag, MY_FLOAT amplitude )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
      break;
    }
  }
}

void Voicer :: setFrequency( MY_FLOAT noteNumber, int channel )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
    }
  }
}

void Voicer :: setFrequency( long tag, MY_FLOAT noteNumber )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
      break;
    }
  }
}

void Voicer :: pitchBend( MY_FLOAT value, int channel )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
  }
}

void Voicer :: pitchBend( long tag, MY_FLOAT value )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
      break;
    }
  }
}

void Voicer :: controlChange( int number, MY_FLOAT value, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->controlChange( number, value );
  }
}

void Voicer :: controlChange( long tag, int number, MY_FLOAT value )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->controlChange( number, value );
      break;
    }
  }
}

void Voicer :: silence( void )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding > 0 )
      voices[i].instrument->noteOff( 0.5 );
  }
}

MY_FLOAT Voicer :: tick()
{
  lastOutput = lastOutputLeft = lastOutputRight = 0.0;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding != 0 ) {
      lastOutput += voices[i].instrument->tick();
      lastOutputLeft += voices[i].instrument->lastOutLeft();
      lastOutputRight += voices[i].instrument->lastOutRight();
    }
    if ( voices[i].sounding < 0 ) {
      voices[i].sounding++;
      if ( voices[i].sounding == 0 )
        voices[i].noteNumber = -1;
    }
  }
  return lastOutput / nVoices;
}

MY_FLOAT *Voicer :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vec[i] = tick();

  return vec;
}

MY_FLOAT Voicer :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Voicer :: lastOutLeft() const
{
  return lastOutputLeft;
}

MY_FLOAT Voicer :: lastOutRight() const
{
  return lastOutputRight;
}

/***************************************************/
/*! \class WaveLoop
    \brief STK waveform oscillator class.

    This class inherits from WvIn and provides
    audio file looping functionality.

    WaveLoop supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <math.h>

WaveLoop :: WaveLoop( const char *fileName, bool raw, bool generate )
  : WvIn( fileName, raw ), phaseOffset(0.0)
{
  m_freq = 0;
  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }

}

WaveLoop :: WaveLoop( )
  : WvIn( ), phaseOffset(0.0)
{ m_freq = 0; } 

void
WaveLoop :: openFile( const char * fileName, bool raw, bool norm )
{
    m_loaded = FALSE;
    WvIn::openFile( fileName, raw, norm );
    // If at end of file, redo extra sample frame for looping.
    if (chunkPointer+bufferSize == fileSize) {
      for (unsigned int j=0; j<channels; j++)
        data[bufferSize*channels+j] = data[j];
    }
    m_loaded = TRUE;
}

WaveLoop :: ~WaveLoop()
{
    m_loaded = FALSE;
}

void WaveLoop :: readData( unsigned long index )
{
  WvIn::readData( index );

  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }
}

void WaveLoop :: setFrequency(MY_FLOAT aFrequency)
{
  // This is a looping frequency.
  m_freq = aFrequency; // chuck data

  rate = fileSize * aFrequency / sampleRate();
}

void WaveLoop :: addTime(MY_FLOAT aTime)
{
  // Add an absolute time in samples 
  time += aTime;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhase(MY_FLOAT anAngle)
{
  // Add a time in cycles (one cycle = fileSize).
  time += fileSize * anAngle;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhaseOffset(MY_FLOAT anAngle)
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset = fileSize * anAngle;
}

const MY_FLOAT *WaveLoop :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  // Check limits of time address ... if necessary, recalculate modulo fileSize.
  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;

  if (phaseOffset) {
    tyme = time + phaseOffset;
    while (tyme < 0.0)
      tyme += fileSize;
    while (tyme >= fileSize)
      tyme -= fileSize;
  }
  else {
    tyme = time;
  }

  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Always do linear interpolation here ... integer part of time address.
  index = (unsigned long) tyme;

  // Fractional part of time address.
  alpha = tyme - (MY_FLOAT) index;
  index *= channels;
  for (i=0; i<channels; i++) {
    lastOutput[i] = data[index];
    lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
    index++;
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;

  return lastOutput;
}

/***************************************************/
/*! \class Whistle
    \brief STK police/referee whistle instrument class.

    This class implements a hybrid physical/spectral
    model of a police whistle (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Fipple Modulation Frequency = 11
       - Fipple Modulation Gain = 1
       - Blowing Frequency Modulation = 2
       - Volume = 128

    by Perry R. Cook  1996 - 2002.
*/
/***************************************************/

#include <stdlib.h>
#include <math.h>

#define CAN_RADIUS 100
#define PEA_RADIUS 30
#define BUMP_RADIUS 5

#define NORM_CAN_LOSS 0.97
#define SLOW_CAN_LOSS 0.90
#define GRAVITY 20.0
//  GRAVITY WAS 6.0

#define NORM_TICK_SIZE 0.004
#define SLOW_TICK_SIZE 0.0001

#define ENV_RATE 0.001 

Whistle :: Whistle()
{
    tempVector = new Vector3D(0,0,0);
  can = new Sphere(CAN_RADIUS);
  pea = new Sphere(PEA_RADIUS);
  bumper = new Sphere(BUMP_RADIUS);

  // Concatenate the STK rawwave path to the rawwave file
  sine = new WaveLoop( "special:sinewave", TRUE );
  sine->setFrequency(2800.0);

  can->setPosition(0, 0, 0); // set can location
  can->setVelocity(0, 0, 0); // and the velocity

  onepole.setPole(0.95);  // 0.99

  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  bumper->setPosition(0.0, CAN_RADIUS-BUMP_RADIUS, 0);
  pea->setPosition(0, CAN_RADIUS/2, 0);
  pea->setVelocity(35, 15, 0);

  envelope.setRate(ENV_RATE);
  envelope.keyOn();

    fippleFreqMod = 0.5;
    fippleGainMod = 0.5;
    blowFreqMod = 0.25;
    noiseGain = 0.125;
    maxPressure = (MY_FLOAT) 0.0;
    baseFrequency = 2000;

    tickSize = NORM_TICK_SIZE;
    canLoss = NORM_CAN_LOSS;

    subSample = 1;
    subSampCount = subSample;
}

Whistle :: ~Whistle()
{
  delete tempVector;
  delete can;
  delete pea;
  delete bumper;
  delete sine;
}

void Whistle :: clear()
{
}

void Whistle :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency * 4;  // the whistle is a transposing instrument
  if ( frequency <= 0.0 ) {
    CK_STDCERR << "[chuck](via STK): Whistle: setFrequency parameter is less than or equal to zero!" << CK_STDENDL;
    freakency = 220.0;
  }

  baseFrequency = freakency;
}

void Whistle :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
    envelope.setRate(ENV_RATE);
    envelope.setTarget(amplitude);
}

void Whistle :: stopBlowing(MY_FLOAT rate)
{
  envelope.setRate(rate);
  envelope.keyOff();
}

void Whistle :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing(amplitude*2.0 ,amplitude * 0.2);
#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Whistle: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

void Whistle :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Whistle: NoteOff amplitude = " << amplitude << CK_STDENDL;
#endif
}

int frameCount = 0;

MY_FLOAT Whistle :: tick()
{
  MY_FLOAT soundMix, tempFreq;
  double envOut = 0, temp, temp1, temp2, tempX, tempY;
  double phi, cosphi, sinphi;
  double gain = 0.5, mod = 0.0;

    if (--subSampCount <= 0)    {
        tempVectorP = pea->getPosition();
        subSampCount = subSample;
        temp = bumper->isInside(tempVectorP);
#ifdef WHISTLE_ANIMATION
    frameCount += 1;
    if (frameCount >= (1470 / subSample))   {
      frameCount = 0;
      // printf("%f %f %f\n",tempVectorP->getX(),tempVectorP->getY(),envOut);
      CK_FFLUSH_STDOUT();
    }
#endif
    envOut = envelope.tick();

    if (temp < (BUMP_RADIUS + PEA_RADIUS)) {
      tempX = envOut * tickSize * 2000 * noise.tick();
      tempY = -envOut * tickSize * 1000 * (1.0 + noise.tick());
      pea->addVelocity(tempX,tempY,0); 
      pea->tick(tickSize);
    }
        
    mod  = exp(-temp * 0.01);   // exp. distance falloff of fipple/pea effect
    temp = onepole.tick(mod);   // smooth it a little
    gain = (1.0 - (fippleGainMod*0.5)) + (2.0 * fippleGainMod * temp);
    gain *= gain;               // squared distance/gain
    //    tempFreq = 1.0                //  Normalized Base Freq
    //          + (fippleFreqMod * 0.25) - (fippleFreqMod * temp) // fippleModulation 
    //          - (blowFreqMod) + (blowFreqMod * envOut); // blowingModulation
    // short form of above
    tempFreq = 1.0 + fippleFreqMod*(0.25-temp) + blowFreqMod*(envOut-1.0);
    tempFreq *= baseFrequency;

    sine->setFrequency(tempFreq);
    
    tempVectorP = pea->getPosition();
    temp = can->isInside(tempVectorP);
    temp  = -temp;       // We know (hope) it's inside, just how much??
    if (temp < (PEA_RADIUS * 1.25))        {            
      pea->getVelocity(tempVector); //  This is the can/pea collision
      tempX = tempVectorP->getX();  // calculation.  Could probably
      tempY = tempVectorP->getY();  // simplify using tables, etc.
      phi = -atan2(tempY,tempX);
      cosphi = cos(phi);
      sinphi = sin(phi);
      temp1 = (cosphi*tempVector->getX()) - (sinphi*tempVector->getY());
      temp2 = (sinphi*tempVector->getX()) + (cosphi*tempVector->getY());
      temp1 = -temp1;
      tempX = (cosphi*temp1) + (sinphi*temp2);
      tempY = (-sinphi*temp1) + (cosphi*temp2);
      pea->setVelocity(tempX, tempY, 0);
      pea->tick(tickSize);
      pea->setVelocity(tempX*canLoss, tempY*canLoss, 0);
      pea->tick(tickSize);
    }
        
    temp = tempVectorP->getLength();    
    if (temp > 0.01)        {
      tempX = tempVectorP->getX();
      tempY = tempVectorP->getY();
      phi = atan2(tempY,tempX);
      phi += 0.3 * temp / CAN_RADIUS;
      cosphi = cos(phi);
      sinphi = sin(phi);
      tempX = 3.0 * temp * cosphi;
      tempY = 3.0 * temp * sinphi;
    }
    else {
      tempX = 0.0;
      tempY = 0.0;
    }
    
    temp = (0.9 + 0.1*subSample*noise.tick()) * envOut * 0.6 * tickSize;
    pea->addVelocity(temp * tempX,
                     (temp*tempY) - (GRAVITY*tickSize),0);
    pea->tick(tickSize);

    //    bumper->tick(0.0);
    }

    temp = envOut * envOut * gain / 2;
    soundMix = temp * (sine->tick() + (noiseGain*noise.tick()));
    lastOutput = 0.25 * soundMix; // should probably do one-zero filter here

    return lastOutput;
}

void Whistle :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    CK_STDCERR << "[chuck](via STK): Whistle: Control value less than zero!" << CK_STDENDL;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    CK_STDCERR << "[chuck](via STK): Whistle: Control value exceeds nominal range!" << CK_STDENDL;
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain = 0.25 * norm;
  else if (number == __SK_ModFrequency_) // 11
    fippleFreqMod = norm;
  else if (number == __SK_ModWheel_) // 1
    fippleGainMod = norm;
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope.setTarget( norm * 2.0 );
  else if (number == __SK_Breath_) // 2
    blowFreqMod = norm * 0.5;
  else if (number == __SK_Sustain_)  // 64
  { if (value < 1.0) subSample = 1; }
  else
    CK_STDCERR << "[chuck](via STK): Whistle: Undefined Control Number (" << number << ")!!" << CK_STDENDL;

#if defined(_STK_DEBUG_)
  CK_STDCERR << "[chuck](via STK): Whistle: controlChange number = " << number << ", value = " << value << CK_STDENDL;
#endif
}

/***************************************************/
/*! \class Wurley
    \brief STK Wurlitzer electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


Wurley :: Wurley()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
  waves[i] = new WaveLoop( "special:sinewave", TRUE );
  waves[3] = new WaveLoop( "special:fwavblnk", TRUE );

  this->setRatio(0, 1.0);
  this->setRatio(1, 4.0);
  this->setRatio(2, -510.0);
  this->setRatio(3, -510.0);

  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[82];
  gains[2] = __FM_gains[92];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 0.25, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.001, 0.15, 0.0, 0.04);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 8.0 );
}  

Wurley :: ~Wurley()
{
}

void Wurley :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;
  waves[0]->setFrequency(baseFrequency * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ratios[1]);
  waves[2]->setFrequency(ratios[2]);    // Note here a 'fixed resonance'.
  waves[3]->setFrequency(ratios[3]);

  // chuck
  m_frequency = baseFrequency;
}

void Wurley :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99];
  gains[1] = amplitude * __FM_gains[82];
  gains[2] = amplitude * __FM_gains[82];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  CK_STDCERR << "Wurley: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << CK_STDENDL;
#endif
}

MY_FLOAT Wurley :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}

// CHUCK HACK:
void Wurley :: controlChange( int which, MY_FLOAT value )
{
  if( which == 3 )
  {
    adsr[0]->setAllTimes( 0.001, 1.50 * value, 0.0, 0.04);
    adsr[1]->setAllTimes( 0.001, 1.50 * value, 0.0, 0.04);
    adsr[2]->setAllTimes( 0.001, 0.25 * value, 0.0, 0.04);
    adsr[3]->setAllTimes( 0.001, 0.15 * value, 0.0, 0.04);
  }
  else
  {
    // call parent
    FM::controlChange( which, value );
  }
}


/***************************************************/
/*! \class WvIn
    \brief STK audio data input base class.

    This class provides input support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvIn loads the contents of an audio file for
    subsequent output.  Linear interpolation is
    used for fractional "read rates".

    WvIn supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which return samples produced
    by averaging across sample frames, from the 
    tickFrame() methods, which return pointers to
    multi-channel sample frames.  For single-channel
    data, these methods return equivalent values.

    Small files are completely read into local memory
    during instantiation.  Large files are read
    incrementally from disk.  The file size threshold
    and the increment size values are defined in
    WvIn.h.

    WvIn currently supports WAV, AIFF, SND (AU),
    MAT-file (Matlab), and STK RAW file formats.
    Signed integer (8-, 16-, and 32-bit) and floating-
    point (32- and 64-bit) data types are supported.
    Uncompressed data types are not supported.  If
    using MAT-files, data should be saved in an array
    with each data channel filling a matrix row.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#include <iostream>

#include "util_raw.h"

WvIn :: WvIn()
{
    init();
}

WvIn :: WvIn( const char *fileName, bool raw, bool doNormalize, bool generate )
{
    init();
    openFile( fileName, raw, generate );
}

WvIn :: ~WvIn()
{
    if (fd)
        fclose(fd);

    if (data)
        delete [] data;

    if (lastOutput)
        delete [] lastOutput;

    m_loaded = false;
}

void WvIn :: init( void )
{
    fd = 0;
    m_loaded = false;
    // strcpy ( m_filename, "" );
    data = 0;
    lastOutput = 0;
    chunking = false;
    finished = true;
    interpolate = false;
    bufferSize = 0;
    channels = 0;
    time = 0.0;
}

void WvIn :: closeFile( void )
{
    if ( fd ) fclose( fd );
    finished = true;
    str_filename.set( "" );
}

void WvIn :: openFile( const char *fileName, bool raw, bool doNormalize, bool generate )
{
    unsigned long lastChannels = channels;
    unsigned long samples, lastSamples = (bufferSize+1)*channels;
    str_filename.set( fileName );
    //strncpy ( m_filename, fileName, 255 );
    //m_filename[255] = '\0';
    if(!generate || !strstr(fileName, "special:"))
    {
        closeFile();

        // Try to open the file.
        fd = fopen(fileName, "rb");
        if (!fd) {
            sprintf(msg, "[chuck](via WvIn): Could not open or find file (%s).", fileName);
            handleError(msg, StkError::FILE_NOT_FOUND);
        }

        bool result = false;
        if ( raw )
            result = getRawInfo( fileName );
        else {
            char header[12];
            if ( fread(&header, 4, 3, fd) != 3 ) goto error;
            if ( !strncmp(header, "RIFF", 4) &&
                !strncmp(&header[8], "WAVE", 4) )
                result = getWavInfo( fileName );
            else if ( !strncmp(header, ".snd", 4) )
                result = getSndInfo( fileName );
            else if ( !strncmp(header, "FORM", 4) &&
                    (!strncmp(&header[8], "AIFF", 4) || !strncmp(&header[8], "AIFC", 4) ) )
                result = getAifInfo( fileName );
            else {
                if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
                if ( fread(&header, 2, 1, fd) != 1 ) goto error;
                if (!strncmp(header, "MI", 2) ||
                    !strncmp(header, "IM", 2) )
                    result = getMatInfo( fileName );
                else {
                    raw = TRUE;
                    result = getRawInfo( fileName );
                    // sprintf(msg, "WvIn: File (%s) format unknown.", fileName);
                    // handleError(msg, StkError::FILE_UNKNOWN_FORMAT);
                }
            }
        }

        if ( result == false )
            handleError(msg, StkError::FILE_ERROR);

        if ( fileSize == 0 ) {
            sprintf(msg, "[chuck](via WvIn): File (%s) data size is zero!", fileName);
            handleError(msg, StkError::FILE_ERROR);
        }
    }
    else
    {
        bufferSize = 1024;
        channels = 1;
    }

    // Allocate new memory if necessary.
    samples = (bufferSize+1)*channels;
    if ( lastSamples < samples ) {
        if ( data ) delete [] data;
        data = (MY_FLOAT *) new MY_FLOAT[samples];
    }
    if ( lastChannels < channels ) {
        if ( lastOutput ) delete [] lastOutput;
        lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
    }

    if ( fmod(rate, 1.0) != 0.0 ) interpolate = true;
    chunkPointer = 0;

    reset();
    if(generate && strstr(fileName, "special:"))
    {
        // STK rawwave files have no header and are assumed to contain a
        // monophonic stream of 16-bit signed integers in big-endian byte
        // order with a sample rate of 22050 Hz.
        fileSize = bufferSize;
        dataOffset = 0;
        interpolate = true;
        chunking = false;
        dataType = STK_SINT16;
        byteswap = false;
        fileRate = 22050.0;
        rate = (MY_FLOAT)fileRate / Stk::sampleRate();
        

        // which
        if( strstr(fileName, "special:sinewave") )
        {
            for (unsigned int j=0; j<bufferSize; j++)
                data[j] = (SHRT_MAX) * sin(2*ONE_PI*j/bufferSize);
        }
        else
        {
            SAMPLE * rawdata = NULL;
            int rawsize = 0;

            if( strstr(fileName, "special:aah") ) {
                rawsize = ahh_size; rawdata = ahh_data;
            }
            else if( strstr(fileName, "special:britestk") ) {
                rawsize = britestk_size; rawdata = britestk_data;
            }
            else if( strstr(fileName, "special:dope") ) {
                rawsize = dope_size; rawdata = dope_data;
            }
            else if( strstr(fileName, "special:eee") ) {
                rawsize = eee_size; rawdata = eee_data;
            }
            else if( strstr(fileName, "special:fwavblnk") ) {
                rawsize = fwavblnk_size; rawdata = fwavblnk_data;
            }
            else if( strstr(fileName, "special:halfwave") ) {
                rawsize = halfwave_size; rawdata = halfwave_data;
            }
            else if( strstr(fileName, "special:impuls10") ) {
                rawsize = impuls10_size; rawdata = impuls10_data;
            }
            else if( strstr(fileName, "special:impuls20") ) {
                rawsize = impuls20_size; rawdata = impuls20_data;
            }
            else if( strstr(fileName, "special:impuls40") ) {
                rawsize = impuls40_size; rawdata = impuls40_data;
            }
            else if( strstr(fileName, "special:mand1") ) {
                rawsize = mand1_size; rawdata = mand1_data;
            }
            else if( strstr(fileName, "special:mandpluk") ) {
                rawsize = mandpluk_size; rawdata = mandpluk_data;
            }
            else if( strstr(fileName, "special:marmstk1") ) {
                rawsize = marmstk1_size; rawdata = marmstk1_data;
            }
            else if( strstr(fileName, "special:ooo") ) {
                rawsize = ooo_size; rawdata = ooo_data;
            }
            else if( strstr(fileName, "special:peksblnk") ) {
                rawsize = peksblnk_size; rawdata = peksblnk_data;
            }
            else if( strstr(fileName, "special:ppksblnk") ) {
                rawsize = ppksblnk_size; rawdata = ppksblnk_data;
            }
            else if( strstr(fileName, "special:silence") ) {
                rawsize = silence_size; rawdata = silence_data;
            }
            else if( strstr(fileName, "special:sineblnk") ) {
                rawsize = sineblnk_size; rawdata = sineblnk_data;
            }
            else if( strstr(fileName, "special:sinewave") ) {
                rawsize = sinewave_size; rawdata = sinewave_data;
            }
            else if( strstr(fileName, "special:snglpeak") ) {
                rawsize = snglpeak_size; rawdata = snglpeak_data;
            }
            else if( strstr(fileName, "special:twopeaks") ) {
                rawsize = twopeaks_size; rawdata = twopeaks_data;
            }
            else if( strstr(fileName, "special:glot_pop") ) {
                rawsize = glot_pop_size; rawdata = glot_pop_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_ahh") ) {
                rawsize = glot_ahh_size; rawdata = glot_ahh_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_eee" ) ) {
                rawsize = glot_eee_size; rawdata = glot_eee_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }
            else if( strstr(fileName, "special:glot_ooo" ) ) {
                rawsize = glot_ooo_size; rawdata = glot_ooo_data;
                fileRate = 44100.0; rate = (MY_FLOAT)44100.0 / Stk::sampleRate();
            }

            if ( rawdata ) {
                if ( data ) delete [] data;
                data = (MY_FLOAT *) new MY_FLOAT[rawsize+1];
                bufferSize = rawsize;
                fileSize = bufferSize;
                for ( int j=0; j < rawsize; j++ ) {
                    data[j] = (MY_FLOAT) rawdata[j];
                }
            }
            else
                goto error;
        }
        data[bufferSize] = data[0];
    }
    else readData( 0 );  // Load file data.

    if ( doNormalize ) normalize();
    m_loaded = true;
    finished = false;
    interpolate = ( fmod( rate, 1.0 ) != 0.0 );
    return;

error:
    sprintf(msg, "[chuck](via WvIn): Error reading file (%s).", fileName);
    handleError(msg, StkError::FILE_ERROR);
}

bool WvIn :: getRawInfo( const char *fileName )
{
  // Use the system call "stat" to determine the file length.
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(msg, "[chuck](via WvIn): Could not stat RAW file (%s).", fileName);
    return false;
  }

  fileSize = (long) filestat.st_size / 2;  // length in 2-byte samples
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
    gain = 1.0 / 32768.0;
  }

  // STK rawwave files have no header and are assumed to contain a
  // monophonic stream of 16-bit signed integers in big-endian byte
  // order with a sample rate of 22050 Hz.
  channels = 1;
  dataOffset = 0;
  rate = (MY_FLOAT) 22050.0 / Stk::sampleRate();
  fileRate = 22050.0;
  interpolate = false;
  dataType = STK_SINT16;
  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;
}

bool WvIn :: getWavInfo( const char *fileName )
{
  // Find "format" chunk ... it must come before the "data" chunk.
  char id[4];
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "fmt ", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Check that the data is not compressed.
  SINT16 format_tag;
  if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error; // Read fmt chunk size.
  if ( fread(&format_tag, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
{
  swap16((unsigned char *)&format_tag);
  swap32((unsigned char *)&chunkSize);
}
  if (format_tag != 1 && format_tag != 3 ) { // PCM = 1, FLOAT = 3
    sprintf(msg, "[chuck](via WvIn): %s contains an unsupported data format type (%d).", fileName, format_tag);
    return false;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap16((unsigned char *)&temp);

  channels = (unsigned int ) temp;

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap32((unsigned char *)&srate);

  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / Stk::sampleRate() );

  // Determine the data type.
  dataType = 0;
  if ( fseek(fd, 6, SEEK_CUR) == -1 ) goto error;   // Locate bits_per_sample info.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap16((unsigned char *)&temp);

  if ( format_tag == 1 ) {
    if (temp == 8)
      dataType = STK_SINT8;
    else if (temp == 16)
      dataType = STK_SINT16;
    else if (temp == 32)
      dataType = STK_SINT32;
  }
  else if ( format_tag == 3 ) {
    if (temp == 32)
      dataType = MY_FLOAT32;
    else if (temp == 64)
      dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "[chuck](via WvIn): %d bits per sample with data format %d are not supported (%s).", temp, format_tag, fileName);
    return false;
  }

  // Jump over any remaining part of the "fmt" chunk.
  if ( fseek(fd, chunkSize-16, SEEK_CUR) == -1 ) goto error;

  // Find "data" chunk ... it must come after the "fmt" chunk.
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;

  while ( strncmp(id, "data", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get length of data from the header.
  SINT32 bytes;
  if ( fread(&bytes, 4, 1, fd) != 1 ) goto error;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fileSize = 8 * bytes / temp / channels;  // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  dataOffset = ftell(fd);
  byteswap = false;
if( !little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading WAV file (%s).", fileName);
  return false;
}

bool WvIn :: getSndInfo( const char *fileName )
{
  // Determine the data type.
  SINT32 format;
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;   // Locate format
  if ( fread(&format, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&format);

  if (format == 2) dataType = STK_SINT8;
  else if (format == 3) dataType = STK_SINT16;
  else if (format == 5) dataType = STK_SINT32;
  else if (format == 6) dataType = MY_FLOAT32;
  else if (format == 7) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "[chuck](via WvIn): data format in file %s is not supported.", fileName);
    return false;
  }

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&srate);

  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / sampleRate() );

  // Get number of channels from the header.
  SINT32 chans;
  if ( fread(&chans, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&chans);

  channels = chans;

  if ( fseek(fd, 4, SEEK_SET) == -1 ) goto error;
  if ( fread(&dataOffset, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&dataOffset);

  // Get length of data from the header.
  if ( fread(&fileSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&fileSize);

  fileSize /= 2 * channels;  // Convert to sample frames.
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading SND file (%s).", fileName);
  return false;
}

bool WvIn :: getAifInfo( const char *fileName )
{
  bool aifc = false;
  char id[4];

  // Determine whether this is AIFF or AIFC.
  if ( fseek(fd, 8, SEEK_SET) == -1 ) goto error;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  if ( !strncmp(id, "AIFC", 4) ) aifc = true;

  // Find "common" chunk
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1) goto error;
  while ( strncmp(id, "COMM", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error; // Jump over chunk size
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( little_endian )
  swap16((unsigned char *)&temp);

  channels = temp;

  // Get length of data from the header.
  SINT32 frames;
  if ( fread(&frames, 4, 1, fd) != 1 ) goto error;
if( little_endian )
  swap32((unsigned char *)&frames);

  fileSize = frames; // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Read the number of bits per sample.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
if( little_endian )
  swap16((unsigned char *)&temp);

  // Get file sample rate from the header.  For AIFF files, this value
  // is stored in a 10-byte, IEEE Standard 754 floating point number,
  // so we need to convert it first.
  unsigned char srate[10];
  unsigned char exp;
  unsigned long mantissa;
  unsigned long last;
  if ( fread(&srate, 10, 1, fd) != 1 ) goto error;
  mantissa = (unsigned long) *(unsigned long *)(srate+2);
if( little_endian )
  swap32((unsigned char *)&mantissa);

  exp = 30 - *(srate+1);
  last = 0;
  while (exp--) {
    last = mantissa;
    mantissa >>= 1;
  }
  if (last & 0x00000001) mantissa++;
  fileRate = (MY_FLOAT) mantissa;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  // Determine the data format.
  dataType = 0;
  if ( aifc == false ) {
    if ( temp == 8 ) dataType = STK_SINT8;
    else if ( temp == 16 ) dataType = STK_SINT16;
    else if ( temp == 32 ) dataType = STK_SINT32;
  }
  else {
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
    if ( (!strncmp(id, "fl32", 4) || !strncmp(id, "FL32", 4)) && temp == 32 ) dataType = MY_FLOAT32;
    else if ( (!strncmp(id, "fl64", 4) || !strncmp(id, "FL64", 4)) && temp == 64 ) dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "[chuck](via WvIn): %d bits per sample in file %s are not supported.", temp, fileName);
    return false;
  }

  // Start at top to find data (SSND) chunk ... chunk order is undefined.
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;

  // Find data (SSND) chunk
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "SSND", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
if( little_endian )
    swap32((unsigned char *)&chunkSize);

    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Skip over chunk size, offset, and blocksize fields
  if ( fseek(fd, 12, SEEK_CUR) == -1 ) goto error;

  dataOffset = ftell(fd);
  byteswap = false;
if( little_endian )
  byteswap = true;

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading AIFF file (%s).", fileName);
  return false;
}

bool WvIn :: getMatInfo( const char *fileName )
{
  // Verify this is a version 5 MAT-file format.
  char head[4];
  if ( fseek(fd, 0, SEEK_SET) == -1 ) goto error;
  if ( fread(&head, 4, 1, fd) != 1 ) goto error;
  // If any of the first 4 characters of the header = 0, then this is
  // a Version 4 MAT-file.
  if ( strstr(head, "0") ) {
    sprintf(msg, "[chuck](via WvIn): %s appears to be a Version 4 MAT-file, which is not currently supported.",
            fileName);
    return false;
  }

  // Determine the endian-ness of the file.
  char mi[2];
  byteswap = false;
  // Locate "M" and "I" characters in header.
  if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
  if ( fread(&mi, 2, 1, fd) != 1) goto error;
if( little_endian )
{
  if ( !strncmp(mi, "MI", 2) )
    byteswap = true;
  else if ( strncmp(mi, "IM", 2) ) goto error;
}
else
{
  if ( !strncmp(mi, "IM", 2))
    byteswap = true;
  else if ( strncmp(mi, "MI", 2) ) goto error;
}

  // Check the data element type
  SINT32 datatype;
  if ( fread(&datatype, 4, 1, fd) != 1 ) goto error;
  if ( byteswap ) swap32((unsigned char *)&datatype);
  if (datatype != 14) {
    sprintf(msg, "[chuck](via WvIn): The file does not contain a single Matlab array (or matrix) data element.");
    return false;
  }

  // Determine the array data type.
  SINT32 tmp;
  SINT32 size;
  if ( fseek(fd, 168, SEEK_SET) == -1 ) goto error;
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if (tmp == 1) {  // array name > 4 characters
    if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;  // get array name length
    if (byteswap) swap32((unsigned char *)&tmp);
    size = (SINT32) ceil((float)tmp / 8);
    if ( fseek(fd, size*8, SEEK_CUR) == -1 ) goto error;  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error;
  }
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if ( tmp == 1 ) dataType = STK_SINT8;
  else if ( tmp == 3 ) dataType = STK_SINT16;
  else if ( tmp == 5 ) dataType = STK_SINT32;
  else if ( tmp == 7 ) dataType = MY_FLOAT32;
  else if ( tmp == 9 ) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "[chuck](via WvIn): The MAT-file array data format (%d) is not supported.", tmp);
    return false;
  }

  // Get number of rows from the header.
  SINT32 rows;
  if ( fseek(fd, 160, SEEK_SET) == -1 ) goto error;
  if ( fread(&rows, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&rows);

  // Get number of columns from the header.
  SINT32 columns;
  if ( fread(&columns,4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&columns);

  // Assume channels = smaller of rows or columns.
  if (rows < columns) {
    channels = rows;
    fileSize = columns;
  }
  else {
    sprintf(msg, "[chuck](via WvIn): Transpose the MAT-file array so that audio channels fill matrix rows (not columns).");
    return false;
  }
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Move read pointer to the data in the file.
  SINT32 headsize;
  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fread(&headsize, 4, 1, fd) != 1 ) goto error; // file size from 132nd byte
  if (byteswap) swap32((unsigned char *)&headsize);
  headsize -= fileSize * 8 * channels;
  if ( fseek(fd, headsize, SEEK_CUR) == -1 ) goto error;
  dataOffset = ftell(fd);

  // Assume MAT-files have 44100 Hz sample rate.
  fileRate = 44100.0;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  return true;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading MAT-file (%s).", fileName);
  return false;
}





void WvIn :: readData( unsigned long index )
{
  while (index < (unsigned long)chunkPointer) {
    // Negative rate.
    chunkPointer -= CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if (chunkPointer < 0) {
      bufferSize += chunkPointer;
      chunkPointer = 0;
    }
  }
  while (index >= chunkPointer+bufferSize) {
    // Positive rate.
    chunkPointer += CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if ( (unsigned long)chunkPointer+CHUNK_SIZE >= fileSize) {
      bufferSize = fileSize - chunkPointer;
    }
  }

  long i, length = bufferSize;
  bool endfile = (chunkPointer+bufferSize == fileSize);
  if ( !endfile ) length += 1;

  // Read samples into data[].  Use MY_FLOAT data structure
  // to store samples.
  if ( dataType == STK_SINT16 ) {
    SINT16 *buf = (SINT16 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*2), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 2, fd) != 2 ) goto error;
    if ( byteswap ) {
      SINT16 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap16((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 *buf = (SINT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      SINT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      FLOAT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*8), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 8, fd) != 8 ) goto error;
    if ( byteswap ) {
      FLOAT64 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap64((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT8 ) {
    unsigned char *buf = (unsigned char *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 1, fd) != 1 ) goto error;
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i] - 128.0;  // 8-bit WAV data is unsigned!
  }

  // If at end of file, repeat last sample frame for interpolation.
  if ( endfile ) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[(bufferSize-1)*channels+j];
  }

  if (!chunking) {
    fclose(fd);
    fd = 0;
  }

  return;

 error:
  sprintf(msg, "[chuck](via WvIn): Error reading file data.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvIn :: reset(void)
{
  time = (MY_FLOAT) 0.0;
  for (unsigned int i=0; i<channels; i++)
    lastOutput[i] = (MY_FLOAT) 0.0;
  finished = false;
}

void WvIn :: normalize(void)
{
  this->normalize((MY_FLOAT) 1.0);
}

// Normalize all channels equally by the greatest magnitude in all of the data.
void WvIn :: normalize(MY_FLOAT peak)
{
  if (chunking) {
    if ( dataType == STK_SINT8 ) gain = peak / 128.0;
    else if ( dataType == STK_SINT16 ) gain = peak / 32768.0;
    else if ( dataType == STK_SINT32 ) gain = peak / 2147483648.0;
    else if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) gain = peak;

    return;
  }

  unsigned long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;

  for (i=0; i<channels*bufferSize; i++) {
    if (fabs(data[i]) > max)
      max = (MY_FLOAT) fabs((double) data[i]);
  }

  if (max > 0.0) {
    max = (MY_FLOAT) 1.0 / max;
    max *= peak;
    for (i=0;i<=channels*bufferSize;i++)
        data[i] *= max;
  }
}

unsigned long WvIn :: getSize(void) const
{
  return fileSize;
}

unsigned int WvIn :: getChannels(void) const
{
  return channels;
}

MY_FLOAT WvIn :: getFileRate(void) const
{
  return fileRate;
}

bool WvIn :: isFinished(void) const
{
  return finished;
}

void WvIn :: setRate(MY_FLOAT aRate)
{
  rate = aRate;

  // If negative rate and at beginning of sound, move pointer to end
  // of sound.
  if ( (rate < 0) && (time == 0.0) ) time += rate + fileSize;

  if (fmod(rate, 1.0) != 0.0) interpolate = true;
  else interpolate = false;
}

void WvIn :: addTime(MY_FLOAT aTime)   
{
  // Add an absolute time in samples 
  time += aTime;

  if (time < 0.0) time = 0.0;
  if (time >= fileSize) {
    time = fileSize;
    finished = true;
  }
}

void WvIn :: setInterpolate(bool doInterpolate)
{
  interpolate = doInterpolate;
}

const MY_FLOAT *WvIn :: lastFrame(void) const
{
  return lastOutput;
}

MY_FLOAT WvIn :: lastOut(void) const
{
  if ( channels == 1 )
    return *lastOutput;

  MY_FLOAT output = 0.0;
  for (unsigned int i=0; i<channels; i++ ) {
    output += lastOutput[i];
  }
  return output / channels;
}

MY_FLOAT WvIn :: tick(void)
{
  tickFrame();
  return lastOut();
}

MY_FLOAT *WvIn :: tick(MY_FLOAT *vec, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vec[i] = tick();

  return vec;
}

const MY_FLOAT *WvIn :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  if (finished) return lastOutput;

  tyme = time;
  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Integer part of time address.
  index = (long) tyme;

  if (interpolate) {
    // Linear interpolation ... fractional part of time address.
    alpha = tyme - (MY_FLOAT) index;
    index *= channels;
    for (i=0; i<channels; i++) {
      lastOutput[i] = data[index];
      lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
      index++;
    }
  }
  else {
    index *= channels;
    for (i=0; i<channels; i++)
      lastOutput[i] = data[index++];
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;
  if ( time < 0.0 || time >= fileSize ) finished = true;

  return lastOutput;
}

MY_FLOAT *WvIn :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    tickFrame();
    for ( j=0; j<channels; j++ )
      frameVector[i*channels+j] = lastOutput[j];
  }

  return frameVector;
}
/***************************************************/
/*! \class WvOut
    \brief STK audio data output base class.

    This class provides output support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvOut writes samples to an audio file.  It
    supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which output single samples
    to all channels in a sample frame, from the
    tickFrame() method, which takes a pointer
    to multi-channel sample frame data.

    WvOut currently supports WAV, AIFF, AIFC, SND
    (AU), MAT-file (Matlab), and STK RAW file
    formats.  Signed integer (8-, 16-, and 32-bit)
    and floating- point (32- and 64-bit) data types
    are supported.  STK RAW files use 16-bit
    integers by definition.  MAT-files will always
    be written as 64-bit floats.  If a data type
    specification does not match the specified file
    type, the data type will automatically be
    modified.  Uncompressed data types are not
    supported.

    Currently, WvOut is non-interpolating and the
    output rate is always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/


const WvOut::FILE_TYPE WvOut :: WVOUT_RAW = 1;
const WvOut::FILE_TYPE WvOut :: WVOUT_WAV = 2;
const WvOut::FILE_TYPE WvOut :: WVOUT_SND = 3;
const WvOut::FILE_TYPE WvOut :: WVOUT_AIF = 4;
const WvOut::FILE_TYPE WvOut :: WVOUT_MAT = 5;

// WAV header structure. See ftp://ftp.isi.edu/in-notes/rfc2361.txt
// for information regarding format codes.
struct wavhdr {
  char riff[4];           // "RIFF"
  SINT32 file_size;      // in bytes
  char wave[4];           // "WAVE"
  char fmt[4];            // "fmt "
  SINT32 chunk_size;     // in bytes (16 for PCM)
  SINT16 format_tag;     // 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
  SINT16 num_chans;      // 1=mono, 2=stereo
  SINT32 sample_rate;
  SINT32 bytes_per_sec;
  SINT16 bytes_per_samp; // 2=16-bit mono, 4=16-bit stereo
  SINT16 bits_per_samp;
  char data[4];           // "data"
  SINT32 data_length;    // in bytes
};

// SND (AU) header structure (NeXT and Sun).
struct sndhdr {
  char pref[4];
  SINT32 hdr_length;
  SINT32 data_length;
  SINT32 format;
  SINT32 sample_rate;
  SINT32 num_channels;
  char comment[16];
};

// AIFF/AIFC header structure ... only the part common to both
// formats.
struct aifhdr {
  char form[4];               // "FORM"
  SINT32 form_size;          // in bytes
  char aiff[4];               // "AIFF" or "AIFC"
  char comm[4];               // "COMM"
  SINT32 comm_size;          // "COMM" chunk size (18 for AIFF, 24 for AIFC)
  SINT16 num_chans;          // number of channels
  unsigned long sample_frames; // sample frames of audio data
  SINT16 sample_size;        // in bits
  unsigned char srate[10];      // IEEE 754 floating point format
};

struct aifssnd {
  char ssnd[4];               // "SSND"
  SINT32 ssnd_size;          // "SSND" chunk size
  unsigned long offset;         // data offset in data block (should be 0)
  unsigned long block_size;     // not used by STK (should be 0)
};

// MAT-file 5 header structure.
struct mathdr {
  char heading[124];   // Header text field
  SINT16 hff[2];      // Header flag fields
  SINT32 adf[11];     // Array data format fields
  // There's more, but it's of variable length
};


XWriteThread *WvOut::s_writeThread = NULL;


size_t WvOut::fwrite(const void * ptr, size_t size, size_t nitems, FILE * stream)
{
    if(asyncIO)
        return s_writeThread->fwrite(ptr, size, nitems, stream);
    else
        return ::fwrite(ptr, size, nitems, stream);
}

int WvOut::fseek(FILE *stream, long offset, int whence)
{
    if(asyncIO)
        return s_writeThread->fseek(stream, offset, whence);
    else
        return ::fseek(stream, offset, whence);
}

int WvOut::fflush(FILE *stream)
{
    if(asyncIO)
        return s_writeThread->fflush(stream);
    else
        return ::fflush(stream);
}

int WvOut::fclose(FILE *stream)
{
    if(asyncIO)
        return s_writeThread->fclose(stream);
    else
        return ::fclose(stream);
}

size_t WvOut::fread(void *ptr, size_t size, size_t nitems, FILE *stream)
{
    // can't read asynchronously (yet)
    assert(0);
    return 0;
}

void WvOut::shutdown()
{
    if(s_writeThread)
    {
        s_writeThread->shutdown(); // deletes itself
        s_writeThread = NULL;
    }
}

WvOut :: WvOut()
{
  init();    
}

WvOut::WvOut( const char *fileName, unsigned int nChannels, FILE_TYPE type, Stk::STK_FORMAT format )
{
  init();
  openFile( fileName, nChannels, type, format );
}

WvOut :: ~WvOut()
{
  closeFile();

  if (data)
    delete [] data;
}

void WvOut :: init()
{
  fd = 0;
  data = 0;
  fileType = 0;
  dataType = 0;
  channels = 0;
  counter = 0;
  totalCount = 0;
  //m_filename[0] = '\0';
  start = TRUE;
  flush = 0;
  fileGain = 1;
    
    if(s_writeThread == NULL)
        s_writeThread = new XWriteThread(2<<20, 32);
    asyncIO = TRUE;
}

void WvOut :: closeFile( void )
{
  if( fd ) {
    // If there's an existing file, close it first.
    writeData( counter );

    if ( fileType == WVOUT_RAW )
      fclose( fd );
    else if ( fileType == WVOUT_WAV )
      closeWavFile();
    else if ( fileType == WVOUT_SND )
      closeSndFile();
    else if ( fileType == WVOUT_AIF )
      closeAifFile();
    else if ( fileType == WVOUT_MAT )
      closeMatFile();
    fd = 0;

    // printf("%f Seconds Computed\n\n", getTime() );
    totalCount = 0;
  }

  str_filename.set( "" );
  //m_filename[0] = '\0';
}

void WvOut :: openFile( const char *fileName, unsigned int nChannels, WvOut::FILE_TYPE type, Stk::STK_FORMAT format )
{
  closeFile();
  str_filename.set( fileName );
  //strncpy( m_filename, fileName, 255);
  //if ( strlen( fileName ) > 255 ) 
  //  m_filename[255] = '\0';

  if ( nChannels < 1 ) {
    sprintf(msg, "[chuck](via WvOut): the channels argument must be greater than zero!");
    handleError( msg, StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels;
  channels = nChannels;
  fileType = type;

  if ( format != STK_SINT8 && format != STK_SINT16 &&
       format != STK_SINT32 && format != MY_FLOAT32 && 
       format != MY_FLOAT64 ) {
    sprintf( msg, "[chuck](via WvOut): Unknown data type specified (%ld).", format );
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  } 
  dataType = format;

  bool result = false;
  if ( fileType == WVOUT_RAW ) {
    if ( channels != 1 ) {
      sprintf(msg, "[chuck](via WvOut): STK RAW files are, by definition, always monaural (channels = %d not supported)!", nChannels);
      handleError( msg, StkError::FUNCTION_ARGUMENT );
    }
    result = setRawFile( fileName );
  }
  else if ( fileType == WVOUT_WAV )
    result = setWavFile( fileName );
  else if ( fileType == WVOUT_SND )
    result = setSndFile( fileName );
  else if ( fileType == WVOUT_AIF )
    result = setAifFile( fileName );
  else if ( fileType == WVOUT_MAT )
    result = setMatFile( fileName );
  else {
    sprintf(msg, "[chuck](via WvOut): Unknown file type specified (%ld).", fileType);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  if ( result == false )
  {
    //closeFile();
    handleError(msg, StkError::FILE_ERROR);
    // handleError( msg, StkError::WARNING );
    // return;
  }

  // Allocate new memory if necessary.
  if ( lastChannels < channels ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[BUFFER_SIZE*channels];
  }
  counter = 0;
}

bool WvOut :: setRawFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".raw") == NULL) strcat(name, ".raw");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create RAW file: %s", name);
    return false;
  }

  if ( dataType != STK_SINT16 ) {
    dataType = STK_SINT16;
    sprintf(msg, "[chuck](via WvOut): Using 16-bit signed integer data format for file %s.", name);
    handleError(msg, StkError::WARNING);
  }

  byteswap = false;
  if( little_endian )
    byteswap = true;

  // printf("\nCreating RAW file: %s\n", name);
  return true;
}

bool WvOut :: setWavFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if( strstr(name, ".wav") == NULL ) strcat(name, ".wav");
  fd = fopen( name, "wb" );
  if( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create WAV file: %s", name);
    return false;
  }

  struct wavhdr hdr = {"RIF", 44, "WAV", "fmt", 16, 1, 1,
                        (SINT32) Stk::sampleRate(), 0, 2, 16, "dat", 0};
  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
  hdr.num_chans = (SINT16) channels;
  if ( dataType == STK_SINT8 )
    hdr.bits_per_samp = 8;
  else if ( dataType == STK_SINT16 )
    hdr.bits_per_samp = 16;
  else if ( dataType == STK_SINT32 )
    hdr.bits_per_samp = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 32;
  }
  else if ( dataType == MY_FLOAT64 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 64;
  }
  hdr.bytes_per_samp = (SINT16) (channels * hdr.bits_per_samp / 8);
  hdr.bytes_per_sec = (SINT32) (hdr.sample_rate * hdr.bytes_per_samp);

  byteswap = false;
  if( !little_endian )
  {
    byteswap = true;
    swap32((unsigned char *)&hdr.file_size);
    swap32((unsigned char *)&hdr.chunk_size);
    swap16((unsigned char *)&hdr.format_tag);
    swap16((unsigned char *)&hdr.num_chans);
    swap32((unsigned char *)&hdr.sample_rate);
    swap32((unsigned char *)&hdr.bytes_per_sec);
    swap16((unsigned char *)&hdr.bytes_per_samp);
    swap16((unsigned char *)&hdr.bits_per_samp);
  }

  if ( fwrite(&hdr, 4, 11, fd) != 11 ) {
    sprintf(msg, "[chuck](via WvOut): Could not write WAV header for file %s", name);
    return false;
  }

  // printf("\nCreating WAV file: %s\n", name);
  return true;
}

void WvOut :: closeWavFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * channels * bytes_per_sample;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 40, SEEK_SET); // jump to data length
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * channels * bytes_per_sample + 44;
if( !little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);
  fclose( fd );
}

bool WvOut :: setSndFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".snd") == NULL) strcat(name, ".snd");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create SND file: %s", name);
    return false;
  }

  struct sndhdr hdr = {".sn", 40, 0, 3, (SINT32) Stk::sampleRate(), 1, "Created by STK"};
  hdr.pref[3] = 'd';
  hdr.num_channels = channels;
  if ( dataType == STK_SINT8 )
    hdr.format = 2;
  else if ( dataType == STK_SINT16 )
    hdr.format = 3;
  else if ( dataType == STK_SINT32 )
    hdr.format = 5;
  else if ( dataType == MY_FLOAT32 )
    hdr.format = 6;
  else if ( dataType == MY_FLOAT64 )
    hdr.format = 7;

  byteswap = false;
if( little_endian )
{
  byteswap = true;
  swap32 ((unsigned char *)&hdr.hdr_length);
  swap32 ((unsigned char *)&hdr.format);
  swap32 ((unsigned char *)&hdr.sample_rate);
  swap32 ((unsigned char *)&hdr.num_channels);
}

  if ( fwrite(&hdr, 4, 10, fd) != 10 ) {
    sprintf(msg, "[chuck](via WvOut): Could not write SND header for file %s", name);
    return false;
  }

  // printf("\nCreating SND file: %s\n", name);
  return true;
}

void WvOut :: closeSndFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * bytes_per_sample * channels;
if( little_endian )
  swap32 ((unsigned char *)&bytes);

  fseek(fd, 8, SEEK_SET); // jump to data size
  fwrite(&bytes, 4, 1, fd);
  fclose(fd);
}

bool WvOut :: setAifFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".aif") == NULL) strcat(name, ".aif");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create AIF file: %s", name);
    return false;
  }

  // Common parts of AIFF/AIFC header.
  struct aifhdr hdr = {"FOR", 46, "AIF", "COM", 18, 0, 0, 16, "0"};
  struct aifssnd ssnd = {"SSN", 8, 0, 0};
  hdr.form[3] = 'M';
  hdr.aiff[3] = 'F';
  hdr.comm[3] = 'M';
  ssnd.ssnd[3] = 'D';
  hdr.num_chans = channels;
  if ( dataType == STK_SINT8 )
    hdr.sample_size = 8;
  else if ( dataType == STK_SINT16 )
    hdr.sample_size = 16;
  else if ( dataType == STK_SINT32 )
    hdr.sample_size = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 32;
    hdr.comm_size = 24;
  }
  else if ( dataType == MY_FLOAT64 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 64;
    hdr.comm_size = 24;
  }

  // For AIFF files, the sample rate is stored in a 10-byte,
  // IEEE Standard 754 floating point number, so we need to
  // convert to that.
  SINT16 i;
  unsigned long exp;
  unsigned long rate = (unsigned long) Stk::sampleRate();
  memset(hdr.srate, 0, 10);
  exp = rate;
  for (i=0; i<32; i++) {
    exp >>= 1;
    if (!exp) break;
  }
  i += 16383;
if( little_endian )
  swap16((unsigned char *)&i);

  *(SINT16 *)(hdr.srate) = (SINT16) i;

  for (i=32; i; i--) {
    if (rate & 0x80000000) break;
    rate <<= 1;
  }

if( little_endian )
  swap32((unsigned char *)&rate);

  *(unsigned long *)(hdr.srate+2) = (unsigned long) rate;

  byteswap = false;  
if( little_endian )
{
  byteswap = true;
  swap32((unsigned char *)&hdr.form_size);
  swap32((unsigned char *)&hdr.comm_size);
  swap16((unsigned char *)&hdr.num_chans);
  swap16((unsigned char *)&hdr.sample_size);
  swap32((unsigned char *)&ssnd.ssnd_size);
  swap32((unsigned char *)&ssnd.offset);
  swap32((unsigned char *)&ssnd.block_size);
}

  // The structure boundaries don't allow a single write of 54 bytes.
  if ( fwrite(&hdr, 4, 5, fd) != 5 ) goto error;
  if ( fwrite(&hdr.num_chans, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_frames, 4, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_size, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.srate, 10, 1, fd) != 1 ) goto error;

  if ( dataType == MY_FLOAT32 ) {
    char type[4] = {'f','l','3','2'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  else if ( dataType == MY_FLOAT64 ) {
    char type[4] = {'f','l','6','4'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  
  if ( fwrite(&ssnd, 4, 4, fd) != 4 ) goto error;

  // printf("\nCreating AIF file: %s\n", name);
  return true;

 error:
  sprintf(msg, "[chuck](via WvOut): Could not write AIF header for file %s", name);
  return false;
}

void WvOut :: closeAifFile( void )
{
  unsigned long frames = (unsigned long) totalCount;
if( little_endian )
  swap32((unsigned char *)&frames);

  fseek(fd, 22, SEEK_SET); // jump to "COMM" sample_frames
  fwrite(&frames, 4, 1, fd);

  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  unsigned long bytes = totalCount * bytes_per_sample * channels + 46;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
if( little_endian )
  swap32((unsigned char *)&bytes);

  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * bytes_per_sample * channels + 8;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
if( little_endian )
  swap32((unsigned char *)&bytes);

  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 )
    fseek(fd, 48, SEEK_SET); // jump to "SSND" chunk size
  else
    fseek(fd, 42, SEEK_SET); // jump to "SSND" chunk size
  fwrite(&bytes, 4, 1, fd);

  fclose( fd );
}

bool WvOut :: setMatFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".mat") == NULL) strcat(name, ".mat");
  fd = fopen(name, "w+b");
  if ( !fd ) {
    sprintf(msg, "[chuck](via WvOut): Could not create MAT file: %s", name);
    return false;
  }

  if ( dataType != MY_FLOAT64 ) {
    dataType = MY_FLOAT64;
    sprintf(msg, "[chuck](via WvOut): Using 64-bit floating-point data format for file %s", name);
    handleError(msg, StkError::WARNING);
  }

  struct mathdr hdr;
  strcpy(hdr.heading,"MATLAB 5.0 MAT-file, Generated using the Synthesis ToolKit in C++ (STK). By Perry R. Cook and Gary P. Scavone, 1995-2002.");

  int i;
  for (i=strlen(hdr.heading);i<124;i++) hdr.heading[i] = ' ';

  // Header Flag Fields
  hdr.hff[0] = (SINT16) 0x0100;   // Version field
  hdr.hff[1] = (SINT16) 'M';      // Endian indicator field ("MI")
  hdr.hff[1] <<= 8;
  hdr.hff[1] += 'I';

  hdr.adf[0] = (SINT32) 14;       // Matlab array data type value
  hdr.adf[1] = (SINT32) 0;        // Size of file after this point to end (in bytes)
                                 // Don't know size yet.

  // Numeric Array Subelements (4):
  // 1. Array Flags
  hdr.adf[2] = (SINT32) 6;        // Matlab 32-bit unsigned integer data type value
  hdr.adf[3] = (SINT32) 8;        // 8 bytes of data to follow
  hdr.adf[4] = (SINT32) 6;        // Double-precision array, no array flags set
  hdr.adf[5] = (SINT32) 0;        // 4 bytes undefined
  // 2. Array Dimensions
  hdr.adf[6] = (SINT32) 5;        // Matlab 32-bit signed integer data type value
  hdr.adf[7] = (SINT32) 8;        // 8 bytes of data to follow (2D array)
  hdr.adf[8] = (SINT32) channels; // This is the number of rows
  hdr.adf[9] = (SINT32) 0;        // This is the number of columns

  // 3. Array Name
  // We'll use fileName for the matlab array name (as well as the file name).
  // If fileName is 4 characters or less, we have to use a compressed data element
  // format for the array name data element.  Otherwise, the array name must
  // be formatted in 8-byte increments (up to 31 characters + NULL).
  SINT32 namelength = (SINT32) strlen(fileName);
  if (strstr(fileName, ".mat")) namelength -= 4;
  if (namelength > 31) namelength = 31; // Truncate name to 31 characters.
  char arrayName[64];
  strncpy(arrayName, fileName, namelength);
  arrayName[namelength] = '\0';
  if (namelength > 4) {
    hdr.adf[10] = (SINT32) 1;        // Matlab 8-bit signed integer data type value
  }
  else { // Compressed data element format
    hdr.adf[10] = namelength;
    hdr.adf[10] <<= 16;
    hdr.adf[10] += 1;
  }
  SINT32 headsize = 40;        // Number of bytes in data element so far.

  // Write the fixed portion of the header
  if ( fwrite(&hdr, 172, 1, fd) != 1 ) goto error;

  // Write MATLAB array name
  SINT32 tmp;
  if (namelength > 4) {
    if ( fwrite(&namelength, 4, 1, fd) != 1) goto error;
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = (SINT32) ceil((float)namelength / 8);
    if ( fseek(fd, tmp*8-namelength, SEEK_CUR) == -1 ) goto error;
    headsize += tmp * 8;
  }
  else { // Compressed data element format
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = 4 - namelength;
    if ( fseek(fd, tmp, SEEK_CUR) == -1 ) goto error;
  }

  // Finish writing known header information
  tmp = 9;        // Matlab IEEE 754 double data type
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  tmp = 0;        // Size of real part subelement in bytes (8 per sample)
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  headsize += 8;  // Total number of bytes in data element so far

  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fwrite(&headsize, 4, 1, fd) != 1 ) goto error; // Write header size ... will update at end
  if ( fseek(fd, 0, SEEK_END) == -1 ) goto error;

  byteswap = false;
  CK_FPRINTF_STDERR( "[chuck]:(via STK): creating MAT-file (%s) containing MATLAB array: %s\n", name, arrayName);
  return true;

 error:
  sprintf(msg, "[chuck](via WvOut): Could not write MAT-file header for file %s", name);
  return false;
}

void WvOut :: closeMatFile( void )
{
  fseek(fd, 164, SEEK_SET); // jump to number of columns
  fwrite(&totalCount, 4, 1, fd);

  SINT32 headsize, temp;
  fseek(fd, 132, SEEK_SET);  // jump to header size
  fread(&headsize, 4, 1, fd);
  temp = headsize;
  headsize += (SINT32) (totalCount * 8 * channels);
  fseek(fd, 132, SEEK_SET);
  // Write file size (minus some header info)
  fwrite(&headsize, 4, 1, fd);

  fseek(fd, temp+132, SEEK_SET); // jumpt to data size (in bytes)
  temp = totalCount * 8 * channels;
  fwrite(&temp, 4, 1, fd);

  fclose(fd);
}

unsigned long WvOut :: getFrames( void ) const
{
  return totalCount;
}

MY_FLOAT WvOut :: getTime( void ) const
{
  return (MY_FLOAT) totalCount / Stk::sampleRate();
}

void WvOut :: writeData( unsigned long frames )
{
  // make sure we have file descriptor
  if( !fd ) return;

  if ( dataType == STK_SINT8 ) {
    if ( fileType == WVOUT_WAV ) { // 8-bit WAV data is unsigned!
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        float float_sample = data[k] * 127.0 + 128.0;
        if(float_sample < 0) float_sample = 0;
        if(float_sample > 255) float_sample = 255;
        unsigned char sample = (unsigned char) float_sample;
        
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
    else {
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        float float_sample = data[k] * 127.0;
        if(float_sample < -128) float_sample = -128;
        if(float_sample > 127) float_sample = 127;
        signed char sample = (signed char) float_sample;
        
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
  }
  else if ( dataType == STK_SINT16 ) {
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      float float_sample = data[k] * 32767.0;
      if(float_sample < -32767) float_sample = -32767;
      if(float_sample > 32767) float_sample = 32767;
      SINT16 sample = (SINT16) float_sample;
      
      if ( byteswap ) swap16( (unsigned char *)&sample );
      if ( fwrite(&sample, 2, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == STK_SINT32 ) {
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      float float_sample = data[k] * 32767.0;
      if(float_sample < -2147483647) float_sample = (float)-2147483647;
      if(float_sample > 2147483647) float_sample = (float)2147483647;
      SINT32 sample = (SINT32) float_sample;
      
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT32) (data[k]);
      
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT64) (data[k]);
      
      if ( byteswap ) swap64( (unsigned char *)&sample );
      if ( fwrite(&sample, 8, 1, fd) != 1 ) goto error;
    }
  }

  flush += frames;
  if( flush >= 8192 )
  {
      flush = 0;
      fflush( fd );
  }

  return;

 error:
  sprintf(msg, "[chuck](via WvOut): Error writing data to file.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvOut :: tick(const MY_FLOAT sample)
{
  if ( !fd ) return;

  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter == BUFFER_SIZE ) {
    writeData( BUFFER_SIZE );
    counter = 0;
  }
}

void WvOut :: tick(const MY_FLOAT *vec, unsigned int vectorSize)
{
  if ( !fd ) return;

  for (unsigned int i=0; i<vectorSize; i++)
    tick( vec[i] );
}

void WvOut :: tickFrame(const MY_FLOAT *frameVector, unsigned int frames)
{
  if ( !fd ) return;

  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    for ( j=0; j<channels; j++ ) {
      data[counter*channels+j] = frameVector[i*channels+j];
    }
    counter++;
    totalCount++;

    if ( counter == BUFFER_SIZE ) {
      writeData( BUFFER_SIZE );
      counter = 0;
    }
  }
}



/**********************************************************************/
/*! \class MidiFileIn
 \brief A standard MIDI file reading/parsing class.
 
 This class can be used to read events from a standard MIDI file.
 Event bytes are copied to a C++ vector and must be subsequently
 interpreted by the user.  The function getNextMidiEvent() skips
 meta and sysex events, returning only MIDI channel messages.
 Event delta-times are returned in the form of "ticks" and a
 function is provided to determine the current "seconds per tick".
 Tempo changes are internally tracked by the class and reflected in
 the values returned by the function getTickSeconds().
 
 by Gary P. Scavone, 2003 - 2010.
 */
/**********************************************************************/

#include <cstring>
#include <iostream>

#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#define __LITTLE_ENDIAN__
#endif // !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)

namespace stk {
    
MidiFileIn :: MidiFileIn( std::string fileName )
{
    // Attempt to open the file.
    file_.open( fileName.c_str(), std::ios::in | std::ios::binary );
    if ( !file_ ) {
        CK_STDCOUT << "MidiFileIn: error opening or finding file (" <<  fileName << ").";
        handleError( "", StkError::FILE_NOT_FOUND );
    }
    
    // Parse header info.
    char chunkType[4];
    char buffer[4];
    SINT32 *length;
    if ( !file_.read( chunkType, 4 ) ) goto error;
    if ( !file_.read( buffer, 4 ) ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&buffer);
#endif
    length = (SINT32 *) &buffer;
    if ( strncmp( chunkType, "MThd", 4 ) || ( *length != 6 ) ) {
        CK_STDCOUT << "MidiFileIn: file (" <<  fileName << ") does not appear to be a MIDI file!";
        handleError( "", StkError::FILE_UNKNOWN_FORMAT );
    }
    
    // Read the MIDI file format.
    SINT16 *data;
    if ( !file_.read( buffer, 2 ) ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)&buffer);
#endif
    data = (SINT16 *) &buffer;
    if ( *data < 0 || *data > 2 ) {
        CK_STDCOUT << "MidiFileIn: the file (" <<  fileName << ") format is invalid!";
        handleError( "", StkError::FILE_ERROR );
    }
    format_ = *data;
    
    // Read the number of tracks.
    if ( !file_.read( buffer, 2 ) ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)&buffer);
#endif
    if ( format_ == 0 && *data != 1 ) {
        CK_STDCOUT << "MidiFileIn: invalid number of tracks (>1) for a file format = 0!";
        handleError( "", StkError::FILE_ERROR );
    }
    nTracks_ = *data;
    
    // Read the beat division.
    if ( !file_.read( buffer, 2 ) ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)&buffer);
#endif
    division_ = (int) *data;
    double tickrate;
    usingTimeCode_ = false;
    if ( *data & 0x8000 ) {
        // Determine ticks per second from time-code formats.
        tickrate = (double) -(*data & 0x7F00);
        // If frames per second value is 29, it really should be 29.97.
        if ( tickrate == 29.0 ) tickrate = 29.97;
        tickrate *= (*data & 0x00FF);
        usingTimeCode_ = true;
    }
    else {
        tickrate = (double) (*data & 0x7FFF); // ticks per quarter note
    }
    
    // Now locate the track offsets and lengths.  If not using time
    // code, we can initialize the "tick time" using a default tempo of
    // 120 beats per minute.  We will then check for tempo meta-events
    // afterward.
    unsigned int i;
    for ( i=0; i<nTracks_; i++ ) {
        if ( !file_.read( chunkType, 4 ) ) goto error;
        if ( strncmp( chunkType, "MTrk", 4 ) ) goto error;
        if ( !file_.read( buffer, 4 ) ) goto error;
#ifdef __LITTLE_ENDIAN__
        swap32((unsigned char *)&buffer);
#endif
        length = (SINT32 *) &buffer;
        trackLengths_.push_back( *length );
        trackOffsets_.push_back( (long) file_.tellg() );
        trackPointers_.push_back( (long) file_.tellg() );
        trackStatus_.push_back( 0 );
        file_.seekg( *length, std::ios_base::cur );
        if ( usingTimeCode_ ) tickSeconds_.push_back( (double) (1.0 / tickrate) );
        else tickSeconds_.push_back( (double) (0.5 / tickrate) );
    }
    
    // Save the initial tickSeconds parameter.
    TempoChange tempoEvent;
    tempoEvent.count = 0;
    tempoEvent.tickSeconds = tickSeconds_[0];
    tempoEvents_.push_back( tempoEvent );
    
    // If format 1 and not using time code, parse and save the tempo map
    // on track 0.
    if ( format_ == 1 && !usingTimeCode_ ) {
        std::vector<unsigned char> event;
        unsigned long value, count;
        
        // We need to temporarily change the usingTimeCode_ value here so
        // that the getNextEvent() function doesn't try to check the tempo
        // map (which we're creating here).
        usingTimeCode_ = true;
        count = getNextEvent( &event, 0 );
        while ( event.size() ) {
            if ( ( event.size() == 6 ) && ( event[0] == 0xff ) &&
                ( event[1] == 0x51 ) && ( event[2] == 0x03 ) ) {
                tempoEvent.count = count;
                value = ( event[3] << 16 ) + ( event[4] << 8 ) + event[5];
                tempoEvent.tickSeconds = (double) (0.000001 * value / tickrate);
                if ( count > tempoEvents_.back().count )
                    tempoEvents_.push_back( tempoEvent );
                else
                    tempoEvents_.back() = tempoEvent;
            }
            count += getNextEvent( &event, 0 );
        }
        rewindTrack( 0 );
        for ( unsigned int i=0; i<nTracks_; i++ ) {
            trackCounters_.push_back( 0 );
            trackTempoIndex_.push_back( 0 );
        }
        // Change the time code flag back!
        usingTimeCode_ = false;
    }
    
    return;
    
error:
    CK_STDCOUT << "MidiFileIn: error reading from file (" <<  fileName << ").";
    handleError( "", StkError::FILE_ERROR );
}

MidiFileIn :: ~MidiFileIn()
{
    // An ifstream object implicitly closes itself during destruction
    // but we'll make an explicit call to "close" anyway.
    file_.close();
}

void MidiFileIn :: rewindTrack( unsigned int track )
{
    if ( track >= nTracks_ ) {
        CK_STDCOUT << "MidiFileIn::getNextEvent: invalid track argument (" <<  track << ").";
        handleError( "", StkError::WARNING ); return;
    }
    
    trackPointers_[track] = trackOffsets_[track];
    trackStatus_[track] = 0;
    tickSeconds_[track] = tempoEvents_[0].tickSeconds;
}

double MidiFileIn :: getTickSeconds( unsigned int track )
{
    // Return the current tick value in seconds for the given track.
    if ( track >= nTracks_ ) {
        CK_STDCOUT << "MidiFileIn::getTickSeconds: invalid track argument (" <<  track << ").";
        handleError( "", StkError::WARNING ); return 0.0;
    }
    
    return tickSeconds_[track];
}

unsigned long MidiFileIn :: getNextEvent( std::vector<unsigned char> *event, unsigned int track )
{
    // Fill the user-provided vector with the next event in the
    // specified track (default = 0) and return the event delta time in
    // ticks.  This function assumes that the stored track pointer is
    // positioned at the start of a track event.  If the track has
    // reached its end, the event vector size will be zero.
    //
    // If we have a format 0 or 2 file and we're not using timecode, we
    // should check every meta-event for tempo changes and make
    // appropriate updates to the tickSeconds_ parameter if so.
    //
    // If we have a format 1 file and we're not using timecode, keep a
    // running sum of ticks for each track and update the tickSeconds_
    // parameter as needed based on the stored tempo map.
    
    event->clear();
    if ( track >= nTracks_ ) {
        CK_STDCOUT << "MidiFileIn::getNextEvent: invalid track argument (" <<  track << ").";
        handleError( "", StkError::WARNING ); return 0;
    }
    
    // Check for the end of the track.
    if ( (trackPointers_[track] - trackOffsets_[track]) >= trackLengths_[track] )
        return 0;
    
    unsigned long ticks = 0, bytes = 0;
    bool isTempoEvent = false;
    
    // Read the event delta time.
    file_.seekg( trackPointers_[track], std::ios_base::beg );
    if ( !readVariableLength( &ticks ) ) goto error;
    
    // Parse the event stream to determine the event length.
    unsigned char c;
    if ( !file_.read( (char *)&c, 1 ) ) goto error;
    switch ( c ) {
            
        case 0xFF: // A Meta-Event
            unsigned long position;
            trackStatus_[track] = 0;
            event->push_back( c );
            if ( !file_.read( (char *)&c, 1 ) ) goto error;
            event->push_back( c );
            if ( format_ != 1 && ( c == 0x51 ) ) isTempoEvent = true;
            position = file_.tellg();
            if ( !readVariableLength( &bytes ) ) goto error;
            bytes += ( (unsigned long)file_.tellg() - position );
            file_.seekg( position, std::ios_base::beg );
            break;
            
        case 0xF0:
        case 0xF7: // The start or continuation of a Sysex event
            trackStatus_[track] = 0;
            event->push_back( c );
            position = file_.tellg();
            if ( !readVariableLength( &bytes ) ) goto error;
            bytes += ( (unsigned long)file_.tellg() - position );
            file_.seekg( position, std::ios_base::beg );
            break;
            
        default: // Should be a MIDI channel event
            if ( c & 0x80 ) { // MIDI status byte
                if ( c > 0xF0 ) goto error;
                trackStatus_[track] = c;
                event->push_back( c );
                c &= 0xF0;
                if ( (c == 0xC0) || (c == 0xD0) ) bytes = 1;
                else bytes = 2;
            }
            else if ( trackStatus_[track] & 0x80 ) { // Running status
                event->push_back( trackStatus_[track] );
                event->push_back( c );
                c = trackStatus_[track] & 0xF0;
                if ( (c != 0xC0) && (c != 0xD0) ) bytes = 1;
            }
            else goto error;
            
    }
    
    // Read the rest of the event into the event vector.
    unsigned long i;
    for ( i=0; i<bytes; i++ ) {
        if ( !file_.read( (char *)&c, 1 ) ) goto error;
        event->push_back( c );
    }
    
    if ( !usingTimeCode_ ) {
        if ( isTempoEvent ) {
            // Parse the tempo event and update tickSeconds_[track].
            double tickrate = (double) (division_ & 0x7FFF);
            unsigned long value = ( event->at(3) << 16 ) + ( event->at(4) << 8 ) + event->at(5);
            tickSeconds_[track] = (double) (0.000001 * value / tickrate);
        }
        
        if ( format_ == 1 ) {
            // Update track counter and check the tempo map.
            trackCounters_[track] += ticks;
            TempoChange tempoEvent = tempoEvents_[ trackTempoIndex_[track] ];
            if ( trackCounters_[track] >= tempoEvent.count && trackTempoIndex_[track] < tempoEvents_.size() - 1 ) {
                trackTempoIndex_[track]++;
                tickSeconds_[track] = tempoEvent.tickSeconds;
            }
        }
    }
    
    // Save the current track pointer value.
    trackPointers_[track] = file_.tellg();
    
    return ticks;
    
error:
    CK_STDCOUT << "MidiFileIn::getNextEvent: file read error!";
    handleError( "", StkError::FILE_ERROR );
    return 0;
}

unsigned long MidiFileIn :: getNextMidiEvent( std::vector<unsigned char> *midiEvent, unsigned int track )
{
    // Fill the user-provided vector with the next MIDI event in the
    // specified track (default = 0) and return the event delta time in
    // ticks.  Meta-Events preceeding this event are skipped and ignored.
    if ( track >= nTracks_ ) {
        CK_STDCOUT << "MidiFileIn::getNextMidiEvent: invalid track argument (" <<  track << ").";
        handleError( "", StkError::WARNING ); return 0;
    }
    
    unsigned long ticks = getNextEvent( midiEvent, track );
    while ( midiEvent->size() && ( midiEvent->at(0) >= 0xF0 ) ) {
        //for ( unsigned int i=0; i<midiEvent->size(); i++ )
        //CK_STDCOUT << "event byte = " << i << ", value = " << (int)midiEvent->at(i) << CK_STDENDL;
        ticks = getNextEvent( midiEvent, track );
    }
    
    //for ( unsigned int i=0; i<midiEvent->size(); i++ )
    //CK_STDCOUT << "event byte = " << i << ", value = " << (int)midiEvent->at(i) << CK_STDENDL;
    
    return ticks;
}

bool MidiFileIn :: readVariableLength( unsigned long *value )
{
    // It is assumed that this function is called with the file read
    // pointer positioned at the start of a variable-length value.  The
    // function returns "true" if the value is successfully parsed and
    // "false" otherwise.
    *value = 0;
    char c;
    
    if ( !file_.read( &c, 1 ) ) return false;
    *value = (unsigned long) c;
    if ( *value & 0x80 ) {
        *value &= 0x7f;
        do {
            if ( !file_.read( &c, 1 ) ) return false;
            *value = ( *value << 7 ) + ( c & 0x7f );
        } while ( c & 0x80 );
    }
    
    return true;
} 
    
} // stk namespace


// chuck - import
// wrapper functions

//convenience functions 
/* void 
ck_domidi ( Instrmnt * inst, unsigned int i ) { 
    unsigned char status = (i>>16)&&0xff;
    unsigned char data1  = (i>>8)&&0xff;
    unsigned char data2  = (i)&&0xff;

}

void 
ck_domidi ( Instrmnt * inst, unsigned char status, unsigned char data1, unsigned char data2) { 
    unsigned char type = status && 0xf0; 
    switch ( type ) { 
    case __SK_NoteOn_: 
      inst->noteOn( mtof ( (float)data1 ), ((float)data2) / 128.0 );
      break;
    case __SK_NoteOff_:
      inst->noteOff( ((float)data2) / 128.0 );
      break;
    case __SK_ControlChange_:
      inst->controlChange( data1, data2 );
      break;
    case __SK_AfterTouch_:
      inst->controlChange( __SK_AfterTouch_, ((float)data1) / 128.0 );
    case __SK_PitchBend_:
      unsigned int mnum = ((unsigned int)data2) << 7 || data1 );
      inst->controlChange( __SK_PitchBend_, ((float)mnum) / 16384.0 );
      break;
    }
}

*/




// StkInstrument
//-----------------------------------------------------------------------------
// name: Instrmnt_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Instrmnt_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)0;
    // CK_FPRINTF_STDERR( "[chuck](via STK): error : StkInstrument is virtual!\n");
}


//-----------------------------------------------------------------------------
// name: Instrmnt_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Instrmnt_dtor )
{
    // should be done already by now
}


//-----------------------------------------------------------------------------
// name: Instrmnt_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Instrmnt_tick )
{
    Instrmnt * i = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    if( !out ) CK_FPRINTF_STDERR( "[chuck](via STK): we warned you...\n");
    *out = i->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Instrmnt_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Instrmnt_ctrl_noteOn()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Instrmnt_ctrl_noteOn )
{
    Instrmnt * i = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    i->noteOn( i->m_frequency, f );
}


//-----------------------------------------------------------------------------
// name: Instrmnt_ctrl_noteOff()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Instrmnt_ctrl_noteOff )
{
    Instrmnt * i = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    i->noteOff( f );
}


//-----------------------------------------------------------------------------
// name: Instrmnt_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Instrmnt_ctrl_freq )
{
    Instrmnt * i = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    i->setFrequency( f );
    RETURN->v_float = (t_CKFLOAT)i->m_frequency;
}


//-----------------------------------------------------------------------------
// name: Instrmnt_cget_freq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Instrmnt_cget_freq )
{
    Instrmnt * i = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
//    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    RETURN->v_float = (t_CKFLOAT)i->m_frequency;
}


//-----------------------------------------------------------------------------
// name: Instrmnt_ctrl_controlChange()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL(Instrmnt_ctrl_controlChange )
{
    Instrmnt * ii = (Instrmnt *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKINT i = GET_NEXT_INT(ARGS);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    ii->controlChange( i, f );
}




//-----------------------------------------------------------------------------
// name: BandedWG_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BandedWG_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT) new BandedWG();
}

//-----------------------------------------------------------------------------
// name: BandedWG_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( BandedWG_dtor )
{
    delete (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}

//-----------------------------------------------------------------------------
// name: BandedWG_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BandedWG_tick )
{
    *out = ((BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data))->tick();
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: BandedWG_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BandedWG_pmsg )
{
    return FALSE;
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_pluck()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_pluck )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->pluck( GET_NEXT_FLOAT(ARGS));
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_strikePosition()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_strikePosition )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->setStrikePosition( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)f->m_strikePosition;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_strikePosition()
// desc: CGET function ...
//-----------------------------------------------------------------------------

CK_DLL_CGET( BandedWG_cget_strikePosition )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->m_strikePosition;
}


//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_bowRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_bowRate )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->m_rate =  GET_NEXT_FLOAT(ARGS);
    RETURN->v_float = (t_CKFLOAT) f->m_rate;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_bowRate()
// desc: CGET function ...
//-----------------------------------------------------------------------------

CK_DLL_CGET( BandedWG_cget_bowRate )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->m_rate;
}


//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_bowPressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_bowPressure )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->controlChange( __SK_BowPressure_, GET_NEXT_FLOAT(ARGS) * 128.0 );
    RETURN->v_float = (t_CKFLOAT)f->m_bowPressure;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_bowPressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BandedWG_cget_bowPressure )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->m_bowPressure;
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_bowMotion()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_bowMotion )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->controlChange( 4, GET_NEXT_FLOAT(ARGS) * 128.0 );
    RETURN->v_float = (t_CKFLOAT)f->m_bowMotion;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_bowMotion()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BandedWG_cget_bowMotion )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->m_bowMotion;
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_vibratoFreq )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->controlChange( 4, GET_NEXT_FLOAT(ARGS) * 128.0 );
    RETURN->v_float = (t_CKFLOAT)f->integrationConstant;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BandedWG_cget_vibratoFreq )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->integrationConstant;
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_modesGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_modesGain )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->controlChange( 1, GET_NEXT_FLOAT(ARGS) * 128.0 );
    RETURN->v_float = (t_CKFLOAT)f->m_modesGain;
}

//-----------------------------------------------------------------------------
// name: BandedWG_cget_modesGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BandedWG_cget_modesGain )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->m_modesGain;
}

//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_preset()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_preset )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->setPreset ( GET_NEXT_INT(ARGS) );
    RETURN->v_int = (t_CKINT) f->m_preset;
}


//-----------------------------------------------------------------------------
// name: BandedWG_cget_preset()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BandedWG_cget_preset )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_int = (t_CKINT) f->m_preset;
}


//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_startBowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_startBowing )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->startBowing( GET_NEXT_FLOAT(ARGS), f->m_rate );
}


//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_stopBowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_stopBowing )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    f->stopBowing( GET_NEXT_FLOAT(ARGS) );
}


//-----------------------------------------------------------------------------
// name: BandedWG_ctrl_controlChange()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BandedWG_ctrl_controlChange )
{
    BandedWG * f = (BandedWG *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKINT i = GET_NEXT_INT(ARGS);
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    f->controlChange( i, v );
}


// BiQuad
struct BiQuad_
{
    BiQuad biquad;
    t_CKFLOAT pfreq;
    t_CKFLOAT prad;
    t_CKFLOAT zfreq;
    t_CKFLOAT zrad;
    t_CKBOOL norm;
};


//-----------------------------------------------------------------------------
// name: BiQuad_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BiQuad_ctor )
{
    BiQuad_ * d = new BiQuad_;
    d->pfreq = 0.0;
    d->prad = 0.0;
    d->zfreq = 0.0;
    d->zrad = 0.0;
    d->norm = FALSE;

    OBJ_MEMBER_UINT(SELF, BiQuad_offset_data) = (t_CKUINT)d;
}


//-----------------------------------------------------------------------------
// name: BiQuad_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( BiQuad_dtor )
{
    delete (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    OBJ_MEMBER_UINT(SELF, BiQuad_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: BiQuad_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BiQuad_tick )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    *out = (SAMPLE)f->biquad.tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BiQuad_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BiQuad_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_b2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_b2 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setB2( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[2];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_b2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_b2 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[2];
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_b1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_b1 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setB1( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[1];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_b1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_b1 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[1];
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_b0 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setB0( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[0];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_b0 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.b[0];
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_a2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_a2 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setA2( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT) f->biquad.a[2];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_a2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_a2 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.a[2];
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_a1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_a1 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setA1( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT) f->biquad.a[1];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_a1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_a1 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.a[1];
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_a0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BiQuad_cget_a0 )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT) f->biquad.a[0];
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_pfreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_pfreq )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->pfreq = GET_NEXT_FLOAT(ARGS);
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
    RETURN->v_float = (t_CKFLOAT)f->pfreq;
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_pfreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_cget_pfreq )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->pfreq;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_prad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_prad )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->prad = GET_NEXT_FLOAT(ARGS);
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
    RETURN->v_float = (t_CKFLOAT)f->prad;
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_prad()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_cget_prad )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->prad;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_zfreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_zfreq )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->zfreq = GET_NEXT_FLOAT(ARGS);
    f->biquad.setNotch( f->zfreq, f->zrad );
    RETURN->v_float = (t_CKFLOAT)f->zfreq;
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_zfreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_cget_zfreq )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = (t_CKFLOAT)f->zfreq;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_zrad()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_zrad )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->zrad = GET_NEXT_FLOAT(ARGS);
    f->biquad.setNotch( f->zfreq, f->zrad );
    RETURN->v_float = f->zrad;
}


//-----------------------------------------------------------------------------
// name: BiQuad_cget_zrad()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_cget_zrad )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    RETURN->v_float = f->zrad;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_norm()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_norm )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->norm = GET_NEXT_UINT(ARGS) != 0;
    f->biquad.setResonance( f->pfreq, f->prad, f->norm != 0 );
    RETURN->v_int = f->norm;
}


//-----------------------------------------------------------------------------
// name: BiQuad_ctrl_eqzs()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BiQuad_ctrl_eqzs )
{
    BiQuad_ * f = (BiQuad_ *)OBJ_MEMBER_UINT(SELF, BiQuad_offset_data);
    f->biquad.setEqualGainZeroes();
}


// BlowBotl
//-----------------------------------------------------------------------------
// name: BlowBotl_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BlowBotl_ctor )
{
    // initialize member object
    BlowBotl * botl = new BlowBotl();
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)botl;
    // default
    botl->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: BlowBotl_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( BlowBotl_dtor )
{
    delete (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BlowBotl_tick )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = p->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BlowBotl_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_startBlowing )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->startBlowing( f );
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_stopBlowing )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_rate )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)p->m_rate;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowBotl_cget_rate )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) p->m_rate;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_noiseGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_noiseGain )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 4, f * 128 );
    RETURN->v_float = (t_CKFLOAT)p->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_cget_noiseGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowBotl_cget_noiseGain )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_vibratoFreq )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setVibratoFreq( f );
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowBotl_cget_vibratoFreq )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_vibratoGain )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 1, f * 128 );
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowBotl_cget_vibratoGain )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_ctrl_volume()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowBotl_ctrl_volume )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 128, f * 128 );
    RETURN->v_float = (t_CKFLOAT)p->m_volume;
}


//-----------------------------------------------------------------------------
// name: BlowBotl_cget_volume()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowBotl_cget_volume )
{
    BlowBotl * p = (BlowBotl *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_volume;
}


// BlowHole
//-----------------------------------------------------------------------------
// name: BlowHole_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BlowHole_ctor )
{
    // initialize member object
    BlowHole * hole = new BlowHole( 20 );
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)hole;
    // default
    hole->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: BlowHole_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( BlowHole_dtor )
{
    delete (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: BlowHole_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BlowHole_tick )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = p->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BlowHole_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BlowHole_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_startBlowing )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->startBlowing ( f );
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_stopBlowing )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_rate )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)p->m_rate;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_rate )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_rate;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_tonehole()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_tonehole )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setTonehole( f );
    RETURN->v_float = (t_CKFLOAT)p->m_tonehole;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_tonehole()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_tonehole )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_tonehole;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_vent()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_vent )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setVent( f );
    RETURN->v_float = (t_CKFLOAT)p->m_vent;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_vent()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_vent )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_vent;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_reed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_reed )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange(__SK_ReedStiffness_, f * 128.0);
    RETURN->v_float = (t_CKFLOAT)p->m_reed;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_reed()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_reed )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_reed;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_noiseGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_noiseGain )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 4, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_noiseGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_noiseGain )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: BlowHole_ctrl_pressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( BlowHole_ctrl_pressure )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 128, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_pressure;
}


//-----------------------------------------------------------------------------
// name: BlowHole_cget_pressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( BlowHole_cget_pressure )
{
    BlowHole * p = (BlowHole *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_pressure;
}


// Bowed
//-----------------------------------------------------------------------------
// name: Bowed_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Bowed_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT) new Bowed(40.0);
}


//-----------------------------------------------------------------------------
// name: Bowed_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Bowed_dtor )
{
    delete (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Bowed_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Bowed_tick )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = p->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Bowed_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Bowed_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_startBowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_startBowing )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->startBowing( f );
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_stopBowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_stopBowing )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->stopBowing( f );
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_bowPressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_bowPressure )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 2, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_bowPressure;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_bowPressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_bowPressure )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_bowPressure;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_bowPos()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_bowPos )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 4, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_bowPosition;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_bowPos()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_bowPos )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_bowPosition;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_vibratoFreq )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setVibratoFreq( f );
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_vibratoFreq )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_vibratoGain )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 1, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_vibratoGain )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_volume()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_volume )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->controlChange( 128, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)p->m_volume;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_volume()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_volume )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)p->m_volume;
}


//-----------------------------------------------------------------------------
// name: Bowed_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Bowed_ctrl_rate )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->m_rate = f;
    RETURN->v_float = (t_CKFLOAT) p->m_rate;
}


//-----------------------------------------------------------------------------
// name: Bowed_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Bowed_cget_rate )
{
    Bowed * p = (Bowed *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) p->m_rate;
}


// Chorus
//-----------------------------------------------------------------------------
// name: Chorus_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Chorus_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Chorus_offset_data) = (t_CKUINT) new Chorus( 3000 );
}


//-----------------------------------------------------------------------------
// name: Chorus_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Chorus_dtor )
{
    delete (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    OBJ_MEMBER_UINT(SELF, Chorus_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Chorus_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Chorus_tick )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    *out = p->tick(in);
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Chorus_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Chorus_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Chorus_ctrl_mix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Chorus_ctrl_mix )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setEffectMix( f );
    RETURN->v_float = (t_CKFLOAT) p->effectMix;
}


//-----------------------------------------------------------------------------
// name: Chorus_ctrl_modDepth()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Chorus_ctrl_modDepth )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setModDepth( f );
    RETURN->v_float = (t_CKFLOAT) p->modDepth;
}


//-----------------------------------------------------------------------------
// name: Chorus_ctrl_modFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Chorus_ctrl_modFreq )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setModFrequency( f );
    RETURN->v_float = (t_CKFLOAT) p->mods[0]->m_freq;
}


//-----------------------------------------------------------------------------
// name: Chorus_ctrl_baseDelay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Chorus_ctrl_baseDelay )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    t_CKDUR f = GET_NEXT_DUR(ARGS);
    p->setDelay( f );
    RETURN->v_float = (t_CKFLOAT) p->baseLength;
}


//-----------------------------------------------------------------------------
// name: Chorus_cget_mix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Chorus_cget_mix )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    RETURN->v_float = (t_CKFLOAT) p->effectMix;
}


//-----------------------------------------------------------------------------
// name: Chorus_cget_baseDelay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Chorus_cget_baseDelay )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    RETURN->v_dur = (t_CKFLOAT)p->baseLength;
}


//-----------------------------------------------------------------------------
// name: Chorus_cget_modDepth()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Chorus_cget_modDepth )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    RETURN->v_float = (t_CKFLOAT) p->modDepth;
}


//-----------------------------------------------------------------------------
// name: Chorus_cget_modFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Chorus_cget_modFreq )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    RETURN->v_float = (t_CKFLOAT) p->mods[0]->m_freq;
}


//-----------------------------------------------------------------------------
// name: Chorus_ctrl_set()
// desc: set ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Chorus_ctrl_set )
{
    Chorus * p = (Chorus *)OBJ_MEMBER_UINT(SELF, Chorus_offset_data);
    t_CKDUR d = GET_NEXT_DUR(ARGS);
    t_CKFLOAT dd = GET_NEXT_FLOAT(ARGS);
    p->set( d, dd );
}


// Brass
/*
struct Brass_
{
   Brass * imp;

   t_CKFLOAT m_frequency;
   t_CKFLOAT m_rate;
   t_CKFLOAT m_lip;

   Brass_( t_CKFLOAT d )
   { 
      imp = new Brass(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_lip = 0.1;        
   }

   ~Brass_()
   {
       SAFE_DELETE( imp );
   }
}; */


//-----------------------------------------------------------------------------
// name: Brass_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Brass_ctor )
{
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new Brass( 20.0 );
}


//-----------------------------------------------------------------------------
// name: Brass_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Brass_dtor )
{
    delete (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Brass_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Brass_tick )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Brass_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Brass_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_startBlowing )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->startBlowing( f, b->m_rate );
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_stopBlowing )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_clear )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_rate )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_rate )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_lip()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_lip )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 2, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_lip;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_lip()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_lip )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_lip;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_slide()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_slide )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 4, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_slide;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_slide()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_slide )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_slide;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_vibratoFreq )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setVibratoFreq( f );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_vibratoFreq )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_vibratoGain )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 1, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_vibratoGain)
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Brass_ctrl_volume()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Brass_ctrl_volume )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 128, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_volume;
}


//-----------------------------------------------------------------------------
// name: Brass_cget_volume()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Brass_cget_volume )
{
    Brass * b = (Brass *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_volume;
}




/*
struct Clarinet_ { 
   Clarinet * imp;
   double m_frequency;
   double m_rate;
   double m_reed;
   Clarinet_ ( double d ) { 
      imp = new Clarinet(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_reed = 0.5;
   }
}; */


// Clarinet
//-----------------------------------------------------------------------------
// name: Clarinet_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Clarinet_ctor )
{
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new Clarinet( 30.0 );
}


//-----------------------------------------------------------------------------
// name: Clarinet_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Clarinet_dtor )
{
    delete (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}

//-----------------------------------------------------------------------------
// name: Clarinet_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Clarinet_tick )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Clarinet_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Clarinet_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_startBlowing )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->startBlowing( f, b->m_rate );
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_stopBlowing )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_clear )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_reed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_reed )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 2, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_reed;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_reed()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_reed )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_reed;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_rate )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_rate )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_noiseGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_noiseGain )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 4, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_noiseGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_noiseGain )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_vibratoFreq )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setVibratoFreq( f );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_vibratoFreq )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_vibratoGain )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 1, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_vibratoGain )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Clarinet_ctrl_pressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Clarinet_ctrl_pressure )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 128, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_volume;
}


//-----------------------------------------------------------------------------
// name: Clarinet_cget_pressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Clarinet_cget_pressure )
{
    Clarinet * b = (Clarinet *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_volume;
}


// Flute
struct Flute_ { 
   Flute * imp;

   double m_frequency;
   double m_rate;
   double m_jetDelay;
   double m_jetReflection;
   double m_endReflection;

   Flute_ ( double d ) { 
      imp = new Flute(d);
      m_frequency = 100.0;
      m_rate = 0.5;
      m_jetDelay = 0.0;
      m_jetReflection = 0.0;
      m_endReflection = 0.0;
   }
};


//-----------------------------------------------------------------------------
// name: Flute_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Flute_ctor )
{
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT) new Flute( 40.0 );
}


//-----------------------------------------------------------------------------
// name: Flute_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Flute_dtor )
{
    Flute * f = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    SAFE_DELETE(f);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Flute_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Flute_tick )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}

//-----------------------------------------------------------------------------
// name: Flute_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Flute_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_startBlowing )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->startBlowing( f, b->m_rate );
}

//-----------------------------------------------------------------------------
// name: Flute_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_stopBlowing )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_clear )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_rate )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_rate )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_jetDelay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_jetDelay )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setJetDelay( f);
    RETURN->v_float = (t_CKFLOAT)b->m_jetDelay;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_jetDelay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_jetDelay )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_jetDelay;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_jetReflection()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_jetReflection )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setJetReflection( f);
    RETURN->v_float = (t_CKFLOAT)b->m_jetReflection;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_jetReflection()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_jetReflection )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_jetReflection;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_endReflection()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_endReflection )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setEndReflection(f);
    RETURN->v_float = (t_CKFLOAT)b->m_endReflection;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_endReflection()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_endReflection )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_endReflection;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_noiseGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_noiseGain )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 4, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_noiseGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_noiseGain )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_vibratoFreq )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setVibratoFreq( f );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_vibratoFreq )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_vibratoGain )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 1, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_vibratoGain )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Flute_ctrl_pressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flute_ctrl_pressure )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 128, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)b->m_pressure;
}


//-----------------------------------------------------------------------------
// name: Flute_cget_pressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flute_cget_pressure )
{
    Flute * b = (Flute *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_pressure;
}


// ModalBar
struct ModalBar_ { 
   ModalBar modalbar;

   int    m_preset;
   int    m_modeIndex;
   double m_modeRatio;
   double m_modeRadius;
   
   ModalBar_ ( ) { 
      m_preset = 0;
      m_modeIndex = 0;
   }
};


//-----------------------------------------------------------------------------
// name: ModalBar_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( ModalBar_ctor )
{
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new ModalBar_();
}


//-----------------------------------------------------------------------------
// name: ModalBar_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( ModalBar_dtor )
{
    delete (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: ModalBar_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( ModalBar_tick )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->modalbar.tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ModalBar_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( ModalBar_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_strike()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_strike )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.strike( f );
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_damp()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_damp )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.damp( f );
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_clear )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->modalbar.clear();
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_preset()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_preset )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    int f = GET_CK_INT(ARGS);
    b->m_preset = f;
    b->modalbar.setPreset ( f );
    RETURN->v_int = (t_CKINT)b->m_preset;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_preset()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_preset )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_int = (t_CKINT)b->m_preset;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_strikePosition()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_strikePosition )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.setStrikePosition( f );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.strikePosition;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_strikePosition()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_strikePosition )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.strikePosition;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_vibratoGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.controlChange( 11, f*128 );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_vibratoGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_vibratoFreq )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.vibrato->setFrequency( f );
    b->modalbar.m_vibratoFreq = b->modalbar.vibrato->m_freq;
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_vibratoFreq )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_stickHardness()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_stickHardness )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.setStickHardness ( f );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.stickHardness;
}



//-----------------------------------------------------------------------------
// name: ModalBar_cget_stickHardness()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_stickHardness )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.stickHardness;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_masterGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_masterGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.setMasterGain( f );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.masterGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_masterGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_masterGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.masterGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_directGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_directGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.setDirectGain( f );
    RETURN->v_float = (t_CKFLOAT) b->modalbar.directGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_directGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_directGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.directGain;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_mode()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_mode )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    int i = GET_CK_INT(ARGS);
    if ( i >= 0 && i < b->modalbar.nModes )
    { 
        b->m_modeIndex = i;
        b->m_modeRatio = b->modalbar.ratios[i];
        b->m_modeRadius = b->modalbar.radii[i];
        RETURN->v_float = (t_CKFLOAT)b->m_modeIndex;
    } 
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_mode()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_mode )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_modeIndex;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_modeGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_modeGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.setModeGain( b->m_modeIndex, f );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.filters[b->m_modeIndex]->getGain();
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_modeGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_modeGain )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.filters[b->m_modeIndex]->getGain();
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_modeRatio()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_modeRatio )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    if ( b->m_modeIndex >= 0 && b->m_modeIndex < b->modalbar.nModes )
    {
        b->modalbar.setRatioAndRadius( b->m_modeIndex, f , b->m_modeRadius );
        b->m_modeRatio = b->modalbar.ratios[b->m_modeIndex];
        RETURN->v_float = (t_CKFLOAT) b->m_modeRatio;
    }
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_modeRatio()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_modeRatio )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_modeRatio;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_modeRadius()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_modeRadius )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    if( b->m_modeIndex >= 0 && b->m_modeIndex < b->modalbar.nModes )
    {
        b->modalbar.setRatioAndRadius ( b->m_modeIndex, b->m_modeRatio, f );
        b->m_modeRadius = b->modalbar.radii[b->m_modeIndex];
        RETURN->v_float = (t_CKFLOAT) b->m_modeRadius;
    }
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_modeRadius()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_modeRadius )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_modeRadius;
}


//-----------------------------------------------------------------------------
// name: ModalBar_ctrl_volume()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ModalBar_ctrl_volume )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->modalbar.controlChange( 128, f*128 );
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_volume;
}


//-----------------------------------------------------------------------------
// name: ModalBar_cget_volume()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ModalBar_cget_volume )
{
    ModalBar_ * b = (ModalBar_ *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->modalbar.m_volume;
}




// Sitar
/*
struct Sitar_ { 
   Sitar * imp;
   double m_frequency;

   Sitar_ ( double d ) { 
      imp = new Sitar(d);
      m_frequency = 100.0;
   }
};*/


//-----------------------------------------------------------------------------
// name: Sitar_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Sitar_ctor )
{
    Sitar * sitar = new Sitar( 20 );
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)sitar;
    // default
    sitar->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: Sitar_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Sitar_dtor )
{
    Sitar * s = (Sitar *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    SAFE_DELETE(s);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Sitar_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Sitar_tick )
{
    Sitar * b = (Sitar *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Sitar_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Sitar_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Sitar_ctrl_pluck()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Sitar_ctrl_pluck )
{
    Sitar * b = (Sitar *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->pluck( f );
}


//-----------------------------------------------------------------------------
// name: Sitar_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Sitar_ctrl_clear )
{
    Sitar * b = (Sitar *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}




//-----------------------------------------------------------------------------
// name: Saxofony_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Saxofony_ctor )
{
    Saxofony * fony = new Saxofony( 20 );
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)fony;
    // default
    fony->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: Saxofony_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Saxofony_dtor )
{
    Saxofony * d = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    SAFE_DELETE(d);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Saxofony_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Saxofony_tick )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Saxofony_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Saxofony_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_startBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_startBlowing )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->startBlowing( f, b->m_rate );
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_stopBlowing()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_stopBlowing )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->stopBlowing( f );
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_clear )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_rate )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->m_rate = f;
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_rate )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_rate;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_blowPosition()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_blowPosition )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setBlowPosition(f);
    RETURN->v_float = (t_CKFLOAT)b->position;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_blowPosition()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_blowPosition )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->position;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_reed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_reed )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 2, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_stiffness;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_reed()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_reed )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_stiffness;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_aperture()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_aperture )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 26, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_aperture;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_aperture()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_aperture )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_aperture;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_noiseGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_noiseGain )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 4, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_noiseGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_noiseGain )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_noiseGain;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_vibratoFreq )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->vibrato->setFrequency( f );
    RETURN->v_float = (t_CKFLOAT)b->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_vibratoFreq )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_vibratoGain )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 1, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_vibratoGain )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Saxofony_ctrl_pressure()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Saxofony_ctrl_pressure )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( 128, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_pressure;
}


//-----------------------------------------------------------------------------
// name: Saxofony_cget_pressure()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Saxofony_cget_pressure )
{
    Saxofony * b = (Saxofony *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_pressure;
}




// StifKarp

//-----------------------------------------------------------------------------
// name: StifKarp_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( StifKarp_ctor )
{
    // initialize member object
    StifKarp * karp = new StifKarp( 20.0 );
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)karp;
    // default
    karp->setFrequency( 220.0 );
}


//-----------------------------------------------------------------------------
// name: StifKarp_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( StifKarp_dtor )
{
    delete (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: StifKarp_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( StifKarp_tick )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = b->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: StifKarp_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( StifKarp_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_pluck()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_pluck )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->pluck( f );
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_clear()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_clear )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    b->clear();
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_pickupPosition()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_pickupPosition )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setPickupPosition( f );
    RETURN->v_float = (t_CKFLOAT)b->pickupPosition;
}


//-----------------------------------------------------------------------------
// name: StifKarp_cget_pickupPosition()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( StifKarp_cget_pickupPosition )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->pickupPosition;
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_stretch()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_stretch )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setStretch( f );
    RETURN->v_float = (t_CKFLOAT)b->stretching;
}


//-----------------------------------------------------------------------------
// name: StifKarp_cget_stretch()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( StifKarp_cget_stretch )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->stretching;
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_sustain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_sustain )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->controlChange( __SK_StringDamping_, f * 128 );
    RETURN->v_float = (t_CKFLOAT)b->m_sustain;
}


//-----------------------------------------------------------------------------
// name: StifKarp_cget_sustain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( StifKarp_cget_sustain )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->m_sustain;
}


//-----------------------------------------------------------------------------
// name: StifKarp_ctrl_baseLoopGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( StifKarp_ctrl_baseLoopGain )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    b->setBaseLoopGain( f );
    RETURN->v_float = (t_CKFLOAT)b->baseLoopGain;
}


//-----------------------------------------------------------------------------
// name: StifKarp_cget_baseLoopGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( StifKarp_cget_baseLoopGain )
{
    StifKarp * b = (StifKarp *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)b->baseLoopGain;
}



// Delay
//-----------------------------------------------------------------------------
// name: Delay_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Delay_ctor )
{
    OBJ_MEMBER_UINT(SELF, Delay_offset_data) = (t_CKUINT)new Delay;
}


//-----------------------------------------------------------------------------
// name: Delay_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Delay_dtor )
{
    delete (Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data);
    OBJ_MEMBER_UINT(SELF, Delay_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Delay_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Delay_tick )
{
    *out = (SAMPLE)((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Delay_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Delay_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: Delay_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Delay_ctrl_delay )
{
    ((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->setDelay( (long)(GET_NEXT_DUR(ARGS)+.5) );
    RETURN->v_dur = (t_CKDUR)((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: Delay_cget_delay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Delay_cget_delay )
{
    RETURN->v_dur = (t_CKDUR)((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: Delay_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Delay_ctrl_max )
{
    Delay * delay = (Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data);
    t_CKFLOAT val = (t_CKFLOAT)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(ARGS);
    delay->set( (long)(val+.5), (long)(max+1.5) );
    RETURN->v_dur = (t_CKDUR)((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->length-1.0;
}


//-----------------------------------------------------------------------------
// name: Delay_cget_max()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Delay_cget_max )
{
    RETURN->v_dur = (t_CKDUR)((Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data))->length-1.0;
}


//-----------------------------------------------------------------------------
// name: Delay_clear()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Delay_clear )
{
    Delay * delay = (Delay *)OBJ_MEMBER_UINT(SELF, Delay_offset_data);
    delay->clear();
}




// DelayA
//-----------------------------------------------------------------------------
// name: DelayA_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( DelayA_ctor )
{
    OBJ_MEMBER_UINT(SELF, DelayA_offset_data) = (t_CKUINT)new DelayA;
}


//-----------------------------------------------------------------------------
// name: DelayA_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( DelayA_dtor )
{
    delete (DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data);
    OBJ_MEMBER_UINT(SELF, DelayA_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: DelayA_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( DelayA_tick )
{
    *out = (SAMPLE)((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: DelayA_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( DelayA_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: DelayA_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DelayA_ctrl_delay )
{
    ((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->setDelay( GET_NEXT_DUR(ARGS) );
    RETURN->v_dur = (t_CKDUR)((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: DelayA_cget_delay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayA_cget_delay )
{
    RETURN->v_dur = (t_CKDUR)((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: DelayA_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DelayA_ctrl_max )
{
    DelayA * delay = (DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data);
    t_CKDUR val = (t_CKDUR)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(ARGS);
    delay->set( val, (long)(max+1.5) );
    RETURN->v_dur = (t_CKDUR)((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->length-1.0;
}


//-----------------------------------------------------------------------------
// name: DelayA_cget_max()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayA_cget_max )
{
    RETURN->v_dur = (t_CKDUR)((DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data))->length-1.0;
}


//-----------------------------------------------------------------------------
// name: DelayA_clear()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayA_clear )
{
    DelayA * delay = (DelayA *)OBJ_MEMBER_UINT(SELF, DelayA_offset_data);
    delay->clear();
}




// DelayL
//-----------------------------------------------------------------------------
// name: DelayL_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( DelayL_ctor )
{
    OBJ_MEMBER_UINT(SELF, DelayL_offset_data) = (t_CKUINT)new DelayL;
}


//-----------------------------------------------------------------------------
// name: DelayL_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( DelayL_dtor )
{
    delete (DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data);
    OBJ_MEMBER_UINT(SELF, DelayL_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: DelayL_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( DelayL_tick )
{
    *out = (SAMPLE)((DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data))->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: DelayL_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( DelayL_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: DelayL_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DelayL_ctrl_delay )
{
    ((DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data))->setDelay( GET_NEXT_DUR(ARGS) );
    RETURN->v_dur = (t_CKDUR)((DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: DelayL_cget_delay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayL_cget_delay )
{
    RETURN->v_dur = (t_CKDUR)((DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data))->getDelay();
}


//-----------------------------------------------------------------------------
// name: DelayL_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DelayL_ctrl_max )
{
    DelayL * delay = (DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data);
    t_CKDUR val = (t_CKDUR)delay->getDelay();
    t_CKDUR max = GET_NEXT_DUR(ARGS);
    delay->set( val, (long)(max+1.5) );
    RETURN->v_dur = (t_CKDUR)delay->length-1.0;
}


//-----------------------------------------------------------------------------
// name: DelayL_cget_max()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayL_cget_max )
{
    RETURN->v_dur = (t_CKDUR)((DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data))->length-1.0;
}


//-----------------------------------------------------------------------------
// name: DelayL_clear()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( DelayL_clear )
{
    DelayL * delay = (DelayL *)OBJ_MEMBER_UINT(SELF, DelayL_offset_data);
    delay->clear();
}




// Echo
//-----------------------------------------------------------------------------
// name: Echo_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Echo_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Echo_offset_data) = (t_CKUINT)new Echo( Stk::sampleRate() / 2.0 );
}


//-----------------------------------------------------------------------------
// name: Echo_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Echo_dtor )
{
    delete (Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data);
    OBJ_MEMBER_UINT(SELF, Echo_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Echo_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Echo_tick )
{
    *out = (SAMPLE)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Echo_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Echo_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: Echo_ctrl_delay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Echo_ctrl_delay )
{
    ((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->setDelay( GET_NEXT_DUR(ARGS) );
    RETURN->v_dur = (t_CKDUR)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->getDelay(); 
}


//-----------------------------------------------------------------------------
// name: Echo_cget_delay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Echo_cget_delay )
{
    RETURN->v_dur = (t_CKDUR)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->getDelay(); 
}


//-----------------------------------------------------------------------------
// name: Echo_ctrl_max()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Echo_ctrl_max )
{
    ((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->set( GET_NEXT_DUR(ARGS) );
    RETURN->v_dur = (t_CKDUR)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->length;
}


//-----------------------------------------------------------------------------
// name: Echo_cget_max()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Echo_cget_max )
{
    RETURN->v_dur = (t_CKDUR)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->length;
}


//-----------------------------------------------------------------------------
// name: Echo_ctrl_mix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Echo_ctrl_mix )
{
    ((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->setEffectMix( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->effectMix;
}


//-----------------------------------------------------------------------------
// name: Echo_cget_mix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Echo_cget_mix )
{
    RETURN->v_float = (t_CKFLOAT)((Echo *)OBJ_MEMBER_UINT(SELF, Echo_offset_data))->effectMix;
}


//-----------------------------------------------------------------------------
// name: Envelope - import
// desc: ..
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// name: Envelope_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Envelope_ctor )
{
    OBJ_MEMBER_UINT(SELF, Envelope_offset_data) = (t_CKUINT)new Envelope;
}


//-----------------------------------------------------------------------------
// name: Envelope_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Envelope_dtor )
{
    delete (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    OBJ_MEMBER_UINT(SELF, Envelope_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Envelope_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Envelope_tick )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    *out = in * d->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Envelope_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Envelope_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_time()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_time )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setTime( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = d->m_time;
}


//-----------------------------------------------------------------------------
// name: Envelope_cget_time()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Envelope_cget_time )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = d->m_time;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_duration()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_duration )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setTime( GET_NEXT_FLOAT(ARGS) / Stk::sampleRate() );
    RETURN->v_float = d->m_time * Stk::sampleRate();
}


//-----------------------------------------------------------------------------
// name: Envelope_cget_duration()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Envelope_cget_duration )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = d->m_time * Stk::sampleRate();
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_rate )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setRate( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->rate;
}


//-----------------------------------------------------------------------------
// name: Envelope_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Envelope_cget_rate )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->rate;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_target()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_target )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setTarget( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->target;
}


//-----------------------------------------------------------------------------
// name: Envelope_cget_target()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Envelope_cget_target )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->target;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_value()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_value )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setValue( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->value;
}


//-----------------------------------------------------------------------------
// name: Envelope_cget_value()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Envelope_cget_value )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->value;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_keyOn0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_keyOn0 )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->keyOn();
    RETURN->v_int = 1;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_keyOn()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_keyOn )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    if( GET_NEXT_INT(ARGS) )
    {
        d->keyOn();
        RETURN->v_int = 1;
    }
    else
    {
        d->keyOff();
        RETURN->v_int = 0;
    }
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_keyOff0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_keyOff0 )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->keyOff();
    RETURN->v_int = 1;
}


//-----------------------------------------------------------------------------
// name: Envelope_ctrl_keyOff()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Envelope_ctrl_keyOff )
{
    Envelope * d = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    if( !GET_NEXT_INT(ARGS) )
    {
        d->keyOn();
        RETURN->v_int = 0;
    }
    else
    {
        d->keyOff();
        RETURN->v_int = 1;
    }
}


//-----------------------------------------------------------------------------
// name: ADSR - import
// desc: ..
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// name: ADSR_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( ADSR_ctor )
{
    // TODO: fix this horrid thing
    Envelope * e = (Envelope *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    SAFE_DELETE(e);

    OBJ_MEMBER_UINT(SELF, Envelope_offset_data) = (t_CKUINT)new ADSR;
}


//-----------------------------------------------------------------------------
// name: ADSR_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( ADSR_dtor )
{
    delete (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    OBJ_MEMBER_UINT(SELF, Envelope_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: ADSR_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( ADSR_tick )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    *out = in * d->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ADSR_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( ADSR_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_attackTime()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_attackTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    t_CKDUR t = GET_NEXT_DUR(ARGS);
    d->setAttackTime( t / Stk::sampleRate() );
    RETURN->v_dur = t;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_attackTime()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_attackTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_dur = d->getAttackTime() * Stk::sampleRate();
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_attackRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_attackRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setAttackRate( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->attackRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_attackRate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_attackRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->attackRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_decayTime()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_decayTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    t_CKDUR t = GET_NEXT_DUR(ARGS);
    d->setDecayTime( t / Stk::sampleRate() );
    RETURN->v_dur = t;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_decayTime()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_decayTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_dur = d->getDecayTime() * Stk::sampleRate();
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_decayRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_decayRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setDecayRate( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->decayRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_decayRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_decayRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->decayRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_sustainLevel()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_sustainLevel )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setSustainLevel( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->sustainLevel;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_sustainLevel()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_sustainLevel )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->sustainLevel;
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_releaseTime()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_releaseTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    t_CKDUR t = GET_NEXT_DUR(ARGS);
    d->setReleaseTime( t / Stk::sampleRate() );
    RETURN->v_dur = t;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_releaseTime()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_releaseTime )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_dur = d->getReleaseTime() * Stk::sampleRate();
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_releaseRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_releaseRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    d->setReleaseRate( GET_NEXT_FLOAT(ARGS) );
    RETURN->v_float = (t_CKFLOAT)d->releaseRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_releaseRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_releaseRate )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->releaseRate;
}


//-----------------------------------------------------------------------------
// name: ADSR_cget_state()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( ADSR_cget_state )
{
    ADSR * d = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    RETURN->v_int = (t_CKINT) d->state;
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_set()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_set )
{
    ADSR * e = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    t_CKFLOAT a = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT d = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT s = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    e->setAttackTime( a );
    e->setDecayTime( d );
    e->setSustainLevel( s );
    e->setReleaseTime( r );
}


//-----------------------------------------------------------------------------
// name: ADSR_ctrl_set2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( ADSR_ctrl_set2 )
{
    ADSR * e = (ADSR *)OBJ_MEMBER_UINT(SELF, Envelope_offset_data);
    t_CKDUR a = GET_NEXT_DUR(ARGS);
    t_CKDUR d = GET_NEXT_DUR(ARGS);
    t_CKFLOAT s = GET_NEXT_FLOAT(ARGS);
    t_CKDUR r = GET_NEXT_DUR(ARGS);
    e->setAttackTime( a / Stk::sampleRate() );
    e->setDecayTime( d / Stk::sampleRate() );
    e->setSustainLevel( s );
    e->setReleaseTime( r / Stk::sampleRate() );
}




// FilterStk
//-----------------------------------------------------------------------------
// name: FilterStk_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FilterStk_ctor )
{
    OBJ_MEMBER_UINT(SELF, FilterStk_offset_data) = (t_CKUINT)new FilterStk;
}


//-----------------------------------------------------------------------------
// name: FilterStk_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FilterStk_dtor )
{
    delete (FilterStk *)OBJ_MEMBER_UINT(SELF, FilterStk_offset_data);
    OBJ_MEMBER_UINT(SELF, FilterStk_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: FilterStk_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( FilterStk_tick )
{
    FilterStk * d = (FilterStk *)OBJ_MEMBER_UINT(SELF, FilterStk_offset_data);
    *out = d->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FilterStk_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FilterStk_pmsg )
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: FilterStk_ctrl_coefs()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FilterStk_ctrl_coefs )
{
    // FilterStk * d = (FilterStk *)OBJ_MEMBER_UINT(SELF, FilterStk_offset_data);
    CK_FPRINTF_STDERR( "FilterStk.coefs :: not implemented\n" );
}


//-----------------------------------------------------------------------------
// name: OnePole_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( OnePole_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, OnePole_offset_data) = (t_CKUINT)new OnePole();
}


//-----------------------------------------------------------------------------
// name: OnePole_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( OnePole_dtor  )
{ 
    delete (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    OBJ_MEMBER_UINT(SELF, OnePole_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: OnePole_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( OnePole_tick )
{
    OnePole * m = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    *out = m->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: OnePole_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( OnePole_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: OnePole_ctrl_a1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OnePole_ctrl_a1 )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setA1( f );
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: OnePole_cget_a1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OnePole_cget_a1 )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: OnePole_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OnePole_ctrl_b0 )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB0( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: OnePole_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OnePole_cget_b0 )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: OnePole_ctrl_pole()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OnePole_ctrl_pole )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setPole( f );
    RETURN->v_float = (t_CKFLOAT) -filter->a[1];
}


//-----------------------------------------------------------------------------
// name: OnePole_cget_pole()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OnePole_cget_pole )
{
    OnePole * filter = (OnePole *)OBJ_MEMBER_UINT(SELF, OnePole_offset_data);
    RETURN->v_float = (t_CKFLOAT) -filter->a[1];
}



//TwoPole functions

//-----------------------------------------------------------------------------
// name: TwoPole_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( TwoPole_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, TwoPole_offset_data) = (t_CKUINT)new TwoPole();
}


//-----------------------------------------------------------------------------
// name: TwoPole_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( TwoPole_dtor  )
{ 
    delete (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    OBJ_MEMBER_UINT(SELF, TwoPole_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: TwoPole_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( TwoPole_tick )
{
    TwoPole * m = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    *out = m->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: TwoPole_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( TwoPole_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_a1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_a1 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setA1( f );
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_a1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_a1 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_a2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_a2 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setA2( f );
    RETURN->v_float = (t_CKFLOAT) filter->a[2];
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_a2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_a2 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->a[2];
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_b0 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB0( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_b0 )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_freq )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->ck_setResFreq( f );
    RETURN->v_float = (t_CKFLOAT) filter->m_resFreq;
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_freq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_freq )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->m_resFreq;
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_radius()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_radius )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->ck_setResRad( f );
    RETURN->v_float = (t_CKFLOAT) filter->m_resRad;
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_radius()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_radius )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->m_resRad;
}


//-----------------------------------------------------------------------------
// name: TwoPole_ctrl_norm()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoPole_ctrl_norm )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    bool b = ( GET_CK_INT(ARGS) != 0 ); 
    filter->ck_setResNorm( b );
    RETURN->v_int = (t_CKINT) filter->m_resNorm;
}


//-----------------------------------------------------------------------------
// name: TwoPole_cget_norm()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoPole_cget_norm )
{
    TwoPole * filter = (TwoPole *)OBJ_MEMBER_UINT(SELF, TwoPole_offset_data);
    RETURN->v_int = (t_CKINT) filter->m_resNorm;
}




//OneZero functions

//-----------------------------------------------------------------------------
// name: OneZero_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( OneZero_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, OneZero_offset_data) = (t_CKUINT) new OneZero();
}


//-----------------------------------------------------------------------------
// name: OneZero_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( OneZero_dtor )
{ 
    delete (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    OBJ_MEMBER_UINT(SELF, OneZero_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: OneZero_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( OneZero_tick )
{
    OneZero * m = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    *out = m->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: OneZero_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( OneZero_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: OneZero_ctrl_zero()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OneZero_ctrl_zero )
{
    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setZero( f );
    double zeeroo = ( filter->b[0] == 0 ) ? 0 : -filter->b[1] / filter->b[0]; 
    RETURN->v_float = (t_CKFLOAT) zeeroo; 
}


//-----------------------------------------------------------------------------
// name: OneZero_cget_zero()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OneZero_cget_zero )
{
    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    double zeeroo = ( filter->b[0] == 0 ) ? 0 : -filter->b[1] / filter->b[0]; 
    RETURN->v_float = (t_CKFLOAT)zeeroo; 
}


//-----------------------------------------------------------------------------
// name: OneZero_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OneZero_ctrl_b0 )
{
    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB0( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: OneZero_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OneZero_cget_b0 )
{
    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: OneZero_ctrl_b1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( OneZero_ctrl_b1 )
{
    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB1( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[1];
}


//-----------------------------------------------------------------------------
// name: OneZero_cget_b1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( OneZero_cget_b1 )
{

    OneZero * filter = (OneZero *)OBJ_MEMBER_UINT(SELF, OneZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[1];
}


//TwoZero functions

//-----------------------------------------------------------------------------
// name: TwoZero_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( TwoZero_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, TwoZero_offset_data) = (t_CKUINT)new TwoZero();
}


//-----------------------------------------------------------------------------
// name: TwoZero_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( TwoZero_dtor )
{ 
    delete (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    OBJ_MEMBER_UINT(SELF, TwoZero_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: TwoZero_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( TwoZero_tick )
{
    TwoZero * m = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    *out = m->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: TwoZero_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( TwoZero_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: TwoZero_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoZero_ctrl_b0 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB0( f );
}


//-----------------------------------------------------------------------------
// name: TwoZero_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoZero_cget_b0 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: TwoZero_ctrl_b1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoZero_ctrl_b1 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB1( f );
}

//-----------------------------------------------------------------------------
// name: TwoZero_cget_b1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoZero_cget_b1 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[1];
}

//-----------------------------------------------------------------------------
// name: TwoZero_ctrl_b2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoZero_ctrl_b2 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB2( f );
}


//-----------------------------------------------------------------------------
// name: TwoZero_cget_b2()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoZero_cget_b2 )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[2];
}


//-----------------------------------------------------------------------------
// name: TwoZero_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoZero_ctrl_freq )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->ck_setNotchFreq( f );
}


//-----------------------------------------------------------------------------
// name: TwoZero_cget_freq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoZero_cget_freq )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->m_notchFreq;
}


//-----------------------------------------------------------------------------
// name: TwoZero_ctrl_radius()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( TwoZero_ctrl_radius )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->ck_setNotchRad( f );
}

//-----------------------------------------------------------------------------
// name: TwoZero_cget_radius()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( TwoZero_cget_radius )
{
    TwoZero * filter = (TwoZero *)OBJ_MEMBER_UINT(SELF, TwoZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->m_notchRad;
}



//PoleZero functions

//-----------------------------------------------------------------------------
// name: PoleZero_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( PoleZero_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, PoleZero_offset_data) = (t_CKUINT) new PoleZero();
}


//-----------------------------------------------------------------------------
// name: PoleZero_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( PoleZero_dtor )
{
    delete (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    OBJ_MEMBER_UINT(SELF, PoleZero_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: PoleZero_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( PoleZero_tick )
{
    PoleZero * m = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    *out = m->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PoleZero_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( PoleZero_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PoleZero_ctrl_a1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PoleZero_ctrl_a1 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setA1( f );
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: PoleZero_cget_a1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PoleZero_cget_a1 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->a[1];
}


//-----------------------------------------------------------------------------
// name: PoleZero_ctrl_b0()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PoleZero_ctrl_b0 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB0( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: PoleZero_cget_b0()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PoleZero_cget_b0 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}



//-----------------------------------------------------------------------------
// name: PoleZero_ctrl_b1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PoleZero_ctrl_b1 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setB1( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[1];
}


//-----------------------------------------------------------------------------
// name: PoleZero_cget_b1()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PoleZero_cget_b1 )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[1];
}


//-----------------------------------------------------------------------------
// name: PoleZero_ctrl_allpass()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PoleZero_ctrl_allpass )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setAllpass( f );
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: PoleZero_cget_allpass()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PoleZero_cget_allpass )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) filter->b[0];
}


//-----------------------------------------------------------------------------
// name: PoleZero_ctrl_blockZero()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PoleZero_ctrl_blockZero )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    filter->setBlockZero( f );
    RETURN->v_float = (t_CKFLOAT) -filter->a[1];
}


//-----------------------------------------------------------------------------
// name: PoleZero_cget_blockZero()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PoleZero_cget_blockZero )
{
    PoleZero * filter = (PoleZero *)OBJ_MEMBER_UINT(SELF, PoleZero_offset_data);
    RETURN->v_float = (t_CKFLOAT) -filter->a[1];
}




// FM functions

//-----------------------------------------------------------------------------
// name: FM_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FM_ctor  )
{
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
    // CK_FPRINTF_STDERR( "[chuck](via STK): error -- FM is virtual!\n" );
}


//-----------------------------------------------------------------------------
// name: FM_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FM_dtor  )
{ 
    // delete (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    // CK_FPRINTF_STDERR( "error -- FM is virtual!\n" );
}


//-----------------------------------------------------------------------------
// name: FM_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( FM_tick )
{
    FM * m = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    CK_FPRINTF_STDERR( "[chuck](via STK): error -- FM tick is virtual!\n" );
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FM_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FM_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_modDepth()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_ctrl_modDepth )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    fm->setModulationDepth( f );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: FM_cget_modDepth()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_cget_modDepth )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    RETURN->v_float = fm->modDepth;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_modSpeed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_ctrl_modSpeed )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    fm->setModulationSpeed( f );
    RETURN->v_float = fm->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_modSpeed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_cget_modSpeed )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    RETURN->v_float = fm->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_control1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_ctrl_control1 )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    fm->setControl1( f );
    RETURN->v_float = fm->control1 / 2.0;
}


//-----------------------------------------------------------------------------
// name: FM_cget_control1()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_cget_control1 )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    RETURN->v_float = fm->control1 / 2.0;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_control2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_ctrl_control2 )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    fm->setControl2( f );
    RETURN->v_float = fm->control2 / 2.0;
}


//-----------------------------------------------------------------------------
// name: FM_cget_control2()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_cget_control2 )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    RETURN->v_float = fm->control2 / 2.0;
}


//-----------------------------------------------------------------------------
// name: FM_ctrl_afterTouch()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_ctrl_afterTouch )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    fm->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
    RETURN->v_float = fm->adsr[1]->target;
}


//-----------------------------------------------------------------------------
// name: FM_cget_afterTouch()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FM_cget_afterTouch )
{
    FM * fm = (FM *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    RETURN->v_float = fm->adsr[1]->target;
}




//-----------------------------------------------------------------------------
// name: BeeThree_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BeeThree_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT) new BeeThree();
}


//-----------------------------------------------------------------------------
// name: BeeThree_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( BeeThree_dtor  )
{ 
    delete (BeeThree *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: BeeThree_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( BeeThree_tick )
{
    BeeThree * m = (BeeThree *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: BeeThree_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( BeeThree_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: FMVoices_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FMVoices_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT) new FMVoices();
}


//-----------------------------------------------------------------------------
// name: FMVoices_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FMVoices_dtor  )
{ 
    delete (FMVoices *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: FMVoices_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( FMVoices_tick )
{
    FMVoices * m = (FMVoices *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FMVoices_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FMVoices_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FMVoices_ctrl_vowel()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_ctrl_vowel )
{
    FMVoices * voc= (FMVoices *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    voc->controlChange( __SK_Breath_, f * 128.0 );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: FMVoices_cget_vowel()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_cget_vowel )
{
}


//-----------------------------------------------------------------------------
// name: FMVoices_ctrl_spectralTilt()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_ctrl_spectralTilt )
{
    FMVoices * voc= (FMVoices *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    voc->controlChange( __SK_FootControl_, f * 128.0);
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: FMVoices_cget_spectralTilt()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_cget_spectralTilt )
{
}


//-----------------------------------------------------------------------------
// name: FMVoices_ctrl_adsrTarget()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_ctrl_adsrTarget )
{
    FMVoices * voc= (FMVoices *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    voc->controlChange( __SK_AfterTouch_Cont_, f * 128.0);
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: FMVoices_cget_adsrTarget()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FMVoices_cget_adsrTarget )
{
}




//-----------------------------------------------------------------------------
// name: HevyMetl_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HevyMetl_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT) new HevyMetl();
}


//-----------------------------------------------------------------------------
// name: HevyMetl_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( HevyMetl_dtor  )
{ 
    delete (HevyMetl *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: HevyMetl_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( HevyMetl_tick )
{
    HevyMetl * m = (HevyMetl *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: HevyMetl_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( HevyMetl_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: PercFlut_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( PercFlut_ctor  )
{
    // initialize member object
    PercFlut * flut = new PercFlut();
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT)flut;
    // default
    flut->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: PercFlut_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( PercFlut_dtor  )
{ 
    delete (PercFlut *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: PercFlut_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( PercFlut_tick )
{
    PercFlut * m = (PercFlut *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PercFlut_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( PercFlut_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Rhodey_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Rhodey_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT) new Rhodey();
}


//-----------------------------------------------------------------------------
// name: Rhodey_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Rhodey_dtor  )
{ 
    delete (Rhodey *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Rhodey_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Rhodey_tick )
{
    Rhodey * m = (Rhodey *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Rhodey_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Rhodey_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: TubeBell_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( TubeBell_ctor  )
{
    // initialize member object
    TubeBell * bell = new TubeBell();
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT)bell;
    // default
    bell->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: TubeBell_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( TubeBell_dtor  )
{
    delete (TubeBell *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: TubeBell_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( TubeBell_tick )
{
    TubeBell * m = (TubeBell *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: TubeBell_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( TubeBell_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: Wurley_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Wurley_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = (t_CKUINT)new Wurley();
}


//-----------------------------------------------------------------------------
// name: Wurley_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Wurley_dtor  )
{ 
    delete (Wurley *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    OBJ_MEMBER_UINT(SELF, FM_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Wurley_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Wurley_tick )
{
    Wurley * m = (Wurley *)OBJ_MEMBER_UINT(SELF, FM_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Wurley_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Wurley_pmsg )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: FormSwep_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FormSwep_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, FormSwep_offset_data) = (t_CKUINT)new FormSwep();
}


//-----------------------------------------------------------------------------
// name: FormSwep_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FormSwep_dtor )
{ 
    delete (FormSwep *)OBJ_MEMBER_UINT(SELF, FormSwep_offset_data);
    OBJ_MEMBER_UINT(SELF, FormSwep_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: FormSwep_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( FormSwep_tick )
{
    FormSwep * m = (FormSwep *)OBJ_MEMBER_UINT(SELF, FormSwep_offset_data);
    *out = m->tick(in);
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: FormSwep_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FormSwep_pmsg )
{
    return TRUE;
}

//FormSwep requires multiple arguments
//to most of its parameters. 



//-----------------------------------------------------------------------------
// name: JCRev_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( JCRev_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, JCRev_offset_data) = (t_CKUINT)new JCRev( 4.0f );
}


//-----------------------------------------------------------------------------
// name: JCRev_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( JCRev_dtor )
{
    delete (JCRev *)OBJ_MEMBER_UINT(SELF, JCRev_offset_data);
    OBJ_MEMBER_UINT(SELF, JCRev_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: JCRev_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( JCRev_tick )
{
    JCRev * j = (JCRev *)OBJ_MEMBER_UINT(SELF, JCRev_offset_data);
    *out = j->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: JCRev_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( JCRev_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: JCRev_ctrl_mix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( JCRev_ctrl_mix )
{
    JCRev * j = (JCRev *)OBJ_MEMBER_UINT(SELF, JCRev_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setEffectMix( f );
    RETURN->v_float = (t_CKFLOAT) j->effectMix;
}


//-----------------------------------------------------------------------------
// name: JCRev_cget_mix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( JCRev_cget_mix )
{
    JCRev * j = (JCRev *)OBJ_MEMBER_UINT(SELF, JCRev_offset_data);
    RETURN->v_float = (t_CKFLOAT) j->effectMix;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Mandolin_ctor  )
{
    // initialize member object
    Mandolin * m = new Mandolin( 20 );
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)m;
    // default
    m->setFrequency( 220 );
}


//-----------------------------------------------------------------------------
// name: Mandolin_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Mandolin_dtor  )
{ 
    delete (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Mandolin_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Mandolin_tick )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Mandolin_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Mandolin_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_pluck()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_pluck )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    m->pluck( f );
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_pluckPos()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_pluckPos )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    m->setPluckPosition( f );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: Mandolin_cget_pluckPos()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Mandolin_cget_pluckPos )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)m->pluckPosition;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_bodySize()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_bodySize )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    m->setBodySize( f );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: Mandolin_cget_bodySize()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Mandolin_cget_bodySize )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)m->m_bodySize;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_stringDamping()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_stringDamping )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    // m->setBaseLoopGain( f );
    m->setBaseLoopGain( 0.97f + f * 0.03f );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: Mandolin_cget_stringDamping()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Mandolin_cget_stringDamping )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)m->baseLoopGain;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_stringDetune()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_stringDetune )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    // m->setDetune( f );
    m->setDetune( 1.0f - 0.1f * f );
    RETURN->v_float = f;
}


//-----------------------------------------------------------------------------
// name: Mandolin_cget_stringDetune()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Mandolin_cget_stringDetune )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)m->detuning;
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_afterTouch()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_afterTouch )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    // not sure what this does in stk version so we'll just call controlChange
    m->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
}


//-----------------------------------------------------------------------------
// name: Mandolin_ctrl_bodyIR()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Mandolin_ctrl_bodyIR )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    Chuck_String * str = GET_NEXT_STRING(ARGS);
    m->setBodyIR( str->str().c_str(), strstr(str->str().c_str(), ".raw") != NULL );
    RETURN->v_string = str;
}


//-----------------------------------------------------------------------------
// name: Mandolin_cget_bodyIR()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Mandolin_cget_bodyIR )
{
    Mandolin * m = (Mandolin *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_string = &(m->soundfile[0]->str_filename);    
}


// Modulate
//-----------------------------------------------------------------------------
// name: Modulate_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Modulate_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Modulate_offset_data) = (t_CKUINT) new Modulate( );
}


//-----------------------------------------------------------------------------
// name: Modulate_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Modulate_dtor )
{
    delete (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    OBJ_MEMBER_UINT(SELF, Modulate_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Modulate_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Modulate_tick )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    *out = j->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Modulate_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Modulate_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Modulate_ctrl_vibratoRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Modulate_ctrl_vibratoRate )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setVibratoRate( f );
    RETURN->v_float = (t_CKFLOAT) j->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: Modulate_cget_vibratoRate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Modulate_cget_vibratoRate )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    RETURN->v_float = (t_CKFLOAT) j->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: Modulate_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Modulate_ctrl_vibratoGain )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setVibratoGain( f );
    RETURN->v_float = (t_CKFLOAT) j->vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Modulate_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------

CK_DLL_CGET( Modulate_cget_vibratoGain )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    RETURN->v_float = (t_CKFLOAT) j->vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Modulate_ctrl_randomGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Modulate_ctrl_randomGain )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setRandomGain(f );
    RETURN->v_float = (t_CKFLOAT) j->randomGain;
}


//-----------------------------------------------------------------------------
// name: Modulate_cget_randomGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Modulate_cget_randomGain )
{
    Modulate * j = (Modulate *)OBJ_MEMBER_UINT(SELF, Modulate_offset_data);
    RETURN->v_float = (t_CKFLOAT) j->randomGain;
}


//-----------------------------------------------------------------------------
// name: Moog_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Moog_ctor  )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new Moog();
}


//-----------------------------------------------------------------------------
// name: Moog_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Moog_dtor  )
{ 
    delete (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Moog_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Moog_tick )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Moog_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Moog_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_modSpeed()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_modSpeed )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->setModulationSpeed(f);
    RETURN->v_float = (t_CKFLOAT) m->loops[1]->m_freq;
}
 

//-----------------------------------------------------------------------------
// name: Moog_cget_modSpeed()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_modSpeed )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) m->loops[1]->m_freq;
}
 

//-----------------------------------------------------------------------------
// name: Moog_ctrl_modDepth()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_modDepth )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->setModulationDepth(f);
    RETURN->v_float = (t_CKFLOAT) m->modDepth * 2.0;
}


//-----------------------------------------------------------------------------
// name: Moog_cget_modDepth()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_modDepth )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) m->modDepth * 2.0;
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_filterQ()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_filterQ )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->controlChange( __SK_FilterQ_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)  10.0 * ( m->filterQ - 0.80 );
}


//-----------------------------------------------------------------------------
// name: Moog_cget_filterQ()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_filterQ )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)  10.0 * ( m->filterQ - 0.80 );
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_filterSweepRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_filterSweepRate )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->controlChange( __SK_FilterSweepRate_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)  m->filterRate * 5000;
}


//-----------------------------------------------------------------------------
// name: Moog_cget_filterSweepRate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_filterSweepRate )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)  m->filterRate * 5000;
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_afterTouch()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_afterTouch )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_vibratoFreq )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    m->setModulationSpeed(f);
    RETURN->v_float = (t_CKFLOAT)m->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Moog_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_vibratoFreq )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)m->m_vibratoFreq;
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_vibratoGain )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->controlChange( __SK_ModWheel_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)  m->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Moog_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_vibratoGain )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)  m->m_vibratoGain;
}


//-----------------------------------------------------------------------------
// name: Moog_ctrl_volume()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Moog_ctrl_volume )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    m->controlChange( __SK_AfterTouch_Cont_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)  m->m_volume;
}


//-----------------------------------------------------------------------------
// name: Moog_cget_volume()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Moog_cget_volume )
{
    Moog * m = (Moog *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)  m->m_volume;
}




// NRev
//-----------------------------------------------------------------------------
// name: NRev_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( NRev_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, NRev_offset_data) = (t_CKUINT)new NRev( 4.0f );
}


//-----------------------------------------------------------------------------
// name: NRev_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( NRev_dtor )
{
    delete (NRev *)OBJ_MEMBER_UINT(SELF, NRev_offset_data);
    OBJ_MEMBER_UINT(SELF, NRev_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: NRev_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( NRev_tick )
{
    NRev * j = (NRev *)OBJ_MEMBER_UINT(SELF, NRev_offset_data);
    *out = j->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: NRev_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( NRev_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: NRev_ctrl_mix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( NRev_ctrl_mix )
{
    NRev * j = (NRev *)OBJ_MEMBER_UINT(SELF, NRev_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setEffectMix( f );
    RETURN->v_float = (t_CKFLOAT) j->effectMix;
}


//-----------------------------------------------------------------------------
// name: NRev_cget_mix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( NRev_cget_mix )
{
    NRev * j = (NRev *)OBJ_MEMBER_UINT(SELF, NRev_offset_data);
    RETURN->v_float = (t_CKFLOAT) j->effectMix;
}



// PitShift
//-----------------------------------------------------------------------------
// name: PitShift_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( PitShift_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, PitShift_offset_data) = (t_CKUINT)new PitShift( );
}


//-----------------------------------------------------------------------------
// name: PitShift_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( PitShift_dtor )
{
    delete (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    OBJ_MEMBER_UINT(SELF, PitShift_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: PitShift_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( PitShift_tick )
{
    PitShift * p = (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    *out = p->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PitShift_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( PitShift_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PitShift_ctrl_shift()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PitShift_ctrl_shift )
{
    PitShift * p = (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setShift( f );
    RETURN->v_float = (t_CKFLOAT)  1.0 - p->rate;
}


//-----------------------------------------------------------------------------
// name: PitShift_cget_shift()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PitShift_cget_shift )
{
    PitShift * p = (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    RETURN->v_float = (t_CKFLOAT)  1.0 - p->rate;
}


//-----------------------------------------------------------------------------
// name: PitShift_ctrl_effectMix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PitShift_ctrl_effectMix )
{
    PitShift * p = (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    p->setEffectMix( f );
    RETURN->v_float = (t_CKFLOAT)  p->effectMix;
}


//-----------------------------------------------------------------------------
// name: PitShift_cget_effectMix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PitShift_cget_effectMix )
{
    PitShift * p = (PitShift *)OBJ_MEMBER_UINT(SELF, PitShift_offset_data);
    RETURN->v_float = (t_CKFLOAT)  p->effectMix;
}



// PRCRev
//-----------------------------------------------------------------------------
// name: PRCRev_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( PRCRev_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, PRCRev_offset_data) = (t_CKUINT)new PRCRev( 4.0f );
}


//-----------------------------------------------------------------------------
// name: PRCRev_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( PRCRev_dtor )
{
    delete (PRCRev *)OBJ_MEMBER_UINT(SELF, PRCRev_offset_data);
    OBJ_MEMBER_UINT(SELF, PRCRev_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: PRCRev_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( PRCRev_tick )
{
    PRCRev * j = (PRCRev *)OBJ_MEMBER_UINT(SELF, PRCRev_offset_data);
    *out = j->tick( in );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PRCRev_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( PRCRev_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: PRCRev_ctrl_mix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( PRCRev_ctrl_mix )
{
    PRCRev * j = (PRCRev *)OBJ_MEMBER_UINT(SELF, PRCRev_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    j->setEffectMix( f );
    RETURN->v_float = (t_CKFLOAT)j->effectMix;
}


//-----------------------------------------------------------------------------
// name: PRCRev_cget_mix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( PRCRev_cget_mix )
{
    PRCRev * j = (PRCRev *)OBJ_MEMBER_UINT(SELF, PRCRev_offset_data);
    RETURN->v_float = (t_CKFLOAT)j->effectMix;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Shakers_ctor )
{
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new Shakers;
}


//-----------------------------------------------------------------------------
// name: Shakers_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Shakers_dtor )
{
    delete (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: Shakers_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( Shakers_tick )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = s->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Shakers_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Shakers_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_which()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_which )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKINT c = GET_CK_INT(ARGS);
    s->setupNum( c );
    RETURN->v_int = (t_CKINT)s->m_noteNum;
}


//-----------------------------------------------------------------------------
// name: Shakers_cget_which()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Shakers_cget_which )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_int = (t_CKINT)s->m_noteNum;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_energy()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_energy )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT e = GET_NEXT_FLOAT(ARGS);
    s->controlChange( 2, e * 128.0 );
    s->m_energy = e;
    RETURN->v_float = (t_CKFLOAT)s->m_energy;
}


//-----------------------------------------------------------------------------
// name: Shakers_cget_energy()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Shakers_cget_energy )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)s->m_energy;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_decay()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_decay )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT e = GET_NEXT_FLOAT(ARGS);
    s->controlChange( 4, e * 128.0 );
    s->m_decay = e;
    RETURN->v_float = (t_CKFLOAT)s->m_decay;
}


//-----------------------------------------------------------------------------
// name: Shakers_cget_decay()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Shakers_cget_decay )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)s->m_decay;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_objects()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_objects )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT e = GET_NEXT_FLOAT(ARGS);
    s->controlChange( 11, e );
    s->m_objects = e;
    RETURN->v_float = (t_CKFLOAT)s->m_objects;
}


//-----------------------------------------------------------------------------
// name: Shakers_cget_objects()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Shakers_cget_objects )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)s->m_objects;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_freq )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    s->freq = GET_NEXT_FLOAT(ARGS);
    s->controlChange( __SK_ModWheel_, ftom(s->freq) );
    RETURN->v_float = (t_CKFLOAT)s->freq;
}


//-----------------------------------------------------------------------------
// name: Shakers_cget_freq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( Shakers_cget_freq )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT( SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)s->freq;
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_noteOn()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_noteOn )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    s->ck_noteOn( f );
}


//-----------------------------------------------------------------------------
// name: Shakers_ctrl_noteOff()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Shakers_ctrl_noteOff )
{
    Shakers * s = (Shakers *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    s->noteOff( f );
}




//-----------------------------------------------------------------------------
// name: SubNoise_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( SubNoise_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, SubNoise_offset_data) = (t_CKUINT)new SubNoise( );
}


//-----------------------------------------------------------------------------
// name: SubNoise_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( SubNoise_dtor )
{
    delete (SubNoise *)OBJ_MEMBER_UINT(SELF, SubNoise_offset_data);
    OBJ_MEMBER_UINT(SELF, SubNoise_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: SubNoise_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( SubNoise_tick )
{
    SubNoise * p = (SubNoise *)OBJ_MEMBER_UINT(SELF, SubNoise_offset_data);
    *out = p->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: SubNoise_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( SubNoise_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: SubNoise_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( SubNoise_ctrl_rate )
{
    SubNoise * p = (SubNoise *)OBJ_MEMBER_UINT(SELF, SubNoise_offset_data);
    int i = GET_CK_INT(ARGS);
    p->setRate( i );
    RETURN->v_int = (t_CKINT) (int)((SubNoise *)OBJ_MEMBER_UINT(SELF, SubNoise_offset_data))->subRate();
}


//-----------------------------------------------------------------------------
// name: SubNoise_cget_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( SubNoise_cget_rate )
{
    RETURN->v_int = (t_CKINT) (int)((SubNoise *)OBJ_MEMBER_UINT(SELF, SubNoise_offset_data))->subRate();
}





//-----------------------------------------------------------------------------
// name: VoicForm_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( VoicForm_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = (t_CKUINT)new VoicForm();
}


//-----------------------------------------------------------------------------
// name: VoicForm_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( VoicForm_dtor )
{ 
    delete (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: VoicForm_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( VoicForm_tick )
{
    VoicForm * m = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    *out = m->tick();
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: VoicForm_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( VoicForm_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_speak()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_speak )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    v->speak();
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_quiet()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_quiet )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    v->quiet();
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_phoneme()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_phoneme )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    const char * c = GET_CK_STRING(ARGS)->str().c_str();
    v->setPhoneme( c );
    RETURN->v_string = &(v->str_phoneme);
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_phoneme()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_phoneme )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_string = &(v->str_phoneme);
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_voiced()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_voiced )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->setVoiced( f );
    RETURN->v_float = (t_CKFLOAT)v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_voiced()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_voiced )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);

    RETURN->v_float = (t_CKFLOAT)v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_unVoiced()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_unVoiced )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->setUnVoiced( f ); //not sure if this should be multiplied
    RETURN->v_float = (t_CKFLOAT)v->noiseEnv->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_unVoiced()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_unVoiced )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)v->noiseEnv->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_voiceMix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_voiceMix )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->controlChange(__SK_Breath_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT)v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_voiceMix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_voiceMix )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_selPhoneme()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_selPhoneme )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    int i = GET_CK_INT(ARGS); 
    v->controlChange(__SK_FootControl_, i);
    RETURN->v_float = (t_CKFLOAT)v->m_phonemeNum;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_selPhoneme()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_selPhoneme )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)v->m_phonemeNum;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_vibratoFreq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_vibratoFreq )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    v->voiced->modulator->vibrato->setFrequency( f );
    RETURN->v_float = (t_CKFLOAT)v->voiced->modulator->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_vibratoFreq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_vibratoFreq )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT)v->voiced->modulator->vibrato->m_freq;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_vibratoGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_vibratoGain )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->controlChange(__SK_ModWheel_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT) v->voiced->modulator->vibratoGain;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_vibratoGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_vibratoGain )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) v->voiced->modulator->vibratoGain;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_loudness()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_loudness )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->controlChange(__SK_AfterTouch_Cont_, f * 128.0 );
    RETURN->v_float = (t_CKFLOAT) v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_loudness()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_loudness )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) v->voiced->envelope->value;
}


//-----------------------------------------------------------------------------
// name: VoicForm_ctrl_pitchSweepRate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( VoicForm_ctrl_pitchSweepRate )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS); 
    v->setPitchSweepRate( f );
    RETURN->v_float = (t_CKFLOAT) v->voiced->m_freq;
}


//-----------------------------------------------------------------------------
// name: VoicForm_cget_pitchSweepRate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( VoicForm_cget_pitchSweepRate )
{
    VoicForm * v = (VoicForm *)OBJ_MEMBER_UINT(SELF, Instrmnt_offset_data);
    RETURN->v_float = (t_CKFLOAT) v->voiced->m_freq;
}



// WvIn
//-----------------------------------------------------------------------------
// name: WvIn_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( WvIn_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, WvIn_offset_data) = (t_CKUINT)new WvIn;
}


//-----------------------------------------------------------------------------
// name: WvIn_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( WvIn_dtor )
{
    delete (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    OBJ_MEMBER_UINT(SELF, WvIn_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: WvIn_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( WvIn_tick )
{
    WvIn * w = (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    *out = ( w->m_loaded ? (t_CKFLOAT)w->tick() / SHRT_MAX : (SAMPLE)0.0 );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: WvIn_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( WvIn_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: WvIn_ctrl_rate()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvIn_ctrl_rate )
{
    WvIn * w = (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    w->setRate( f );
    RETURN->v_float = (t_CKFLOAT) w->rate;
}


//-----------------------------------------------------------------------------
// name: WvIn_ctrl_path()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvIn_ctrl_path )
{
    WvIn * w = (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    const char * c = GET_CK_STRING(ARGS)->str().c_str();
    try { w->openFile( c, FALSE, FALSE ); }
    catch( StkError & e )
    {
        const char * s = e.getMessage();
        // CK_FPRINTF_STDERR( "[chuck](via STK): WvIn cannot load file '%s'\n", c );
        s = "";
    }
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvIn_cget_rate()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvIn_cget_rate )
{
    WvIn * w = (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    RETURN->v_float = (t_CKFLOAT) w->rate;
}


//-----------------------------------------------------------------------------
// name: WvIn_cget_path()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvIn_cget_path )
{
    WvIn * w = (WvIn *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    RETURN->v_string = &(w->str_filename);
}


// WaveLoop
//-----------------------------------------------------------------------------
// name: WaveLoop_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( WaveLoop_ctor )
{
    // initialize member object
    OBJ_MEMBER_UINT(SELF, WvIn_offset_data) = (t_CKUINT)new WaveLoop;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( WaveLoop_dtor )
{
    delete (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    OBJ_MEMBER_UINT(SELF, WvIn_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( WaveLoop_tick )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    *out = ( w->m_loaded ? (t_CKFLOAT)w->tick() / SHRT_MAX : (SAMPLE)0.0 );
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( WaveLoop_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_ctrl_freq()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WaveLoop_ctrl_freq )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    w->setFrequency( f );
    RETURN->v_float = (t_CKFLOAT) w->m_freq;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_cget_freq()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WaveLoop_cget_freq )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    RETURN->v_float = (t_CKFLOAT) w->m_freq;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_ctrl_phase()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WaveLoop_ctrl_phase )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    float f = (float)GET_NEXT_FLOAT(ARGS);
    w->addPhase( f );
    RETURN->v_float = (t_CKFLOAT) w->time / w->fileSize;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_cget_phase()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WaveLoop_cget_phase )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    RETURN->v_float = (t_CKFLOAT) w->time / w->fileSize;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_ctrl_phaseOffset()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WaveLoop_ctrl_phaseOffset )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    w->addPhaseOffset( f );
    RETURN->v_float = (t_CKFLOAT) w->phaseOffset;
}


//-----------------------------------------------------------------------------
// name: WaveLoop_cget_phaseOffset()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WaveLoop_cget_phaseOffset )
{
    WaveLoop * w = (WaveLoop *)OBJ_MEMBER_UINT(SELF, WvIn_offset_data);
    RETURN->v_float = (t_CKFLOAT) w->phaseOffset;
}


std::map<WvOut *, WvOut *> g_wv;

// WvOut
//-----------------------------------------------------------------------------
// name: WvOut_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( WvOut_ctor )
{
    WvOut * yo = new WvOut;
    yo->autoPrefix.set( "chuck-session" );
    // REFACTOR-2017 TODO Ge: Fix wvout realtime audio
/*    // ge: 1.3.5.3
    yo->asyncIO = g_enable_realtime_audio;
*/
    yo->asyncIO = FALSE;
    // yo->asyncIO = SHRED->vm_ref->m_audio;
    OBJ_MEMBER_UINT(SELF, WvOut_offset_data) = (t_CKUINT)yo;
}


//-----------------------------------------------------------------------------
// name: WvOut_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( WvOut_dtor )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    w->closeFile();
    std::map<WvOut *, WvOut *>::iterator iter;
    iter = g_wv.find( w );
    if(iter != g_wv.end())
        g_wv.erase( iter );
    delete (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    OBJ_MEMBER_UINT(SELF, WvOut_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: WvOut_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( WvOut_tick )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    // added 1.3.0.0: apply fileGain
    if( w->start ) w->tick( w->fileGain * in );
    *out = in; // pass samples downstream
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: WvOut_tick()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICKF( WvOut2_tickf )
{
    // assumption: stereo (2-channel) operation
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    MY_FLOAT frame[2];
    for(int i = 0; i < nframes; i++)
    {
        frame[0] = in[i*2] * w->fileGain;
        frame[1] = in[i*2+1] * w->fileGain;
        
        if( w->start ) w->tickFrame( frame, 1 );
        
        out[i*2] = in[i*2]; // pass samples downstream
        out[i*2+1] = in[i*2+1]; // pass samples downstream
    }
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: WvOut_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( WvOut_pmsg )
{
    return TRUE;
}


// XXX chuck got mono, so we have one channel. fix later.
//-----------------------------------------------------------------------------
// name: WvOut_ctrl_matFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_matFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").mat" );
        filename = buffer;
    }
    try { w->openFile( filename, 1, WvOut::WVOUT_MAT, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut2_ctrl_matFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut2_ctrl_matFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").mat" );
        filename = buffer;
    }
    try { w->openFile( filename, 2, WvOut::WVOUT_MAT, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_sndFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_sndFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").snd" );
        filename = buffer;
    }
    try { w->openFile( filename, 1, WvOut::WVOUT_SND, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut2_ctrl_sndFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut2_ctrl_sndFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").snd" );
        filename = buffer;
    }
    try { w->openFile( filename, 2, WvOut::WVOUT_SND, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_wavFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_wavFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").wav" );
        filename = buffer;
    }
    try { w->openFile( filename, 1, WvOut::WVOUT_WAV, Stk::STK_SINT16 ); }
    catch( StkError & e )
    {
        // CK_FPRINTF_STDERR( "%s\n", e.getMessage() );
        goto done;
    }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut2_ctrl_wavFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut2_ctrl_wavFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").wav" );
        filename = buffer;
    }
    try { w->openFile( filename, 2, WvOut::WVOUT_WAV, Stk::STK_SINT16 ); }
    catch( StkError & e )
    {
        // CK_FPRINTF_STDERR( "%s\n", e.getMessage() );
        goto done;
    }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_rawFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_rawFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").raw" );
        filename = buffer;
    }
    try { w->openFile( filename, 1, WvOut::WVOUT_RAW, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut2_ctrl_rawFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut2_ctrl_rawFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").raw" );
        filename = buffer;
    }
    try { w->openFile( filename, 2, WvOut::WVOUT_RAW, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_aifFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_aifFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").aiff" );
        filename = buffer;
    }
    try { w->openFile( filename, 1, WvOut::WVOUT_AIF, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut2_ctrl_aifFilename()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut2_ctrl_aifFilename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str().c_str();
    char buffer[1024];
    
    // special
    if( strstr( filename, "special:auto" ) )
    {
        time_t t; time(&t);
        strcpy( buffer, w->autoPrefix.str().c_str() );
        strcat( buffer, "(" );
        strncat( buffer, ctime(&t), 24 );
        buffer[strlen(w->autoPrefix.str().c_str())+14] = 'h';
        buffer[strlen(w->autoPrefix.str().c_str())+17] = 'm';
        strcat( buffer, ").aiff" );
        filename = buffer;
    }
    try { w->openFile( filename, 2, WvOut::WVOUT_AIF, Stk::STK_SINT16 ); }
    catch( StkError & e ) { goto done; }
    g_wv[w] = w;
    
done:
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_cget_filename()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvOut_cget_filename )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    RETURN->v_string = &(w->str_filename);
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_closeFile()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_closeFile )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    w->closeFile();
    
    std::map<WvOut *, WvOut *>::iterator iter;
    iter = g_wv.find( w );
    if(iter != g_wv.end())
        g_wv.erase( iter );
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_record()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_record )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    t_CKINT i = GET_NEXT_INT(ARGS);
    w->start = i ? 1 : 0;
    RETURN->v_int = (t_CKINT) w->start;
}


//-----------------------------------------------------------------------------
// name: WvOut_cget_record()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvOut_cget_record )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    RETURN->v_int = (t_CKINT) w->start;
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_autoPrefix()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_autoPrefix )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    w->autoPrefix.set( GET_NEXT_STRING(ARGS)->str().c_str() );
    RETURN->v_string = &w->autoPrefix;
}


//-----------------------------------------------------------------------------
// name: WvOut_cget_autoPrefix()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvOut_cget_autoPrefix )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    RETURN->v_string = &w->autoPrefix;
}


//-----------------------------------------------------------------------------
// name: WvOut_ctrl_fileGain()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( WvOut_ctrl_fileGain )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    w->fileGain = GET_NEXT_FLOAT(ARGS);
    RETURN->v_float = w->fileGain;
}


//-----------------------------------------------------------------------------
// name: WvOut_cget_fileGain()
// desc: CGET function ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( WvOut_cget_fileGain )
{
    WvOut * w = (WvOut *)OBJ_MEMBER_UINT(SELF, WvOut_offset_data);
    RETURN->v_float = w->fileGain;
}


//-----------------------------------------------------------------------------
// BLT
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BLT_ctor )
{ /* do nothing here */ }

CK_DLL_DTOR( BLT_dtor )
{ /* do nothing here */ }

CK_DLL_TICK( BLT_tick )
{
    CK_FPRINTF_STDERR( "BLT is virtual!\n" );
    return TRUE;
}

CK_DLL_PMSG( BLT_pmsg )
{
    return TRUE;
}

CK_DLL_CTRL( BLT_ctrl_phase )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    blt->setPhase( f );
    blt->m_phase = f;
    RETURN->v_float = f;
}

CK_DLL_CGET( BLT_cget_phase )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    RETURN->v_float = blt->getValuePhase();
}

CK_DLL_CTRL( BLT_ctrl_freq )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    blt->setFrequency( f );
    blt->m_freq = f;
    RETURN->v_float = f;
}

CK_DLL_CGET( BLT_cget_freq )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    RETURN->v_float = blt->getValueFreq();
}

CK_DLL_CTRL( BLT_ctrl_harmonics )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    t_CKINT i = GET_CK_INT(ARGS);
    if( i < 0 ) i = -i;
    blt->setHarmonics( i );
    blt->m_harmonics = i;
    RETURN->v_int = i;
}

CK_DLL_CGET( BLT_cget_harmonics )
{
    BLT * blt = (BLT *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    RETURN->v_int = blt->getValueHarmonics();
}


//-----------------------------------------------------------------------------
// Blit
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Blit_ctor )
{
    Blit * blit = new Blit;
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = (t_CKUINT)blit;
}

CK_DLL_DTOR( Blit_dtor )
{
    delete (Blit *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = 0;
}

CK_DLL_TICK( Blit_tick )
{
    Blit * blit = (Blit *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    *out = blit->tick();
    return TRUE;
}

CK_DLL_PMSG( Blit_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// BlitSaw
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BlitSaw_ctor )
{
    BlitSaw * blit = new BlitSaw;
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = (t_CKUINT)blit;
}

CK_DLL_DTOR( BlitSaw_dtor )
{
    delete (BlitSaw *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = 0;
}

CK_DLL_TICK( BlitSaw_tick )
{
    BlitSaw * blit = (BlitSaw *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    *out = blit->tick();
    return TRUE;
}

CK_DLL_PMSG( BlitSaw_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// BlitSquare
//-----------------------------------------------------------------------------
CK_DLL_CTOR( BlitSquare_ctor )
{
    BlitSquare * blit = new BlitSquare;
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = (t_CKUINT)blit;
}

CK_DLL_DTOR( BlitSquare_dtor )
{
    delete (BlitSquare *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    OBJ_MEMBER_UINT(SELF, BLT_offset_data) = 0;
}

CK_DLL_TICK( BlitSquare_tick )
{
    BlitSquare * blit = (BlitSquare *)OBJ_MEMBER_UINT(SELF, BLT_offset_data);
    *out = blit->tick();
    return TRUE;
}

CK_DLL_PMSG( BlitSquare_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: JetTable
// desc: ...
//-----------------------------------------------------------------------------

CK_DLL_CTOR( JetTabl_ctor )
{
    JetTabl * j = new JetTabl;
    OBJ_MEMBER_UINT(SELF, JetTabl_offset_data) = (t_CKUINT)j;
}

CK_DLL_DTOR( JetTabl_dtor )
{
    JetTabl * j = (JetTabl *)OBJ_MEMBER_UINT(SELF, JetTabl_offset_data);
    SAFE_DELETE(j);
    OBJ_MEMBER_UINT(SELF, JetTabl_offset_data) = 0;
}

CK_DLL_TICK( JetTabl_tick )
{
    JetTabl * j = (JetTabl *)OBJ_MEMBER_UINT(SELF, JetTabl_offset_data);
    *out = j->tick( in );
    return TRUE;
}

CK_DLL_PMSG( JetTabl_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: Mesh2D ctor
// desc: CGET function ...
//-----------------------------------------------------------------------------

CK_DLL_CTOR( Mesh2D_ctor ) { 
    Mesh2D * m = new Mesh2D( 2,2 );
    OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data) = (t_CKUINT)m;
}


CK_DLL_DTOR( Mesh2D_dtor ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    SAFE_DELETE(m);
    OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data) = 0;
}


CK_DLL_TICK( Mesh2D_tick ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    *out = m->tick( in );
    return TRUE;
}

CK_DLL_PMSG( Mesh2D_pmsg ) { 
    return TRUE;
}

CK_DLL_CTRL( Mesh2D_ctrl_nx ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    m->setNX( GET_NEXT_INT ( ARGS ) );
}

CK_DLL_CTRL( Mesh2D_ctrl_ny ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    m->setNY( GET_NEXT_INT ( ARGS ) );
    
}

CK_DLL_CGET( Mesh2D_cget_nx ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    RETURN->v_int = m->NX;
}

CK_DLL_CGET( Mesh2D_cget_ny ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    RETURN->v_int = m->NY;
}


CK_DLL_CTRL( Mesh2D_ctrl_input_position ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    t_CKFLOAT xpos = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT ypos = GET_NEXT_FLOAT(ARGS);
    m->setInputPosition(xpos,ypos);
}

CK_DLL_CTRL( Mesh2D_cget_input_position ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    RETURN->v_float = m->xInput / (m->NX - 1);
}


CK_DLL_CTRL( Mesh2D_ctrl_decay ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    t_CKFLOAT dec = GET_NEXT_FLOAT ( ARGS );
    m->setDecay( dec );
    RETURN->v_float = dec;
}

CK_DLL_CGET( Mesh2D_cget_decay ) { 
}

CK_DLL_CTRL( Mesh2D_ctrl_note_on ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    t_CKFLOAT note = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT vel = GET_NEXT_FLOAT(ARGS);
    m->noteOn( note, vel );
}

CK_DLL_CTRL( Mesh2D_ctrl_note_off ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    m->noteOff( GET_NEXT_FLOAT(ARGS) ); //need a version that takes a float
}

CK_DLL_CGET( Mesh2D_cget_energy ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    RETURN->v_float = m->energy();
}

CK_DLL_CTRL ( Mesh2D_ctrl_control_change ) { 
    Mesh2D * m = (Mesh2D *)OBJ_MEMBER_UINT(SELF, Mesh2D_offset_data);
    t_CKINT ctrl = GET_NEXT_INT(ARGS);
    t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);
    m->controlChange( ctrl, val );
    
}

// MidiFileIn
CK_DLL_CTOR( MidiFileIn_ctor )
{
    OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data) = 0;
}

CK_DLL_DTOR( MidiFileIn_dtor )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    SAFE_DELETE(f);
    OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data) = 0;
}

CK_DLL_MFUN( MidiFileIn_open )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    SAFE_DELETE(f);

    Chuck_String * str = GET_NEXT_STRING(ARGS);
    
    try
    {
        f = new stk::MidiFileIn(str->str());
        OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data) = (t_CKUINT) f;
        RETURN->v_int = 1;
    }
    catch (StkError)
    {
        RETURN->v_int = 0;
    }
}

CK_DLL_MFUN( MidiFileIn_close )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    SAFE_DELETE(f);
    OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data) = 0;
}

CK_DLL_MFUN( MidiFileIn_numTracks )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    
    if(f)
        RETURN->v_int = f->getNumberOfTracks();
    else
        RETURN->v_int = 0;
}

CK_DLL_MFUN( MidiFileIn_read )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    
    RETURN->v_int = 0;
    
    if(f)
    {
        Chuck_Object * msg = GET_NEXT_OBJECT(ARGS);

        std::vector<unsigned char> event;
        t_CKDUR dur = f->getNextMidiEvent(&event) * f->getTickSeconds() * Stk::sampleRate();
        
        if(event.size())
        {
            OBJ_MEMBER_INT(msg, MidiMsg_offset_data1) = event[0];
            OBJ_MEMBER_INT(msg, MidiMsg_offset_data2) = event.size() >= 2 ? event[1] : 0;
            OBJ_MEMBER_INT(msg, MidiMsg_offset_data3) = event.size() >= 3 ? event[2] : 0;
            OBJ_MEMBER_DUR(msg, MidiMsg_offset_when) = dur;
            
            RETURN->v_int = 1;
        }
    }
}

CK_DLL_MFUN( MidiFileIn_readTrack )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    
    RETURN->v_int = 0;
    
    if(f)
    {
        Chuck_Object * msg = GET_NEXT_OBJECT(ARGS);
        t_CKINT track = GET_NEXT_INT(ARGS);
        
        if(track >= 0 && track < f->getNumberOfTracks())
        {
            std::vector<unsigned char> event;
            t_CKDUR dur = f->getNextMidiEvent(&event, track) * f->getTickSeconds() * Stk::sampleRate();
            
            if(event.size())
            {
                OBJ_MEMBER_INT(msg, MidiMsg_offset_data1) = event[0];
                OBJ_MEMBER_INT(msg, MidiMsg_offset_data2) = event.size() >= 2 ? event[1] : 0;
                OBJ_MEMBER_INT(msg, MidiMsg_offset_data3) = event.size() >= 3 ? event[2] : 0;
                OBJ_MEMBER_DUR(msg, MidiMsg_offset_when) = dur;
                
                RETURN->v_int = 1;
            }
        }
    }
}

CK_DLL_MFUN( MidiFileIn_rewind )
{
    stk::MidiFileIn *f = (stk::MidiFileIn *) OBJ_MEMBER_UINT(SELF, MidiFileIn_offset_data);
    
    if(f)
        f->rewindTrack();
}

//-----------------------------------------------------------------------------
// name: ck_detach()
// desc: ...
//-----------------------------------------------------------------------------
t_CKBOOL stk_detach( t_CKUINT type, void * data )
{
    std::map<WvOut *, WvOut *>::iterator iter;

    // log
    EM_log( CK_LOG_INFO, "(via STK): detaching file handles..." );
    
    for( iter = g_wv.begin(); iter != g_wv.end(); iter++ )
    {
        (*iter).second->closeFile();
    }
    
    // TODO: release the WvOut
    g_wv.clear();
    
    WvOut::shutdown();
    
    return TRUE;
}
