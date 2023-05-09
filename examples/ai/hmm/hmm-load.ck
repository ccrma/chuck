//------------------------------------------------------------------------------
// name: hmm-load.ck
// desc: basic hidden markov model (HMM) usage example (using load)
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

// load
[0.6, 0.2, 0.2] @=> float initial[];
[[0.8, 0.1, 0.1],
 [0.2, 0.7, 0.1],
 [0.1, 0.3, 0.6]] @=> float transition[][];
[[0.7, 0.0, 0.3],
 [0.1, 0.9, 0.0],
 [0.0, 0.2, 0.8]] @=> float emission[][];
hmm.load( initial, transition, emission );

// generate
int observations[30];
hmm.generate( 30, observations );

// output
for ( 0 => int i; i < observations.size(); i++ )
{
    chout <= observations[i] <= " ";
}
chout <= IO.newline();
