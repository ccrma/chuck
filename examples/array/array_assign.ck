// partial array (17.ck)

// declare int array 2x2
int x[2][2];

// assign
2 => x[1][1];

// assign partial array reference to 'y'
x[1] @=> int y[];

// test
if( y[1] == 2 ) <<<"success">>>;
