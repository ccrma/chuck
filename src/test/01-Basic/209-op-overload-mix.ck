// test public operator overloading; mixture of binary, pre, postfix

public class Foo
{ 5 => int n; }

// PUBLIC overloading
public int @operator +(Foo lhs, Foo rhs)
{
    return lhs.n + rhs.n;
}

// PUBLIC unary operator overload
fun int @operator ~( Foo foo )
{ return !foo.num; }

// PUBLIC unary pre overloading
public int @operator ++(Foo foo)
{
    return ++foo.n;
}

// PUBLIC unary post overloading
public int @operator (Foo foo) ++
{
    return foo.n++;
}

Foo a;
Foo b;
<<< a + b, ~a, ++a, b++, b.n >>>;