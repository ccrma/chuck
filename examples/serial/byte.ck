// serial IO list
SerialIO.list() @=> string list[];
// print list
for( int i; i < list.size(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// serial device number
0 => int device;
if( me.args() ) me.arg(0) => Std.atoi => device;

// serial IO
SerialIO cereal;
// open serial
cereal.open( device, SerialIO.B9600, SerialIO.BINARY );

// infinite time-loop
while( true )
{
    // wait for byte
    cereal.onByte() => now;
    // get the byte
    cereal.getByte() => int byte;
    // print
    chout <= "byte: " <= byte <= IO.newline();
}
