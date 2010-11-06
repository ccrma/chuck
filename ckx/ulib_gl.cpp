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
// file: ulib_gl.cpp
// desc: ...
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Philip L. Davidson (philipd@alumni.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_gl.h"

#ifdef __MACOSX_CORE__
#include <OpenGL/gl.h>
#else 

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#endif


// lazy phil's preprocessor functions
// if these shouldn't be here, tell me
#define GL_CKADDEXPORT(t, n) QUERY->add_export( QUERY, #t, #n, gl_##n##_impl, TRUE )
#define GL_CKADDPARAM(t, n)  QUERY->add_param ( QUERY, #t, #n )
//define them variables!

/* Boolean values */
static uint gl_FALSE				 = 0x0;
static uint gl_TRUE					 = 0x1;

/* Primitives */
static uint gl_POINTS				 = 0x0000;
static uint gl_LINES				 = 0x0001;
static uint gl_LINE_LOOP				 = 0x0002;
static uint gl_LINE_STRIP				 = 0x0003;
static uint gl_TRIANGLES				 = 0x0004;
static uint gl_TRIANGLE_STRIP			 = 0x0005;
static uint gl_TRIANGLE_FAN				 = 0x0006;
static uint gl_QUADS				 = 0x0007;
static uint gl_QUAD_STRIP				 = 0x0008;
static uint gl_POLYGON				 = 0x0009;

/* Data types */
static uint gl_BYTE					 = 0x1400;
static uint gl_UNSIGNED_BYTE			 = 0x1401;
static uint gl_SHORT				 = 0x1402;
static uint gl_UNSIGNED_SHORT			 = 0x1403;
static uint gl_INT					 = 0x1404;
static uint gl_UNSIGNED_INT				 = 0x1405;
static uint gl_FLOAT				 = 0x1406;
static uint gl_DOUBLE				 = 0x140A;
static uint gl_2_BYTES				 = 0x1407;
static uint gl_3_BYTES				 = 0x1408;
static uint gl_4_BYTES				 = 0x1409;

/* Matrix Mode */
static uint gl_MATRIX_MODE				 = 0x0BA0;
static uint gl_MODELVIEW				 = 0x1700;
static uint gl_PROJECTION				 = 0x1701;
static uint gl_TEXTURE				 = 0x1702;

/* Points */
static uint gl_POINT_SMOOTH				 = 0x0B10;
static uint gl_POINT_SIZE				 = 0x0B11;
static uint gl_POINT_SIZE_GRANULARITY 		 = 0x0B13;
static uint gl_POINT_SIZE_RANGE			 = 0x0B12;

/* Lines */
static uint gl_LINE_SMOOTH				 = 0x0B20;
static uint gl_LINE_STIPPLE				 = 0x0B24;
static uint gl_LINE_STIPPLE_PATTERN			 = 0x0B25;
static uint gl_LINE_STIPPLE_REPEAT			 = 0x0B26;
static uint gl_LINE_WIDTH				 = 0x0B21;
static uint gl_LINE_WIDTH_GRANULARITY		 = 0x0B23;
static uint gl_LINE_WIDTH_RANGE			 = 0x0B22;

/* Polygons */
static uint gl_POINT				 = 0x1B00;
static uint gl_LINE					 = 0x1B01;
static uint gl_FILL					 = 0x1B02;
static uint gl_CW					 = 0x0900;
static uint gl_CCW					 = 0x0901;
static uint gl_FRONT				 = 0x0404;
static uint gl_BACK					 = 0x0405;
static uint gl_POLYGON_MODE				 = 0x0B40;
static uint gl_POLYGON_SMOOTH			 = 0x0B41;
static uint gl_POLYGON_STIPPLE			 = 0x0B42;
static uint gl_EDGE_FLAG				 = 0x0B43;
static uint gl_CULL_FACE				 = 0x0B44;
static uint gl_CULL_FACE_MODE			 = 0x0B45;
static uint gl_FRONT_FACE				 = 0x0B46;
static uint gl_POLYGON_OFFSET_FACTOR		 = 0x8038;
static uint gl_POLYGON_OFFSET_UNITS			 = 0x2A00;
static uint gl_POLYGON_OFFSET_POINT			 = 0x2A01;
static uint gl_POLYGON_OFFSET_LINE			 = 0x2A02;
static uint gl_POLYGON_OFFSET_FILL			 = 0x8037;

/* Display Lists */
static uint gl_COMPILE				 = 0x1300;
static uint gl_COMPILE_AND_EXECUTE			 = 0x1301;
static uint gl_LIST_BASE				 = 0x0B32;
static uint gl_LIST_INDEX				 = 0x0B33;
static uint gl_LIST_MODE				 = 0x0B30;

/* Depth buffer */
static uint gl_NEVER				 = 0x0200;
static uint gl_LESS					 = 0x0201;
static uint gl_EQUAL				 = 0x0202;
static uint gl_LEQUAL				 = 0x0203;
static uint gl_GREATER				 = 0x0204;
static uint gl_NOTEQUAL				 = 0x0205;
static uint gl_GEQUAL				 = 0x0206;
static uint gl_ALWAYS				 = 0x0207;
static uint gl_DEPTH_TEST				 = 0x0B71;
static uint gl_DEPTH_BITS				 = 0x0D56;
static uint gl_DEPTH_CLEAR_VALUE			 = 0x0B73;
static uint gl_DEPTH_FUNC				 = 0x0B74;
static uint gl_DEPTH_RANGE				 = 0x0B70;
static uint gl_DEPTH_WRITEMASK			 = 0x0B72;
static uint gl_DEPTH_COMPONENT			 = 0x1902;

/* Lighting */
static uint gl_LIGHTING				 = 0x0B50;
static uint gl_LIGHT0				 = 0x4000;
static uint gl_LIGHT1				 = 0x4001;
static uint gl_LIGHT2				 = 0x4002;
static uint gl_LIGHT3				 = 0x4003;
static uint gl_LIGHT4				 = 0x4004;
static uint gl_LIGHT5				 = 0x4005;
static uint gl_LIGHT6				 = 0x4006;
static uint gl_LIGHT7				 = 0x4007;
static uint gl_SPOT_EXPONENT			 = 0x1205;
static uint gl_SPOT_CUTOFF				 = 0x1206;
static uint gl_CONSTANT_ATTENUATION			 = 0x1207;
static uint gl_LINEAR_ATTENUATION			 = 0x1208;
static uint gl_QUADRATIC_ATTENUATION		 = 0x1209;
static uint gl_AMBIENT				 = 0x1200;
static uint gl_DIFFUSE				 = 0x1201;
static uint gl_SPECULAR				 = 0x1202;
static uint gl_SHININESS				 = 0x1601;
static uint gl_EMISSION				 = 0x1600;
static uint gl_POSITION				 = 0x1203;
static uint gl_SPOT_DIRECTION			 = 0x1204;
static uint gl_AMBIENT_AND_DIFFUSE			 = 0x1602;
static uint gl_COLOR_INDEXES			 = 0x1603;
static uint gl_LIGHT_MODEL_TWO_SIDE			 = 0x0B52;
static uint gl_LIGHT_MODEL_LOCAL_VIEWER		 = 0x0B51;
static uint gl_LIGHT_MODEL_AMBIENT			 = 0x0B53;
static uint gl_FRONT_AND_BACK			 = 0x0408;
static uint gl_SHADE_MODEL				 = 0x0B54;
static uint gl_FLAT					 = 0x1D00;
static uint gl_SMOOTH				 = 0x1D01;
static uint gl_COLOR_MATERIAL			 = 0x0B57;
static uint gl_COLOR_MATERIAL_FACE			 = 0x0B55;
static uint gl_COLOR_MATERIAL_PARAMETER		 = 0x0B56;
static uint gl_NORMALIZE				 = 0x0BA1;

/* Blending */
static uint gl_BLEND				 = 0x0BE2;
static uint gl_BLEND_SRC				 = 0x0BE1;
static uint gl_BLEND_DST				 = 0x0BE0;
static uint gl_ZERO					 = 0x0;
static uint gl_ONE					 = 0x1;
static uint gl_SRC_COLOR				 = 0x0300;
static uint gl_ONE_MINUS_SRC_COLOR			 = 0x0301;
static uint gl_SRC_ALPHA				 = 0x0302;
static uint gl_ONE_MINUS_SRC_ALPHA			 = 0x0303;
static uint gl_DST_ALPHA				 = 0x0304;
static uint gl_ONE_MINUS_DST_ALPHA			 = 0x0305;
static uint gl_DST_COLOR				 = 0x0306;
static uint gl_ONE_MINUS_DST_COLOR			 = 0x0307;
static uint gl_SRC_ALPHA_SATURATE			 = 0x0308;
static uint gl_CONSTANT_COLOR			 = 0x8001;
static uint gl_ONE_MINUS_CONSTANT_COLOR		 = 0x8002;
static uint gl_CONSTANT_ALPHA			 = 0x8003;
static uint gl_ONE_MINUS_CONSTANT_ALPHA		 = 0x8004;

/* Buffers, Pixel Drawing/Reading */
static uint gl_NONE					 = 0x0;
static uint gl_LEFT					 = 0x0406;
static uint gl_RIGHT				 = 0x0407;
static uint gl_FRONT_LEFT				 = 0x0400;
static uint gl_FRONT_RIGHT				 = 0x0401;
static uint gl_BACK_LEFT				 = 0x0402;
static uint gl_BACK_RIGHT				 = 0x0403;
static uint gl_AUX0					 = 0x0409;
static uint gl_AUX1					 = 0x040A;
static uint gl_AUX2					 = 0x040B;
static uint gl_AUX3					 = 0x040C;
static uint gl_COLOR_INDEX				 = 0x1900;
static uint gl_RED					 = 0x1903;
static uint gl_GREEN				 = 0x1904;
static uint gl_BLUE					 = 0x1905;
static uint gl_ALPHA				 = 0x1906;
static uint gl_LUMINANCE				 = 0x1909;
static uint gl_LUMINANCE_ALPHA			 = 0x190A;
static uint gl_ALPHA_BITS				 = 0x0D55;
static uint gl_RED_BITS				 = 0x0D52;
static uint gl_GREEN_BITS				 = 0x0D53;
static uint gl_BLUE_BITS				 = 0x0D54;
static uint gl_INDEX_BITS				 = 0x0D51;
static uint gl_SUBPIXEL_BITS			 = 0x0D50;
static uint gl_AUX_BUFFERS				 = 0x0C00;
static uint gl_READ_BUFFER				 = 0x0C02;
static uint gl_DRAW_BUFFER				 = 0x0C01;
static uint gl_DOUBLEBUFFER				 = 0x0C32;
static uint gl_STEREO				 = 0x0C33;
static uint gl_BITMAP				 = 0x1A00;
static uint gl_COLOR				 = 0x1800;
static uint gl_DEPTH				 = 0x1801;
static uint gl_STENCIL				 = 0x1802;
static uint gl_DITHER				 = 0x0BD0;
static uint gl_RGB					 = 0x1907;
static uint gl_RGBA					 = 0x1908;

/* Gets */
static uint gl_ATTRIB_STACK_DEPTH			 = 0x0BB0;
static uint gl_CLIENT_ATTRIB_STACK_DEPTH		 = 0x0BB1;
static uint gl_COLOR_CLEAR_VALUE			 = 0x0C22;
static uint gl_COLOR_WRITEMASK			 = 0x0C23;
static uint gl_CURRENT_INDEX			 = 0x0B01;
static uint gl_CURRENT_COLOR			 = 0x0B00;
static uint gl_CURRENT_NORMAL			 = 0x0B02;
static uint gl_CURRENT_RASTER_COLOR			 = 0x0B04;
static uint gl_CURRENT_RASTER_DISTANCE		 = 0x0B09;
static uint gl_CURRENT_RASTER_INDEX			 = 0x0B05;
static uint gl_CURRENT_RASTER_POSITION		 = 0x0B07;
static uint gl_CURRENT_RASTER_TEXTURE_COORDS	 = 0x0B06;
static uint gl_CURRENT_RASTER_POSITION_VALID	 = 0x0B08;
static uint gl_CURRENT_TEXTURE_COORDS		 = 0x0B03;
static uint gl_INDEX_CLEAR_VALUE			 = 0x0C20;
static uint gl_INDEX_MODE				 = 0x0C30;
static uint gl_INDEX_WRITEMASK			 = 0x0C21;
static uint gl_MODELVIEW_MATRIX			 = 0x0BA6;
static uint gl_MODELVIEW_STACK_DEPTH		 = 0x0BA3;
static uint gl_NAME_STACK_DEPTH			 = 0x0D70;
static uint gl_PROJECTION_MATRIX			 = 0x0BA7;
static uint gl_PROJECTION_STACK_DEPTH		 = 0x0BA4;
static uint gl_RENDER_MODE				 = 0x0C40;
static uint gl_RGBA_MODE				 = 0x0C31;
static uint gl_TEXTURE_MATRIX			 = 0x0BA8;
static uint gl_TEXTURE_STACK_DEPTH			 = 0x0BA5;
static uint gl_VIEWPORT				 = 0x0BA2;

/* Texture mapping */
static uint gl_TEXTURE_ENV				 = 0x2300;
static uint gl_TEXTURE_ENV_MODE			 = 0x2200;
static uint gl_TEXTURE_1D				 = 0x0DE0;
static uint gl_TEXTURE_2D				 = 0x0DE1;
static uint gl_TEXTURE_WRAP_S			 = 0x2802;
static uint gl_TEXTURE_WRAP_T			 = 0x2803;
static uint gl_TEXTURE_MAG_FILTER			 = 0x2800;
static uint gl_TEXTURE_MIN_FILTER			 = 0x2801;
static uint gl_TEXTURE_ENV_COLOR			 = 0x2201;
static uint gl_TEXTURE_GEN_S			 = 0x0C60;
static uint gl_TEXTURE_GEN_T			 = 0x0C61;
static uint gl_TEXTURE_GEN_MODE			 = 0x2500;
static uint gl_TEXTURE_BORDER_COLOR			 = 0x1004;
static uint gl_TEXTURE_WIDTH			 = 0x1000;
static uint gl_TEXTURE_HEIGHT			 = 0x1001;
static uint gl_TEXTURE_BORDER			 = 0x1005;
static uint gl_TEXTURE_COMPONENTS			 = 0x1003;
static uint gl_TEXTURE_RED_SIZE			 = 0x805C;
static uint gl_TEXTURE_GREEN_SIZE			 = 0x805D;
static uint gl_TEXTURE_BLUE_SIZE			 = 0x805E;
static uint gl_TEXTURE_ALPHA_SIZE			 = 0x805F;
static uint gl_TEXTURE_LUMINANCE_SIZE		 = 0x8060;
static uint gl_TEXTURE_INTENSITY_SIZE		 = 0x8061;
static uint gl_NEAREST_MIPMAP_NEAREST		 = 0x2700;
static uint gl_NEAREST_MIPMAP_LINEAR		 = 0x2702;
static uint gl_LINEAR_MIPMAP_NEAREST		 = 0x2701;
static uint gl_LINEAR_MIPMAP_LINEAR			 = 0x2703;
static uint gl_OBJECT_LINEAR			 = 0x2401;
static uint gl_OBJECT_PLANE				 = 0x2501;
static uint gl_EYE_LINEAR				 = 0x2400;
static uint gl_EYE_PLANE				 = 0x2502;
static uint gl_SPHERE_MAP				 = 0x2402;
static uint gl_DECAL				 = 0x2101;
static uint gl_MODULATE				 = 0x2100;
static uint gl_NEAREST				 = 0x2600;
static uint gl_REPEAT				 = 0x2901;
static uint gl_CLAMP				 = 0x2900;
static uint gl_S					 = 0x2000;
static uint gl_T					 = 0x2001;
static uint gl_R					 = 0x2002;
static uint gl_Q					 = 0x2003;
static uint gl_TEXTURE_GEN_R			 = 0x0C62;
static uint gl_TEXTURE_GEN_Q			 = 0x0C63;

/* glPush/PopAttrib bits */
static uint gl_CURRENT_BIT				 = 0x00000001;
static uint gl_POINT_BIT				 = 0x00000002;
static uint gl_LINE_BIT				 = 0x00000004;
static uint gl_POLYGON_BIT				 = 0x00000008;
static uint gl_POLYGON_STIPPLE_BIT			 = 0x00000010;
static uint gl_PIXEL_MODE_BIT			 = 0x00000020;
static uint gl_LIGHTING_BIT				 = 0x00000040;
static uint gl_FOG_BIT				 = 0x00000080;
static uint gl_DEPTH_BUFFER_BIT			 = 0x00000100;
static uint gl_ACCUM_BUFFER_BIT			 = 0x00000200;
static uint gl_STENCIL_BUFFER_BIT			 = 0x00000400;
static uint gl_VIEWPORT_BIT				 = 0x00000800;
static uint gl_TRANSFORM_BIT			 = 0x00001000;
static uint gl_ENABLE_BIT				 = 0x00002000;
static uint gl_COLOR_BUFFER_BIT			 = 0x00004000;
static uint gl_HINT_BIT				 = 0x00008000;
static uint gl_EVAL_BIT				 = 0x00010000;
static uint gl_LIST_BIT				 = 0x00020000;
static uint gl_TEXTURE_BIT				 = 0x00040000;
static uint gl_SCISSOR_BIT				 = 0x00080000;
static uint gl_ALL_ATTRIB_BITS			 = 0x000FFFFF; 

//-----------------------------------------------------------------------------
// name: gl_query()
// desc: query entry point
//-----------------------------------------------------------------------------
//DLL_QUERY gl_query( Chuck_DL_Query * QUERY )
CK_DLL_QUERY
{
    QUERY->set_name( QUERY, "gl" );
    
    //! \sectionMain Functions

    //! \section Drawing 
    GL_CKADDEXPORT ( int, Begin ); //! primitive constants are enumerated below
    GL_CKADDPARAM  ( uint, which );

    GL_CKADDEXPORT ( void, End );

    GL_CKADDEXPORT ( void, Color3f );
    GL_CKADDPARAM( float, r ); 
    GL_CKADDPARAM( float, g ); 
    GL_CKADDPARAM( float, b ); 

    GL_CKADDEXPORT ( void, Color4f );
    GL_CKADDPARAM( float, r ); 
    GL_CKADDPARAM( float, g ); 
    GL_CKADDPARAM( float, b ); 
    GL_CKADDPARAM( float, a ); 

    GL_CKADDEXPORT ( int, Normal3f );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );


    GL_CKADDEXPORT ( int, TexCoord1f );
    GL_CKADDPARAM ( float, u );

    GL_CKADDEXPORT ( int, TexCoord2f );
    GL_CKADDPARAM ( float, u );
    GL_CKADDPARAM ( float, v );


    GL_CKADDEXPORT ( int, Vertex3f );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );

    GL_CKADDEXPORT ( int, Vertex2f );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );


    //! \section Draw State


    GL_CKADDEXPORT ( void, Clear );
    GL_CKADDPARAM( uint, mask );  //do we have a bitfield op?

    GL_CKADDEXPORT ( void, ClearColor );
    GL_CKADDPARAM( float, r ); 
    GL_CKADDPARAM( float, g ); 
    GL_CKADDPARAM( float, b ); 
    GL_CKADDPARAM( float, a ); 

    GL_CKADDEXPORT ( int, FrontFace );
    GL_CKADDPARAM(uint, mode);

    GL_CKADDEXPORT ( int, LineWidth );
    GL_CKADDPARAM ( float, width );

    GL_CKADDEXPORT ( int, PointSize );
    GL_CKADDPARAM ( float, width );

    GL_CKADDEXPORT ( int, PolygonMode );
    GL_CKADDPARAM ( uint, face );
    GL_CKADDPARAM ( uint, mode );

    //! \section Attributes and State

    GL_CKADDEXPORT ( int, PushAttrib );
    GL_CKADDPARAM ( uint, which );

    GL_CKADDEXPORT ( int, PopAttrib );

    GL_CKADDEXPORT ( void, Disable );
    GL_CKADDPARAM( uint, cap ); 

    GL_CKADDEXPORT ( void, Enable );
    GL_CKADDPARAM( uint, cap ); 

    GL_CKADDEXPORT ( void, BlendFunc );
    GL_CKADDPARAM( uint, src ); 
    GL_CKADDPARAM( uint, dst ); 


    GL_CKADDEXPORT ( int, Flush );

    //let's do gl gets... but we need arrays, and then pointers to them!
    //GL_CKADDEXPORT ( int, GetBooleanv );
    GL_CKADDEXPORT( uint, GetError );


    //! \section Lighting 

    GL_CKADDEXPORT ( void, LightOn );
    GL_CKADDPARAM ( uint, light );

    GL_CKADDEXPORT ( void, LightOff );
    GL_CKADDPARAM ( uint, light );

    GL_CKADDEXPORT ( void, Lighti );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( int, param );

    GL_CKADDEXPORT ( void, Lightf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( float, param );

    GL_CKADDEXPORT ( void, Light3i );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( int, param1 );
    GL_CKADDPARAM ( int, param2 );
    GL_CKADDPARAM ( int, param3 );

    GL_CKADDEXPORT ( void, Light3f );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( float, param1 );
    GL_CKADDPARAM ( float, param2 );
    GL_CKADDPARAM ( float, param3 );

    GL_CKADDEXPORT ( void, Light4i );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( int, param1 );
    GL_CKADDPARAM ( int, param2 );
    GL_CKADDPARAM ( int, param3 );
    GL_CKADDPARAM ( int, param4 );

    GL_CKADDEXPORT ( void, Light4f );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( uint, pname );
    GL_CKADDPARAM ( float, param1 );
    GL_CKADDPARAM ( float, param2 );
    GL_CKADDPARAM ( float, param3 );
    GL_CKADDPARAM ( float, param4 );


    GL_CKADDEXPORT ( void, LightPosf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );
    GL_CKADDPARAM ( float, w );


    GL_CKADDEXPORT ( void, LightAmbientf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, r );
    GL_CKADDPARAM ( float, g );
    GL_CKADDPARAM ( float, b );
    GL_CKADDPARAM ( float, a );

    GL_CKADDEXPORT ( void, LightDiffusef );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, r );
    GL_CKADDPARAM ( float, g );
    GL_CKADDPARAM ( float, b );
    GL_CKADDPARAM ( float, a );

    GL_CKADDEXPORT ( void, LightSpecularf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, r );
    GL_CKADDPARAM ( float, g );
    GL_CKADDPARAM ( float, b );
    GL_CKADDPARAM ( float, a );

    GL_CKADDEXPORT ( void, LightSpotDirf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );

    GL_CKADDEXPORT ( void, LightSpotExpf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, f );

    GL_CKADDEXPORT ( void, LightSpotCutofff );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, f );

    GL_CKADDEXPORT ( void, LightAttenConstantf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, f );

    GL_CKADDEXPORT ( void, LightAttenLinearf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, f );

    GL_CKADDEXPORT ( void, LightAttenQuadf );
    GL_CKADDPARAM ( uint, light );
    GL_CKADDPARAM ( float, f );


    GL_CKADDEXPORT ( int, ShadeModel );
    GL_CKADDPARAM ( uint, mode );

    //! \section Matrices and Transformations

    GL_CKADDEXPORT ( int, MatrixMode );
    GL_CKADDPARAM ( uint, mode );

    GL_CKADDEXPORT ( int, LoadIdentity );

    GL_CKADDEXPORT ( int, PushMatrix );

    GL_CKADDEXPORT ( int, PopMatrix );

    //! \section View ( other functions available in glu )

    GL_CKADDEXPORT ( int, Frustum );
    GL_CKADDPARAM(float, left );
    GL_CKADDPARAM(float, right );
    GL_CKADDPARAM(float, bottom );
    GL_CKADDPARAM(float, top );
    GL_CKADDPARAM(float, znear );
    GL_CKADDPARAM(float, zfar );

    GL_CKADDEXPORT ( int, Ortho );
    GL_CKADDPARAM(float, left );
    GL_CKADDPARAM(float, right );
    GL_CKADDPARAM(float, bottom );
    GL_CKADDPARAM(float, top );
    GL_CKADDPARAM(float, znear );
    GL_CKADDPARAM(float, zfar );
    
    GL_CKADDEXPORT ( int, Viewport );
    GL_CKADDPARAM ( int, x );
    GL_CKADDPARAM ( int, y );
    GL_CKADDPARAM ( uint, width );
    GL_CKADDPARAM ( uint, height );

    //! \section Transforms

    GL_CKADDEXPORT ( int, Rotatef );
    GL_CKADDPARAM ( float, degrees );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );

    GL_CKADDEXPORT ( int, Scalef );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );

    GL_CKADDEXPORT ( int, Translatef );
    GL_CKADDPARAM ( float, x );
    GL_CKADDPARAM ( float, y );
    GL_CKADDPARAM ( float, z );

    //! \section Texture State

    GL_CKADDEXPORT ( int, BindTexture );
    GL_CKADDPARAM  ( uint, target ); 
    GL_CKADDPARAM  ( uint, texture ); 

    GL_CKADDEXPORT ( int, TexImage1D );
    GL_CKADDPARAM ( uint, target );
    GL_CKADDPARAM ( int, level );
    GL_CKADDPARAM ( int, internalformat );
    GL_CKADDPARAM ( uint, width );
    GL_CKADDPARAM ( int, border );
    GL_CKADDPARAM ( uint, format );
    GL_CKADDPARAM ( uint, type );
    GL_CKADDPARAM ( void, pixels ); // void*!!

    GL_CKADDEXPORT ( int, TexImage2D );
    GL_CKADDPARAM ( uint, target );
    GL_CKADDPARAM ( uint, level );
    GL_CKADDPARAM ( uint, internalformat );
    GL_CKADDPARAM ( uint, width );
    GL_CKADDPARAM ( uint, height );
    GL_CKADDPARAM ( uint, border );
    GL_CKADDPARAM ( uint, format );
    GL_CKADDPARAM ( uint, type );
    GL_CKADDPARAM ( void, pixels ); // void*!!


    //! \sectionMain Constants    
    //export!!!
