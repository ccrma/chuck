// name: super-4-multilevel.ck
// desc: more comprehensive usage example of `super` keyword
//       across multiple levels of inheritance
// author: Niccolo, Fall 2025

class Base
{
    fun Base()
    {
        <<< "Base ctor" >>>;
    }

    fun void overridden()
    {
        <<< "Base overridden" >>>;
    }

    fun void not_overridden()
    {
        <<< "Base not_overridden" >>>;
    }

    fun void not_overridden_by_child()
    {
        <<< "Base not_overridden_by_child" >>>;
    }

    fun void bubble_up()
    {
        <<< "Base bubble_up" >>>;
    }
}

class Parent extends Base
{
    fun Parent()
    {
        <<< "Parent ctor" >>>;
    }

    fun void overridden()
    {
        <<< "Parent overridden" >>>;
    }

    fun void not_overridden_by_child()
    {
        <<< "Parent not_overridden_by_child" >>>;
    }

    fun void bubble_up()
    {
        <<< "Parent bubble_up" >>>;
        super.bubble_up();
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
        <<< "test start" >>>;

        // call parent contructor - seems to be allowed by ChucK so it should work with super as well
        <<< "super.Parent() - expecting parent ctor" >>>;
        super.Parent();
        // should be allowed by same logic
        <<< "super.Base() - expecting base ctor" >>>;
        super.Base();
        // should call base version of not_overridden (since no parent version exists / the base version is the parent version)
        <<< "super.not_overridden() - expecting base not_overridden" >>>;
        super.not_overridden();
        // should call parent version of overridden
        <<< "super.overridden() - expecting parent overridden" >>>;
        super.overridden();
        // should call child version of overridden
        <<< "this.overridden() - expecting child overridden" >>>;
        this.overridden();
        // should call parent version of function, not base version
        <<< "super.not_overridden_by_child() - expecting parent not_overridden_by_child" >>>;
        super.not_overridden_by_child();
        // should call child version of function, which will then bubble up to the base
        <<< "this.bubble_up() - expecting child bubble_up / parent bubble_up / base bubble_up" >>>;
        this.bubble_up();

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

    fun void bubble_up()
    {
        <<< "Child bubble_up" >>>;
        super.bubble_up();
    }
}

Child child;
child.test();
