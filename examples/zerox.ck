// step generator, zero crossing detector, dac
step s => zerox z  => dac;
1.0 => float v;

// infnite time-loop
while( true )
{
    // set the step value
    v => s.value;
    // change step value
    -v => v;
    // advance time
    100::ms => now;
}
