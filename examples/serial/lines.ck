// list of Serial IO
SerialIO.list() @=> string list[];
// print list
for( int i; i < list.size(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// parse first argument as device number
0 => int device;
if( me.args() ) me.arg(0) => Std.atoi => device;
if( device >= list.size() )
{
    cherr <= "serial device #" <= device <= " not available\n";
    me.exit(); 
}

// serial io
SerialIO cereal;
// open device
if( !cereal.open(device, SerialIO.B9600, SerialIO.ASCII) )
{
	chout <= "unable to open serial device '" <= list[device] <= "'\n";
	me.exit();
}

// infinite time-loop
while( true )
{
    // wait for event
    cereal.onLine() => now;
    // get line
    cereal.getLine() => string line;
    // print
    if( line$Object != null )
        chout <= "line: " <= line <= IO.newline();
}
