// separate dependency tracking for context-top-level vars and class-top-level vars

// instantiate Foo
Foo foo;
// call go(), which needs myVal
// this function calls happens BEFORE myVal initialization in the code...
// but OKAY here since my Val is in class definition
if( foo.go() == 5 ) <<< "success" >>>;

// class definition
class Foo
{
    // member
    5 => int myVal;

    // member func
    fun int go()
    {
        // myVal should be initialized at this point
        return myVal;
    }
}
