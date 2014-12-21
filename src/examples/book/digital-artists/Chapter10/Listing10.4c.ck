// Listing 10.4c Using one shred to signal() multiple other shreds

// Declare an event we will use for signaling
Event evnt;

// function that waits on an event
fun void bar(Event anEvent, string msg, float freq)
{
    Impulse imp => ResonZ rez => dac;
    50 => rez.Q;
    while( true )
    {
        // wait
        anEvent => now;
        // action
        <<< msg, freq, now / second >>>;
        freq => rez.freq;
        50 => imp.next;
    }
}

// spork a few bar shreds
spork ~ bar( evnt, "Hi ", 500.0 );
spork ~ bar( evnt, "There ", 700.0 );
spork ~ bar( evnt, "Sport! ", 900.0 );

// then signal the event forever in a loop
while( true )
{
    // fire the next broadcast
    evnt.broadcast();
    // advance time
    1::second => now;
}
