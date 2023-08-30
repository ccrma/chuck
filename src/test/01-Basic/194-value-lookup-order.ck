// this test value lookup when there are multiple variables of the same
// name declared in different situations
// 1) local scope: test2()'s `8000 int foo`
// 2) up scope within class def; second print of `foo` in test2()
// 3) inherited member (2000 A.foo)
// 3) up scope beyond class def (global-scope 4000 int foo)

class A 
{
    // member of A
    2000 => int foo;
}

// global-scope
4000 => int foo;

// subclass of A
class B extends A 
{
    {
        {
            // this should resolve to A.foo over the global-scope foo
            <<< "b", foo >>>;
        }
    }

    fun void test1()
    {
        {
            // this should resolve to A.foo over the global-scope foo
            <<< "b", foo >>>;
        }
    }
    
    fun void test2()
    {
        // local variable
        8000 => int foo;

        // this should resolve to the local variable foo
        <<< "b", foo >>>;
        {
            // this should resolve to the local variable foo
            <<< "b", foo >>>;
        }
    }
}

B b;

// don't use comparison here (b.foo) but instead
// observe output to verify value in specific contexts
b.test1();
b.test2();
