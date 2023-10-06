// this verifies that inheritance parent type is recognized if
// use before the class definition in file
// https://github.com/ccrma/chuck/issues/375

// instance of our event
TheEvent e;

// spork a shred to broadcast later
spork ~ timer( 1::samp );

// verify e can be recognized as child of Event,
// before TheEvent definition
e => now;

// if we got here, then done
<<< "success" >>>;

// custom Event class
class TheEvent extends Event { }

// a function to wait a bit and broadcast
fun void timer( dur T )
{
    T => now;
    e.broadcast();
}
