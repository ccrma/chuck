//---------------------------------------------------------------------
// name: wekinator.ck
// desc: basic example using the Wekinator example in ChucK,
//       based on Rebecca Fiebrink's Wekinator framework
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

// instantiate a global Wekinator, so other files can easily use
global Wekinator wek;

// clear training set
wek.clear();

// input and output (could be set in another file)
wek.input( [0.1, 0.2, 0.3] );
wek.output( [0.4, 0.5] );

// print
cherr <= "adding"; cherr.flush();
// collect data
for( int i; i < 10; i++ )
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

// model type
<<< "model type: ", "MLP" >>>;
// set model type
AI.MLP => wek.modelType;
// train using current training set
wek.train();

// input
[0.4, 0.5, 0.6] @=> float x[];
// output
float y[2];
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
