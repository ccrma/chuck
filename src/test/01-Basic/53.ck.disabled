// 53.ck : one event broadcast

// the event
Event e;

// handler
fun int hi( Event e, string id )
{
    while( true )
    {
        // wait on event
        e => now;
        <<<id>>>;
    }
}

// spork
spork ~ hi( e, "yo" );
spork ~ hi( e, "hi" );
spork ~ hi( e, "hello" );
spork ~ hi( e, "hey come back here!" );

// infinite time loop
while( true )
{
    // advance time
    1::second => now;
    // broadcast all waiting shreds
    e.broadcast();
}
