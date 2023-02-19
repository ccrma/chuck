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
#include "util_string.h"
#include <math.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

// global model type
static t_CKINT g_mt_mlp = 0;
static t_CKINT g_mt_knn = 1;
static t_CKINT g_mt_svm = 2;
static t_CKINT g_mt_lr = 3;
static t_CKINT g_mt_pr = 4;
static t_CKINT g_mt_dt = 5;
static t_CKINT g_mt_boost = 6;
static t_CKINT g_mt_naivebayes = 7;
// global activation type
static t_CKINT g_at_linear = 0;
static t_CKINT g_at_sigmoid = 1;
static t_CKINT g_at_relu = 2;
static t_CKINT g_at_tanh = 3;
static t_CKINT g_at_softmax = 4;



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
CK_DLL_MFUN( KNN_search2 );
CK_DLL_MFUN( KNN_weigh );
// 1.4.2.1 (yikai) added KNN2
CK_DLL_CTOR( KNN2_ctor );
CK_DLL_DTOR( KNN2_dtor );
CK_DLL_MFUN( KNN2_train );
CK_DLL_MFUN( KNN2_predict );
CK_DLL_MFUN( KNN2_search );
CK_DLL_MFUN( KNN2_search2 );
CK_DLL_MFUN( KNN2_search3 );
CK_DLL_MFUN( KNN2_weigh );
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

// 1.4.2.0 (yikai) added PCA
CK_DLL_CTOR( PCA_ctor );
CK_DLL_DTOR( PCA_dtor );
CK_DLL_SFUN( PCA_reduce );
// offset
static t_CKUINT PCA_offset_data = 0;

// 1.4.2.0 (yikai) added Wekinator
CK_DLL_CTOR( Wekinator_ctor );
CK_DLL_DTOR( Wekinator_dtor );
CK_DLL_MFUN( Wekinator_input );
CK_DLL_MFUN( Wekinator_output );
CK_DLL_MFUN( Wekinator_add );
CK_DLL_MFUN( Wekinator_train );
CK_DLL_MFUN( Wekinator_clear );
CK_DLL_MFUN( Wekinator_predict );
CK_DLL_MFUN( Wekinator_ctrl_model_type );
CK_DLL_MFUN( Wekinator_cget_model_type );
CK_DLL_MFUN( Wekinator_save_data );
CK_DLL_MFUN( Wekinator_load_data );
CK_DLL_MFUN( Wekinator_cget_input_dims );
CK_DLL_MFUN( Wekinator_cget_output_dims );
CK_DLL_MFUN( Wekinator_cget_num_obs );
// offset
static t_CKUINT Wekinator_offset_data = 0;

