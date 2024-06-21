/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
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
#include "chuck_errmsg.h"
#ifndef __DISABLE_THREADS__
#include "util_thread.h"
#endif
#include <vector>
#include <queue>
#include <iostream>
#include <atomic> // c++11

#define DWORD__                t_CKUINT
#define SINT__                 t_CKINT
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
    #ifndef __DISABLE_THREADS__
    XMutex m_mutex;
    #endif

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

#ifndef __DISABLE_THREADS__
    // added | 1.5.1.5 (ge & andrew) twilight zone
    XMutex m_mutex;
#endif
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
    void get_most_recent( SAMPLE * buffer, t_CKINT num_elem );

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
        // allocate
        m_elements = new T[m_numElements];
    }

    ~CircularBuffer()
    {
        CK_SAFE_DELETE_ARRAY( m_elements );
        // if(m_elements != NULL)
        // {
        //    delete[] m_elements;
        //    m_elements = NULL;
        // }
    }

    // put one element
    // returns number of elements successfully put
    size_t put(T element)
    {
        // check for overflow
        if( (m_write + 1)%m_numElements == m_read )
        {
            // no space | 1.5.0.1 (ge) added log message
            EM_log( CK_LOG_WARNING, "[circular-buffer]: buffer full, dropping items!" );
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
// name: FinalRingBuffer
// desc: hopefully this will be the last lock-free queue we need to implement...
//       hopefully not like FinalFantasy which has sequels
//
// added 1.5.2.5 (ge) | adapted from:
// Lock-Free Ring Buffer (LFRB) for embedded systems
// GitHub: https://github.com/QuantumLeaps/lock-free-ring-buffer
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
/* the following FinalBufferAtomic type is assumed to be "atomic" in a
* target CPU, meaning that the CPU needs to write the whole FinalBufferAtomic
* in one machine instruction. An example of violating this assumption would
* be an 8-bit CPU, which writes uint16_t (2-bytes) in 2 machine instructions.
* For such 8-bit CPU, the maximum size of FinalBufferAtomic would be uint8_t
* (1-byte).
*
* Another case of violating the "atomic" writes to FinalBufferAtomic type
* would be misalignment of a FinalBufferAtomic variable in memory, which could
* cause the compiler to generate multiple instructions to write a
* FinalBufferAtomic value. Therefore, it is further assumed that all
* FinalBufferAtomic variables in the following FinalBuffer struct *are*
* correctly aligned for "atomic" access. In practice, most C compilers
* should provide such natural alignment (by inserting some padding into the
* ::FinalRingBuffer class/struct, if necessary). */
//-----------------------------------------------------------------------------
typedef std::atomic_ulong FinalRingBufferAtomic;
//-----------------------------------------------------------------------------
template <typename T>
class FinalRingBuffer
{
public:
    FinalRingBuffer();
    ~FinalRingBuffer();
    // reset length of buffer (capacity)
    void init( t_CKUINT capacity );

public:
    bool put( T & element );
    bool get( T * pElement );
    bool more();

protected:
    /* the type of ring buffer elements is not critical for the lock-free
    * operation and does not need to be "atomic". For example, it can be
    * an integer type (uint16_t, uint32_t, uint64_t), a pointer type,
    * or even a struct type. */
    T * m_buf; // pointer to the start of the ring buffer
    FinalRingBufferAtomic m_end;  // offset of the end of the ring buffer
    FinalRingBufferAtomic m_head; // offset to where next el. will be inserted
    FinalRingBufferAtomic m_tail; // offset of where next el. will be removed
};

template <typename T>
FinalRingBuffer<T>::FinalRingBuffer()
{
    m_buf = NULL;
    m_end = m_head = m_tail = 0;
}

template <typename T>
void FinalRingBuffer<T>::init( t_CKUINT capacity )
{
    // dealloc
    CK_SAFE_DELETE_ARRAY( m_buf );
    // allocate
    m_buf = new T[capacity];
    m_end = capacity;
    m_head = m_tail = 0;
}

template <typename T>
FinalRingBuffer<T>::~FinalRingBuffer()
{
    CK_SAFE_DELETE_ARRAY( m_buf );
    m_end = m_head = m_tail = 0;
}

template <typename T>
bool FinalRingBuffer<T>::put( T & element )
{
    // check
    if( !m_buf ) return false;

    // local head variable (need to be atomic?)
    t_CKUINT head = m_head + 1;
    if( head == m_end ) head = 0;

    // buffer NOT full?
    if( head != m_tail )
    {
        // copy the element into the buffer
        m_buf[m_head] = element;
        // update the head to a *valid* index
        m_head = head;
        // element placed in the buffer
        return true;
    }

    // element NOT placed in the buffer
    return false;
}

template <typename T>
bool FinalRingBuffer<T>::get( T * pElement )
{
    // check for NULL
    if( !m_buf || !pElement ) return false;

    // local tail variable (need to be atomic?)
    t_CKUINT tail = m_tail;
    // ring buffer NOT empty?
    if( m_head != tail )
    {
        // copy element
        *pElement = m_buf[tail];
        // advance local tail
        ++tail; if( tail == m_end ) { tail = 0; }
        // update the tail to a *valid* index
        m_tail = tail;
        // gotten
        return true;
    }

    // not gotten
    return false;
}

template <typename T>
bool FinalRingBuffer<T>::more()
{
    // has elements?
    return m_head != m_tail;
}




////-----------------------------------------------------------------------------
//// name: class XCircleBuffer
//// desc: templated circular buffer class (different impl from ge-X-lib)
////       NOTE: the lock-free queue DOES NOT WORK; can crash; keeping for study
////       NOTE: use FinalRingBuffer instead | 1.5.2.5 (ge)
////-----------------------------------------------------------------------------
//template <typename T>
//class XCircleBuffer
//{
//public:
//    XCircleBuffer( long length = 0 );
//    ~XCircleBuffer();
//
//public:
//    // reset length of buffer (capacity)
//    void init( long length );
//    // get length
//    long length() const;
//    // clear (does no explicit memory management)
//    void clear();
//
//public:
//    // put an element into the buffer - the item will be copied
//    // NOTE: if over-capacity, will discard least recently put item
//    void put( const T & item );
//    // get next item (FIFO)
//    bool get( T * pItem );
//    // number of valid elements in buffer
//    long numElements() const;
//    // are there more elements?
//    bool more() const;
//    // get elements without advancing - returns number of valid elements
//    long peek( T * array, long numItems, unsigned long stride = 0 );
//    // pop
//    long pop( long numItems = 1 );
//
//protected: // helper functions
//    inline void advanceWrite();
//    inline void advanceRead();
//
//protected:
//    // the buffer
//    T * m_buffer;
//    // the buffer length (capacity)
//    std::atomic_ulong m_length;
//    // write index
//    std::atomic_ulong m_writeIndex;
//    // read index
//    std::atomic_ulong m_readIndex;
//    // num elements
//    std::atomic_ulong m_numElements;
//};
//
//
//
//
////-----------------------------------------------------------------------------
//// name: XCircleBuffer()
//// desc: constructor
////-----------------------------------------------------------------------------
//template <typename T>
//XCircleBuffer<T>::XCircleBuffer( long length )
//{
//    // zero out first
//    m_buffer = NULL;
//    m_length = m_readIndex = m_writeIndex = m_numElements = 0;
//
//    // call init
//    this->init( length );
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: ~XCircleBuffer
//// desc: destructor
////-----------------------------------------------------------------------------
//template <typename T>
//XCircleBuffer<T>::~XCircleBuffer()
//{
//
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: init()
//// desc: reset length of buffer
////-----------------------------------------------------------------------------
//template <typename T>
//void XCircleBuffer<T>::init( long length )
//{
//    // clean up is necessary
//    if( m_buffer )
//    {
//        // delete array - should call destructors and zero out variable
//        CK_SAFE_DELETE_ARRAY( m_buffer );
//        // zero out
//        m_length = m_readIndex = m_writeIndex = m_numElements = 0;
//    }
//
//    // sanity check
//    if( length < 0 )
//    {
//        // doh
//        std::cerr << "[XCircleBuffer]: error invalid length '"
//                  << length << "' requested" << std::endl;
//
//        return;
//    }
//
//    // check for zero length
//    if( length == 0 ) return;
//
//    // allocate
//    m_buffer = new T[length];
//    // check
//    if( m_buffer == NULL )
//    {
//        // doh
//        std::cerr << "[XCircleBuffer]: failed to allocate buffer of length '"
//                  << length << "'..." << std::endl;
//
//        return;
//    }
//
//    // save
//    m_length = length;
//    // zero out
//    m_readIndex = m_writeIndex = m_numElements = 0;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: length()
//// desc: get length
////-----------------------------------------------------------------------------
//template <typename T>
//long XCircleBuffer<T>::length() const
//{
//    return m_length;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: clear()
//// desc: clear (does no explicit memory management)
////-----------------------------------------------------------------------------
//template <typename T>
//void XCircleBuffer<T>::clear()
//{
//    // zero out
//    m_readIndex = m_writeIndex = m_numElements = 0;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: advanceWrite()
//// desc: helper to advance write index
////-----------------------------------------------------------------------------
//template <typename T>
//void XCircleBuffer<T>::advanceWrite()
//{
//    // increment
//    m_writeIndex++;
//
//    // check for bounds
//    if( m_writeIndex >= m_length )
//    {
//        // wrap
//        m_writeIndex -= m_length;
//    }
//
//    // increment count
//    m_numElements++;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: advanceRead()
//// desc: helper to advance read index
////-----------------------------------------------------------------------------
//template <typename T>
//void XCircleBuffer<T>::advanceRead()
//{
//    // increment
//    m_readIndex++;
//
//    // check for bounds
//    if( m_readIndex >= m_length )
//    {
//        // wrap
//        m_readIndex -= m_length;
//    }
//
//    // decrement count
//    m_numElements--;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: put()
//// desc: put an element into the buffer - the item will be copied
////       if over-capacity, will discard least recently put item
////-----------------------------------------------------------------------------
//template <typename T>
//void XCircleBuffer<T>::put( const T & item )
//{
//    // sanity check
//    if( m_buffer == NULL ) return;
//
//    // copy it
//    m_buffer[m_writeIndex] = item;
//
//    // advance write index
//    advanceWrite();
//
//    // if read and write pointer are the same, over-capacity
//    if( m_writeIndex == m_readIndex )
//    {
//        // warning | 1.5.0.1 (ge) make this a log message
//        EM_log( CK_LOG_WARNING, "[circular-buffer]: buffer full, dropping items!" );
//        // advance read!
//        advanceRead();
//    }
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: numElements()
//// desc: get number of valid elements in buffer
////-----------------------------------------------------------------------------
//template <typename T>
//long XCircleBuffer<T>::numElements() const
//{
//    // return our count
//    return m_numElements;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: hasMore()
//// desc: are there more elements?
////-----------------------------------------------------------------------------
//template <typename T>
//bool XCircleBuffer<T>::more() const
//{
//    return m_numElements > 0;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: peek
//// desc: get elements without advancing - returns number returned
////-----------------------------------------------------------------------------
//template <typename T>
//long XCircleBuffer<T>::peek( T * array, long numItems, unsigned long stride )
//{
//    // sanity check
//    if( m_buffer == NULL ) return 0;
//
//    // sanity check (so the wrap can be sure to land inbounds)
//    if( stride >= m_length ) return 0;
//
//    // count
//    long count = 0;
//    // actual count, taking stride out of the equation
//    long actualCount = 0;
//
//    // starting index
//    long index = m_writeIndex - 1;
//    if( index < 0 ) index += m_length;
//
//    // while need more but haven't reached write index...
//    while( (count < numItems) && (count < m_numElements) )
//    {
//        // copy
//        array[actualCount] = m_buffer[index];
//        // increment
//        count++; count += stride;
//        // advance
//        index--; index -= stride;
//        // actual count, don't stride
//        actualCount++;
//        // wrap
//        if( index < 0 ) index += m_length;
//    }
//
//    // reverse contents
//    for( int i = 0; i < actualCount/2; i++ )
//    {
//        T v = array[i];
//        array[i] = array[actualCount-1-i];
//        array[actualCount-1-i] = v;
//    }
//
//    return actualCount;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: pop()
//// desc: pop one or more elements
////-----------------------------------------------------------------------------
//template <typename T>
//long XCircleBuffer<T>::pop( long numItems )
//{
//    // sanity check
//    if( m_buffer == NULL ) return 0;
//
//    // count
//    long count = 0;
//
//    // while there is more to pop and need to pop more
//    while( more() && count < numItems )
//    {
//        // advance read
//        advanceRead();
//        // increment count
//        count++;
//    }
//
//    return count;
//}
//
//
//
//
////-----------------------------------------------------------------------------
//// name: get()
//// desc: get next read element
////-----------------------------------------------------------------------------
//template <typename T>
//bool XCircleBuffer<T>::get( T * result )
//{
//    // sanity check
//    if( m_buffer == NULL || m_readIndex == m_writeIndex ) return false;
//
//    // get item to read
//    *result = m_buffer[m_readIndex];
//    // advance read
//    advanceRead();
//
//    return true;
//}




#endif
