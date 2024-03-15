// 1.5.2.2 
// make sure default ctor is called (was crashing on cleanup)
global OscIn aaa;
// try a second one
global OscIn bbb;
// try a non-global 
OscIn cc;

<<< "success" >>>;
