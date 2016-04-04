// test me.dir in conjunction with Machine.add

3 => int num;
if(me.args())
    me.arg(0) => Std.atoi => num;

me.dir() => string meDir;
me.dir(1) => string meDir1;
me.dir(2) => string meDir2;
me.dir(3) => string meDir3;

if(meDir1 + "01-Basic/" != meDir)
{
    <<< "failure", meDir1, "!=", meDir >>>;
    me.exit();
}

if(meDir2 + "test/01-Basic/" != meDir)
{
    <<< "failure", meDir2, "!=", meDir >>>;
    me.exit();
}

if(meDir3 + "src/test/01-Basic/" != meDir)
{
    <<< "failure", meDir3, "!=", meDir >>>;
    me.exit();
}

if(num > 0)
{
    me.dir(-1)+"/01-Basic/77.ck:"+(num-1) => string add;
    Machine.add(add);
}
else
{
    <<< "success" >>>;
}
