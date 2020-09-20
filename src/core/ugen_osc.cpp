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
// file: ugen_osc.cpp
// desc: oscilliator unit generators
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Dan Trueman (dtrueman@princeton.edu)
// date: Summer 2004
//-----------------------------------------------------------------------------
#include "ugen_osc.h"
#include "chuck_type.h"
#include "chuck_ugen.h"
#include "chuck_compile.h"
#include <math.h>
#include <stdio.h>

static t_CKUINT g_srate = 0;
// for member data offset
static t_CKUINT osc_offset_data = 0;


//-----------------------------------------------------------------------------
// name: osc_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY osc_query( Chuck_DL_Query * QUERY )
{
    // srate
    g_srate = QUERY->srate;
    // get the env
    Chuck_Env * env = QUERY->env();

    Chuck_DL_Func * func = NULL;

    // register deprecate
    type_engine_register_deprecate( env, "osc", "Osc" );
    type_engine_register_deprecate( env, "phasor", "Phasor" );
    type_engine_register_deprecate( env, "sinosc", "SinOsc" );
    type_engine_register_deprecate( env, "triosc", "TriOsc" );
    type_engine_register_deprecate( env, "sawosc", "SawOsc" );
    type_engine_register_deprecate( env, "pulseosc", "PulseOsc" );
    type_engine_register_deprecate( env, "sqrosc", "SqrOsc" );

    std::string doc;
    
    //---------------------------------------------------------------------
    // init as base class: osc
    //---------------------------------------------------------------------
    doc = "Base class for simple oscillator unit generators.";
    if( !type_engine_import_ugen_begin( env, "Osc", "UGen", env->global(), 
                                        osc_ctor, osc_dtor, osc_tick, osc_pmsg,
                                        doc.c_str() ) )
        return FALSE;

    // add member variable
    osc_offset_data = type_engine_import_mvar( env, "int", "@osc_data", FALSE );
    if( osc_offset_data == CK_INVALID_OFFSET ) goto error;

    // add ctrl: freq
    func = make_new_mfun( "float", "freq", osc_ctrl_freq );
    func->add_arg( "float", "hz" );
    func->doc = "Frequency of oscillator in Hertz (cycles per second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "freq", osc_cget_freq );
    func->doc = "Frequency of oscillator in Hertz (cycles per second).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: period
    func = make_new_mfun( "dur", "period", osc_ctrl_period );
    func->add_arg( "dur", "value" );
    func->doc = "Period of oscillator (inverse of frequency).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "dur", "period", osc_cget_period );
    func->doc = "Period of oscillator (inverse of frequency).";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // add ctrl: sfreq ( == freq ) 
    func = make_new_mfun( "float", "sfreq", osc_ctrl_freq );
    func->add_arg( "float", "hz" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: phase
    func = make_new_mfun( "float", "phase", osc_ctrl_phase );
    func->add_arg( "float", "phase" );
    func->doc = "Oscillator phase, in range [0,1). ";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "float", "phase", osc_cget_phase );
    func->doc = "Oscillator phase, in range [0,1). ";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ctrl: sync
    func = make_new_mfun( "int", "sync", osc_ctrl_sync );
    func->add_arg( "int", "type" );
    func->doc = "Mode for input (if any). 0: sync frequency to input, 1: sync phase to input, 2: frequency modulation (add input to set frequency)";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "sync", osc_cget_sync );
    func->doc = "Mode for input (if any). 0: sync frequency to input, 1: sync phase to input, 2: frequency modulation (add input to set frequency)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // phasor
    //---------------------------------------------------------------------
    doc = "Phasor oscillator. Linearly rises from 0 to 1. Can be used as a phase control.";
    if( !type_engine_import_ugen_begin( env, "Phasor", "Osc", env->global(), 
                                        NULL, NULL, osc_tick, NULL,
                                        doc.c_str() ) )
        return FALSE;

    // end the class import
    type_engine_import_class_end( env );
    
    //---------------------------------------------------------------------
    // sinosc
    //---------------------------------------------------------------------
    doc = "Sine wave oscillator.";
    if( !type_engine_import_ugen_begin( env, "SinOsc", "Osc", env->global(),
                                        NULL, NULL, sinosc_tick, NULL,
                                        doc.c_str() ) )
        return FALSE;
    
    type_engine_import_add_ex( env, "basic/whirl.ck" );

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // triosc - triangle oscillator
    //---------------------------------------------------------------------
    doc = "Triangle wave oscillator.";
    if( !type_engine_import_ugen_begin( env, "TriOsc", "Osc", env->global(),
                                        NULL, NULL, triosc_tick, NULL,
                                        doc.c_str() ) )
        return FALSE;
    
    func = make_new_mfun( "float", "width", osc_ctrl_width );
    func->add_arg( "float", "width" );
    func->doc = "Width of triangle wave (ratio of rise time to fall time).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "width", osc_cget_width );
    func->doc = "Width of triangle wave (ratio of rise time to fall time).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // sawosc - sawtooth oscillator  (  0 | 1  triangle wave  )
    //---------------------------------------------------------------------
    doc = "Sawtooth wave oscillator.";
    if( !type_engine_import_ugen_begin( env, "SawOsc", "TriOsc", env->global(),
                                        sawosc_ctor, NULL, NULL, NULL,
                                        doc.c_str() ) )
        return FALSE;

    func = make_new_mfun( "float", "width", sawosc_ctrl_width );
    func->add_arg( "float", "width" );
    func->doc = "Whether falling sawtooth wave (0) or rising sawtooth wave (1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float", "width", osc_cget_width );
    func->doc = "Whether falling sawtooth wave (0) or rising sawtooth wave (1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // pulseosc - pulse-width oscillator
    //---------------------------------------------------------------------
    doc = "Pulse width oscillator.";
    if( !type_engine_import_ugen_begin( env, "PulseOsc", "Osc", env->global(),
                                        NULL, NULL, pulseosc_tick, NULL,
                                        doc.c_str() ) )
        return FALSE;

    func = make_new_mfun( "float", "width", osc_ctrl_width );
    func->add_arg( "float", "width" );
    func->doc = "Length of duty cycle [0,1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    func = make_new_mfun( "float", "width", osc_cget_width );
    func->doc = "Length of duty cycle [0,1).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // sqrosc - square_wave oscillator ( 0.5 pulse ) 
    //---------------------------------------------------------------------
    doc = "Square wave oscillator (pulse with 0.5 duty cycle).";
    if( !type_engine_import_ugen_begin( env, "SqrOsc", "PulseOsc", env->global(),
                                        sqrosc_ctor, NULL, NULL, NULL,
                                        doc.c_str() ) )
        return FALSE;
    
    func = make_new_mfun( "float", "width", sqrosc_ctrl_width );
    func->doc = "Length of duty cycle (always 0.5)";
    if( !type_engine_import_mfun( env, func ) ) goto error;    

    // end the class import
    type_engine_import_class_end( env );

    // include GenX!!!
    if( !genX_query( QUERY ) )
        return FALSE;

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: struct Osc_Data
// desc: ...
//-----------------------------------------------------------------------------
struct Osc_Data
{
    t_CKFLOAT num;
    t_CKFLOAT freq;
    int    sync; 
    t_CKUINT srate;
    t_CKFLOAT width;
    
