
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

0 => int device;
if(me.args()) me.arg(0) => Std.atoi => device;

SerialIO cereal;
cereal.open(device, SerialIO.B9600, SerialIO.BINARY);

while(true)
{
    cereal.onInts(1) => now;
    cereal.getInts() @=> int i[];
    if(i.size() > 0)
        chout <= "int: " <= i[0] <= "\n";
}
