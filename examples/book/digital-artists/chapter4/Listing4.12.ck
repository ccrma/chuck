// Listing 4.12a SndBufs and panning connections for your big drum machine

// Drum Machine, version 4.0
// by block-rockin programmer, Jan 1, 2099

// Here we'll use Modulo % and random to play drums
// Define array of master Gains for left, center, right
Gain master[3];          
master[0] => dac.left;   // Connects left ([0]) to dac.left.
master[1] => dac;        // Connects center ([1]) to dac (both
                         //   left and right automatically).
master[2] => dac.right;  // Connects right ([2]) to dac.right.

// Declare SndBufs for lots of drums
// hook them up to pan positions
SndBuf kick => master[1];  // Connects kick drum SndBuf to center master gain.
SndBuf snare => master[1]; // Connects snare drum to center also.
SndBuf hihat => master[2]; // Connects hihat to right master gain.
SndBuf cowbell => master[0]; // Connects cowbell SndBuf to left master gain.

// Use a Pan2 for the hand claps,
// we'll use random panning later
SndBuf claps => Pan2 claPan; // Connects clap SndBuf to a Pan2 object.
claPan.chan(0) => master[0]; // Connects the left (0) channel of the Pan2 to master gain left.
claPan.chan(1) => master[2]; // Connects the right (1) channel of the Pan2 to master gain right.

// load up some samples of those            // Loads all the sound files.
me.dir()+"/audio/kick_01.wav" => kick.read;
me.dir()+"/audio/snare_01.wav" => snare.read;
me.dir()+"/audio/hihat_01.wav" => hihat.read;
me.dir()+"/audio/cowbell_01.wav" => cowbell.read;
me.dir()+"/audio/clap_01.wav" => claps.read;

// Listing 4.12b Setting up variables for your big drum machine

// (1) Array to control cowbell strikes.
[1,0,1,0, 1,0,0,1, 0,1,0,1, 0,1,1,1] @=> int cowHits[];

// controls the overall length of our "measures"
// .cap() determines the maximum number of beats in your measure.
cowHits.cap() => int MAX_BEAT; // define using all caps, remember?

// modulo number for controlling kick and snare
4 => int MOD;  // Constant for MOD operator--
               // You'll use this to control
               // kick and snare drum hits.

// overall speed control     // Master speed control (tempo)--
0.15 :: second => dur tempo; // you'll use this to advance time each beat.

// counters: beat within measures, and measure
0 => int beat;    // Two counters, one for beat 
0 => int measure; // and one for measure number.

// Listing 4.12c Main loop to actually play drum patterns

// Main infinite drum loop
while (true)
{
    // play kick drum on all main beats (0, 4, ...)
    if (beat % 4 == 0)     // (1) Uses MOD 4 to play 
    {                      //     kick drum every fourth beat
        0 => kick.pos;
    }

    // after a time, play snare on off beats (2, 6, ...)
    if (beat % 4 == 2 && measure %2 == 1) // (2) Plays snare only on specific beats
    {
        0 => snare.pos;
    }

    // After a time, randomly play hihat or cowbell
    if (measure > 1) {    //  (3) Plays cowbell and hihat only after measure 1
        if (cowHits[beat])   // (4) Plays cowbell, controlled by array
        {
            0 => cowbell.pos;
        }
        else              // (5) If not cowbell, then hi-hat
        {
            Math.random2f(0.0,1.0) => hihat.gain;  // (6) Hi-hat has random gain
            0 => hihat.pos;
        }
    }

    // after a time, play randomly spaced claps at end of measure
    if (beat > 11 && measure > 3)  // (7) Plays claps only on certain measures and beats
    {
        Math.random2f(-1.0,1.0) => claPan.pan; // (8) Claps have random pan
        0 => claps.pos;
    }

    tempo => now;               // (9) Waits for one beat...

    (beat + 1) % MAX_BEAT => beat; // (10) ...and then updates beat counter (MOD MAX)
    if (beat==0)
    {            // (11) Increments measure counter at each new measure
        measure++;  
    }
}
