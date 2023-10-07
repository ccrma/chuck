class Foo
{
    5 => int x;
}

class Bar
{
    Foo foo;
    fun static Foo getFoo() { return foo; }
}

Bar bar;

bar.getFoo();
