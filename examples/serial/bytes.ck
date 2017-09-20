
SerialIO.list() @=> string list[];

for(int i; i < list.size(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

SerialIO cereal;
cereal.open(0, SerialIO.B9600, SerialIO.BINARY);

while(true)
{
    cereal.onBytes(4) => now;
    cereal.getBytes() @=> int bytes[];
    chout <= "bytes: ";
    for(int i; i < bytes.size(); i++)
    {
        chout <= bytes[i] <= " ";
    }
    
    chout <= IO.newline();
}
