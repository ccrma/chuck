// name: super-3-singlelevel.ck
// desc: more comprehensive usage example of `super` keyword
//       in single level of inheritance 
// author: Niccolo, Fall 2025

class Parent
{
    1 => int parent_var;

    fun Parent()
    {
        <<< "Parent ctor" >>>;
    }

    fun void overridden()
    {
        <<< "Parent overridden" >>>;
    }

    fun void not_overridden()
    {
        <<< "Parent not_overridden" >>>;
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

        // call parent contructor - seems to be allowed by ChucK so it should work with super as well
        <<< "super.Parent() - expecting parent ctor" >>>;
        super.Parent();
        // should call parent version of not_overridden - need to test in 3 generation setup
        <<< "super.not_overridden() - expecting parent not_overridden" >>>;
        super.not_overridden();
        // should call parent version of overridden
        <<< "super.overridden() - expecting parent overridden" >>>;
        super.overridden();
        // should call child version of overridden
        <<< "this.overridden() - expecting child overrridden" >>>;
        this.overridden();

        // should access the child variable (same as this)
        <<< "super.parent_var - expecting 1" >>>;
        <<< super.parent_var >>>;
        
        <<< "test complete" >>>;
    }

    fun void overridden()
    {
        <<< "Child overridden" >>>;
    }

    fun void not_override()
    {
        <<< "Child not_override" >>>;
    }
}

Child child;
child.test();
