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
#include "chuck_errmsg.h"
#include "util_math.h"
#include "util_string.h"
#include <math.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
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
CK_DLL_MFUN( KNN_search );
// 1.4.2.1 (yikai) added KNN2
CK_DLL_CTOR( KNN2_ctor );
CK_DLL_DTOR( KNN2_dtor );
CK_DLL_MFUN( KNN2_train );
CK_DLL_MFUN( KNN2_predict );
CK_DLL_MFUN( KNN2_search );
CK_DLL_MFUN( KNN2_search2 );
CK_DLL_MFUN( KNN2_search3 );
// offset
static t_CKUINT KNN_offset_data = 0;
static t_CKUINT KNN2_offset_data = 0;

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
CK_DLL_MFUN( Word2Vec_load2 );
CK_DLL_MFUN( Word2Vec_getMostSimilarByWord );
CK_DLL_MFUN( Word2Vec_getMostSimilarByVector );
CK_DLL_MFUN( Word2Vec_getVector );
CK_DLL_MFUN( Word2Vec_getDictionarySize );
CK_DLL_MFUN( Word2Vec_getDictionaryDim );
CK_DLL_MFUN( Word2Vec_getUseKDTree );
CK_DLL_MFUN( Word2Vec_getDimMinMax );
CK_DLL_MFUN( Word2Vec_contains );
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
        "a basic k-NN utility that returns the indices of k nearest neighbors. (also see KNN2)";

    // begin class definition
    if( !type_engine_import_class_begin( env, "KNN", "Object", env->global(), KNN_ctor, KNN_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    KNN_offset_data = type_engine_import_mvar( env, "float", "@KNN_data", FALSE );
    if( KNN_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", KNN_train );
    func->add_arg( "float[][]", "x" );
    func->doc = "Train the KNN model with the given samples vectors 'x'";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN_search );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "indices" );
    func->doc = "Search for the 'k' nearest neighbors of 'query' and return their respective indices.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: KNN2
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc =
        "a k-NN utility that predicts probabilities of class membership based on distances from a test input to its k nearest neighbors. (also see KNN)";

    // begin class definition
    if( !type_engine_import_class_begin( env, "KNN2", "Object", env->global(), KNN2_ctor, KNN2_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    KNN_offset_data = type_engine_import_mvar( env, "float", "@KNN2_data", FALSE );
    if( KNN_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", KNN2_train );
    func->add_arg( "float[][]", "x" );
    func->add_arg( "int[]", "y" );
    func->doc = "Train the KNN model with the given samples 'x' and corresponding labels 'y'.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "int", "predict", KNN2_predict );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "float[]", "prob" );
    func->doc =
        "Predict the output probabilities ('prob') given unlabeled test input 'query' based on distances to 'k' nearest neighbors.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN2_search );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "labels" );
    func->doc = "Search for the 'k' nearest neighbors of 'query' and return their labels.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN2_search2 );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "labels" );
    func->add_arg( "int[]", "indices" );
    func->doc =
        "Search for the 'k' nearest neighbors of 'query' and return their labels and indices.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN2_search3 );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "labels" );
    func->add_arg( "int[]", "indices" );
    func->add_arg( "float[][]", "features" );
    func->doc =
        "Search for the 'k' nearest neighbors of 'query' and return their labels, indices, and feature vectors.";
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
    doc = "a word embeddings utility that maps words to vectors; can load a model and perform similarity retrieval.";

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

    // load
    func = make_new_mfun( "int", "load", Word2Vec_load2 );
    func->add_arg( "string", "path" );
    func->add_arg( "int", "useKDTreeDim" );
    func->doc =
        "Load pre-trained word embedding model from the given path; will use KDTree for similarity searches if the data dimension is less than or equal to 'useKDTreeDim'. Set 'useKDTreeDim' to 0 to use linear (brute force) similarity search; set 'useKDTreeDim' to less than 0 to always use KDTree.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getMostSimilar
    func = make_new_mfun( "int", "getSimilar", Word2Vec_getMostSimilarByWord );
    func->add_arg( "string", "word" );
    func->add_arg( "int", "k" );
    func->add_arg( "string[]", "output" );
    func->doc = "Get the k most similar words to the given word; return false if 'word' is not in model.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getByVector
    func = make_new_mfun( "int", "getSimilar", Word2Vec_getMostSimilarByVector );
    func->add_arg( "float[]", "vec" );
    func->add_arg( "int", "k" );
    func->add_arg( "string[]", "output" );
    func->doc = "Get the k most similar words to the given vector.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // getVector
    func = make_new_mfun( "int", "getVector", Word2Vec_getVector );
    func->add_arg( "string", "word" );
    func->add_arg( "float[]", "output" );
    func->doc = "Get the vector of the given word; returns false if 'word' is not in model.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // contains
    func = make_new_mfun( "int", "contains", Word2Vec_contains );
    func->add_arg( "string", "word" );
    func->doc = "Query if 'word' is in the current model.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // size
    func = make_new_mfun( "int", "size", Word2Vec_getDictionarySize );
    func->doc = "Get number of words in dictionary.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // dim
    func = make_new_mfun( "int", "dim", Word2Vec_getDictionaryDim );
    func->doc = "Get number of dimensions for word embedding.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // dim
    func = make_new_mfun( "void", "minMax", Word2Vec_getDimMinMax );
    func->add_arg( "float[]", "mins" );
    func->add_arg( "float[]", "maxs" );
    func->doc = "Retrieve the minimums and maximums for each dimension.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // dim
    func = make_new_mfun( "int", "useKDTree", Word2Vec_getUseKDTree );
    func->doc = "Get whether a KDTree is used for similarity search.";
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
    t_CKINT train( Chuck_Array4 & x_ )
    {
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKINT x_dim = x_i->size();
        X->allocate( n_sample, x_dim );
        // copy
        for( t_CKINT i = 0; i < n_sample; i++ )
        {
            x_.get( i, &v );
            x_i = (Chuck_Array8 *)v;
            for( t_CKINT j = 0; j < x_dim; j++ )
            {
                x_i->get( j, &X->v( i, j ) );
            }
        }
        return 0;
    }

    // train
    t_CKINT train( Chuck_Array4 & x_, Chuck_Array4 & y_ )
    {
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKUINT x_dim = x_i->size();
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

    void getNearestNeighbors( const vector<t_CKFLOAT> & query, t_CKINT k, ChaiVectorFast<t_CKINT> & indices )
    {
        // check query dimension against training dimensions
        if( query.size() < X->yDim() )
        {
            // warning
            EM_error3( "k-NN query dimension %d less than training dimension %d; zero-padding...",
                       query.size(), X->yDim() );
        }

        // allocate
        ChaiVectorFast<t_CKFLOAT> top_distances( k );
        ChaiVectorFast<t_CKINT> top_indices( k );
        // initialize
        for( t_CKINT i = 0; i < k; i++ )
        {
            top_distances.v( i ) = 1e10;
            top_indices.v( i ) = 0;
        }
        // compute distances
        t_CKFLOAT distance, diff;
        for( t_CKINT i = 0; i < X->xDim(); i++ )
        {
            distance = 0.0;
            for( t_CKINT j = 0; j < X->yDim(); j++ )
            {
                // extra check in case query less than training dimensions
                diff = (j < query.size() ? query[j] : 0.0) - X->v( i, j );
                distance += diff * diff;
            }
            // check
            for( t_CKINT j = 0; j < k; j++ )
            {
                if( distance < top_distances.v( j ) )
                {
                    for( t_CKINT l = k - 1; l > j; l-- )
                    {
                        top_distances.v( l ) = top_distances.v( l - 1 );
                        top_indices.v( l ) = top_indices.v( l - 1 );
                    }
                    top_distances.v( j ) = distance;
                    top_indices.v( j ) = i;
                    break;
                }
            }
        }
        // copy
        for( t_CKINT i = 0; i < k; i++ )
        {
            indices.v( i ) = top_indices.v( i );
        }
    }

    // search; returns indices
    void search0( const vector<t_CKFLOAT> & query, t_CKINT k, Chuck_Array4 & indices_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // resize
        indices_.set_size( k );
        // copy
        for( t_CKINT i = 0; i < k; i++ )
        {
            indices_.m_vector[i] = indices[i];
        }
    }

    // search; returns labels
    void search1( const vector<t_CKFLOAT> & query, t_CKINT k, Chuck_Array4 & labels_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // resize
        labels_.set_size( k );
        // copy
        for( t_CKINT i = 0; i < k; i++ )
        {
            labels_.m_vector[i] = Y->v( indices.v( i ) );
        }
    }

    // search; returns labels and indices
    void search2( const vector<t_CKFLOAT> & query, t_CKINT k, Chuck_Array4 & labels_, Chuck_Array4 & indices_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // resize
        labels_.set_size( k );
        indices_.set_size( k );
        for( t_CKINT i = 0; i < k; i++ )
        {
            labels_.m_vector[i] = Y->v( indices.v( i ) );
            indices_.m_vector[i] = indices[i];
        }
    }

    // search; returns labels, indices, and feature vectors
    void search3( const vector<t_CKFLOAT> & query, t_CKINT k, Chuck_Array4 & labels_, Chuck_Array4 & indices_, Chuck_Array4 & features_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // resize
        labels_.set_size( k );
        indices_.set_size( k );
        // check dimensions
        if( features_.size() < k )
        {
            Chuck_Array8 * x_i = features_.size() ? (Chuck_Array8 *)features_.m_vector[0] : NULL;
            EM_error3( "KNN2: insufficient 'features' matrix provided: %dx%d (expecting %dx%d)",
                       features_.size(), x_i != NULL ? x_i->m_vector.size() : 0, k, this->X->xDim() );
            return;
        }
        // copy
        Chuck_Array8 * x_i;
        for( t_CKINT i = 0; i < k; i++ )
        {
            labels_.m_vector[i] = Y->v( indices.v( i ) );
            indices_.m_vector[i] = indices[i];
            x_i = (Chuck_Array8 *)features_.m_vector[i];
            for( t_CKINT j = 0; j < X->yDim(); j++ )
            {
                x_i->m_vector[j] = X->v( indices.v( i ), j );
            }
        }
    }

    // predict
    void predict( const vector<t_CKFLOAT> & query, t_CKINT k, Chuck_Array8 & prob_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // copy
        prob_.set_size( Y->getMax() + 1 );
        // zero out
        prob_.zero( 0, prob_.size() );
        for( t_CKINT i = 0; i < k; i++ )
        {
            prob_.m_vector[Y->v( indices.v( i ) )] += 1.0;
        }
        for( t_CKINT i = 0; i < prob_.size(); i++ )
        {
            prob_.m_vector[i] /= k;
        }
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

    // check for NULL
    if( x == NULL  )
    {
        EM_error3( "KNN.train(): NULL input encountered...");
        return;
    }

    // train
    knn->train( *x );
}

CK_DLL_MFUN( KNN_search )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array4 * indices = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || indices == NULL )
    {
        EM_error3( "KNN.search(): NULL input encountered...");
        return;
    }

    // search0
    knn->search0( query->m_vector, k, *indices );
}




//-----------------------------------------------------------------------------
// KNN2 c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( KNN2_ctor )
{
    KNN_Object * knn = new KNN_Object();
    OBJ_MEMBER_UINT( SELF, KNN2_offset_data ) = (t_CKUINT)knn;
}

CK_DLL_DTOR( KNN2_dtor )
{
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    SAFE_DELETE( knn );
    OBJ_MEMBER_UINT( SELF, KNN2_offset_data ) = 0;
}

CK_DLL_MFUN( KNN2_train )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array4 * x = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * y = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( x == NULL || y == NULL )
    {
        EM_error3( "KNN2.train(): NULL input encountered...");
        return;
    }

    // train
    knn->train( *x, *y );
}

CK_DLL_MFUN( KNN2_predict )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array8 * prob = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || prob == NULL )
    {
        EM_error3( "KNN2.predict(): NULL input encountered...");
        return;
    }

    // predict
    knn->predict( query->m_vector, k, *prob );
}

