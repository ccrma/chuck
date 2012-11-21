// multiple declarations
int a, b, c;

1 => a;
2 => b;
3 => c;

float fa, fb, fc;

2 => fa;
3*fa => fb;
fa + fb => fc;

public class X
{
    int i;
    float f;

    // <<< i, f >>>;
}

X xa, xb, xc;

class Y
{
    int a, b, c;
}

Y y;
1 => y.a;
2 => y.b;
3 => y.c;

class Z
{
    static int a, b, c;
}

4 => Z.a;
Z.a * 2 => Z.b;
Z.b * 2 => Z.c;

if( ( y.a + y.b + y.c == 6 ) &&
    ( c == ( a + b ) ) &&
    ( fc > 7.8 ) &&
    ( Z.a + Z.b + Z.c == 28 ) )
    <<< "success" >>>;
