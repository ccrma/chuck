// Listing 9.18 A smarter MyClarinet that Inherits from Clarinet
// A clarinet that understands MIDI note numbers in noteOn!!
public class MyClarinet extends Clarinet  // (1) Subclass of Clarinet UGen.
{
    // here we define one new function
    fun float noteOn(int noteNum, float volume) { // (2) New noteOn function with two arguments.
        // we set frequency using MIDI note Number
        Std.mtof(noteNum) => this.freq;           // (3) Clarinet's built in .freq method... 
        // then call the built-in inherited noteOn
        volume => this.noteOn;                    // (4) ...and uses Clarinet's noteOn to set volume // 
    }
}


// make a new instance of our special clarinet
MyClarinet myclar => dac;

// test our new noteOn function
myclar.noteOn(60, 1.0);                           // (5) Tests it all out.

second => now;
1 => myclar.noteOff;
second => now;

// test our old noteOn function
myclar.noteOn(1.0);        // (6) The old Clarinet noteOn still works too!
second => now;
