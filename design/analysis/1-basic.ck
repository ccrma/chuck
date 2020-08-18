// analysis
adc => FFT fft => blackhole;
// synthesis
IFFT ifft => dac;

// set analysis parameters
( hamming, 512 ) => fft.window;
512 => fft.padding;

// set synthesis paramters
( hamming, 512 ) => ifft.ola;

// infinite time loop
complex results[];
while( true )
{
	// take fft
	fft.go() @=> results;
	
	// take ifft
	results => ifft.go;
	
	// advance time, hop size
	128::samp => now;
}
