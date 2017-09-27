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
// file: ulib_std.cpp
// desc: standard class library
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Spring 2004
//-----------------------------------------------------------------------------
#include "ulib_std.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "util_buffers.h"
#include "util_console.h"
#include "util_math.h"
#include "util_string.h"
#include "util_thread.h"
#include "chuck_type.h"
#include "chuck_compile.h"
#include "chuck_instr.h"

#if defined(__PLATFORM_WIN32__)

#include <windows.h>

int setenv( const char *n, const char *v, int i )
{
    return !SetEnvironmentVariable(n, v);
}

#else

#include <unistd.h>

#endif

// for ConsoleInput and StringTokenizer
#include <sstream>
#include <iostream>
#include "util_thread.h"
using namespace std;


#ifndef __DISABLE_KBHIT__
// KBHit
CK_DLL_CTOR( KBHit_ctor );
CK_DLL_DTOR( KBHit_dtor );
CK_DLL_MFUN( KBHit_on );
CK_DLL_MFUN( KBHit_off );
CK_DLL_MFUN( KBHit_state );
CK_DLL_MFUN( KBHit_hit );
CK_DLL_MFUN( KBHit_more );
CK_DLL_MFUN( KBHit_getchar );
CK_DLL_MFUN( KBHit_can_wait );

static t_CKUINT KBHit_offset_data = 0;
#endif // __DISABLE_KBHIT__

#ifndef __DISABLE_PROMPTER__
// Skot functions
CK_DLL_CTOR( Skot_ctor );
CK_DLL_DTOR( Skot_dtor );
CK_DLL_MFUN( Skot_prompt );
CK_DLL_MFUN( Skot_prompt2 );
CK_DLL_MFUN( Skot_more );
CK_DLL_MFUN( Skot_getLine );
CK_DLL_MFUN( Skot_can_wait );

static t_CKUINT Skot_offset_data = 0;
#endif // __DISABLE_PROMPTER__

// StrTok functions
CK_DLL_CTOR( StrTok_ctor );
CK_DLL_DTOR( StrTok_dtor );
CK_DLL_MFUN( StrTok_set );
CK_DLL_MFUN( StrTok_reset );
CK_DLL_MFUN( StrTok_more );
CK_DLL_MFUN( StrTok_next );
CK_DLL_MFUN( StrTok_next2 );
CK_DLL_MFUN( StrTok_get );
CK_DLL_MFUN( StrTok_get2 );
CK_DLL_MFUN( StrTok_size );

static t_CKUINT StrTok_offset_data = 0;

#ifdef AJAY

#include <fstream>

// VCR functions
CK_DLL_CTOR( VCR_ctor );
CK_DLL_MFUN( VCR_load );
CK_DLL_MFUN( VCR_reset );
CK_DLL_MFUN( VCR_seek );
CK_DLL_MFUN( VCR_more );
CK_DLL_MFUN( VCR_curr );
CK_DLL_MFUN( VCR_next );
CK_DLL_MFUN( VCR_pos );
CK_DLL_MFUN( VCR_size );
CK_DLL_MFUN( VCR_name );

static t_CKUINT VCR_offset_data = 0;

// Cereal functions
CK_DLL_CTOR( Cereal_ctor );
CK_DLL_MFUN( Cereal_open );
CK_DLL_MFUN( Cereal_close );
CK_DLL_MFUN( Cereal_send );
CK_DLL_MFUN( Cereal_recv );
CK_DLL_MFUN( Cereal_more );

static t_CKUINT Cereal_offset_data = 0;

#endif




