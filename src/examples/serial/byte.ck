
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

SerialIO cereal;
cereal.open(0, SerialIO.B9600, SerialIO.BINARY);

while(true)
{
    cereal.onByte() => now;
    cereal.getByte() => int byte;
    chout <= "byte: " <= byte <= IO.newline();
}
