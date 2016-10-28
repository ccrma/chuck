// 93.ck : functions with normal and default arguments

fun int foo(int w; 0 => int x, 2 => int y)
{
    return w + x + y;
}

fun int bar(int w, int x; 5 => int y, 10 => int z)
{
    return w + x + y + z;
}

if (foo(4, 5, 6) == 15 && 
    foo(2, 3) == 7 && 
    foo(1) == 3 &&
    bar(4, 5, 6, 7) == 22 &&
    bar(2, 3, 4) == 19 &&
    bar(0, 1) == 16)
{
    <<<"success">>>;
}
