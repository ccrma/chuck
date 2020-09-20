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
// file: chuck_console.h
// desc: chuck command line console
//
// author: Spencer Salazar (spencer@ccrma.stanford.edu)
// date: Autumn 2005
//-----------------------------------------------------------------------------
#ifndef __CHUCK_CONSOLE_H__
#define __CHUCK_CONSOLE_H__

#include "chuck_def.h"
#include "chuck_shell.h"




//-----------------------------------------------------------------------------
// name: class Chuck_Console
// desc: for ttyp access to chuck shell
//-----------------------------------------------------------------------------
class Chuck_Console : public Chuck_Shell_UI
{
public:
    Chuck_Console();
    virtual ~Chuck_Console();

    t_CKBOOL init();
    t_CKBOOL next_command( const string &, string &);
    void next_result(const string &);

};




#endif //__CHUCK_CONSOLE_H__
