// test multiple semicolons after last statement in statement lists in fuctinos
// this is significant as the control path analysis looks at the last (non-null)
// statement and semicolons actually parse as NULL statements

public class Class
{
    fun void foo()
    {
       // double semicolons
       samp => now;;
    }
}

fun void bar()
{
    while( false )
    {
        // multiple semicolons
        samp => now;;;;;;
    }
}

// if no crash, then good lol
<<< "success" >>>;
