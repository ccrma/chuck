// demo1.ck
// candidate for most trivial demo

0 => int a => int t;
1 => int b;
15 => int c;

while( c > 0 ) 
{
    a + b => t;
    b => a;
    <<<t => b>>>;
    c - 1 => c;
}
