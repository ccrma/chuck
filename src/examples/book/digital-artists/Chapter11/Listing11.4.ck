// Listing 11.4 Simple OSC receiver

// Make a receiver, set port#, set up to listen for event
OscIn oin;                   // (1) Makes an OSC input object
6449 => oin.port;            // (2) Port number to receive OSC on
OscMsg msg;                  // (3) Makes an OSC message holder

// create an address in the receiver, store in new variable
oin.addAddress( "/myChucK/OSCNote" );  // (4) Sets beginning of message to listen for

// Our synthesizer to be controlled by sender process
Rhodey piano => dac;         // (5) Rhodey piano to play with received OSC messages

// Infinite loop to wait for messages and play notes
while (true)                 // (6) Infinite loop
{
    // OSC message is an event, chuck it to now
    oin => now;              // (7) Sleeps until OSC received

    // when event(s) received, process them
    while (oin.recv(msg) != 0) { // (8) Deals with potentially multiple messages
        // peel off integer, float, string
        msg.getInt(0) => int note;         // (9) Gets note number 
        msg.getFloat(1) => float velocity; // (10) Gets velocity
        msg.getString(2) => string howdy;  // (11) Gets string

        // use them to make music
        Std.mtof(note) => piano.freq;      // (12) Sets piano frequency from note
        velocity => piano.gain;            // (13) Sets gain for noteOff
        velocity => piano.noteOn;          // (14) Prints out message received from sender via OSC.

        // print it all out
        <<< howdy, note, velocity >>>;
    }
}

