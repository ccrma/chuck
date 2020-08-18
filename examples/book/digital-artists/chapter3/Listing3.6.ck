// Listing 3.6 Storing durations in an array

// Let's Twinkle with a square wave
SqrOsc s => dac;           // (1) Square wave oscillator for melody

// gains to separate our notes
0.7 => float onGain;       // (2) Note on/off gains 
0.0 => float offGain;

// (3) declare and initialize array of MIDI notes (int) for melody
[57,57,64,64,66,66,64,62,62,61,61,59,59,57] @=> int myNotes[];

// quarter note and half note durations
0.3 :: second => dur q;    // (4) Duration for quarter notes
0.8 :: second => dur h;    // (5) Duration for half notes
                           // (6) Array of durations for melody notes
[q, q, q, q, q, q, h, q, q, q, q, q, q, h] @=> dur myDurs[];

// loop for length of array
for (0 => int i; i < myNotes.cap(); i++)  // (7) For loop iterates 
{                                         //     over length of note array
    // set frequency and gain to turn on our note
    Std.mtof(myNotes[i]) => s.freq;       // (8) Sets pitch for melody notes
    onGain => s.gain;                     // (9) Note on
    myDurs[i] => now;        // (10) For duration stored in array for that note

    // turn off our note to separate from the next
    offGain => s.gain;                    // (11) Note off
    0.2::second => now;
}
