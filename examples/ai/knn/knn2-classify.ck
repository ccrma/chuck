//------------------------------------------------------------------------------
// name: knn2-classify.ck
// desc: k-nearest-neighbor (k-NN) classification example; it simulates a 
//       simplified musical genre classification task where three labeled
//       categories are given respective locations in a 2D feature space.
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment the next line to learn more about the KNN2 object:
// KNN2.help();
//
// author: Yikai Li and Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// create a k-NN classifier
KNN2 knn;

// prepare data: labels
["classical", "pop", "rock"] @=> string labelNames[];
// prepare data: using 2-dimensional feature space for this example;
// in general, the feature space could be much higher
[[0.0, 0.0],
 [1.0, 1.0],
 [2.0, 2.0]] @=> float features[][];
// prepare data: corresponding IDs between labels and features
[0, 1, 2] @=> int labels[];
// number of labels
labels.size() => int NUM_LABELS;
// train the model
knn.train( features, labels );
// optional: weigh the feature dimensions
[1.0, 1.0] @=> float weights[];
// optional: set the weights across feature dimensions
knn.weigh( weights );

// provide an unlabeled test query
[0.5, 0.5] @=> float q[];

// predict with new query and k=1
predict( q, 1 );
// predict with the same query and k=2
predict( q, 2 );

// based on a new unlabeled test query and its distance to its
// k nearest neighbors, predict and print the probabilities of
// class membership NOTE: for the sake of the example, this
// function assumes query has 2 dimensions
fun void predict( float query[], int k )
{
    // how we get the data back
    float prob[NUM_LABELS];

    // predict with k nearest neighbors
    knn.predict( query, k, prob );
    // print
    chout <= "query=(" + query[0] + "," + query[1] + "); k=" + k + ":" <= IO.newline();
    for( 0 => int i; i < 3; i++ )
    {
        chout <= labelNames[i] + ": " + prob[i] <= IO.newline();
    }
    chout <= IO.newline();
}
