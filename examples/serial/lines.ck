
SerialIO.list() @=> string list[];

for(int i; i < list.cap(); i++)
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}

SerialIO cereal;
cereal.open(0, SerialIO.B9600, SerialIO.ASCII);

while(true)
{
    cereal.onLine().onLine() => now;
    chout <= "line1: " <= cereal.getLine() <= IO.newline();
    chout <= "line2: " <= cereal.getLine() <= IO.newline();
}
