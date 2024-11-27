// test static non-function members
Math.PI;

// test for stack overflow
repeat(1000000) Math.PI;

class Foo
{
    static int bar;
}

// test for stack overflow
repeat(1000000) Foo.bar;

// if we get here, ok
<<< "success" >>>;
