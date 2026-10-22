// all control paths return 

fun float foo( int v )
{
    // function definitions (before statement lists)
    fun void bar1() { }   
    fun void bar2() { }
        
    if( v == 1 )
    {
        return 1;
    }
    else if( v == 2 )
    {
        // missing
    }
    else
    {
        return 3;
    }

    // more nested function definitions
    fun void bar3() { }
    fun void bar4() { }
}

<<< foo( 1 ) >>>;