//-----------------------------------------------------------------------------
// name: libstd_query()
// desc: query entry point
//-----------------------------------------------------------------------------
DLL_QUERY libstd_query( Chuck_DL_Query * QUERY )
{
    // get global
    Chuck_Env * env = QUERY->env();
    // set name
    QUERY->setname( QUERY, "Std" );

    /*! \example
    std.rand2f( 100.0, 1000.0 ) => stdout;
    */

    // register deprecate
    type_engine_register_deprecate( env, "std", "Std" );

    // begin class
    QUERY->begin_class( QUERY, "Std", "Object" );
    QUERY->doc_class( QUERY, "Std is a standard library in ChucK, which includes utility functions for random number generation, unit conversions, and absolute value." );

    // add abs
    QUERY->add_sfun( QUERY, abs_impl, "int", "abs" );
    QUERY->add_arg( QUERY, "int", "value" );
    QUERY->doc_func( QUERY, "Return absolute value of integer. " );
    
    // add fabs
    QUERY->add_sfun( QUERY, fabs_impl, "float", "fabs" );
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Return absolute value of float. " );

    // add rand
    QUERY->add_sfun( QUERY, rand_impl, "int", "rand"); //! return int between 0 and RAND_MAX
    QUERY->doc_func( QUERY, "Generates random integer between 0 and Std.RAND_MAX. (NOTE: soon-to-be-deprecated; use Math.random2())." );

    // add rand2
    QUERY->add_sfun( QUERY, rand2_impl, "int", "rand2" ); //! integer between [min,max]
    QUERY->add_arg( QUERY, "int", "min" ); 
    QUERY->add_arg( QUERY, "int", "max" );
    QUERY->doc_func( QUERY, "Generates random integer in range [min, max]. (NOTE: soon-to-be-deprecated; use Math.random2())." );

    // add rand
    QUERY->add_sfun( QUERY, randf_impl, "float", "randf" ); //! rand between -1.0,1.0
    QUERY->doc_func( QUERY, "Generates random floating point number in the range [-1, 1]. (Note: soon-to-be-deprecated; use Math.randomf())" );

    // add rand2
    QUERY->add_sfun( QUERY, rand2f_impl, "float", "rand2f" ); //! rand between min and max
    QUERY->add_arg( QUERY, "float", "min" );
    QUERY->add_arg( QUERY, "float", "max" );
    QUERY->doc_func( QUERY, "Generates random floating point number in the range [min, max]. (NOTE: soon-to-be-deprecated; use Math.random2f())" );

    // add srand
    QUERY->add_sfun( QUERY, srand_impl, "void", "srand" );
    QUERY->add_arg( QUERY, "int", "seed" );
    QUERY->doc_func( QUERY, "Provide a seed to the random function. Different seeds will generate very different sequences of random numbers even if the seeds are close together. "
                     "Alternatively, a sequence of random numbers can be repeated by setting the same seed. " );

    // add sgn
    QUERY->add_sfun( QUERY, sgn_impl, "float", "sgn" ); //! return sign of value (-1, 0, 1)
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Computes the sign of the input as -1.0 (negative), 0 (zero), or 1.0 (positive)." );

    // add system
    //! see \example std.ck
    QUERY->add_sfun( QUERY, system_impl, "int", "system" ); //! issue system command
    QUERY->add_arg( QUERY, "string", "cmd" );
    QUERY->doc_func( QUERY, "Pass a command to be executed in the shell (requires --caution-to-the-wind command-line flag)." );

    // add atoi
    QUERY->add_sfun( QUERY, atoi_impl, "int", "atoi" ); //! string to integer
    QUERY->add_arg( QUERY, "string", "value" );
    QUERY->doc_func( QUERY, "Converts ascii (string) to integer (int)." );

    // add atof
    QUERY->add_sfun( QUERY, atof_impl, "float", "atof" ); //! string to float
    QUERY->add_arg( QUERY, "string", "value" );
    QUERY->doc_func( QUERY, "Converts ascii (string) to floating point value (float)." );

    // add itoa
    QUERY->add_sfun( QUERY, itoa_impl, "string", "itoa" ); //! int to string
    QUERY->add_arg( QUERY, "int", "i" );
    QUERY->doc_func( QUERY, "Converts integer (int) to ascii (string)." );

    // add ftoa
    QUERY->add_sfun( QUERY, ftoa_impl, "string", "ftoa" ); //! float to string
    QUERY->add_arg( QUERY, "float", "f" );
    QUERY->add_arg( QUERY, "int", "precision" );
    QUERY->doc_func( QUERY, "Converts floating point value (float) to ascii (string) with specified precision (number of decimal digits)." );

    // add ftoi
    QUERY->add_sfun( QUERY, ftoi_impl, "int", "ftoi" ); //! float to int
    QUERY->add_arg( QUERY, "float", "f" );
    QUERY->doc_func( QUERY, "Convert float to int. " );

    // add getenv
    QUERY->add_sfun( QUERY, getenv_impl, "string", "getenv" ); //! fetch environment variable
    QUERY->add_arg( QUERY, "string", "value" );
    QUERY->doc_func( QUERY, "Returns the value of an environment variable, such as of PATH." );

    // add setenv
    QUERY->add_sfun( QUERY, setenv_impl, "int", "setenv" ); //! set environment variable
    QUERY->add_arg( QUERY, "string", "key" );
    QUERY->add_arg( QUERY, "string", "value" );
    QUERY->doc_func( QUERY, "Set value of environment variable named key to value." );

    // add mtof
    //! see \example mand-o-matic.ck
    QUERY->add_sfun( QUERY, mtof_impl, "float", "mtof" ); //! midi note to frequency
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Converts a MIDI note number to frequency (Hz). Note the input value is of type float (supports fractional note number)." );

    // add ftom
    QUERY->add_sfun( QUERY, ftom_impl, "float", "ftom" ); //! frequency to midi note
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Converts frequency (Hz) to MIDI note number space." );

    // add powtodb
    QUERY->add_sfun( QUERY, powtodb_impl, "float", "powtodb" ); //! linear power to decibel 
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Converts signal power ratio to decibels (dB)." );

    // add rmstodb
    QUERY->add_sfun( QUERY, rmstodb_impl, "float", "rmstodb" ); //! rms to decibel
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Converts rms to decibels (dB)." );

    // add dbtopow
    QUERY->add_sfun( QUERY, dbtopow_impl, "float", "dbtopow" ); //! decibel to linear
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Converts decibels (dB) to signal power ratio." );

    // add dbtorms
    QUERY->add_sfun( QUERY, dbtorms_impl, "float", "dbtorms" ); //! decibel to rms
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "Convert " );

    // add dbtolin
    QUERY->add_sfun( QUERY, dbtolin_impl, "float", "dbtolin" ); //! decibel to linear
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "" );

    // add lintodb
    QUERY->add_sfun( QUERY, lintodb_impl, "float", "lintodb" ); //! linear to decibel
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->doc_func( QUERY, "" );

    // add clamp
    QUERY->add_sfun( QUERY, clamp_impl, "int", "clamp" ); //! clamp to range (int)
    QUERY->add_arg( QUERY, "int", "value" );
    QUERY->add_arg( QUERY, "int", "min" );
    QUERY->add_arg( QUERY, "int", "max" );
    QUERY->doc_func( QUERY, "" );

    // add clampf
    QUERY->add_sfun( QUERY, clampf_impl, "float", "clampf" ); //! clamp to range (float)
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "min" );
    QUERY->add_arg( QUERY, "float", "max" );
    QUERY->doc_func( QUERY, "" );

    // add scalef
    QUERY->add_sfun( QUERY, scalef_impl, "float", "scalef" ); //! scale from source range to dest range (float)
    QUERY->add_arg( QUERY, "float", "value" );
    QUERY->add_arg( QUERY, "float", "srcmin" );
    QUERY->add_arg( QUERY, "float", "srcmax" );
    QUERY->add_arg( QUERY, "float", "dstmin" );
    QUERY->add_arg( QUERY, "float", "dstmax" );
    QUERY->doc_func( QUERY, "" );

    // finish class
    QUERY->end_class( QUERY );

    // seed the rand
    srand( time( NULL ) );

    Chuck_DL_Func * func = NULL;
    
