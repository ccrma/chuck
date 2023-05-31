// PowerADSR is a power based ADSR envelope that
// allows separate power curves for each envelope phase.
// In general, curves under 1.0 are sharp, while curves over 1.0 are soft.

// ~ linearEnvelope^curveValue ~

// ~options
//
// set (duration a, duration d, float s, duration r), default (0.0, 0.0, 1.0, 0.0)
//   sets duration for attack, decay, and release phase; sets sustain level
//
// attack or attackTime (duration a), default 1.0::second
//   sets length for the attack phase
//
// decay or decayTime (duration d), default 1.0::second
//   sets length for the decay phase
//
// release or releasetTime (duration r), default 1:0::second
//
// sustainLevel (float s), default 0.5
//   sets level for the sustain phase
//
// setCurves (float ac, float dc, float rc), default (1.0, 1.0, 1.0) (all linear)
//   sets power curve for attack, decay, and release phase
//
// attackCurve (float ac), default 1.0 (linear)
//   sets power curve for the attack phase
//
// decayCurve (float dc), default 1.0 (linear)
//   sets power curve for the decay phase
//
// releaseCurve(float rc), default 1.0 (linear)
//   sets power curve for the release phase

Noise nois;
PowerADSR env;

// sound chain
nois => env => dac;

// you can set the envelope on paramter at a time
env.attackTime(1::second);
env.releaseTime(1::second);

// defaults to a 1.0 curve, you have to set
// the curves for the power functionality

// curves under 1.0 are sharper
env.attackCurve(0.5);

// curves over 1.0 are softer
env.releaseCurve(2.0);

env.keyOn();
1::second => now;
env.keyOff();
1::second => now;

// you can also set all the durations and sustain at once
env.set(2::second, 1::second, 0.8, 0.5::second);

// and all the curves can be set at once, without the sustain
env.setCurves(1.75, 1.25, 1.5);

env.keyOn();
3::second => now;
env.keyOff();
1.0::second => now;

env.set(1::second, 1::second, 0.5, 1::second);

// and you can retrieve the state and value of the envelope
<<< "Done\t", env.state(), env.value(), "" >>>;

env.keyOn();
<<< "Attack\t", env.state(), env.value(), "" >>>;
1::second => now;

<<< "Decay\t", env.state(), env.value(), "" >>>;
1::second => now;

<<< "Sustain\t", env.state(), env.value(), "" >>>;
1::second => now;

env.keyOff();
<<< "Release\t", env.state(), env.value(), "" >>>;
1::second => now;

<<< "Done\t", env.state(), env.value(), "" >>>;
