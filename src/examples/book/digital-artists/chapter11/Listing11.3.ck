// Listing 11.3 Simple OSC sender

// First, make an OSC send object and set port #
OscOut xmit;                           // (1) Makes an OSC output object.
6449 => int port;                      // (2) Port number to send OSC on.
xmit.dest ( "localhost", port );       // (3) Sets network destination to this computer.

// infinite time loop
while( true )                          // (4) Infinite loop.
{
    // start the message, after prefix (our choice)
    // expects one int, one float, one string
    xmit.start( "/myChucK/OSCNote" );  // (5) Starts OSC output.

    Math.random2(48,80) => int note;   // (6) Computes a random int note
    Math.random2f(0.1,1.0) => float velocity; // (7) Computes a random float velocity
    "Hi out there!!" => string message; // (8) Makes a string message

    // a message is kicked as soon as it is complete
    // - type string is satisfied and bundles are closed
    note => xmit.add;                  // (9) Adds note to output.
    velocity => xmit.add;              // (10) Adds velocity to output.
    message => xmit.add;               // (11) Adds string to output.
    xmit.send();                       // (12) Send it! 

    // hang a bit, then do it again
    0.2 :: second => now;
}
