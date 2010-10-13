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
// name: util_xform.c
// desc: transforms
//
// authors: Ge Wang (gewang@cs.princeton.edu)
//          Perry R. Cook (prc@cs.princeton.edu)
//          FFT: CARL music distribution
// date: 11.27.2003
//-----------------------------------------------------------------------------
#include "util_xforms.h"
#include <stdlib.h>
#include <math.h>




//-----------------------------------------------------------------------------
// name: hanning()
// desc: make window
//-----------------------------------------------------------------------------
void hanning( FLOAT * window, unsigned long length )
{
    unsigned long i;
    double pi, phase = 0, delta;

    pi = 4.*atan(1.0);
    delta = 2 * pi / (double) length;

    for( i = 0; i < length; i++ )
    {
        window[i] = (FLOAT)(0.5 * (1.0 - cos(phase)));
        phase += delta;
    }
}




//-----------------------------------------------------------------------------
// name: hamming()
// desc: make window
//-----------------------------------------------------------------------------
void hamming( FLOAT * window, unsigned long length )
{
    unsigned long i;
    double pi, phase = 0, delta;

    pi = 4.*atan(1.0);
    delta = 2 * pi / (double) length;

    for( i = 0; i < length; i++ )
    {
        window[i] = (FLOAT)(0.54 - .46*cos(phase));
        phase += delta;
    }
}




//-----------------------------------------------------------------------------
// name: blackman()
// desc: make window
//-----------------------------------------------------------------------------
void blackman( FLOAT * window, unsigned long length )
{
    unsigned long i;
    double pi, phase = 0, delta;

    pi = 4.*atan(1.0);
    delta = 2 * pi / (double) length;

    for( i = 0; i < length; i++ )
    {
        window[i] = (FLOAT)(0.42 - .5*cos(phase) + .08*cos(2*phase));
        phase += delta;
    }
}




//-----------------------------------------------------------------------------
// name: bartlett()
// desc: make window
//-----------------------------------------------------------------------------
void bartlett( FLOAT * window, unsigned long length )
{
    unsigned long i;
    FLOAT half = (FLOAT)length / 2;

    for( i = 0; i < length; i++ )
    {
        if( i < half ) window[i] = i / half;
        else window[i] = (length - i) / half;
    }
}




//-----------------------------------------------------------------------------
// name: apply_window()
// desc: apply a window to data
//-----------------------------------------------------------------------------
void apply_window( FLOAT * data, FLOAT * window, unsigned long length )
{
    unsigned long i;

    for( i = 0; i < length; i++ )
        data[i] *= window[i];
}

static FLOAT PI ;
static FLOAT TWOPI ;
void bit_reverse( FLOAT * x, long N );

