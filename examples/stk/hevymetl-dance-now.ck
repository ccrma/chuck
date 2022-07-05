//-----------------------------------------------------------------------------
// name: hevymetl-dance-now.ck
// desc: riff from "Everybody Dance Now"
//       created using FM Synthesis, for "Everybody SLOrk Now!"
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2022
//
// original: inspired by "Gonna Make You Sweat (Everybody Dance Now)"
//           C+C Music Factory
//
// code notes:
//   * OFFSET: pitch of third note in power chord (try 0, -12, 12)
//   * DO_ECHO: set to 0 for clean chords; 1 for feedback echo
//   * USE_ENV: controls the attack; set to 0 for hevymetl attack
//-----------------------------------------------------------------------------
// note: want to learn more about HevyMetl? uncomment following line:
// HevyMetl.help();
//-----------------------------------------------------------------------------

// quarter note duration
0.2615::second => dur R;
// detune ("Everybody Dance Now" in Bb + somewhat sharper)
.3 => float TUNE;
// pitch of third note in power chord (try 0, -12, 12)
0  => int OFFSET;
// controls the attack; set to 0 for hevymetl attack
1 => int USE_ENV;
// set to 0 for clean chords; 1 for feedback echo
1 => int DO_ECHO;

// patch
HevyMetl h[3];
// high pass (for echoes)
HPF hpf[3];
// reverb
NRev r => dac; .5 => dac.gain;
// reverb mix
0.0 => r.mix;
// feedback delay
Delay d => r;
d => Gain feedback => d;
// delay
R => d.max => d.delay;
// delay gain
0.4 => d.gain;
// feedback
0.15 => feedback.gain;

// FM operator envelope indices
[31,31,31,31] @=> int attacks[]; // [18,14,15,15] from patch
[31,31,31,31] @=> int decays[];  // [31,31,26,31] from patch
[15,15,15,10] @=> int sustains[]; // [15,15,13,15] from patch
[31,31,31,31] @=> int releases[]; // [8,8,8,8] from patch

// connect
for( int i; i < 3; i++ )
{
    h[i] => r;
    // set delay
    h[i] => hpf[i] => d;
    // set high pass
    600 => hpf[i].freq;
    
    // LFO depth
    0.0 => h[i].lfoDepth;
    
    if( USE_ENV)
    {
        // ops
        for( 0=>int op; op < 4; op++ )
        {
            h[i].opADSR( op,
            h[i].getFMTableTime(attacks[op]),
            h[i].getFMTableTime(decays[op]),
            h[i].getFMTableSusLevel(sustains[op]),
            h[i].getFMTableTime(releases[op]) );
        }
    }
}

// time loop
while( true )
{
    // echo gain
    DO_ECHO * .5 => d.gain;
    
    // do it
    playChord( 46, 53, 46+OFFSET, .8, R );
    2*R => now;
    playChord( 46, 53, 46+OFFSET, .8, R );
    1*R => now;
    playChord( 41, 48, 41+OFFSET, 1.0, R*.8 ); .2*R => now;
    R/2+0*R => now;
    playChord( 44, 51, 44+OFFSET, .9, R*.8 ); .2*R => now;
    R/2+0*R => now;
}

fun void playChord( int a, int b, int c, float vel, dur D )
{
    // set the pitches
    Std.mtof(a+TUNE) => h[0].freq;
    Std.mtof(b+TUNE) => h[1].freq;
    Std.mtof(c+TUNE) => h[2].freq;
    
    // note on
    for( 0 => int i; i < 3; i++ )
    { vel => h[i].noteOn; }
    // sound
    0.85*(D) => now;
    
    // note off
    for( 0 => int i; i < 3; i++ )
    { 1 => h[i].noteOff; }
    // let ring
    0.15*(D) => now;
}
