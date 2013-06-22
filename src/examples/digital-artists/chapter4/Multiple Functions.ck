// Global Variables
0 => int x;

// Functions 
fun void FunOne()
{
    x+1 => x;  // Add one to x
    1::second => now; // advance time
    <<<x>>>; // print x
}

fun void FunTwo()
{
    x-1 =>x;  // Subtract one from x
    1::second => now; // advance time
    <<<x>>>; // print x
}

// Main Program
while (true)
{
    FunOne(); // Execute FunOne
    FunTwo(); // Wait until FunOne() complete
              // then execute FunTwo()
}

