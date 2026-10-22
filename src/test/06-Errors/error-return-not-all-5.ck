// missing return in non-void function
fun float foo()
{
    5 => int i;

    fun void bar()
    { }

    fun void bar2()
    { }
    
    // missing return after nested function defs at same scope
}

// should yield a compile-time error

<<< foo() >>>;
