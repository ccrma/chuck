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
// desc: Music and AI class library; part of ChAI (ChucK for AI)
//
// date: Fall 2022 -- present
// authors: Ge Wang (ge@ccrma.stanford.edu)
//          Yikai Li (yikaili@stanford.edu)
//
// (HISTORICAL NOTE)
// Much of ChAI and its ways of thinking and doing can be foundationally
// attributed to the work of Dr. Rebecca Fiebrink, her landmark Ph.D. Thesis,
// /Real-time Human Interaction With Supervised Learning Algorithms for Music
// Composition and Performance/ (2011) [1], the Wekinator framework [2], her
// teaching at the intersections of AI, HCI, and Music [3], as well as earlier
// collaborations between Rebecca and Ge [4,5] including SMIRK [6] (Small Musical
// Information Retrieval toolKit; 2008; MARSYAS and the MIR work of Dr. George
// Tzanetakis; early efforts in on-the-fly learning), the unit analyzer
// framework [7] (2007; afford real-time audio feature extraction). All of
// these have directly or indirectly contributed the creation of ChAI.
// Additionally, ChAI benefitted from the teaching and design philosophy of
// Dr. Perry R. Cook (Ph.D. advisor to Rebecca, George, and Ge at Princeton,
// and ChucK co-author), who argued for real-time human interaction, parametric
// sound synthesis, and the importance of play in the design of musical tools.
//
// [1] Fiebrink, Rebecca. 2011. /Real-time Human Interaction With Supervised
// Learning Algorithms for Music Composition and Performance/ Ph.D. Thesis.
// Princeton University. https://www.cs.princeton.edu/techreports/2010/891.pdf
//
// [2] http://www.wekinator.org/
// Wekinator | Software for real-time, interactive machine learning
//
// [3] Fiebrink, Rebecca. "Machine Learning for Musicians and Artists" MOOC
// https://www.kadenze.com/courses/machine-learning-for-musicians-and-artists/info
//
// [4] Fiebrink, R., G. Wang, P. R. Cook. 2008. "Foundations for On-the-fly
// Learning in the ChucK Programming Language." /International Computer Music
// Conference/. https://mcd.stanford.edu/publish/files/2008-icmc-learning.pdf
//
// [5] Fiebrink, R., G. Wang, P. R. Cook. 2008. "Support for Music Information
// Retrieval in the ChucK Programming Language." /International Conference
// on Music Information Retrieval/ (ISMIR).
// https://mcd.stanford.edu/publish/files/2008-ismir-proto.pdf
//
// [6] http://smirk.cs.princeton.edu/
// sMIRk | Small Music Information Retrieval toolKit
//
// [7] Tzanetakis, G. and P. R. Cook. 2000 "MARSYAS: A Framework for Audio
// Analysis." Organised Sound. 4:(3)
//
// [8] Tzanetakis, G. and P. R. Cook. 2002 "Musical Genre Classification of
// Audio Signals." IEEE Transaction on Speech and Audio Processing. 10(5).
//
// [9] Wang, G., R. Fiebrink, P. R. Cook. 2007. "Combining Analysis and
// Synthesis in the ChucK Programming Language." /International Computer
// Music Conference/. https://mcd.stanford.edu/publish/files/2007-icmc-uana.pdf
//-----------------------------------------------------------------------------
#ifndef __ULIB_AI_H__
#define __ULIB_AI_H__

#include "chuck_dl.h"
#include "chuck_errmsg.h"
#include "util_math.h"




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

    void shuffle()
    {
        T temp;
        t_CKINT j;
        for( t_CKINT i = m_length - 1; i > 0; i-- )
        {
            j = ck_random() % ( i + 1 );
            temp = m_vector[i];
            m_vector[i] = m_vector[j];
            m_vector[j] = temp;
        }
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

    void setAll( T value )
    {
        // iterate
        for( t_CKINT i = 0; i < m_length; i++ )
        {
            // set to value
            m_vector[i] = value;
        }
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
