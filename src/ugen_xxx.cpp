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
// file: ugen_xxx.cpp
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#if defined(__CK_SNDFILE_NATIVE__)
#include <sndfile.h>
#else
#include "util_sndfile.h"
#endif

#include "ugen_xxx.h"
#include "chuck_type.h"
#include "chuck_ugen.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_instr.h"

#include <fstream>
using namespace std;


// subgraph
CK_DLL_CTOR( subgraph_ctor );

// foogen
CK_DLL_CTOR( foogen_ctor );
CK_DLL_DTOR( foogen_dtor );
CK_DLL_TICK( foogen_tick );


// LiSa query
DLL_QUERY lisa_query( Chuck_DL_Query * query );


t_CKUINT g_srate;

// offset
static t_CKUINT subgraph_offset_inlet = 0;
static t_CKUINT subgraph_offset_outlet = 0;
static t_CKUINT foogen_offset_data = 0;
static t_CKUINT stereo_offset_left = 0;
static t_CKUINT stereo_offset_right = 0;
static t_CKUINT stereo_offset_pan = 0;
static t_CKUINT cnoise_offset_data = 0;
static t_CKUINT impulse_offset_data = 0;
static t_CKUINT step_offset_data = 0;
static t_CKUINT delayp_offset_data = 0;
static t_CKUINT sndbuf_offset_data = 0;
static t_CKUINT dyno_offset_data = 0;
// static t_CKUINT zerox_offset_data = 0;




