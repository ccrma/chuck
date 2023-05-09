//------------------------------------------------------------------------------
// name: poem-i-feel.ck
// desc: a sus poem made with chAI and word2vec
//
// version: need chuck version 1.5.0.0 or higher
// sorting: part of ChAI (ChucK for AI)
//
// "i feel"
// -- a stream of unconsciousness poem generator
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

// starter "i feel" emotion words with positive connotations
["fine", "joy", "excited", "inspired", "included",
"understood", "artful", "playful", "free", "myself",
"grateful", "optimistic" ] @=> string positives[];

// starter "i feel" emotion words with negative connotations
["sad", "bored", "meh", "spent", "worried",
"afraid", "marginalized", "misunderstood", "powerless",
"undone", "small", "guilt", "cringe", "dread" ] @=> string negatives[];

// starter "i feel" emotion words with other connotations
["numb", "nothing", "weird", "meh", "disjointed", "scattered",
"complicated", "sus" ] @=> string others[];

// conditions
3 => int LINES_PER_STANZA;
3 => int NUM_SECTIONS; // here each section is a fixed stanzas format

// number of nearest words to retrieve for each word
// higher this number the higher the variance per word
10 => int K_NEAREST;

// timing
400::ms => dur T_WORD; // duration per word
false => int shouldScaleTimeToWordLength; // longer words take longer?
T_WORD => dur T_LINE_PAUSE; // a little pause after each line
T_WORD * 2 => dur T_STANZA_PAUSE; // pause after each stanza

// sound
ModalBar crystal => Gain left => dac.left;
crystal => Gain right => dac.right;
1 => crystal.preset;
// feedback delay for echos
left => DelayL DL => left; .5 => DL.gain; T_WORD*1.5 => DL.max => DL.delay;
right => DelayL DR => right; .3 => DR.gain; T_WORD*2 => DR.max => DR.delay;

// current word
string feeling;
// word vector
float vec[model.dim()];
// search results
string words[K_NEAREST];

// line break
chout <= IO.newline(); chout.flush();

// loop over stanzas
for( int s; s < NUM_SECTIONS; s++ )
{
    // grab a word out of the "good" bag
    positives[Math.random2(0,positives.size()-1)] => feeling;
    // print a stanza
    stanza( feeling, LINES_PER_STANZA, 48, 64 );
    
    // grab a word out of the "bad" bag
    negatives[Math.random2(0,negatives.size()-1)] => feeling;
    // print a stanza
    stanza( feeling, LINES_PER_STANZA, 48, 63 );
    
    // grab a word out of the "other" bag
    others[Math.random2(0,others.size()-1)] => feeling;
    // print a stanza
    stanza( feeling, LINES_PER_STANZA, 36, Math.random2(33,36)*2 );
    
    endStanza();
    chout <= IO.newline() <= IO.newline(); chout.flush();
    // pause at end of line
    T_STANZA_PAUSE => now;
}

// print at the end
chout <= "\"i feel\"" <= IO.newline();
chout <= "-- a stream of unconsciousness poem" <= IO.newline();
chout.flush();

// print a stanza from a starter feeling word
fun void stanza( string feeling, int numLines, int pitch1, int pitch2 )
{
    // loop over lines in a stanza
    for( int n; n < numLines; n++ )
    {
        // a line
        say("i"); play(pitch1); wait();
        say("feel"); wait();
        say(feeling); play(pitch2); wait();
        endl(); wait();
        
        // get similar words
        model.getSimilar( feeling, words.size(), words );
        // choose one at random
        words[Math.random2(0,words.size()-1)] => feeling;
    }
    // next line!
    chout <= IO.newline(); chout.flush();
    // pause at end of line
    T_LINE_PAUSE => now;
}

fun void endStanza()
{
    // stanza break; no new sound for now
    say("one"); wait();
    say("day"); wait();
    say("ends"); wait();
    say("as"); wait();
    say("another"); wait();
    say("day"); wait();
    say("dawns..."); wait();
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

// sonify
fun void play( int pitch )
{
    play( pitch, Math.random2f(.8,1) );
}

// sonify
fun void play( int pitch, float velocity )
{
    // convert pitch to frequency and set it
    pitch => Std.mtof => crystal.freq;
    // note on
    velocity => crystal.noteOn;
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
