//------------------------------------------------------------------------------
// name: poem-spew.ck
// desc: yet another sus poem made with chAI and word2vec
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// "Spew"
// -- yet another stream of unconsciousness poem generator
//
// NOTE: need a pre-trained word vector model, e.g., from
//       https://chuck.stanford.edu/chai/data/glove/
//       glove-wiki-gigaword-50-tsne-2.txt (400000 words x 2 dimensions)
//
// author: Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// default starting word
"love" => string STARTING_WORD;
// check arguments, set new starting word if present
if( me.args() > 0 ) me.arg(0) => STARTING_WORD;

// random seed (if set, sequence can be reproduce)
// Math.srandom( 515 );

// instantiate
Word2Vec model;
// pre-trained model to load
me.dir() + "glove-wiki-gigaword-50-tsne-2.txt" => string filepath;
// load a pre-trained model (see URLs above for download)
// this could take a few seconds, depending on model size
if( !model.load( filepath ) )
{
    <<< "cannot load model:", filepath >>>;
    me.exit();
}

// conditions
2500 => int TOTAL_WORDS; // to spew
1000 => int K_NEAREST; // number of nearest words to retrieve for each word

// timing
40::ms => dur T_WORD; // duration per word
40::ms => dur T_LINE_PAUSE; // a little pause after each line

// ranges for each dimension (for sound mapping)
float mins[0], maxs[0];
// get bounds for each dimension
model.minMax( mins, maxs );

// sound
ModalBar bar => NRev reverb => dac;
// reverb wet/dry mix
.1 => reverb.mix;
// which preset
7 => bar.preset;

// current word
STARTING_WORD => string word;
// word vector
float vec[model.dim()];
// similarity search results from word2vec
string words[K_NEAREST];
// total number of words spewed
0 => int spew;
// words to go on current line
0 => int lineWords;

// go
while( spew < TOTAL_WORDS )
{
    if( lineWords <= 0 )
    {
        // line break
        chout <= IO.newline(); chout.flush();
        // generate how many words in next line
        Math.random2( 5, 12 ) => lineWords;
        // a little pause after each line
        25::ms => dur T_LINE_PAUSE;
    }

    // print the word, with a space
    chout <= word <= " "; chout.flush();
    // get word vector
    model.getVector( word, vec );
    // sonify from word vector (see mapping possibilities of ModalBar)
    // https://chuck.stanford.edu/doc/program/ugen_full.html#ModalBar
    // feel free to experiement with what parameters to control with the vector
    Math.remap( vec[0], mins[0], maxs[0], 24, 96 ) => Std.mtof => bar.freq;
    // use pow to expand/compress the dimensions to affect mapping sensitivity
    Math.pow(Math.remap( vec[1], mins[1], maxs[1], 0, 1), .72) => bar.stickHardness;
    // ding!
    Math.random2f(.5,1) => bar.noteOn;
    // let time pass, let sound...sound
    T_WORD => now;
    // get similar words
    model.getSimilar( word, words.size(), words );
    // choose one of the farthest one
    words[Math.random2((words.size()-1*.8)$int,words.size()-1)] => word;
    
    // count
    spew++;
    // count line
    lineWords--;
}

// print at the end
chout <= "\"Spew\"" <= IO.newline();
chout <= "-- another stream of unconsciousness poem" <= IO.newline();
chout.flush();