// 1.4.2.0 (yikai) added MLP
CK_DLL_CTOR( MLP_ctor );
CK_DLL_DTOR( MLP_dtor );
CK_DLL_MFUN( MLP_init );
CK_DLL_MFUN( MLP_init2 );
CK_DLL_MFUN( MLP_init3 );
CK_DLL_MFUN( MLP_train );
CK_DLL_MFUN( MLP_train2 );
CK_DLL_MFUN( MLP_predict );
CK_DLL_MFUN( MLP_get_weights );
CK_DLL_MFUN( MLP_get_biases );
CK_DLL_MFUN( MLP_get_gradients );
CK_DLL_MFUN( MLP_get_activations );
CK_DLL_MFUN( MLP_forward );
CK_DLL_MFUN( MLP_backprop );
CK_DLL_MFUN( MLP_save );
CK_DLL_MFUN( MLP_load );
CK_DLL_SFUN( MLP_shuffle );
// offset
static t_CKUINT MLP_offset_data = 0;



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
    // model type
    // add class
    QUERY->begin_class( QUERY, "AI", "Object" );
    // add documentatiopn
    QUERY->doc_class( QUERY, "AI class library." );
    // add svar
    QUERY->add_svar( QUERY, "int", "MLP", TRUE, &g_mt_mlp );
    QUERY->doc_var( QUERY, "Model type: Multi-layer perceptron" );
    QUERY->add_svar( QUERY, "int", "KNN", TRUE, &g_mt_knn );
    QUERY->doc_var( QUERY, "Model type: K-nearest neighbor" );
    QUERY->add_svar( QUERY, "int", "SVM", TRUE, &g_mt_svm );
    QUERY->doc_var( QUERY, "Model type: Support vector machine" );
    QUERY->add_svar( QUERY, "int", "LR", TRUE, &g_mt_lr );
    QUERY->doc_var( QUERY, "Model type: Logistic regression" );
    QUERY->add_svar( QUERY, "int", "PR", TRUE, &g_mt_pr );
    QUERY->doc_var( QUERY, "Model type: Polynomial regression" );
    QUERY->add_svar( QUERY, "int", "DT", TRUE, &g_mt_dt );
    QUERY->doc_var( QUERY, "Model type: Decision tree" );
    QUERY->add_svar( QUERY, "int", "BOOST", TRUE, &g_mt_boost );
    QUERY->doc_var( QUERY, "Model type: Boosting" );
    QUERY->add_svar( QUERY, "int", "NAIVEBAYES", TRUE, &g_mt_naivebayes );
    QUERY->doc_var( QUERY, "Model type: Naive Bayes" );
    QUERY->add_svar( QUERY, "int", "ReLU", TRUE, &g_at_relu );
    QUERY->doc_var( QUERY, "Activation type: ReLU" );
    QUERY->add_svar( QUERY, "int", "Sigmoid", TRUE, &g_at_sigmoid );
    QUERY->doc_var( QUERY, "Activation type: Sigmoid" );
    QUERY->add_svar( QUERY, "int", "Tanh", TRUE, &g_at_tanh );
    QUERY->doc_var( QUERY, "Activation type: Tanh" );
    QUERY->add_svar( QUERY, "int", "Softmax", TRUE, &g_at_softmax );
    QUERY->doc_var( QUERY, "Activation type: Softmax" );
    QUERY->add_svar( QUERY, "int", "Linear", TRUE, &g_at_linear );
    QUERY->doc_var( QUERY, "Activation type: Linear" );
    // done
    QUERY->end_class( QUERY );


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
    SVM_offset_data = type_engine_import_mvar( env, "int", "@SVM_data", FALSE );
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
        "a basic k-NN utility that searches for k nearest neighbors from a set of observations / feature vectors. (Also see KNN2. The differrence between KNN and KNN2 is that KNN does not deal with labels whereas KNN2 is designed to work with labels.)";

    // begin class definition
    if( !type_engine_import_class_begin( env, "KNN", "Object", env->global(), KNN_ctor, KNN_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    KNN_offset_data = type_engine_import_mvar( env, "int", "@KNN_data", FALSE );
    if( KNN_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", KNN_train );
    func->add_arg( "float[][]", "x" );
    func->doc = "Train the KNN model with the given observations 'x'";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN_search );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "indices" );
    func->doc = "Search for the 'k' nearest neighbors of 'query' and return their corresponding indices.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // search
    func = make_new_mfun( "void", "search", KNN_search2 );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "int[]", "indices" );
    func->add_arg( "float[][]", "observations" );
    func->doc =
        "Search for the 'k' nearest neighbors of 'query' and return their corresponding indices and observations";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // weigh
    func = make_new_mfun( "void", "weigh", KNN_weigh );
    func->add_arg( "float[]", "weights" );
    func->doc = "Set the weights for each dimension in the data.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: KNN2
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc =
        "a k-NN utility that predicts probabilities of class membership based on distances from a test input to its k nearest neighbors. (Also see KNN. The differrence between KNN and KNN2 is that KNN does not deal with labels whereas KNN2 is designed to work with labels.)";

    // begin class definition
    if( !type_engine_import_class_begin( env, "KNN2", "Object", env->global(), KNN2_ctor, KNN2_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    KNN_offset_data = type_engine_import_mvar( env, "int", "@KNN2_data", FALSE );
    if( KNN_offset_data == CK_INVALID_OFFSET ) goto error;

    // train
    func = make_new_mfun( "int", "train", KNN2_train );
    func->add_arg( "float[][]", "x" );
    func->add_arg( "int[]", "labels" );
    func->doc = "Train the KNN model with the given observations 'x' and corresponding labels.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "int", "predict", KNN2_predict );
    func->add_arg( "float[]", "query" );
    func->add_arg( "int", "k" );
    func->add_arg( "float[]", "prob" );
    func->doc =
        "Predict the output probabilities 'prob' given unlabeled test input 'query' based on distances to 'k' nearest neighbors.";
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
    func->add_arg( "float[][]", "observations" );
    func->doc =
        "Search for the 'k' nearest neighbors of 'query' and return their labels, indices, and observations.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // weigh
    func = make_new_mfun( "void", "weigh", KNN2_weigh );
    func->add_arg( "float[]", "weights" );
    func->doc = "Set the weights for each dimension in the data.";
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
    HMM_offset_data = type_engine_import_mvar( env, "int", "@HMM_data", FALSE );
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
    Word2Vec_offset_data = type_engine_import_mvar( env, "int", "@Word2Vec_data", FALSE );
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

    //---------------------------------------------------------------------
    // init as base class: PCA
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc = "a principle component analysis (PCA) utility, commonly used for dimensionality reduction.";

    // begin class definition
    if( !type_engine_import_class_begin( env, "PCA", "Object", env->global(), PCA_ctor, PCA_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    PCA_offset_data = type_engine_import_mvar( env, "int", "@PCA_data", FALSE );
    if( PCA_offset_data == CK_INVALID_OFFSET ) goto error;

    // transform
    func = make_new_sfun( "void", "reduce", PCA_reduce );
    func->add_arg( "float[][]", "input" );
    func->add_arg( "int", "D" );
    func->add_arg( "float[][]", "output" );
    func->doc =
        "dimension-reduce 'input' (NxM) to 'output' (NxD) as the projection of the input data onto its first 'D' principle components";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

    //---------------------------------------------------------------------
    // init as base class: Wekinator
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc =
        "a Wekinator utility that maps input vectors to output vectors, commonly used for interactive machine learning combining human-computer interaction and ML. Based on Rebecca Fiebrink's Wekinator framework.";

    // begin class definition
    if( !type_engine_import_class_begin( env,
                                         "Wekinator",
                                         "Object",
                                         env->global(),
                                         Wekinator_ctor,
                                         Wekinator_dtor,
                                         doc.c_str() ) )
        return FALSE;

    // data offset
    Wekinator_offset_data = type_engine_import_mvar( env, "float", "@Wekinator_data", FALSE );
    if( Wekinator_offset_data == CK_INVALID_OFFSET ) goto error;

    // input
    func = make_new_mfun( "void", "input", Wekinator_input );
    func->add_arg( "float[]", "vec" );
    func->doc = "Set the current input vector.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // output
    func = make_new_mfun( "void", "output", Wekinator_output );
    func->add_arg( "float[]", "vec" );
    func->doc = "Set the current output vector.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // add
    func = make_new_mfun( "void", "add", Wekinator_add );
    func->doc = "Add the current input and output vectors to the training set.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // train
    func = make_new_mfun( "void", "train", Wekinator_train );
    func->doc = "Train the model with the current training set.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // clear
    func = make_new_mfun( "void", "clear", Wekinator_clear );
    func->doc = "Clear the training set.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "void", "predict", Wekinator_predict );
    func->add_arg( "float[]", "input" );
    func->add_arg( "float[]", "output" );
    func->doc = "Predict the output vector from the input vector.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // model type
    func = make_new_mfun( "int", "modelType", Wekinator_ctrl_model_type );
    func->add_arg( "int", "type" );
    func->doc = "Set the model type. Possible values: AI.MLP (default), AI.KNN, AI.SVM.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // model type
    func = make_new_mfun( "int", "modelType", Wekinator_cget_model_type );
    func->doc = "Get the model type. Possible values: AI.MLP (default), AI.KNN, AI.SVM.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // save data
    func = make_new_mfun( "void", "saveData", Wekinator_save_data );
    func->add_arg( "string", "filename" );
    func->doc = "Save the training data to a file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // load data
    func = make_new_mfun( "void", "loadData", Wekinator_load_data );
    func->add_arg( "string", "filename" );
    func->doc = "Load the training data from a file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // input dims
    func = make_new_mfun( "int", "inputDims", Wekinator_cget_input_dims );
    func->doc = "Get the number of input dimensions.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // output dims
    func = make_new_mfun( "int", "outputDims", Wekinator_cget_output_dims );
    func->doc = "Get the number of output dimensions.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // num obs
    func = make_new_mfun( "int", "numObs", Wekinator_cget_num_obs );
    func->doc = "Get the number of observations in the training set.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    //---------------------------------------------------------------------
    // init as base class: MLP
    // 1.4.1.2 added by Yikai Li, Fall 2022
    //---------------------------------------------------------------------
    // doc string
    doc =
        "a multilayer perceptron (MLP; a basic artificial neural network) that maps an input layer to an output layer, across a specified number of fully-connected hidden layers. This  implemention can be trained either 1) by using one of the comprehensive .train() functions OR 2) by 'manually' iterating over calls to .forward() and .backprop() for each input-output observation, and using .shuffle() for each epoch. Commonly used for regression or classification.";

    // begin class definition
    if( !type_engine_import_class_begin( env, "MLP", "Object", env->global(), MLP_ctor, MLP_dtor, doc.c_str() ) )
        return FALSE;

    // data offset
    MLP_offset_data = type_engine_import_mvar( env, "float", "@MLP_data", FALSE );
    if( MLP_offset_data == CK_INVALID_OFFSET ) goto error;

    // init
    func = make_new_mfun( "void", "init", MLP_init );
    func->add_arg( "int[]", "nodesPerLayer" );
    func->doc = "Initialize the MLP with the given number of nodes per layer.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // init2
    func = make_new_mfun( "void", "init", MLP_init2 );
    func->add_arg( "int[]", "nodesPerLayer" );
    func->add_arg( "int[]", "activationPerLayer" );
    func->doc =
        "Initialize the MLP with the given number of nodes per layer and the given activation function per layer, as specified in 'activationPerLayer' (options: AI.Linear, AI.Sigmoid, AI.ReLU, AI.Tanh, or AI.Softmax).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // init3
    func = make_new_mfun( "void", "init", MLP_init3 );
    func->add_arg( "int[]", "nodesPerLayer" );
    func->add_arg( "int", "activationFunction" );
    func->doc =
        "Initialize the MLP with the given number of nodes per layer and the given activation function for all layers (options: AI.Linear, AI.Sigmoid, AI.ReLU, AI.Tanh, or AI.Softmax).";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // train
    func = make_new_mfun( "void", "train", MLP_train );
    func->add_arg( "float[][]", "inputs" );
    func->add_arg( "float[][]", "outputs" );
    func->doc =
        "Train the MLP with the given input and output observations with default learning rate=.001 and epochs=100. (Also see MLP.train(inputs,outputs,learningRate,epochs).)";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // train2
    func = make_new_mfun( "void", "train", MLP_train2 );
    func->add_arg( "float[][]", "inputs" );
    func->add_arg( "float[][]", "outputs" );
    func->add_arg( "float", "learningRate" );
    func->add_arg( "int", "epochs" );
    func->doc = "Train the MLP with the given input and output observations, the learning rate, and number of epochs.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // predict
    func = make_new_mfun( "void", "predict", MLP_predict );
    func->add_arg( "float[]", "input" );
    func->add_arg( "float[]", "output" );
    func->doc = "Predict the output layer from an input layer.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // get_weights
    func = make_new_mfun( "void", "getWeights", MLP_get_weights );
    func->add_arg( "int", "layer" );
    func->add_arg( "float[][]", "weights" );
    func->doc = "Get the weights of the given layer.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // get_biases
    func = make_new_mfun( "void", "getBiases", MLP_get_biases );
    func->add_arg( "int", "layer" );
    func->add_arg( "float[]", "biases" );
    func->doc = "Get the biases of the given layer.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // get_gradients
    func = make_new_mfun( "void", "getGradients", MLP_get_gradients );
    func->add_arg( "int", "layer" );
    func->add_arg( "float[]", "gradients" );
    func->doc = "Get the gradients of the given layer, after a manual .backprop().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // get_activations
    func = make_new_mfun( "void", "getActivations", MLP_get_activations );
    func->add_arg( "int", "layer" );
    func->add_arg( "float[]", "activations" );
    func->doc = "Get the activations of the given layer, after a manual .forward().";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // forward
    func = make_new_mfun( "void", "forward", MLP_forward );
    func->add_arg( "float[]", "input" );
    func->doc = "(Manually) forward-propagate the input vector through the network.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // backprop
    func = make_new_mfun( "void", "backprop", MLP_backprop );
    func->add_arg( "float[]", "output" );
    func->add_arg( "float", "learningRate" );
    func->doc =
        "(Manually) backpropagate from the output layer, for a single input-output observation; compute the gradient of the loss function with respect to the weights in the network, one layer at a time.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // shuffle
    func = make_new_sfun( "void", "shuffle", MLP_shuffle );
    func->add_arg( "float[][]", "X" );
    func->add_arg( "float[][]", "Y" );
    func->doc = "(Manually) shuffle the given input and output vectors.";
    if( !type_engine_import_sfun( env, func ) ) goto error;

    // save
    func = make_new_mfun( "void", "save", MLP_save );
    func->add_arg( "string", "filename" );
    func->doc = "Save the MLP to a file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // load
    func = make_new_mfun( "void", "load", MLP_load );
    func->add_arg( "string", "filename" );
    func->doc = "Load the MLP from a file.";
    if( !type_engine_import_mfun( env, func ) ) goto error;

    // end the class import
    type_engine_import_class_end( env );

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
// name: shuffle()
// desc: shuffle training data
//-----------------------------------------------------------------------------
void shuffle( Chuck_Array4 & X, Chuck_Array4 & Y )
{
    t_CKUINT temp, j;
    for( t_CKINT i = X.size() - 1; i > 0; i-- )
    {
        j = ck_random() % ( i + 1 );
        temp = X.m_vector[i];
        X.m_vector[i] = X.m_vector[j];
        X.m_vector[j] = temp;
        temp = Y.m_vector[i];
        Y.m_vector[i] = Y.m_vector[j];
        Y.m_vector[j] = temp;
    }
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
        w = NULL;
    }

    // destructor
    ~SVM_Object()
    {
        SAFE_DELETE( w );
    }

    // train
    t_CKINT train( Chuck_Array4 & x_, Chuck_Array4 & y_ )
    {
        ChaiMatrixFast<t_CKFLOAT> * x = chuck2chai( x_ );
        ChaiMatrixFast<t_CKFLOAT> * y = chuck2chai( y_ );
        train( *x, *y, x->xDim() );
        SAFE_DELETE( x );
        SAFE_DELETE( y );
        return 0;
    }

    void train( ChaiMatrixFast<t_CKFLOAT> & x, ChaiMatrixFast<t_CKFLOAT> & y, t_CKINT n_sample )
    {
        // init
        SAFE_DELETE( w );
        t_CKINT x_dim = x.yDim(), y_dim = y.yDim();
        w = new ChaiMatrixFast<t_CKFLOAT>( x_dim + 1, y_dim );
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

    // predict
    t_CKINT predict( ChaiVectorFast<t_CKFLOAT> & x_, ChaiVectorFast<t_CKFLOAT> & y_ )
    {
        // init
        t_CKINT x_dim = x_.size();
        t_CKINT y_dim = y_.size();
        // compute
        for( int i = 0; i < y_dim; i++ )
        {
            y_.v( i ) = 0.0;
            for( int j = 0; j < x_dim; j++ )
            {
                y_.v( i ) += w->v( j, i ) * x_.v( j );
            }
            y_.v( i ) += w->v( x_dim, i );
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
        X = NULL;
        Y = NULL;
        weights = NULL;
    }

    // destructor
    ~KNN_Object()
    {
        clear();
    }

    // clear
    void clear()
    {
        SAFE_DELETE( X );
        SAFE_DELETE( Y );
        SAFE_DELETE( weights );
    }

    // train
    t_CKINT train( Chuck_Array4 & x_ )
    {
        clear();
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKINT x_dim = x_i->size();
        X = new ChaiMatrixFast<t_CKFLOAT>( n_sample, x_dim );
        weights = new ChaiVectorFast<t_CKFLOAT>( x_dim );
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
        // init weights
        for( t_CKINT i = 0; i < x_dim; i++ )
        {
            weights->v( i ) = 1.0;
        }
        return 0;
    }

    // train
    void train( ChaiMatrixFast<t_CKFLOAT> & x_, t_CKINT size )
    {
        clear();
        // init
        X = new ChaiMatrixFast<t_CKFLOAT>( size, x_.yDim() );
        weights = new ChaiVectorFast<t_CKFLOAT>( x_.yDim() );
        // copy
        for( t_CKINT i = 0; i < size; i++ )
        {
            for( t_CKINT j = 0; j < x_.yDim(); j++ )
            {
                X->v( i, j ) = x_.v( i, j );
            }
        }
        // init weights
        for( t_CKINT i = 0; i < x_.yDim(); i++ )
        {
            weights->v( i ) = 1.0;
        }
        return;
    }

    // train
    t_CKINT train( Chuck_Array4 & x_, Chuck_Array4 & y_ )
    {
        clear();
        // init
        t_CKINT n_sample = x_.size();
        t_CKUINT v;
        x_.get( 0, &v );
        Chuck_Array8 * x_i = (Chuck_Array8 *)v;
        t_CKUINT x_dim = x_i->size();
        X = new ChaiMatrixFast<t_CKFLOAT>( n_sample, x_dim );
        Y = new ChaiVectorFast<t_CKUINT>( n_sample );
        weights = new ChaiVectorFast<t_CKFLOAT>( x_dim );
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
        // init weights
        for( t_CKINT i = 0; i < x_dim; i++ )
        {
            weights->v( i ) = 1.0;
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
                diff = ( j < query.size() ? query[j] : 0.0 ) - X->v( i, j );
                distance += diff * diff * weights->v( j );
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
    void search3( const vector<t_CKFLOAT> & query,
                  t_CKINT k,
                  Chuck_Array4 & labels_,
                  Chuck_Array4 & indices_,
                  Chuck_Array4 & features_ )
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

    // search; returns labels, indices, and feature vectors
    void search3b( const vector<t_CKFLOAT> & query,
                   t_CKINT k,
                   Chuck_Array4 & indices_,
                   Chuck_Array4 & features_ )
    {
        // init
        ChaiVectorFast<t_CKINT> indices( k );
        // search
        getNearestNeighbors( query, k, indices );
        // resize
        indices_.set_size( k );
        // check dimensions
        if( features_.size() < k )
        {
            Chuck_Array8 * x_i = features_.size() ? (Chuck_Array8 *)features_.m_vector[0] : NULL;
            EM_error3( "KNN: insufficient 'features' matrix provided: %dx%d (expecting %dx%d)",
                       features_.size(), x_i != NULL ? x_i->m_vector.size() : 0, k, this->X->xDim() );
            return;
        }
        // copy
        Chuck_Array8 * x_i;
        for( t_CKINT i = 0; i < k; i++ )
        {
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

    // weigh
    void weigh( const vector<t_CKFLOAT> & weights_ )
    {
        // sanity check
        if( X == NULL )
        {
            EM_error3( "KNN: cannot weigh without training data" );
            return;
        }
        if( weights_.size() != X->yDim() )
        {
            EM_error3( "KNN: invalid 'weights' vector provided: %d (expecting %d)",
                       weights_.size(), X->yDim() );
            return;
        }
        // copy
        for( t_CKINT i = 0; i < weights_.size(); i++ )
        {
            weights->v( i ) = weights_[i];
        }
    }

private:
    ChaiMatrixFast<t_CKFLOAT> * X;
    ChaiVectorFast<t_CKUINT> * Y;
    ChaiVectorFast<t_CKFLOAT> * weights;
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
    if( x == NULL )
    {
        EM_error3( "KNN.train(): NULL input encountered..." );
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
        EM_error3( "KNN.search(): NULL input encountered..." );
        return;
    }

    // search0
    knn->search0( query->m_vector, k, *indices );
}

CK_DLL_MFUN( KNN_search2 )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * query = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT k = GET_NEXT_INT( ARGS );
    Chuck_Array4 * indices = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * features = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );

    // check for NULL
    if( query == NULL || indices == NULL || features == NULL )
    {
        EM_error3( "KNN2.search(): NULL input encountered..." );
        return;
    }

    // search3
    knn->search3b( query->m_vector, k, *indices, *features );
}


CK_DLL_MFUN( KNN_weigh )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN_offset_data );
    // get args
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // weigh
    knn->weigh( weights->m_vector );
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
        EM_error3( "KNN2.train(): NULL input encountered..." );
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
        EM_error3( "KNN2.predict(): NULL input encountered..." );
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
        EM_error3( "KNN2.search(): NULL input encountered..." );
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
        EM_error3( "KNN2.search(): NULL input encountered..." );
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
        EM_error3( "KNN2.search(): NULL input encountered..." );
        return;
    }

    // search3
    knn->search3( query->m_vector, k, *labels, *indices, *features );
}

CK_DLL_MFUN( KNN2_weigh )
{
    // get object
    KNN_Object * knn = (KNN_Object *)OBJ_MEMBER_UINT( SELF, KNN2_offset_data );
    // get args
    Chuck_Array8 * weights = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // weigh
    knn->weigh( weights->m_vector );
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
// name: struct PCA_Object
// desc: Principal Component Analysis Implementation | added 1.4.2.1 (yikai)
//-----------------------------------------------------------------------------
struct PCA_Object
{
public:
    // constructor
    PCA_Object()
    { }

    // destructor
    ~PCA_Object()
    { }

    /*  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */
    static void tred2( ChaiMatrixFast<t_CKFLOAT> & a,
                       t_CKINT m,
                       ChaiVectorFast<t_CKFLOAT> & d,
                       ChaiVectorFast<t_CKFLOAT> & e )
    /* Householder reductiom of matrix a to tridiagomal form.
    Algorithm: Martim et al., Num. Math. 11, 181-195, 1968.
    Ref: Smith et al., Matrix Eigemsystem Routimes -- EISPACK Guide
    Sprimger-Verlag, 1976, pp. 489-494.
    W H Press et al., Numerical Recipes im C, Cambridge U P,
    1988, pp. 373-374.
    Source code adapted from F. Murtagh, Munich, 6 June 1989
    http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
    */
    {
        t_CKINT l, k, j, i;
        t_CKFLOAT scale, hh, h, g, f;

        for( i = m - 1; i > 0; i-- )
        {
            l = i - 1;
            h = scale = 0.0;
            if( l > 0 )
            {
                for( k = 0; k <= l; k++ )
                    scale += fabs( a( i, k ) );
                if( scale == 0.0 )
                    e[i] = a( i, l );
                else
                {
                    for( k = 0; k <= l; k++ )
                    {
                        a( i, k ) /= scale;
                        h += a( i, k ) * a( i, k );
                    }
                    f = a( i, l );
                    g = f > 0 ? -sqrt( h ) : sqrt( h );
                    e[i] = scale * g;

                    h -= f * g;
                    a( i, l ) = f - g;
                    f = 0.0;
                    for( j = 0; j <= l; j++ )
                    {
                        a( j, i ) = a( i, j ) / h;
                        g = 0.0;
                        for( k = 0; k <= j; k++ )
                            g += a( j, k ) * a( i, k );
                        for( k = j + 1; k <= l; k++ )
                            g += a( k, j ) * a( i, k );
                        e[j] = g / h;
                        f += e[j] * a( i, j );
                    }
                    hh = f / ( h + h );
                    for( j = 0; j <= l; j++ )
                    {
                        f = a( i, j );
                        e[j] = g = e[j] - hh * f;
                        for( k = 0; k <= j; k++ )
                            a( j, k ) -= ( f * e[k] + g * a( i, k ) );
                    }
                }
            }
            else
                e[i] = a( i, l );
            d[i] = h;
        }
        d[0] = 0.0;
        e[0] = 0.0;
        for( i = 0; i < m; ++i )
        {
            l = i - 1;
            if( d[i] )
            {
                for( j = 0; j <= l; j++ )
                {
                    g = 0.0;
                    for( k = 0; k <= l; k++ )
                        g += a( i, k ) * a( k, j );
                    for( k = 0; k <= l; k++ )
                        a( k, j ) -= g * a( k, i );
                }
            }
            d[i] = a( i, i );
            a( i, i ) = 1.0;

            for( j = 0; j <= l; j++ )
                a( j, i ) = a( i, j ) = 0.0;
        }
    }

    /*  Tridiagonal QL algorithm -- Implicit  */
    static void tqli( ChaiVectorFast<t_CKFLOAT> & d,
                      ChaiVectorFast<t_CKFLOAT> & e,
                      t_CKINT m,
                      ChaiMatrixFast<t_CKFLOAT> & z )
    /*
     Source code adapted from F. Murtagh, Munich, 6 June 1989
     http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
    */
    {
        t_CKINT n, l, i, k;
        t_CKFLOAT s, r, p, g, f, dd, c, b;

        for( i = 1; i < m; ++i )
            e[i - 1] = e[i];
        e[m - 1] = 0.0;
        for( l = 0; l < m; l++ )
        {
            do
            {
                for( n = l; n < m - 1; n++ )
                {
                    dd = fabs( d[n] ) + fabs( d[n + 1] );
                    if( fabs( e[n] ) + dd == dd ) break;
                }
                if( n != l )
                {
                    g = ( d[l + 1] - d[l] ) / ( 2.0 * e[l] );
                    r = sqrt( ( g * g ) + 1.0 );
                    g = d[n] - d[l] + e[l] / ( g + ( ( g ) < 0 ? -fabs( r ) : fabs( r ) ) );
                    s = c = 1.0;
                    p = 0.0;
                    for( i = n - 1; i >= l; i-- )
                    {
                        f = s * e[i];
                        b = c * e[i];
                        if( fabs( f ) >= fabs( g ) )
                        {
                            c = g / f;
                            r = sqrt( ( c * c ) + 1.0 );
                            e[i + 1] = f * r;
                            c *= ( s = 1.0 / r );
                        }
                        else
                        {
                            s = f / g;
                            r = sqrt( ( s * s ) + 1.0 );
                            e[i + 1] = g * r;
                            s *= ( c = 1.0 / r );
                        }
                        g = d[i + 1] - p;
                        r = ( d[i] - g ) * s + 2.0 * c * b;
                        p = s * r;
                        d[i + 1] = g + p;
                        g = c * r - b;
                        for( k = 0; k < m; k++ )
                        {
                            f = z( k, i + 1 );
                            z( k, i + 1 ) = s * z( k, i ) + c * f;
                            z( k, i ) = c * z( k, i ) - s * f;
                        }
                    }
                    d[l] = d[l] - p;
                    e[l] = g;
                    e[n] = 0.0;
                }
            } while( n != l );
        }
    }

    // transform
    static void transform( Chuck_Array4 & input_, t_CKINT npc, Chuck_Array4 & output_ )
    {
        ChaiMatrixFast<t_CKFLOAT> * input = chuck2chai( input_ );

        t_CKINT t, o;
        t_CKINT o1, o2;
        t_CKINT n = input->xDim(), d = input->yDim();

        ChaiMatrixFast<t_CKFLOAT> temp_matrix( d, n );
        for( t = 0; t < n; t++ )
        {
            for( o = 0; o < d; o++ )
            {
                temp_matrix( o, t ) = input->v( t, o );
            }
        }

        ChaiVectorFast<t_CKFLOAT> means( d );
        ChaiVectorFast<t_CKFLOAT> stds( d );
        for( o = 0; o < d; o++ )
        {
            means[o] = 0;
            for( t = 0; t < n; t++ )
            {
                means[o] += temp_matrix( o, t );
            }
            means[o] /= n;
            stds[o] = 0;
            for( t = 0; t < n; t++ )
            {
                stds[o] += pow( temp_matrix( o, t ) - means[o], 2 );
            }
            stds[o] = sqrt( stds[o] / n );
        }

        // Adjust data : ( X - means(X) ) / ( sqrt(n) * stds(X) )
        for( o = 0; o < d; o++ )
            for( t = 0; t < n; t++ )
                temp_matrix( o, t ) =
                    ( temp_matrix( o, t ) - means[o] ) / ( sqrt( (t_CKFLOAT)n ) * stds[o] );

        ChaiMatrixFast<t_CKFLOAT> corr_matrix( d, d );
        // Calculate the correlation matrix
        for( o1 = 0; o1 < d; o1++ )
        {
            corr_matrix( o1, o1 ) = 1.0;
            for( o2 = o1; o2 < d; o2++ )
            {
                corr_matrix( o1, o2 ) = 0.0;
                for( t = 0; t < n; t++ )
                    corr_matrix( o1, o2 ) += temp_matrix( o1, t ) * temp_matrix( o2, t );
                corr_matrix( o2, o1 ) = corr_matrix( o1, o2 );
            }
        }
        corr_matrix( d - 1, d - 1 ) = 1.0;

        ChaiVectorFast<t_CKFLOAT> evals( d );
        ChaiVectorFast<t_CKFLOAT> interm( d );

        // Triangular decomposition
        tred2( corr_matrix, d, evals, interm );

        // Reduction of symmetric tridiagonal matrix
        tqli( evals, interm, d, corr_matrix );

//        t_CKFLOAT percent_eig = 0.0;
//        t_CKFLOAT sum_eig = 0.0;
//        for( t_CKINT m = d - 2; m >= 0; m-- )
//            sum_eig += evals[m];
//
//        for( t_CKINT m = d - 2; m >= 0; m-- )
//        {
//            percent_eig += evals[m];
//            std::cout << evals[m] / sum_eig << "\t";
//            std::cout << percent_eig / sum_eig << std::endl;
//        }

        /* evals now contains the eigenvalues,
           corr_matrix now contains the associated eigenvectors. */

        /* Project row data onto the top "npc_" principal components. */
        Chuck_Array8 * vi;
        for( t = 0; t < n; t++ )
        {
            vi = (Chuck_Array8 *)output_.m_vector[t];
            for( o = 0; o < npc; o++ )
            {
                vi->m_vector[o] = 0.0;
                for( o1 = 0; o1 < d; o1++ )
                    vi->m_vector[o] += input->v( t, o1 ) * corr_matrix( o1, d - 1 - o );
            }
        }

        // clean up
        SAFE_DELETE( input );
    }

private:
};

//-----------------------------------------------------------------------------
// PCA c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( PCA_ctor )
{
    PCA_Object * pca = new PCA_Object();
    OBJ_MEMBER_UINT( SELF, PCA_offset_data ) = (t_CKUINT)pca;
}

CK_DLL_DTOR( PCA_dtor )
{
    PCA_Object * pca = (PCA_Object *)OBJ_MEMBER_UINT( SELF, PCA_offset_data );
    SAFE_DELETE( pca );
    OBJ_MEMBER_UINT( SELF, PCA_offset_data ) = 0;
}

CK_DLL_SFUN( PCA_reduce )
{
    // get args
    Chuck_Array4 * input = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT n_components = GET_NEXT_INT( ARGS );
    Chuck_Array4 * output = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // transform
    PCA_Object::transform( *input, n_components, *output );
}

//-----------------------------------------------------------------------------
// name: struct MLP_Object
// desc: Multilayer Perceptron Implementation | added 1.4.2.1 (yikai)
//-----------------------------------------------------------------------------
struct MLP_Object
{
private:
    vector<t_CKINT> units_per_layer;
    vector<t_CKINT> activation_per_layer;
    vector<ChaiMatrixFast<t_CKFLOAT> *> weights;
    vector<ChaiVectorFast<t_CKFLOAT> *> biases;
    vector<ChaiVectorFast<t_CKFLOAT> *> activations;
    vector<ChaiVectorFast<t_CKFLOAT> *> gradients;
public:
    // constructor
    MLP_Object()
    { }

    // destructor
    ~MLP_Object()
    {
        clear();
    }

    // clear
    void clear()
    {
        for( t_CKINT i = 0; i < weights.size(); i++ )
            SAFE_DELETE( weights[i] );
        for( t_CKINT i = 0; i < biases.size(); i++ )
            SAFE_DELETE( biases[i] );
        for( t_CKINT i = 0; i < activations.size(); i++ )
            SAFE_DELETE( activations[i] );
        for( t_CKINT i = 0; i < gradients.size(); i++ )
            SAFE_DELETE( gradients[i] );
        units_per_layer.clear();
        activation_per_layer.clear();
        weights.clear();
        biases.clear();
        activations.clear();
        gradients.clear();
    }

    // init
    bool init( const vector<t_CKUINT> & units_per_layer_ )
    {
        // clear
        clear();

        // check
        if( units_per_layer_.size() == 0 )
        {
            EM_error3( "MLP.init(): empty array 'unitsPerLayer'" );
            return false;
        }

        // get units per layer
        for( t_CKINT i = 0; i < units_per_layer_.size(); i++ )
            units_per_layer.push_back( units_per_layer_[i] );
        // allocate
        for( t_CKINT i = 0; i < units_per_layer.size(); i++ )
        {
            activations.push_back( new ChaiVectorFast<t_CKFLOAT>( units_per_layer[i] ) );
            gradients.push_back( new ChaiVectorFast<t_CKFLOAT>( units_per_layer[i] ) );
        }

        for( t_CKINT i = 0; i < units_per_layer.size() - 1; i++ )
        {
            weights.push_back( new ChaiMatrixFast<t_CKFLOAT>( units_per_layer[i + 1], units_per_layer[i] ) );
            biases.push_back( new ChaiVectorFast<t_CKFLOAT>( units_per_layer[i + 1] ) );
            activation_per_layer.push_back( g_at_sigmoid ); // default to sigmoid...
        }
        // set output layer activation function to linear by default
        activation_per_layer.back() = g_at_linear;

        // random
        for( t_CKINT i = 0; i < weights.size(); i++ )
            for( t_CKINT j = 0; j < weights[i]->xDim(); j++ )
                for( t_CKINT k = 0; k < weights[i]->yDim(); k++ )
                    weights[i]->v( j, k ) = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;
        for( t_CKINT i = 0; i < biases.size(); i++ )
            for( t_CKINT j = 0; j < biases[i]->size(); j++ )
                biases[i]->v( j ) = ck_random() / (t_CKFLOAT)CK_RANDOM_MAX;

        // done
        return true;
    }

    // init2
    void init( const vector<t_CKUINT> & units_per_layer_, const vector<t_CKUINT> & activation_per_layer_ )
    {
        if( init( units_per_layer_ ) )
        {
            for( t_CKINT i = 0; i < activation_per_layer_.size(); i++ )
                activation_per_layer.push_back( activation_per_layer_[i] );
        }
    }

    // init3
    void init( const vector<t_CKUINT> & units_per_layer_, t_CKINT activation_function_ )
    {
        if( init( units_per_layer_ ) )
        {
            for( t_CKINT i = 0; i < units_per_layer_.size(); i++ )
                activation_per_layer.push_back( activation_function_ );
        }
    }

    // forward
    void forward( ChaiVectorFast<t_CKFLOAT> & input )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            EM_error3( "MLP.forward(): network not initialized" );
            return;
        }

        // copy
        for( t_CKINT i = 0; i < input.size(); i++ )
            activations[0]->v( i ) = input[i];
        // forward
        t_CKFLOAT v, sum;
        for( t_CKINT i = 0; i < weights.size(); i++ )
        {
            for( t_CKINT j = 0; j < weights[i]->xDim(); j++ )
            {
                v = 0.0;
                for( t_CKINT k = 0; k < weights[i]->yDim(); k++ )
                    v += weights[i]->v( j, k ) * activations[i]->v( k );
                v += biases[i]->v( j );
                if( activation_per_layer[i] == g_at_sigmoid )
                    v = 1.0 / ( 1.0 + exp( -v ) );
                else if( activation_per_layer[i] == g_at_tanh )
                    v = tanh( v );
                else if( activation_per_layer[i] == g_at_relu )
                    v = v > 0.0 ? v : 0.0;
                else if( activation_per_layer[i] == g_at_softmax )
                    v = exp( v );
                else if( activation_per_layer[i] == g_at_linear )
                    v = v;
                activations[i + 1]->v( j ) = v;
            }
            if( activation_per_layer[i] == g_at_softmax )
            {
                sum = 0;
                for( t_CKINT j = 0; j < activations[i + 1]->size(); j++ )
                    sum += activations[i + 1]->v( j );
                for( t_CKINT j = 0; j < activations[i + 1]->size(); j++ )
                    activations[i + 1]->v( j ) /= sum;
            }
        }
    }

    // backprop
    void backprop( ChaiVectorFast<t_CKFLOAT> & output, t_CKFLOAT lr )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            EM_error3( "MLP.backprop(): network not initialized" );
            return;
        }
        // error
        for( t_CKINT i = 0; i < output.size(); i++ )
            gradients.back()->v( i ) = output[i] - activations.back()->v( i );

        // backprop
        t_CKFLOAT v;
        for( t_CKINT i = weights.size() - 1; i >= 0; i-- )
        {
            // prev layer
            for( t_CKINT j = 0; j < weights[i]->yDim(); j++ )
            {
                v = 0.0;
                for( t_CKINT k = 0; k < weights[i]->xDim(); k++ )
                    v += weights[i]->v( k, j ) * gradients[i + 1]->v( k );
                gradients[i]->v( j ) = v;
            }
            // gradient
            for( t_CKINT j = 0; j < weights[i]->xDim(); j++ )
            {
                v = activations[i + 1]->v( j );
                if( activation_per_layer[i] == g_at_sigmoid )
                    v = v * ( 1.0 - v );
                else if( activation_per_layer[i] == g_at_tanh )
                    v = 1.0 - v * v;
                else if( activation_per_layer[i] == g_at_relu )
                    v = v > 0.0 ? 1.0 : 0.0;
                else if( activation_per_layer[i] == g_at_softmax )
                    v = v * ( 1.0 - v );
                else if( activation_per_layer[i] == g_at_linear )
                    v = 1.0;
                v = v * gradients[i + 1]->v( j ) * lr;
                for( t_CKINT k = 0; k < weights[i]->yDim(); k++ )
                    weights[i]->v( j, k ) += v * activations[i]->v( k );
                biases[i]->v( j ) += v;
                gradients[i + 1]->v( j ) = v;
            }
        }
    }

    // train
    void train( ChaiMatrixFast<t_CKFLOAT> & X,
                ChaiMatrixFast<t_CKFLOAT> & Y,
                t_CKINT size,
                t_CKFLOAT lr,
                t_CKINT epochs )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            EM_error3( "MLP.train(): network not initialized" );
            return;
        }

        ChaiVectorFast<t_CKFLOAT> x( X.yDim() );
        ChaiVectorFast<t_CKFLOAT> y( Y.yDim() );
        ChaiVectorFast<t_CKINT> indices( size );
        for( t_CKINT i = 0; i < size; i++ )
            indices[i] = i;
        for( t_CKINT i = 0; i < epochs; i++ )
        {
            indices.shuffle();
            for( t_CKINT j = 0; j < size; j++ )
            {
                for( t_CKINT k = 0; k < X.yDim(); k++ )
                    x[k] = X.v( indices[j], k );
                forward( x );
                for( t_CKINT k = 0; k < Y.yDim(); k++ )
                    y[k] = Y.v( indices[j], k );
                backprop( y, lr );
            }
        }
    }

