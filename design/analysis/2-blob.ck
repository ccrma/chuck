// patch
adc => FFT fft => IFFT ifft => dac;

// fft => crazy_spectral_mod => IFFT ifft2 => dac;
// fft => crazy2 => IFFT ifft3 => dac;

// set window
( hamming, 512 ) => fft.window;
// zero padding
512 => fft.padding;
// name for enqueuing onto info-stream
"joanne" => fft.signature;

InfoBlob results;

// infinite time loop
while( true )
{
	// take fft, result implicitly enqueued
	fft.go();
	
	// immediately look for it
	fft.output() @=> results;
	// examine
	if( results.good() )
	{
		analyze( results );
	}
	
	// advance time, hop size
	128::samp => now;
}

// uses FFT frames to do something
fun void analyze( InfoBlob blob )
{
    // get # of FFT bins
    blob.getIntMetaData( fft.NUM_BINS ) => int numBins;
    // get data
    blob.cVector @=> complex bins[];
    
    // do stuff, we have data
}
