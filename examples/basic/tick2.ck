// tick tock

// wait until the start of the next second
second - (now % second) => now;

// infinite time loop
while( true )
{
    // print now in seconds
    <<< "tick:", (now / second) $ int, "seconds" >>>; 
    1::second => now;
}
