// Radio Drum Mapping 1
// Whack mode
// z controls gain
// x controls pitch
// y controls effect mix (reverb)

ModalBar synth1 => gain g1 => Chorus rev1 => dac;
ModalBar synth2 => gain g2 => Chorus rev2=> dac; 

MidiIn min;
MidiMsg msg;

// open midi port
if ( !min.open( 0 ) )
   me.exit;

.3 => rev1.modFreq;
.7 => rev2.modFreq;

.3 => rev1.modDepth;
.7 => rev2.modDepth;

-1 => rev1.op;
-1 => rev2.op;



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
			   (msg.data3/127.0) => synth1.noteOn;
			}	
			if (msg.data2 == 15)
			{
			   std.mtof(127 - msg.data3) => synth1.freq;
			}
			if (msg.data2 == 16)
			{
			   (msg.data3/127.0) => rev1.mix;
			}	

			// right stick whack -- vel, x, y
			if (msg.data2 == 2)
			{
			   (msg.data3/127.0)*15. => g2.gain;
			   (msg.data3/127.0) => synth2.noteOn;
			}	
			if (msg.data2 == 17)
			{
			   std.mtof(127 - msg.data3) => synth2.freq;
			}
			if (msg.data2 == 18)
			{
			   (msg.data3/127.0) => rev2.mix;
			}
		
		}
	}

}