#ifndef __DISABLE_KBHIT__
    // KBHit
    // begin class (KBHit)
    if( !type_engine_import_class_begin( env, "KBHit", "Event",
                                         env->global(), KBHit_ctor,
                                         KBHit_dtor ) )
        return FALSE;

    // add member variable
    KBHit_offset_data = type_engine_import_mvar( env, "int", "@KBHit_data", FALSE );
    if( KBHit_offset_data == CK_INVALID_OFFSET ) goto error;

    // add on()
    func = make_new_mfun( "void", "on", KBHit_on );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add off()
    func = make_new_mfun( "void", "off", KBHit_off );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add state()
    func = make_new_mfun( "void", "state", KBHit_state );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add hit()
    func = make_new_mfun( "Event", "hit", KBHit_hit );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", KBHit_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getchar()
    func = make_new_mfun( "int", "getchar", KBHit_getchar );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add can_wait()
    func = make_new_mfun( "int", "can_wait", KBHit_can_wait );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end class
    type_engine_import_class_end( env );

    // start it
    KBHitManager::init();
#endif // __DISABLE_KBHIT__


    // register deprecate
    type_engine_register_deprecate( env, "Skot", "ConsoleInput" );

#ifndef __DISABLE_PROMPTER__
    // begin class (Skot)
    if( !type_engine_import_class_begin( env, "ConsoleInput", "Event",
                                         env->global(), Skot_ctor,
                                         Skot_dtor ) )
        return FALSE;

    // add member variable
    Skot_offset_data = type_engine_import_mvar( env, "int", "@Skot_data", FALSE );
    if( Skot_offset_data == CK_INVALID_OFFSET ) goto error;

    // add prompt()
    func = make_new_mfun( "Event", "prompt", Skot_prompt );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add prompt()
    func = make_new_mfun( "Event", "prompt", Skot_prompt2 );
    func->add_arg( "string", "what" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add ready()
    func = make_new_mfun( "int", "more", Skot_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add getString()
    func = make_new_mfun( "string", "getLine", Skot_getLine );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add can_wait()
    func = make_new_mfun( "int", "can_wait", Skot_can_wait );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end class
    type_engine_import_class_end( env );
#endif // __DISABLE_PROMPTER__


    // register deprecate
    type_engine_register_deprecate( env, "PRC", "StringTokenizer" );

    // begin class (StrTok)
    if( !type_engine_import_class_begin( env, "StringTokenizer", "Object",
                                         env->global(), StrTok_ctor,
                                         StrTok_dtor ) )
        return FALSE;

    // add member variable
    StrTok_offset_data = type_engine_import_mvar( env, "int", "@StrTok_data", FALSE );
    if( StrTok_offset_data == CK_INVALID_OFFSET ) goto error;

    // add set()
    func = make_new_mfun( "void", "set", StrTok_set );
    func->add_arg( "string", "line" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add reset()
    func = make_new_mfun( "void", "reset", StrTok_reset );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", StrTok_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add next()
    func = make_new_mfun( "string", "next", StrTok_next );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add get()
    func = make_new_mfun( "string", "next", StrTok_next2 );
    func->add_arg( "string", "out" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add get()
    func = make_new_mfun( "string", "get", StrTok_get );
    func->add_arg( "int", "index" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add get()
    func = make_new_mfun( "string", "get", StrTok_get2 );
    func->add_arg( "int", "index" );
    func->add_arg( "string", "out" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add size()
    func = make_new_mfun( "int", "size", StrTok_size );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end class
    type_engine_import_class_end( env );


#ifdef AJAY

    // begin class
    // init base class
    if( !type_engine_import_class_begin( env, "VCR", "Object",
                                         env->global(), VCR_ctor ) )
        return FALSE;

    // add member variable
    VCR_offset_data = type_engine_import_mvar( env, "int", "@me", FALSE );
    if( VCR_offset_data == CK_INVALID_OFFSET ) goto error;

    // add load()
    func = make_new_mfun( "int", "load", VCR_load );
    func->add_arg( "string", "filename" );
    func->add_arg( "int", "column" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add reset()
    func = make_new_mfun( "int", "reset", VCR_reset );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add seek()
    func = make_new_mfun( "int", "seek", VCR_seek );
    func->add_arg( "int", "where" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", VCR_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add curr()
    func = make_new_mfun( "float", "curr", VCR_curr );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add next()
    func = make_new_mfun( "int", "next", VCR_next );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add pos()
    func = make_new_mfun( "int", "pos", VCR_pos );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add size()
    func = make_new_mfun( "int", "size", VCR_size );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add name()
    func = make_new_mfun( "string", "name", VCR_name );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );


#if defined(__PLATFORM_WIN32__)
    // begin class (Cereal)
    if( !type_engine_import_class_begin( env, "Cereal", "Object",
                                         env->global(), Cereal_ctor ) )
        return FALSE;

    // add member
    Cereal_offset_data = type_engine_import_mvar( env, "int", "@Cereal_data", FALSE );
    if( Cereal_offset_data == CK_INVALID_OFFSET ) goto error;

    // add open()
    func = make_new_mfun( "int", "open", Cereal_open );
    func->add_arg( "string", "name" );
    func->add_arg( "int", "baudrate" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add close()
    func = make_new_mfun( "void", "close", Cereal_close );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add more()
    func = make_new_mfun( "int", "more", Cereal_more );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add send()
    func = make_new_mfun( "int", "send", Cereal_send );
    func->add_arg( "int", "bite" );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add recv()
    func = make_new_mfun( "int", "recv", Cereal_recv );
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end class
    type_engine_import_class_end( env );
#endif

    return TRUE;

#else

    return TRUE;

#endif

error:

    // end the class import
    type_engine_import_class_end( env );
    
    return FALSE;
}


#define RAND_INV_RANGE(r) (RAND_MAX / (r))

int irand_exclusive ( int max ) { 
  int x = ::rand();
  
  while (x >= max * RAND_INV_RANGE (max)) 
    x = ::rand();

  x /= RAND_INV_RANGE (max);
  return x;
}


// abs
CK_DLL_SFUN( abs_impl )
{
    t_CKINT v = GET_CK_INT(ARGS);
    RETURN->v_int = v >= 0 ? v : -v;
}

// fabs
CK_DLL_SFUN( fabs_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = v >= 0.0 ? v : -v;
    //RETURN->v_float = ::fabs( GET_CK_FLOAT(ARGS) );
}

// rand
CK_DLL_SFUN( rand_impl )
{
    RETURN->v_int = ::rand();
}

// randf
CK_DLL_SFUN( randf_impl )
{
    RETURN->v_float = ( 2.0 * ::rand() / (t_CKFLOAT)RAND_MAX - 1.0 );
}

// randf
CK_DLL_SFUN( rand2f_impl )
{
    t_CKFLOAT min = GET_CK_FLOAT(ARGS), max = *((t_CKFLOAT *)ARGS + 1);
    RETURN->v_float = min + (max-min)*(::rand()/(t_CKFLOAT)RAND_MAX);
}

// randi
CK_DLL_SFUN( rand2_impl ) // inclusive.
{
    // 1.3.1.0: converted int to t_CKINT for 64-bit compatibility
    t_CKINT min = *(t_CKINT *)ARGS, max = *((t_CKINT *)ARGS + 1);
    t_CKINT range = max - min; 
    if ( range == 0 )
    {
        RETURN->v_int = min;
    }
    //else if ( range < RAND_MAX / 2 ) { 
    //  RETURN->v_int = ( range > 0 ) ? min + irand_exclusive(1 + range) : max + irand_exclusive ( -range + 1 ) ;
    //}
    else
    {
        if( range > 0 )
        { 
            RETURN->v_int = min + (t_CKINT)( (1.0 + range) * ( ::rand()/(RAND_MAX+1.0) ) );
        }
        else
        { 
            RETURN->v_int = min - (t_CKINT)( (-range + 1.0) * ( ::rand()/(RAND_MAX+1.0) ) );
        }
    }
}

// srand
CK_DLL_SFUN( srand_impl )
{
    t_CKINT seed = GET_CK_INT(ARGS);
    ::srand( seed );
}

// sgn
CK_DLL_SFUN( sgn_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = v == 0.0f ? 0.0f : ( v > 0.0f ? 1.0f : -1.0f );
}

// system
CK_DLL_SFUN( system_impl )
{
    const char * cmd = GET_CK_STRING(ARGS)->str().c_str();

    // check globals for permission
// REFACTOR-2017: TODO reenable --caution-to-the-wind?
/*    if( !g_enable_system_cmd )
    {
        CK_FPRINTF_STDERR( "[chuck]:error: VM not authorized to call Std.system( string )...\n" );
        CK_FPRINTF_STDERR( "[chuck]:  (command string was: \"%s\")\n", cmd );
        CK_FPRINTF_STDERR( "[chuck]:  (note: enable via --caution-to-the-wind flag or other means)\n" );
        RETURN->v_int = 0;
    }
    else
*/
    {
        // log
        EM_log( CK_LOG_SEVERE, "invoking system( CMD )..." );
        EM_pushlog();
        EM_log( CK_LOG_SEVERE, "CMD: \"%s\"", cmd );
        EM_poplog();
        RETURN->v_int = system( cmd );
    }
}

// aoti
CK_DLL_SFUN( atoi_impl )
{
    Chuck_String * str = GET_CK_STRING(ARGS);
    if( str )
    {
        const char * v = str->str().c_str();
        RETURN->v_int = atoi( v );
    }
    else
    {
        RETURN->v_int = 0;
    }
}

// atof
CK_DLL_SFUN( atof_impl )
{
    Chuck_String * str = GET_CK_STRING(ARGS);
    if( str )
    {
        const char * v = GET_CK_STRING(ARGS)->str().c_str();
        RETURN->v_float = atof( v );
    }
    else
    {
        RETURN->v_float = 0.0;
    }
}

// itoa
CK_DLL_SFUN( itoa_impl )
{
    t_CKINT i = GET_CK_INT(ARGS);
    // TODO: memory leak, please fix.  Thanks.
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    a->set( itoa( i ) );
    RETURN->v_string = a;
}

// ftoa
CK_DLL_SFUN( ftoa_impl )
{
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    t_CKINT p = GET_NEXT_INT(ARGS);
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    a->set( ftoa( f, (t_CKUINT)p ) );
    RETURN->v_string = a;
}

// ftoi
CK_DLL_SFUN( ftoi_impl )
{
    t_CKFLOAT f = GET_NEXT_FLOAT(ARGS);
    RETURN->v_int = (t_CKINT) f;
}

// getenv
static Chuck_String g_str; // PROBLEM: not thread friendly
CK_DLL_SFUN( getenv_impl )
{
    const char * v = GET_CK_STRING(ARGS)->str().c_str();
    const char * s = getenv( v );
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    a->set( s ? s : "" );
    RETURN->v_string = a;
}

// setenv
CK_DLL_SFUN( setenv_impl )
{
    const char * v1 = GET_NEXT_STRING(ARGS)->str().c_str();
    const char * v2 = GET_NEXT_STRING(ARGS)->str().c_str();
    RETURN->v_int = setenv( v1, v2, 1 );
}


// mtof
CK_DLL_SFUN( mtof_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = mtof(v);
}

// ftom
CK_DLL_SFUN( ftom_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = ftom(v);
}

// powtodb
CK_DLL_SFUN( powtodb_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = powtodb(v);
}

// rmstodb
CK_DLL_SFUN( rmstodb_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = rmstodb(v);
}

// dbtopow
CK_DLL_SFUN( dbtopow_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = dbtopow(v);
}

// dbtorms
CK_DLL_SFUN( dbtorms_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = dbtorms(v);
}

CK_DLL_SFUN( dbtolin_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = pow(10.0, v/20.0);
}

CK_DLL_SFUN( lintodb_impl )
{
    t_CKFLOAT v = GET_CK_FLOAT(ARGS);
    RETURN->v_float = 20.0*log10(v);
}

CK_DLL_SFUN( clamp_impl )
{
    t_CKINT v = GET_NEXT_INT(ARGS);
    t_CKINT min = GET_NEXT_INT(ARGS);
    t_CKINT max = GET_NEXT_INT(ARGS);
    
    if(v < min) RETURN->v_int = min;
    else if( v > max) RETURN->v_int = max;
    else RETURN->v_int = v;
}

CK_DLL_SFUN( clampf_impl )
{
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT min = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT max = GET_NEXT_FLOAT(ARGS);
    
    if(v < min) RETURN->v_float = min;
    else if( v > max) RETURN->v_float = max;
    else RETURN->v_float = v;
}

CK_DLL_SFUN( scalef_impl )
{
    t_CKFLOAT v = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT srcmin = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT srcmax = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT dstmin = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT dstmax = GET_NEXT_FLOAT(ARGS);
    
    RETURN->v_float = dstmin + (dstmax-dstmin) * ((v-srcmin)/(srcmax-srcmin));
}




#ifndef __DISABLE_KBHIT__
// static
CBufferAdvance * KBHitManager::the_buf = NULL;
t_CKINT KBHitManager::the_onoff = 0;
t_CKBOOL KBHitManager::the_init = FALSE;
XThread * KBHitManager::the_thread;
#define BUFFER_SIZE 1024


#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
static void * kb_loop( void * )
#else
static unsigned int __stdcall kb_loop( void * )
#endif
{
    t_CKINT c;
    EM_log( CK_LOG_INFO, "starting kb loop..." );

    // go
    while( KBHitManager::the_init )
    {
        // if on
        if( KBHitManager::the_onoff )
        {
            // see if we have kb hit
            while( kb_hit() )
            {
                // get the next char
                c = kb_getch();
                // queue it
                KBHitManager::the_buf->put( &c, 1 );
            }
        }

        // wait
        usleep( 5000 );
    }

    return 0;
}


// init
t_CKBOOL KBHitManager::init()
{
    // sanity check
    if( the_buf ) return FALSE;

    EM_log( CK_LOG_INFO, "initializing KBHitManager..." );
    the_buf = new CBufferAdvance;
    if( !the_buf->initialize( BUFFER_SIZE, sizeof(t_CKINT) ) )
    {
        EM_log( CK_LOG_SEVERE, "KBHitManager: couldn't allocate central KB buffer..." );
        SAFE_DELETE( the_buf );
        return FALSE;
    }

    the_onoff = 0;
    the_init = TRUE;
    the_thread = new XThread;
    the_thread->start( kb_loop, NULL );


    return TRUE;
}


// shutdown
void KBHitManager::shutdown()
{
    EM_log( CK_LOG_INFO, "shutting down KBHitManager..." );
    SAFE_DELETE( the_buf );
    kb_endwin();

    the_onoff = 0;
    the_init = FALSE;
}


// on()
void KBHitManager::on()
{
    the_onoff++;
    if( !kb_ready() )
        kb_initscr();
}


// off()
void KBHitManager::off()
{
    the_onoff--;
    if( kb_ready() && the_onoff == 0 )
        kb_endwin();
}


// open()
t_CKBOOL KBHitManager::open( KBHit * kb )
{
    if( the_buf == NULL ) return FALSE;
    if( kb->m_buffer != NULL ) return FALSE;

    EM_log( CK_LOG_INFO, "adding KBHit..." );
    // init the kb
    kb->m_buffer = the_buf;
    // read index
    kb->m_read_index = kb->m_buffer->join( (Chuck_Event *)kb->SELF );

    return TRUE;
}


// close()
t_CKBOOL KBHitManager::close( KBHit * kb )
{
    if( the_buf == NULL ) return FALSE;
    if( kb->m_buffer == NULL ) return FALSE;

    EM_log( CK_LOG_INFO, "removing KBHit..." );
    // unjoin
    kb->m_buffer->resign( kb->m_read_index );
    // done
    kb->m_read_index = 0;

    return TRUE;
}


// KBHit
KBHit::KBHit()
{
    m_buffer = NULL;
    m_read_index = 0;
    m_valid = FALSE;
    m_onoff = FALSE;
    m_echo = FALSE;
    SELF = NULL;
}


// ~KBHit()
KBHit::~KBHit()
{
    this->close();
}


// open()
t_CKBOOL KBHit::open()
{
    return KBHitManager::open( this );
}


// close()
t_CKBOOL KBHit::close()
{
    return KBHitManager::close( this );
}


// on()
void KBHit::on()
{
    if( m_onoff == FALSE )
    {
        KBHitManager::on();
        open();
        m_onoff = TRUE;
    }
}


// off()
void KBHit::off()
{
    if( m_onoff == TRUE )
    {
        KBHitManager::off();
        open();
        m_onoff = FALSE;
    }
}


// empty
t_CKBOOL KBHit::empty()
{
    if( m_buffer == NULL ) return TRUE;
    if( m_onoff == FALSE ) return TRUE;
    // if( m_read_index == 0 ) return TRUE;
    return m_buffer->empty( m_read_index );
}


// getch
t_CKINT KBHit::getch()
{
    t_CKINT c;
    if( empty() ) return 0;
    m_buffer->get( &c, 1, m_read_index );
    return c;
}


// ctor
CK_DLL_CTOR( KBHit_ctor )
{
    KBHit * kb = new KBHit;
    OBJ_MEMBER_INT(SELF, KBHit_offset_data) = (t_CKINT)kb;
    kb->SELF = SELF;
    kb->on();
}


// dtor
CK_DLL_DTOR( KBHit_dtor )
{
    delete (KBHit *)OBJ_MEMBER_INT(SELF, KBHit_offset_data);
    OBJ_MEMBER_INT(SELF, KBHit_offset_data) = 0;
}

// on
CK_DLL_MFUN( KBHit_on )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    kb->on();
}


// off
CK_DLL_MFUN( KBHit_off )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    kb->off();
}


// state
CK_DLL_MFUN( KBHit_state )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    RETURN->v_int = kb->state();
}


// hit
CK_DLL_MFUN( KBHit_hit )
{
    //KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    RETURN->v_object = SELF;
}


// more
CK_DLL_MFUN( KBHit_more )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    RETURN->v_int = (kb->empty() == FALSE);
}


// getchar
CK_DLL_MFUN( KBHit_getchar )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    RETURN->v_int = kb->getch();
}


// can wait
CK_DLL_MFUN( KBHit_can_wait )
{
    KBHit * kb = (KBHit *)(OBJ_MEMBER_INT(SELF, KBHit_offset_data));
    RETURN->v_int = kb->empty();
}
#endif // __DISABLE_KBHIT__




#ifndef __DISABLE_PROMPTER__
class LineEvent : public Chuck_Event
{
public:
    LineEvent( Chuck_Event * SELF );
    ~LineEvent();

public:
    void prompt( const string & what = "" );
    t_CKBOOL more();
    string getLine();
    t_CKBOOL can_wait();
    void enqueue( const string & line )
    { m_q.push( line ); }

    Chuck_Event * SELF;

protected:
    queue<string> m_q;
};

// global variables
t_CKBOOL g_le_launched = FALSE;
t_CKBOOL g_le_wait = TRUE;
CHUCK_THREAD g_le_thread = 0;
map<LineEvent *, LineEvent *> g_le_map;
XMutex g_le_mutex;
string g_le_what;

// final cleanup (per host)
void le_cleanup()
{
    if( g_le_thread )
    {
        // cancel thread
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
        pthread_cancel( g_le_thread );
#else
        CloseHandle( g_le_thread );
#endif
        g_le_thread = 0;
        // reset
        g_le_launched = FALSE;
        g_le_wait = TRUE;
    }
}

// le callback
void * le_cb( void * p )
{
    char line[2048];
    map<LineEvent *, LineEvent *>::iterator iter;
    LineEvent * le = NULL;

    // loop
    while( true )
    {
        // wait
        while( g_le_wait )
            usleep( 10000 );

        // REFACTOR-2017: TODO Ge:
        // I removed the check here for if there are no more vms running
        // Theoretically, we will always just kill this thread from outside of
        // it. Does that sound right to you?

        // do the prompt
        cout << g_le_what;
        cout.flush();
        if( !cin.getline( line, 2048 ) ) break;

        // lock
        g_le_mutex.acquire();
        // go through
        for( iter = g_le_map.begin(); iter != g_le_map.end(); iter++ )
        {
            // get the line event
            le = (*iter).first;
            // add to its queue
            le->enqueue( line );
            // broadcast it
            le->SELF->queue_broadcast();
        }
        // unlock
        g_le_mutex.release();

        // reset wait
        g_le_wait = TRUE;
    }
    
    return NULL;
}


LineEvent::LineEvent( Chuck_Event * SELF )
{
    // launch the cb
    if( !g_le_launched )
    {
#if !defined(__PLATFORM_WIN32__) || defined(__WINDOWS_PTHREAD__)
        pthread_create( &g_le_thread, NULL, le_cb, NULL );
#else
        g_le_thread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)le_cb, NULL, 0, 0 );
#endif
        g_le_launched = TRUE;
    }

    // lock
    g_le_mutex.acquire();
    // add
    g_le_map[this] = this;
    this->SELF = SELF;
    // unlock
    g_le_mutex.release();
}

