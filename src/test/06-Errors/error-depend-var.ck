// dependency tracking for context-top-level variable

// does not work here, since it precedes 'bar' initialization
foo();

// where 'bar' initializes
5 => int bar;

// FYI calling foo() works here because it's after 'bar' initializes
// foo();

// the function definition
fun void foo()
{
    // needs 'bar' to be initialized
    <<< "bar:", bar >>>;
}
