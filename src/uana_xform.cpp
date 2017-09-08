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
// file: uana_xform.cpp
// desc: unit analyzer transforms
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//         Rebecca Fiebrink (fiebrink@cs.princeton.edu)
// date: Spring 2007
//-----------------------------------------------------------------------------
#include "uana_xform.h"
#include "chuck_type.h"
#include "chuck_vm.h"
#include "chuck_compile.h"
#include "chuck_instr.h"
#include "chuck_lang.h"
#include "util_buffers.h"
#include "util_xforms.h"


// FFT
CK_DLL_CTOR( FFT_ctor );
CK_DLL_DTOR( FFT_dtor );
CK_DLL_TICK( FFT_tick );
CK_DLL_PMSG( FFT_pmsg );
CK_DLL_TOCK( FFT_tock );
CK_DLL_CTRL( FFT_ctrl_window );
CK_DLL_CGET( FFT_cget_window );
CK_DLL_CGET( FFT_cget_windowSize );
CK_DLL_CTRL( FFT_ctrl_size );
CK_DLL_CGET( FFT_cget_size );
CK_DLL_MFUN( FFT_transform );
CK_DLL_MFUN( FFT_spectrum );
// static FFT offset
static t_CKUINT FFT_offset_data = 0;


// IFFT
CK_DLL_CTOR( IFFT_ctor );
CK_DLL_DTOR( IFFT_dtor );
CK_DLL_TICK( IFFT_tick );
CK_DLL_PMSG( IFFT_pmsg );
CK_DLL_TOCK( IFFT_tock );
CK_DLL_CTRL( IFFT_ctrl_window );
CK_DLL_CGET( IFFT_cget_window );
CK_DLL_CGET( IFFT_cget_windowSize );
CK_DLL_CTRL( IFFT_ctrl_size );
CK_DLL_CGET( IFFT_cget_size );
CK_DLL_MFUN( IFFT_transform );
CK_DLL_MFUN( IFFT_inverse );
// static IFFT offset
static t_CKUINT IFFT_offset_data = 0;


// Flip
CK_DLL_CTOR( Flip_ctor );
CK_DLL_DTOR( Flip_dtor );
CK_DLL_TICK( Flip_tick );
CK_DLL_PMSG( Flip_pmsg );
CK_DLL_TOCK( Flip_tock );
CK_DLL_CTRL( Flip_ctrl_window );
CK_DLL_CGET( Flip_cget_window );
CK_DLL_CGET( Flip_cget_windowSize );
CK_DLL_CTRL( Flip_ctrl_size );
CK_DLL_CGET( Flip_cget_size );
CK_DLL_MFUN( Flip_take );
CK_DLL_MFUN( Flip_output );
// static Flip offset
static t_CKUINT Flip_offset_data = 0;


// UnFlip
CK_DLL_CTOR( UnFlip_ctor );
CK_DLL_DTOR( UnFlip_dtor );
CK_DLL_TICK( UnFlip_tick );
CK_DLL_PMSG( UnFlip_pmsg );
CK_DLL_TOCK( UnFlip_tock );
CK_DLL_CTRL( UnFlip_ctrl_window );
CK_DLL_CGET( UnFlip_cget_window );
CK_DLL_CGET( UnFlip_cget_windowSize );
CK_DLL_CTRL( UnFlip_ctrl_size );
CK_DLL_CGET( UnFlip_cget_size );
CK_DLL_MFUN( UnFlip_take );
CK_DLL_MFUN( UnFlip_output );
// static UnFlip offset
static t_CKUINT UnFlip_offset_data = 0;


// DCT
CK_DLL_CTOR( DCT_ctor );
CK_DLL_DTOR( DCT_dtor );
CK_DLL_TICK( DCT_tick );
CK_DLL_PMSG( DCT_pmsg );
CK_DLL_TOCK( DCT_tock );
CK_DLL_CTRL( DCT_ctrl_window );
CK_DLL_CGET( DCT_cget_window );
CK_DLL_CGET( DCT_cget_windowSize );
CK_DLL_CTRL( DCT_ctrl_size );
CK_DLL_CGET( DCT_cget_size );
CK_DLL_MFUN( DCT_transform );
CK_DLL_MFUN( DCT_spectrum );
// static DCT offset
static t_CKUINT DCT_offset_data = 0;


// IDCT
CK_DLL_CTOR( IDCT_ctor );
CK_DLL_DTOR( IDCT_dtor );
CK_DLL_TICK( IDCT_tick );
CK_DLL_PMSG( IDCT_pmsg );
CK_DLL_TOCK( IDCT_tock );
CK_DLL_CTRL( IDCT_ctrl_window );
CK_DLL_CGET( IDCT_cget_window );
CK_DLL_CGET( IDCT_cget_windowSize );
CK_DLL_CTRL( IDCT_ctrl_size );
CK_DLL_CGET( IDCT_cget_size );
CK_DLL_MFUN( IDCT_transform );
CK_DLL_MFUN( IDCT_inverse );
// static IDCT offset
static t_CKUINT IDCT_offset_data = 0;


// Windowing
CK_DLL_SFUN( Windowing_hamming );
CK_DLL_SFUN( Windowing_hann );
CK_DLL_SFUN( Windowing_blackmanHarris );
CK_DLL_SFUN( Windowing_rectangle );
CK_DLL_SFUN( Windowing_triangle );
// static array
static Chuck_Array8 * Windowing_array = NULL;
static FLOAT * float_array = NULL;
static t_CKINT float_array_size = 0;




