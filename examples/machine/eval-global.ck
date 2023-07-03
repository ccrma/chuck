// for this to work, need chuck-1.5.0.5 or higher

// a shared value among shreds
global int g_value;

// initial value
100 => g_value;

// code str
"global int g_value; g_value++;" => string code;

repeat( 3 )
{
    // this shows the execution order of Machine eval within a line of code
    // Machine.eval() code runs immediately, yielding the current shred
    cherr <= g_value <= " "
          <= (Machine.eval(code) ? "(eval)" : "(error)") <= " "
          <= g_value <= " "
          <= (Machine.eval(code) ? "(eval)" : "(error)")
          <= IO.nl();
}