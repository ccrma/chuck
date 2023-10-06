// error case: out of order class definitions; Y extends X, but
// Y is defined ahead of X; certain definitions can appear after
// usage or invocation, but for now this is not permitted


// define Y
class Y extends X
{
    public void foo() { }
}

// define X
class X
{
    public void foo() { }
    int y;
    dur z;
}

// instantiate a X
X x;

// instantiate a Y
Y y;

// shouldn't get here...
<<<"shouldn't get here...">>>;
