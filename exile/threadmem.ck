
PercFlut w => dac;
0.8 => w.gain;

fun void play() { 
	while ( true ) { 
		std.mtof ( (float)std.rand2( 60, 72 ) ) => w.freq;
		std.rand2f(0.6, 0.8) => w.noteOn;
		0.25::second  * (float)std.rand2(1,2) => now;
	}
}

spork ~ play() => int playid;

4::second => now;

//uncomment this line to exit safely..
//machine.remove(playid);

