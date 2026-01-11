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
void ck_srandom(unsigned s) { g_ck_global_rng.seed(s); }
// randomize using underlying mechanic
void ck_randomize() { ck_srandom(g_ck_rd()); }
//-----------------------------------------------------------------------------
#else // using old school random (pre-c++11)
//-----------------------------------------------------------------------------
// name: ck_random() and ck_srandom()
// desc: chuck wrappers for random number generators | 1.4.2.0 (ge)
//-----------------------------------------------------------------------------
#ifndef __PLATFORM_WINDOWS__
t_CKINT ck_random() { return random(); }
t_CKFLOAT ck_random_f() { return random() / (t_CKFLOAT)CK_RANDOM_MAX; }
void ck_srandom(unsigned s) { srandom(s); }
#else // __PLATFORM_WINDOWS__
t_CKINT ck_random() { return rand(); }
t_CKFLOAT ck_random_f() { return rand() / (t_CKFLOAT)CK_RANDOM_MAX; }
void ck_srandom(unsigned s) { srand(s); }
#endif
// randomize using underlying mechanic
void ck_randomize() { ck_srandom((unsigned)time(NULL)); }
//-----------------------------------------------------------------------------
#endif // __OLDSCHOOL_RANDOM__




// windows / visual c++
#ifdef __PLATFORM_WINDOWS__
#ifdef __CK_MATH_DEFINE_ROUND_TRUNC__
//-----------------------------------------------------------------------------
// name: round()
// desc: round a to the nearest interger number
//-----------------------------------------------------------------------------
double round(double a)
{
    if (a >= 0) return (double)(t_CKINT)(a + .5);
    else return (double)(t_CKINT)(a - .5);
}


//-----------------------------------------------------------------------------
// name: trunc()
// desc: remove non-integer aspect of a
//-----------------------------------------------------------------------------
double trunc(double a)
{
    return (double)(long long)a;
}
#endif // __CK_MATH_DEFINE_ROUND_TRUNC
#endif // __PLATFORM_WINDOWS__




