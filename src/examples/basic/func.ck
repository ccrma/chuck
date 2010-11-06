8 => int a;

fun int abs( int v )
{
    if( v < 0 ) return -v;
    return v;
}

fun void args( int y, int b )
{
    4 => a;
    <<<b>>>;
}

fun float sum( float a, float b )
{
    return a + b;
}

fun void go( int a )
{
    <<<abs(a)>>>;
    if( a == 0 )
        return;

    go( abs(a)-1 );
}

int i;
for( 0 => i; i < 10; i + 1 => i )
    go( 1000 );
<<<abs(-1)>>>;
args( 1, 2 );
<<<sum( 1.0, 2.0 )>>>;
<<<a>>>;

