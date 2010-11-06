// run each stooge, or run three stooges concurrently
// %> chuck moe larry curly

// impulse to filter to dac
impulse i => biquad f => dac;
// set the filter's pole radius
.99 => f.prad; 
// set equal gain zeros
1 => f.eqzs;
// initialize float variable
0.0 => float v;
// set filter gain
.5 => f.gain;
  
// infinite time-loop   
while( true )
{
    // set the current sample/impulse
    1.0 => i.value;
    // sweep the filter resonant frequency
    std.abs(math.sin(v)) * 4000.0 => f.pfreq;
    // increment v
    v + .1 => v;
    // advance time
    99::ms => now;
}

