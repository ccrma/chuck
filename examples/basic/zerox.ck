// step generator, zero crossing detector, dac
Step s => ZeroX z => dac;
1.0 => float v;

// infinite time-loop
while( true )
{
    // set the step value
    v => s.next;
    // change step value
    -v => v;
    // advance time
    100::ms => now;
}
