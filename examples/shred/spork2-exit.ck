// name: spork3-exit.ck
// desc: how to spork and terminate specific child shreds

// a function to spork
fun void go( int n )
{
    while( true )
    {
        <<< "go", n >>>;
        250::ms => now;
    }
}

// spork three shreds, save the resulting shred reference
spork ~ go(1) @=> Shred s1;
spork ~ go(2) @=> Shred s2;
spork ~ go(3) @=> Shred s3;

// exit the shreds
1::second => now;
s1.exit();
1::second => now;
s2.exit();
1::second => now;
s3.exit();

// yield this shred, give chance for s3 to run at current time
me.yield();
// print done
<<< "done...", "" >>>;

// wait a bit more to see nothing is printed from child shreds
1::second => now;

// print out Shred functionality, if you are curious
// Shred.help();

// print done
<<< "exiting parent shred...", "" >>>;