//-----------------------------------------------------------------------------
// name: xxx_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY xxx_query( Chuck_DL_Query * QUERY )
{
    g_srate = QUERY->srate;
    // get the env
    Chuck_Env * env = QUERY->env();

    Chuck_DL_Func * func = NULL;
    std::string doc;

    // deprecate
    type_engine_register_deprecate( env, "dac", "DAC" );
    type_engine_register_deprecate( env, "adc", "ADC" );
    type_engine_register_deprecate( env, "pan2", "Pan2" );
    type_engine_register_deprecate( env, "mix2", "Mix2" );
    type_engine_register_deprecate( env, "gain", "Gain" );
    type_engine_register_deprecate( env, "noise", "Noise" );
    type_engine_register_deprecate( env, "cnoise", "CNoise" );
    type_engine_register_deprecate( env, "impulse", "Impulse" );
    type_engine_register_deprecate( env, "step", "Step" );
    type_engine_register_deprecate( env, "halfrect", "HalfRect" );
    type_engine_register_deprecate( env, "fullrect", "FullRect" );
    type_engine_register_deprecate( env, "zerox", "ZeroX" );
    type_engine_register_deprecate( env, "delayp", "DelayP" );
    type_engine_register_deprecate( env, "sndbuf", "SndBuf" );

    //! \section audio output
    
    //-------------------------------------------------------------------------
    // init as base class: Subgraph
    //-------------------------------------------------------------------------
    doc = "Base class for chubgraph-based user unit generators. ";
    if( !type_engine_import_ugen_begin( env, "Chubgraph", "UGen", env->global(),
                                        subgraph_ctor, NULL, NULL, NULL, 1, 1, doc.c_str() ) )
        return FALSE;
    
    if( !type_engine_import_add_ex( env, "extend/chubgraph.ck" ) ) goto error;
    
    doc = "Terminal for sources chucked into this ugen. ";
    subgraph_offset_inlet = type_engine_import_mvar( env, "UGen", "inlet", TRUE, doc.c_str() );
    if( subgraph_offset_inlet == CK_INVALID_OFFSET ) goto error;
    
    doc = "Terminal for the output of this ugen. ";
    subgraph_offset_outlet = type_engine_import_mvar( env, "UGen", "outlet", TRUE, doc.c_str() );
    if( subgraph_offset_outlet == CK_INVALID_OFFSET ) goto error;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    

    //-------------------------------------------------------------------------
    // init as base class: FooGen
    //-------------------------------------------------------------------------
    doc = "Base class for chugen-based user unit generators.";
    if( !type_engine_import_ugen_begin( env, "Chugen", "UGen", env->global(),
                                        foogen_ctor, foogen_dtor, foogen_tick, NULL, 1, 1, doc.c_str() ) )
        return FALSE;
    
    if( !type_engine_import_add_ex( env, "extend/chugen.ck" ) ) goto error;

    foogen_offset_data = type_engine_import_mvar( env, "int", "@foogen_data", FALSE );
    if( foogen_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    

    //-------------------------------------------------------------------------
    // init as base class: UGen_Multi
    //-------------------------------------------------------------------------
    doc = "Base class for multi-channel unit generators.";
    if( !type_engine_import_ugen_begin( env, "UGen_Multi", "UGen", env->global(),
                                        multi_ctor, (f_dtor)NULL, (f_tick)NULL, (f_pmsg)NULL, 0, 0, doc.c_str() ) )
        return FALSE;
    
    // add chan
    func = make_new_mfun( "UGen", "chan", multi_cget_chan );
    func->add_arg( "int", "which" );
    func->doc = "Returns the ugen representing a specific channel of this ugen, or null if no such channel is available.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add pan
    /* func = make_new_mfun( "float", "pan", multi_ctrl_pan );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "pan", multi_cget_pan );
    if( !type_engine_import_mfun( env, func ) ) goto error; */

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    

    //---------------------------------------------------------------------
    // init as base class: UGen_Stereo
    //---------------------------------------------------------------------
    doc = "Base class for stereo unit generators.";
    if( !type_engine_import_ugen_begin( env, "UGen_Stereo", "UGen_Multi", env->global(),
                                        stereo_ctor, NULL, NULL, NULL, 2, 2, doc.c_str() ) )
        return FALSE;

    // add left
    doc = "Left channel (same as .chan(0)).";
    stereo_offset_left = type_engine_import_mvar( env, "UGen", "left", FALSE, doc.c_str() );
    if( stereo_offset_left == CK_INVALID_OFFSET ) goto error;
    
    // add right
    doc = "Right channel (same as .chan(1)).";
    stereo_offset_right = type_engine_import_mvar( env, "UGen", "right", FALSE, doc.c_str() );
    if( stereo_offset_right == CK_INVALID_OFFSET ) goto error;

    // add pan
    stereo_offset_pan = type_engine_import_mvar( env, "float", "@pan", FALSE );
    if( stereo_offset_pan == CK_INVALID_OFFSET ) goto error;
    
    // add pan
    func = make_new_mfun( "float", "pan", stereo_ctrl_pan );
    func->add_arg( "float", "val" );
    func->doc = "Panning between left and right channels, in range [-1,1], with -1 being far-left, 1 far-right, and 0 centered.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "pan", stereo_cget_pan );
    func->doc = "Panning between left and right channels, in range [-1,1], with -1 being far-left, 1 far-right, and 0 centered.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;

    // add dac
    //! digital/analog converter
    //! abstraction for underlying audio output device
    //---------------------------------------------------------------------
    // init as base class: dac
    //---------------------------------------------------------------------
    if( !(env->t_dac = type_engine_import_ugen_begin( env, "DAC", "UGen_Stereo", env->global(),
                                                   NULL, NULL, NULL, NULL, 2, 2 )) )
        return FALSE;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    // add adc
    //! analog/digital converter
    //! abstraction for underlying audio input device
    //---------------------------------------------------------------------
    // init as base class: adc
    //---------------------------------------------------------------------
    if( !(env->t_adc = type_engine_import_ugen_begin( env, "ADC", "UGen_Stereo", env->global(),
                                                   (f_ctor)NULL, (f_dtor)NULL, (f_tick)NULL, (f_pmsg)NULL, 0, 2 )) )
        return FALSE;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;

    //---------------------------------------------------------------------
    // init as base class: pan2
    //---------------------------------------------------------------------
    doc = "Spread mono signal to stereo.";
    if( !type_engine_import_ugen_begin( env, "Pan2", "UGen_Stereo", env->global(),
                                        NULL, NULL, NULL, NULL, 2, 2, doc.c_str() ) )
        return FALSE;
    
    if( !type_engine_import_add_ex( env, "stereo/moe2.ck" ) ) goto error;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;

    //---------------------------------------------------------------------
    // init as base class: mix2
    //---------------------------------------------------------------------
    doc = "Unit generator for mixing stereo signal source to mono.";
    if( !type_engine_import_ugen_begin( env, "Mix2", "UGen_Stereo", env->global(),
                                        NULL, NULL, NULL, NULL, 2, 2, doc.c_str() ) )
        return FALSE;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    // add blackhole
    //! sample rate sample sucker
    //! ( like dac, ticks ugens, but no more )
    //! see \example pwm.ck
    //QUERY->ugen_add( QUERY, "blackhole", NULL );
    // set funcs
    //QUERY->ugen_func( QUERY, NULL, NULL, bunghole_tick, NULL );
    
    // add bunghole
    //! sample rate sample sucker
    //! ( like dac, ticks ugens, but no more )
    //QUERY->ugen_add( QUERY, "bunghole", NULL );
    // set funcs
    //QUERY->ugen_func( QUERY, NULL, NULL, bunghole_tick, NULL );
    
    // add gain
    //! gain control
    //! (NOTE - all unit generators can themselves change their gain)
    //! (this is a way to add N outputs together and scale them) 
    //! used in \example i-robot.ck
    //---------------------------------------------------------------------
    // init as base class: gain
    //---------------------------------------------------------------------
    doc = "Gain control. (Note: all unit generators can themselves change their gain. This is a way to add multiple outputs together and scale them.)";
    if( !type_engine_import_ugen_begin( env, "Gain", "UGen", env->global(), 
                                        NULL, NULL, NULL, NULL, doc.c_str() ) )
        return FALSE;
    
    if( !type_engine_import_add_ex( env, "basic/i-robot.ck" ) ) goto error;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    /*! \example
        noise n => gain g => dac;
    sinosc s => g;
    .3 => g.gain;
    while( true ) { 100::ms => now; }
    */
    
    //! \section wave forms
    
    // add noise
    //! white noise generator 
    //! see \example noise.ck \example powerup.ck
    //---------------------------------------------------------------------
    // init as base class: noise
    //---------------------------------------------------------------------
    doc = "White noise generator.";
    if( !type_engine_import_ugen_begin( env, "Noise", "UGen", env->global(), 
                                        NULL, NULL, noise_tick, NULL, doc.c_str() ) )
        return FALSE;

    if( !type_engine_import_add_ex( env, "basic/wind.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "shred/powerup.ck" ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
//zzz

    // add cnoise 
    /*QUERY->ugen_add( QUERY, "cnoise", NULL );
    QUERY->ugen_func( QUERY, cnoise_ctor, cnoise_dtor, cnoise_tick, NULL );
    QUERY->ugen_ctrl( QUERY, cnoise_ctrl_mode, NULL, "string", "mode" );
    QUERY->ugen_ctrl( QUERY, cnoise_ctrl_fprob, NULL, "float", "fprob" );*/

    //---------------------------------------------------------------------
    // init as base class: cnoise
    //---------------------------------------------------------------------
    doc = "Noise generator with multiple noise synthesis modes. ";
    if( !type_engine_import_ugen_begin( env, "CNoise", "UGen", env->global(),
                                        cnoise_ctor, cnoise_dtor, cnoise_tick, NULL, doc.c_str() ) )
        return FALSE;

    // add member variable
    cnoise_offset_data = type_engine_import_mvar( env, "int", "@cnoise_data", FALSE );
    if( cnoise_offset_data == CK_INVALID_OFFSET ) goto error;
    
    // add ctrl: mode
    func = make_new_mfun( "string", "mode", cnoise_ctrl_mode );
    func->add_arg( "string", "mode" );
    func->doc = "Noise synthesis mode. Supported modes are &quot;white&quot;, &quot;pink&quot;, &quot;flip&quot;, and &quot;xor&quot;.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: mode
    //func = make_new_mfun( "string", "mode", cnoise_cget_mode );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: fprob
    func = make_new_mfun( "float", "fprob", cnoise_ctrl_fprob );
    func->doc = "Probability [0-1] used for calculating XOR noise.";
    func->add_arg( "float", "fprob" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: fprob
    //func = make_new_mfun( "float", "fprob", cnoise_cget_fprob );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;


    // add impulse
    //! pulse generator - can set the value of the current sample
    //! default for each sample is 0 if not set
        //QUERY->ugen_add( QUERY, "impulse", NULL );
    // set funcs
        //QUERY->ugen_func( QUERY, impulse_ctor, impulse_dtor, impulse_tick, NULL );
    // ctrl func
        //QUERY->ugen_ctrl( QUERY, impulse_ctrl_value, impulse_cget_value, "float", "value" );
        //QUERY->ugen_ctrl( QUERY, impulse_ctrl_value, impulse_cget_value, "float", "next" ); //! set value of next sample
    /*! \example
        impulse i => dac;
    
    while( true ) {
        1.0 => i.next;
        100::ms => now;
    }
    */
    //---------------------------------------------------------------------
    // init as base class: impulse
    //---------------------------------------------------------------------
    doc = "Pulse generator - can set the value of the current sample. Default for each sample is 0 if not set.";
    if( !type_engine_import_ugen_begin( env, "Impulse", "UGen", env->global(), 
                                        impulse_ctor, impulse_dtor, impulse_tick, NULL, doc.c_str() ) )
        return FALSE;

    // add ctrl: value
    //func = make_new_mfun( "float", "value", impulse_ctrl_value );
    //func->add_arg( "float", "value" );
    //if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: value
    //func = make_new_mfun( "float", "value", impulse_cget_value );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // add member variable
    impulse_offset_data = type_engine_import_mvar( env, "int", "@impulse_data", FALSE );
    if( impulse_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: next
    func = make_new_mfun( "float", "next", impulse_ctrl_next );
    func->add_arg( "float", "next" );
    func->doc = "Value of next sample to be generated. (Note: if you are using the UGen.last method to read the output of the impulse, the value set by Impulse.next does not appear as the output until after the next sample boundary. In this case, there is a consistent 1::samp offset between setting .next and reading that value using .last.)";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: next
    func = make_new_mfun( "float", "next", impulse_cget_next );
    func->doc = "Value of next sample to be generated.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    // add step
    //! step generator - like impulse, but once a value is set, 
    //! it is held for all following samples, until value is set again
    //! see \example step.ck
        //QUERY->ugen_add( QUERY, "step", NULL );
    // set funcs
        //QUERY->ugen_func( QUERY, step_ctor, step_dtor, step_tick, NULL );
    // ctrl func
        //QUERY->ugen_ctrl( QUERY, step_ctrl_value, step_cget_value, "float", "value" );
        //QUERY->ugen_ctrl( QUERY, step_ctrl_value, step_cget_value, "float", "next" ); //! set the step value 
    /*! \example
        step s => dac;
    -1.0 => float amp;
    
    // square wave using step
    while( true ) {
        -amp => amp => s.next;
        800::samp => now;
    }
    */
    //---------------------------------------------------------------------
    // init as base class: step
    //---------------------------------------------------------------------
    doc = "Step generator - like Impulse, but once a value is set, it is held for all following samples, until value is set again.";
    if( !type_engine_import_ugen_begin( env, "Step", "UGen", env->global(), 
                                        step_ctor, step_dtor, step_tick, NULL, doc.c_str() ) )
        return FALSE;

    if( !type_engine_import_add_ex( env, "basic/step.ck" ) ) goto error;
    
    // add ctrl: value
    //func = make_new_mfun( "float", "value", step_ctrl_value );
    //func->add_arg( "float", "value" );
    //if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: value
    //func = make_new_mfun( "float", "value", step_cget_value );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // add member variable
    step_offset_data = type_engine_import_mvar( env, "int", "@step_data", FALSE );
    if( step_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: next
    func = make_new_mfun( "float", "next", step_ctrl_next );
    func->add_arg( "float", "next" );
    func->doc = "The step value.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: next
    func = make_new_mfun( "float", "next", step_cget_next );
    func->doc = "The step value.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    // add halfrect
    //! half wave rectifier
    //! for half-wave rectification. 
        //QUERY->ugen_add( QUERY, "halfrect", NULL );
    // set funcs
        //QUERY->ugen_func( QUERY, NULL, NULL, halfrect_tick, NULL );
    //---------------------------------------------------------------------
    // init as base class: halfrect
    //---------------------------------------------------------------------
    doc = "Half wave rectifier.";
    if( !type_engine_import_ugen_begin( env, "HalfRect", "UGen", env->global(), 
                                        NULL, NULL, halfrect_tick, NULL, doc.c_str() ) )
        return FALSE;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;


    // add fullrect
    //! full wave rectifier
        //QUERY->ugen_add( QUERY, "fullrect", NULL );
    // set funcs
        //QUERY->ugen_func( QUERY, NULL, NULL, fullrect_tick, NULL );
    //---------------------------------------------------------------------
    // init as base class: fullrect
    //---------------------------------------------------------------------
    doc = "Full wave rectifier.";
    if( !type_engine_import_ugen_begin( env, "FullRect", "UGen", env->global(),
                                        NULL, NULL, fullrect_tick, NULL, doc.c_str() ) )
        return FALSE;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;


    // add zerox
    //! zero crossing detector
    //! emits a single pulse at the the zero crossing in the direction of the zero crossing.  
    //! (see \example zerox.ck)
    //QUERY->ugen_add( QUERY, "zerox", NULL );
    // set funcs
    //QUERY->ugen_func( QUERY, zerox_ctor, zerox_dtor, zerox_tick, NULL );
    //---------------------------------------------------------------------
    // init as base class: zerox
    //---------------------------------------------------------------------
    /* if( !type_engine_import_ugen_begin( env, "ZeroX", "UGen", env->global(), 
                                        zerox_ctor, zerox_dtor, zerox_tick, NULL ) )
        return FALSE;

    // add member variable
    zerox_offset_data = type_engine_import_mvar( env, "int", "@zerox_data", FALSE );
    if( zerox_offset_data == CK_INVALID_OFFSET ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE; */


    //! \section delay lines
    
    //! delay with varying write position ( instead of read position ) 
    //! change to delay length will not affect the delay of samples already in
    //! the buffer.
    //! see \example delayp.ck

    //QUERY->ugen_add( QUERY, "delayp" , NULL);
    //QUERY->ugen_func ( QUERY, delayp_ctor, delayp_dtor, delayp_tick, delayp_pmsg);
    //QUERY->ugen_ctrl( QUERY, delayp_ctrl_delay, delayp_cget_delay , "dur", "delay" ); //! delay before subsequent values emerge
    //QUERY->ugen_ctrl( QUERY, delayp_ctrl_window, delayp_cget_window , "dur", "window" ); //! time for 'write head' to move
    //QUERY->ugen_ctrl( QUERY, delayp_ctrl_max, delayp_cget_max , "dur", "max" ); //! max delay possible.  trashes buffer, so do it first! 
    
    //---------------------------------------------------------------------
    // init as base class: delayp
    //---------------------------------------------------------------------
    if( !type_engine_import_ugen_begin( env, "DelayP", "UGen", env->global(), 
                                        delayp_ctor, delayp_dtor, 
                                        delayp_tick, delayp_pmsg ) )
        return FALSE;

    // add member variable
    delayp_offset_data = type_engine_import_mvar( env, "int", "@delayp_data", FALSE );
    if( delayp_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: delay
    func = make_new_mfun( "dur", "delay", delayp_ctrl_delay );
    func->add_arg( "dur", "delay" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: delay
    func = make_new_mfun( "dur", "delay", delayp_cget_delay );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: window
    func = make_new_mfun( "dur", "window", delayp_ctrl_window );
    func->add_arg( "dur", "window" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: window
    func = make_new_mfun( "dur", "window", delayp_cget_window );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: max
    func = make_new_mfun( "dur", "max", delayp_ctrl_max );
    func->add_arg( "dur", "max" );
    if( !type_engine_import_mfun( env, func ) ) goto error; 
    // add cget: max
    func = make_new_mfun( "dur", "max", delayp_cget_max );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    //! \section sound files
    
#ifndef __DISABLE_SNDBUF__
    
    // add sndbuf
    //! sound buffer ( now interpolating ) 
    //! reads from a variety of file formats
    //! see \example sndbuf.ck
    //QUERY->ugen_add( QUERY, "sndbuf", NULL );
    // set funcs
    //QUERY->ugen_func( QUERY, sndbuf_ctor, sndbuf_dtor, sndbuf_tick, NULL );
    // set ctrl
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_read, NULL, "string", "read" ); //! loads file for reading
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_write, NULL, "string", "write" ); //! loads a file for writing ( or not ) 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_pos, sndbuf_cget_pos, "int", "pos" ); //! set position ( 0 < p < .samples ) 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_loop, sndbuf_cget_loop, "int", "loop" ); //! toggle looping 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_interp, sndbuf_cget_interp, "int", "interp" ); //! set interpolation ( 0=drop, 1=linear, 2=sinc )
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_rate, sndbuf_cget_rate, "float", "rate" ); //! playback rate ( relative to file's natural speed ) 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_rate, sndbuf_cget_rate, "float", "play" ); //! play (same as rate) 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_freq, sndbuf_cget_freq, "float", "freq" ); //! playback rate ( file loops / second ) 
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_phase, sndbuf_cget_phase, "float", "phase" ); //! set phase position ( 0-1 )
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_channel, sndbuf_cget_channel, "int", "channel" ); //! select channel ( 0 < p < .channels )
    //QUERY->ugen_ctrl( QUERY, sndbuf_ctrl_phase_offset, sndbuf_cget_phase, "float", "phase_offset" ); //! set a phase offset
    //QUERY->ugen_ctrl( QUERY, NULL, sndbuf_cget_samples, "int", "samples" ); //! fetch number of samples
    //QUERY->ugen_ctrl( QUERY, NULL, sndbuf_cget_length, "dur", "length" ); //! fetch length
    //QUERY->ugen_ctrl( QUERY, NULL, sndbuf_cget_channels, "int", "channels" ); //! fetch number of channels
    
    //---------------------------------------------------------------------
    // init as base class: sndbuf
    //---------------------------------------------------------------------
    doc = "Interpolating sound buffer with single-channel output. Reads from a variety of uncompressed formats.";
    if( !type_engine_import_ugen_begin( env, "SndBuf", "UGen", env->global(), 
                                        sndbuf_ctor, sndbuf_dtor,
                                        sndbuf_tick, NULL, 1, 1, doc.c_str() ) )
        return FALSE;

    if( !type_engine_import_add_ex( env, "basic/sndbuf.ck" ) ) goto error;
    
    // add member variable
    sndbuf_offset_data = type_engine_import_mvar( env, "int", "@sndbuf_data", FALSE );
    if( sndbuf_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: read
    func = make_new_mfun( "string", "read", sndbuf_ctrl_read );
    func->add_arg( "string", "read" );
    func->doc = "Load file for reading.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: read // area
    //func = make_new_mfun( "string", "read", sndbuf_cget_read );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: write
    func = make_new_mfun( "string", "write", sndbuf_ctrl_write );
    func->add_arg( "string", "read" );
    func->doc = "Set file for writing (currently unsupported).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: write
    //func = make_new_mfun( "string", "write", sndbuf_cget_write );
    //if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: pos
    func = make_new_mfun( "int", "pos", sndbuf_ctrl_pos );
    func->add_arg( "int", "pos" );
    func->doc = "Set position (between 0 and number of samples).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: pos
    func = make_new_mfun( "int", "pos", sndbuf_cget_pos );
    func->doc = "Current position (between 0 and number of samples).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: loop
    func = make_new_mfun( "int", "loop", sndbuf_ctrl_loop );
    func->add_arg( "int", "loop" );
    func->doc = "Toggle for looping file playback.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: loop
    func = make_new_mfun( "int", "loop", sndbuf_cget_loop );
    func->doc = "Toggle for looping file playback.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: interp
    func = make_new_mfun( "int", "interp", sndbuf_ctrl_interp );
    func->add_arg( "int", "interp" );
    func->doc = "Interpolation mode. 0: drop sample, 1: linear interpolation, 2: sinc interpolation";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: interp
    func = make_new_mfun( "int", "interp", sndbuf_cget_interp );
    func->doc = "Interpolation mode. 0: drop sample, 1: linear interpolation, 2: sinc interpolation";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: rate
    func = make_new_mfun( "float", "rate", sndbuf_ctrl_rate );
    func->add_arg( "float", "rate" );
    func->doc = "Playback rate (relative to file's natural speed). For example, 0.5 is half speed and 2 is twice as fast.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: rate
    func = make_new_mfun( "float", "rate", sndbuf_cget_rate );
    func->doc = "Playback rate (relative to file's natural speed). For example, 0.5 is half speed and 2 is twice as fast.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: play
    func = make_new_mfun( "float", "play", sndbuf_ctrl_rate );
    func->add_arg( "float", "play" );
    func->doc = "Same as .rate. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: play // good
    func = make_new_mfun( "float", "play", sndbuf_cget_rate );
    func->doc = "Same as .rate. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: freq
    func = make_new_mfun( "float", "freq", sndbuf_ctrl_freq );
    func->add_arg( "float", "freq" );
    func->doc = "Loop rate (in file loops per second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: freq
    func = make_new_mfun( "float", "freq", sndbuf_cget_freq );
    func->doc = "Loop rate (in file loops per second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: phase
    func = make_new_mfun( "float", "phase", sndbuf_ctrl_phase );
    func->add_arg( "float", "phase" );
    func->doc = "Phase position, normalized to [0,1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: phase
    func = make_new_mfun( "float", "phase", sndbuf_cget_phase );
    func->doc = "Phase position, normalized to [0,1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: channel
    func = make_new_mfun( "int", "channel", sndbuf_ctrl_channel );
    func->add_arg( "int", "channel" );
    func->doc = "If the sound file contains more than one channel of audio, select which channel to play.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: channel
    func = make_new_mfun( "int", "channel", sndbuf_cget_channel );
    func->doc = "If the sound file contains more than one channel of audio, the selected channel to play.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: phase_offset
    func = make_new_mfun( "float", "phaseOffset", sndbuf_ctrl_phase_offset );
    func->add_arg( "float", "value" );
    func->doc = "Advance the playhead by the specified phase offset in [0,1), where 0 is no advance and 1 advance the entire length of the file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: phase_offset
    // func = make_new_mfun( "float", "phaseOffset", sndbuf_cget_phase_offset );
    // if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: chunks
    func = make_new_mfun( "int", "chunks", sndbuf_ctrl_chunks );
    func->add_arg( "int", "frames" );
    func->doc = "Chunk size, in frames, for loading the file from disk. Set to 0 to disable chunking.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    // add cget: chunks
    func = make_new_mfun( "int", "chunks", sndbuf_cget_chunks );
    func->doc = "Chunk size, in frames, for loading the file from disk. 0 indicates that chunking is disabled.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cget: samples
    func = make_new_mfun( "int", "samples", sndbuf_cget_samples );
    func->doc = "Total number of sample frames in the file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cget: length
    func = make_new_mfun( "dur", "length", sndbuf_cget_length );
    func->doc = "Total length of the file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add cget: channels
    func = make_new_mfun( "int", "channels", sndbuf_cget_channels );
    func->doc = "Number of channels available in the sound file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cget: valueAt
    func = make_new_mfun( "float", "valueAt", sndbuf_cget_valueAt );
    func->add_arg( "int", "pos" );
    func->doc = "Sample value at given position (in samples). ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
    
    //---------------------------------------------------------------------
    // init as base class: SndBuf2
    //---------------------------------------------------------------------
    doc = "Interpolating sound buffer with two-channel output. Reads from a variety of uncompressed formats.";
    if( !type_engine_import_ugen_begin( env, "SndBuf2", "SndBuf", env->global(),
                                        NULL, NULL,
                                        NULL, sndbuf_tickf, NULL, 2, 2, doc.c_str() ) )
        return FALSE;
    
    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;
    
#endif // __DISABLE_SNDBUF__


    //---------------------------------------------------------------------
    // init as base class: Dyno
    //---------------------------------------------------------------------
    doc = "Dynamics processor. Includes limiter, compressor, expander, noise gate, and ducker presets. ";
    if( !type_engine_import_ugen_begin( env, "Dyno", "UGen", env->global(), 
                                        dyno_ctor, dyno_dtor, dyno_tick, NULL, doc.c_str() ) )
        return FALSE;

    if( !type_engine_import_add_ex( env, "Dyno-compress.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "Dyno-duck.ck" ) ) goto error;
    if( !type_engine_import_add_ex( env, "Dyno-limit.ck" ) ) goto error;
    
    // add member variable
    dyno_offset_data = type_engine_import_mvar( env, "int", "@dyno_data", FALSE );
    if( dyno_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: limit
    func = make_new_mfun( "void", "limit", dyno_ctrl_limit );
    //func->add_arg( "string", "mode" );
    func->doc = "Set parameters to default limiter values. slopeAbove = 0.1, slopeBelow = 1.0, thresh = 0.5, attackTime = 5 ms, releaseTime = 300 ms, externalSideInput = 0 (false)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: compress
    func = make_new_mfun( "void", "compress", dyno_ctrl_compress );
    //func->add_arg( "string", "mode" );
    func->doc = "Set parameters to default compressor values.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: gate
    func = make_new_mfun( "void", "gate", dyno_ctrl_gate );
    //func->add_arg( "string", "mode" );
    func->doc = "Set parameters to default noise gate values.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: expand
    func = make_new_mfun( "void", "expand", dyno_ctrl_expand );
    //func->add_arg( "string", "mode" );
    func->doc = "Set parameters to default expander values.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: duck
    func = make_new_mfun( "void", "duck", dyno_ctrl_duck );
    //func->add_arg( "string", "mode" );
    func->doc = "Set parameters ot default ducker values. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: thresh
    func = make_new_mfun( "float", "thresh", dyno_ctrl_thresh );
    func->add_arg( "float", "thresh" );
    func->doc = "Threshold above which to stop using slopeBelow and start using slopeAbove to determine output gain vs input gain. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add cget: thresh
    func = make_new_mfun( "float", "thresh", dyno_cget_thresh );
    func->doc = "Threshold above which to stop using slopeBelow and start using slopeAbove to determine output gain vs input gain. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: attackTime
    func = make_new_mfun( "dur", "attackTime", dyno_ctrl_attackTime );
    func->add_arg( "dur", "aTime" );
    func->doc = "Duration for the envelope to move linearly from current value to the absolute value of the signal's amplitude.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    //add cget: attackTime
    func = make_new_mfun( "dur", "attackTime", dyno_cget_attackTime );
    func->doc = "Duration for the envelope to move linearly from current value to the absolute value of the signal's amplitude.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    //add ctrl: releaseTime
    func = make_new_mfun( "dur", "releaseTime", dyno_ctrl_releaseTime );
    func->add_arg( "dur", "rTime" );
    func->doc = "Duration for the envelope to decay down to around 1/10 of its current amplitude, if not brought back up by the signal.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    //add ctrl: releaseTime
    func = make_new_mfun( "dur", "releaseTime", dyno_cget_releaseTime );
    func->doc = "Duration for the envelope to decay down to around 1/10 of its current amplitude, if not brought back up by the signal.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    //add ctrl: ratio
    func = make_new_mfun( "float", "ratio", dyno_ctrl_ratio );
    func->add_arg( "float", "ratio" );
    func->doc = "Alternate way of setting slopeAbove and slopeBelow; sets slopeBelow to 1.0 and slopeAbove to 1.0 / ratio.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add cget: ratio
    func = make_new_mfun( "float", "ratio", dyno_cget_ratio );
    func->doc = "Alternate way of setting slopeAbove and slopeBelow; sets slopeBelow to 1.0 and slopeAbove to 1.0 / ratio.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: slopeBelow
    func = make_new_mfun( "float", "slopeBelow", dyno_ctrl_slopeBelow );
    func->add_arg( "float", "slopeBelow" );
    func->doc = "Determines the slope of the output gain vs the input envelope's level when the envelope is below thresh. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add cget: slopeBelow
    func = make_new_mfun( "float", "slopeBelow", dyno_cget_slopeBelow );
    func->doc = "Determines the slope of the output gain vs the input envelope's level when the envelope is below thresh. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: slopeAbove
    func = make_new_mfun( "float", "slopeAbove", dyno_ctrl_slopeAbove );
    func->add_arg( "float", "slopeAbove" );
    func->doc = "Determines the slope of the output gain vs the input envelope's level when the envelope is above thresh. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add cget: slopeAbove
    func = make_new_mfun( "float", "slopeAbove", dyno_cget_slopeAbove );
    func->doc = "Determines the slope of the output gain vs the input envelope's level when the envelope is above thresh. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: sideInput
    func = make_new_mfun( "float", "sideInput", dyno_ctrl_sideInput );
    func->add_arg( "float", "sideInput" );
    func->doc = "If externalSideInput is set to true, replaces the signal being processed as the input to the amplitude envelope.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add cget: sideInput
    func = make_new_mfun( "float", "sideInput", dyno_cget_sideInput );
    func->doc = "If externalSideInput is set to true, replaces the signal being processed as the input to the amplitude envelope.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add ctrl: externalSideInput
    func = make_new_mfun( "int", "externalSideInput", dyno_ctrl_externalSideInput );
    func->add_arg( "int", "externalSideInput" );
    func->doc = "Set to true to cue the amplitude envelope off of sideInput instead of the input signal. Note that this means you will need to manually set sideInput every so often. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //add cget: externalSideInput
    func = make_new_mfun( "int", "externalSideInput", dyno_cget_externalSideInput );
    func->doc = "Set to true to cue the amplitude envelope off of sideInput instead of the input signal. Note that this means you will need to manually set sideInput every so often. ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;

    // import LiSa!
    if( !lisa_query( QUERY ) )
        return FALSE;

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




// LiSa (live sampling data offset)
//static t_CKUINT LiSaBasic_offset_data = 0;
static t_CKUINT LiSaMulti_offset_data = 0;

//-----------------------------------------------------------------------------
// name: lisa_query()
// desc: ...
//-----------------------------------------------------------------------------
#define LiSa_channels 10 //max channels for multichannel LiSa
DLL_QUERY lisa_query( Chuck_DL_Query * QUERY )
{
    Chuck_Env * env = QUERY->env();
    Chuck_DL_Func * func = NULL;

    //---------------------------------------------------------------------
    // init class: LiSa; overloaded class for both LiSaBasic and LiSaMulti
    //              - probably don't need the others anymore....
    // author: Dan Trueman (dan /at/ music.princeton.edu)
    //---------------------------------------------------------------------
	
    
    if( !type_engine_import_ugen_begin( env, "LiSa", "UGen", env->global(),
                                        LiSaMulti_ctor, LiSaMulti_dtor,
                                        LiSaMulti_tick, NULL,
                                        LiSaMulti_pmsg, 1, 1 ))
        return FALSE;
	
    // set/get buffer size
    func = make_new_mfun( "dur", "duration", LiSaMulti_size );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
	func = make_new_mfun( "dur", "duration", LiSaMulti_cget_size );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // start/stop recording
    func = make_new_mfun( "int", "record", LiSaMulti_start_record );
    func->add_arg( "int", "toggle" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // start/stop playing
    func = make_new_mfun( "int", "play", LiSaMulti_start_play );
    func->add_arg( "int", "voice" );
    func->add_arg( "int", "toggle" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "play", LiSaMulti_start_play0 );
    func->add_arg( "int", "toggle" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // set/get playback rate
    func = make_new_mfun( "float", "rate", LiSaMulti_ctrl_rate );
    func->add_arg( "int", "voice" );
    func->add_arg( "float", "val" );
	if( !type_engine_import_mfun( env, func ) ) goto error;
	func = make_new_mfun( "float", "rate", LiSaMulti_cget_rate );
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "rate", LiSaMulti_ctrl_rate0 );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
	func = make_new_mfun( "float", "rate", LiSaMulti_cget_rate0 );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // playback position
    func = make_new_mfun( "dur", "playPos", LiSaMulti_ctrl_pindex );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "playPos", LiSaMulti_cget_pindex );
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "playPos", LiSaMulti_ctrl_pindex0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "playPos", LiSaMulti_cget_pindex0 );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // record position
    func = make_new_mfun( "dur", "recPos", LiSaMulti_ctrl_rindex );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "recPos", LiSaMulti_cget_rindex );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // loopstart position
    func = make_new_mfun( "dur", "loopStart", LiSaMulti_ctrl_lstart );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopStart", LiSaMulti_cget_lstart );
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopStart", LiSaMulti_ctrl_lstart0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopStart", LiSaMulti_cget_lstart0 );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // loopend position
    func = make_new_mfun( "dur", "loopEnd", LiSaMulti_ctrl_lend );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopEnd", LiSaMulti_cget_lend);
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopEnd", LiSaMulti_ctrl_lend0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopEnd", LiSaMulti_cget_lend0);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // loop
    func = make_new_mfun( "int", "loop", LiSaMulti_ctrl_loop );
    func->add_arg( "int", "voice" );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "loop", LiSaMulti_cget_loop);
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "loop", LiSaMulti_ctrl_loop0 );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "loop", LiSaMulti_cget_loop0);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // bidirectional looping
    func = make_new_mfun( "int", "bi", LiSaMulti_ctrl_bi );
    func->add_arg( "int", "voice" );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "getbi", LiSaMulti_cget_bi);
    func->add_arg( "int", "voice" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "bi", LiSaMulti_ctrl_bi0 );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "bi", LiSaMulti_cget_bi0);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // loopend_rec position
    func = make_new_mfun( "dur", "loopEndRec", LiSaMulti_ctrl_loop_end_rec );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "loopEndRec", LiSaMulti_cget_loop_end_rec);
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// loop_rec toggle set; for turning on/off loop recording
    func = make_new_mfun( "int", "loopRec", LiSaMulti_ctrl_loop_rec );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "loopRec", LiSaMulti_cget_loop_rec);
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// look at or put sample directly in record buffer, do not pass go
    func = make_new_mfun( "float", "valueAt", LiSaMulti_ctrl_sample );
    func->add_arg( "float", "val" );
	func->add_arg( "dur", "index" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "valueAt", LiSaMulti_cget_sample);
	func->add_arg( "dur", "index" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// set/get voiceGain
    func = make_new_mfun( "float", "voiceGain", LiSaMulti_ctrl_voicegain );
    func->add_arg( "int", "voice" );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "voiceGain", LiSaMulti_cget_voicegain);
    func->add_arg( "int", "voice" );
	if( !type_engine_import_mfun( env, func ) ) goto error;
    
	// set/get voicePan [value between 0 and numchans-1, to place voice]
    func = make_new_mfun( "float", "voicePan", LiSaMulti_ctrl_voicepan );
    func->add_arg( "int", "voice" );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "voicePan", LiSaMulti_cget_voicepan);
    func->add_arg( "int", "voice" );
	if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// set/get voicePan [value between 0 and numchans-1, to place voice]
    func = make_new_mfun( "float", "pan", LiSaMulti_ctrl_voicepan );
    func->add_arg( "int", "voice" );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "pan", LiSaMulti_cget_voicepan);
    func->add_arg( "int", "voice" );
	if( !type_engine_import_mfun( env, func ) ) goto error;
	
	func = make_new_mfun( "float", "pan", LiSaMulti_ctrl_voicepan0 );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "pan", LiSaMulti_cget_voicepan0);
	if( !type_engine_import_mfun( env, func ) ) goto error;
	
    // set record feedback coefficient
    func = make_new_mfun( "float", "feedback", LiSaMulti_ctrl_coeff );
    func->add_arg( "float", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "feedback", LiSaMulti_cget_coeff);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // clear buffer
    func = make_new_mfun( "void", "clear", LiSaMulti_ctrl_clear );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // get free voice
    func = make_new_mfun( "int", "getVoice", LiSaMulti_cget_voice );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // set maxvoices
    func = make_new_mfun( "int", "maxVoices", LiSaMulti_ctrl_maxvoices );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "maxVoices", LiSaMulti_cget_maxvoices);
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // ramp stuff
    func = make_new_mfun( "void", "rampUp", LiSaMulti_ctrl_rampup );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "void", "rampDown", LiSaMulti_ctrl_rampdown );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "dur", "recRamp", LiSaMulti_ctrl_rec_ramplen );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "void", "rampUp", LiSaMulti_ctrl_rampup0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    func = make_new_mfun( "void", "rampDown", LiSaMulti_ctrl_rampdown0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // get value
    func = make_new_mfun( "dur", "value", LiSaMulti_cget_value );
    func->add_arg( "int", "voice" );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "value", LiSaMulti_cget_value0 );
    func->add_arg( "dur", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // track
    func = make_new_mfun( "int", "track", LiSaMulti_ctrl_track );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "track", LiSaMulti_cget_track);
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// sync = track
    func = make_new_mfun( "int", "sync", LiSaMulti_ctrl_track );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "sync", LiSaMulti_cget_track);
    if( !type_engine_import_mfun( env, func ) ) goto error;
	
	// playing
    func = make_new_mfun( "int", "playing", LiSaMulti_cget_playing );
    func->add_arg( "int", "val" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    
    // end the class import
    type_engine_import_class_end( env );
    
    // multichannel
    if( !type_engine_import_ugen_begin( env, "LiSa10", "LiSa", env->global(),
                                       LiSaMulti_ctor, LiSaMulti_dtor,
                                       NULL, LiSaMulti_tickf,
                                       LiSaMulti_pmsg, 1, LiSa_channels ))
        return FALSE;
	
    type_engine_import_class_end( env );
    
    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}


