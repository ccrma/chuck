// static function test: sporking using instance

public class Foo
{
    fun static void foo( string s )
    { if( s == "bar") <<< "success" >>>; }
}

// instantiate a Foo
Foo f;
// spork it
spork ~ f.foo( "bar" );

// give it a chance to run
me.yield();
