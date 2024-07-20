// name: blit3.ck
// desc: lounge blit
// date: spring 2024; made for svork's concert lounge music

// a duration... 250::ms can bring the vibes too
350::ms => dur T;

// patch
Blit s => LPF lpf => PoleZero dcb => ADSR e => DelayL eicho => JCRev r => dac;
// feedback
eicho => Gain g(.8) => eicho;
// block zero
.99 => dcb.blockZero;
// lower the volume a bit
.65 => s.gain;

// delay settings
T*2/3 => eicho.max => eicho.delay;
// reverb mix
.15 => r.mix;
// lowpass cutoff
8000 => lpf.freq;

// set adsr
e.set( 15::ms, 13::ms, .5, 35::ms );

// intervals array
[ 0, 2, 4, 7, 9, 11 ] @=> int hi[];
// roots array
[ 33, 24 ] @=> int roots[];

// infinite time loop
while( true )
{
    // for each root
    for( int r : roots )
    {
        // repeat with r for some number of times
        repeat( Math.random2(16,32) )
        {
            // frequency
            Std.mtof( r + Math.random2(0,3) * 12 +
                hi[Math.random2(0,hi.size()-1)] ) => s.freq;
            
            // harmonics
            Math.random2( 1, 5 ) => s.harmonics;
            
            // key on
            e.keyOn();
            // advance time
            T-e.releaseTime() => now;
            // key off
            e.keyOff();
            // advance time
            e.releaseTime() => now;
        }
    }
}
