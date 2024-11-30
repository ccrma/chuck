// error test: calling member function in static initialization
// #2024-static-init

// a class
class Foo
{
    // ERROR calling member function to initialize static var
    foo() => static int THE_NUM;
    
    // member function
    fun int foo() { return 5; }
}

