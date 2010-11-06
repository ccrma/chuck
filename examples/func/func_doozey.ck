// adapted from Mike McGonagle's bug report

class B1
{
    fun void f(float f[])
    {
        <<< "in B1.f", "" >>>;
    }
}

class E1 extends B1
{
    fun void f(float x, float y, float z)
    {
        <<< "in E1.f", "" >>>;
    }
}

class B2
{
    fun void f(float x, float y, float z)
    {
        <<< "in B2.f", "" >>>;
    }
}

class E2 extends B2
{
    fun void f(float f[])
    {
        <<< "in E2.f", "" >>>;
    }
}

B1 b1;
B2 b2;
E1 e1;
E2 e2;

[ 0.1, 0.3, 0.7 ] @=> float myArray[];

b1.f( myArray );       // in B1.f - correct
e1.f( myArray );       // in B1.f - correct
e1.f( 0.7, .553, 77 ); // in E1.f - correct
b2.f( 0.7, .553, 77 ); // in B2.f - correct
e2.f( 0.7, .553, 77 ); // in B2.f - correct
e2.f( myArray );       // in E2.f - correct

