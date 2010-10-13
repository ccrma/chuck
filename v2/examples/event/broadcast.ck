// broadcast.ck : event broadcast

// delcare event e
Event e;

// base function for shred
fun int hi( Event e )
{
    // wait on event
    e => now;

    // print stuff out
    <<<"success">>>;
}

// spork 4 shreds
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );

// yield current shred (without advancing time)
// ... allow other shreds to start running ...
// (alternative: advance time to allow all shreds to start running)
me.yield();

// signal all shreds waiting on event e
e.broadcast();

// advance time since this parent shred will free all child shreds on exit
// (alternative: me.yield())
1::ms => now;
