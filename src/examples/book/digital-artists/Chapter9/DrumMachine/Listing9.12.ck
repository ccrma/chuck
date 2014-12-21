//  Listing 9.12 snare.ck plays on beats 2 and 4 and more, conducted by global BPM

// snare.ck
// on the fly drumming with global BPM conducting
SndBuf snare => dac;
me.dir(-1)+"/audio/snare_01.wav" => snare.read;
0.5 => snare.gain;
snare.samples() => snare.pos;

// make a conductor for our tempo
// this is set and updated elsewhere
BPM tempo;
while (1) {
    // update our basic beat each measure
    tempo.quarterNote => dur quarter; // (1) Updates tempo at beginning of each measure...

    // play measure of: rest, snare, rest, sna-snare
    quarter => now;           // (2) ...waits a beat (quarter)...
    0 => snare.pos;           // (3) ...plays a snare hit...
    2.0*quarter => now;       // (4) ...waits two beats...

    0 => snare.pos;           // (5) ...plays another hit...
    quarter/4.0 => now;       // (6) ...waits for half a beat...

    0 => snare.pos;           // (7) ...plays another hit...
    3.0*quarter/4.0 => now;   //(8) ...waits for remainder of the measure.
}

