// 15.ck : multi-dimensional array instantiation

// delcare int array 2x2x2
int a[2][2][2];
// delcare float array 2x2x2
float b[2][2][2];
// assign
5 => a[1][1][1] => b[1][1][1];

// multi-dimensional array initialization
[ [1,2], [3,4] ] @=> int c[][];

// test
if( a[1][1][1] == 5 && b[1][1][1] == 5.0 && c[0][1] == 2 ) <<<"success">>>;
