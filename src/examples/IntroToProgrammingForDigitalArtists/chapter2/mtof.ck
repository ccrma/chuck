// sound chain
TriOsc s => dac;

// loop 
for( 0 => int i; i <= 127; i++ )
{
    Std.mtof(i) => float Hz; // MIDI to hertz
    <<< i, Hz >>>; // print out i and Hz 
    Hz => s.freq; // update frequency
    200::ms => now; // advance time
}

