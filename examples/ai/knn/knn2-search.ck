//------------------------------------------------------------------------------
// name: knn2-search.ck
// desc: basic k-nearest-neighbor (k-NN) search example; it simulates a
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

// create a k-NN object
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

// provide an unlabled test query
[0.5, 0.5] @=> float q[];

// search for nearest neighbor label
int neighbor_labels[0];
// corresponding indices
int neighbor_indices[0];

// search type #1: get labels
knn.search( q, 1 , neighbor_labels );
chout <= "query=(" + q[0] + ", " + q[1] + "); k=1:\n" + labelNames[neighbor_labels[0]] <= IO.newline();

// search type #2: get labels and indices
knn.search( q, 2 , neighbor_labels, neighbor_indices );
chout <= "\n" <= "query=(" + q[0] + ", " + q[1] + "); k=2:\n";
for (int i; i < 2; i++)
{
    chout <= labelNames[neighbor_labels[i]] + " [" + neighbor_indices[i] + "]" <= IO.newline();
}

// search for nearest neighbor label and feature
float neighbor_features[2][2];
knn.search( q, 2 , neighbor_labels, neighbor_indices, neighbor_features );
chout <= "\n" <= "query=(" + q[0] + ", " + q[1] + "); k=2:\n";
for (int i; i < 2; i++)
{
    chout <= labelNames[neighbor_labels[i]] + " [" + neighbor_indices[i] + "]" + 
             " (" + neighbor_features[i][0] + ", " + neighbor_features[i][1] + ")" <= IO.newline();
}


