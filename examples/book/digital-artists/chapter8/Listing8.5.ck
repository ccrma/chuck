// Listing 8.5 Jazz band: 
// save as "piano.ck" 
// sound chain 
Rhodey piano[4];             // (1) Four notes' worth of electric piano 

piano[0] => dac.left;        // (2) Connects them to left...
piano[1] => dac;             //     ...and center... 
piano[2] => dac;             
piano[3] => dac.right;       // (3) ...and right dac channels.

// chord 2D array            // (4) Array to hold two chords of four notes each.
[[53,57,60,64],[51,55,60,63]] @=> int chordz[][]; 

// loop
while( true )                  // (5) Infinite loop
{
    // build first chord
    for( 0 => int i; i < 4; i++ )
    {                          // (6) Plays first chord...
        Std.mtof(chordz[0][i]) => piano[i].freq;
        Math.random2f(0.3,.7) => piano[i].noteOn;
    }
    1.0 :: second => now;
    // build second chord
    for( 0 => int i; i < 4; i++ )
    {                          // (7) ...then second chord
        Math.mtof(chordz[1][i]) => piano[i].freq;
        Math.random2f(0.3,.7) => piano[i].noteOn;
    }
    1.4 :: second => now;
}
