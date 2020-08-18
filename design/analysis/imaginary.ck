
/* need this
class FFTFrame
{
	string name;
	int size;
	Type type;
	
	complex data[];
	complexe[] getData() { return data; }
}
*/

// patch
adc => FFT fft => blackhole;

// set window
( hamming, 512 ) => fft.window;
// zero padding
512 => fft.padding;
// name for enqueuing onto info-stream
"joanne" => fft.signature;

FFTFrame results[];

// infinite time loop
while( true )
{
	// take fft, result implicitly enqueued
	fft.go();
	
	// immediately look for it
	Info.stream().query( <FFTFrame>, "joanne" ) @=> results;
	// examine
	if( results != null )
	{
		analyze( results.getData() );
	}
	
	// advance time, hop size
	128::samp => now;
}
