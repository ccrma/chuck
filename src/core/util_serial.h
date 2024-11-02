/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: util_serial.h
// desc: utility for serial I/O
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2012
//-----------------------------------------------------------------------------
#ifndef __UTIL_SERIAL_H__
#define __UTIL_SERIAL_H__

#include "chuck_def.h"

#include <vector>
#include <string>




//-----------------------------------------------------------------------------
// name: class SerialIOManager
// desc: serial I/O manager
//-----------------------------------------------------------------------------
class SerialIOManager
{
public:
    static std::vector<std::string> availableSerialDevices();

private:
    static std::vector<std::string> s_availableSerialDevices;
};




#endif /* __UTIL_SERIAL_H__ */
