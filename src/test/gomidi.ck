
MidiIn min;
MidiMsg msg;

if( !min.open( 0 ) )
{
    <<<"gomidi error: wrong port specified...">>>;
    me.exit();
}

// infinite time-loop
while( true )
{
    min => now;
    while( min.recv(msg) )
    {
        // print out midi message
        <<<msg.data1, msg.data2, msg.data3>>>;
    }
}
