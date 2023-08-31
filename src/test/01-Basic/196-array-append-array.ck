// test array append array (float)
// SEE: https://github.com/ccrma/chuck/issues/202

// 2D float array
float foo[0][0];
// float array
float bar[0];
// append to bar
bar << 1 << 2 << 3 << 5;
// append 1D array into 2D array
foo << bar;

// print contents
for( float x[] : foo )
{
    for( float y : x )
    {
        <<< y >>>;
    }
}
