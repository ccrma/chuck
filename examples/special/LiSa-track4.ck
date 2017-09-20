//ugens
SndBuf buf;
LiSa loopme => dac;

//change this path to your own sample
"/Users/dan/Files/Chuck/LiSa_examples/TomVega.wav" => buf.read;

//set lisa buffer size to sample size
buf.samples()::samp => loopme.duration;

//transfer values from SndBuf to LiSa
for ( 0 => int i; i < buf.samples(); i++ ) {

	loopme.valueAt(buf.valueAt(i), i::samp);

}

//set sync/track mode to 1, where the input chooses playback position
1 => loopme.sync;


//use an oscillator to set playback position
SinOsc tracker => loopme;
Step off => loopme;

//tracker parameters; will determine how far into the buffer
//to go (gain), with what offset (off) and with what speed (freq)
0.05 => tracker.freq;
0.2 => tracker.gain;
0.3 => off.next;

//play it back
1   => loopme.play;
0.5 => loopme.gain;

while ( true ) { 1::second => now; }