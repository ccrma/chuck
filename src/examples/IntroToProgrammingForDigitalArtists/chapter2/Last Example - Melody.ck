// sound chain
SqrOsc s => Pan2 p =>  dac;

// array declaration 
[54,56,62,54,48,50,52] @=> int A[];
// timing durations (same length as A[])
[.5,.2,.4,.6,.5,.3,.4] @=> float notes[];

// loop
for( 0 => int i; i < A.cap(); i++ )
{
    <<< i, A[i], notes[i] >>>; // print index, value, durations
    Std.mtof(A[i]) => s.freq; // MIDI to freq
    Std.rand2f(.2,.6) => s.gain; // random value for gain
    Math.cos(now/1::second*2*pi) => p.pan; // panning with math
    notes[i]::second => now; // advance time
}



