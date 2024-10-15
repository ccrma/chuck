// name: JacoBass-algo1.ck
// desc: ALL NEW for 2024! JacoBass 4op FM (TX81Z Algorithm 1)
//       This Algorithm can also do many other classic patches
//       See honkeytonk-algo1.ck, nylon-guitar-algo1.ck, etc.
//    for how to do this
//
// author: Perry R. Cook
// date: June 2024, for Hackathon 2024
//       needs chuck 1.4.1.0 or above

// to learn more about FM Algorithm 1, uncomment this:
// HnkyTonk.help();

public class JacoBass extends Chugraph {
    // use Algorithm 1
    HnkyTonk b => Pan2 p => Chorus c => NRev r => outlet;
    0.04 => r.mix;
    
    [1,1,1,7] @=> int waveForms[];
    // [99,80,81,67] @=> int opGains[]; // from the actual patch, too bright
    [99,80,81,37] @=> int opGains[];
    [0.5,0.48,0.52,1.0] @=> float ratios[];
    // [17,16,21,7] @=> int attacks[]; // from the actual patch
    [17,21,21,7] @=> int attacks[]; // I like these better
    [8,11,11,2] @=> int decays[];
    [11,12,11,15] @=> int sustains[];
    [8,8,8,8] @=> int releases[];
    
    for (int op; op < 4; op++) {
        // b.setOpWave(op,waveForms[op]);  // sine waves all
        b.opGain(op,b.getFMTableGain(opGains[op]));
        b.opADSR(op,	b.getFMTableTime(attacks[op]),
                        b.getFMTableTime(decays[op]),
                        b.getFMTableSusLevel(sustains[op]),
                        b.getFMTableTime(releases[op]));
        b.opRatio(op,ratios[op]);    
        b.lfoDepth(0.1);
    }

    fun void freq(float f) {
        f => b.freq;
    }

    fun void noteOn(int n) {
        n => b.noteOn;
    }

    fun void noteOff(int n) {
        n => b.noteOff;
    }
}

JacoBass b => NRev rev => dac; // solo bass
0.03 => rev.mix;
Noise n => ADSR e => ResonZ rez => Pan2 p => rev; // "hi hat"
0.4 => p.pan;
Impulse imp => ResonZ rez2 => rev; // "kick drum"
9000 => rez.freq; 12 => rez.Q; 3.0 => rez.gain; // hi hat
100 => rez2.freq; 8 => rez2.Q;  // kick drum
.5 => rev.gain; // a volume knob

0.12::second => dur S;
0 => int beat;
0 => int meas; // 0 = flag to start,  > 0 = measure#
1 => int notDone;

Std.mtof(48) => b.freq; // a couple of solo bass notes
1 => b.noteOn;
second/2 => now;
Std.mtof(64) => b.freq; // to start things off
//1 => b.noteOn;
1.3*second => now;

Std.mtof(60) => b.freq;
// 1 => b.noteOn; 0.12::second;
// 1 => b.noteOn; 3*0.12::second; 

while( notDone ) {
    // do drums and keep track of song position
    spork ~ teenTown();
    for (1 => int meas; meas < 33; meas++)
    {
        300 => imp.next; // kick drum
        (0.001,0.12,0.0,0.001) => e.set; // hi hat closed
        1 => e.keyOn;
        S => now;
        if (maybe) 1 => e.keyOn;
        S => now;
        (0.001,0.24,0.0,0.001) => e.set; // hi hat open
        1 => e.keyOn;
        S => now;
        if (beat % 4 == 3) 1 => e.keyOn;
        S => now;
        beat++;
    }
}

fun void teenTown() {
    // transcription of the 1st few bars
    [36,48,36,46,36,43,36,39,40,43,45,43,40,38,37,45,
    42,35,35,29,32,33,36,38,36,41,33,43,44,45,48,50,
    51,52,47,45,47,38,38,39,40,43,38,38,35,36,37,45,
    42,36,35,40,37,38,38,36,29,29,36,37,38,40,41,42,
    43,44,45,39] @=> int notes[];
    [0.00, 0.12, 0.23, 0.35, 0.47, 0.59, 0.70, 0.82, 
    0.94, 1.05, 1.17, 1.29, 1.41, 1.52, 1.64, 1.88, 
    1.99, 2.11, 2.34, 3.75, 3.98, 4.10, 4.22, 4.34, 
    4.45, 4.57, 4.69, 4.80, 4.92, 5.04, 5.16, 5.27, 
    5.39, 5.51, 5.62, 5.74, 5.86, 5.98, 6.09, 7.85, 
    7.97, 8.09, 8.20, 8.44, 9.49, 9.61, 9.73, 9.84, 
    9.96, 10.08, 10.20, 10.31, 10.43, 11.60, 11.72, 11.84, 
    11.95, 12.29, 13.69, 13.81, 13.93, 14.16, 14.28, 14.40, 
    14.63, 14.85, 14.87, 14.95] @=> float ons[];

    [S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, 
    S, S, S, 0.23::second, S, S, S, S, S, S, S, S, S, S, S, S, 
    S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, 
    S, S, S, S, 0.70::second, S, S, S, S, S, S, S, S, S, S, S, 
    S, S, S, S] @=> dur durs[];
    
    3*S => now; // rests to begin
    now => time start;
    for (int i; i < notes. cap(); i++) {
        Std.mtof(notes[i]+12) => b.freq;
        1 => b.noteOn;
        <<< Std.ftoa(now/second,2), i, notes[i] >>>;
        durs[i] => now;
        1 => b.noteOff;
        while (ons[i] > ((now-start)/second)) ms => now;
    }
}
