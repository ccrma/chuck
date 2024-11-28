// test object reference integrity during constructor
// THANKS to Miro Swisher for discovering this issue
// chuck-1.5.4.3 (ge) added Thanksgiving 2024 lol

// a class definition
public class Foo
{
    // a member function that returns `this`
    fun Foo getFoo() { return this; }    
    // a constructor (takes argument for good measure)
    fun Foo( vec3 sz )
    {
        // return ourself -- note this is INSIDE Foo constructor
        // which means it's before this instance can be assigned
        // to a variable (as below); yet statements likes these
        // have a clean-up logic that balances reference counts;
        // so the refcount for Foo during instantiation must 
        // account for this interregnum; FYI this is typically
        // handled by temporarily boosting the refcount during
        // instantiation so it cannot be deleted from within
        // its own constructor!
        this.getFoo();
    }
}

// instantiate and assignment to variable
Foo foo( @(1,2,3) );

// no crash? ok
<<< "success" >>>;
