// #2024-func-call-update verification

// test custom non-class function
fun void foo( int a ) { }

// call it
foo(1);
// get the function as value only
foo;

// check for stack overflow
repeat(100000) foo(2);
repeat(100000) foo;

// if we get here we are ok
<<< "success" >>>;