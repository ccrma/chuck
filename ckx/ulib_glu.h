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
// file: ulib_glu.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __ULIB_GLU_H__
#define __ULIB_GLU_H__

#include "chuck_dl.h"

// query
//DLL_QUERY glu_query( Chuck_DL_Query * QUERY );
CK_DLL_QUERY;

// exports

//view matrices
CK_DLL_FUNC( glu_LookAt_impl );
CK_DLL_FUNC( glu_Perspective_impl );

//shapes

CK_DLL_FUNC( glu_NewQuadric_impl );
CK_DLL_FUNC( glu_DeleteQuadric_impl );
CK_DLL_FUNC( glu_Disk_impl );
CK_DLL_FUNC( glu_Cylinder_impl );
CK_DLL_FUNC( glu_PartialDisk_impl );
CK_DLL_FUNC( glu_Sphere_impl );


#endif
