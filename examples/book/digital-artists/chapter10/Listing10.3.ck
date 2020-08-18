// Listing 10.3 Simple event signaling

// Declare an event we will use for signaling
Event evnt;                  // (1) Declare an event object.

// function that waits on an event
fun void foo( Event anEvent)
{                            // (2) Declare a function to wait on any event.
    Impulse imp => dac;      // (3) Sonify the function with a click.
    while( true )            // (4) Infinite loop.
    {
        // wait
        anEvent => now;      // (5) Wait on event...

        // action            // (6) when event is sent, 
        <<< "Hey!!!", now / second >>>;  //  print out...
        5 => imp.next;       // (7) and make click.
    }
}

// spork a foo       
spork ~ foo( evnt );  // (8) To test, spork your event-waiting function.

// then signal the event forever in a loop
while( true )                // (9) Infinite loop...
{
    // fire the next signal
    evnt.signal();           // (10) ...to signal the event...
    // advance time
    1::second => now;        // (11) ...every second.
}
