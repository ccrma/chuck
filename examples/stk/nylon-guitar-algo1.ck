// name: nylon-guitar-algo1.ck
// desc: how to turn HnkyTonk (Algorithm 1) into an acoustic guitar!!
//       now you can transcribe almost directly from a TX81 Patch!!!
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

NRev r => dac;  // reverb is output mixer
HnkyTonk g => r; // instance for shorthand (you'll see...) , and solo
HnkyTonk guit[6];
0.1 => r.gain; 0.08 => r.mix;

[0,0,0,0] @=> int waveForms[];
[97,71,77,81] @=> int opGains[];
[1.0,3.0,3.02,8.0] @=> float ratios[];
[31,28,31,31] @=> int attacks[];
[18,6,10,9] @=> int decays[];
[14,1,0,0] @=> int sustains[]; // 15,0,0,0 ???
[8,8,9,6] @=> int releases[];

for( int i; i < 6; i++ )
{
    guit[i] => r;

    for( int op; op < 4; op++ )
    {
        guit[i].opWave( op,1 );  // sine waves all
        guit[i].opGain( op,g.getFMTableGain(opGains[op]) );
        guit[i].opADSR( op,	g.getFMTableTime(attacks[op]),
                            g.getFMTableTime(decays[op]),
                            g.getFMTableSusLevel(sustains[op]),
                            g.getFMTableTime(releases[op]) );
        guit[i].opRatio( op,ratios[op] );    
        g.opWave(op,1); // we do this 6 times, but it's easier
        g.opGain( op,g.getFMTableGain(opGains[op]) );
        g.opADSR( op,	g.getFMTableTime(attacks[op]),
                            g.getFMTableTime(decays[op]),
                            g.getFMTableSusLevel(sustains[op]),
                            g.getFMTableTime(releases[op]) );
        g.opRatio( op,ratios[op] );    
    }
    guit[i].lfoDepth( 0.0 );
}
g.lfoDepth( 0.0 );

[38,45,50,55,59,64] @=> int DTuning[];
[38,45,50,57,62,66] @=> int DMaj[];
[40,47,52,55,59,64] @=> int Emi[];

slowStrum(DTuning, 0.8);
second * 1 => now;
allOff();
second/2 => now;

0.2::second => dur E;

spork ~ solo();

for( int i; i < 2; i++ )
{
    <<< now/second >>>;
    spork ~ fastStrum(Emi,1.0); 2*E => now;
    fastUp(Emi,0.95); E/4 => now; allOff(); 3*E/4 => now; 
    spork ~ fastStrum(Emi,0.97); 2*E => now;
    fastUp(Emi,0.8); E/4 => now; allOff(); 3*E/4 => now; 
    spork ~ fastStrum(Emi,0.95); 2*E => now;
    fastUp(Emi,0.9); E/4 => now; allOff(); 3*E/4 => now; 
    spork ~ fastStrum(Emi,0.93); 2*E => now;
    fastUp(Emi,0.9); E/4 => now; allOff(); 3*E/4 => now; 
    spork ~ fastStrum(DMaj,1.0); E => now; allOff(); E => now;
    spork ~ fastStrum(DMaj,0.9); E => now;
    if (maybe) { allOff(); }
    else { spork ~ fastStrum(DMaj,0.8); }
    E => now;
}

slowStrum(Emi,0.8);
Std.mtof(76) => g.freq;
1 => g.noteOn;
2*second => now;
allOff();
second => now;

fun void allOff() {
    for (int i; i < 6; i++) 1 => guit[i].noteOff;
}

fun void slowStrum( int chord[], float vel) {
    for (int i; i < 6; i++) {
        Std.mtof(chord[i]) => guit[i].freq;
        vel => guit[i].noteOn;
        Math.random2f(0.05,0.15)::second => now;
    }
}

fun void fastStrum( int chord[], float vel) {
    for (int i; i < 6; i++) {
        Std.mtof(chord[i]) => guit[i].freq;
        vel => guit[i].noteOn;
        Math.random2f(0.005,0.03)::second => now;
    }
}

fun void fastUp( int chord[], float vel) {
    for (int i; i < 6; i++) {
        Std.mtof(chord[5-i]) => guit[5-i].freq;
        vel => guit[5-i].noteOn;
        Math.random2f(0.001,0.01)::second => now;
    }
}

fun void solo()  {
    [71, 69, 71] @=> int solo1[];
    [71, 72, 74, 72, 71] @=> int solo2[];
    2*E => now;
    Std.mtof(solo1[0]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo1[1]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo1[2]) => g.freq; 1 => g.noteOn;
    0.8::second => now;
    1 => g.noteOff;
    2::second => now;
    Std.mtof(solo2[0]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo2[1]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo2[2]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo2[3]) => g.freq; 1 => g.noteOn;
    E/2 => now;
    Std.mtof(solo2[4]) => g.freq; 1 => g.noteOn;
    1::second => now;
    1 => g.noteOff;
}
