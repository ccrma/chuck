// specifying and overloading class constructors
// requires chuck-1.5.2.0 or higher

// a class
class Foo
{
    // a member variable (this will be initialized before
    // any actual constructors, as Foo's "pre-constructor")
    1 => int num;

    // constructor 1 "default"
    fun void Foo()
    {
        2 => num;
        <<< "constructor 1:", num >>>;
    }

    // constructor 2
    fun void Foo( int x )
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
}

// declare a Foo, invoke constructor 1
Foo f1();
// declare a Foo, invoke constructor 2
Foo f2(15);
// instantiate a Foo, invoke constructor 3
new Foo(8,9) @=> Foo @ f3;

// print
<<< f1.num, f2.num, f3.num >>>;
