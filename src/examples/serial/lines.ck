
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

// parse first argument as device number
0 => int device;
if(me.args()) me.arg(0) => Std.atoi => device;

if(device >= list.cap())
{
    cherr <= "serial device #" <= device <= " not available\n";
    me.exit(); 
}

SerialIO cereal;
if(!cereal.open(device, SerialIO.B9600, SerialIO.ASCII))
{
	chout <= "unable to open serial device '" <= list[device] <= "'\n";
	me.exit();
}

while(true)
{
    cereal.onLine() => now;
    cereal.getLine() => string line;
    if(line$Object != null)
        chout <= "line: " <= line <= IO.newline();
}
