event e;

fun int hi( event e, string id )
{
    while( true )
    {
        // wait on event
        e => now;
        <<<id>>>;
    }
}

spork ~ hi( e, "yo" );
spork ~ hi( e, "hi" );
spork ~ hi( e, "hello" );
spork ~ hi( e, "hey come back here!" );

while( true )
{
    1::second => now;
    e.broadcast();
}