//    // train
//    void train( Chuck_Array4 & inputs_, Chuck_Array4 & outputs_ )
//    {
//        ChaiMatrixFast<t_CKFLOAT> * X = chuck2chai( inputs_ );
//        ChaiMatrixFast<t_CKFLOAT> * Y = chuck2chai( outputs_ );
//        train( *X, *Y, X->xDim(), 1e-3, 100 );
//        SAFE_DELETE( X );
//        SAFE_DELETE( Y );
//    }

    // train2
    void train( Chuck_Array4 & inputs_, Chuck_Array4 & outputs_, t_CKFLOAT learning_rate, t_CKINT max_iterations )
    {
        ChaiMatrixFast<t_CKFLOAT> * X = chuck2chai( inputs_ );
        ChaiMatrixFast<t_CKFLOAT> * Y = chuck2chai( outputs_ );
        train( *X, *Y, X->xDim(), learning_rate, max_iterations );
        SAFE_DELETE( X );
        SAFE_DELETE( Y );
    }

    // predict
    void predict( Chuck_Array8 & input_, Chuck_Array8 & output_ )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            // error message
            EM_error3( "MLP.predict(): network not initialized" );
            // zero out
            output_.zero();
            // done
            return;
        }
        if( input_.size() != units_per_layer.front() )
        {
            // error message
            EM_error3( "MLP.predict(): input size mismatch" );
            // zero out
            output_.zero();
            // done
            return;
        }

        ChaiVectorFast<t_CKFLOAT> * input = chuck2chai( input_ );
        forward( *input );
        output_.set_size( units_per_layer.back() );
        for( t_CKINT i = 0; i < units_per_layer.back(); i++ )
            output_.m_vector[i] = activations.back()->v( i );
        SAFE_DELETE( input );
    }

    // predict2
    void predict( ChaiVectorFast<t_CKFLOAT> & input_, ChaiVectorFast<t_CKFLOAT> & output_ )
    {
        forward( input_ );
        for( t_CKINT i = 0; i < units_per_layer.back(); i++ )
            output_.v( i ) = activations.back()->v( i );
    }

    // get_weights
    void get_weights( t_CKINT layer, Chuck_Array4 & weights_ )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            // error
            EM_error3( "MLP.getWeights(): network not initialized" );
            // courtesy clear the array
            Chuck_Array8 * wi = NULL;
            for( t_CKINT i = 0; i < weights_.size(); i++ )
            {
                wi = (Chuck_Array8 *)weights_.m_vector[i];
                for( t_CKINT j = 0; j < wi->size(); j++ )
                    wi->m_vector[j] = 0.0;
            }
            return;
        }

        Chuck_Array8 * wi = NULL;
        for( t_CKINT i = 0; i < weights_.size(); i++ )
        {
            wi = (Chuck_Array8 *)weights_.m_vector[i];
            for( t_CKINT j = 0; j < wi->size(); j++ )
                wi->m_vector[j] = weights[layer]->v( j, i );
        }
    }

    // get_biases
    void get_biases( t_CKINT layer, Chuck_Array8 & biases_ )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            // error
            EM_error3( "MLP.getBiases(): network not initialized" );
            // clear
            biases_.clear();
            // done
            return;
        }

        biases_.set_size( biases[layer]->size() );
        for( t_CKINT i = 0; i < biases_.size(); i++ )
            biases_.m_vector[i] = biases[layer]->v( i );
    }

    // get_gradients
    void get_gradients( t_CKINT layer, Chuck_Array8 & gradients_ )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            // error
            EM_error3( "MLP.getGradients(): network not initialized" );
            // clear
            gradients_.clear();
            // done
            return;
        }

        gradients_.set_size( gradients[layer]->size() );
        for( t_CKINT i = 0; i < gradients_.size(); i++ )
            gradients_.m_vector[i] = gradients[layer]->v( i );
    }

    // get_activations
    void get_activations( t_CKINT layer, Chuck_Array8 & activations_ )
    {
        // sanity check
        if( units_per_layer.size() == 0 )
        {
            // error
            EM_error3( "MLP.getActivations(): network not initialized" );
            // clear
            activations_.clear();
            // done
            return;
        }

        activations_.set_size( activations[layer]->size() );
        for( t_CKINT i = 0; i < activations_.size(); i++ )
            activations_.m_vector[i] = activations[layer]->v( i );
    }

    // forward
    void forward( Chuck_Array8 & input_ )
    {
        ChaiVectorFast<t_CKFLOAT> * input = chuck2chai( input_ );
        forward( *input );
        SAFE_DELETE( input );
    }

    // backprop
    void backprop( Chuck_Array8 & output_, t_CKFLOAT learning_rate )
    {
        ChaiVectorFast<t_CKFLOAT> * output = chuck2chai( output_ );
        backprop( *output, learning_rate );
        SAFE_DELETE( output );
    }

    // save
    void save( const string & filename )
    {
        ofstream fout( filename.c_str() );

        fout << "# layers" << endl;
        // fout << units_per_layer.size() << endl;
        for( t_CKINT i = 0; i < units_per_layer.size(); i++ )
            fout << units_per_layer[i] << " ";
        fout << endl;

        fout << "# activation functions (0=Linear, 1=Sigmoid, 2=ReLU, 3=Tanh, 4=Softmax)" << endl;
        for( t_CKINT i = 0; i < activation_per_layer.size(); i++ )
            fout << activation_per_layer[i] << " ";
        fout << endl;

        fout << "# weights" << endl;
        for( t_CKINT i = 0; i < weights.size(); i++ )
        {
            for( t_CKINT j = 0; j < weights[i]->xDim(); j++ )
            {
                for( t_CKINT k = 0; k < weights[i]->yDim(); k++ )
                    fout << weights[i]->v( j, k ) << " ";
                fout << endl;
            }
        }

        fout << "# biases" << endl;
        for( t_CKINT i = 0; i < biases.size(); i++ )
        {
            for( t_CKINT j = 0; j < biases[i]->size(); j++ )
                fout << biases[i]->v( j ) << " ";
            fout << endl;
        }
    }

    // read next non-empty or commented line
    t_CKBOOL nextline( std::ifstream & fin, string & line, t_CKBOOL commentIsHash )
    {
        // skip over empty lines and commented lines (starts with #)
        do
        {
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
    void load( const string & filename )
    {
        // fin
        ifstream fin( filename.c_str() );
        string line;
        t_CKINT num;

        vector<t_CKUINT> units_per_layer_;
        nextline( fin, line, TRUE ); // get nodes per layer
        istringstream strin( line );

        // tokenize
        while( strin >> num )
        {
            units_per_layer_.push_back( num );
        }
        t_CKINT num_layers = units_per_layer_.size();

        // initialize
        init( units_per_layer_ );

        strin.clear();
        nextline( fin, line, TRUE );
        strin.str( line );
        for( t_CKINT i = 0; i < num_layers - 1; i++ )
            strin >> activation_per_layer[i];

        weights.resize( num_layers - 1 );
        for( t_CKINT i = 0; i < num_layers - 1; i++ )
        {
            weights[i] = new ChaiMatrixFast<t_CKFLOAT>( units_per_layer[i + 1], units_per_layer[i] );
            for( t_CKINT j = 0; j < units_per_layer[i + 1]; j++ )
            {
                strin.clear();
                nextline( fin, line, TRUE );
                strin.str( line );
                for( t_CKINT k = 0; k < units_per_layer[i]; k++ )
                    strin >> weights[i]->v( j, k );
            }
        }

        biases.resize( num_layers - 1 );
        for( t_CKINT i = 0; i < num_layers - 1; i++ )
        {
            biases[i] = new ChaiVectorFast<t_CKFLOAT>( units_per_layer[i + 1] );
            strin.clear();
            nextline( fin, line, TRUE );
            strin.str( line );
            for( t_CKINT j = 0; j < units_per_layer[i + 1]; j++ )
                strin >> biases[i]->v( j );
        }
    }
};


//-----------------------------------------------------------------------------
// MLP c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( MLP_ctor )
{
    MLP_Object * mlp = new MLP_Object();
    OBJ_MEMBER_UINT( SELF, MLP_offset_data ) = (t_CKUINT)mlp;
}

CK_DLL_DTOR( MLP_dtor )
{
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    SAFE_DELETE( mlp );
    OBJ_MEMBER_UINT( SELF, MLP_offset_data ) = 0;
}

CK_DLL_MFUN( MLP_init )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array4 * units_per_layer = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // init
    mlp->init( units_per_layer->m_vector );
}

