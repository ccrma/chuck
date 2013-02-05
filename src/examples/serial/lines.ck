
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

while(true)
{
    cereal.onLine() => now;
    chout <= "line: " <= cereal.getLine() <= IO.newline();
}
