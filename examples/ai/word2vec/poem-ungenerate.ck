//------------------------------------------------------------------------------
// name: poem-ungenerate.ck
// desc: they just keep coming; more sus poem word2vec examples
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// NOTE: because this example needs ConsoleInput, it needs to
//       run command line chuck
//       > chuck poem-ungenerate.ck
//
// "Degenerate"
// -- a stream of unconsciousness poem generator
//
//       this example prompts for a line of text
//       (e.g., roses are red) and a number of iterations,
//       and outputs a series of lines that use word2vec to
//       change one word to an adjacent word from the previous
//       iteration, while generating unsettling descending
//       shepard tones
//
// NOTE: need a pre-trained word vector model, e.g., from
//       https://chuck.stanford.edu/chai/data/glove/
//       glove-wiki-gigaword-50-tsne-2.txt (400000 words x 2 dimensions)
//
// author: Ge Wang
// date: Spring 2023
//------------------------------------------------------------------------------

// random seed (if set, sequence can be reproduce)
// Math.srandom( 515 );

// instantiate
Word2Vec model;
// pre-trained model to load
me.dir() + "glove-wiki-gigaword-50-tsne-2.txt" => string filepath;
// load pre-trained model (see URLs above for download)
if( !model.load( filepath ) )
{
    <<< "cannot load model:", filepath >>>;
    me.exit();
}

// conditions
3 => int LINES_PER_STANZA;
3 => int NUM_SECTIONS; // here each section is a fixed stanzas format

// number of nearest words to retrieve for each word
// higher this number the higher the variance per word
10 => int K_NEAREST;

// timing
100::ms => dur T_WORD; // duration per word
false => int shouldScaleTimeToWordLength; // longer words take longer?
T_WORD => dur T_LINE_PAUSE; // a little pause after each line
T_WORD * 2 => dur T_STANZA_PAUSE; // pause after each stanza

// make a ConsoleInput
ConsoleInput in;
// tokenizer
StringTokenizer tok;
// line
string line[0];

// ------------ audio: shepard tone generator ---------
// starting pitches (in MIDI note numbers, octaves apart)
[ 12.0, 24, 36, 48, 60, 72, 84, 96, 108 ] @=> float pitches[];
// number of tones
pitches.size() => int N;
// bank of tones
TriOsc tones[N];
// overall gain
Gain gain => Envelope envelope => NRev reverb => dac;
1.0/N => gain.gain;
.05 => reverb.mix;
// connect to dac
for( int i; i < N; i++ ) { tones[i] => gain; }
// set envelope
envelope.duration(1::second);

// increment per unit time (use negative for descending)
-.001 => float INC;

// keep shepard tone generating in the background;
// we will raise and lower volume
spork ~ shepard();
// ------------ audio: shepard tone generator ---------

// current word
string word;
// word vector
float vec[model.dim()];
// search results
string words[K_NEAREST];

// state
0 => int GET_TEXT;
1 => int GET_NUM;
GET_TEXT => int state;
// prompt
string prompt;
// the line 
string theText[0];

// new line
endl();
// print program name
chout <= "\"Degenerate\"" <= IO.newline();
chout <= "-- a stream of unconsciousness poem generator" <= IO.newline();
chout.flush();
// new line
endl();

// loop
while( true )
{
    if( state == GET_TEXT )
    {
        "enter a line (e.g., roses are red) => " => prompt;
    }
    else
    {
        "how many degenerations (e.g., 20) => " => prompt;
    }
    // prompt
    in.prompt( prompt ) => now;
    
    // read
    while( in.more() )
    {
        // clear tokens array
        line.clear();
        // clear if in text input mode
        if( state == GET_TEXT ) theText.clear();
        // get the input line as one string; send to tokenizer
        tok.set( in.getLine() );
        // tokenize line into words separated by white space
        while( tok.more() )
        {
            // put into array (lowercased)
            line << tok.next().lower();
            // copy into our text for later
            if( state == GET_TEXT ) theText << line[line.size()-1];
        }
        // if non-empty
        if( !line.size() )
            continue;
        
        // check state
        if( state == GET_TEXT )
        {
            // set state to GET_NUM
            GET_NUM => state;
        }
        else // GET_NUM
        {
            // check input
            Std.atoi( line[0] ) => int n;
            // make sure in range
            if( n <= 0 ) continue;
            // degenerate the input across n iterations
            degenerate( theText, n );
            // reset state to get text
            GET_TEXT => state;
        }
    }
}

// do something with the text input 
fun void degenerate( string input[], int iterations )
{
    // start envelope (bring up audio)
    envelope.keyOn();
    
    // endline
    endl();

    // each iteration updates one word from the previous
    for( int i; i < iterations; i++ )
    {
        for( int j; j < input.size(); j++ )
        {
            say(input[j]); wait();
        }
        endl();
        
        // random word in the line
        Math.random2(0,input.size()-1) => int which;
        // take that work
        input[which] => word;
        // find similar words
        model.getSimilar( word, words.size(), words );
        // randomly take one and replace the word in question
        words[Math.random2(0,words.size()-1)] => input[which];
    }
    
    // close envelope (ramp down audio)
    envelope.keyOff();

    // endline
    endl();
    // pause
    T_STANZA_PAUSE => now;
}

// say a word with space after
fun void say( string word )
{
    say( word, " " );
}

// say a word
fun void say( string word, string append )
{
    // print it
    chout <= word <= append; chout.flush();
}

// wait
fun void wait()
{
    wait( T_WORD );
}

// wait
fun void wait( dur T )
{    
    // let time pass, let sound...sound
    T => now;
}

// new line with timing
fun void endl()
{
    endl( T_WORD );
}

// new line with timing
fun void endl( dur T )
{
    // new line
    chout <= IO.newline(); chout.flush();
    // let time pass
    T => now;
}

// shepard tone generator
fun void shepard()
{
//--------------------------------------------------------------------
// continuous shepard-risset tone generator
// adapted from https://chuck.stanford.edu/doc/examples/deep/shepard.ck 
//--------------------------------------------------------------------    

    // mean for normal intensity curve
    66 => float MU;
    // standard deviation for normal intensity curve
    42 => float SIGMA;
    // normalize to 1.0 at x==MU
    1 / Math.gauss(MU, MU, SIGMA) => float SCALE;
    // unit time (change interval)
    1::ms => dur T;
    
    // infinite time loop
    while( true )
    {
        for( int i; i < N; i++ )
        {
            // set frequency from pitch
            pitches[i] => Std.mtof => tones[i].freq;
            // compute loundess for each tone
            Math.gauss( pitches[i], MU, SIGMA ) * SCALE => float intensity;
            // map intensity to amplitude
            intensity*96 => Math.dbtorms => tones[i].gain;
            // increment pitch
            INC +=> pitches[i];
            // wrap (for positive INC)
            if( pitches[i] > 120 ) 108 -=> pitches[i];
            // wrap (for negative INC)
            else if( pitches[i] < 12 ) 108 +=> pitches[i];
        }
        
        // advance time
        T => now;
    }
}