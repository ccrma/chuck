// cast parent class to subclass ( fail-correct )

class Parent
{
    public void foo() { }
    int index;
}

class Child extends Parent { 
    int value;
}

Parent p;
Child c;
p @=> c;

<<<"success">>>;