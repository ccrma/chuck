// verify dangling references are cleaned up
// across nested func calls that return Objects

// a class
class Foo
{
    5 => int bar;
    
    fun Foo yikes( Foo f)
    {
        return f;
    }
}

// a Foo
Foo bob;
// func returning bob
fun Foo go() { return bob; }
// print
fun void huh( Foo arg ) { <<< arg.bar >>>; }

bob; // just bob
go(); // fine?
go().bar; // problem?
<<< go() >>>;
go().bar + 5;
// call a func with arg
huh( go() );
go().yikes( go() ); // all instances released?
go().yikes( go() ).bar; // oh no
<<< go().yikes(go()).bar++ >>>; // this can't possibly work
<<< go().yikes(go()).bar++ >>>; 
<<< go().yikes(go()).bar++ >>>;

// test
if( Machine.refcount(bob) == 1 ) <<< "success" >>>;