//-----------------------------------------------------------------------------
// name: subgraph_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( subgraph_ctor )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    
    ugen->init_subgraph();
    
    if( ugen->inlet() )
    {
        OBJ_MEMBER_OBJECT(SELF, subgraph_offset_inlet) = ugen->inlet();
    }
    
    if( ugen->outlet() )
    {
        OBJ_MEMBER_OBJECT(SELF, subgraph_offset_outlet) = ugen->outlet();
    }
}


//-----------------------------------------------------------------------------
// name: FooGen_Data
// desc: ...
//-----------------------------------------------------------------------------
struct FooGen_Data
{
    Chuck_VM_Shred * shred;
    Chuck_VM * vm;
    
    t_CKFLOAT input;
    t_CKFLOAT output;
};


//-----------------------------------------------------------------------------
// name: foogen_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( foogen_ctor )
{
    FooGen_Data * data = new FooGen_Data;
    
    data->shred = NULL;
    data->vm = SHRED->vm_ref;
    
    // 1.3.1.0: changed from unsigned int to t_CKUINT
    OBJ_MEMBER_UINT(SELF, foogen_offset_data) = (t_CKUINT)data;

    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    int tick_fun_index = -1;
    
    for(int i = 0; i < ugen->vtable->funcs.size(); i++)
    {
        Chuck_Func * func = ugen->vtable->funcs[i];
        if(func->name.find("tick") == 0 && 
           // ensure has one argument
           func->def->arg_list != NULL &&
           // ensure first argument is float
           func->def->arg_list->type == SHRED->vm_ref->env()->t_float &&
           // ensure has only one argument
           func->def->arg_list->next == NULL &&
           // ensure returns float
           func->def->ret_type == SHRED->vm_ref->env()->t_float )
        {
            tick_fun_index = i;
            break;
        }
    }
    
    if( tick_fun_index != -1 )
    {
        vector<Chuck_Instr *> instrs;
        // push arg (float input)
        instrs.push_back(new Chuck_Instr_Reg_Push_Deref2( (t_CKUINT)&data->input ) );
        // push this (as func arg)
        instrs.push_back(new Chuck_Instr_Reg_Push_Imm((t_CKUINT)SELF) ); // 1.3.1.0: changed to t_CKUINT
        // reg dup last (push this again) (for member func resolution)
        instrs.push_back(new Chuck_Instr_Reg_Dup_Last);
        // dot member func
        instrs.push_back(new Chuck_Instr_Dot_Member_Func(tick_fun_index) );
        // func to code
        instrs.push_back(new Chuck_Instr_Func_To_Code);
        // push stack depth
        instrs.push_back(new Chuck_Instr_Reg_Push_Imm(12));
        // func call
        instrs.push_back(new Chuck_Instr_Func_Call());
        // push immediate
        instrs.push_back(new Chuck_Instr_Reg_Push_Imm((t_CKUINT)&data->output) ); // 1.3.1.0: changed to t_CKUINT
        // assign primitive
        instrs.push_back(new Chuck_Instr_Assign_Primitive2);
        // pop
        instrs.push_back(new Chuck_Instr_Reg_Pop_Word2);
        // EOC
        instrs.push_back(new Chuck_Instr_EOC);
        
        Chuck_VM_Code * code = new Chuck_VM_Code;
        
        code->instr = new Chuck_Instr*[instrs.size()];
        code->num_instr = instrs.size();
        for(int i = 0; i < instrs.size(); i++) code->instr[i] = instrs[i];
        code->stack_depth = 0;
        code->need_this = 0;
        
        data->shred = new Chuck_VM_Shred;
        data->shred->vm_ref = SHRED->vm_ref;
        data->shred->initialize(code);
    }
    else
    {
        // SPENCERTODO: warn on Chugen definition instead of instantiation?
        EM_log(CK_LOG_WARNING, "ChuGen '%s' does not define a suitable tick function",
               ugen->type_ref->name.c_str());
    }
}


//-----------------------------------------------------------------------------
// name: foogen_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( foogen_dtor )
{
    FooGen_Data * data = (FooGen_Data *) OBJ_MEMBER_UINT(SELF, foogen_offset_data);
    OBJ_MEMBER_UINT(SELF, foogen_offset_data) = 0;
    SAFE_DELETE(data);
}

//-----------------------------------------------------------------------------
// name: foogen_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( foogen_tick )
{
    FooGen_Data * data = (FooGen_Data *) OBJ_MEMBER_UINT(SELF, foogen_offset_data);
        
    // default: passthru
    data->output = in;
    
    if(data->shred)
    {
        // reset shred
        
        // program counter
        data->shred->pc = 0;
        data->shred->next_pc = 1;
        // shred in dump (all done)
        data->shred->is_dumped = FALSE;
        // shred done
        data->shred->is_done = FALSE;
        // shred running
        data->shred->is_running = FALSE;
        // shred abort
        data->shred->is_abort = FALSE;
        // set the instr
        data->shred->instr = data->shred->code->instr;
        // zero out the id
        data->shred->xid = 0;
        // set vmRef
        data->shred->vm_ref = data->vm;
        
        // set input
        data->input = in;
        
        // run shred
        data->shred->run( data->vm );
    }
    
    *out = data->output;
    
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: multi_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( multi_ctor )
{
    // do nothing
}




//-----------------------------------------------------------------------------
// name: multi_cget_chan()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( multi_cget_chan )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    // value
    t_CKINT index = GET_NEXT_INT( ARGS );
    // return
    RETURN->v_object = index >= 0 && index < ugen->m_multi_chan_size ?
        ugen->m_multi_chan[index] : NULL ;
}




//-----------------------------------------------------------------------------
// name: stereo_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( stereo_ctor )
{
    // get ugen
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;

    // multi
    if( ugen->m_multi_chan_size )
    {
        // set left
        OBJ_MEMBER_UINT(SELF, stereo_offset_left) = (t_CKUINT)(ugen->m_multi_chan[0]);
        // set right
        OBJ_MEMBER_UINT(SELF, stereo_offset_right) = (t_CKUINT)(ugen->m_multi_chan[1]);
    }
    else // mono
    {
        // set left and right to self
        OBJ_MEMBER_UINT(SELF, stereo_offset_left) = (t_CKUINT)ugen;
        OBJ_MEMBER_UINT(SELF, stereo_offset_right) = (t_CKUINT)ugen;
    }
}




//-----------------------------------------------------------------------------
// name: stereo_ctrl_pan()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( stereo_ctrl_pan )
{
    Chuck_UGen * ugen = (Chuck_UGen * )SELF;
    Chuck_UGen * left = ugen->m_multi_chan[0];
    Chuck_UGen * right = ugen->m_multi_chan[1];
    // get arg
    t_CKFLOAT pan = GET_CK_FLOAT(ARGS);
    // clip it
    if( pan < -1.0 ) pan = -1.0;
    else if( pan > 1.0 ) pan = 1.0;
    // set it
    OBJ_MEMBER_FLOAT(SELF, stereo_offset_pan) = pan;
    // pan it
    left->m_pan = pan < 0.0 ? 1.0 : 1.0 - pan;
    right->m_pan = pan > 0.0 ? 1.0 : 1.0 + pan;

    RETURN->v_float = pan;
}




//-----------------------------------------------------------------------------
// name: stereo_cget_pan()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( stereo_cget_pan )
{
    RETURN->v_float = OBJ_MEMBER_FLOAT(SELF, stereo_offset_pan);
}




//-----------------------------------------------------------------------------
// name: dac_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( dac_tick )
{
    *out = in; return TRUE;
}




//-----------------------------------------------------------------------------
// name: bunghole_tick
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( bunghole_tick )
{
    *out = 0.0f; return 0;
}




//-----------------------------------------------------------------------------
// name: noise_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( noise_tick )
{
    *out = -1.0 + 2.0 * (SAMPLE)rand() / RAND_MAX;
    return TRUE;
}


enum { NOISE_WHITE=0, NOISE_PINK, NOISE_BROWN, NOISE_FBM, NOISE_FLIP, NOISE_XOR };

class CNoise_Data
{
private:
  SAMPLE value;

  t_CKFLOAT fbmH;
  t_CKINT counter;
  t_CKINT * pink_array;
  t_CKINT  pink_depth;
  bool pink_rand;
  t_CKINT rand_bits;
  double scale;
  double bias;

  t_CKINT last;

public:
  CNoise_Data() { 
    value = 0; 
    mode = NOISE_PINK; 
    pink_depth = 24;
    pink_array = NULL;
    counter = 1;
    scale = 2.0 / (double) RAND_MAX ;
    bias = -1.0;
    pink_rand = false;
    t_CKINT randt = RAND_MAX;
    rand_bits = 0;
    fprob = (t_CKINT)( (double)RAND_MAX * 1.0 / 32.0 );
    while ( randt > 0 ) { 
      rand_bits++;
      randt = randt >> 1;
    }
    // CK_FPRINTF_STDERR( "random bits - %d", rand_bits );
    setMode ( "pink" );
  } 
  ~CNoise_Data() {}
   
  t_CKINT fprob;
  t_CKUINT mode;
  void tick( t_CKTIME now, SAMPLE * out );
  void setMode( const char * c );

  t_CKINT pink_tick( SAMPLE * out);
  t_CKINT brown_tick( SAMPLE * out);
  t_CKINT xor_tick( SAMPLE * out);
  t_CKINT flip_tick( SAMPLE * out);
  t_CKINT fbm_tick( SAMPLE * out);
};


CK_DLL_CTOR( cnoise_ctor )
{
    OBJ_MEMBER_UINT(SELF, cnoise_offset_data) = (t_CKUINT)new CNoise_Data;
}

CK_DLL_DTOR( cnoise_dtor )
{
    delete (CNoise_Data *)OBJ_MEMBER_UINT(SELF, cnoise_offset_data);
    OBJ_MEMBER_UINT(SELF, cnoise_offset_data) = 0;
}

CK_DLL_TICK( cnoise_tick )
{
  CNoise_Data * d = ( CNoise_Data * )OBJ_MEMBER_UINT(SELF, cnoise_offset_data);
  switch( d->mode ) { 
  case NOISE_WHITE: 
    return noise_tick(SELF,in,out,SHRED,API);
    break;
  case NOISE_PINK:
    return d->pink_tick(out);
    break;
  case NOISE_BROWN:
    return d->brown_tick(out);
    break;
  case NOISE_XOR:
    return d->xor_tick(out);
    break;
  case NOISE_FLIP:
    return d->flip_tick(out);
    break;
  case NOISE_FBM:
    return d->fbm_tick(out);
    break;
  }
  return TRUE;
}

t_CKINT CNoise_Data::pink_tick( SAMPLE * out )
{ 
  //based on Voss-McCartney

  if ( pink_array == NULL ) { 
    pink_array = (t_CKINT *) malloc ( sizeof ( t_CKINT ) * pink_depth );
    last = 0;
    for ( t_CKINT i = 0 ; i < pink_depth ; i++ ) { pink_array[i] = rand(); last += pink_array[i]; } 
    scale = 2.0 / ((double)RAND_MAX  * ( pink_depth + 1.0 ) );
    bias = 0.0;
    // CK_FPRINTF_STDERR( "scale %f %f %d %d \n", scale, bias, RAND_MAX, pink_depth + 1 );
  }

  t_CKINT pind = 0;

  //count trailing zeroes
  while ( pind < pink_depth && ! (counter & ( 1 << pind ) ) ) pind++;

  //  fprintf (stderr, "counter %d pink - %d \n", counter, pind );

  if ( pind < pink_depth ) { 
    t_CKINT diff = rand() - pink_array[pind];
    pink_array[pind] += diff;
    last += diff;
  }

  *out = bias + scale * ( rand() + last );
  counter++;
  if ( pink_rand ) counter = rand();
  return TRUE;
}

t_CKINT CNoise_Data::xor_tick( SAMPLE * out )
{ 
  t_CKINT mask = 0;
  for ( t_CKINT i = 0; i < rand_bits ; i++ ) 
    if ( rand() <= fprob ) 
      mask |= ( 1 << i );
  last = last ^ mask;  
  *out = bias + scale * (SAMPLE)last;
  return TRUE;
}

t_CKINT CNoise_Data::flip_tick( SAMPLE * out )
{
  t_CKINT ind = (t_CKINT) ( (double) rand_bits * rand() / ( RAND_MAX + 1.0 ) );
 
  last = last ^ ( 1 << ind );
  //  fprintf ( stderr, "ind - %d %d %f %f", ind, last, bias, scale );
  *out = bias + scale * (SAMPLE)last;
  return TRUE;
}

t_CKINT
CNoise_Data::brown_tick( SAMPLE * out ) { 
  //brownian noise function..later!
  *out = 0;
  return TRUE;
}

t_CKINT
CNoise_Data::fbm_tick( SAMPLE * out ) { 
  //non-brownian noise function..later!
  *out = 0;
  return TRUE;
}

