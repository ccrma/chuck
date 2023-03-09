// computer key input, with sound
KBHit kb;

// patch
Impulse i => BiQuad f => dac;
// set the filter's pole radius
.99 => f.prad;
// set equal gain zeros
1 => f.eqzs;
// initialize float variable
0.0 => float v;
// set filter gain
.5 => f.gain;

// time-loop
while( true )
{
    // wait on event
    kb => now;
    // generate impulse
    1.0 => i.next;

    // loop through 1 or more keys
    while( kb.more() )
    {
        // set filtre freq
        kb.getchar() => int c => Std.mtof => f.pfreq;
        // print int value
        <<< "ascii:", c >>>;
    }
}
