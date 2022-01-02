//--------------------------------------------------------------------
// name: chant.ck
// desc: chant synthesizer; demonstrates multi-shredded concurrency,
//       variable rates, source-filter model, and interpolation.
//
// This is a classic source-filter model for rudimentary singing
// synthesis: an impulse train (the "source", crudely modeling
// opening/closing of the glottis in the vocal tract) going through
// a bank of three formant filters (roughly modeling the filtering
// by the vocal cavity to induce the perception of different vowels).
//
// This example demonstrates an elegant way to implement the above
// in ChucK, by breaking up the tasks into three concurrent shreds:
//   1. a main shred selects the next target pitch and formants
//   2. doImpulse() generates the impulse train; using ChucK's
//      strongly-timed mechanisms to modulate the impulse train
//      period to create vibrato
//   3. doInterpolation() interpolates the period and formants,
//      to smoothly glide from note to note, vowel to vowel
//
// author: Perry R. Cook (2006)
//         modified by Rebecca Fiebrink and Ge Wang (2007, 2021)
//         published in ChucK examples 2021
//--------------------------------------------------------------------

// synthesis patch
Impulse i => TwoZero t => TwoZero t2 => OnePole p;
// formant filters
p => TwoPole f1 => Gain g;
p => TwoPole f2 => g;
p => TwoPole f3 => g;
// reverbs
g => JCRev r => dac;
g => JCRev rL => dac.left;
g => JCRev rR => dac.right;
// delays
g => Delay d1 => Gain g1 => r;
g => Delay d2 => Gain g2 => rL;
g => Delay d3 => Gain g3 => rR;
// connect gains to delays
g1 => d1; g2 => d2; g3 => d3;

// source gain (amplitude of the impulse train)
0.25 => float sourceGain;

// set filter coefficients
1.0 => t.b0;  0.0 => t.b1; -1.0 => t.b2;
1.0 => t2.b0; 0.0 => t2.b1; 1.0 => t2.b2;
// set gains
0.1 => g1.gain;	0.1 => g2.gain;	0.1 => g3.gain;
// set reverb mix
0.025 => r.mix;
// set delay max and length
1.5 :: second => d1.max;
2.0 :: second => d2.max;
2.8 :: second => d3.max;
1.41459 :: second => d1.delay;
1.97511 :: second => d2.delay;
2.71793 :: second => d3.delay;

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

// leaky integrator
0.99 => p.pole;
1.0 => p.gain;

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
float theFreq;

// spork two concurrent child shreds...
spork ~ doImpulse(); // generate voice source
spork ~ doInterpolation( 10::ms ); // interpolate pitch and formants

// main shred loop
while( true )
{
    // determine new formant targets
    Math.random2f( 230.0, 660.0 ) => target_f1freq;
    Math.random2f( 800.0, 2300.0 ) => target_f2freq;
    Math.random2f( 1700.0, 3000.0 ) => target_f3freq;

    // next pitch (random walk the scale)
    Math.random2(-1,1) + scalepoint => scalepoint;
    if( scalepoint < 0 ) 0 => scalepoint;
    if( scalepoint > 15 ) 15 => scalepoint;
    // compute the frequency
    32 + scale[scalepoint] => Std.mtof => theFreq;
    // print things for fun
    <<< names[scalepoint], theFreq >>>;
    // calculate corresponding target period
    1.0 / theFreq  => targetPeriod;

    // wait until next note
    Math.random2f( 0.2, 0.9 )::second => now;
}

// entry point for shred: generate source impulse train
fun void doImpulse()
{
    // infinite time-loop
    while( true )
    {
        // fire impulse
        sourceGain => i.next;
        // phase variable
        modphase + period => modphase;
        // vibrato depth
        .0001 => vibratoDepth;
        // modulate wait time until next impulse: vibrato
        (period + vibratoDepth*Math.sin(2*pi*modphase*6.0))::second => now;
    }
}

// entry point for shred: interpolate period and formant frequencies
fun void doInterpolation( dur T )
{
    // percentage progress per time slice
    0.10 => float slew;
    // infinite time-loop
    while( true )
    {
        // go towards target period (pitch)
        (targetPeriod - period) * slew + period => period;
        // go towards targat formant frequencies
        (target_f1freq - f1freq) * slew + f1freq => f1freq => f1.freq;
        (target_f2freq - f2freq) * slew + f2freq => f2freq => f2.freq;
        (target_f3freq - f3freq) * slew + f3freq => f3freq => f3.freq;

        // interpolation rate
        T => now;
    }
}
