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
// file: ugen_filter.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ugen_filter.h"
#include <math.h>
#include <stdlib.h>


static t_CKUINT g_srate = 0;


//-----------------------------------------------------------------------------
// name: filter_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY filter_query( Chuck_DL_Query * QUERY )
{
    // set srate
    g_srate = QUERY->srate;
    
    // add filter
    QUERY->ugen_add( QUERY, "filter", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, filter_ctor, filter_dtor, filter_tick, filter_pmsg );
    // ctrl func
    // QUERY->ugen_ctrl( QUERY, filter_ctrl_top, "float", "value" );
    // QUERY->ugen_ctrl( QUERY, filter_ctrl_bottom, "float", "value" );

    // add biquad
    QUERY->ugen_add( QUERY, "biquad", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, biquad_ctor, biquad_dtor, biquad_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_pfreq, NULL, "float", "pfreq" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_prad, NULL, "float", "prad" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_zfreq, NULL, "float", "zfreq" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_zrad, NULL, "float", "zrad" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_norm, NULL, "int", "norm" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_pregain, NULL, "float", "pregain" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_eqzs, NULL, "int", "eqzs" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_b0, NULL, "float", "b0" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_b1, NULL, "float", "b1" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_b2, NULL, "float", "b2" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_a0, NULL, "float", "a0" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_a1, NULL, "float", "a1" );
    QUERY->ugen_ctrl( QUERY, biquad_ctrl_a2, NULL, "float", "a2" );
    
    // add onepole
    QUERY->ugen_add( QUERY, "onepole", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, onepole_ctor, onepole_dtor, onepole_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, onepole_ctrl_pole, NULL, "float", "pole" );
    
    // add onezero
    QUERY->ugen_add( QUERY, "onezero", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, onezero_ctor, onezero_dtor, onezero_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, onezero_ctrl_zero, NULL,"float", "zero" );

    // add twopole
    QUERY->ugen_add( QUERY, "twopole", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, twopole_ctor, twopole_dtor, twopole_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, twopole_ctrl_freq, NULL, "float", "freq" );
    QUERY->ugen_ctrl( QUERY, twopole_ctrl_rad, NULL, "float", "rad" );
    QUERY->ugen_ctrl( QUERY, twopole_ctrl_norm, NULL, "int", "norm" );

    // add twozero
    QUERY->ugen_add( QUERY, "twozero", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, twozero_ctor, twozero_dtor, twozero_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, twozero_ctrl_freq, NULL, "float", "freq" );
    QUERY->ugen_ctrl( QUERY, twozero_ctrl_rad, NULL, "float", "rad" );

    // add gQ
    // QUERY->ugen_add( QUERY, "gQ", NULL );
    // set funcs
    // QUERY->ugen_func( QUERY, gQ_ctor, gQ_dtor, gQ_tick, NULL );
    // ctrl func
    // QUERY->ugen_ctrl( QUERY, gQ_ctrl_freq, NULL, "float", "freq" );
    // QUERY->ugen_ctrl( QUERY, gQ_ctrl_rad, NULL, "float", "rad" );
    // QUERY->ugen_ctrl( QUERY, gQ_ctrl_norm, NULL, "int", "norm" );

    // add allpass
    // QUERY->ugen_add( QUERY, "allpass", NULL );
    // set funcs
    // QUERY->ugen_func( QUERY, allpass_ctor, allpass_dtor, allpass_tick, allpass_pmsg );

    // add delay
    QUERY->ugen_add( QUERY, "delay", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, delay_ctor, delay_dtor, delay_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, delay_ctrl_delay, NULL, "float", "delay" );
    QUERY->ugen_ctrl( QUERY, delay_ctrl_max, NULL, "float", "max" );
    // QUERY->ugen_ctrl( QUERY, delay_ctrl_energy, NULL,"int", "energy" );
    // QUERY->ugen_ctrl( QUERY, delay_ctrl_tap, NULL, "int", "tap" );
    // QUERY->ugen_ctrl( QUERY, delay_ctrl_ftap, NULL, "float", "ftap" );

    // add delayA
    QUERY->ugen_add( QUERY, "delayA", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, delayA_ctor, delayA_dtor, delayA_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, delayA_ctrl_delay, NULL,  "float", "delay" );
    QUERY->ugen_ctrl( QUERY, delayA_ctrl_max, NULL, "float", "max" );

    // add one
    QUERY->ugen_add( QUERY, "delayL", NULL );
    // set funcs
    QUERY->ugen_func( QUERY, delayL_ctor, delayL_dtor, delayL_tick, NULL );
    // ctrl func
    QUERY->ugen_ctrl( QUERY, delayL_ctrl_delay, NULL, "float", "delay" );
    QUERY->ugen_ctrl( QUERY, delayL_ctrl_max, NULL, "float", "max" );


    return TRUE;
}




