// test dangling object cleanup on recursive functions that return Objects

// test object
Object foo;

// a recrusive function
fun Object get( int i )
{
    // stop recursion
    if( i == 10 ) return foo;
    // calls
    get(i+1), get(i+1), get(i+1);
    // another call
    return get(i+1);
}

// get started
get(0);

// check
if( Machine.refcount(foo) == 1 ) <<< "success" >>>;

