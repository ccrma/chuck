// let's check reg stack pointers
// 1.5.1.7 or higher

// save reg sp
Machine.sp_reg() => int sp;

// a function
fun void nope() { }

// one statement of several expressions
//<<< 5 >>>, 
1, 2, 3, 4, nope(), 5, 6, 7, nope(), nope();

// print
<<< 1, 2, 3, @(4,5), @(6,7,8) >>>;

// test
if( Machine.sp_reg() == sp ) <<< "success" >>>;