// test functions that access variables declared after the function
// definition; this is (correctly) not caught by the dependency
// system, which assumes function defs to be order independent
//
// this was an issue that was fixed in 1.5.4.4 
// https://github.com/ccrma/chuck/issues/482

// func def
fun void foo()
{
    // access vars
    <<< bar, boo >>>;
    // write to a var
    3 => boo;
}

// the vars
1 => int bar;
2 => int boo;

// call the function
foo();

// print after function call
<<< bar, boo >>>;