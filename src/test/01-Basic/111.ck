// static function test: sporking a static function defined in a 
// parent class, without using the dot notation

class FooParent
{
    fun static void foo( string s )
    {
        if( s == "bar" ) <<< "success" >>>;
    }
}

class Foo extends FooParent
{
    fun static void start()
    {
        // spork without dot notation e.g., f.foo(...)
        spork ~ foo( "bar" );
        // yield to let new shred run
        me.yield();
    }
}

Foo.start();
