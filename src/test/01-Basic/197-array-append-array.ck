// test array append array (int)
// SEE: https://github.com/ccrma/chuck/issues/202

// 2D int array
int foo[0][0];
// int array
int bar[0];
// append to bar
bar << 1 << 2 << 3 << 5;
// appent 1D array into 2D array
foo << bar;

// print contents
for( int x[ ] : foo )
{
    for( int y : x )
    {
        <<< y >>>;
    }
}
