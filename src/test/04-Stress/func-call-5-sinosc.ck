// #2024-func-call-update verification

// test various flavors of call member function...
SinOsc foo;

// test stack alignment; just emit function as value
repeat(100000) foo.freq;
// test stack alignment; call the function
repeat(100000) foo.freq(440);
// test stack alignment; call the function with =>
repeat(100000) 440 => foo.freq;

<<< "success" >>>;