// 27.ck : 26, float

class X
{
    float f;
}

class Y
{
    X x;
    fun X foo() { return x; };
}

Y y;
12.2 => y.x.f;

if( y.foo().f == 12.2 ) <<<"success">>>;
