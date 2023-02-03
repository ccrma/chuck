// this is a test of cleaning up multi-dim local array objects
// floating point version of test #98 (see 98.ck for details)
fun void test()
{
    [ [1.0,2.0,3.0], [1.0,2.0,3.0] ] @=> float arr[][];
}

test();
test();

// hopefully we get here!
<<< "success" >>>;

