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
// file: ulib_ai.cpp
// desc: class library for libAI -- part of ChAI (ChucK for AI)
//
// author: Ge Wang (ge@ccrma.stanford.edu)
//         Yikai Li (yikaili@stanford.edu)
// date: Fall 2022
//-----------------------------------------------------------------------------
#include "ulib_ai.h"
#include "chuck_type.h"
#include "util_math.h"
#include <math.h>

#include <string>
#include <vector>
#include <map>
using namespace std;





// 1.4.2.0 (yikai) added SVM
CK_DLL_CTOR( SVM_ctor );
CK_DLL_DTOR( SVM_dtor );
CK_DLL_MFUN( SVM_train );
CK_DLL_MFUN( SVM_predict );
// offset
static t_CKUINT SVM_offset_data = 0;

// 1.4.2.0 (yikai) added KNN
CK_DLL_CTOR( KNN_ctor );
CK_DLL_DTOR( KNN_dtor );
CK_DLL_MFUN( KNN_train );
CK_DLL_MFUN( KNN_predict );
// offset
static t_CKUINT KNN_offset_data = 0;

// 1.4.2.0 (yikai) added HMM
CK_DLL_CTOR( HMM_ctor );
CK_DLL_DTOR( HMM_dtor );
CK_DLL_MFUN( HMM_init );
CK_DLL_MFUN( HMM_train );
CK_DLL_MFUN( HMM_generate );
// offset
static t_CKUINT HMM_offset_data = 0;

// 1.4.2.1 (yikai) added Word2Vec
CK_DLL_CTOR( Word2Vec_ctor );
CK_DLL_DTOR( Word2Vec_dtor );
CK_DLL_MFUN( Word2Vec_load );
CK_DLL_MFUN( Word2Vec_getMostSimilar );
CK_DLL_MFUN( Word2Vec_getByVector );
CK_DLL_MFUN( Word2Vec_getVector );
CK_DLL_MFUN( Word2Vec_getDictionarySize );
CK_DLL_MFUN( Word2Vec_getDictionaryDim );
// offset
static t_CKUINT Word2Vec_offset_data = 0;




