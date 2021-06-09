// towers of annoy : non-sonified version
// demonstrates: recursion

// number of disks
10 => int disks;
// step number
0 => int STEPS;

// moves
0 => int moves;

// the hanoi
fun void hanoi( int num, int src, int dest, int other )
{
    // stop here
    if( num <= 0 ) return;

    // increment steps
    STEPS++;

    // move all except the biggest
    hanoi( num - 1, src, other, dest );
    // move the biggest
    <<< "step", STEPS, " | move disk from peg", src, " -> ", "peg", dest >>>;
    // move onto the biggest
    hanoi( num - 1, other, dest, src );
}

// start it
hanoi( disks, 1, 3, 2 );

<<< "done.", "" >>>;
