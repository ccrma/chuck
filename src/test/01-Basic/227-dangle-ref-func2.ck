class Foo
{
    int i;
}

fun void go( Foo foo )
{
    if( Machine.refcount(foo) != 2 ) <<< "problem found" >>>;
}

Foo foo;

go( foo );

// test
if( Machine.refcount(foo) == 1 ) <<< "success" >>>;