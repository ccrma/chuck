//---------------------------------------------------------------------
// name: thx.ck
// desc: THX Deep Note emulator
//       (original Deep Note by James Andy Moorer)
//
// authors: 
// Perry R. Cook (Jan 8, 2007) -- original ChucK version
// Ge Wang -- modified final chord: D1,D1,D2,D3,D4,A4,D5,A5,D6,A6
//         -- added detune (original Deep Note root between D and Eb)
//         -- time-driven loops (was counter-driven loops)
//---------------------------------------------------------------------

// 30 target frequencies, corresponding to pitches in a big chord:
// D1,  D1,   D2,   D3,    D4,    A4,    D5,    A5,  D6,   A6
[ 36.7, 36.7, 73.4, 146.8, 293.7, 440.0, 587.3, 880.0, 1174.7, 1760.0,
  36.7, 36.7, 73.4, 146.8, 293.7, 440.0, 587.3, 880.0, 1174.7, 1760.15,
  36.7, 36.7, 73.4, 146.8, 293.7, 440.0, 587.3, 880.0, 1174.7, 1759.85 
] @=> float targets[];
// detune mutiplier (original Deep Note between D and Eb)
Math.mtof(26.3) / Math.mtof(26) => float detune;

// initial frequencies
float initials[30];

// parameters (play with these to control timing)
3.0::second => dur initialHold; // initial steady segment
5.5::second => dur sweepTime; // duration over which to change freq
3.5::second => dur targetHold; // duration to hold target chord
2.0::second => dur decayTime; // duration for reverb tail to decay to 0

// sound objects
SawOsc saw[30]; // sawtooth waveform (30 of them)
Gain gainL[30]; // left gain (volume)
Gain gainR[30]; // right gain (volume)
// connect stereo reverberators to output
NRev reverbL => dac.left;
NRev reverbR => dac.right;
// set the amount of reverb
0.075 => reverbL.mix => reverbR.mix;

// for each sawtooth: connect, compute frequency trajectory
for( 0 => int i; i < 30; i++ )
{
    // connect sound objects (left channel)
    saw[i] => gainL[i] => reverbL;
    // connect sound objects (right channel)
    saw[i] => gainR[i] => reverbR;
    // randomize initial frequencies
    Math.random2f( 200.0, 400.0 ) => initials[i] => saw[i].freq;
    // initial gain for each sawtooth generator
    0.1 => saw[i].gain;
    // randomize gain (volume)
    Math.random2f( 0.0, 1.0 ) => gainL[i].gain;
    // right.gain is 1-left.gain -- effectively panning in stereo
    1.0 - gainL[i].gain() => gainR[i].gain;
}

// hold steady cluster (initial chaotic random frequencies)
now + initialHold => time end;
// fade in from silence
while( now < end )
{
    // percentage (should go from 0 to 1)
    1 - (end-now) / initialHold => float progress;
    // for each sawtooth
    for( 0 => int i; i < 30; i++ ) {
        // set gradually decaying values to volume
        0.1 * Math.pow(progress,3) => saw[i].gain;
    }
    // advance time
    10::ms => now;
}

// when to stop
now + sweepTime => end;
// sweep freqs towards target freqs
while( now < end )
{
    // percentage (should go from 0 to 1)
    1 - (end-now)/sweepTime => float progress;
    // for each sawtooth
    for( 0 => int i; i < 30; i++ ) {
        // update frequency by delta, towards target
        initials[i] + (targets[i]*detune-initials[i])*progress => saw[i].freq;
    }
    // advance time
    10::ms => now;
}

// at this point: reached target freqs; briefly hold
targetHold => now;

// when to stop
now + decayTime => end;
// chord decay (fade to silence)
while( now < end )
{
    // percentage (should go from 1 to 0)
    (end-now) / decayTime => float progress;
    // for each sawtooth
    for( 0 => int i; i < 30; i++ ) {
        // set gradually decaying values to volume
        0.1 * progress => saw[i].gain;
    }
    // advance time
    10::ms => now;
}

// wait for reverb tail before ending
2::second => now;
