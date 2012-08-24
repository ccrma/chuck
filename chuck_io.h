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
// name: chuck_io.h
// desc: chuck i/o
//
// authors: Spencer Salazar (spencer@ccrma.stanford.edu)
//    date: Summer 2012
//-----------------------------------------------------------------------------

#ifndef __CHUCK_IO_H__
#define __CHUCK_IO_H__

#include "chuck_oo.h"

struct Chuck_IO_Serial : public Chuck_IO_File
{
public:
    Chuck_IO_Serial();
    virtual ~Chuck_IO_Serial();
    
public:
    virtual t_CKBOOL open( const t_CKUINT i, t_CKINT flags );
    virtual t_CKBOOL open( const std::string & path, t_CKINT flags );

    virtual Chuck_String * readLine();

    virtual t_CKBOOL ready();
    
protected:
    
    int m_fd;
    FILE * m_cfd;
    
    char * m_buf;
    t_CKUINT m_buf_size;
    t_CKUINT m_buf_begin, m_buf_end;
};

t_CKBOOL init_class_serialio( Chuck_Env * env ); // added 1.3.1

#endif // __CHUCK_IO_H__

