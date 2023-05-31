//---------------------------------------------------------------------
// name: mlp-load.ck
// desc: loading an MLP model from file (created by MLP.save())
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

// file name
"model.txt" => string filename;
// print
<<< "loading model from file:", filename >>>;
// load the model into network (created by MLP.save() elsewhere)
if( !mlp.load( me.dir() + filename ) ) me.exit();

// predict given a new input
[0.7, 0.8, 0.9] @=> float x[];
// array to how output
float y[2];

// predict output given input
mlp.predict(x, y);
// print the output
<<< y[0], y[1] >>>;
