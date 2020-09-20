//  Listing 3.4 Playing a melody stored in an array

// Let's Twinkle with a square wave
SqrOsc s => dac;        // (1) Square wave oscillator for melody

// gains to separate our notes
0.7 => float onGain;    // (2) Note on/off gains
0.0 => float offGain;

// (3) declare and initialize array of MIDI notes (int) for melody
[57,57,64,64,66,66,64,62,62,61,61,59,59,57] @=> int a[];

// loop for length of array
for (0 => int i; i < a.cap(); i++)
{
    <<< i, a[i] >>>;                 // (4) Prints index and array note

    // set frequency and gain to turn on our note
    Std.mtof(a[i]) => s.freq;        // (5) Sets pitch for melody notes
    onGain => s.gain;                // (6) Note on
    0.3::second => now;              // (7) Duration for note on

    // turn off our note to separate from the next
    offGain => s.gain;               // (8) Note off
    0.2::second => now;
}
