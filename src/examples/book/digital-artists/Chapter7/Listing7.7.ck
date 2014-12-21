// Listing 7.7 Playing a scale on the STK Bowed string model

// Bowed string demo
Bowed viol => dac;                   // (1) Bowed string physical model

// define a D Major Scale (in MIDI note numbers)
[62, 64, 66, 67, 69, 71, 73, 74] @=> int scale[];

// run through our scale one note at a time
for (0 => int i; i < scale.cap(); i++)
{
    // set frequency according to note number
    Std.mtof(scale[i]) => viol.freq; // (2) Sets pitch with .freq, just like other UGens


    // note on for a while, then note off
    1 => viol.noteOn;     // (3) noteOn starts it bowing automatically
    0.3 :: second => now;

    1 => viol.noteOff;
    0.1 :: second => now;
}

// repeat last note with lots of vibrato
1 => viol.noteOn; 
0.1 => viol.vibratoGain;  // (4) A little extra vibrato on the last note
2.0 :: second => now;

0 => viol.noteOff;
0.2 :: second => now;
