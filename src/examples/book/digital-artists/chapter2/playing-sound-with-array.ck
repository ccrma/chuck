// sound chain
SqrOsc s =>  dac;

// array declaration 
[54,56,62,54,48,50,52] @=> int A[];

// loop
for( 0 => int i; i < A.cap(); i++ )
{
    <<< i, A[i] >>>; // print index, value, durations
    Std.mtof(A[i]) => s.freq; // MIDI to freq
    .5::second => now; // advance time
}

