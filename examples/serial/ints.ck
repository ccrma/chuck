
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

SerialIO cereal;
cereal.open(0, SerialIO.B9600, SerialIO.ASCII);

while(true)
{
    cereal.onInts(2) => now;
    cereal.getInts() @=> int ints[];
    chout <= "ints: ";
    for(int i; i < ints.cap(); i++)
    {
        chout <= ints[i] <= " ";
    }
    
    chout <= IO.newline();
}
