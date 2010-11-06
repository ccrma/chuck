// white noise to audio output - cover your ears
noise n => Envelope ne => dac;
cnoise pink => Envelope pe => dac;
"pink" => pink.mode;
cnoise xor => Envelope xe => dac;
"xor" => xor.mode;
cnoise flip => Envelope fe => dac;
"flip" => flip.mode;

0.5 => ne.time;
0.5 => pe.time;
0.5 => xe.time;
0.5 => fe.time;

0.7 => xor.fprob;
// go!
while ( true ) { 
	<<<"white">>>;
	0.2 => ne.target;
	0.5::second => now;
	0.0 => ne.target;
	0.5::second => now;
	<<<"pink">>>;
	0.8 => pe.target;
	0.5::second => now;
	0.0 => pe.target;
	0.5::second => now;
	<<<"xor">>>;
	0.2 => xe.target;
	0.5::second => now;
	0.0 => xe.target;
	0.5::second => now;
	<<<"flip">>>;
	0.2 => fe.target;
	0.5::second => now;
	0.0 => fe.target;
	0.5::second => now;
}
