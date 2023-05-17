////////////////////////////////////////////////////////////////////
// PitchTrack is a monophonic autocorrelation pitch tracker with  //
// a fast response and extremely high accuracy, even at low       //
// frequencies. It is adapted from [helmholtz~] for Pd by Katja,  //
// documented at http://www.katjaas.nl/helmholtz/helmholtz.html   //
////////////////////////////////////////////////////////////////////

// Options
// get(): (read only) get calculated frequency
//
// fidelity: (float) [0-1], default 0.95
//   This is a threshold for certainty about the result. A highly periodic
//   signal (ie one that has a strong pitch center) should produce a result
//   with a high fidelity, which a non-periodic signal (eg noise) will have
//   a very low fidelity. Setting this parameter close to 1 should reduce
//   the number of inaccurate reports.
//
// sensitivity (float) [0-1], default 0.003
//   This is the minimum RMS value to trigger a pitch calculation. Setting
//   this parameter low forces PitchTrack to attempt to find the pitch of
//   even very quiet sounds. Higher values will cause it to trigger only
//   on louder notes.
//
// overlap (int) [1-?], default 2
//   How much to overlap successive analysis frames. Higher values should
//   produce smoother values, at the cost of an increase of CPU load.
//
// frame (int) [128-?], default 2048
//   Size of FFT frame for analysis. Smaller values result in lower latency
//   and high responsiveness but less accuracy. Higher values result in
//   considerably greater CPU load. Values that aren't powers of 2 get
//   rounded up to the next power of 2. Recommend 512, 1024, or 2048.
//
// bias (float) [0-1], default 0.2
//   Katja's pitch tracker introduces a small bias to help with the tracking.
//   (See the link above.) I don't know how this parameter affects the output.

// Example
SinOsc osc => dac; // create an oscillator
// PitchTrack must connect to blackhole to run
osc => PitchTrack pitch => blackhole;

0.5 => osc.gain;

while (true)
{
	// set the oscillator to a random frequency
	Math.random2f(200,1000) => osc.freq;
	<<< "Actual oscillator frequency:",osc.freq(),"Hz" >>>;
	250::ms => now; // pass a little time
	<<< "PitchTrack thinks it's this:",pitch.get(),"Hz" >>>;
	<<< "Difference:",Math.fabs(osc.freq() - pitch.get()),"Hz\n" >>>;
	2::second => now;
}
