//---------------------------------------------------------------------
// name: mlp-load.ck
// desc: loading an MLP model from file (created by MLP.save())
//
// version: need chuck version 1.4.2.1 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment for MLP API:
// MLP.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// instantiate a multi-layer perception
MLP mlp;
// load the model into network (created by MLP.save() elsewhere)
mlp.load( me.dir() + "model.txt");

// predict given a new input
[0.7, 0.8, 0.9] @=> float x[];
// array to how output
float y[2];

// predict output given input
mlp.predict(x, y);
// print the output
<<< y[0], y[1] >>>;
