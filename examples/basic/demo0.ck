// demo0.ck
// basic demo showing time and duration

5::second + now => time later;

while( now < later )
{
    <<<now>>>;
    1::second => now;
}

<<<now>>>;