void
CNoise_Data::setMode( const char * c ) { 
  if ( strcmp ( c, "white" ) == 0 ) { 
    // CK_FPRINTF_STDERR( "white noise\n" );
    mode = NOISE_WHITE;    
    scale = 2.0 / (t_CKFLOAT)RAND_MAX;
    bias = -1.0;
  }
  if ( strcmp ( c, "pink" ) == 0 ) { 
    // CK_FPRINTF_STDERR( "pink noise\n" );
    mode = NOISE_PINK;
    scale = 2.0 / (double)(RAND_MAX  * ( pink_depth + 1 ) );
    bias = -1.0;
  }
  if ( strcmp ( c, "flip" ) == 0) { 
    // CK_FPRINTF_STDERR( "bitflip noise\n" );
    mode = NOISE_FLIP;
    scale = 2.0 / (t_CKFLOAT)RAND_MAX;
    bias = -1.0;
  }
  if ( strcmp ( c, "xor" ) == 0) {
    // CK_FPRINTF_STDERR( "xor noise\n" );
    mode = NOISE_XOR;
    scale = 2.0 / (t_CKFLOAT)RAND_MAX;
    bias = -1.0;
  }
  if ( strcmp ( c, "brown" ) == 0) { 
    // CK_FPRINTF_STDERR( "brownian noise\n" );
    mode = NOISE_BROWN;
    scale = 2.0 / (t_CKFLOAT)RAND_MAX;
    bias = -1.0;
  }
  if ( strcmp ( c, "fbm" ) == 0) {   
    // CK_FPRINTF_STDERR( "fbm noise\n" );
    mode = NOISE_FBM;
    scale = 2.0 / (t_CKFLOAT)RAND_MAX;
    bias = -1.0;
  }

}

CK_DLL_CTRL( cnoise_ctrl_mode )
{
    CNoise_Data * d = ( CNoise_Data * )OBJ_MEMBER_UINT(SELF, cnoise_offset_data);
    //const char * mode= (const char *)*(char **)GET_CK_STRING(ARGS);
    const char * mode= GET_NEXT_STRING(ARGS)->str().c_str();
    d->setMode(mode);
}

CK_DLL_CTRL( cnoise_ctrl_fprob )
{
    CNoise_Data * d = ( CNoise_Data * )OBJ_MEMBER_UINT(SELF, cnoise_offset_data);
    t_CKFLOAT p= GET_CK_FLOAT(ARGS);
    d->fprob = (t_CKINT) ( (double)RAND_MAX * p );
}



//-----------------------------------------------------------------------------
// name: struct Pulse_Data
// desc: ...
//-----------------------------------------------------------------------------
struct Pulse_Data
{
    SAMPLE value;
    t_CKUINT when;
    Pulse_Data( ) { value = 1.0f; when = 0; }
};


//-----------------------------------------------------------------------------
// name: impulse_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( impulse_ctor )
{
    // return data to be used later
    OBJ_MEMBER_UINT(SELF, impulse_offset_data) = (t_CKUINT)new Pulse_Data;
}




//-----------------------------------------------------------------------------
// name: impulse_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( impulse_dtor )
{
    // delete
    delete (Pulse_Data *)OBJ_MEMBER_UINT(SELF, impulse_offset_data);
    OBJ_MEMBER_UINT(SELF, impulse_offset_data) = 0;
}




//-----------------------------------------------------------------------------
// name: impulse_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( impulse_tick )
{
    Pulse_Data * d = (Pulse_Data *)OBJ_MEMBER_UINT(SELF, impulse_offset_data);
    if( d->when )
    {
        *out = d->value;
        d->when = 0;
    }
    else
        *out = 0.0f;
    
    return TRUE;
}



//-----------------------------------------------------------------------------
// name: impulse_ctrl_next()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( impulse_ctrl_next )
{
    Pulse_Data * d = (Pulse_Data *)OBJ_MEMBER_UINT(SELF, impulse_offset_data);
    d->value = (SAMPLE)GET_CK_FLOAT(ARGS);
    d->when = 1;
    RETURN->v_float = (t_CKFLOAT)d->value;
}




