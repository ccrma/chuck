
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

SerialIO cereal;
cereal.open(0, SerialIO.B9600, SerialIO.ASCII);

2::second => now;

while(true)
{
    cereal <= "hi!\n";
    
    cereal.onLine() => now;
    <<< cereal.getLine() >>>;
    
    1::second => now;
}
