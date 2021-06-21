// name: hevymetl-trumpet-algo3.ck
// desc: demo of how to turn HevyMetl (Algorithm 3) into an FM Trumpet!!
//       now you can transcribe (somewhat) directly from a TX81 Patch!!!
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above


// patch
HevyMetl t => NRev r => dac;
// reverb mix
0.06 => r.mix;

// want to learn more about HevyMetl? uncomment this:
// t.apropos();

// FM
[1,6,6,1] @=> int waveForms[];
[99,85,74,99] @=> int opGains[]; // [93,75,54,81] from patch
[1.0,1.0,2.9,1.0] @=> float ratios[];
[15,14,15,15] @=> int attacks[]; // [18,14,15,15] from patch
[31,20,26,14] @=> int decays[];  // [31,31,26,31] from patch
[15,10,13,15] @=> int sustains[]; // [15,15,13,15] from patch
[10,10,10,10] @=> int releases[]; // [8,8,8,8] from patch

// ops
for( int op; op < 4; op++ )
{
    t.opWave( op, waveForms[op] );  // sine waves all
    t.opGain( op, t.getFMTableGain(opGains[op]) );
    t.opADSR( op, t.getFMTableTime(attacks[op]),
                  t.getFMTableTime(decays[op]),
                  t.getFMTableSusLevel(sustains[op]),
                  t.getFMTableTime(releases[op]) );
    t.opRatio( op, ratios[op] );
}

// t.setOpGain(1,0.0);
t.op4Feedback(0.5);

// our notes
[ 64, 62, 67, 69, 74, 71, 69, 74, 71, 67, 72, 69, 67] @=> int notes[];
[2, 2, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 9] @=> int durs[];

0.005 => t.lfoDepth;

for( 0 => int i; i < notes.cap(); i++ )
{
    Std.mtof( 14 + notes[i] ) => t.freq;
    0.7 => t.noteOn;
    if (i == notes.cap()-1) 0.05 => t.lfoDepth;
    (0.2 * durs[i]) :: second => now;
    1.0 => t.noteOff;
    (0.1 * durs[i]) :: second => now;
}
