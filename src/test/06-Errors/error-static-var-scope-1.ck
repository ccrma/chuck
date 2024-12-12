// error case: static variables must be declared at class scope

class Foo
{
    fun static void bar()
    {
        5 => static int X;
    }
}