LineEvent::~LineEvent()
{
    // remove from global map
    g_le_map.erase( this );
}

void LineEvent::prompt( const string & what )
{
    // set what
    g_le_what = what;
    if( g_le_what != "" ) g_le_what += " ";
    // signal
    g_le_wait = FALSE;
}

t_CKBOOL LineEvent::more()
{
    // more
    return m_q.size() > 0;
}

string LineEvent::getLine()
{
    string ret;

    // lock
    g_le_mutex.acquire();
    // get next line
    if( m_q.size() )
    {
        // get it
        ret = m_q.front();
        // dequeue it
        m_q.pop();
    }
    else
    {
        ret = "[ERROR -> getLine() called on empty Skot]";
    }
    // unlock
    g_le_mutex.release();

    return ret;
}

t_CKBOOL LineEvent::can_wait()
{
    return !more();
}


// Skot
CK_DLL_CTOR( Skot_ctor )
{
    LineEvent * le = new LineEvent((Chuck_Event *)SELF);
    OBJ_MEMBER_INT(SELF, Skot_offset_data) = (t_CKINT)le;
}

CK_DLL_DTOR( Skot_dtor )
{
    delete (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    OBJ_MEMBER_INT(SELF, Skot_offset_data) = 0;
}

CK_DLL_MFUN( Skot_prompt )
{
    LineEvent * le = (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    le->prompt();
    RETURN->v_int = (t_CKINT)(SELF);
}

CK_DLL_MFUN( Skot_prompt2 )
{
    LineEvent * le = (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    const char * v = GET_CK_STRING(ARGS)->str().c_str();
    le->prompt( v );
    RETURN->v_int = (t_CKINT)(SELF);
}

CK_DLL_MFUN( Skot_more )
{
    LineEvent * le = (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    RETURN->v_int = le->more();
}

CK_DLL_MFUN( Skot_getLine )
{
    LineEvent * le = (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    // TODO: memory leak
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    a->set( le->getLine() );
    RETURN->v_string = a;
}

CK_DLL_MFUN( Skot_can_wait )
{
    LineEvent * le = (LineEvent *)OBJ_MEMBER_INT(SELF, Skot_offset_data);
    RETURN->v_int = le->can_wait();
}
#endif // __DISABLE_PROMPTER__


// StrTok
class StrTok
{
public:
    StrTok();
    ~StrTok();

public:
    void set( const string & line );
    void reset();
    t_CKBOOL more();
    string next();
    string get( t_CKINT index );
    t_CKINT size();

protected:
    istringstream * m_ss;
    string m_next;
    vector<string> m_tokens;
    vector<string>::size_type m_index;
};

StrTok::StrTok()
{
    m_ss = NULL;
    m_index = 0;
}

StrTok::~StrTok()
{
    SAFE_DELETE( m_ss );
}

void StrTok::set( const string & line )
{
    string s;

    // delete
    SAFE_DELETE( m_ss );
    // alloc
    m_ss = new istringstream( line );
    // read
    reset();
    m_tokens.clear();
    while( (*m_ss) >> s )
        m_tokens.push_back( s );    
}

void StrTok::reset()
{
    m_index = 0;
}

t_CKBOOL StrTok::more()
{
    return m_index < m_tokens.size();
}

string StrTok::next()
{
    if( !more() ) return "";
    return m_tokens[m_index++];
}

string StrTok::get( t_CKINT index )
{
    if( index >= (t_CKINT)m_tokens.size() ) return "";
    return m_tokens[index];
}

t_CKINT StrTok::size()
{
    return (t_CKINT)m_tokens.size();
}

CK_DLL_CTOR( StrTok_ctor )
{
    StrTok * tokens = new StrTok;
    OBJ_MEMBER_INT(SELF, StrTok_offset_data) = (t_CKINT)tokens;
}

CK_DLL_DTOR( StrTok_dtor )
{
    delete (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    OBJ_MEMBER_INT(SELF, StrTok_offset_data) = 0;
}

CK_DLL_MFUN( StrTok_set )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    Chuck_String * s = GET_CK_STRING(ARGS);
    if( s ) tokens->set( s->str() );
    else tokens->set( "" );
}

CK_DLL_MFUN( StrTok_reset )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    tokens->reset();
}

CK_DLL_MFUN( StrTok_more )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    RETURN->v_int = (t_CKINT)tokens->more();
}

CK_DLL_MFUN( StrTok_next )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    a->set( tokens->next() );
    RETURN->v_string = a;
}

CK_DLL_MFUN( StrTok_next2 )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    Chuck_String * a = GET_CK_STRING(ARGS);
    string s = tokens->next();
    if( a ) a->set( s );
    RETURN->v_string = a;
}

CK_DLL_MFUN( StrTok_get )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    t_CKINT index = GET_NEXT_INT(ARGS);
    Chuck_String * a = (Chuck_String *)instantiate_and_initialize_object( SHRED->vm_ref->env()->t_string, SHRED );
    string s = tokens->get( index );
    a->set( s );
    RETURN->v_string = a;
}

