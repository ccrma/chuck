//---------------------------------------------------------------------
// name: model-save.ck
// desc: training a basic artificial neural network and save the
//       parameters to a model file, which can later be loaded
//       using MLP.load()
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment for MLP API:
// MLP.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// instantiate a multilayer perception
MLP mlp;

// nodes per layer: input, [hidden layer(s)], output
[3, 5, 2] @=> int nodesPerLayer[];
// initialize with the units per layer with default activation function
// default: AI.Sigmoid for all layers except output layer;
// default: AI.Linear for output layer
mlp.init( nodesPerLayer );

// (ALTERNATE #1) initialize with activation function
// mlp.init( nodesPerLayer, AI.Sigmoid );

// (ALTERNATE #2) specify activation function per layer
// [AI.ReLU, AI.Tanh, AI.Softmax] @=> int activationPerLayer[];
// (ALTERNATE #2) initialize with activation function per layer
// mlp.init( nodesPerLayer, activationPerLayer );

// input observations
[[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]] @=> float X[][];
// output observations
[[0.1, 0.2], [0.3, 0.4]] @=> float Y[][];
// learning rate
0.001 => float learningRate;
// number of epochs
100 => int epochs;
// train the network
mlp.train( X, Y, learningRate, epochs );
// ALTERNATE: train with default learning_rate=0.001, epochs=100
// mlp.train(X, Y);

// file name
"model2.txt" => string filename;
// save the network
mlp.save( me.dir() + filename );
// print
<<< "saved model to file:", filename >>>;
