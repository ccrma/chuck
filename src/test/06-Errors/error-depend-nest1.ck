// error case: test for dependency through transitive function calls
// i.e., in this case, nest() essentially inherits the dependencies
// of printArray() which includes 'array'

// nest function definition
fun void nest()
{
    // causes nest to inherit printArray()'s dependencies
    printArray();
}

// should not work here, since nest() needs printArray()
// which needs 'array', but nest() is called before 'array'
// initializes
nest();

// 'array' initializes here
[1, 2, 3] @=> int array[];

// function definition
fun void printArray()
{
    // access 'array'
    <<< array[0], array[1], array[2] >>>;
}
