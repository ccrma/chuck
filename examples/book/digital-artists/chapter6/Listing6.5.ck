// Listing 6.5 Simple violin using SawOsc, Envelope, and SinOsc for vibrato

// Simple SawOsc-based violin with ADSR envelope and vibrato
SinOsc vibrato => SawOsc viol => ADSR env => dac;

// (1) Tells the oscillator to interpret input as frequency modulation
2 => viol.sync;   

6.0 => vibrato.freq;  // (2) set vibrato frequency to 6 Hz

// (3) set all A D S R parameters at once
env.set(0.1 :: second, 0.1 :: second, 0.5, 0.1 :: second);

// define a D Major Scale (in MIDI note numbers)
[62, 64, 66, 67, 69, 71, 73, 74] @=> int scale[];  // (4) Makes a scale note array

// run through our scale one note at a time
for (0 => int i; i < scale.cap(); i++)  // (5) Plays through whole scale using for loop
{
    // set frequency according to note number
    Std.mtof(scale[i]) => viol.freq;        // (6) Sets frequency for each note

    // trigger note and wait a bit
    1 => env.keyOn;
    0.3 :: second => now;

    // turn off note and wait a bit
    1 => env.keyOff; 
    0.1 :: second => now;
}

// repeat last note with lots of vibrato
1 => env.keyOn;
10.0 => vibrato.gain;
1.0 :: second => now;

0 => env.keyOff;
0.2 :: second => now;
