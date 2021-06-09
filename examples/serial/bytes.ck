// serial IO list
SerialIO.list() @=> string list[];
// print list
for( int i; i < list.size(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// serial io
SerialIO cereal;
// open serial
cereal.open( 0, SerialIO.B9600, SerialIO.BINARY );

// infinite time-loop
while( true )
{
    // wait on bytes
    cereal.onBytes(4) => now;
    // get bytes
    cereal.getBytes() @=> int bytes[];
    // print
    chout <= "bytes: ";
    for( int i; i < bytes.size(); i++ )
    {
        chout <= bytes[i] <= " ";
    }
    // output endline
    chout <= IO.newline();
}
