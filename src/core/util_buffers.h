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
#include <iostream>

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
    // i = -1 would correspond to the most recent (not implemented)
    // returns 1 if elements successfully peeked
    size_t peek(T &element, size_t i)
    {
        if(i == 0)
            return 0;
        
        if(i > 0 && i <= numElements())
        {
            element = m_elements[(m_read+(i-1))%m_numElements];
            return 1;
        }
        
        return 0;
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




//-----------------------------------------------------------------------------
// name: class XCircleBuffer
// desc: templated circular buffer class (different impl from ge-X-lib)
//-----------------------------------------------------------------------------
template <typename T>
class XCircleBuffer
{
public:
    XCircleBuffer( long length = 0 );
    ~XCircleBuffer();
    
public:
    // reset length of buffer (capacity)
    void init( long length );
    // get length
    long length() const;
    // clear (does no explicit memory management)
    void clear();
    
public:
    // put an element into the buffer - the item will be copied
    // NOTE: if over-capacity, will discard least recently put item
    void put( const T & item );
    // get next item (FIFO)
    bool get( T * pItem );
    // number of valid elements in buffer
    long numElements() const;
    // are there more elements?
    bool more() const;
    // get elements without advancing - returns number of valid elements
    long peek( T * array, long numItems, unsigned long stride = 0 );
    // pop
    long pop( long numItems = 1 );
    
protected: // helper functions
    inline void advanceWrite();
    inline void advanceRead();
    
protected:
    // the buffer
    T * m_buffer;
    // the buffer length (capacity)
    volatile long m_length;
    // write index
    volatile long m_writeIndex;
    // read index
    volatile long m_readIndex;
    // num elements
    volatile long m_numElements;
};




//-----------------------------------------------------------------------------
// name: XCircleBuffer()
// desc: constructor
//-----------------------------------------------------------------------------
template <typename T>
XCircleBuffer<T>::XCircleBuffer( long length )
{
    // zero out first
    m_buffer = NULL;
    m_length = m_readIndex = m_writeIndex = m_numElements = 0;
    
    // call init
    this->init( length );
}




//-----------------------------------------------------------------------------
// name: ~XCircleBuffer
// desc: destructor
//-----------------------------------------------------------------------------
template <typename T>
XCircleBuffer<T>::~XCircleBuffer()
{
    
}




//-----------------------------------------------------------------------------
// name: init()
// desc: reset length of buffer
//-----------------------------------------------------------------------------
template <typename T>
void XCircleBuffer<T>::init( long length )
{
    // clean up is necessary
    if( m_buffer )
    {
        // delete array - should call destructors and zero out variable
        SAFE_DELETE_ARRAY( m_buffer );
        // zero out
        m_length = m_readIndex = m_writeIndex = m_numElements = 0;
    }
    
    // sanity check
    if( length < 0 )
    {
        // doh
        std::cerr << "[XCircleBuffer]: error invalid length '"
        << length << "' requested" << std::endl;
        
        return;
    }
    
    // check for zero length
    if( length == 0 ) return;
    
    // allocate
    m_buffer = new T[length];
    // check
    if( m_buffer == NULL )
    {
        // doh
        std::cerr << "[XCircleBuffer]: failed to allocate buffer of length '"
        << length << "'..." << std::endl;
        
        return;
    }
    
    // save
    m_length = length;
    // zero out
    m_readIndex = m_writeIndex = m_numElements = 0;
}




//-----------------------------------------------------------------------------
// name: length()
// desc: get length
//-----------------------------------------------------------------------------
template <typename T>
long XCircleBuffer<T>::length() const
{
    return m_length;
}




//-----------------------------------------------------------------------------
// name: clear()
// desc: clear (does no explicit memory management)
//-----------------------------------------------------------------------------
template <typename T>
void XCircleBuffer<T>::clear()
{
    // zero out
    m_readIndex = m_writeIndex = m_numElements = 0;
}




//-----------------------------------------------------------------------------
// name: advanceWrite()
// desc: helper to advance write index
//-----------------------------------------------------------------------------
template <typename T>
void XCircleBuffer<T>::advanceWrite()
{
    // increment
    m_writeIndex++;
    
    // check for bounds
    if( m_writeIndex >= m_length )
    {
        // wrap
        m_writeIndex -= m_length;
    }
    
    // increment count
    m_numElements++;
}




//-----------------------------------------------------------------------------
// name: advanceRead()
// desc: helper to advance read index
//-----------------------------------------------------------------------------
template <typename T>
void XCircleBuffer<T>::advanceRead()
{
    // increment
    m_readIndex++;
    
    // check for bounds
    if( m_readIndex >= m_length )
    {
        // wrap
        m_readIndex -= m_length;
    }
    
    // decrement count
    m_numElements--;
}




//-----------------------------------------------------------------------------
// name: put()
// desc: put an element into the buffer - the item will be copied
//       if over-capacity, will discard least recently put item
//-----------------------------------------------------------------------------
template <typename T>
void XCircleBuffer<T>::put( const T & item )
{
    // sanity check
    if( m_buffer == NULL ) return;
    
    // copy it
    m_buffer[m_writeIndex] = item;
    
    // advance write index
    advanceWrite();
    
    // if read and write pointer are the same, over-capacity
    if( m_writeIndex == m_readIndex )
    {
        // warning
        std::cerr << "[circular-buffer]: buffer full, dropping items!" << std::endl;
        // advance read!
        advanceRead();
    }
}




//-----------------------------------------------------------------------------
// name: numElements()
// desc: get number of valid elements in buffer
//-----------------------------------------------------------------------------
template <typename T>
long XCircleBuffer<T>::numElements() const
{
    // return our count
    return m_numElements;
}




//-----------------------------------------------------------------------------
// name: hasMore()
// desc: are there more elements?
//-----------------------------------------------------------------------------
template <typename T>
bool XCircleBuffer<T>::more() const
{
    return m_numElements > 0;
}




//-----------------------------------------------------------------------------
// name: peek
// desc: get elements without advancing - returns number returned
//-----------------------------------------------------------------------------
template <typename T>
long XCircleBuffer<T>::peek( T * array, long numItems, unsigned long stride )
{
    // sanity check
    if( m_buffer == NULL ) return 0;
    
    // sanity check (so the wrap can be sure to land inbounds)
    if( stride >= m_length ) return 0;
    
    // count
    long count = 0;
    // actual count, taking stride out of the equation
    long actualCount = 0;
    
    // starting index
    long index = m_writeIndex - 1;
    if( index < 0 ) index += m_length;
    
    // while need more but haven't reached write index...
    while( (count < numItems) && (count < m_numElements) )
    {
        // copy
        array[actualCount] = m_buffer[index];
        // increment
        count++; count += stride;
        // advance
        index--; index -= stride;
        // actual count, don't stride
        actualCount++;
        // wrap
        if( index < 0 ) index += m_length;
    }
    
    // reverse contents
    for( int i = 0; i < actualCount/2; i++ )
    {
        T v = array[i];
        array[i] = array[actualCount-1-i];
        array[actualCount-1-i] = v;
    }
    
    return actualCount;
}




//-----------------------------------------------------------------------------
// name: pop()
// desc: pop one or more elements
//-----------------------------------------------------------------------------
template <typename T>
long XCircleBuffer<T>::pop( long numItems )
{
    // sanity check
    if( m_buffer == NULL ) return 0;
    
    // count
    long count = 0;
    
    // while there is more to pop and need to pop more
    while( more() && count < numItems )
    {
        // advance read
        advanceRead();
        // increment count
        count++;
    }
    
    return count;
}




//-----------------------------------------------------------------------------
// name: get()
// desc: get next read element
//-----------------------------------------------------------------------------
template <typename T>
bool XCircleBuffer<T>::get( T * result )
{
    // sanity check
    if( m_buffer == NULL || m_readIndex == m_writeIndex ) return false;
    
    // get item to read
    *result = m_buffer[m_readIndex];
    // advance read
    advanceRead();
    
    return true;
}




#endif
