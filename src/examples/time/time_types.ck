// 05.ck : time, now, duration

// time + duration
now + 5::second => time later;

// time loop
while( now < later )
{
    // print out the time
    <<<now>>>;
    // advance time
    1::second => now;
}

<<<"success">>>;
