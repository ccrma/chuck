// 17.ck : partial array

// assign 1 to newly declared int
1 => int i;

// increment assign i
3 +=> i;

// reference assignment
Object foo @=> Object @ bar;

// (foo bar both reference same object)

// declare int array 2x2
int x[2][2];

// assign
2 => x[1][1];

// assign partial array reference to 'y'
x[1] @=> int y[];

// test
if( y[1] == 2 ) <<<"success">>>;
