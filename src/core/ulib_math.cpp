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
// file: ulib_math.cpp
// desc: class library for 'Math'
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "chuck_type.h"
#include "chuck_compile.h"
#include "ulib_math.h"
#include "util_math.h"
#include "ulib_std.h"

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <time.h>


static double g_pi = ONE_PI;
static double g_twopi = TWO_PI;
static double g_e = ::exp( 1.0 );
static t_CKFLOAT g_floatMax = DBL_MAX;
static t_CKFLOAT g_floatMin = DBL_MIN;
static t_CKFLOAT g_inf = 0.0;
static t_CKINT g_intMax = LONG_MAX;
static t_CKINT g_randomMax = CK_RANDOM_MAX;
static t_CKCOMPLEX g_i = { 0.0, 1.0 };
static t_CKFLOAT fzero() { return 0.0; }


#ifdef __WINDOWS_DS__
static long random() { return rand(); }
static void srandom( unsigned s ) { srand( s ); }
#endif // __WINDOWS_DS__


// query
DLL_QUERY libmath_query( Chuck_DL_Query * QUERY )
{
    // get global
    Chuck_Env * env = QUERY->env();
    // name
    QUERY->setname( QUERY, "Math" );

    /*! \example
    math.sin( math.pi /2.0 ) => stdout;
    */

    // register deprecate
    type_engine_register_deprecate( env, "math", "Math" );

    // add class
    QUERY->begin_class( QUERY, "Math", "Object" );
    
    // add abs
    QUERY->add_sfun( QUERY, abs_impl, "int", "abs" );
    QUERY->add_arg( QUERY, "int", "value" );
    
    // add fabs
    QUERY->add_sfun( QUERY, fabs_impl, "float", "fabs" );
    QUERY->add_arg( QUERY, "float", "value" );

    // add sgn
    QUERY->add_sfun( QUERY, sgn_impl, "float", "sgn" );
    QUERY->add_arg( QUERY, "float", "value" );
    
    // sin
    QUERY->add_sfun( QUERY, sin_impl, "float", "sin" );
    QUERY->add_arg( QUERY, "float", "x" );
    
    // cos
    QUERY->add_sfun( QUERY, cos_impl, "float", "cos" );
    QUERY->add_arg( QUERY, "float", "x" );

    // tan
    QUERY->add_sfun( QUERY, tan_impl, "float", "tan" );
    QUERY->add_arg( QUERY, "float", "x" );

    // asin
    QUERY->add_sfun( QUERY, asin_impl, "float", "asin" );
    QUERY->add_arg( QUERY, "float", "x" );

    // acos
    QUERY->add_sfun( QUERY, acos_impl, "float", "acos" );
    QUERY->add_arg( QUERY, "float", "x" );

    // atan
    QUERY->add_sfun( QUERY, atan_impl, "float", "atan" );
    QUERY->add_arg( QUERY, "float", "x" );

    // atan2
    QUERY->add_sfun( QUERY, atan2_impl, "float", "atan2" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->add_arg( QUERY, "float", "x" );

    // sinh
    QUERY->add_sfun( QUERY, sinh_impl, "float", "sinh" );
    QUERY->add_arg( QUERY, "float", "x" );

    // cosh
    QUERY->add_sfun( QUERY, cosh_impl, "float", "cosh" );
    QUERY->add_arg( QUERY, "float", "x" );

    // tanh
    QUERY->add_sfun( QUERY, tanh_impl, "float", "tanh" );
    QUERY->add_arg( QUERY, "float", "x" );

    // hypot
    QUERY->add_sfun( QUERY, hypot_impl, "float", "hypot" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // pow
    QUERY->add_sfun( QUERY, pow_impl, "float", "pow" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // sqrt
    QUERY->add_sfun( QUERY, sqrt_impl, "float", "sqrt" );
    QUERY->add_arg( QUERY, "float", "x" );

    // exp
    QUERY->add_sfun( QUERY, exp_impl, "float", "exp" );
    QUERY->add_arg( QUERY, "float", "x" );

    // log
    QUERY->add_sfun( QUERY, log_impl, "float", "log" );
    QUERY->add_arg( QUERY, "float", "x" );

    // log2
    QUERY->add_sfun( QUERY, log2_impl, "float", "log2" );
    QUERY->add_arg( QUERY, "float", "x" );

    // log10
    QUERY->add_sfun( QUERY, log10_impl, "float", "log10" );
    QUERY->add_arg( QUERY, "float", "x" );

    // floor
    QUERY->add_sfun( QUERY, floor_impl, "float", "floor" );
    QUERY->add_arg( QUERY, "float", "x" );

    // ceil
    QUERY->add_sfun( QUERY, ceil_impl, "float", "ceil" );
    QUERY->add_arg( QUERY, "float", "x" );

    // round
    QUERY->add_sfun( QUERY, round_impl, "float", "round" );
    QUERY->add_arg( QUERY, "float", "x" );

    // trunc
    QUERY->add_sfun( QUERY, trunc_impl, "float", "trunc" );
    QUERY->add_arg( QUERY, "float", "x" );

    // fmod
    QUERY->add_sfun( QUERY, fmod_impl, "float", "fmod" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // remainder
    QUERY->add_sfun( QUERY, remainder_impl, "float", "remainder" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // min
    QUERY->add_sfun( QUERY, min_impl, "float", "min" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // max
    //! see \example powerup.ck
    QUERY->add_sfun( QUERY, max_impl, "float", "max" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );

    // isinf
    QUERY->add_sfun( QUERY, isinf_impl, "int", "isinf" );
    QUERY->add_arg( QUERY, "float", "x" );

    // isnan
    QUERY->add_sfun( QUERY, isnan_impl, "int", "isnan" );
    QUERY->add_arg( QUERY, "float", "x" );

    // nextpow2
    QUERY->add_sfun( QUERY, nextpow2_impl, "int", "nextpow2" );
    QUERY->add_arg( QUERY, "int", "n" );

    // ensurepow2
    QUERY->add_sfun( QUERY, ensurepow2_impl, "int", "ensurePow2" );
    QUERY->add_arg( QUERY, "int", "n" );

    // floatMax
    // QUERY->add_sfun( QUERY, floatMax_impl, "float", "floatMax" );
    
    // intMax
    // QUERY->add_sfun( QUERY, intMax_impl, "int", "intMax" );
    
    // rand
    // QUERY->add_sfun( QUERY, rand_impl, "int", "rand" ); //! return int between 0 and RAND_MAX
    
    // rand2
    // QUERY->add_sfun( QUERY, rand2_impl, "int", "rand2" ); //! integer between [min,max]
    // QUERY->add_arg( QUERY, "int", "min" ); 
    // QUERY->add_arg( QUERY, "int", "max" ); 
    
    // randf
    // QUERY->add_sfun( QUERY, randf_impl, "float", "randf" ); //! rand between -1.0,1.0

    // rand2f
    // QUERY->add_sfun( QUERY, rand2f_impl, "float", "rand2f" ); //! rand between min and max
    // QUERY->add_arg( QUERY, "float", "min" );
    // QUERY->add_arg( QUERY, "float", "max" );

    // add mtof
    //! see \example mand-o-matic.ck
    QUERY->add_sfun( QUERY, mtof_impl, "float", "mtof" ); //! midi note to frequency
    QUERY->add_arg( QUERY, "float", "value" );

    // add ftom
    QUERY->add_sfun( QUERY, ftom_impl, "float", "ftom" ); //! frequency to midi note
    QUERY->add_arg( QUERY, "float", "value" );

    // add powtodb
    QUERY->add_sfun( QUERY, powtodb_impl, "float", "powtodb" ); //! linear power to decibel 
    QUERY->add_arg( QUERY, "float", "value" );

    // add rmstodb
    QUERY->add_sfun( QUERY, rmstodb_impl, "float", "rmstodb" ); //! rms to decibel
    QUERY->add_arg( QUERY, "float", "value" );

    // add dbtopow
    QUERY->add_sfun( QUERY, dbtopow_impl, "float", "dbtopow" ); //! decibel to linear
    QUERY->add_arg( QUERY, "float", "value" );

    // add dbtorms
    QUERY->add_sfun( QUERY, dbtorms_impl, "float", "dbtorms" ); //! decibel to rms
    QUERY->add_arg( QUERY, "float", "value" );
    
    // add re
    QUERY->add_sfun( QUERY, re_impl, "float", "re" ); //! real component of complex
    QUERY->add_arg( QUERY, "complex", "value" );
    
    // add im
    QUERY->add_sfun( QUERY, im_impl, "float", "im" ); //! imaginary component of complex
    QUERY->add_arg( QUERY, "complex", "value" );
    
    // add mag
    QUERY->add_sfun( QUERY, modulus_impl, "float", "mag" ); //! mag
    QUERY->add_arg( QUERY, "polar", "value" );
    
    // add phase
    QUERY->add_sfun( QUERY, phase_impl, "float", "phase" ); //! phase
    QUERY->add_arg( QUERY, "polar", "value" );
    
    // add rtop
    QUERY->add_sfun( QUERY, rtop_impl, "int", "rtop" ); // rect to polar
    QUERY->add_arg( QUERY, "complex[]", "from" );
    QUERY->add_arg( QUERY, "polar[]", "to" );
    
    // add ptor
    QUERY->add_sfun( QUERY, ptor_impl, "int", "ptor" ); // polar to rect
    QUERY->add_arg( QUERY, "polar[]", "from" );
    QUERY->add_arg( QUERY, "complex[]", "to" );

    // add random (1.3.1.0)
    QUERY->add_sfun( QUERY, random_impl, "int", "random"); //! return int between 0 and CK_RANDOM_MAX
    
    // add random2 (1.3.1.0)
    QUERY->add_sfun( QUERY, random2_impl, "int", "random2" ); //! integer between [min,max]
    QUERY->add_arg( QUERY, "int", "min" ); 
    QUERY->add_arg( QUERY, "int", "max" ); 
    
    // add randomf (1.3.1.0) -- NOTE different in return semantics
    QUERY->add_sfun( QUERY, randomf_impl, "float", "randomf" ); //! random between 0 and 1.0
    
    // add random2f (1.3.1.0)
    QUERY->add_sfun( QUERY, random2f_impl, "float", "random2f" ); //! random between min and max
    QUERY->add_arg( QUERY, "float", "min" );
    QUERY->add_arg( QUERY, "float", "max" );
    
    // add srandom (1.3.1.0)
    QUERY->add_sfun( QUERY, srandom_impl, "void", "srandom" );
    QUERY->add_arg( QUERY, "int", "seed" );
    
    // go ahead and seed (the code can seed again for repeatability; 1.3.1.0)
    srandom( time( NULL ) );

    // add gauss (ge: added 1.3.5.3)
    QUERY->add_sfun( QUERY, gauss_impl, "float", "gauss" ); //! Gaussian function
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "mean" );
    QUERY->add_arg( QUERY, "float", "sd" );

    // pi
    //! see \example math.ck
    QUERY->add_svar( QUERY, "float", "PI", TRUE, &g_pi );
    //! see \example math.ck
    // QUERY->add_svar( QUERY, "float", "pi", TRUE, &g_pi );

    // twopi
    QUERY->add_svar( QUERY, "float", "TWO_PI", TRUE, &g_twopi );
    // twopi
    // QUERY->add_svar( QUERY, "float", "two_pi", TRUE, &g_twopi );

    // e
    QUERY->add_svar( QUERY, "float", "E", TRUE, &g_e );
    // e
    QUERY->add_svar( QUERY, "float", "e", TRUE, &g_e );

    // float max
    assert( sizeof(t_CKFLOAT) == sizeof(double) );
    QUERY->add_svar( QUERY, "float", "FLOAT_MAX", TRUE, &g_floatMax );

    // float min
    QUERY->add_svar( QUERY, "float", "FLOAT_MIN_MAG", TRUE, &g_floatMin );

    // int max
#ifdef _WIN64 // REFACTOR-2017
    assert( sizeof(t_CKINT) == sizeof(long long) );
#else
    assert( sizeof(t_CKINT) == sizeof(long) );
#endif
    QUERY->add_svar( QUERY, "int", "INT_MAX", TRUE, &g_intMax );

    // infinity, using function to avoid potential "smart" compiler warning
    g_inf = 1.0 / fzero();
    QUERY->add_svar( QUERY, "float", "INFINITY", TRUE, &g_inf );

    // random max
    QUERY->add_svar( QUERY, "int", "RANDOM_MAX", TRUE, &g_randomMax );

    // i
    QUERY->add_svar( QUERY, "complex", "I", TRUE, &g_i );
    QUERY->add_svar( QUERY, "complex", "i", TRUE, &g_i );

    // j
    QUERY->add_svar( QUERY, "complex", "J", TRUE, &g_i );
    QUERY->add_svar( QUERY, "complex", "j", TRUE, &g_i );

    // done
    QUERY->end_class( QUERY );

    return TRUE;
}

// sin
CK_DLL_SFUN( sin_impl )
{
    RETURN->v_float = ::sin( GET_CK_FLOAT(ARGS) );
}

// cos
CK_DLL_SFUN( cos_impl )
{
    RETURN->v_float = ::cos( GET_CK_FLOAT(ARGS) );
}

// tan
CK_DLL_SFUN( tan_impl )
{
    RETURN->v_float = ::tan( GET_CK_FLOAT(ARGS) );
}

// asin
CK_DLL_SFUN( asin_impl )
{
    RETURN->v_float = ::asin( GET_CK_FLOAT(ARGS) );
}

// acos
CK_DLL_SFUN( acos_impl )
{
    RETURN->v_float = ::acos( GET_CK_FLOAT(ARGS) );
}

// atan
CK_DLL_SFUN( atan_impl )
{
    RETURN->v_float = ::atan( GET_CK_FLOAT(ARGS) );
}

// atan2
CK_DLL_SFUN( atan2_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1 );
    RETURN->v_float = ::atan2( x, y );
}

// sinh
CK_DLL_SFUN( sinh_impl )
{
    RETURN->v_float = ::sinh( GET_CK_FLOAT(ARGS) );
}

// cosh
CK_DLL_SFUN( cosh_impl )
{
    RETURN->v_float = ::cosh( GET_CK_FLOAT(ARGS) );
}

// tanh
CK_DLL_SFUN( tanh_impl )
{
    RETURN->v_float = ::tanh( GET_CK_FLOAT(ARGS) );
}

// hypot
CK_DLL_SFUN( hypot_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = ::hypot( x, y );
}

// pow
CK_DLL_SFUN( pow_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = ::pow( x, y );
}

// sqrt
CK_DLL_SFUN( sqrt_impl )
{
    RETURN->v_float = ::sqrt( GET_CK_FLOAT(ARGS) );
}

// exp
CK_DLL_SFUN( exp_impl )
{
    RETURN->v_float = ::exp( GET_CK_FLOAT(ARGS) );
}

// log
CK_DLL_SFUN( log_impl )
{
    RETURN->v_float = ::log( GET_CK_FLOAT(ARGS) );
}

// log2
CK_DLL_SFUN( log2_impl )
{
    RETURN->v_float = ::log( GET_CK_FLOAT(ARGS) )/::log( 2.0 );
}

// log10
CK_DLL_SFUN( log10_impl )
{
    RETURN->v_float = ::log10( GET_CK_FLOAT(ARGS) );
}

// floor
CK_DLL_SFUN( floor_impl )
{
    RETURN->v_float = ::floor( GET_CK_FLOAT(ARGS) );
}

// ceil
CK_DLL_SFUN( ceil_impl )
{
    RETURN->v_float = ::ceil( GET_CK_FLOAT(ARGS) );
}

// round
CK_DLL_SFUN( round_impl )
{
    RETURN->v_float = round( GET_CK_FLOAT(ARGS) );
}

// trunc
CK_DLL_SFUN( trunc_impl )
{
    RETURN->v_float = trunc( GET_CK_FLOAT(ARGS) );
}

// fmod
CK_DLL_SFUN( fmod_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = fmod( x, y );
}

// remainder
CK_DLL_SFUN( remainder_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = remainder( x, y );
}

// min
CK_DLL_SFUN( min_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = x < y ? x : y;
}

// max
CK_DLL_SFUN( max_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = x > y ? x : y;
}

// isinf
CK_DLL_SFUN( isinf_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
#ifdef __PLATFORM_WIN32__
    RETURN->v_int = !_finite( x );
#else
    RETURN->v_int = isinf( x );
#endif
}

// isnan
CK_DLL_SFUN( isnan_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
#ifdef __PLATFORM_WIN32__
    RETURN->v_int = _isnan( x );
#else
    RETURN->v_int = isnan( x );
#endif
}

// floatMax
CK_DLL_SFUN( floatMax_impl )
{
    // check size
    if( sizeof(t_CKFLOAT) == 8 )
        RETURN->v_float = DBL_MAX;
    else if( sizeof(t_CKFLOAT) == 4 )
        RETURN->v_float = FLT_MAX;
    else
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: internal error determining size of 'float' in floatMax()\n" );
        RETURN->v_float = 0; // TODO: return NaN?
    }
}

// intMax
CK_DLL_SFUN( intMax_impl )
{
    // check size
    if( sizeof(t_CKINT) == 4 )
        RETURN->v_int = LONG_MAX;
    else
    {
        // error
        CK_FPRINTF_STDERR( "[chuck]: internal error determining size of 'int' in intMax()\n" );
        RETURN->v_int = 0;
    }
}

// nextpow2 - thanks to Niklas Werner, via music-dsp
CK_DLL_SFUN( nextpow2_impl )
{
    t_CKINT x = GET_CK_INT(ARGS);
    t_CKINT xx = x;
    for( ; x &= x-1; xx = x );
    RETURN->v_int = xx * 2;
}

// ensurepow2 - thanks to Niklas Werner, via music-dsp
CK_DLL_SFUN( ensurepow2_impl )
{
    t_CKINT x = GET_CK_INT(ARGS);
    t_CKINT xx = --x;
    for( ; x &= x-1; xx = x );
    RETURN->v_int = xx * 2;
}

// re
CK_DLL_SFUN( re_impl )
{
    t_CKCOMPLEX x = GET_NEXT_COMPLEX(ARGS);
    RETURN->v_float = x.re;
}

// im
CK_DLL_SFUN( im_impl )
{
    t_CKCOMPLEX x = GET_NEXT_COMPLEX(ARGS);
    RETURN->v_float = x.im;
}

// modulus
CK_DLL_SFUN( modulus_impl )
{
    t_CKPOLAR x = GET_NEXT_POLAR(ARGS);
    RETURN->v_float = x.modulus;
}

// phase
CK_DLL_SFUN( phase_impl )
{
    t_CKPOLAR x = GET_NEXT_POLAR(ARGS);
    RETURN->v_float = x.phase;
}


// rtop
CK_DLL_SFUN( rtop_impl )
{
    // get array
    Chuck_Array16 * from = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array16 * to = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);

    // make sure not null
    if( !from || !to )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.rtop( ... ) was given one or more NULL arrays..." );
        return;
    }
    
    // find how much to copy
    t_CKUINT count = ck_min( from->size(), to->size() );
    t_CKCOMPLEX c;
    t_CKCOMPLEX p;
    
    // convert
    for( t_CKUINT i = 0; i < count; i++ )
    {
        // go for it
        from->get( i, &c );
        p.re = ::hypot( c.re, c.im );
        p.im = ::atan2( c.im, c.re );
        to->set( i, p );
    }
    
    // zero out the rest
    // if( count < to->size() ) to->zero( count, to->size() );
    if( count < to->size() ) to->set_size( count );
    
    // return number of elements done
    RETURN->v_int = count;
}


// ptor
CK_DLL_SFUN( ptor_impl )
{
    // get array
    Chuck_Array16 * from = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);
    Chuck_Array16 * to = (Chuck_Array16 *)GET_NEXT_OBJECT(ARGS);

    // make sure not null
    if( !from || !to )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.ptor( ... ) was given one or more NULL arrays..." );
        return;
    }
    
    // find how much to copy
    t_CKUINT count = ck_min( from->size(), to->size() );
    t_CKCOMPLEX c;
    t_CKPOLAR p;
    
    // convert
    for( t_CKUINT i = 0; i < count; i++ )
    {
        // go for it
        from->get( i, (t_CKCOMPLEX *)&p );
        c.re = p.modulus * ::cos( p.phase );
        c.im = p.modulus * ::sin( p.phase );
        to->set( i, c );
    }
    
    // zero out the rest
    // if( count < to->capacity() ) to->zero( count, to->size() );
    if( count < to->size() ) to->set_size( count );
    
    // return number of elements done
    RETURN->v_int = count;
}


