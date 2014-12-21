// Listing 8.8 Jazz band: 
// save as "flute.ck"

// Our famous headliner flute solo (with EFX)
Flute solo => JCRev rev => dac;    // (1) Flute soloist through reverb...
0.1 => rev.mix;
solo => Delay d => d => rev;       // (2) ...also through a delay line. 
0.8 :: second => d.max => d.delay; // (3) Sets delay to 0.8 seconds. 
0.5 => d.gain;
0.5 => solo.gain;

// jazz scale data
[41,43,45,48,50,51,53,60,63] @=> int scale[]; // (4) Jazz scale array.

// then our main loop headliner flute soloist
while (1) {           // (5) Infinite loop.
                      // (6) Waits either 0.2, 0.4, 0.6, 0.8, or 1.0 seconds.
    (Math.random2(1,5) * 0.2) :: second => now;
    if (Math.random2(0,3) > 1) {                   // (7) Occasionally (50% of time) 
        Math.random2(0,scale.cap()-1) => int note; //     picks a new note...
        Math.mtof(24+scale[note])=> solo.freq;
        Math.random2f(0.3,1.0) => solo.noteOn;
    }
    else 1 => solo.noteOff;      // (8) ...otherwise turns note off.
}
