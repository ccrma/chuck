/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
// name: util_math.cpp
// desc: a mini-compatibility library for math functions
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "util_math.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>




//-----------------------------------------------------------------------------
#ifndef __OLDSCHOOL_RANDOM__ // not using old school, pre-c++11 compatibility
//-----------------------------------------------------------------------------
// name: ck_random() and ck_srandom()
// desc: chuck wrappers for random number generators
// 1.5.0.1 (ge) using mt19937 (requires c++11)
//-----------------------------------------------------------------------------
#include <random>
// non-deterministic thing
static std::random_device g_ck_rd;
// mersenne twister RNG, based on the Mersenne prime (2^19937-1)
static std::mt19937 g_ck_global_rng;
// int and real distributions
static std::uniform_int_distribution<t_CKINT> g_ck_int_dist(0, CK_RANDOM_MAX);
static std::uniform_real_distribution<t_CKFLOAT> g_ck_real_dist(0.0, 1.0);
// ck_random() returns a signed int no greater than CK_RANDOM_MAX
// to maintain parity between 64-bit and 32-bit systems, CK_RANDOM_MAX is set
// to 0x7fffffff (or 2,147,483,647--the largest 32-bit signed number) rather
// than mt19937's actual max of 0xffffffff (or 4,294,967,295, 2^32-1)
t_CKINT ck_random() { return g_ck_int_dist(g_ck_global_rng); }
// get t_CKFLOAT in [0,1]
t_CKFLOAT ck_random_f() { return g_ck_real_dist(g_ck_global_rng); }
// seed the random number generator
void ck_srandom( unsigned s ) { g_ck_global_rng.seed(s); }
// randomize using underlying mechanic
void ck_randomize() { ck_srandom( g_ck_rd() ); }
//-----------------------------------------------------------------------------
#else // using old school random (pre-c++11)
//-----------------------------------------------------------------------------
// name: ck_random() and ck_srandom()
// desc: chuck wrappers for random number generators | 1.4.2.0 (ge)
//-----------------------------------------------------------------------------
#ifndef __PLATFORM_WINDOWS__
  t_CKINT ck_random() { return random(); }
  t_CKFLOAT ck_random_f() { return random() / (t_CKFLOAT)CK_RANDOM_MAX; }
  void ck_srandom( unsigned s ) { srandom( s ); }
#else // __PLATFORM_WINDOWS__
  t_CKINT ck_random() { return rand(); }
  t_CKFLOAT ck_random_f() { return rand() / (t_CKFLOAT)CK_RANDOM_MAX; }
  void ck_srandom( unsigned s ) { srand( s ); }
#endif
// randomize using underlying mechanic
void ck_randomize() { ck_srandom( (unsigned)time(NULL) ); }
//-----------------------------------------------------------------------------
#endif // __OLDSCHOOL_RANDOM__




// windows / visual c++
#ifdef __PLATFORM_WINDOWS__
#ifdef __CK_MATH_DEFINE_ROUND_TRUNC__
//-----------------------------------------------------------------------------
// name: round()
// desc: round a to the nearest interger number
//-----------------------------------------------------------------------------
  double round( double a )
  {
      if (a >= 0) return (double)(t_CKINT)(a + .5);
      else return (double)(t_CKINT)(a - .5);
  }


  //-----------------------------------------------------------------------------
  // name: trunc()
  // desc: remove non-integer aspect of a
  //-----------------------------------------------------------------------------
  double trunc( double a )
  {
      return (double)(long long)a;
  }
#endif // __CK_MATH_DEFINE_ROUND_TRUNC
#endif // __PLATFORM_WINDOWS__




//-----------------------------------------------------------------------------
// name: ck_remainder()
// desc: the remainder of a / b
//-----------------------------------------------------------------------------
double ck_remainder( double a, double b )
{
#ifdef __PLATFORM_WINDOWS__
    t_CKINT div = a/b;
    return a - b*div;
#else
    return remainder( a, b );
#endif
}




// the following 6 functions are lifted from PD source
// specifically x_acoustics.c
// http://puredata.info/downloads
#define LOGTWO 0.69314718055994528623
#define LOGTEN 2.302585092994




//-----------------------------------------------------------------------------
// name: ck_mtof()
// desc: midi to freq
//-----------------------------------------------------------------------------
double ck_mtof( double f )
{
    if( f <= -1500 ) return (0);
    else if( f > 1499 ) return (ck_mtof(1499));
    // else return (8.17579891564 * exp(.0577622650 * f));
    // TODO: optimize
    else return ( pow(2,(f-69)/12.0) * 440.0 );
}




//-----------------------------------------------------------------------------
// name: ck_ftom()
// desc: freq to midi
//-----------------------------------------------------------------------------
double ck_ftom( double f )
{   
    // return (f > 0 ? 17.3123405046 * log(.12231220585 * f) : -1500);
    // TODO: optimize
    return (f > 0 ? (log(f/440.0) / LOGTWO) * 12.0 + 69 : -1500);
}




