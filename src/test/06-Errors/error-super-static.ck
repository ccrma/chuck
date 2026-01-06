class X
{
    fun void boo() { }
}

class Y extends X
{
    fun static void noo()
    {
        // super can only be used from non-static contexts
        super.foo();
    }    
}
