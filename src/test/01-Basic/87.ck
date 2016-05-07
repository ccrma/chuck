// float div-by-zero
<<< 1./0 >>>;
<<< 1/0. >>>;
<<< 0./0 >>>;
<<< 0/0. >>>;
// vec div-by-zero
<<< @(1,1,1)/0 >>>;
<<< @(1,0,1,0)/0 >>>;
// complex div-by-zero
<<< #(1,1) / 0 >>>;
<<< #(1,1) / 0. >>>;
<<< #(1,1) / #(0,0) >>>;
// polar div-by-zero
0 => polar p;
<<< p / p >>>;

// integer div-by-zero
<<< 1/
0 >>>;