//-----------------------------------------------------------------------------
// name: impulse_cget_next()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( impulse_cget_next )
{
    Pulse_Data * d = (Pulse_Data *)OBJ_MEMBER_UINT(SELF, impulse_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->value;
}




//-----------------------------------------------------------------------------
// name: step_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( step_ctor )
{
    // return data to be used later
    OBJ_MEMBER_UINT(SELF, step_offset_data) = (t_CKUINT)new SAMPLE( 1.0f );
}




//-----------------------------------------------------------------------------
// name: step_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( step_dtor )
{
    // delete
    delete (SAMPLE *)OBJ_MEMBER_UINT(SELF, step_offset_data);
    OBJ_MEMBER_UINT(SELF, step_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: step_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( step_tick )
{
    SAMPLE * d = (SAMPLE *)OBJ_MEMBER_UINT(SELF, step_offset_data);
    *out = *d;
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: step_ctrl_next()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( step_ctrl_next )
{
    SAMPLE * d = (SAMPLE *)OBJ_MEMBER_UINT(SELF, step_offset_data);
    *d = (SAMPLE)GET_CK_FLOAT(ARGS);
    RETURN->v_float = (t_CKFLOAT)(*d);
}


//-----------------------------------------------------------------------------
// name: step_cget_next()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CGET( step_cget_next )
{
    SAMPLE * d = (SAMPLE *)OBJ_MEMBER_UINT(SELF, step_offset_data);
    RETURN->v_float = (t_CKFLOAT)(*d);
}




//-----------------------------------------------------------------------------
// name: halfrect_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( halfrect_tick )
{
    *out = in > 0.0f ? in : 0.0f;
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: fullrect_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( fullrect_tick )
{
    *out = in >= 0.0f ? in : -in;
    return TRUE;
}




/*
//-----------------------------------------------------------------------------
// name: zerox_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( zerox_ctor )
{
    OBJ_MEMBER_UINT(SELF, zerox_offset_data) = (t_CKUINT)new SAMPLE( 0.0f );
}


//-----------------------------------------------------------------------------
// name: zerox_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( zerox_dtor )
{
    delete (SAMPLE *)OBJ_MEMBER_UINT(SELF, zerox_offset_data);
    OBJ_MEMBER_UINT(SELF, zerox_offset_data) = 0;
}


#define __SGN(x)  (x >= 0.0f ? 1.0f : -1.0f )
//-----------------------------------------------------------------------------
// name: zerox_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( zerox_tick )
{
    SAMPLE * d = (SAMPLE *)OBJ_MEMBER_UINT(SELF, zerox_offset_data);
    *out = __SGN(in) != __SGN(*d);
    *out *= __SGN(in);
    *d = in;
    
    return TRUE;
}
*/




struct delayp_data 
{ 

    SAMPLE *    buffer;
    t_CKINT     bufsize;
    
    t_CKTIME    now;

    double      readpos;   //readpos ( moves at constant speed, sample per sample
    double      writepos; // relative to read position
    
    t_CKTIME    offset; // distance between read and write
    
    t_CKDUR     offset_start; 
    t_CKDUR     offset_target;

    t_CKTIME    move_end_time; //target time
    t_CKDUR     move_duration; //time we started shift
    
    SAMPLE      last_sample;
    t_CKDUR     last_offset;
    
#ifdef _DEBUG
    int         lasti;
#endif

    double      acoeff[2];
    double      bcoeff[2];
    SAMPLE      outputs[3];
    SAMPLE      inputs[3];
    
    delayp_data() 
    { 
        bufsize  = 2 * g_srate;
        buffer   = ( SAMPLE * ) realloc ( NULL, sizeof ( SAMPLE ) * bufsize );
        t_CKINT i;
        
        for ( i = 0 ; i < bufsize ; i++ ) buffer[i] = 0;
        for ( i = 0 ; i < 2 ; i++ ) { acoeff[i] = 0; bcoeff[i] = 0; }
        
        acoeff[0] = 1.0;
        acoeff[1] = -.99;
        bcoeff[0] = 1.0;
        bcoeff[1] = -1.0;
        
        readpos  = 0.0;        

        now             = 0.0;

        offset          = 0.0; 
        last_offset     = 0.0;
        offset_start    = 0.0;
        offset_target   = 0.0;

        move_duration   = 1.0;
        move_end_time   = 0.0;

#ifdef _DEBUG
        lasti       = -1;
#endif
        last_sample = 0;
    }

    ~delayp_data()
    {
        SAFE_DELETE_ARRAY( buffer );
    }
};

CK_DLL_CTOR( delayp_ctor )
{
    OBJ_MEMBER_UINT(SELF, delayp_offset_data) = (t_CKUINT)new delayp_data;    
}

CK_DLL_DTOR( delayp_dtor )
{
    delayp_data * d = (delayp_data *)OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    SAFE_DELETE(d);
    OBJ_MEMBER_UINT(SELF, delayp_offset_data) = 0;
}

CK_DLL_PMSG( delayp_pmsg )
{
    return TRUE;
}

CK_DLL_TICK( delayp_tick )
{
    delayp_data * d = (delayp_data *)OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    if ( !d->buffer ) return FALSE;

    // area
    d->now = ((Chuck_UGen*)SELF)->shred->vm_ref->shreduler()->now_system;
    
    //calculate new write-offset position ( we interpolate if we've been assigned a new write-offset )
    if ( d->now >= d->move_end_time || d->move_duration == 0 ) d->offset = d->offset_target;
    else  { 
        double dt = 1.0 + ( d->now - d->move_end_time ) / ( d->move_duration ); 
        d->offset = d->offset_start + dt * ( d->offset_target - d->offset_start );
        // fprintf (stderr, "dt %f, off %f , start %f target %f\n", dt, d->writeoff,  d->writeoff_start, d->writeoff_target );
    }
    
    //find locations in buffer...
    double write        = (d->readpos )         + d->offset;
    double last_write   = (d->readpos - 1.0)    + d->last_offset ;
    
    //linear interpolation.  will introduce some lowpass/aliasing.
    double write_delta  = write - last_write;
    double sample_delta = in - d->last_sample;

    double duck_constant = 0.69;

    
    double gee = fabs(write_delta) - 1.0;

    if ( gee < 24.0 ) { 
        double head_contact = ( gee > 0 ) ? exp ( - duck_constant * gee ) : 1.0;
        t_CKINT i, smin, smax, sampi;
        SAMPLE sampf = 0;
        if ( write_delta >= 0 ) { //forward.
            smin = (t_CKINT) floor ( last_write );
            smax = (t_CKINT) floor ( write );
            for ( i = smin+1 ; i <= smax ; i++ ) { 
                sampf = d->last_sample + sample_delta * ( double(i) - last_write ) / write_delta;
                sampi = ( i + d->bufsize * 2 ) % d->bufsize;
#ifdef _DEBUG 
                if ( d->lasti == sampi ) { 
                    CK_FPRINTF_STDERR( "[chuck](via Curve): over!\n");
                }
                d->lasti = sampi;
#endif
                d->buffer[sampi] += sampf * head_contact ;
            }
        }
        else { //moving in reverse
            smin = (t_CKINT) floor ( write );
            smax = (t_CKINT) floor ( last_write );
            for ( i = smin+1 ; i <= smax ; i++ ) { 
                sampf = d->last_sample + sample_delta * ( double(i) - last_write ) / write_delta;
                sampi = ( i + d->bufsize * 2 ) % d->bufsize;
#ifdef _DEBUG
                if ( d->lasti == sampi ) { 
                    CK_FPRINTF_STDERR( "[chuck](via Curve): over!\n" );
                }
                d->lasti = sampi;
#endif
                d->buffer[sampi] += sampf * head_contact;   
            }
        }
    }
    
    d->last_offset = d->offset;
    d->last_sample = in;
    
    
    //output should go through a dc blocking filter, for cases where 
    //we are zipping around in the buffer leaving a fairly constant
    //trail of samples
    
    //output last sample
    
    t_CKINT rpos = (t_CKINT) fmod( d->readpos, d->bufsize ) ; 
    
    //   *out = d->buffer[rpos];

    /*    
    // did i try to write a dc blocking filter? 
    d->outputs[0] =  0.0;
    d->inputs [0] =  d->buffer[rpos];

    d->outputs[0] += d->bcoeff[1] * d->inputs[1];
    d->inputs [1] =  d->inputs[0];
    
    d->outputs[0] += d->bcoeff[0] * d->inputs[0];
    
    d->outputs[0] += -d->acoeff[1] * d->outputs[1];
    d->outputs[1] =  d->outputs[0];

    //clear at readpos ( write doesn't !)
    *out = d->outputs[0];
    */

    *out = d->buffer[rpos];
    
    d->buffer[rpos] = 0; //clear once it's been read
    d->readpos = fmod ( d->readpos + 1.0 , double( d->bufsize ) );
 
    return TRUE;
    
}

CK_DLL_CTRL( delayp_ctrl_delay )
{
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    t_CKDUR target = GET_CK_DUR(ARGS); // rate     
    // area

    if ( target != d->offset_target ) {
        if ( target > d->bufsize ) { 
            CK_FPRINTF_STDERR( "[chuck](via delayp): delay time %f over max!  set max first!\n", target);
            return;
        }
        d->offset_target = target;
        d->offset_start  = d->last_offset;

        t_CKTIME snow = ((Chuck_UGen*)SELF)->shred->now;
        d->move_end_time = snow + d->move_duration; 
    }
    RETURN->v_dur = d->last_offset; // TODO:
}

CK_DLL_CGET( delayp_cget_delay )
{
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    //SET_NEXT_DUR( out, d->writeoff_last );
    RETURN->v_dur = d->last_offset; // TODO:
}

CK_DLL_CTRL( delayp_ctrl_window )
{
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    t_CKDUR window = GET_CK_DUR(ARGS); // rate     
    if ( window >= 0 ) {
        d->move_duration = window;
        //fprintf ( stderr, "set window time %f , %f , %d \n", d->writeoff_window_time, d->writeoff, d->bufsize );
    }
    RETURN->v_dur = d->move_duration; // TODO:
}

CK_DLL_CGET( delayp_cget_window )
{
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    RETURN->v_dur = d->move_duration; // TODO:
}


CK_DLL_CTRL( delayp_ctrl_max )
{   
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    t_CKDUR nmax = GET_CK_DUR(ARGS); // rate 
    if ( d->bufsize != (t_CKINT)nmax && nmax > 1.0 ) { 
        d->bufsize = (t_CKINT)(nmax+.5); 
        d->buffer = ( SAMPLE * ) realloc ( d->buffer, sizeof ( SAMPLE ) * d->bufsize );
        for ( t_CKINT i = 0; i < d->bufsize; i++ ) d->buffer[i] = 0;
    }
    RETURN->v_dur = d->bufsize; // TODO??
}

CK_DLL_CGET( delayp_cget_max )
{
    delayp_data * d = ( delayp_data * ) OBJ_MEMBER_UINT(SELF, delayp_offset_data);
    //SET_NEXT_DUR( out, (t_CKDUR) d->bufsize );
    RETURN->v_dur = d->bufsize;
}


#ifndef __DISABLE_SNDBUF__

//-----------------------------------------------------------------------------
// name: sndbuf
// desc: ...
//-----------------------------------------------------------------------------
enum { SNDBUF_DROP = 0, SNDBUF_INTERP, SNDBUF_SINC};


#define WIDTH 16                /* this controls the number of neighboring samples
which are used to interpolate the new samples.  The
processing time is linearly related to this width */
#define DELAY_SIZE 140

// default chunk size
#define CK_SNDBUF_DEFAULT_CHUNK_SIZE (32768) // a little less than 1s of 44.1kHz

#define USE_TABLE TRUE          /* this controls whether a linearly interpolated lookup
table is used for sinc function calculation, or the
sinc is calculated by floating point trig function calls.  */

#define USE_INTERP TRUE        /*  this controls whether the table is linear
interpolated or not.  If you re not using the
table, this has no effect         */

#define SAMPLES_PER_ZERO_CROSSING 32    /* this defines how finely the sinc function 
is sampled for storage in the table  */

#ifdef CK_SNDBUF_MEMORY_BUFFER
//------------------------------------------------------------------------------
// name: MultiBuffer
// desc: presents multiple buffers in memory as a single sequential logical
// buffer, for constant time reallocing.  tries to be as discreet as possible,
// so you can basically use it like a pointer.  optimized for iterative access,
// as random access has T = O(bufsize)
//------------------------------------------------------------------------------
template< class _type >
class MultiBuffer
{

public:
    MultiBuffer()
    {
        m_size = 0;
    }
    
    ~MultiBuffer()
    {
        deallocate();
    }
    
    size_t size()
    {
        return m_size;
    }
    
    void resize( size_t size )
    {
        assert( size > m_size ); // for now
        size_t new_buffer_size = size - m_size;
        
        extent new_extent;
        new_extent.position = new _type[new_buffer_size];
        new_extent.length = new_buffer_size;
        m_bufs.push_back( new_extent );
        m_size = size;
    }
    
    void deallocate()
    {
        // delete everything
        size_t i, len = m_bufs.size();
        for( i = 0; i < len; i++ )
            delete[] m_bufs[i].position;
        m_size = 0;
    }
    
    class Pointer
    {
    public:
        Pointer()
        {
            m_mb = NULL;
            m_lpos = 0;
            m_extent = 0;
            m_extpos = 0;
        }
        
        Pointer( MultiBuffer * mb )
        {
            m_mb = mb;
            m_lpos = 0;
            m_extent = 0;
            m_extpos = 0;
        }
        
        Pointer( const Pointer & mbp )
        {
            m_mb = mbp.m_mb;
            m_lpos = mbp.m_lpos;
            m_extent = mbp.m_extent;
            m_extpos = mbp.m_extpos;      
        }
        
        Pointer & operator=( MultiBuffer & mb )
        {
            m_mb = &mb;
            m_lpos = 0;
            m_extent = 0;
            m_extpos = 0;
            
            return *this;
        }
        
        Pointer & operator=( const Pointer & mbp )
        {
            m_mb = mbp.m_mb;
            m_lpos = mbp.m_lpos;
            m_extent = mbp.m_extent;
            m_extpos = mbp.m_extpos;
            
            return *this;
        }
        
        Pointer & operator=( size_t i )
        {
            reset();
            increment( i );
            return *this;
        }

        _type & operator*()
        {
            return m_mb->m_bufs[m_extent].position[m_extpos];
        }
        
        Pointer operator+( size_t i ) const
        {
            Pointer mbp( *this );
            mbp.increment( i );
            return mbp;
        }
        
        Pointer operator++( int )
        {
            Pointer mbp = *this;
            this->increment( 1 );
            return mbp;
        }
        
        bool operator>=( const Pointer & mbp ) const
        {
            return ( m_lpos >= mbp.m_lpos );
        }
        
        bool operator>=( const size_t i ) const
        {
            return ( m_lpos >= i );
        }
        
        void increment( size_t i )
        {
            m_lpos += i;
            if( m_lpos >= (m_mb->m_size) )
            {
                m_extent = m_mb->m_bufs.size();
                m_extpos = m_lpos - m_mb->m_size;
                return;
            }
            
            extent ext_current = m_mb->m_bufs[m_extent];
            i += m_extpos;
            while( i >= ext_current.length )
            {
                i -= ext_current.length;
                m_extent++;
                ext_current = m_mb->m_bufs[m_extent];
            }
            
            m_extpos = i;
        }
        
        void reset()
        {
            m_lpos = 0;
            m_extent = 0;
            m_extpos = 0;
        }
        
    protected:
        MultiBuffer * m_mb;

        size_t m_lpos; // position in logical buffer
        size_t m_extent; // current extent
        size_t m_extpos; // position within the extent
    };
    
protected:
    
    struct extent
    {
        _type * position;
        size_t length;
    };
    
    /* data shared by a set of MultiBuffers */
    vector< extent > m_bufs; // array of sequentially allocated buffers
    size_t m_size; // overall size of total memory represented
};
#endif /* CK_SNDBUF_MEMORY_BUFFER */

// data for each sndbuf
struct sndbuf_data
{
    SAMPLE * buffer;
    t_CKUINT num_samples;
    t_CKUINT num_channels;
    t_CKUINT num_frames;
    t_CKUINT samplerate;
    t_CKUINT chan;
    
    t_CKUINT chunks;
    t_CKUINT chunks_read;
    t_CKUINT chunk_num;
    SAMPLE ** chunk_map;

    SAMPLE * eob;
    //SAMPLE * curr;
    SAMPLE current_val;
    t_CKFLOAT sampleratio;
    t_CKFLOAT curf;
    t_CKFLOAT rate_factor;
    t_CKFLOAT rate;
    t_CKINT interp;
    t_CKBOOL loop;
    
    bool sinc_table_built;
    bool sinc_use_table;
    bool sinc_use_interp;
    t_CKINT sinc_samples_per_zero_crossing;
    t_CKINT sinc_width;
    double * sinc_table;

#ifdef CK_SNDBUF_MEMORY_BUFFER
    MultiBuffer< SAMPLE > mb_buffer;
    t_CKUINT mb_max_samples;
    MultiBuffer< SAMPLE >::Pointer mb_record_position;
    MultiBuffer< SAMPLE >::Pointer mb_playback_position;
#endif /* CK_SNDBUF_MEMORY_BUFFER */

    SNDFILE * fd;

    // constructor
    sndbuf_data()
    {
        buffer = NULL;
        interp = SNDBUF_INTERP;
        num_channels = 0;
        num_frames = 0;
        num_samples = 0;
        chunks = CK_SNDBUF_DEFAULT_CHUNK_SIZE;
        chunks_read = 0;
        samplerate = 0;
        sampleratio = 1.0;
        chan = 0;
        curf = 0.0;
        rate_factor = 1.0;
        rate = 1.0;
        eob = NULL;
        current_val = 0.0;
        chunk_map = NULL;
        chunk_num = 0;
        
        sinc_table_built = false;
        sinc_use_table = USE_TABLE;
        sinc_use_interp = USE_INTERP;
        sinc_width = WIDTH;
        sinc_samples_per_zero_crossing = SAMPLES_PER_ZERO_CROSSING;
        sinc_table = NULL;
        
#ifdef CK_SNDBUF_MEMORY_BUFFER
        mb_buffer = MultiBuffer< SAMPLE >();
#endif /* CK_SNDBUF_MEMORY_BUFFER */
        
        loop = FALSE;
        fd = NULL;
    }

    ~sndbuf_data()
    {
        SAFE_DELETE_ARRAY( buffer );
        
        if( chunk_map )
        {
            for(int i = 0; i < chunk_num; i++)
                SAFE_DELETE_ARRAY(chunk_map[i]);
            SAFE_DELETE_ARRAY(chunk_map);
        }
    }
    
    inline void sampleIndex2FrameIndexAndChannel(t_CKINT sample, t_CKINT *frame, t_CKINT *channel)
    {
        *frame = (t_CKINT) floorf(sample/this->num_channels);
        *channel = sample%this->num_channels;
    }
};


double sndbuf_sinc( sndbuf_data * d, double x );
double sndbuf_t_sinc( sndbuf_data * d, double x );
void   sndbuf_make_sinc( sndbuf_data * d );
void   sndbuf_sinc_interpolate( sndbuf_data * d, SAMPLE * out );

CK_DLL_CTOR( sndbuf_ctor )
{
    OBJ_MEMBER_UINT(SELF, sndbuf_offset_data) = (t_CKUINT)new sndbuf_data;

}

CK_DLL_DTOR( sndbuf_dtor )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    SAFE_DELETE(d);
    OBJ_MEMBER_UINT(SELF, sndbuf_offset_data) = 0;
}

inline t_CKUINT sndbuf_read( sndbuf_data * d, t_CKUINT frame, t_CKUINT num_frames )
{
    // check
    if( d->fd == NULL ) return 0;
    if( frame >= d->num_frames ) return 0;
    
    // log
    // EM_log( CK_LOG_FINE, "(sndbuf): reading %d:%d frames...", offset, howmuch );

    // prevent overflow
    if( num_frames > d->num_frames - frame )
        num_frames = d->num_frames - frame;

    t_CKUINT n;
    // seek
    sf_seek( d->fd, frame, SEEK_SET );
    t_CKUINT index = frame*d->num_channels;
#if defined(__CHUCK_USE_64_BIT_SAMPLE__)
    assert(0); // SPENCERTODO
    n = sf_readf_double( d->fd, d->buffer+frame*d->num_channels, num_frames );
#else
    if(d->buffer)
        n = sf_readf_float( d->fd, d->buffer+frame*d->num_channels, num_frames );
    else
        n = sf_readf_float( d->fd, d->chunk_map[index/d->chunks], num_frames );
#endif

    d->chunks_read += n*d->num_channels;

    // close
    if( d->chunks_read >= d->num_frames*d->num_channels )
    {
        // log
        EM_log( CK_LOG_INFO, "(sndbuf): all frames read, closing file..." );
        sf_close( d->fd );
        d->fd = NULL;
    }

    return n;
}

inline t_CKINT sndbuf_load( sndbuf_data * d, t_CKUINT sample )
{
    // map to bin
    t_CKUINT bin = floor(((t_CKFLOAT) sample) / ((t_CKFLOAT) d->chunks));
    
    assert(bin < d->chunk_num);
    
    // already loaded
    if( d->chunk_map[bin] ) return 0;
    
    // allocate
    d->chunk_map[bin] = new SAMPLE[d->chunks];

    // read it
    t_CKINT ret = sndbuf_read( d, bin*d->chunks/d->num_channels, d->chunks/d->num_channels );
    
    // log
    // EM_log( CK_LOG_FINER, "chunk test: pos: %d bin: %d read:%d/%d", where, bin, d->chunks_read, d->num_frames );
//    CK_FPRINTF_STDERR( "chunks: loaded bin %lu\n", bin );

    return ret;
}

inline void sndbuf_setpos( sndbuf_data *d, double frame_pos )
{
    if( !(d->buffer || d->chunk_map) ) return;

    d->curf = frame_pos;

    // set curf within bounds
    if( d->loop )
    {
        while( d->curf >= d->num_frames ) d->curf -= d->num_frames;
        while( d->curf < 0 ) d->curf += d->num_frames;
    } 
    else
    {
        if( d->curf < 0 ) d->curf = 0;
        else if( d->curf >= d->num_frames )
        {
            d->curf = d->num_frames; // ge:
            d->current_val = 0;
            return;
        }
    }
    
    t_CKUINT index = d->chan + ((t_CKINT)d->curf) * d->num_channels;
    // ensure load
    if( d->fd != NULL ) sndbuf_load( d, index );
    
    // sets curr to correct position ( account for channels )
//    t_CKUINT index = d->chan + i * d->num_channels;
    if(d->buffer)
        d->current_val = d->buffer[index];
    else
        d->current_val = d->chunk_map[index/d->chunks][index%d->chunks];
}

inline SAMPLE sndbuf_sampleAt( sndbuf_data * d, t_CKINT frame_pos, t_CKINT arg_chan = -1 )
{
    // boundary cases
    t_CKINT nf = d->num_frames;
    if( d->loop ) { 
        while( frame_pos >= nf ) frame_pos -= nf;
        while( frame_pos <  0 ) frame_pos += nf;
    }
    else { 
        if( frame_pos >= nf ) frame_pos = nf-1;
        if( frame_pos < 0 ) frame_pos = 0;
    }
    
    // if specific channel was requested, use that one
    t_CKUINT chan = 0;
    // I love sentinal values
    if(arg_chan < 0)
        chan = d->chan;
    else if(arg_chan < d->num_channels)
        chan = arg_chan;
    
    t_CKUINT index = chan + frame_pos * d->num_channels;
    // ensure load
    if( d->fd != NULL ) sndbuf_load( d, index );
    
    // return sample
    if(d->buffer != NULL)
        return d->buffer[index];
    else
        return d->chunk_map[index/d->chunks][index%d->chunks];
}

inline double sndbuf_getpos( sndbuf_data * d )
{
    if( !(d->buffer || d->chunk_map) ) return 0;
    return floor(d->curf);
}

CK_DLL_CTRL( sndbuf_ctrl_loop )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    d->loop = GET_CK_INT(ARGS);
    RETURN->v_int = d->loop; // TODO: Check (everything)
} 

CK_DLL_CGET( sndbuf_cget_loop )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_int = d->loop;
}

// PRC's sinc interpolation function.. as found 
// http://www.cs.princeton.edu/courses/archive/spring03/cs325/src/TimeStuf/srconvrt.c
//
// there's probably a lot in there that could be optimized, if we care to..

// #define PI 3.14159265358979323846
// wow... we are sensitive.. 

inline double sndbuf_linear_interp( double * first, double * second, double * frac );
bool sinc_table_built = false;

void sndbuf_sinc_interpolate( sndbuf_data *d, SAMPLE * out )
{
    signed long j;
    double factor = d->rate;
    double time_now = d->curf;
    double one_over_factor;
    // UNUSED: double int_time = 0;
    // UNUSED: double last_time = 0;
    double temp1 = 0.0;
    
    long time_i = (long)time_now;
    
    // bounds checking now in sampleAt function...
    if( factor < 1.0 ) {
        for( j = -d->sinc_width + 1; j < d->sinc_width; j++ )
        {
            temp1 += sndbuf_sampleAt(d,time_i+j) * sndbuf_sinc( d, (double)j );
        }
        *out = (SAMPLE)temp1;
    }
    else {
        one_over_factor = 1.0 / factor;
        for( j = -d->sinc_width + 1; j < d->sinc_width; j++ ) {
            temp1 += sndbuf_sampleAt(d,time_i+j) * one_over_factor * sndbuf_sinc( d, one_over_factor * (double)j );
        } 
        *out = (SAMPLE)temp1;
    }
}

void sndbuf_make_sinc( sndbuf_data * d )
{
    t_CKINT i;
    // CK_FPRINTF_STDERR( "building sinc table\n" );
    double temp, win_freq, win;
    win_freq = ONE_PI / d->sinc_width / d->sinc_samples_per_zero_crossing;
    t_CKINT tabsize = d->sinc_width * d->sinc_samples_per_zero_crossing;
    
    d->sinc_table = (double *) realloc( d->sinc_table, tabsize * sizeof(double) );
    d->sinc_table[0] = 1.0;
    for( i = 1; i < tabsize; i++ ) {
        temp = (double) i * ONE_PI / d->sinc_samples_per_zero_crossing;
        d->sinc_table[i] = (float)(sin(temp) / temp);
        win = 0.5 + 0.5 * cos(win_freq * i);
        d->sinc_table[i] *= (float)win;
    }
    d->sinc_table_built = true;
}

inline double sndbuf_linear_interp( double first_number, double second_number, double fraction )
{
    return (first_number + ((second_number - first_number) * fraction));
}

double sndbuf_t_sinc( sndbuf_data * d, double x )
{
    t_CKINT low;
    double temp,delta;
    if( !d->sinc_table_built ) sndbuf_make_sinc(d);
    if( fabs(x) >= d->sinc_width-1 )
        return 0.0;
    else {
        temp = fabs(x) * (double) d->sinc_samples_per_zero_crossing;
        low = (t_CKINT)temp; /* these are interpolation steps */
        if( d->sinc_use_interp ) {
            delta = temp - low;  /* and can be ommited if desired */
            return sndbuf_linear_interp( d->sinc_table[low], d->sinc_table[low + 1], delta );
        }
        else return d->sinc_table[low];
    }
}

double sndbuf_sinc( sndbuf_data * d, double x )
{
    double temp;
    
    if( d->sinc_use_table ) return sndbuf_t_sinc(d,x);
    else {
        if( x == 0.0 ) return 1.0;
        else {
            temp = ONE_PI * x;
            return sin(temp) / (temp);
        }
    }
}

CK_DLL_TICK( sndbuf_tick )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    
#ifdef CK_SNDBUF_MEMORY_BUFFER
    // spencer's memory buffer stuff
    if( d->fd == 0 )
        // no file open - memory buffer mode
    {
        Chuck_UGen * ugen = (Chuck_UGen *)SELF;
        if( ugen->m_num_src ) 
            // recording mode
        {
            if( d->mb_buffer.size() == 0 )
            {
                d->mb_buffer.resize( 44100 * 4 );   // default size: 4 seconds
                d->mb_record_position = d->mb_buffer;
                d->mb_playback_position = d->mb_buffer;
            }
            
            if( d->mb_record_position >= d->mb_buffer.size() )
                d->mb_buffer.resize( d->mb_buffer.size() * 2 );
            
            *(d->mb_record_position++) = in;
        }
        
        if( d->mb_buffer.size() )
        {
            if( d->mb_playback_position >= d->mb_record_position )
            {
                if( d->loop )
                    d->mb_playback_position = 0;
                
                else if( ugen->m_num_src )
                {
                    *out = 0;
                    return TRUE;
                }
                
                else
                    return FALSE;
            }
            
            *out = *(d->mb_playback_position++);
        }
        
        return TRUE;
    }
#endif /* CK_SNDBUF_MEMORY_BUFFER */
    
    if( !( d->buffer || d->chunk_map ) )
    {
        *out = 0;
        return TRUE;
    }
    
    // we're ticking once per sample ( system )
    // curf in samples;
    
    if( !d->loop && d->curf >= d->num_frames )
    {
        *out = 0;
        return TRUE;
    }
    
    // calculate frame
    if( d->interp == SNDBUF_DROP )
    { 
        *out = d->current_val;
    }
    else if( d->interp == SNDBUF_INTERP )
    {
        // samplewise linear interp
        double alpha = d->curf - floor(d->curf);
        *out = d->current_val;
        *out += (float)alpha * ( sndbuf_sampleAt(d, (long)d->curf+1 ) - *out );
    }
    else if( d->interp == SNDBUF_SINC ) {
        // do that fancy sinc function!
        sndbuf_sinc_interpolate(d, out);
    }
    
    // advance
    d->curf += d->rate;
    sndbuf_setpos(d, d->curf);
    
    return TRUE;    
}

/* multi-chan tick */
CK_DLL_TICKF( sndbuf_tickf )
{
    Chuck_UGen * ugen = (Chuck_UGen *) SELF;
    
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    
    if( !( d->buffer || d->chunk_map ) ) return FALSE;
    
    // we're ticking once per sample ( system )
    // curf in samples;
    
    if( !d->loop && d->curf >= d->num_frames ) return FALSE;
    
    // normalize amplitude across channels
    // TODO: normalize power?
    SAMPLE amp_factor = 1;
    if(d->num_channels == 1)
    {
        amp_factor = 0.5;
    }
    else if(d->num_channels == 2)
    {
        amp_factor = 1;
    }
    else
    {
        // ruh roh
    }
    
    unsigned int frame_idx;
    unsigned int nchans = ugen->m_num_outs;
    for(frame_idx = 0; frame_idx < nframes && (d->loop || d->curf < d->num_frames); frame_idx++)
    {
        for(unsigned int chan_idx = 0; chan_idx < nchans; chan_idx++)
        {
            // calculate frame
            if( d->interp == SNDBUF_DROP )
            {
                out[frame_idx*nchans+chan_idx] = sndbuf_sampleAt(d, d->curf, chan_idx % d->num_channels) * amp_factor;
            }
            else if( d->interp == SNDBUF_INTERP )
            {
                // SPENCERTODO
                // samplewise linear interp
                double alpha = d->curf - floor(d->curf);
                SAMPLE current_sample = sndbuf_sampleAt(d, d->curf, chan_idx % d->num_channels) * amp_factor;
                out[frame_idx*nchans+chan_idx] = current_sample;
                out[frame_idx*nchans+chan_idx] += (float)alpha * ( sndbuf_sampleAt(d, (long)d->curf+1, chan_idx % d->num_channels ) - current_sample );
                out[frame_idx*nchans+chan_idx] *= amp_factor;
            }
            else if( d->interp == SNDBUF_SINC )
            {
                // SPENCERTODO
                // do that fancy sinc function!
                //sndbuf_sinc_interpolate(d, out);
            }
        }
        
        // advance
        sndbuf_setpos(d, d->curf + d->rate);
    }
    
    for(; frame_idx < nframes; frame_idx++)
    {
        for(unsigned int chan_idx = 0; chan_idx < nchans; chan_idx++)
        {
            out[frame_idx*nchans+chan_idx] = 0;
        }
    }
                
    
    return TRUE;    
}


#include "util_raw.h"


CK_DLL_CTRL( sndbuf_ctrl_read )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    Chuck_String * ckfilename = GET_CK_STRING(ARGS);
    const char * filename = ckfilename->str().c_str();
    
    // return filename
    RETURN->v_string = ckfilename;
    
    SAFE_DELETE_ARRAY(d->buffer);
    
    if( d->chunk_map )
    {
        for(int i = 0; i < d->chunk_num; i++)
            SAFE_DELETE_ARRAY(d->chunk_map[i]);
        SAFE_DELETE_ARRAY(d->chunk_map);
        d->chunk_num = 0;
    }
    
    if( d->fd )
    {
        sf_close( d->fd );
        d->fd = NULL;
    }

    // log
    EM_log( CK_LOG_INFO, "(sndbuf): reading '%s'...", filename );
    
    // built in
    if( strstr(filename, "special:") )
    {
        SAMPLE * rawdata = NULL;
        t_CKUINT rawsize = 0;
        t_CKUINT srate = 22050;

        // which
        if( strstr(filename, "special:sinewave") ) {
            rawsize = 1024; rawdata = NULL;
        }
        else if( strstr(filename, "special:ahh") ) {
            rawsize = ahh_size; rawdata = ahh_data;
        }
        else if( strstr(filename, "special:britestk") ) {
            rawsize = britestk_size; rawdata = britestk_data;
        }
        else if( strstr(filename, "special:dope") ) {
            rawsize = dope_size; rawdata = dope_data;
        }
        else if( strstr(filename, "special:eee") ) {
            rawsize = eee_size; rawdata = eee_data;
        }
        else if( strstr(filename, "special:fwavblnk") ) {
            rawsize = fwavblnk_size; rawdata = fwavblnk_data;
        }
        else if( strstr(filename, "special:halfwave") ) {
            rawsize = halfwave_size; rawdata = halfwave_data;
        }
        else if( strstr(filename, "special:impuls10") ) {
            rawsize = impuls10_size; rawdata = impuls10_data;
        }
        else if( strstr(filename, "special:impuls20") ) {
            rawsize = impuls20_size; rawdata = impuls20_data;
        }
        else if( strstr(filename, "special:impuls40") ) {
            rawsize = impuls40_size; rawdata = impuls40_data;
        }
        else if( strstr(filename, "special:mand1") ) {
            rawsize = mand1_size; rawdata = mand1_data;
        }
        else if( strstr(filename, "special:mandpluk") ) {
            rawsize = mandpluk_size; rawdata = mandpluk_data;
        }
        else if( strstr(filename, "special:marmstk1") ) {
            rawsize = marmstk1_size; rawdata = marmstk1_data;
        }
        else if( strstr(filename, "special:ooo") ) {
            rawsize = ooo_size; rawdata = ooo_data;
        }
        else if( strstr(filename, "special:peksblnk") ) {
            rawsize = peksblnk_size; rawdata = peksblnk_data;
        }
        else if( strstr(filename, "special:ppksblnk") ) {
            rawsize = ppksblnk_size; rawdata = ppksblnk_data;
        }
        else if( strstr(filename, "special:silence") ) {
            rawsize = silence_size; rawdata = silence_data;
        }
        else if( strstr(filename, "special:sineblnk") ) {
            rawsize = sineblnk_size; rawdata = sineblnk_data;
        }
        else if( strstr(filename, "special:sinewave") ) {
            rawsize = sinewave_size; rawdata = sinewave_data;
        }
        else if( strstr(filename, "special:snglpeak") ) {
            rawsize = snglpeak_size; rawdata = snglpeak_data;
        }
        else if( strstr(filename, "special:twopeaks") ) {
            rawsize = twopeaks_size; rawdata = twopeaks_data;
        }
        else if( strstr(filename, "special:glot_ahh") ) {
            rawsize = glot_ahh_size; rawdata = glot_ahh_data; srate = 44100;
        }
        else if( strstr(filename, "special:glot_eee") ) {
            rawsize = glot_eee_size; rawdata = glot_eee_data; srate = 44100;
        }
        else if( strstr(filename, "special:glot_ooo") ) {
            rawsize = glot_ooo_size; rawdata = glot_ooo_data; srate = 44100;
        }
        else if( strstr(filename, "special:glot_pop") ) {
            rawsize = glot_pop_size; rawdata = glot_pop_data; srate = 44100;
        }

        d->num_frames = rawsize;
        d->num_channels = 1;
        d->chan = 0;
        d->samplerate = srate;
        d->num_samples = rawsize;

        // no chunking
        d->chunks_read = d->num_frames;

        if( rawdata ) {
            d->buffer = new SAMPLE[rawsize+1];
            for( t_CKUINT j = 0; j < rawsize; j++ ) {
                d->buffer[j] = (SAMPLE)rawdata[j]/(SAMPLE)SHRT_MAX;
            }
        }
        else if( strstr(filename, "special:sinewave") ) {
            d->buffer = new SAMPLE[rawsize+1];
            for( t_CKUINT j = 0; j < rawsize; j++ )
                d->buffer[j] = sin(2*ONE_PI*j/rawsize);
        }
        else {
            CK_FPRINTF_STDERR( "[chuck](via SndBuf): cannot load '%s'\n", filename );
            return;
        }

        d->buffer[rawsize] = d->buffer[0];
    }
    else // read file
    {
        // stat the file first
        struct stat s;
        if( stat( filename, &s ) )
        {
            CK_FPRINTF_STDERR( "[chuck](via SndBuf): cannot stat file '%s'...\n", filename );
            return;
        }

        // open it
        SF_INFO info;
        info.format = 0;
        const char * format = (const char *)strrchr( filename, '.');
        if( format && strcmp( format, ".raw" ) == 0 )
        { 
            CK_FPRINTF_STDERR( "[chuck](via SndBuf) %s :: type is '.raw'...\n    assuming 16 bit signed mono (PCM)\n", filename );
            info.format = SF_FORMAT_RAW | SF_FORMAT_PCM_16 | SF_ENDIAN_CPU ;
            info.channels = 1;
            info.samplerate = 44100;
        }

        // open the handle
        d->fd = sf_open( filename, SFM_READ, &info );
        t_CKINT er = sf_error( d->fd );
        if( er )
        {
            CK_FPRINTF_STDERR( "[chuck](via SndBuf): sndfile error '%li' opening '%s'...\n", er, filename );
            CK_FPRINTF_STDERR( "[chuck](via SndBuf): (reason: %s)\n", sf_strerror( d->fd ) );
            if( d->fd ) sf_close( d->fd );
            // escape
            return;
        }

        // allocate
        t_CKINT size = info.channels * info.frames;
        if(d->chunks)
        {
            // split into small allocations
            d->chunk_num = ceilf(((t_CKFLOAT) size) / ((t_CKFLOAT) d->chunks));
            d->buffer = NULL;
            d->chunk_map = new SAMPLE*[d->chunk_num];
            memset(d->chunk_map, 0, d->chunk_num * sizeof(SAMPLE *));
            d->chunks_read = 0;
        }
        else
        {
            // all in one go
            d->buffer = new SAMPLE[size+info.channels];
            memset( d->buffer, 0, (size+info.channels)*sizeof(SAMPLE) );
            d->chunk_map = NULL;
        }
        
        d->chan = 0;
        d->num_frames = info.frames;
        d->num_channels = info.channels;
        d->samplerate = info.samplerate;
        d->num_samples = size;

        // log
        EM_pushlog();
        EM_log( CK_LOG_INFO, "channels: %d", d->num_channels );
        EM_log( CK_LOG_INFO, "frames: %d", d->num_frames );
        EM_log( CK_LOG_INFO, "srate: %d", d->samplerate );
        EM_log( CK_LOG_INFO, "chunks: %d", d->chunks );
        EM_poplog();

        // read
        sf_seek( d->fd, 0, SEEK_SET );

        // no chunk
        if( !d->chunks )
        {
            // read all
            t_CKUINT f = sndbuf_read( d, 0, d->num_frames );
            // check
            if( f != (t_CKUINT)d->num_frames )
            {
                CK_FPRINTF_STDERR( "[chuck](via SndBuf): read %lu rather than %ld frames from %s\n",
                         f, size, filename );
                sf_close( d->fd ); d->fd = NULL;
                return;
            }

            assert( d->fd == NULL );
        }
    }

    // d->interp = SNDBUF_INTERP;
    d->sampleratio = (double)d->samplerate / (double)g_srate;
    // set the rate
    d->rate = d->sampleratio * d->rate_factor;
    d->current_val = 0;
    d->curf = 0;
    d->eob = d->buffer + d->num_samples;
}

