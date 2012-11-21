// assign one class to another ( fail-correct )

class ListItem
{
    public void foo() { }
    int index;
}

class ListValue { 
    int value;
}

ListItem item;
ListValue value;
item @=> value;

<<<"success">>>;