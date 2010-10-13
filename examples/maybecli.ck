// maybe click, maybe not

impulse i => dac;

// infinite time-loop
while( true )
{
    if( maybe ) 1.0 => i.next;
    50::ms => now;
}
