// two objects both incrementing the same static variable
class Foo {
    static int i;
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    };   
}
// every instance runs a spork that affects static int
// so every samp, i increments by 2
Foo f1;
Foo f2;

<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
1::samp => now;
<<< Foo.i >>>;
