// error case: runtime invalid cast
// as of 1.5.2.0 chuck performs runtime cast verification
// for all Object casts

// ok
Gain foo;
// ok
foo @=> UGen @ ugen;
// acceptable at compile time; NOT OK at runtime
// expect exception for incompatible dyanmic cast
ugen $ SinOsc @=> SinOsc @ uh;
// shouldn't get here because that would be bad...
440 => uh.freq;
