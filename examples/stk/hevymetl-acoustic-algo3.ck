// name: hevymetl-acoustic-algo3.ck
// desc: how to turn HnkyTonk (Algorithm 3) into an acoustic guitar!!
//       now you can transcribe almost directly from a TX81 Patch!!!
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

HnkyTonk g; // instance for shorthand, you'll see...
HnkyTonk guit[6];
NRev r => dac;  
0.25 => r.gain; 0.07 => r.mix;

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
        guit[i].lfoDepth( 0.0 );
    }
}

[38,45,50,55,59,64] @=> int DTuning[];
[38,45,50,57,62,66] @=> int DMaj[];
[40,47,52,55,59,64] @=> int Emi[];

slowStrum(DTuning, 0.8);
second * 1 => now;
allOff();
second/2 => now;

0.2::second => dur E;
for( int i; i < 4; i++ )
{
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
        Math.random2f(0.005,0.02)::second => now;
    }
}

fun void fastUp( int chord[], float vel) {
    for (int i; i < 6; i++) {
        Std.mtof(chord[5-i]) => guit[5-i].freq;
        vel => guit[5-i].noteOn;
        // Math.random2f(0.001,0.005)::second => now;
    }
}

