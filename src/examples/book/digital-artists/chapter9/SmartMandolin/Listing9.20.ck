// Listing 9.20 Smart mandolin instrument and player class
// Four Mando "strings", plus some smarts
// by Perry R. Cook, March 2013

public class MandoPlayer {     // (1) Public MandoPlayer class definition.
    // make an array of four mandolin strings and connect them up
    Mandolin m[4];             // (2) Contains four Mandolin UGens.
    m[0] => JCRev rev => dac;  // (3) Hooks them all up so you can hear them.
    m[1] => rev;
    m[2] => rev;
    m[3] => rev;
    0.25 => rev.gain;
    0.02 => rev.mix;

    // set all four string frequencies in one function
    fun void freqs(float gStr, float dStr, float aStr, float eStr)
    {                      // (4) Sets all four string frequencies.
        m[0].freq(gStr);
        m[1].freq(aStr);
        m[2].freq(dStr);
        m[3].freq(eStr);
    }

    // set all four string notes in one function
    fun void notes(int gNote, int dNote, int aNote, int eNote) {
        m[0].freq(Std.mtof(gNote));   // (5) Sets all four string note numbers... 
        m[1].freq(Std.mtof(dNote));   // (6) ...using .mtof.
        m[2].freq(Std.mtof(aNote));
        m[3].freq(Std.mtof(eNote));
    }

    // a few named chords to get you started, add your own!!
    fun void chord(string which) {                   // (7) Chords by name.
        if (which == "G") this.notes(55,62,71,79);   // (8) G Chord is G3, D4, B4, G5.
        else if (which == "C") this.notes(55,64,72,79);
        else if (which == "D") this.notes(57,62,69,78);
        else <<< "I don't know this chord: ", which >>>;
    }

    // roll a chord from lowest note to highest at rate
    fun void roll(string chord, dur rate) 
    {                               // (9) Chord roll (arpeggiate) function.
        this.chord(chord);          // (10) Sets chord by string using MandoPlayer .chord function.
        for (0 => int i; i < 4; i++) {
            1 => m[i].noteOn;       // (11) Plays notes one at a time...
            rate => now;            // (12) ...with rate duration between.
        }
    }

    // Archetypical tremolo strumming
    fun void strum(int note, dur howLong) 
    {                                      // (13) Strumming function (tremolo).
        int whichString;
        if (note < 62) 0 => whichString;      // (14) Figures out which string to strum.
        else if (note < 69) 1 => whichString;
        else if (note < 76) 2 => whichString;
        else 3 => whichString;
        Std.mtof(note) => m[whichString].freq; // (15) Sets frequency.
        now + howLong => time stop;            // (16) Time to stop strumming...
        while (now < stop) {                   // (17) ...do it until you get to that time.
            Std.rand2f(0.5,1.0) => m[whichString].noteOn;  // (18) Somewhat random volume.
            Std.rand2f(0.06,0.09) :: second => now;       // (19) Somewhat random time.
        }
    }

    // Damp all strings by amount
    // 0.0 = lots of damping, 1.0 = none
    fun void damp(float amount) {        // (20) Damping function.
        for (0 => int i; i < 4; i++) {
            amount => m[i].stringDamping;
        }
    }

// END the MandoPlayer Class Definition
       // (21) Whew! Finished defining the smart Mandolin player class.
}