//-----------------------------------------------------------------------------
// name: rfft()
// desc: real value fft
//
//   these routines from the CARL software, spect.c
//   check out the CARL CMusic distribution for more source code
//
//   if forward is true, rfft replaces 2*N real data points in x with N complex 
//   values representing the positive frequency half of their Fourier spectrum,
//   with x[1] replaced with the real part of the Nyquist frequency value.
//
//   if forward is false, rfft expects x to contain a positive frequency 
//   spectrum arranged as before, and replaces it with 2*N real values.
//
//   N MUST be a power of 2.
//
//-----------------------------------------------------------------------------
void rfft( FLOAT * x, long N, unsigned int forward )
{
    static int first = 1 ;
    FLOAT c1, c2, h1r, h1i, h2r, h2i, wr, wi, wpr, wpi, temp, theta ;
    FLOAT xr, xi ;
    long i, i1, i2, i3, i4, N2p1 ;

    if( first )
    {
        PI = (FLOAT) (4.*atan( 1. )) ;
        TWOPI = (FLOAT) (8.*atan( 1. )) ;
        first = 0 ;
    }

    theta = PI/N ;
    wr = 1. ;
    wi = 0. ;
    c1 = 0.5 ;

    if( forward )
    {
        c2 = -0.5 ;
        cfft( x, N, forward ) ;
        xr = x[0] ;
        xi = x[1] ;
    }
    else
    {
        c2 = 0.5 ;
        theta = -theta ;
        xr = x[1] ;
        xi = 0. ;
        x[1] = 0. ;
    }
    
    wpr = (FLOAT) (-2.*pow( sin( 0.5*theta ), 2. )) ;
    wpi = (FLOAT) sin( theta ) ;
    N2p1 = (N<<1) + 1 ;
    
    for( i = 0 ; i <= N>>1 ; i++ )
    {
        i1 = i<<1 ;
        i2 = i1 + 1 ;
        i3 = N2p1 - i2 ;
        i4 = i3 + 1 ;
        if( i == 0 )
        {
            h1r =  c1*(x[i1] + xr ) ;
            h1i =  c1*(x[i2] - xi ) ;
            h2r = -c2*(x[i2] + xi ) ;
            h2i =  c2*(x[i1] - xr ) ;
            x[i1] =  h1r + wr*h2r - wi*h2i ;
            x[i2] =  h1i + wr*h2i + wi*h2r ;
            xr =  h1r - wr*h2r + wi*h2i ;
            xi = -h1i + wr*h2i + wi*h2r ;
        }
        else
        {
            h1r =  c1*(x[i1] + x[i3] ) ;
            h1i =  c1*(x[i2] - x[i4] ) ;
            h2r = -c2*(x[i2] + x[i4] ) ;
            h2i =  c2*(x[i1] - x[i3] ) ;
            x[i1] =  h1r + wr*h2r - wi*h2i ;
            x[i2] =  h1i + wr*h2i + wi*h2r ;
            x[i3] =  h1r - wr*h2r + wi*h2i ;
            x[i4] = -h1i + wr*h2i + wi*h2r ;
        }

        wr = (temp = wr)*wpr - wi*wpi + wr ;
        wi = wi*wpr + temp*wpi + wi ;
    }

    if( forward )
        x[1] = xr ;
    else
        cfft( x, N, forward ) ;
}




//-----------------------------------------------------------------------------
// name: cfft()
// desc: complex value fft
//
//   these routines from CARL software, spect.c
//   check out the CARL CMusic distribution for more software
//
//   cfft replaces FLOAT array x containing NC complex values (2*NC FLOAT 
//   values alternating real, imagininary, etc.) by its Fourier transform 
//   if forward is true, or by its inverse Fourier transform ifforward is 
//   false, using a recursive Fast Fourier transform method due to 
//   Danielson and Lanczos.
//
//   NC MUST be a power of 2.
//
//-----------------------------------------------------------------------------
void cfft( FLOAT * x, long NC, unsigned int forward )
{
    FLOAT wr, wi, wpr, wpi, theta, scale ;
    long mmax, ND, m, i, j, delta ;
    ND = NC<<1 ;
    bit_reverse( x, ND ) ;
    
    for( mmax = 2 ; mmax < ND ; mmax = delta )
    {
        delta = mmax<<1 ;
        theta = TWOPI/( forward? mmax : -mmax ) ;
        wpr = (FLOAT) (-2.*pow( sin( 0.5*theta ), 2. )) ;
        wpi = (FLOAT) sin( theta ) ;
        wr = 1. ;
        wi = 0. ;

        for( m = 0 ; m < mmax ; m += 2 )
        {
            register FLOAT rtemp, itemp ;
            for( i = m ; i < ND ; i += delta )
            {
                j = i + mmax ;
                rtemp = wr*x[j] - wi*x[j+1] ;
                itemp = wr*x[j+1] + wi*x[j] ;
                x[j] = x[i] - rtemp ;
                x[j+1] = x[i+1] - itemp ;
                x[i] += rtemp ;
                x[i+1] += itemp ;
            }

            wr = (rtemp = wr)*wpr - wi*wpi + wr ;
            wi = wi*wpr + rtemp*wpi + wi ;
        }
    }

    // scale output
    scale = (FLOAT)(forward ? 1./ND : 2.) ;
    {
        register FLOAT *xi=x, *xe=x+ND ;
        while( xi < xe )
            *xi++ *= scale ;
    }
}




