// static function test: sporking from class

public class Foo
{
    fun static void foo( string s )
    { if( s == "bar") <<< "success" >>>; }
}

// spork it
spork ~ Foo.foo( "bar" );

// give it a chance to run
me.yield();
