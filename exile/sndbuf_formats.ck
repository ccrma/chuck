sndbuf buf => dac;

1.0::second => dur plen;

fun void play_file_once( string name ) { 
	name => stdout;
	name => buf.read;
	"samples" => stdout;
	buf.samples => stdout;
	"length" => stdout;
	buf.length => stdout;
	"channels" => stdout;
	buf.channels => stdout;
	0 => buf.pos;
	1 => buf.interp;
	0 => buf.channel;
	1.0 => buf.rate;
	1::second * buf.length => now;
}

play_file_once("espresso.wav");

play_file_once("prairiedawn_mercy.aiff");

play_file_once("puzzler.aiff");

play_file_once("puzzler_8b_11k.aiff");

play_file_once("puzzler.wav");

play_file_once ("puzzler.au");

"end" => stdout;