//-----------------------------------------------------------------------------
// name: libai_query()
// desc: add class definition to chuck internal type system
//-----------------------------------------------------------------------------
DLL_QUERY libai_query( Chuck_DL_Query * QUERY )
{
    // get environment reference
    Chuck_Env * env = QUERY->env();
    // set module name
    QUERY->setname( QUERY, "AI" );
    // function
    Chuck_DL_Func * func = NULL;
    // documentation string
    std::string doc;

    //---------------------------------------------------------------------
    // init as base class: SVM
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc = "a support vector machine (SVM) utility trains a model and predicts output based on new input";

    // begin class definition
    if( !type_engine_import_class_begin( env, "SVM", "Object", env->global(), SVM_ctor, SVM_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    SVM_offset_data = type_engine_import_mvar( env, "float", "@SVM_data", FALSE );
    if( SVM_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", SVM_train );
    func->add_arg( "float[][]", "x" );
    func->add_arg( "float[][]", "y" );
    func->doc = "Train the SVM model with the given samples 'x' and 'y'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "int", "predict", SVM_predict );
    func->add_arg( "float[]", "x" );
    func->add_arg( "float[]", "y" );
    func->doc = "Predict the output 'y' given the input 'x'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: KNN
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc =
        "a supervised learning utility that predicts probabilities of class membership based on distances from a test input to its k nearest neighbors.";

    // begin class definition
    if( !type_engine_import_class_begin( env, "KNN", "Object", env->global(), KNN_ctor, KNN_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    KNN_offset_data = type_engine_import_mvar( env, "float", "@KNN_data", FALSE );
    if( KNN_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", KNN_train );
    func->add_arg( "float[][]", "x" );
    func->add_arg( "int[]", "y" );
    func->doc = "Train the KNN model with the given samples 'x' and corresponding labels 'y'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "int", "predict", KNN_predict );
    func->add_arg( "int", "k" );
    func->add_arg( "float[]", "x" );
    func->add_arg( "float[]", "prob" );
    func->doc =
        "Predict the output probabilities ('prob') given unlabeled test input 'x' based on distances to 'k' nearest neighbors.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: HMM
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc = "a hidden markov model (HMM) utility that generates a sequence of observations based on the training data.";

    // begin class definition
    if( !type_engine_import_class_begin( env, "HMM", "Object", env->global(), HMM_ctor, HMM_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    HMM_offset_data = type_engine_import_mvar( env, "float", "@HMM_data", FALSE );
    if( HMM_offset_data == CK_INVALID_OFFSET ) goto error;

    // init
    func = make_new_mfun( "int", "load", HMM_init );
    func->add_arg( "float[]", "initiailDistribution" );
    func->add_arg( "float[][]", "transitionMatrix" );
    func->add_arg( "float[][]", "emissionMatrix" );
    func->doc =
        "Initialize the HMM model with the given initial state distribution, transition matrix, and emission matrix.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // train
    func = make_new_mfun( "int", "train", HMM_train );
    func->add_arg( "int", "numStates" );
    func->add_arg( "int", "numEmissions" );
    func->add_arg( "int[]", "observations" );
    func->doc = "Train the HMM model with the given observations.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // generate
    func = make_new_mfun( "int", "generate", HMM_generate );
    func->add_arg( "int", "length" );
    func->add_arg( "int[]", "output" );
    func->doc = "Generate a sequence of observations of the given length.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: Word2Vec
    // 1.4.2.1 added by Yikai Li, Spring 2023
    //---------------------------------------------------------------------
    // doc string
    doc = "a word embeddings utility that maps words to vectors; can load a model and perform similiarity retrieval.";

    // begin class definition
    if( !type_engine_import_class_begin( env,
                                         "Word2Vec",
                                         "Object",
                                         env->global(),
                                         Word2Vec_ctor,
                                         Word2Vec_dtor,
                                         doc.c_str() ) )
        return FALSE;

    // data offset
    Word2Vec_offset_data = type_engine_import_mvar( env, "float", "@Word2Vec_data", FALSE );
    if( Word2Vec_offset_data == CK_INVALID_OFFSET ) goto error;

    // load
    func = make_new_mfun( "int", "load", Word2Vec_load );
    func->add_arg( "string", "path" );
    func->doc = "Load pre-trained word embedding model from the given path.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getMostSimilar
    func = make_new_mfun( "void", "similar", Word2Vec_getMostSimilar );
    func->add_arg( "string", "word" );
    func->add_arg( "int", "k" );
    func->add_arg( "string[]", "output" );
    func->doc = "Get the k most similar words to the given word.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getByVector
    func = make_new_mfun( "void", "similar", Word2Vec_getByVector );
    func->add_arg( "float[]", "vec" );
    func->add_arg( "int", "k" );
    func->add_arg( "string[]", "output" );
    func->doc = "Get the k most similar words to the given vector.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getVector
    func = make_new_mfun( "void", "getVector", Word2Vec_getVector );
    func->add_arg( "string", "word" );
    func->add_arg( "float[]", "output" );
    func->doc = "Get the vector of the given word.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", Word2Vec_getDictionarySize );
    func->doc = "Get number of words in dictionary.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // dim
    func = make_new_mfun( "int", "dim", Word2Vec_getDictionaryDim );
    func->doc = "Get number of dimensions for word embedding.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    return TRUE;

    error:
    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}


//-----------------------------------------------------------------------------
// name: chuck2chai()
// desc: chuck array (assuming float[][]) conversion to new chai matrix
//-----------------------------------------------------------------------------
ChaiMatrixFast<t_CKFLOAT> * chuck2chai( Chuck_Array4 & array )
{
    t_CKINT rows = array.size();
    Chuck_Array8 * row = (Chuck_Array8 *)array.m_vector[0];
    t_CKINT cols = row->size();
    ChaiMatrixFast<t_CKFLOAT> * matrix = new ChaiMatrixFast<t_CKFLOAT>( rows, cols );
    for( t_CKINT i = 0; i < rows; i++ )
    {
        row = (Chuck_Array8 *)array.m_vector[i];
        for( t_CKINT j = 0; j < cols; j++ )
        {
            matrix->v( i, j ) = row->m_vector[j];
        }
    }
    return matrix;
}


//-----------------------------------------------------------------------------
// name: chuck2chai()
// desc: chuck array (float []) conversion to new chai vector
//-----------------------------------------------------------------------------
ChaiVectorFast<t_CKFLOAT> * chuck2chai( Chuck_Array8 & array )
{
    t_CKINT size = array.size();
    ChaiVectorFast<t_CKFLOAT> * vector = new ChaiVectorFast<t_CKFLOAT>( size );
    for( t_CKINT i = 0; i < size; i++ )
    {
        vector->v( i ) = array.m_vector[i];
    }
    return vector;
}




//-----------------------------------------------------------------------------
// name: struct SVM_Object
// desc: SVM implementation | added 1.4.2.0 (yikai)
//-----------------------------------------------------------------------------
struct SVM_Object
{
public:

    // constructor
    SVM_Object()
    {
        w = new ChaiMatrixFast<t_CKFLOAT>();
    }

    // destructor
    ~SVM_Object()
    {
        SAFE_DELETE( w );
    }

    // train
    t_CKINT train( Chuck_Array4 & x_, Chuck_Array4 & y_ )
    {
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKINT x_dim = x_i->size();
        y_.get( 0, &v );
        Chuck_Array8 * y_i = (Chuck_Array8 *)v;
        t_CKINT y_dim = y_i->size();
        w->allocate( x_dim + 1, y_dim );
        // copy
        ChaiMatrixFast<t_CKFLOAT> x( n_sample, x_dim );
        ChaiMatrixFast<t_CKFLOAT> y( n_sample, x_dim );
        for( t_CKINT i = 0; i < n_sample; i++ )
        {
            x_.get( i, &v );
            x_i = (Chuck_Array8 *)v;
            for( t_CKINT j = 0; j < x_dim; j++ )
            {
                x_i->get( j, &x.v( i, j ) );
            }
            y_.get( i, &v );
            y_i = (Chuck_Array8 *)v;
            for( t_CKINT j = 0; j < y_dim; j++ )
            {
                y_i->get( j, &y.v( i, j ) );
            }
        }
        // compute svm
        ChaiMatrixFast<t_CKFLOAT> xtx( x_dim, x_dim );
        for( int i = 0; i < x_dim; i++ )
        {
            for( int j = 0; j < x_dim; j++ )
            {
                xtx.v( i, j ) = 0.0;
                for( int k = 0; k < n_sample; k++ )
                {
                    xtx.v( i, j ) += x.v( k, i ) * x.v( k, j );
                }
            }
        }
        // inverse matrix
        ChaiMatrixFast<t_CKFLOAT> xtx_inv( x_dim, x_dim );
        for( int i = 0; i < x_dim; i++ )
        {
            for( int j = 0; j < x_dim; j++ )
            {
                xtx_inv.v( i, j ) = 0.0;
                if( i == j )
                {
                    xtx_inv.v( i, j ) = 1.0;
                }
            }
        }
        for( int i = 0; i < x_dim; i++ )
        {
            t_CKFLOAT pivot = xtx.v( i, i );
            for( int j = 0; j < x_dim; j++ )
            {
                xtx.v( i, j ) /= pivot;
                xtx_inv.v( i, j ) /= pivot;
            }
        }
        for( int i = 0; i < x_dim; i++ )
        {
            for( int j = 0; j < x_dim; j++ )
            {
                if( i != j )
                {
                    t_CKFLOAT factor = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
                    for( int k = 0; k < x_dim; k++ )
                    {
                        xtx.v( j, k ) -= factor * xtx.v( i, k );
                        xtx_inv.v( j, k ) -= factor * xtx_inv.v( i, k );
                    }
                }
            }
        }
        // compute w
        for( int i = 0; i < x_dim; i++ )
        {
            for( int j = 0; j < y_dim; j++ )
            {
                w->v( i, j ) = 0.0;
                for( int k = 0; k < x_dim; k++ )
                {
                    for( int l = 0; l < n_sample; l++ )
                    {
                        w->v( i, j ) += xtx_inv.v( i, k ) * x.v( l, k ) * y.v( l, j );
                    }
                }
            }
        }
        // compute b
        ChaiVectorFast<t_CKFLOAT> b( y_dim );
        for( int i = 0; i < y_dim; i++ )
        {
            b.v( i ) = 0.0;
            for( int j = 0; j < n_sample; j++ )
            {
                b.v( i ) += y.v( j, i );
                for( int k = 0; k < x_dim; k++ )
                {
                    b.v( i ) -= w->v( k, i ) * x.v( j, k );
                }
            }
            b.v( i ) /= n_sample;
            w->v( x_dim, i ) = b.v( i );
        }
        return 0;
    }

    // predict
    t_CKINT predict( Chuck_Array8 & x_, Chuck_Array8 & y_ )
    {
        // init
        t_CKINT x_dim = x_.size();
        t_CKINT y_dim = y_.size();
        // copy
        ChaiVectorFast<t_CKFLOAT> x( x_dim );
        for( t_CKINT i = 0; i < x_dim; i++ )
        {
            x_.get( i, &x.v( i ) );
        }
        // compute
        ChaiVectorFast<t_CKFLOAT> y( y_dim );
        for( int i = 0; i < y_dim; i++ )
        {
            y.v( i ) = 0.0;
            for( int j = 0; j < x_dim; j++ )
            {
                y.v( i ) += w->v( j, i ) * x.v( j );
            }
            y.v( i ) += w->v( x_dim, i );
        }
        // copy
        for( t_CKINT i = 0; i < y_dim; i++ )
        {
            y_.set( i, y.v( i ) );
        }
        return 0;
    }

private:
    ChaiMatrixFast<t_CKFLOAT> * w;
};




//-----------------------------------------------------------------------------
// SVM c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( SVM_ctor )
{
    SVM_Object * svm = new SVM_Object();
    OBJ_MEMBER_UINT( SELF, SVM_offset_data ) = (t_CKUINT)svm;
}

CK_DLL_DTOR( SVM_dtor )
{
    SVM_Object * svm = (SVM_Object *)OBJ_MEMBER_UINT( SELF, SVM_offset_data );
    SAFE_DELETE( svm );
    OBJ_MEMBER_UINT( SELF, SVM_offset_data ) = 0;
}

CK_DLL_MFUN( SVM_train )
{
    // get object
    SVM_Object * svm = (SVM_Object *)OBJ_MEMBER_UINT( SELF, SVM_offset_data );
    // get args
    Chuck_Array4 * x = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * y = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // add
    svm->train( *x, *y );
}

CK_DLL_MFUN( SVM_predict )
{
    // get object
    SVM_Object * svm = (SVM_Object *)OBJ_MEMBER_UINT( SELF, SVM_offset_data );
    // get args
    Chuck_Array8 * x = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array8 * y = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // add
    svm->predict( *x, *y );
}




//-----------------------------------------------------------------------------
// name: struct KDTree
// desc: kd-tree implementation | added 1.4.2.0 (yikai)
//-----------------------------------------------------------------------------
struct kd_node_t
{
    ChaiVectorFast<t_CKFLOAT> * x;
    struct kd_node_t * left, * right;
};

struct KDTree
{
public:
    inline t_CKFLOAT dist( struct kd_node_t * a, struct kd_node_t * b )
    {
        t_CKFLOAT t, d = 0.0;
        for( t_CKINT i = 0; i < a->x->size(); i++ )
        {
            t = a->x->v( i ) - b->x->v( i );
            d += t * t;
        }
        return d;
    }

    inline void swap( struct kd_node_t * x, struct kd_node_t * y )
    {
        ChaiVectorFast<t_CKFLOAT> * t = x->x;
        x->x = y->x;
        y->x = t;
    }

    struct kd_node_t * find_median( struct kd_node_t * start, struct kd_node_t * end, t_CKINT idx )
    {
        if( end <= start ) return NULL;
        if( end == start + 1 ) return start;
        struct kd_node_t * p, * store, * md = start + ( end - start ) / 2;
        t_CKFLOAT pivot;
        while( 1 )
        {
            pivot = md->x->v( idx );
            swap( md, end - 1 );
            for( store = p = start; p < end; p++ )
            {
                if( p->x->v( idx ) < pivot )
                {
                    if( p != store )
                        swap( p, store );
                    store++;
                }
            }
            swap( store, end - 1 );
            if( store->x->v( idx ) == md->x->v( idx ) ) return md;
            if( store > md ) end = store;
            else start = store;
        }
    }

    struct kd_node_t * make_tree( struct kd_node_t * t, t_CKINT len, t_CKINT i, t_CKINT dim )
    {
        struct kd_node_t * n;
        if( !len ) return 0;
        if( ( n = find_median( t, t + len, i ) ) )
        {
            i = ( i + 1 ) % dim;
            n->left = make_tree( t, n - t, i, dim );
            n->right = make_tree( n + 1, t + len - ( n + 1 ), i, dim );
        }
        return n;
    }

    void nearest( struct kd_node_t * root,
                  struct kd_node_t * nd,
                  t_CKINT i,
                  t_CKINT dim,
                  struct kd_node_t ** best,
                  t_CKFLOAT * best_dist )
    {
        t_CKFLOAT d, dx, dx2;
        if( !root ) return;
        d = dist( nd, root );
        dx = nd->x->v( i ) - root->x->v( i );
        dx2 = dx * dx;
        if( !*best || d < *best_dist )
        {
            *best_dist = d;
            *best = root;
        }
        if( !*best_dist ) return;
        if( ++i >= dim ) i = 0;
        nearest( dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist );
        if( dx2 >= *best_dist ) return;
        nearest( dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist );
    }

private:
};




//-----------------------------------------------------------------------------
// name: struct KNN_Object
// desc: k-nearest neighbor implementation | added 1.4.2.0 (yikai)
//-----------------------------------------------------------------------------
struct KNN_Object
{
public:
    // constructor
    KNN_Object()
    {
        X = new ChaiMatrixFast<t_CKFLOAT>();
        Y = new ChaiVectorFast<t_CKUINT>();
    }

    // destructor
    ~KNN_Object()
    {
        SAFE_DELETE( X );
        SAFE_DELETE( Y );
    }

    // train
    t_CKINT train( Chuck_Array4 & x_, Chuck_Array4 & y_ )
    {
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKINT x_dim = x_i->size();
        X->allocate( n_sample, x_dim );
        Y->allocate( n_sample );
        // copy
        for( t_CKINT i = 0; i < n_sample; i++ )
        {
            x_.get( i, &v );
            x_i = (Chuck_Array8 *)v;
            for( t_CKINT j = 0; j < x_dim; j++ )
            {
                x_i->get( j, &X->v( i, j ) );
            }
            y_.get( i, &Y->v( i ) );
        }
        return 0;
    }

    // predict
    t_CKINT predict( t_CKINT k, Chuck_Array8 & x_, Chuck_Array8 & prob_ )
    {
        // init
        t_CKINT n_sample = Y->length();
        t_CKINT n_label = prob_.size();
        t_CKINT x_dim = x_.size();
        ChaiVectorFast<t_CKFLOAT> x( x_dim );
        ChaiVectorFast<t_CKFLOAT> dist( n_sample );
        ChaiVectorFast<t_CKFLOAT> prob( n_label );
        // copy
        for( t_CKINT i = 0; i < x_dim; i++ )
        {
            x_.get( i, &x.v( i ) );
        }
        // compute
        for( t_CKINT i = 0; i < n_sample; i++ )
        {
            dist.v( i ) = 0.0;
            for( t_CKINT j = 0; j < x_dim; j++ )
            {
                dist.v( i ) += pow( x.v( j ) - X->v( i, j ), 2 );
            }
        }
        for( t_CKINT i = 0; i < n_label; i++ )
        {
            prob.v( i ) = 0.0;
        }
        for( t_CKINT i = 0; i < k; i++ )
        {
            t_CKINT min_idx = 0;
            t_CKFLOAT min_dist = dist.v( 0 );
            for( t_CKINT j = 1; j < n_sample; j++ )
            {
                if( dist.v( j ) < min_dist )
                {
                    min_idx = j;
                    min_dist = dist.v( j );
                }
            }
            prob.v( Y->v( min_idx ) ) += 1.0;
            dist.v( min_idx ) = 1e10;
        }
        for( t_CKINT i = 0; i < n_label; i++ )
        {
            prob.v( i ) /= (t_CKFLOAT)k;
            prob_.set( i, prob.v( i ) );
        }
        return 0;
    }

private:
    ChaiMatrixFast<t_CKFLOAT> * X;
    ChaiVectorFast<t_CKUINT> * Y;
};




//-----------------------------------------------------------------------------
// KNN c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( KNN_ctor )
{
    KNN_Object * knn = new KNN_Object();
    OBJ_MEMBER_UINT( SELF, KNN_offset_data ) = (t_CKUINT)knn;
}

CK_DLL_DTOR( KNN_dtor )
{
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN_offset_data );
    SAFE_DELETE( knn );
    OBJ_MEMBER_UINT( SELF, KNN_offset_data ) = 0;
}

CK_DLL_MFUN( KNN_train )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN_offset_data );
    // get args
    Chuck_Array4 * x = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * y = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // train
    knn->train( *x, *y );
}

CK_DLL_MFUN( KNN_predict )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN_offset_data );
    // get args
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array8 * x = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array8 * y = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // predict
    knn->predict( k, *x, *y );
}




//-----------------------------------------------------------------------------
// name: struct HMM_Object
// desc: hidden markov model implementation | added 1.4.2.0 (yikai)
//-----------------------------------------------------------------------------
struct HMM_Object
{
public:
    // constructor
    HMM_Object() : initial( NULL ), transition( NULL ), emission( NULL )
    { }

    // destructor
    ~HMM_Object()
    {
        clear();
    }

    // clear
    void clear()
    {
        SAFE_DELETE( initial );
        SAFE_DELETE( transition );
        SAFE_DELETE( emission );
    }

    // init
    t_CKINT init( Chuck_Array8 & initial_, Chuck_Array4 & transition_, Chuck_Array4 & emission_ )
    {
        clear();
        initial = chuck2chai( initial_ );
        transition = chuck2chai( transition_ );
        emission = chuck2chai( emission_ );
        return 0;
    }

    void filter( ChaiVectorFast<t_CKINT> & y, ChaiMatrixFast<t_CKFLOAT> & phi, ChaiVectorFast<t_CKFLOAT> & c )
    {
        t_CKINT i, j, t, n = y.size(), N = emission->xDim(); // M = emission->yDim();
        ChaiVectorFast<t_CKFLOAT> z( N );
        c.v( 0 ) = 0;
        for( j = 0; j < N; j++ )
        {
            z.v( j ) = initial->v( j ) * emission->v( j, y.v( 0 ) );
            c.v( 0 ) += z.v( j );
        }
        for( j = 0; j < N; j++ )
            phi.v( 0, j ) = z.v( j ) / c.v( 0 );
        for( t = 1; t < n; t++ )
        {
            c.v( t ) = 0;
            for( j = 0; j < N; j++ )
            {
                z.v( j ) = 0;
                for( i = 0; i < N; i++ )
                    z.v( j ) += phi.v( t - 1, i ) * transition->v( i, j ) * emission->v( j, y.v( t ) );
                c.v( t ) += z.v( j );
            }
            for( j = 0; j < N; j++ )
                phi.v( t, j ) = z.v( j ) / c.v( t );
        }
    }

    void smoother( ChaiVectorFast<t_CKINT> & y, ChaiVectorFast<t_CKFLOAT> & c, ChaiMatrixFast<t_CKFLOAT> & beta )
    {
        t_CKINT i, j, t, n = y.size(), N = emission->xDim(); // M = emission->yDim();
        for( j = 0; j < N; j++ )
            beta.v( n - 1, j ) = 1;
        for( t = n - 2; t >= 0; t-- )
            for( i = 0; i < N; i++ )
            {
                beta.v( t, i ) = 0;
                for( j = 0; j < N; j++ )
                    beta.v( t, i ) += transition->v( i, j ) * emission->v( j, y.v( t + 1 ) ) * beta.v( t + 1, j );
                beta.v( t, i ) /= c.v( t + 1 );
            }
    }

    void randomTransitionKernel()
    {
        t_CKINT i, j, N = transition->xDim();
        t_CKFLOAT s;
        for( i = 0; i < N; i++ )
        {
            s = 0;
            for( j = 0; j < N; j++ )
            {
                transition->v( i, j ) = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
                s += transition->v( i, j );
            }
            for( j = 0; j < N; j++ )
            {
                transition->v( i, j ) /= s;
            }
        }
    }

    void randomEmissionKernel()
    {
        t_CKINT i, j, N = emission->xDim(), M = emission->yDim();
        t_CKFLOAT s;
        for( i = 0; i < N; i++ )
        {
            s = 0;
            for( j = 0; j < M; j++ )
            {
                emission->v( i, j ) = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
                s += emission->v( i, j );
            }
            for( j = 0; j < M; j++ )
            {
                emission->v( i, j ) /= s;
            }
        }
    }

    // train
    t_CKINT train( t_CKINT n_state, t_CKINT n_emission, Chuck_Array4 & observations )
    {
        clear();
        // init
        initial = new ChaiVectorFast<t_CKFLOAT>( n_state );
        transition = new ChaiMatrixFast<t_CKFLOAT>( n_state, n_state );
        emission = new ChaiMatrixFast<t_CKFLOAT>( n_state, n_emission );
        // train
        t_CKINT maxIt = 100;
        t_CKFLOAT tol = 1e-4;
        t_CKINT i, j, it, t, n = observations.size(), N = n_state, M = n_emission;
        t_CKFLOAT z, l = 0, change = tol + 1;
        initial->v( 0 ) = 1;
        for( i = 1; i < N; i++ )
            initial->v( i ) = 0;
        randomTransitionKernel();
        randomEmissionKernel();
        ChaiMatrixFast<t_CKFLOAT> phi( n, N );
        ChaiMatrixFast<t_CKFLOAT> beta( n, N );
        ChaiVectorFast<t_CKFLOAT> c( n );
        ChaiMatrixFast<t_CKFLOAT> A( N, M );
        ChaiVectorFast<t_CKFLOAT> s( N );
        ChaiMatrixFast<t_CKFLOAT> B( N, N );
        ChaiVectorFast<t_CKFLOAT> s2( N );
        ChaiVectorFast<t_CKINT> y( n );
        for( i = 0; i < n; i++ )
        {
            y.v( i ) = observations.m_vector[i];
        }
        for( it = 0; ( change > tol ) && ( it < maxIt ); it++ )
        {
            change = 0;
            filter( y, phi, c );
            smoother( y, c, beta );
            for( i = 0; i < N; i++ )
            {
                s.v( i ) = 0;
                s2.v( i ) = 0;
                for( j = 0; j < M; j++ )
                    A.v( i, j ) = 0;
                for( j = 0; j < N; j++ )
                    B.v( i, j ) = 0;
            }
            for( t = 0; t < n; t++ )
                for( i = 0; i < N; i++ )
                {
                    z = phi.v( t, i ) * beta.v( t, i );
                    A.v( i, y.v( t ) ) += z;
                    s.v( i ) += z;
                }
            for( i = 0; i < N; i++ )
                for( j = 0; j < M; j++ )
                {
                    change = ck_max( change, fabs( emission->v( i, j ) - A.v( i, j ) / s.v( i ) ) );
                    emission->v( i, j ) = A.v( i, j ) / s.v( i );
                }
            for( t = 1; t < n; t++ )
                for( i = 0; i < N; i++ )
                    for( j = 0; j < N; j++ )
                    {
                        z = phi.v( t - 1, i ) * transition->v( i, j ) * emission->v( j, y.v( t ) ) * beta.v( t, j )
                            / c.v( t );
                        B.v( i, j ) += z;
                        s2.v( i ) += z;
                    }
            for( i = 0; i < N; i++ )
                for( j = 0; j < N; j++ )
                {
                    change = ck_max( change, fabs( transition->v( i, j ) - B.v( i, j ) / s2.v( i ) ) );
                    transition->v( i, j ) = B.v( i, j ) / s2.v( i );
                }

        }
        for( i = 0; i < n; i++ )
            l += log( c.v( i ) );
        return 0;
    }

    // generate
    t_CKINT generate( t_CKINT length, Chuck_Array4 & output_ )
    {
        // compute
        t_CKINT state;
        t_CKFLOAT r;
        for( t_CKINT i = 0; i < length; i++ )
        {
            // state
            if( i == 0 )
            {
                r = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
                for( t_CKINT j = 0; j < initial->size(); j++ )
                {
                    if( r < initial->v( j ) )
                    {
                        state = j;
                        break;
                    }
                    r -= initial->v( j );
                }
            }
            else
            {
                r = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
                for( t_CKINT j = 0; j < transition->yDim(); j++ )
                {
                    if( r < transition->v( state, j ) )
                    {
                        state = j;
                        break;
                    }
                    r -= transition->v( state, j );
                }
            }
            // observation
            r = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
            for( t_CKINT j = 0; j < emission->yDim(); j++ )
            {
                if( r < emission->v( state, j ) )
                {
                    output_.set( i, j );
                    break;
                }
                r -= emission->v( state, j );
            }
        }
        return 0;
    }

private:
    ChaiVectorFast<t_CKFLOAT> * initial;
    ChaiMatrixFast<t_CKFLOAT> * transition;
    ChaiMatrixFast<t_CKFLOAT> * emission;
};




//-----------------------------------------------------------------------------
// HMM c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( HMM_ctor )
{
    HMM_Object * hmm = new HMM_Object();
    OBJ_MEMBER_UINT( SELF, HMM_offset_data ) = (t_CKUINT)hmm;
}

CK_DLL_DTOR( HMM_dtor )
{
    HMM_Object * hmm = (HMM_Object *)OBJ_MEMBER_UINT( SELF, HMM_offset_data );
    SAFE_DELETE( hmm );
    OBJ_MEMBER_UINT( SELF, HMM_offset_data ) = 0;
}

CK_DLL_MFUN( HMM_init )
{
    // get object
    HMM_Object * hmm = (HMM_Object *)OBJ_MEMBER_UINT( SELF, HMM_offset_data );
    // get args
    Chuck_Array8 * initial = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * transition = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * emission = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // init
    hmm->init( *initial, *transition, *emission );
}

CK_DLL_MFUN( HMM_train )
{
    // get object
    HMM_Object * hmm = (HMM_Object *)OBJ_MEMBER_UINT( SELF, HMM_offset_data );
    // get args
    t_CKINT n_state = GET_NEXT_INT( ARGS );
    t_CKINT n_emission = GET_NEXT_INT( ARGS );
    Chuck_Array4 * observations = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // train
    hmm->train( n_state, n_emission, *observations );
}

CK_DLL_MFUN( HMM_generate )
{
    // get object
    HMM_Object * hmm = (HMM_Object *)OBJ_MEMBER_UINT( SELF, HMM_offset_data );
    // get args
    t_CKINT length = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // generate
    hmm->generate( length, *output );
}




//-----------------------------------------------------------------------------
// name: struct W2V_Dictionary
// desc: struct for caching Word2Vec | added 1.4.2.1 (ge)
//-----------------------------------------------------------------------------
struct W2V_Dictionary
{
    t_CKINT dictionarySize, vectorLength;
    std::map<std::string, t_CKUINT> * key_to_index;
    ChaiVectorFast<std::string> * index_to_key;
    ChaiMatrixFast<t_CKFLOAT> * word_vectors;
    
    W2V_Dictionary() : key_to_index(NULL), index_to_key(NULL), word_vectors(NULL), dictionarySize(0), vectorLength(0)
    { }
    
    // clear
    void clear()
    {
        dictionarySize = 0;
        vectorLength = 0;
        SAFE_DELETE( key_to_index );
        SAFE_DELETE( index_to_key );
        SAFE_DELETE( word_vectors );
    }
};




//-----------------------------------------------------------------------------
// name: struct Word2Vec_Object
// desc: Word2Vec implementation | added 1.4.2.1 (yikai)
//-----------------------------------------------------------------------------
struct Word2Vec_Object
{
private:
    // static (shared) cache | added (ge) 1.4.2.1
    static map<string, W2V_Dictionary *> o_cache;
    
private:
    // pointer to dictionary
    W2V_Dictionary * dictionary;

public:
    // constructor
    Word2Vec_Object()
        : dictionary(NULL)
    { }

    // destructor
    ~Word2Vec_Object()
    {
        clear();
    }

    // clear
    void clear()
    {
        // just zero the pointer (no delete since we are caching)
        dictionary = NULL;
    }
    
    // get dictionary size
    t_CKINT getDictionarySize() const
    {
        return dictionary != NULL ? dictionary->dictionarySize : 0;
    }

    // get dictionary dimensions
    t_CKINT getDictionaryDim() const
    {
        return dictionary != NULL ? dictionary->vectorLength : 0;
    }

    // load
    t_CKINT load( Chuck_String & path )
    {
        // clear
        clear();
        
        // check cache
        if( o_cache.find(path.str()) != o_cache.end() )
        {
            // retrieve from cache
            dictionary = o_cache[path.str()];
            // done
            return true;
        }
        
        // open file
        std::ifstream fin( path.str() );
        if( !fin.is_open() )
            return false;
        
        // instantiate dictionary
        dictionary = new W2V_Dictionary();
        // read header
        fin >> dictionary->dictionarySize >> dictionary->vectorLength;
        // allocate
        dictionary->key_to_index = new std::map<std::string, t_CKUINT>();
        dictionary->index_to_key = new ChaiVectorFast<std::string>( dictionary->dictionarySize );
        dictionary->word_vectors = new ChaiMatrixFast<t_CKFLOAT>( dictionary->dictionarySize, dictionary->vectorLength );
        // read vectors
        std::string key;
        t_CKFLOAT value;
        for( t_CKINT i = 0; i < dictionary->dictionarySize; i++ )
        {
            fin >> key;
            dictionary->index_to_key->v( i ) = key;
            (*(dictionary->key_to_index))[key] = i;
            for( t_CKINT j = 0; j < dictionary->vectorLength; j++ )
            {
                fin >> value;
                dictionary->word_vectors->v( i, j ) = value;
            }
        }
        
        // close file
        fin.close();
        // add to cache
        o_cache[path.str()] = dictionary;
        
        return true;
    }

    // get nearest neighbors
    void getNearestNeighbors( ChaiVectorFast<t_CKFLOAT> & vector, t_CKINT topn, ChaiVectorFast<t_CKINT> & indices )
    {
        // allocate
        ChaiVectorFast<t_CKFLOAT> top_distances( topn );
        ChaiVectorFast<t_CKINT> top_indices( topn );
        // initialize
        for( t_CKINT i = 0; i < topn; i++ )
        {
            top_distances.v( i ) = 1e10;
            top_indices.v( i ) = 0;
        }
        // compute distances
        t_CKFLOAT distance, diff;
        for( t_CKINT i = 0; i < dictionary->dictionarySize; i++ )
        {
            distance = 0;
            for( t_CKINT j = 0; j < dictionary->vectorLength; j++ )
            {
                diff = vector.v( j ) - dictionary->word_vectors->v( i, j );
                distance += diff * diff;
            }
            for( t_CKINT j = 0; j < topn; j++ )
            {
                if( distance < top_distances.v( j ) )
                {
                    for( t_CKINT k = topn - 1; k > j; k-- )
                    {
                        top_distances.v( k ) = top_distances.v( k - 1 );
                        top_indices.v( k ) = top_indices.v( k - 1 );
                    }
                    top_distances.v( j ) = distance;
                    top_indices.v( j ) = i;
                    break;
                }
            }
        }
        // copy
        for( t_CKINT i = 0; i < topn; i++ )
        {
            indices.v( i ) = top_indices.v( i );
        }
    }

    // getMostSimilar
    void getMostSimilar( Chuck_String & word, t_CKINT topn, Chuck_Array4 & output_ )
    {
        // get index
        t_CKINT index = (*(dictionary->key_to_index))[word.str()];
        // get vector
        ChaiVectorFast<t_CKFLOAT> vector( dictionary->vectorLength );
        for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
            vector.v( i ) = dictionary->word_vectors->v( index, i );
        // get nearest neighbors
        ChaiVectorFast<t_CKINT> top_indices( topn );
        getNearestNeighbors( vector, topn, top_indices );
        // copy
        for( t_CKINT i = 0; i < topn; i++ )
        {
            ( (Chuck_String *)output_.m_vector[i] )->set( dictionary->index_to_key->v( top_indices.v( i ) ) );
        }
    }

    // getByVector
    void getByVector( Chuck_Array8 & vec_, t_CKINT topn, Chuck_Array4 & output_ )
    {
        // get vector
        ChaiVectorFast<t_CKFLOAT> vector( dictionary->vectorLength );
        for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
            vector.v( i ) = vec_.m_vector[i];
        // get nearest neighbors
        ChaiVectorFast<t_CKINT> top_indices( topn );
        getNearestNeighbors( vector, topn, top_indices );
        // copy
        for( t_CKINT i = 0; i < topn; i++ )
        {
            ( (Chuck_String *)output_.m_vector[i] )->set( dictionary->index_to_key->v( top_indices.v( i ) ) );
        }
    }

    // getVector
    void getVector( Chuck_String & word, Chuck_Array8 & output_ )
    {
        // get index
        t_CKINT index = (*(dictionary->key_to_index) )[word.str()];
        // copy
        for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
        {
            output_.m_vector[i] = dictionary->word_vectors->v( index, i );
        }
    }

    // print
    void print()
    {
        // TODO
    }

};

// static instantiation | added (ge) 1.4.2.1
map<string, W2V_Dictionary *> Word2Vec_Object::o_cache;




//-----------------------------------------------------------------------------
// Word2Vec c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Word2Vec_ctor )
{
    Word2Vec_Object * word2vec = new Word2Vec_Object();
    OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data ) = (t_CKUINT)word2vec;
}

CK_DLL_DTOR( Word2Vec_dtor )
{
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    SAFE_DELETE( word2vec );
    OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data ) = 0;
}

CK_DLL_MFUN( Word2Vec_load )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * path = GET_NEXT_STRING( ARGS );
    // load
    RETURN->v_int = word2vec->load( *path );
}

CK_DLL_MFUN( Word2Vec_getMostSimilar )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * word = GET_NEXT_STRING( ARGS );
    t_CKINT topn = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // getMostSimilar
    word2vec->getMostSimilar( *word, topn, *output );
}

CK_DLL_MFUN( Word2Vec_getByVector )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_Array8 * vec = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT topn = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // getByVector
    word2vec->getByVector( *vec, topn, *output );
}

CK_DLL_MFUN( Word2Vec_getVector )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * word = GET_NEXT_STRING( ARGS );
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // getVector
    word2vec->getVector( *word, *output );
}

CK_DLL_MFUN( Word2Vec_getDictionarySize )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get size
    RETURN->v_int = word2vec->getDictionarySize();
}

CK_DLL_MFUN( Word2Vec_getDictionaryDim )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get dim
    RETURN->v_int = word2vec->getDictionaryDim();
}

CK_DLL_MFUN( Word2Vec_print )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // print
    word2vec->print();
}
