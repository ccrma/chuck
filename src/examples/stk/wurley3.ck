// polyphonic wurley's
// (also see ../midi/polyfony.ck for more polyfony)

// the size of the array should be the max number of wurlies
// you want to play simultaneously, at any point.
Wurley wurlies[3];
// variable to remember the last one played
int which;

// patch
Gain g => dac;
.4 => g.gain;
// connect the wurlies
for( int i; i < wurlies.size(); i++ )
    wurlies[i] => g;

// our notes
[ 61, 63, 65, 66, 68 ] @=> int notes[];

// infinite time-loop
while( true )
{
    for( int i; i < notes.size(); i++ )
    {
        play( notes[i], Math.random2f( .3, .9 ) );
        300::ms => now;
    }
}

// basic play function (add more arguments as needed)
fun void play( float note, float velocity )
{
    // first figure which to play
    // round robin may work
    ( which + 1 ) % wurlies.size() => which;

    // start the note
    Std.mtof( note ) => wurlies[which].freq;
    velocity => wurlies[which].noteOn;
}
