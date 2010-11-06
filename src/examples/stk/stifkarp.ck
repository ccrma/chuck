// STK StifKarp

// patch
StifKarp m => JCRev r => dac;
.75 => r.gain;
.05 => r.mix;

// our notes
[ 61, 63, 65, 66, 68, 66, 65, 63 ] @=> int notes[];

// infinite time-loop
while( true )
{
    Std.rand2f( 0, 1 ) => m.pickupPosition;
    Std.rand2f( 0, 1 ) => m.sustain;
    Std.rand2f( 0, 1 ) => m.stretch;

    <<< "---", "" >>>;
    <<< "pickup:", m.pickupPosition() >>>;
    <<< "sustain:", m.sustain() >>>;
    <<< "stretch:", m.stretch() >>>;

    // factor
    Std.rand2f( 1, 4 ) => float factor;

    for( int i; i < notes.cap(); i++ )
    {
        play( Std.rand2(0,2)*12 + notes[i], Std.rand2f( .6, .9 ) );
        100::ms * factor => now;
    }
}

// basic play function (add more arguments as needed)
fun void play( float note, float velocity )
{
    // start the note
    Std.mtof( note ) => m.freq;
    velocity => m.pluck;
}
