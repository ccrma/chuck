// name: frenchrn-algo2.ck
// desc: FM 4 Operator (TX81Z Algorithm 2) French Horn Demo
//
// author: Perry R. Cook
// date: June 2021, for REPAIRATHON 2021
//       needs chuck 1.4.1.0 or above

// patch
FrencHrn f => NRev r => dac;
// turn down the volume a bit
.5 => r.gain;
// reverb mix
0.07 => r.mix;

// to learn more about FrencHrn, uncomment this:
// f.apropos();

36 => Std.mtof => f.freq; // Play an unually low note
1 => f.noteOn;  second => now;
1 => f.noteOff;  second => now;

// Test the controllers
<<< f.controlOne(), f.controlTwo() >>>;
arpeg();

1.0 => f.controlOne;  1.0 => f.controlTwo;
<<< f.controlOne(), f.controlTwo() >>>;
arpeg();

0.0 => f.controlTwo;
<<< f.controlOne(), f.controlTwo() >>>;
arpeg();

0.0 => f.controlOne;  1.0 => f.controlTwo;
<<< f.controlOne(), f.controlTwo() >>>;
arpeg();

//  Now play a proper French Horn solo
0.5 => f.controlOne; 0.8 => f.controlTwo;

// PBDSL:  Perry's Bizarre Demo Score Language //
// Integer part is MIDI note#
// Fractional part is duration
// If only fractional, then it's a rest duration
// Negative cause slur or hold (no notOn rearticulation)
// if MIDI note illegal (>127) then it's vibrato 
[60.2,0.1,65.2,0.05,67.2,0.05,68.7,-69.3,0.1,
 60.2,0.05,65.25,67.25,68.7,-69.3,0.1,
 60.2,65.2,67.2,68.4,69.4,71.25,72.2,74.2,72.2,69.2,67.2,65.3,60.3,57.3,53.4,0.2,48.4,0.4,41.9,600.0,-41.9,1] @=> float notes[];

0.0 => float lastNote;
for (int i; i < notes.cap()-1; i++) {
    Std.abs(Std.ftoi(notes[i])) => int note;
    if (Std.fabs(notes[i]) < 1.0) {
        Std.fabs(notes[i])::second => now;
    }
    else if (notes[i] > 128.0) { // illegal note# = vibrato amt * 10000
        notes[i]/10000.0 => f.lfoDepth;
        <<< "Vibrato!!", f.lfoDepth() >>>;
    }
    else {
        Std.mtof(note) => f.freq;
        if (notes[i] > 0.0) {
            <<< "New Note:", note >>>;
            1 => f.noteOn;
        }
        0.8*(Std.fabs(notes[i])-note)::second => now;
        if (notes[i+1] < 128 & notes[i+1] > 0.0) {
            1 => f.noteOff;
        }
        else <<< "Slur!", "or Hold" >>> ;
        0.2*(Std.fabs(notes[i])-note)::second => now;
    }
}

// let ring...
2*second => now;

fun void arpeg() {
    Std.mtof(48) => f.freq;
    1 => f.noteOn; second => now; 1 => f.noteOff; 0.2*second => now;
    Std.mtof(52) => f.freq;
    1 => f.noteOn; second => now; 1 => f.noteOff; 0.2*second => now;
    Std.mtof(55) => f.freq;
    1 => f.noteOn; second => now; 1 => f.noteOff; 0.2*second => now;
    Std.mtof(60) => f.freq;
    1 => f.noteOn; second => now; 1 => f.noteOff; 0.2*second => now;
}
