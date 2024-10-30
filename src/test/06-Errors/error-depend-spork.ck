// error checking: as of 1.5.4.0 dependency tracking
// for context-top-level variable where sporked functions
// are now verified same as non-sporked function calls
//
// previously, if sporking, then no dependency check and it's up to
// programmer to ensure behavior is as intended

// spork a function (that depends on foo)
spork ~ printArray();

// initialize foo
5 => int foo;

// function that needs array
fun void printArray()
{
    // need 'foo' initialized here
    if( foo == 5 ) <<< "success" >>>;
}

// give the sporked shred a chance to run
me.yield();
