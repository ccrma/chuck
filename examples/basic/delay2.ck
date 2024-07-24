// name: delay2.ck
// desc: example of modulating delay using DelayL
// requires: 1.5.2.5 or higher

// patch
SndBuf doh("special:doh") => DelayL delay(.25::second, 1::second) => dac;

// shred to say "doh!" periodically
spork ~ sayDOH();

// infinite time loop
while( true )
{
    // modulate delay length
    .5::second + .4*Math.sin(now/second*2)::second => delay.delay;
    // every sample; might be extreme...but smooth
    1::samp => now;
}

// say doh
fun void sayDOH()
{
    // time loop
    while( true )
    {
        // reset playhead
        0 => doh.pos;
        // advance time
        400::ms => now;
    }
}