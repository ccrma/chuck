// towers of annoy
// demonstrates: recursion, arrays, sndbuf, stereo, questionable sonification

// number of disks
11 => int disks;
// duration for each move
200::ms => dur wait;
// step number
0 => int STEPS;

// gain to pan to dac
Gain g => Pan2 pan => JCRev r => dac;
// set gain
.5 => g.gain;
// set mix
.15 => r.mix;

// the pegs (0 not used - for easy indexing)
SndBuf pegs[4];

// load files
"data/hihat-open.wav" => pegs[1].read;
"data/hihat.wav" => pegs[2].read;
"data/snare-chili.wav" => pegs[3].read;

// connect to gain
for( 1 => int i; i < pegs.size(); i++ )
    pegs[i] => g;

// the hanoi
fun void hanoi( int num, int src, int dest, int other )
{
    // move all except the biggest
    if( num > 1 ) hanoi( num - 1, src, other, dest );

    // increment steps
    STEPS++;
    // move the biggest
    <<< "step", STEPS, " | move disk from peg", src, " -> ", "peg", dest >>>;
    // sonify
    0 => pegs[dest].pos;
    // pan
    dest - 2 => pan.pan;
    // advance time
    wait => now;

    // move onto the biggest
    if( num > 1 ) hanoi( num - 1, other, dest, src );
}

hanoi( disks, 1, 3, 2 );

<<< "done.", "" >>>;

// let time pass for reverb to go...
2::second => now;
