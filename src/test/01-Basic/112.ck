// object and variable with one instantiation
class Foo {
    int i;
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    };   
}
Foo f;

<<< f.i >>>;
1::samp => now;
<<< f.i >>>;
1::samp => now;
<<< f.i >>>;
1::samp => now;
<<< f.i >>>;
