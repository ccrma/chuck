// verify that binary, unary-pre, and unary-post overloadings that
// return Objects are correctly ref-counted across op func calls

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
a + b * b + a @=> Foo @ c; // note c is not a new instance
// post ++ on c
c++;
// another instance
Foo d;
// post ++ on d
d++;

// should print 1s
if( Machine.refcount(a) == 1 &&
    Machine.refcount(b) == 1 &&
    Machine.refcount(c) == 1 &&
    Machine.refcount(d) == 1 ) <<< "success" >>>;
