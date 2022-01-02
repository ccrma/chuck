// static function test: calling from instance

public class Foo
{
    fun static void foo( string s )
    {
        if( s == "bar" ) <<< "success">>>;
    }
}

// instantiate
Foo f;
// call here
f.foo( "bar" );
