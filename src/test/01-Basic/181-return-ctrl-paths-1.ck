// all control paths return 

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
    else
    {
        return 3;
    }
}

// success case
if( Math.equal(foo(1)+foo(2)+foo(3), 6) )
    <<< "success" >>>;
