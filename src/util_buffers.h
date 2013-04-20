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
// file: util_buffers.h
// desc: buffer utilities
//
// author: Ge Wang (gewang@cs.princeton.edu)
//         Perry R. Cook (prc@cs.princeton.edu)
//         Ananya Misra (amisra@cs.princeton.edu)
// date: Spring 2004
//       Summer 2005 - allow multiple readers
//-----------------------------------------------------------------------------
#ifndef __UTIL_BUFFERS_H__
#define __UTIL_BUFFERS_H__

#include "chuck_oo.h"
#include "util_thread.h"
#include <vector>
#include <queue>

#define DWORD__                unsigned long
#define SINT__                 long
#define UINT__                 DWORD__
#define BOOL__                 DWORD__
#define BYTE__                 unsigned char

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif


class CBufferSimple;


//-----------------------------------------------------------------------------
// name: class CBufferAdvance
// desc: circular buffer
//-----------------------------------------------------------------------------
class CBufferAdvance
{
public:
    CBufferAdvance();
    ~CBufferAdvance();

public:
    BOOL__ initialize( UINT__ num_elem, UINT__ width, CBufferSimple * event_buffer = NULL );
    void cleanup();

public:
    UINT__ get( void * data, UINT__ num_elem, UINT__ read_offset_index );
    void put( void * data, UINT__ num_elem );
    BOOL__ empty( UINT__ read_offset_index );
    UINT__ join( Chuck_Event * event = NULL );
    void resign( UINT__ read_offset_index );

protected:
    BYTE__ * m_data;
    UINT__   m_data_width;
    //UINT__   m_read_offset;

    // this holds the offset allocated by join(), paired with an optional
    // Chuck_Event to notify when things are put in the buffer
    struct ReadOffset
    {
        SINT__ read_offset;
        Chuck_Event * event;
        ReadOffset( SINT__ ro, Chuck_Event * e = NULL )
        { read_offset = ro; event = e; }
    };
    std::vector<ReadOffset> m_read_offsets;
    std::queue<UINT__> m_free;

    SINT__   m_write_offset;
    SINT__   m_max_elem;

    // TODO: necessary?
    XMutex m_mutex;
    
    CBufferSimple * m_event_buffer;
};




//-----------------------------------------------------------------------------
// name: class CBufferSimple
// desc: circular buffer - one reader one writer
//-----------------------------------------------------------------------------
class CBufferSimple
{
public:
    CBufferSimple();
    ~CBufferSimple();

public:
    BOOL__ initialize( UINT__ num_elem, UINT__ width );
    void cleanup();

public:
    UINT__ get( void * data, UINT__ num_elem );
    void put( void * data, UINT__ num_elem );

protected:
    BYTE__ * m_data;
    UINT__   m_data_width;
    UINT__   m_read_offset;
    UINT__   m_write_offset;
    UINT__   m_max_elem;
};




//-----------------------------------------------------------------------------
// name: class AccumBuffer
// desc: circular buffer for sample accumulation
//-----------------------------------------------------------------------------
class AccumBuffer
{
public:
    AccumBuffer();
    ~AccumBuffer();

public:
    t_CKINT resize( t_CKINT new_size );
    void cleanup();

public:
    void put( SAMPLE next );
    void get( SAMPLE * buffer, t_CKINT num_elem );

protected:
    SAMPLE * m_data;
    t_CKUINT m_write_offset;
    t_CKUINT m_max_elem;
};




//-----------------------------------------------------------------------------
// name: class DeccumBuffer
// desc: circular buffer for sample deccumulation
//-----------------------------------------------------------------------------
class DeccumBuffer
{
public:
    DeccumBuffer();
    ~DeccumBuffer();

public:
    t_CKINT resize( t_CKINT new_size );
    void cleanup();

public:
    void put( SAMPLE * next, t_CKINT num_elem );
    void get( SAMPLE * out );
    void get( SAMPLE * buffer, t_CKINT num_elem );

protected:
    SAMPLE * m_data;
    t_CKUINT m_read_offset;
    t_CKUINT m_max_elem;
};




//-----------------------------------------------------------------------------
// name: class CircularBuffer
// desc: templated circular buffer
//-----------------------------------------------------------------------------
template<typename T>
class CircularBuffer
{
public:
    
    CircularBuffer(size_t numElements) :
    m_read(0),
    m_write(0),
    m_numElements(numElements+1) // need 1 element to pad
    {
        m_elements = new T[m_numElements];
    }
    
    ~CircularBuffer()
    {
        if(m_elements != NULL)
        {
            delete[] m_elements;
            m_elements = NULL;
        }
    }
    
    // put one element
    // returns number of elements successfully put
    size_t put(T element)
    {
        if((m_write + 1)%m_numElements == m_read)
        {
            // no space
            return 0;
        }
        
        m_elements[m_write] = element;
        
        m_write = (m_write+1)%m_numElements;
        
        return 1;
    }
    
    // get one element
    // returns number of elements successfully got
    size_t get(T &element)
    {
        if(m_read == m_write)
        {
            // nothing to get
            return 0;
        }
        
        element = m_elements[m_read];
        
        m_read = (m_read+1)%m_numElements;
        
        return 1;
    }
    
    // peek at element i without removing it
    // i = 1 would correspond to the least recently put element;
    // i = -1 would correspond to the most recent
    // WARNING: doesn't sanity check that i is a valid index
    // returns number of elements successfully peeked
    size_t peek(T &element, size_t i)
    {
        if(i == 0)
            return 0;
        
        if(i > 0)
            element = m_elements[(m_read+(i-1))%m_numElements];
        
        return 1;
    }
    
    void clear() { m_write = m_read; }
    
    // return maximum number of elements that can be held
    size_t maxElements() { return m_numElements-1; }
    
    // return if buffer is full
    bool atMaximum() { return (m_write + 1)%m_numElements == m_read; }
    
    // return number of valid elements in the buffer
    size_t numElements()
    {
        if(m_read == m_write)
            return 0;
        else if(m_read < m_write)
            return m_write - m_read;
        else
            return m_numElements - m_read + m_write;
    }
    
private:
    T * m_elements;
    size_t m_read, m_write;
    const size_t m_numElements;
};




//-----------------------------------------------------------------------------
// name: class FastCircularBuffer
// desc: uses memcpy instead of assignment
//       useful for streaming large blocks of data
//-----------------------------------------------------------------------------
class FastCircularBuffer
{
public:
    FastCircularBuffer();
    ~FastCircularBuffer();
    
public:
    t_CKUINT initialize( t_CKUINT num_elem, t_CKUINT width );
    void cleanup();
    
public:
    t_CKUINT get( void * data, t_CKUINT num_elem );
    t_CKUINT put( void * data, t_CKUINT num_elem );
    inline bool hasMore() { return (m_read_offset != m_write_offset); }
    inline void clear() { m_read_offset = m_write_offset = 0; }
    
protected:
    t_CKBYTE * m_data;
    t_CKUINT   m_data_width;
    t_CKUINT   m_read_offset;
    t_CKUINT   m_write_offset;
    t_CKUINT   m_max_elem;
};




#endif
