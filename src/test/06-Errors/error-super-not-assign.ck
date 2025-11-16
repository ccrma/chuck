class Parent
{
    fun Parent()
    {
        <<< "Parent ctor" >>>;
    }
}

class Child extends Parent
{
    fun Child()
    {
        <<< "Child ctor" >>>;
    }

    fun void test()
    {
        <<< "Test start" >>>;

        // should throw compile error since super is not be assignable
        (this $ Parent) @=> super;
        
        <<< "Test complete" >>>;
    }

    fun void not_override()
    {
        <<< "Child not_override" >>>;
    }
}

Child child;
child.test();