CK_DLL_MFUN( MLP_init2 )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array4 * units_per_layer = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * activation_per_layer = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // init
    mlp->init( units_per_layer->m_vector, activation_per_layer->m_vector );
}

CK_DLL_MFUN( MLP_init3 )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array4 * units_per_layer = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    t_CKINT activation_function = GET_NEXT_INT( ARGS );
    // init
    mlp->init( units_per_layer->m_vector, activation_function );
}

CK_DLL_MFUN( MLP_train )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array4 * inputs = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * outputs = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // train, with defaults
    mlp->train( *inputs, *outputs, 1e-3, 100 );
}

CK_DLL_MFUN( MLP_train2 )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array4 * inputs = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * outputs = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    t_CKFLOAT learning_rate = GET_NEXT_FLOAT( ARGS );
    t_CKINT max_iterations = GET_NEXT_INT( ARGS );
    // train2
    mlp->train( *inputs, *outputs, learning_rate, max_iterations );
}

CK_DLL_MFUN( MLP_predict )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array8 * input = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // predict
    mlp->predict( *input, *output );
}

CK_DLL_MFUN( MLP_get_weights )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    t_CKINT layer = GET_NEXT_INT( ARGS );
    Chuck_Array4 * weights = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // get_weights
    mlp->get_weights( layer, *weights );
}

