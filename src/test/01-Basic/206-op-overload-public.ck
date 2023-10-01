// global operator overloading using the 'public' keyword
// overloading an operator in one context, and using it in another

// a custom class
public class Foo
{ 5 => int n; }

// binary operator overloading
// NOTE the use of 'public' keyword
public int @operator +(Foo lhs, Foo rhs)
{
    return lhs.n + rhs.n;
}

// binary unary prefix overloading
public int @operator !(Foo foo)
{
    return !foo.n;
}

// binary unary postfix overloading
public int @operator (Foo foo) ++
{
    return foo.n++;
}

// use the overloading in a different code context
// (could be another file; here we use Machine.eval())
Machine.eval( "Foo a, b; <<<a++, !b, a+b>>>;" );
