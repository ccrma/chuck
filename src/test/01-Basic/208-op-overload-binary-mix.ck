// test binary overloading on objects that return primitives

// our example class
public class Foo
{ 3 => int n; }

// unary pre overloading, takes Foo returns int
fun int @operator +(Foo a, Foo b)
{
    return a.n + b.n;
}

// make foo
Foo a, b;
// invoke operator overloading
if( a + b == 6 ) <<< "success" >>>;
