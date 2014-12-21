// Listing 9.3 Add these overloaded pitch() functions to Simple

// Improved example of a resonant pop sound-making class
public class Simple {
    // our Simple instrument patch
    Impulse imp => ResonZ filt => dac;

    // some default settings
    100.0 => filt.Q => filt.gain;
    1000.0 => filt.freq;

    // set freq as we would any instrument
    fun void freq(float freq)
    {
        freq => filt.freq;
    }

    // method to allow setting Q
    fun void setQ(float Q)
    {
        Q => filt.Q;
    }

    // method to allow setting gain
    fun void setGain(float gain)
    {
        filt.Q() * gain => imp.gain;
    }

    // play a note by firing impulse
    fun void noteOn(float volume)
    {
        volume => imp.next;
    }

    // three ways of setting pitch
    // one by float frequency
    fun float pitch(float freq)        // (1) Sets pitch as a float frequency in Hertz.
    {
        return freq => filt.freq;
    }

    // another to set pitch by MIDI note number
    fun float pitch(int noteNum)       // (2) Sets pitch using an integer MIDI note number.
    {
        return Std.mtof(noteNum) => filt.freq;
    }

    // another way to set pitch by noteName, capital A-G required
    // Needs specific format: C4, D#5, Eb3, As3, Bf6
    fun float pitch(string noteName)   // (3) Sets pitch using a note name (string).
    {
        [21,23,12,14,16,17,19] @=> int notes[]; // Note numbers A thru G
        noteName.charAt(0) - 65 => int base;    // (4 Manipulates ASCII code to come up with array pointer.
        notes[base] => int note;
        0.0 => float freq;
        if (base > -1 && base < 7) {
            if (noteName.charAt(1) == '#') // either '#' symbol
                notes[base] + 1 => note;
            if (noteName.charAt(1) == 's') // or 's' counts for sharp
                notes[base] + 1 => note;
            if (noteName.charAt(1) == 'b') // either 'b' or
                notes[base] - 1 => note;
            if (noteName.charAt(1) == 'f') // 'f' counts for flat
                notes[base] - 1 => note;
        }
        else {
            <<< "Illegal Note Name!!" >>>;
            return 0.0;
        }
        noteName.charAt(noteName.length()-1) - 48 => int oct;
        if (oct > -1 && oct < 10) {
            note + 12*oct => note;     // (5) After figuring out note number, 
            return this.pitch(note);   //     calls MIDI pitch function.

        }
        else {
            <<< "Illegal Octave!!" >>>;
            return 0.0;
        }
    }
}

// Make a Simple, and test new pitch functions
Simple s;

// MIDI note number pitch
s.pitch(60);    // (6) Tests MIDI integer pitch setting function.
1 => s.noteOn;
second => now;

// pitch as float frequency
s.pitch(440.0); // (7) Tests float pitch-setting function.
1 => s.noteOn;
second => now;

// pitch as note name string
s.pitch("G#5"); // (8) Tests string note name setting function.
1 => s.noteOn;
second => now;
