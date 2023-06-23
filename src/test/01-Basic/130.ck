// this tests implicitly treating RHS declaration as a @ reference
// june 2023

// this basically is seen by chuck as
// 'new SinOsc @=> SinOsc @ foo;'
new SinOsc @=> SinOsc foo;

// should have no errors
<<< "success" >>>;
