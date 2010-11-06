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
// file: ugen_osc.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Summer 2004
//-----------------------------------------------------------------------------
#include "ugen_osc.h"
#include <math.h>
#include <stdio.h>


#define TWO_PI (2.0 * 3.14159265358979323846)
static t_CKUINT g_srate = 0;

//-----------------------------------------------------------------------------
// name: osc_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY osc_query( Chuck_DL_Query * QUERY )
{
    // srate
    g_srate = QUERY->srate;

    //! \sectionMain oscillators

    //! phasor - simple ramp generator ( 0 to 1 )
    //! this can be fed into other oscillators ( with sync mode of 2 ) 
    //! as a phase control.  see \example sixty.ck for an example
    QUERY->ugen_add( QUERY, "phasor", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, phasor_ctor, osc_dtor, osc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase, osc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to global ( 1 ) or self ( 0 ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_width, osc_cget_width, "float", "width" ); //! set duration of the ramp in each cycle. ( default 1.0) 

    //! sine oscillator
    //! (see \example osc.ck)
    QUERY->ugen_add( QUERY, "sinosc", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, osc_ctor, osc_dtor, sinosc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, sinosc_ctrl_phase, sinosc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to self(0), global(1), input(2)  

    //! pulse oscillators
    //! a pulse wave oscillator with variable width.
    QUERY->ugen_add( QUERY, "pulseosc", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, osc_ctor, osc_dtor, pulseosc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase, osc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to self(0), global(1), input(2)  
    QUERY->ugen_ctrl( QUERY, osc_ctrl_width, osc_cget_width, "float", "width" ); //! length of duty cycle ( 0-1 ) 

    //! square wave oscillator ( pulse with fixed width of 0.5 )
    QUERY->ugen_add( QUERY, "sqrosc", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, osc_ctor, osc_dtor, sqrosc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase, osc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to self(0), global(1), input(2)  
 
    //! triangle wave oscillator
    QUERY->ugen_add( QUERY, "triosc", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, osc_ctor, osc_dtor, triosc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase, osc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to self(0), global(1), input(2)  
    QUERY->ugen_ctrl( QUERY, osc_ctrl_width, osc_cget_width, "float", "width" ); //! control midpoint of triangle ( 0 to 1 ) 
	
    //! sawtooth wave oscillator ( triangle, width forced to 0.0 or 1.0 )
    QUERY->ugen_add( QUERY, "sawosc", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, osc_ctor, osc_dtor, sawosc_tick, osc_pmsg );
    // add ctrls / cgets
    QUERY->ugen_ctrl( QUERY, osc_ctrl_freq, osc_cget_freq, "float", "freq" );  //! oscillator frequency ( Hz ) 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sfreq, osc_cget_freq, "float", "sfreq" );  //! oscillator frequency ( Hz ) , phase-matched
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase_offset, osc_cget_phase_offset, "float", "phase_offset" ); //! phase offset 
    QUERY->ugen_ctrl( QUERY, osc_ctrl_phase, osc_cget_phase, "float", "phase" ); //! current phase
    QUERY->ugen_ctrl( QUERY, osc_ctrl_sync, osc_cget_sync, "int", "sync" ); //! sync to self(0), global(1), input(2)  
    QUERY->ugen_ctrl( QUERY, osc_ctrl_width, osc_cget_width, "float", "width" ); //! increasing ( w > 0.5 ) or decreasing ( w < 0.5 )

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: struct Osc_Data
// desc: ...
//-----------------------------------------------------------------------------
struct Osc_Data
{
    double t;
    double num;
    double freq;
    double phase_offset;
    int    sync; 
    t_CKUINT srate;
    double width;
    
    double phase;
    
    Osc_Data( )
    {
        t = 0.0;
        num = 0.0;
        freq = 220.0;
        sync = 0; //internal 
        width = 0.5;
        phase_offset = 0.0;
        srate = g_srate;
        osc_ctrl_freq( 0, this, &freq );
        osc_ctrl_phase_offset( 0, this, &phase_offset );
        
        phase = 0.0;
    }
};


//-----------------------------------------------------------------------------
// name: sinosc_ctor()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CTOR osc_ctor( t_CKTIME now )
{
    // return data to be used later
    return new Osc_Data;
}

//-----------------------------------------------------------------------------
// name: sinosc_ctor()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CTOR phasor_ctor( t_CKTIME now )
{
    // return data to be used later
    Osc_Data * d = new Osc_Data;
    d->width = 1.0;
    return d;
}




//-----------------------------------------------------------------------------
// name: osc_dtor()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_DTOR osc_dtor( t_CKTIME now, void * data )
{
    // delete
    delete (Osc_Data *)data;
}




//-----------------------------------------------------------------------------
// name: osc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK osc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Osc_Data * d = (Osc_Data *)data;
    //phase offsets don't mean so much when oscillators are keeping 
    //track of their own ticks, unless they are created at the same time..

    if ( d->sync == 1 )  d->t = (double) now;
    float ph = ( d->sync == 2 ) ? in : d->phase_offset + d->t * d->num;
    ph -= floor ( ph );
    *out = (SAMPLE) ( d->width != 0.0 && ph <= d->width ) ? ph / d->width : 0.0 ;
 
    if ( !d->sync ) d->t += 1.0;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: sinosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK sinosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    // get this
    Osc_Data * d = (Osc_Data *)data;
    // sync to now
    if( d->sync == 1 ) d->phase = now * d->num;
    // compute
    *out = (SAMPLE) ( d->sync == 2 ) ? sin( TWO_PI * in ) : sin( TWO_PI * d->phase );
    // move phase
    if( d->sync == 0 ) d->phase += d->num;
    // unwrap
    if( d->phase > TWO_PI ) fmod( d->phase, TWO_PI );

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: sinosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK triosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Osc_Data * d = (Osc_Data *)data;
    //phase offsets don't mean so much when oscillators are keeping 
    //track of their own ticks, unless they are created at the same time..

    if ( d->sync == 1 )  d->t = (double) now;

    float ph = ( d->sync == 2 ) ? in : d->phase_offset + d->t * d->num;
    ph -= floor ( ph );
    float width = d->width;
    if ( ph < width ) *out = ( width == 0.0 ) ? 1.0 :  -1.0 + 2.0 * ph / width;
    else *out = ( width == 1.0 ) ? 0 : 1.0 - 2.0 * (ph - width) / (1.0 - width);
	
    if ( !d->sync ) d->t += 1.0;

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: pulseosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK pulseosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Osc_Data * d = (Osc_Data *)data;
    //phase offsets don't mean so much when oscillators are keeping 
    //track of their own ticks, unless they are created at the same time..

    if ( d->sync == 1 )  d->t = (double) now;

    float ph = ( d->sync == 2 ) ? in : d->phase_offset + d->t * d->num;
    ph -= floor ( ph );
    if ( ph < d->width ) *out = -1.0;
    else *out = 1.0;
	
    if ( !d->sync ) d->t += 1.0;

    return TRUE;
}

//-----------------------------------------------------------------------------
// name: sqrosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK sqrosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Osc_Data * d = (Osc_Data *)data;
    d->width = 0.5;
    return pulseosc_tick ( now, data, in , out );
}

//-----------------------------------------------------------------------------
// name: sawosc_tick()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_TICK sawosc_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    Osc_Data * d = (Osc_Data *)data;
    d->width = ( d->width < 0.5 ) ? 0.0 : 1.0;
    return triosc_tick ( now, data, in , out );
}

//-----------------------------------------------------------------------------
// name: osc_ctrl_freq()
// desc: set oscillator frequency
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    d->freq = GET_CK_FLOAT(value);
    d->num = d->freq / d->srate;
    d->phase = 0.0;
}


//-----------------------------------------------------------------------------
// name: sinosc_ctrl_sfreq()
// desc: sets oscillator frequency, implicitly adjusts phase offset to match
//       avoid signal discontinuity. - pld
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_sfreq( t_CKTIME now, void * data, void * value )
{
    //phase matching freq adjust
    Osc_Data * d = (Osc_Data *)data;
    double curnum = d->num;
    d->freq = (float)GET_CK_FLOAT(value);
    d->num = d->freq / d->srate;
    double nphase = d->phase_offset + d->t * ( curnum - d->num );
    d->phase_offset = nphase - floor ( nphase );
}

//-----------------------------------------------------------------------------
// name: sinosc_cget_freq()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CGET osc_cget_freq( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_FLOAT( out, (t_CKFLOAT)d->freq );
}



