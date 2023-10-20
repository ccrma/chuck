// test reference count
// related: https://github.com/ccrma/chuck/issues/374

// class
class Foo
{
    5 => int x;
}

// another class
class Bar
{
    // instance of Foo
    Foo foo;
    // get it
    fun Foo getFoo() { return foo; }
    // take as argument
    fun void setFoo( Foo f )
    {
        // should be 3
        <<< Machine.refcount(f) >>>;
    }
}

// instance of Bar
Bar bar;
// should be 1
<<< Machine.refcount(bar.foo) >>>;

// verify no missing release from getFoo()
bar.getFoo() @=> Foo @ foo;
//should be 2
<<< Machine.refcount(bar.foo) >>>;

// multi-exp stmt
bar.getFoo(), bar.getFoo().x, bar.getFoo();
// should be 2
<<< Machine.refcount(bar.foo) >>>;

// call a function
bar.setFoo( bar.foo );
