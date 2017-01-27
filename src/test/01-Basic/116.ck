// global var not touched when static class var in scope
int i;
class Foo {
    static int i;
    // this doesn't run until we instantiate foo
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    };
}
Foo f;

<<< Foo.i, i >>>;
1::samp => now;
<<< Foo.i, i >>>;
1::samp => now;
<<< Foo.i, i >>>;
1::samp => now;
<<< Foo.i, i >>>;
