// Listing 9.13 cowbell.ck plays only on last eighth note of measures, conducted by BPM
// cowbell.ck
// on the fly drumming with global BPM conducting
SndBuf cow => dac;
0.3 => cow.gain;
me.dir(-1)+"/audio/cowbell_01.wav" => cow.read;

// make a conductor for our tempo
// this is set and updated elsewhere
BPM tempo;

while (1) {
    // update our basic beat each measure
    tempo.eighthNote => dur eighth;  // (1) Synchronizes on eighth notes.
    // play measure of eighths
    for (0 => int beat; beat < 8; beat++) {  // (2) Measure is 8 total eighth notes.
        // but only play on the last 8th
        if (beat == 7) {             // (3) Plays only on the last eighth note.
            0 => cow.pos;
        }
        eighth => now;               // (4) Advance time by eighth.
    }
}
