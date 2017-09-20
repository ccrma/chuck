//--
// name: harm.ck
// desc: using Tracking and Smacking to harmonize
//
// author: Rebecca Fiebrink + Ge Wang
// date: 2007
//--


// synthesis
BlitSaw s => JCRev r => dac;
4 => s.harmonics;

// set reverb mix
.05 => r.mix;
// possible hamonizing intervals
[ 3-24, 4-12, 7, 9+12 ] @=> int harms[];
float the_harm;

// interpolate
float target_freq, curr_freq, target_gain, curr_gain;
spork ~ ramp_stuff();
spork ~ smack_handler();

// go for it
while( true )
{
    // wait for analysis event
    Tracking.the_event => now;
    
    // set freq
    Tracking.the_freq * Math.pow(1.059, the_harm) => target_freq;
    // set gain
    Tracking.the_gain => target_gain;
}

// interpolation
fun void ramp_stuff()
{
    // mysterious 'slew'
    0.025 => float slew;
    
    // infinite time loop
    while( true )
    {
        (target_freq - curr_freq) * 1 * slew + curr_freq => curr_freq => s.freq;
        (target_gain - curr_gain) * .25 * slew + curr_gain => curr_gain => s.gain;
        0.0025::second => now;
    }
}

// handles smack
fun void smack_handler()
{
    // go
    while( true )
    {
        // wait
        Smacking.the_event => now;
        
        // choose random interval
        harms[Math.random2(0,harms.size()-1)] => the_harm;
        // set freq
        Tracking.the_freq * Math.pow(1.059, the_harm) => target_freq;
        // print
        <<< "new interval:", the_harm >>>;
    }
}
