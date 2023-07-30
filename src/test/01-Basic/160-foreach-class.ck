// testing for-each inside class and function defs

// class definition
class Foo
{
    // member
    [1,2,3] @=> int array[];

    // pre-initializer
    for( int a : array )
    { <<< a >>>; }
    
    // static
    static int array_static[];
    
    // member fun
    fun void go()
    {
        for( int a : array_static )
        {
            <<< a >>>; 
        }
    }

    // static fun
    fun void go_static( int offset )
    {
        for( int a : array_static )
        {
            <<< a + offset >>>; 
        }
    }
}

// set static member
[4,5] @=> Foo.array_static;

// instantiate
Foo foo;

// call using instance
foo.go();

// call using instance
foo.go_static( Foo.array_static.size() );

// iterate
for( auto a : Foo.array_static )
{
    <<< a + 2*Foo.array_static.size() >>>;
}

// iterate
for( auto a : foo.array )
{
    <<< a >>>;
}
