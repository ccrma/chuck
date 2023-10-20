// verify vec3 2D array
vec3 v2d[16][4];

// indices
int a, b;

for( ; a < v2d.size(); a++ )
{
    for( 0 => b; b < v2d[a].size(); b++ )
    {
        a => v2d[a][b].x;
        b => v2d[a][b].y;
    }
}

// reset
0 => a => b;
// test auto 
for( auto v1d[] : v2d )
{
    // test not auto
    for( vec3 v : v1d )
    {
        <<< "[",a,"][",b,"]", v.x, v.y, v.z >>>;
        b++;
    }
    // update
    0 => b;
    a++;
}