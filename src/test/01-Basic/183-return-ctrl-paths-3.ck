// all control paths return, extra returns

fun int foo( int a, int b )
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
        
        // unreachable but okay
        return -1;
    }
    else if( a == 2 )
    {
        if( b == 1 )
        {
            return 3;
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
            return 6;
        }
    }
}

foo( 1, 1 ) => int r1;
foo( 1, 2 ) => int r2;
foo( 2, 1 ) => int r3;
foo( 2, 2 ) => int r4;
foo( 3, 1 ) => int r5;
foo( 3, 2 ) => int r6;

// success condition
if( r1 == 1 && r2 == 2 && r3 == 3 && r4 == 4 && r5 == 5 && r6 == 6 )
    <<< "success" >>>;
