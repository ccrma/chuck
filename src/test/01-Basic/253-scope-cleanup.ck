// (non-control structure) local scope Object cleanup
// 1.5.2.5 (ge) added

// a class
class Foo
{
    int val;
    
    fun @construct( int x )
    { x => val; }

    fun @destruct()
    { <<< "destructor:", val >>>; }
}

// control structure scope
if( true )
{
    // nested scope
    {
        Foo foo(-1);
        <<< 1, "" >>>;

        // nested nested scope
        {
            Foo bar(-2);
            <<< 2, "" >>>;
        }
        <<< 3, "" >>>;

    }
    <<< 4, "" >>>;

}
<<< 5, "" >>>;

// top-level scope
{
    Foo foo(0);
}
<<< 6, "" >>>;
