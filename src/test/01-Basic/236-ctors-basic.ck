// specifying and overloading class constructors
// requires chuck-1.5.2.0 or higher

// a class
class Foo
{
    // a member variable (this will be initialized before
    // any actual constructors, as Foo's "pre-constructor")
    1 => int num;

    // constructor 1 "default"
    fun @construct()
    {
        2 => num;
        <<< "constructor 1:", num >>>;
    }

    // constructor 2
    fun @construct( int x )
    {
        x => num;
        <<< "constructor 2:", x >>>;
    }
    
    // constructor 3
    fun void Foo( int x, int y )
    {
        x*y => num;
        <<< "constructor 3:", x, y >>>;
    }

    // constructor 4 (okay to omit void return type)
    fun Foo( int x, int y, int z )
    {
        x*y*z => num;
        <<< "constructor 4:", x, y, z >>>;
    }
}

// declare a Foo, invoke constructor 1
Foo f0;
// declare a Foo, invoke constructor 1
Foo f1();
// declare a Foo, invoke constructor 2
Foo f2(15);
// instantiate a Foo, invoke constructor 3
new Foo(8,9) @=> Foo @ f3;
// instantiate a Foo, invoke constructor 4
new Foo(10,11,12) @=> Foo @ f4;

// print
<<< f0.num, f1.num, f2.num, f3.num, f4.num >>>;
