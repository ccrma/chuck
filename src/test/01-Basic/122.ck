// same as 121 but exits using nested sporks instead of shred objects
class Foo {
    int i;
    Event myExit;
    spork ~ {
        spork ~ {
            while( true ) { me.yield(); i++; 1::samp => now; }
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
f1.myExit.broadcast();
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
