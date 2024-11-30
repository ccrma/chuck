// static variable initialization
// some extended tests for static variable initialization
// #2024-static-init

// a class
class Bar
{
    // a non-static function
    fun int bar() { return 1; }
}

// another class
public class Foo
{
    // a static function
    fun static int foo() { return 2; }

    // static Bar object
    static Bar myBar;

    // OK: using static object myBar to initialize a static var
    myBar.bar() => static int myFoo1;
    // OK: explicitly using Foo.myBar.bar() to initialize a static var
    Foo.myBar.bar() => static int myFoo2;
    // OK: although uses `this` AND bar() is not static, myBar is static
    this.myBar.bar() => static int myFoo3;
    // OK: although uses `this`, the function foo() is static
    this.foo() => static int myFoo4;
}

// should print 1 1 1 2
<<< Foo.myFoo1, Foo.myFoo2, Foo.myFoo3, Foo.myFoo4 >>>;
