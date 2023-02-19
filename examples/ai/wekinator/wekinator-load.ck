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

// instantiate a global Wekinator, so other files can easily use
global Wekinator wek;

// print
cherr <= "loading observations from file..." <= IO.newline();
// load an ARFF data file containing input-output observations
// (generated either from Wekinator.save() or Wekinator app)
wek.loadData( me.dir() + "currentData.arff" );

// print what we loaded
cherr <= "inputDims: " <= wek.inputDims() <= " "
      <= "outputDims: " <= wek.outputDims() <= " "
      <= "numObs: " <= wek.numObs() <= IO.newline();

// train using current observations
wek.train();

// input (dimensions should match inputDims)
[0.3, 0.25, 0.4] @=> float x[];
// output (dimensions should match outputDims)
float y[2];
// predict output based on input
wek.predict(x, y);
// print
<<< y[0], y[1] >>>;
