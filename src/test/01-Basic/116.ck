// test static array references
class X
{
    static int a[];
    static float b[];
    // hah
    static int c[], d;
}

new int[5] @=> X.a;
new float[6] @=> X.b;
new int[7] @=> X.c;
8 => X.d;

<<< X.a.size(), X.b.size(), X.c.size(), X.d >>>;
