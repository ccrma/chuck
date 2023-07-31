// error verification
// external var access from public class pre-constructor

// file-level variable
fun void foo()
{
    <<< "local function" >>>;
}

// public classes can be instantiated from any file
public class X
{
    // calling a local function from within a public class
    // things gets weird if X is instantiated from another file
    foo();
}
