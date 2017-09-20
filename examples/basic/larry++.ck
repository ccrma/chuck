// run each stooge, or run three stooges concurrently
// %> chuck moe++ larry++ curly++

// impulse to filter to dac
SndBuf i => BiQuad f => Gain g => JCRev r => dac;
// second formant
i => BiQuad f2 => g;
// third formant
i => BiQuad f3 => g;

// set the filter's pole radius
0.800 => f.prad; .995 => f2.prad; .995 => f3.prad;
// set equal gain zeroes
1 => f.eqzs; 1 => f2.eqzs; 1 => f3.eqzs;
// initialize float variable
0.0 => float v => float v2;
// set filter gain
.1 => f.gain; .1 => f2.gain; .01 => f3.gain;
0.05 => r.mix;
// load glottal pop
"special:glot_pop" => i.read;
// play
1.0 => i.rate;
  
// infinite time-loop   
while( true )
{
    // set the current sample/impulse
    0 => i.pos;
    // sweep the filter resonant frequency
    250.0 + Math.sin(v*100.0)*20.0 => v2 => f.pfreq;
    2290.0 + Math.sin(v*200.0)*50.0 => f2.pfreq;
    3010.0 + Math.sin(v*300.0)*80.0 => f3.pfreq;
    // increment v
    v + .05 => v;
    // gain
    0.2 + Math.sin(v)*.1 => g.gain;
    // advance time
    (1000.0 + Math.random2f(-100.0, 100.0))::ms => now;
}
