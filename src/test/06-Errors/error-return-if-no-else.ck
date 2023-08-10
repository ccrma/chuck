// not all control paths return
// IF without an ELSE clause, for example

fun float foo( int v )
{
    if( v == 1 )
    {
        return 1;
    }
    else if( v == 2 )
    {
        return 2;
    }
    
    // ok if uncommented
    // return 3;
}

<<< foo( 1 ) >>>;
