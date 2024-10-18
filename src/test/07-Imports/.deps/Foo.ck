// let's say we define a custom class...
public class Foo { int num; }

// persistent operator overloading (transcends file contexts)
// NOTE the use of 'public' instead of 'fun' -- it's fun for all!
// define binary operator overload for '+'
public Foo @operator +( Foo lhs, Foo rhs )
{ Foo retval; lhs.num + rhs.num => retval.num; return retval; }

// LOCAL binary operator overload for '=>' (local to this context)
fun void @operator =^( Foo lhs, Foo rhs )
{ <<< lhs.num, "=^", rhs.num >>>; }

