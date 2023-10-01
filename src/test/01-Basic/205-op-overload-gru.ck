// test gruck (graphical chuck) operator

// a class
class Thing { float x; }

// overload gruck operator
fun Thing @operator -->( Thing lhs, Thing rhs )
{
    <<< lhs.x, rhs.x >>>;
    return rhs;
}

// set up a few Things
Thing foo; 1 => foo.x;
Thing bar; 2 => bar.x;
Thing zaz; 3 => zaz.x;

// gruck foo to bar
foo --> bar;

// gruck foo to bar to zaz
foo --> bar --> zaz;

// (should have printed some stuff)
