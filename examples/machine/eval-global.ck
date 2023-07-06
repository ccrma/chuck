// name: eval-global.ck
// desc: this example uses Machine.eval() to run generated code
//       to update a global variable; this also shows Machine.eval()
//       running the evaluatd code synchronously and immediately
//
// version: requires chuck-1.5.0.5 or higher
//          Machine.eval() was first introduced in 1.5.0.0;
//          the operation semantics was changed in 1.5.0.5 for
//          Machine eval to run immediately, and automatically
//          yielding the current shred to give the eval'ed code
//          a chance to run -- all this without advancing time
//
// uncomment this to print out info about Machine:
// Machine.help();
//
// date: Summer 2023

// a shared value among shreds
global int g_value;

// initial value
100 => g_value;

// code str
"global int g_value; g_value++;" => string code;

// do this a couple of times
repeat( 3 )
{
    // this shows the execution order of Machine.eval() within
    // a single line of code -- Machine.eval() runs the new code
    // immediately (and synchronously) before continuing the
    // current shred (even if it's in the middle of a statement)
    // as shown below 
    cherr <= g_value <= " "
          <= (Machine.eval(code) ? "(eval)" : "(error)") <= " "
          <= g_value <= " "
          <= (Machine.eval(code) ? "(eval)" : "(error)")
          <= IO.nl();
}
