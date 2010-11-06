// quick survey of chuck primitives

// <<<>>> will print the type and value of any variable; 

// integers 
1 => int i; 
<<<i>>>;
// modify value and print again...
2 => i;
<<<i>>>;

// hexadecimal notation
0x22 => i;
<<<i>>>;

// floats 
5.678 => float f; 
<<<f>>>;

// durations are a measure of time 
// the value we print will be in audio samples. 

9.0::second => dur d; 
<<<d>>>;

now => time t; 
<<<t>>>;

// we advance time, and now will change to reflect 
<<<"waiting 10 seconds...">>>;
10::second => now;
<<<now>>>;
