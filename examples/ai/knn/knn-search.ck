//------------------------------------------------------------------------------
// name: knn2-search.ck
// desc: basic k-nearest-neighbor (k-NN) search example;
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment the next line to learn more about the KNN object:
// KNN.help();
//
// author: Yikai Li and Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// create a KNN object
KNN knn;

// prepare data: using 2-dimensional feature space for this example;
// in general, the feature space could be much higher
[[0.0, 0.0],
 [1.0, 1.0],
 [2.0, 2.0]] @=> float features[][];
// train the model
knn.train( features );
// optional: weigh feature dimensions
[1.0, 1.0] @=> float weights[];
// optional: set the weights across dimensions
knn.weigh( weights );

// provide a test query
[0.5, 0.5] @=> float q[];

// corresponding indices
int neighbor_indices[0];

// search
knn.search( q, 2 , neighbor_indices );
chout <= "query = ("+ q[0]+","+q[1]+"); k=2: ["+ neighbor_indices[0]+","+neighbor_indices[1]+"]" <= IO.newline();
