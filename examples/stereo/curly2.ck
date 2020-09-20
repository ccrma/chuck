// impulse to filter to dac
Impulse i => BiQuad f => Pan2 p => dac;
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
    Math.random2f( 50, 5000 ) => f.pfreq;
    // pan
    Math.random2f( -1, 1 ) => p.pan;
    // advance time
    99::ms => now;
}

