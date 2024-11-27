// #2024-func-call-update verification

// test member function
SinOsc osc;

// emit as value without call
osc.gain;

// test for stack overflow
repeat(1000000) osc.gain;

// this might yield a compiler later on, until then...
repeat(1000000) osc.gain == "hello";

// if we get here, ok
<<< "success" >>>;
