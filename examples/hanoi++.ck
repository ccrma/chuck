// towers of annoy
// demonstrates: recursion, arrays, stereo, sndbuf,
//               questionable sonification

// number of disks
10 => int disks;
// min duration for each move
200::ms => dur wait => dur the_wait;
// step number
0 => int STEPS;

// gain to pan to dac
Gain g => Pan2 pan;
// stereo from here
pan.left => NRev r1 => dac.left;
pan.right => NRev r2 => dac.right;
// set gain
.5 => g.gain;
// set mix
.1 => r1.mix => r2.mix;

// the pegs (0 not used - for easy indexing)
SndBuf pegs[4];

// load files
me.dir() + "/data/snare-chili.wav" => pegs[1].read;
me.dir() + "/data/kick.wav" => pegs[2].read;
me.dir() + "/data/snare-hop.wav" => pegs[3].read;

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
    // gain
    Math.random2f( .2, .9 ) => pegs[dest].gain;
    // pan
    .8 * (dest - 2) => pan.pan;
    // advance time
    the_wait => now;

    // move onto the biggest
    if( num > 1 ) hanoi( num - 1, other, dest, src );
}

// start it
hanoi( disks, 1, 3, 2 );

<<<"done!">>>;

// let time pass for reverb to go...
2::second => now;


