// Listing 2.1 Playing a chromatic scale using Std.mtof()

// sound chain
TriOsc t => dac;
0.4 => t.gain;

// loop
for (0 => int i; i < 127; i++)
{
         // (1) Use Std.mtof to convert note number to frequency
    Std.mtof(i) => float Hz; // MIDI to Hertz frequency
    <<< i, Hz >>>; // print out result
    Hz => t.freq; // update frequency       // (2) Set oscillator frequency to Hz
    200::ms => now; // advance time
}
