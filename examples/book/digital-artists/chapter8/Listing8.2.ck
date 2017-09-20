// Listing 8.2 Parallel, multithreaded, concurrent drum machine

// by Drummond Base, October 2020
// First define some global durations
1 :: second => dur whole; // (1) Defines whole, half, and quarter note durations.
whole / 2 => dur half;
whole / 4 => dur quarter;

// Kick drum function
fun void kick() {         // (2) Function to play kick drum.
    SndBuf kick => dac;
    me.dir()+"/audio/kick_01.wav" => kick.read;
    // only play every whole note
    while (true) {
        whole => now;
        0 => kick.pos;
    }
}

// Snare drum function
fun void snare() {        // (3) Function to play snare drum.
    SndBuf snare => dac;
    me.dir()+"/audio/snare_01.wav" => snare.read;
    // play with half note tempo
    while (true) {
        half => now;
        0 => snare.pos;
    }
}

// Hi hat drum function
fun void hihat() {        // (4) Function to play hi-hat.
    SndBuf hihat => dac;
    me.dir()+"/audio/hihat_01.wav" => hihat.read;
    0.2 => hihat.gain;
    // play every quarter note
    while (true) {
        quarter => now;
        0 => hihat.pos;
    }
}

// Main program to spork our individual drum functions
// start off with kick drum for two measures
spork ~ kick();         // (5) Sporking kick function starts it playing
2*whole => now;

// then add in hi hat after two measures
spork ~ hihat();        // (6) Sporks hi-hat after a time
2*whole => now;

// add snare, but on off beats (quarter delayed start)
quarter => now;
spork ~ snare();        // (7) Sporks snare starting on off beat

// let it run for four more measures
4*whole => now;         // (8) Lets it all run for four more measures
