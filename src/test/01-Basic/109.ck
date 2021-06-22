// static function test: calling a static function defined in a 
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
        // call without dot notation e.g., f.foo(...)
        foo( "bar" );
    }
}

Foo.start();
