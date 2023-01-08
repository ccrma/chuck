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
// file: uana_extract.cpp
// desc: unit analyzer extract
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Rebecca Fiebrink (fiebrink@cs.princeton.edu)
// date: Summer 2007
//-----------------------------------------------------------------------------
#include "uana_extract.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_instr.h"
#include "chuck_lang.h"
#include "chuck_errmsg.h"
#include "util_math.h"
#include "util_xforms.h"




// Centroid
CK_DLL_TICK( Centroid_tick );
CK_DLL_TOCK( Centroid_tock );
CK_DLL_PMSG( Centroid_pmsg );
CK_DLL_SFUN( Centroid_compute );

// Flux
CK_DLL_CTOR( Flux_ctor );
CK_DLL_DTOR( Flux_dtor );
CK_DLL_TICK( Flux_tick );
CK_DLL_TOCK( Flux_tock );
CK_DLL_PMSG( Flux_pmsg );
CK_DLL_MFUN( Flux_ctrl_reset );
CK_DLL_SFUN( Flux_compute );
CK_DLL_SFUN( Flux_compute2 );
// offset
static t_CKUINT Flux_offset_data = 0;

// RMS
CK_DLL_TICK( RMS_tick );
CK_DLL_TOCK( RMS_tock );
CK_DLL_PMSG( RMS_pmsg );
CK_DLL_SFUN( RMS_compute );

// MFCC | 1.4.2.0 (yikai) added
CK_DLL_CTOR( MFCC_ctor );
CK_DLL_DTOR( MFCC_dtor );
CK_DLL_TICK( MFCC_tick );
CK_DLL_TOCK( MFCC_tock );
CK_DLL_PMSG( MFCC_pmsg );
CK_DLL_MFUN( MFCC_compute );
CK_DLL_MFUN( MFCC_ctrl_sample_rate );
CK_DLL_MFUN( MFCC_cget_sample_rate );
CK_DLL_MFUN( MFCC_ctrl_num_filters );
CK_DLL_MFUN( MFCC_cget_num_filters );
CK_DLL_MFUN( MFCC_ctrl_num_coeffs );
CK_DLL_MFUN( MFCC_cget_num_coeffs );
// offset
static t_CKUINT MFCC_offset_data = 0;

// RollOff
CK_DLL_CTOR( RollOff_ctor );
CK_DLL_DTOR( RollOff_dtor );
CK_DLL_TICK( RollOff_tick );
CK_DLL_TOCK( RollOff_tock );
CK_DLL_PMSG( RollOff_pmsg );
CK_DLL_SFUN( RollOff_compute );
CK_DLL_MFUN( RollOff_ctrl_percent );
CK_DLL_MFUN( RollOff_cget_percent );
// offset
static t_CKUINT RollOff_offset_percent = 0;

// Feature Collector
CK_DLL_TICK( FeatureCollector_tick);
CK_DLL_TOCK( FeatureCollector_tock);
CK_DLL_PMSG( FeatureCollector_pmsg);

// AutoCorr
CK_DLL_CTOR( AutoCorr_ctor );
CK_DLL_DTOR( AutoCorr_dtor );
CK_DLL_TICK( AutoCorr_tick );
CK_DLL_TOCK( AutoCorr_tock );
CK_DLL_PMSG( AutoCorr_pmsg );
CK_DLL_SFUN( AutoCorr_compute );
CK_DLL_MFUN( AutoCorr_ctrl_normalize );
CK_DLL_MFUN( AutoCorr_cget_normalize );
// offset
static t_CKUINT AutoCorr_offset_data = 0;

// XCorr
CK_DLL_CTOR( XCorr_ctor );
CK_DLL_DTOR( XCorr_dtor );
CK_DLL_TICK( XCorr_tick );
CK_DLL_TOCK( XCorr_tock );
CK_DLL_PMSG( XCorr_pmsg );
CK_DLL_SFUN( XCorr_compute );
CK_DLL_MFUN( XCorr_ctrl_normalize );
CK_DLL_MFUN( XCorr_cget_normalize );
// offset
static t_CKUINT XCorr_offset_data = 0;

// ZeroX
CK_DLL_CTOR( ZeroX_ctor );
CK_DLL_DTOR( ZeroX_dtor );
CK_DLL_TICK( ZeroX_tick );
CK_DLL_TOCK( ZeroX_tock );
CK_DLL_PMSG( ZeroX_pmsg );
CK_DLL_SFUN( ZeroX_compute );
// offset
static t_CKUINT ZeroX_offset_data = 0;

// LPC
CK_DLL_CTOR( LPC_ctor );
CK_DLL_DTOR( LPC_dtor );
CK_DLL_TICK( LPC_tick );
CK_DLL_TOCK( LPC_tock );
CK_DLL_PMSG( LPC_pmsg );
CK_DLL_SFUN( LPC_compute );
CK_DLL_MFUN( LPC_ctrl_pitch );
CK_DLL_MFUN( LPC_cget_pitch );
CK_DLL_MFUN( LPC_ctrl_power );
CK_DLL_MFUN( LPC_cget_power );
CK_DLL_MFUN( LPC_ctrl_coefs );
CK_DLL_MFUN( LPC_cget_coefs );
// offset
//static t_CKUINT LPC_offset_data = 0;


// utility functions
void xcorr_fft( SAMPLE * f, t_CKINT fs, SAMPLE * g, t_CKINT gs, SAMPLE * buffer, t_CKINT bs );
void xcorr_normalize( SAMPLE * buffy, t_CKINT bs, SAMPLE * f, t_CKINT fs, SAMPLE * g, t_CKINT gs );


// 1.4.2.0 (ge) | local global sample rate variable (e.g., for MFCC)
static t_CKUINT g_srate = 0;



