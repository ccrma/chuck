// error test: using local (non-class) var in static initialization
// #2024-static-init

// local var
5 => int N;

// non-public class (accessing local variables allowed, except
// for static initialization)
class Foo
{
    // ERROR using local variable to initialize static variable
    N => static int THE_NUM;
}
