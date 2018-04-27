// 42.ck : custom event broadcast

class myEvent extends Event
{
    int myStorage;
}

// delcare event e
global myEvent e;
5 => e.myStorage;
int i;

fun int hi( myEvent e )
{
    // wait on event
    e => now;

    // count
    e.myStorage +=> i;
    e.myStorage++;
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

if(i == 26)
    <<<"success">>>;
else
    <<<"failure">>>;