//-----------------------------------------------------------------------------
// name: extract_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY extract_query( Chuck_DL_Query * QUERY )
{
    Chuck_Env * env = QUERY->env();
    Chuck_DL_Func * func = NULL;

    // 1.4.2.0 (ge) | store sample rate
    g_srate = QUERY->srate;

    std::string doc;

    //---------------------------------------------------------------------
    // init as base class: FeatureCollector
    //---------------------------------------------------------------------

    doc = "Turns UAna input into a single feature vector, upon .upchuck()";

    if( !type_engine_import_uana_begin( env, "FeatureCollector", "UAna", env->global(),
                                        NULL, NULL,
                                        FeatureCollector_tick, FeatureCollector_tock, FeatureCollector_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: Centroid
    //---------------------------------------------------------------------

    doc = "A unit analyzer that computes the spectral centroid from a magnitude spectrum (either from incoming UAna or manually given), and outputs a single number.";

    if( !type_engine_import_uana_begin( env, "Centroid", "UAna", env->global(),
                                        NULL, NULL,
                                        Centroid_tick, Centroid_tock, Centroid_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // compute
    func = make_new_sfun( "float", "compute", Centroid_compute );
    func->add_arg( "float[]", "input" );
    func->doc = "Manually computes the centroid from a float array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: Flux
    //---------------------------------------------------------------------

    doc = "A unit analyzer that computes the spectral flux between successive magnitude spectra (via incoming UAna, or given manually), and outputs a single number.";

    if( !type_engine_import_uana_begin( env, "Flux", "UAna", env->global(),
                                        Flux_ctor, Flux_dtor,
                                        Flux_tick, Flux_tock, Flux_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // data offset
    Flux_offset_data = type_engine_import_mvar( env, "int", "@Flux_data", FALSE );
    if( Flux_offset_data == CK_INVALID_OFFSET ) goto error;

    // compute
    func = make_new_mfun( "void", "reset", Flux_ctrl_reset );
    func->doc = "Reset the extractor.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute
    func = make_new_sfun( "float", "compute", Flux_compute );
    func->add_arg( "float[]", "lhs" );
    func->add_arg( "float[]", "rhs" );
    func->doc = "Manually computes the flux between two frames.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // compute2
    func = make_new_sfun( "float", "compute", Flux_compute2 );
    func->add_arg( "float[]", "lhs" );
    func->add_arg( "float[]", "rhs" );
    func->add_arg( "float[]", "diff" );
    func->doc = "Manually computes the flux between two frames, and stores the difference in a third array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: RMS
    //---------------------------------------------------------------------

    doc = "A unit analyzer that computes the RMS power mean from a magnitude spectrum (either from an incoming UAna, or given manually), and outputs a single number.";

    if( !type_engine_import_uana_begin( env, "RMS", "UAna", env->global(),
                                        NULL, NULL,
                                        RMS_tick, RMS_tock, RMS_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // compute
    func = make_new_sfun( "float", "compute", RMS_compute );
    func->add_arg( "float[]", "input" );
    func->doc = "Manually computes the RMS from a float array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: MFCC
    //---------------------------------------------------------------------

    doc = "A unit analyzer that computes Mel-frequency Cepstral Coefficients (MFCCs), and outputs a vector of coefficients.";

    // 1.4.2.0 (yikai) | added
    if( !type_engine_import_uana_begin( env, "MFCC", "UAna", env->global(),
                                        MFCC_ctor, MFCC_dtor,
                                        MFCC_tick, MFCC_tock, MFCC_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // data offset
    MFCC_offset_data = type_engine_import_mvar( env, "float", "@MFCC_data", FALSE );
    if( MFCC_offset_data == CK_INVALID_OFFSET ) goto error;

    // sample rate
    func = make_new_mfun( "int", "sampleRate", MFCC_ctrl_sample_rate );
    func->add_arg( "int", "sr" );
    func->doc = "Set the sample rate for MFCC analysis; NOTE: by default this is set to current ChucK sample rate.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // sample rate
    func = make_new_mfun( "int", "sampleRate", MFCC_cget_sample_rate );
    func->doc = "Get the sample rate for MFCC analysis.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // num filters
    func = make_new_mfun( "int", "numFilters", MFCC_ctrl_num_filters );
    func->doc = "Set the number of linearly spaced filters in MEL space.";
    func->add_arg( "int", "n" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // num filters
    func = make_new_mfun( "int", "numFilters", MFCC_cget_num_filters );
    func->doc = "Get the number of linearly spaced filters in MEL space.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // num coeffs
    func = make_new_mfun( "int", "numCoeffs", MFCC_ctrl_num_coeffs );
    func->add_arg( "int", "n" );
    func->doc = "Set the number of MFCC coefficients to compute.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // num coeffs
    func = make_new_mfun( "int", "numCoeffs", MFCC_cget_num_coeffs );
    func->doc = "Get the number of MFCC coefficients to compute.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute (manual alternative to calling upchuck())
    func = make_new_mfun( "float", "compute", MFCC_compute );
    func->add_arg( "float[]", "input" );
    func->add_arg( "float[]", "output" );
    func->doc = "Manually computes the MFCC of the input (an FFT spectrum), and stores the results in the output array (MFCC coefficients).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: RollOff
    //---------------------------------------------------------------------

    doc = "A unit analyzer that computes the spectral rolloff from a magnitude spectrum (either from incoming UAna, or given manually), and outputs a single number.";

    if( !type_engine_import_uana_begin( env, "RollOff", "UAna", env->global(),
                                        RollOff_ctor, RollOff_dtor,
                                        RollOff_tick, RollOff_tock, RollOff_pmsg,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // data offset
    RollOff_offset_percent = type_engine_import_mvar( env, "float", "@RollOff_data", FALSE );
    if( RollOff_offset_percent == CK_INVALID_OFFSET ) goto error;

    // compute
    func = make_new_mfun( "float", "percent", RollOff_ctrl_percent );
    func->add_arg( "float", "percent" );
    func->doc = "Set the percentage for computing rolloff.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute
    func = make_new_mfun( "float", "percent", RollOff_cget_percent );
    func->doc = "Get the percentage specified for the rolloff.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute
    func = make_new_sfun( "float", "compute", RollOff_compute );
    func->add_arg( "float[]", "input" );
    func->add_arg( "float", "percent" );
    func->doc = "Manually computes the rolloff from a float array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: AutoCorr
    //---------------------------------------------------------------------
    if( !type_engine_import_uana_begin( env, "AutoCorr", "UAna", env->global(),
                                        AutoCorr_ctor, AutoCorr_dtor,
                                        AutoCorr_tick, AutoCorr_tock, AutoCorr_pmsg ) )
        return FALSE;

    // data offset
    AutoCorr_offset_data = type_engine_import_mvar( env, "float", "@AutoCorr_data", FALSE );
    if( AutoCorr_offset_data == CK_INVALID_OFFSET ) goto error;

    // normalize
    func = make_new_mfun( "int", "normalize", AutoCorr_ctrl_normalize );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // normalize
    func = make_new_mfun( "int", "normalize", AutoCorr_cget_normalize );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute
    func = make_new_sfun( "float[]", "compute", AutoCorr_compute );
    func->add_arg( "float[]", "input" );
    func->add_arg( "int", "normalize" );
    func->add_arg( "float[]", "output" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: XCorr
    //---------------------------------------------------------------------
    if( !type_engine_import_uana_begin( env, "XCorr", "UAna", env->global(),
                                        XCorr_ctor, XCorr_dtor,
                                        XCorr_tick, XCorr_tock, XCorr_pmsg ) )
        return FALSE;

    // data offset
    XCorr_offset_data = type_engine_import_mvar( env, "float", "@XCorr_data", FALSE );
    if( XCorr_offset_data == CK_INVALID_OFFSET ) goto error;

    // normalize
    func = make_new_mfun( "int", "normalize", XCorr_ctrl_normalize );
    func->add_arg( "int", "flag" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // normalize
    func = make_new_mfun( "int", "normalize", XCorr_cget_normalize );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // compute
    func = make_new_sfun( "float[]", "compute", XCorr_compute );
    func->add_arg( "float[]", "f" );
    func->add_arg( "float[]", "g" );
    func->add_arg( "int", "normalize" );
    func->add_arg( "float[]", "y" );
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: zerox
    //---------------------------------------------------------------------

    doc = "Zero crossing detector.";

    if( !type_engine_import_uana_begin( env, "ZeroX", "UAna", env->global(),
                                        ZeroX_ctor, ZeroX_dtor,
                                        ZeroX_tick, ZeroX_tock, NULL,
                                        CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE, CK_NO_VALUE,
                                        doc.c_str()) )
        return FALSE;

    // data offset
    ZeroX_offset_data = type_engine_import_mvar( env, "int", "@ZeroX_data", FALSE );
    if( ZeroX_offset_data == CK_INVALID_OFFSET ) goto error;

    // compute
    func = make_new_sfun( "float", "compute", ZeroX_compute );
    func->add_arg( "float[]", "input" );
    func->doc = "Manually computes the zero crossing rate for an array.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end import
    if( !type_engine_import_class_end( env ) )
        return FALSE;


    return TRUE;

    error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}

CK_DLL_TICK( FeatureCollector_tick )
{
    // do nothing
    return TRUE;
}

// FeatureCollector_tock creates a flat vector from its upstream UAnae
// TODO: Stick complex features in Blob, too? But what about fft? (don't want duplication)
CK_DLL_TOCK( FeatureCollector_tock )
{
    //t_CKFLOAT * features;
    t_CKINT num_feats = 0;
    t_CKINT num_incoming = UANA->numIncomingUAnae();
    t_CKINT i, j;


    // Get all incoming features and agglomerate into one vector
    if( num_incoming > 0 )
    {
        //count the number of features in the array we're making
        for( i = 0; i < num_incoming; i++ )
        {
            // get next blob
            Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( i );
            // sanity check
            assert( BLOB_IN != NULL );
            // count number of features from this UAna
            Chuck_Array8 & these_fvals = BLOB_IN->fvals();
            num_feats += these_fvals.size();
        }

        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        if( fvals.size() != num_feats )
            fvals.set_size( num_feats );

        t_CKINT next_index = 0;
        for( i = 0; i < num_incoming; i++ )
        {
            // get next blob
            Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( i );
            Chuck_Array8 & these_fvals = BLOB_IN->fvals();
            t_CKINT num_these = these_fvals.size();
            for( j = 0; j < num_these; j++ )
            {
                t_CKFLOAT v;
                these_fvals.get( j, &v );
                fvals.set( next_index, v);
                next_index++;
            }
        }
    } else {
        // no input to collect
        BLOB->fvals().set_size(0);
    }

    return TRUE;
}

CK_DLL_PMSG( FeatureCollector_pmsg )
{
    // do nothing
    return TRUE;
}

static t_CKFLOAT compute_centroid( Chuck_Array8 & buffer, t_CKUINT size )
{
    t_CKFLOAT m0 = 0.0;
    t_CKFLOAT m1 = 0.0;
    t_CKFLOAT centroid;
    t_CKFLOAT v;
    t_CKUINT i;

    // Compute centroid using moments
    for( i = 0; i < size; i++ )
    {
        buffer.get( i, &v );
        m1 += (i * v);
        m0 += v;
    }

    if( m0 != 0.0 )
        centroid = m1 / m0;
    else
        centroid = size / 2.0; // Perfectly balanced

    return centroid / size;
}


CK_DLL_TICK( Centroid_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( Centroid_tock )
{
    t_CKFLOAT result = 0.0;

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // compute centroid
        result = compute_centroid( mag, mag.size() );
    }
        // otherwise zero out
    else
    {
        // no input!
        result = 0.0;
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != 1 )
        fvals.set_size( 1 );
    // copy the result in
    fvals.set( 0, result );

    return TRUE;
}

CK_DLL_PMSG( Centroid_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_SFUN( Centroid_compute )
{
    // get array
    Chuck_Array8 * array = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // sanity check
    if( !array )
    {
        // no centroid
        RETURN->v_float = 0.0;
    }
    else
    {
        // do it
        RETURN->v_float = compute_centroid( *array, array->size() );
    }
}


// Flux state
struct StateOfFlux
{
    Chuck_Array8 prev;
    Chuck_Array8 norm;
    t_CKBOOL initialized;

    StateOfFlux()
    {
        initialized = FALSE;
    }
};


// compute norm rms
static void compute_norm_rms( Chuck_Array8 & curr, Chuck_Array8 & norm )
{
    t_CKUINT i;
    t_CKFLOAT energy = 0.0;
    t_CKFLOAT v;

    // check size
    if( norm.size() != curr.size() )
        norm.set_size( curr.size() );

    // get energy
    for( i = 0; i < curr.size(); i++ )
    {
        curr.get( i, &v );
        energy += v * v;
    }

    // check energy
    if (energy == 0.0)
    {
        // all zeros
        norm.zero( 0, norm.size() );
        return;
    }
    else
        energy = ::sqrt( energy );

    for( i = 0; i < curr.size(); i++ )
    {
        curr.get( i, & v );
        if( v > 0.0)
            norm.set( i, v / energy );
        else
            norm.set( i, 0.0 );
    }
}

// compute flux
static t_CKFLOAT compute_flux( Chuck_Array8 & curr, Chuck_Array8 & prev, Chuck_Array8 * write )
{
    // sanity check
    assert( curr.size() == prev.size() );

    // ensure size
    if( write != NULL && (write->size() != curr.size()) )
        write->set_size( curr.size() );

    // find difference
    t_CKFLOAT v, w, result = 0.0;
    for( t_CKUINT i = 0; i < curr.size(); i++ )
    {
        curr.get( i, &v );
        prev.get( i, &w );
        // accumulate into flux
        result += (v - w)*(v - w);
        // copy to write
        if( write != NULL ) write->set( i, v );
    }

    // take sqrt of flux
    return ::sqrt( result );
}

// compute flux
static t_CKFLOAT compute_flux( Chuck_Array8 & curr, StateOfFlux & sof )
{
    // flux
    t_CKFLOAT result = 0.0;
    t_CKFLOAT v;

    // verify size
    if( curr.size() != sof.prev.size() )
    {
        sof.initialized = FALSE;
        // resize prev
        sof.prev.set_size( curr.size() );
    }

    // check initialized
    if( sof.initialized )
    {
        // compute normalize rms
        compute_norm_rms( curr, sof.norm );
        // do it
        result = compute_flux( sof.norm, sof.prev, &sof.prev );
    }

    // copy curr to prev
    for( t_CKUINT i = 0; i < curr.size(); i++ )
    {
        // get the value
        sof.norm.get( i, &v );
        // set it
        sof.prev.set( i, v );
    }

    // initialize
    sof.initialized = TRUE;

    // return result
    return result;
}

CK_DLL_CTOR( Flux_ctor )
{
    // allocate the flux state
    StateOfFlux * state = new StateOfFlux();
    OBJ_MEMBER_UINT( SELF, Flux_offset_data ) = (t_CKUINT)state;
}


CK_DLL_DTOR( Flux_dtor )
{
    // clean up
    StateOfFlux * state = (StateOfFlux *)OBJ_MEMBER_UINT( SELF, Flux_offset_data );
    SAFE_DELETE( state );
    OBJ_MEMBER_UINT( SELF, Flux_offset_data ) = 0;
}


CK_DLL_TICK( Flux_tick )
{
    // do nothing
    return TRUE;
}


CK_DLL_TOCK( Flux_tock )
{
    t_CKFLOAT result = 0.0;

    // get state
    StateOfFlux * state = (StateOfFlux *)OBJ_MEMBER_UINT( SELF, Flux_offset_data );

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // compute flux
        result = compute_flux( mag, *state );
    }
        // otherwise zero out
    else
    {
        // no input!
        result = 0.0;
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != 1 )
        fvals.set_size( 1 );
    // copy the result in
    fvals.set( 0, result );

    return TRUE;
}

CK_DLL_PMSG( Flux_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_MFUN( Flux_ctrl_reset )
{
    // get state
    StateOfFlux * state = (StateOfFlux *)OBJ_MEMBER_UINT( SELF, Flux_offset_data );
    // set
    state->initialized = FALSE;
}

CK_DLL_SFUN( Flux_compute )
{
    // get inputs
    Chuck_Array8 * lhs = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array8 * rhs = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);

    // verify
    if( !lhs || !rhs )
    {
        // 0
        RETURN->v_float = 0.0;
    }
    else
    {
        // verify size
        if( lhs->size() != rhs->size() )
        {
            // message
            EM_error3( "(via Flux): compute() expects two arrays of equal size" );
            // 0
            RETURN->v_float = 0.0;
        }
        else
        {
            // flux
            RETURN->v_float = compute_flux( *lhs, *rhs, NULL );
        }
    }
}

CK_DLL_SFUN( Flux_compute2 )
{
    // get inputs
    Chuck_Array8 * lhs = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array8 * rhs = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array8 * diff = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);

    // verify
    if( !lhs || !rhs )
    {
        // 0
        RETURN->v_float = 0.0;
    }
    else
    {
        // verify size
        if( lhs->size() != rhs->size() )
        {
            // message
            EM_error3( "(via Flux): compute() expects two arrays of equal size" );
            // 0
            RETURN->v_float = 0.0;
        }
        else
        {
            // flux
            RETURN->v_float = compute_flux( *lhs, *rhs, diff );
        }
    }
}


static t_CKFLOAT compute_rms( Chuck_Array8 & buffer, t_CKUINT size )
{
    t_CKFLOAT rms = 0.0;
    t_CKFLOAT v;
    t_CKUINT i;

    // get sum of squares
    for( i = 0; i < size; i++ )
    {
        buffer.get( i, &v );
        rms += (v * v);
    }

    // go
    rms /= size;
    rms = ::sqrt(rms);

    return rms;
}


CK_DLL_TICK( RMS_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( RMS_tock )
{
    t_CKFLOAT result = 0.0;

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // compute RMS
        result = compute_rms( mag, mag.size() );
    }
        // otherwise zero out
    else
    {
        // no input!
        result = 0.0;
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != 1 )
        fvals.set_size( 1 );
    // copy the result in
    fvals.set( 0, result );

    return TRUE;
}

CK_DLL_PMSG( RMS_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_SFUN( RMS_compute )
{
    // get array
    Chuck_Array8 * array = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // sanity check
    if( !array )
    {
        // no RMS
        RETURN->v_float = 0.0;
    }
    else
    {
        // do it
        RETURN->v_float = compute_rms( *array, array->size() );
    }
}




// 1.4.2.0 (yikai) | MFCC implementation
// Yikai Li, Fall 2022
struct MFCC_Object
{
    t_CKINT size;
    t_CKINT sample_rate;
    t_CKINT num_filters;
    t_CKINT num_coeffs;
    t_CKINT curr_sample_rate;
    t_CKINT curr_num_filters;
    t_CKINT curr_num_coeffs;

    t_CKFLOAT * filterbank;
    t_CKINT * filterpoints;
    t_CKFLOAT * filterfreqs;
    t_CKFLOAT * dct;

    t_CKFLOAT * spectrum;
    t_CKFLOAT * filtered;
    t_CKFLOAT * result;

    // static mfcc instance
    static MFCC_Object * ourMFCC;

    // constructor
    MFCC_Object()
    {
        size = 1024;
        sample_rate = g_srate;
        num_filters = 10;
        num_coeffs = 40;
        curr_sample_rate = 0.0;
        curr_num_filters = 0;
        curr_num_coeffs = 0;
        filterbank = NULL;
        filterpoints = NULL;
        filterfreqs = NULL;
        dct = NULL;
        spectrum = NULL;
        filtered = NULL;
        result = NULL;
        this->prepare(size);
    }

    // destructor
    ~MFCC_Object()
    {
        // done
        this->reset();
    }

    // reset
    void reset()
    {
        // delete
        SAFE_DELETE_ARRAY( filterbank );
        SAFE_DELETE_ARRAY( filterpoints );
        SAFE_DELETE_ARRAY( filterfreqs );
        SAFE_DELETE_ARRAY( dct );
        SAFE_DELETE_ARRAY( spectrum );
        SAFE_DELETE_ARRAY( filtered );
        SAFE_DELETE_ARRAY( result );
        // zero out
        size = 0;
        sample_rate = 0.0;
        num_filters = 0;
        num_coeffs = 0;
        curr_sample_rate = 0.0;
        curr_num_filters = 0;
        curr_num_coeffs = 0;
    }

    // clear
    void clear()
    {
        if (spectrum) memset(spectrum, 0, sizeof(t_CKFLOAT) * size);
        if (filtered) memset(filtered, 0, sizeof(t_CKFLOAT) * num_filters);
        if (result) memset(result, 0, sizeof(t_CKFLOAT) * num_coeffs);
        if (filterbank) memset(filterbank, 0, sizeof(t_CKFLOAT) * num_filters * size);
        if (filterpoints) memset(filterpoints, 0, sizeof(t_CKINT) * num_filters);
        if (filterfreqs) memset(filterfreqs, 0, sizeof(t_CKFLOAT) * num_filters);
        if (dct) memset(dct, 0, sizeof(t_CKFLOAT) * num_filters * num_coeffs);
    }

    static t_CKFLOAT freq2mel( t_CKFLOAT freq )
    {
        return 1127.01048 * ::log( 1.0 + freq / 700.0 );
    }

    static t_CKFLOAT mel2freq( t_CKFLOAT mel )
    {
        return 700.0 * ( ::exp( mel / 1127.01048 ) - 1.0 );
    }

    // prepare filterband and dct
    void prepare(t_CKINT size)
    {
        if ( this->size == size
             && this->sample_rate == this->curr_sample_rate
             && this->num_filters == this->curr_num_filters
             && this->num_coeffs == this->curr_num_coeffs )
            return;

        this->size = size;
        this->curr_sample_rate = this->sample_rate;
        this->curr_num_filters = this->num_filters;
        this->curr_num_coeffs = this->num_coeffs;

        // filter bank
        SAFE_DELETE_ARRAY( this->filterbank );
        this->filterbank = new t_CKFLOAT[num_filters * size];
        memset(this->filterbank, 0, sizeof(t_CKFLOAT) * num_filters * size);

        SAFE_DELETE_ARRAY(this->filterpoints);
        this->filterpoints = new t_CKINT[num_filters + 2];
        SAFE_DELETE_ARRAY(this->filterfreqs);
        this->filterfreqs = new t_CKFLOAT[num_filters + 2];

        t_CKFLOAT filterwidth = this->freq2mel(this->sample_rate / 2.0) / (this->num_filters + 1.0);
        for (int i=0; i<this->num_filters + 2; i++)
        {
            this->filterfreqs[i] = this->mel2freq(i * filterwidth);
            this->filterpoints[i] = (t_CKINT) (this->filterfreqs[i] / this->sample_rate * this->size);
        }

        t_CKFLOAT energy;
        for (int i=0; i<this->num_filters; i++)
        {
            energy = 2.0 / (this->filterfreqs[i+2] - this->filterfreqs[i]);
            for (t_CKINT j=this->filterpoints[i]; j<this->filterpoints[i+1]; j++)
            {
                this->filterbank[i*size + j] = energy * (j - this->filterpoints[i]) / (this->filterpoints[i+1] - this->filterpoints[i]);
            }
            for (t_CKINT j=this->filterpoints[i+1]; j<this->filterpoints[i+2]; j++)
            {
                this->filterbank[i*size + j] = energy * (this->filterpoints[i+2] - j) / (this->filterpoints[i+2] - this->filterpoints[i+1]);
            }
        }

        // dct
        SAFE_DELETE_ARRAY(this->dct);
        this->dct = new t_CKFLOAT[num_coeffs * num_filters];
        for (int i=0; i<this->num_coeffs; i++)
        {
            if (i == 0)
            {
                for (int j=0; j<this->num_filters; j++)
                {
                    this->dct[i*num_filters + j] = 1.0 / sqrt(this->num_filters);
                }
            }
            else
            {
                for (int j=0; j<this->num_filters; j++)
                {
                    this->dct[i*num_filters + j] = sqrt(2.0 / this->num_filters) * cos(ONE_PI * i * (j + 0.5) / this->num_filters);
                }
            }
        }

        // spectrum
        SAFE_DELETE_ARRAY(this->spectrum);
        this->spectrum = new t_CKFLOAT[size];

        // filtered
        SAFE_DELETE_ARRAY(this->filtered);
        this->filtered = new t_CKFLOAT[num_filters];

        // result
        SAFE_DELETE_ARRAY(this->result);
        this->result = new t_CKFLOAT[num_coeffs];
    }

    // compute
    void compute()
    {
        // compute spectrum
        for (int i=0; i<this->size; i++)
        {
            this->spectrum[i] = this->spectrum[i] * this->spectrum[i];
        }

        // compute filterbank
        for (int i=0; i<this->num_filters; i++)
        {
            this->filtered[i] = 0.0;
            for (int j=0; j<this->size; j++)
            {
                this->filtered[i] += this->spectrum[j] * this->filterbank[i*this->size + j];
            }
        }

        // compute dct
        for (int i=0; i<this->num_coeffs; i++)
        {
            this->result[i] = 0.0;
            for (int j=0; j<this->num_filters; j++)
            {
                this->result[i] += this->filtered[j] * this->dct[i*this->num_filters + j];
            }
        }
    }

    // get our singleton
//    static MFCC_Object * getOurObject()
//    {
//        // instantiate, if needed
//        if (ourMFCC == NULL)
//        {
//            ourMFCC = new MFCC_Object();
//            assert(ourMFCC != NULL);
//        }
//
//        // return new instance
//        return ourMFCC;
//    }
};

// static initialization
MFCC_Object * MFCC_Object::ourMFCC = NULL;

// compute mfcc
static void compute_mfcc( MFCC_Object * mfcc, Chuck_Array8 & f, t_CKUINT fs, Chuck_Array8 & buffy )
{
    t_CKUINT i;
    t_CKFLOAT v;
    t_CKINT size;

    mfcc->prepare(fs);

    // copy into buffers
    for( i = 0; i < fs; i++ )
    {
        f.get( i, &v );
        mfcc->spectrum[i] = v;
    }

    // compute
    mfcc->compute();

    // copy into result
    size = mfcc->num_coeffs;
    buffy.set_size( size );
    for( i = 0; i < size; i++ )
    {
        buffy.set( i, mfcc->result[i] );
    }
}

// MFCC
CK_DLL_CTOR( MFCC_ctor )
{
    MFCC_Object * mfcc = new MFCC_Object();
    OBJ_MEMBER_UINT(SELF, MFCC_offset_data) = (t_CKUINT)mfcc;
}

CK_DLL_DTOR( MFCC_dtor )
{
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT(SELF, MFCC_offset_data);
    SAFE_DELETE( mfcc );
    OBJ_MEMBER_UINT(SELF, MFCC_offset_data) = 0;
}


CK_DLL_TICK( MFCC_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( MFCC_tock )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT(SELF, MFCC_offset_data);

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // compute MFCC
        compute_mfcc(mfcc, mag, mag.size(), fvals);
    }
        // otherwise zero out
    else
    {
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // resize
        fvals.set_size( 0 );
    }

    return TRUE;
}

CK_DLL_PMSG( MFCC_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_CTRL( MFCC_ctrl_sample_rate )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // get sample_rate
    mfcc->sample_rate = GET_NEXT_INT(ARGS);
    // return it
    RETURN->v_float = mfcc->sample_rate;
}

CK_DLL_CGET( MFCC_cget_sample_rate )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // return it
    RETURN->v_float = mfcc->sample_rate;
}

CK_DLL_CTRL( MFCC_ctrl_num_filters )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // get num_filters
    mfcc->num_filters = GET_NEXT_INT(ARGS);
    // return it
    RETURN->v_int = mfcc->num_filters;
}

CK_DLL_CGET( MFCC_cget_num_filters )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // return it
    RETURN->v_int = mfcc->num_filters;
}

CK_DLL_CTRL( MFCC_ctrl_num_coeffs )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // get num_coeffs
    mfcc->num_coeffs = GET_NEXT_INT(ARGS);
    // return it
    RETURN->v_int = mfcc->num_coeffs;
}

CK_DLL_CGET( MFCC_cget_num_coeffs )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // return it
    RETURN->v_int = mfcc->num_coeffs;
}

CK_DLL_MFUN( MFCC_compute )
{
    // get object
    MFCC_Object * mfcc = (MFCC_Object *)OBJ_MEMBER_UINT( SELF, MFCC_offset_data );
    // get input
    Chuck_Array8 * input = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // get output
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // compute mfcc
    compute_mfcc( mfcc, *input, input->size(), *output );
}

static t_CKFLOAT compute_rolloff( Chuck_Array8 & buffer, t_CKUINT size, t_CKFLOAT percent )
{
    t_CKFLOAT sum = 0.0, v, target;
    t_CKINT i;

    // sanity check
    assert( percent >= 0 && percent <= 1 );

    // iterate
    for( i = 0; i < size; i++ )
    {
        buffer.get( i, &v );
        sum += v;
    }

    // the target
    target = sum * percent;
    sum = 0.0;

    // iterate
    for( i = 0; i < size; i++ )
    {
        buffer.get( i, &v );
        sum += v;
        if( sum >= target ) break;
    }

    return i/(t_CKFLOAT)size;
}


CK_DLL_CTOR( RollOff_ctor )
{
    OBJ_MEMBER_FLOAT( SELF, RollOff_offset_percent ) = .85;
}

CK_DLL_DTOR( RollOff_dtor )
{
}

CK_DLL_TICK( RollOff_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( RollOff_tock )
{
    t_CKFLOAT result = 0.0;

    // get percent
    t_CKFLOAT percent = OBJ_MEMBER_FLOAT( SELF, RollOff_offset_percent );

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // compute rolloff
        result = compute_rolloff( mag, mag.size(), percent );
    }
        // otherwise zero out
    else
    {
        // no input!
        result = 0.0;
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != 1 )
        fvals.set_size( 1 );
    // copy the result in
    fvals.set( 0, result );

    return TRUE;
}

CK_DLL_PMSG( RollOff_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_CTRL( RollOff_ctrl_percent )
{
    // get percent
    t_CKFLOAT percent = GET_NEXT_FLOAT(ARGS);
    // check it
    if( percent < 0.0 ) percent = 0.0;
    else if( percent > 1.0 ) percent = 1.0;
    // set it
    OBJ_MEMBER_FLOAT(SELF, RollOff_offset_percent) = percent;
    // return it
    RETURN->v_float = percent;
}

CK_DLL_CGET( RollOff_cget_percent )
{
    // return it
    RETURN->v_float = OBJ_MEMBER_FLOAT(SELF, RollOff_offset_percent);
}

CK_DLL_SFUN( RollOff_compute )
{
    // get array
    Chuck_Array8 * array = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // get percent
    t_CKFLOAT percent = GET_NEXT_FLOAT(ARGS);

    // sanity check
    if( !array )
    {
        // no RollOff
        RETURN->v_float = 0.0;
    }
    else
    {
        // do it
        RETURN->v_float = compute_rolloff( *array, array->size(), percent );
    }
}




// struct
struct Corr_Object
{
    // whether to normalize
    t_CKBOOL normalize;
    // f
    SAMPLE * fbuf;
    t_CKINT fcap;
    // g
    SAMPLE * gbuf;
    t_CKINT gcap;
    // result
    SAMPLE * buffy;
    t_CKINT bufcap;

    // static corr instance
    static Corr_Object * ourCorr;

    // constructor
    Corr_Object()
    {
        // default: do normalize
        normalize = FALSE;
        // zero out pointers
        fbuf = gbuf = buffy = NULL;
        fcap = gcap = bufcap = 0;
        // TODO: default
        resize( 512, 512 );
    }

    // destructor
    ~Corr_Object()
    {
        // done
        this->reset();
    }

    // reset
    void reset()
    {
        SAFE_DELETE_ARRAY( fbuf );
        SAFE_DELETE_ARRAY( gbuf );
        SAFE_DELETE_ARRAY( buffy );
        fcap = gcap = bufcap = 0;
    }

    // clear
    void clear()
    {
        if( fbuf ) memset( fbuf, 0, fcap * sizeof(SAMPLE) );
        if( gbuf ) memset( gbuf, 0, gcap * sizeof(SAMPLE) );
        if( buffy ) memset( buffy, 0, bufcap * sizeof(SAMPLE) );
    }

    // resize inputs
    t_CKBOOL resize( t_CKINT fs, t_CKINT gs )
    {
        // minimum size
        t_CKINT mincap = fs + gs - 1;
        // ensure power of two
        mincap = ensurepow2( mincap );
        // log
        EM_log( CK_LOG_FINE, "Corr resizing to %d-element buffers...", mincap );

        // verify
        if( fcap < mincap )
        {
            SAFE_DELETE_ARRAY( fbuf );
            fbuf = new SAMPLE[mincap];
            fcap = mincap;
        }
        if( gcap < mincap )
        {
            SAFE_DELETE_ARRAY( gbuf );
            gbuf = new SAMPLE[mincap];
            gcap = mincap;
        }
        if( bufcap < mincap )
        {
            SAFE_DELETE_ARRAY( buffy );
            buffy = new SAMPLE[mincap];
            bufcap = mincap;
        }

        // hopefully
        if( fbuf == NULL || gbuf == NULL || buffy == NULL )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: Corr failed to allocate %ld-element buffer(s)...",
                               mincap );
            // clean up
            this->reset();
            // done
            return FALSE;
        }

        // clear it
        this->clear();

        return TRUE;
    }

    // get our singleton
    static Corr_Object * getOurObject()
    {
        // instantiate, if needed
        if( ourCorr == NULL )
        {
            ourCorr = new Corr_Object();
            assert( ourCorr != NULL );
        }

        // return new instance
        return ourCorr;
    }
};

// static initialization
Corr_Object * Corr_Object::ourCorr = NULL;

// compute correlation
static void compute_corr( Corr_Object * corr, Chuck_Array8 & f, t_CKINT fs,
                          Chuck_Array8 & g, t_CKINT gs, Chuck_Array8 & buffy )
{
    t_CKINT i;
    t_CKFLOAT v;
    t_CKINT size;

    // ensure size
    corr->resize( fs, gs );

    // copy into buffers
    for( i = 0; i < fs; i++ )
    {
        f.get( i, &v );
        corr->fbuf[i] = v;
    }
    for( i = 0; i < gs; i++ )
    {
        g.get( i, &v );
        corr->gbuf[i] = v;
    }

    // compute
    xcorr_fft( corr->fbuf, corr->fcap, corr->gbuf, corr->gcap,
               corr->buffy, corr->bufcap );

    // check flags
    if( corr->normalize )
    {
        // normalize
        xcorr_normalize( corr->buffy, corr->bufcap,
                         corr->fbuf, corr->fcap, corr->gbuf, corr->gcap );
    }

    // copy into result
    size = fs + gs - 1;
    buffy.set_size( size );
    for( i = 0; i < size; i++ )
    {
        buffy.set( i, corr->buffy[i] );
    }
}

// AutoCorr
CK_DLL_CTOR( AutoCorr_ctor )
{
    Corr_Object * ac = new Corr_Object();
    OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data ) = (t_CKUINT)ac;
}

CK_DLL_DTOR( AutoCorr_dtor )
{
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data );
    SAFE_DELETE(ac);
    OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data ) = 0;
}

CK_DLL_TICK( AutoCorr_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( AutoCorr_tock )
{
    // get object
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data );

    // TODO: get buffer from stream, and set
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // compute autocorr
        compute_corr( ac, mag, mag.size(), mag, mag.size(), fvals );
    }
    // otherwise zero out
    else
    {
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // resize
        fvals.set_size( 0 );
    }

    return TRUE;
}

CK_DLL_PMSG( AutoCorr_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_CTRL( AutoCorr_ctrl_normalize )
{
    // get object
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data );
    // get percent
    ac->normalize = GET_NEXT_INT(ARGS) != 0;
    // return it
    RETURN->v_int = ac->normalize;
}

CK_DLL_CGET( AutoCorr_cget_normalize )
{
    // get object
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, AutoCorr_offset_data );
    // return it
    RETURN->v_int = ac->normalize;
}

CK_DLL_SFUN( AutoCorr_compute )
{
    // get input
    Chuck_Array8 * input = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // get normalize flag
    t_CKINT normalize = GET_NEXT_INT(ARGS) != 0;
    // get input
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);

    // set normalize
    Corr_Object::getOurObject()->normalize = normalize;
    // compute autocrr
    compute_corr( Corr_Object::getOurObject(), *input, input->size(),
                  *input, input->size(), *output );
}


// XCorr
CK_DLL_CTOR( XCorr_ctor )
{
    Corr_Object * xc = new Corr_Object();
    OBJ_MEMBER_UINT( SELF, XCorr_offset_data ) = (t_CKUINT)xc;
}

CK_DLL_DTOR( XCorr_dtor )
{
    Corr_Object * xc = (Corr_Object *)OBJ_MEMBER_UINT( SELF, XCorr_offset_data );
    SAFE_DELETE(xc);
    OBJ_MEMBER_UINT( SELF, XCorr_offset_data ) = 0;
}

CK_DLL_TICK( XCorr_tick )
{
    // do nothing
    return TRUE;
}

CK_DLL_TOCK( XCorr_tock )
{
    // get object
    Corr_Object * xc = (Corr_Object *)OBJ_MEMBER_UINT( SELF, XCorr_offset_data );

    // TODO: get buffer from stream, and set
    if( UANA->numIncomingUAnae() > 1 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_F = UANA->getIncomingBlob( 0 );
        // get second
        Chuck_UAnaBlobProxy * BLOB_G = UANA->getIncomingBlob( 1 );
        // sanity check
        assert( BLOB_F != NULL && BLOB_G != NULL );
        // get the array
        Chuck_Array8 & mag_f = BLOB_F->fvals();
        Chuck_Array8 & mag_g = BLOB_G->fvals();
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // compute xcorr
        compute_corr( xc, mag_f, mag_f.size(), mag_g, mag_g.size(), fvals );
    }
        // otherwise zero out
    else
    {
        // get fvals of output BLOB
        Chuck_Array8 & fvals = BLOB->fvals();
        // resize
        fvals.set_size( 0 );
    }

    return TRUE;
}

CK_DLL_PMSG( XCorr_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_CTRL( XCorr_ctrl_normalize )
{
    // get object
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, XCorr_offset_data );
    // get percent
    ac->normalize = GET_NEXT_INT(ARGS) != 0;
    // return it
    RETURN->v_int = ac->normalize;
}

CK_DLL_CGET( XCorr_cget_normalize )
{
    // get object
    Corr_Object * ac = (Corr_Object *)OBJ_MEMBER_UINT( SELF, XCorr_offset_data );
    // return it
    RETURN->v_int = ac->normalize;
}

CK_DLL_SFUN( XCorr_compute )
{
    // get input
    Chuck_Array8 * f = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array8 * g = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // get normalize flag
    t_CKINT normalize = GET_NEXT_INT(ARGS) != 0;
    // get output
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);

    // set normalize
    Corr_Object::getOurObject()->normalize = normalize;
    // compute autocrr
    compute_corr( Corr_Object::getOurObject(), *f, f->size(),
                  *g, g->size(), *output );
}




//-----------------------------------------------------------------------------
// name: xcorr_fft()
// desc: FFT-based cross correlation
//-----------------------------------------------------------------------------
void xcorr_fft( SAMPLE * f, t_CKINT fsize, SAMPLE * g, t_CKINT gsize, SAMPLE * buffy, t_CKINT size )
{
    // sanity check
    assert( fsize == gsize && gsize == size );

    // take fft
    rfft( f, size/2, FFT_FORWARD );
    rfft( g, size/2, FFT_FORWARD );

    // complex
    t_CKCOMPLEX_SAMPLE * F = (t_CKCOMPLEX_SAMPLE *)f;
    t_CKCOMPLEX_SAMPLE * G = (t_CKCOMPLEX_SAMPLE *)g;
    t_CKCOMPLEX_SAMPLE * Y = (t_CKCOMPLEX_SAMPLE *)buffy;

    // loop
    for( t_CKINT i = 0; i < size/2; i++ )
    {
        // conjugate F
        F[i].im = -F[i].im;
        // complex multiply
        Y[i].re = F[i].re*G[i].re - F[i].im*G[i].im;
        Y[i].im = F[i].im*G[i].re + F[i].re*G[i].im;
    }

    // inverse fft
    rfft( buffy, size/2, FFT_INVERSE );
}




//-----------------------------------------------------------------------------
// name: xcorr_normalize()
// desc: ...
//-----------------------------------------------------------------------------
void xcorr_normalize( SAMPLE * buffy, t_CKINT size, SAMPLE * f, t_CKINT fs, SAMPLE * g, t_CKINT gs )
{
    float sum = 0.000001f;

    // f^2(t)
    for( long i = 0; i < fs; i++ )
        sum += f[i]*f[i];
    // g^2(t)
    for( long j = 0; j < gs; j++ )
        sum += g[j]*g[j];
    // normalize: taking coherence into account
    for( long k = 0; k < size; k++ )
        buffy[k] /= sum;
}




// ZeroX
#define __SGN(x)  (x >= 0.0f ? 1.0f : -1.0f )
static t_CKINT compute_zerox( Chuck_Array8 & buffer, t_CKUINT size )
{
    t_CKUINT i, xings = 0;
    t_CKFLOAT v = 0, p = 0;
    buffer.get( 0, &p );

    // count number of zero crossings
    for( i = 0; i < size; i++ )
    {
        buffer.get( i, &v );
        xings += __SGN(v) != __SGN(p);
        p = v;
    }

    return xings;
}

CK_DLL_CTOR( ZeroX_ctor )
{
    OBJ_MEMBER_UINT(SELF, ZeroX_offset_data) = (t_CKUINT)new SAMPLE( 0.0f );
}

CK_DLL_DTOR( ZeroX_dtor )
{
    delete (SAMPLE *)OBJ_MEMBER_UINT(SELF, ZeroX_offset_data);
    OBJ_MEMBER_UINT(SELF, ZeroX_offset_data) = 0;
}

CK_DLL_TICK( ZeroX_tick )
{
    SAMPLE * d = (SAMPLE *)OBJ_MEMBER_UINT(SELF, ZeroX_offset_data);
    *out = __SGN(in) != __SGN(*d);
    *out *= __SGN(in);
    *d = in;

    return TRUE;
}

CK_DLL_TOCK( ZeroX_tock )
{
    t_CKFLOAT result = 0.0;

    // TODO: get buffer from stream, and set in SVM
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & mag = BLOB_IN->fvals();
        // compute ZeroX
        result = (t_CKFLOAT)( compute_zerox( mag, mag.size() ) + .5 );
    }
        // otherwise zero out
    else
    {
        // no input!
        result = 0.0;
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != 1 )
        fvals.set_size( 1 );
    // copy the result in
    fvals.set( 0, result );

    return TRUE;
}

CK_DLL_PMSG( ZeroX_pmsg )
{
    // do nothing
    return TRUE;
}

CK_DLL_SFUN( ZeroX_compute )
{
    // get array
    Chuck_Array8 * array = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // sanity check
    if( !array )
    {
        // input buffer was empty, so no zero crossings
        RETURN->v_float = 0.0;
    }
    else
    {
        // do it
        RETURN->v_float = (t_CKFLOAT)( compute_zerox( *array, array->size() ) + .5 );
    }
}
