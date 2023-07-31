// separate dependency tracking for context-top-level vars and class-top-level vars

// class definition
class Foo
{
    // the dependency
    5 => int myVal;

    // ok since after myVal initialize
    go() + 2 => myVal;
    
    fun int go()
    {
        return myVal;
    }
}

// should work before or after the class def
Foo foo;
// should work before or after the class def
if( foo.go() == 7 ) <<< "success" >>>;

