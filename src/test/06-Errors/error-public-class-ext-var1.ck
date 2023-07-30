// error verification
// external var access from public class function
//
// NOTE this should not work (unless explicitly declared as
// 'global') and should result in an error, as public class X
// uses 'foo', but 'foo' only exists per-file is only valid
// if X is instantiated from the same file its declared in;
// however, a public class can be instantiated from any file,
// and the access of 'foo' would not make sense

// file-level variable
7 => int foo;

// public classes can be instantiated from any file
public class X
{
    fun void go()
    {
        // using an external variable from within a public class
        // things gets weird if X is instantiated from another file
        <<< foo + 5 >>>;
    }
}
