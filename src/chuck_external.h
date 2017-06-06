/*----------------------------------------------------------------------------
 ChucK Concurrent, On-the-fly Audio Programming Language
   Compiler and Virtual Machine
 
 Copyright (c) 2003 Ge Wang and Perry R. Cook.  All rights reserved.
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
// file: chuck_external.h
// desc: chuck external encapsulation
//
// author: Jack Atherton (lja@ccrma.stanford.edu)
// date: spring 2017
//
//
// additional contributors:
//         Ananya Misra (amisra@cs.princeton.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
// date: version 1.1.x.x - Autumn 2002
//       version 1.2.x.x - Autumn 2004
//       version 1.3.x.x - Spring 2012
//-----------------------------------------------------------------------------
#ifndef __CHUCK_EXTERNAL_H__
#define __CHUCK_EXTERNAL_H__

#include "chuck_system.h"
#include <string>






//-----------------------------------------------------------------------------
// name: class Chuck_External
// desc: ChucK system encapsulation
//-----------------------------------------------------------------------------
class Chuck_External
{
public:
    static void initialSetup();
    static void initialSetup( const char * chuginDir );
    static void finalCleanup();

    static Chuck_System * startChuck();
    static Chuck_System * startChuck( const char * dataDir );
    static void quitChuck( Chuck_System * chuck );
    static void audioCallback( Chuck_System * chuck, SAMPLE * inbuffer, SAMPLE * outbuffer, unsigned int length, unsigned int inchannels, unsigned int outchannels );
    static t_CKBOOL runCode( Chuck_System * chuck, const char * code );
    static t_CKBOOL setExternalInt( Chuck_System * chuck, const char * name, t_CKINT val );
    static t_CKBOOL getExternalInt( Chuck_System * chuck, const char * name, void (* callback)(t_CKINT) );
    static t_CKBOOL setExternalFloat( Chuck_System * chuck, const char * name, t_CKFLOAT val );
    static t_CKBOOL getExternalFloat( Chuck_System * chuck, const char * name, void (* callback)(t_CKFLOAT) );
    static t_CKBOOL signalExternalEvent( Chuck_System * chuck, const char * name );
    static t_CKBOOL broadcastExternalEvent( Chuck_System * chuck, const char * name );
    static t_CKBOOL setChoutCallback( void (* callback)(const char *) );
    static t_CKBOOL setCherrCallback( void (* callback)(const char *) );
    static t_CKBOOL setStdoutCallback( void (* callback)(const char *) );
    static t_CKBOOL setStderrCallback( void (* callback)(const char *) );
};




#endif
