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
// name: util_math.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip Davidson (philipd@alumni.princeton.edu)
//-----------------------------------------------------------------------------
#include "util_math.h"


// windows / visual c++
#ifdef __PLATFORM_WIN32__


//-----------------------------------------------------------------------------
// name: round()
// desc: ...
//-----------------------------------------------------------------------------
double round( double a )
{
    double off = a - (long)a;
    if( off > .5 ) return (double)(long)a + 1.0;
    return (double)(long)a;
}


//-----------------------------------------------------------------------------
// name: trunc()
// desc: ...
//-----------------------------------------------------------------------------
double trunc( double a )
{
    return (double)(long)a;
}


//-----------------------------------------------------------------------------
// name: remainder()
// desc: ...
//-----------------------------------------------------------------------------
double remainder( int a, int b )
{
    int div = a/b;
    return a - b*div;
}


#endif
