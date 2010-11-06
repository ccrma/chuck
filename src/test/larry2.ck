
// impulse to filter to dac
impulse i => biquad f => pan2 p => dac;
// set the filter's pole radius
.99 => f.prad; 
// set equal gain zeros
1 => f.eqzs;
// set filter gain
.5 => f.gain;
  
// infinite time-loop   
while( true )
{
    // set the current sample/impulse
    1.0 => i.next;
    // set filter resonant frequency
    std.rand2f( 500, 5000 ) => f.pfreq;
    // pan
    std.rand2f( -1, 1 ) => p.pan;
    // advance time
    101::ms => now;
}