//-----------------------------------------------------------------------------
// name: sinosc_ctrl_phase_offset()
// desc: explicitly set oscillator phase-offset   -pld
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_phase_offset( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    d->phase_offset = (float)GET_CK_FLOAT(value);
}

//-----------------------------------------------------------------------------
// name: sinosc_cget_phase_offset()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CGET osc_cget_phase_offset( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_FLOAT( out, (t_CKFLOAT)d->phase_offset );
}



//-----------------------------------------------------------------------------
// name: osc_ctrl_phase()
// desc: explicitly set oscillator phase   -pld
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_phase ( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    t_CKFLOAT phase = GET_CK_FLOAT(value);
    double cphase =  phase - ( d->t * d->num );
    d->phase_offset = cphase - floor ( cphase );
}

//-----------------------------------------------------------------------------
// name: osc_cget_phase()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CGET osc_cget_phase ( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_FLOAT( out, (t_CKFLOAT) d->phase_offset + d->t * d->num );
}


//-----------------------------------------------------------------------------
// name: osc_ctrl_phase()
// desc: explicitly set oscillator phase   -pld
//-----------------------------------------------------------------------------
UGEN_CTRL sinosc_ctrl_phase ( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    d->phase = GET_CK_FLOAT(value) / TWO_PI;
}

//-----------------------------------------------------------------------------
// name: osc_cget_phase()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CGET sinosc_cget_phase ( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_FLOAT( out, (t_CKFLOAT) TWO_PI * d->phase );
}

