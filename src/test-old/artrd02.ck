// Radio Drum Mapping 1
// Whack mode
// z controls gain
// x controls pitch
// y controls effect mix (reverb)

StifKarp synth1 => gain g1 => JCRev rev1 => dac;
StifKarp synth2 => gain g2 => JCRev rev2=> dac; 

MidiIn min;
MidiMsg msg;

// open midi port
if ( !min.open( 0 ) )
   me.exit;

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
			   (msg.data3/127.0)*2. => g1.gain;
			   (msg.data3/127.0) => synth1.noteOn;
			}	




			// right stick whack -- vel, x, y
			if (msg.data2 == 11)
			{

				if (msg.data3 < 50)	
			  	30. => synth1.freq;

				if (msg.data3 > 50)	
			  	10. => synth1.freq;
			}	
		
		
		}
	}

}