CK_DLL_MFUN( MLP_get_biases )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    t_CKINT layer = GET_NEXT_INT( ARGS );
    Chuck_Array8 * biases = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // get_biases
    mlp->get_biases( layer, *biases );
}

CK_DLL_MFUN( MLP_get_gradients )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    t_CKINT layer = GET_NEXT_INT( ARGS );
    Chuck_Array8 * gradients = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // get_gradients
    mlp->get_gradients( layer, *gradients );
}

CK_DLL_MFUN( MLP_get_activations )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    t_CKINT layer = GET_NEXT_INT( ARGS );
    Chuck_Array8 * activations = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // get_activations
    mlp->get_activations( layer, *activations );
}

CK_DLL_MFUN( MLP_forward )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array8 * input = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // forward
    mlp->forward( *input );
}

CK_DLL_MFUN( MLP_backprop )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    t_CKFLOAT learning_rate = GET_NEXT_FLOAT( ARGS );
    // backprop
    mlp->backprop( *output, learning_rate );
}

CK_DLL_MFUN( MLP_save )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_String * filename = GET_NEXT_STRING( ARGS );
    // save
    mlp->save( filename->str() );
}

CK_DLL_MFUN( MLP_load )
{
    // get object
    MLP_Object * mlp = (MLP_Object *)OBJ_MEMBER_UINT( SELF, MLP_offset_data );
    // get args
    Chuck_String * filename = GET_NEXT_STRING( ARGS );
    // load
    mlp->load( filename->str() );
}

