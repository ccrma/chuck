//---------------------------------------------------------------------
// name: mlp.ck
// desc: training a multilayer perception (MLP),
//       a basic artificial neural network
//       (for a step-by-step example, see mlp-manual.ck)
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
// learning rate
0.01 => float learningRate;
// number of epochs
100 => int epochs;

// nodes per layer: input, [hidden layer(s)], output
[3, 5, 5, 2] @=> int nodesPerLayer[];

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
// print
<<< "training model...", "" >>>;
// train the network
mlp.train( X, Y, learningRate, epochs );
// ALTERNATE: train with default learning_rate=0.01, epochs=100
// mlp.train(X, Y);

// save the network into a model file, which can loaded later
// mlp.save( me.dir() + "model.txt" );

// predict given a new input
[0.7, 0.8, 0.9] @=> float x[];
// array to how output
float y[2];
// predict output given input
mlp.predict(x, y);
// print the output
<<< "predicted output:", y[0], y[1] >>>;
