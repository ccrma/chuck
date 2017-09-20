// SIMPLE ENVELOPE FOLLOWER, by P. Cook

/*
Hi all.  I keep meaning to post to this list about the under-exploited
feature that all unit generators have, in that you can cause their inputs
to multiply rather than add.  As an example, here's a simple power
envelope follower that doesn't require sample-level chuck intervention.  A
gain UG is used to square the incoming A/D signal (try it on your built-in
mic), then a OnePole UG is used as a "leaky integrator" to keep a running
estimate of the signal power. The main loop wakes up each 100 ms and
checks the power, and prints out a message if it's over a certain level. 
You might need to change the threshold, but you get the idea.
*/

// patch
adc => Gain g => OnePole p => blackhole;
// square the input
adc => g;
// multiply
3 => g.op;

// set pole position
0.99 => p.pole;

// loop on
while( true )
{
    if( p.last() > 0.01 )
    {
        <<< "BANG!!" >>>;
        80::ms => now;
    }
    20::ms => now;
}