CK_DLL_SFUN( MLP_shuffle )
{
    // get args
    Chuck_Array4 * X = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array4 * Y = (Chuck_Array4 *)GET_NEXT_OBJECT( ARGS );
    // shuffle
    shuffle( *X, *Y );
}

//-----------------------------------------------------------------------------
// name: struct Wekinator_Object
// desc: Wekinator Implementation | added 1.4.2.1 (yikai)
//-----------------------------------------------------------------------------
struct Wekinator_Object
{
private:
    ChaiVectorFast<t_CKFLOAT> * x;
    ChaiVectorFast<t_CKFLOAT> * y;
    ChaiMatrixFast<t_CKFLOAT> * X;
    ChaiMatrixFast<t_CKFLOAT> * Y;
    t_CKINT Xi;

    // vector to do multi-target regression (see below)
    vector<MLP_Object *> mlps;
    vector<KNN_Object *> knns;
    vector<SVM_Object *> svms;

    //-------------------------------------------------------------------------
    // whether to train a separate model for each output dimension
    // t_CKBOOL trainModelPerOutput;
    /* looool thanks chatGPT; sounds astoundingly competent
    Ge: in working with multi-layer perceptrons, what is the difference
       between having all output dimensions in one network, versus having a
       separate network trained on each dimension of the output?

    chatGPT: In multi-layer perceptrons (MLPs), the number of output dimensions
       represents the number of distinct classes or values that the model can predict.

    Having all output dimensions in one network means that the MLP outputs
       vector with multiple dimensions, each representing a distinct class
       or value that the model can predict. This approach is often used in
       multi-class classification problems or in problems where the output
       is a vector of continuous values (e.g., regression problems). The
       advantage of this approach is that the model can learn complex relationships
       between input features and output classes/values, and it can capture
       dependencies between different output dimensions.

    On the other hand, having a separate network trained on each dimension of
       the output means that multiple MLPs are trained, each one responsible
       for predicting a single dimension of the output. This approach is often
       used in problems where the output dimensions are independent of each
       other (e.g., multi-target regression problems). The advantage of this
       approach is that it can be easier to train and optimize each individual
       MLP, and it can also be more computationally efficient than training a
       single MLP with multiple output dimensions.

    In general, whether to use a single MLP with multiple output dimensions or
       multiple MLPs depends on the specific problem and the nature of the
       output variables. It's worth noting that some architectures, such as the
       encoder-decoder architecture, combine both approaches by using a single
       etwork to encode input features and separate networks to decode the
       output into multiple dimensions. */
    //-------------------------------------------------------------------------

public:
    t_CKINT model_type;
    // constructor
    Wekinator_Object()
    {
        x = NULL;
        y = NULL;
        X = NULL;
        Y = NULL;
        Xi = 0;
        model_type = 0;
    }