CK_DLL_CTRL( sndbuf_ctrl_write )
{
#ifdef SPENCER_SNDBUF_WRITE
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    const char * filename = GET_CK_STRING(ARGS)->str.c_str();
    
    if( d->buffer )
    {
        delete [] d->buffer;
        d->buffer = NULL;
    }
    
    struct stat s;
    if( stat( filename, &s ) )
    {
        CK_FPRINTF_STDERR( "[chuck](via SndBuf): cannot stat file '%s'...\n", filename );
        return;
    }
    
    d->curr = d->buffer;
    d->eob = d->buffer + d->num_samples;
#endif
}


CK_DLL_CTRL( sndbuf_ctrl_rate )
{
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKFLOAT rate = GET_CK_FLOAT(ARGS); // rate
    d->rate = rate * d->sampleratio;
    d->rate_factor = rate;
    RETURN->v_float = d->rate_factor; // TODO: (or not TODO:)
}

CK_DLL_CGET( sndbuf_cget_rate )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_float = d->rate_factor;
}


CK_DLL_CTRL( sndbuf_ctrl_freq )
{ 
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKFLOAT freq = GET_CK_FLOAT(ARGS);  //hz
    
    d->rate = ( freq * (double) d->num_frames / (double) g_srate );
    d->rate_factor = d->rate / d->sampleratio;
    RETURN->v_float = d->rate * (t_CKFLOAT) g_srate / ( (t_CKFLOAT) d->num_frames ); // TODO: really?
}

CK_DLL_CGET( sndbuf_cget_freq )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_float = d->rate * (t_CKFLOAT) g_srate / ( (t_CKFLOAT) d->num_frames ); 
}

CK_DLL_CTRL( sndbuf_ctrl_phase )
{ 
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKFLOAT phase = GET_CK_FLOAT(ARGS);
    sndbuf_setpos(d, phase * (double)d->num_frames);
    RETURN->v_float = (t_CKFLOAT) d->curf / (t_CKFLOAT)d->num_frames; // TODO: 
}

CK_DLL_CGET( sndbuf_cget_phase )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    //SET_NEXT_FLOAT( out, (t_CKFLOAT) d->curf / (t_CKFLOAT)d->num_frames );
    RETURN->v_float = (t_CKFLOAT) d->curf / (t_CKFLOAT)d->num_frames;
}

CK_DLL_CTRL( sndbuf_ctrl_channel )
{ 
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKINT chan = GET_CK_INT(ARGS);
    if ( chan >= 0 && chan < d->num_channels ) { 
        d->chan = chan;
    }
    RETURN->v_int = (t_CKINT)d->chan;
}

CK_DLL_CGET( sndbuf_cget_channel )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    //SET_NEXT_INT( out, d->chan );
    RETURN->v_int = (t_CKINT)d->chan;
}

CK_DLL_CTRL( sndbuf_ctrl_pos )
{ 
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKINT pos = GET_CK_INT(ARGS);
#ifdef CK_SNDBUF_MEMORY_BUFFER
    if( pos >= 0 && pos < d->mb_max_samples )
        d->mb_playback_position = pos;
#endif /* CK_SNDBUF_MEMORY_BUFFER */
    sndbuf_setpos(d, pos);
    RETURN->v_int = (t_CKINT)sndbuf_getpos(d); // TODO TODO TODOOO
}

CK_DLL_CGET( sndbuf_cget_pos )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_int = (t_CKINT)sndbuf_getpos(d); 
}

CK_DLL_CTRL( sndbuf_ctrl_interp )
{ 
    sndbuf_data * d = ( sndbuf_data * ) OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKINT interp = GET_CK_INT(ARGS);
    d->interp = interp;
    RETURN->v_int = d->interp;
}

CK_DLL_CGET( sndbuf_cget_interp )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_int = d->interp;
}

CK_DLL_CTRL( sndbuf_ctrl_chunks )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKINT frames = GET_NEXT_INT(ARGS);
    d->chunks = frames >= 0 ? frames : 0;
    RETURN->v_int = d->chunks;
}

CK_DLL_CGET( sndbuf_cget_chunks )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    RETURN->v_int = d->chunks;
}

CK_DLL_CTRL( sndbuf_ctrl_phase_offset )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKFLOAT phase_offset = GET_CK_FLOAT(ARGS);
    sndbuf_setpos(d, d->curf + phase_offset * (t_CKFLOAT)d->num_frames );
}

CK_DLL_CGET( sndbuf_cget_samples )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    //SET_NEXT_INT( out, d->num_frames );
    RETURN->v_int = d->num_frames;
}

CK_DLL_CGET( sndbuf_cget_length )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    //SET_NEXT_DUR( out, (t_CKDUR)d->num_frames );
    RETURN->v_dur = (t_CKDUR)d->num_frames / d->sampleratio;
}

CK_DLL_CGET( sndbuf_cget_channels )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    //SET_NEXT_INT( out, d->num_channels );
    RETURN->v_int = d->num_channels;
}

CK_DLL_CGET( sndbuf_cget_valueAt )
{
    sndbuf_data * d = (sndbuf_data *)OBJ_MEMBER_UINT(SELF, sndbuf_offset_data);
    t_CKINT sample = GET_CK_INT(ARGS);
    t_CKINT frame, channel;
    d->sampleIndex2FrameIndexAndChannel(sample, &frame, &channel);
    if( d->fd ) sndbuf_load( d, sample );
    RETURN->v_float = ( frame > d->num_frames || frame < 0 ) ? 0 : sndbuf_sampleAt(d, frame, channel);
}

#endif // __DISABLE_SNDBUF__


class Dyno_Data
{
private:
  t_CKDUR ms; // changed 1.3.5.3 ge from const static

public:
  t_CKFLOAT slopeAbove;
  t_CKFLOAT slopeBelow;
  t_CKFLOAT thresh;
  t_CKFLOAT rt;
  t_CKFLOAT at;
  t_CKFLOAT xd; //sidechain
  int externalSideInput; // use input signal or a ctrl signal for env
  t_CKFLOAT sideInput;   // the ctrl signal for the envelope
  
  int count; //diagnostic

  Dyno_Data( Chuck_VM * vm ) {
    ms = vm->srate() / 1000.0;
    xd = 0.0;
    count = 0;
    sideInput = 0;
    limit();
  }
  ~Dyno_Data() {}

  void limit();
  void compress();
  void gate();
  void expand();
  void duck();

  //set the time constants for rt, at, and tav
  static t_CKFLOAT computeTimeConst(t_CKDUR t) {
    //AT = 1 - e ^ (-2.2T/t<AT)
    //as per chuck_type.cpp, T(sampling period) = 1.0
    return 1.0 - exp( -2.2 / t );
  }

  static t_CKDUR timeConstToDur(t_CKFLOAT x) {
    return -2.2 / log(1.0 - x);
  }

  //setters for timing constants
  void setAttackTime(t_CKDUR t);
  void setReleaseTime(t_CKDUR t);

  //other setters
  void setRatio(t_CKFLOAT newRatio);
  t_CKFLOAT getRatio();
};

//setters for the timing constants
void Dyno_Data::setAttackTime(t_CKDUR t) {
  at = computeTimeConst(t);
}

void Dyno_Data::setReleaseTime(t_CKDUR t) {
  rt = computeTimeConst(t);
}

void Dyno_Data::setRatio(t_CKFLOAT newRatio) {
  this->slopeAbove = 1.0 / newRatio;
  this->slopeBelow = 1.0;
}

t_CKFLOAT Dyno_Data::getRatio()
{
  return this->slopeBelow / this->slopeAbove;
}

//TODO: come up with better/good presets?

//presets for the dynomics processor
void Dyno_Data::limit() {
  slopeAbove = 0.1;   // 10:1 compression above thresh
  slopeBelow = 1.0;    // no compression below
  thresh = 0.5;
  at = computeTimeConst( 5.0 * ms );
  rt = computeTimeConst( 300.0 * ms );
  externalSideInput = 0;
}

void Dyno_Data::compress() {
  slopeAbove = 0.5;   // 2:1 compression
  slopeBelow = 1.0;
  thresh = 0.5;
  at = computeTimeConst( 5.0 * ms );
  rt = computeTimeConst( 500.0 * ms );
  externalSideInput = 0;
}

void Dyno_Data::gate() {
  slopeAbove = 1.0;
  slopeBelow = 100000000; // infinity (more or less)
  thresh = 0.1;
  at = computeTimeConst( 11.0 * ms );
  rt = computeTimeConst( 100.0 * ms );
  externalSideInput = 0;
}

void Dyno_Data::expand() {
  slopeAbove = 2.0;    // 1:2 expansion
  slopeBelow = 1.0;
  thresh = 0.5;
  at = computeTimeConst( 20.0 * ms );
  rt = computeTimeConst( 400.0 * ms );
  externalSideInput = 0;
}

void Dyno_Data::duck() {
  slopeAbove = 0.5;    // when sideInput rises above thresh, gain starts going
  slopeBelow = 1.0;    // down. it'll drop more as sideInput gets louder.
  thresh = 0.1;
  at = computeTimeConst( 10.0 * ms );
  rt = computeTimeConst( 1000.0 * ms );
  externalSideInput = 1;
}


// controls for the preset modes
CK_DLL_CTRL( dyno_ctrl_limit ) {
     Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
     d->limit();
}

CK_DLL_CTRL( dyno_ctrl_compress ) {
     Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
     d->compress();
}

CK_DLL_CTRL( dyno_ctrl_gate ) {
     Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
     d->gate();
}

CK_DLL_CTRL( dyno_ctrl_expand ) {
     Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
     d->expand();
}

CK_DLL_CTRL( dyno_ctrl_duck ) {
     Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
     d->duck();
}

