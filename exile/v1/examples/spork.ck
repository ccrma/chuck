// this shows how to dynamically spork shreds...

fun void foo( string s )
{
    while( true )
    {
        s => stdout;
        500::ms => now;
    }
}

spork ~ foo( "you" );
250::ms => now;
spork ~ foo( "me" );


// infinite time loop - children shreds leave with parent shred
while( true )
    1::second => now;
