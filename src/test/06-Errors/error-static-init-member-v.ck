// error test: using member var in static initialization
// #2024-static-init

// a class
class Foo
{
    // member variable
    1 => int n;
    2 => int m;

    // ERROR using member variable to initialize
    n + m => static int THE_NUM;
}

