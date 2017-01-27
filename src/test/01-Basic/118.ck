// every instance runs a spork that affects its own int
class Foo {
    int i;
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    };   
}

Foo f1;
Foo f2;

<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
