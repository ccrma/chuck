// 54.ck : creating custom event objects

// new event class
class TheEvent extends Event
{
    int value;
}

// the event
TheEvent e;

// handler
fun int hi( TheEvent e )
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

// infinite time loop
while( true )
{
    // advance time
    1::second => now;

    // set data
    math.rand2( 0, 5 ) => e.value;

    // signal one waiting shred
    e.signal();
}
