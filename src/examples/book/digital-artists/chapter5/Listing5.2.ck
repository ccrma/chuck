// Listing 5.2 Testing the addOctave function with sound

// Define addOctave
fun int addOctave( int note )
{
    int result;
    note + 12 => result;
    return result;
}

// Let's use addOctave for music
SinOsc s => dac;         // (1) Oscillator so you can hear addOctave function
60 => int myNote;        // (2) Initial note

Std.mtof(myNote) => s.freq;     // (3) Plays initial note
second => now;

myNote => addOctave => Std.mtof => s.freq;  // (4) Plays one octave up
second => now;
