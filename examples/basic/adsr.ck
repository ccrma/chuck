// name: adsr.ck
// desc: ADSR envelope example
//     (A)ttack: duration of initial attack
//     (D)ecay: duration of decay
//     (S)ustain: the level of the sustain
//     (R)elease): duration the release
//
// for more info on envelopes and ADSRs see:
//     https://en.wikipedia.org/wiki/Envelope_(music)
//     https://en.wikipedia.org/wiki/Envelope_(music)#ADSR

// our patch
SinOsc s => ADSR e => dac;

// set A, D, S, and R
e.set( 10::ms, 8::ms, .5, 500::ms );
// (note: A, D, R are durations; S is a number between 0 and 1)

// set gain
.5 => s.gain;

// infinite time-loop
while( true )
{
    // choose freq
    Math.random2( 32, 96 ) => Std.mtof => s.freq;

    // key on: begin ATTACK
    // (note: ATTACK automatically transitions to DECAY;
    //        DECAY automatically transitions to SUSTAIN)
    e.keyOn();
    // advance time by 500 ms
    // (note: this is the duration from the
    //        beginning of ATTACK to the end of SUSTAIN)
    500::ms => now;
    // key off; start RELEASE
    e.keyOff();
    // allow the RELEASE to ramp down to 0
    e.releaseTime() => now;

    // advance time by 300 ms (duration until the next sound)
    300::ms => now;
}
