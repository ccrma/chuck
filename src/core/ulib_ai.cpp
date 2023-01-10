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
    doc = "a supervised learning utility that predicts probabilities of class membership based on distances from a test input to its k nearest neighbors.";

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
    func->doc = "Predict the output probabilities ('prob') given unlabeled test input 'x' based on distances to 'k' nearest neighbors.";
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
    func->doc = "Initialize the HMM model with the given initial state distribution, transition matrix, and emission matrix.";
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

    return TRUE;

    error:
    // end the class import
    type_engine_import_class_end( env );

    return FALSE;
}

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
    HMM_Object() : initial(NULL), transition(NULL), emission(NULL)
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
