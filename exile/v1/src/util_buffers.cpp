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
// file: util_buffers.cpp
// desc: buffer implementation
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//-----------------------------------------------------------------------------
#include "util_buffers.h"
#include <stdlib.h>




//-----------------------------------------------------------------------------
// name: Cbuffer()
// desc: constructor
//-----------------------------------------------------------------------------
CBuffer::CBuffer()
{
    m_data = NULL;
    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: ~CBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
CBuffer::~CBuffer()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize
//-----------------------------------------------------------------------------
BOOL__ CBuffer::initialize( UINT__ num_elem, UINT__ width )
{
    // cleanup
    cleanup();

    // allocate
    m_data = (BYTE__ *)malloc( num_elem * width );
    if( !m_data )
        return false;

    m_data_width = width;
    m_read_offset = 0;
    m_write_offset = 0;
    m_max_elem = num_elem;

    return true;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: cleanup
//-----------------------------------------------------------------------------
void CBuffer::cleanup()
{
    if( !m_data )
        return;

    free( m_data );

    m_data = NULL;
    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: put()
// desc: put
//-----------------------------------------------------------------------------
void CBuffer::put( void * data, UINT__ num_elem )
{
    UINT__ i, j;
    BYTE__ * d = (BYTE__ *)data;

    // copy
    for( i = 0; i < num_elem; i++ )
    {
        for( j = 0; j < m_data_width; j++ )
        {
            m_data[m_write_offset*m_data_width+j] = d[i*m_data_width+j];
        }

        // move the write
        m_write_offset++;

        // wrap
        if( m_write_offset >= m_max_elem )
            m_write_offset = 0;
    }
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
UINT__ CBuffer::get( void * data, UINT__ num_elem )
{
    UINT__ i, j;
    BYTE__ * d = (BYTE__ *)data;

    // read catch up with write
    if( m_read_offset == m_write_offset )
        return 0;

    // copy
    for( i = 0; i < num_elem; i++ )
    {
        for( j = 0; j < m_data_width; j++ )
        {
            d[i*m_data_width+j] = m_data[m_read_offset*m_data_width+j];
        }

        // move read
        m_read_offset++;
        
        // catch up
        if( m_read_offset == m_write_offset )
        {
            i++;
            break;
        }

        // wrap
        if( m_read_offset >= m_max_elem )
            m_read_offset = 0;
    }

    // return number of elems
    return 1;
}
