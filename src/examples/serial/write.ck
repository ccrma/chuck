
// list serial devices
SerialIO.list() @=> string list[];

// no serial devices available
if(list.cap() == 0)
{
    cherr <= "no serial devices available\n";
    me.exit(); 
}

// print list of serial devices
chout <= "Available devices\n";
for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// parse first argument as device number
0 => int device;
if(me.args()) me.arg(0) => Std.atoi => device;

if(device >= list.cap())
{
	cherr <= "serial device #" <= device <= "not available\n";
	me.exit(); 
}

// open serial device
SerialIO cereal;
if(!cereal.open(device, SerialIO.B9600, SerialIO.ASCII))
{
	chout <= "unable to open serial device '" <= list[device] <= "'\n";
	me.exit();
}

// pause to let serial device finish opening
2::second => now;

// loop forever
while(true)
{
    // write to serial device
    cereal <= "hi!\n";
    chout <= "<= hi!\n";
    
    // read one line
    cereal.onLine() => now;
    chout <= "=> " <= cereal.getLine() <= IO.nl();
    
    1::second => now;
}
