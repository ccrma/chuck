//------------------------------------------------------------------------------
// name: poem-randomwalk.ck
// desc: another sus poem made with chAI and word2vec
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// "Random Walk"
// -- another stream of unconsciousness poem generator
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
7 => int WORDS_PER_LINE;
4 => int LINES_PER_STANZA;
3 => int NUM_STANZAS;

// number of nearest words to retrieve for each word
// higher this number the higher the variance per word
100 => int K_NEAREST;

// timing
200::ms => dur T_WORD; // duration per word
false => int shouldScaleTimeToWordLength; // longer words take longer?
250::ms => dur T_LINE_PAUSE; // a little pause after each line
750::ms => dur T_STANZA_PAUSE; // pause after each stanza

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
// used for scaling up word timing to word length
dur addTime;

// line break
chout <= IO.newline(); chout.flush();

// loop over stanzas
for( int s; s < NUM_STANZAS; s++ )
{
    // loop over lines in a stanza
    for( int n; n < LINES_PER_STANZA; n++ )
    {
        // loop over words
        for( int w; w < WORDS_PER_LINE; w++ )
        {
            // print the word, with a space
            chout <= word <= " "; chout.flush();
            // get word vector (for sonification)
            model.getVector( word, vec );
            // sonify from word vector (see mapping possibilities of ModalBar)
            // https://chuck.stanford.edu/doc/program/ugen_full.html#ModalBar
            // feel free to experiement with what parameters to control with the vector
            Math.remap( vec[0], mins[0], maxs[0], 24, 84 ) => Std.mtof => bar.freq;
            // use pow to expand/compress the dimensions to affect mapping sensitivity
            Math.pow(Math.remap( vec[1], mins[1], maxs[1],0,1), 1.1) => bar.stickHardness;
            // ding!
            Math.random2f(.5,1) => bar.noteOn;
            // if scale time with word length
            if( shouldScaleTimeToWordLength )
                word.length() * T_WORD/20 => addTime;
            // let time pass, let sound...sound
            T_WORD + addTime => now;
            // get similar words
            model.getSimilar( word, words.size(), words );
            // choose one at random
            words[Math.random2(0,words.size()-1)] => word;
        }
        // next line!
        chout <= IO.newline(); chout.flush();
        // pause at end of line
        T_LINE_PAUSE => now;
    }
    
    // stanza break!
    chout <= IO.newline(); chout.flush();
    // pause at end of line
    T_STANZA_PAUSE => now;
}

// print at the end
chout <= "\"Random Walk\"" <= IO.newline();
chout <= "-- a stream of unconsciousness poem" <= IO.newline();
chout.flush();
