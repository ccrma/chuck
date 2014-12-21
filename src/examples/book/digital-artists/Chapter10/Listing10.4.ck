// Listing 10.4 Using one shred to signal() multiple other shreds

// Declare an event we will use for signaling
Event evnt;                            // (1) Declares an event.

// function that waits on an event
fun void bar(Event anEvent, string msg, float freq)  // (3) Frequency for pop.
{              // (2) New event-waiting function with extra arguments.
    Impulse imp => ResonZ rez => dac;  // (4) Tuned pop sound.
    50 => rez.Q;                       // (5) High resonance for tuned pop.
    while( true )                      // (6) Infinite loop
    {
        // wait
        anEvent => now;                // (7) Sleeps until event is signaled.
        // action
        <<< msg, freq, now / second >>>; // (8) When event comes, print out.
        freq => rez.freq;              // (3) Frequency for pop.
        50 => imp.next;                // (9) Triggers pop sound.
    }
}

// spork a few bar shreds
spork ~ bar( evnt, "Hi ", 500.0 );     // (10) Spork one event-waiting function
spork ~ bar( evnt, "There ", 700.0 );  // (11) ...and another, with different string and frequency...
spork ~ bar( evnt, "Sport! ", 900.0 ); // (12) ...and yet another, different from the other two.

// then signal the event forever in a loop
while( true )                   // (13) Infinite loop to test sporked functions.
{
    // fire the next signal
    evnt.signal();              // (14) Fire event using signal() method.
    // advance time
    1::second => now;
}
