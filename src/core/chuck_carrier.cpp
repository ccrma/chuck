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
// file: chuck_carrier.cpp
// desc: carrier of things associated with each ChucK instance
//       REFACTOR-2017
//
// author: Ge Wang (http://www.gewang.com/)
//         Jack Atherton (lja@ccrma.stanford.edu)
//         Spencer Salazar (spencer@ccrma.stanford.edu)
// date: fall 2017
//-----------------------------------------------------------------------------
#include "chuck_carrier.h"
#include "chuck.h"




//-----------------------------------------------------------------------------
// name: hintIsRealtimeAudio()
// desc: get hint: is this VM on a realtime audio thread?
//-----------------------------------------------------------------------------
t_CKBOOL Chuck_Carrier::hintIsRealtimeAudio()
{
    // check
    if( !chuck ) return FALSE;
    // get hint
    return chuck->getParamInt( CHUCK_PARAM_HINT_IS_REALTIME_AUDIO ) != 0;
}
