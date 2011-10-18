// SIMPLE ENVELOPE FOLLOWER, by P. Cook
// slight modifications and MAUI_Gauge addition by Spencer Salazar

// change to adc, dac, or whatever
adc @=> UGen source;
// adjust as needed
1000.0 => float scale;
//adjust as needed
20::ms => dur update_period;

// patch
source => Gain g => OnePole p => blackhole;
// square the input
source => g;
// multiply
3 => g.op;

// set pole position
0.9999 => p.pole;

// create the gauge
MAUI_Gauge gauge;
// display it
gauge.display();

// loop forever
while( true )
{
	// update gauge value
	p.last() * scale => gauge.value;
	// wait
	update_period => now;
}
