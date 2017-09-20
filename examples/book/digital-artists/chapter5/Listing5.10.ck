// Listing 5.10 Void functions on global variables, for scalar musical fun

// global variables
Mandolin mand => dac; // (1) Makes and connects a Mandolin instrument UGen.
60 => int note;       // (2) Global note variable 

// functions
fun void noteUp()     // (3) noteUp function definition
{
    1 +=> note; // note half-step up, (4) Adds 1 to global note variable
    <<< note >>>;                  // (5) Prints it out
    play(); // call play function // (6) Plays it
}

fun void noteDown()               // (7) noteDown function
{                       // (8) Subtracts 1 from global note variable
    1 -=> note; // note half-step down 
    <<< note >>>; // print new note value
    play(); // call play function
}

// play global note on global mand UG
fun void play()                 // (9) Defines play function
{
    Std.mtof(note) => mand.freq; // (10) Sets global Mandolin frequency using global note
    1 => mand.noteOn;            // (11) Plays note on Mandolin
    second => now;               // (12) Hangs out for a second before returning to main loop
}

<<< "This code makes no sound.", "Run Listing 5.11 to hear something" >>>;
