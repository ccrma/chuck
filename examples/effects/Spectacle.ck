//////////////////////////////////////////////////////
// Spectacle - FFT-based spectral delay and EQ      //
//                                                  //
// SPECTACLE is an FFT-based delay instrument by    //
// John Gibson and inspired by the totally awesome  //
// Spektral Delay plug-in by Native Instruments.    //
// This version is adapted from RTcmix (rtcmix.org) //
// by Joel Matthys                                  //
//////////////////////////////////////////////////////

// Options
//
// mix (float) [0-1]: mix of processed and unprocessed signal. default 1.0
// clear (void) : reset Spectacle
// fftlen (int) : set FFT frame size (power of 2)
// overlap (int) : set frame overlap; best between 2 and 6
// delayMax (dur) : maximum delay time
// delayMin (dur) : minimum delay time
// freqMax (float) : maximum frequency processed by Spectacle
// freqMin (float) : minimum frequency processed by Spectacle
// range (float, float) : set both min and min freqs in one command
// bands (int) : set number of frequency bands, 1-512, default 64
// delay (dur) : set the same duration for all bands
// eq (float) : set the same EQ value for all bands (value is +/- dB)
// feedback (float) : set the same feedback value for all bands (-1.0 - 1.0)
//
// table (string, string) : set delay, eq, or feedback tables to
//                          random, ascending, or descending
//    example: table ("delay", "random");

// warning: use headphones or you'll get feedback!
adc => Spectacle spect => dac;
0.5 => spect.mix; // mix half processed and unprocessed signal
spect.range(100,4100); // limit frequency range
20 => spect.bands; // set number of bands to 20
// This will divide the spectrum evenly within the frequency range.
// Band 1 will cover 100-300 Hz, Band 2 will cover 300-500 Hz, etc.
// up to Band 20, which covers 3900-4100 Hz.
<<< spect.bands(), "frequency bands with random delay by default" >>>;
10::second => now;
<<< "switching to ascending delay and descending eq","">>>;
spect.table("delay","ascending");
spect.table("eq","descending");
10::second => now;
<<< "switching to ascending eq with feedback","" >>>;
0 => spect.eq;
0.8 => spect.feedback;
20::second => now;
adc =< spect; // disconnect input
<<< "ringing down","" >>>;
minute => now; // let it ring down