//---------------------------------------------------------------------
// name: mlp-manual.ck
// desc: (manually) training a basic artificial neural network;
//       for a less manual approach, see mlp.ck
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

// units per layer: input, [hidden layer(s)], output 
[3, 5, 2] @=> int unitsPerLayer[];
// initialize with the units per layer
mlp.init( unitsPerLayer );

// input observations
[[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]] @=> float X[][];
// output observations
[[0.1, 0.2], [0.3, 0.4]] @=> float Y[][];
// learning rate
0.001 => float learningRate;
// number of epochs
100 => int epochs;

// array references
float x[], y[];


//---------------------------------------------------------------------
// train epoch by epoch
//---------------------------------------------------------------------
for( int i; i < epochs; i++ )
{
    // shuffle the observations
    MLP.shuffle( X, Y );

    // train over all observations
    for( int j; j < X.size(); j++ )
    {
        // print weights and biases
        chout <= IO.newline() <= "--- WEIGHTS AND BIASES ---" <= IO.newline();
        visWeightsBiases();

        // forward propagation
        chout <= IO.newline() <= "--- FORWARD ---" <= IO.newline();
        mlp.forward(X[j]);
        visActivations();

        // backpropagation
        chout <= IO.newline() <= "--- BACKPROP ---" <= IO.newline();
        mlp.backprop(Y[j], learningRate);
        visGradients();
    }
    
    // print
    chout <= IO.newline() <= "--- DONE : EPOCH: " <= i+1 <= " ---" <= IO.newline();
    // advance time for print
    100::ms => now;
}

// print weights and biases
chout <= IO.newline() <= "--- WEIGHTS AND BIASES ---" <= IO.newline();
visWeightsBiases();
chout <= IO.newline();


//---------------------------------------------------------------------
// visualize activations
//---------------------------------------------------------------------
fun void visActivations()
{
    // chout <= "[activations]" <= IO.newline();
    for ( 0 => int k; k < unitsPerLayer.size(); k++ )
    {
        // activations
        chout <= "layer " <= k <= " ACTIVATIONS" <= IO.newline();
        float a[unitsPerLayer[k]];
        mlp.getActivations(k, a);
        for ( 0 => int l; l < unitsPerLayer[k]; l++ )
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
    for ( 0 => int k; k < unitsPerLayer.size() - 1; k++ )
    {
        // weights
        chout <= "layer " <= k <= " WEIGHTS" <= IO.newline();
        float w[unitsPerLayer[k]][unitsPerLayer[k+1]];
        mlp.getWeights(k, w);
        for ( 0 => int l; l < unitsPerLayer[k]; l++ )
        {
            for ( 0 => int m; m < unitsPerLayer[k+1]; m++ )
                chout <= w[l][m] <= " ";
            chout <= IO.newline();
        }
    }
    
    chout <= "---" <= IO.newline();
    // chout <= "[biases]" <= IO.newline();
    for ( 0 => int k; k < unitsPerLayer.size() - 1; k++ )
    {
        // biases
        chout <= "layer " <= k <= " BIASES" <= IO.newline();
        float b[unitsPerLayer[k+1]];
        mlp.getBiases(k, b);
        for ( 0 => int l; l < unitsPerLayer[k+1]; l++ )
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
    for ( 0 => int k; k < unitsPerLayer.size(); k++ )
    {
        // gradients
        chout <= "layer " <= k <= " GRADIENTS" <= IO.newline();
        float g[unitsPerLayer[k]];
        mlp.getGradients(k, g);
        for ( 0 => int l; l < unitsPerLayer[k]; l++ )
            chout <= g[l] <= " ";
        chout <= IO.newline();
    }
}
