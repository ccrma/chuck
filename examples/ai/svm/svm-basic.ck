//------------------------------------------------------------------------------
// name: svm-basic.ck
// desc: basic support vector machine example (SVM)
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// uncomment the next line to learn more about the KNN object:
// HMM.help();
//
// author: Yikai Li
// date: Spring 2023
//------------------------------------------------------------------------------

// instance
SVM svm;

// prepare data
[[1.0, 1.0],
 [2.0, 2.0],
 [3.0, 3.0]] @=> float x[][];
[[1.0],
 [2.0],
 [3.0]] @=> float y[][];

// train
// <<< "training...", "" >>>;
svm.train(x, y);

// test
float res[1];
// <<< "predicting...", "" >>>;
svm.predict([1.5, 1.5], res);
<<< res[0] >>>;