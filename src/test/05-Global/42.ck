// 42.ck : event broadcast

// delcare event e
global Event e;
int i;

fun int hi( Event e )
{
    // wait on event
    e => now;

    // count
    i++;
}

// spork 4 shreds
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );
spork ~ hi( e );

// advance time to allow all shreds to start running
1::second => now;

// signal all shreds waiting on event e
e.broadcast();

// advance time since this parent shred will free all child shreds on exit
1::ms => now;

if(i == 4)
    <<<"success">>>;
else
    <<<"failure">>>;

