// all control paths return 

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
    }
    else if( a == 2 )
    {
        if( b == 1 )
            return 3;
        else
            return 4;
    }
    else
    {
        if( b == 1 )
        {
            return 5;
        }
        else if( b == 2 )
            return 6;
        else
        {
            return 7;
        }
    }
}

foo( 1, 1 ) => int r1;
foo( 1, 2 ) => int r2;
foo( 2, 1 ) => int r3;
foo( 2, 2 ) => int r4;
foo( 3, 1 ) => int r5;
foo( 3, 2 ) => int r6;
foo( 3, 3 ) => int r7;

// success condition
if( r1 == 1 && r2 == 2 && r3 == 3 && r4 == 4 && r5 == 5 && r6 == 6 && r7 == 7 )
    <<< "success" >>>;
