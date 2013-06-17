// Overloading
fun int add(int x)
{
    return (x+x);
}

fun int add(int x, int y)
{
    return (x+y);
}

fun float add(float x, int y)
{
    return (x+y);
}

fun float add(int x, float y)
{
    return (x+y);
}

// Main Program
add(1) => int a;
add(1,2) => int b;
add(1.423, 2) => float c;
add(3, 24.242) => float d;
<<<a,b,c,d>>>;
