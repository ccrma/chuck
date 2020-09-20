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
// file: util_buffers.cpp
// desc: buffer utilities
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Spring 2004
//       Summer 2005 - allow multiple readers
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include "util_buffers.h"
#include "chuck_errmsg.h"


#ifndef CALLBACK
#define CALLBACK
#endif



//-----------------------------------------------------------------------------
// name: CBufferAdvance()
// desc: constructor
//-----------------------------------------------------------------------------
CBufferAdvance::CBufferAdvance()
{
    m_data = NULL;
    m_data_width = m_write_offset = m_max_elem = 0; // = m_read_offset
}




//-----------------------------------------------------------------------------
// name: ~CBufferAdvance()
// desc: destructor
//-----------------------------------------------------------------------------
CBufferAdvance::~CBufferAdvance()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize
//-----------------------------------------------------------------------------
BOOL__ CBufferAdvance::initialize( UINT__ num_elem, UINT__ width, CBufferSimple * event_buffer )
{
    // cleanup
    cleanup();

    // allocate
    m_data = (BYTE__ *)malloc( num_elem * width );
    if( !m_data )
        return false;

    m_data_width = width;
    //m_read_offset = 0;
    m_write_offset = 0;
    m_max_elem = (SINT__)num_elem;
    
    m_event_buffer = event_buffer;

    return true;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: cleanup
//-----------------------------------------------------------------------------
void CBufferAdvance::cleanup()
{
    if( !m_data )
        return;

    free( m_data );

    m_data = NULL;
    m_data_width = 0;
    m_write_offset = m_max_elem = 0; // = m_read_offset
}




//-----------------------------------------------------------------------------
// name: UINT__ join()
// desc: shred can call this to get an index into the vector of read pointers
//-----------------------------------------------------------------------------
UINT__ CBufferAdvance::join( Chuck_Event * event )
{
    // TODO: necessary?
    m_mutex.acquire();

    // index of new pointer that will be pushed back
    UINT__ read_offset_index;
    
    // add new pointer pointing (as pointers do) to current write offset
    // (shreds don't get interrupted, so m_write_offset will always be correct, right?)
    // (uh, hope so...)
    if( !m_free.empty() )
    {
        read_offset_index = m_free.front();
        m_free.pop();
        //assert( read_offset_index < m_read_offsets.size() );
        m_read_offsets[read_offset_index] = ReadOffset( m_write_offset, event );
    }
    else
    {
        read_offset_index = m_read_offsets.size();
        m_read_offsets.push_back( ReadOffset( m_write_offset, event ) );
    }

    // TODO: necessary?
    m_mutex.release();

    // return index
    return read_offset_index;
}


//-----------------------------------------------------------------------------
// name: resign
// desc: shred quits buffer; frees its index
//-----------------------------------------------------------------------------
void CBufferAdvance::resign( UINT__ read_offset_index )
{
    // make sure read_offset_index passed in is valid
    if( read_offset_index >= m_read_offsets.size() )
        return;

    // TODO: necessary?
    m_mutex.acquire();

    // add this index to free queue
    m_free.push( read_offset_index );

    // "invalidate" the pointer at that index
    m_read_offsets[read_offset_index].read_offset = -1;
    m_read_offsets[read_offset_index].event = NULL;

    // TODO: necessary?
    m_mutex.release();
}


//-----------------------------------------------------------------------------
// name: put()
// desc: put
//-----------------------------------------------------------------------------
/*void CBufferAdvance::put( void * data, UINT__ num_elem )
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
}*/


void CBufferAdvance::put( void * data, UINT__ num_elem )
{
    UINT__ i, j;
    BYTE__ * d = (BYTE__ *)data;

    // TODO: necessary?
    m_mutex.acquire();

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

        // possibility of expelling evil shreds
        for( j = 0; j < m_read_offsets.size(); j++ )
        {
            if( m_write_offset == m_read_offsets[j].read_offset )
            {
                // inform shred with index j that it has lost its privileges?
                // invalidate its read_offset
                // m_read_offsets[j].read_offset = -1;
            }

            if( m_read_offsets[j].event )
                m_read_offsets[j].event->queue_broadcast( m_event_buffer );
        }
    }

    // TODO: necessary?
    m_mutex.release();
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
/*UINT__ CBufferAdvance::get( void * data, UINT__ num_elem )
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
}*/


BOOL__ CBufferAdvance::empty( UINT__ read_offset_index )
{
    // make sure index is valid
    if( read_offset_index >= m_read_offsets.size() )
        return TRUE;
    if( m_read_offsets[read_offset_index].read_offset < 0 )
        return TRUE;

    SINT__ m_read_offset = m_read_offsets[read_offset_index].read_offset;

    // see if caught up
    return m_read_offset == m_write_offset;
}


UINT__ CBufferAdvance::get( void * data, UINT__ num_elem, UINT__ read_offset_index )
{
    UINT__ i, j;
    BYTE__ * d = (BYTE__ *)data;

    // TODO: necessary?
    m_mutex.acquire();

    // make sure index is valid
    if( read_offset_index >= m_read_offsets.size() )
    {
        m_mutex.release();
        return 0;
    }
    if( m_read_offsets[read_offset_index].read_offset < 0 )
    {
        m_mutex.release();
        return 0;
    }

    SINT__ m_read_offset = m_read_offsets[read_offset_index].read_offset;

    // read catch up with write
    if( m_read_offset == m_write_offset )
    {
        m_mutex.release();
        return 0;
    }

    // copy
    for( i = 0; i < num_elem; i++ )
    {
        for( j = 0; j < m_data_width; j++ )
        {
            d[i*m_data_width+j] = m_data[m_read_offset*m_data_width+j];
        }

        // move read
        m_read_offset++;
        // wrap
        if( m_read_offset >= m_max_elem )
            m_read_offset = 0;
        
        // catch up
        if( m_read_offset == m_write_offset )
        {
            i++;
            break;
        }
    }

    // update read offset at given index
    m_read_offsets[read_offset_index].read_offset = m_read_offset;

    // TODO: necessary?
    m_mutex.release();

    // return number of elems
    return i;
}




//-----------------------------------------------------------------------------
// name: CBufferSimple()
// desc: constructor
//-----------------------------------------------------------------------------
CBufferSimple::CBufferSimple()
{
    m_data = NULL;
    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: ~CBufferSimple()
// desc: destructor
//-----------------------------------------------------------------------------
CBufferSimple::~CBufferSimple()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize
//-----------------------------------------------------------------------------
BOOL__ CBufferSimple::initialize( UINT__ num_elem, UINT__ width )
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
void CBufferSimple::cleanup()
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
void CBufferSimple::put( void * data, UINT__ num_elem )
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
        // Aug 2014 - spencer
        // change to fully "atomic" increment+wrap
        m_write_offset = (m_write_offset + 1) % m_max_elem;
    }
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
UINT__ CBufferSimple::get( void * data, UINT__ num_elem )
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
        // Aug 2014 - spencer
        // change to fully "atomic" increment+wrap
        m_read_offset = (m_read_offset + 1) % m_max_elem;
        
        // catch up
        if( m_read_offset == m_write_offset )
        {
            i++;
            break;
        }
    }

    // return number of elems
    return 1; // shouldn't it return i?
}




