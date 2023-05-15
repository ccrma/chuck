//---------------------------------------------------------------------
// name: wekinator-customize.ck
// desc: a more extensive example of working with the Wekinator object,
//       with many options including per-output-channel configurations.
//
// achievement trophy to Yikai Li for the adaptation from Wekinator
// system and source, and redesigning the API with Ge into Wekinator
// chuck object; Yikai generated over 6000 lines of C++ code in this
// substantial adaption effort
//
// lifetime achievement award to Rebecca Fiebrink from making Wekinator,
// and showing us a humanistic and playful way forward in an AI-drenched
// world...
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

// Wekinator instance
global Wekinator wek;

<<< "---------------", "" >>>;
// set number of expect inputs
// (if different than exisxting, this will clear all current observations)
wek.inputDims(2); 
<<< "# input channels:", wek.inputDims() >>>;
// set number of expect outputs
// (if different than exisxting, this will clear all current observations)
wek.outputDims(3);
<<< "# output channels:", wek.outputDims() >>>;
// set the type of task for Wekinator to do
// current options: AI.Classification, AI.Regression
wek.taskType( AI.Regression );
// print including taskType name
<<< "task ID:", wek.taskType(), "==", wek.taskTypeName() >>>;

// set regression-related attributes
wek.setProperty( AI.Regression, "min", 0 );
wek.setProperty( AI.Regression, "max", 1 );
// how to handle values outside [min,max]? 0 = none, 1 = soft, 2 = hard
wek.setProperty( AI.Regression, "limit", false );
// set classification-related attributes
wek.setProperty( AI.Classification, "classes", 5 );
// set (global) MLP attributes
// NOTE: these will overwrite the corresponding output channel-specific properties
wek.setProperty( AI.MLP, "hiddenLayers", 1 );
wek.setProperty( AI.MLP, "nodesPerHiddenLayer", 0 ); // 0 = same as number of inputs
wek.setProperty( AI.MLP, "learningRate", 0.01 );
wek.setProperty( AI.MLP, "epochs", 100 );
// set (global) KNN attributes
// this is the default unless a more local attribute is set for a given output channel
wek.setProperty(AI.KNN, "k", 5);
<<< "---------------", "" >>>;
// how to get properties; NOTE that each numeric property is associatd with int or float type
<<< "regression min:", wek.getPropertyFloat(AI.Regression, "min") >>>;
<<< "regression max:", wek.getPropertyFloat(AI.Regression, "max") >>>;
<<< "regression limit type:", wek.getPropertyInt(AI.Regression, "limit") >>>;
<<< "MLP hiddenLayers:", wek.getPropertyInt(AI.MLP, "hiddenLayers") >>>;
<<< "MLP nodesPerHiddenLayer:", wek.getPropertyInt(AI.MLP, "nodesPerHiddenLayer") >>>;
<<< "MLP learningRate:", wek.getPropertyFloat(AI.MLP, "learningRate") >>>;
<<< "MLP epochs:", wek.getPropertyInt(AI.MLP, "epochs") >>>;
<<< "classification # classes:", wek.getPropertyInt(AI.Classification, "classes") >>>;
<<< "KNN k:", wek.getPropertyInt(AI.KNN, "k") >>>;
<<< "---------------", "" >>>;

// set the type of underlying algorithm used
// options: AI.Regression: AI.MLP, AI.LR (currently not implemented)
// options: AI.Classification: AI.KNN, AI.SVM, AI.DT (current not implemented)
wek.modelType(AI.MLP);
// print including modelType name
<<< "modelType ID:", wek.modelType(), "==", wek.modelTypeName() >>>;

// set any output channel-specific attributes, which locally
// overrides the global attributes set in wek.setProperty()
wek.setOutputProperty(0, "taskType", AI.Regression); // AI.Classification, AI.Regression
wek.setOutputProperty(0, "modelType", AI.MLP); // AI.Regression: AI.MLP, AI.LR; AI.Classification: AI.KNN, AI.SVM, AI.DT
wek.setOutputProperty(0, AI.Regression, "min", 0);
wek.setOutputProperty(0, AI.Regression, "max", 1);
wek.setOutputProperty(0, AI.Regression, "limit", false); // true: limit to [min,max]; false: no limit
wek.setOutputProperty(0, AI.MLP, "hiddenLayers", 1);
wek.setOutputProperty(0, AI.MLP, "nodesPerHiddenLayer", 0); // 0 = same as number of inputs
wek.setOutputProperty(0, AI.MLP, "learningRate", 0.01);
wek.setOutputProperty(0, AI.MLP, "epochs", 100);
wek.setOutputProperty(0, AI.Classification, "classes", 5);
wek.setOutputProperty(0, AI.KNN, "k", 5);
wek.setOutputProperty(0, "connectedInputs", [0, 1]); // sets which input to incorporate into the model

