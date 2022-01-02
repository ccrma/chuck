/*----------------------------------------------------------------------------
  ChucK Concurrent, On-the-fly Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2004 Ge Wang and Perry R. Cook.  All rights reserved.
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
// name: chuck_lang.h
// desc: chuck class library base
//
// authors: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
//          Spencer Salazar (spencer@ccrma.stanford.edu)
//          Ananya Misra (amisra@cs.princeton.edu)
//          Andrew Schran (aschran@princeton.edu)
//    date: spring 2005
//-----------------------------------------------------------------------------
#ifndef __CHUCK_LANG_H__
#define __CHUCK_LANG_H__

#include "chuck_def.h"
#include "chuck_oo.h"
#include "chuck_dl.h"
#include <queue>


// forward reference
struct Chuck_UAna;

// query
DLL_QUERY lang_query( Chuck_DL_Query * QUERY );

// base class initialization
t_CKBOOL init_class_object( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_ugen( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_uana( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_blob( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_event( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_shred( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_string( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_array( Chuck_Env * env, Chuck_Type * type );
t_CKBOOL init_class_vec3( Chuck_Env * env, Chuck_Type * type ); // 1.3.5.3
t_CKBOOL init_class_vec4( Chuck_Env * env, Chuck_Type * type ); // 1.3.5.3





//-----------------------------------------------------------------------------
// object API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( object_ctor );
CK_DLL_DTOR( object_dtor );
CK_DLL_MFUN( object_equals );
CK_DLL_MFUN( object_getType );
CK_DLL_MFUN( object_hashCode );
CK_DLL_MFUN( object_toString );
CK_DLL_MFUN( object_dump );
CK_DLL_SFUN( object_help );


//-----------------------------------------------------------------------------
// ugen API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( ugen_ctor );
CK_DLL_DTOR( ugen_dtor );
CK_DLL_MFUN( ugen_op );
CK_DLL_MFUN( ugen_cget_op );
CK_DLL_MFUN( ugen_last );
CK_DLL_MFUN( ugen_cget_last );
CK_DLL_MFUN( ugen_next );
CK_DLL_MFUN( ugen_cget_next );
CK_DLL_MFUN( ugen_gain );
CK_DLL_MFUN( ugen_cget_gain );
CK_DLL_MFUN( ugen_numChannels );
CK_DLL_MFUN( ugen_cget_numChannels );
CK_DLL_MFUN( ugen_chan );
CK_DLL_MFUN( ugen_connected );
CK_DLL_MFUN( ugen_buffered );
CK_DLL_MFUN( ugen_cget_buffered );


//-----------------------------------------------------------------------------
// uana API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( uana_ctor );
CK_DLL_DTOR( uana_dtor );
CK_DLL_MFUN( uana_upchuck );
CK_DLL_MFUN( uana_blob );
CK_DLL_MFUN( uana_fvals );
CK_DLL_MFUN( uana_cvals );
CK_DLL_MFUN( uana_fval );
CK_DLL_MFUN( uana_cval );
CK_DLL_MFUN( uana_connected );


//-----------------------------------------------------------------------------
// uanablob API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( uanablob_ctor );
CK_DLL_DTOR( uanablob_dtor );
CK_DLL_MFUN( uanablob_fvals );
CK_DLL_MFUN( uanablob_cvals );
CK_DLL_MFUN( uanablob_fval );
CK_DLL_MFUN( uanablob_cval );
CK_DLL_MFUN( uanablob_when );


//-----------------------------------------------------------------------------
// name: Chuck_UAnaBlobProxy
// desc: proxy for interfacing with UAnaBlob, which is a Chuck class
//-----------------------------------------------------------------------------
struct Chuck_UAnaBlobProxy
{
public:
    Chuck_UAnaBlobProxy( Chuck_Object * blob );
    virtual ~Chuck_UAnaBlobProxy();

public:
    t_CKTIME & when();
    Chuck_Array8 & fvals();
    Chuck_Array16 & cvals();

public:
    Chuck_Object * realblob() { return m_blob; }

protected:
    Chuck_Object * m_blob;
};

// get proxy
Chuck_UAnaBlobProxy * getBlobProxy( const Chuck_UAna * uana );


//-----------------------------------------------------------------------------
// shred API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( shred_ctor );
CK_DLL_DTOR( shred_dtor );
CK_DLL_MFUN( shred_exit );
CK_DLL_MFUN( shred_clone );
CK_DLL_MFUN( shred_id );
CK_DLL_MFUN( shred_yield );
CK_DLL_MFUN( shred_running );
CK_DLL_MFUN( shred_done );
CK_DLL_MFUN( shred_numArgs );
CK_DLL_MFUN( shred_getArg );
CK_DLL_MFUN( shred_sourcePath ); // added 1.3.0.0
CK_DLL_MFUN( shred_sourceDir ); // added 1.3.0.0
CK_DLL_MFUN( shred_sourceDir2 ); // added 1.3.2.0
CK_DLL_SFUN( shred_fromId ); // added 1.3.2.0


//-----------------------------------------------------------------------------
// array API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( array_ctor );
CK_DLL_DTOR( array_dtor );
CK_DLL_MFUN( array_set_capacity );
CK_DLL_MFUN( array_get_capacity );
CK_DLL_MFUN( array_get_capacity_hack );
CK_DLL_MFUN( array_set_size );
CK_DLL_MFUN( array_get_size );
CK_DLL_MFUN( array_push_back );
CK_DLL_MFUN( array_pop_back );
CK_DLL_MFUN( array_pop_out );
CK_DLL_MFUN( array_push_front );
CK_DLL_MFUN( array_pop_front );
CK_DLL_MFUN( array_find );
CK_DLL_MFUN( array_erase );
CK_DLL_MFUN( array_clear );
CK_DLL_MFUN( array_reset );


//-----------------------------------------------------------------------------
// event API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( event_ctor );
CK_DLL_DTOR( event_dtor );
CK_DLL_MFUN( event_signal );
CK_DLL_MFUN( event_broadcast );
CK_DLL_MFUN( event_wait );
CK_DLL_MFUN( event_can_wait );


//-----------------------------------------------------------------------------
// string API
//-----------------------------------------------------------------------------
CK_DLL_CTOR( string_ctor );
CK_DLL_DTOR( string_dtor );
CK_DLL_MFUN( string_length );
CK_DLL_MFUN( string_upper );
CK_DLL_MFUN( string_lower );
CK_DLL_MFUN( string_ltrim );
CK_DLL_MFUN( string_rtrim );
CK_DLL_MFUN( string_trim );
CK_DLL_MFUN( string_toString );
CK_DLL_MFUN( string_set_at );
CK_DLL_MFUN( string_get_at );
CK_DLL_MFUN( string_charAt);
CK_DLL_MFUN( string_setCharAt);
CK_DLL_MFUN( string_substring);
CK_DLL_MFUN( string_substringN);
CK_DLL_MFUN( string_insert);
CK_DLL_MFUN( string_replace);
CK_DLL_MFUN( string_replaceN);
CK_DLL_MFUN( string_find);
CK_DLL_MFUN( string_findStart);
CK_DLL_MFUN( string_findStr);
CK_DLL_MFUN( string_findStrStart);
CK_DLL_MFUN( string_rfind);
CK_DLL_MFUN( string_rfindStart);
CK_DLL_MFUN( string_rfindStr);
CK_DLL_MFUN( string_rfindStrStart);
CK_DLL_MFUN( string_erase);
CK_DLL_MFUN( string_toInt);
CK_DLL_MFUN( string_toFloat);
CK_DLL_MFUN( string_parent);


//-----------------------------------------------------------------------------
// vector API (vec3, vec4, eventually vector) ge: 1.3.5.3
//-----------------------------------------------------------------------------
CK_DLL_MFUN( vec3_set );
CK_DLL_MFUN( vec3_setAll );
CK_DLL_MFUN( vec3_magnitude );
CK_DLL_MFUN( vec3_normalize );
CK_DLL_MFUN( vec3_interp );
CK_DLL_MFUN( vec3_interp_delta_float );
CK_DLL_MFUN( vec3_interp_delta_dur );
CK_DLL_MFUN( vec3_update_goal );
CK_DLL_MFUN( vec3_update_goal_slew );
CK_DLL_MFUN( vec3_updateSet_goalAndValue );
CK_DLL_MFUN( vec3_updateSet_goalAndValue_slew );
CK_DLL_MFUN( vec4_set );
CK_DLL_MFUN( vec4_setAll );
CK_DLL_MFUN( vec4_magnitude );
CK_DLL_MFUN( vec4_normalize );


//-----------------------------------------------------------------------------
// name: Data_Exception
// desc: data for base Chuck Exception class
//-----------------------------------------------------------------------------
struct Data_Exception
{
public:
    Data_Exception();
    ~Data_Exception();

public:
};
//-----------------------------------------------------------------------------
// exception API
//-----------------------------------------------------------------------------





#endif
