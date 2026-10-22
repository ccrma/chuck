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

    fun void bar()
    { }

    fun void bar2()
    { }
}

// check
if( foo(2) == 2 ) <<< "success" >>>;
