//---------------------------------------------------------------------
// name: wekinator-load.ck
// desc: this examples uses the built-in Wekinator object and loads
//       training data from an .arff file, which can be saved either
//       from the Wekinator app or using Wekinator.saveData().
//
// version: need chuck version 1.4.2.1 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment for Wekinator API:
// Wekinator.help();
//
// authors: Yikai Li
//          Ge Wang (https://ccrma.stanford.edu/~ge/)
//          Rebecca Fiebrink (original Wekinator framework)
// date: Winter 2023
//---------------------------------------------------------------------

// instantiate a global Wekinator, so other files can easily use
global Wekinator wek;

// print
cherr <= "loading observations from file..." <= IO.newline();
// load an ARFF data file containing input-output observations
// (generated either from Wekinator.save() or Wekinator app)
// NOTE: Wekinator.loadData() clears existing training data,
//       and uses the inputDims and outputDims inferred from file
// NOTE: it is possible to add additional observations after
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
float y[wek.outputDims()];
// predict output based on input
wek.predict( x, y );

// print
for( int i; i < y.size(); i++ ) { cherr <= y[i] <= " "; }
cherr <= IO.newline();
