// test me.dir in conjunction with Machine.add

3 => global int num;
if(me.args())
    me.arg(0) => Std.atoi => num;

me.dir() => string meDir;
me.dir(1) => string meDir1;
me.dir(2) => string meDir2;
me.dir(3) => string meDir3;

if(meDir1 + "05-Global/" != meDir)
{
    <<< "failure", meDir1, "!=", meDir >>>;
    me.exit();
}

if(meDir2 + "test/05-Global/" != meDir)
{
    <<< "failure", meDir2, "!=", meDir >>>;
    me.exit();
}

if(meDir3 + "src/test/05-Global/" != meDir)
{
    <<< "failure", meDir3, "!=", meDir >>>;
    me.exit();
}

if(num > 0)
{
    me.dir(-1)+"/05-Global/77.ck:"+(num-1) => string add;
    Machine.add(add);
}
else
{
    <<< "success" >>>;
}
