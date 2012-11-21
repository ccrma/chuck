noise synth1 => gain g1 => ADSR win1 => Chorus rev1 => dac;
noise synth2 => gain g2 => ADSR win2 => Chorus rev2=> dac; 

MidiIn min;
MidiMsg msg;

// open midi port
if ( !min.open( 0 ) )
   me.exit;

-1 => rev1.op => rev2.op;

fun void win1time(){
.05::second => now;
1 => win1.keyOff;

}


fun void win2time(){
.05::second => now;
1 => win2.keyOff;
}

while ( true )
{
	min => now;
	while ( min.recv ( msg ) )
	{

		if (msg.data1 == 160)
		{


			// left stick whack  -- vel, x, y
			if (msg.data2 == 1)
			{
			   // z-axis output
			   (msg.data3/127.0)*10. => g1.gain;
			//   (msg.data3/127.0) => synth1.noteOn;

			   1 => win1.keyOn;
			   spork ~ win1time();

			}	
			if (msg.data2 == 15)
			{
		//	   std.mtof(127 - msg.data3) => synth1.freq;
			}
			if (msg.data2 == 16)
			{
			   (msg.data3/127.0) => rev1.mix;
			}	

			// right stick whack -- vel, x, y
			if (msg.data2 == 2)
			{
			   (msg.data3/127.0)*15. => g2.gain;
			//   (msg.data3/127.0) => synth2.noteOn;
			 
			   1 => win2.keyOn;
  			   spork ~ win2time();

			}	
			if (msg.data2 == 17)
			{
		//	   std.mtof(127 - msg.data3) => synth2.freq;
			}
			if (msg.data2 == 18)
			{
			   (msg.data3/127.0) => rev2.mix;
			}
		
		}
	}

}