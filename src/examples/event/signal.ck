// signal.ck : signaling events

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

// yield to give the shred a chance to run (alternative: advance time)
me.yield();

// signal e
e.signal();

// advance time to let the other shred run
// (the parent shred will free the child shred)
// (alternative: me.yield())
1::samp => now;