//-----------------------------------------------------------------------------
// name: sinosc_ctrl_sync()
// desc: set sync   -pld
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_sync ( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    d->sync = GET_CK_INT(value);
}


//-----------------------------------------------------------------------------
// name: sinosc_cget_sync()
// desc: get sync
//-----------------------------------------------------------------------------
UGEN_CGET osc_cget_sync ( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_INT( out, d->sync );
}

//-----------------------------------------------------------------------------
// name: sinosc_cget_width()
// desc: get sync
//-----------------------------------------------------------------------------
UGEN_CGET osc_cget_width ( t_CKTIME now, void * data, void * out )
{
    Osc_Data * d = (Osc_Data *)data;
    SET_NEXT_FLOAT( out, d->width );
}


//-----------------------------------------------------------------------------
// name: sinosc_ctrl_width()
// desc: set sync   -pld
//-----------------------------------------------------------------------------
UGEN_CTRL osc_ctrl_width ( t_CKTIME now, void * data, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    d->width = GET_CK_FLOAT(value);
    d->width -= floor ( d->width );
}



//-----------------------------------------------------------------------------
// name: sinosc_pmsg()
// desc: ...
//-----------------------------------------------------------------------------
UGEN_PMSG osc_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    Osc_Data * d = (Osc_Data *)data;
    if( !strcmp( msg, "print" ) )
    {
        fprintf( stdout, "sinosc: (freq=%f)", d->freq );
        return TRUE;
    }
    
    // didn't handle
    return FALSE;
}
