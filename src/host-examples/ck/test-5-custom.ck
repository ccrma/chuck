// in this example, this demonstrates using
// a custom class defined by the C++ host
// (see example-5-custom-api.cpp)
FooFoo foo( 220 );

// testing constructor, should print 220
<<< "FooFoo value from constructor:", foo.getValue() >>>;
// testing static function call, should print 3
<<< "static function test FooFoo.add():", FooFoo.add( 1, 2 ) >>>;

// base duration
300::ms => dur T;

//-------- some audio setup ------------
// (adapted from blit4.ck, or rather blit4 came out of this)
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
    foo.setValue( 55 * Math.pow(2,Math.random2(0,3)) );
    // increment
    foo.getValue() => float inc;
    // print marker
    <<< "--------", "" >>>;

    // repeat
    repeat( Math.random2(3,12) )
    {
        // 5
        <<< "foo's value:", foo.getValue() >>>;
        // set frequency
        foo.getValue() => s.freq;
        // harmonics
        Math.random2( 1, 6 ) => s.harmonics;
        // key on
        e.keyOn();
        // advance time
        T - e.releaseTime() => now;
        // key off
        e.keyOff();
        // advance time
        e.releaseTime() => now;
        // increment
        foo.increment( inc );
    }
}

// let it ring
500::ms => now;
