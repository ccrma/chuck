/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
#include "ulib_math.h"
#include "ulib_std.h"
#include "chuck_type.h"
#include "chuck_compile.h"
#include "util_math.h"

#include <float.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include <vector> // 1.5.0.0 (ge) | added
#include <limits> // 1.5.2.2 (nick) | added


static double g_pi = CK_ONE_PI;
static double g_twopi = CK_TWO_PI;
static double g_e = ::exp( 1.0 );
static t_CKFLOAT g_floatMax = CK_FLT_MAX;
static t_CKFLOAT g_floatMin = CK_FLT_MIN;
static t_CKFLOAT g_inf = 0.0;
static t_CKINT g_intMax = CK_INT_MAX;
static t_CKINT g_randomMax = CK_RANDOM_MAX;
static t_CKCOMPLEX g_i = { 0.0, 1.0 };
static t_CKFLOAT fzero() { return 0.0; }


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
    // add documentatiopn
    QUERY->doc_class( QUERY, "math class library." );

    // add examples | 1.5.0.4 (ge) added
    QUERY->add_ex( QUERY, "basic/blit2.ck" );
    QUERY->add_ex( QUERY, "stk/mand-o-matic.ck" );
    QUERY->add_ex( QUERY, "math/randomize.ck" );
    QUERY->add_ex( QUERY, "math/maybe.ck" );
    QUERY->add_ex( QUERY, "math/int-dist.ck" );
    QUERY->add_ex( QUERY, "math/map.ck" );

    // add abs
    QUERY->add_sfun( QUERY, abs_impl, "int", "abs" );
    QUERY->add_arg( QUERY, "int", "value" );
    QUERY->doc_func( QUERY, "Return absolute value of an integer value." );

    // add fabs
    QUERY->add_sfun( QUERY, fabs_impl, "float", "fabs" );
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Return absolute value of a floating point value." );

    // add sgn
    QUERY->add_sfun( QUERY, sgn_impl, "float", "sgn" );
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Return sign of 'value' as -1.0 (negative), 0.0, or 1.0 (positive)." );

    // sin
    QUERY->add_sfun( QUERY, sin_impl, "float", "sin" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute sine of x (measured in radians)." );

    // cos
    QUERY->add_sfun( QUERY, cos_impl, "float", "cos" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute cosine of x (measured in radians)." );

    // tan
    QUERY->add_sfun( QUERY, tan_impl, "float", "tan" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute tangent of x (measured in radians)." );

    // asin
    QUERY->add_sfun( QUERY, asin_impl, "float", "asin" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute arcsine of x; result in [-pi/2, +pi/2]." );

    // acos
    QUERY->add_sfun( QUERY, acos_impl, "float", "acos" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute arccosine of x; result in [0, pi]." );

    // atan
    QUERY->add_sfun( QUERY, atan_impl, "float", "atan" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute arctangent of x; result in [-pi/2, +pi/2]." );

    // atan2
    QUERY->add_sfun( QUERY, atan2_impl, "float", "atan2" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute arc tangent of two variables (y/x)." );

    // sinh
    QUERY->add_sfun( QUERY, sinh_impl, "float", "sinh" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the hyperbolic sine of x." );

    // cosh
    QUERY->add_sfun( QUERY, cosh_impl, "float", "cosh" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the hyperbolic cosine of x." );

    // tanh
    QUERY->add_sfun( QUERY, tanh_impl, "float", "tanh" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the hyperbolic tangent of x." );

    // hypot
    QUERY->add_sfun( QUERY, hypot_impl, "float", "hypot" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Compute the euclidean distance sqrt(x*x+y*y)." );

    // pow
    QUERY->add_sfun( QUERY, pow_impl, "float", "pow" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Compute x raised to the y-th power." );

    // sqrt
    QUERY->add_sfun( QUERY, sqrt_impl, "float", "sqrt" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the non-negative square root of x." );

    // exp
    QUERY->add_sfun( QUERY, exp_impl, "float", "exp" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute e^x, the base-e exponential of x." );

    // exp2
    QUERY->add_sfun( QUERY, exp2_impl, "float", "exp2" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute 2^x, the base-2 exponential of x." );

    // log
    QUERY->add_sfun( QUERY, log_impl, "float", "log" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the natural logarithm of x." );

    // log2
    QUERY->add_sfun( QUERY, log2_impl, "float", "log2" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the logarithm of x to base 2." );

    // log10
    QUERY->add_sfun( QUERY, log10_impl, "float", "log10" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Compute the logarithm of x to base 10." );

    // floor
    QUERY->add_sfun( QUERY, floor_impl, "float", "floor" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return the largest integer value (returned as float) not greater than x." );

    // ceil
    QUERY->add_sfun( QUERY, ceil_impl, "float", "ceil" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return the smallest integer value (returned as float) not less than x." );

    // round
    QUERY->add_sfun( QUERY, round_impl, "float", "round" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return the integer value (returned as float) nearest to x (rounding halfway cases away from zero)." );

    // trunc
    QUERY->add_sfun( QUERY, trunc_impl, "float", "trunc" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return the integer value nearest to but no greater in magnitude than x." );

    // fmod
    QUERY->add_sfun( QUERY, fmod_impl, "float", "fmod" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Compute the floating-point remainder of x / y." );

    // remainder
    QUERY->add_sfun( QUERY, remainder_impl, "float", "remainder" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Compute the value r such that r=x-n*y, where n is the integer nearest the exact value of x / y. If there are two integers closest to x / y, n shall be the even one. If r is zero, it is given the same sign as x." );

    // min | 1.5.2.2 (ge) added -- NOTE: for now put before float version; overloading needs to implement best match
    QUERY->add_sfun( QUERY, min_int_impl, "int", "min" );
    QUERY->add_arg( QUERY, "int", "x" );
    QUERY->add_arg( QUERY, "int", "y" );
    QUERY->doc_func( QUERY, "Return the lesser of x and y (int)." );

    // min
    QUERY->add_sfun( QUERY, min_impl, "float", "min" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Return the lesser of x and y (float)." );

    // max | 1.5.2.2 (ge) added -- NOTE: for now put before float version; overloading needs to implement best match
    QUERY->add_sfun( QUERY, max_int_impl, "int", "max" );
    QUERY->add_arg( QUERY, "int", "x" );
    QUERY->add_arg( QUERY, "int", "y" );
    QUERY->doc_func( QUERY, "Return the greater of x and y (integer)." );

    // max
    //! see \example powerup.ck
    QUERY->add_sfun( QUERY, max_impl, "float", "max" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Return the greater of x and y (float)." );

    // isinf
    QUERY->add_sfun( QUERY, isinf_impl, "int", "isinf" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return true if x is infinity, else return false.");

    // isnan
    QUERY->add_sfun( QUERY, isnan_impl, "int", "isnan" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->doc_func( QUERY, "Return true if x is not a number, else return false.");

    QUERY->add_sfun( QUERY, equal_impl, "int", "equal" );
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "y" );
    QUERY->doc_func( QUERY, "Return whether two floats are considered equal." );

    // nextpow2
    QUERY->add_sfun( QUERY, nextpow2_impl, "int", "nextpow2" );
    QUERY->add_arg( QUERY, "int", "x" );
    QUERY->doc_func( QUERY, "Compute the smallest power-of-2 greater than x." );

    // ensurepow2
    QUERY->add_sfun( QUERY, ensurepow2_impl, "int", "ensurePow2" );
    QUERY->add_arg( QUERY, "int", "x" );
    QUERY->doc_func( QUERY, "Return the smallest power-of-2 greater than or equal to the value of x.");

    // add mtof
    QUERY->add_sfun( QUERY, mtof_impl, "float", "mtof" ); //! midi note to frequency
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert a MIDI note number to frequency (Hz). Note that the input value is of type float and supports fractional note numbers." );

    // add ftom
    QUERY->add_sfun( QUERY, ftom_impl, "float", "ftom" ); //! frequency to midi note
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert frequency (Hz) to MIDI note number space." );

    // add powtodb
    QUERY->add_sfun( QUERY, powtodb_impl, "float", "powtodb" ); //! linear power to decibel
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert signal power ratio to decibels (dB)." );

    // add rmstodb
    QUERY->add_sfun( QUERY, rmstodb_impl, "float", "rmstodb" ); //! rms to decibel
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert linear amplitude to decibels (dB)." );

    // add dbtopow
    QUERY->add_sfun( QUERY, dbtopow_impl, "float", "dbtopow" ); //! decibel to linear
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert decibels (dB) to signal power ratio." );

    // add dbtorms
    QUERY->add_sfun( QUERY, dbtorms_impl, "float", "dbtorms" ); //! decibel to rms
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert decibles (dB) to linear amplitude." );

    // add re
    QUERY->add_sfun( QUERY, re_impl, "float", "re" ); //! real component of complex
    QUERY->add_arg( QUERY, "complex", "v" );
    QUERY->doc_func( QUERY, "Return the real component of complex value v." );

    // add im
    QUERY->add_sfun( QUERY, im_impl, "float", "im" ); //! imaginary component of complex
    QUERY->add_arg( QUERY, "complex", "v" );
    QUERY->doc_func( QUERY, "Return the imaginary component of complex value v." );

    // add mag
    QUERY->add_sfun( QUERY, modulus_impl, "float", "mag" ); //! mag
    QUERY->add_arg( QUERY, "polar", "v" );
    QUERY->doc_func( QUERY, "Return the magnitude component of polar value v." );

    // add phase
    QUERY->add_sfun( QUERY, phase_impl, "float", "phase" ); //! phase
    QUERY->add_arg( QUERY, "polar", "v" );
    QUERY->doc_func( QUERY, "Return the phase component of polar value v." );

    // add rtop
    QUERY->add_sfun( QUERY, rtop_impl, "int", "rtop" ); // rect to polar
    QUERY->add_arg( QUERY, "complex[]", "from" );
    QUERY->add_arg( QUERY, "polar[]", "to" );
    QUERY->doc_func( QUERY, "Convert complex values to polar values; returns number of values converted." );

    // add ptor
    QUERY->add_sfun( QUERY, ptor_impl, "int", "ptor" ); // polar to rect
    QUERY->add_arg( QUERY, "polar[]", "from" );
    QUERY->add_arg( QUERY, "complex[]", "to" );
    QUERY->doc_func( QUERY, "Convert polar values to complex values; returns number of values converted." );

    // add random (1.3.1.0) | [0,(2^31)-1]
    QUERY->add_sfun( QUERY, random_impl, "int", "random");
    QUERY->doc_func( QUERY, "Return successive pseudo-random integer numbers in the range [0, Math.RANDOM_MAX]]." );

    // add random2 (1.3.1.0)
    QUERY->add_sfun( QUERY, random2_impl, "int", "random2" ); //! integer between [min,max]
    QUERY->add_arg( QUERY, "int", "min" );
    QUERY->add_arg( QUERY, "int", "max" );
    QUERY->doc_func( QUERY, "Return successive pseudo-random numbers in the range [min, max]." );

    // add randomf (1.3.1.0) -- NOTE different in return semantics
    QUERY->add_sfun( QUERY, randomf_impl, "float", "randomf" ); //! random between 0 and 1.0
    QUERY->doc_func( QUERY, "Return successive pseudo-random floating-point numbers in the range [0,1]." );

    // add random2f (1.3.1.0)
    QUERY->add_sfun( QUERY, random2f_impl, "float", "random2f" ); //! random between min and max
    QUERY->add_arg( QUERY, "float", "min" );
    QUERY->add_arg( QUERY, "float", "max" );
    QUERY->doc_func( QUERY, "Return successive pseudo-random floating-point numbers in the range [min, max]." );

    // add srandom (1.3.1.0)
    QUERY->add_sfun( QUERY, srandom_impl, "void", "srandom" );
    QUERY->add_arg( QUERY, "int", "seed" );
    QUERY->doc_func( QUERY, "Seed the random number generator (RNG). Different seeds will generate very different sequences of random numbers even if the seeds are close together. Alternatively, a deterministic sequence of pseudo-random numbers can repeatably generated by setting the same seed.");

    // add randomize
    QUERY->add_sfun( QUERY, randomize_impl, "void", "randomize" );
    QUERY->doc_func( QUERY, "Randomize the seed of the random number generator (RNG), using an non-deterministic mechanism. Whereas srandom() explicitly seeds the RNG and will produce a deterministic sequence of pseudo-random numbers, randomize() \"shakes things up\" and causes RNG to start generating from a practically unpredicable seed. The quality of randomize() depends on the underlying implementation." );

    // go ahead and seed (the code can seed again for repeatability; 1.3.1.0)
    // updated to use ck_srandom() wrapper | 1.4.2.0 (ge)
    // ck_srandom( (unsigned)time( NULL ) );
    // updated to use ck_randomize() | 1.5.0.4 (ge)
    ck_randomize();

    // add gauss (ge: added 1.3.5.3)
    QUERY->add_sfun( QUERY, gauss_impl, "float", "gauss" ); //! Gaussian function
    QUERY->add_arg( QUERY, "float", "x" );
    QUERY->add_arg( QUERY, "float", "mean" );
    QUERY->add_arg( QUERY, "float", "sd" );
    QUERY->doc_func( QUERY, "Compute gaussian function at x, given mean and SD." );

    // add cossim (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, cossim_impl, "float", "cossim" ); //! cosine similarity
    QUERY->add_arg( QUERY, "float", "a[]" );
    QUERY->add_arg( QUERY, "float", "b[]" );
    QUERY->doc_func( QUERY, "Compute the cosine similarity between arrays a and b." );

    // add cossim (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, cossim3d_impl, "float", "cossim" ); //! cosine similarity
    QUERY->add_arg( QUERY, "vec3", "a" );
    QUERY->add_arg( QUERY, "vec3", "b" );
    QUERY->doc_func( QUERY, "Compute the cosine similarity between 3D vectors a and b." );

    // add cossim (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, cossim4d_impl, "float", "cossim" ); //! cosine similarity
    QUERY->add_arg( QUERY, "vec4", "a" );
    QUERY->add_arg( QUERY, "vec4", "b" );
    QUERY->doc_func( QUERY, "Compute the cosine similarity between 4D vectors a and b." );

    // add euclean (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, euclidean_impl, "float", "euclidean" ); //! euclidean similarity
    QUERY->add_arg( QUERY, "float", "a[]" );
    QUERY->add_arg( QUERY, "float", "b[]" );
    QUERY->doc_func( QUERY, "Compute the euclidean distance between arrays a and b." );

    // add euclean (ge: added 1.5.1.7)
    QUERY->add_sfun( QUERY, euclidean2d_impl, "float", "euclidean" ); //! euclidean similarity
    QUERY->add_arg( QUERY, "vec2", "a" );
    QUERY->add_arg( QUERY, "vec2", "b" );
    QUERY->doc_func( QUERY, "Compute the euclidean distance between 2D vectors a and b." );

    // add euclean (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, euclidean3d_impl, "float", "euclidean" ); //! euclidean similarity
    QUERY->add_arg( QUERY, "vec3", "a" );
    QUERY->add_arg( QUERY, "vec3", "b" );
    QUERY->doc_func( QUERY, "Compute the euclidean distance between 3D vectors a and b." );

    // add euclean (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, euclidean4d_impl, "float", "euclidean" ); //! euclidean similarity
    QUERY->add_arg( QUERY, "vec4", "a" );
    QUERY->add_arg( QUERY, "vec4", "b" );
    QUERY->doc_func( QUERY, "Compute the euclidean distance between 4D vectors a and b." );

    // add map (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, map_impl, "float", "map" ); //! map
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "x1" );
    QUERY->add_arg( QUERY, "float", "y1" );
    QUERY->add_arg( QUERY, "float", "x2" );
    QUERY->add_arg( QUERY, "float", "y2" );
    QUERY->doc_func( QUERY, "Map 'value' from range [x1,y1] into range [x2,y2]; 'value' can be outside range[x1,y1]. (see also: Math.map2())" );

    // add map (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, map2_impl, "float", "map2" ); //! map
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "x1" );
    QUERY->add_arg( QUERY, "float", "y1" );
    QUERY->add_arg( QUERY, "float", "x2" );
    QUERY->add_arg( QUERY, "float", "y2" );
    QUERY->doc_func( QUERY, "Map 'value' from range [x1,y1] into range [x2,y2]; 'value' will be clamped to [x1,y1] if outside range. (see also: Math.map())" );

    // add remap (ge: added 1.5.0.0)
    QUERY->add_sfun( QUERY, map2_impl, "float", "remap" ); //! remap
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "x1" );
    QUERY->add_arg( QUERY, "float", "y1" );
    QUERY->add_arg( QUERY, "float", "x2" );
    QUERY->add_arg( QUERY, "float", "y2" );
    QUERY->doc_func( QUERY, "Same as Math.map2()." );

    // add clamp (ge: added 1.5.0.0 -- actually migrated from Std)
    QUERY->add_sfun( QUERY, clamp_impl, "int", "clampi" ); //! clamp to range (int)
    QUERY->add_arg( QUERY, "int", "value" );
    QUERY->add_arg( QUERY, "int", "min" );
    QUERY->add_arg( QUERY, "int", "max" );
    QUERY->doc_func( QUERY, "Clamp an integer to range [min,max]." );

    // add clampf (ge: added 1.5.0.0 -- actually migrated from Std)
    QUERY->add_sfun( QUERY, clampf_impl, "float", "clampf" ); //! clamp to range (float)
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "min" );
    QUERY->add_arg( QUERY, "float", "max" );
    QUERY->doc_func( QUERY, "Clamp a float to range [min,max]." );

    // pi
    //! see \example math.ck
    QUERY->add_svar( QUERY, "float", "PI", TRUE, &g_pi );
    QUERY->doc_var( QUERY, "An approximation of pi. (Same as global keyword 'pi'.)" );

    // 1.4.1.0 (ge): special disable to avoid conflict (don't forget to reenabled)
    type_engine_enable_reserved( env, "pi", FALSE );
    QUERY->add_svar( QUERY, "float", "pi", TRUE, &g_pi );
    QUERY->doc_var( QUERY, "An approximation of pi. (Same as global keyword 'pi'.)" );

    // twopi
    QUERY->add_svar( QUERY, "float", "two_pi", TRUE, &g_twopi );
    QUERY->doc_var( QUERY, "An approximation of 2*pi" );
    // twopi
    QUERY->add_svar( QUERY, "float", "TWO_PI", TRUE, &g_twopi );
    QUERY->doc_var( QUERY, "An approximation of 2*pi" );

    // e
    QUERY->add_svar( QUERY, "float", "E", TRUE, &g_e );
    QUERY->doc_var( QUERY, "Euler's number; base of the natural logarithm." );
    // e
    QUERY->add_svar( QUERY, "float", "e", TRUE, &g_e );
    QUERY->doc_var( QUERY, "Euler's number; base of the natural logarithm." );

    // float max
    //assert( sizeof(t_CKFLOAT) == sizeof(double) );
    QUERY->add_svar( QUERY, "float", "FLOAT_MAX", TRUE, &g_floatMax );
    QUERY->doc_var( QUERY, "Largest representable floating-point value." );

    // float min
    QUERY->add_svar( QUERY, "float", "FLOAT_MIN_MAG", TRUE, &g_floatMin );
    QUERY->doc_var( QUERY, "Smallest representable non-negative floating-point value." );

    // int max
#ifdef _WIN64 // REFACTOR-2017
    //assert( sizeof(t_CKINT) == sizeof(long long) );
#else
    //assert( sizeof(t_CKINT) == sizeof(long) );
#endif
    QUERY->add_svar( QUERY, "int", "INT_MAX", TRUE, &g_intMax );
    QUERY->doc_var( QUERY, "Largest representable integer value." );

    // infinity, using function to avoid potential "smart" compiler warning
    // g_inf = 1.0 / fzero();
    // 1.5.0.0 (ge) | updated to use limits
    g_inf = std::numeric_limits<t_CKFLOAT>::infinity();
    QUERY->add_svar( QUERY, "float", "INFINITY", TRUE, &g_inf );
    QUERY->doc_var( QUERY, "Like, infinity." );

    // random max
    QUERY->add_svar( QUERY, "int", "RANDOM_MAX", TRUE, &g_randomMax );
    QUERY->doc_var( QUERY, "The largest possible value returned by random()." );

    // i
    QUERY->add_svar( QUERY, "complex", "I", TRUE, &g_i );
    QUERY->doc_var( QUERY, "The complex number sqrt(-1)." );
    QUERY->add_svar( QUERY, "complex", "i", TRUE, &g_i );
    QUERY->doc_var( QUERY, "The complex number sqrt(-1)." );

    // j
    QUERY->add_svar( QUERY, "complex", "J", TRUE, &g_i );
    QUERY->doc_var( QUERY, "The complex number sqrt(-1)." );
    QUERY->add_svar( QUERY, "complex", "j", TRUE, &g_i );
    QUERY->doc_var( QUERY, "The complex number sqrt(-1)." );

    // add examples
    // QUERY->add_ex( QUERY, "map.ck" );
    // QUERY->add_ex( QUERY, "math-help.ck" );

    // done
    QUERY->end_class( QUERY );

    // 1.4.1.0 (ge): IMPORTANT re-enable keyword
    type_engine_enable_reserved( env, "pi", TRUE );

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

// exp2
CK_DLL_SFUN( exp2_impl )
{
    RETURN->v_float = ::pow( 2, GET_CK_FLOAT(ARGS) );
    // exp2 is not in some earlier Visual C++ version, e.g., VC++ 2010
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
    // for older windows version (and miniAudicle windows build)
    // enable this in the build __CK_MATH_DEFINE_ROUND_TRUNC__
    RETURN->v_float = round( GET_CK_FLOAT(ARGS) );
}

// trunc
CK_DLL_SFUN( trunc_impl )
{
    // for older windows version (and miniAudicle windows build)
    // enable this in the build __CK_MATH_DEFINE_ROUND_TRUNC__
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
    RETURN->v_float = ck_remainder( x, y );
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

// min
CK_DLL_SFUN( min_int_impl )
{
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    RETURN->v_int = x < y ? x : y;
}

// max
CK_DLL_SFUN( max_int_impl )
{
    t_CKINT x = GET_NEXT_INT(ARGS);
    t_CKINT y = GET_NEXT_INT(ARGS);
    RETURN->v_int= x > y ? x : y;
}

// isinf
CK_DLL_SFUN( isinf_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
#ifdef __PLATFORM_WINDOWS__
    RETURN->v_int = !_finite( x );
#else
    RETURN->v_int = isinf( x );
#endif
}

// isnan
CK_DLL_SFUN( isnan_impl )
{
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
#ifdef __PLATFORM_WINDOWS__
    RETURN->v_int = _isnan( x );
#else
    RETURN->v_int = isnan( x );
#endif
}

// equal( x, y ) -- 1.4.1.1 (added ge)
// returns whether x and y (floats) are considered equal
CK_DLL_SFUN( equal_impl )
{
    // get arguments
    t_CKFLOAT x = GET_CK_FLOAT(ARGS);
    t_CKFLOAT y = *((t_CKFLOAT *)ARGS + 1);
    // equal
    RETURN->v_int = ck_equals(x,y);
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
    Chuck_ArrayVec2 * from = (Chuck_ArrayVec2 *)GET_NEXT_OBJECT(ARGS);
    Chuck_ArrayVec2 * to = (Chuck_ArrayVec2 *)GET_NEXT_OBJECT(ARGS);

    // make sure not null
    if( !from || !to )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.rtop(...) was given one or more NULL arrays..." );
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
    Chuck_ArrayVec2 * from = (Chuck_ArrayVec2 *)GET_NEXT_OBJECT(ARGS);
    Chuck_ArrayVec2 * to = (Chuck_ArrayVec2 *)GET_NEXT_OBJECT(ARGS);

    // make sure not null
    if( !from || !to )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.ptor(...) was given one or more NULL arrays..." );
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
    // 1.4.2.0 (ge) | updated to use ck_random() wrapper
    RETURN->v_int = ck_random();
}


// randomf (added 1.3.1.0)
CK_DLL_SFUN( randomf_impl )
{
    // 1.5.0.4 (ge) | updated to use ck_random_f() wrapper
    RETURN->v_float = ck_random_f();
}


// randomf (added 1.3.1.0)
CK_DLL_SFUN( random2f_impl )
{
    t_CKFLOAT min = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT max = GET_NEXT_FLOAT(ARGS);
    // 1.5.0.4 (ge) | updated to use ck_random_f() wrapper
    t_CKFLOAT normRand = ck_random_f();
    //CK_FPRINTF_STDERR( "[chuck random2f]: %G --> %G, %G\n", normRand, min, max );
    RETURN->v_float = min + (max-min) * normRand;
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
    else
    {
        if( range > 0 )
        {
            // 1.4.2.0 (ge) | updated to use ck_random() wrapper
            RETURN->v_int = min + (t_CKINT)( (1.0 + range) * ( ck_random()/((t_CKFLOAT)CK_RANDOM_MAX) ) );
        }
        else
        {
            // 1.4.2.0 (ge) | updated to use ck_random() wrapper
            RETURN->v_int = min - (t_CKINT)( (-range + 1.0) * ( ck_random()/((t_CKFLOAT)CK_RANDOM_MAX) ) );
        }
    }
}


// srandom (added 1.3.1.0)
CK_DLL_SFUN( srandom_impl )
{
    t_CKINT seed = GET_CK_INT(ARGS);
    // 1.4.2.0 (ge) | updated to use ck_srandom() wrapper
    ck_srandom( (unsigned)seed );
}


// randomize (added 1.5.0.4)
CK_DLL_SFUN( randomize_impl )
{
    // undo the seeding with some non-determinism
    ck_randomize();
}


// gauss (ge: added 1.3.5.3)
CK_DLL_SFUN( gauss_impl )
{
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT mu = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT sd = GET_NEXT_FLOAT(ARGS);

    // compute gaussian
    RETURN->v_float = (1.0 / (sd*::sqrt(CK_TWO_PI))) * ::exp( -(x-mu)*(x-mu) / (2*sd*sd) );
}


// cossim (ge) | added 1.5.0.0
CK_DLL_SFUN( cossim_impl )
{
    Chuck_ArrayFloat * a = (Chuck_ArrayFloat *)GET_NEXT_OBJECT( ARGS );
    Chuck_ArrayFloat * b = (Chuck_ArrayFloat *)GET_NEXT_OBJECT( ARGS );
    t_CKINT size = 0;

    // in case of error
    RETURN->v_float = 0.0;

    // if either is NULL, go to error
    if( a == NULL || b == NULL )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) was given one or more NULL arrays..." );
        // error out
        return;
    }

    // take either if equal, or the smaller if different
    size = a->size() < b->size() ? a->size() : b->size();
    // if either is size 0
    if( size == 0 )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) was given one or more 0-length arrays..." );
        // error out
        return;
    }

    // get the vecs
    std::vector<t_CKFLOAT> & v1 = a->m_vector;
    std::vector<t_CKFLOAT> & v2 = b->m_vector;

    t_CKFLOAT sum = 0.0, norm1 = 0.0, norm2 = 0.0;
    for( t_CKINT i = 0; i < size; i++ )
    {
        sum += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }

    // set return value
    RETURN->v_float = sum / (::sqrt(norm1) * ::sqrt(norm2));
}


// cossim (ge) | added 1.5.0.0
CK_DLL_SFUN( cossim3d_impl )
{
    t_CKVEC3 a = GET_NEXT_VEC3( ARGS );
    t_CKVEC3 b = GET_NEXT_VEC3( ARGS );

    // in case of error
    RETURN->v_float = 0.0;

    t_CKFLOAT sum = 0.0, norm1 = 0.0, norm2 = 0.0;

    // x
    sum += a.x * b.x;
    norm1 += a.x * a.x;
    norm2 += b.x * b.x;
    // y
    sum += a.y * b.y;
    norm1 += a.y * a.y;
    norm2 += b.y * b.y;
    // z
    sum += a.z * b.z;
    norm1 += a.z * a.z;
    norm2 += b.z * b.z;

    // if either norm1 or norm2 is 0
    if( norm1 == 0 || norm2 == 0 )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) zero in denominator..." );
        // error out
        return;
    }

    // set return value
    RETURN->v_float = sum / (::sqrt(norm1) * ::sqrt(norm2));
}


// euclidean4d (ge) | added 1.5.0.0
CK_DLL_SFUN( cossim4d_impl )
{
    t_CKVEC4 a = GET_NEXT_VEC4( ARGS );
    t_CKVEC4 b = GET_NEXT_VEC4( ARGS );

    // in case of error
    RETURN->v_float = 0.0;

    t_CKFLOAT sum = 0.0, norm1 = 0.0, norm2 = 0.0;

    // x
    sum += a.x * b.x;
    norm1 += a.x * a.x;
    norm2 += b.x * b.x;
    // y
    sum += a.y * b.y;
    norm1 += a.y * a.y;
    norm2 += b.y * b.y;
    // z
    sum += a.z * b.z;
    norm1 += a.z * a.z;
    norm2 += b.z * b.z;
    // 2
    sum += a.w * b.w;
    norm1 += a.w * a.w;
    norm2 += b.w * b.w;

    // if either norm1 or norm2 is 0
    if( norm1 == 0 || norm2 == 0 )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) zero in denominator..." );
        // error out
        return;
    }

    // set return value
    RETURN->v_float = sum / (::sqrt(norm1) * ::sqrt(norm2));
}



// euclidean (ge) | added 1.5.0.0
CK_DLL_SFUN( euclidean_impl )
{
    Chuck_ArrayFloat * a = (Chuck_ArrayFloat *)GET_NEXT_OBJECT( ARGS );
    Chuck_ArrayFloat * b = (Chuck_ArrayFloat *)GET_NEXT_OBJECT( ARGS );
    t_CKINT size = 0;

    // in case of error
    RETURN->v_float = 0.0;

    // if either is NULL, go to error
    if( a == NULL || b == NULL )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) was given one or more NULL arrays..." );
        // error out
        return;
    }

    // take either if equal, or the smaller if different
    size = a->size() < b->size() ? a->size() : b->size();
    // if either is size 0
    if( size == 0 )
    {
        // log
        EM_log( CK_LOG_WARNING, "Math.cossim(...) was given one or more 0-length arrays..." );
        // error out
        return;
    }

    // get the vecs
    std::vector<t_CKFLOAT> & v1 = a->m_vector;
    std::vector<t_CKFLOAT> & v2 = b->m_vector;

    t_CKFLOAT d;
    t_CKFLOAT sum = 0.0;
    for( t_CKINT i = 0; i < size; i++ )
    {
        d = v1[i] - v2[i];
        sum += d*d;
    }

    // set return value
    RETURN->v_float = ::sqrt(sum);
}


// euclidean (ge) | added 1.5.1.7
CK_DLL_SFUN( euclidean2d_impl )
{
    t_CKVEC2 a = GET_NEXT_VEC2( ARGS );
    t_CKVEC2 b = GET_NEXT_VEC2( ARGS );

    // in case of error
    RETURN->v_float = 0.0;

    t_CKFLOAT d;
    t_CKFLOAT sum = 0.0;
    d = a.x - b.x; sum += d*d;
    d = a.y - b.y; sum += d*d;

    // set return value
    RETURN->v_float = ::sqrt(sum);
}


// euclidean (ge) | added 1.5.0.0
CK_DLL_SFUN( euclidean3d_impl )
{
    t_CKVEC3 a = GET_NEXT_VEC3( ARGS );
    t_CKVEC3 b = GET_NEXT_VEC3( ARGS );

    // in case of error
    RETURN->v_float = 0.0;

    t_CKFLOAT d;
    t_CKFLOAT sum = 0.0;
    d = a.x - b.x; sum += d*d;
    d = a.y - b.y; sum += d*d;
    d = a.z - b.z; sum += d*d;

    // set return value
    RETURN->v_float = ::sqrt(sum);
}


// euclidean (ge) | added 1.5.0.0
CK_DLL_SFUN( euclidean4d_impl )
{
    t_CKVEC4 a = GET_NEXT_VEC4( ARGS );
    t_CKVEC4 b = GET_NEXT_VEC4( ARGS );

    // in case of error
    RETURN->v_float = 0.0;

    t_CKFLOAT d;
    t_CKFLOAT sum = 0.0;
    d = a.x - b.x; sum += d*d;
    d = a.y - b.y; sum += d*d;
    d = a.z - b.z; sum += d*d;
    d = a.w - b.w; sum += d*d;

    // set return value
    RETURN->v_float = ::sqrt(sum);
}

// map (ge) | added 1.5.0.0
CK_DLL_SFUN( map_impl )
{
    t_CKFLOAT v = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT x1 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT y1 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT x2 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT y2 = GET_NEXT_FLOAT( ARGS );

    // t_CKFLOAT min1 = x1 < y1 ? x1 : y1;
    // t_CKFLOAT max1 = x1 > y1 ? x1 : y1;

    // std::cerr << v << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    // remap
    RETURN->v_float = x2 + (v-x1)/(y1-x1)*(y2-x2);
}

// map2 (ge) | added 1.5.0.0
CK_DLL_SFUN( map2_impl )
{
    t_CKFLOAT v = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT x1 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT y1 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT x2 = GET_NEXT_FLOAT( ARGS );
    t_CKFLOAT y2 = GET_NEXT_FLOAT( ARGS );

    t_CKFLOAT min1 = x1 < y1 ? x1 : y1;
    t_CKFLOAT max1 = x1 > y1 ? x1 : y1;

    // clamp v in [x1, y1]
    if( v < min1 ) v = min1;
    else if( v > max1 ) v = max1;

    // std::cerr << v << " " << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    // remap
    RETURN->v_float = x2 + (v-x1)/(y1-x1)*(y2-x2);
}
