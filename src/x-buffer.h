/*----------------------------------------------------------------------------
  MCD-X: General API for audio/graphics/interaction programming
    (sibling of MCD-Y API)
    http://www.gewang.com/software/mcd-api/

  Copyright (c) 2007-present Ge Wang
    All rights reserved.
    http://www.gewang.com/

  Music, Computing, Design Group @ CCRMA, Stanford University
    http://ccrma.stanford.edu/groups/mcd/

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
// name: x-buffer.h
// desc: templated simple circular buffer
//
// authors: Ge Wang (ge@ccrma.stanford.edu)
//    date: Spring 2012
// version: 1.0.0
//-----------------------------------------------------------------------------
#ifndef __MCD_X_BUFFER_H__
#define __MCD_X_BUFFER_H__

#include <iostream>




//-----------------------------------------------------------------------------
// name: class MoCircleBuffer
// desc: templated circular buffer class
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
    if( m_buffer < 0 )
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
