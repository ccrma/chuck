// static test: calling a static function with dot notation

class Foo
{
    static Foo @ ours;

    fun static void start()
    {
        Foo f;
        f @=> Foo.ours;
        ours.foo( "bar" );
    }

    fun static void foo( string s )
    {
        if( s == "bar" ) <<< "success" >>>;
    }
}

Foo.start();
