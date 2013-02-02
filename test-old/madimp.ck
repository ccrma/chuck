impulse i => dac;

int count;

while( true )
{
    1000 => count;
    while( count > 0 )
    {
        1.0 => i.next;
        <<< 1.0 / count >>>;
        (1.0/count)::second => now;
        count - 1 => count;
    }
}