/* Boolean values */
//! \section Booleans
	QUERY->add_export( QUERY, "uint", "FALSE", (f_ck_func)&gl_FALSE, FALSE );
	QUERY->add_export( QUERY, "uint", "TRUE", (f_ck_func)&gl_TRUE, FALSE );

/* Primitives */
//! \section Drawing Primitives
	QUERY->add_export( QUERY, "uint", "POINTS", (f_ck_func)&gl_POINTS, FALSE );
	QUERY->add_export( QUERY, "uint", "LINES", (f_ck_func)&gl_LINES, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_LOOP", (f_ck_func)&gl_LINE_LOOP, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_STRIP", (f_ck_func)&gl_LINE_STRIP, FALSE );
	QUERY->add_export( QUERY, "uint", "TRIANGLES", (f_ck_func)&gl_TRIANGLES, FALSE );
	QUERY->add_export( QUERY, "uint", "TRIANGLE_STRIP", (f_ck_func)&gl_TRIANGLE_STRIP, FALSE );
	QUERY->add_export( QUERY, "uint", "TRIANGLE_FAN", (f_ck_func)&gl_TRIANGLE_FAN, FALSE );
	QUERY->add_export( QUERY, "uint", "QUADS", (f_ck_func)&gl_QUADS, FALSE );
	QUERY->add_export( QUERY, "uint", "QUAD_STRIP", (f_ck_func)&gl_QUAD_STRIP, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON", (f_ck_func)&gl_POLYGON, FALSE );

/* Data types */
//! \section Data Types
	QUERY->add_export( QUERY, "uint", "BYTE", (f_ck_func)&gl_BYTE, FALSE );
	QUERY->add_export( QUERY, "uint", "UNSIGNED_BYTE", (f_ck_func)&gl_UNSIGNED_BYTE, FALSE );
	QUERY->add_export( QUERY, "uint", "SHORT", (f_ck_func)&gl_SHORT, FALSE );
	QUERY->add_export( QUERY, "uint", "UNSIGNED_SHORT", (f_ck_func)&gl_UNSIGNED_SHORT, FALSE );
	QUERY->add_export( QUERY, "uint", "INT", (f_ck_func)&gl_INT, FALSE );
	QUERY->add_export( QUERY, "uint", "UNSIGNED_INT", (f_ck_func)&gl_UNSIGNED_INT, FALSE );
	QUERY->add_export( QUERY, "uint", "FLOAT", (f_ck_func)&gl_FLOAT, FALSE );
	QUERY->add_export( QUERY, "uint", "DOUBLE", (f_ck_func)&gl_DOUBLE, FALSE );
	QUERY->add_export( QUERY, "uint", "2_BYTES", (f_ck_func)&gl_2_BYTES, FALSE );
	QUERY->add_export( QUERY, "uint", "3_BYTES", (f_ck_func)&gl_3_BYTES, FALSE );
	QUERY->add_export( QUERY, "uint", "4_BYTES", (f_ck_func)&gl_4_BYTES, FALSE );

/* Matrix Mode */
//! \section Matrix Mode
	QUERY->add_export( QUERY, "uint", "MATRIX_MODE", (f_ck_func)&gl_MATRIX_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "MODELVIEW", (f_ck_func)&gl_MODELVIEW, FALSE );
	QUERY->add_export( QUERY, "uint", "PROJECTION", (f_ck_func)&gl_PROJECTION, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE", (f_ck_func)&gl_TEXTURE, FALSE );

/* Points */
//! \section Points
	QUERY->add_export( QUERY, "uint", "POINT_SMOOTH", (f_ck_func)&gl_POINT_SMOOTH, FALSE );
	QUERY->add_export( QUERY, "uint", "POINT_SIZE", (f_ck_func)&gl_POINT_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "POINT_SIZE_GRANULARITY", (f_ck_func)&gl_POINT_SIZE_GRANULARITY, FALSE );
	QUERY->add_export( QUERY, "uint", "POINT_SIZE_RANGE", (f_ck_func)&gl_POINT_SIZE_RANGE, FALSE );

/* Lines */
//! \section Lines
	QUERY->add_export( QUERY, "uint", "LINE_SMOOTH", (f_ck_func)&gl_LINE_SMOOTH, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_STIPPLE", (f_ck_func)&gl_LINE_STIPPLE, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_STIPPLE_PATTERN", (f_ck_func)&gl_LINE_STIPPLE_PATTERN, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_STIPPLE_REPEAT", (f_ck_func)&gl_LINE_STIPPLE_REPEAT, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_WIDTH", (f_ck_func)&gl_LINE_WIDTH, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_WIDTH_GRANULARITY", (f_ck_func)&gl_LINE_WIDTH_GRANULARITY, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_WIDTH_RANGE", (f_ck_func)&gl_LINE_WIDTH_RANGE, FALSE );

/* Polygons */
//! \section Polygons
	QUERY->add_export( QUERY, "uint", "POINT", (f_ck_func)&gl_POINT, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE", (f_ck_func)&gl_LINE, FALSE );
	QUERY->add_export( QUERY, "uint", "FILL", (f_ck_func)&gl_FILL, FALSE );
	QUERY->add_export( QUERY, "uint", "CW", (f_ck_func)&gl_CW, FALSE );
	QUERY->add_export( QUERY, "uint", "CCW", (f_ck_func)&gl_CCW, FALSE );
	QUERY->add_export( QUERY, "uint", "FRONT", (f_ck_func)&gl_FRONT, FALSE );
	QUERY->add_export( QUERY, "uint", "BACK", (f_ck_func)&gl_BACK, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_MODE", (f_ck_func)&gl_POLYGON_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_SMOOTH", (f_ck_func)&gl_POLYGON_SMOOTH, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_STIPPLE", (f_ck_func)&gl_POLYGON_STIPPLE, FALSE );
	QUERY->add_export( QUERY, "uint", "EDGE_FLAG", (f_ck_func)&gl_EDGE_FLAG, FALSE );
	QUERY->add_export( QUERY, "uint", "CULL_FACE", (f_ck_func)&gl_CULL_FACE, FALSE );
	QUERY->add_export( QUERY, "uint", "CULL_FACE_MODE", (f_ck_func)&gl_CULL_FACE_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "FRONT_FACE", (f_ck_func)&gl_FRONT_FACE, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_OFFSET_FACTOR", (f_ck_func)&gl_POLYGON_OFFSET_FACTOR, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_OFFSET_UNITS", (f_ck_func)&gl_POLYGON_OFFSET_UNITS, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_OFFSET_POINT", (f_ck_func)&gl_POLYGON_OFFSET_POINT, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_OFFSET_LINE", (f_ck_func)&gl_POLYGON_OFFSET_LINE, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_OFFSET_FILL", (f_ck_func)&gl_POLYGON_OFFSET_FILL, FALSE );

/* Display Lists */
//! \section Display Lists
	QUERY->add_export( QUERY, "uint", "COMPILE", (f_ck_func)&gl_COMPILE, FALSE );
	QUERY->add_export( QUERY, "uint", "COMPILE_AND_EXECUTE", (f_ck_func)&gl_COMPILE_AND_EXECUTE, FALSE );
	QUERY->add_export( QUERY, "uint", "LIST_BASE", (f_ck_func)&gl_LIST_BASE, FALSE );
	QUERY->add_export( QUERY, "uint", "LIST_INDEX", (f_ck_func)&gl_LIST_INDEX, FALSE );
	QUERY->add_export( QUERY, "uint", "LIST_MODE", (f_ck_func)&gl_LIST_MODE, FALSE );

/* Depth buffer */
//! \section Depth Buffer
	QUERY->add_export( QUERY, "uint", "NEVER", (f_ck_func)&gl_NEVER, FALSE );
	QUERY->add_export( QUERY, "uint", "LESS", (f_ck_func)&gl_LESS, FALSE );
	QUERY->add_export( QUERY, "uint", "EQUAL", (f_ck_func)&gl_EQUAL, FALSE );
	QUERY->add_export( QUERY, "uint", "LEQUAL", (f_ck_func)&gl_LEQUAL, FALSE );
	QUERY->add_export( QUERY, "uint", "GREATER", (f_ck_func)&gl_GREATER, FALSE );
	QUERY->add_export( QUERY, "uint", "NOTEQUAL", (f_ck_func)&gl_NOTEQUAL, FALSE );
	QUERY->add_export( QUERY, "uint", "GEQUAL", (f_ck_func)&gl_GEQUAL, FALSE );
	QUERY->add_export( QUERY, "uint", "ALWAYS", (f_ck_func)&gl_ALWAYS, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_TEST", (f_ck_func)&gl_DEPTH_TEST, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_BITS", (f_ck_func)&gl_DEPTH_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_CLEAR_VALUE", (f_ck_func)&gl_DEPTH_CLEAR_VALUE, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_FUNC", (f_ck_func)&gl_DEPTH_FUNC, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_RANGE", (f_ck_func)&gl_DEPTH_RANGE, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_WRITEMASK", (f_ck_func)&gl_DEPTH_WRITEMASK, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_COMPONENT", (f_ck_func)&gl_DEPTH_COMPONENT, FALSE );

/* Lighting */
//! \section Lighting
	QUERY->add_export( QUERY, "uint", "LIGHTING", (f_ck_func)&gl_LIGHTING, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT0", (f_ck_func)&gl_LIGHT0, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT1", (f_ck_func)&gl_LIGHT1, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT2", (f_ck_func)&gl_LIGHT2, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT3", (f_ck_func)&gl_LIGHT3, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT4", (f_ck_func)&gl_LIGHT4, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT5", (f_ck_func)&gl_LIGHT5, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT6", (f_ck_func)&gl_LIGHT6, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT7", (f_ck_func)&gl_LIGHT7, FALSE );
	QUERY->add_export( QUERY, "uint", "SPOT_EXPONENT", (f_ck_func)&gl_SPOT_EXPONENT, FALSE );
	QUERY->add_export( QUERY, "uint", "SPOT_CUTOFF", (f_ck_func)&gl_SPOT_CUTOFF, FALSE );
	QUERY->add_export( QUERY, "uint", "CONSTANT_ATTENUATION", (f_ck_func)&gl_CONSTANT_ATTENUATION, FALSE );
	QUERY->add_export( QUERY, "uint", "LINEAR_ATTENUATION", (f_ck_func)&gl_LINEAR_ATTENUATION, FALSE );
	QUERY->add_export( QUERY, "uint", "QUADRATIC_ATTENUATION", (f_ck_func)&gl_QUADRATIC_ATTENUATION, FALSE );
	QUERY->add_export( QUERY, "uint", "AMBIENT", (f_ck_func)&gl_AMBIENT, FALSE );
	QUERY->add_export( QUERY, "uint", "DIFFUSE", (f_ck_func)&gl_DIFFUSE, FALSE );
	QUERY->add_export( QUERY, "uint", "SPECULAR", (f_ck_func)&gl_SPECULAR, FALSE );
	QUERY->add_export( QUERY, "uint", "SHININESS", (f_ck_func)&gl_SHININESS, FALSE );
	QUERY->add_export( QUERY, "uint", "EMISSION", (f_ck_func)&gl_EMISSION, FALSE );
	QUERY->add_export( QUERY, "uint", "POSITION", (f_ck_func)&gl_POSITION, FALSE );
	QUERY->add_export( QUERY, "uint", "SPOT_DIRECTION", (f_ck_func)&gl_SPOT_DIRECTION, FALSE );
	QUERY->add_export( QUERY, "uint", "AMBIENT_AND_DIFFUSE", (f_ck_func)&gl_AMBIENT_AND_DIFFUSE, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_INDEXES", (f_ck_func)&gl_COLOR_INDEXES, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT_MODEL_TWO_SIDE", (f_ck_func)&gl_LIGHT_MODEL_TWO_SIDE, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT_MODEL_LOCAL_VIEWER", (f_ck_func)&gl_LIGHT_MODEL_LOCAL_VIEWER, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHT_MODEL_AMBIENT", (f_ck_func)&gl_LIGHT_MODEL_AMBIENT, FALSE );
	QUERY->add_export( QUERY, "uint", "FRONT_AND_BACK", (f_ck_func)&gl_FRONT_AND_BACK, FALSE );
	QUERY->add_export( QUERY, "uint", "SHADE_MODEL", (f_ck_func)&gl_SHADE_MODEL, FALSE );
	QUERY->add_export( QUERY, "uint", "FLAT", (f_ck_func)&gl_FLAT, FALSE );
	QUERY->add_export( QUERY, "uint", "SMOOTH", (f_ck_func)&gl_SMOOTH, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_MATERIAL", (f_ck_func)&gl_COLOR_MATERIAL, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_MATERIAL_FACE", (f_ck_func)&gl_COLOR_MATERIAL_FACE, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_MATERIAL_PARAMETER", (f_ck_func)&gl_COLOR_MATERIAL_PARAMETER, FALSE );
	QUERY->add_export( QUERY, "uint", "NORMALIZE", (f_ck_func)&gl_NORMALIZE, FALSE );

/* Blending */
//! \section Blending
	QUERY->add_export( QUERY, "uint", "BLEND", (f_ck_func)&gl_BLEND, FALSE );
	QUERY->add_export( QUERY, "uint", "BLEND_SRC", (f_ck_func)&gl_BLEND_SRC, FALSE );
	QUERY->add_export( QUERY, "uint", "BLEND_DST", (f_ck_func)&gl_BLEND_DST, FALSE );
	QUERY->add_export( QUERY, "uint", "ZERO", (f_ck_func)&gl_ZERO, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE", (f_ck_func)&gl_ONE, FALSE );
	QUERY->add_export( QUERY, "uint", "SRC_COLOR", (f_ck_func)&gl_SRC_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_SRC_COLOR", (f_ck_func)&gl_ONE_MINUS_SRC_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "SRC_ALPHA", (f_ck_func)&gl_SRC_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_SRC_ALPHA", (f_ck_func)&gl_ONE_MINUS_SRC_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "DST_ALPHA", (f_ck_func)&gl_DST_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_DST_ALPHA", (f_ck_func)&gl_ONE_MINUS_DST_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "DST_COLOR", (f_ck_func)&gl_DST_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_DST_COLOR", (f_ck_func)&gl_ONE_MINUS_DST_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "SRC_ALPHA_SATURATE", (f_ck_func)&gl_SRC_ALPHA_SATURATE, FALSE );
	QUERY->add_export( QUERY, "uint", "CONSTANT_COLOR", (f_ck_func)&gl_CONSTANT_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_CONSTANT_COLOR", (f_ck_func)&gl_ONE_MINUS_CONSTANT_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "CONSTANT_ALPHA", (f_ck_func)&gl_CONSTANT_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "ONE_MINUS_CONSTANT_ALPHA", (f_ck_func)&gl_ONE_MINUS_CONSTANT_ALPHA, FALSE );

/* Buffers, Pixel Drawing/Reading */
//! \section Buffers, Pixel Draw/Fetch
	QUERY->add_export( QUERY, "uint", "NONE", (f_ck_func)&gl_NONE, FALSE );
	QUERY->add_export( QUERY, "uint", "LEFT", (f_ck_func)&gl_LEFT, FALSE );
	QUERY->add_export( QUERY, "uint", "RIGHT", (f_ck_func)&gl_RIGHT, FALSE );
	QUERY->add_export( QUERY, "uint", "FRONT_LEFT", (f_ck_func)&gl_FRONT_LEFT, FALSE );
	QUERY->add_export( QUERY, "uint", "FRONT_RIGHT", (f_ck_func)&gl_FRONT_RIGHT, FALSE );
	QUERY->add_export( QUERY, "uint", "BACK_LEFT", (f_ck_func)&gl_BACK_LEFT, FALSE );
	QUERY->add_export( QUERY, "uint", "BACK_RIGHT", (f_ck_func)&gl_BACK_RIGHT, FALSE );
	QUERY->add_export( QUERY, "uint", "AUX0", (f_ck_func)&gl_AUX0, FALSE );
	QUERY->add_export( QUERY, "uint", "AUX1", (f_ck_func)&gl_AUX1, FALSE );
	QUERY->add_export( QUERY, "uint", "AUX2", (f_ck_func)&gl_AUX2, FALSE );
	QUERY->add_export( QUERY, "uint", "AUX3", (f_ck_func)&gl_AUX3, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_INDEX", (f_ck_func)&gl_COLOR_INDEX, FALSE );
	QUERY->add_export( QUERY, "uint", "RED", (f_ck_func)&gl_RED, FALSE );
	QUERY->add_export( QUERY, "uint", "GREEN", (f_ck_func)&gl_GREEN, FALSE );
	QUERY->add_export( QUERY, "uint", "BLUE", (f_ck_func)&gl_BLUE, FALSE );
	QUERY->add_export( QUERY, "uint", "ALPHA", (f_ck_func)&gl_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "LUMINANCE", (f_ck_func)&gl_LUMINANCE, FALSE );
	QUERY->add_export( QUERY, "uint", "LUMINANCE_ALPHA", (f_ck_func)&gl_LUMINANCE_ALPHA, FALSE );
	QUERY->add_export( QUERY, "uint", "ALPHA_BITS", (f_ck_func)&gl_ALPHA_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "RED_BITS", (f_ck_func)&gl_RED_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "GREEN_BITS", (f_ck_func)&gl_GREEN_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "BLUE_BITS", (f_ck_func)&gl_BLUE_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "INDEX_BITS", (f_ck_func)&gl_INDEX_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "SUBPIXEL_BITS", (f_ck_func)&gl_SUBPIXEL_BITS, FALSE );
	QUERY->add_export( QUERY, "uint", "AUX_BUFFERS", (f_ck_func)&gl_AUX_BUFFERS, FALSE );
	QUERY->add_export( QUERY, "uint", "READ_BUFFER", (f_ck_func)&gl_READ_BUFFER, FALSE );
	QUERY->add_export( QUERY, "uint", "DRAW_BUFFER", (f_ck_func)&gl_DRAW_BUFFER, FALSE );
	QUERY->add_export( QUERY, "uint", "DOUBLEBUFFER", (f_ck_func)&gl_DOUBLEBUFFER, FALSE );
	QUERY->add_export( QUERY, "uint", "STEREO", (f_ck_func)&gl_STEREO, FALSE );
	QUERY->add_export( QUERY, "uint", "BITMAP", (f_ck_func)&gl_BITMAP, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR", (f_ck_func)&gl_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH", (f_ck_func)&gl_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "STENCIL", (f_ck_func)&gl_STENCIL, FALSE );
	QUERY->add_export( QUERY, "uint", "DITHER", (f_ck_func)&gl_DITHER, FALSE );
	QUERY->add_export( QUERY, "uint", "RGB", (f_ck_func)&gl_RGB, FALSE );
	QUERY->add_export( QUERY, "uint", "RGBA", (f_ck_func)&gl_RGBA, FALSE );

/* Gets */
//! \section Get Values
	QUERY->add_export( QUERY, "uint", "ATTRIB_STACK_DEPTH", (f_ck_func)&gl_ATTRIB_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "CLIENT_ATTRIB_STACK_DEPTH", (f_ck_func)&gl_CLIENT_ATTRIB_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_CLEAR_VALUE", (f_ck_func)&gl_COLOR_CLEAR_VALUE, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_WRITEMASK", (f_ck_func)&gl_COLOR_WRITEMASK, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_INDEX", (f_ck_func)&gl_CURRENT_INDEX, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_COLOR", (f_ck_func)&gl_CURRENT_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_NORMAL", (f_ck_func)&gl_CURRENT_NORMAL, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_COLOR", (f_ck_func)&gl_CURRENT_RASTER_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_DISTANCE", (f_ck_func)&gl_CURRENT_RASTER_DISTANCE, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_INDEX", (f_ck_func)&gl_CURRENT_RASTER_INDEX, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_POSITION", (f_ck_func)&gl_CURRENT_RASTER_POSITION, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_TEXTURE_COORDS", (f_ck_func)&gl_CURRENT_RASTER_TEXTURE_COORDS, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_RASTER_POSITION_VALID", (f_ck_func)&gl_CURRENT_RASTER_POSITION_VALID, FALSE );
	QUERY->add_export( QUERY, "uint", "CURRENT_TEXTURE_COORDS", (f_ck_func)&gl_CURRENT_TEXTURE_COORDS, FALSE );
	QUERY->add_export( QUERY, "uint", "INDEX_CLEAR_VALUE", (f_ck_func)&gl_INDEX_CLEAR_VALUE, FALSE );
	QUERY->add_export( QUERY, "uint", "INDEX_MODE", (f_ck_func)&gl_INDEX_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "INDEX_WRITEMASK", (f_ck_func)&gl_INDEX_WRITEMASK, FALSE );
	QUERY->add_export( QUERY, "uint", "MODELVIEW_MATRIX", (f_ck_func)&gl_MODELVIEW_MATRIX, FALSE );
	QUERY->add_export( QUERY, "uint", "MODELVIEW_STACK_DEPTH", (f_ck_func)&gl_MODELVIEW_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "NAME_STACK_DEPTH", (f_ck_func)&gl_NAME_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "PROJECTION_MATRIX", (f_ck_func)&gl_PROJECTION_MATRIX, FALSE );
	QUERY->add_export( QUERY, "uint", "PROJECTION_STACK_DEPTH", (f_ck_func)&gl_PROJECTION_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "RENDER_MODE", (f_ck_func)&gl_RENDER_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "RGBA_MODE", (f_ck_func)&gl_RGBA_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_MATRIX", (f_ck_func)&gl_TEXTURE_MATRIX, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_STACK_DEPTH", (f_ck_func)&gl_TEXTURE_STACK_DEPTH, FALSE );
	QUERY->add_export( QUERY, "uint", "VIEWPORT", (f_ck_func)&gl_VIEWPORT, FALSE );

/* Texture mapping */
//! \section Texture Mapping
	QUERY->add_export( QUERY, "uint", "TEXTURE_ENV", (f_ck_func)&gl_TEXTURE_ENV, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_ENV_MODE", (f_ck_func)&gl_TEXTURE_ENV_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_1D", (f_ck_func)&gl_TEXTURE_1D, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_2D", (f_ck_func)&gl_TEXTURE_2D, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_WRAP_S", (f_ck_func)&gl_TEXTURE_WRAP_S, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_WRAP_T", (f_ck_func)&gl_TEXTURE_WRAP_T, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_MAG_FILTER", (f_ck_func)&gl_TEXTURE_MAG_FILTER, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_MIN_FILTER", (f_ck_func)&gl_TEXTURE_MIN_FILTER, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_ENV_COLOR", (f_ck_func)&gl_TEXTURE_ENV_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GEN_S", (f_ck_func)&gl_TEXTURE_GEN_S, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GEN_T", (f_ck_func)&gl_TEXTURE_GEN_T, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GEN_MODE", (f_ck_func)&gl_TEXTURE_GEN_MODE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_BORDER_COLOR", (f_ck_func)&gl_TEXTURE_BORDER_COLOR, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_WIDTH", (f_ck_func)&gl_TEXTURE_WIDTH, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_HEIGHT", (f_ck_func)&gl_TEXTURE_HEIGHT, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_BORDER", (f_ck_func)&gl_TEXTURE_BORDER, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_COMPONENTS", (f_ck_func)&gl_TEXTURE_COMPONENTS, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_RED_SIZE", (f_ck_func)&gl_TEXTURE_RED_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GREEN_SIZE", (f_ck_func)&gl_TEXTURE_GREEN_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_BLUE_SIZE", (f_ck_func)&gl_TEXTURE_BLUE_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_ALPHA_SIZE", (f_ck_func)&gl_TEXTURE_ALPHA_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_LUMINANCE_SIZE", (f_ck_func)&gl_TEXTURE_LUMINANCE_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_INTENSITY_SIZE", (f_ck_func)&gl_TEXTURE_INTENSITY_SIZE, FALSE );
	QUERY->add_export( QUERY, "uint", "NEAREST_MIPMAP_NEAREST", (f_ck_func)&gl_NEAREST_MIPMAP_NEAREST, FALSE );
	QUERY->add_export( QUERY, "uint", "NEAREST_MIPMAP_LINEAR", (f_ck_func)&gl_NEAREST_MIPMAP_LINEAR, FALSE );
	QUERY->add_export( QUERY, "uint", "LINEAR_MIPMAP_NEAREST", (f_ck_func)&gl_LINEAR_MIPMAP_NEAREST, FALSE );
	QUERY->add_export( QUERY, "uint", "LINEAR_MIPMAP_LINEAR", (f_ck_func)&gl_LINEAR_MIPMAP_LINEAR, FALSE );
	QUERY->add_export( QUERY, "uint", "OBJECT_LINEAR", (f_ck_func)&gl_OBJECT_LINEAR, FALSE );
	QUERY->add_export( QUERY, "uint", "OBJECT_PLANE", (f_ck_func)&gl_OBJECT_PLANE, FALSE );
	QUERY->add_export( QUERY, "uint", "EYE_LINEAR", (f_ck_func)&gl_EYE_LINEAR, FALSE );
	QUERY->add_export( QUERY, "uint", "EYE_PLANE", (f_ck_func)&gl_EYE_PLANE, FALSE );
	QUERY->add_export( QUERY, "uint", "SPHERE_MAP", (f_ck_func)&gl_SPHERE_MAP, FALSE );
	QUERY->add_export( QUERY, "uint", "DECAL", (f_ck_func)&gl_DECAL, FALSE );
	QUERY->add_export( QUERY, "uint", "MODULATE", (f_ck_func)&gl_MODULATE, FALSE );
	QUERY->add_export( QUERY, "uint", "NEAREST", (f_ck_func)&gl_NEAREST, FALSE );
	QUERY->add_export( QUERY, "uint", "REPEAT", (f_ck_func)&gl_REPEAT, FALSE );
	QUERY->add_export( QUERY, "uint", "CLAMP", (f_ck_func)&gl_CLAMP, FALSE );
	QUERY->add_export( QUERY, "uint", "S", (f_ck_func)&gl_S, FALSE );
	QUERY->add_export( QUERY, "uint", "T", (f_ck_func)&gl_T, FALSE );
	QUERY->add_export( QUERY, "uint", "R", (f_ck_func)&gl_R, FALSE );
	QUERY->add_export( QUERY, "uint", "Q", (f_ck_func)&gl_Q, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GEN_R", (f_ck_func)&gl_TEXTURE_GEN_R, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_GEN_Q", (f_ck_func)&gl_TEXTURE_GEN_Q, FALSE );

/* glPush/PopAttrib bits */
//! \section Attributes
	QUERY->add_export( QUERY, "uint", "CURRENT_BIT", (f_ck_func)&gl_CURRENT_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "POINT_BIT", (f_ck_func)&gl_POINT_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "LINE_BIT", (f_ck_func)&gl_LINE_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_BIT", (f_ck_func)&gl_POLYGON_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "POLYGON_STIPPLE_BIT", (f_ck_func)&gl_POLYGON_STIPPLE_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "PIXEL_MODE_BIT", (f_ck_func)&gl_PIXEL_MODE_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "LIGHTING_BIT", (f_ck_func)&gl_LIGHTING_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "FOG_BIT", (f_ck_func)&gl_FOG_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "DEPTH_BUFFER_BIT", (f_ck_func)&gl_DEPTH_BUFFER_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "ACCUM_BUFFER_BIT", (f_ck_func)&gl_ACCUM_BUFFER_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "STENCIL_BUFFER_BIT", (f_ck_func)&gl_STENCIL_BUFFER_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "VIEWPORT_BIT", (f_ck_func)&gl_VIEWPORT_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "TRANSFORM_BIT", (f_ck_func)&gl_TRANSFORM_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "ENABLE_BIT", (f_ck_func)&gl_ENABLE_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "COLOR_BUFFER_BIT", (f_ck_func)&gl_COLOR_BUFFER_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "HINT_BIT", (f_ck_func)&gl_HINT_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "EVAL_BIT", (f_ck_func)&gl_EVAL_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "LIST_BIT", (f_ck_func)&gl_LIST_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "TEXTURE_BIT", (f_ck_func)&gl_TEXTURE_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "SCISSOR_BIT", (f_ck_func)&gl_SCISSOR_BIT, FALSE );
	QUERY->add_export( QUERY, "uint", "ALL_ATTRIB_BITS", (f_ck_func)&gl_ALL_ATTRIB_BITS, FALSE );


    return TRUE;
}


// Begin-->glBegin();
CK_DLL_FUNC( gl_Begin_impl )
{
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glBegin( mode );
    RETURN->v_int = 0;
}

// BindTexture-->glBindTexture();
CK_DLL_FUNC( gl_BindTexture_impl )
{
    t_CKUINT target  = GET_NEXT_UINT(ARGS);
    t_CKUINT texture = GET_NEXT_UINT(ARGS);
    glBindTexture( target, texture );
}

// Enable-->glEnable
CK_DLL_FUNC( gl_BlendFunc_impl )
{
    t_CKUINT src = GET_NEXT_UINT(ARGS);
    t_CKUINT dst = GET_NEXT_UINT(ARGS);
    glBlendFunc( src, dst );
}

// Clear-->glClear()
CK_DLL_FUNC( gl_Clear_impl )
{
    t_CKUINT v = GET_NEXT_UINT(ARGS);
    glClear( v );
}

// ClearColor-->glClearColor()
CK_DLL_FUNC( gl_ClearColor_impl )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT g = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT b = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT a = GET_NEXT_FLOAT(ARGS);
    glClearColor( r, g, b, a );
    RETURN->v_int = 0;
}

// Color3f->glColor3d()
CK_DLL_FUNC( gl_Color3f_impl )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT g = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT b = GET_NEXT_FLOAT(ARGS);

    glColor3d( r, g, b);
}

// Color4f->glColor4d()
CK_DLL_FUNC( gl_Color4f_impl )
{
    t_CKFLOAT r = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT g = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT b = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT a = GET_NEXT_FLOAT(ARGS);

    glColor4d( r, g, b, a);
}

// Disable-->glDisable()
CK_DLL_FUNC( gl_Disable_impl )
{
    t_CKUINT v = GET_NEXT_UINT(ARGS);
    glDisable( v );
}

// Enable-->glEnable
CK_DLL_FUNC( gl_Enable_impl )
{
    t_CKUINT which = GET_NEXT_UINT(ARGS);
    glEnable( which );
}

// End->glEnd()
CK_DLL_FUNC( gl_End_impl )
{
    glEnd();
}

// etc....
CK_DLL_FUNC( gl_Flush_impl )
{
    glFlush();
}

CK_DLL_FUNC( gl_FrontFace_impl )
{
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glFrontFace(mode);
}

CK_DLL_FUNC( gl_Frustum_impl )
{
    t_CKFLOAT left = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT right = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT bottom = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT top = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT znear = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT zfar = GET_NEXT_FLOAT(ARGS);
    glFrustum(left,right,bottom,top,znear,zfar);
}


CK_DLL_FUNC( gl_GenLists_impl )
{
    int n = GET_NEXT_INT(ARGS);
    RETURN->v_int = glGenLists(n);
}

CK_DLL_FUNC( gl_NewList_impl )
{
    int list = GET_NEXT_INT(ARGS);
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glNewList( list, mode );
}

CK_DLL_FUNC( gl_EndList_impl )
{
    glEndList();
}

//Textures?


//Lighting
CK_DLL_FUNC( gl_LightOn_impl ) {  
  uint light = GET_NEXT_INT(ARGS);
  glEnable(GL_LIGHT0 + light );
}

CK_DLL_FUNC( gl_LightOff_impl ) {  
  uint light = GET_NEXT_INT(ARGS);
  glDisable(GL_LIGHT0 + light );
}

CK_DLL_FUNC( gl_Lighti_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    int n = GET_NEXT_INT(ARGS);
    glLighti( light, pname, n );
}

CK_DLL_FUNC( gl_Lightf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    glLightf( light, pname, v );
}
CK_DLL_FUNC( gl_Light3i_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    GLint n[3];
    n[0] = GET_NEXT_INT(ARGS);
    n[1] = GET_NEXT_INT(ARGS);
    n[2] = GET_NEXT_INT(ARGS);
    glLightiv(light, pname, (GLint*)n );
}
CK_DLL_FUNC( gl_Light3f_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    GLfloat v[3];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, pname, (GLfloat *)v );
}

CK_DLL_FUNC( gl_Light4i_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    GLint n[4];
    n[0] = GET_NEXT_INT(ARGS);
    n[1] = GET_NEXT_INT(ARGS);
    n[2] = GET_NEXT_INT(ARGS);
    n[3] = GET_NEXT_INT(ARGS);
    glLightiv(light, pname, (GLint*)n );
}

CK_DLL_FUNC( gl_Light4f_impl ){
    uint light = GET_NEXT_INT(ARGS);
    uint pname = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    v[3] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, pname, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightPosf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    v[3] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_POSITION, (GLfloat *)v );
}
CK_DLL_FUNC( gl_LightAmbientf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    v[3] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_AMBIENT, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightDiffusef_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    v[3] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_DIFFUSE, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightSpecularf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    v[3] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPECULAR, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightSpotDirf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[4];
    v[0] = GET_NEXT_FLOAT(ARGS);
    v[1] = GET_NEXT_FLOAT(ARGS);
    v[2] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_DIRECTION, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightSpotExpf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[1];
    v[0] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_EXPONENT, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightSpotCutofff_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[1];
    v[0] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_EXPONENT, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightAttenConstantf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[1];
    v[0] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_EXPONENT, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightAttenLinearf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[1];
    v[0] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_EXPONENT, (GLfloat *)v );
}

CK_DLL_FUNC( gl_LightAttenQuadf_impl ){
    uint light = GET_NEXT_INT(ARGS);
    GLfloat v[1];
    v[0] = GET_NEXT_FLOAT(ARGS);
    glLightfv(light, GL_SPOT_EXPONENT, (GLfloat *)v );
}



CK_DLL_FUNC( gl_GetError_impl )
{
    RETURN->v_uint = (uint) glGetError();
}

CK_DLL_FUNC( gl_LineWidth_impl ) {  
    t_CKFLOAT width = GET_NEXT_FLOAT(ARGS);
    glLineWidth(width);
}

CK_DLL_FUNC( gl_PointSize_impl ) {  
    t_CKFLOAT size = GET_NEXT_FLOAT(ARGS);
    glPointSize(size);
}

CK_DLL_FUNC( gl_LoadIdentity_impl ) {   
    glLoadIdentity();
}

CK_DLL_FUNC( gl_MatrixMode_impl ) {   
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glMatrixMode(mode);
}

// Normal3f --> glNormal3d
CK_DLL_FUNC( gl_Normal3f_impl ) {   
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    glNormal3d(x,y,z);
}

CK_DLL_FUNC( gl_Ortho_impl )
{
    t_CKFLOAT left = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT right = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT bottom = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT top = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT znear = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT zfar = GET_NEXT_FLOAT(ARGS);
    glOrtho(left,right,bottom,top,znear,zfar);
}

CK_DLL_FUNC( gl_PushAttrib_impl ) {   
  t_CKUINT mask = GET_NEXT_UINT(ARGS);
    glPushAttrib(mask);
}

CK_DLL_FUNC( gl_PopAttrib_impl ) {   
    glPopAttrib();
}

CK_DLL_FUNC( gl_PushMatrix_impl ) {   
    glPushMatrix();
}

CK_DLL_FUNC( gl_PopMatrix_impl ) {   
    glPopMatrix();
}

CK_DLL_FUNC( gl_PolygonMode_impl ) {   
    t_CKUINT face = GET_NEXT_UINT(ARGS);
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glPolygonMode(face, mode);
}

// Rotatef --> glRotated()
CK_DLL_FUNC( gl_Rotatef_impl ) {   
    t_CKFLOAT degrees = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    glRotated(degrees, x, y, z);
}

// Scalef --> glScaled();
CK_DLL_FUNC( gl_Scalef_impl ) {   
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    glScaled(x,y,z);
}

CK_DLL_FUNC( gl_ShadeModel_impl ) {   
    t_CKUINT mode = GET_NEXT_UINT(ARGS);
    glShadeModel(mode);
}

// TexCoord1f --> glTexCoord1d
CK_DLL_FUNC( gl_TexCoord1f_impl ) {   
    t_CKFLOAT u = GET_NEXT_FLOAT(ARGS);
    glTexCoord1d(u);
}

// TexCoord2f --> glTexCoord2d
CK_DLL_FUNC( gl_TexCoord2f_impl ) {   
    t_CKFLOAT u = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    glTexCoord2d(u,v);
}

CK_DLL_FUNC( gl_TexImage1D_impl ) {   

    t_CKUINT target = GET_NEXT_UINT(ARGS);
    int  level  = GET_NEXT_INT(ARGS);
    int  internalformat  = GET_NEXT_INT(ARGS);
    t_CKUINT width = GET_NEXT_UINT(ARGS);
    int  border = GET_NEXT_INT(ARGS);
    t_CKUINT format = GET_NEXT_UINT(ARGS);
    t_CKUINT type = GET_NEXT_UINT(ARGS);
    t_CKUINT pixu = GET_NEXT_UINT(ARGS);
    void* pixels = (void*)pixu;

    glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
}

CK_DLL_FUNC( gl_TexImage2D_impl ) {   

    t_CKUINT target = GET_NEXT_UINT(ARGS);
    int  level  = GET_NEXT_INT(ARGS);
    int  internalformat  = GET_NEXT_INT(ARGS);
    t_CKUINT width = GET_NEXT_UINT(ARGS);
    t_CKUINT height = GET_NEXT_UINT(ARGS);
    int  border = GET_NEXT_INT(ARGS);
    t_CKUINT format = GET_NEXT_UINT(ARGS);
    t_CKUINT type = GET_NEXT_UINT(ARGS);
    t_CKUINT pixu = GET_NEXT_UINT(ARGS);
    void* pixels = (void*)pixu;

    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

}

// Translatef --> glTranslated
CK_DLL_FUNC( gl_Translatef_impl ) {   
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    glTranslated(x,y,z);
}

CK_DLL_FUNC( gl_Vertex2f_impl ) {   
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    glVertex2d(x,y);
}

// Vertex3f --> glVertex3d()
CK_DLL_FUNC( gl_Vertex3f_impl ) {   
    t_CKFLOAT x = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT y = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT z = GET_NEXT_FLOAT(ARGS);
    glVertex3d(x,y,z);
}

// 
CK_DLL_FUNC( gl_Viewport_impl ) {   
    int  x = GET_NEXT_INT(ARGS);
    int  y = GET_NEXT_INT(ARGS);
    t_CKUINT width = GET_NEXT_UINT(ARGS);
    t_CKUINT height = GET_NEXT_UINT(ARGS);
    glViewport(x,y,width,height);
}










