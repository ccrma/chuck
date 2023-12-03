// verify that accessing file-level var from function is allowed
// (FYI related to https://github.com/ccrma/chuck/issues/376)

// a class
class Foo
{
    // a func
    fun void go()
    {
        // this shouldn't produce an error
        <<< a >>>;
    }
}

// another func, non-class
fun void bar()
{
    <<< a >>>;
}

// decl and init
5 => int a;

// if we get here then consider success
<<< "success" >>>;
