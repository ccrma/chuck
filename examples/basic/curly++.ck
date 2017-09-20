// run each stooge, or run three stooges concurrently
// %> chuck moe++ larry++ curly++

// impulse to filter to dac
SndBuf i => NRev r => dac;

// load glottal ooo
"special:glot_ooo" => i.read;
// play 
//5.0 => i.rate;
.1 => r.mix;

0.0 => float v;
  
// infinite time-loop   
while( true )
{
    // set the current sample/impulse
    0 => i.pos;
    // control gain
    Math.cos(v) => i.gain;
    // increment v
    .05 +=> v;
    // advance time
    81.0::ms => now;
}




