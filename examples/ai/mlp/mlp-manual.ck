//---------------------------------------------------------------------
// name: mlp-manual.ck
// desc: training (step-by-step) a multilayer perception (MLP):
//       (for a less manaul example, see mlp.ck)
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
// delay for each epoch (change for print speed)
// (or run with --silent for no print delay)
100::ms => dur T_PRINT;
// learning rate
0.01 => float learningRate;
// number of epochs
100 => int epochs;

// instantiate a multilayer perception (a basic neural network)
MLP mlp;

// neurons per layer: input, [hidden layer(s)], output
[3, 5, 5, 2] @=> int nodesPerLayer[];
// initialize with the # of neurons per layer
mlp.init( nodesPerLayer );

// input observations
[[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]] @=> float X[][];
// output observations
[[0.1, 0.2], [0.3, 0.4]] @=> float Y[][];
// input and output array references
float x[], y[];


//---------------------------------------------------------------------
// train the neural network epoch by epoch
// this is a more "manual" alternative to simply calling MLP.train()
// intended for education and curiosity
//---------------------------------------------------------------------
for( int i; i < epochs; i++ )
{
    // shuffle the observations
    MLP.shuffle( X, Y );

    // train over all observations
    for( int j; j < X.size(); j++ )
    {
        // print weights and biases
        chout <= IO.newline() <= "--- WEIGHTS AND BIASES |"
              <= " EPOCH:" <= i+1 <= " OBSERVATION:" <= j+1 <= " ---" <= IO.newline();
        visWeightsBiases();

        // forward propagation
        chout <= IO.newline() <= "--- FORWARD-PROPAGATION |"
              <= " EPOCH:" <= i+1 <= " OBSERVATION:" <= j+1 <= " ---" <= IO.newline();
        mlp.forward( X[j] );
        visActivations();

        // backpropagation
        chout <= IO.newline() <= "--- BACKPROPAGATION |"
              <= " EPOCH:" <= i+1 <= " OBSERVATION:" <= j+1 <= " ---" <= IO.newline();
        mlp.backprop( Y[j], learningRate );
        visGradients();
    }
    
    // print
    chout <= IO.newline();
    chout <= "********************************************" <= IO.newline();
    chout <= "*** LEARNING-RATE:" <= learningRate <= " | EPOCH:" <= i+1 <= " DONE" <= IO.newline();
    chout <= "********************************************" <= IO.newline();

    // pause for print
    T_PRINT => now;
}

// print weights and biases
chout <= IO.newline() <= "--- FINAL WEIGHTS AND BIASES |"
      <= " EPOCH:" <= epochs <= " ---" <= IO.newline();
visWeightsBiases();
chout <= IO.newline();

// print
chout <= "********************************************" <= IO.newline();
chout <= "************* TRAINING DONE ****************" <= IO.newline();
chout <= "********************************************" <= IO.newline();
chout <= IO.newline();


//---------------------------------------------------------------------
// visualize activations
//---------------------------------------------------------------------
fun void visActivations()
{
    // chout <= "[activations]" <= IO.newline();
    for ( 0 => int k; k < nodesPerLayer.size(); k++ )
    {
        // activations
        chout <= "layer " <= k <= " ACTIVATIONS" <= IO.newline();
        float a[nodesPerLayer[k]];
        mlp.getActivations(k, a);
        for ( 0 => int l; l < nodesPerLayer[k]; l++ )
            chout <= a[l] <= " ";
        chout <= IO.newline();
    }
}


//---------------------------------------------------------------------
// visualize weights and biases
//---------------------------------------------------------------------
fun void visWeightsBiases()
{
    // chout <= "[weights]" <= IO.newline();
    for ( 0 => int k; k < nodesPerLayer.size() - 1; k++ )
    {
        // weights
        chout <= "layer " <= k <= " WEIGHTS" <= IO.newline();
        float w[nodesPerLayer[k]][nodesPerLayer[k+1]];
        mlp.getWeights(k, w);
        for ( 0 => int l; l < nodesPerLayer[k]; l++ )
        {
            for ( 0 => int m; m < nodesPerLayer[k+1]; m++ )
                chout <= w[l][m] <= " ";
            chout <= IO.newline();
        }
    }
    
    chout <= "---" <= IO.newline();
    // chout <= "[biases]" <= IO.newline();
    for ( 0 => int k; k < nodesPerLayer.size() - 1; k++ )
    {
        // biases
        chout <= "layer " <= k <= " BIASES" <= IO.newline();
        float b[nodesPerLayer[k+1]];
        mlp.getBiases(k, b);
        for ( 0 => int l; l < nodesPerLayer[k+1]; l++ )
            chout <= b[l] <= " ";
        chout <= IO.newline();
    }
}


//---------------------------------------------------------------------
// visualize gradients
//---------------------------------------------------------------------
fun void visGradients()
{
    // chout <= "[gradients]" <= IO.newline();
    for ( 0 => int k; k < nodesPerLayer.size(); k++ )
    {
        // gradients
        chout <= "layer " <= k <= " GRADIENTS" <= IO.newline();
        float g[nodesPerLayer[k]];
        mlp.getGradients(k, g);
        for ( 0 => int l; l < nodesPerLayer[k]; l++ )
            chout <= g[l] <= " ";
        chout <= IO.newline();
    }
}
