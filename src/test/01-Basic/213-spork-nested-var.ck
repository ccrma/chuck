// test nested sporking, and accessing file-scope vars from
// nested sporked member function

// accessing this var
5 => int a;

// a (non-public) class
// (public classes wouldn't be able to access file-scope vars)
class Foo
{
    // a member function
    fun void update( float dt )
    {
        while( true )
        {
            // access global-scope var a
            a++;
            // print
            <<< a, dt >>>;
            // advance time
            200::ms => now;
        }
    }
    
    fun void go()
    {
        // spork
        spork ~ this.update(2);
        // wait a long time (until the parent shred is done)
        eon => now;
    }
}

// instantiate a Foo
Foo foo;
// call go
spork ~ foo.go();
// let time pass
2.1::second => now;
// (should remove child and grandchild shreds as this shred exits)
