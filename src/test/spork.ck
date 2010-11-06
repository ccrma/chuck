// this shows how to dynamically spork shreds...

fun void foo( int a )
{
    while( true )
    {
        <<<a>>>;
        500::ms => now;
    }
}

spork ~ foo( 1 );
250::ms => now;
spork ~ foo( 2 );


// infinite time loop - children shreds leave with parent shred
while( true )
    1::second => now;
