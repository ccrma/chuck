// 17.ck : partial array

// declare int array 2x2
external int x[2][2];

// assign
2 => x[1][1];

// assign partial array reference to 'y'
x[1] @=> external int y[];

// test
if( y[1] == 2 ) <<<"success">>>;
