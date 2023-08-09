// test decl in a multi-part chuck statement: A => decl B => C
// NOTE decl B is type-checked more than once: as part of `A => decl B`
// and `decl B => C`

SinOsc osc;

// A => decl B => C
440 => float foo => osc.freq;

// test
if( 440 == foo ) <<< "success" >>>;
