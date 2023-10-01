// 1) verify overloading on objects that return primitives
// 2) verify simultaneous overloading of pre/post for ++
// 3) verify execution of pre/post increment

// our example class
public class Foo
{ 0 => int n; }

// unary pre overloading
fun int @operator !(Foo foo)
{
    return !foo.n;
}

// unary pre overloading
fun int @operator ++(Foo foo)
{
    return ++foo.n;
}

// unary postfix overloading
fun int @operator (Foo foo)++
{
    return foo.n++;
}

// make foo
Foo a;
// print 1 1 2 2 2 3
<<< !a, ++a, ++a, a.n, a++, a.n >>>;
