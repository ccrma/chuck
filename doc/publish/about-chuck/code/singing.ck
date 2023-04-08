// synthesis patch
Impulse i => TwoZero t => TwoZero t2 => OnePole lpf;
// formant filters
lpf => TwoPole f1 => Gain node => NRev reverb => dac;
lpf => TwoPole f2 => node;
lpf => TwoPole f3 => node;

// reverbs
node => JCRev r => dac;
node => JCRev rL => dac.left;
node => JCRev rR => dac.right;

// source gain (amplitude of the impulse train)
0.25 => float sourceGain;

// set filter coefficients
1.0 => t.b0;  0.0 => t.b1; -1.0 => t.b2;
1.0 => t2.b0; 0.0 => t2.b1; 1.0 => t2.b2;
// set reverb mix
0.01 => r.mix;

// set two pole filter radii and gain
0.997 => f1.radius; 0.997 => f2.radius; 0.997 => f3.radius;
1.0 => f1.gain; 0.8 => f2.gain; 0.6 => f3.gain;
// randomize initial formant frequencies
Math.random2f( 230.0, 660.0 ) => f1.freq;
Math.random2f( 800.0, 2300.0 ) => f2.freq;
Math.random2f( 1700.0, 3000.0 ) => f3.freq;

// variables for interpolating current and target formant frequencies
400.0 => float f1freq;
1000.0 => float f2freq;
2800.0 => float f3freq;
400.0 => float target_f1freq;
1000.0 => float target_f2freq;
2800.0 => float target_f3freq;

// leaky integrator / low pass filter
0.99 => lpf.pole;
1.0 => lpf.gain;

// variables that control impulse train source
0.013 => float period;
0.013 => float targetPeriod;
0.0 => float modphase;
0.0001 => float vibratoDepth;

// scale
[ 0, 1, 5, 7,
  8, 11, 8, 7,
  11, 12, 14, 15,
  19, 17, 20, 24 ] @=> int scale[];
// names (for printing)
[ "ut0", "ra0", "fa0", "ut0",
  "ra0", "mi0", "ra1", "ut1", 
  "mi0", "ut1", "re1", "mi1", 
  "ut1", "fa1", "re1", "ut2" ] @=> string names[];
// current location in scale
9 => int scalepoint;
// frequency
float freq;

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

// shred #3: perform interpolation for various parameters
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

// helper function
fun int randWalk()
{
    // next pitch (random walk the scale)
    Math.random2(-1,1) + scalepoint => scalepoint;
    if( scalepoint < 0 ) 0 => scalepoint;
    if( scalepoint > 15 ) 15 => scalepoint;
    // return
    return scalepoint;
}