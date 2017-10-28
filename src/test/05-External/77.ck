// test me.dir in conjunction with Machine.add

3 => external int num;
if(me.args())
    me.arg(0) => Std.atoi => num;

me.dir() => external string meDir;
me.dir(1) => external string meDir1;
me.dir(2) => external string meDir2;
me.dir(3) => external string meDir3;

if(meDir1 + "05-External/" != meDir)
{
    <<< "failure", meDir1, "!=", meDir >>>;
    me.exit();
}

if(meDir2 + "test/05-External/" != meDir)
{
    <<< "failure", meDir2, "!=", meDir >>>;
    me.exit();
}

if(meDir3 + "src/test/05-External/" != meDir)
{
    <<< "failure", meDir3, "!=", meDir >>>;
    me.exit();
}

if(num > 0)
{
    me.dir(-1)+"/05-External/77.ck:"+(num-1) => string add;
    Machine.add(add);
}
else
{
    <<< "success" >>>;
}
