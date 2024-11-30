// error test: calling local (non-class) fun in static initialization
// #2024-static-init

// local function
fun int foo() { return 5; }

// non-public class (calling local functions allowed, except
// for static initialization)
class Foo
{
    // ERROR calling local fun in static initialization statement
    foo() => static int THE_NUM;
}
