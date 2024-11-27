// #2024-func-call-update verification

// test special prim-object hybrids
vec2 v;

// test for stack overflow
repeat(1000000) v.dot;

// if we get here, ok
<<< "success" >>>;
