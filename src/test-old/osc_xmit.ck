
OSC_Send xmit;
xmit.setHost("localhost", 6999 );

xmit.openBundle(now);
xmit.startMesg( "/hello_world", "," );

xmit.openBundle(now);
xmit.startMesg( "/hello_float", ",sif" );
xmit.addString( "rats" );
xmit.addInt( 1 );
xmit.addFloat( 1.0 );
xmit.closeBundle();
xmit.startMesg( "/hello_world", "," );
xmit.closeBundle();

<<<"goodbye">>>;