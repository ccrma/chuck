// Listing 8.7 Jazz band:
// save as "drums.ck"

// sound chain
SndBuf hihat => dac;                      // (1) Hi-hat sound for percussion.

// me.dir, one level up to find the /audio directory
me.dir(-1) + "/audio/hihat_01.wav" => hihat.read;  // (2) Reads the wave file.

// parameter setup
.5 => hihat.gain;

// loop
while( true )                             // (3) Infinite loop.
{
    Math.random2f(0.1,.3) => hihat.gain;  // (4) Random volume...
    Math.random2f(.9,1.2) => hihat.rate;  // (5) ...and pitch (rate).
                                  // (6) Waits either .2 or .4 seconds
    (Math.random2(1,2) * 0.2) :: second => now;
    0 => hihat.pos;                       // (7) ...then plays and loops.
}