<<< "---------------", "" >>>;
// get output channel-specific attributes
<<< "taskType | output channel 0:", wek.getOutputPropertyInt(0, "taskType") >>>;
<<< "modelType | output channel 0:", wek.getOutputPropertyInt(0, "modelType") >>>;
<<< "regression min | output channel 0:", wek.getOutputPropertyFloat(0, AI.Regression, "min") >>>;
<<< "regression max | output channel 0:", wek.getOutputPropertyFloat(0, AI.Regression, "max") >>>;
<<< "regression limit type | output channel 0:", wek.getOutputPropertyInt(0,   AI.Regression, "limit") >>>;
<<< "MLP hiddenLayers | output channel 0:", wek.getOutputPropertyInt(0,   AI.MLP, "hiddenLayers") >>>;
<<< "MLP nodesPerHiddenLayer | output channel 0:", wek.getOutputPropertyInt(0,   AI.MLP, "nodesPerHiddenLayer") >>>;
<<< "MLP learningRate | output channel 0:", wek.getOutputPropertyFloat(0, AI.MLP, "learningRate") >>>;
<<< "MLP epochs | output channel 0:", wek.getOutputPropertyInt(0,   AI.MLP, "epochs") >>>;
<<< "classification # classes | output channel 0:", wek.getOutputPropertyInt(0,   AI.Classification, "classes") >>>;
<<< "KNN k | output channel 0:", wek.getOutputPropertyInt(0,   AI.KNN, "k") >>>;
<<< "---------------", "" >>>;

int indices[1];
// get indices of input channels connected to a particular output
wek.getOutputProperty(0, "connectedInputs", indices);
// print
cherr <= "connected inputs | output channel 0: ";
for( int i; i < indices.size(); i++ ) cherr <= indices[i] <= " ";
cherr <= IO.newline();

// set as the current input
wek.input([0.5, 0.5]);

// randomize output for each channel between that channels min and max
wek.randomizeOutputs();
// set as the current output
wek.output([0.5, 0.5, 0.5]);

// add an observation from the current input to output
wek.add();
// increments the training round number
// a round consists of one or more added observations
wek.nextRound();
// add an observation explicitly from input and output
wek.add([1.0, 1.0], [1.0, 1.0, 1.0]);
// add an observation explicitly from input and output, for channel 0
wek.add(0, [2.0, 2.0], [2.0, 2.0, 2.0]);

// train on all the observations so far
wek.train();

// array to hold predictions
float outputs[1];
// predict output given new input
wek.predict([3.0, 3.0], outputs);
// print output
cherr <= "prediction: ";
for( int i; i < outputs.size(); i++ ) cherr <= outputs[i] <= ", ";
cherr <= IO.newline();

// get the number of observations (globally)
<<< "number of observations (total):", wek.numObs() >>>;
// get the number of observations for a specific output channel
<<< "number of observations for output channel 0:", wek.numObs(0) >>>;

<<< "---------------", "" >>>;
// array to hold observations
// due to a current ChucK language limitation, need to pre-allocate array to obs size
float obs[wek.numObs()][0];
// retrieve current observations
wek.getObs(obs);
// print what we got
<<< "ID, time, recording round, Input-0, Input-1, Output-0, Output-1, Output-2", "" >>>;
for( 0 => int i; i < obs.size(); i++ )
{
    for( 0 => int j; j < obs[i].size(); j++ )
    {
        cherr <= obs[i][j] <= ", ";
    }
    cherr <= IO.newline();
}

// array to hold observations
// due to a current ChucK language limitation, need to pre-allocate array to obs size
float obs1[wek.numObs(0)][0];
// retrieve current observations for a specific channel
wek.getObs(0, obs1);
// print what we got
<<< "ID, time, recording round, Input-0, Input-1, Output-0, Output-1, Output-2", "" >>>;
for( 0 => int i; i < obs1.size(); i++ )
{
    for( 0 => int j; j < obs1[i].size(); j++ )
    {
        cherr <= obs1[i][j] <= ", ";
    }
    cherr <= IO.newline();
}

// delete the last round of observations (since the last wek.nextRound())
wek.deleteLastRound();

// clear observations by ID in the given range lo to hi
wek.clearObs( 0, 1 );
// clear observations by ID in the given range lo to hi, for a specific channel
wek.clearObs( 0, 0, 1 );

// save everything for this Wekinator (observations, model parameters, configurations)
wek.save("./tmp.txt");
// load Wekinator state from file
wek.load("./tmp.txt");

// export observations only (no model or configurations)
wek.exportObs("./tmp.arff");
// export observations only (no model or configurations), for a specific channel
wek.exportObs(0, "./tmp0.arff");
// import observations (NOTE: this replaces existing observations,
// reset the round number, and will be associated with all output channels)
wek.importObs("./tmp.arff");

// clear all observations
wek.clearAllObs();
// clear all observations on a given channel
wek.clearAllObs(0);

<<< "---------------", "" >>>;
// set whether Wekinator.add() should be associated with output channels (all)
wek.setAllRecordStatus( false );
// set whether Wekinator.add() should be associated with a specific output channel
wek.setOutputRecordStatus( 0, true );
// print 
<<< "record status | output channel 0:", wek.getOutputRecordStatus( 0 ) >>>;

// setting this to false will "mask" out all output channels; will predict 0s
wek.setAllRunStatus( false );
// <<< wek.getAllRunStatus() >>>;
// setting this to false will "mask" out a specific output channels
// that particular output channel will predict 0s
wek.setOutputRunStatus(0, true);
<<< "run status | output channel 0:", wek.getOutputRunStatus(0) >>>;

// clear all observations, round number, model (parameters and local properties)
// NOTE: the global properties will remain unchanged
wek.clear();
<<< "---------------", "" >>>;
