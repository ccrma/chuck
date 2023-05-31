// testing for filter blowup | 1.5.0.0 (ge) added
// NOTE: intended to be run with --silent
//
// filter explosions observed, before this commit d53cdfb
// https://github.com/ccrma/chuck/commit/d53cdfbf448c1c52a760fc2769e94204ad3a6f4d
//
// macOS (64-bit, macOS Venture)
// *** BLOW UP at time: 19.815533 value: 100.037407 freq: 0.100000 Q: 7.844161
// linux (64-bit, planetCCRMA)
// *** BLOW UP at time: 33.817312 value: 100.002319 freq: 0.100000 Q: 1.309424
// windows (32-bit and 64-bit, windows 10)
// *** BLOW UP at time: 18.635624 value: 100.004440 freq: 0.100000 Q: 9.886994
//
// author: Ge Wang
// date: Winter 2023

// blackhole for maximum safety from filter explosion
Step n => HPF f => blackhole;

// sus
30000 => f.freq;
// set step function
1 => n.next;
// did we detect blow up
false => int blowup;

// later
now + 50::second => time later;
// go until later
while( now < later )
{
    // sweep Q
    .5 + 10*(1+Math.sin(now/second))/2 => f.Q;
    1::samp => now;
    
    // consider this a blow up
    if( f.last() > 100 )
    {
        // note
        <<< "*** BLOW UP at time:", now/second, "value:", f.last(),
            "freq:", f.freq(), "Q:", f.Q() >>>;
        // flag
        true => blowup;
        // bail
        break;
    }
}

// no blow up? great
if( !blowup ) <<< "success" >>>;