//additional controls: thresh
CK_DLL_CTRL( dyno_ctrl_thresh ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->thresh = GET_CK_FLOAT(ARGS);
    RETURN->v_float = (t_CKFLOAT)d->thresh;
}

CK_DLL_CGET( dyno_cget_thresh ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_float = (t_CKFLOAT)d->thresh;
}

//additional controls: attackTime
CK_DLL_CTRL( dyno_ctrl_attackTime ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->setAttackTime( GET_CK_FLOAT(ARGS) );
    RETURN->v_dur = d->timeConstToDur(d->at);
}

CK_DLL_CGET( dyno_cget_attackTime ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_dur = d->timeConstToDur(d->at);
}

//additional controls: releaseTime
CK_DLL_CTRL( dyno_ctrl_releaseTime ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->setReleaseTime( GET_CK_FLOAT(ARGS) );
    RETURN->v_dur = d->timeConstToDur(d->rt);
}

CK_DLL_CGET( dyno_cget_releaseTime ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_dur = d->timeConstToDur(d->rt);
}

//additional controls: ratio
CK_DLL_CTRL( dyno_ctrl_ratio ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->setRatio( GET_CK_FLOAT(ARGS) );
    RETURN->v_float = d->getRatio();
}

CK_DLL_CGET( dyno_cget_ratio ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_float = d->getRatio();
}

//additional controls: slopeBelow
CK_DLL_CTRL( dyno_ctrl_slopeBelow ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->slopeBelow = GET_CK_FLOAT(ARGS);
    
    RETURN->v_float = d->slopeBelow;
}

CK_DLL_CGET( dyno_cget_slopeBelow ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_float = d->slopeBelow;
}

//additional controls: slopeAbove
CK_DLL_CTRL( dyno_ctrl_slopeAbove ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->slopeAbove = GET_CK_FLOAT(ARGS);
    
    RETURN->v_float = d->slopeAbove;
}

CK_DLL_CGET( dyno_cget_slopeAbove ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_float = d->slopeAbove;
}

//additional controls: sideInput
CK_DLL_CTRL( dyno_ctrl_sideInput ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->sideInput = GET_CK_FLOAT(ARGS);
    
    RETURN->v_float = d->sideInput;
}

CK_DLL_CGET( dyno_cget_sideInput ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_float = d->sideInput;
}

//additional controls: externalSideInput
CK_DLL_CTRL( dyno_ctrl_externalSideInput ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    d->externalSideInput = GET_CK_INT(ARGS);
    
    RETURN->v_int = d->externalSideInput;
}

CK_DLL_CGET( dyno_cget_externalSideInput ) {
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    RETURN->v_int = d->externalSideInput;
}

//constructor
CK_DLL_CTOR( dyno_ctor )
{
    OBJ_MEMBER_UINT(SELF, dyno_offset_data) = (t_CKUINT)new Dyno_Data( SHRED->vm_ref );
}

CK_DLL_DTOR( dyno_dtor )
{
    delete (Dyno_Data *)OBJ_MEMBER_UINT(SELF, dyno_offset_data);
    OBJ_MEMBER_UINT(SELF, dyno_offset_data) = 0;
}

// recomputes envelope, determines how the current amp envelope compares with
// thresh, applies the appropriate new slope depending on how far above/below
// the threshold the current envelope is.
CK_DLL_TICK( dyno_tick )
{
    Dyno_Data * d = ( Dyno_Data * )OBJ_MEMBER_UINT(SELF, dyno_offset_data);

    // only change sideInput if we're not using an external ctrl signal.
    // otherwise we'll just use whatever the user sent us last as the ctrl signal
    if(!d->externalSideInput)
        d->sideInput = in >= 0 ? in : -in;

    // 'a' is signal left after subtracting xd (to recompute sideChain envelope)
    double a = d->sideInput - d->xd;
    // a is only needed if positive to pull the envelope up, not to bring it down
    if ( a < 0 ) a=0;
    // the attack/release (peak) exponential filter to guess envelope
    d->xd = d->xd * (1 - d->rt) + d->at * a;

    // if you were to use the rms filter,
    // it would probably look, a sumpthin' like this
    // d->xd = TAV * in*in + (1+TAV) * d->xd

    // decide which slope to use, depending on whether we're below/above thresh
    double slope = d->xd > d->thresh ? d->slopeAbove : d->slopeBelow;
    // the gain function - apply the slope chosen above
    double f = slope == 1.0 ? 1.0 : pow( d->xd / d->thresh, slope - 1.0 );

    // apply the gain found above to input sample
    *out = f * in;

    return TRUE;
}



/*
LiSa updates/fixes:
1. added playing(int voice) method. returns whether a voice is playing
2. added getbi(int voice) method. returns whether a voice is bi
3. added multichan; pan(int voice, float where (between 0. and C-1) ) method, to equal-power place across the channels
4. corrected loopEnd so it is the same as duration, not 1 less than duration, when set...
*/

#define LiSa_MAXVOICES 200
#define LiSa_MAXBUFSIZE 44100000
//-----------------------------------------------------------------------------
// name: LiSaMulti_data
// desc: ...
//-----------------------------------------------------------------------------
struct LiSaMulti_data
{
    SAMPLE * mdata;
	SAMPLE * outsamples; //samples by channel to send out
    t_CKINT mdata_len;
    t_CKINT maxvoices;
    t_CKINT loop_start[LiSa_MAXVOICES], loop_end[LiSa_MAXVOICES], loop_end_rec;
    t_CKINT rindex; // record and play indices
    t_CKBOOL record, looprec, loopplay[LiSa_MAXVOICES], reset, append, play[LiSa_MAXVOICES], bi[LiSa_MAXVOICES];
    t_CKFLOAT coeff; // feedback coeff
	t_CKFLOAT voiceGain[LiSa_MAXVOICES]; //gain control for each voice
	t_CKFLOAT voicePan[LiSa_MAXVOICES];  //pan control for each voice; places voice between any pair of channels.
	t_CKFLOAT channelGain[LiSa_MAXVOICES][LiSa_channels]; 
    t_CKDOUBLE p_inc[LiSa_MAXVOICES], pindex[LiSa_MAXVOICES]; // playback increment
    
    // ramp stuff
    t_CKDOUBLE rampup_len[LiSa_MAXVOICES], rampdown_len[LiSa_MAXVOICES], rec_ramplen, rec_ramplen_inv;
    t_CKDOUBLE rampup_len_inv[LiSa_MAXVOICES], rampdown_len_inv[LiSa_MAXVOICES];
    t_CKDOUBLE rampctr[LiSa_MAXVOICES];
    t_CKBOOL rampup[LiSa_MAXVOICES], rampdown[LiSa_MAXVOICES];
    
    t_CKINT track;
	
	t_CKINT num_chans;

    // allocate memory, length in samples
    inline int buffer_alloc(t_CKINT length)
    {
        mdata = (SAMPLE *)malloc((length + 1) * sizeof(SAMPLE)); //extra sample for safety....
        if(!mdata)  {
            CK_FPRINTF_STDERR( "LiSaBasic: unable to allocate memory!\n" );
            return false;
        }
        
        memset(mdata, 0, (length + 1) * sizeof(SAMPLE));
            
        mdata_len = length;
        maxvoices = 10; // default; user can set
        rec_ramplen = 0.;
        rec_ramplen_inv = 1.;
        
        track = 0;
        
        for (t_CKINT i=0; i < LiSa_MAXVOICES; i++) {
            loop_start[i] = 0;
            //loop_end[i] = length - 1; //no idea why i had this
			loop_end[i] = length;
			loop_end_rec = length;
            
            pindex[i] = rindex = 0;
            play[i] = record = bi[i] = false;
            looprec = loopplay[i] = true;
            coeff = 0.;
            p_inc[i] = 1.;
			voiceGain[i] = 1.;
			voicePan[i] = 0.5;
            
            // ramp stuff
            rampup[i] = rampdown[i] = false;
            rampup_len[i] = rampdown_len[i] = 0.;
            rampup_len_inv[i] = rampdown_len_inv[i] = 1.;
            rampctr[i] = 0.;
			
			for(t_CKINT j=2; j<num_chans; j++) {
				channelGain[i][j] = 1.;
			}
			channelGain[i][0] = 1.0;
//			channelGain[i][0] = 0.707;
//			channelGain[i][1] = 0.707;
        }
        
        return true;
    }
    
    // dump a sample into the buffer; retain existing sample, scaled by "coeff"
    inline void recordSamp(SAMPLE insample)
    {
        SAMPLE tempsample;
        
        if(record) {
            if(looprec) {
                if(rindex >= loop_end_rec)  rindex = 0;
                tempsample = coeff * mdata[rindex] + insample;
                //mdata[rindex] = coeff * mdata[rindex] + insample;
                //rindex++;
            } else {
                if (rindex < loop_end_rec) {
                    //mdata[rindex] = coeff * mdata[rindex] + insample;
                    tempsample = coeff * mdata[rindex] + insample;
                    //rindex++;
                } else {
                    record = 0;
                    return;
                }
            }
            
            // ramp stuff here
            if(rindex < rec_ramplen) {
                tempsample *= (rindex * rec_ramplen_inv);
				//CK_FPRINTF_STDERR( "ramping up %f\n", rindex * rec_ramplen_inv );
            } else if(rindex > (loop_end_rec - rec_ramplen)) {
                tempsample *= (loop_end_rec - rindex) * rec_ramplen_inv;
				//CK_FPRINTF_STDERR( "ramping down %f\n", (loop_end_rec - rindex) * rec_ramplen_inv );
            }
            mdata[rindex] = tempsample;
            rindex++;
        }
    }
    
    // grab a sample from the buffer, with linear interpolation (add prc's SINC interp later)
    // increment play index
	// which specifies voice number
    inline SAMPLE getNextSamp(t_CKINT which)
    {
        // constrain
        if(loopplay[which]) {
            if(bi[which]) { // change direction if bidirectional mode
                if(pindex[which] >= loop_end[which] || pindex[which] < loop_start[which]) { //should be >= ?
                    pindex[which]  -=  p_inc[which];
                    p_inc[which]    = -p_inc[which];
                } 
            }
			if( loop_start[which] == loop_end[which] ) pindex[which] = loop_start[which]; //catch this condition to avoid infinite while loops
			else {
				while(pindex[which] >= loop_end[which]) pindex[which] = loop_start[which] + (pindex[which] - loop_end[which]); //again, >=?
				while(pindex[which] < loop_start[which]) pindex[which] = loop_end[which] - (loop_start[which] - pindex[which]);
			}

        } else if(pindex[which] >= mdata_len || pindex[which] < 0) { //should be >=, no?
            play[which] = 0;
            //CK_FPRINTF_STDERR( "turning voice %d off!\n", which );
            return (SAMPLE) 0.;
        }
        
        // interp
        t_CKINT whereTrunc = (t_CKINT) pindex[which];
        t_CKDOUBLE whereFrac = pindex[which] - (t_CKDOUBLE)whereTrunc;
        t_CKINT whereNext = whereTrunc + 1;
        
        if (loopplay[which]) {
			if((whereNext) >= loop_end[which]) {
				whereNext = loop_start[which];
			}
			if((whereTrunc) >= loop_end[which]) {
				whereTrunc = loop_start[which];
			}
		} else {
			if((whereTrunc) >= mdata_len) {
				whereTrunc = mdata_len - 1; //should correct this, in case we've overshot by more than 1 sample
				whereNext = 0;
			}
			if((whereNext) >= mdata_len) {
				whereNext = 0;
			}
		}
        
        pindex[which] += p_inc[which];
        
        t_CKDOUBLE outsample;
        outsample = (t_CKDOUBLE)mdata[whereTrunc] + (t_CKDOUBLE)(mdata[whereNext] - mdata[whereTrunc]) * whereFrac;
        
        // ramp stuff
        if(rampup[which]) {
            outsample *= rampctr[which]++ * rampup_len_inv[which]; //remove divide
            if(rampctr[which] >= rampup_len[which]) rampup[which] = false;
        }
        else if(rampdown[which]) {
            outsample *= (rampdown_len[which] - rampctr[which]++) * rampdown_len_inv[which];
            if(rampctr[which] >= rampdown_len[which]) {
                rampdown[which] = false;
                play[which] = false; 
            }
        }
		
		outsample *= voiceGain[which];
        
        return (SAMPLE)outsample;        
    }
    
    // grab a sample from the buffer, with linear interpolation (add prc's SINC interp later)
    // given a position within the buffer
    inline SAMPLE getSamp(t_CKDOUBLE where, t_CKINT which)
    {
        // constrain
        if(where > loop_end[which])   where = loop_end[which];
        if(where < loop_start[which]) where = loop_start[which];
        
        // interp
        t_CKINT whereTrunc = (t_CKINT) where;
        t_CKDOUBLE whereFrac = where - (t_CKDOUBLE)whereTrunc;
        t_CKINT whereNext = whereTrunc + 1;
        
        if((whereNext) == loop_end[which]) whereNext = loop_start[which];

        t_CKDOUBLE outsample;
        outsample = (t_CKDOUBLE)mdata[whereTrunc] + (t_CKDOUBLE)(mdata[whereNext] - mdata[whereTrunc]) * whereFrac;
		outsample *= voiceGain[which];
        
		//add voiceGain ctl here; return (SAMPLE)vgain[which]*outsample;
        return (SAMPLE)outsample;        
    }

    // ramp stuff
    inline void ramp_up(t_CKINT voicenum, t_CKDUR uptime)
    {            
        // CK_FPRINTF_STDERR( "ramping up voice %d", voicenum );

        rampup[voicenum] = true;
        play[voicenum] = true;
        rampup_len[voicenum] = (t_CKDOUBLE)uptime;
        if(rampup_len[voicenum] > 0.) rampup_len_inv[voicenum] = 1./rampup_len[voicenum];
        else rampup_len[voicenum] = 1.;

        // check to make sure we are not mid ramping down
        if(rampdown[voicenum]) {
            rampctr[voicenum] = rampup_len[voicenum] * (1. - rampctr[voicenum]/rampdown_len[voicenum]);
            rampdown[voicenum] = false;
        } else rampctr[voicenum] = 0;
    }
    
    inline void ramp_down(t_CKINT voicenum, t_CKDUR downtime)
    {
        rampdown[voicenum] = true;
        rampdown_len[voicenum] = (t_CKDOUBLE)downtime;
        if(rampdown_len[voicenum] > 0.) rampdown_len_inv[voicenum] = 1./rampdown_len[voicenum];
        else rampdown_len[voicenum] = 1.;
        
        // check to make sure we are not mid ramping up
        if(rampup[voicenum]) {
            rampctr[voicenum] = rampdown_len[voicenum] * (1. - rampctr[voicenum]/rampup_len[voicenum]);
            rampup[voicenum] = false;
        } else rampctr[voicenum] = 0;
    }
    
    inline void set_rec_ramplen(t_CKDUR newlen)
    {
        rec_ramplen = (t_CKDOUBLE)newlen;
        if(rec_ramplen > 0.) rec_ramplen_inv = 1./rec_ramplen;
        else rec_ramplen_inv = 1.;
		//fprintf ( stderr, "rec_ramplen = %f, inv = %f \n", rec_ramplen, rec_ramplen_inv );
    }


	//for simple stereo panning of a particular voice, and...
	//	l.channelGain(voice, channel, gain)
	//to set the gain for a particular voice going to a particular channel; good for >2 voices (like 6 channels!)
    inline SAMPLE * tick_multi( SAMPLE in)
    {

        SAMPLE tempsample = 0.;

		for(t_CKINT i=0;i<num_chans;i++) outsamples[i] = 0.;
		
		if(!mdata) return outsamples;
        
        recordSamp(in);

        if(track==0) {
            for (t_CKINT i=0; i<maxvoices; i++) {
                if(play[i]) {
					tempsample = getNextSamp(i);
					for(t_CKINT j=0;j<num_chans;j++) {
						//outsamples[j] += tempsample; //mono for now, testing...
						outsamples[j] += tempsample * channelGain[i][j]; //channelGain should return gain for voice i in channel j
					}
				}
            }
        } else if(track==1) {
            if(in<0.) in = -in; 
			for (t_CKINT i=0; i<maxvoices; i++) {
				if(play[i]) {
                    t_CKDOUBLE location = loop_start[i] + (t_CKDOUBLE)in * (loop_end[i] - loop_start[i]);
                    tempsample = getSamp(location, i);
                    
                    // spencer 2013/5/13: fix LiSa track-mode multichannel
                    for(t_CKINT j=0;j<num_chans;j++) {
                        outsamples[j] += tempsample * channelGain[i][j]; //channelGain should return gain for voice i in channel j
                    }
                }
			}
        } else if(track==2 && play[0]) {
            if(in<0.) in = -in; //only use voice 0 when tracking with durs.
            tempsample = getSamp( (t_CKDOUBLE)in, 0 );
            
            // spencer 2013/5/13: fix LiSa track-mode multichannel
            for(t_CKINT j=0;j<num_chans;j++) {
                //outsamples[j] += tempsample; //mono for now, testing...
                outsamples[j] += tempsample * channelGain[0][j]; //channelGain should return gain for voice i in channel j
            }
        }

        return outsamples;
    }
    
    inline void clear_buf()
    {
        for (t_CKINT i = 0; i < mdata_len; i++)
            mdata[i] = 0.;
    }
    
    inline t_CKINT get_free_voice()
    {
        t_CKINT voicenumber = 0;
        while(play[voicenumber] && voicenumber < maxvoices) {
            voicenumber++;
        }
        if(voicenumber == maxvoices) voicenumber = -1;
        return voicenumber;
    }
	
	//stick sample in record buffer
	inline void pokeSample( SAMPLE insample, t_CKINT index ) {
	
		if ( index >= mdata_len || index < 0 ) {
			index = 0;
			CK_FPRINTF_STDERR( "LiSa: trying to put sample out of buffer range; ignoring" );
		} else mdata[index] = insample;
	
	}
	
