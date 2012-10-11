
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
    cereal.onLine().onLine() => now;
    chout <= "line1: " <= cereal.getLine() <= IO.newline();
    chout <= "line2: " <= cereal.getLine() <= IO.newline();
}
