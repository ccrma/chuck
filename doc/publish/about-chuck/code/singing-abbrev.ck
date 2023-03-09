// synthesis patch
Impulse i => TwoZero t => TwoZero t2 => OnePole lpf;
// formant filters
lpf => TwoPole f1 => Gain node => NRev reverb => dac;
lpf => TwoPole f2 => node;
lpf => TwoPole f3 => node;
// ... (omitted: initialization code to set formant filter Qs,
// adjust reverb mix, etc.) ...

// concurrency
spork ~ generate(); // spawn shred #2: voice source
spork ~ interpolate( 10::ms ); // spawn shred #3: interpolate pitch & formants

// shred #1: time loop on the "main" shred
while( true )
{
    // set next formant targets
    Math.random2f( 230.0, 660.0 ) => target_f1freq;
    Math.random2f( 800.0, 2300.0 ) => target_f2freq;
    Math.random2f( 1700.0, 3000.0 ) => target_f3freq;
    // random walk the scale, choose next frequency
    32 + scale[randWalk()] => Std.mtof => freq;
    // set target period from frequency
    1.0 / freq => targetPeriod;
    // wait until next note
    Math.random2f( 0.4, 1.2 )::second => now;
}

// shred #2: generate pitched source, with vibrato
fun void generate()
{
    // infinite time loop
    while( true )
    {
        // fire impulse!
        sourceGain => i.next;
        // advance phase based on period
        modphase + period => modphase;
        // advance time (modulated to achieve vibrato)
        (period + 0.001*Math.sin(2*pi*modphase*6.0))::second => now;
    }
}

// shred #3: to perform interpolation for various parameters
fun void interpolate( dur interpRate )
{
    // slewing factor to control interpolation rate
    0.10 => float slew;
    // infinite time loop
    while( true )
    {
        // interpolate
        (targetPeriod - period) * slew + period => period;
        (target_f1freq - f1freq) * slew + f1freq => f1freq => f1.freq;
        (target_f2freq - f2freq) * slew + f2freq => f2freq => f2.freq;
        (target_f3freq - f3freq) * slew + f3freq => f3freq => f3.freq;
        // our interpolation rate
        interpRate => now;
    }
}
