// all control paths return, multi-level if/else

fun float foo( int a, int b )
{
    if( a == 1 )
    {
        if( b == 1 )
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else if( a == 2 )
    {
        if( b == 1 )
        {
            // commenting this out leads to not all
            // control paths returning a value
            // return 3;
        }
        else
        {
            return 4;
        }
    }
    else
    {
        if( b == 1 )
        {
            return 5;
        }
        else
        {
            return 5;
        }
    }
}

<<< foo( 2, 1 ) >>>;
