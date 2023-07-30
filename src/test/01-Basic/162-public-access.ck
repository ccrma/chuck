// verify access from within public class definitions
//
// SEE ALSO:
//     05-Global/98.ck
//     06-Errors/error-public-class-ext-var1.ck
//     06-Errors/error-public-class-ext-var2.ck

// public classes can be instantiated from any file
public class X
{
    fun void go()
    {
        // use Machine to spork another shred
        Machine.eval( "<<<\"success\">>>;" );
    }
}

// instantiate
X x;
// call
x.go();
