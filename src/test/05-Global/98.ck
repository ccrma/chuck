// global variable used from within a public class
//
// NOTE this would not work if NOT explicitly global, and would
// results in an error, since public class X uses 'foo', but a
// non-global int would only be file-scope whereas a public class
// can be instantiated from any file
//
// SEE ALSO:
//     06-Errors/error-public-class-ext-var1.ck
//     06-Errors/error-public-class-ext-var2.ck

// global int
7 => global int foo;

// public classes can be instantiated from any file
public class X
{
    fun void go()
    {
        // this is OK since foo is declared as "global"
        if( (foo + 5) == 12 ) <<< "success" >>>;
    }
}

// evaluate, this will be on a different file context
Machine.eval( "X x; x.go();" );
