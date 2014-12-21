// Listing 4.9 Improving the while loop of your drum machine

// Drum Machine, version 2.0
// by block-rockin programmer, Dec 32, 1999

// SndBufs for kick (bass) and snare drums
SndBuf kick => Gain master => dac; // (1) Kick to master mixer Gain to dac.
SndBuf snare => master;            // (2) Snare also to master mixer.

// load up some samples of those   // (3) Loads your kick and snare drum sound files.
me.dir()+"/audio/kick_01.wav" => kick.read;
me.dir()+"/audio/snare_01.wav" => snare.read;

// declare a new tempo variable
0.15 :: second => dur tempo;       // (4) Tempo duration is time between beats.

while (true)
{
    for (0 => int beat; beat < 16; beat++)  // (5) Loops over a "measure" of 16 beats.
    {
        // play kick drum on beat 0, 4, 8, and 12
        if (beat==0 || beat==4 || beat==8 || beat==12)  // (6) Plays kick only on specific beats.
        {
            0 => kick.pos;
        }

        // play snare drum on beat 4, 10, 13, and 14
        if (beat==4 || beat==10 || beat==13 || beat==14) // (7) Plays snare only on other specific beats.
        {
            0 => snare.pos;
        }
        tempo => now;
    }
}
