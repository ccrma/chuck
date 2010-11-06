OSC_Recv recv;

6448 => recv.port;
recv.listen();

<<<"string receive test started">>>;
function void trigger_s () { 
	<<<"function started">>>;
	recv.event( "/foo/zoop,s" ) @=> OSC_Addr oscev;
	while ( true  ) { 
		oscev => now;	
		<<<"Event occurred">>>;
		while ( oscev.nextMesg() != 0 ) { 
			<<<"fetching string">>>;
			string c;
			oscev.getString() => c;	
			<<<"zoop">>>;	
			<<<c>>>;
		}
	}
}

spork ~ trigger_s();


while ( true ) { 
	1::second => now;
}

