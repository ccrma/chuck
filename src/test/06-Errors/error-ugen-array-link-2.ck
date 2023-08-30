// error case: detect empty array decls in array ugen linking
// https://github.com/ccrma/chuck/issues/39

// sine osc
SinOsc bar => dac;

// empty array decl cannot connect to other UGen(s)
SinOsc foos[] => [bar];
