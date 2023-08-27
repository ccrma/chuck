// test context unloading
// during 1.5.1.1 development, errors could cause a crash
// depending where the error was, due to deleting rather thna
// releasing the context's nspc

// this should result in an error (without crashing lol)
x;

// the crash would be related to context-local values like 'bar'
// cleaning up and releasing references to its owner namespace
fun int foo( int bar )
{
    return bar;
}
