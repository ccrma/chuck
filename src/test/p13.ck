// cast a subclass to a parent class

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
c @=> p;

<<<"success">>>;