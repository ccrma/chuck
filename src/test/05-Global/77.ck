// test me.dir in conjunction with Machine.add

3 => global int num;
if(me.args())
    me.arg(0) => Std.atoi => num;

me.dir() => global string meDir;
me.dir(1) => global string meDir1;
me.dir(2) => global string meDir2;
me.dir(3) => global string meDir3;

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
