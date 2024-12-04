// test using a local variable declared after a function that uses that variable
//
// added 1.5.4.4; previously this would cause a crash because the emitter
// emitted foo() first, at which point the variable `osc` has not yet received
// it's stack offset, since `SinOsc osc(440);` appears later in the file
// fix: now code emisison is sorted for file-level code, function defs, class
// defs.

// function def
fun void foo()
{
    // uses `osc`
    if( Math.equal(osc.freq(),440) ) <<< "success" >>>;
}

// osc is declared
SinOsc osc(440);

// call foo
foo();
