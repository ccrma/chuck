// Listing 5.1 Defining and testing a function that adds an octave to any MIDI note number

// A Simple Function example
// Declare our function here

fun int addOctave( int note )    // (1) Function declaration
{
    int result;                  // (2) Result to return
    note + 12 => result;         // (3) Calculates the value to return
    return result;               // (4) Returns it
}

// Main addOctave Test Program, call and print out result

addOctave(60) => int answer;     // (5) Uses the function

<<< answer >>>;                  // (6) Checks the result