CK_DLL_MFUN( KNN2_search )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array4 * labels = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || labels == NULL )
    {
        EM_error3( "KNN2.search(): NULL input encountered...");
        return;
    }

    // search1
    knn->search1( query->m_vector, k, *labels );
}

CK_DLL_MFUN( KNN2_search2 )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array4 * labels = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * indices = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || labels == NULL || indices == NULL )
    {
        EM_error3( "KNN2.search(): NULL input encountered...");
        return;
    }

    // search2
    knn->search2( query->m_vector, k, *labels, *indices );
}

CK_DLL_MFUN( KNN2_search3 )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array4 * labels = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * indices = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * features = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || labels == NULL || indices == NULL || features == NULL )
    {
        EM_error3( "KNN2.search(): NULL input encountered...");
        return;
    }

    // search3
    knn->search3( query->m_vector, k, *labels, *indices, *features );
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
    struct kdtree * tree;
    // bounds for each dimension
    ChaiVectorFast<t_CKFLOAT> mins;
    ChaiVectorFast<t_CKFLOAT> maxs;

    W2V_Dictionary()
        : key_to_index( NULL ),
          index_to_key( NULL ),
          word_vectors( NULL ),
          dictionarySize( 0 ),
          vectorLength( 0 ),
          tree( NULL )
    { }

    // clear
    void clear()
    {
        dictionarySize = 0;
        vectorLength = 0;
        SAFE_DELETE( key_to_index );
        SAFE_DELETE( index_to_key );
        SAFE_DELETE( word_vectors );
        kdtree_destroy( tree );
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
    // line number for parsing error reporting
    t_CKINT m_lineNum;

public:
    // constructor
    Word2Vec_Object()
        : dictionary( NULL ), m_lineNum( 0 )
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
        // zero out line number
        m_lineNum = 0;
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

    // get dim mins and maxs
    void getDimMinMaxs( Chuck_Array8 * mins, Chuck_Array8 * maxs )
    {
        // get dim
        t_CKINT dims = getDictionaryDim();
        // allocate
        if( dims == 0 ) return;

        // if not null
        if( mins != NULL )
        {
            // set size
            mins->set_size( dims );
            // copy minimums
            for( int i = 0; i < dims; i++ )
            { mins->set( i, dictionary->mins.v( i ) ); }
        }

        // if not null
        if( maxs != NULL )
        {
            // set size
            maxs->set_size( dims );
            // copy minimums
            for( int i = 0; i < dims; i++ )
            { maxs->set( i, dictionary->maxs.v( i ) ); }
        }
    }

    // get dictionary dimensions
    t_CKBOOL getUseKDTree() const
    {
        return dictionary != NULL ? dictionary->tree != NULL : 0;
    }

    // read next non-empty or commented line
    t_CKBOOL nextline( std::ifstream & fin, string & line, t_CKBOOL commentIsHash )
    {
        // skip over empty lines and commented lines (starts with #)
        do
        {
            // increment line number
            m_lineNum++;
            // get line
            if( !std::getline( fin, line ) )
            {
                line = "";
                return FALSE;
            }
            // ltrim leading white spaces
            line = ltrim( line );
        } while( line == "" || ( commentIsHash && line[0] == '#' ) );

        return TRUE;
    }

    // load
    t_CKINT load( Chuck_String & path, t_CKINT useKDTreeIfDimLEQtoThis )
    {
        // clear
        clear();

        // check cache
        if( o_cache.find( path.str() ) != o_cache.end() )
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
        // string to hold the line
        string line;

        // get next non-empty, non-commented # line
        if( !nextline( fin, line, TRUE ) )
        {
            EM_error3( "Word2Vec: error parsing model on line: %d", m_lineNum );
            return false;
        }

        // string stream
        istringstream strin( line );
        // read header
        strin >> dictionary->dictionarySize >> dictionary->vectorLength;

        // allocate
        dictionary->key_to_index = new std::map<std::string, t_CKUINT>();
        dictionary->index_to_key = new ChaiVectorFast<std::string>( dictionary->dictionarySize );
        dictionary->mins.allocate( dictionary->vectorLength );
        dictionary->maxs.allocate( dictionary->vectorLength );
        for( int i = 0; i < dictionary->vectorLength; i++ )
        {
            // initialize
            dictionary->mins.v( i ) = 1e10;
            dictionary->maxs.v( i ) = -1e10;
        }

        // check if using kdtree
        if( useKDTreeIfDimLEQtoThis < 0 || dictionary->vectorLength <= useKDTreeIfDimLEQtoThis )
        {
            // read vectors and build kd-tree
            dictionary->tree = kdtree_init( dictionary->vectorLength );
            std::string key;
            t_CKFLOAT value;
            ChaiVectorFast<t_CKFLOAT> vector( dictionary->vectorLength );
            for( t_CKINT i = 0; i < dictionary->dictionarySize; i++ )
            {
                // get next non-empty line
                if( !nextline( fin, line, FALSE ) )
                {
                    EM_error3( "Word2Vec: error parsing model on line: %d", m_lineNum );
                    return false;
                }
                // set into string stream
                strin.str( line );
                strin.clear();

                // get the word
                strin >> key;
                dictionary->index_to_key->v( i ) = key;
                ( *( dictionary->key_to_index ) )[key] = i;
                for( t_CKINT j = 0; j < dictionary->vectorLength; j++ )
                {
                    strin >> value;
                    vector.v( j ) = value;

                    // find min/max
                    if( value < dictionary->mins.v( j ) ) dictionary->mins.v( j ) = value;
                    else if( value > dictionary->maxs.v( j ) ) dictionary->maxs.v( j ) = value;
                }
                kdtree_insert( dictionary->tree, vector.m_vector );
            }

            // rebuild kdtree
            kdtree_rebuild( dictionary->tree );
        }
        else
        {
            // allocate word vectors
            dictionary->word_vectors =
                new ChaiMatrixFast<t_CKFLOAT>( dictionary->dictionarySize, dictionary->vectorLength );

            // read vectors
            std::string key;
            t_CKFLOAT value;
            for( t_CKINT i = 0; i < dictionary->dictionarySize; i++ )
            {
                // get next non-empty line
                if( !nextline( fin, line, FALSE ) )
                {
                    EM_error3( "Word2Vec: error parsing model on line: %d", m_lineNum );
                    return false;
                }
                // set into string stream
                strin.str( line );
                strin.clear();

                strin >> key;
                dictionary->index_to_key->v( i ) = key;
                ( *( dictionary->key_to_index ) )[key] = i;
                for( t_CKINT j = 0; j < dictionary->vectorLength; j++ )
                {
                    strin >> value;
                    dictionary->word_vectors->v( i, j ) = value;

                    // find min/max
                    if( value < dictionary->mins.v( j ) ) dictionary->mins.v( j ) = value;
                    else if( value > dictionary->maxs.v( j ) ) dictionary->maxs.v( j ) = value;
                }
            }
        }

        // close file
        fin.close();

        // add to cache
        o_cache[path.str()] = dictionary;

        return true;
    }

    // get nearest neighbors (brute force)
    void getNearestNeighborsBF( ChaiVectorFast<t_CKFLOAT> & vector, t_CKINT topn, ChaiVectorFast<t_CKINT> & indices )
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

    // get nearest neighbors
    void getNearestNeighborsKDTree( ChaiVectorFast<t_CKFLOAT> & vector,
                                    t_CKINT topn,
                                    ChaiVectorFast<t_CKINT> & indices )
    {
        kdtree_knn_search( dictionary->tree, vector.m_vector, topn );
        struct knn_list * p = dictionary->tree->knn_list_head.next;
        t_CKINT i = 0;
        // TODO: check this -- ge: add index check to prevent out of bounds
        while( p != &dictionary->tree->knn_list_head && i < indices.length() )
        {
            indices.v( i++ ) = p->node->coord_index;
            p = p->next;
        }
    }

    // get nearest neighbors
    void getNearestNeighbors( ChaiVectorFast<t_CKFLOAT> & vector, t_CKINT topn, ChaiVectorFast<t_CKINT> & indices )
    {
        // check
        if( dictionary == NULL )
        {
            EM_error3( "Word2Vec: no model loaded..." );
            return;
        }

        // check if using kdtree
        if( dictionary->tree != NULL ) return getNearestNeighborsKDTree( vector, topn, indices );
        else return getNearestNeighborsBF( vector, topn, indices );
    }

    // getMostSimilar
    t_CKBOOL getMostSimilarByWord( const string & word, t_CKINT topn, Chuck_Array4 & output_ )
    {
        // error flag
        t_CKBOOL hasError = FALSE;
        // clear
        for( t_CKINT i = 0; i < output_.size(); i++ )
        { ( (Chuck_String *)output_.m_vector[i] )->set( "" ); }

        // check
        if( dictionary == NULL )
        {
            EM_error3( "Word2Vec: no model loaded..." );
            return false;
        }

        // check
        if( output_.size() < topn )
        {
            EM_error3( "Word2Vec.getSimilar() results array insufficient size %d (expecting %d)...",
                       output_.size(),
                       topn );
            // reduce k and do our best
            topn = output_.size();
            // but mark error to return false (since we won't fully fulfill our function)
            hasError = TRUE;
        }

        // see if word is there
        if( !contains( word ) )
        { return false; }

        // get index
        t_CKINT index = ( *( dictionary->key_to_index ) )[word];
        // get vector
        ChaiVectorFast<t_CKFLOAT> vector( dictionary->vectorLength );

        // use tree?
        if( dictionary->tree != NULL )
        {
            for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
                vector.v( i ) = dictionary->tree->coord_table[index][i];
            // get nearest neighbors
            ChaiVectorFast<t_CKINT> top_indices( topn );
            getNearestNeighbors( vector, topn, top_indices );
            // copy
            for( t_CKINT i = 0; i < topn; i++ )
            {
                ( (Chuck_String *)output_.m_vector[i] )->set( dictionary->index_to_key->v( top_indices.v( i ) ) );
            }
        }
        else
        {
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

        return !hasError;
    }

    // getByVector
    t_CKBOOL getMostSimilarByVector( Chuck_Array8 & vec_, t_CKINT topn, Chuck_Array4 & output_ )
    {
        // error flag
        t_CKBOOL hasError = FALSE;
        // clear
        for( t_CKINT i = 0; i < output_.size(); i++ )
        { ( (Chuck_String *)output_.m_vector[i] )->set( "" ); }

        // check
        if( dictionary == NULL )
        {
            EM_error3( "Word2Vec: no model loaded..." );
            return false;
        }

        // check
        if( output_.size() < topn )
        {
            EM_error3( "Word2Vec.getSimilar() results array insufficient size %d (expecting %d)...",
                       output_.size(),
                       topn );
            // reduce k and do our best
            topn = output_.size();
            // but mark error to return false (since we won't fully fulfill our function)
            hasError = TRUE;
        }

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

        return !hasError;
    }

    // contains
    t_CKBOOL contains( const string & word )
    {
        // see if word is there
        std::map<string, t_CKUINT> & key2index = *( dictionary->key_to_index );
        return key2index.find( word ) != key2index.end();
    }

    // getVector
    t_CKBOOL getVector( const string & word, Chuck_Array8 & output_ )
    {
        // check
        if( dictionary == NULL )
        {
            EM_error3( "Word2Vec: no model loaded..." );
            return false;
        }

        // ensure length
        if( output_.size() < dictionary->vectorLength )
        { output_.set_size( dictionary->vectorLength ); }
        // zero out
        output_.zero( 0, output_.size() );
        // see if word is there
        if( !contains( word ) )
        { return false; }

        // get index
        std::map<string, t_CKUINT> & key2index = *( dictionary->key_to_index );
        t_CKINT index = key2index[word];

        // whether we are using
        if( dictionary->tree != NULL )
        {
            // copy
            for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
            {
                output_.m_vector[i] = dictionary->tree->coord_table[index][i];
            }
        }
        else
        {
            // copy
            for( t_CKINT i = 0; i < dictionary->vectorLength; i++ )
            {
                output_.m_vector[i] = dictionary->word_vectors->v( index, i );
            }
        }

        // done
        return true;
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
    RETURN->v_int = word2vec->load( *path, 8 );
}

CK_DLL_MFUN( Word2Vec_load2 )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * path = GET_NEXT_STRING( ARGS );
    t_CKINT useKDTreeDim = GET_NEXT_INT( ARGS );
    // load
    RETURN->v_int = word2vec->load( *path, useKDTreeDim );
}

CK_DLL_MFUN( Word2Vec_getMostSimilarByWord )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * word = GET_NEXT_STRING( ARGS );
    t_CKINT topn = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( word == NULL || output == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }

    // get by word
    RETURN->v_int = word2vec->getMostSimilarByWord( word->str(), topn, *output );
}

