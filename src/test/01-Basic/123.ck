// exit the shred of different object at different times
class Foo {
    int i;
    spork ~ {
        while( true ) { i++; 1::samp => now; }
    } @=> Shred myShred;
    
    fun void stopCounting() {
        myShred.exit();
    }   
}
// every instance runs a spork that affects its own int
Foo f1;
Foo f2;

<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
f2.stopCounting();
1::samp => now;
<<< f1.i, f2.i >>>;
f1.myShred.exit();
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
