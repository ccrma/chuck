// error case: test for dependency through transitive function calls
// i.e., in this case, nest() essentially inherits the dependencies
// of printArray() which includes 'array'

// should not work here
nest();

[1, 2, 3] @=> int array[];

// function definition
fun void nest()
{
    // calls printArray(); in dependency tracking, nest() inherits
    // printArray()'s dependencies
    printArray();
}

// function definition
fun void printArray()
{
    // need 'array' to be initialized
    <<< array[0], array[1], array[2] >>>;
}