CK_DLL_MFUN( StrTok_get2 )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    t_CKINT index = GET_NEXT_INT(ARGS);
    Chuck_String * a = GET_NEXT_STRING(ARGS);
    string s = tokens->get( index );
    if( a ) a->set( s );
    RETURN->v_string = a;
}

CK_DLL_MFUN( StrTok_size )
{
    StrTok * tokens = (StrTok *)OBJ_MEMBER_INT(SELF, StrTok_offset_data);
    RETURN->v_int = tokens->size();
}




#ifdef AJAY


class ColumnReader
{
public:
    ColumnReader();
    virtual ~ColumnReader();

    bool init( const string & filename, long col );
    
    bool reset() { if( !fin.good() ) return false; where = 0; return true; }
    bool seek( long pos ) { if( pos < 0 || pos >= values.size() ) return false; where = pos; return true; }
    bool more() { return where < values.size(); }
    double curr();
    bool next() { if( where < values.size() ) where++; return more(); }
    long pos() { return where; }
    long size() { return values.size(); }
    string name() { return n; }

    Chuck_String s;

protected:
    bool get_str( string & out );
    bool get_double( double & out );

protected:
    ifstream fin;
    string n;
    long column;
    long where;
    char line[0x1000];
    long len;

    vector<double> values;
};