    t_CKFLOAT phase;
    
    Osc_Data()
    {
        num = 0.0;
        freq = 220.0;
        sync = 0; // internal 
        width = 0.5;
        srate = g_srate;
        phase = 0.0;
    }
};




//-----------------------------------------------------------------------------
// name: osc_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( osc_ctor )
{
    Osc_Data * d = new Osc_Data;
    Chuck_DL_Return r;
    // return data to be used later
    OBJ_MEMBER_UINT(SELF, osc_offset_data) = (t_CKUINT)d;
    osc_ctrl_freq( SELF, &(d->freq), &r, VM, SHRED, API );
}




//-----------------------------------------------------------------------------
// name: osc_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( osc_dtor )
{
    // get the data
    Osc_Data * data = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // delete
    delete data;
    // set to NULL
    OBJ_MEMBER_UINT(SELF, osc_offset_data) = 0;
}




//-----------------------------------------------------------------------------
// name: osc_tick()
// desc: ...
//
// basic osx is a phasor... 
// we use a duty-cycle rep ( 0 - 1 ) rather than angular ( 0 - TWOPI )
// sinusoidal oscillators are special
//
// (maybe) as a rule, we store external phase control values
// so that we can have a smooth change back to internal control -pld
//
// technically this should happen even with external phase control
// that we'd be in the right place when translating back to internal... 
// this was decidely inefficient and nit-picky.  -pld 
//
//-----------------------------------------------------------------------------
CK_DLL_TICK( osc_tick )
{
    // get the data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    t_CKBOOL inc_phase = TRUE;

    // if input
    if( ugen->m_num_src )
    {
        // sync frequency to input
        if( d->sync == 0 )
        {
            // set freq
            d->freq = in;
            // phase increment
            d->num = d->freq / d->srate;
            // bound it
            if( d->num >= 1.0 || d->num < 0.0 ) d->num -= floor( d->num );
        }
        // synch phase to input
        else if( d->sync == 1 )
        {
            // set phase
            d->phase = in;
            // no update
            inc_phase = FALSE;
            // bound it (thanks Pyry)
            if( d->phase > 1.0 || d->phase < 0.0 ) d->phase -= floor( d->phase );
        }
        // fm synthesis
        else if( d->sync == 2 )
        {
            // set freq
            t_CKFLOAT freq = d->freq + in;
            // phase increment
            d->num = freq / d->srate;
            // bound it
            if( d->num >= 1.0 || d->num < 0.0 ) d->num -= floor( d->num );
        }
        // sync to now
        // else if( d->sync == 3 )
        // {
        //     d->phase = now * d->num;
        //     inc_phase = FALSE;
        // }
    }

    // set output to current phase
    *out = (SAMPLE)d->phase;
    
    // check
    if( inc_phase )
    {
        // step the phase.
        d->phase += d->num;
        // keep the phase between 0 and 1
        if( d->phase > 1.0 ) d->phase -= 1.0;
        else if( d->phase < 0.0 ) d->phase += 1.0;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: sinosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( sinosc_tick )
{
    // get the data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    t_CKBOOL inc_phase = TRUE;

    // if input
    if( ugen->m_num_src )
    {
        // sync frequency to input
        if( d->sync == 0 )
        {
            // set freq
            d->freq = in;
            // phase increment
            d->num = d->freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync phase to input
        else if( d->sync == 1 )
        {
            // set freq
            d->phase = in;
            inc_phase = FALSE;
        }
        // FM synthesis
        else if( d->sync == 2 )
        {
            // set freq
            t_CKFLOAT freq = d->freq + in;
            // phase increment
            d->num = freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync phase to now
        // else if( d->sync == 3 )
        // {
        //     d->phase = now * d->num;
        //     inc_phase = FALSE;
        // }
    }

    // set output
    *out = (SAMPLE) ::sin( d->phase * TWO_PI );

    if( inc_phase )
    {
        // next phase
        d->phase += d->num;
        // keep the phase between 0 and 1
        if( d->phase > 1.0 ) d->phase -= 1.0;
        else if( d->phase < 0.0 ) d->phase += 1.0;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: triosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( triosc_tick )
{
    // get the data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    t_CKBOOL inc_phase = TRUE;

    // if input
    if( ugen->m_num_src )
    {
        // sync frequency to input
        if( d->sync == 0 )
        {
            // set freq
            d->freq = in;
            // phase increment
            d->num = d->freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync phase to input
        else if( d->sync == 1 )
        {
            // set freq
            d->phase = in;
            inc_phase = FALSE;
        }
        // FM synthesis
        else if( d->sync == 2 )
        {
            // set freq
            t_CKFLOAT freq = d->freq + in;
            // phase increment
            d->num = freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync to now
        // if( d->sync == 3 )
        // {
        //     d->phase = now * d->num;
        //     inc_phase = FALSE;
        // }
    }

    // compute
    t_CKFLOAT phase = d->phase + .25; if( phase > 1.0 ) phase -= 1.0;
    if( phase < d->width ) *out = (SAMPLE) (d->width == 0.0) ? 1.0 : -1.0 + 2.0 * phase / d->width; 
    else *out = (SAMPLE) (d->width == 1.0) ? 0 : 1.0 - 2.0 * (phase - d->width) / (1.0 - d->width);

    // advance internal phase
    if( inc_phase )
    {
        d->phase += d->num;
        // keep the phase between 0 and 1
        if( d->phase > 1.0 ) d->phase -= 1.0;
        else if( d->phase < 0.0 ) d->phase += 1.0;
    }

    return TRUE;
}


// sawosc_tick is tri_osc tick with width=0.0 or width=1.0  -pld 


//-----------------------------------------------------------------------------
// name: pulseosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( pulseosc_tick )
{
    // get the data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
    t_CKBOOL inc_phase = TRUE;

    // if input
    if( ugen->m_num_src )
    {
        // sync frequency to input
        if( d->sync == 0 )
        {
            // set freq
            d->freq = in;
            // phase increment
            d->num = d->freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync phase to input
        else if( d->sync == 1 )
        {
            // set freq
            d->phase = in;
            inc_phase = FALSE;
        }
        // FM synthesis
        else if( d->sync == 2 )
        {
            // set freq
            t_CKFLOAT freq = d->freq + in;
            // phase increment
            d->num = freq / d->srate;
            // bound it
            if( d->num >= 1.0 ) d->num -= floor( d->num );
            else if( d->num <= -1.0 ) d->num += floor( d->num );
        }
        // sync to now
        // if( d->sync == 3 )
        // {
        //     d->phase = now * d->num;
        //     inc_phase = FALSE;
        // }
    }

    // compute
    *out = (SAMPLE) (d->phase < d->width) ? 1.0 : -1.0;

    // move phase
    if( inc_phase )
    {
        d->phase += d->num;
        // keep the phase between 0 and 1
        if( d->phase > 1.0 ) d->phase -= 1.0;
		else if( d->phase < 0.0 ) d->phase += 1.0;
    }

    return TRUE;
}


// sqrosc_tick is pulseosc_tick at width=0.5 -pld;


//-----------------------------------------------------------------------------
// name: osc_ctrl_freq()
// desc: set oscillator frequency
//-----------------------------------------------------------------------------
CK_DLL_CTRL( osc_ctrl_freq )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data);
    // set freq
    d->freq = GET_CK_FLOAT(ARGS);
    // phase increment
    d->num = d->freq / d->srate;
    // bound it
    if( d->num >= 1.0 ) d->num -= ::floor( d->num );
    // return
    RETURN->v_float = (t_CKFLOAT)d->freq;
}




//-----------------------------------------------------------------------------
// name: osc_cget_freq()
// desc: get oscillator frequency
//-----------------------------------------------------------------------------
CK_DLL_CGET( osc_cget_freq )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data);
    // return
    RETURN->v_float = (t_CKFLOAT)d->freq;
}




//-----------------------------------------------------------------------------
// name: osc_ctrl_period()
// desc: set oscillator period
//-----------------------------------------------------------------------------
CK_DLL_CTRL( osc_ctrl_period )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data);
    t_CKDUR period = GET_CK_DUR(ARGS);
    // test
    if( period == 0.0 ) d->freq = 0.0;
    // set freq
    else d->freq = 1 / (period / d->srate);
    d->num = d->freq / d->srate;
    // bound it
    if( d->num >= 1.0 ) d->num -= ::floor( d->num );
    // return
    RETURN->v_dur = period;
}




//-----------------------------------------------------------------------------
// name: osc_cget_period()
// desc: get oscillator period
//-----------------------------------------------------------------------------
CK_DLL_CGET( osc_cget_period )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data);
    t_CKDUR period = 0;

    // get period
    if( d->freq != 0.0 ) period = 1 / d->freq * d->srate;

    // return
    RETURN->v_dur = period;
}




//-----------------------------------------------------------------------------
// name: osc_ctrl_phase()
// desc: set oscillator phase wrapped to ( 0 - 1 )
//-----------------------------------------------------------------------------
CK_DLL_CTRL( osc_ctrl_phase )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // set freq
    d->phase = GET_CK_FLOAT(ARGS);
    //bound ( this could be set arbitrarily high or low ) 
    if ( d->phase >= 1.0 || d->phase < 0.0 ) d->phase -= floor( d->num );
    // return
    RETURN->v_float = (t_CKFLOAT)d->phase;
}




