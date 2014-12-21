// Listing 9.1 Making and using your first new class, testData

// Simple Class to show data access
public class testData {          // (1) Declares a public class called testData
    1 => int myInt;              // (2) Local int variable
    0.0 => float myFrac;         // (3) Local float variable
    // a simple function that adds the data
    fun float sum() {            // (4) Function to add the two
        return myInt + myFrac;
    }
}

// make one of our new testData objects
testData d;                          // (5) Tests it all by making a testData

<<< d.myInt, d.myFrac, d.sum() >>>;  // (6) Prints out default initial testData values

// change the data, just like we would variables
3 => d.myInt;
0.141592 => d.myFrac;

// check to show that everything changed
<<< d.myInt, d.myFrac, d.sum() >>>;
