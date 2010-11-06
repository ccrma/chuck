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
// file: ulib_glu.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_glu.h"


#ifdef __MACOSX_CORE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#define GLU_CKADDEXPORT(t, n) QUERY->add_export( QUERY, #t, #n, glu_##n##_impl, TRUE )
#define GLU_CKADDPARAM(t, n)  QUERY->add_param ( QUERY, #t, #n )

//-----------------------------------------------------------------------------
// name: glu_query()
// desc: query entry point
//-----------------------------------------------------------------------------
//DLL_QUERY glu_query( Chuck_DL_Query * QUERY )
CK_DLL_QUERY
{
    QUERY->set_name( QUERY, "glu" );
    
    // add LookAt
    GLU_CKADDEXPORT ( int, LookAt);
    GLU_CKADDPARAM ( float, eye_x );
    GLU_CKADDPARAM ( float, eye_y );
    GLU_CKADDPARAM ( float, eye_z );
    GLU_CKADDPARAM ( float, at_x );
    GLU_CKADDPARAM ( float, at_y );
    GLU_CKADDPARAM ( float, at_z );
    GLU_CKADDPARAM ( float, up_x );
    GLU_CKADDPARAM ( float, up_y );
    GLU_CKADDPARAM ( float, up_z );

    GLU_CKADDEXPORT ( int, Perspective );
    GLU_CKADDPARAM ( float, fovy );
    GLU_CKADDPARAM ( float, aspect );
    GLU_CKADDPARAM ( float, near );
    GLU_CKADDPARAM ( float, far );
    
    return TRUE;
}


// LookAt
CK_DLL_FUNC( glu_LookAt_impl )
{

  t_CKFLOAT eye_x = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT eye_y = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT eye_z = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT at_x = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT at_y = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT at_z = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT up_x = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT up_y = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT up_z = GET_NEXT_FLOAT(ARGS);

  gluLookAt(eye_x, eye_y, eye_z, at_x, at_y, at_z, up_x, up_y, up_z);

}

CK_DLL_FUNC( glu_Perspective_impl ) { 
  t_CKFLOAT fovy = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT aspect = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT znear = GET_NEXT_FLOAT(ARGS);
  t_CKFLOAT zfar = GET_NEXT_FLOAT(ARGS);

  gluPerspective (fovy, aspect, znear, zfar );
}