//-----------------------------------------------------------------------------
// name: osc_cget_phase()
// desc: get oscillator phase wrapped to ( 0 - 1 )
//-----------------------------------------------------------------------------
CK_DLL_CGET( osc_cget_phase )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // return
    RETURN->v_float = (t_CKFLOAT)d->phase;
}




//-----------------------------------------------------------------------------
// name: osc_ctrl_width()
// desc: set width of active phase ( bound 0.0 - 1.0 );
//-----------------------------------------------------------------------------
CK_DLL_CTRL( osc_ctrl_width )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // set freq
    d->width = GET_CK_FLOAT(ARGS);
    //bound ( this could be set arbitrarily high or low ) 
    d->width = ck_max( 0.0, ck_min( 1.0, d->width ) );
    // return
    RETURN->v_float = (t_CKFLOAT)d->width;
}




//-----------------------------------------------------------------------------
// name: osc_cget_width()
// desc: get width of active phase ( bound 0.0 - 1.0 );
//-----------------------------------------------------------------------------
CK_DLL_CGET( osc_cget_width )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // return
    RETURN->v_float = (t_CKFLOAT)d->width;
}




//-----------------------------------------------------------------------------
// name: sqrosc_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( sqrosc_ctor )
{
    Osc_Data * d = new Osc_Data;
    Chuck_DL_Return r;
    sqrosc_ctrl_width( SELF, &(d->width), &r, VM, SHRED, API );
}