//-----------------------------------------------------------------------------
// name: AccumBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
AccumBuffer::AccumBuffer()
{
    m_data = NULL;
    m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: ~AccumBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
AccumBuffer::~AccumBuffer()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: resize
//-----------------------------------------------------------------------------
t_CKINT AccumBuffer::resize( t_CKINT size )
{
    // if the same, then done
    if( size == m_max_elem ) return size;

    // allocate
    SAMPLE * next = (SAMPLE *)malloc( size * sizeof(SAMPLE) );
    // zero it
    memset( next, 0, size * sizeof(SAMPLE) );
    if( !next )
    {
        // log
        EM_log( CK_LOG_WARNING, "AccumBuffer:resize(%ld) failed to allocated..." );
        // clean up
        this->cleanup();
        // done
        return FALSE;
    }
    
    // if no current
    if( !m_data )
    {
        // reset everything
        m_data = next;
        m_write_offset = 0;
        m_max_elem = size;
    }
    else
    {
        // retrieve next
        this->get( next, size );
        // clean up
        this->cleanup();
        // copy
        m_data = next;
        // write pointer is at the end
        m_write_offset = 0;
        // update max elem
        m_max_elem = size;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: cleanup
//-----------------------------------------------------------------------------
void AccumBuffer::cleanup()
{
    if( !m_data )
        return;

    free( m_data );
    m_data = NULL;
    m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: put()
// desc: put
//-----------------------------------------------------------------------------
void AccumBuffer::put( SAMPLE data )
{
    // copy
    m_data[m_write_offset] = data;
    // move the write
    m_write_offset++;
    // wrap
    if( m_write_offset >= m_max_elem )
        m_write_offset = 0;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
void AccumBuffer::get( SAMPLE * buffer, t_CKINT num_elem )
{
    // left to copy
    t_CKINT left = num_elem;
    // amount
    t_CKINT amount = m_max_elem - m_write_offset;
    // to copy
    t_CKINT tocopy = ck_min( left, amount );
    // update left
    left -= tocopy;
    // copy after the write pointer
    memcpy( buffer, m_data + m_write_offset, tocopy * sizeof(SAMPLE) );

    // more?
    if( left )
    {
        // amount
        amount = m_write_offset;
        // to copy
        t_CKINT tocopy2 = ck_min( left, amount );
        // update left
        left -= tocopy2;
        // copy before the write pointer
        memcpy( buffer + tocopy, m_data, tocopy2 * sizeof(SAMPLE) );

        // more?
        if( left )
        {
            // make sure
            assert( m_max_elem == (tocopy + tocopy2) );
            assert( num_elem > m_max_elem );
            // zero it out
            memset( buffer + m_max_elem, 0, (num_elem - m_max_elem) * sizeof(SAMPLE) );
        }
    }
}




//-----------------------------------------------------------------------------
// name: DeccumBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
DeccumBuffer::DeccumBuffer()
{
    m_data = NULL;
    m_read_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: ~DeccumBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
DeccumBuffer::~DeccumBuffer()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: resize()
// desc: resize
//-----------------------------------------------------------------------------
t_CKINT DeccumBuffer::resize( t_CKINT size )
{
    // if the same, then done
    if( size == m_max_elem ) return size;

    // allocate
    SAMPLE * next = (SAMPLE *)malloc( size * sizeof(SAMPLE) );
    // zero it
    memset( next, 0, size * sizeof(SAMPLE) );
    if( !next )
    {
        // log
        EM_log( CK_LOG_WARNING, "DeccumBuffer:resize(%ld) failed to allocated..." );
        // clean up
        this->cleanup();
        // done
        return FALSE;
    }
    
    // if no current
    if( !m_data )
    {
        // reset everything
        m_data = next;
        m_read_offset = 0;
        m_max_elem = size;
    }
    else
    {
        // retrieve next
        this->get( next, size );
        // clean up
        this->cleanup();
        // copy
        m_data = next;
        // read pointer is at the end
        m_read_offset = 0;
        // update max elem
        m_max_elem = size;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// name: cleanup()
// desc: cleanup
//-----------------------------------------------------------------------------
void DeccumBuffer::cleanup()
{
    if( !m_data )
        return;

    free( m_data );
    m_data = NULL;
    m_read_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: put()
// desc: put
//-----------------------------------------------------------------------------
void DeccumBuffer::get( SAMPLE * data )
{
    // copy
    *data = m_data[m_read_offset];
    // zero
    m_data[m_read_offset] = 0;
    // move the write
    m_read_offset++;
    // wrap
    if( m_read_offset >= m_max_elem )
        m_read_offset = 0;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
void DeccumBuffer::get( SAMPLE * buffer, t_CKINT num_elem )
{
    // left to copy
    t_CKINT left = num_elem;
    // amount
    t_CKINT amount = m_max_elem - m_read_offset;
    // to copy
    t_CKINT tocopy = ck_min( left, amount );
    // update left
    left -= tocopy;
    // copy after the write pointer
    memcpy( buffer, m_data + m_read_offset, tocopy * sizeof(SAMPLE) );

    // more?
    if( left )
    {
        // amount
        amount = m_read_offset;
        // to copy
        t_CKINT tocopy2 = ck_min( left, amount );
        // update left
        left -= tocopy2;
        // copy before the write pointer
        memcpy( buffer + tocopy, m_data, tocopy2 * sizeof(SAMPLE) );

        // more?
        if( left )
        {
            // make sure
            assert( m_max_elem == (tocopy + tocopy2) );
            assert( num_elem > m_max_elem );
            // zero it out
            memset( buffer + m_max_elem, 0, (num_elem - m_max_elem) * sizeof(SAMPLE) );
        }
    }
}




//-----------------------------------------------------------------------------
// name: put()
// desc: put
//-----------------------------------------------------------------------------
void DeccumBuffer::put( SAMPLE * buffer, t_CKINT num_elem )
{
    // left to add
    t_CKINT left = num_elem;
    // amount
    t_CKINT amount = m_max_elem - m_read_offset;
    // to add
    t_CKINT tocopy = ck_min( left, amount );
    // update left
    left -= tocopy;
    // copy after the write pointer
    t_CKINT i;
    for( i = 0; i < tocopy; i++ )
        m_data[m_read_offset+i] += buffer[i];

    // more?
    if( left )
    {
        // amount
        amount = m_read_offset;
        // to copy
        t_CKINT tocopy2 = ck_min( left, amount );
        // update left
        left -= tocopy2;
        // copy before the write pointer
        for( i = 0; i < tocopy2; i++ )
            m_data[i] += buffer[tocopy+i];

        // more?
        if( left )
        {
            // make sure
            assert( m_max_elem == (tocopy + tocopy2) );
            assert( num_elem > m_max_elem );
            // log
            EM_log( CK_LOG_WARNING, "(IFFT): discarding data during OLA synthesis..." );
        }
    }
}


//-----------------------------------------------------------------------------
// name: SMCircularBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
FastCircularBuffer::FastCircularBuffer()
{
    m_data = NULL;
    m_data_width = m_read_offset = m_write_offset = m_max_elem = 0;
}




//-----------------------------------------------------------------------------
// name: ~SMCircularBuffer()
// desc: destructor
//-----------------------------------------------------------------------------
FastCircularBuffer::~FastCircularBuffer()
{
    this->cleanup();
}




//-----------------------------------------------------------------------------
// name: initialize()
// desc: initialize
//-----------------------------------------------------------------------------
t_CKUINT FastCircularBuffer::initialize( t_CKUINT num_elem, t_CKUINT width )
{
    // cleanup
    cleanup();
    
    // allocate
    m_data = (t_CKBYTE *)malloc( num_elem * width );
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
void FastCircularBuffer::cleanup()
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
t_CKUINT FastCircularBuffer::put( void * _data, t_CKUINT num_elem )
{
    t_CKBYTE * data = (t_CKBYTE *)_data;
    
    // TODO: overflow checking
    if(!(num_elem < ((m_read_offset > m_write_offset) ?
                     (m_read_offset - m_write_offset) :
                     (m_max_elem - m_write_offset + m_read_offset))))
    {
        return 0;
    }
    
    t_CKUINT elems_before_end = ck_min(num_elem, m_max_elem - m_write_offset);
    t_CKUINT elems_after_end = num_elem - elems_before_end;
    
    if(elems_before_end)
        memcpy(m_data + m_write_offset * m_data_width,
               data,
               elems_before_end * m_data_width);
    
    if(elems_after_end)
        memcpy(m_data,
               data + elems_before_end * m_data_width,
               elems_after_end * m_data_width);
    
    if(elems_after_end)
        m_write_offset = elems_after_end;
    else
        m_write_offset += elems_before_end;
    
    return elems_before_end + elems_after_end;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get
//-----------------------------------------------------------------------------
t_CKUINT FastCircularBuffer::get( void * _data, t_CKUINT num_elem )
{
    t_CKBYTE * data = (t_CKBYTE *)_data;
    
    t_CKUINT elems_before_end;
    t_CKUINT elems_after_end;
    if(m_write_offset < m_read_offset)
    {
        elems_before_end = m_max_elem - m_read_offset;
        elems_after_end = m_write_offset;
    }
    else
    {
        elems_before_end = m_write_offset - m_read_offset;
        elems_after_end = 0;
    }
    
    if(elems_before_end > num_elem)
    {
        elems_before_end = num_elem;
        elems_after_end = 0;
    }
    else if(elems_before_end + elems_after_end > num_elem)
    {
        elems_after_end = num_elem - elems_before_end;
    }
    
    //    UInt32 elems_before_end = min(m_write_offset - m_read_offset, m_max_elem - m_read_offset);
    //    UInt32 elems_after_end = num_elem - elems_before_end;
    
    if(elems_before_end)
        memcpy(data,
               m_data + m_read_offset * m_data_width,
               elems_before_end * m_data_width);
    
    if(elems_after_end)
        memcpy(data + elems_before_end * m_data_width,
               m_data,
               elems_after_end * m_data_width);
    
    if(elems_after_end)
        m_read_offset = elems_after_end;
    else
        m_read_offset += elems_before_end;
    
    return elems_before_end + elems_after_end;
}


