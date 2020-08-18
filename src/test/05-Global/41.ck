// 41.ck : event

// declare an event
global Event e;

// declare function
fun int hi( Event e )
{
    // wait on event e
    e => now;

    // this only happens AFTER e is signaled
    <<<"success">>>;
}

// spork shred with e
spork ~ hi( e );

// allow time to pass to give the shred a chance to run
1::ms => now;

// signal e
e.signal();

// advance time to let the other shred run
// (the parent shred will free the child shred)
1::samp => now;