ColumnReader::ColumnReader()
{
    n = "[NONE]";
    column = -1;
    where = -1;
    len = 0x1000;
}


ColumnReader::~ColumnReader()
{
    // close file
    if( fin.good() ) 
        fin.close();
}

bool ColumnReader::init( const string & filename, long col )
{
    // hmm
    if( col < 1 )
    {
        CK_STDCERR << "column must be greater than 0!!!" << CK_STDENDL;
        return false;
    }

    // open file
    fin.open( filename.c_str(), ios::in );
    // yes
    if( !fin.good() )
    {
        CK_STDCERR << "ColumnReader: cannot open file: '" << filename << "'..." << CK_STDENDL;
        return false;
    }

    // set column
    column = col;

    // read first line
    if( !fin.getline( line, len ) )
    {
        CK_STDCERR << "ColumnReader: cannot read first line: '" << filename << "'..." << CK_STDENDL;
        return false;
    }

    // get the name
    if( !get_str( n ) )
    {
        CK_STDCERR << "ColumnReader: cannot seek to column " << col << ": " << filename << "..." << CK_STDENDL;
        return false;
    }

    double v;
    long i = 1;
    // read values
    while( fin.getline( line, len ) )
    {
        v = 0.0;
        // get value
        if( !get_double( v ) )
        {
            CK_STDCERR << "ColumnReader: cannot read column " << v << " on line i: " << n << "..." << CK_STDENDL;
            return false;
        }

        values.push_back( v );
    }

    // well
    if( values.size() == 0 )
    {
        CK_STDCERR << "ColumnReader: file doesn't not contain data after first line: " << n << "..." << CK_STDENDL;
        return false;
    }

    // set location
    where = 0;

    s.str = n;

    return true;
}

