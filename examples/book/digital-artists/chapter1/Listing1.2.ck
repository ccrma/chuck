// Listing 1.2 Sine wave music

/* Sine Music         // (1) comment block; ChucK ignores this.
by ChucK Programmer
January 2025 */
SinOsc s => dac;

// Play one note      // (2) Connects sine oscillator to dac
220 => s.freq;        // (3) Sets the gain to 1.0 and frequency to 220 Hz.
1.0 => s.gain;        
0.3 :: second => now; //     Let this run for 0.3 second by ChucKing it to
                      //     now (advance time).

0.0 => s.gain;        // (4) Makes your sine oscillator silent
                      //     for 0.3 seconds to separate it
0.3 :: second => now; //     from the next note.

// Play another note, same pitch
1.0 => s.gain;        // (5) Repeats the process of
0.3 :: second => now; //     blocks B and D.
0.0 => s.gain;
0.3 :: second => now;

// Play two more notes, higher, less loud
330 => s.freq;        // (6)  Repeats the same two-note
0.3 => s.gain;        //      pattern of blocks B, D, and E
0.3 :: second => now; //      but with a different frequency
                      //      (pitch) and gain (loudness).
0.0 => s.gain;
0.3 :: second => now;

0.3 => s.gain;
0.3 :: second => now;

0.0 => s.gain;
0.3 :: second => now;
