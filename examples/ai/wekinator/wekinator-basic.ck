//---------------------------------------------------------------------
// name: wekinator-basic.ck
// desc: basic example using the built-in Wekinator object in ChucK,
//       based on Rebecca Fiebrink's Wekinator framework;
//       This example adds two groups of 3-2 input-output
//       observations, trains, and predicts output based on new input.
//       default task: regression
//       default model: MLPs (one hidden layer with same # of nodes as input)
//
// version: need chuck version 1.5.0.0 or higher
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

// instantiate a global Wekinator, so other files can access
global Wekinator wek;

// clear training observations
wek.clear();

// group count
1 => int groupNum;

cherr <= "-----------" <= IO.newline();
// add a group of data
addGroup( [0.1, 0.1, 0.1], [48.0, 60.0], 20 );
addGroup( [0.4, 0.4, 0.4], [5.0, 8.0], 20 );
addGroup( [0.8, 0.0, 0.4], [30.0, 30.0], 20 );
cherr <= "-----------" <= IO.newline();

// (optional) set model type; MLP is default
// AI.MLP => wek.modelType;
// (optional) set task type; regression is default
AI.Regression => wek.taskType;
// print
<<< "modelType:", wek.modelTypeName(),
    "| taskType:", wek.taskTypeName() >>>;

// (optional) set properties to your liking
// <<< "changing Wekinator MLP properties...", "" >>>;
// wek.setProperty( AI.MLP, "hiddenLayers", 1 );
// note: 0 nodesPerHiddenLayer means "same as input layer"
// wek.setProperty( AI.MLP, "nodesPerHiddenLayer", 0 );
// wek.setProperty( AI.MLP, "learningRate", 0.01 );
// wek.setProperty( AI.MLP, "epochs", 100 );

 // print model properties
<<< "Wekinator MLP hiddenLayers:", wek.getPropertyInt( AI.MLP, "hiddenLayers" ) >>>;
<<< "Wekinator MLP nodesPerHiddenLayer:", wek.getPropertyInt( AI.MLP, "nodesPerHiddenLayer" ) >>>;
<<< "Wekinator MLP learningRate", wek.getPropertyFloat( AI.MLP, "learningRate" ) >>>;
<<< "Wekinator MLP epochs:", wek.getPropertyInt( AI.MLP, "epochs" ) >>>;
// (optional) save the observations for loading later
// wek.exportObs( me.dir() + "currentData.arff" );

// print
cherr <= "-----------" <= IO.newline();
cherr <= "training..." <= IO.newline();
cherr <= "-----------" <= IO.newline();
// train using current training set
wek.train();

// new input
float x[];
// to hold predicted output
float y[2];

cherr <= "given new input, predict output..." <= IO.newline();
// predict and print
predict( [0.101, 0.1, 0.1], y );
predict( [0.401, 0.4, 0.4], y );
predict( [0.801, 0.0, 0.4], y );

// test run
repeat( 20 )
{
    // generate random input
    [Math.random2f(0,1), Math.random2f(0,1), Math.random2f(0,1)] @=> x;
    // predict and print
    predict( x, y );
    // advance time (just for effect)
    50::ms => now;
}


// add a group of training observations
fun void addGroup( float inputs[], float outputs[], int N )
{
    // print
    cherr <= "adding group " <= groupNum++;
    // collect data
    repeat( N )
    {
        // input and output (could be set in another file)
        wek.input( inputs ); wek.output( outputs );
        // add current input and output (typically these are different)
        wek.add();
        // print
        cherr <= ".";
        // advance time (delay on purpose to visualize the process)
        (300.0/N)::ms => now;
    }
    // print
    cherr <= IO.newline();
}

// predict and print
fun void predict( float inputs[], float outputs[] )
{
    // predict output based on input; 3 inputs -> 2 outputs
    wek.predict(inputs, outputs);
    // print
    cherr <= "(" <= inputs[0] <= ","<= inputs[1] <= ","<= inputs[2] <= ") -> ("
          <= outputs[0] <= ", " <= outputs[1] <= ")" <= IO.newline();
}
