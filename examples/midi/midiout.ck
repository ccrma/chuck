MidiOut mout;
mout.open(0);

while(true)
{
    MidiMsg msg;
    
    0x90 => msg.data1;
    60 => msg.data2;
    127 => msg.data3;
    mout.send(msg);

    1::second => now;
    
    0x80 => msg.data1;
    60 => msg.data2;
    0 => msg.data3;
    mout.send(msg);
    
    1::second => now;
}
