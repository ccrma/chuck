///////////////////////////////////////////////////
// Sigmund: sinusoidal analysis & pitch tracking //
//                                               //
// by Miller Puckette                            //
//                                               //
///////////////////////////////////////////////////

// Options:
// npts (int): number of points used in analysis.
//     Must be a power of 2, at least 128. The
//     minimum frequency that can be tracked is
//     about 2 * samplerate / npts
//     default: 1024
//
// npeak (int): maximum number of sinusoidal peaks
//     to look for. The computation time is
//     quadratic in the number of peaks actually
//     found (this number only sets an upper
//     limit). Use it to balance CPU time with
//     quality of results.
//
// maxfreq (float): maximum frequency of sinusoidal
//     peaks to look for. This can be useful in
//     situations where background nose creates
//     high-frequency, spurious peaks...
//
// minpower (float): minimum dB level to
//     report a pitch. Signals quieter than this
//     will be assumed to be crosstalk and ignored.
//     default: 50
//
// freq() (read-only): get reported frequency of
//     input signal
//
// env() (read-only): get reported RMS value (in dB)
//     of input signal
//
// peak (int): report freq of nth sinusoundal peak
//     sorting depends on parameter "dotracks"
//
// amp (int): report amplitude of nth sinusoundal peak
//     sorting depends on parameter "dotracks"
//
// tracks (0/1): toggle whether peak and amp are
//     sorted in order of amplitude or organized into
//     tracks
//
// clear(): clear buffers and reset
//
// param1, param2, param3 (float): mysterious settings...

// must connect to blackhole to perform DSP
TriOsc foo => Sigmund siggy => blackhole;

4096 => siggy.npts;
20 => siggy.npeak;
1000 => siggy.maxfreq;
25 => siggy.minpower;

while (true)
{
	Math.random2f(5,300) => foo.freq;
	Math.random2f(0,1) => foo.gain;
	second => now;
	<<< "Real frequency:",foo.freq(),"real gain:",foo.gain(),"Sigmund found this:",siggy.freq(), "power:", siggy.env() >>>;
}