//-----------------------------------------------------------------------------
// name: filter
// desc: ...
//-----------------------------------------------------------------------------
struct filter_data
{
    t_CKUINT nB;
    t_CKUINT nA;
    SAMPLE * b;
    SAMPLE * a;
    SAMPLE * input;
    SAMPLE * output;
    
    filter_data()
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
    
    SAMPLE tick( SAMPLE in )
    {
        int i;

        output[0] = 0.0;
        input[0] = in;
        for( i = nB-1; i > 0; i-- )
        {
            output[0] += b[i] * input[i];
            input[i] = input[i-1];
        }
        output[0] += b[0] * input[0];

        for (i=nA-1; i>0; i--)
        {
            output[0] += -a[i] * output[i];
            output[i] = output[i-1];
        }

        return output[0];
    }
};

UGEN_CTOR filter_ctor( t_CKTIME now )
{
    return new filter_data;
}

UGEN_DTOR filter_dtor( t_CKTIME now, void * data )
{
    delete (filter_data *)data;
}

UGEN_TICK filter_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    filter_data * d = (filter_data *)data;
    *out = d->tick( in );
    return TRUE;
}

UGEN_CTRL filter_ctrl_coefs( t_CKTIME now, void * data, void * value )
{
}

UGEN_PMSG filter_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
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

    float pfreq, zfreq;
    float prad, zrad;
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

UGEN_CTOR biquad_ctor( t_CKTIME now )
{
    return new biquad_data;
}

UGEN_DTOR biquad_dtor( t_CKTIME now, void * data )
{
    delete (biquad_data *)data;
}

UGEN_TICK biquad_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    biquad_data * d = (biquad_data *)data;
    
    d->m_input0 = d->m_a0 * in;
    d->m_output0 = d->m_b0 * d->m_input0 + d->m_b1 * d->m_input1 + d->m_b2 * d->m_input2;
    d->m_output0 -= d->m_a2 * d->m_output2 + d->m_a1 * d->m_output1;
    d->m_input2 = d->m_input1;
    d->m_input1 = d->m_input0;
    d->m_output2 = d->m_output1;
    d->m_output1 = d->m_output0;

    *out = d->m_output0;

    return TRUE;
}

void biquad_set_reson( biquad_data * d )
{
    d->m_a2 = d->prad * d->prad;
    d->m_a1 = -2.0f * d->prad * (float)cos(2.0 * 3.141592653590 * (double)d->pfreq / (double)d->srate);

    if ( d->norm ) {
        // Use zeros at +- 1 and normalize the filter peak gain.
        d->m_b0= 0.5f - 0.5f * d->m_a2;
        d->m_b1 = -1.0f;
        d->m_b2 = -d->m_b0;
    }    
}

UGEN_CTRL biquad_ctrl_pfreq( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->pfreq = (float)GET_CK_FLOAT(value);
    biquad_set_reson( d ); 
}

UGEN_CTRL biquad_ctrl_prad( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->prad = (float)GET_CK_FLOAT(value);
    biquad_set_reson( d );
}

void biquad_set_notch( biquad_data * d )
{
    d->m_b2 = d->zrad * d->zrad;
    d->m_b1 = -2.0f * d->zrad * (float)cos(2.0 * 3.141592653590 * (double)d->zfreq / (double)d->srate);
}

