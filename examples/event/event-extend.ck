//---------------------------------------------------------------------
// name: event-extend.ck
// desc: defining and using a custom Event
//---------------------------------------------------------------------

// new event class
class TheEvent extends Event
{
    int value;
}

// the event
TheEvent e;

// handler
fun void hi( TheEvent e )
{
    while( true )
    {
        // wait on event
        e => now;
        // get the data
        <<<e.value>>>;
    }
}

// spork
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );

// yield this shred to give sporked shreds a chance to run
me.yield();

// infinite time loop
while( true )
{
    // set data
    Math.random2( 0, 5 ) => e.value;

    // signal one waiting shred
    e.signal();

    // advance time
    1::second => now;
}
