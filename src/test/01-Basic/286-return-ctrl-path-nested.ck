// all control paths return, with nested function definitions
// before and after the statement lists

// a function
fun float foo( int v )
{
    // nested function definitions
    fun void bar1() { }
    fun void bar2() { }

    // control structure        
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

    // more nested function definitions
    fun void bar3() { }
    fun void bar4() { }
}

if( foo(2) == 2 ) <<< "success" >>>;
