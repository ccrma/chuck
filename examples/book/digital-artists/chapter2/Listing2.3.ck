//  Listing 2.3 Random music using the Math Library

// Some random square wave music!
SqrOsc s => dac;                       // Makes a SinOsc to play your random notes.
for (0 => int i; i < 16; i++)          // (1) for loop plays 16 notes.
{
    Math.random2(48,72) => int myNote; // (2) Random integer note number (C3-C5).
    Math.random2f(0.05,0.9) => float myGain; // (3) Random gain from .05 to .9.
    <<< myNote, myGain >>>;            // (4) Prints current note and gain.
    Std.mtof(myNote) => s.freq;        // (5) Sets oscillator frequency and gain.
    myGain => s.gain;
    0.2 :: second => now;              // (6) Lets each note sound for 1/5 second.
}

