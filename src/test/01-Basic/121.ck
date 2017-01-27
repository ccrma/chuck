// shreds seem to exit one "wait" after they are really exited
// unless the shreds call me.yield() before the increment
// because otherwise it runs before the global shred can call exit
// (see also test 119)
class Foo {
    int i;
    spork ~ {
        while( true ) { me.yield(); i++; 1::samp => now; }
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
f1.myShred.exit();
1::samp => now;
<<< f1.i, f2.i >>>;
1::samp => now;
<<< f1.i, f2.i >>>;
