// name: blit4.ck
// desc: rising fizzing blit
// date: made during ChucK Hackathon Winter 2025, Stanford CCRMA

// base duration
300::ms => dur T;

//-------- audio setup ------------
Blit s => LPF lpf => PoleZero dcb => ADSR e => Gain g => JCRev r => dac;
// feedback
g => DelayL eicho => g; .65 => eicho.gain;
// block zero
.99 => dcb.blockZero;
// lower the volume a bit
.65 => s.gain;
// delay settings
T/2*3 => eicho.max => eicho.delay;
// reverb mix
.15 => r.mix;
// lowpass cutoff
8000 => lpf.freq;
// set adsr
e.set( 15::ms, 13::ms, .5, T/3 );
//---------------------------------

// time loop
while( true )
{
    // set value
    55 * Math.pow(2,Math.random2(0,3)) => float N => float inc;
    // print marker
    <<< "--------", "" >>>;
    // repeat
    repeat( Math.random2(3,12) )
    {
        // set frequency
        N => s.freq;
        // harmonics
        Math.random2( 1, 6 ) => s.harmonics;
        // print
        <<< "frequency:", s.freq() >>>;
        // key on
        e.keyOn();
        // advance time
        T - e.releaseTime() => now;
        // key off
        e.keyOff();
        // advance time
        e.releaseTime() => now;
        // increment
        inc +=> N;
    }
}

// let it ring
500::ms => now;