//-----------------------------------------------------------------------------
// name: ck_powtodb()
// desc: pow to db
//-----------------------------------------------------------------------------
double ck_powtodb( double f )
{
    if( f <= 0 ) return (0);
    else
    {
        double val = 100 + 10./LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}




//-----------------------------------------------------------------------------
// name: ck_rmstodb()
// desc: rms to db
//-----------------------------------------------------------------------------
double ck_rmstodb( double f )
{
    if( f <= 0 ) return (0);
    else
    {
        double val = 100 + 20./LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}




//-----------------------------------------------------------------------------
// name: ck_dbtopow()
// desc: db to pow
//-----------------------------------------------------------------------------
double ck_dbtopow( double f )
{
    if( f <= 0 )
        return (0);
    else
    {
        if( f > 870 ) f = 870;
        return (exp((LOGTEN * 0.1) * (f-100.)));
    }
}




//-----------------------------------------------------------------------------
// name: ck_dbtorms()
// desc: db to rms
//-----------------------------------------------------------------------------
double ck_dbtorms( double f )
{
    if( f <= 0 )
        return (0);
    else
    {
        if( f > 485 ) f = 485;
        return (exp((LOGTEN * 0.05) * (f-100.)));
    }
}




//-----------------------------------------------------------------------------
// name: ck_nextpow2()
// desc: compute the next power of two greater than n
//-----------------------------------------------------------------------------
unsigned long ck_nextpow2( unsigned long n )
{
    unsigned long nn = n;
    for( ; n &= n-1; nn = n );
    return nn * 2;
}




//-----------------------------------------------------------------------------
// name: ck_ensurepow2()
// desc: return the largest power of two no less than n
//-----------------------------------------------------------------------------
unsigned long ck_ensurepow2( unsigned long n )
{
    return ck_nextpow2( n-1 );
}




//-----------------------------------------------------------------------------
// equal( x, y ) -- 1.4.1.1 (added ge)
// returns whether x and y (floats) are considered equal
//
// Knuth, Donald E., /The Art of Computer Programming
// Volume II: Seminumerical Algorithms/, Addison-Wesley, 1969.
// based on Knuth section 4.2.2 pages 217-218
// https://www.cs.technion.ac.il/users/yechiel/c++-faq/floating-point-arith.html
//-----------------------------------------------------------------------------
t_CKBOOL ck_equals( t_CKFLOAT x, t_CKFLOAT y )
{
    const t_CKFLOAT epsilon = .00000001; // a small number 1e-8
    // absolute values
    t_CKFLOAT abs_x = (x >= 0.0 ? x : -x);
    t_CKFLOAT abs_y = (y >= 0.0 ? y : -y);
    // smaller of the two absolute values (this step added by ge; ensures symmetry)
    t_CKFLOAT min = abs_x < abs_y ? abs_x : abs_y;
    // absolute value of the difference
    t_CKFLOAT v = x-y; t_CKFLOAT abs_v = (v >= 0.0 ? v : -v);
    // test whether difference is less/equal to episilon * smaller of two abs values
    return abs_v <= (epsilon * min);
}




//-----------------------------------------------------------------------------
// name: ck_complex_magnitude()
// desc: magnitude of complex number
//-----------------------------------------------------------------------------
t_CKFLOAT ck_complex_magnitude( const t_CKCOMPLEX & cmp )
{
    return ::sqrt( cmp.re*cmp.re + cmp.im*cmp.im );
}




//-----------------------------------------------------------------------------
// name: ck_complex_phase()
// desc: phase of complex number
//-----------------------------------------------------------------------------
t_CKFLOAT ck_complex_phase( const t_CKCOMPLEX & cmp )
{
    return ::atan2( cmp.im, cmp.re );
}




//-----------------------------------------------------------------------------
// name: ck_vec2_magnitude()
// desc: magnitude of vec2
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec2_magnitude( const t_CKVEC2 & v )
{
    return ::sqrt( v.x*v.x + v.y*v.y );
}




//-----------------------------------------------------------------------------
// name: ck_vec2_phase()
// desc: phase of vec2 (treated as a complex value)
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec2_phase( const t_CKVEC2 & cmp )
{
    return ::atan2( cmp.x, cmp.y );
}




//-----------------------------------------------------------------------------
// name: ck_vec3_magnitude()
// desc: magnitude of vec3
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec3_magnitude( const t_CKVEC3 & v )
{
    return ::sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}




//-----------------------------------------------------------------------------
// name: ck_vec4_magnitude()
// desc: magnitude of vec4
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec4_magnitude( const t_CKVEC4 & v )
{
    return ::sqrt( v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w );
}
