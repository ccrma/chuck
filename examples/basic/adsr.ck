// name: adsr.ck
// desc: an ADSR envelope describes how a sound changes over time.
//       It's often used in music synthesis to control the volume of
//       a sound. ADSR stands for Attack, Decay, Sustain, and Release.
//       Each of these terms describes a different part of the sound's
//       volume over time. In this example, an ADSR is used to shape
//       a continuous sine wave into individual notes.
//
//          /\
//         /  \_____
//        /         \
//       The general shape of a ADSR envelope.
//
//       (A)ttack: This is the initial burst of sound when a note is
//       played. It's how quickly the volume reaches its maximum level.
//
//       (D)ecay: After the attack, the sound level will decrease over
//       time to a lower level. The decay time is how long it takes for
//       the sound to reach the sustain level.
//
//       (S)ustain: This is the steady state of the sound after the
//       initial attack and decay. The sound remains at a constant level
//       as long as the note is held down.
//
//       (R)elease: When the note is released, the sound level will drop
//       back down to zero. The release time is how long it takes for the
//       sound to fade away completely.
//
//       for more info on envelopes and ADSRs see:
//           https://en.wikipedia.org/wiki/Envelope_(music)#ADSR

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
