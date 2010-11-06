// analysis patch
SinOsc foo => Flip flip =^ ZeroX zerox => blackhole;

// set flip params
4096 => flip.size;

// control
fun void ctrl()
{
    // go
    while( true )
    {
        Std.rand2f( 100, 2000 ) => foo.freq;
        <<< "setting new freq:", foo.freq() >>>;
        Std.rand2f( 1, 2 )::second => now;
    }
}

// spork down
spork ~ ctrl();

// go
while( true )
{
    // compute ZeroX
    zerox.upchuck() @=> UAnaBlob blob;

    // print result
    <<< blob.fvals()[0], "" >>>;

    // advance time
    flip.size()::samp => now;
}
