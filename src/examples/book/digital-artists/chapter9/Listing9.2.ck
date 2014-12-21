// Listing 9.2 Simple resonant pop class wraps UGens and functions for accessing them

// Simple example of a resonant pop sound-making class
public class Simple {                  // (1) Declares a new class called Simple
    // our Simple instrument patch
    Impulse imp => ResonZ filt => dac; // (2) Impulse through resonant filter to dac
    // some default settings
    100.0 => filt.Q => filt.gain;
    1000.0 => filt.freq;

    // set freq as we would any instrument
    fun void freq(float freq)          // (3) Frequency setting method
    {
        freq => filt.freq;
    }

    // method to allow setting Q
    fun void setQ(float Q)             // (4) Resonance (Q) setting method
    {
        Q => filt.Q;
    }

    // method to allow setting gain
    fun void setGain(float gain)       // (5) Gain setting method
    {
        filt.Q() * gain => imp.gain;
    }

    // play a note by firing impulse
    fun void noteOn(float volume)      // (6) noteOn sets impulse to make a click
    {
        volume => imp.next;
    }
}
// Make an instance of (declare) one of our Simples
Simple s;                              // (7) Declares a new Simple instrument

while (1) {                            // (8) Infinite loop
    // random frequency
    Std.rand2f(1100.0,1200.0) => s.freq;  // (9) Random pitch frequency for pop

    // play a note and wait a bit
    1 => s.noteOn;                     // (10) Triggers a simple pop sound
    0.1 :: second => now;
}