//-----------------------------------------------------------------------------
// name: sqrosc_ctrl_width()
// desc: force width to 0.5;
//-----------------------------------------------------------------------------
CK_DLL_CTRL( sqrosc_ctrl_width )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // force value
    d->width = 0.5; 
    // return
    RETURN->v_float = (t_CKFLOAT)d->width;
}




//-----------------------------------------------------------------------------
// name: sawosc_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( sawosc_ctor )
{
    Osc_Data * d = new Osc_Data;
    Chuck_DL_Return r;
    sawosc_ctrl_width( SELF, &(d->width), &r, VM, SHRED, API );
}




//-----------------------------------------------------------------------------
// name: sawosc_ctrl_width()
// force width to 0.0 ( falling ) or 1.0 ( rising ) 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( sawosc_ctrl_width )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // set freq
    d->width = GET_CK_FLOAT(ARGS);
    // bound ( this could be set arbitrarily high or low ) 
    d->width = ( d->width < 0.5 ) ? 0.0 : 1.0;  //rising or falling
    // return
    RETURN->v_float = (t_CKFLOAT)d->width;
}




//-----------------------------------------------------------------------------
// name: osc_ctrl_sync()
// desc: select sync mode for oscillator
//-----------------------------------------------------------------------------
CK_DLL_CTRL( osc_ctrl_sync )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // set sync
    t_CKINT psync = d->sync;
    d->sync = GET_CK_INT(ARGS);

    // bound ( this could be set arbitrarily high or low ) 
    if( d->sync < 0 || d->sync > 2 )
        d->sync = 0;

    if( d->sync == 0 && psync != d->sync )
    {
        // if we are switching to internal tick
        // we need to pre-advance the phase... 
        // this is probably stupid.  -pld
        d->phase += d->num;
        // keep the phase between 0 and 1
        if( d->phase > 1.0 ) d->phase -= 1.0;
    }
    // return
    RETURN->v_int = (t_CKINT)d->sync;
}




//-----------------------------------------------------------------------------
// name: osc_cget_sync()
// desc: get sync mode for oscillator
//-----------------------------------------------------------------------------
CK_DLL_CGET( osc_cget_sync )
{
    // get data
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    // return
    RETURN->v_int = (t_CKINT)d->sync;
}




//-----------------------------------------------------------------------------
// name: osc_pmsg()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( osc_pmsg )
{
    Osc_Data * d = (Osc_Data *)OBJ_MEMBER_UINT(SELF, osc_offset_data );
    if( !strcmp( MSG, "print" ) )
    {
        CK_FPRINTF_STDOUT( "SinOsc: (freq=%f)", d->freq );
        return TRUE;
    }
    
    // didn't handle
    return FALSE;
}




//-----------------------------------------------------------------------------
// file: ugen_genX.cpp
// desc: thought it would be a good way to learn the fascinating innards of
//       ChucK by porting some of the classic lookup table functions and adding
//       a few new ones that might be of use.
//       mostly ported from RTcmix (all by WarpTable)
//
// author: Dan Trueman (dtrueman.princeton.edu)
// date: Winter 2007
//-----------------------------------------------------------------------------
// for member data offset
static t_CKUINT genX_offset_data = 0;
// for internal usage
static void _transition( t_CKDOUBLE a, t_CKDOUBLE alpha, t_CKDOUBLE b, 
                         t_CKINT n, t_CKDOUBLE * output );



