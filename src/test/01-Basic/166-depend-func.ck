// dependency tracking for context-top-level variable
// ALSO SEE: 06-Errors/error-depend-func.ck

// no work here (it would skip 'array' initialization)
// printArray();

[1, 2, 3] @=> int array[];

// works here
printArray();

// function that needs array
fun void printArray()
{
   <<< array[0], array[1], array[2] >>>;
}
