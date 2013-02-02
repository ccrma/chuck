// class with a declared, undefined class as member ( fail-correct )

class ListValue;

class ListItem
{
    public void foo() { }
    ListValue myval;
    int index;
}

class ListValue { 
    int value;
}

ListItem root;

<<<"success">>>;