//-----------------------------------------------------------------------------
// name: genX_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY genX_query( Chuck_DL_Query * QUERY )
{
    // srate
    g_srate = QUERY->srate;
    // get the env
    Chuck_Env * env = QUERY->env();
    std::string doc;
    Chuck_DL_Func * func = NULL;
    
    doc = "Ported from rtcmix. See <a href=\"http://www.music.columbia.edu/cmix/makegens.html\">\
    http://www.music.columbia.edu/cmix/makegens.html</a> \
    for more information on the GenX family of UGens. Currently coefficients past \
    the 100th are ignored.\
    \
    Lookup can either be done using the lookup() function, or by driving the \
    table with an input UGen, typically a Phasor. For an input signal between \
    [ -1, 1 ], using the absolute value for [ -1, 0 ), GenX will output the \
    table value indexed by the current input.";
    
    //---------------------------------------------------------------------
    // init as base class: genX
    //---------------------------------------------------------------------
    if( !type_engine_import_ugen_begin( env, "GenX", "UGen", env->global(), 
                                        genX_ctor, genX_dtor, genX_tick, genX_pmsg,
                                        doc.c_str() ) )
        return FALSE;

    // add member variable
    genX_offset_data = type_engine_import_mvar( env, "int", "@GenX_data", FALSE );
    if( genX_offset_data == CK_INVALID_OFFSET ) goto error;
        
    func = make_new_mfun( "float", "lookup", genX_lookup ); //lookup table value
    func->add_arg( "float", "which" );
    func->doc = "Returns lookup table value at index i [ -1, 1 ]. Absolute value is used in the range [ -1, 0 ).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    func = make_new_mfun( "float[]", "coefs", genX_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients. Meaning is dependent on subclass.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // gen5
    //---------------------------------------------------------------------
    doc = "Constructs a lookup table composed of sequential exponential curves. "
    "For a table with N curves, starting value of y', and value yn for lookup "
    "index xn, set the coefficients to [ y', y0, x0, ..., yN-1, xN-1 ]. "
    "Note that there must be an odd number of coefficients. "
    "If an even number of coefficients is specified, behavior is undefined. "
    "The sum of xn for 0 &le; n < N must be 1. yn = 0 is approximated as 0.000001 "
    "to avoid strange results arising from the nature of exponential curves.";
    
    if( !type_engine_import_ugen_begin( env, "Gen5", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
    
    func = make_new_mfun( "float[]", "coefs", gen5_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );



    //---------------------------------------------------------------------
    // gen7
    //---------------------------------------------------------------------
    doc = "Constructs a lookup table composed of sequential line segments. "
    "For a table with N lines, starting value of y', and value yn for lookup"
    "index xn, set the coefficients to [ y', y0, x0, ..., yN-1, xN-1 ]. "
    "Note that there must be an odd number of coefficients. "
    "If an even number of coefficients is specified, behavior is undefined. "
    "The sum of xn for 0 &le; n < N must be 1.";

    if( !type_engine_import_ugen_begin( env, "Gen7", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;

    func = make_new_mfun( "float[]", "coefs", gen7_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    

    //---------------------------------------------------------------------
    // gen9
    //---------------------------------------------------------------------
    doc = "Constructs a lookup table of partials with specified amplitudes, "
    "phases, and harmonic ratios to the fundamental. "
    "Coefficients are specified in triplets of [ ratio, amplitude, phase ] "
    "arranged in a single linear array.";
    
    if( !type_engine_import_ugen_begin( env, "Gen9", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
        
    func = make_new_mfun( "float[]", "coefs", gen9_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    


    //---------------------------------------------------------------------
    // gen10
    //---------------------------------------------------------------------
    doc = "Constructs a lookup table of harmonic partials with specified "
    "amplitudes. The amplitude of partial n is specified by the nth element of "
    "the coefficients. For example, setting coefs to [ 1 ] will produce a sine wave.";

    if( !type_engine_import_ugen_begin( env, "Gen10", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
        
    func = make_new_mfun( "float[]", "coefs", gen10_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // gen17
    //---------------------------------------------------------------------
    doc = "Constructs a Chebyshev polynomial wavetable with harmonic partials "
    "of specified weights. The weight of partial n is specified by the nth "
    "element of the coefficients. Primarily used for waveshaping, driven by a "
    "SinOsc instead of a Phasor. See "
    "<a href=\"http://crca.ucsd.edu/~msp/techniques/v0.08/book-html/node74.html\">"
    "http://crca.ucsd.edu/~msp/techniques/v0.08/book-html/node74.html</a> and "
    "<a href=\"http://en.wikipedia.org/wiki/Distortion_synthesis\">"
    "http://en.wikipedia.org/wiki/Distortion_synthesis</a> for more information.";

    if( !type_engine_import_ugen_begin( env, "Gen17", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
        
    func = make_new_mfun( "float[]", "coefs", gen17_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    
    //---------------------------------------------------------------------
    // Curve
    //---------------------------------------------------------------------
    doc = "Constructs a wavetable composed of segments of variable times, "
    "values, and curvatures. Coefficients are specified as a single linear "
    "array of triplets of [ time, value, curvature ] followed by a final duple "
    "of [ time, value ] to specify the final value of the table. time values "
    "are expressed in unitless, ascending values. For curvature equal to 0, "
    "the segment is a line; for curvature less than 0, the segment is a convex "
    "curve; for curvature greater than 0, the segment is a concave curve.";

    if( !type_engine_import_ugen_begin( env, "CurveTable", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
        
    func = make_new_mfun( "float[]", "coefs", curve_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    
    //---------------------------------------------------------------------
    // Warp
    //---------------------------------------------------------------------
    doc = "";

    if( !type_engine_import_ugen_begin( env, "WarpTable", "GenX", env->global(),
                                        NULL, NULL, genX_tick, NULL, doc.c_str() ) )
        return FALSE;
        
    func = make_new_mfun( "float[]", "coefs", warp_coeffs ); //load table
    func->add_arg( "float", "v[]" );
    func->doc = "Set lookup table coefficients.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    /*
    func = make_new_mfun( "float", "coefs", warp_coeffs ); //load table
    func->add_arg( "float", "asym" );
    func->add_arg( "float", "sym" );
    if( !type_engine_import_mfun( env, func ) ) goto error;
    */

    // end the class import
    type_engine_import_class_end( env );
    

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}




//-----------------------------------------------------------------------------
// name: struct genX_Data
// desc: ...
//-----------------------------------------------------------------------------
#define genX_tableSize 4096
#define genX_MAX_COEFFS 100

struct genX_Data
{    
    t_CKUINT genX_type;
    t_CKDOUBLE genX_table[genX_tableSize];
    // gewang: was int
    t_CKINT sync; 
    t_CKUINT srate;
    t_CKFLOAT xtemp;
    t_CKDOUBLE coeffs[genX_MAX_COEFFS];

    genX_Data()
    {
        //initialize data here
        sync        = 0;
        srate       = g_srate;

        t_CKINT i;
        for( i=0; i<genX_MAX_COEFFS; i++ ) coeffs[i] = 0.;
        for( i=0; i<genX_tableSize; i++ ) genX_table[i] = 0.;
    }
};




//-----------------------------------------------------------------------------
// name: genX_ctor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTOR( genX_ctor )
{
    genX_Data * d = new genX_Data;
    Chuck_DL_Return r;
    // return data to be used later
    OBJ_MEMBER_UINT(SELF, genX_offset_data) = (t_CKUINT)d;
    //gen10_coeffs( SELF, &(d->xtemp), &r );
}




//-----------------------------------------------------------------------------
// name: genX_dtor()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_DTOR( genX_dtor )
{
    // get the data
    genX_Data * data = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data );
    // delete
    SAFE_DELETE(data);
    // set to NULL
    OBJ_MEMBER_UINT(SELF, genX_offset_data) = 0;
}




//-----------------------------------------------------------------------------
// name: genX_tick()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_TICK( genX_tick )
{
    // get the data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data );
    Chuck_UGen * ugen = (Chuck_UGen *)SELF;
//    t_CKBOOL inc_phase = TRUE;
    
    t_CKDOUBLE in_index = 0.0;
    t_CKDOUBLE scaled_index = 0.0;
    t_CKDOUBLE alpha = 0.0, omAlpha = 0.0;
    t_CKUINT lowIndex = 0, hiIndex = 0;
    t_CKDOUBLE outvalue = 0.0;

    // if input
    if( ugen->m_num_src ) {
        in_index = in;
        // gewang: moved this to here
        if( in_index < 0. ) in_index = -in_index;
        //scaled_index = (in_index + 1.) * 0.5 * genX_tableSize; //drive with oscillator, [-1, 1]
        scaled_index = in_index * genX_tableSize; //drive with phasor [0, 1]
    } else {
        scaled_index = 0.;
    }
        
    // set up interpolation parameters
    lowIndex = (t_CKUINT)scaled_index;
    hiIndex = lowIndex + 1;
    alpha = scaled_index - lowIndex;
    omAlpha = 1. - alpha;
    
    // check table index ranges
    while(lowIndex >= genX_tableSize) lowIndex -= genX_tableSize; 
    while(hiIndex >= genX_tableSize) hiIndex -= genX_tableSize; 

    // could just call
    // outvalue = genX_lookup(in_index);?
    
    // calculate output value with linear interpolation
    outvalue = d->genX_table[lowIndex]*omAlpha + d->genX_table[hiIndex]*alpha;
    
    // set output
    *out = (SAMPLE)outvalue;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: genX_lookup()
// desc: lookup call for all gens
//-----------------------------------------------------------------------------
CK_DLL_CTRL( genX_lookup )
{
    // get the data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data );
    
    t_CKFLOAT in_index;
    t_CKFLOAT scaled_index;
    t_CKFLOAT alpha, omAlpha;
    t_CKUINT lowIndex, hiIndex;
    t_CKFLOAT outvalue;

    in_index = GET_NEXT_FLOAT(ARGS);

    // gewang: moved to here
    if (in_index < 0.) in_index = -in_index;
    scaled_index = in_index * (genX_tableSize - 1); //drive with phasor [0, 1]

    //set up interpolation parameters
    lowIndex = (t_CKUINT)scaled_index;
    hiIndex = lowIndex + 1;
    alpha = scaled_index - lowIndex;
    omAlpha = 1. - alpha;
    
    //check table index ranges
    while(lowIndex >= genX_tableSize) lowIndex -= genX_tableSize; 
    while(hiIndex >= genX_tableSize) hiIndex -= genX_tableSize; 
    
    //calculate output value with linear interpolation
    outvalue = d->genX_table[lowIndex]*omAlpha + d->genX_table[hiIndex]*alpha;
    
    RETURN->v_float = (t_CKFLOAT)outvalue;

}


//-----------------------------------------------------------------------------
// name: gen5_coeffs()
// desc: setup table for gen5
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gen5_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i = 0, j, k, l, size;
    t_CKFLOAT wmax, xmax=0.0, c, amp2, amp1, coeffs[genX_MAX_COEFFS];
    
    Chuck_Array8 * in_args = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen10coeffs, %d\n", weights );
    if(in_args == 0) return;
    size = in_args->size();
    if(size >= genX_MAX_COEFFS) size = genX_MAX_COEFFS - 1;
    
    t_CKFLOAT v;
    for(t_CKUINT ii = 0; ii<size; ii++) {
        in_args->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        coeffs[ii] = v;
    }

    amp2 = coeffs[0];
    if (amp2 <= 0.0) amp2 = 0.000001;
    for(k = 1; k < size; k += 2) {
        amp1 = amp2;
        amp2 = coeffs[k+1];
        if (amp2 <= 0.0) amp2 = 0.000001;
        j = i + 1;
        d->genX_table[i] = amp1;
        c = (t_CKFLOAT) pow((amp2/amp1),(1./(coeffs[k]*genX_tableSize)));
        i = (t_CKINT)((j - 1) + coeffs[k]*genX_tableSize);
        for(l = j; l < i; l++) {
            if(l < genX_tableSize)
                d->genX_table[l] = d->genX_table[l-1] * c;
            }
        }
   
    for(j = 0; j < genX_tableSize; j++) {
        if ((wmax = fabs(d->genX_table[j])) > xmax) xmax = wmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", wmax);
    }
    // CK_FPRINTF_STDOUT( "table max = %f\n", xmax);
    for(j = 0; j < genX_tableSize; j++) {
        d->genX_table[j] /= xmax;
    }

    // return
    RETURN->v_object = in_args;
}


//-----------------------------------------------------------------------------
// name: gen7_coeffs()
// desc: setup table for gen7
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gen7_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i=0, j, k, l, size;
    t_CKFLOAT wmax, xmax = 0.0, amp2, amp1, coeffs[genX_MAX_COEFFS];
    
    Chuck_Array8 * in_args = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen10coeffs, %d\n", weights );
    if(in_args == 0) return;
    size = in_args->size();
    if(size >= genX_MAX_COEFFS) size = genX_MAX_COEFFS - 1;
    
    t_CKFLOAT v;
    for(t_CKUINT ii = 0; ii<size; ii++) {
        in_args->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        coeffs[ii] = v;
    }
    
    amp2 = coeffs[0];
    for (k = 1; k < size; k += 2) {
      amp1 = amp2;
      amp2 = coeffs[k + 1];
      j = i + 1;
      i = (t_CKINT)(j + coeffs[k]*genX_tableSize - 1);
      for (l = j; l <= i; l++) {
         if (l <= genX_tableSize)
            d->genX_table[l - 1] = amp1 +
                (amp2 - amp1) * (double) (l - j) / (i - j + 1);
        }
    }
   
    for(j = 0; j < genX_tableSize; j++) {
        if ((wmax = fabs(d->genX_table[j])) > xmax) xmax = wmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", wmax);
    }
    // CK_FPRINTF_STDOUT( "table max = %f\n", xmax);
    for(j = 0; j < genX_tableSize; j++) {
        d->genX_table[j] /= xmax;
    }

    // return
    RETURN->v_object = in_args;
}


//-----------------------------------------------------------------------------
// name: gen9_coeffs()
// desc: setup table for gen9
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gen9_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i, j, size;
    t_CKDOUBLE wmax, xmax=0.0;
    t_CKFLOAT coeffs[genX_MAX_COEFFS];
    
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen10coeffs, %d\n", weights );
    if(weights == 0) return;
    size = weights->size();
    if(size >= genX_MAX_COEFFS) size = genX_MAX_COEFFS - 1;
    
    
    t_CKFLOAT v;
    for(t_CKUINT ii = 0; ii<size; ii++) {
        weights->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        coeffs[ii] = v;
    }
    
    for(j = size - 1; j > 0; j -= 3) {
        if(coeffs[j - 1] != 0) {
            for(i = 0; i < genX_tableSize; i++) {
                t_CKDOUBLE val = sin(TWO_PI * ((t_CKDOUBLE) i / ((t_CKDOUBLE) (genX_tableSize)
                                 / coeffs[j - 2]) + coeffs[j] / 360.));
                d->genX_table[i] += val * coeffs[j - 1];
            }
        }
    }
    
    for(j = 0; j < genX_tableSize; j++) {
        if ((wmax = fabs(d->genX_table[j])) > xmax) xmax = wmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", wmax);
    }
    // CK_FPRINTF_STDOUT( "table max = %f\n", xmax);
    for(j = 0; j < genX_tableSize; j++) {
        d->genX_table[j] /= xmax;
    }

    // return 
    RETURN->v_object = weights;
}


//-----------------------------------------------------------------------------
// name: gen10_coeffs()
// desc: setup table for gen10
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gen10_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i, j, size;
    t_CKDOUBLE wmax, xmax=0.0;
    
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen10coeffs, %d\n", weights );
    if(weights==0) return;
    size = weights->size();
    if(size >= genX_MAX_COEFFS) size = genX_MAX_COEFFS - 1;
    
    t_CKFLOAT v;
    for(t_CKUINT ii = 0; ii<size; ii++) {
        weights->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        d->coeffs[ii] = v;
    }
    
    // reset table - this allow not only to add but also to remove partials
    for( i=0; i<genX_tableSize; i++ ) d->genX_table[i] = 0.;

    j = size;
    while (j--) {
      if (d->coeffs[j] != 0) {
         for (i = 0; i < genX_tableSize; i++) {
            t_CKDOUBLE val = (t_CKDOUBLE) (TWO_PI * (t_CKDOUBLE) i / (genX_tableSize / (t_CKFLOAT)(j + 1)));
            d->genX_table[i] += sin(val) * d->coeffs[j];
         }
      }
    }
       
    for(j = 0; j < genX_tableSize; j++) {
        if ((wmax = fabs(d->genX_table[j])) > xmax) xmax = wmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", wmax);
    }

    // CK_FPRINTF_STDOUT( "table max = %f\n", xmax);
    for(j = 0; j < genX_tableSize; j++) {
        d->genX_table[j] /= xmax;
    }

    // return
    RETURN->v_object = weights;
}


//-----------------------------------------------------------------------------
// name: gen17_coeffs()
// desc: setup table for gen17
//-----------------------------------------------------------------------------
CK_DLL_CTRL( gen17_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i, j, size;
    t_CKDOUBLE Tn, Tn1, Tn2, dg, x, wmax = 0.0, xmax = 0.0;
    t_CKFLOAT coeffs[genX_MAX_COEFFS];
    
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen17coeffs, %d\n", weights );
    if(weights == 0) return;
    size = weights->size();
    if(size >= genX_MAX_COEFFS) size = genX_MAX_COEFFS - 1;
    
    dg = (t_CKDOUBLE) (genX_tableSize / 2. - .5);
    
    t_CKFLOAT v;
    for(t_CKUINT ii = 0; ii<size; ii++) {
        weights->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        coeffs[ii] = v;
    }
    
    for (i = 0; i < genX_tableSize; i++) {
        x = (t_CKDOUBLE)(i / dg - 1.);
        d->genX_table[i] = 0.0;
        Tn1 = 1.0;
        Tn = x;
        for (j = 0; j < size; j++) {
            d->genX_table[i] = coeffs[j] * Tn + d->genX_table[i];
            Tn2 = Tn1;
            Tn1 = Tn;
            Tn = 2.0 * x * Tn1 - Tn2;
        }
    }
   
    for(j = 0; j < genX_tableSize; j++) {
        if ((wmax = fabs(d->genX_table[j])) > xmax) xmax = wmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", wmax);
    }
    // CK_FPRINTF_STDOUT( "table max = %f\n", xmax);
    for(j = 0; j < genX_tableSize; j++) {
        d->genX_table[j] /= xmax;
        // CK_FPRINTF_STDOUT( "table current = %f\n", d->genX_table[j]);
    }

    // return
    RETURN->v_object = weights;
}


//-----------------------------------------------------------------------------
// name: curve_coeffs()
// desc: setup table for Curve
// ported from RTcmix
//-----------------------------------------------------------------------------
#define MAX_CURVE_PTS 256
CK_DLL_CTRL( curve_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i, points, nargs, seglen = 0, len = genX_tableSize;
    t_CKDOUBLE factor, *ptr;//, xmax=0.0;
    t_CKDOUBLE time[MAX_CURVE_PTS], value[MAX_CURVE_PTS], alpha[MAX_CURVE_PTS];
    t_CKFLOAT coeffs[genX_MAX_COEFFS];
    t_CKUINT ii = 0;
    t_CKFLOAT v = 0.0;
    
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    
    // CK_FPRINTF_STDOUT( "calling gen17coeffs, %d\n", weights );
    if(weights==0) goto done;

    nargs = weights->size();
    if (nargs < 5 || (nargs % 3) != 2)  {   // check number of args
        CK_FPRINTF_STDERR( "[chuck](via CurveTable): usage: \n size, time1, value1, curvature1, [ timeN-1, valueN-1, curvatureN-1, ] timeN, valueN)\n" );
        goto done;
    }
    if ((nargs / 3) + 1 > MAX_CURVE_PTS) {
        CK_FPRINTF_STDERR( "[chuck](via CurveTable): too many arguments.\n" );
        goto done;
    }
    
    for(ii = 0; ii<nargs; ii++) {
        weights->get(ii, &v);
        // CK_FPRINTF_STDOUT( "weight %d = %f...\n", ii, v );
        coeffs[ii] = v;
    }
    
    if (coeffs[0] != 0.0) {
        CK_FPRINTF_STDERR( "[chuck](via CurveTable): first time must be zero.\n" );
        goto done;
    }
    
    for (i = points = 0; i < nargs; points++) {
        time[points] = (t_CKDOUBLE) coeffs[i++];
        if (points > 0 && time[points] < time[points - 1])
            goto time_err;
        value[points] = (t_CKDOUBLE) coeffs[i++];
        if (i < nargs)
            alpha[points] = (t_CKDOUBLE) coeffs[i++];
    }

    factor = (t_CKDOUBLE) (len - 1) / time[points - 1];
    for (i = 0; i < points; i++)
        time[i] *= factor;

    ptr = d->genX_table;
    for (i = 0; i < points - 1; i++) {
        seglen = (t_CKINT) (floor(time[i + 1] + 0.5) - floor(time[i] + 0.5)) + 1;
        _transition(value[i], alpha[i], value[i + 1], seglen, ptr);
        ptr += seglen - 1;
    }

done:
    // return
    RETURN->v_object = weights;

    return;
    
time_err:
    CK_FPRINTF_STDERR( "[chuck](via CurveTable): times must be in ascending order.\n" );

    // return
    RETURN->v_object = weights;

    return;
}


static void _transition(t_CKDOUBLE a, t_CKDOUBLE alpha, t_CKDOUBLE b, t_CKINT n, t_CKDOUBLE *output)
{
    t_CKINT  i;
    t_CKDOUBLE delta, interval = 0.0;

    delta = b - a;

    if (n <= 1) {
        //warn("maketable (curve)", "Trying to transition over 1 array slot; "
        //                                "time between points is too short");
        *output = a;
        return;
    }
    interval = 1.0 / (n - 1.0);

    if (alpha != 0.0) {
        t_CKDOUBLE denom = 1.0 / (1.0 - exp(alpha));
        for (i = 0; i < n; i++)
            *output++ = (a + delta
                                * (1.0 - exp((double) i * alpha * interval)) * denom);
    }
    else
        for (i = 0; i < n; i++)
            *output++ = a + delta * i * interval;
}


//-----------------------------------------------------------------------------
// name: warp_coeffs()
// desc: setup table for warp
//-----------------------------------------------------------------------------
CK_DLL_CTRL( warp_coeffs )
{
    // get data
    genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data);
    t_CKINT i = 0;

    t_CKFLOAT k_asym = 1.; 
    t_CKFLOAT k_sym  = 1.; 

    // gewang:
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_CK_OBJECT(ARGS);
    // check for size
    if( weights->size() != 2 )
    {
        // error
        CK_FPRINTF_STDERR( "[chuck](via WarpTable): expects array of exactly 2 elements.\n" );
        goto done;
    }

    weights->get( 0, &k_asym ); // (t_CKDOUBLE) GET_NEXT_FLOAT(ARGS);
    weights->get( 1, &k_sym ); // (t_CKDOUBLE) GET_NEXT_FLOAT(ARGS);

    for (i = 0; i < genX_tableSize; i++) {
        t_CKDOUBLE inval = (t_CKDOUBLE) i/(genX_tableSize - 1);
        if(k_asym == 1 && k_sym == 1) {
            d->genX_table[i]    = inval;
        } else if(k_sym == 1) {
            d->genX_table[i]    = _asymwarp(inval, k_asym);
        } else if(k_asym == 1) {
            d->genX_table[i]    = _symwarp(inval, k_sym);
        } else {
            inval               = _asymwarp(inval, k_asym);
            d->genX_table[i]    = _symwarp(inval, k_sym);
        }
        // CK_FPRINTF_STDOUT( "table %d = %f\n", i, d->genX_table[i] );
    }

done:

    // return
    RETURN->v_object = weights;
}


t_CKDOUBLE _asymwarp(t_CKDOUBLE inval, t_CKDOUBLE k)
{
    return (pow(k, inval) - 1.) / (k - 1.);
}


t_CKDOUBLE _symwarp(t_CKDOUBLE inval, t_CKDOUBLE k)
{
    t_CKDOUBLE sym_warped;
    if(inval >= 0.5) {
        sym_warped = pow(2.*inval - 1., 1./k);
        return (sym_warped + 1.) * 0.5;

    }
    inval = 1. - inval; // for S curve
    sym_warped = pow(2.*inval - 1., 1./k);
    sym_warped = (sym_warped + 1.) * 0.5;

    return 1. - sym_warped;
}


// also do RTcmix "spline" on a rainy day


//-----------------------------------------------------------------------------
// name: genX_pmsg()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_PMSG( genX_pmsg )
{
    //genX_Data * d = (genX_Data *)OBJ_MEMBER_UINT(SELF, genX_offset_data );
    if( !strcmp( MSG, "print" ) )
    {
        // CK_FPRINTF_STDOUT( "genX:" );
        return TRUE;
    }
    
    // didn't handle
    return FALSE;
}


//-----------------------------------------------------------------------------
// name: genX_coeffs()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( genX_coeffs )
{
    // nope
    CK_FPRINTF_STDERR( "[chuck](via GenX): .coeffs called on abstract base class!\n" );

    // return
    RETURN->v_object = GET_NEXT_OBJECT(ARGS);

    return;
}
