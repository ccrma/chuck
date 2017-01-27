// class body only run when instantiated
class Foo {
    static int i;
    // this doesn't run until we instantiate foo
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    };
}

<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
