// Listing 4.11 Using the modulo operator

// Modulo math for music
// by musical math dude, 11/11/11

// make and load a couple of SndBufs to "sonify" modulo
SndBuf clickhi => dac;        // (1) Loads two different sound files
SndBuf clicklo => dac;
me.dir()+"/audio/click_02.wav" => clickhi.read;
me.dir()+"/audio/click_01.wav" => clicklo.read;

// define a number for our modulo
4 => int MOD;                 // (2) Modulo limit MOD

for (0 => int beat; beat < 24; beat++) {  // (3) 24-beat "measure"
    // print out beat and beat modulo MOD
    <<< beat, "Modulo ", MOD, " is: ", beat % MOD >>>;

    // click this on every beat
    0 => clickhi.pos;       // (4) High sound on every beat

    // but only click this on every "MODth" beat
    if (beat % MOD == 0) {  // (5) Low sound only every MOD beat
        0 => clicklo.pos;  
    }

    0.5 :: second => now;
}
