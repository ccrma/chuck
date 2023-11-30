// test that when calling overloaded function, it calls the one
// with the "closer" signature, as determine by distance to parent
// types; in this case, SinOsc is "closer" to UGen than to Object

fun int foo( Object obj )
{
    return 1;
}

fun int foo( UGen obj )
{
    return 2;
}

// a SinOsc which is an UGen, which in turn is an Object
SinOsc bar;

// which to call?
if( foo( bar ) == 2 ) <<< "success" >>>;