double ColumnReader::curr()
{
    if( where >= values.size() )
    {
        CK_STDCERR << "ColumnReader: trying to read beyond end of file: " << n << "..." << CK_STDENDL;
        return 0.0;
    }

    return values[where];
}

bool ColumnReader::get_double( double & out )
{
    assert( column > 0 );
    long c = 1;
    
    char * start = line;
    char * curr = start;

    while( c < column )
    {
        // move past value
        while( *curr && *curr != ',' ) curr++;
        // move past ,
        while( *curr && *curr == ',' ) curr++;
        // check
        if( *curr == '\0' )
        {
            CK_STDCERR << "ColumnReader: cannot find column " << column << ": " << n << CK_STDENDL;
            return false;
        }

        // increment
        c++;
        // set start
        start = curr;
    }

    // move past value
    while( *curr && *curr != ',' ) curr++;
    // end
    *curr = '\0';

    out = atof( start );
    
    return true;
}


bool ColumnReader::get_str( string & out )
{
    assert( column > 0 );
    long c = 1;
    
    char * start = line;
    char * curr = start;

    while( c < column )
    {
        // move past value
        while( *curr && *curr != ',' ) curr++;
        // move past ,
        while( *curr && *curr == ',' ) curr++;
        // check
        if( *curr == '\0' )
        {
            CK_STDCERR << "ColumnReader: cannot find column " << column << ": " << n << CK_STDENDL;
            return false;
        }

        // increment
        c++;
        // set start
        start = curr;
    }

    // move past value
    while( *curr && *curr != ',' ) curr++;
    // end
    *curr = '\0';

    out = start;
    
    return true;
}


