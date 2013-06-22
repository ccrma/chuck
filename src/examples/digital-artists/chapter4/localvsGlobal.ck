// Functions
fun int MyAdd(int x, int y)
{
    // two inputs locally known as x and y
    // returns an Integer
    
    int result; // variable to store final answer
    x+y => result; // add input x to input y
    return result; // output final answer
    <<<k>>>; // can print k because Global Variable
}

// Main Program
MyAdd(8,12) => int Answer; // Call MyAdd Function
<<<Answer>>>; // print out Result of MyAdd
<<<result>>>; // Compile ERROR: result is local to MyAdd
