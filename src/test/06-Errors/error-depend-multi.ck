// it's possible for a function or class def to have multiple dependencies;
// here printArray() needs both 'foo' and 'bar' to be initialized -- it
// is called AFTER foo initializes (good), but BEFORE 'bar' initializes,
// and should lead to a compiler error regarding 'bar'

// declare/initialize 'foo'
1 => int foo;

// does not work here, since printArray() needs 'foo' and 'bar' to be initialized
printArray();

// declare/initialize 'bar'
2 => int bar;

// works here, since it's after both 'foo' and 'bar' initialize
// printArray();

// function definition
fun void printArray()
{
    // need both 'foo' and 'bar' to be initialized
    <<< foo, bar >>>;
}
