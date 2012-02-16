//--------------------------------------|
// Dyno demo 3: "Ducker? She didn't even throw a punch!"
// Shows how to make one sound fade out when another comes up. This is 
// probably more often used by radio DJs than electronic musicians, but
// what the hey.
//
// authors: Matt Hoffman (mdhoffma at cs.princeton.edu)
//--------------------------------------|

// A kick drum that will get quieter when the SinOsc swells.
SndBuf buf => Gain g1 => Dyno dy => JCRev r => dac;
me.sourceDir() + "/../data/kick.wav" => buf.read;
.05 => r.mix;

// A SinOsc that, when its volume swells, will make the kick drum get quieter.
SinOsc s => Gain g2 => r;
220 => s.freq;

// Make dy go into ducker mode
dy.duck();

// spork the shred that makes the kick kick.
spork ~ playKick();

// th is the phase of the low-freq sine that modulates the SinOsc's gain
0 => float th;
// make the SinOsc a bit less annoying
0.5 => g2.gain;

// loop
while( true )
{
	// modulate the SinOsc's gain
	Math.sin(th) + 1.0 => s.gain;
	// send the SinOsc's last sample to dy (manually. yes, this is a pain.)
	s.last() => dy.sideInput;
	// advance the modulating sine's phase a tiny bit
	th + 0.00003 => th;
	// advance time by one sample.
	1::samp => now;
}

// retriggers the kick drum over and over and over and over and over.
fun void playKick()
{
	while(true)
    {
		0 => buf.pos;
		500::ms => now;
	}
}
