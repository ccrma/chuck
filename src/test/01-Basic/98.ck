// this is a test of cleaning up multi-dim local array objects
fun void test()
{
    [ [1,2,3], [1,2,3] ] @=> int x[][];
}

// call once; upon exiting, this should clean up x[][], including
// the two one-dimensional arrays contained within
test();

// if things go well, this should also work
test();

// hopefully we get here!
<<< "success" >>>;

// 1.4.1.2 (ge) | added fall 2022
// based on bug report by Terry F.
//
// Hi Ge,
//
// I was working on 256A this week when I found this interesting bug
// that causes miniAudicle to crash. Took a look at it with Nick this
// afternoon and found out that it crashes on Mac (M1) but Windows,
// seems to be fine. Not sure how it is on other systems but thought
// I'd share this. 
//
// Code to reproduce: 
//
// fun void test()
// {
//      [ [1,2,3], [1,2,3] ] @=> int arr[][];
//      1::second => now;
// }
//
// test();
// test();
//
// Output:
//
// If running in terminal:
// Assertion failed: (FALSE), function execute, file chuck_instr.cpp, line 5166.
// zsh: abort      chuck test.ck
//
// If running in miniAudicle, it'll just crash.
