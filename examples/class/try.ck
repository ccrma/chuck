// uses the Dinky class
// (run dinky.ck before running this, for now...)
//
//     > chuck dinky try
//
// NOTE: in a future version of chuck...
//       1. we will have better dependency/include system
//       2. we can extend Dinky from UGen, so we don't have 
//          to use a 'connect( UGen )' function in Dinky
// NOTE: the above (#2) is now possible using Chugraphs!
//       check out: extend/chugraph.ck

// instantiate a Dinky (not connected yet)
Dinky imp;

// connect the rest of the patch
Gain g => NRev r => Echo e => Echo e2 => dac;
// direct/dry
g => dac;
e => dac;

// set up delay, gain, and mix
1500::ms => e.max => e.delay;
3000::ms => e2.max => e2.delay;
1 => g.gain;
.5 => e.gain;
.25 => e2.gain;
.1 => r.mix;

// connect the Dinky
// (in a future version of chuck, Dinky can be defined as an UGen)
imp.connect( g );
// set the radius (should never be more than 1)
imp.radius( .999 );

// an array (our scale)
[ 0, 2, 4, 7, 9, 11 ] @=> int hi[];

// infinite time-loop
while( true )
{
    // trigger
    45 + Math.random2(0,3) * 12 + 
         hi[Math.random2(0,hi.size()-1)] => imp.t;
    // let time pass
    195::ms => now;
    // close the envelope
    imp.c();
    // let a bit more time pass
    5::ms => now;
}
