// cast a subclass array to a parent class array

class Parent
{
    public void foo() { }
    int index;
}

class Child extends Parent { 
    int value;
}

Parent p[4];
Child c[4];
c @=> p;

<<<"success">>>;