// step gen to dac
step s => dac;
1.0 => float v;

// infinite time-loop
while( 1 )
{
    // advance time
    1::ms => now;
    // set value
    v => s.next;
    -v => v;
}
