// exit the shred of different object at different times
class Foo {
    int i;
    Event myExit;
    spork ~ {
        spork ~ {
            while( true ) { i++; 1::samp => now; }
        };
        myExit => now;
    };
    
    fun void stopCounting() {
        myExit.broadcast();
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
f1.myExit.broadcast();
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
