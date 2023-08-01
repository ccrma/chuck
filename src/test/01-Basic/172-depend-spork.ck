// dependency tracking for context-top-level variable
// if sporking, then no dependency check and it's up to
// programmer to ensure behavior is as intended

// works here, due to spork
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
