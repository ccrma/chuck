// 1.5.0.5 or higher

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
