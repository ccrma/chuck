// error case: detect empty array decls in array ugen linking
// https://github.com/ccrma/chuck/issues/39

// a sinosc
SinOsc foo => dac;

// make into array (ok); connect to empty array decl (error)
[foo] => SinOsc bars[];
