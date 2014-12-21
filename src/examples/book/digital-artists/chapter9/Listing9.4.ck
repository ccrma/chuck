//  Listing 9.4 Public class BPM acting like a global conductor

public class BPM
{
         // (1) Variables declared this way are accessible globally
    dur quarterNote, eighthNote, sixteenthNote, thirtysecondNote;
    fun void tempo(float beat) {   // (2) Function to set tempo (all variables)
        // beat argument is BPM, example 120 beats per minute
               // (3) Does a little math to go from BPM to seconds per beat
        60.0/(beat) => float SPB; // seconds per beat  
        SPB :: second => quarterNote;   // (4) Uses that to set all durations
        quarterNote*0.5 => eighthNote;
        eighthNote*0.5 => sixteenthNote;
        sixteenthNote*0.5 => thirtysecondNote;
    }
}

SinOsc s => dac;
BPM t; // Define t Object of Class BPM // (5) Tests your BPM object by making one
t.tempo(300); // set tempo in BPM  // (6) Sets tempo of new BPM object

400 => int freq;
while (freq < 800)
{
    freq => s.freq;
    t.quarterNote => now;  // (7) Uses .quarterNote method to advance time
    50 +=> freq;
}