    // destructor
    ~Wekinator_Object()
    {
        clear();
    }

    // input
    void input( Chuck_Array8 & vec_ )
    {
        // check
        if( !x )
        {
            x = new ChaiVectorFast<t_CKFLOAT>( vec_.size() );
        }
        else if( x->size() != vec_.size() )
        {
            EM_error3( "Wekinator: input vector size mismatch: %d != %d",
                       x->size(), vec_.size() );
        }
        // copy
        for( t_CKINT i = 0; i < vec_.size(); i++ )
            x->v( i ) = vec_.m_vector[i];
    }

    // output
    void output( Chuck_Array8 & vec_ )
    {
        // check
        if( !y )
        {
            y = new ChaiVectorFast<t_CKFLOAT>( vec_.size() );
        }
        else if( y->size() != vec_.size() )
        {
            EM_error3( "Wekinator: output vector size mismatch: %d != %d",
                       y->size(), vec_.size() );
        }
        // copy
        for( t_CKINT i = 0; i < vec_.size(); i++ )
            y->v( i ) = vec_.m_vector[i];
    }

    // add
    void add()
    {
        // check
        if( !X )
        {
            X = new ChaiMatrixFast<t_CKFLOAT>( 100, x->size() );
            Y = new ChaiMatrixFast<t_CKFLOAT>( 100, y->size() );
        }
        else if( X->xDim() == Xi )
        {
            ChaiMatrixFast<t_CKFLOAT> * X_ = new ChaiMatrixFast<t_CKFLOAT>( Xi * 2, x->size() );
            ChaiMatrixFast<t_CKFLOAT> * Y_ = new ChaiMatrixFast<t_CKFLOAT>( Xi * 2, y->size() );
            for( t_CKINT i = 0; i < Xi; i++ )
            {
                for( t_CKINT j = 0; j < x->size(); j++ )
                    X_->v( i, j ) = X->v( i, j );
                for( t_CKINT j = 0; j < y->size(); j++ )
                    Y_->v( i, j ) = Y->v( i, j );
            }
            SAFE_DELETE( X );
            SAFE_DELETE( Y );
            X = X_;
            Y = Y_;
        }
        // copy
        for( t_CKINT i = 0; i < x->size(); i++ )
            X->v( Xi, i ) = x->v( i );
        for( t_CKINT i = 0; i < y->size(); i++ )
            Y->v( Xi, i ) = y->v( i );
        // increment
        Xi++;
    }

