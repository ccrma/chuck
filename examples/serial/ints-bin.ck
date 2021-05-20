// serial IO list
SerialIO.list() @=> string list[];
// print list
for( int i; i < list.size(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// device number
0 => int device;
if( me.args() ) me.arg(0) => Std.atoi => device;
// serial io
SerialIO cereal;
// open serial device
cereal.open( device, SerialIO.B9600, SerialIO.BINARY );

// infinite time-loop
while( true )
{
    // wait on event
    cereal.onInts(1) => now;
    // get int
    cereal.getInts() @=> int i[];
    // print
    if( i.size() > 0 )
        chout <= "int: " <= i[0] <= "\n";
}