//-----------------------------------------------------------------------------
// name: bit_reverse()
// desc: bitreverse places FLOAT array x containing N/2 complex values
//       into bit-reversed order
//-----------------------------------------------------------------------------
void bit_reverse( FLOAT * x, long N )
{
    FLOAT rtemp, itemp ;
    long i, j, m ;
    for( i = j = 0 ; i < N ; i += 2, j += m )
    {
        if( j > i )
        {
            rtemp = x[j] ; itemp = x[j+1] ; /* complex exchange */
            x[j] = x[i] ; x[j+1] = x[i+1] ;
            x[i] = rtemp ; x[i+1] = itemp ;
        }

        for( m = N>>1 ; m >= 2 && j >= m ; m >>= 1 )
            j -= m ;
    }
}




//-----------------------------------------------------------------------------
// name: the_dct()
// desc: type ii dct on N reals
//-----------------------------------------------------------------------------
void the_dct( FLOAT * x, unsigned long N, FLOAT * out, unsigned long Nout )
{
    unsigned long k, n;

    // sanity check
    assert( Nout <= N );
    // zero out
    memset( out, 0, sizeof(FLOAT)*Nout );

    // go for it
    for( k = 0; k < Nout; k++ )
    {
        for( n = 0; n < N; n++ )
        {
            out[k] += x[n] * cos( ONE_PI / N * k * (n + .5) );
        }
    }
}





//-----------------------------------------------------------------------------
// name: the_dct_matrix()
// desc: type ii dct matrx on N reals
//-----------------------------------------------------------------------------
void the_dct_matrix( FLOAT ** out, unsigned long N )
{
    unsigned long k, n;

    // go for it
    for( k = 0; k < N; k++ )
    {
        for( n = 0; n < N; n++ )
        {
            out[k][n] = cos( ONE_PI / N * k * (n + .5) );
        }
    }
}


//-----------------------------------------------------------------------------
// name: the_dct_now()
// desc: apply dct matrx on N reals
//-----------------------------------------------------------------------------
void the_dct_now( FLOAT * x, FLOAT ** matrix, unsigned long N,
                  FLOAT * out, unsigned long Nout )
{
    unsigned long k, n;

    // sanity check
    assert( Nout <= N );
    // zero out
    memset( out, 0, sizeof(FLOAT)*Nout );

    // go for it
    for( k = 0; k < Nout; k++ )
    {
        for( n = 0; n < N; n++ )
        {
            out[k] = x[n] * matrix[k][n];
        }
    }
}




//-----------------------------------------------------------------------------
// name: the_inverse_dct()
// desc: type iii dct, or inverse dct
//-----------------------------------------------------------------------------
void the_inverse_dct( FLOAT * x, unsigned long N, FLOAT * out, unsigned long Nout )
{
}


//-----------------------------------------------------------------------------
// name: the_inverse_dct_matrix()
// desc: generates the NxN DCT matrix
//-----------------------------------------------------------------------------
void the_inverse_dct_matrix( FLOAT ** out, unsigned long N )
{
    unsigned long k, n;

    // go for it
    for( k = 0; k < N; k++ )
    {
        for( n = 0; n < N; n++ )
        {
            out[k][n] = cos( ONE_PI / N * n * (k + .5) );
        }
    }
}




//-----------------------------------------------------------------------------
// name: the_inverse_dct_now()
// desc: apply inverse dct matrx on N reals
//-----------------------------------------------------------------------------
void the_inverse_dct_now( FLOAT * x, FLOAT ** matrix, unsigned long N,
                  FLOAT * out, unsigned long Nout )
{
    unsigned long k, n;

    // sanity check
    assert( Nout <= N );
    // zero out
    memset( out, 0, sizeof(FLOAT)*Nout );

    // go for it
    for( k = 0; k < Nout; k++ )
    {
        for( n = 0; n < N; n++ )
        {
            out[k] = x[0] / 2 + x[n] * matrix[k][n];
        }
    }
}
