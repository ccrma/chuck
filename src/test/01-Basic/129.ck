// this tests assignment into arrays
// june 2023

// error: [1.0, 1.0] @=> float foo[2];
// this is ok:
[1, 5] @=> int foo[];

// error: [ new Object ] @=> Object @ bar[1];
// this is ok:
[ new Object ] @=> Object @ bar[];

if( foo[1] == 5 && bar[0] != null ) <<< "success" >>>;
