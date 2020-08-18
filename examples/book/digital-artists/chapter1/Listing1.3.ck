// Listing 1.3 Using comments to document code and control execution

// Author: ChucK Team            // Initial comment, documents who
// Date: Today's date            // wrote the program and when.

// make a sound chain ("patch")
SinOsc s => dac;                 // Sets up sound signal chain.

// prints out program name
<<< "Hello Sine!" >>>;           // (1) Prints a greeting message.

// set volume to 0.6             // Sets up parameters to play a note.
.6 => s.gain;
// set frequency to 220.0
220.0 => s.freq;
// play for 1 second
second => now;

0.5 => s.gain; // set volume to 0.5    // Plays another note.
440 => s.freq; // set frequency to 440
2::second => now; // play for two seconds

// comment out this third note for now
/*
0.3 => s.gain;                        // (2) Don't play this last note
330 => s.freq;                        //     right now, because we've
3::second => now;                     //     commented it out.
*/
