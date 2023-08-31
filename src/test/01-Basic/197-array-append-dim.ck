// error append float arrays to higher dimension arrays
// SEE: https://github.com/ccrma/chuck/issues/202
// NOTE: works with int instead of float...

// this doesn't crash (but float does)
int foo[0][0];
int bar[0];
bar << 1 << 2 << 3 << 5;
foo << bar;

for( int x[] : foo )
{
    for( int y : x )
    {
        <<< y >>>;
    }
}