CK_DLL_MFUN( Word2Vec_getMostSimilarByVector )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_Array8 * vec = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT topn = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( vec == NULL || output == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }

    // get by vector
    RETURN->v_int = word2vec->getMostSimilarByVector( *vec, topn, *output );
}

CK_DLL_MFUN( Word2Vec_getVector )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * word = GET_NEXT_STRING( ARGS );
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( word == NULL || output == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }

    // getVector
    RETURN->v_int = word2vec->getVector( word->str(), *output );
}

CK_DLL_MFUN( Word2Vec_contains )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get args
    Chuck_String * word = GET_NEXT_STRING( ARGS );

    // check for NULL
    if( word == NULL )
    {
        RETURN->v_int = FALSE;
        return;
    }

    // contains
    RETURN->v_int = word2vec->contains( word->str() );
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

CK_DLL_MFUN( Word2Vec_getDimMinMax )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get chuck arrays
    Chuck_Array8 * mins = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array8 * maxs = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );

    // get it
    word2vec->getDimMinMaxs( mins, maxs );
}

CK_DLL_MFUN( Word2Vec_getUseKDTree )
{
    // get object
    Word2Vec_Object * word2vec = (Word2Vec_Object *)OBJ_MEMBER_UINT( SELF, Word2Vec_offset_data );
    // get dim
    RETURN->v_int = word2vec->getUseKDTree();
}