// random (added 1.3.1.0)
CK_DLL_SFUN( random_impl )
{
    RETURN->v_int = ::random();
}


// randomf (added 1.3.1.0)
CK_DLL_SFUN( randomf_impl )
{
    RETURN->v_float = ( ::random() / (t_CKFLOAT)CK_RANDOM_MAX );
}


// randomf (added 1.3.1.0)
CK_DLL_SFUN( random2f_impl )
{
    t_CKFLOAT min = GET_CK_FLOAT(ARGS), max = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = min + (max-min)*(::random()/(t_CKFLOAT)CK_RANDOM_MAX);
}


// random2 (added 1.3.1.0)
CK_DLL_SFUN( random2_impl ) // inclusive.
{
    // 1.3.1.0: converted int to t_CKINT for 64-bit compatibility
    t_CKINT min = *(t_CKINT *)ARGS, max = *((t_CKINT *)ARGS + 1);
    t_CKINT range = max - min; 
    if( range == 0 )
    {
        RETURN->v_int = min;
    }
    //else if( range < RAND_MAX / 2 ) { 
    //  RETURN->v_int = ( range > 0 ) ? min + irandom_exclusive(1 + range) : max + irandom_exclusive ( -range + 1 ) ;
    //}
    else
    {
        if( range > 0 )
        { 
            RETURN->v_int = min + (t_CKINT)( (1.0 + range) * ( ::random()/(CK_RANDOM_MAX+1.0) ) );
        }
        else
        { 
            RETURN->v_int = min - (t_CKINT)( (-range + 1.0) * ( ::random()/(CK_RANDOM_MAX+1.0) ) );
        }
    }
}


// srandom (added 1.3.1.0)
CK_DLL_SFUN( srandom_impl )
{
    t_CKINT seed = GET_CK_INT(ARGS);
    ::srandom( seed );
}


// gauss (ge: added 1.3.5.3)
CK_DLL_SFUN( gauss_impl )
{
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT mu = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT sd = GET_NEXT_FLOAT(ARGS);

    // compute gaussian
    RETURN->v_float = (1.0 / (sd*::sqrt(TWO_PI))) * ::exp( -(x-mu)*(x-mu) / (2*sd*sd) );
}
