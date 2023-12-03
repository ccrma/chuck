// within class definition of Foo, class member 'myVal' is used before declaration

// class definition
class Foo
{
    // no work -- depends on myVal being initialized
    <<< myVal >>>;

    // the dependency
    5 => int myVal;
}

// should have error'ed out by this point
Foo foo;