//-----------------------------------------------------------------------------
// name: ck_remainder()
// desc: the remainder of a / b
//-----------------------------------------------------------------------------
double ck_remainder(double a, double b)
{
#ifdef __PLATFORM_WINDOWS__
    t_CKINT div = a / b;
    return a - b * div;
#else
    return remainder(a, b);
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
double ck_mtof(double f)
{
    if (f <= -1500) return (0);
    else if (f > 1499) return (ck_mtof(1499));
    // else return (8.17579891564 * exp(.0577622650 * f));
    // TODO: optimize
    else return (pow(2, (f - 69) / 12.0) * 440.0);
}




//-----------------------------------------------------------------------------
// name: ck_ftom()
// desc: freq to midi
//-----------------------------------------------------------------------------
double ck_ftom(double f)
{
    // return (f > 0 ? 17.3123405046 * log(.12231220585 * f) : -1500);
    // TODO: optimize
    return (f > 0 ? (log(f / 440.0) / LOGTWO) * 12.0 + 69 : -1500);
}




//-----------------------------------------------------------------------------
// name: ck_powtodb()
// desc: pow to db
//-----------------------------------------------------------------------------
double ck_powtodb(double f)
{
    if (f <= 0) return (0);
    else
    {
        double val = 100 + 10. / LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}




//-----------------------------------------------------------------------------
// name: ck_rmstodb()
// desc: rms to db
//-----------------------------------------------------------------------------
double ck_rmstodb(double f)
{
    if (f <= 0) return (0);
    else
    {
        double val = 100 + 20. / LOGTEN * log(f);
        return (val < 0 ? 0 : val);
    }
}




//-----------------------------------------------------------------------------
// name: ck_dbtopow()
// desc: db to pow
//-----------------------------------------------------------------------------
double ck_dbtopow(double f)
{
    if (f <= 0)
        return (0);
    else
    {
        if (f > 870) f = 870;
        return (exp((LOGTEN * 0.1) * (f - 100.)));
    }
}




//-----------------------------------------------------------------------------
// name: ck_dbtorms()
// desc: db to rms
//-----------------------------------------------------------------------------
double ck_dbtorms(double f)
{
    if (f <= 0)
        return (0);
    else
    {
        if (f > 485) f = 485;
        return (exp((LOGTEN * 0.05) * (f - 100.)));
    }
}




//-----------------------------------------------------------------------------
// name: ck_nextpow2()
// desc: compute the next power of two greater than n
//-----------------------------------------------------------------------------
unsigned long ck_nextpow2(unsigned long n)
{
    unsigned long nn = n;
    for (; n &= n - 1; nn = n);
    return nn * 2;
}




//-----------------------------------------------------------------------------
// name: ck_ensurepow2()
// desc: return the largest power of two no less than n
//-----------------------------------------------------------------------------
unsigned long ck_ensurepow2(unsigned long n)
{
    return ck_nextpow2(n - 1);
}



//-----------------------------------------------------------------------------
// overloaded; uses default epsilon of 1e-8
//-----------------------------------------------------------------------------
t_CKBOOL ck_equals( t_CKFLOAT x, t_CKFLOAT y )
{
    // a small number 1e-8
    const t_CKFLOAT epsilon = .00000001;
    // call with default epsilon
    return ck_equals_ex( x, y, epsilon );
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
t_CKBOOL ck_equals_ex( t_CKFLOAT x, t_CKFLOAT y, t_CKFLOAT epsilon )
{
    // absolute values
    t_CKFLOAT abs_x = (x >= 0.0 ? x : -x);
    t_CKFLOAT abs_y = (y >= 0.0 ? y : -y);
    // smaller of the two absolute values (this step added by ge; ensures symmetry)
    t_CKFLOAT min = abs_x < abs_y ? abs_x : abs_y;
    // absolute value of the difference
    t_CKFLOAT v = x - y; t_CKFLOAT abs_v = (v >= 0.0 ? v : -v);
    // test whether difference is less/equal to episilon * smaller of two abs values
    return abs_v <= (epsilon * min);
}




//-----------------------------------------------------------------------------
// name: ck_complex_magnitude()
// desc: magnitude of complex number
//-----------------------------------------------------------------------------
t_CKFLOAT ck_complex_magnitude(const t_CKCOMPLEX& cmp)
{
    return ::sqrt(cmp.re * cmp.re + cmp.im * cmp.im);
}




//-----------------------------------------------------------------------------
// name: ck_complex_phase()
// desc: phase of complex number
//-----------------------------------------------------------------------------
t_CKFLOAT ck_complex_phase(const t_CKCOMPLEX& cmp)
{
    return ::atan2(cmp.im, cmp.re);
}




//-----------------------------------------------------------------------------
// name: ck_vec2_magnitude()
// desc: magnitude of vec2
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec2_magnitude(const t_CKVEC2& v)
{
    return ::sqrt(v.x * v.x + v.y * v.y);
}




//-----------------------------------------------------------------------------
// name: ck_vec2_phase()
// desc: phase of vec2 (treated as a complex value)
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec2_phase(const t_CKVEC2& cmp)
{
    return ::atan2(cmp.x, cmp.y);
}




//-----------------------------------------------------------------------------
// name: ck_vec3_magnitude()
// desc: magnitude of vec3
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec3_magnitude(const t_CKVEC3& v)
{
    return ::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}




//-----------------------------------------------------------------------------
// name: ck_vec4_magnitude()
// desc: magnitude of vec4
//-----------------------------------------------------------------------------
t_CKFLOAT ck_vec4_magnitude(const t_CKVEC4& v)
{
    return ::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

//-----------------------------------------------------------------------------
// name: factori
// desc: factorial array for associated_legendre()
// author: everett m. carpenter
//-----------------------------------------------------------------------------
static const double factori[] = {
        1.0,                    
        1.0,                    
        2.0,                    
        6.0,                    
        24.0,                   
        120.0,                  
        720.0,                  
        5040.0,                 
        40320.0,                
        362880.0,               
        3628800.0,              
        39916800.0,             
        479001600.0,            
        6227020800.0,           
        87178291200.0,          
        1307674368000.0,        
        20922789888000.0,       
        355687428096000.0,      
        6402373705728000.0,     
        121645100408832000.0,  
        2432902008176640000.0 
};

//-----------------------------------------------------------------------------
// name: associated_legendre() 
// desc: closed form associated legendre polynomials of some order l
// author: everett m. carpenter
//-----------------------------------------------------------------------------
float associated_legendre(int m, int l, float x)
{
    // check bounds
    if (l < 0 || l > 5) return 0.0;
    if (abs(m) > l) return 0.0;

    float x2 = x * x; // x^2
    float x3 = x2 * x; // x^3
    float x4 = x2 * x2;  // x^4
    float cf = 1.0 - x2; // pops up in every associated legendre
    float sqrtcf = sqrt(cf);; // also pops up everywhere
    int absm = abs(m); // assume m is positive, then apply -m case at end
    float result = 0.f;
    float flip = 1.f;
    if (m < 0) flip = pow(-1.0, absm) * (float)(factori[l - absm] / factori[l + absm]);

    switch (l) // order and absm
    {
    case(0): { result = 1.0; break; }
    case(1):
    {
        if (absm == 0) { result = x; }
        else if (absm == 1) { result = -sqrtcf; }
        break;
    }
    case(2):
    {
        if (absm == 0) { result = 0.5 * (3.0 * x2 - 1); }
        else if (absm == 1) { result = -3.0 * x * sqrtcf; }
        else if (absm == 2) { result = 3.0 * cf; }
        break;
    }
    case(3):
    {
        if (absm == 0) { result = 0.5 * (5.0 * x3 - 3.0 * x); }
        else if (absm == 1) { result = 1.5 * (1.0 - 5.0 * x2) * sqrtcf; }
        else if (absm == 2) { result = 15.0 * x * cf; }
        else if (absm == 3) { result = -15.0 * sqrtcf * cf; }
        break;
    }
    case(4):
    {
        if (absm == 0) { result = 0.125 * (35.0 * x4 - 30.0 * x2 + 3.0); }
        else if (absm == 1) { result = -2.5 * (7.0 * x3 - 3.0 * x) * sqrtcf; }
        else if (absm == 2) { result = 7.5 * (7.0 * x2 - 1) * cf; }
        else if (absm == 3) { result = -105.0 * x * sqrtcf * cf; }
        else if (absm == 4) { result = 105.0 * cf * cf; }
        break;
    }
    case(5):
    {
        if (absm == 0) { result = 0.125 * (63.0 * x3 * x2 - 70 * x3 + 15 * x); }
        else if (absm == 1) { result = -1.875 * sqrtcf * (21.0 * x4 - 14 * x2 + 1); }
        else if (absm == 2) { result = 52.5 * x * cf * (3.0 * x2 - 1.0); }
        else if (absm == 3) { result = -52.5 * cf * sqrtcf * (9.0 * x2 - 1.0); }
        else if (absm == 4) { result = 945.0 * x * cf * cf; }
        else if (absm == 5) { result = -945.0 * sqrtcf * cf * cf; }
        break;
    }
    }
    return pow(-1.0, m) * result * flip;
}

//-----------------------------------------------------------------------------
// name: NLOUP (normlization look-up)
// desc: look-up table of SN3D and N3D normalization terms created at compile time
// author: everett m. carpenter
//-----------------------------------------------------------------------------

class NLOUP
{
protected:
    const static unsigned MAX_ORDER = 6; // set
    const static unsigned MAX_DEGREE = 6;
    std::array<std::array<float, MAX_DEGREE>, MAX_ORDER> loup;

    float calcSN3D(unsigned order, int degree) // calculate SN3D value for loup
    {
        int d = (degree == 0) ? 1 : 0;																	   // Kronecker delta
        float ratio = static_cast<float>((factori[order - abs(degree)]) / (factori[order + abs(degree)])); // ratio of factorials
        return sqrtf((2.f - d) * ratio);
    }

    void fillLoup() // fill loup with results
    {
        for (int order = 0; order < MAX_ORDER; order++)
        {
            for (int degree = 0; degree <= order; degree++)
            {
                loup[order][degree] = calcSN3D(order, degree);
            }
        }
    }

public:
    NLOUP() : loup{} // init
    {
        fillLoup();
    }

    float SN3D(unsigned order, int degree) // retrieve SN3D
    {
        return order <= MAX_ORDER ? loup[order][abs(degree)] : 1; // safety check
    }

    float N3D(unsigned order, int degree) // retrieve N3D
    {
        return order <= MAX_ORDER ? sqrtf(2 * order + 1) * loup[order][abs(degree)] : 1; // safety check + convert to N3D
    }
};

NLOUP norms; // create LOUP

//-----------------------------------------------------------------------------
// name: SH()
// desc: calculation of spherical harmonics according to some order and normalization
// author: everett m. carpenter
//-----------------------------------------------------------------------------
float* SH(unsigned order_, const float azimuth_, const float zenith_, bool n3d) // SH calc
{
    float azimuth_shift = (azimuth_) * 0.01745329252; // degree 2 rad
    float zenith_shift = (90.f - zenith_) * 0.01745329252;
    float coszeni = cosf(zenith_shift);
    int size = (order_ + 1) * (order_ + 1);
    float* result = new float[size + 1]; // end result
    for (int order = 0; order <= (int)order_; order++)
    {
        if (order == 0)
            result[0] = norms.SN3D(order, 0);
        for (int degree = -order; degree <= order; degree++)
        {
            float n = n3d ? norms.N3D(order, degree) : norms.SN3D(order, degree); // normalization term if n3d bool = TRUE, return N3D else SN3D
            float p = (associated_legendre((int)abs(degree), (int)order, coszeni));
            float r = (degree < 0) ? sinf(abs(degree) * (azimuth_shift)) : cosf(degree * (azimuth_shift)); // degree positive? Re(exp(i*azimuth*degree)) degree negative? Im(exp(i*azimuth*degree))
            result[(order * order) + order + degree] = n * p * r;
        }
    }
    return result;
}



