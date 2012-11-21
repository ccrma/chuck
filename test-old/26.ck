// 26.ck : member functions that return object

// delcare class
class X
{
    int i;
}

// declare another class
class Y
{
    // declare a X as a member of Y
    X x;

    // return x through a function
    fun X foo() { return x; };
}

// instantiate a Y
Y y;

// assign to y's x's field i
1 => y.x.i;

// get x through the function and test the value
if( y.foo().i != 1 ) <<<"fail">>>;
else <<<"success">>>;
