// define a public Foo class (picked up by @import)
public class Foo
{
    // member variable
    int num;

    // constructor
    fun Foo( int n ) { n => num; }
}

// a non-public class definition (ignored by @import)
class Bar
{
    // ...
}

// public operator overloading + for Foo (picked up by @import)
public Foo @operator +( Foo lhs, Foo rhs )
{
    // return a new Foo
    return new Foo ( lhs.num + rhs.num );
}

// non-public binary operator overload for '=>' (ignored by @import)
fun void @operator =^( Foo lhs, Foo rhs )
{ 
    // for sake of example, just print contents
    <<< lhs.num, "=^", rhs.num >>>;
}
