// Listing 1.20a Putting "Twinkle" all together, with two waves!

// Twinkle, with two oscillators!
SinOsc s => dac;             // (1) Sine wave oscillator.
TriOsc t => dac;             // (2) Another oscillator (triangle wave).

// our main pitch variable
110.0 => float melody;       // (3) Sets initial pitch.

// gain control for our triangle wave melody
0.3 => float onGain;         // (4) Gains for note on.

// we'll use this for our on and off times
0.3 :: second => dur myDur;  // (5) Notes duration.

// Listing 1.20b Putting "Twinkle" all together (part B, Sweeping Upward)

// only play t at first, sweeping pitch upward
onGain => t.gain;            // (6) Turns on triangle oscillator.
0 => s.gain;                 // (7) Turns off sine osc. 

while (melody < 220.0) {     // (8) Loops until pitch reaches 220.
    melody => t.freq;
    1.0 +=> melody;          // (9) Steps up pitch by 1 Hz. 
    0.01 :: second => now;   // (10) Every 1/100 of a second.
}

// Listing 1.20c Putting "Twinkle" all together, first "Twinkle"

// turn both on, set up the pitches
0.7 => s.gain;               // (11) Now turn on sin osc too.
110 => s.freq;               // (12) ...and initialize its pitch.

// play two notes, 1st "Twinkle"
for (0 => int i; i < 2; i++) { // (13) Use a for loop to play two notes.
    onGain => t.gain;        // (14) Turn on triangle.
    myDur => now;            // (15) Let note play.
    0 => t.gain;             // (16) Turn off triangle.
    myDur => now;            // (17) Silence to separate notes.
}

// Listing 1.20d Putting "Twinkle" all together , the second "twinkle"

// new pitches!
138.6 => s.freq;            // (19) Sets up next "twinkle" frequency.
1.5*melody => t.freq;

// two more notes, 2nd "twinkle"
for (0 => int i; i < 2; i++) { // (20) Plays that twice (for loop).
    onGain => t.gain;
    myDur => now;
    0 => t.gain;
    myDur => now;
}

// Listing 1.20e Putting "Twinkle" all together, playing "little" and "star"

// pitches for "little"
146.8 => s.freq;            // (20) Sets up next frequency for "little".
1.6837 * melody => t.freq;

// play 2 notes for "little"
for (0 => int i; i < 2; i++) { // (21) Plays that twice (for loop).
    onGain => t.gain;
    myDur => now;
    0 => t.gain;
    myDur => now;
}

// set up and play "star!"
138.6 => s.freq;            // (22) Sets up next frequency for "star".
1.5*melody => t.freq;
onGain => t.gain;           // (23) Plays that note...
second => now;              // (24) ...for a second.

// Listing 1.20f Putting "Twinkle" all together

// end by sweeping both oscillators down to zero
for (330 => int i; i > 0; i--) {  // (25) Uses a for loop to sweep down from 330 Hz.
    i => t.freq;                  
    i*1.333 => s.freq;
    0.01 :: second => now;  // (25) Uses a for loop to sweep down from 330 Hz.
}


