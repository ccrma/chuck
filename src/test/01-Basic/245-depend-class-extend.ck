// test declaring/instantation classes with inheritance, involving
// a member variable, declared in the file below its access
//
// related:
// https://github.com/ccrma/chuck/issues/349
// https://github.com/ccrma/chuck/issues/376

public class Foo
{
    // this implicitly uses Bar.name, but okay since .name gets
    // initialized in pre-ctor before all this runs
    BarFactory.make() @=> Bar bar;
    // test new
    new BarChild @=> BarChild @ bc;
    // test decl
    BarChild bc2;
}

class Bar
{
    "i am bar" => string name;
}

class BarChild extends Bar
{
    "success" => name;
}

class BarFactory
{
    fun static BarChild make()
    {
        return new BarChild;
    }
}

// make a Foo
Foo foo;
// print
<<< foo.bar.name >>>;
