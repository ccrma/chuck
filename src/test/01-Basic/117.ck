// global object shared, doesn't clobber other global object
class IntPocket {
    int myI;
}
IntPocket i;
class Foo {
    static IntPocket @ i;
    // this doesn't run until we instantiate foo
    spork ~ {
        while( true ) { i.myI++; 1::samp => now; }
    };
}
IntPocket i2 @=> Foo.i;
Foo f;

<<< Foo.i.myI, i2.myI, i.myI >>>;
1::samp => now;
<<< Foo.i.myI, i2.myI, i.myI >>>;
1::samp => now;
<<< Foo.i.myI, i2.myI, i.myI >>>;
1::samp => now;
<<< Foo.i.myI, i2.myI, i.myI >>>;
