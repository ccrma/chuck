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
// file: chuck_external.cpp
// desc: static functions for importing chuck as a library
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
// date: Spring 2017
//-----------------------------------------------------------------------------

#include "chuck_external.h"

// local globals
bool g_globals_initted = FALSE;




//-----------------------------------------------------------------------------
// name: ensureGlobalsInitted()
// desc: launch and destroy a ChucK to init all global resources
//-----------------------------------------------------------------------------
void ensureGlobalsInitted( const char * dataDir )
{
    if( !g_globals_initted )
    {
        // starting and quitting a chuck causes many global variables to be initialized
        Chuck_External::quitChuck( Chuck_External::startChuck( dataDir ) );
        g_globals_initted = TRUE;
    }
}




//-----------------------------------------------------------------------------
// name: initialSetup()
// desc: set up without chugins
//-----------------------------------------------------------------------------
void Chuck_External::initialSetup()
{
    ensureGlobalsInitted("");
}




//-----------------------------------------------------------------------------
// name: initialSetup()
// desc: set up with chugins
//-----------------------------------------------------------------------------
void Chuck_External::initialSetup( const char * chuginDir )
{
    ensureGlobalsInitted( chuginDir );
}




//-----------------------------------------------------------------------------
// name: finalCleanup()
// desc: clean up globals in preparation for final quit
//-----------------------------------------------------------------------------
void Chuck_External::finalCleanup()
{
    global_cleanup();
}




//-----------------------------------------------------------------------------
// name: startChuck()
// desc: launch a ChucK with no data dir
//-----------------------------------------------------------------------------
Chuck_System * Chuck_External::startChuck()
{
    return startChuck( "" );
};




//-----------------------------------------------------------------------------
// name: startChuck()
// desc: launch a ChucK
//-----------------------------------------------------------------------------
Chuck_System * Chuck_External::startChuck( const char * dataDir )
{
    Chuck_System * chuck = new Chuck_System;
    
    // equivalent to "chuck --loop --silent" on command line,
    // but without engaging the audio loop -- the caller is responsible
    // for calling audioCallback()
    std::vector< const char * > argsVector;
    argsVector.push_back( "chuck" );
    argsVector.push_back( "--external-callback" );
    // need to store arg2 for the c_str() pointer to be right
    std::string arg2 = std::string( "--data-dir:" ) + std::string( dataDir );
    argsVector.push_back( arg2.c_str() );
    const char ** args = (const char **) & argsVector[0];
    chuck->go( 3, args );
    
    return chuck;
};




//-----------------------------------------------------------------------------
// name: quitChuck()
// desc: stop a chuck
//-----------------------------------------------------------------------------
void Chuck_External::quitChuck( Chuck_System * chuck )
{
    delete chuck;
};




//-----------------------------------------------------------------------------
// name: runCode()
// desc: compile a code string
//-----------------------------------------------------------------------------
void Chuck_External::audioCallback(
    Chuck_System * chuck, SAMPLE * inbuffer, SAMPLE * outbuffer,
    unsigned int length, unsigned int inchannels, unsigned int outchannels )
{
    if( chuck->vm()->m_num_adc_channels != inchannels )
    {
        CK_STDCERR << "[chuck]: expected " << chuck->vm()->m_num_adc_channels << " in-channels and got " << inchannels << CK_STDENDL;
    }
    else if( chuck->vm()->m_num_dac_channels != outchannels )
    {
        CK_STDCERR << "[chuck]: expected " << chuck->vm()->m_num_dac_channels << " out-channels and got " << outchannels << CK_STDENDL;
    }
    else
    {
        chuck->run(inbuffer, outbuffer, length);
    }
}





//-----------------------------------------------------------------------------
// name: runCode()
// desc: compile a code string
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::runCode( Chuck_System * chuck, const char * code )
{
    return chuck->compileCode( code, "" );
}




//-----------------------------------------------------------------------------
// name: setExternalInt()
// desc: set a chuck external int's value
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setExternalInt( Chuck_System * chuck, const char * name, t_CKINT val )
{
    return chuck->vm()->set_external_int( std::string( name ), val );
}




//-----------------------------------------------------------------------------
// name: getExternalInt()
// desc: pass a callback for getting an external int's value
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::getExternalInt( Chuck_System * chuck, const char * name, void (*callback)(t_CKINT) )
{
    return chuck->vm()->get_external_int( std::string( name ), callback );
}





//-----------------------------------------------------------------------------
// name: setExternalFloat()
// desc: set a chuck external float's value
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setExternalFloat( Chuck_System * chuck, const char * name, t_CKFLOAT val )
{
    return chuck->vm()->set_external_float( std::string( name ), val );
}




//-----------------------------------------------------------------------------
// name: getExternalFloat()
// desc: pass a callback for getting an external float's value
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::getExternalFloat( Chuck_System * chuck, const char * name, void (*callback)(t_CKFLOAT) )
{
    return chuck->vm()->get_external_float( std::string( name ), callback );
}




//-----------------------------------------------------------------------------
// name: signalExternalEvent()
// desc: signal an external chuck Event
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::signalExternalEvent( Chuck_System * chuck, const char * name )
{
    return chuck->vm()->signal_external_event( std::string( name ) );
}




//-----------------------------------------------------------------------------
// name: broadcastExternalEvent()
// desc: signal an external chuck Event
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::broadcastExternalEvent( Chuck_System * chuck, const char * name )
{
    return chuck->vm()->broadcast_external_event( std::string( name ) );
}




//-----------------------------------------------------------------------------
// name: setChoutCallback()
// desc: set the callback for chout print statements
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setChoutCallback( void (* callback)(const char *) )
{
    if( !g_globals_initted ) return FALSE;
    Chuck_IO_Chout::getInstance()->set_output_callback( callback );
    return true;
}




//-----------------------------------------------------------------------------
// name: setCherrCallback()
// desc: set the callback for chout print statements
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setCherrCallback( void (* callback)(const char *) )
{
    if( !g_globals_initted ) return FALSE;
    Chuck_IO_Cherr::getInstance()->set_output_callback( callback );
    return true;
}




//-----------------------------------------------------------------------------
// name: setStdoutCallback()
// desc: set the callback for chout print statements
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setStdoutCallback( void (* callback)(const char *) )
{
    if( !g_globals_initted ) return FALSE;
    ck_set_stdout_callback( callback );
    return true;
}




//-----------------------------------------------------------------------------
// name: setStderrCallback()
// desc: set the callback for chout print statements
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_External::setStderrCallback( void (* callback)(const char *) )
{
    if( !g_globals_initted ) return FALSE;
    ck_set_stderr_callback( callback );
    return true;
}






