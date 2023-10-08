//-----------------------------------------------------------
// the general format for overload an operator is as follows:
//-----------------------------------------------------------
// let's say we define a custom class...
public class Foo { int num; }

// persistent operator overloading (trascends contexts)
// NOTE the use of 'public' instead of 'fun' -- it's fun for all!
public Foo @operator =^( Foo lhs, Foo rhs )
{ /* do stuff for Foo =^ Foo */ return rhs; }

// LOCAL binary operator overload for '=>' (local to this context)
fun Foo @operator =>( Foo lhs, Foo rhs )
{ /* do stuff for Foo => Foo */ return rhs; }

// define binary operator overload for '+'
fun Foo @operator +( Foo lhs, Foo rhs )
{ Foo retval; lhs.num + rhs.num => retval.num; return retval; }

// define binary operator overload for '*'
fun Foo @operator *( Foo lhs, Foo rhs )
{ Foo retval; lhs.num * rhs.num => retval.num; return retval; }

// NOTE % can be overloaded, but due to polar e.g., `%(1,pi)`
// need to use the formal parenthesis notation to disambiguate
fun int @operator(%)( Foo foo, int mod )
{ mod %=> foo.num; return foo.num; }

// define unary operator overload for '!'
fun int @operator !( Foo foo )
{ return !foo.num; }

// define postfix operator overload for '++'
fun Foo @operator ( Foo foo ) ++
{ foo.num++; return foo; }

//-----------------------------------------------------------
// using the new overloaded operators
//-----------------------------------------------------------
// create 2 Foos
Foo a, b; 1 => a.num; 2 => b.num;
// operator in action (follows default ck operator precedence)
a + b * b + a @=> Foo c;
// post ++ on c
c++;
// should print 3
<<< c % 4 >>>;
