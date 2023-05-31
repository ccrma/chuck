//--------------------------------------------------------------------
// name: thx.ck
// desc: emulation of the original THX Deep Note
//       (by Dr. James Andy Moorer)
//
// author: Perry R. Cook (https://www.cs.princeton.edu/~prc/)
//         Ge Wang (https://ccrma.stanford.edu/~ge/)
//
// Perry R. Cook (Jan 8, 2007) -- original ChucK version
// Ge Wang -- modified final chord to align with original Deep Note
//         -- added beginning "chaotic" section
//         -- time-driven loops (was counter-driven loops)
//
// THX resources from /Artful Design/:
//     https://artful.design/thx/
//
// Andy Moorer's personal account
//.    http://www.jamminpower.org/THX.html
//
// -------------------------------------------------------------------
// Ge, Fall 2017: from Andy Mooorer:
// -------------------------------------------------------------------
// OK - I dug out the original program. Here are the frequency
// bounds of the cluster:
// #define LOCLUST 40.0
// #define HICLUST 350.0
//
// I had started with them in a more narrow range, but then 
// widened it. With the randomness, they never get anywhere 
// near the limits.  And here are the pitches in the final
// chord for all 30 voices:
//
// double freqs[NOSCS], initialfreqs[NOSCS],
// finalfreqs[] =
// { 1800.0, 1800.0, 1800.0,
//   1500.0, 1500.0,
//   1200.0, 1200.0, 1200.0, 1200.0,
//   900.0, 900.0, 900.0, 900.0,
//   600.0, 600.0, 600.0,
//   300.0, 300.0, 300.0, 300.0,
//   150.0, 150.0, 150.0, 150.0,
//   75.0, 75.0, 75.0
//   37.5, 37.5, 37.5,
// };
//
// Note that in the final chord, they are detuned a bit by 
// injecting a bit of randomness to make sure they don't fuse
// totally. Several people have commented that the chord 
// sounds "bigger" than an equivalent orchestra or organ 
// chord (like the big chord in the Bm fugue which was my
// inspiration). I believe this is because of the just 
// temperament of the chord. Moving the thirds and fifths 
// to equal temperament just doesn't have the same impact. 
//
// Let me know if you have any other questions. I am really 
// excited to see your new book. It looks like great fun!
// -A
//--------------------------------------------------------------------
 
// 30 target frequencies, corresponding to pitches in a big chord:
// D1,  D2, D3,  D4,  D5,  A5,  D6,   F#6,  A6
[ 37.5, 75, 150, 300, 600, 900, 1200, 1500, 1800,
  37.5, 75, 150, 300, 600, 900, 1200, 1500, 1800,
  37.5, 75, 150, 300, 600, 900, 1200,       1800,
            150, 300,      900, 1200  
] @=> float targets[];

// initial frequencies
float initials[30];
// for the initial "wavering" in the steady state
float initialsBase[30];
float randomRates[30];

// parameters (play with these to control timing)
12.5::second => dur initialHold; // initial steady segment
6.0::second => dur sweepTime; // duration over which to change freq
5.5::second => dur targetHold; // duration to hold target chord
6.0::second => dur decayTime; // duration for reverb tail to decay to 0

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
    Math.random2f( 160.0, 360.0 ) => initials[i] 
               => initialsBase[i] => saw[i].freq;
    // initial gain for each sawtooth generator
    0.1 => saw[i].gain;
    // randomize gain (volume)
    Math.random2f( 0.0, 1.0 ) => gainL[i].gain;
    // right.gain is 1-left.gain -- effectively panning in stereo
    1.0 - gainL[i].gain() => gainR[i].gain;
    // rate at which to waver the initial voices
    Math.random2f(.1,1) => randomRates[i];
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
        // waver the voices
        initialsBase[i] + (1.25-progress)*.5*initialsBase[i]*Math.sin(now/second*randomRates[i])
             => initials[i] => saw[i].freq;
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
        initials[i] + (targets[i]-initials[i])*progress => saw[i].freq;
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
5::second => now;