CK_DLL_CTOR( VCR_ctor )
{
    OBJ_MEMBER_INT(SELF, VCR_offset_data) = (t_CKUINT)new ColumnReader;
}

CK_DLL_MFUN( VCR_load )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    const char * filename = GET_NEXT_STRING(ARGS)->str.c_str();
    t_CKINT column = GET_NEXT_INT(ARGS);
    RETURN->v_int = vcr->init( filename, column );
}

CK_DLL_MFUN( VCR_reset )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_int = vcr->reset();
}

CK_DLL_MFUN( VCR_seek )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    t_CKINT where = GET_CK_INT(ARGS);
    RETURN->v_int = vcr->seek( where );
}

CK_DLL_MFUN( VCR_more )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_int = vcr->more();
}

CK_DLL_MFUN( VCR_curr )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_float = vcr->curr();
}

CK_DLL_MFUN( VCR_next )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_int = vcr->next();
}

CK_DLL_MFUN( VCR_pos )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_int = vcr->pos();
}

CK_DLL_MFUN( VCR_size )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_int = vcr->size();
}

CK_DLL_MFUN( VCR_name )
{
    ColumnReader * vcr = (ColumnReader*)OBJ_MEMBER_INT(SELF, VCR_offset_data);
    RETURN->v_string = &(vcr->s);
}


#ifdef __PLATFORM_WIN32__


// jeff's cereal
class Serial
{
public:
    // one stop bit, 8 bits, no parity
	Serial();
	~Serial();

	unsigned read( char * buffer, unsigned numberOfBytesToRead );
	unsigned write( char * buffer, unsigned numberOfBytesToWrite );

	void write( char c );
	char read();

	unsigned available() const;

    t_CKBOOL open( char * port, t_CKUINT baudrate );
	void close();

private:
	HANDLE serialFile;
};


Serial::Serial( )
{
    serialFile = NULL;
}

Serial::~Serial( )
{
	close();
}

t_CKBOOL Serial::open( char * port, t_CKUINT baudrate )
{
    if( serialFile )
        close();

	// open port
	serialFile = CreateFile( port,
							 GENERIC_READ | GENERIC_WRITE,
							 0,
							 0,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 0 );
	if( serialFile == INVALID_HANDLE_VALUE ) 
	{
		if( GetLastError() == ERROR_FILE_NOT_FOUND ) 
		{
            EM_log( CK_LOG_SYSTEM, "error: cannot open serial port '%s'...", port );
		}
        else
        {
            EM_log( CK_LOG_SYSTEM, "error opening serial port '%s'...", port ); 
        }

        return FALSE;
	}

	// set params
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof( dcbSerialParams );
	if( !GetCommState( serialFile, &dcbSerialParams) ) 
	{
        EM_log( CK_LOG_SYSTEM, "error getting serial state..." );
        close();
        return FALSE;
	}

    dcbSerialParams.BaudRate = baudrate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if( !SetCommState( serialFile, &dcbSerialParams ) )
	{
        EM_log( CK_LOG_SYSTEM, "error setting serial state..." );
        close();
        return FALSE;
	}

	// SET TIMEOUTS
    /*
	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant= 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if( !SetCommTimeouts( serialFile, &timeouts ) )
	{
		//error occureed. Inform user
	}
	*/

    return TRUE;
}

void Serial::close()
{
    if( serialFile )
    {
	    CloseHandle( serialFile );
        serialFile = NULL;
    }
}

void Serial::write( char c )
{
	write( &c, 1 );
}

char Serial::read()
{
	char c = '\0';
	read( &c, 1 );
	return c;
}

unsigned Serial::available() const
{
	struct _COMSTAT status;
    unsigned long etat;
    memset( &status, 0, sizeof(status) );

    if( serialFile )
    {
	    ClearCommError( serialFile, &etat, &status);
        return status.cbInQue;
    }

    return 0;
}

// try to read numberOfBytesToRead into buffer, return how many bytes read
unsigned Serial::read( char * buffer, unsigned numberOfBytesToRead )
{
	DWORD bytesRead = 0;
    if( serialFile )
	    ReadFile( serialFile, buffer, numberOfBytesToRead, &bytesRead, NULL );
	return bytesRead;
}

// try to write numberOfBytesToWrite to serial from buffer, return how many bytes written
unsigned Serial::write( char * buffer, unsigned numberOfBytesToWrite )
{
	DWORD bytesWritten = 0;
    if( serialFile )
	    WriteFile( serialFile, buffer, numberOfBytesToWrite, &bytesWritten, NULL );
	return bytesWritten;
}


// ctor
CK_DLL_CTOR( Cereal_ctor )
{
    OBJ_MEMBER_UINT(SELF, Cereal_offset_data) = (t_CKUINT)new Serial;
}

// open
CK_DLL_MFUN( Cereal_open )
{
    Serial * s = (Serial *)OBJ_MEMBER_UINT(SELF, Cereal_offset_data);
    Chuck_String * str = GET_NEXT_STRING(ARGS);
    t_CKINT i = GET_NEXT_INT(ARGS);

    // close
    s->close();

    if( str )
        RETURN->v_int = s->open( (char *)str->str.c_str(), i );
    else
        RETURN->v_int = 0;
}

// close
CK_DLL_MFUN( Cereal_close )
{
    Serial * s = (Serial *)OBJ_MEMBER_UINT(SELF, Cereal_offset_data);
    s->close();
}

// more
CK_DLL_MFUN( Cereal_more )
{
    Serial * s = (Serial *)OBJ_MEMBER_UINT(SELF, Cereal_offset_data);
    RETURN->v_int = s->available();
}

// send
CK_DLL_MFUN( Cereal_send )
{
    Serial * s = (Serial *)OBJ_MEMBER_UINT(SELF, Cereal_offset_data);
    t_CKINT i = GET_NEXT_INT(ARGS);
    s->write( i );
    RETURN->v_int = i;
}

// recv
CK_DLL_MFUN( Cereal_recv )
{
    Serial * s = (Serial *)OBJ_MEMBER_UINT(SELF, Cereal_offset_data);
    if( s->available() )
        RETURN->v_int = (t_CKINT)s->read();
    else
        RETURN->v_int = 0;
}

#endif


#endif
