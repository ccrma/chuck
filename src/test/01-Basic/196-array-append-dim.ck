// error append float arrays to higher dimension arrays
// SEE: https://github.com/ccrma/chuck/issues/202
// NOTE: works with int instead of float...

float foo[0][0];
float bar[0];
bar << 1 << 2 << 3 << 5;
foo << bar;

for( float x[ ] : foo )
{
    for( float y : x )
    {
        <<< y >>>;
    }
}
