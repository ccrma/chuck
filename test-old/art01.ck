MidiIn min;
MidiMsg msg;

if( !min.open( 0 ) )
    <<<"bad">>>;

while( true)
{
    min => now;
    while( min.recv( msg ) )
    {
	<<<msg.data1,msg.data2,msg.data3>>>;
	<<<"-----------">>>;
    }

}