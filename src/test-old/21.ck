// 21.ck : classes inside classes

// X
class X
{
    float f;
    int i;
}

// Y has an X
class Y
{
    X x;
}

// foo that takes a Y
fun void foo( Y y )
{
    // assignment
    10.0 => y.x.f;
    // assignment
    1 => y.x.i;
}

// instantiate a Y
Y y;

// call
foo( y );

// assign reference
y.x @=> X @ x;

// test
if( x.f == 10.0 && x.i == 1 ) <<<"success">>>;