	//grab sample directly from record buffer, with linear interpolation
	inline SAMPLE grabSample ( t_CKDOUBLE where ) {
	
		if ( where > mdata_len || where < 0 ) {
			where = 0;
			CK_FPRINTF_STDERR( "LiSa: trying to grab sample out of buffer range; ignoring" );
			return 0.;
		} else {
		        
			// interp
			t_CKINT whereTrunc = (t_CKINT) where;
			t_CKDOUBLE whereFrac = where - (t_CKDOUBLE)whereTrunc;
			t_CKINT whereNext = whereTrunc + 1;
			
			if((whereNext) == mdata_len) whereNext = 0;

			t_CKDOUBLE outsample;
			outsample = (t_CKDOUBLE)mdata[whereTrunc] + (t_CKDOUBLE)(mdata[whereNext] - mdata[whereTrunc]) * whereFrac;
			
			//add voiceGain ctl here; return (SAMPLE)vgain[which]*outsample;
			return (SAMPLE)outsample;   
		
		  }  
	}
	
};




//++++++++++++++++++++++++++++++++++++++++
//
//
//++++++++++++++++++++++++++++++++++++++++
//
//
//++++++++++++++++++++++++++++++++++++++++




//-----------------------------------------------------------------------------
// name: LiSaMulti_ctor()
// desc: CTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( LiSaMulti_ctor )
{

    LiSaMulti_data * f =  new LiSaMulti_data;
    memset( f, 0, sizeof(LiSaMulti_data) );
			
	Chuck_UGen * ugen = (Chuck_UGen *)SELF;
	f->num_chans = ugen->m_multi_chan_size > 0 ? ugen->m_multi_chan_size : 1;
    //CK_FPRINTF_STDERR( "LiSa: number of channels = %d\n", f->num_chans );
	f->outsamples = new SAMPLE[f->num_chans];
	memset( f->outsamples, 0, (f->num_chans)*sizeof(SAMPLE) );
	
    OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data) = (t_CKUINT)f;
		

	
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_dtor()
// desc: DTOR function ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( LiSaMulti_dtor )
{
    // get data
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    // delete
    SAFE_DELETE(d);
    // set
    OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: ()
// desc: TICK function ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( LiSaMulti_tick )
{
//	Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	SAMPLE * temp_out_samples = d->tick_multi( in );
	
    //	for( t_CKUINT i = 0; i < ugen->m_multi_chan_size; i++ )
    //		ugen->m_multi_chan[i]->m_sum = ugen->m_multi_chan[i]->m_current = temp_out_samples[i]; //yay this works!
    *out = temp_out_samples[0];
	
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: ()
// desc: TICKF function ...
//-----------------------------------------------------------------------------
CK_DLL_TICKF( LiSaMulti_tickf )
{
	Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	
    unsigned int nchans = ugen->m_num_outs;
    for(unsigned int frame_idx = 0; frame_idx < nframes; frame_idx++)
    {
        SAMPLE * temp_out_samples = d->tick_multi( in[frame_idx*nchans+1] );
//        CK_FPRINTF_STDERR( "%0.2f ", in[frame_idx*nchans+0] );
        
        for(unsigned int chan_idx = 0; chan_idx < nchans; chan_idx++)
        {
            out[frame_idx*nchans+chan_idx] = temp_out_samples[chan_idx];
        }
    }
	
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_size()
// desc: set size of buffer allocation
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_size )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKDUR buflen = GET_NEXT_DUR(ARGS);
    if (buflen > LiSa_MAXBUFSIZE) {
        CK_FPRINTF_STDERR( "LiSa: buffer size request too large, resizing\n" );
        buflen = LiSa_MAXBUFSIZE;
    }
    d->buffer_alloc((t_CKINT)buflen);
    
    RETURN->v_dur = (t_CKDUR)buflen;
}

CK_DLL_CGET( LiSaMulti_cget_size )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	
	RETURN->v_dur = (t_CKDUR)d->mdata_len;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_start_record()
// desc: CTRL function ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_start_record )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->record = GET_NEXT_INT(ARGS);
    
    RETURN->v_int = (t_CKINT)d->record;    
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_start_play()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_start_play )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->play[which] = GET_NEXT_INT(ARGS);
    //CK_FPRINTF_STDERR( "voice %d playing = %d\n", which, d->play[which] );
	
	//turn off ramping toggles
	d->rampdown[which] = false;
	d->rampup[which] = false;
    
    RETURN->v_int = (t_CKINT)d->play[which];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_start_play()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_start_play0 )
{

    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->play[0] = GET_NEXT_INT(ARGS);
    //CK_FPRINTF_STDERR( "voice %d playing = %d\n", which, d->play[which] );

	//turn off ramping toggles
	d->rampdown[0] = false;
	d->rampup[0] = false;
    
    RETURN->v_int = (t_CKINT)d->play[0];
    
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_rate()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_rate )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->p_inc[which] = (t_CKDOUBLE)GET_NEXT_FLOAT(ARGS); 
														
    //CK_FPRINTF_STDERR( "setting voice %d rate to %f\n", which, d->p_inc[which] );
    
    RETURN->v_float = d->p_inc[which];
}


CK_DLL_CTRL( LiSaMulti_ctrl_rate0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->p_inc[0] = (t_CKDOUBLE)GET_CK_FLOAT(ARGS);  
	
    //CK_FPRINTF_STDERR( "setting voice %d rate to %f\n", 0, d->p_inc[0] );
    
    RETURN->v_float = d->p_inc[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_rate()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_cget_rate )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    //CK_FPRINTF_STDERR( "setting voice %d rate to %f\n", which, d->p_inc[which] );
    
    RETURN->v_float = d->p_inc[which];
}


CK_DLL_CTRL( LiSaMulti_cget_rate0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    //CK_FPRINTF_STDERR( "setting voice %d rate to %f\n", which, d->p_inc[which] );
    
    RETURN->v_float = d->p_inc[0];
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_pindex()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_pindex )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->pindex[which] = (t_CKDOUBLE)GET_NEXT_DUR(ARGS);
    
    RETURN->v_dur = (t_CKDUR)d->pindex[which];
}


CK_DLL_CTRL( LiSaMulti_ctrl_pindex0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->pindex[0] = (t_CKDOUBLE)GET_NEXT_DUR(ARGS);
    
    RETURN->v_dur = (t_CKDUR)d->pindex[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_pindex()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_pindex )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);

    // return
    RETURN->v_dur = (t_CKDUR)d->pindex[which];
}


CK_DLL_CGET( LiSaMulti_cget_pindex0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_dur = (t_CKDUR)d->pindex[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_pindex()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_rindex )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->rindex = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
    
    RETURN->v_dur = (t_CKDUR)d->rindex;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_pindex()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_rindex )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_dur = (t_CKDUR)d->rindex;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_lstart()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_lstart )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->loop_start[which] = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
    
	if (d->loop_start[which] < 0) d->loop_start[which] = 0;
	
    RETURN->v_dur = (t_CKDUR)d->loop_start[which];
}


CK_DLL_CTRL( LiSaMulti_ctrl_lstart0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->loop_start[0] = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
    
	if (d->loop_start[0] < 0) d->loop_start[0] = 0;
	
    RETURN->v_dur = (t_CKDUR)d->loop_start[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_lstart()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_lstart )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    
    t_CKINT which = GET_NEXT_INT(ARGS);
    // return
    RETURN->v_dur = (t_CKDUR)d->loop_start[which];
}


CK_DLL_CGET( LiSaMulti_cget_lstart0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    
    // return
    RETURN->v_dur = (t_CKDUR)d->loop_start[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_lend()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_lend )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->loop_end[which] = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
	
	//check to make sure loop_end is not too large
	if (d->loop_end[which] >= d->mdata_len) d->loop_end[which] = d->mdata_len - 1;
	
	RETURN->v_dur = (t_CKDUR)d->loop_end[which];
}


CK_DLL_CTRL( LiSaMulti_ctrl_lend0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->loop_end[0] = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
	
	//check to make sure loop_end is not too large
	if (d->loop_end[0] >= d->mdata_len) d->loop_end[0] = d->mdata_len - 1;
    
    RETURN->v_dur = (t_CKDUR)d->loop_end[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_lend()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_lend )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);

    // return
    RETURN->v_dur = (t_CKDUR)d->loop_end[which];
}


CK_DLL_CGET( LiSaMulti_cget_lend0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_dur = (t_CKDUR)d->loop_end[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_loop()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_loop )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->loopplay[which] = (t_CKBOOL)GET_NEXT_INT(ARGS);
}


CK_DLL_CTRL( LiSaMulti_ctrl_loop0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->loopplay[0] = (t_CKBOOL)GET_NEXT_INT(ARGS);
    
    RETURN->v_int = (t_CKINT)d->loopplay[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_loop()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_loop )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);

    // return
    RETURN->v_int = (t_CKINT)d->loopplay[which];
}


CK_DLL_CGET( LiSaMulti_cget_loop0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = (t_CKINT)d->loopplay[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_bi()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_bi )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);
    d->bi[which] = (t_CKBOOL)GET_NEXT_INT(ARGS);
    
    RETURN->v_int = (t_CKINT)d->bi[which];
}


CK_DLL_CTRL( LiSaMulti_ctrl_bi0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->bi[0] = (t_CKBOOL)GET_NEXT_INT(ARGS);
    
    RETURN->v_int = (t_CKINT)d->bi[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_bi()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_bi )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT which = GET_NEXT_INT(ARGS);

    // return
    RETURN->v_int = (t_CKINT)d->bi[which];
}


CK_DLL_CGET( LiSaMulti_cget_bi0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = (t_CKINT)d->bi[0];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_loop_end_rec()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_loop_end_rec )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->loop_end_rec = /* gewang-> */(t_CKINT)GET_NEXT_DUR(ARGS);
    
    RETURN->v_dur = (t_CKDUR)d->loop_end_rec;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_loop_end_rec()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_loop_end_rec )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_dur = (t_CKDUR)d->loop_end_rec;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_loop_rec()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_loop_rec )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->looprec = (t_CKBOOL)GET_NEXT_INT(ARGS);
    
    RETURN->v_int = (t_CKINT)d->looprec;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_loop_rec()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_loop_rec )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = (t_CKINT)d->looprec;
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_sample(); put a sample directly into record buffer
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_sample )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	SAMPLE sample_in = (SAMPLE)GET_NEXT_FLOAT(ARGS);
    int index_in = (t_CKINT)GET_NEXT_DUR(ARGS);
	
	d->pokeSample( sample_in, index_in );
    
    RETURN->v_float = (t_CKFLOAT)sample_in; //pass input through
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_sample(); grab a sample from the record buffer
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_sample )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	double index_in = (t_CKDOUBLE)GET_NEXT_DUR(ARGS);
    // return
    RETURN->v_float = (t_CKFLOAT)d->grabSample( index_in ); //change this to getSamp for interpolation
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_voicegain()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_voicegain )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT which = GET_NEXT_INT(ARGS);
    d->voiceGain[which] = (t_CKDOUBLE)GET_NEXT_FLOAT(ARGS);
    
    RETURN->v_float = (t_CKFLOAT)d->voiceGain[which];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_voicegain()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_voicegain )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT which = GET_NEXT_INT(ARGS);
	
    // return
    RETURN->v_float = (t_CKFLOAT)d->voiceGain[which];
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_voicepan()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_voicepan )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT which = GET_NEXT_INT(ARGS);
    d->voicePan[which] = (t_CKFLOAT)GET_NEXT_FLOAT(ARGS);
	
    t_CKINT i;

	for(i=0; i<d->num_chans; i++) d->channelGain[which][i] = 0.;
	
	for(i=0; i<d->num_chans; i++) {
		t_CKINT panTrunc = (t_CKINT)d->voicePan[which];
		//CK_FPRINTF_STDERR( "panTrunc = %d, panFloat = %f, i = %d\n", panTrunc, d->voicePan[which], i );
		if(i == panTrunc) {
			d->channelGain[which][i] = 1. - ( d->voicePan[which] - (t_CKFLOAT)i );
			if(i == d->num_chans - 1) {
				d->channelGain[which][0] = 1. - d->channelGain[which][i];
				d->channelGain[which][0] = sqrt(d->channelGain[which][0]);
			}
			else {
				d->channelGain[which][i+1] = 1. - d->channelGain[which][i];
				d->channelGain[which][i+1] = sqrt(d->channelGain[which][i+1]);
			}
			
			d->channelGain[which][i] = sqrt(d->channelGain[which][i]);
			
		}
		
		//CK_FPRINTF_STDERR( "gain for channel %d and voice %d = %f\n", i, which, d->channelGain[which][i] );
	}
    
    RETURN->v_float = (t_CKFLOAT)d->voicePan[which];
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_voicepan()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_voicepan )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT which = GET_NEXT_INT(ARGS);
	
    // return
    RETURN->v_float = (t_CKFLOAT)d->voicePan[which];
}

CK_DLL_CTRL( LiSaMulti_ctrl_voicepan0 )
{
	LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT which = 0; //voice 0 for this one
    d->voicePan[which] = (t_CKFLOAT)GET_NEXT_FLOAT(ARGS);
	
    t_CKINT i;

	for(i=0; i<d->num_chans; i++) d->channelGain[which][i] = 0.;
	
	for(i=0; i<d->num_chans; i++) {
		t_CKINT panTrunc = (t_CKINT)d->voicePan[which];
		//CK_FPRINTF_STDERR( "panTrunc = %d, panFloat = %f, i = %d\n", panTrunc, d->voicePan[which], i );
		if(i == panTrunc) {
			d->channelGain[which][i] = 1. - ( d->voicePan[which] - (t_CKFLOAT)i );
			if(i == d->num_chans - 1) {
				d->channelGain[which][0] = 1. - d->channelGain[which][i];
				d->channelGain[which][0] = sqrt(d->channelGain[which][0]);
			}
			else {
				d->channelGain[which][i+1] = 1. - d->channelGain[which][i];
				d->channelGain[which][i+1] = sqrt(d->channelGain[which][i+1]);
			}
			
			d->channelGain[which][i] = sqrt(d->channelGain[which][i]);
			
		}
		
		//CK_FPRINTF_STDERR( "gain for channel %d and voice %d = %f\n", i, which, d->channelGain[which][i] );
	}
    
    RETURN->v_float = (t_CKFLOAT)d->voicePan[which];
}

CK_DLL_CGET( LiSaMulti_cget_voicepan0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	
	// return
    RETURN->v_float = (t_CKFLOAT)d->voicePan[0];
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_coeff()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_coeff )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	d->coeff = (t_CKDOUBLE)GET_NEXT_FLOAT(ARGS);
	
	RETURN->v_float = (t_CKFLOAT)d->coeff;
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_coeff()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_coeff )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_float = (t_CKFLOAT)d->coeff;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_clear()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_clear )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->clear_buf();
}



//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_voice()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_voice )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = (t_CKINT)d->get_free_voice();
}


// ramp stuff
//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_rampup()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_rampup )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT voice = GET_NEXT_INT(ARGS);
    t_CKDUR len = GET_NEXT_DUR(ARGS);

    d->ramp_up(voice, len);
    
    RETURN->v_dur = (t_CKDUR)len;    
}


CK_DLL_CTRL( LiSaMulti_ctrl_rampup0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKDUR len = GET_NEXT_DUR(ARGS);

    d->ramp_up(0, len);
    
    RETURN->v_dur = (t_CKDUR)len;    
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_rampdown()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_rampdown )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT voice = GET_NEXT_INT(ARGS);
    t_CKDUR len = GET_NEXT_DUR(ARGS);

    d->ramp_down(voice, len);
    
    RETURN->v_dur = (t_CKDUR)len;    
}


CK_DLL_CTRL( LiSaMulti_ctrl_rampdown0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKDUR len = GET_NEXT_DUR(ARGS);

    d->ramp_down(0, len);
    
    RETURN->v_dur = (t_CKDUR)len;    
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_rec_ramplen()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_rec_ramplen )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKDUR newramplen = GET_NEXT_DUR(ARGS);
    d->set_rec_ramplen(newramplen);
    
    RETURN->v_dur = (t_CKDUR)newramplen;    
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_maxvoices()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_maxvoices )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->maxvoices = GET_NEXT_INT(ARGS);
	if( d->maxvoices > LiSa_MAXVOICES) {
		d->maxvoices = LiSa_MAXVOICES;
		CK_FPRINTF_STDERR( "LiSa: MAXVOICES limited to  %d.\n", LiSa_MAXVOICES );
	}
    RETURN->v_int = d->maxvoices;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_maxvoices()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_maxvoices )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = d->maxvoices;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_samp()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_value )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    t_CKINT voice = GET_NEXT_INT(ARGS);
    t_CKDOUBLE where = (t_CKDOUBLE) GET_NEXT_DUR(ARGS);

    // return
    RETURN->v_dur = (t_CKDUR)d->getSamp(where, voice);
}


CK_DLL_CGET( LiSaMulti_cget_value0 )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    t_CKDOUBLE where = (t_CKDOUBLE) GET_NEXT_DUR(ARGS);
    // return
    RETURN->v_dur = (t_CKDUR)d->getSamp(where, 0);
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_ctrl_track()
// desc: CTRL function
//-----------------------------------------------------------------------------
CK_DLL_CTRL( LiSaMulti_ctrl_track )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
    d->track = (t_CKINT)GET_NEXT_INT(ARGS);
    
    RETURN->v_int = d->track;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_sync()
// desc: CGET function
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_track )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);

    // return
    RETURN->v_int = d->track;
}


//-----------------------------------------------------------------------------
// name: LiSaMulti_cget_playing()
// desc: CGET function; indicates whether a particular voice is playing
//-----------------------------------------------------------------------------
CK_DLL_CGET( LiSaMulti_cget_playing )
{
    LiSaMulti_data * d = (LiSaMulti_data *)OBJ_MEMBER_UINT(SELF, LiSaMulti_offset_data);
	t_CKINT voice = GET_NEXT_INT(ARGS);
	if(voice >= d->maxvoices) {
		CK_FPRINTF_STDERR( "LiSa: requesting info greater than MAXVOICES %d.\n", LiSa_MAXVOICES );
		voice = 0;
	}
	
    // return
    RETURN->v_int = d->play[voice];
}

//-----------------------------------------------------------------------------
// name: LiSaMulti_pmsg()
// desc: PMSG function ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG(LiSaMulti_pmsg )
{
    return FALSE;
}
