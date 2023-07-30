// error verification
// external var access from public class pre-constructor

// file-level variable
7 => int foo;

// public classes can be instantiated from any file
public class X
{
    // using an external variable from within a public class
    // things gets weird if X is instantiated from another file
    <<< foo + 5 >>>;
}
