//  Listing 9.5 Redefining BPM's variables as static

public class BPM
{
    // global variables  // (1) Declaring variables as static makes
                         // them global to all instances of BPM
    static dur quarterNote, eighthNote, sixteenthNote, thirtysecondNote;

    fun void tempo(float beat) {
        // beat argument is BPM, example 120 beats per minute
        60.0/(beat) => float SPB; // seconds per beat
        SPB :: second => quarterNote;
        quarterNote*0.5 => eighthNote;
        eighthNote*0.5 => sixteenthNote;
        quarterNote*0.5 => thirtysecondNote;
    }
}
