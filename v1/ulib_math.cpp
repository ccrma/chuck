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
// file: ulib_math.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_math.h"
#include "util_math.h"


static double g_pi = 3.14159265358979323846;
static double g_twopi = 2.0 * 3.14159265358979323846;
static double g_e = ::exp( 1.0 );


// query
DLL_QUERY libmath_query( Chuck_DL_Query * QUERY )
{
    // name
    QUERY->set_name( QUERY, "math" );

   /*! \example
     math.sin( math.pi /2.0 ) => stdout;
   */    
    
    // sin
    //! see \example math.ck
    QUERY->add_export( QUERY, "float", "sin", sin_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    
    // cos
    QUERY->add_export( QUERY, "float", "cos", cos_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // tan
    QUERY->add_export( QUERY, "float", "tan", tan_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // asin
    QUERY->add_export( QUERY, "float", "asin", asin_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // acos
    QUERY->add_export( QUERY, "float", "acos", acos_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // atan
    QUERY->add_export( QUERY, "float", "atan", atan_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // atan2
    QUERY->add_export( QUERY, "float", "atan2", atan2_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // sinh
    QUERY->add_export( QUERY, "float", "sinh", sinh_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // cosh
    QUERY->add_export( QUERY, "float", "cosh", cosh_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // tanh
    QUERY->add_export( QUERY, "float", "tanh", tanh_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // hypot
    QUERY->add_export( QUERY, "float", "hypot", hypot_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // pow
    QUERY->add_export( QUERY, "float", "pow", pow_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // sqrt
    QUERY->add_export( QUERY, "float", "sqrt", sqrt_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // exp
    QUERY->add_export( QUERY, "float", "exp", exp_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // log
    QUERY->add_export( QUERY, "float", "log", log_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // log2
    QUERY->add_export( QUERY, "float", "log2", log2_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // log10
    QUERY->add_export( QUERY, "float", "log10", log10_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // floor
    QUERY->add_export( QUERY, "float", "floor", floor_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // ceil
    QUERY->add_export( QUERY, "float", "ceil", ceil_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // round
    QUERY->add_export( QUERY, "float", "round", round_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // trunc
    QUERY->add_export( QUERY, "float", "trunc", trunc_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );

    // fmod
    QUERY->add_export( QUERY, "float", "fmod", fmod_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // remainder
    QUERY->add_export( QUERY, "float", "remainder", remainder_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // min
    QUERY->add_export( QUERY, "float", "min", min_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // max
    //! see \example powerup.ck
    QUERY->add_export( QUERY, "float", "max", max_impl, TRUE );
    QUERY->add_param( QUERY, "float", "x" );
    QUERY->add_param( QUERY, "float", "y" );

    // nextpow2
    QUERY->add_export( QUERY, "int", "nextpow2", nextpow2_impl, TRUE );
    QUERY->add_param( QUERY, "int", "n" );

    // pi
    //! see \example math.ck
    QUERY->add_export( QUERY, "float", "pi", (f_ck_func)&g_pi, FALSE );

    // twopi
    QUERY->add_export( QUERY, "float", "twopi", (f_ck_func)&g_twopi, FALSE );

    // e
    QUERY->add_export( QUERY, "float", "e", (f_ck_func)&g_e, FALSE );

    return TRUE;
}

// sin
CK_DLL_FUNC( sin_impl )
{
    RETURN->v_float = ::sin( GET_CK_FLOAT(ARGS) );
}

// cos
CK_DLL_FUNC( cos_impl )
{
    RETURN->v_float = ::cos( GET_CK_FLOAT(ARGS) );
}

// tan
CK_DLL_FUNC( tan_impl )
{
    RETURN->v_float = ::tan( GET_CK_FLOAT(ARGS) );
}

// asin
CK_DLL_FUNC( asin_impl )
{
    RETURN->v_float = ::asin( GET_CK_FLOAT(ARGS) );
}

// acos
CK_DLL_FUNC( acos_impl )
{
    RETURN->v_float = ::acos( GET_CK_FLOAT(ARGS) );
}

// atan
CK_DLL_FUNC( atan_impl )
{
    RETURN->v_float = ::atan( GET_CK_FLOAT(ARGS) );
}

// atan2
CK_DLL_FUNC( atan2_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1 );
    RETURN->v_float = ::atan2( x, y );
}

// sinh
CK_DLL_FUNC( sinh_impl )
{
    RETURN->v_float = ::sinh( GET_CK_FLOAT(ARGS) );
}

// cosh
CK_DLL_FUNC( cosh_impl )
{
    RETURN->v_float = ::cosh( GET_CK_FLOAT(ARGS) );
}

// tanh
CK_DLL_FUNC( tanh_impl )
{
    RETURN->v_float = ::tanh( GET_CK_FLOAT(ARGS) );
}

// hypot
CK_DLL_FUNC( hypot_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = ::hypot( x, y );
}

// pow
CK_DLL_FUNC( pow_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = ::pow( x, y );
}

// sqrt
CK_DLL_FUNC( sqrt_impl )
{
    RETURN->v_float = ::sqrt( GET_CK_FLOAT(ARGS) );
}

// exp
CK_DLL_FUNC( exp_impl )
{
    RETURN->v_float = ::exp( GET_CK_FLOAT(ARGS) );
}

// log
CK_DLL_FUNC( log_impl )
{
    RETURN->v_float = ::log( GET_CK_FLOAT(ARGS) );
}

// log2
CK_DLL_FUNC( log2_impl )
{
    RETURN->v_float = ::log( GET_CK_FLOAT(ARGS) )/::log( 2.0 );
}

// log10
CK_DLL_FUNC( log10_impl )
{
    RETURN->v_float = ::log10( GET_CK_FLOAT(ARGS) );
}

// floor
CK_DLL_FUNC( floor_impl )
{
    RETURN->v_float = ::floor( GET_CK_FLOAT(ARGS) );
}

// ceil
CK_DLL_FUNC( ceil_impl )
{
    RETURN->v_float = ::ceil( GET_CK_FLOAT(ARGS) );
}

// round
CK_DLL_FUNC( round_impl )
{
    RETURN->v_float = round( GET_CK_FLOAT(ARGS) );
}

// trunc
CK_DLL_FUNC( trunc_impl )
{
    RETURN->v_float = trunc( GET_CK_FLOAT(ARGS) );
}

// fmod
CK_DLL_FUNC( fmod_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = fmod( x, y );
}

// remainder
CK_DLL_FUNC( remainder_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = remainder( x, y );
}

// min
CK_DLL_FUNC( min_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = x < y ? x : y;
}

// max
CK_DLL_FUNC( max_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = x > y ? x : y;
}

// nextpow2 - thanks to Niklas Werner, via music-dsp
CK_DLL_FUNC( nextpow2_impl )
{
    t_CKINT x = GET_CK_INT(ARGS);
    t_CKINT xx = x;
    for( ; x &= x-1; xx = x );
    RETURN->v_int = xx * 2;
}
