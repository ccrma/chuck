// serial IO list
SerialIO.list() @=> string list[];
// print list
for( int i; i < list.size(); i++ )
{
    chout <= i <= ": " <= list[i] <= IO.newline();
}
