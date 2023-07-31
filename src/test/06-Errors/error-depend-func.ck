// dependency tracking for context-top-level variable
// PRC's error example

// does not work here, since it precedes 'array' decl/initialization
printArray();

// where 'array' initializes
[1, 2, 3] @=> int array[];

// FYI calling printArray() works here because it's after array initializes
// printArray();

// the function definition
fun void printArray()
{
    // needs 'array' to be initialized
    <<< array[0], array[1], array[2] >>>;
}
