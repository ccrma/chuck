// usage examples for a number of Machine functions
// (requires chuck-1.5.1.3 or higher)
//
// see ChucK API Reference on Machine
// https://chuck.stanford.edu/doc/reference/base.html#Machine
//
// or uncomment the next line to print Machine usage
// Machine.help()

// print language version
<<< "chuck version:", Machine.version() >>>;

// keep informed
<<< "\nMachine.eval() example...", "" >>>;

// compile the string as code and spork it as a new shred
// also see eval.ck for more on Machine.eval()
Machine.eval( "repeat(5) { <<< Math.random2f(0,1) >>>; }" );

// keep informed
<<< "\nwaiting for half a second...", "" >>>;

// advance time
.5::second => now;

// keep informed
<<< "\ntrying some shred management...", "" >>>;

// add
Machine.add( me.dir() + "test1.ck:foo" ) => int id;
// yield to give new shred a chance to run
me.yield();

// replace
Machine.replace( id, me.dir() + "test2.ck:bar:5" ) => id;
// yield to give new shred a chance to run
me.yield();

// print current VM status (list of shreds)
Machine.printStatus();
// remove shred
Machine.remove( id );

// keep informed
<<< "\ntrying more Machine functions...", "" >>>;
// print current VM time
Machine.printTimeCheck();
// reset shred IDs to current highest + 1
Machine.resetShredID();
// remove all shreds (including this one!)
Machine.removeAllShreds();
// reset type system and all global variables; remove all shreds
// Machine.clearVM();

// unreachable code
<<< "shouldn't get here" >>>;
