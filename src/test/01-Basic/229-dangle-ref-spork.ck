// verify no dangling reference with sporked
// functions that return Objects

Object foo;

// function that returns an Object
fun Object get()
{
    return foo;
}

// spork
spork ~ get();

// give sporked function a chance to run
10::ms => now;

// check reference count
if( Machine.refcount(foo) == 1 ) <<< "success" >>>;