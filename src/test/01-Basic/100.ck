// static function test: calling from class name (no instance)

public class Foo
{
    fun static void foo( string s )
    {
        if( s == "bar" ) <<< "success">>>;
    }
}

// call here
Foo.foo( "bar" );
