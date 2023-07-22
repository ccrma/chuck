// 41.ck : event

// declare an event
Event e;

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

// yield() to let the sporked shreds run and wait on event
me.yield();

// signal e
e.signal();

// yield again to let the other shred run (and print success)
me.yield();

// FYI from here freeing the parent shred will free the child shred
