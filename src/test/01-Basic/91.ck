// 91.ck : function with only default arguments

fun int add(0 => int xx, 2 => int yy)
{
    return xx + yy;
}

if (add(2, 3) == 5 && add(1) == 3 && add() == 2)
{
    <<<"success">>>;
}
