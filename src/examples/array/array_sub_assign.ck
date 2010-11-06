// multi-dimensional array
// array to array assignment

int a[2][2][2];
int b[2][2];
// assignment
1 => b[0][1];
2 => b[0][0];

// assign b to 0th element of a
b @=> a[0];

// verify
if ( a[0][0][1] == 1 ) <<<"success">>>;
