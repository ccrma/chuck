// towers of annoy : non-sonified version

// number of disks
10 => int disks;

// moves
0 => int moves;

// the hanoi
fun void hanoi( int num, int src, int dest, int other )
{
    // stop here
    if( num <= 0 ) return;

    // move all except the biggest
    hanoi( num - 1, src, other, dest );
    // move the biggest
    <<< "move disk from peg", src, " -> ", "peg", dest, ": #", ++moves >>>;
    // move onto the biggest
    hanoi( num - 1, other, dest, src );
}

// start it
hanoi( disks, 1, 3, 2 );

<<<"done!">>>;
