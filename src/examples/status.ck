// useful little shred for on-the-fly programming...

// infinite time loop
while( true )
{
    // print out machine status (same as chuck ^)
    Machine.status();
    // every so often
    3::second => now;
}
