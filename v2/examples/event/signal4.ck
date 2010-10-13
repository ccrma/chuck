// signal-4.ck : one event signals 4 shreds

// our event
Event e;

// the base function of our shreds
fun int hi( Event e, string id )
{
    // time/event loop
    while( true )
    {
        // wait on event
        e => now;
        <<<id>>>;
    }
}

// spork our shreds with different strings
spork ~ hi( e, "yo" );
spork ~ hi( e, "hi" );
spork ~ hi( e, "hello" );
spork ~ hi( e, "hey come back here!" );

// may want to yield to give shreds chance to start
me.yield();

// time loop
while( true )
{
    // signal one shred
    e.signal();
    // advance time by 1 second (alternative: me.yield())
    1::second => now;
}
