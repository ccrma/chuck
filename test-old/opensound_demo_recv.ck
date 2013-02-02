// the patch 
// launch with opensound_demo_send.ck
sndbuf buf => dac;

//create our OSC receiver
OSC_Recv orec;
6449 => orec.port;  //use port 6449
orec.listen(); //start listening ( launches thread ) 

function void rate_control_Shred() { 
	//create an address in the receiver 
	//and store it in a new variable.\
	orec.event("/sndbuf/buf/rate,f") @=> OSC_Addr rate_addr; 
	while ( true ) { 

		rate_addr => now; //wait for Events to arrive.

		//grab the next message from the queue. 
		while ( rate_addr.nextMesg() != 0 ) { 
			//getFloat fetches the expected float
			//as indicated in the type string ",f"
			buf.play( rate_addr.getFloat() );
			0 => buf.pos;
		}
	}	
}

// load the file
"data/snare.wav" => buf.read;
0 => buf.loop;
0.0 => buf.play; 

// start our listening shred
spork ~ rate_control_Shred();

// time loop
while( true )
{
	1::second => now;
}
