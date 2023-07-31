// within class definition of Foo, go() needs 'myVal', cannot call go() before before 'myVal' initializes

// class definition
class Foo
{
    // no work -- go() depends on myVal being initialized
    go();
    
    // the dependency
    5 => int myVal;

    // member func
    fun void go()
    {
        // needs myVal to be initialized
        <<< myVal >>>;
    }
}

// should have error'ed out by this point
Foo foo;
// call go()
foo.go();

