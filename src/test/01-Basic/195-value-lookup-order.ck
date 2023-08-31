// github era issue #6 (2014)
// https://github.com/ccrma/chuck/issues/16
// finally resolved (2023)

// should print three 3000s in B.bar(),
// value lookup should prioritize local variable foo over
// A.foo (1000) and the global-scope foo (2000)

// class A
class A 
{
    // member var
    1000 => int foo;
}

// global scope var of same name
2000 => int foo;

// class B subclass of A
class B extends A 
{
    // function
    fun void bar()
    {
        // local var
        3000 => int foo;
        
        // 1000
        <<< "a", foo >>>;
        {
            // 1000
            <<< "b", foo >>>;
        }
        // 1000
        <<< "c", foo >>>;
    }
}

B b;
b.bar();
