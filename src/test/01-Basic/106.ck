// static function test: multiple arguments, sporked

public class Foo
{
    fun static void foo( string a, int b, float c )
    {
        if( a == "bar" && b == 1 && c == 4.0 ) <<< "success" >>>;
    }
}

// spork it
spork ~ Foo.foo( "bar", 1, 4.0 );

// give it a chance to run
me.yield();
