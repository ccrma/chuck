// verify statement list | function defintion | statement list
// related: see 06-Errors/error-return-not-all-6.ck

fun float foo()
{
    5 => int i;
    
    // nested function definition
    fun float bar()
    {
        6 => int i;
        return i;
    }
    
    return i;
}

// check
if( foo() == 5 ) <<< "success" >>>;
