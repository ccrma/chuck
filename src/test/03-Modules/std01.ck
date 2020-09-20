
[[1, 0, 10, 1],
 [5, 0, 10, 5],
 [-1, 0, 10, 0],
 [15, 0, 10, 10]]
@=> int t1[][]; // Std.clamp
 
[[0.5, 0, 1, 0.5],
 [2.0, 0, 1, 1],
 [-10.0, 0, 1, 0],
 [0.0, 0, 1, 0],
 [1.0, 0, 1, 1]]
@=> float t2[][]; // Std.clampf

[[0.0, 1.0],
 [-20.0, 0.1],
 [20.0, 10],
 [-60.0, 0.001]]
@=> float t3[][]; // Std.dbtolin

[[1.0, 0.0],
 [0.1, -20.0],
 [10.0, 20.0],
 [0.001, -60.0]]
@=> float t4[][]; // Std.lintodb

for(int i; i < t1.size(); i++)
{
    if(Std.clamp(t1[i][0], t1[i][1], t1[i][2]) != t1[i][3])
    {
        <<< "failure" >>>;
        me.exit();
    }
}

for(int i; i < t2.size(); i++)
{
    if(Std.clampf(t2[i][0], t2[i][1], t2[i][2]) != t2[i][3])
    {
        <<< "failure" >>>;
        me.exit();
    }
}

for(int i; i < t3.size(); i++)
{
    if(Std.dbtolin(t3[i][0]) != t3[i][1])
    {
        <<< "failure" >>>;
        me.exit();
    }
}

for(int i; i < t4.size(); i++)
{
    if(Std.lintodb(t4[i][0]) != t4[i][1])
    {
        <<< "failure" >>>;
        me.exit();
    }
}

<<< "success" >>>;

