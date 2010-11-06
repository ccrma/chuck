// infinite time-loop
while( 50::ms => now )
    while( midiin => int i )  // print out midi message
        chout => "type: " => (i>>16)&0xff
              => " data1: " => ( (i>>8)&0xff )
              => " data2: " => ( i&0xff )
              => endl;
