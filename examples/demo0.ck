5::second + now => time later;

while( now < later )
{
    now => stdout;
    1::second => now;
}

now => stdout;
