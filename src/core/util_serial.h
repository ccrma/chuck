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
// file: util_serial.h
// desc: utility for serial I/O
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Summer 2012
//-----------------------------------------------------------------------------
#ifndef __UTIL_SERIAL_H__
#define __UTIL_SERIAL_H__

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
