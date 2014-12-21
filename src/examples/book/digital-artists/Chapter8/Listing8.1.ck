//Listing 8.1 Basic concurrency example, print with sound

// function foo
fun void foo()
{                                   // (1) Defines a function foo.
    // sound for foo process
    Impulse foo => ResonZ rez => dac.left;
    800.0 => rez.freq;              // (2) foo will make pops at 800 Hz
    50 => rez.Q;
    // infinite time loop
    while( true )
    {
        // print and make sound
        <<< "foo!", "" >>>;         // (3) ...and print itself out...
        100 => foo.next;
        // advance time
        250::ms => now;             // (4) ...every 1/4 second.
    }
}

fun void bar() // function bar
{                                   // (5) Function bar...
    // sound for bar process
    Impulse bar => ResonZ rez => dac.right;
    700.0 => rez.freq;              // (6) ...will make pops at 700 Hz...
    50 => rez.Q;
    while( true ) // infinite time loop
    {
        // print and make sound
        <<< "BAAAAAAAR!!!", "" >>>;  // (7) ...and print out something different from foo...
        100 => bar.next;
        // advance time
        1::second => now;            // (8) ...every second.
    }
}

// spork foo() and bar() each on a child shred
spork ~ foo();                    // (9) Starts foo by sporking it.
spork ~ bar();                    // (10) Starts bar by sporking it.

// infinite time loop to serve as a parent shred
while( true ) 1::second => now;   // (11) Infinite parent loop, so 
                                  //      children foo and bar can run.
