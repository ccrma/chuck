// Listing 5.14 Drum machine using patterns stored in arrays

// sound chain: two drums
SndBuf kick => dac;  // (1) SndBufs for kick and snare drum sounds
SndBuf snare => dac;

// (2) load the sound files for our kick and snare drums
me.dir() + "/audio/kick_01.wav" => kick.read;
me.dir() + "/audio/snare_03.wav" => snare.read;

// (3) set their pointers to end, to make no sound
kick.samples() => kick.pos;
snare.samples() => snare.pos;
// drum patterns as logical variables, // (4) Arrays to hold logical values, play=1/not play=0
[1,0,0,0,1,0,0,0] @=> int kickPattern1[];
[0,0,1,0,0,0,1,0] @=> int kickPattern2[];
[1,0,1,0,1,0,1,0] @=> int snarePattern1[];
[1,1,1,1,0,1,1,1] @=> int snarePattern2[];

// function to play pattern arrays
fun void playSection(int kickA[], int snareA[], float beattime)
{  // (5) playSection function, arrayarguments control patterns
    for (0 => int i; i < kickA.cap(); i++)
    {
        if (kickA[i])
        {
            0 => kick.pos;
        }
        if (snareA[i])
        {
            0 => snare.pos;
        }
        beattime::second => now;
    }
}

// Main program, infinite loop
while (true)    // (6) Infinite test loop
{               // (7) Call playSection with different patterns
    playSection(kickPattern1,snarePattern2,0.2);
    playSection(kickPattern2,snarePattern2,0.2);
    playSection(kickPattern1,snarePattern2,0.2);
    playSection(kickPattern2,snarePattern1,0.2);
}