    // train
    void train()
    {
        // train
        if( model_type == g_mt_mlp )
        {
            for( t_CKINT i = 0; i < mlps.size(); i++ )
                SAFE_DELETE( mlps[i] );
            mlps.clear();

            for( t_CKINT i = 0; i < y->size(); i++ )
            {
                mlps.push_back( new MLP_Object() );
                // init
                vector<t_CKUINT> units_per_layer( 3 );
                units_per_layer[0] = x->size(); // input layer
                units_per_layer[1] = x->size(); // hidden layer: same number of nodes as input layer
                units_per_layer[2] = 1; // output layer
                mlps[i]->init( units_per_layer );
                // slice Y
                ChaiMatrixFast<t_CKFLOAT> Y_( Y->xDim(), 1 );
                for( t_CKINT j = 0; j < Y->xDim(); j++ )
                    Y_.v( j, 0 ) = Y->v( j, i );
                // train
                mlps[i]->train( *X, Y_, Xi, 1e-3, 100 );
            }
        }
        else if( model_type == g_mt_knn )
        {
            for( t_CKINT i = 0; i < knns.size(); i++ )
                SAFE_DELETE( knns[i] );
            knns.clear();

            knns.push_back( new KNN_Object() );
            knns[0]->train( *X, Xi );
        }
        else if( model_type == g_mt_svm )
        {
            for( t_CKINT i = 0; i < svms.size(); i++ )
                SAFE_DELETE( svms[i] );
            svms.clear();

            for( t_CKINT i = 0; i < y->size(); i++ )
            {
                svms.push_back( new SVM_Object() );
                // slice Y
                ChaiMatrixFast<t_CKFLOAT> Y_( Y->xDim(), 1 );
                for( t_CKINT j = 0; j < Y->xDim(); j++ )
                    Y_.v( j, 0 ) = Y->v( j, i );
                // train
                svms[i]->train( *X, Y_, Xi );
            }
        }
    }

    // clear
    void clear()
    {
        SAFE_DELETE( x );
        SAFE_DELETE( y );
        SAFE_DELETE( X );
        SAFE_DELETE( Y );
        Xi = 0;
        for( t_CKINT i = 0; i < mlps.size(); i++ )
            SAFE_DELETE( mlps[i] );
        for( t_CKINT i = 0; i < knns.size(); i++ )
            SAFE_DELETE( knns[i] );
        for( t_CKINT i = 0; i < svms.size(); i++ )
            SAFE_DELETE( svms[i] );
        mlps.clear();
        knns.clear();
        svms.clear();
        model_type = 0;
    }

    // predict
    void predict( Chuck_Array8 & input_, Chuck_Array8 & output_ )
    {
        if( model_type == 0 )
        {
            ChaiVectorFast<t_CKFLOAT> * input = chuck2chai( input_ );
            ChaiVectorFast<t_CKFLOAT> * output = new ChaiVectorFast<t_CKFLOAT>( 1 );
            for( t_CKINT i = 0; i < mlps.size(); i++ )
            {
                mlps[i]->predict( *input, *output );
                output_.m_vector[i] = output->v( 0 );
            }
            SAFE_DELETE( input );
            SAFE_DELETE( output );
        }
        else if( model_type == 1 )
        {
            ChaiVectorFast<t_CKINT> indices( 10 );
            knns[0]->getNearestNeighbors( input_.m_vector, 10, indices );
            output_.set_size( Y->yDim() );
            for( t_CKINT i = 0; i < Y->yDim(); i++ )
            {
                output_.m_vector[i] = 0;
                t_CKFLOAT sum = 0;
                for( t_CKINT j = 0; j < 10; j++ )
                {
                    sum += Y->v( indices.v( j ), i );
                }
                output_.m_vector[i] = sum / 10;
            }
        }
        else if( model_type == 2 )
        {
            ChaiVectorFast<t_CKFLOAT> * input = chuck2chai( input_ );
            ChaiVectorFast<t_CKFLOAT> * output = new ChaiVectorFast<t_CKFLOAT>( 1 );
            for( t_CKINT i = 0; i < svms.size(); i++ )
            {
                svms[i]->predict( *input, *output );
                output_.m_vector[i] = output->v( 0 );
            }
            SAFE_DELETE( input );
            SAFE_DELETE( output );
        }
    }

    // save
    void save_data( const string & filename )
    {
        ofstream fout( filename.c_str() );

        string header = "@relation dataset"
                        "\n"
                        "@attribute ID numeric"
                        "@attribute Time date 'yyyy/MM/dd HH:mm:ss:SSS'"
                        "@attribute 'Training round' numeric";
        fout << header << endl;

        for( t_CKINT i = 0; i < x->size(); i++ )
        {
            fout << "@attribute inputs-" << i << " numeric" << endl;
        }
        for( t_CKINT i = 0; i < y->size(); i++ )
        {
            fout << "@attribute outputs-" << i << " numeric" << endl;
        }

        fout << "\n@data" << endl;
        for( t_CKINT i = 0; i < Xi; i++ )
        {
            fout << i << ", 2023/01/01 00:00:00:000, 1";
            for( t_CKINT j = 0; j < x->size(); j++ )
            {
                fout << ", " << X->v( i, j );
            }
            for( t_CKINT j = 0; j < y->size(); j++ )
            {
                fout << ", " << Y->v( i, j );
            }
            fout << endl;
        }
    }

    // load
    void load_data( const string & filename )
    {
        // clear
        clear();
        // init
        t_CKINT x_size = 0, y_size = 0;
        ifstream fin( filename.c_str() );
        string line;
        while( getline( fin, line ) )
        {
            if( line.find( "@attribute" ) != string::npos )
            {
                if( line.find( "inputs" ) != string::npos )
                {
                    x_size++;
                }
                else if( line.find( "outputs" ) != string::npos )
                {
                    y_size++;
                }
            }
            else if( line.find( "@data" ) != string::npos )
            {
                break;
            }
        }
        x = new ChaiVectorFast<t_CKFLOAT>( x_size );
        y = new ChaiVectorFast<t_CKFLOAT>( y_size );

        // load
        stringstream ss;
        string token;
        vector<string> tokens;
        while( getline( fin, line ) )
        {
            ss.clear();
            ss.str( line );
            tokens.clear();
            while( getline( ss, token, ',' ) )
            {
                tokens.push_back( token );
            }
            for( t_CKINT i = 0; i < x_size; i++ )
            {
                x->v( i ) = atof( tokens[i + 3].c_str() );
            }
            for( t_CKINT i = 0; i < y_size; i++ )
            {
                y->v( i ) = atof( tokens[i + x_size + 3].c_str() );
            }
            add();
        }

        // don't retrain here; should be called explicitly
        // train();
    }

    t_CKINT get_input_dims()
    {
        return x == NULL ? 0 : x->size();
    }

    t_CKINT get_output_dims()
    {
        return y == NULL ? 0 : y->size();
    }

    t_CKINT get_num_obs()
    {
        return Xi;
    }
};

//-----------------------------------------------------------------------------
// Wekinator c++ hooks
//-----------------------------------------------------------------------------
CK_DLL_CTOR( Wekinator_ctor )
{
    Wekinator_Object * wekinator = new Wekinator_Object();
    OBJ_MEMBER_UINT( SELF, Wekinator_offset_data ) = (t_CKUINT)wekinator;
}

CK_DLL_DTOR( Wekinator_dtor )
{
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    SAFE_DELETE( wekinator );
    OBJ_MEMBER_UINT( SELF, Wekinator_offset_data ) = 0;
}

CK_DLL_MFUN( Wekinator_input )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    Chuck_Array8 * vec = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // input
    wekinator->input( *vec );
}

CK_DLL_MFUN( Wekinator_output )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    Chuck_Array8 * vec = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // output
    wekinator->output( *vec );
}

CK_DLL_MFUN( Wekinator_add )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // add
    wekinator->add();
}

CK_DLL_MFUN( Wekinator_train )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // train
    wekinator->train();
}

CK_DLL_MFUN( Wekinator_clear )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // clear
    wekinator->clear();
}

CK_DLL_MFUN( Wekinator_predict )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    Chuck_Array8 * input = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    Chuck_Array8 * output = (Chuck_Array8 *)GET_NEXT_OBJECT( ARGS );
    // predict
    wekinator->predict( *input, *output );
}

CK_DLL_CTRL( Wekinator_ctrl_model_type )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    t_CKINT model_type = GET_NEXT_INT( ARGS );
    // set
    wekinator->model_type = model_type;
    // pass through
    RETURN->v_int = model_type;
}

CK_DLL_CGET( Wekinator_cget_model_type )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get
    RETURN->v_int = wekinator->model_type;
}

CK_DLL_MFUN( Wekinator_save_data )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    Chuck_String * filename = (Chuck_String *)GET_NEXT_OBJECT( ARGS );
    // save
    wekinator->save_data( filename->str() );
}

CK_DLL_MFUN( Wekinator_load_data )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get args
    Chuck_String * filename = (Chuck_String *)GET_NEXT_OBJECT( ARGS );
    // load
    wekinator->load_data( filename->str() );
}

CK_DLL_CGET( Wekinator_cget_input_dims )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get
    RETURN->v_int = wekinator->get_input_dims();
}

CK_DLL_CGET( Wekinator_cget_output_dims )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get
    RETURN->v_int = wekinator->get_output_dims();
}

CK_DLL_CGET( Wekinator_cget_num_obs )
{
    // get object
    Wekinator_Object * wekinator = (Wekinator_Object *)OBJ_MEMBER_UINT( SELF, Wekinator_offset_data );
    // get
    RETURN->v_int = wekinator->get_num_obs();
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
