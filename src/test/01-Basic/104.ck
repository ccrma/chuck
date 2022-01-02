// static function test: multiple arguments

public class Foo
{
    fun static void foo( string a, int b, float c )
    {
        if( a == "bar" && b == 1 && c == 4.0 ) <<< "success" >>>;
    }
}

// spork it
Foo.foo( "bar", 1, 4.0 );
