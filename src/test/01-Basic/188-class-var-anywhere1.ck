// verifying accessing member declared below where it's used

// instance of Foo
Foo foo;
// using val above declaration should be ok
5 => foo.val;

// this should be good too
8 => Foo.sval;

class Foo
{
    int val;
    static int sval;
}

// success if no errors
<<< "success" >>>;
