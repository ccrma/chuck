// 52.ck : one event, many shreds

// the event
Event e;

// event to say hi
fun int hi( Event e, string id )
{
    while( true )
    {
        // wait on event
        e => now;
        // print the id
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
    // signal one waiting shred
    e.signal();
}
