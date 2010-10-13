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
// file: ulib_gl.h
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (phlipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#ifndef __ULIB_GL_H__
#define __ULIB_GL_H__

#include "chuck_dl.h"

#ifndef uint
#define uint unsigned int
#endif

// query
// DLL_QUERY gl_query( Chuck_DL_Query * QUERY );
CK_DLL_QUERY;

// exports
CK_DLL_FUNC( gl_Begin_impl );
CK_DLL_FUNC( gl_BindTexture_impl );
CK_DLL_FUNC( gl_Clear_impl );
CK_DLL_FUNC( gl_ClearColor_impl );
CK_DLL_FUNC( gl_Color3f_impl );
CK_DLL_FUNC( gl_Color4f_impl );
CK_DLL_FUNC( gl_Disable_impl );
CK_DLL_FUNC( gl_Enable_impl );
CK_DLL_FUNC( gl_End_impl );
CK_DLL_FUNC( gl_Flush_impl );
CK_DLL_FUNC( gl_FrontFace_impl );
CK_DLL_FUNC( gl_Frustum_impl );
CK_DLL_FUNC( gl_BlendFunc_impl );

//display lists

CK_DLL_FUNC( gl_GenLists_impl );
CK_DLL_FUNC( gl_NewList_impl );
CK_DLL_FUNC( gl_EndList_impl );

//gentextures

CK_DLL_FUNC( gl_BindTexture_impl );
CK_DLL_FUNC( gl_TexImage1D_impl );
CK_DLL_FUNC( gl_TexImage2D_impl );
CK_DLL_FUNC( gl_TexEnv_impl);


CK_DLL_FUNC( gl_GetError_impl );
CK_DLL_FUNC( gl_LineWidth_impl );
CK_DLL_FUNC( gl_PointSize_impl );

//lighting
CK_DLL_FUNC( gl_LightOn_impl );
CK_DLL_FUNC( gl_LightOff_impl );
CK_DLL_FUNC( gl_Lighti_impl );
CK_DLL_FUNC( gl_Lightf_impl );
CK_DLL_FUNC( gl_Light3i_impl );
CK_DLL_FUNC( gl_Light3f_impl );
CK_DLL_FUNC( gl_Light4i_impl );
CK_DLL_FUNC( gl_Light4f_impl );

CK_DLL_FUNC( gl_LightPosf_impl );
CK_DLL_FUNC( gl_LightAmbientf_impl );
CK_DLL_FUNC( gl_LightDiffusef_impl );
CK_DLL_FUNC( gl_LightSpecularf_impl );
CK_DLL_FUNC( gl_LightSpotDirf_impl );
CK_DLL_FUNC( gl_LightSpotExpf_impl );
CK_DLL_FUNC( gl_LightSpotCutofff_impl );
CK_DLL_FUNC( gl_LightAttenConstantf_impl );
CK_DLL_FUNC( gl_LightAttenLinearf_impl );
CK_DLL_FUNC( gl_LightAttenQuadf_impl );



CK_DLL_FUNC( gl_LoadIdentity_impl );
CK_DLL_FUNC( gl_MatrixMode_impl );
CK_DLL_FUNC( gl_Normal3f_impl );
CK_DLL_FUNC( gl_Ortho_impl );
CK_DLL_FUNC( gl_PointSize_impl );

CK_DLL_FUNC( gl_PushAttrib_impl );
CK_DLL_FUNC( gl_PopAttrib_impl );

CK_DLL_FUNC( gl_PushMatrix_impl );
CK_DLL_FUNC( gl_PopMatrix_impl );

CK_DLL_FUNC( gl_PolygonMode_impl );
CK_DLL_FUNC( gl_Rotatef_impl );
CK_DLL_FUNC( gl_Scalef_impl );
CK_DLL_FUNC( gl_ShadeModel_impl );
CK_DLL_FUNC( gl_TexCoord1f_impl );
CK_DLL_FUNC( gl_TexCoord2f_impl );
CK_DLL_FUNC( gl_TexImage1D_impl );
CK_DLL_FUNC( gl_TexImage2D_impl );
CK_DLL_FUNC( gl_Translatef_impl );
CK_DLL_FUNC( gl_Vertex2f_impl );
CK_DLL_FUNC( gl_Vertex3f_impl );
CK_DLL_FUNC( gl_Viewport_impl );


#endif
