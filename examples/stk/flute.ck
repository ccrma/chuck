// STK Flute

// patch
Flute flute => PoleZero f => JCRev r => dac;
.75 => r.gain;
.05 => r.mix;
.99 => f.blockZero;

// our notes
[ 61, 63, 65, 66, 68 ] @=> int notes[];

// infinite time-loop
while( true )
{
    // clear
    flute.clear( 1.0 );

    // set
    Math.random2f( 0, 1 ) => flute.jetDelay;
    Math.random2f( 0, 1 ) => flute.jetReflection;
    Math.random2f( 0, 1 ) => flute.endReflection;
    Math.random2f( 0, 1 ) => flute.noiseGain;
    Math.random2f( 0, 12 ) => flute.vibratoFreq;
    Math.random2f( 0, 1 ) => flute.vibratoGain;
    Math.random2f( 0, 1 ) => flute.pressure;

    // print
    <<< "---", "" >>>;
    <<< "jetDelay:", flute.jetDelay() >>>;
    <<< "jetReflection:", flute.jetReflection() >>>;
    <<< "endReflection:", flute.endReflection() >>>;
    <<< "noiseGain:", flute.noiseGain() >>>;
    <<< "vibratoFreq:", flute.vibratoFreq() >>>;
    <<< "vibratoGain:", flute.vibratoGain() >>>;
    <<< "breath pressure:", flute.pressure() >>>;

    // factor
    Math.random2f( .75, 2 ) => float factor;

    for( int i; i < notes.size(); i++ )
    {
        play( 12 + notes[i], Math.random2f( .6, .9 ) );
        300::ms * factor => now;
    }
}

// basic play function (add more arguments as needed)
fun void play( float note, float velocity )
{
    // start the note
    Std.mtof( note ) => flute.freq;
    velocity => flute.noteOn;
}
