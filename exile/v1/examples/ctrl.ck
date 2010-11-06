// loops are useful

"" => stdout;
"while 1" => stdout;
0 => int i;
while( i < 5 )
{
    ++i => stdout;
}

"" => stdout;
"while 2" => stdout;
0 => i;
while( i < 5 )
    ++i => stdout;

"" => stdout;
"until 1" => stdout;
0 => i;
until( i >= 5 )
    ++i => stdout;

"" => stdout;
"until 2" => stdout;
0 => i;
until( 5 <= i )
    ++i => stdout;

"" => stdout;
"for 1" => stdout;
for( 0 => i; i < 5; i++ )
    i => stdout;

"" => stdout;
"for 2" => stdout;
0 => i;
for( ; i < 5; i++ )
    i => stdout;

"" => stdout;
"for 3" => stdout;
0 => i;
for( ; i < 5; )
    ++i => stdout;

"" => stdout;
"for 3" => stdout;
1 => i;
for( ; i < 64; 2*i => i )
    i => stdout;

