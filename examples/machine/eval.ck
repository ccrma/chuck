// name: eval.ck
// desc: use Machine.eval to run-time compile and run string as code
//       this enables chuck to generate more code to run!
//       powerful! perilous!
//
// version: need chuck-1.5.0.5 or higher
//          first introduced in 1.5.0.0; behavior was changed
//          in 1.5.0.5 to run immediately, yielding the current
//          shred
//
// uncomment this to print out info about Machine:
// Machine.help();
//
// date: Spring 2023

// our code to run
"cherr <= \"hello!\" <= IO.newline();" => string codeStr;

// compile the string as code and spork it as a new shred
if( !Machine.eval( codeStr ) ) <<< "error evaluating code!", "" >>>;

// each of these will be evaluated and run as code,
// each on its independent shred; Machine.eval() automatically
// yields the originating "evaluator" shred, giving the evaluated
// code a chance to run without advancing time
Machine.eval( "<<< 1 >>>;" );
<<< "a" >>>;
Machine.eval( "<<< 2 >>>;" );
<<< "b" >>>;
Machine.eval( "<<< 3 >>>;" );
<<< "c" >>>;
