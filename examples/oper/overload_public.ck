// global operator overloading using the 'public' keyword
// overloading an operator in one context, and using it in another
//
// requires chuck-1.5.5.8 or higher

// a custom class
public class Foo
{ 5 => int n; }

// binary operator overloading
// NOTE the use of 'public' keyword
public int @operator +(Foo lhs, Foo rhs)
{
    return lhs.n + rhs.n;
}

// unary prefix overloading
// NOTE: '!' is no longer overloadable as of v1.5.5.8,
// in order to support conditional checks, e.g., if(!obj)
public int @operator ~(Foo foo)
{
    return !foo.n;
}

// unary postfix overloading
public int @operator (Foo foo) ++
{
    return foo.n++;
}

// use the overloading in a different code context
// (could be another file; here we use Machine.eval())
Machine.eval( "Foo a, b; <<<a++, ~b, a+b>>>;" );