//-----------------------------------------------------------------------------
// KDTree implementation
//-----------------------------------------------------------------------------
static inline t_CKINT is_leaf( struct kdnode * node )
{
    return node->left == node->right;
}

static inline void swap( long * a, long * b )
{
    long tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline double square( double d )
{
    return d * d;
}

static inline double distance( double * c1, double * c2, t_CKINT dim )
{
    double distance = 0;
    while( dim-- > 0 )
    {
        distance += square( *c1++ - *c2++ );
    }
    return distance;
}

static inline double knn_max( struct kdtree * tree )
{
    return tree->knn_list_head.prev->distance;
}

static inline double D( struct kdtree * tree, long index, t_CKINT r )
{
    return tree->coord_table[index][r];
}

static inline t_CKINT kdnode_passed( struct kdtree * tree, struct kdnode * node )
{
    return node != NULL ? tree->coord_passed[node->coord_index] : 1;
}

static inline t_CKINT knn_search_on( struct kdtree * tree, t_CKINT k, double value, double target )
{
    return tree->knn_num < k || square( target - value ) < knn_max( tree );
}

static inline void coord_index_reset( struct kdtree * tree )
{
    long i;
    for( i = 0; i < tree->capacity; i++ )
    {
        tree->coord_indexes[i] = i;
    }
}

static inline void coord_table_reset( struct kdtree * tree )
{
    long i;
    for( i = 0; i < tree->capacity; i++ )
    {
        tree->coord_table[i] = tree->coords + i * tree->dim;
    }
}

static inline void coord_deleted_reset( struct kdtree * tree )
{
    memset( tree->coord_deleted, 0, tree->capacity );
}

static inline void coord_passed_reset( struct kdtree * tree )
{
    memset( tree->coord_passed, 0, tree->capacity );
}

static void coord_dump_all( struct kdtree * tree )
{
    long i, j;
    for( i = 0; i < tree->count; i++ )
    {
        long index = tree->coord_indexes[i];
        double * coord = tree->coord_table[index];
        printf( "(" );
        for( j = 0; j < tree->dim; j++ )
        {
            if( j != tree->dim - 1 )
            {
                printf( "%.2f,", coord[j] );
            }
            else
            {
                printf( "%.2f)\n", coord[j] );
            }
        }
    }
}

static void coord_dump_by_indexes( struct kdtree * tree, long low, long high, t_CKINT r )
{
    long i;
    printf( "r=%d:", (int)r );
    for( i = 0; i <= high; i++ )
    {
        if( i < low )
        {
            printf( "%8s", " " );
        }
        else
        {
            long index = tree->coord_indexes[i];
            printf( "%8.2f", tree->coord_table[index][r] );
        }
    }
    printf( "\n" );
}

static void quicksort( struct kdtree * tree, long lo, long hi, t_CKINT r )
{
    long i, j, pivot, * indexes;
    // double * p;

    if( lo >= hi )
    {
        return;
    }

    i = lo;
    j = hi;
    indexes = tree->coord_indexes;
    pivot = indexes[lo];

    while( i < j )
    {
        while( i < j && D( tree, indexes[j], r ) >= D( tree, pivot, r ) ) j--;
        /* Loop invariant: nums[j] > pivot or i == j */
        indexes[i] = indexes[j];
        while( i < j && D( tree, indexes[i], r ) <= D( tree, pivot, r ) ) i++;
        /* Loop invariant: nums[i] < pivot or i == j */
        indexes[j] = indexes[i];
    }
    /* Loop invariant: i == j */
    indexes[i] = pivot;

    quicksort( tree, lo, i - 1, r );
    quicksort( tree, i + 1, hi, r );
}

static struct kdnode * kdnode_alloc( double * coord, long index, t_CKINT r )
{
    struct kdnode * node = (struct kdnode *)malloc( sizeof( *node ) );
    if( node != NULL )
    {
        memset( node, 0, sizeof( *node ) );
        node->coord = coord;
        node->coord_index = index;
        node->r = r;
    }
    return node;
}

static void kdnode_free( struct kdnode * node )
{
    free( node );
}

static t_CKINT coord_cmp( double * c1, double * c2, t_CKINT dim )
{
    t_CKINT i;
    double ret;
    for( i = 0; i < dim; i++ )
    {
        ret = *c1++ - *c2++;
        if( fabs( ret ) >= DBL_EPSILON )
        {
            return ret > 0 ? 1 : -1;
        }
    }

    if( fabs( ret ) < DBL_EPSILON )
    {
        return 0;
    }
    else
    {
        return ret > 0 ? 1 : -1;
    }
}

static void knn_list_add( struct kdtree * tree, struct kdnode * node, double distance )
{
    if( node == NULL ) return;

    struct knn_list * head = &tree->knn_list_head;
    struct knn_list * p = head->prev;
    if( tree->knn_num == 1 )
    {
        if( p->distance > distance )
        {
            p = p->prev;
        }
    }
    else
    {
        while( p != head && p->distance > distance )
        {
            p = p->prev;
        }
    }

    if( p == head || coord_cmp( p->node->coord, node->coord, tree->dim ) )
    {
        struct knn_list * log = (struct knn_list *)malloc( sizeof( *log ) );
        if( log != NULL )
        {
            log->node = node;
            log->distance = distance;
            log->prev = p;
            log->next = p->next;
            p->next->prev = log;
            p->next = log;
            tree->knn_num++;
        }
    }
}

static void knn_list_adjust( struct kdtree * tree, struct kdnode * node, double distance )
{
    if( node == NULL ) return;

    struct knn_list * head = &tree->knn_list_head;
    struct knn_list * p = head->prev;
    if( tree->knn_num == 1 )
    {
        if( p->distance > distance )
        {
            p = p->prev;
        }
    }
    else
    {
        while( p != head && p->distance > distance )
        {
            p = p->prev;
        }
    }

    if( p == head || coord_cmp( p->node->coord, node->coord, tree->dim ) )
    {
        struct knn_list * log = head->prev;
        /* Replace the original max one */
        log->node = node;
        log->distance = distance;
        /* Remove from the max position */
        head->prev = log->prev;
        log->prev->next = head;
        /* insert as a new one */
        log->prev = p;
        log->next = p->next;
        p->next->prev = log;
        p->next = log;
    }
}

static void knn_list_clear( struct kdtree * tree )
{
    struct knn_list * head = &tree->knn_list_head;
    struct knn_list * p = head->next;
    while( p != head )
    {
        struct knn_list * prev = p;
        p = p->next;
        free( prev );
    }
    tree->knn_num = 0;
}

static void resize( struct kdtree * tree )
{
    tree->capacity *= 2;
    tree->coords = (double *)realloc( tree->coords, tree->dim * sizeof( double ) * tree->capacity );
    tree->coord_table = (double **)realloc( tree->coord_table, sizeof( double * ) * tree->capacity );
    tree->coord_indexes = (long *)realloc( tree->coord_indexes, sizeof( long ) * tree->capacity );
    tree->coord_deleted = (unsigned char *)realloc( tree->coord_deleted, sizeof( char ) * tree->capacity );
    tree->coord_passed = (unsigned char *)realloc( tree->coord_passed, sizeof( char ) * tree->capacity );
    coord_table_reset( tree );
    coord_index_reset( tree );
    coord_deleted_reset( tree );
    coord_passed_reset( tree );
}

static void kdnode_dump( struct kdnode * node, t_CKINT dim )
{
    t_CKINT i;
    if( node->coord != NULL )
    {
        printf( "(" );
        for( i = 0; i < dim; i++ )
        {
            if( i != dim - 1 )
            {
                printf( "%.2f,", node->coord[i] );
            }
            else
            {
                printf( "%.2f)\n", node->coord[i] );
            }
        }
    }
    else
    {
        printf( "(none)\n" );
    }
}

void kdtree_insert( struct kdtree * tree, double * coord )
{
    if( tree->count + 1 > tree->capacity )
    {
        resize( tree );
    }
    memcpy( tree->coord_table[tree->count++], coord, tree->dim * sizeof( double ) );
}

static void knn_pickup( struct kdtree * tree, struct kdnode * node, double * target, t_CKINT k )
{
    double dist = distance( node->coord, target, tree->dim );
    if( tree->knn_num < k )
    {
        knn_list_add( tree, node, dist );
    }
    else
    {
        if( dist < knn_max( tree ) )
        {
            knn_list_adjust( tree, node, dist );
        }
        else if( fabs( dist - knn_max( tree ) ) < DBL_EPSILON )
        {
            knn_list_add( tree, node, dist );
        }
    }
}

static void kdtree_search_recursive( struct kdtree * tree,
                                     struct kdnode * node,
                                     double * target,
                                     t_CKINT k,
                                     t_CKINT * pickup )
{
    if( node == NULL || kdnode_passed( tree, node ) )
    {
        return;
    }

    t_CKINT r = node->r;
    if( !knn_search_on( tree, k, node->coord[r], target[r] ) )
    {
        return;
    }

    if( is_leaf( node ) )
    {
        *pickup = 1;
    }
    else
    {
        if( target[r] <= node->coord[r] )
        {
            kdtree_search_recursive( tree, node->left, target, k, pickup );
            if( *pickup || node->left == NULL )
            {
                kdtree_search_recursive( tree, node->right, target, k, pickup );
            }
        }
        else
        {
            kdtree_search_recursive( tree, node->right, target, k, pickup );
            if( *pickup || node->right == NULL )
            {
                kdtree_search_recursive( tree, node->left, target, k, pickup );
            }
        }
    }

    /* back track and pick up  */
    if( *pickup )
    {
        tree->coord_passed[node->coord_index] = 1;
        knn_pickup( tree, node, target, k );
    }
}

void kdtree_knn_search( struct kdtree * tree, double * target, t_CKINT k )
{
    if( k > 0 )
    {
        t_CKINT pickup = 0;
        // HACK: reset for each new search
        coord_passed_reset( tree );
        struct knn_list * p = tree->knn_list_head.next;
        while( p != &tree->knn_list_head )
        {
            p->distance = 1e10;
            p = p->next;
        }
        // END HACK
        kdtree_search_recursive( tree, tree->root, target, k, &pickup );
    }
}

//void kdtree_knn_search( struct kdtree * tree, double * target, t_CKINT k )
//{
//    if( k > 0 )
//    {
//        t_CKINT pickup = 0;
//        kdtree_search_recursive( tree, tree->root, target, k, &pickup );
//    }
//}

void kdtree_delete( struct kdtree * tree, double * coord )
{
    t_CKINT r = 0;
    struct kdnode * node = tree->root;
    struct kdnode * parent = node;

    while( node != NULL )
    {
        if( node->coord == NULL )
        {
            if( parent->right->coord == NULL )
            {
                break;
            }
            else
            {
                node = parent->right;
                continue;
            }
        }

        if( coord[r] < node->coord[r] )
        {
            parent = node;
            node = node->left;
        }
        else if( coord[r] > node->coord[r] )
        {
            parent = node;
            node = node->right;
        }
        else
        {
            t_CKINT ret = coord_cmp( coord, node->coord, tree->dim );
            if( ret < 0 )
            {
                parent = node;
                node = node->left;
            }
            else if( ret > 0 )
            {
                parent = node;
                node = node->right;
            }
            else
            {
                node->coord = NULL;
                break;
            }
        }
        r = ( r + 1 ) % tree->dim;
    }
}

static void kdnode_build( struct kdtree * tree, struct kdnode ** nptr, t_CKINT r, long low, long high )
{
    if( low == high )
    {
        long index = tree->coord_indexes[low];
        *nptr = kdnode_alloc( tree->coord_table[index], index, r );
    }
    else if( low < high )
    {
        /* Sort and fetch the median to build a balanced BST */
        quicksort( tree, low, high, r );
        long median = low + ( high - low ) / 2;
        long median_index = tree->coord_indexes[median];
        struct kdnode * node = *nptr = kdnode_alloc( tree->coord_table[median_index], median_index, r );
        r = ( r + 1 ) % tree->dim;
        kdnode_build( tree, &node->left, r, low, median - 1 );
        kdnode_build( tree, &node->right, r, median + 1, high );
    }
}

static void kdtree_build( struct kdtree * tree )
{
    kdnode_build( tree, &tree->root, 0, 0, (long)( tree->count - 1 ) );
}

void kdtree_rebuild( struct kdtree * tree )
{
    long i, j;
    size_t size_of_coord = tree->dim * sizeof( double );
    for( i = 0, j = 0; j < tree->count; i++, j++ )
    {
        while( j < tree->count && tree->coord_deleted[j] )
        {
            j++;
        }
        if( i != j && j < tree->count )
        {
            memcpy( tree->coord_table[i], tree->coord_table[j], size_of_coord );
            tree->coord_deleted[i] = 0;
        }
    }
    tree->count = i;
    coord_index_reset( tree );
    kdtree_build( tree );
}

struct kdtree * kdtree_init( t_CKINT dim )
{
    struct kdtree * tree = (struct kdtree *)malloc( sizeof( *tree ) );
    if( tree != NULL )
    {
        tree->root = NULL;
        tree->dim = dim;
        tree->count = 0;
        tree->capacity = 65536;
        tree->knn_list_head.next = &tree->knn_list_head;
        tree->knn_list_head.prev = &tree->knn_list_head;
        tree->knn_list_head.node = NULL;
        tree->knn_list_head.distance = 0;
        tree->knn_num = 0;
        tree->coords = (double *)malloc( dim * sizeof( double ) * tree->capacity );
        tree->coord_table = (double **)malloc( sizeof( double * ) * tree->capacity );
        tree->coord_indexes = (long *)malloc( sizeof( long ) * tree->capacity );
        tree->coord_deleted = (unsigned char *)malloc( sizeof( char ) * tree->capacity );
        tree->coord_passed = (unsigned char *)malloc( sizeof( char ) * tree->capacity );
        coord_index_reset( tree );
        coord_table_reset( tree );
        coord_deleted_reset( tree );
        coord_passed_reset( tree );
    }
    return tree;
}

static void kdnode_destroy( struct kdnode * node )
{
    if( node == NULL ) return;
    kdnode_destroy( node->left );
    kdnode_destroy( node->right );
    kdnode_free( node );
}

void kdtree_destroy( struct kdtree * tree )
{
    kdnode_destroy( tree->root );
    knn_list_clear( tree );
    free( tree->coords );
    free( tree->coord_table );
    free( tree->coord_indexes );
    free( tree->coord_deleted );
    free( tree->coord_passed );
    free( tree );
}
