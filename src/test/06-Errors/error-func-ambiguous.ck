// test that when calling overloaded function, it calls the one
// with the "closer" signature, as determine by distance to parent
// types; in this case, SinOsc is "closer" to UGen than to Object

fun void foo( Object a, UGen b )
{
    <<< 1 >>>;
}

fun void foo( UGen a, Object b )
{
    <<< 2 >>>;
}

// a SinOsc which is an UGen, which in turn is an Object
SinOsc bar;

// which to call?
foo( bar, bar );

