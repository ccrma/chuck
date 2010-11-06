8 => int a;

function int abs( int v )
{
    if( v < 0 ) return -v;
    return v;
}

function void args( int y, int b )
{
    4 => a;
    b => stdout;
}

function float sum( float a, float b )
{
    return a + b;
}

function void go( int a )
{
    abs(a) => stdout;
    if( a == 0 )
        return;

    go( abs(a)-1 );
}

int i;
for( 0 => i; i < 10; i + 1 => i )
    go( 1000 );
abs(-1) => stdout;
args( 1, 2 );
sum( 1.0, 2.0 ) => stdout;
a => stdout;

