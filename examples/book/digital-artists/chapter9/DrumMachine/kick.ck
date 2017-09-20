// Listing 9.11 kick.ck kick drum file, conducted by BPM
// kick.ck, on the fly drumming with global BPM conducting
SndBuf kick => dac;
1 => kick.gain;
me.dir(-1)+"/audio/kick_04.wav" => kick.read;  // (1) Reads a kick drum sound for this instrument

// make a conductor for our tempo
// this is set and updated elsewhere
BPM tempo;                      // (2) Makes a BPM to use to sync with others
while (1) {                     // (3) Infinite loop

    // update our basic beat each measure
    tempo.quarterNote => dur quarter;       // (4) Uses quarter note from BPM

    // play a measure of quarter note kicks
    for (0 => int beat; beat < 4; beat++) { // (5) Four-beat measure
        0 => kick.pos;       // (6) Plays on every beat
        quarter => now;
    }
}
