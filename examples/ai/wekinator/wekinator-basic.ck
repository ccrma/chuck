//---------------------------------------------------------------------
// name: wekinator.ck
// desc: basic example using the Wekinator example in ChucK,
//       based on Rebecca Fiebrink's Wekinator framework;
//       This example adds two groups of 5 observations each,
//       trains, and predicts output based on a new input
//
// version: need chuck version 1.4.2.1 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment for Wekinator API:
// Wekinator.help();
//
// author: Yikai Li
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Winter 2023
//---------------------------------------------------------------------

// instantiate a global Wekinator, so other files can access
global Wekinator wek;

// clear training observations
wek.clear();

// print
cherr <= "adding group 1"; cherr.flush();
// input and output (could be set in another file)
wek.input( [0.1, 0.1, 0.1] ); wek.output( [48.0, 60.0] );
// collect data
for( int i; i < 5; i++ )
{
    // print
    cherr <= "."; cherr.flush();
    // add current input and output (typically these are different)
    wek.add();
    // advance time
    50::ms => now;
}
// print
cherr <= IO.newline();

// print
cherr <= "adding group 2"; cherr.flush();
// input and output (could be set in another file)
wek.input( [0.5, 0.5, 0.5] ); wek.output( [5.0, 8.0] );
// collect data
for( int i; i < 5; i++ )
{
    // print
    cherr <= "."; cherr.flush();
    // add current input and output (typically these are different)
    wek.add();
    // advance time
    50::ms => now;
}
// print
cherr <= IO.newline();

// (optional) save the observations for loading later
wek.saveData( me.dir() + "currentData.arff" );

// new input
[0.3, 0.25, 0.4] @=> float x[];
// to hold predicted output
float y[2];

// model type
<<< "model type: ", "MLP" >>>;
// set model type
AI.MLP => wek.modelType;
// train using current training set
wek.train();
// predict output based on input
wek.predict(x, y);
// print
<<< y[0], y[1] >>>;

// model type
<<< "Model type: ", "KNN" >>>;
// set model type
AI.KNN => wek.modelType;
// train
wek.train();
// predict output based on input
wek.predict(x, y);
// print
<<< y[0], y[1] >>>;

// model type
<<< "Model type: ", "SVM" >>>;
// predict output based on input
AI.SVM => wek.modelType;
// train
wek.train();
// predict
wek.predict(x, y);
// print
<<< y[0], y[1] >>>;
