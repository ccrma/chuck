// 143.ck : exiting while child shreds waiting on event

// declare an event
Event e;

// declare function
fun void hi( Event e, int id )
{
    while( true )
    {
        // wait on event e
        e => now;
        // print id
        <<< id, "" >>>;
    }
}

// spork shred with e
spork ~ hi( e, 1 );
spork ~ hi( e, 2 );
spork ~ hi( e, 3 );

// yield() to let the sporked shreds run and wait on event
me.yield();

// signal e
e.broadcast();

// yield() again to give others shreds a chance to receive event trigger
me.yield();

<<< "success" >>>;

// after this the parent shred will free the child shred
// if nothing else gets printed (e.g., some kind of a crash!),
// then call it actual success
