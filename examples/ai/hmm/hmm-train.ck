//------------------------------------------------------------------------------
// name: hmm-train.ck
// desc: basic hidden markov model (HMM) usage example (using train)
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

// init
HMM hmm;

// train
[0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2] @=> int observations[];
hmm.train( 2, 3, observations );

// generate
int results[30];
hmm.generate( 30, results );

// output
for ( 0 => int i; i < results.size(); i++ )
{
    chout <= results[i] <= " ";
}
chout <= IO.newline();