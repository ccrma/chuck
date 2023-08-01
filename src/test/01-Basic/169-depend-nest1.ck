// test for dependency through transitive function calls
// i.e., in this case, nest() essentially inherits the dependencies
// of printArray() which include 'array'. Since nest() is called
// after 'array' initializes, we are good

// nest function definition, which can be anywhere in the code
// as long as calling this function doesn't precede any needed
// variable initialization
fun void nest()
{
    // causes nest to inherit printArray()'s dependencies
    printArray();
}

// 'array' initializes here
[1, 2, 3] @=> int array[];

// should work here, since we calling nest() after 'array' init
nest();

// function definition
fun void printArray()
{
    // access 'array'
    <<< array[0], array[1], array[2] >>>;
}
