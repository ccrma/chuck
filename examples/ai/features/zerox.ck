//----------------------------------------------------------------
// name: ZeroX (Zero Crossing Detector)
// desc: Detect the number of zero crosses in a Flip of samples
//       1. Flip will convert N samples to a UAna Frame
//       2. ZeroX will then count the number of zero crossings in 
//       the UAna Frame 
//       
//       Note the ZeroX output in this example is equal to
//       zeroX = 2*freq / (sample_rate / N) 
//----------------------------------------------------------------

// analysis patch
SinOsc foo => Flip flip =^ ZeroX zerox => blackhole;

// set flip size (N)
4096 => flip.size;

// change the frequency of foo randomly every 1-2 seconds
fun void ctrl()
{
    while( true )
    {
        // new random freq
        Math.random2f( 100, 2000 ) => foo.freq;
        <<< "setting new freq:", foo.freq() >>>;
        // wait 1-2 seconds
        Math.random2f( 1, 2 )::second => now;
    }
}

// spork frequency changer shred
spork ~ ctrl();

// main
while( true )
{
    // compute ZeroX in the last UAna Frame
    // store the result in a UAnaBlob
    zerox.upchuck() @=> UAnaBlob blob;

    // print number of zero crossings
    <<< blob.fvals()[0], "" >>>;

    // advance time
    flip.size()::samp => now;
}
