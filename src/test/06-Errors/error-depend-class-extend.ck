// test declaring/instantation classes with inheritance, involving
// a member variable, declared in the file below its access

class Bar
{
    // this implicitly uses Bar.name...
    // also would cause a infinite loop, but won't get that far
    BarFactory.make();
    // the var
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

// shouldn't quite get to here
BarFactory.make();