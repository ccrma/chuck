// Listing 3.8 "Twinkle" with melody, harmony, and lyrics!

// by ChucK Team, July 2050

// two oscillators, melody and harmony
SinOsc s => Pan2 mpan => dac;       // (1) SinOsc through Pan2 for melody
TriOsc t => dac;                    // (2) TriOsc fixed at center for harmony

// we will use these to separate notes later
0.5 => float onGain;                // (3) Note on/off gains
0.0 => float offGain;

// declare and initialize our arrays of MIDI note #s
[57, 57, 64, 64, 66, 66, 64,               // (4) Melody (int) MIDI note array
62, 62, 61, 61, 59, 59, 57] @=> int melNotes[];
[61, 61, 57, 61, 62, 62, 61,               // (5) Harmony (int) MIDI note array
59, 56, 57, 52, 52, 68, 69] @=> int harmNotes[];

// quarter note and half note (6) Duration (dur) array
0.5 :: second => dur q;
1.0 :: second => dur h;
[ q, q, q, q, q, q, h, q, q, q, q, q, q, h] @=> dur myDurs[];

// make one more array to hold the words
["Twin","kle","twin","kle","lit","tle","star,", // (7) Lyrics (string) array
"how", "I","won","der","what","you","are."] @=> string words[];

// loop over all the arrays
// (make sure they're the same length!!)
for (0 => int i; i < melNotes.cap(); i++) // (8) Plays through all 
{                                         //     notes in array
    // (9) print out index, MIDI notes, and words from arrays
    <<< i, melNotes[i], harmNotes[i], words[i] >>>;

    // set melody and harmony from arrays
    Std.mtof(harmNotes[i]) => s.freq;    // (10) Sets frequencies from
    Std.mtof(melNotes[i]) => t.freq;     //      array MIDI notes

    // melody has a random pan for each note
    Math.random2f(-1.0,1.0) => mpan.pan; // (11) Random pan for melody oscillator

    // notes are on for 70% of duration from array
    onGain => s.gain => t.gain;          // (12) Turns on both oscillators
    0.7*myDurs[i] => now;                // (13) 70% of array duration is note on time

    // space between notes is 30% of array duration
    offGain => s.gain => t.gain;
    0.3*myDurs[i] => now;                // (14) 30% of array duration is off time
}
