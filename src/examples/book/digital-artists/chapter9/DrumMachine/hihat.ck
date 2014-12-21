// Listing 9.14 hihat.ck plays on all eighth notes except the last, synced to BPM
// hihat.ck
// on the fly drumming with global BPM conducting
SndBuf hat => dac;
0.3 => hat.gain;
me.dir(-1)+"/audio/hihat_02.wav" => hat.read;

// make a conductor for our tempo
// this is set and updated elsewhere
BPM tempo;
while (1) {
    // update our basic beat each measure
    tempo.eighthNote => dur eighth;
    // play a measure of eighth notes
    for (0 => int beat; beat < 8; beat++) {
        // play mostly, but leave out last eighth
        if (beat != 7) {     // (1) Plays all eighths except the last one 
            0 => hat.pos;    //     (the anti-cowbell)
        }
        eighth => now;
    }
}
