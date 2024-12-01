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
        13 => num; // set num to something
        <<< "constructor 1:", num >>>;
    }

    // constructor 2
    fun @construct( int x )
    {
        x => num;
        <<< "constructor 2:", x >>>;
    }
    
    // constructor 3
    // (using class name OR @construct is interchaneable)
    fun Foo( int x, int y )
    {
        x*y => num;
        <<< "constructor 3:", x, y >>>;
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

// print
<<< f0.num, f1.num, f2.num, f3.num >>>;

// can also invoke constructor for each element in array
Foo array1(2)[3];
// print each element's num
for( auto f : array1 ) <<< f.num, "" >>>;

// instantiate an array of Foo, invoking constructor for each
new Foo(6,5)[4] @=> Foo array2[];
// print each element's num
for( auto f : array2 ) <<< f.num, "" >>>;
