// name: eval.ck
// desc: use Machine.eval to run-time compile and run string as code
//       this enables chuck to generate more code to run!
//       powerful! perilous!
//
// version: need chuck-1.4.2.1 or higher
//
// uncomment this to print out info about Machine:
// Machine.help();
//
// date: Spring 2023

// our code to run
"cherr <= 1+1 <= IO.newline();" => string codeStr;

// compile the string as code and spork it as a new shred
if( !Machine.eval( codeStr ) ) <<< "error evaluating code!", "" >>>;

// NOTE: since Machine.eval() sporks the code as a child shred, we
// need to give it a chance to run; (a parent shred upon ending will
// also end all of its child shreds)
1::samp => now;
