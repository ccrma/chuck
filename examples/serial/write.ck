
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

if(list.cap() == 0)
{
	chout <= "no serial devices available\n";
	me.exit(); 
}

SerialIO cereal;
if(!cereal.open(0, SerialIO.B9600, SerialIO.ASCII))
{
	chout <= "unable to open serial device '" <= list[0] <= "'\n";
	me.exit();
}

// pause to let serial device finish opening
2::second => now;

while(true)
{
    cereal <= "hi!\n";
    
    cereal.onLine() => now;
    <<< cereal.getLine() >>>;
    
    1::second => now;
}
