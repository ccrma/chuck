// testing overloaded functions to return different types
// 1.5.2.0 -- reinstated

// a custom class
public class Foo { int num; }

fun Foo @operator =>( Foo lhs, Foo rhs )
{ /* do stuff for Foo => Foo */ return rhs; }

fun int @operator =>( int lhs, int rhs )
{ /* do stuff for Foo => Foo */ return rhs; }

// foo() overload 1 returns float
fun float foo( float b ) { return b; }

// foo() overload 2 returns int
fun int foo( int a ) { return a; }


if( Type.of(foo(1)).name() == "int" &&
    Type.of(foo(2.0)).name() == "float" ) <<< "success" >>>;
