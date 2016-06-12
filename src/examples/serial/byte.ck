
SerialIO.list() @=> string list[];

for(int i; i < list.size(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

0 => int device;
if(me.args()) me.arg(0) => Std.atoi => device;

SerialIO cereal;
cereal.open(device, SerialIO.B9600, SerialIO.BINARY);

while(true)
{
    cereal.onByte() => now;
    cereal.getByte() => int byte;
    chout <= "byte: " <= byte <= IO.newline();
}
