// Listing 8.6 Jazz band:
// save as "bass.ck"

// sound chain (mandolin for bass)
Mandolin bass => NRev r => dac;     // (1) Mandolin bass player through reverb.

// parameter setup 
0.1 => r.mix; 
0.0 => bass.stringDamping; // (2) Makes strings ring a long time. 
0.02 => bass.stringDetune; // (3) Gives it a really big bass-sized body.
0.05 => bass.bodySize;
.5 => bass.gain;

// jazz scale data         // (4) Scale array for walking bass line.
[41,43,45,48,50,51,53,60,63] @=> int scale[]; 
4 => int walkPos;          // (5) Pointer for position within scale.

// loop
while( true )              // (6) Walks forever.
{
    Math.random2(-1,1) +=> walkPos;          // (7) Adds 1 or -1 or 0 to scale position.
    if (walkPos < 0) 1 => walkPos;           // (8) Makes sure position doesn't go outside the array...
    if (walkPos >= scale.cap()) scale.cap()-2 => walkPos; // (9) ...on the top end too.
    Std.mtof(scale[walkPos]-12) => bass.freq;    // (10) Sets pitch from scale note.
    Math.random2f(0.05,0.5) => bass.pluckPos;    // (11) Random plucking position.
    1 => bass.noteOn;
    0.55 :: second => now;
    1 => bass.noteOff;
    0.05 :: second => now;
}