UGEN_CTRL biquad_ctrl_zfreq( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->zfreq = (float)GET_CK_FLOAT(value);
    biquad_set_notch( d );
}

UGEN_CTRL biquad_ctrl_zrad( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->zrad = (float)GET_CK_FLOAT(value);
    biquad_set_notch( d );
}

UGEN_CTRL biquad_ctrl_norm( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->norm = *(t_CKBOOL *)value;
    biquad_set_reson( d );
}

UGEN_CTRL biquad_ctrl_pregain( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_a0 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_eqzs( t_CKTIME now, void * data, void * value )
{
    if( *(t_CKUINT *)value )
    {
        biquad_data * d = (biquad_data *)data;
        d->m_b0 = 1.0f;
        d->m_b1 = 0.0f;
        d->m_b2 = -1.0f;
    }
}

UGEN_CTRL biquad_ctrl_b0( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_b0 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_b1( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_b1 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_b2( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_b2 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_a0( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_a0 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_a1( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_a1 = (float)GET_CK_FLOAT(value);
}

UGEN_CTRL biquad_ctrl_a2( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->m_a2 = (float)GET_CK_FLOAT(value);
}




//-----------------------------------------------------------------------------
// name: onepole
// desc: onepole filter
//-----------------------------------------------------------------------------
UGEN_CTOR onepole_ctor( t_CKTIME now )
{
    return new biquad_data;
}

UGEN_DTOR onepole_dtor( t_CKTIME now, void * data )
{
    delete (biquad_data *)data;
}

UGEN_TICK onepole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    biquad_data * d = (biquad_data *)data;

    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 - d->m_a1 * d->m_output1;
    d->m_output1 = d->m_output0;

    *out = d->m_output0;

    return TRUE;
}

UGEN_CTRL onepole_ctrl_pole( t_CKTIME now, void * data, void * value )
{
    float f = (float)GET_CK_FLOAT(value);
    biquad_data * d = (biquad_data *)data;
    
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
UGEN_CTOR onezero_ctor( t_CKTIME now )
{
    return new biquad_data;
}

UGEN_DTOR onezero_dtor( t_CKTIME now, void * data )
{
    delete (biquad_data *)data;
}

UGEN_TICK onezero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    biquad_data * d = (biquad_data *)data;
    
    d->m_input0 = in;
    d->m_output0 = d->m_b1 * d->m_input1 + d->m_b0 * d->m_input0;
    d->m_input1 = d->m_input0;

    *out = d->m_output0;

    return TRUE;
}

UGEN_CTRL onezero_ctrl_zero( t_CKTIME now, void * data, void * value )
{
    float f = (float)GET_CK_FLOAT(value);
    biquad_data * d = (biquad_data *)data;
    
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
UGEN_CTOR twopole_ctor( t_CKTIME now )
{
    return new biquad_data;
}

UGEN_DTOR twopole_dtor( t_CKTIME now, void * data )
{
    delete (biquad_data *)data;
}

UGEN_TICK twopole_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    biquad_data * d = (biquad_data *)data;
    
    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 - d->m_a2 * d->m_output2 - d->m_a1 * d->m_output1;
    d->m_output2 = d->m_output1;
    d->m_output1 = d->m_output0;

    *out = d->m_output0;

    return TRUE;
}

UGEN_CTRL twopole_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->pfreq = (float)GET_CK_FLOAT(value);
    biquad_set_reson( d );
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terribly efficient.
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}

UGEN_CTRL twopole_ctrl_rad( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->prad = (float)GET_CK_FLOAT(value);
    biquad_set_reson( d );
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terrbly efficient
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}

UGEN_CTRL twopole_ctrl_norm( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->norm = *(t_CKBOOL *)value;
    
    if( d->norm )
    {
        // Normalize the filter gain ... not terribly efficient
        double real = 1.0 - d->prad + (d->m_a2 - d->prad) * cos( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        double imag = (d->m_a2 - d->prad) * sin( 2.0 * 3.14159265358979323846 * d->pfreq / d->srate );
        d->m_b0 = sqrt( real*real + imag*imag );
    }
}




//-----------------------------------------------------------------------------
// name: twozero
// desc: twozero filter
//-----------------------------------------------------------------------------
UGEN_CTOR twozero_ctor( t_CKTIME now )
{
    return new biquad_data;
}

UGEN_DTOR twozero_dtor( t_CKTIME now, void * data )
{
    delete (biquad_data *)data;
}

UGEN_TICK twozero_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    biquad_data * d = (biquad_data *)data;
    
    d->m_input0 = in;
    d->m_output0 = d->m_b0 * d->m_input0 + d->m_b1 * d->m_input1 + d->m_b2 * d->m_input2;
    d->m_input2 = d->m_input1;
    d->m_input1 = d->m_input0;

    *out = d->m_output0;
    
    return TRUE;
}

UGEN_CTRL twozero_ctrl_freq( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->zfreq = (float)GET_CK_FLOAT(value);
    biquad_set_notch( d );
    
    // normalize the filter gain
    if( d->m_b1 > 0.0f )
        d->m_b0 = 1.0f / (1.0f+d->m_b1+d->m_b2);
    else
        d->m_b0 = 1.0f / (1.0f-d->m_b1+d->m_b2);
    d->m_b1 *= d->m_b0;
    d->m_b2 *= d->m_b0;
}

UGEN_CTRL twozero_ctrl_rad( t_CKTIME now, void * data, void * value )
{
    biquad_data * d = (biquad_data *)data;
    d->zrad = (float)GET_CK_FLOAT(value);
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
UGEN_CTOR gQ_ctor( t_CKTIME now )
{
    return NULL;
}

UGEN_DTOR gQ_dtor( t_CKTIME now, void * data )
{
}

UGEN_TICK gQ_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    return TRUE;
}

UGEN_CTRL gQ_ctrl_freq( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL gQ_ctrl_rad( t_CKTIME now, void * data, void * value )
{
}




//-----------------------------------------------------------------------------
// name: allpass
// desc: allpass filter
//-----------------------------------------------------------------------------
UGEN_CTOR allpass_ctor( t_CKTIME now )
{
    return NULL;
}

UGEN_DTOR allpass_dtor( t_CKTIME now, void * data )
{
}

UGEN_TICK allpass_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    return TRUE;
}

UGEN_PMSG allpass_pmsg( t_CKTIME now, void * data, const char * msg, void * value )
{
    return TRUE;
}




//-----------------------------------------------------------------------------
// name: delay
// desc: ...
//-----------------------------------------------------------------------------
UGEN_CTOR delay_ctor( t_CKTIME now )
{
    return NULL;
}

UGEN_DTOR delay_dtor( t_CKTIME now, void * data )
{
}

UGEN_TICK delay_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    return TRUE;
}

UGEN_CTRL delay_ctrl_delay( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL delay_ctrl_max( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL delay_ctrl_tap( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL delay_ctrl_energy( t_CKTIME now, void * data, void * value )
{
}




//-----------------------------------------------------------------------------
// name: delayA
// desc: delay - allpass interpolation
//-----------------------------------------------------------------------------
UGEN_CTOR delayA_ctor( t_CKTIME now )
{
    return NULL;
}

UGEN_DTOR delayA_dtor( t_CKTIME now, void * data )
{
}

UGEN_TICK delayA_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    return TRUE;
}

UGEN_CTRL delayA_ctrl_delay( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL delayA_ctrl_max( t_CKTIME now, void * data, void * value )
{
}




//-----------------------------------------------------------------------------
// name: delayL
// desc: delay - linear interpolation
//-----------------------------------------------------------------------------
UGEN_CTOR delayL_ctor( t_CKTIME now )
{
    return NULL;
}

UGEN_DTOR delayL_dtor( t_CKTIME now, void * data )
{
}

UGEN_TICK delayL_tick( t_CKTIME now, void * data, SAMPLE in, SAMPLE * out )
{
    return TRUE;
}

UGEN_CTRL delayL_ctrl_delay( t_CKTIME now, void * data, void * value )
{
}

UGEN_CTRL delayL_ctrl_max( t_CKTIME now, void * data, void * value )
{
}