//-----------------------------------------------------------------------------
// name: xform_query()
// desc: ...
//-----------------------------------------------------------------------------
DLL_QUERY xform_query( Chuck_DL_Query * QUERY )
{
    Chuck_Env * env = QUERY->env();
    Chuck_DL_Func * func = NULL;
    
    std::string doc;

    //---------------------------------------------------------------------
    // init as base class: FFT
    //---------------------------------------------------------------------

    doc = "This UAna computes the Fast Fourier Transform on incoming audio samples, and outputs the result via its UAnaBlob as both the complex spectrum and the magnitude spectrum. A buffering mechanism maintains the previous FFTsize # of samples, allowing FFT's to be taken at any point in time, on demand (via .upchuck() or by upchucking a downstream UAna. The window size (along with an arbitry window shape) is controlled via the .window method. The hop size is complete dynamic, and is throttled by how time is advanced. ";
    
    if( !type_engine_import_uana_begin( env, "FFT", "UAna", env->global(),
                                        FFT_ctor, FFT_dtor,
                                        FFT_tick, FFT_tock, FFT_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    FFT_offset_data = type_engine_import_mvar( env, "int", "@FFT_data", FALSE );
    if( FFT_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_mfun( "void", "transform", FFT_transform );
    func->add_arg( "float[]", "from" );
    func->doc = "Manually take FFT (as opposed to using .upchuck() / upchuck operator)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", FFT_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", FFT_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", FFT_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the FFT-size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", FFT_cget_size );
    func->doc = "Get the FFT-size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // spectrum
    func = make_new_mfun( "void", "spectrum", FFT_spectrum );
    func->add_arg( "complex[]", "buffer" );
    func->doc = "Manually retrieve the results of a transform.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init as base class: IFFT
    //---------------------------------------------------------------------
    
    doc = "This UAna computes the inverse Fast Fourier Transform on incoming spectral frames (on demand), and overlap-adds the results into its internal buffer, ready to be sent to other UGen's connected via =>.  The window size (along with an arbitry window shape) is controlled via the .window method.";
    
    if( !type_engine_import_uana_begin( env, "IFFT", "UAna", env->global(), 
                                        IFFT_ctor, IFFT_dtor,
                                        IFFT_tick, IFFT_tock, IFFT_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    IFFT_offset_data = type_engine_import_mvar( env, "int", "@IFFT_data", FALSE );
    if( IFFT_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_mfun( "void", "transform", IFFT_transform );
    func->add_arg( "complex[]", "from" );
    func->doc = "Manually take IFFT (as opposed to using .upchuck() / upchuck operator)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", IFFT_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", IFFT_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", IFFT_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the IFFT-size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", IFFT_cget_size );
    func->doc = "Get the IFFT-size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // spectrum
    func = make_new_mfun( "void", "samples", IFFT_inverse );
    func->add_arg( "float[]", "buffer" );
    func->doc = "Manually take IFFT (as opposed to using .upchuck() / upchuck operator)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init as base class: Windowing
    //---------------------------------------------------------------------
    QUERY->begin_class( QUERY, "Windowing", "Object" );

    // add rectangle
    QUERY->add_sfun( QUERY, Windowing_rectangle, "float[]", "rectangle" );
    QUERY->add_arg( QUERY, "int", "size" );

    // add triangle
    QUERY->add_sfun( QUERY, Windowing_triangle, "float[]", "triangle" );
    QUERY->add_arg( QUERY, "int", "size" );

    // add hann
    QUERY->add_sfun( QUERY, Windowing_hann, "float[]", "hann" );
    QUERY->add_arg( QUERY, "int", "size" );

    // add hamming
    QUERY->add_sfun( QUERY, Windowing_hamming, "float[]", "hamming" );
    QUERY->add_arg( QUERY, "int", "size" );

    // add blackman-harris
    QUERY->add_sfun( QUERY, Windowing_blackmanHarris, "float[]", "blackmanHarris" );
    QUERY->add_arg( QUERY, "int", "size" );

    // done
    QUERY->end_class( QUERY );

    // initialize static data
    Windowing_array = new Chuck_Array8();
    initialize_object( Windowing_array, QUERY->env()->t_array );
    // TODO: yes? reference count
    Windowing_array->add_ref();
    // set size
    float_array_size = 16384;
    float_array = new FLOAT[float_array_size];


    //---------------------------------------------------------------------
    // init as base class: Flip
    //---------------------------------------------------------------------
    
    doc = "Turns audio samples into frames in the UAna domain.";
    
    if( !type_engine_import_uana_begin( env, "Flip", "UAna", env->global(), 
                                        Flip_ctor, Flip_dtor,
                                        Flip_tick, Flip_tock, Flip_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    Flip_offset_data = type_engine_import_mvar( env, "int", "@Flip_data", FALSE );
    if( Flip_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_mfun( "void", "transform", Flip_take );
    func->add_arg( "float[]", "from" );
    func->doc = "Manually take Flip (as opposed to using .upchuck() / upchuck operator).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", Flip_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", Flip_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // size
    func = make_new_mfun( "int", "size", Flip_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the Flip size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", Flip_cget_size );
    func->doc = "Get the Flip size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // output
    func = make_new_mfun( "void", "output", Flip_output );
    func->add_arg( "float[]", "buffer" );
    func->doc = "Manually take Flip (as opposed to using .upchuck() / upchuck operator)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    
    //---------------------------------------------------------------------
    // init as base class: pilF
    //---------------------------------------------------------------------
    
    doc = "Turns UAna frames into audio samples, via overlap add.";
    
    if( !type_engine_import_uana_begin( env, "pilF", "UAna", env->global(), 
                                        UnFlip_ctor, UnFlip_dtor,
                                        UnFlip_tick, UnFlip_tock, UnFlip_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    UnFlip_offset_data = type_engine_import_mvar( env, "int", "@UnFlip_data", FALSE );
    if( UnFlip_offset_data == CK_INVALID_OFFSET ) goto error;

    // go
    func = make_new_mfun( "void", "transform", UnFlip_take );
    func->doc = "Manually take pilF (as opposed to using .upchuck() / upchuck operator).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", UnFlip_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", UnFlip_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    
    // size
    func = make_new_mfun( "int", "size", UnFlip_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the pilF size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", UnFlip_cget_size );
    func->doc = "Get the pilF size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // output
    func = make_new_mfun( "void", "output", UnFlip_output );
    func->add_arg( "float[]", "buffer" );
    func->doc = "Manually take pilF (as opposed to using .upchuck() / upchuck operator)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );
    
    
    //---------------------------------------------------------------------
    // init as base class: DCT
    //---------------------------------------------------------------------
    
    doc = "This UAna computes the Discrete Cosine Transform on incoming audio samples, and outputs the result via its UAnaBlob as real values in the D.C. spectrum. A buffering mechanism maintains the previous DCT size # of samples, allowing DCT to be taken at any point in time, on demand (via .upchuck() or by upchucking a downstream UAna; see UAna documentation). The window size (along with an arbitry window shape) is controlled via the .window method. The hop size is complete dynamic, and is throttled by how time is advanced.";
    
    if( !type_engine_import_uana_begin( env, "DCT", "UAna", env->global(), 
                                        DCT_ctor, DCT_dtor,
                                        DCT_tick, DCT_tock, DCT_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    DCT_offset_data = type_engine_import_mvar( env, "int", "@DCT_data", FALSE );
    if( DCT_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_mfun( "void", "transform", DCT_transform );
    func->add_arg( "float[]", "from" );
    func->doc = "Manually take DCT (as opposed to using .upchuck() / upchuck operator).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", DCT_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", DCT_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", DCT_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the DCT size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", DCT_cget_size );
    func->doc = "Get the DCT size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // spectrum
    func = make_new_mfun( "void", "spectrum", DCT_spectrum );
    func->add_arg( "float[]", "buffer" );
    func->doc = "Manually retrieve the results of a transform.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


    //---------------------------------------------------------------------
    // init as base class: IDCT
    //---------------------------------------------------------------------
    
    doc = " This UAna computes the inverse Discrete Cosine Transform on incoming spectral frames (on demand), and overlap-adds the results into its internal buffer, ready to be sent to other UGen's connected via =>.  The window size (along with an arbitry window shape) is controlled via the .window method.";
    
    if( !type_engine_import_uana_begin( env, "IDCT", "UAna", env->global(), 
                                        IDCT_ctor, IDCT_dtor,
                                        IDCT_tick, IDCT_tock, IDCT_pmsg,
                                        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                        doc.c_str()) )
        return FALSE;

    // member variable
    IDCT_offset_data = type_engine_import_mvar( env, "int", "@IDCT_data", FALSE );
    if( IDCT_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_mfun( "void", "transform", IDCT_transform );
    func->add_arg( "complex[]", "from" );
    func->doc = "Manually take IDCT (as opposed to using .upchuck() / upchuck operator).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // window
    func = make_new_mfun( "float[]", "window", IDCT_ctrl_window );
    func->add_arg( "float[]", "win" );
    func->doc = "Set/get the transform window/size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // windowSize
    func = make_new_mfun( "int", "windowSize", IDCT_cget_windowSize );
    func->doc = "Get the current window size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", IDCT_ctrl_size );
    func->add_arg( "int", "size" );
    func->doc = "Set the IDCT size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;
    func = make_new_mfun( "int", "size", IDCT_cget_size );
    func->doc = "Get the IDCT size.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // spectrum
    func = make_new_mfun( "void", "samples", IDCT_inverse );
    func->add_arg( "float[]", "buffer" );
    func->doc = "Manually get result of previous IDCT.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

error:

    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}




//-----------------------------------------------------------------------------
// name: FFT_object
// desc: standalone object for FFT UAna
//-----------------------------------------------------------------------------
struct FFT_object
{
public:
    FFT_object();
    virtual ~FFT_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transformFromAccum( );
    void transform( Chuck_Array8 * frame );
    void copyTo( Chuck_Array16 * cmp );

public:
    // size of FFT
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample accumulation buffer
    AccumBuffer m_accum;
    // FFT buffer
    SAMPLE * m_buffer;
    // result
    t_CKCOMPLEX * m_spectrum;
};




//-----------------------------------------------------------------------------
// name: FFT_object()
// desc: constructor
//-----------------------------------------------------------------------------
FFT_object::FFT_object()
{
    // initialize
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    m_buffer = NULL;
    m_spectrum = NULL;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~FFT_object()
// desc: destructor
//-----------------------------------------------------------------------------
FFT_object::~FFT_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    SAFE_DELETE_ARRAY( m_spectrum );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set FFT size
//-----------------------------------------------------------------------------
t_CKBOOL FFT_object::resize( t_CKINT size )
{
    // sanity check
    assert( size > 0 );
    
    // next power of 2
    t_CKINT x = --size;
    for( ; size &= size-1; x = size );
    size = x*2;

    // log
    EM_log( CK_LOG_FINE, "FFT resize %d -> %d", m_size, size );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    SAFE_DELETE_ARRAY( m_spectrum );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    m_spectrum = new t_CKCOMPLEX[size/2];
    // check it
    if( !m_buffer || !m_spectrum )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: FFT failed to allocate %ld, %ld buffers...\n",
            size, size/2 );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        SAFE_DELETE_ARRAY( m_spectrum );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    memset( m_spectrum, 0, size/2 * sizeof(t_CKCOMPLEX) );
    // set
    m_size = size;
    // if no window specified, then set accum size
    if( !m_window )
    {
        m_window_size = m_size;
        // resize
        m_accum.resize( m_window_size );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL FFT_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: FFT failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    // resize
    m_accum.resize( m_window_size );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void FFT_object::transform()
{
    // buffer could be null
    if( m_buffer == NULL && m_spectrum == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: FFT failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );
    
    // apply window, if there is one
    if( m_window )
        apply_window( m_buffer, m_window, m_window_size );
    // zero pad
    memset( m_buffer + m_window_size, 0, (m_size - m_window_size)*sizeof(SAMPLE) );
    // go for it
    rfft( m_buffer, m_size/2, FFT_FORWARD );
    // copy into the result
    SAMPLE * ptr = m_buffer;
    for( t_CKINT i = 0; i < m_size/2; i++ )
    {
        // real and imaginary
        m_spectrum[i].re = *ptr++;
        m_spectrum[i].im = *ptr++;
    }
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void FFT_object::transformFromAccum()
{
    // get the last buffer of samples
    m_accum.get( m_buffer, m_window_size );

    // um
    transform();
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void FFT_object::transform( Chuck_Array8 * frame )
{
    // convert to right type
    t_CKINT amount = ck_min( frame->size(), m_size );
    // copy
    t_CKFLOAT v;
    for( t_CKINT i = 0; i < amount; i++ )
    {
        // real and imag
        frame->get( i, &v );
        m_buffer[i] = v;
    }
    
    // zero pad
    for( t_CKINT j = amount; j < m_size; j++ )
        m_buffer[j] = 0;

    // um
    this->transform();
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void FFT_object::copyTo( Chuck_Array16 * cmp )
{
    // buffer could be null
    if( m_buffer == NULL && m_spectrum == NULL )
    {
        // zero out
        cmp->clear();
        // bye
        return;
    }

    // get amount
    t_CKINT amount = m_size/2;
    // allocate
    cmp->set_size( amount );

    t_CKINT i;
    for( i = 0; i < amount; i++ )
         cmp->set( i, m_spectrum[i] );

    /*
    // copy modulo 2*pi
    t_CKINT left = cmp->size();
    t_CKINT amount, i, j, sum = 0, which = 0;

    // go
    while( left )
    {
        // get
        amount = ck_min( m_size, left );

        // go over spectrum
        if( which == 0 )
            for( i = 0; i < amount; i++ )
                 cmp->set( i+sum, m_spectrum[i] );
        else
            for( i = 0, j = m_size-1; i < amount; i++, j-- )
                 cmp->set( i+sum, m_spectrum[j] );

        // update
        left -= amount;
        sum += amount;
        which = !which;
    }
    */
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( FFT_ctor )
{
    // allocate the fft object
    FFT_object * fft = new FFT_object;
    OBJ_MEMBER_UINT( SELF, FFT_offset_data ) = (t_CKUINT)fft;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( FFT_dtor )
{
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    SAFE_DELETE( fft );
    OBJ_MEMBER_UINT(SELF, FFT_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( FFT_tick )
{
    // accumulate samples
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    fft->m_accum.put( in );
    // zero output
    *out = 0;
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( FFT_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( FFT_tock )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // take transform
    fft->transformFromAccum();
    // microsoft blows
    t_CKINT i;

    // get cvals of output BLOB
    Chuck_Array16 & cvals = BLOB->cvals();
    // ensure capacity == resulting size
    if( cvals.size() != fft->m_size/2 )
        cvals.set_size( fft->m_size/2 );
    // copy the result in
    for( i = 0; i < fft->m_size/2; i++ )
        cvals.set( i, fft->m_spectrum[i] );

    // get fvals of output BLOB; fill with magnitude spectrum
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != fft->m_size/2 )
        fvals.set_size( fft->m_size/2 );
    // copy the result in
    for( i = 0; i < fft->m_size/2; i++ )
        fvals.set( i, __modulus(fft->m_spectrum[i]) );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( FFT_transform )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // do it
    fft->transform( arr );
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FFT_ctrl_window )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // get window (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    fft->window( arr, arr != NULL ? arr->size() : 0 );
    // return it through
    RETURN->v_object = arr;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( FFT_cget_window )
{
    // TODO: implement this!
    RETURN->v_object = NULL;
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( FFT_cget_windowSize )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // return
    RETURN->v_int = fft->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( FFT_ctrl_size )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity check
    if( size <= 0 ) goto invalid_size;
    // set size
    fft->resize( size );
    // set RETURN
    RETURN->v_int = fft->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IFFT): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( FFT_cget_size )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // set RETURN
    RETURN->v_int = fft->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( FFT_spectrum )
{
    // get object
    FFT_object * fft = (FFT_object *)OBJ_MEMBER_UINT(SELF, FFT_offset_data);
    // get array
    Chuck_Array16 * arr = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via FFT): null array passed to spectrum(...)" );
        return;
    }
    
    // copy it
    fft->copyTo( arr );
}




//-----------------------------------------------------------------------------
// name: IFFT_object
// desc: standalone object for IFFT UAna
//-----------------------------------------------------------------------------
struct IFFT_object
{
public:
    IFFT_object();
    virtual ~IFFT_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transform( Chuck_Array16 * cmp );
    void copyTo( Chuck_Array8 * samples );

public:
    // size of IFFT
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample deccumulation buffer
    DeccumBuffer m_deccum;
    // IFFT buffer
    SAMPLE * m_buffer;
    // result
    SAMPLE * m_inverse;
};




//-----------------------------------------------------------------------------
// name: IFFT_object()
// desc: constructor
//-----------------------------------------------------------------------------
IFFT_object::IFFT_object()
{
    // initialize
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    m_buffer = NULL;
    m_inverse = NULL;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~IFFT_object()
// desc: destructor
//-----------------------------------------------------------------------------
IFFT_object::~IFFT_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    SAFE_DELETE_ARRAY( m_inverse );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set IFFT size
//-----------------------------------------------------------------------------
t_CKBOOL IFFT_object::resize( t_CKINT size )
{
    // sanity check
    assert( size > 0 );

    // next power of 2
    t_CKINT x = --size;
    for( ; size &= size-1; x = size );
    size = x*2;

    // log
    EM_log( CK_LOG_FINE, "IFFT resize %d -> %d", m_size, size );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    SAFE_DELETE_ARRAY( m_inverse );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    m_inverse = new SAMPLE[size];
    // check it
    if( !m_buffer || !m_inverse )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: IFFT failed to allocate %ld, %ld buffers...\n",
            size/2, size );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        SAFE_DELETE_ARRAY( m_inverse );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    memset( m_inverse, 0, size * sizeof(SAMPLE) );
    // set
    m_size = size;
    // set deccum size
    m_deccum.resize( m_size );
    // if no window specified, then set accum size
    if( !m_window )
        m_window_size = m_size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL IFFT_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: IFFT failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void IFFT_object::transform( )
{
    // buffer could be null
    if( m_buffer == NULL && m_inverse == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: IFFT failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );
    // go for it
    rfft( m_buffer, m_size/2, FFT_INVERSE );
    // copy
    memcpy( m_inverse, m_buffer, m_size * sizeof(SAMPLE) );
    // apply window, if there is one
    if( m_window )
        apply_window( m_inverse, m_window, m_window_size );
    // zero
    memset( m_inverse + m_window_size, 0, (m_size-m_window_size)*sizeof(SAMPLE) );
    // put in deccum buffer
    m_deccum.put( m_inverse, m_size );
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void IFFT_object::transform( Chuck_Array16 * cmp )
{
    // convert to right type
    t_CKINT amount = ck_min( cmp->size(), m_size/2 );
    // copy
    t_CKCOMPLEX v;
    for( t_CKINT i = 0; i < amount; i++ )
    {
        // real and imag
        cmp->get( i, &v );
        m_buffer[i*2] = v.re;
        m_buffer[i*2+1] = v.im;
    }
    // zero pad
    for( t_CKINT j = amount; j < m_size/2; j++ )
        m_buffer[j*2] = m_buffer[j*2+1] = 0;
    
    // um
    this->transform();
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void IFFT_object::copyTo( Chuck_Array8 * samples )
{
    // buffer could be null
    if( m_buffer == NULL && m_inverse == NULL )
    {
        // zero out
        samples->clear();
        // samples->zero( 0, samples->size() );
        // bye
        return;
    }

    // the amount
    t_CKINT amount = m_size; // ck_min( m_size, samples->size() );
    // allocate
    samples->set_size( amount );

    // go over
    for( t_CKINT i = 0; i < amount; i++ )
        samples->set( i, m_inverse[i] );

    // any left
    // if( amount < samples->size() )
    //     samples->set_size( amount );
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( IFFT_ctor )
{
    // allocate the ifft object
    IFFT_object * ifft = new IFFT_object;
    OBJ_MEMBER_UINT( SELF, IFFT_offset_data ) = (t_CKUINT)ifft;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( IFFT_dtor )
{
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    SAFE_DELETE( ifft );
    OBJ_MEMBER_UINT(SELF, IFFT_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( IFFT_tick )
{
    // accumulate samples
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // get output
    ifft->m_deccum.get( out );
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( IFFT_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( IFFT_tock )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // TODO: get buffer from stream, and set in ifft
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array16 & cmp = BLOB_IN->cvals();
        // resize if necessary
        if( cmp.size()*2 > ifft->m_size )
            ifft->resize( cmp.size()*2 );
        // sanity check
        assert( ifft->m_buffer != NULL );
        // copy into transform buffer
        t_CKCOMPLEX cval;
        for( t_CKINT i = 0; i < ifft->m_size/2; i++ )
        {
            // copy complex value in
            cmp.get( i, &cval );
            ifft->m_buffer[i*2] = cval.re;
            ifft->m_buffer[i*2+1] = cval.im;
        }

        // take transform
        ifft->transform();
    }
    // otherwise zero out
    else
    {
        // sanity check
        assert( ifft->m_buffer != NULL );
        memset( ifft->m_buffer, 0, sizeof(SAMPLE)*ifft->m_size );
        memset( ifft->m_inverse, 0, sizeof(SAMPLE)*ifft->m_size );
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != ifft->m_size )
        fvals.set_size( ifft->m_size );
    // copy the result in
    for( t_CKINT i = 0; i < ifft->m_size; i++ )
        fvals.set( i, ifft->m_inverse[i] );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( IFFT_transform )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // get complex array
    Chuck_Array16 * cmp = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);
    // sanity
    if( cmp == NULL ) goto null_pointer;
    // resize if bigger
    if( cmp->size()*2 > ifft->m_size )
        ifft->resize( cmp->size()*2 );
    // transform it
    ifft->transform( cmp );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IFFT): NullPointerException (argument is NULL)\n");
    goto done;

done:
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( IFFT_ctrl_window )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data );
    // get win (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    ifft->window( arr, arr != NULL ? arr->size() : 0 );
    // return
    RETURN->v_object = arr;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IFFT_cget_window )
{
    // TODO: implement this
    RETURN->v_object = NULL;
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IFFT_cget_windowSize )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // return
    RETURN->v_int = ifft->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( IFFT_ctrl_size )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity
    if( size <= 0 ) goto invalid_size;
    // set size
    ifft->resize( size );
    // set RETURN
    RETURN->v_int = ifft->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IFFT): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IFFT_cget_size )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // set RETURN
    RETURN->v_int = ifft->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( IFFT_inverse )
{
    // get object
    IFFT_object * ifft = (IFFT_object *)OBJ_MEMBER_UINT(SELF, IFFT_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via IFFT): null array passed to samples(...)" );
        return;
    }
    
    // copy it
    ifft->copyTo( arr );
}


//-----------------------------------------------------------------------------
// name: prepare_window()
// desc: ...
//-----------------------------------------------------------------------------
static t_CKBOOL prepare_window( void * ARGS, Chuck_VM_Shred * SHRED, t_CKINT & size )
{
    // get arg
    size = GET_NEXT_INT(ARGS);
    // sanity
    if( size <= 0 )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via Windowing): negative window size..." );
        return FALSE;
    }

    // resize the thing
    if( size != Windowing_array->size() )
        Windowing_array->set_size( size );
    // sanity
    if( size != Windowing_array->size() )
        goto out_of_memory;
    // resize if necessary
    if( size > float_array_size )
    {
        float_array_size = size;
        SAFE_DELETE_ARRAY( float_array );
        float_array = new FLOAT[float_array_size];
        if( !float_array ) goto out_of_memory;
    }

    return TRUE;

out_of_memory:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](Windowing): OutOfMemoryException (allocating FLOAT[%ld])\n",
        float_array_size );
    goto done;

done:
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }

    return FALSE;
}


//-----------------------------------------------------------------------------
// name: hamming()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_SFUN( Windowing_hamming )
{
    // prepare the thing
    t_CKINT size = 0;
    if( !prepare_window( ARGS, SHRED, size ) )
        return;

    // get the window
    hamming( float_array, size );
    // copy it
    for( t_CKINT i = 0; i < size; i++ )
        Windowing_array->set( i, (t_CKFLOAT)float_array[i] );
    // return
    RETURN->v_object = Windowing_array;
}


//-----------------------------------------------------------------------------
// name: hann()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_SFUN( Windowing_hann )
{
    // prepare the thing
    t_CKINT size = 0;
    if( !prepare_window( ARGS, SHRED, size ) )
        return;

    // get the window
    hanning( float_array, size );
    // copy it
    for( t_CKINT i = 0; i < size; i++ )
        Windowing_array->set( i, (t_CKFLOAT)float_array[i] );
    // return
    RETURN->v_object = Windowing_array;
}


//-----------------------------------------------------------------------------
// name: blackmanHarris()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_SFUN( Windowing_blackmanHarris )
{
    // prepare the thing
    t_CKINT size = 0;
    if( !prepare_window( ARGS, SHRED, size ) )
        return;

    // get the window
    blackman( float_array, size );
    // copy it
    for( t_CKINT i = 0; i < size; i++ )
        Windowing_array->set( i, (t_CKFLOAT)float_array[i] );
    // return
    RETURN->v_object = Windowing_array;
}


//-----------------------------------------------------------------------------
// name: rectangle()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_SFUN( Windowing_rectangle )
{
    // prepare the thing
    t_CKINT size = 0;
    if( !prepare_window( ARGS, SHRED, size ) )
        return;

    // 1's
    for( t_CKINT i = 0; i < size; i++ )
        Windowing_array->set( i, 1 );
    // return
    RETURN->v_object = Windowing_array;
}


//-----------------------------------------------------------------------------
// name: Windowing_triangle()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_SFUN( Windowing_triangle )
{
    // prepare the thing
    t_CKINT size = 0;
    if( !prepare_window( ARGS, SHRED, size ) )
        return;

    // get the window
    bartlett( float_array, size );
    // copy it
    for( t_CKINT i = 0; i < size; i++ )
        Windowing_array->set( i, (t_CKFLOAT)float_array[i] );
    // return
    RETURN->v_object = Windowing_array;
}




//-----------------------------------------------------------------------------
// name: delete_matrix()
// desc: deallocates NxN matrix
//-----------------------------------------------------------------------------
static void delete_matrix( SAMPLE ** matrix, t_CKUINT N )
{
    t_CKUINT i;

    // check
    if( !matrix ) return;

    // delete
    for( i = 0; i < N; i++ )
        SAFE_DELETE_ARRAY( matrix[i] );

    // delete
    SAFE_DELETE_ARRAY( matrix );
}




//-----------------------------------------------------------------------------
// name: Flip_object
// desc: standalone object for Flip UAna
//-----------------------------------------------------------------------------
struct Flip_object
{
public:
    Flip_object();
    virtual ~Flip_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transform( Chuck_Array8 * frame );
    void copyTo( Chuck_Array8 * val );

public:
    // buffer
    SAMPLE * m_buffer;
    // size of Flip
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample accumulation buffer
    AccumBuffer m_accum;
};




//-----------------------------------------------------------------------------
// name: Flip_object()
// desc: constructor
//-----------------------------------------------------------------------------
Flip_object::Flip_object()
{
    // initialize
    m_buffer = NULL;
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~Flip_object()
// desc: destructor
//-----------------------------------------------------------------------------
Flip_object::~Flip_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set Flip size
//-----------------------------------------------------------------------------
t_CKBOOL Flip_object::resize( t_CKINT size )
{
    // sanity check
    assert( size > 0 );

    // log
    EM_log( CK_LOG_FINE, "Flip resize %d -> %d", m_size, size );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    // check it
    if( !m_buffer )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: Flip failed to allocate %ld buffer...\n",
            size );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    // set
    m_size = size;
    // if no window specified, then set accum size
    if( !m_window )
    {
        m_window_size = m_size;
        // resize
        m_accum.resize( m_window_size );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL Flip_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: Flip failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    // resize
    m_accum.resize( m_window_size );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void Flip_object::transform( )
{
    // buffer could be null
    if( m_buffer == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: Flip failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );

    // get the last buffer of samples
    m_accum.get( m_buffer, m_window_size );
    // apply window, if there is one
    if( m_window )
        apply_window( m_buffer, m_window, m_window_size );
    // zero pad
    memset( m_buffer + m_window_size, 0, (m_size - m_window_size)*sizeof(SAMPLE) );
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void Flip_object::transform( Chuck_Array8 * frame )
{
    // convert to right type
    t_CKINT amount = ck_min( frame->size(), m_size );
    // copy
    t_CKFLOAT v;
    for( t_CKINT i = 0; i < amount; i++ )
    {
        // real and imag
        frame->get( i, &v );
        m_buffer[i] = v;
    }
    // zero pad
    for( t_CKINT j = amount; j < m_size; j++ )
        m_buffer[j] = 0;

    // um
    this->transform();
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void Flip_object::copyTo( Chuck_Array8 * val )
{
    // buffer could be null
    if( m_buffer == NULL )
    {
        // zero out
        val->clear();
        // bye
        return;
    }

    // get amount
    t_CKINT amount = m_size;
    // allocate
    val->set_size( amount );

    t_CKINT i;
    for( i = 0; i < amount; i++ )
         val->set( i, m_buffer[i] );
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Flip_ctor )
{
    // allocate the Flip object
    Flip_object * flip = new Flip_object;
    OBJ_MEMBER_UINT( SELF, Flip_offset_data ) = (t_CKUINT)flip;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( Flip_dtor )
{
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    SAFE_DELETE( flip );
    OBJ_MEMBER_UINT(SELF, Flip_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( Flip_tick )
{
    // accumulate samples
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    flip->m_accum.put( in );
    // zero output
    *out = 0;
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( Flip_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( Flip_tock )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // take transform
    flip->transform();
    // microsoft blows
    t_CKINT i;

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure capacity == resulting size
    if( fvals.size() != flip->m_size )
        fvals.set_size( flip->m_size );
    // copy the result in
    for( i = 0; i < flip->m_size; i++ )
        fvals.set( i, flip->m_buffer[i] );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( Flip_take )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // do it
    flip->transform( arr );
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flip_ctrl_window )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // get window (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    flip->window( arr, arr != NULL ? arr->size() : 0 );
    // return it through
    RETURN->v_object = arr;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flip_cget_window )
{
    // TODO: implement this!
    RETURN->v_object = NULL;
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flip_cget_windowSize )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // return
    RETURN->v_int = flip->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( Flip_ctrl_size )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity check
    if( size <= 0 ) goto invalid_size;
    // set size
    flip->resize( size );
    // set RETURN
    RETURN->v_int = flip->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IFFT): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( Flip_cget_size )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // set RETURN
    RETURN->v_int = flip->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( Flip_output )
{
    // get object
    Flip_object * flip = (Flip_object *)OBJ_MEMBER_UINT(SELF, Flip_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via Flip): null array passed to spectrum(...)" );
        return;
    }
    
    // copy it
    flip->copyTo( arr );
}




//-----------------------------------------------------------------------------
// name: UnFlip_object
// desc: standalone object for UnFlip UAna
//-----------------------------------------------------------------------------
struct UnFlip_object
{
public:
    UnFlip_object();
    virtual ~UnFlip_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transform( Chuck_Array8 * val );
    void copyTo( Chuck_Array8 * samples );

public:
    // size of IFFT
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample deccumulation buffer
    DeccumBuffer m_deccum;
    // UnFlip buffer
    SAMPLE * m_buffer;
};




//-----------------------------------------------------------------------------
// name: UnFlip_object()
// desc: constructor
//-----------------------------------------------------------------------------
UnFlip_object::UnFlip_object()
{
    // initialize
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    m_buffer = NULL;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~UnFlip_object()
// desc: destructor
//-----------------------------------------------------------------------------
UnFlip_object::~UnFlip_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set UnFlip size
//-----------------------------------------------------------------------------
t_CKBOOL UnFlip_object::resize( t_CKINT size )
{
    // sanity check
    assert( size > 0 );

    // log
    EM_log( CK_LOG_FINE, "UnFlip resize %d -> %d", m_size, size );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    // check it
    if( !m_buffer )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: UnFlip failed to allocate %ld buffer...\n",
            size );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    // set
    m_size = size;
    // set deccum size
    m_deccum.resize( m_size );
    // if no window specified, then set accum size
    if( !m_window )
        m_window_size = m_size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL UnFlip_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: UnFlip failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void UnFlip_object::transform( )
{
    // buffer could be null
    if( m_buffer == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: UnFlip failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );
    // apply window, if there is one
    if( m_window )
        apply_window( m_buffer, m_window, m_window_size );
    // zero
    memset( m_buffer + m_window_size, 0, (m_size-m_window_size)*sizeof(SAMPLE) );
    // put in deccum buffer
    m_deccum.put( m_buffer, m_size );
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void UnFlip_object::transform( Chuck_Array8 * val )
{
    // convert to right type
    t_CKINT amount = ck_min( val->size(), m_size );
    // copy
    t_CKFLOAT v;
    for( t_CKINT i = 0; i < amount; i++ )
    {
        // real and imag
        val->get( i, &v );
        m_buffer[i] = v;
    }
    // zero pad
    for( t_CKINT j = amount; j < m_size; j++ )
        m_buffer[j] = 0;
    
    // um
    this->transform();
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void UnFlip_object::copyTo( Chuck_Array8 * samples )
{
    // buffer could be null
    if( m_buffer == NULL )
    {
        // zero out
        samples->clear();
        // samples->zero( 0, samples->size() );
        // bye
        return;
    }

    // the amount
    t_CKINT amount = m_size; // ck_min( m_size, samples->size() );
    // allocate
    samples->set_size( amount );

    // go over
    for( t_CKINT i = 0; i < amount; i++ )
        samples->set( i, m_buffer[i] );
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( UnFlip_ctor )
{
    // allocate the UnFlip object
    UnFlip_object * unflip = new UnFlip_object;
    OBJ_MEMBER_UINT( SELF, UnFlip_offset_data ) = (t_CKUINT)unflip;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( UnFlip_dtor )
{
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    SAFE_DELETE( unflip );
    OBJ_MEMBER_UINT(SELF, UnFlip_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( UnFlip_tick )
{
    // accumulate samples
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // get output
    unflip->m_deccum.get( out );
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( UnFlip_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( UnFlip_tock )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // TODO: get buffer from stream, and set in UnFlip
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array8 & val = BLOB_IN->fvals();
        // resize if necessary
        if( val.size() > unflip->m_size )
            unflip->resize( val.size() );
        // sanity check
        assert( unflip->m_buffer != NULL );
        // copy into transform buffer
        t_CKFLOAT v;
        for( t_CKINT i = 0; i < unflip->m_size; i++ )
        {
            // copy value in
            val.get( i, &v );
            unflip->m_buffer[i] = v;
        }

        // take transform
        unflip->transform();
    }
    // otherwise zero out
    else
    {
        // sanity check
        assert( unflip->m_buffer != NULL );
        memset( unflip->m_buffer, 0, sizeof(SAMPLE)*unflip->m_size );
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != unflip->m_size )
        fvals.set_size( unflip->m_size );
    // copy the result in
    for( t_CKINT i = 0; i < unflip->m_size; i++ )
        fvals.set( i, unflip->m_buffer[i] );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( UnFlip_take )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // get complex array
    Chuck_Array8 * val = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // sanity
    if( val == NULL ) goto null_pointer;
    // resize if bigger
    if( val->size() > unflip->m_size )
        unflip->resize( val->size() );
    // transform it
    unflip->transform( val );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](UnFlip): NullPointerException (argument is NULL)\n");
    goto done;

done:
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( UnFlip_ctrl_window )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data );
    // get win (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    unflip->window( arr, arr != NULL ? arr->size() : 0 );
    // return
    RETURN->v_object = arr;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( UnFlip_cget_window )
{
    // TODO: implement this
    RETURN->v_object = NULL;
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( UnFlip_cget_windowSize )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // return
    RETURN->v_int = unflip->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( UnFlip_ctrl_size )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity
    if( size <= 0 ) goto invalid_size;
    // set size
    unflip->resize( size );
    // set RETURN
    RETURN->v_int = unflip->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](UnFlip): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( UnFlip_cget_size )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // set RETURN
    RETURN->v_int = unflip->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( UnFlip_output )
{
    // get object
    UnFlip_object * unflip = (UnFlip_object *)OBJ_MEMBER_UINT(SELF, UnFlip_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via UnFlip): null array passed to samples(...)" );
        return;
    }
    
    // copy it
    unflip->copyTo( arr );
}




//-----------------------------------------------------------------------------
// name: DCT_object
// desc: standalone object for DCT UAna
//-----------------------------------------------------------------------------
struct DCT_object
{
public:
    DCT_object();
    virtual ~DCT_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transform( const t_CKFLOAT * in, t_CKCOMPLEX * out );
    void copyTo( Chuck_Array8 * frame );

public:
    // size of DCT
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample accumulation buffer
    AccumBuffer m_accum;
    // DCT buffer
    SAMPLE * m_buffer;
    // DCT matrix
    SAMPLE ** m_matrix;
    // result
    SAMPLE * m_spectrum;
};




//-----------------------------------------------------------------------------
// name: DCT_object()
// desc: constructor
//-----------------------------------------------------------------------------
DCT_object::DCT_object()
{
    // initialize
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    m_buffer = NULL;
    m_matrix = NULL;
    m_spectrum = NULL;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~DCT_object()
// desc: destructor
//-----------------------------------------------------------------------------
DCT_object::~DCT_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    delete_matrix( m_matrix, m_size );
    SAFE_DELETE_ARRAY( m_spectrum );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set DCT size
//-----------------------------------------------------------------------------
t_CKBOOL DCT_object::resize( t_CKINT size )
{
    t_CKUINT i;

    // sanity check
    assert( size > 0 );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    delete_matrix( m_matrix, m_size );
    SAFE_DELETE_ARRAY( m_spectrum );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    m_spectrum = new SAMPLE[size];
    m_matrix = new SAMPLE *[size];
    for( i = 0; i < size; i++ ) m_matrix[i] = new SAMPLE[size];

    // check it
    if( !m_buffer || !m_spectrum || !m_matrix )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: DCT failed to allocate %ld, %ld buffers...\n",
            size, size/2 );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        delete_matrix( m_matrix, size );
        SAFE_DELETE_ARRAY( m_spectrum );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    memset( m_spectrum, 0, size * sizeof(SAMPLE) );
    // compute dct matrix
    the_dct_matrix( m_matrix, size );
    // set
    m_size = size;
    // if no window specified, then set accum size
    if( !m_window )
    {
        m_window_size = m_size;
        // resize
        m_accum.resize( m_window_size );
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL DCT_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: DCT failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    // resize
    m_accum.resize( m_window_size );

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void DCT_object::transform( )
{
    // buffer could be null
    if( m_buffer == NULL && m_spectrum == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: DCT failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );

    // get the last buffer of samples
    m_accum.get( m_buffer, m_window_size );
    // apply window, if there is one
    if( m_window )
        apply_window( m_buffer, m_window, m_window_size );
    // zero pad
    memset( m_buffer + m_window_size, 0, (m_size - m_window_size)*sizeof(SAMPLE) );
    // go for it
    the_dct_now( m_buffer, m_matrix, m_size, m_spectrum, m_size );
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void DCT_object::copyTo( Chuck_Array8 * frame )
{
    // buffer could be null
    if( m_buffer == NULL && m_spectrum == NULL )
    {
        // zero out
        frame->clear();
        // bye
        return;
    }

    // amount
    t_CKINT amount = m_size;
    // allocate
    frame->set_size( amount );

    t_CKINT i;
    for( i = 0; i < amount; i++ )
        frame->set( i, m_spectrum[i] );

    /*
    // copy modulo 2*pi
    t_CKINT left = frame->size();
    t_CKINT amount, i, sum = 0, which = 0;

    // go
    while( left )
    {
        // get
        amount = ck_min( m_size, left );

        // go over spectrum
        if( which == 0 )
            for( i = 0; i < amount; i++ )
                 frame->set( i+sum, m_spectrum[i] );
        else
            for( i = 0; i < amount; i++ )
                 frame->set( i+sum, 0.0 );

        // update
        left -= amount;
        sum += amount;
        if( !which ) which = 1;
    }
    */
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( DCT_ctor )
{
    // allocate the dct object
    DCT_object * dct = new DCT_object;
    OBJ_MEMBER_UINT( SELF, DCT_offset_data ) = (t_CKUINT)dct;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( DCT_dtor )
{
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    SAFE_DELETE( dct );
    OBJ_MEMBER_UINT(SELF, DCT_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( DCT_tick )
{
    // accumulate samples
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    dct->m_accum.put( in );
    // zero output
    *out = 0;
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( DCT_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( DCT_tock )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // take transform
    dct->transform();
    // microsoft blows
    t_CKINT i;

    // get cvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure capacity == resulting size
    if( fvals.size() != dct->m_size )
        fvals.set_size( dct->m_size );
    // copy the result in
    for( i = 0; i < dct->m_size; i++ )
        fvals.set( i, dct->m_spectrum[i] );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( DCT_transform )
{
    // get object
//    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // get array
//    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DCT_ctrl_window )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // get window (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    dct->window( arr, arr != NULL ? arr->size() : 0 );
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( DCT_cget_window )
{
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( DCT_cget_windowSize )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // return
    RETURN->v_int = dct->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( DCT_ctrl_size )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity check
    if( size <= 0 ) goto invalid_size;
    // set size
    dct->resize( size );
    // set RETURN
    RETURN->v_int = dct->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IDCT): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( DCT_cget_size )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // set RETURN
    RETURN->v_int = dct->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( DCT_spectrum )
{
    // get object
    DCT_object * dct = (DCT_object *)OBJ_MEMBER_UINT(SELF, DCT_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via DCT): null array passed to spectrum(...)" );
        return;
    }
    
    // copy it
    dct->copyTo( arr );
}




//-----------------------------------------------------------------------------
// name: IDCT_object
// desc: standalone object for IDCT UAna
//-----------------------------------------------------------------------------
struct IDCT_object
{
public:
    IDCT_object();
    virtual ~IDCT_object();
    
public:
    t_CKBOOL resize( t_CKINT size );
    t_CKBOOL window( Chuck_Array8 * window, t_CKINT win_size );
    void transform( );
    void transform( Chuck_Array8 * frame );
    void copyTo( Chuck_Array8 * samples );

public:
    // size of IDCT
    t_CKINT m_size;
    // window
    SAMPLE * m_window;
    // window size
    t_CKINT m_window_size;
    // sample deccumulation buffer
    DeccumBuffer m_deccum;
    // IDCT buffer
    SAMPLE * m_buffer;
    // IDCT matrix
    SAMPLE ** m_matrix;
    // result
    SAMPLE * m_inverse;
};




//-----------------------------------------------------------------------------
// name: IDCT_object()
// desc: constructor
//-----------------------------------------------------------------------------
IDCT_object::IDCT_object()
{
    // initialize
    m_size = 512;  // TODO: default
    m_window = NULL;
    m_window_size = m_size;
    m_buffer = NULL;
    m_matrix = NULL;
    m_inverse = NULL;
    // initialize window
    this->window( NULL, m_window_size );
    // allocate buffer
    this->resize( m_size );
}




//-----------------------------------------------------------------------------
// name: ~IDCT_object()
// desc: destructor
//-----------------------------------------------------------------------------
IDCT_object::~IDCT_object()
{
    // clean up
    SAFE_DELETE_ARRAY( m_window );
    SAFE_DELETE_ARRAY( m_buffer );
    delete_matrix( m_matrix, m_size );
    SAFE_DELETE_ARRAY( m_inverse );
    m_window_size = 0;
    m_size = 0;
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: set IDCT size
//-----------------------------------------------------------------------------
t_CKBOOL IDCT_object::resize( t_CKINT size )
{
    t_CKUINT i;

    // sanity check
    assert( size > 0 );

    // reallocate
    SAFE_DELETE_ARRAY( m_buffer );
    delete_matrix( m_matrix, m_size );
    SAFE_DELETE_ARRAY( m_inverse );
    m_size = 0;
    m_buffer = new SAMPLE[size];
    m_matrix = new SAMPLE *[size];
    for( i = 0; i < size; i++ ) m_matrix[i] = new SAMPLE[size];
    m_inverse = new SAMPLE[size];
    // check it TODO: check individual m_matrix[i]
    if( !m_buffer || !m_inverse || !m_matrix )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: IDCT failed to allocate %ld, %ld, %ldx%ld buffers...\n",
            size, size, size, size );
        // clean
        SAFE_DELETE_ARRAY( m_buffer );
        delete_matrix( m_matrix, size );
        SAFE_DELETE_ARRAY( m_inverse );
        // done
        return FALSE;
    }

    // zero it
    memset( m_buffer, 0, size * sizeof(SAMPLE) );
    memset( m_inverse, 0, size * sizeof(SAMPLE) );
    // compute IDCT matrix
    the_inverse_dct_matrix( m_matrix, size );
    // set
    m_size = size;
    // set deccum size
    m_deccum.resize( m_size );
    // if no window specified, then set accum size
    if( !m_window )
        m_window_size = m_size;

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: window()
// desc: set window
//-----------------------------------------------------------------------------
t_CKBOOL IDCT_object::window( Chuck_Array8 * win, t_CKINT win_size )
{
    // sanity check
    assert( win_size >= 0 );
    
    // in any case, clean up
    SAFE_DELETE_ARRAY( m_window );
    // reset
    m_window_size = 0;

    // could be NULL
    if( win != NULL )
    {
        m_window = new SAMPLE[win_size];
        // check it
        if( !m_window )
        {
            // out of memory
            CK_FPRINTF_STDERR( "[chuck]: IDCT failed to allocate %ldxSAMPLE window...\n",
                m_size );
            // done
            return FALSE;
        }

        // zero it
        memset( m_window, 0, win_size * sizeof(SAMPLE) );

        // set window    
        m_window_size = win_size;
        // copy
        t_CKFLOAT sample;
        for( t_CKINT i = 0; i < win_size; i++ )
        {
            // get
            win->get( i, &sample );
            // set
            m_window[i] = (SAMPLE)sample;
        }
    }
    else
    {
        // set
        m_window_size = m_size;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void IDCT_object::transform( )
{
    // buffer could be null
    if( m_buffer == NULL && m_inverse == NULL )
    {
        // out of memory
        CK_FPRINTF_STDERR( "[chuck]: IDCT failure due to NULL buffer...\n" );
        // bye
        return;
    }
    
    // sanity
    assert( m_window_size <= m_size );
    // go for it
    the_inverse_dct_now( m_buffer, m_matrix, m_size, m_inverse, m_size );
    // apply window, if there is one
    if( m_window )
        apply_window( m_inverse, m_window, m_window_size );
    // zero
    memset( m_inverse + m_window_size, 0, (m_size-m_window_size)*sizeof(SAMPLE) );
    // put in deccum buffer
    m_deccum.put( m_inverse, m_size );
}




//-----------------------------------------------------------------------------
// name: transform()
// desc: ...
//-----------------------------------------------------------------------------
void IDCT_object::transform( Chuck_Array8 * frame )
{
    // convert to right type
    t_CKINT amount = ck_min( frame->size(), m_size );
    // copy
    t_CKFLOAT v;
    for( t_CKINT i = 0; i < amount; i++ )
    {
        // real and imag
        frame->get( i, &v );
        m_buffer[i] = v;
    }
    // zero pad
    for( t_CKINT j = amount; j < m_size; j++ )
        m_buffer[j] = 0;
    
    // um
    this->transform();
}




//-----------------------------------------------------------------------------
// name: copyTo()
// desc: ...
//-----------------------------------------------------------------------------
void IDCT_object::copyTo( Chuck_Array8 * samples )
{
    // buffer could be null
    if( m_buffer == NULL && m_inverse == NULL )
    {
        // zero out
        samples->clear();
        // bye
        return;
    }

    // the amount
    t_CKINT amount = m_size; // ck_min( m_size, samples->size() );
    // allocate
    samples->set_size( amount );

    // go over
    for( t_CKINT i = 0; i < amount; i++ )
        samples->set( i, m_inverse[i] );

    // any left
    // if( amount < samples->size() )
    //     samples->set_size( amount );
}




//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTOR( IDCT_ctor )
{
    // allocate the idct object
    IDCT_object * idct = new IDCT_object;
    OBJ_MEMBER_UINT( SELF, IDCT_offset_data ) = (t_CKUINT)idct;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_DTOR( IDCT_dtor )
{
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    SAFE_DELETE( idct );
    OBJ_MEMBER_UINT(SELF, IDCT_offset_data) = 0;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TICK( IDCT_tick )
{
    // accumulate samples
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // get output
    idct->m_deccum.get( out );
    
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_PMSG( IDCT_pmsg )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_TOCK( IDCT_tock )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // TODO: get buffer from stream, and set in idct
    if( UANA->numIncomingUAnae() > 0 )
    {
        // get first
        Chuck_UAnaBlobProxy * BLOB_IN = UANA->getIncomingBlob( 0 );
        // sanity check
        assert( BLOB_IN != NULL );
        // get the array
        Chuck_Array16 & cmp = BLOB_IN->cvals();
        // resize if necessary
        if( cmp.size()*2 > idct->m_size )
            idct->resize( cmp.size()*2 );
        // sanity check
        assert( idct->m_buffer != NULL );
        // copy into transform buffer
        t_CKCOMPLEX cval;
        for( t_CKINT i = 0; i < idct->m_size/2; i++ )
        {
            // copy complex value in
            cmp.get( i, &cval );
            idct->m_buffer[i*2] = cval.re;
            idct->m_buffer[i*2+1] = cval.im;
        }

        // take transform
        idct->transform();
    }
    // otherwise zero out
    else
    {
        // sanity check
        assert( idct->m_buffer != NULL );
        memset( idct->m_buffer, 0, sizeof(SAMPLE)*idct->m_size );
        memset( idct->m_inverse, 0, sizeof(SAMPLE)*idct->m_size );
    }

    // get fvals of output BLOB
    Chuck_Array8 & fvals = BLOB->fvals();
    // ensure size == resulting size
    if( fvals.size() != idct->m_size )
        fvals.set_size( idct->m_size );
    // copy the result in
    for( t_CKINT i = 0; i < idct->m_size; i++ )
        fvals.set( i, idct->m_inverse[i] );

    return TRUE;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( IDCT_transform )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // get complex array
    Chuck_Array8 * frame = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // sanity
    if( frame == NULL ) goto null_pointer;
    // resize if bigger
    if( frame->size() > idct->m_size )
        idct->resize( frame->size() );
    // transform it
    idct->transform( frame );

    return;

null_pointer:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IDCT): NullPointerException (argument is NULL)\n");
    goto done;

done:
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CTRL( IDCT_ctrl_window )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data );
    // get win (can be NULL)
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // set it
    idct->window( arr, arr != NULL ? arr->size() : 0 );
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IDCT_cget_window )
{
}


//-----------------------------------------------------------------------------
// name: windowSize()
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IDCT_cget_windowSize )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // return
    RETURN->v_int = idct->m_window_size;
}


//-----------------------------------------------------------------------------
// name: size()
// desc: ...
//-----------------------------------------------------------------------------
CK_DLL_CTRL( IDCT_ctrl_size )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // get arg
    t_CKINT size = GET_NEXT_INT(ARGS);
    // sanity
    if( size <= 0 ) goto invalid_size;
    // set size
    idct->resize( size );
    // set RETURN
    RETURN->v_int = idct->m_size;

    return;

invalid_size:
    // we have a problem
    CK_FPRINTF_STDERR( 
        "[chuck](IDCT): InvalidTransformSizeException (%ld)\n", size );
    goto done;

done:
    // set RETURN type
    RETURN->v_int = 0;
    // do something!
    if( SHRED != NULL )
    {
        SHRED->is_running = FALSE;
        SHRED->is_done = TRUE;
    }
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_CGET( IDCT_cget_size )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // set RETURN
    RETURN->v_int = idct->m_size;
}


//-----------------------------------------------------------------------------
// name: 
// desc: 
//-----------------------------------------------------------------------------
CK_DLL_MFUN( IDCT_inverse )
{
    // get object
    IDCT_object * idct = (IDCT_object *)OBJ_MEMBER_UINT(SELF, IDCT_offset_data);
    // get array
    Chuck_Array8 * arr = (Chuck_Array8 *)GET_NEXT_OBJECT(ARGS);
    // check for null
    if( !arr )
    {
        // log
        EM_log( CK_LOG_WARNING, "(via IDCT): null array passed to samples(...)" );
        return;
    }
    
    // copy it
    idct->copyTo( arr );
}
