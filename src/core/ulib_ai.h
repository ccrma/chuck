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
// file: ulib_ai.h
// desc: class library for libAI -- part of ChAI (ChucK for AI)
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//         Yikai Li (yikaili@stanford.edu)
// date: Fall 2022
//-----------------------------------------------------------------------------
#ifndef __ULIB_AI_H__
#define __ULIB_AI_H__

#include "chuck_dl.h"
#include "chuck_errmsg.h"




// query
DLL_QUERY libai_query( Chuck_DL_Query * QUERY );

//-----------------------------------------------------------------------------
// name: class ChaiMatrixFast | 1.4.2.0 (ge) added
// desc: a templated fast access but not bound-checked 2D matrix
//       allows arbitrary 2D arrays of arbitrary and non-constant dimensions
//       to be allocated locally, e.g., int matrix[xDim][yDim] -- which is not
//       valid syntax with some compilers.
//-----------------------------------------------------------------------------
template<class T>
class ChaiMatrixFast
{
public:
    ChaiMatrixFast()
    {
        // zero out
        m_matrix = NULL;
        // zero out
        m_xDim = m_yDim = 0;
    }

    ChaiMatrixFast( t_CKINT xDim, t_CKINT yDim )
    {
        // zero out
        m_matrix = NULL;
        // allocate
        allocate( xDim, yDim );
    }

    ~ChaiMatrixFast()
    {
        // deallocate
        cleanup();
    }

    t_CKBOOL allocate( t_CKINT xDim, t_CKINT yDim )
    {
        // delete if necessary
        if( m_matrix != NULL ) cleanup();

        // allocate
        m_matrix = new T[xDim * yDim];
        // check
        if( !m_matrix )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: ChaiMatrixFast allocation failure: %s, %d x %d...\n",
                               typeid( T ).name(), xDim, yDim );
            // bail
            return FALSE;
        }
        // remember
        m_xDim = xDim;
        m_yDim = yDim;

        // done
        return TRUE;
    }

    // get element value
    T & v( t_CKINT x, t_CKINT y )
    {
        // at least do a check
        if( x < 0 || x >= m_xDim || y < 0 || y >= m_yDim )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: ChaiMatrixFast out of bound: %s[%d][%d]...\n",
                               typeid( T ).name(), x, y );
            // halt
            // TODO: this should throw (a shred-level, probably) exception
            assert( FALSE );
        }

        return m_matrix[x * m_yDim + y];
    }

    // overloaded operator
    T & operator()( t_CKINT x, t_CKINT y )
    {
        return v( x, y );
    }

    // get dimensions
    t_CKINT xDim() const
    { return m_xDim; }
    t_CKINT yDim() const
    { return m_yDim; }

    void cleanup()
    {
        // reclaim array memory and zero the pointer
        SAFE_DELETE_ARRAY( m_matrix );
        // zero out
        m_xDim = m_yDim = 0;
    }

protected:
    T * m_matrix;
    t_CKINT m_xDim;
    t_CKINT m_yDim;
};

//-----------------------------------------------------------------------------
// name: class ChaiVectorFast | 1.4.2.0 (ge) added
// desc: a templated fast access but not bound-checked 1D array
//       allows arbitrary arrays of arbitrary and non-constant dimension
//       to be allocated locally, e.g., int array[dim] -- which is not
//       valid syntax with some compilers.
//-----------------------------------------------------------------------------
template<class T>
class ChaiVectorFast
{
public:
    ChaiVectorFast()
    {
        // zero out
        m_vector = NULL;
        // zero out
        m_length = 0;
    }

    ChaiVectorFast( t_CKINT length )
    {
        // zero out
        m_vector = NULL;
        // allocate
        allocate( length );
    }

    ~ChaiVectorFast()
    {
        // deallocate
        cleanup();
    }

    t_CKBOOL allocate( t_CKINT length )
    {
        // delete if necessary
        if( m_vector != NULL ) cleanup();

        // allocate
        m_vector = new T[length];
        // check
        if( !m_vector )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: ChaiVectorFast allocation failure: %s[%d]...\n",
                               typeid( T ).name(), length );
            // bail
            return FALSE;
        }
        // remember
        m_length = length;

        // done
        return TRUE;
    }

    t_CKUINT length()
    { return m_length; }

    t_CKINT size()
    { return m_length; }

    T getMax()
    {
        T max = m_vector[0];
        for( t_CKINT i = 1; i < m_length; i++ )
        {
            if( m_vector[i] > max )
            {
                max = m_vector[i];
            }
        }
        return max;
    }

    T & operator[]( t_CKINT i )
    {
        // at least do a check
        if( i < 0 || i >= m_length )
        {
            // error
            CK_FPRINTF_STDERR( "[chuck]: ChaiVectorFast out of bound: %s[%d] (capacity=%d)...\n",
                               typeid( T ).name(), i, m_length );
            // TODO: this should throw (a shred-level, probably) exception
            assert( FALSE );
        }

        return m_vector[i];
    }

    // get element value
    T & v( t_CKINT i )
    {
        return ( *this )[i];
    }

    void cleanup()
    {
        // reclaim array memory and zero the pointer
        SAFE_DELETE_ARRAY( m_vector );
        // zero out
        m_length = 0;
    }

public:
    T * m_vector;
    t_CKINT m_length;
};

#endif

#ifndef _KD_TREE_H
#define _KD_TREE_H

#include <float.h>
// #define DBL_EPSILON 2.2204460492503131e-16

#define KDTREE_MAX_LEVEL 64
#define KDTREE_LEFT_INDEX 0
#define KDTREE_RIGHT_INDEX 1

typedef struct knn_list
{
    struct kdnode * node;
    double distance;
    struct knn_list * prev;
    struct knn_list * next;
} knn_list_t;

typedef struct kdnode
{
    long coord_index;
    double * coord;
    struct kdnode * left;
    struct kdnode * right;
    t_CKINT r;
} kdnode_t;

typedef struct kdtree
{
    struct kdnode * root;
    size_t count;
    size_t capacity;
    double * coords;
    double ** coord_table;
    long * coord_indexes;
    unsigned char * coord_deleted;
    unsigned char * coord_passed;
    struct knn_list knn_list_head;
    t_CKINT dim;
    t_CKINT knn_num;
} kdtree_t;

struct kdtree * kdtree_init( t_CKINT dim );
void kdtree_insert( struct kdtree * tree, double * coord );
void kdtree_rebuild( struct kdtree * tree );
void kdtree_knn_search( struct kdtree * tree, double * coord, t_CKINT k );
void kdtree_destroy( struct kdtree * tree );
void kdtree_dump( struct kdtree * tree );

#endif /* _KD_TREE_H */
