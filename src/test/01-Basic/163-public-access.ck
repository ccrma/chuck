// verify access from within public class definitions
//
// SEE ALSO:
//     05-Global/98.ck
//     06-Errors/error-public-class-ext-var1.ck
//     06-Errors/error-public-class-ext-var2.ck

// public classes can be instantiated from any file
public class X
{
    // member variable
    5 => int myNum;

    fun void go()
    {
        // roundabout way
        if( go2() == 12 )
            Machine.eval( "<<<\"success\">>>;" );
    }

    fun int go2()
    {
        return myNum + 7;
    }
}

// instantiate
X x;
// call
x.go();
