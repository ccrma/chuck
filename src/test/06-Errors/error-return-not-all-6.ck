// missing return in non-void function
fun float foo()
{
    int i;
    
    // inner function missing return as well
    fun float bar()
    {
    }
}

// should yield a compile-time error

<<< foo() >>>;
