// name: honkeytonk-algo1.ck
// desc: ALL NEW! HonkyTonk 4op FM (TX81Z Algorithm 1)
//       This Algorithm can also do PumpOrgn, NylonGuit, others
//       See nylon-guitar-algo1.ck for how to do this
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

HnkyTonk h[8];
Pan2 p[8];
Chorus c[2] => NRev r[2] => dac;
0.5 => r[0].gain;  0.5 => r[1].gain;
0.07 => c[0].mix;
0.07 => c[1].mix;
0.04 => r[0].mix;
0.04 => r[1].mix;

for (int i; i < 8; i++) {
    h[i] => p[i] => c;
    -0.4 + 0.2*i => p[i].pan;
    Math.random2f(6.95,7.05) => h[i].lfoSpeed;
    0.04 => h[i].lfoDepth;
}

Std.mtof(48-12) => h[0].freq;
Std.mtof(53-12) => h[1].freq;
Std.mtof(65-12) => h[2].freq;
Std.mtof(69-12) => h[3].freq;
Std.mtof(72-12) => h[4].freq;

[69,72,74,76,77,81] @=> int melody[];

// start with bass line
bassLine();
// time loop
while( true )
{
    // play melody concurrently over...
    spork ~ doMelody();
    // ... the bass line
    bassLine();
}

fun void doMelody()
{
    while( true )
    {
        Std.mtof(melody[Math.random2(0,5)]) => h[5].freq;
        1 => h[5].noteOn;
        if (maybe*maybe) {
            Std.mtof(melody[Math.random2(0,5)]) => h[6].freq;
            1 => h[6].noteOn;
        }
        Math.random2(1,3)*second/8 => now;
    }
}

fun void bassLine()
{
    for( int i; i < 4; i++ )
    {
        1 => h[1].noteOn;
        second/2 => now;
        1 => h[2].noteOn;
        1 => h[3].noteOn;
        1 => h[4].noteOn;
        second/2 => now;
        if (maybe*maybe) 
            Std.mtof(48) => h[0].freq;
        else
            Std.mtof(48-12) => h[0].freq;
        1 => h[0].noteOn;
        second/2 => now;
        1 => h[2].noteOn;
        1 => h[3].noteOn;
        1 => h[4].noteOn;
        second/2 => now;
    }
}
