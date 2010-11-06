// define function
fun void foo()
{
    while( true )
    {
        <<< "hi" >>>;
        .2::second => now;
    }
}

// spork the function, remember the shred reference
spork ~ foo() @=> Shred @ s;

// wait for 2 seconds
2::second => now;

// remove by id
Machine.remove( s.id() );

// wait a little more
2::second => now;
