// specifying and overloading class constructors within array decl

// a class
class Foo
{
    // a member variable (this will be initialized before
    // any actual constructors, as Foo's "pre-constructor")
    0 => int num;

    // constructor 1 "default"
    fun void Foo()
    {
        1 => num;
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

// invoke constructor for each array elements in array decl
Foo array1()[2];
// print each element's num
for( auto f : array1 ) <<< f.num, "" >>>;


// invoke constructor for each array elements in array decl
Foo array2(2)[3];
// print each element's num
for( auto f : array2 ) <<< f.num, "" >>>;

// instantiate an array of Foo, invoking constructor for each
new Foo(6,5)[4] @=> Foo array3[];
// print each element's num
for( auto f : array3 ) <<< f.num, "" >>>;
