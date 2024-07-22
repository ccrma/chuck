//----------------------------------------------------------------------------
// name: midi2osc-r.ck
// desc: starter code for receiving OSC from midi2osc
//
// author: Ge Wang (https://ccrma.stanford.edu/~ge/)
// date: Spring 2024
//----------------------------------------------------------------------------

// create our OSC receiver
OscIn OIN;
// create our OSC message
OscMsg MSG;
// use port 6449 (or whatever)
6449 => OIN.port;

// received INT
global int R_INT;
// receved FLOATS
global float R_FLOAT1;
global float R_FLOAT2;
global float R_FLOAT3;

// allow any address
OIN.addAddress( "*" );

// infinite event loop
while( true )
{
    // wait for event to arrive
    OIN => now;

    // grab the next message from the queue. 
    while( OIN.recv(MSG) )
    {
        // print stuff (useful for debugging)
        // cherr <= "received OSC message: \"" <= MSG.address <= "\" "
        //       <= "typetag: \"" <= MSG.typetag <= "\" "
        //       <= "arguments: " <= MSG.numArgs() <= IO.newline();

        // check message and typetag
        if( MSG.address == "/foo/control/knob" && MSG.typetag == "if" )
        {
            // fetch the first data element as int
            MSG.getInt(0) => R_INT;
            // fetch the second data element as float
            MSG.getFloat(1) => R_FLOAT1;

            // print
            cherr <= "received OSC: " <= R_INT <= " " <= R_FLOAT1 <= IO.newline();
        }
        // check message and typetag
        else if( MSG.address == "/foo/control/button" && MSG.typetag == "if" )
        {
            // fetch the first data element as int
            MSG.getInt(0) => R_INT;
            // fetch the second data element as float
            MSG.getFloat(1) => R_FLOAT1;
            
            // print
            cherr <= "received OSC: " <= R_INT <= " " <= R_FLOAT1 <= IO.newline();
        }
    }
}

